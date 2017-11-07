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

#include "benviroment.h"

#include "bdebug.h"

#include <QCoreApplication>

#ifdef Q_OS_WIN
#   include <QProcess>
#endif

BEnviroment* BEnviroment::instance = 0;

class BEnviromentPrivate
{
public:
    QList<QDir> myLevelDirList;
    QList<QDir> myEnemyDirList;
    QList<QDir> myImageDirList;
};


BEnviroment::BEnviroment()
    : d(new BEnviromentPrivate)
{
    QString tDir = defaultProjectHomeFolder();
    checkHomeFolder(tDir);
    addFolders(tDir);

#if defined(Q_OS_LINUX) || defined(Q_OS_MAC)
    //if not set we should fail at pro file qmake
    tDir = QString(DATA_PATH);
    if (tDir.isEmpty())
        bError() << "empty DATA_PATH, this will cause problems";
    addFolders(tDir);
#endif

    tDir = QCoreApplication::applicationDirPath();
    addFolders(tDir); //just in case... for all os
}

BEnviroment::~ BEnviroment()
{
    instance = 0;
    delete d;
}

void BEnviroment::addFolders(const QString& aHomeFolder)
{
    if (QFile::exists(aHomeFolder + "/" + levelFolderName()))
        levelList()->append( QDir(aHomeFolder + "/" + levelFolderName()));
    if (QFile::exists(aHomeFolder + "/" + imageFolderName()))
        imageList()->append( QDir(aHomeFolder + "/" + imageFolderName()));
    if (QFile::exists(aHomeFolder + "/" + enemyFolderName()))
        enemyList()->append( QDir(aHomeFolder + "/" + enemyFolderName()));
}

QList<QDir> BEnviroment::levelDirList() const
{
    return d->myLevelDirList;
}

const char* BEnviroment::levelFolderName()
{
    return "level";
}

const char* BEnviroment::projectName()
{
    return "byavs";
}

QString BEnviroment::defaultProjectHomeFolder()
{
#if defined(Q_OS_WIN)
    return getEnviromentVariable("APPDATA") + "/" + projectName();
#elif defined(Q_OS_MAC)
    return QDir::homePath() + "/Library/Preferences/" + projectName();
#else /*defined(Q_OS_LINUX)*/ /*default assume rest is unix like*/
    return QDir::homePath() + "/." + projectName();
#endif
}

#ifdef Q_OS_WIN
QString BEnviroment::getEnviromentVariable(const QString& aEnv)
{
    QStringList tEnvironment = QProcess::systemEnvironment();
    for (int i = 0; i < tEnvironment.size(); ++i)
    {
        QStringList list1 = tEnvironment.at(i).split("=");
        if ( list1[0] == aEnv)
            return list1[1];
    }
    bError("APPDATA var not found, this should not happen");
    return QDir::homePath();
}
#endif

void BEnviroment::checkHomeFolder(const QString& aFolder)
{
    QDir tHome( aFolder );
    if (!tHome.exists())
        tHome.mkpath( aFolder );
}

QList< QDir > BEnviroment::enemyDirList() const
{
    return d->myEnemyDirList;
}

const char* BEnviroment::enemyFolderName()
{
    return "enemy";
}

QList< QDir >* BEnviroment::levelList() const
{
    return &d->myLevelDirList;
}

QList< QDir >* BEnviroment::enemyList() const
{
    return &d->myEnemyDirList;
}

BEnviroment* BEnviroment::getIntance()
{
    if (instance)
        return instance;
    instance = new BEnviroment();
    return instance;
}

void BEnviroment::free()
{
    delete instance;
}

const char* BEnviroment::imageFolderName()
{
    return "image";
}

QList< QDir >* BEnviroment::imageList() const
{
    return &d->myImageDirList;
}

QList< QDir > BEnviroment::imageDirList() const
{
    return d->myImageDirList;
}
