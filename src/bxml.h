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

#ifndef BXML_H
#define BXML_H

#include <QList>
#include <QString>
#include <QSize>
#include <QRect>
#include <QIODevice>
#include <QDomElement>

class QDomDocument;

class Q_DECL_EXPORT BXml
{
public:
    enum SetGroupResult
    {
        Failed = -1,
        Successfully = 0,
        SuccessfullyWithWarning = 1
    };

    static bool hasType( const QString& aFile, const QString& aType);

    explicit BXml( const QString& aConfFile, const QString& aType);
    ~BXml();

    bool open( QIODevice::OpenMode aOpenMode = QIODevice::ReadOnly) const;

    SetGroupResult setGroup( const QString& aGroup) const;
    bool probeGroup( const QString& aGroupName) const;
    bool toRoot() const;
    SetGroupResult switchRoot( const QString& aRoot) const;
    bool up() const;
    QList<QString> listGroups() const;
    QList<QString> listRoots() const;
    QList<QString> listElements() const;

    bool writeInt( const QString& aName, int aValue);
    bool writeUInt( const QString& aName, unsigned int aValue);
    bool writeLong( const QString& aName, qlonglong aValue);
    bool writeULong( const QString& aName, qulonglong aValue);
    bool writeFloat( const QString& aName, float aValue);
    bool writeDouble( const QString& aName, double aValue);
    bool writeString( const QString& aName, const QString& aValue);
    bool writeSize( const QString& aName, const QSize& aValue);
    bool writePoint( const QString& aName, const QPoint& aValue);
    bool writeRect( const QString& aName, const QRect& aValue);
    bool writeByteArray( const QString& aName, const QByteArray& aValue);

    int readInt( const QString& aName, int aDefaultValue) const;
    unsigned int readUInt( const QString& aName, unsigned int aDefaultValue) const;
    qlonglong readLong( const QString& aName, qlonglong aDefaultValue) const;
    qulonglong readULong( const QString& aName, qulonglong aDefaultValue) const;
    float readFloat( const QString& aName, float aDefaultValue) const;
    double readDouble( const QString& aName, double aDefaultValue) const;
    QString readString( const QString& aName, const QString& aDefaultValue = QString()) const;
    QSize readSize( const QString& aName, const QSize& aSize = QSize(-1,-1)) const;
    QPoint readPoint( const QString& aName, const QPoint& aPoint = QPoint(-1,-1)) const;
    QRect readRect( const QString& aName, const QRect& aRect = QRect(-1,-1,-1,-1)) const;
    QByteArray readByteArray( const QString& aName, const QByteArray& aDefaultValue = QByteArray()) const;

    bool sync();

    QString fileName() const;
    QString fileDir() const;
    QString configType() const;

private:
    inline void createBaseElement( QDomDocument* aDocument, const QString& aBaseType) const;

    inline bool setStartElement() const;
    inline void rootType( const QString& aType) const;
    inline QString rootType() const;

    inline QDomDocument* document() const;
    inline QDomElement currentElement() const;
    inline void setCurrentElement( QDomElement aElement) const;
    inline QIODevice::OpenMode openMode() const;

    class BXmlPrivate* const d;
};

#endif //BXML_H
