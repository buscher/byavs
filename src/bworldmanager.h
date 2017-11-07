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

#ifndef BWORLDMANAGER_H
#define BWORLDMANAGER_H

#include <QString>
#include <QObject>

class BObjectList;
class QKeyEvent;
class BXml;
class BShip;
class BShot;
class BObject;
class BExplosion;
class QWidget;
class BHitBox;
class BEnemyShip;

class BWorldManager : public QObject
{
Q_OBJECT

public:

    enum InputDirection {
        None =  0,
        Up =    1,
        Down =  1 << 1,
        Right = 1 << 2,
        Left =  1 << 3
    };

    explicit BWorldManager( BObjectList* aObjectList);
    ~BWorldManager();

    void keyPressEvent ( QKeyEvent* aEvent );
    void keyReleaseEvent ( QKeyEvent* aEvent );
    void keyReleaseAll();

    bool loadLevel(const QString& aLevel) const;
    void start() const;
    void update() const;

    float position() const;

protected slots:
    void slotStopPositionUpdate() const; //to avoid overflows
    void slotWidgetWidthChanged( int aWidth) const;

private:
    inline void deleteExplosions() const;
    inline void appendObject(BObject* aObject) const;
    inline void appendObjectEffect(BObject* aObject) const;
    inline void appendEnemy(BEnemyShip* aEnemy) const;

    inline BHitBox* collision(BObject* aObj1, BObject* aObj2) const;
    inline bool checkCollision() const;
    inline bool checkShipCollision(BShip* aShip, BObject *aObj) const;
    inline bool checkShotCollision(BShot* aShot, BObject *aObj) const;
    inline bool collision(BHitBox* aObj1, BHitBox* aObj2) const;

    inline void updatePosition() const;
    inline void updateShotPos(int aMSec) const; //TODO merge me?
    inline void updateShipPos(int aMSec) const; //TODO merge me?
    inline void updateEnemyPos(int aMSec) const; //TODO merge me? maybe not me
    inline void updateEnemyFire() const; //TODO merge me? maybe not me
    inline void updateEnemyTrigger() const;

    inline bool outsideWindow(BHitBox* aBox) const;

    inline void deleteItem(BObject* aObject) const;
    inline QList<BObject*> *explosionList() const;
    inline QList<BEnemyShip*> *enemyList() const;
    inline BShip* ship() const;
    inline BObjectList* objectList() const;

    inline int keyToDirection(int aKeyCode) const;
    inline float directionToX(int aDirection) const;
    inline float directionToY(int aDirection) const;

    inline int widgetWidth() const;

    inline QString findLevel(const QString& aLevel) const;
    inline bool initLevel(const QString& aLevel) const;
    inline void createHitBoxes(BObject* aObject, BXml* aXml) const;
    inline void readObject(BXml* aXml, int aObject) const;
    inline void readEnemy(BXml* aXml, int aObject) const;
    inline void readEnemyDirection(BXml* aXml, BEnemyShip* aEnemy) const;

    class BWorldManagerPrivate* const d;
};

#endif //BWORLDMANAGER_H
