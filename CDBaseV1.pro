QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    album.cpp \
    albumitemmodel.cpp \
    albumrepository.cpp \
    main.cpp \
    mainwindow.cpp \
    song.cpp \
    songitemmodel.cpp \
    songrepository.cpp \
    sqlite34/sqlite3.c

HEADERS += \
    album.h \
    albumitemmodel.h \
    albumrepository.h \
    mainwindow.h \
    song.h \
    songitemmodel.h \
    songrepository.h \
    sqlite34/sqlite3.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    CDBaseV1_cs_CZ.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
