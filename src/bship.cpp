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

#include "bship.h"
#include "bobjectlist.h"
#include "bbackburner.h"
#include "bdebug.h"
#include "bupgrade.h"

#include <QTime>

const int CBackBurnerTime = 40;

class BShipPrivate
{
public:
    QTime myFireTime;
    QTime myBackBurnerTime;
    int myFireInterval;
    BBackBurner* myBackBurner;
    short myFireType;
    int myDamage;
};


BShip::BShip(int aX, int aY, const QString& aImage, short aImageModifier)
    : BObject(NULL, aX, aY, aImage, 5, BObject::Ship, aImageModifier ), d(new BShipPrivate)
{
    d->myFireTime = QTime::currentTime();
    d->myBackBurnerTime = QTime::currentTime();
    d->myFireInterval = 230;
    d->myFireType = Default;
    d->myDamage = 50;

    if (imageModifier() == BObject::MirrorX)
        d->myBackBurner = new BBackBurner(this, x()+width(), y()+(height()/3.0f),
                                      width()/2.0f,height()/3.0f, imageModifier());
    else
        d->myBackBurner = new BBackBurner(this, x()-width()/2.0f, y()+(height()/3.0f),
                                      width()/2.0f,height()/3.0f, imageModifier());
}

BShip::~ BShip()
{
    delete d->myBackBurner;
    delete d;
}

QList<BShot*> BShip::fire()
{
    QList<BShot*> tList;
    if (d->myFireTime.msecsTo( QTime::currentTime() ) > d->myFireInterval)
    {
        d->myFireTime = QTime::currentTime();

        switch (fireType())
        {
            case Tripple:
            {
                BShot* tShot = new BShot(this, 650.0f, BObject::None, BShot::Player);
                float tY = y() + height()/2.0f - tShot->height()/2.0f;
                tShot->setY( tY );
                float tX;
                if (imageModifier() == BObject::None)
                    tX = x() + width();
                else
                    tX = x() - width();
                tShot->setX( tX );
                BPointF* tDirection = tShot->direction();
                tDirection->setX(0.8f);
                tDirection->setY(0.2f);
                tList.append( tShot );

                tShot = new BShot(this, 650.0f, BObject::None, BShot::Player);
                tShot->setX( tX );
                tShot->setY( tY );
                tDirection = tShot->direction();
                tDirection->setX(1.0f);
                tDirection->setY(0.0f);
                tList.append( tShot );

                tShot = new BShot(this, 650.0f, BObject::None, BShot::Player);
                tShot->setX( tX );
                tShot->setY( tY );
                tDirection = tShot->direction();
                tDirection->setX(0.8f);
                tDirection->setY(-0.2f);
                tList.append( tShot );

                break;
            }
            case Double:
            {
                BShot* tShot = new BShot(this, 650.0f, BObject::None, BShot::Player);
                tShot->setY(y());
                if (imageModifier() == BObject::None)
                    tShot->setX( x() + width() );
                else
                    tShot->setX( x() - width() );
                tList.append( tShot );

                tShot = new BShot(this, 650.0f, BObject::None, BShot::Player);
                tShot->setY(y()+height() - tShot->height());
                if (imageModifier() == BObject::None)
                    tShot->setX( x() + width() );
                else
                    tShot->setX( x() - width() );
                tList.append( tShot );
                break;
            }
            case Default:
            default:
            {
                BShot* tShot = new BShot(this, 650.0f, BObject::None, BShot::Player);
                tShot->setY( y() + height()/2.0f - tShot->height()/2.0f);

                if (imageModifier() == BObject::None)
                    tShot->setX( x() + width() );
                else
                    tShot->setX( x() - width() );
                tList.append( tShot );
            }
        }
    }
    return tList;
}

void BShip::setWidth(int aWidth)
{
    BHitBox::setWidth(aWidth);
    backBurner()->setWidth( aWidth/2.0f );
}

void BShip::setHeight(int aHeight)
{
    BHitBox::setHeight(aHeight);
    backBurner()->setHeight( aHeight/3.0f );
    backBurner()->setY( y()+(height()/3.0f) ); //to fix initial(0) y pos
}

BBackBurner* BShip::backBurner()
{
    return d->myBackBurner;
}

void BShip::setX(float aX)
{
    BObject::setX(aX);

    if (imageModifier() == BObject::MirrorX)
        backBurner()->setX( x()+width() );
    else
        backBurner()->setX( x()-width()/2.0f );
}

void BShip::setY(float aY)
{
    BObject::setY(aY);
    backBurner()->setY( y()+(height()/3.0f) );
}

short BShip::fireType() const
{
    return d->myFireType;
}

void BShip::upgrade(BObject* aUpgrade)
{
    BUpgrade* tUpgrade = static_cast<BUpgrade*>(aUpgrade);
    d->myDamage += tUpgrade->damageIncrease();
    if (tUpgrade->fireType() != -1)
        d->myFireType = tUpgrade->fireType();
}
