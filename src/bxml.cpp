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

#include "bxml.h"

#include <QFile>
#include <QDomNodeList>
#include <QDir>
#include <QFileInfo>

#include "bdebug.h"

//TODO #define is.. "not so great" change it to something nice, later
#define HEIGHT "height"
#define WIDTH "width"

#define X1 "x1"
#define X2 "x2"
#define Y1 "y1"
#define Y2 "y2"

class BXmlPrivate
{
public:
    QString myXmlFile;
    QString myXmlDir;
    QDomDocument *myDocument;
    QDomElement myCurrentElement;
    QIODevice::OpenMode myOpenMode;
    QString myConfigType;
    QString myRootType;
};


/**
 * BXml is a Class to handle easy all xml-like files. It opens the given <i>aXmlFile</i>
 * The BXml::ConfigType <i>aType</i> is just for
 * predefined BTowerDefense Formats. If you want to open another xmlfile use
 * BXml::Undefined but then you have to call switchRoot( const QString& aRoot) const;
 * first (or QList&#060;QString&#062; listRoots() const; to get all avaiable roots).<br/>
 * use open( QIODevice::OpenMode ); after this.
 * @param aXmlFile File to Open
 * @param aType opens file with predefines ConfigType
 * @param aOpenMode QIODevice::OpenMode
 */
BXml::BXml( const QString& aXmlFile, const QString& aType )
    : d(new BXmlPrivate)
{
    bDebug("File: " + aXmlFile);
    d->myConfigType = aType;
    d->myRootType = aType;
    d->myDocument = NULL;
    d->myXmlFile = QFileInfo( aXmlFile).absoluteFilePath();
    bDebug("absFile: " + fileName());
    bDebug("BaseType: "+ rootType() );

    d->myXmlDir = QFileInfo( fileName() ).absolutePath();
    bDebug("XmlDir: " + fileDir());
}

/**
 * If the File does not exist and we have at least QIODevice::WriteOnly it will be created,
 * if the given aXmlFile is empty and we have QIODevice::WriteOnly permissions it will
 * create a dummy file in your homedir, to make it less crashy. But for all this will
 * an error message appaer at runtime.
 * @param aOpenMode QIODevice::OpenMode
 * @return false if it failed, true if everything worked
 */
bool BXml::open(QIODevice::OpenMode aOpenMode) const
{
    if ( fileName().isEmpty())
    {
        bError() << "No File given";
        return false;
    }

    d->myOpenMode = aOpenMode;
    d->myDocument = new QDomDocument( rootType() );
    QFile tFile( fileName() );

    if (!QFile::exists( fileName()) && !(aOpenMode & QIODevice::WriteOnly))
    {
        bError() << "file does not exists and write permissions are missing";
        return false;
    }

    if (!tFile.open(openMode()))
    {
        bWarning() << "could not create/open conf file";
        return false;
    }

    QString tError;
    int tLine = 0, tColumn = 0;
    if (!document()->setContent(&tFile,&tError,&tLine ,&tColumn))
    {
        bWarning() << "setContent error: " << tError << " in line:column "
                   << QString::number(tLine) << ":" << QString::number(tColumn);
//         return false;
    }
    tFile.close();

    return setStartElement();
}

BXml::~BXml()
{
    if (document()) delete document();
    delete d;
}

bool BXml::setStartElement() const
{
    if (document()->hasChildNodes())
    {
        bool tFoundTag = false;
        for (int i = 0; i < document()->childNodes().size(); i++)
        {
            bDebug("TagName: " + document()->childNodes().at( i ).toElement().tagName());
            if (document()->childNodes().at( i ).toElement().tagName() == rootType())
            {
                setCurrentElement( document()->childNodes().at( i ).toElement() );
                tFoundTag = true;
                break;
            }
        }
        if (!tFoundTag)
        {
            bError() << "Couldnt find " << rootType() << " in file, creating...";
            createBaseElement( document(), rootType());
        }
    } else {
        createBaseElement( document(), rootType());
    }
    return true;
}

void BXml::createBaseElement( QDomDocument* aDocument, const QString& aBaseType) const
{
    setCurrentElement( aDocument->createElement(aBaseType) );
    aDocument->appendChild( currentElement() );
}

/**
 * changes to given group, if found<br/>
 * NOTE: only use setGroup to step one group, things like setGroup("Group1/Setp2");
 * doesnot work yet.
 * @param aGroup Groupname
 * @return -1 - if it fails<br/>
 *  0 - group was found and successfully changed to<br/>
 *  1 - group was not found but created and successfully changed to
 */
BXml::SetGroupResult BXml::setGroup( const QString& aGroup) const
{
    if (!document() || rootType().isEmpty())
        return BXml::Failed;

    if (aGroup == "..")
    {
        bWarning() << " setGroup(\"..\") is OBSOLETE use up() instead";
//         setCurrentElement( currentElement().parentNode().toElement() );
        up();
        return Successfully;
    }

    for (int i = 0; i < currentElement().childNodes().size(); i++)
    {
        if (currentElement().childNodes().at( i ).toElement().tagName() == aGroup)
        {
            setCurrentElement( currentElement().childNodes().at( i ).toElement() );
            return Successfully;
        }
    }

    QDomElement tElement = document()->createElement( aGroup );
    currentElement().appendChild( tElement );
    setCurrentElement( tElement );
    return SuccessfullyWithWarning;
}

/**
 * Sync mem with file on the disk, this is the actual "save" function. Without this
 * all changes will be lost.
 * @return true if everything worked fine<br/>
 * false if an error occurs
 */
bool BXml::sync()
{
    QFile tFile(fileName());
    if (!tFile.open(openMode()))
    {
        bError() << "error while syncing: " << tFile.errorString();
        bError() << "openMode: " << QString::number(openMode()) + " file: "+ fileName();
        return false;
    }

    if (tFile.write( document()->toByteArray() )  == -1 )
    {
        bError() << "error while syncing: " << tFile.errorString();
        tFile.close();
        return false;
    }
    tFile.flush(); /* just to be sure, even if it is not needed */
    tFile.close();
    return true;
}

/**
 * Writes an int <i>aValue</i> under <i>aName</i> in the current element.
 * @param aName Itemname
 * @param aValue Value to be written
 * @return true if everything worked out fine<br/>false if an error occured
 */
bool BXml::writeInt( const QString& aName, int aValue)
{
    if (!document() || rootType().isEmpty())
        return false;
    currentElement().setAttribute(aName,aValue);
    return true;
}

bool BXml::writeUInt( const QString& aName, unsigned int aValue)
{
    if (!document() || rootType().isEmpty())
        return false;
    currentElement().setAttribute(aName,aValue);
    return true;
}

bool BXml::writeLong( const QString& aName, qlonglong aValue)
{
    if (!document() || rootType().isEmpty())
        return false;
    currentElement().setAttribute(aName,aValue);
    return true;
}

bool BXml::writeULong( const QString& aName, qulonglong aValue)
{
    if (!document() || rootType().isEmpty())
        return false;
    currentElement().setAttribute(aName,aValue);
    return true;
}

bool BXml::writeFloat( const QString& aName, float aValue)
{
    if (!document() || rootType().isEmpty())
        return false;
    currentElement().setAttribute(aName,aValue);
    return true;
}

bool BXml::writeDouble( const QString& aName, double aValue)
{
    if (!document() || rootType().isEmpty())
        return false;
    currentElement().setAttribute(aName,aValue);
    return true;
}

bool BXml::writeString( const QString& aName, const QString& aValue )
{
    if (!document() || rootType().isEmpty()) return false;
    currentElement().setAttribute(aName,aValue);
    return true;
}

bool BXml::writeSize( const QString& aName, const QSize& aValue)
{
    if (!document() || rootType().isEmpty())
        return false;

    setGroup(aName);
        writeInt(HEIGHT, aValue.height());
        writeInt(WIDTH, aValue.width());
    up();
    return true;
}

bool BXml::writePoint(const QString& aName, const QPoint& aValue)
{
    if (!document() || rootType().isEmpty())
        return false;

    setGroup(aName);
        writeInt(X1, aValue.x());
        writeInt(Y1, aValue.y());
    up();
    return true;
}

bool BXml::writeRect( const QString& aName, const QRect& aValue)
{
    if (!document() || rootType().isEmpty())
        return false;

    setGroup(aName);
        writeInt(X1, aValue.x());
        writeInt(Y1, aValue.y());
        writeInt(X2, aValue.width());
        writeInt(Y2, aValue.height());
    up();
    return true;
}

/**
 * Reads an in from the Item <i>aName</i>, if it fails or there is no such Item
 * it will return the <i>aDefaultValue</i>.
 * @param aName Itemname
 * @param aDefaultValue defaultvalue that will be returned if there isnt such an Item
 * @return the value of aName or aDefaultValue
 */
int BXml::readInt( const QString& aName, int aDefaultValue) const
{
    QString tmp = currentElement().attribute(aName, QString::number(aDefaultValue));

    bool ok = false;
    int tNum = tmp.toInt(&ok);
    if (ok) return tNum;
    return aDefaultValue;
}

float BXml::readFloat( const QString& aName, float aDefaultValue) const
{
    QString tmp = currentElement().attribute(aName, QString::number(aDefaultValue));

    bool ok = false;
    float tNum = tmp.toFloat(&ok);
    if (ok) return tNum;
    return aDefaultValue;
}

double BXml::readDouble( const QString& aName, double aDefaultValue) const
{
    QString tmp = currentElement().attribute(aName, QString::number(aDefaultValue));

    bool ok = false;
    double tNum = tmp.toDouble(&ok);
    if (ok) return tNum;
    return aDefaultValue;
}

unsigned int BXml::readUInt( const QString& aName, unsigned int aDefaultValue) const
{
    QString tmp = currentElement().attribute(aName, QString::number(aDefaultValue));

    bool ok = false;
    unsigned int tNum = tmp.toUInt(&ok);
    if (ok) return tNum;
    return aDefaultValue;
}

qlonglong BXml::readLong( const QString& aName, qlonglong aDefaultValue) const
{
    QString tmp = currentElement().attribute(aName, QString::number(aDefaultValue));

    bool ok = false;
    qlonglong tNum = tmp.toLongLong(&ok);
    if (ok) return tNum;
    return aDefaultValue;
}

qulonglong BXml::readULong( const QString& aName, qulonglong aDefaultValue) const
{
    QString tmp = currentElement().attribute(aName, QString::number(aDefaultValue));

    bool ok = false;
    qulonglong tNum = tmp.toULongLong(&ok);
    if (ok) return tNum;
    return aDefaultValue;
}

QString BXml::readString( const QString& aName, const QString& aDefaultValue) const
{
    return currentElement().attribute(aName,aDefaultValue);
}

QSize BXml::readSize( const QString& aName, const QSize& aSize) const
{
    if (!probeGroup(aName)) return aSize;
    QSize tSize;
    setGroup(aName);
        tSize.setHeight( readInt( HEIGHT, -1));
        tSize.setWidth( readInt( WIDTH, -1));
    up();
    return tSize;
}

QPoint BXml::readPoint(const QString& aName, const QPoint& aPoint) const
{
    if (!probeGroup(aName)) return aPoint;
    QPoint tPoint;
    setGroup(aName);
        tPoint.setX( readInt( X1, -1));
        tPoint.setY( readInt( Y1, -1));
    up();
    return tPoint;
}

QRect BXml::readRect( const QString& aName, const QRect& aRect) const
{
    QRect tRect;
    if (!probeGroup(aName)) return aRect;
    setGroup(aName);
        tRect.setX( readInt(X1,-1) );
        tRect.setY( readInt(Y1,-1) );
        tRect.setWidth( readInt(X2,-1) );
        tRect.setHeight( readInt(Y2,-1) );
    up();
    return tRect;
}

QString BXml::fileName() const
{
    return d->myXmlFile;
}

QDomDocument* BXml::document() const
{
    return d->myDocument;
}

QDomElement BXml::currentElement() const
{
    return d->myCurrentElement;
}

void BXml::setCurrentElement( QDomElement aElement) const
{
    d->myCurrentElement = aElement;
}

QIODevice::OpenMode BXml::openMode() const
{
    return d->myOpenMode;
}

/**
 * Checks if the aGroupName exits in the current Group
 * @param aGroupName the name of the Group
 * @return true if the group exitst, false if not
 */
bool BXml::probeGroup( const QString& aGroupName) const
{
    if (!document() || rootType().isEmpty())
        return false;

    if (aGroupName == "..")
    {
        bWarning() << " setGroup(\"..\") is OBSOLETE use up() instead";
        return true;
    }

    for (int i = 0; i < currentElement().childNodes().size(); i++)
    {
        if (currentElement().childNodes().at( i ).toElement().tagName() == aGroupName)
        {
            return true;
        }
    }
    return false;
}

QString BXml::configType() const
{
    return d->myConfigType;
}

QString BXml::fileDir() const
{
    return d->myXmlDir;
}

/**
 * Moves up from the current to the parentgroup.
 * @return false if it has no current element otherwise true
 */
bool BXml::up() const
{
    if (rootType().isEmpty())
        return false;
    if ( currentElement() == document()->toElement() )
        return true;
    setCurrentElement( currentElement().parentNode().toElement() );
    return true;
}

/**
 * List all groups from your current Element
 * @return a List of avaiable groups
 */
QList< QString > BXml::listGroups() const
{
    QList<QString> tList;
    if (!document() || rootType().isEmpty())
        return tList;

    for (int i = 0; i < currentElement().childNodes().size(); ++i)
    {
        tList.append( currentElement().childNodes().at( i ).toElement().tagName() );
    }
    return tList;
}

/**
 * Lists all avaiable rootitems
 * @return a list of rootitems
 */
QList<QString> BXml::listRoots() const
{
    QList<QString> tList;
    if (!document())
        return tList;

    for (int i = 0; i < document()->childNodes().size(); ++i)
    {
        tList.append( document()->childNodes().at( i ).toElement().tagName() );
    }
    return tList;
}

void BXml::rootType( const QString& aType) const
{
    d->myRootType = aType;
}

QString BXml::rootType() const
{
    return d->myRootType;
}

bool BXml::toRoot() const
{
    if (!document() || rootType().isEmpty())
        return false;

    for (int i = 0; i < document()->childNodes().size(); ++i)
    {
        if (document()->childNodes().at( i ).toElement().tagName() == rootType())
        {
            setCurrentElement( document()->childNodes().at( i ).toElement() );
            return true;
        }
    }
    return false;
}

BXml::SetGroupResult BXml::switchRoot( const QString& aRoot) const
{
    if (!document())
        return BXml::Failed;

    rootType( aRoot );
    for (int i = 0; i < document()->childNodes().size(); ++i)
    {
        if (document()->childNodes().at( i ).toElement().tagName() == aRoot)
        {
            setCurrentElement( document()->childNodes().at( i ).toElement() );
            return Successfully;
        }
    }

    QDomElement tElement = document()->createElement( aRoot );
    document()->appendChild( tElement );
    setCurrentElement( tElement );
    return SuccessfullyWithWarning;
}

QList< QString > BXml::listElements() const
{
    QList<QString> tList;
    if (!document() || rootType().isEmpty())
        return tList;
    for (int i = 0; i < currentElement().attributes().size() ; ++i)
    {
        tList.append( currentElement().attributes().item( i ).toElement().tagName() );
    }
    return tList;
}

bool BXml::writeByteArray(const QString& aName, const QByteArray& aValue)
{
    return writeString(aName, aValue.toHex());
}

QByteArray BXml::readByteArray(const QString& aName, const QByteArray& aDefaultValue) const
{
    return QByteArray::fromHex(currentElement().attribute(aName, aDefaultValue).toLatin1());
}

bool BXml::hasType(const QString& aFile, const QString& aType)
{
    if ( aFile.isEmpty())
    {
        bError() << "No File given";
        return false;
    }

    if (!QFile::exists( aFile))
    {
        bError() << "file does not exists";
        return false;
    }

    QString tTypeName = aType;
    QDomDocument *tDom = new QDomDocument( tTypeName );
    QFile tFile( aFile );

    if (!tFile.open( QIODevice::ReadOnly ))
    {
        bWarning() << "could not create/open conf file";
        delete tDom;
        return false;
    }

    QString tError;
    if (!tDom->setContent(&tFile,&tError))
    {
        //this is not fatal
        bWarning() << "open error: " << tError;
//         delete tDom;
//         tFile.close();
//         return false;
    }
    tFile.close();

    if (tDom->hasChildNodes())
    {
        for (int i = 0; i < tDom->childNodes().size(); i++)
        {
            bDebug("CheckTagName: " + tDom->childNodes().at( i ).toElement().tagName());
            if (tDom->childNodes().at( i ).toElement().tagName() == tTypeName)
            {
                delete tDom;
                return true;
            }
        }
    } else {
        delete tDom;
        return false;
    }
    return false;
}
