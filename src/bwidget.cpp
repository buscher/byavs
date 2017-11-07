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

#include "bwidget.h"

#include "bobject.h"
#include "bobjectlist.h"
#include "brender.h"
#include "bworldmanager.h"
#include "bdebug.h"
#include "bxml.h"
#include "benviroment.h"

#include <QPainter>
#include <QTimer>
#include <QTime>
#include <QResizeEvent>
#include <QDesktopWidget>

class BWidgetPrivate
{
public:
    BRender* myRender;
    BObjectList myObjectList;
    QPainter myPainter;
    QTimer* myTimer;
    QTimer* myFPSTimer;
    BWorldManager* myManager;
    int myFPS;
};


BWidget::BWidget()
    : QWidget(), d(new BWidgetPrivate)
{
    setupUi(this);
    d->myFPS = 0;

    d->myRender = new BRender(myGLWidget);
    d->myManager = new BWorldManager( objectList() );

    connect(render(), SIGNAL(arrivedLevelEnd()), worldManager(), SLOT(slotStopPositionUpdate()));
    connect(render(), SIGNAL(fixedWidgetWidthChanged(int)), worldManager(), SLOT(slotWidgetWidthChanged(int)));

    worldManager()->loadLevel("level01");

    restoreWindow();

    int tHeight = int((QApplication::desktop()->width()/4.0f)*3.0f);
    setMaximumSize ( QApplication::desktop()->width(), tHeight );

    //game starts here
    worldManager()->start();

    d->myFPSTimer = new QTimer();
    connect ( d->myFPSTimer, SIGNAL(timeout()), this, SLOT(updateFps()));
    d->myFPSTimer->start( 1000 );

    d->myTimer = new QTimer();
    connect ( d->myTimer, SIGNAL(timeout()), this, SLOT(update()));
    d->myTimer->start( 1000.0f/1000.0f );
}

BWidget::~BWidget()
{
    disconnect(d->myFPSTimer,0,0,0);
    disconnect(d->myTimer,0,0,0);
    d->myFPSTimer->stop();
    d->myTimer->stop();

    saveWindowSettings();

    delete d->myFPSTimer;
    delete d->myTimer;

    delete render();
    delete worldManager();

    while(!objectList()->isEmpty())
        delete objectList()->takeFirst();

    delete d;
}

void BWidget::updateGLWidget( bool aResized )
{
//     bDebug("start: " + QTime::currentTime().toString("mm:ss:zzz"));
//     QTime t; t.start();
    worldManager()->update();
//     bDebug( "worldManager()->update(): " + QString::number(t.elapsed()));
//     t.restart();
    painter()->begin( myGLWidget );
//     painter()->setRenderHint(QPainter::Antialiasing);
    render()->update( painter(), objectList()/*->copy()*/ , worldManager()->position(), aResized);
    painter()->end();

//     bDebug("end: " + QTime::currentTime().toString("mm:ss:zzz"));
//     bDebug( "render()->update(...): " + QString::number(t.elapsed()));
    d->myFPS++;
}

BRender* BWidget::render() const
{
    return d->myRender;
}

BObjectList* BWidget::objectList() const
{
    return &d->myObjectList;
}

QPainter* BWidget::painter() const
{
    return &d->myPainter;
}

void BWidget::resizeEvent(QResizeEvent* aEvent)
{
    int tWidth = aEvent->size().width();
    int tHeight = aEvent->size().height();

    float tXPart = (float)tWidth / (float)aEvent->oldSize().width();
    float tYPart = (float)tHeight / (float)aEvent->oldSize().height();

//HACK
    if ((tXPart == 1.0f) && (tYPart != 1.0f))
        tWidth = int(tHeight/3.0f*4.0f);
    else if ((tYPart == 1.0f) && (tXPart != 1.0f))
        tHeight = int(tWidth/4.0f*3.0f);
    else if (tXPart > tYPart)
        tHeight = int(tWidth/4.0f*3.0f);
    else if (tXPart < tYPart)
        tWidth = int(tHeight/3.0f*4.0f);

    resize( tWidth, tHeight);

    updateGLWidget(true);
//    QWidget::resizeEvent(aEvent);
}

void BWidget::showEvent(QShowEvent* /*aEvent*/)
{
    updateGLWidget();
}

void BWidget::keyReleaseEvent(QKeyEvent* aEvent)
{
    QWidget::keyReleaseEvent( aEvent );
    worldManager()->keyReleaseEvent( aEvent );
}

void BWidget::keyPressEvent(QKeyEvent* aEvent)
{
    QWidget::keyPressEvent( aEvent );
    worldManager()->keyPressEvent( aEvent );
}

BWorldManager* BWidget::worldManager() const
{
    return d->myManager;
}

void BWidget::updateFps()
{
    setWindowTitle("byavs FPS: " + QString::number( d->myFPS ));
    d->myFPS = 0;
}

void BWidget::restoreWindow()
{
    BXml tXml( BEnviroment::defaultProjectHomeFolder() + "/" +
               BEnviroment::projectName()+ ".conf", "CONFIG" );
    if (tXml.open(QIODevice::ReadOnly))
    {
        tXml.setGroup( "properties" );
            setGeometry( tXml.readRect( "windowsize", QRect(200,200,400,400) ));
        tXml.up();
    }
}

void BWidget::saveWindowSettings()
{
    BXml tXml( BEnviroment::defaultProjectHomeFolder() + "/" +
               BEnviroment::projectName()+ ".conf", "CONFIG" );
    if (tXml.open(QIODevice::ReadWrite))
    {
        tXml.setGroup( "properties" );
            tXml.writeRect( "windowsize", geometry() );
        tXml.up();
        tXml.sync();
    }
}

void BWidget::paintEvent(QPaintEvent* aEvent)
{
    updateGLWidget();
    QWidget::paintEvent(aEvent);
}

void BWidget::focusOutEvent(QFocusEvent* aEvent)
{
    worldManager()->keyReleaseAll();
    QWidget::focusOutEvent(aEvent);
}
