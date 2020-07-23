CONFIG += lrelease

EXTRA_TRANSLATIONS += \
    $$PWD/apk-editor-studio.de.ts \
    $$PWD/apk-editor-studio.el.ts \
    $$PWD/apk-editor-studio.es.ts \
    $$PWD/apk-editor-studio.ja.ts \
    $$PWD/apk-editor-studio.ko.ts \
    $$PWD/apk-editor-studio.pl.ts \
    $$PWD/apk-editor-studio.pt.ts \
    $$PWD/apk-editor-studio.ro.ts \
    $$PWD/apk-editor-studio.ru.ts \
    $$PWD/apk-editor-studio.tr.ts

updateqm.input = EXTRA_TRANSLATIONS
updateqm.output = $$PWD/../../res/deploy/all/resources/translations/${QMAKE_FILE_IN_BASE}.qm
updateqm.commands = $$[QT_INSTALL_BINS]/lrelease ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
updateqm.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += updateqm
