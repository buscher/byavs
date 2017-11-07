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

#ifndef BSHIP_H
#define BSHIP_H

#include "bship.h"
#include "bobject.h"
#include "bshot.h"

class BObjectList;
class BBackBurner;

class BShip : public BObject
{
public:

    enum FireType {
        Default = 0,
        Double,
        Tripple
    };

    explicit BShip( int aX, int aY, const QString& aImage, short aImageModifier = BObject::None );
    ~BShip();

    QList<BShot*> fire();

    void setX(float aX);
    void setY(float aY);

    void setWidth( int aWidth );
    void setHeight( int aHeight );

    BBackBurner* backBurner();

    void upgrade( BObject* aUpgrade);

private:
    short fireType() const;

    class BShipPrivate* const d;

};

#endif //BSHIP_H
