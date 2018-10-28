# Deploy:

defineReplace(path) {
    return($$shell_quote($$shell_path($$1)))
}

defineReplace(mkdir) {
    return($$QMAKE_MKDIR $$path($$1) $$escape_expand(\\n\\t))
}

defineReplace(copy) {
    return($$QMAKE_COPY_DIR $$path($$PWD/res/deploy/$$1) $$path($$DESTDIR/$$2) $$escape_expand(\\n\\t))
}

win32 {
    DESTDIR  = $$PWD/bin/win32
    RC_ICONS = $$PWD/res/logo/application.ico
    QMAKE_POST_LINK += $$copy(all, .)
    QMAKE_POST_LINK += $$copy(win32, .)
}

unix:!macx {
    DESTDIR = $$PWD/bin/linux/bin
    QMAKE_POST_LINK += $$mkdir($$DESTDIR/../share/$$TARGET)
    QMAKE_POST_LINK += $$copy(all/., ../share/$$TARGET)
    QMAKE_POST_LINK += $$copy(linux/share, ..)
    isEmpty(PACKAGE): QMAKE_POST_LINK += $$copy(linux/bin, ..)
    # Install:
    isEmpty(PREFIX): PREFIX = /usr
    target.path   = $$PREFIX/bin
    share1.files  = $$PWD/res/deploy/all/*
    share1.path   = $$PREFIX/share/apk-editor-studio
    share2.files  = $$PWD/res/deploy/linux/share/*
    share2.path   = $$PREFIX/share
    INSTALLS     += target share1 share2
    isEmpty(PACKAGE) {
        bin.files  = $$PWD/res/deploy/linux/bin/*
        bin.path   = $$PREFIX/bin
        INSTALLS  += bin
    }
}

macx {
    DESTDIR = $$PWD/bin/macos
    QMAKE_MAC_SDK = macosx10.7
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
    QMAKE_POST_LINK += $$copy(all/., $${TARGET}.app/Contents/MacOS)
    QMAKE_POST_LINK += $$copy(macos/bundle/., $${TARGET}.app)
    QMAKE_INFO_PLIST = $$PWD/res/deploy/macos/Info.plist
}
