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

#include "bshot.h"

#include "bhitbox.h"

class BShotPrivate
{
public:
    float mySpeed;
    bool myHit;
    uint myDamage;
    int myShotType;
    BPointF myDirection;
};


BShot::BShot(QObject* aParent, float aSpeed, int aImageModifier, BShot::ShotType aShotType)
    : BObject( aParent, 0, 0, aShotType == BShot::Player ? "shot.png": "eshot.png",
      2, BObject::Shot, aImageModifier), d(new BShotPrivate)
{
    d->mySpeed = aSpeed;
    d->myHit = false;
    d->myDamage = 50;
    d->myShotType = aShotType;

    if (imageModifier() == BObject::MirrorX)
    {
        addHitBox( new BHitBox(x()+width()/4.0f*3.0f,y()+height()/4.0f,width()/4.0f,height()/2.0f) );
        addHitBox( new BHitBox(x(),y(),width()/4.0f*3.0f,height()) );
        d->myDirection.setX(-1.0f);
    } else if (imageModifier() == BObject::None)
    {
        addHitBox( new BHitBox(x()+width()/4.0f,y(),width()/4.0f*3.0f,height()) );
        addHitBox( new BHitBox(x(),y()+height()/4.0f,width()/4.0f,height()/2.0f) );
        d->myDirection.setX(1.0f);
    }
    d->myDirection.setY(0.0f);
}

BShot::~ BShot()
{
    delete d;
}

float BShot::speed() const
{
    return d->mySpeed;
}

void BShot::update(const QString& aImage)
{
    if (aImage=="done")
    {
        deleteAllHitBoxes();
        setDone( true );
        setVisible( false );
    }
    else if (aImage=="1")
    {
        setImage(shotType() == BShot::Player ? "shot-done1.png" : "eshot-done1.png");
        deleteAllHitBoxes();
        if (imageModifier() == None)
            addHitBox( new BHitBox(x()+width()/2.0f, y(), width()/2.0f, height()) );
        else if (imageModifier() == BObject::MirrorX)
            addHitBox( new BHitBox(x(), y(), width()/2.0f, height()) );
        updateSelf(70, "done");
    }
}

void BShot::hit(BHitBox* aObject)
{
    d->myHit = true;
    d->mySpeed = 0;
    setImage(shotType() == BShot::Player ? "shot-done0.png" : "eshot-done0.png");
    setWidth( 15 );
    setHeight( 30 );

    if (imageModifier() == None)
        setX( aObject->x() - width());
    else
        setX( aObject->x() + aObject->width());

    setY( y() - height()/4.0f);
    deleteAllHitBoxes();
    addHitBox( new BHitBox(x(),y(),width(),height()) );
    updateSelf(70, "1");
}

bool BShot::isHit() const
{
    return d->myHit;
}

uint BShot::damage() const
{
    return d->myDamage;
}

int BShot::shotType() const
{
    return d->myShotType;
}

BPointF* BShot::direction() const
{
    return &d->myDirection;
}

void BShot::setDirection(float aX, float aY)
{
    d->myDirection.setX(aX);
    d->myDirection.setY(aY);
}
