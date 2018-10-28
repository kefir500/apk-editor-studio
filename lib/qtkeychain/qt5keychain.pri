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
    # Remove the following LIBSECRET_SUPPORT line
    # to build without libsecret support.
    DEFINES += LIBSECRET_SUPPORT
    contains(DEFINES, LIBSECRET_SUPPORT) {
        packagesExist(libsecret-1) {
            !build_pass:message("Libsecret support: on")
            CONFIG += link_pkgconfig
            PKGCONFIG += libsecret-1
            DEFINES += HAVE_LIBSECRET
        } else {
            !build_pass:warning("Libsecret not found.")
            !build_pass:message("Libsecret support: off")
        }
    } else {
        !build_pass:message("Libsecret support: off")
    }

    # Generate D-Bus interface:
    QT += dbus
    kwallet_interface.files = $$PWD/org.kde.KWallet.xml
    DBUS_INTERFACES += kwallet_interface

    HEADERS += \
        $$QT5KEYCHAIN_PWD/gnomekeyring_p.h \
        $$QT5KEYCHAIN_PWD/plaintextstore_p.h \
        $$QT5KEYCHAIN_PWD/libsecret_p.h
    SOURCES += \
        $$QT5KEYCHAIN_PWD/keychain_unix.cpp \
        $$QT5KEYCHAIN_PWD/plaintextstore.cpp \
        $$QT5KEYCHAIN_PWD/gnomekeyring.cpp \
        $$QT5KEYCHAIN_PWD/libsecret.cpp
}

win32 {
    # Remove the following USE_CREDENTIAL_STORE line
    # to use the CryptProtectData Windows API function
    # instead of the Windows Credential Store.
    DEFINES += USE_CREDENTIAL_STORE
    contains(DEFINES, USE_CREDENTIAL_STORE) {
        !build_pass:message("Windows Credential Store support: on")
        LIBS += -lAdvapi32
    } else {
        !build_pass:message("Windows Credential Store support: off")
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
