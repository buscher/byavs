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

#ifndef BPOINTF_H
#define BPOINTF_H

class BPointF
{
public:
    BPointF(float aX = 0, float aY = 0) : myX(aX), myY(aY) {}
    void setX( float aX ) { myX = aX; }
    void setY( float aY ) { myY = aY; }

    float x() const { return myX; }
    float y() const { return myY; }
private:
    float myX;
    float myY;
};

#endif //BPOINTF_H
