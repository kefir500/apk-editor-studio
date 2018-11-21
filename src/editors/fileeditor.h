#ifndef FILEEDITOR_H
#define FILEEDITOR_H

#include "editors/saveableeditor.h"
#include "apk/resourcemodelindex.h"
#include <QFileSystemWatcher>

class FileEditor : public SaveableEditor
{
    Q_OBJECT

public:
    FileEditor(const ResourceModelIndex &index, QWidget *parent = nullptr);

    virtual bool load() = 0;
    bool saveAs();
    bool replace();
    void explore() const;

    static QStringList supportedFormats();

protected:
    void changeEvent(QEvent *event) Q_DECL_OVERRIDE;

    ResourceModelIndex index;

private:
    void retranslate();

    QFileSystemWatcher watcher;

    QAction *actionReplace;
    QAction *actionSaveAs;
    QAction *actionExplore;
};

#endif // FILEEDITOR_H
