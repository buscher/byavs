SOURCES += main.cpp \
 bxml.cpp
TEMPLATE = app

DESTDIR = ./

QT += xml

HEADERS += bdebug.h \
bxml.h \
 bglobal.h
OBJECTS_DIR = .lib

UI_DIR = .ui

MOC_DIR = .moc

QMAKE_CXXFLAGS_DEBUG += -Wstrict-aliasing

QMAKE_CXXFLAGS_RELEASE += -Wstrict-aliasing

QT += widgets

