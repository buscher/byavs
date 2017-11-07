SOURCES += main.cpp \
 benviroment.cpp \
 bimagecache.cpp \
 bxml.cpp \
 bobject.cpp \
 bobjectlist.cpp \
 brender.cpp \
 bwidget.cpp \
 bworldmanager.cpp \
 bship.cpp \
 bexplosion.cpp \
 bshot.cpp \
 bhitbox.cpp \
 benemyship.cpp \
 bbackburner.cpp \
 bupgrade.cpp
TEMPLATE = app
CONFIG += qt \
 opengl \
 release
TARGET = byavs
win32 {
    DESTDIR = ..
}else {
    DESTDIR = ../bin
}
QT += opengl \
xml
HEADERS += bdebug.h \
benviroment.h \
bimagecache.h \
bxml.h \
 bobject.h \
 bobjectlist.h \
 brender.h \
 bwidget.h \
 bworldmanager.h \
 bship.h \
 bexplosion.h \
 bbackburner.h \
 bshot.h \
 bhitbox.h \
 benemyship.h \
 bglobal.h \
 bpointf.h \
 bupgrade.h
OBJECTS_DIR = .lib

UI_DIR = .ui

MOC_DIR = .moc

FORMS += bwidget.ui

unix {
    DATA_PATH =$$(DATA_PATH)
    if(count(DATA_PATH, 1)){
        DATA_PATH = $(DATA_PATH)
        DEFINES += DATA_PATH=$$(DATA_PATH)
    }    else {
        warning("DATA_PATH is empty")
        DATA_PATH = \\\"$$system(pwd)/..\\\"
        DATA_PATH = $$replace(DATA_PATH,/,\/)
        #        DATA_PATH = $$replace(DATA_PATH,' ',' ') ##isnt that wrong? in c++ '\ '
        warning("using:" $$DATA_PATH )
        DEFINES += DATA_PATH=$$DATA_PATH
    }
    #    DEFINES += DOC_PATH=$(DOC_PATH)
    #    DEFINES += TRANSLATION_PATH=$(TRANSLATION_PATH)
    #    DEFINES += CONF_PATH=$(CONF_PATH)
}

QMAKE_CXXFLAGS_RELEASE += -O2
CONFIG -= thread \
exceptions \
 debug
DESTDIR = ../bin

QT += widgets
