#ifndef BASEFILESHEET_H
#define BASEFILESHEET_H

#include "sheets/baseeditablesheet.h"
#include "apk/resourcemodelindex.h"
#include <QFileSystemWatcher>

class BaseFileSheet : public BaseEditableSheet
{
    Q_OBJECT

public:
    BaseFileSheet(const ResourceModelIndex &index, QWidget *parent = nullptr);

    virtual bool load() = 0;
    virtual bool saveAs();
    bool replace();
    bool explore() const;

protected:
    void changeEvent(QEvent *event) override;

    ResourceModelIndex index;

private:
    void retranslate();

    QFileSystemWatcher watcher;

    QAction *actionReplace;
    QAction *actionSaveAs;
    QAction *actionExplore;
};

#endif // BASEFILESHEET_H
