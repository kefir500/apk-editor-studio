#ifndef ANDROIDEXPLORER_H
#define ANDROIDEXPLORER_H

#include "base/androidfilesystemmodel.h"
#include "base/clipboard.h"
#include "widgets/deselectablelistview.h"
#include <QDialog>
#include <QLineEdit>

class AndroidExplorer : public QDialog
{
    Q_OBJECT

public:
    AndroidExplorer(const QString &serial, QWidget *parent = nullptr);

protected:
    void changeEvent(QEvent *event);

private:
    void go(const QString &directory);
    void goUp();
    void download(const QString &path);
    void upload(const QString &path);
    void copy(const QString &src, const QString &dst);
    void move(const QString &src, const QString &dst);
    void remove(const QModelIndex &index);
    void screenshot(const QString &path);

    void setClipboard(const QModelIndex &index, bool move = false);
    void retranslate();

    QString serial;
    QString currentPath;
    AndroidFileSystemModel fileSystemModel{serial};
    ClipboardEntry<QString> clipboard;

    QAction *actionDownload;
    QAction *actionUpload;
    QAction *actionCopy;
    QAction *actionCut;
    QAction *actionPaste;
    QAction *actionRename;
    QAction *actionDelete;

    QLineEdit *pathInput;
    QToolButton *pathUpButton;
    QToolButton *pathGoButton;
    DeselectableListView *fileList;
};

#endif // ANDROIDEXPLORER_H
