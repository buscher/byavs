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

#include "bobject.h"

#include "bimagecache.h"

#include <QBasicTimer>
#include <QTimerEvent>
#include <QPixmap>

class BObjectPrivate
{
public:
    QString myImage;
    QString myNewImage;
    unsigned short myRenderLevel;
    QBasicTimer myTimer;
    BObject::ObjectType myType;
    bool myIsVisible;
    QObject* myParent;
    short myImageModifier;
    bool myIsDone;
    QList<BHitBox*> myHitBoxList;
};


BObject::BObject(QObject* aParent, int aX, int aY, const QString& aImage, unsigned short aRenderlevel, BObject::ObjectType aType, short aImageModifier)
    : QObject(NULL), BHitBox(aX,aY), d(new BObjectPrivate)
{
    d->myParent = aParent; // cant use QObject parent, it req. to delete all childs first and then the parent
    d->myImage = aImage;
    d->myRenderLevel = aRenderlevel;
    d->myType = aType;
    QPixmap *tPix = BImageCache::getInstance()->getPixmapByName(aImage);
    setWidth( tPix->width() );
    setHeight( tPix->height() );
    d->myIsVisible = true;
    d->myImageModifier = aImageModifier;
    d->myIsDone = false;

//     if ((aType == BObject::Object) /*|| (aType == BObject::Shot) || (aType == BObject::Ship)*/)
//         addHitBox( new BHitBox(x(),y(),width(),height()));
}

BObject::~ BObject()
{
    deleteAllHitBoxes();
    delete d;
}

QString BObject::image() const
{
    return d->myImage;
}

void BObject::updateSelf( unsigned int aMSec, const QString& aImage)
{
    d->myNewImage = aImage;
    basicTimer()->start(aMSec, this);
}

unsigned short BObject::renderlevel() const
{
    return d->myRenderLevel;
}

QString BObject::newImage() const
{
    return d->myNewImage;
}

QBasicTimer* BObject::basicTimer() const
{
    return &d->myTimer;
}

void BObject::timerEvent(QTimerEvent *aEvent)
{
    if (aEvent->timerId() == basicTimer()->timerId())
    {
        basicTimer()->stop();
        update( newImage() );
    }
}

void BObject::update(const QString& /*aImage*/)
{
    ; //override me
}

BObject::ObjectType BObject::type() const
{
    return d->myType;
}

bool BObject::visible() const
{
    return d->myIsVisible;
}

void BObject::setVisible(bool aVisible) const
{
    d->myIsVisible = aVisible;
}

void BObject::setImage(const QString & aImage) const
{
    d->myImage = aImage;
}

QObject * BObject::parent() const
{
    return d->myParent;
}

short BObject::imageModifier() const
{
    return d->myImageModifier;
}

bool BObject::isDone() const
{
    return d->myIsDone;
}

void BObject::setDone(bool aDone) const
{
    d->myIsDone = aDone;
}

QList< BHitBox* >* BObject::hitboxList() const
{
    return &d->myHitBoxList;
}

void BObject::addHitBox( BHitBox* aHitBox) const
{
    hitboxList()->append( aHitBox );
}

QList< BHitBox* >* BObject::hitboxes() const
{
    return &d->myHitBoxList;
}

void BObject::setX(float aX)
{
    float tDiff = aX - x();
    int tSize = hitboxList()->size();
    for (int i = 0; i < tSize; ++i)
    {
        BHitBox* tBox = hitboxList()->at(i);
        tBox->setX( tBox->x() + tDiff );
    }
    BHitBox::setX( aX );
}

void BObject::setY(float aY)
{
    float tDiff = aY - y();
    int tSize = hitboxList()->size();
    for (int i = 0; i < tSize; ++i)
    {
        BHitBox* tBox = hitboxList()->at(i);
        tBox->setY( tBox->y() + tDiff );
    }
    BHitBox::setY( aY );
}

void BObject::deleteAllHitBoxes() const
{
    while( !hitboxList()->isEmpty())
        delete hitboxList()->takeFirst();
}

void BObject::setWidth(int aWidth)
{
    float tDiff = aWidth - width();
    int tSize = hitboxList()->size();
    for (int i = 0; i < tSize; ++i)
    {
        BHitBox* tBox = hitboxes()->at(i);
        tBox->setWidth( tBox->width() + tDiff );
    }
    BHitBox::setWidth(aWidth);
}

void BObject::setHeight(int aHeight)
{
    float tDiff = aHeight - height();
    int tSize = hitboxList()->size();
    for (int i = 0; i < tSize; ++i)
    {
        BHitBox* tBox = hitboxes()->at(i);
        tBox->setHeight( tBox->height() + tDiff );
    }
    BHitBox::setHeight(aHeight);
}
