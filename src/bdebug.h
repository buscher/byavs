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

#ifndef BDEBUG_H
#define BDEBUG_H

#include <stdlib.h>
#include <stdio.h>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QProcess>
#include <QRegExp>
#if 0
#   include <QMessageBox>
#endif


namespace BDebug
{
    /**
     * I didn't want to use Qt here for a simple getenv, but stupid ms visual studio forced me to
     */
    static bool isInit = false;
    static bool SBDebug = false;

    inline bool bDebug()
    {
        if (BDebug::isInit)
            return BDebug::SBDebug;
        BDebug::isInit = true;

        QStringList tEnvironment = QProcess::systemEnvironment();
        QStringList tResultList = tEnvironment.filter( QRegExp("BDEBUG*"));
        if (tResultList.size() > 0)
        {
            foreach (QString tVar, tResultList)
            {
                if (tVar.startsWith("BDEBUG="))
                {
                    tVar = tVar.right( strlen("BDEBUG="));
                    if (tVar != "0" && tVar.toLower() != "false")
                    {
                        BDebug::SBDebug = true;
                        return BDebug::SBDebug;
                    }
                }
            }
        }
        BDebug::SBDebug = false;
        return BDebug::SBDebug;
    }
};

#ifndef bWarning
#define bWarning() \
        qWarning()              \
                << __FILE__     \
                << ":"          \
                << __LINE__     \
                << ":"          \
                << __FUNCTION__ \
                << " "
#endif


#ifndef bError
#define bError()                \
        qCritical()             \
                << __FILE__     \
                << ":"          \
                << __LINE__     \
                << ":"          \
                << __FUNCTION__ \
                << " "
#endif

#if 0 //causes some crashes for not beeing thread safe
//TODO make lib and client version, as this uses again a non-lib libary
#ifndef bError
#define bError(a) \
    QMessageBox::critical(NULL,"Error", \
                QString(__FILE__) +":" +\
                QString::number(__LINE__) +":"+ \
                QString(__FUNCTION__) + " "+ \
                QString(a).toAscii().constData(), \
                   QMessageBox::Ok)
#endif
#endif

//TODO make debuglevel
#ifndef bDebug
#define bDebug(message) \
        if (BDebug::bDebug()) \
            qDebug()          \
            << __FILE__       \
            << ":"            \
            << __LINE__       \
            << ":"            \
            << __FUNCTION__   \
            << " "            \
            << message
#endif

#endif //BDEBUG_H
