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

#ifndef BSHOT_H
#define BSHOT_H

#include "bobject.h"
#include "bpointf.h"

class BHitBox;

class BShot : public BObject
{
public:
    enum ShotType {
        Player = 1,
        Enemy  = 1 <<1
    };

    BShot(QObject* aParent, float aSpeed, int aImageModifier = BObject::None, BShot::ShotType aShotType = BShot::Enemy);
    ~BShot();

    float speed() const;

    uint damage() const;
    int shotType() const;

    void hit(BHitBox* aObject);
    bool isHit() const;

    void setDirection(float aX, float aY);
    BPointF* direction() const;

protected:
    virtual void update( const QString& aImage);

private:
    class BShotPrivate* const d;
};

#endif //BSHOT_H
