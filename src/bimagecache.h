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

#ifndef BIMAGECACHE_H
#define BIMAGECACHE_H

#include <QHash>
#include <QString>

class QPixmap;

class Q_DECL_EXPORT BImageCache
{
public:
    QPixmap* getPixmapByName( const QString& aName, unsigned int aWidth = 0, unsigned int aHeight = 0, bool aMirrorX = false, bool aMirrorY = false) const;

    static BImageCache* getInstance();
    static void free();

private:
    explicit BImageCache();
    ~BImageCache();

    inline QString buildSizeName(unsigned int aX, unsigned int aY, bool aMirrorX = false, bool aMirrorY = false) const;

    inline QHash<QString,QPixmap*>* hash() const;
    inline QPixmap* loadImageByName( const QString& aName, unsigned int aWidth = 0, unsigned int aHeight = 0, bool aMirrorX = false, bool aMirrorY = false) const;
    inline QPixmap* scaleAndInsert( const QString& aName, QPixmap* aPix, unsigned int aWidth, unsigned int aHeight, bool aMirrorX = false, bool aMirrorY = false) const;

    static BImageCache* instance;

    class BImageCachePrivate* const d;
};

#endif //BIMAGECACHE_H
