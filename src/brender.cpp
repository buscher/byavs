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

#include "brender.h"

#include "bimagecache.h"
#include "bobject.h"
#include "bdebug.h"

#include <QWidget>
#include <QPainter>
#include <QTime>

#include <math.h>

// #define BDRAW_HITBOXES 1

#ifdef BDRAW_HITBOXES
#   undef BDRAW_HITBOXES
#endif

class BRenderPrivate
{
public:
    QWidget* myWidget;
    unsigned int myBgPixWidth;
    unsigned int myBgPixHeight;
    unsigned int myBgPixMaxWidth;
    float myPixFactor;
    bool myAtLevelEnd;
    float myCorrectedPosition;
};


BRender::BRender(QWidget * aParent)
    : d (new BRenderPrivate)
{
    d->myWidget = aParent;
    d->myBgPixHeight = 0;
    d->myBgPixWidth = 0;
    d->myBgPixMaxWidth = 0;
    d->myPixFactor = 0.0;
    d->myAtLevelEnd = false;
}

BRender::~ BRender()
{
    BImageCache::free();
    delete d;
}

void BRender::update(QPainter *aPainter, BObjectList *aObjectList, float aPos, bool aResized)
{
    for (unsigned short i = aObjectList->lowestLevel(); i <= aObjectList->highestLevel(); ++i)
    {
        QList<BObject*> tList = aObjectList->level( i );
        int tSize = tList.size();
        for (int j = 0; j < tSize; ++j )
        {
            switch (tList.at( j )->type())
            {
                case BObject::BackGround:
                    d->myCorrectedPosition = correctPosition(aPos);
                    paintBackGround( tList.at( j ), aPainter, aResized);
                    break;
                default:
                    paintObject( tList.at( j ), aPainter);
#ifdef BDRAW_HITBOXES
                    paintHitBox( tList.at( j ), aPainter);
#endif
                    break;
            }
        }
    }
}

void BRender::paintBackGround(BObject* aObject, QPainter *aPainter, bool aResized)
{
//     QTime t; t.start();
    if (aResized)
    {
        QPixmap* tPix = BImageCache::getInstance()->getPixmapByName( aObject->image() );
        d->myPixFactor = float(widget()->height())/float(tPix->height());
        d->myBgPixHeight = /*tPix2->height();*/ widget()->height();
        d->myBgPixWidth = /*tPix2->width()*/ widget()->width();
        d->myBgPixMaxWidth = qRound(factor()*float(tPix->width()));

    //     bDebug(QString::number(widget()->width()) +" "+ QString::number(widget()->height())+" "+QString::number(tPix->height()));

        QPixmap* tPix2 = BImageCache::getInstance()->getPixmapByName(aObject->image(), d->myBgPixMaxWidth, widget()->height() );

        aPainter->drawPixmap(qRound(aObject->x()*factor() - position()), int(aObject->y()), *tPix2);

        if (receivers(SIGNAL(fixedWidgetWidthChanged(int))) > 0)
            emit fixedWidgetWidthChanged( qRound(d->myBgPixWidth/factor()) );
    } else {
        QPixmap* tPix = BImageCache::getInstance()->getPixmapByName( aObject->image(), d->myBgPixMaxWidth, d->myBgPixHeight );
        int tPos = qRound(aObject->x()*factor() - position());
        aPainter->drawPixmap(tPos, qRound(aObject->y()), *tPix);
    }
//     bDebug("BGPaint: " + QString::number(t.elapsed()));
}

QWidget* BRender::widget() const
{
    return d->myWidget;
}

void BRender::paintObject(BObject* aObject, QPainter* aPainter)
{
    if (!aObject->visible())
        return;

//     QTime t; t.start();
    int tWidth = lrintf(aObject->width()*factor());
    int tHeight = lrintf(aObject->height()*factor());

    //HACK this is pure evil, ship is moved with float position, that req. -float
    //     otherwise it flickers when moving, but objects arnt moved so -int is
    //     correct to avoid flicker
    //partly SOLUTION? make extra function again in update, to avoid double type check
    int tX;
    if (aObject->type() == BObject::Ship || aObject->type() == BObject::ShipEffect ||
        aObject->type() == BObject::EnemyShip)
        tX = qRound(aObject->x()*factor() - position());
    else
        tX = (aObject->x()*factor() - qRound(position())); //HACK wanted int cast

    QPixmap* tPix = BImageCache::getInstance()->getPixmapByName( aObject->image(),
                                    tWidth, tHeight,
                                    aObject->imageModifier() & BObject::MirrorX,
                                    aObject->imageModifier() & BObject::MirrorY );

    aPainter->drawPixmap( tX,
                          qRound( aObject->y()*factor()),
                          tWidth, tHeight, *tPix);

//     bDebug("ObjectPaint: " + QString::number(t.elapsed()));
}

float BRender::factor() const
{
    return d->myPixFactor;
}

float BRender::correctPosition(float aPos) const
{
    float tPos = aPos*factor();
    if ((d->myBgPixWidth+tPos) > d->myBgPixMaxWidth)
    {
        tPos = d->myBgPixMaxWidth - d->myBgPixWidth;
        if (!d->myAtLevelEnd)
        {
            if (receivers(SIGNAL(arrivedLevelEnd())) > 0)
                emit arrivedLevelEnd();
            d->myAtLevelEnd = true;
        }
    }
    return tPos;
}

void BRender::paintHitBox(BObject* aObject, QPainter* aPainter)
{
    if (!aObject->visible())
        return;

    if (aObject->hitboxes()->isEmpty())
        return;

    QPen tOldPen = aPainter->pen();
    QBrush tOldBrush = aPainter->brush();

    aPainter->setPen( Qt::red );
    BHitBox *tBox;
    for (int i = 0; i < aObject->hitboxes()->size(); ++i)
    {
        tBox = aObject->hitboxes()->at( i );
        aPainter->drawRect( qRound( tBox->x()*factor() - position()),
                          qRound( tBox->y()*factor()), qRound(tBox->width()*factor()),
                          qRound( tBox->height()*factor()));
    }
    aPainter->setPen( tOldPen );
    aPainter->setBrush( tOldBrush );
}

float BRender::position() const
{
    return d->myCorrectedPosition;
}
