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

#include "benemyship.h"
#include "bupgrade.h"
#include "bdebug.h"

#include <QTime>

class BEnemyShipPrivate
{
public:
    uint myHealth;
    uint myTrigger;
    float mySpeed;
    QList<QPoint> myPointList;
    uint myFireInterval;
    QTime myFireTime;
    short myDirectionType;
    BUpgrade* myUpgrade;
};

BEnemyShip::BEnemyShip(int aX, int aY, const QString & aImage, uint aXTrigger, short aImageModifier)
    : BObject(NULL, aX, aY, aImage, 4, BObject::EnemyShip, aImageModifier ), d(new BEnemyShipPrivate)
{
    d->myDirectionType = BEnemyShip::OneShot;
    d->myTrigger = aXTrigger;
    d->myHealth = 0;
    d->mySpeed = 0.0f;
    d->myFireInterval = 0;
    d->myUpgrade = NULL;
}

BEnemyShip::~ BEnemyShip()
{
    delete d;
}

BShot* BEnemyShip::fire()
{
    if (d->myFireInterval == 0)
        return NULL;

    if (d->myFireTime.msecsTo( QTime::currentTime() ) > (int)d->myFireInterval)
    {
        d->myFireTime = QTime::currentTime();
//         bDebug("fire: " + QTime::currentTime().toString() + " " + d->myFireTime.toString());
        BShot* tShot = new BShot(this, 650.0f, imageModifier(), BShot::Enemy);
        if (imageModifier() == BObject::None)
            tShot->setX( x() + width() );
        else
            tShot->setX( x() - tShot->width() );
        tShot->setY( y() + height()/2.0f - tShot->height()/2.0f);
        return tShot;
    }
    return NULL;
}

void BEnemyShip::setWidth(int aWidth)
{
    BHitBox::setWidth(aWidth);
}

void BEnemyShip::setHeight(int aHeight)
{
    BHitBox::setHeight(aHeight);
}

uint BEnemyShip::trigger() const
{
    return d->myTrigger;
}

QList< QPoint >* BEnemyShip::pointList() const
{
    return &d->myPointList;
}

void BEnemyShip::addPoint(const QPoint& aPoint) const
{
    pointList()->append( QPoint( aPoint ));
}

uint BEnemyShip::health() const
{
    return d->myHealth;
}

void BEnemyShip::setHealth(uint aHealth)
{
    d->myHealth = aHealth;
}

float BEnemyShip::speed() const
{
    return d->mySpeed;
}

void BEnemyShip::setSpeed(float aSpeed)
{
    d->mySpeed = aSpeed;
}

QList< QPoint > BEnemyShip::points() const
{
    return d->myPointList;
}

void BEnemyShip::removeFirst() const
{
    pointList()->removeFirst();
}

void BEnemyShip::setFireInterval(uint aInterval)
{
    d->myFireInterval = aInterval;
}

void BEnemyShip::show() const
{
    d->myFireTime = QTime::currentTime().addMSecs( d->myFireInterval/8.0f );
}

short BEnemyShip::directionType() const
{
    return d->myDirectionType;
}

void BEnemyShip::setDirectionType(short aType)
{
    d->myDirectionType = aType;
}

void BEnemyShip::cyclePoint() const
{
    QPoint tPoint = pointList()->first();
    pointList()->removeFirst();
    pointList()->append( tPoint );
}

BUpgrade* BEnemyShip::item() const
{
    return d->myUpgrade;
}

void BEnemyShip::setItem(BUpgrade * aItem)
{
    d->myUpgrade = aItem;
}
