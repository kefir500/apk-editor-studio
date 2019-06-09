#ifndef FILEEDITOR_H
#define FILEEDITOR_H

#include "editors/editor.h"
#include "apk/resourcemodelindex.h"
#include <QFileSystemWatcher>

class FileEditor : public Editor
{
    Q_OBJECT

public:
    FileEditor(const ResourceModelIndex &index, QWidget *parent = nullptr);

    virtual bool load() = 0;
    virtual bool saveAs();
    bool replace();
    void remove() const;
    void explore() const;

    static QStringList supportedFormats();

protected:
    void changeEvent(QEvent *event) override;

    ResourceModelIndex index;

private:
    void retranslate();

    QFileSystemWatcher watcher;

    QAction *actionReplace;
    QAction *actionRemove;
    QAction *actionSaveAs;
    QAction *actionExplore;
};

#endif // FILEEDITOR_H
