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

#ifndef BOBJKECT_H
#define BOBJKECT_H

#include <QString>
#include <QObject>
#include <QList>

#include "bhitbox.h"

#include "bglobal.h"

class QBasicTimer;

class BObject : public QObject, public BHitBox
{
public:
    enum ObjectType {
        BackGround = 1,
        Ship =       1 << 1,
        EnemyShip =  1 << 2,
        Shot =       1 << 3,
        Object =     1 << 4,
        ShipEffect = 1 << 5,
        Explosion =  1 << 6,
        Upgrade =    1 << 7
    };

    enum DirectionModifier {
        None    = 0,
        MirrorX = 1,
        MirrorY = 1 << 1
    };

    explicit BObject(QObject* aParent, int aX, int aY, const QString& aImage,
                     unsigned short aRenderlevel, BObject::ObjectType aType = Object,
                     short aImageModifier = None);
    ~BObject();

    QString image() const;
    short imageModifier() const;
    void setImage(const QString& aImage) const;
    unsigned short renderlevel() const;
    BObject::ObjectType type() const;
    bool visible() const;
    void setVisible(bool aVisible = true ) const;

    void updateSelf( unsigned int aMSec, const QString& aImage);

    void setX( float aX );
    void setY( float aY );

    void setWidth( int aWidth );
    void setHeight( int aHeight );

    virtual bool isDone() const;

    QObject* parent() const __attribute__((deprecated));

    QList<BHitBox*>* hitboxes() const;

    void addHitBox( BHitBox* aHitBox ) const;
    void setDone( bool aDone = true ) const;

protected:
    void deleteAllHitBoxes() const;

    virtual void update( const QString& aImage); //overwrite me
    virtual void timerEvent(QTimerEvent *aEvent);

private:
    inline QList<BHitBox*>* hitboxList() const;
    inline QString newImage() const;
    inline QBasicTimer* basicTimer() const;

    class BObjectPrivate* const d;
};

#endif //BOBJKECT_H
