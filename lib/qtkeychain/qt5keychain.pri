# Minimal qmake support.
# This file is provided as is without any warranty.
# It can break at anytime or be removed without notice.

QT5KEYCHAIN_PWD = $$PWD

CONFIG += depend_includepath
DEFINES += QTKEYCHAIN_NO_EXPORT

INCLUDEPATH += \
    $$PWD/.. \
    $$QT5KEYCHAIN_PWD

HEADERS += \
    $$QT5KEYCHAIN_PWD/keychain_p.h \
    $$QT5KEYCHAIN_PWD/keychain.h

SOURCES += \
    $$QT5KEYCHAIN_PWD/keychain.cpp

unix:!macx:!ios {
    QT += dbus
    packagesExist(libsecret-1) {
        message(Building with libsecret support)
        CONFIG += link_pkgconfig
        PKGCONFIG += libsecret-1
        DEFINES += HAVE_LIBSECRET
    } else {
        message(Building without libsecret support)
    }
    HEADERS += \
        $$QT5KEYCHAIN_PWD/gnomekeyring_p.h \
        $$QT5KEYCHAIN_PWD/plaintextstore_p.h \
        $$QT5KEYCHAIN_PWD/libsecret_p.h \
        $$QT5KEYCHAIN_PWD/kwallet_interface.h
    SOURCES += \
        $$QT5KEYCHAIN_PWD/keychain_unix.cpp \
        $$QT5KEYCHAIN_PWD/plaintextstore.cpp \
        $$QT5KEYCHAIN_PWD/gnomekeyring.cpp \
        $$QT5KEYCHAIN_PWD/libsecret.cpp \
        $$QT5KEYCHAIN_PWD/kwallet_interface.cpp
}

win32 {
    # Remove the following USE_CREDENTIAL_STORE line
    # to use the CryptProtectData Windows API function
    # instead of the Windows Credential Store.
    DEFINES += USE_CREDENTIAL_STORE
    contains(DEFINES, USE_CREDENTIAL_STORE) {
        message(Building with Windows Credential Store support)
        LIBS += -lAdvapi32
    } else {
        message(Building without Windows Credential Store support)
        LIBS += -lCrypt32
        HEADERS += $$QT5KEYCHAIN_PWD/plaintextstore_p.h
        SOURCES += $$QT5KEYCHAIN_PWD/plaintextstore.cpp
    }
    HEADERS += $$QT5KEYCHAIN_PWD/libsecret_p.h
    SOURCES += \
        $$QT5KEYCHAIN_PWD/keychain_win.cpp \
        $$QT5KEYCHAIN_PWD/libsecret.cpp
}

macx:!ios {
    LIBS += "-framework Security" "-framework Foundation"
    SOURCES += $$QT5KEYCHAIN_PWD/keychain_mac.cpp
}

ios {
    LIBS += "-framework Security" "-framework Foundation"
    OBJECTIVE_SOURCES += $$QT5KEYCHAIN_PWD/keychain_ios.mm
}
