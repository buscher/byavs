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

#ifndef BHITBOX_H
#define BHITBOX_H

class QRect;

class BHitBox
{
public:
    explicit BHitBox(float aX, float aY, int aWidth = 0, int aHeight = 0);
    explicit BHitBox(const QRect& aRect);
    explicit BHitBox(const BHitBox& aHitBox);
    ~BHitBox();

    float x() const;
    float y() const;

    void setX( float aX );
    void setY( float aY );

    int width() const;
    int height() const;

    void setWidth( int aWidth );
    void setHeight( int aHeight );

private:
    class BHitBoxPrivate* const d;
};

#endif //BHITBOX_H
