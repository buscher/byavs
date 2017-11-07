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

#include "bexplosion.h"

// class BExplosionPrivate
// {
// public:
// 
// };

BExplosion::BExplosion(float aX, float aY, int aWidth, int aHeight)
    : BObject(NULL,aX,aY,"explosion0.png", 4, BObject::Explosion )/*, d(new BExplosionPrivate)*/
{
    setObjectName("explosion");
    setWidth(aWidth);
    setHeight(aHeight);
    updateSelf( 160,"explosion1.png");
}

BExplosion::BExplosion(BHitBox* aHitBox)
    : BObject(NULL,0,0,"explosion0.png", 4, BObject::Explosion )/*, d(new BExplosionPrivate)*/
{
    setObjectName("explosion");
    setX( aHitBox->x() );
    setY( aHitBox->y() );
    setWidth( aHitBox->width() );
    setHeight( aHitBox->height() );
    updateSelf( 160,"explosion1.png");
}

BExplosion::~ BExplosion()
{
//     delete d;
}

void BExplosion::update(const QString& aImage)
{
    if ( aImage == "explosion1.png")
    {
        setImage( aImage );
        deleteAllHitBoxes();
        addHitBox( new BHitBox( x()+width()/10,y()+height()/10,
                                width()-((float)width()/10.0f)*2.0f,
                                height()-((float)height()/10.0f)*2.0f) );
        updateSelf( 160,"explosion2.png");
        return;
    }
    if ( aImage == "explosion2.png")
    {
        setImage( aImage );
        deleteAllHitBoxes();
        addHitBox( new BHitBox( x()+width()/5.0,y()+height()/4.0,
                                width()-((float)width()/6.0f)*3.0f,
                                height()-((float)height()/6.0f)*3.0f) );
        updateSelf( 160,"blank");
        return;
    }
    if ( aImage == "blank")
    {
        setVisible( false );
        deleteAllHitBoxes();
        setDone( true );
        return;
    }
}
