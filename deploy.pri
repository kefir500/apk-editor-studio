defineReplace(copy) {
    return($$QMAKE_COPY_DIR \
           $$shell_quote($$shell_path($$PWD/res/deploy/$$1)) \
           $$shell_quote($$shell_path($$DESTDIR/$$2)) \
           $$escape_expand(\\n\\t))
}

defineReplace(makeExecutable) {
    return(chmod +x $$shell_quote($$1) $$escape_expand(\\n\\t))
}

defineReplace(mkdir) {
    return($$QMAKE_MKDIR $$shell_quote($$1) $$escape_expand(\\n\\t))
}

win32 {
    RC_ICONS += $$PWD/src/icons/other/application.ico
    RC_ICONS += $$PWD/src/icons/other/verb-install.ico
    RC_ICONS += $$PWD/src/icons/other/verb-optimize.ico
    RC_ICONS += $$PWD/src/icons/other/verb-sign.ico
    QMAKE_POST_LINK += $$copy(all, .)
    QMAKE_POST_LINK += $$copy(windows, .)
}

unix:!macx {
    QMAKE_POST_LINK += $$mkdir($$DESTDIR/../share/$$TARGET)
    QMAKE_POST_LINK += $$copy(all/., ../share/$$TARGET)
    QMAKE_POST_LINK += $$copy(linux/share, ..)
    isEmpty(PACKAGE) {
        QMAKE_POST_LINK += $$copy(linux/bin, ..)
        QMAKE_POST_LINK += $$makeExecutable($$DESTDIR/adb)
        QMAKE_POST_LINK += $$makeExecutable($$DESTDIR/zipalign)
    }
    # Install:
    isEmpty(PREFIX): PREFIX = /usr
    target.path  = $$PREFIX/bin
    share1.files = $$PWD/res/deploy/all/*
    share1.path  = $$PREFIX/share/apk-editor-studio
    share2.files = $$PWD/res/deploy/linux/share/*
    share2.path  = $$PREFIX/share
    INSTALLS += target
    INSTALLS += share1
    INSTALLS += share2
    isEmpty(PACKAGE) {
        bin.files = $$PWD/res/deploy/linux/bin/*
        bin.path  = $$PREFIX/bin
        INSTALLS += bin
    }
}

macx {
    QMAKE_POST_LINK += $$copy(all/., $${TARGET}.app/Contents/MacOS)
    QMAKE_POST_LINK += $$copy(macos/bundle/., $${TARGET}.app)
    QMAKE_POST_LINK += $$makeExecutable($$DESTDIR/$${TARGET}.app/Contents/MacOS/adb)
    QMAKE_POST_LINK += $$makeExecutable($$DESTDIR/$${TARGET}.app/Contents/MacOS/zipalign)
    QMAKE_INFO_PLIST = $$PWD/res/deploy/macos/Info.plist
}
