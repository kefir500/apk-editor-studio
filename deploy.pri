# Deploy:

defineReplace(path) {
    return($$shell_quote($$shell_path($$1)))
}

defineReplace(copy) {
    return($$QMAKE_COPY_DIR $$path($$PWD/res/deploy/$$1) $$path($$DESTDIR/$$2) $$escape_expand(\\n\\t))
}

win32 {
    DESTDIR  = $$PWD/bin/win32
    QMAKE_POST_LINK += $$copy(all, .)
    QMAKE_POST_LINK += $$copy(win32, .)
    RC_ICONS = $$PWD/res/logo/application.ico
}

unix:!macx {
    DESTDIR = $$PWD/bin/linux/bin
    QMAKE_POST_LINK += $$QMAKE_MKDIR $$shell_quote($$DESTDIR/../share/$$TARGET) $$escape_expand(\\n\\t)
    QMAKE_POST_LINK += $$copy(all, ../share/$$TARGET)
    QMAKE_POST_LINK += $$copy(linux, ..)
}

macx {
    DESTDIR = $$PWD/bin/macos
    QMAKE_MAC_SDK = macosx10.7
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
    QMAKE_POST_LINK += $$copy(all, $$TARGET.app/Contents/MacOS)
    QMAKE_POST_LINK += $$copy(macos, $$TARGET.app/Contents)
}
