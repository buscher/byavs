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

#include "bobjectlist.h"

#include "bobject.h"
#include "bdebug.h"

#include <QTime>

class BObjectListPrivate
{
public:
    unsigned short myLowest;
    unsigned short myHigest;
};


BObjectList::BObjectList()
    : QList<BObject*>(), d(new BObjectListPrivate)
{
    d->myHigest = 0;
    d->myLowest = 0;
}

BObjectList::~ BObjectList()
{
    delete d;
}

void BObjectList::append(BObject* aObject)
{
    if (lowestLevel() > aObject->renderlevel())
        d->myLowest = aObject->renderlevel();
    if (highestLevel() < aObject->renderlevel())
        d->myHigest = aObject->renderlevel();
    QList<BObject*>::append(aObject);
}

unsigned short BObjectList::lowestLevel() const
{
    return d->myLowest;
}

unsigned short BObjectList::highestLevel() const
{
    return d->myHigest;
}

QList< BObject * > BObjectList::level(unsigned short aLevel) const
{
//     QTime t; t.start();
    QList<BObject*> tList;
    int tSize = size();
    for (int i = 0; i < tSize; ++i)
    {
        if (at(i)->renderlevel() == aLevel)
            tList.append(at(i));
    }
//     bDebug("level: " + QString::number(t.elapsed()));
    return tList;
}

BObjectList BObjectList::copy() const
{
//     QTime t; t.start();
    BObjectList tList;
    int tSize = size();
    for (int i = 0; i < tSize; ++i)
    {
        tList.append( at(i) );
    }
//     bDebug("copy: " + QString::number(t.elapsed()));
    return tList;
}
