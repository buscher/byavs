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


#include "bimagecache.h"

#include <QPixmap>
#include <QDir>
#include <QApplication>
#include <QPainter>

#include "bdebug.h"
#include "benviroment.h"

BImageCache* BImageCache::instance = NULL;

class BImageCachePrivate
{
public:
    QHash<QString,QPixmap*> myImageHash;
};

BImageCache::BImageCache()
    : d (new BImageCachePrivate)
{
//     if (QApplication::type() != QApplication::GuiClient)
//     {
//         bError() << "cant create pixmap in nongui app, expect problem!";
//     }
}

BImageCache::~ BImageCache()
{
// This is pretty evil, because we have multiple times of the same QPixmap* under
// a different keys in it, but deleteting it twice would result in a segfault
    QPixmap *tPix;
    QHash<QString, QPixmap*>::const_iterator tHashIndex = hash()->constBegin();
    while (tHashIndex != hash()->constEnd())
    {
        tPix = tHashIndex.value();
        QList<QString> tList = hash()->keys(tPix);
        for (int i = 0; i < tList.count(); ++i)
        {
            hash()->remove( tList.at( i ) );
        }
        delete tPix;
        tHashIndex = hash()->constBegin();
    }

    if (hash()->size() > 0)
        bError() << "hash size after delete greater 0: " << hash()->size();

    delete d;
    BImageCache::instance = NULL;
}

QPixmap* BImageCache::getPixmapByName(const QString& aName, unsigned int aWidth, unsigned int aHeight, bool aMirrorX, bool aMirrorY) const
{
    QString tSizeName = aName;
    if ((aWidth > 0 && aHeight > 0) || aMirrorX || aMirrorY)
        tSizeName += buildSizeName(aWidth, aHeight, aMirrorX, aMirrorY);
    QHash<QString, QPixmap*>::const_iterator i = hash()->find(tSizeName);
    if (i != hash()->end() && i.key() == tSizeName)
        return i.value();
    else
        return loadImageByName(aName, aWidth, aHeight, aMirrorX, aMirrorY);
}

QHash< QString, QPixmap* > * BImageCache::hash() const
{
    return &d->myImageHash;
}

QPixmap* BImageCache::loadImageByName( const QString& aName, unsigned int aWidth, unsigned int aHeight, bool aMirrorX, bool aMirrorY) const
{
    QHash<QString, QPixmap*>::const_iterator i = hash()->find(aName);
    if (i != hash()->end() && i.key() == aName)
        return scaleAndInsert( aName, i.value(), aWidth, aHeight, aMirrorX, aMirrorY);

    QPixmap *tPix = NULL;
    QList<QDir> tDirList = BEnviroment::getIntance()->imageDirList();
    int tDirSize = tDirList.size();
    for (int ti = 0; ti < tDirSize; ++ti)
    {
        QDir tDir(tDirList.at(ti));
        bDebug("defaultImagePath: " + tDir.absolutePath() + " aName: " + aName);
        QStringList tList = tDir.entryList( QStringList(aName),
                                            QDir::Files, QDir::Name);
        int tSize = tList.size();
        for (int i = 0; i < tSize; ++i)
        {
            if (tList.at(i).startsWith(aName))
            {
                tPix = new QPixmap( tDir.absolutePath() + "/" + tList.at(i) );
                bDebug("loading: " + tDir.absolutePath() + "/" + tList.at(i) );
                hash()->insert(aName,tPix);
                if (aWidth > 0 && aHeight > 0)
                    return scaleAndInsert( aName, tPix, aWidth, aHeight, aMirrorX, aMirrorY);
                else {
                    QString tSizeName = buildSizeName(tPix->width(), tPix->height(), aMirrorX, aMirrorY);
                    hash()->insert(tSizeName,tPix);
                    return tPix;
                }
            }
        }
    }

    //souldnt happen at all, but with this is more crashfree
    if (aWidth > 0 && aHeight > 0)
        tPix = new QPixmap(aWidth, aHeight);
    else
        tPix = new QPixmap(128,128);
    tPix->fill( Qt::red );
    hash()->insert(aName,tPix);
    return tPix;
}

BImageCache* BImageCache::getInstance()
{
    if (!BImageCache::instance)
        BImageCache::instance = new BImageCache;
    return BImageCache::instance;
}

void BImageCache::free()
{
    if (BImageCache::instance)
        delete BImageCache::instance;
}

QPixmap* BImageCache::scaleAndInsert( const QString& aName, QPixmap* aPix,
                                      unsigned int aWidth, unsigned int aHeight,
                                      bool aMirrorX, bool aMirrorY ) const
{
    QPixmap *tScalePix;
    if (aWidth > 0 && aHeight > 0)
        tScalePix = new QPixmap( aPix->scaled( aWidth, aHeight,
                                 Qt::IgnoreAspectRatio, Qt::SmoothTransformation ) );
    else
        tScalePix = aPix;

    QPixmap *tScalePix2;
    if (aMirrorX || aMirrorY)
    {
        // improve me?
        tScalePix2 = new QPixmap( QPixmap::fromImage( tScalePix->toImage().mirrored(aMirrorX, aMirrorY)) );
        if (aWidth > 0 && aHeight > 0)
           delete tScalePix;
    } else
        tScalePix2 = tScalePix;

    QString tSizeName = aName + buildSizeName(aWidth, aHeight, aMirrorX, aMirrorY);
    hash()->insert(tSizeName, tScalePix2);
    return tScalePix2;
}

QString BImageCache::buildSizeName(unsigned int aX, unsigned int aY, bool aMirrorX, bool aMirrorY ) const
{
    QString tName = QString::number(aX) + ":" + QString::number(aY);
    tName += aMirrorX ? ":1" : ":0";
    tName += aMirrorY ? ":1" : ":0";
    return tName;
}
