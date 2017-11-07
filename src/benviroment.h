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

#ifndef BENVIROMENT_H
#define BENVIROMENT_H

#include <QList>
#include <QDir>

class Q_DECL_EXPORT BEnviroment
{
public:

    static const char* projectName();
    static QString defaultProjectHomeFolder();
    QList<QDir> levelDirList() const;
    QList<QDir> enemyDirList() const;
    QList<QDir> imageDirList() const;

    static BEnviroment* getIntance();
    static void free();

private:
    static BEnviroment* instance;

    BEnviroment();
    ~BEnviroment();

    inline const char* levelFolderName();
    inline const char* enemyFolderName();
    inline const char* imageFolderName();

    inline QList<QDir>* levelList() const;
    inline QList<QDir>* enemyList() const;
    inline QList<QDir>* imageList() const;

    inline void checkHomeFolder(const QString& aFolder);
    inline void addFolders(const QString& aHomeFolder);

#ifdef Q_WS_WIN
    inline static QString getEnviromentVariable(const QString& aEnv);
#endif

    class BEnviromentPrivate* const d;
};

#endif //BENVIROMENT_H
