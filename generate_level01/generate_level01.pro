SOURCES += main.cpp \
 ../src/bxml.cpp
TEMPLATE = app

DESTDIR = ./

QT += xml

HEADERS += ../src/bdebug.h \
../src/bxml.h \
 ../src/bglobal.h

 INCLUDEPATH += ../src/

OBJECTS_DIR = .lib

UI_DIR = .ui

MOC_DIR = .moc

QMAKE_CXXFLAGS_DEBUG += -Wstrict-aliasing

QMAKE_CXXFLAGS_RELEASE += -Wstrict-aliasing

QT += widgets

