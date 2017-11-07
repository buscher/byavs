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

#ifndef BBACKBURNER_H
#define BBACKBURNER_H

#include "bobject.h"
#include "bpointf.h"

class BBackBurner : public BObject
{
public:
    explicit BBackBurner(BObject* aParent, float aX, float aY, int aWidth, int aHeight, short ImageModifier = BObject::None);
    ~BBackBurner();

    void setX( float aX);
    void setY( float aY);

    void setWidth( int aWidth);
    void setHeight( int aHeight);

    QList<BObject*> objects() const;

// protected:
//     virtual void update( const QString& aImage);

private:
    inline void checkNext();
    inline void moveAll();
    inline void moveFirst();

    inline QList<BObject*>* objectList() const;
    inline QList<BPointF*>* pointList() const;

    class BBackBurnerPrivate* const d;
};

#endif //BEXPLOSION_H
