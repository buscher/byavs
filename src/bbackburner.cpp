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

#include "bbackburner.h"

#include <QTime>

const int CTimeDurian = 10;

class BBackBurnerPrivate
{
public:
    QList<BObject*> myObjectList;
    QList<BPointF*> myPositionList;
    QTime myLastTime;
};


BBackBurner::BBackBurner(BObject* aParent, float aX, float aY, int aWidth, int aHeight, short ImageModifier)
    : BObject(aParent, aX, aY, "backburner.png", 4, BObject::ShipEffect, ImageModifier ),
      d(new BBackBurnerPrivate)
{
    setObjectName("backburner");
    setWidth(aWidth);
    setHeight(aHeight);
    setVisible(false);

    BObject* tObject;
    int tWidth = width();
    int tHeight = height();
    int tX = aX;
    int tY = aY;

    for (int i = 0; i < 5; ++i)
    {
        tObject = new BObject(this, tX, tY,
                                    "backburner.png", 4, BObject::ShipEffect, imageModifier());
        tObject->setWidth( tWidth );
        tObject->setHeight( tHeight );
        objectList()->prepend( tObject );

        pointList()->prepend( new BPointF(tX, tY ) );
        tX = tX - (tWidth/8.0f);
        tY = tY + (tHeight/8.0f);
        tWidth = (tWidth/8.0f)*6.0f;
        tHeight = (tHeight/8.0f)*6.0f;
    }
    d->myLastTime = QTime::currentTime();
}

BBackBurner::~ BBackBurner()
{
    while (!pointList()->isEmpty())
        delete pointList()->takeFirst();

    int tSize = objectList()->size();
    for (int i = 0; i < tSize; ++i)
    {
        objectList()->at( i )->setDone ( true );
    }
    delete d;
}

QList< BObject *>* BBackBurner::objectList() const
{
    return &d->myObjectList;
}

QList< BPointF *>* BBackBurner::pointList() const
{
    return &d->myPositionList;
}

void BBackBurner::setX(float aX)
{
    BHitBox::setX(aX);
    pointList()->last()->setX( aX );
    checkNext();
}

void BBackBurner::setY(float aY)
{
    BHitBox::setY(aY);
    pointList()->last()->setY( aY );
    checkNext();
}

void BBackBurner::checkNext()
{
    if (d->myLastTime.msecsTo( QTime::currentTime()) > CTimeDurian)
    {
        moveAll();
        delete pointList()->takeFirst();
        pointList()->append( new BPointF(x(), y()) );
        d->myLastTime = QTime::currentTime();
    }
    moveFirst();
}

void BBackBurner::moveAll()
{
    int tHeight = height();

    for (int i = objectList()->size()-1; i >= 0; --i)
    {
        BObject* tObj = objectList()->at( i );

        pointList()->at(i)->setY( pointList()->at(i)->y() + (tHeight/8.0f) );

        tObj->setX( pointList()->at(i)->x() );
        tObj->setY( pointList()->at(i)->y() );

        tHeight = (tHeight/8.0f)*6.0f;
    }
}

void BBackBurner::moveFirst()
{
    BObject* tObj = objectList()->last();
    tObj->setX( pointList()->last()->x() );
    tObj->setY( pointList()->last()->y() );
}

QList< BObject*> BBackBurner::objects() const
{
    return d->myObjectList;
}

void BBackBurner::setWidth(int aWidth)
{
    float tDiff = (float)aWidth/width();
    int tSize = objectList()->size();
    for (int i = 0; i < tSize; ++i)
    {
        objectList()->at(i)->setWidth( objectList()->at(i)->width() * tDiff);
    }
    BObject::setWidth(aWidth);
}

void BBackBurner::setHeight(int aHeight)
{
    float tDiff = (float)aHeight/height();
    int tSize = objectList()->size();
    for (int i = 0; i < tSize; ++i)
    {
        objectList()->at(i)->setHeight( objectList()->at(i)->height() * tDiff);
    }
    BObject::setHeight(aHeight);
}
