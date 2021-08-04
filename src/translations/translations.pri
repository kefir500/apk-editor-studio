TRANSLATIONS = $$PWD/apk-editor-studio.en.ts
EXTRA_TRANSLATIONS += \
    $$PWD/apk-editor-studio.az.ts \
    $$PWD/apk-editor-studio.ckb.ts \
    $$PWD/apk-editor-studio.cs.ts \
    $$PWD/apk-editor-studio.de.ts \
    $$PWD/apk-editor-studio.el.ts \
    $$PWD/apk-editor-studio.en_GB.ts \
    $$PWD/apk-editor-studio.es.ts \
    $$PWD/apk-editor-studio.fa.ts \
    $$PWD/apk-editor-studio.fr.ts \
    $$PWD/apk-editor-studio.he.ts \
    $$PWD/apk-editor-studio.hu.ts \
    $$PWD/apk-editor-studio.id.ts \
    $$PWD/apk-editor-studio.it.ts \
    $$PWD/apk-editor-studio.ja.ts \
    $$PWD/apk-editor-studio.ko.ts \
    $$PWD/apk-editor-studio.pl.ts \
    $$PWD/apk-editor-studio.pt.ts \
    $$PWD/apk-editor-studio.ro.ts \
    $$PWD/apk-editor-studio.ru.ts \
    $$PWD/apk-editor-studio.tr.ts \
    $$PWD/apk-editor-studio.zh_CN.ts

lrelease.input = EXTRA_TRANSLATIONS
lrelease.output = $$PWD/../../res/deploy/all/resources/translations/${QMAKE_FILE_IN_BASE}.qm
lrelease.commands = $$shell_path($$[QT_INSTALL_BINS]/lrelease) ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
lrelease.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += lrelease
