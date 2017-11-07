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

#ifndef BENEMYSHIP_H
#define BENEMYSHIP_H

#include "bship.h"
#include "bobject.h"
#include "bshot.h"


#include <QList>
#include <QPoint>

class BObjectList;
class BUpgrade;

class BEnemyShip : public BObject
{
public:

    enum DirectionType {
        OneShot = 1, /*default*/
        Repeat = 1 << 1
    };

    explicit BEnemyShip( int aX, int aY, const QString& aImage, uint aXTrigger,
                         short aImageModifier = BObject::None );
    ~BEnemyShip();

    BShot* fire();

    short directionType() const;
    void setDirectionType( short aType );

    void show() const;

    void setWidth( int aWidth );
    void setHeight( int aHeight );

    uint trigger() const;
    uint health() const;
    float speed() const;

    void setHealth(uint aHealth);
    void setSpeed(float aSpeed);
    void setFireInterval(uint aInterval);

    void addPoint( const QPoint& aPoint) const;
    QList<QPoint> points() const;
    void removeFirst() const;
    void cyclePoint() const;

    BUpgrade* item() const;
    void setItem( BUpgrade* aItem);

// protected:
//     virtual void update( const QString& aImage); //overwrite me

private:
    inline QList<QPoint>* pointList() const;

    class BEnemyShipPrivate* const d;
};

#endif //BENEMYSHIP_H
