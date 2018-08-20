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
    QMAKE_POST_LINK += $$copy(all, ../share/$$TARGET)
    QMAKE_POST_LINK += $$copy(linux, ..)
}
