/***************************************************************************
 *   Copyright (C) 2008 by Bernd Buschinski                                *
 *   b.buschinski@web.de                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#include "bworldmanager.h"

#include "bobjectlist.h"
#include "bobject.h"
#include "bship.h"
#include "benemyship.h"
#include "benviroment.h"
#include "bdebug.h"
#include "bxml.h"
#include "bexplosion.h"
#include "bshot.h"
#include "bhitbox.h"
#include "bbackburner.h"
#include "bupgrade.h"

#include <math.h>

#include <QDir>
#include <QTime>
#include <QKeyEvent>
#include <QTimer>
#include <QWidget>
#include <QPointF>
#include <QPoint>

const float CPositionSpeed = 25.0f;
const int CInternUpdateInterval = 20;
const float CShipSpeed = 159.0f;

class BWorldManagerPrivate
{
public:
    BObjectList *myList;
    BShip* myShip;
    unsigned int myLevelWidth;
    unsigned int myLevelHeight;
    QTime myTime;
    bool myIncreasePos;
    double myPos;
    int myShipDirection;
    bool myShipFire;

    QList<BObject*> myExplosionList;
    QList<BEnemyShip*> myEnemyShipList;
    int myWidgetWidth;
};


BWorldManager::BWorldManager( BObjectList* aObjectList)
    : d (new BWorldManagerPrivate)
{
    d->myIncreasePos = true;
    d->myList = aObjectList;
    d->myLevelHeight = 0;
    d->myLevelWidth = 0;
    d->myPos = 0.0f;
    d->myWidgetWidth = -1;
    d->myShipDirection = 0;
    d->myShipFire = false;
    d->myShip = NULL;
}

BWorldManager::~ BWorldManager()
{
    while (!enemyList()->isEmpty())
        delete enemyList()->takeFirst();

    delete d;
}

void BWorldManager::keyPressEvent(QKeyEvent* aEvent)
{
    d->myShipDirection |= keyToDirection(aEvent->key());

    if (aEvent->key() == Qt::Key_Space)
        d->myShipFire = true;
}

void BWorldManager::keyReleaseEvent(QKeyEvent* aEvent)
{
    d->myShipDirection ^= keyToDirection(aEvent->key());

    if (aEvent->key() == Qt::Key_Space)
        d->myShipFire = false;
}

BObjectList* BWorldManager::objectList() const
{
    return d->myList;
}

QString BWorldManager::findLevel(const QString& aLevel) const
{
    QList<QDir> tList = BEnviroment::getIntance()->levelDirList();
    int tSize = tList.size();
    for (int i = 0; i < tSize; ++i)
    {
        QDir tDir(tList.at(i));
        bDebug("absDirPath=" + tDir.absolutePath()+ " aLevel=" + aLevel);
        QStringList tStringList = tDir.entryList( QStringList(aLevel),
                                                  QDir::Files, QDir::Name);
        int tStringListSize = tStringList.size();
        for (int j = 0; j < tStringListSize; ++j)
        {
            if (tStringList.at(j) == aLevel)
            {
                QString tFile = tDir.absolutePath() + "/" + tStringList.at(j);
                if (BXml::hasType(tFile,"BYAVSLEVEL"))
                    return tFile;
            }
        }
    }
    bError() << "found nothing: " << aLevel;
    return "";
}

bool BWorldManager::initLevel(const QString& aLevel) const
{
    QString tFile = findLevel(aLevel);
    if (!QFile::exists(tFile))
    {
        bError() << "file does not exists: " << tFile;
        return false;
    }

    BXml tXml(tFile,"BYAVSLEVEL");
    if (!tXml.open( QIODevice::ReadOnly ))
    {
        bError() << "tXml.open failed";
        return false;
    }

    if (tXml.setGroup("global") == BXml::Successfully)
    {
        d->myLevelHeight = tXml.readUInt("height",0);
        d->myLevelWidth = tXml.readUInt("width",0);
        if ((d->myLevelWidth == 0) || (d->myLevelHeight == 0))
            return false;

        QString tBGImage = tXml.readString("bgimg","bg01.png");
        appendObject( new BObject(NULL, 0, 0, tBGImage, 0, BObject::BackGround) );

        d->myShip = new BShip( tXml.readSize("startpos").width(),
                               tXml.readSize("startpos").height(),
                               "usership.png", BObject::None);
        ship()->setWidth( tXml.readSize("shipsize").width() );
        ship()->setHeight( tXml.readSize("shipsize").height() );
        createHitBoxes(ship(), &tXml);
        appendObject( ship() );
        for (int i = 0; i < ship()->backBurner()->objects().size(); ++i)
            appendObjectEffect( ship()->backBurner()->objects().at(i) );
    } else {
        bError() << "no global group, broken map?";
        return false;
    }
    tXml.up();
    tXml.setGroup("objects");
        for (int i = 0; i < tXml.readInt("count",0); ++i)
            readObject(&tXml, i);
    tXml.up();
    tXml.setGroup("enemies");
        for (int i = 0; i < tXml.readInt("count",0); ++i)
            readEnemy(&tXml, i);
    tXml.up();

    return true;
}

bool BWorldManager::loadLevel(const QString& aLevel) const
{
    if (!initLevel(aLevel))
        return false;
    return true;
}

void BWorldManager::createHitBoxes(BObject* aObject, BXml* aXml) const
{
    if (aXml->setGroup("hitboxes") == BXml::Successfully)
    {
        for (uint i = 0; i < aXml->readUInt("count",0) ; ++i)
        {
            QRect tRect = aXml->readRect("hitbox"+QString::number(i));
            aObject->addHitBox( new BHitBox(aObject->x() + tRect.x(), aObject->y() + tRect.y(),
                                            tRect.width(), tRect.height()  ) );
        }
    } else
        aObject->addHitBox( new BHitBox(aObject->x(), aObject->y(), aObject->width(), aObject->height()) );
    aXml->up();
}

void BWorldManager::readObject(BXml* aXml, int aObject) const
{
    if (aXml->setGroup("object"+QString::number(aObject)) == BXml::Successfully)
    {
        QString tImg = aXml->readString("image");
        QRect tRect = aXml->readRect("size");
        BObject* tObj = new BObject(NULL,tRect.x(),tRect.y(),tImg,1,BObject::Object);
        tObj->setWidth( tRect.width() );
        tObj->setHeight( tRect.height() );

        createHitBoxes(tObj, aXml);
        appendObject( tObj );
    } else
        bWarning() << "unknown object: object" << aObject;
    aXml->up();
}

void BWorldManager::readEnemy(BXml* aXml, int aObject) const
{
    if (aXml->setGroup("enemy"+QString::number(aObject)) == BXml::Successfully)
    {
        QString tImg = aXml->readString("image");
        QRect tRect = aXml->readRect("size");
        uint tTrigger = aXml->readUInt("trigger",1);
        short tImageModifier = aXml->readUInt("imagemodifier", 1);
        BEnemyShip* tEnemy = new BEnemyShip(tRect.x(),tRect.y(), tImg, tTrigger, tImageModifier);
        tEnemy->setWidth( tRect.width() );
        tEnemy->setHeight( tRect.height() );
        tEnemy->setHealth( aXml->readUInt("health",1) );
        tEnemy->setSpeed( aXml->readFloat("speed",1) );
        tEnemy->setFireInterval( aXml->readUInt("fireinterval", 0) );

        readEnemyDirection( aXml, tEnemy);
        createHitBoxes(tEnemy, aXml);

        if (aXml->setGroup("item") == BXml::Successfully)
        {
            int tDamage = aXml->readInt("damageincrease",0);
            int tFireType = aXml->readInt("firetype",-1);
            QString tItemImage = aXml->readString("image");
            tEnemy->setItem( new BUpgrade(tItemImage, tDamage, tFireType));
        }
        aXml->up();

        appendEnemy( tEnemy );
    } else
        bWarning() << "unknown ememy: enemy" << aObject;
    aXml->up();
}

void BWorldManager::readEnemyDirection(BXml* aXml, BEnemyShip* aEnemy) const
{
    if (aXml->setGroup("direction") == BXml::Successfully)
    {
        for (uint i = 0; i < aXml->readUInt("count",0) ; ++i)
            aEnemy->addPoint( aXml->readPoint("direction"+QString::number(i)) );
        aEnemy->setDirectionType( aXml->readInt("directiontype", BEnemyShip::OneShot ));
    }
    aXml->up();
}

void BWorldManager::update() const
{
//     bDebug("0: " + d->myTime.toString("mm:ss:zzz"));
//     bDebug("start: " + QTime::currentTime().toString("mm:ss:zzz"));
    int tTime = d->myTime.msecsTo( QTime::currentTime() );
    d->myTime = QTime::currentTime();

//     bDebug("tTime: "+ QString::number(tTime));
    for ( ; tTime > CInternUpdateInterval; tTime -= CInternUpdateInterval )
    {
        updatePosition();
        updateShotPos( CInternUpdateInterval );
        updateShipPos( CInternUpdateInterval );
        if (ship() && d->myShipFire)
        {
            QList<BShot*> tShotList = ship()->fire();
            int tSize = tShotList.size();
            for (int i = 0; i < tSize; ++i)
                appendObject( tShotList.at(i) );
        }
        updateEnemyPos( CInternUpdateInterval );
        updateEnemyFire();
        updateEnemyTrigger();

        checkCollision();
    }
    deleteExplosions();
//     bDebug("tTime: "+ QString::number(-tTime));
//     bDebug("1: " + d->myTime.toString("mm:ss:zzz"));
    d->myTime = d->myTime.addMSecs( -tTime );
//     bDebug("2: " + d->myTime.toString("mm:ss:zzz"));
}

void BWorldManager::start() const
{
    d->myIncreasePos = true;
    d->myPos = 0.0;
    d->myShipFire = false;
    d->myTime = QTime::currentTime();
}

float BWorldManager::position() const
{
    return d->myPos;
}

BShip* BWorldManager::ship() const
{
    return d->myShip;
}

int BWorldManager::keyToDirection(int aKeyCode) const
{
    switch (aKeyCode)
    {
        case Qt::Key_Up: return Up;
        case Qt::Key_Down: return Down;
        case Qt::Key_Left: return Left;
        case Qt::Key_Right: return Right;
    }
    return None;
}

float BWorldManager::directionToX(int aDirection) const
{
    float tValue = 0.0f;
    if (aDirection & Left)
        tValue += -1.0f;
    if (aDirection & Right)
        tValue += 1.0f;
    return tValue;
}

float BWorldManager::directionToY(int aDirection) const
{
    float tValue = 0.0f;
    if (aDirection & Up)
        tValue += -1.0f;
    if (aDirection & Down)
        tValue += 1.0f;
    return tValue;
}

void BWorldManager::updateShipPos(int aMSec) const
{
    if (!ship())
        return;

    if (d->myShipDirection == None)
        return;

    float tX = (directionToX( d->myShipDirection )/1000.0f)*aMSec*CShipSpeed;
    float tY = (directionToY( d->myShipDirection )/1000.0f)*aMSec*CShipSpeed;
    float tPosition = position();

    tY += ship()->y();
    if (tY < 0)
        tY = 0;
    else if ((tY + ship()->height())  > d->myLevelHeight )
        tY = d->myLevelHeight - ship()->height();

    tX += ship()->x();
    if (tX < tPosition)
        tX = tPosition;
    else if ((tX + ship()->width()) > widgetWidth()+tPosition )
        tX = (tPosition + widgetWidth()) - ship()->width();

    ship()->setX( tX );
    ship()->setY( tY );
}

BHitBox* BWorldManager::collision(BObject* aObj1, BObject* aObj2) const
{
//hitboxes const? callgrind
    int tObj1Size = aObj1->hitboxes()->size();
    for (int j = 0; j < tObj1Size; ++j)
    {
        int tObj2Size = aObj2->hitboxes()->size();
        for (int i = 0; i < tObj2Size; ++i)
        {
            BHitBox* t2Box = aObj2->hitboxes()->at(i);
            if (collision(aObj1->hitboxes()->at(j), t2Box))
                return t2Box;
        }
    }
    return NULL;
}

bool BWorldManager::collision(BHitBox* aObj1, BHitBox* aObj2) const
{
//where it can never collide
    if (aObj1->x() > aObj2->x() + aObj2->width()) //behind object
        return false;

    if (aObj1->x() + aObj1->width() < aObj2->x()) //before object
        return false;

    if (aObj1->y() + aObj1->height() < aObj2->y()) //over object
        return false;

    if (aObj1->y() > aObj2->y() + aObj2->height()) //under object
        return false;

    return true;
}

void BWorldManager::deleteItem(BObject* aObject) const
{
#if QT_VERSION < 0x040400
    objectList()->removeAll( aObject );
#else
    objectList()->removeOne( aObject );
#endif
    aObject->deleteLater();
}

QList< BObject* >* BWorldManager::explosionList() const
{
    return &d->myExplosionList;
}

void BWorldManager::deleteExplosions() const
{
    int tSize = explosionList()->size();
    for (int i = 0; i < tSize; ++i)
    {
        BObject* tObj = explosionList()->at( i );
        if (tObj->isDone())
        {
            deleteItem( tObj );
            explosionList()->removeAt( i );
            --i;
            --tSize;
        }
    }
}

void BWorldManager::updatePosition() const
{
    if (d->myIncreasePos)
    {
        float tVal = CPositionSpeed/1000.0f * (float)CInternUpdateInterval;
        d->myPos += tVal;
        if (ship()) ship()->setX( ship()->x() + tVal );
    }
}

void BWorldManager::slotStopPositionUpdate() const
{
    d->myIncreasePos = false;
}

void BWorldManager::appendObject(BObject* aObject) const
{
    objectList()->append( aObject );
}

void BWorldManager::appendObjectEffect(BObject* aObject) const
{
    explosionList()->append( aObject );
    appendObject(aObject);
}

void BWorldManager::slotWidgetWidthChanged(int aWidth) const
{
    d->myWidgetWidth = aWidth;
}

int BWorldManager::widgetWidth() const
{
    return d->myWidgetWidth;
}

void BWorldManager::updateShotPos(int aMSec) const
{
    int tSize = objectList()->size();
    for (int i = 0; i < tSize; ++i)
    {
        BObject* tObj = objectList()->at( i );
        if (tObj->isDone())
            continue;
        if (tObj->type() != BObject::Shot)
            continue;
        BShot* tShot = static_cast<BShot*>(tObj);
        float tDistance = (tShot->speed()/1000.0f)*aMSec;

        BPointF* tPoint = tShot->direction();
        tShot->setX( tShot->x() + (tDistance*tPoint->x()) );
        tShot->setY( tShot->y() + (tDistance*tPoint->y()) );

        if (outsideWindow(tShot))
        {
            tShot->setDone( true );
            tShot->setVisible( false );
            explosionList()->append( tShot );
        }
    }
}

bool BWorldManager::outsideWindow(BHitBox* aBox) const
{
    float tPosition = position();
    if (aBox->x() + aBox->width() < tPosition)
        return true;

    if (aBox->x() > tPosition + widgetWidth())
        return true;

    if (aBox->y() + aBox->height() < 0)
        return true;

    if (aBox->y() > d->myLevelHeight)
        return true;

    return false;
}

bool BWorldManager::checkCollision() const
{
    bool tRet = false;

    int tShipCanCrash = BObject::Object | BObject::Shot | BObject::EnemyShip | BObject::Upgrade;
    int tShotCanCrash = BObject::Ship | BObject::Object | BObject::EnemyShip;
    int tCanCrash = tShotCanCrash | tShipCanCrash;

    BObject *tObj, *tObj2;

    int tSize = objectList()->size();
    short tObj1Type, tObj2Type;
    for (int j = 0; j < tSize; ++j)
    {
        tObj = objectList()->at( j );
        if (tObj->isDone())
            continue;

        tObj1Type = tObj->type();
        if (!(tObj1Type & tCanCrash))
            continue;

        for (int i = 0; i < tSize; ++i)
        {
            if( i == j )
                continue;
            tObj2 = objectList()->at( i );

            if (tObj2->isDone())
                continue;

            tObj2Type = tObj2->type();
            if (tObj1Type == BObject::Shot && (tObj2Type & tShotCanCrash))
            {
                BShot *tShot = static_cast<BShot*>(tObj);
                if (checkShotCollision(tShot, tObj2))
                {
                    i--;
                    tSize--;
                }
            }
            else if (tObj1Type == BObject::Ship && (tObj2Type & tShipCanCrash))
            {
                BShip *tShip = static_cast<BShip*>(tObj);
                tRet = checkShipCollision(tShip, tObj2);
                if (tRet)
                {
                    i--;
                    tSize--;
                }
            }
        }
    }
    return tRet;
}

bool BWorldManager::checkShipCollision(BShip* aShip, BObject* aObj) const
{
    int tObjType = aObj->type();
    if (tObjType == BObject::Shot)
    {
        BShot *tShot = static_cast<BShot*>(aObj);
        if (tShot->shotType() == BShot::Player)
            return false;
    }

    if (collision(aShip, aObj))
    {
        if (tObjType == BObject::Upgrade)
        {
            ship()->upgrade( aObj );
            deleteItem( aObj );
            return true;
        }
        ship()->setVisible( false );
        ship()->setDone( true );
        BExplosion *tExplosion = new BExplosion( ship() );
        appendObjectEffect( tExplosion );
        deleteItem( ship() );
        d->myShip = NULL;
        return true;
    }
    return false;
}

bool BWorldManager::checkShotCollision(BShot* aShot, BObject* aObj) const
{
    if (aShot->isHit())
        return false;

    short tObjType = aObj->type();
    if (tObjType == BObject::Shot)
        return false;

    if ((aShot->shotType() == BShot::Enemy && tObjType == BObject::EnemyShip) ||
        (aShot->shotType() == BShot::Player && tObjType == BObject::Ship))
        return false;

    BHitBox* tBox = collision(aShot, aObj);
    if (tBox)
    {
        explosionList()->append( aShot );
        aShot->hit( tBox );
        if (tObjType == BObject::EnemyShip)
        {
            BEnemyShip *tEnemy = static_cast<BEnemyShip*>(aObj);
            tEnemy->setHealth( tEnemy->health() < aShot->damage() ? 0 : tEnemy->health() - aShot->damage() );
            if (tEnemy->health() == 0)
            {
                tEnemy->setVisible( false );
                tEnemy->setDone( true );
                BExplosion *tExplosion = new BExplosion(tEnemy);
                appendObjectEffect( tExplosion );

                if (tEnemy->item())
                {
                    BObject* tItem = tEnemy->item();
                    tItem->setX( tEnemy->x() + tEnemy->width()/2.0f - tItem->width()/2.0f );
                    tItem->setY( tEnemy->y() + tEnemy->height()/2.0f - tItem->height()/2.0f );
                    appendObject( tItem );
                }
                deleteItem( tEnemy );
                bDebug("deleting enemy");
            }
        }
        return true;
    }
    return false;
}

void BWorldManager::keyReleaseAll()
{
    d->myShipFire = false;
    d->myShipDirection = None;
}

QList< BEnemyShip* >* BWorldManager::enemyList() const
{
    return &d->myEnemyShipList;
}

void BWorldManager::appendEnemy(BEnemyShip* aEnemy) const
{
    enemyList()->append(aEnemy);
}

void BWorldManager::updateEnemyPos(int aMSec) const
{
    int tSize = objectList()->size();
    for (int i = 0; i < tSize; ++i)
    {
        BObject* tObj = objectList()->at( i );
        if (tObj->isDone())
            continue;
        if (tObj->type() != BObject::EnemyShip)
            continue;
        BEnemyShip* tEnemy = static_cast<BEnemyShip*>(tObj);
        QPointF tPoint = tEnemy->points().first() - QPointF( tEnemy->x(), tEnemy->y());
        float tDistance = sqrt(pow(tPoint.x(), 2) + pow(tPoint.y(), 2));
        float tSpeedDistance = (tEnemy->speed()/1000.0f)*(float)aMSec;

        if (tSpeedDistance > tDistance)
        {
            if (tEnemy->points().size() < 2)
            {
                tEnemy->setVisible( false );
                tEnemy->setDone( false );
                if (tEnemy->item())
                    delete tEnemy->item();
                deleteItem( tEnemy );
                --i;
                --tSize;
                continue;
            }
            tEnemy->setX( tEnemy->points().first().x() );
            tEnemy->setY( tEnemy->points().first().y() );

            tSpeedDistance -= tDistance;

            if (tEnemy->directionType() == BEnemyShip::OneShot)
                tEnemy->removeFirst();
            else if (tEnemy->directionType() == BEnemyShip::Repeat)
                tEnemy->cyclePoint();
            else
                bError() << "unknown directiontype: " << QString::number(tEnemy->directionType());

            tPoint = tEnemy->points().first() - QPointF( tEnemy->x(), tEnemy->y());
            tDistance = sqrt(pow(tPoint.x(), 2) + pow(tPoint.y(), 2));
        }

        if (tSpeedDistance > tDistance)
            bWarning() << "FIXME: tSpeedDistance still greater than tDistance";

        float tDiv = qAbs(tPoint.x())+qAbs(tPoint.y());
        if (tDiv == 0.0f)
            continue;

        float tXPart = tPoint.x()/tDiv;
        float tYPart = tPoint.y()/tDiv;

        tEnemy->setX( tEnemy->x() + tSpeedDistance*tXPart );
        tEnemy->setY( tEnemy->y() + tSpeedDistance*tYPart );
    }
}

void BWorldManager::updateEnemyTrigger() const
{
    uint tPos = position() + widgetWidth();
    BEnemyShip* tEnemy;
    for (int i = 0; i < enemyList()->size(); ++i)
    {
        tEnemy = enemyList()->at( i );
        if (tPos >= tEnemy->trigger())
        {
            enemyList()->removeAt(i);
            --i;
            tEnemy->show();
            appendObject( tEnemy );
            bDebug("new enemy triggered at: " + QString::number(tPos) );
        }
    }
}

void BWorldManager::updateEnemyFire() const
{
    BObject* tObj;
    int tSize = objectList()->size();
    for (int i = 0; i < tSize; ++i)
    {
        tObj = objectList()->at( i );
        if (tObj->type() == BObject::EnemyShip)
        {
            BEnemyShip *tEnemy = static_cast<BEnemyShip*>(tObj);
            BShot *tShot = tEnemy->fire();
            if (tShot)
                appendObject( tShot );
        }
    }
}
