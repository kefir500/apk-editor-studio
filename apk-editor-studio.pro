TEMPLATE = app

QT += core gui widgets xml network
CONFIG += c++11

TARGET = apk-editor-studio
VERSION = 1.4.0

QMAKE_TARGET_PRODUCT = APK Editor Studio
QMAKE_TARGET_COMPANY = Alexander Gorishnyak
QMAKE_TARGET_DESCRIPTION = APK Editor Studio v$$VERSION
QMAKE_TARGET_COPYRIGHT = Copyright (C) 2019-2020

DEFINES += APPLICATION='"\\\"$$QMAKE_TARGET_PRODUCT\\\""'
DEFINES += VERSION=\\\"$$VERSION\\\"
#DEFINES += PORTABLE

win32 {
    isEmpty(DESTDIR): DESTDIR = $$PWD/bin/win32
}

unix:!macx {
    isEmpty(DESTDIR): DESTDIR = $$PWD/bin/linux
    DESTDIR = $$DESTDIR/bin # Follow Linux FHS
}

macx {
    isEmpty(DESTDIR): DESTDIR = $$PWD/bin/macos
}

include($$PWD/src/apk-editor-studio.pri)
include($$PWD/src/translations/translations.pri)
include($$PWD/lib/qtkeychain/qt5keychain.pri)
include($$PWD/lib/qtsingleapplication/src/qtsingleapplication.pri)
include($$PWD/deploy.pri)
