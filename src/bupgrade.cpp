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

#include "bupgrade.h"

BUpgrade::BUpgrade(const QString& aImage, int aDamageIncrease, int aFireType)
    : BObject(NULL, 0, 0, aImage, 4, BObject::Upgrade, 0)
{
    myFireType = aFireType;
    myDamageIncrease = aDamageIncrease;
    addHitBox( new BHitBox( x(), y(), width(), height()) );
}

int BUpgrade::damageIncrease() const
{
    return myDamageIncrease;
}

int BUpgrade::fireType() const
{
    return myFireType;
}
