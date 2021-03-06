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

#ifndef BEXPLOSION_H
#define BEXPLOSION_H

#include "bobject.h"

class BExplosion : public BObject
{
public:
    explicit BExplosion(float aX, float aY, int aWidth, int aHeight);
    explicit BExplosion(BHitBox* aHitBox);
    ~BExplosion();

protected:
    virtual void update( const QString& aImage);

private:
//     class BExplosionPrivate* const d;
};

#endif //BEXPLOSION_H
