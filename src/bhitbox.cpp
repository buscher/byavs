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

#include "bhitbox.h"

#include <QRect>

class BHitBoxPrivate
{
public:
    float myX;
    float myY;
    int myWidth;
    int myHeight;
};


BHitBox::BHitBox(float aX, float aY, int aWidth, int aHeight)
    : d (new BHitBoxPrivate)
{
    d->myX = aX;
    d->myY = aY;
    d->myHeight = aHeight;
    d->myWidth = aWidth;
}

BHitBox::BHitBox(const QRect& aRect)
    : d (new BHitBoxPrivate)
{
    d->myX = aRect.x();
    d->myY = aRect.y();
    d->myHeight = aRect.height();
    d->myWidth = aRect.width();
}

BHitBox::BHitBox(const BHitBox& aHitBox)
    : d (new BHitBoxPrivate)
{
    setX( aHitBox.x() );
    setY( aHitBox.y() );
    setWidth( aHitBox.width() );
    setHeight( aHitBox.height() );
}

BHitBox::~ BHitBox()
{
    delete d;
}

float BHitBox::x() const
{
    return d->myX;
}

float BHitBox::y() const
{
    return d->myY;
}

void BHitBox::setX(float aX)
{
    d->myX = aX;
}

void BHitBox::setY(float aY)
{
    d->myY = aY;
}

int BHitBox::width() const
{
    return d->myWidth;
}

int BHitBox::height() const
{
    return d->myHeight;
}

void BHitBox::setWidth(int aWidth)
{
    d->myWidth = aWidth;
}

void BHitBox::setHeight(int aHeight)
{
    d->myHeight = aHeight;
}
