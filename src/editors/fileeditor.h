#ifndef FILEEDITOR_H
#define FILEEDITOR_H

#include "editors/baseeditor.h"
#include "apk/resourcemodelindex.h"
#include <QFileSystemWatcher>

class FileEditor : public BaseEditor
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
    ResourceModelIndex index;

private:
    QFileSystemWatcher watcher;
};

#endif // FILEEDITOR_H
