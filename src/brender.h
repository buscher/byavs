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

#ifndef BRENDER_H
#define BRENDER_H

#include "bobjectlist.h"

#include <QObject>

class QWidget;
class QPaintEvent;
class QPainter;
class BObject;

class BRender : public QObject
{
Q_OBJECT

public:
    BRender(QWidget *aParent);
    ~BRender();

    void update(QPainter* aPainter, BObjectList *aObjectList, float aPos, bool aResized);

signals:
    void arrivedLevelEnd() const;
    void fixedWidgetWidthChanged(int aWidth) const;

private:
    inline void paintBackGround( BObject* aObject, QPainter *aPainter, bool aResized = false);
    inline void paintObject( BObject* aObject, QPainter *aPainter);
    inline QWidget* widget() const;
    inline float factor() const;
    inline float correctPosition(float aPos) const;
    inline float position() const;

    inline void paintHitBox( BObject* aObject, QPainter *aPainter);

    class BRenderPrivate* const d;
};

#endif //BRENDER_H
