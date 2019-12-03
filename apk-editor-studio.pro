TEMPLATE = app

QT += core gui widgets xml network
CONFIG += c++11

TARGET = apk-editor-studio
VERSION = 1.3.1
CI = $$(CI)

QMAKE_TARGET_PRODUCT = APK Editor Studio
QMAKE_TARGET_COMPANY = Alexander Gorishnyak
QMAKE_TARGET_DESCRIPTION = APK Editor Studio v$$VERSION
QMAKE_TARGET_COPYRIGHT = Copyright (C) 2019

DEFINES += APPLICATION='"\\\"$$QMAKE_TARGET_PRODUCT\\\""'
DEFINES += VERSION=\\\"$$VERSION\\\"
#DEFINES += PORTABLE

include($$PWD/src/apk-editor-studio.pri)
include($$PWD/lib/qtkeychain/qt5keychain.pri)
include($$PWD/lib/qtsingleapplication/src/qtsingleapplication.pri)
include($$PWD/deploy.pri)

TRANSLATIONS += \
    $$PWD/res/translations/apk-editor-studio.de.ts \
    $$PWD/res/translations/apk-editor-studio.es.ts \
    $$PWD/res/translations/apk-editor-studio.ja.ts \
    $$PWD/res/translations/apk-editor-studio.pt.ts \
    $$PWD/res/translations/apk-editor-studio.ru.ts

updateqm.input = TRANSLATIONS
updateqm.output = $$PWD/res/deploy/all/resources/translations/${QMAKE_FILE_IN_BASE}.qm
updateqm.commands = $$[QT_INSTALL_BINS]/lrelease ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
updateqm.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += updateqm
