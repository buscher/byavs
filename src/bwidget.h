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

#ifndef BWIDGET_H
#define BWIDGET_H

#include "ui_bwidget.h"
#include <QWidget>

class BRender;
class BObjectList;
class BWorldManager;

class BWidget : public QWidget, public Ui::BWidget
{
Q_OBJECT

public:
    BWidget();
    ~BWidget();

protected:
    virtual void resizeEvent ( QResizeEvent* aEvent );
    virtual void showEvent ( QShowEvent* aEvent );
    virtual void keyPressEvent ( QKeyEvent* aEvent );
    virtual void keyReleaseEvent ( QKeyEvent* aEvent );
    virtual void paintEvent ( QPaintEvent* aEvent );
    virtual void focusOutEvent ( QFocusEvent * event );

protected slots:
    void updateGLWidget( bool aResized = false);
    void updateFps();

private:
    inline void restoreWindow();
    inline void saveWindowSettings();

    inline QPainter* painter() const;
    inline BRender* render() const;
    inline BObjectList* objectList() const;
    inline BWorldManager* worldManager() const;

    class BWidgetPrivate* const d;
};

#endif //BGLWIDGET_H
