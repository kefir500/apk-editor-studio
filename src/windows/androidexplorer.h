#ifndef ANDROIDEXPLORER_H
#define ANDROIDEXPLORER_H

#include "base/clipboard.h"
#include <QMainWindow>

class AndroidFileSystemModel;
class DeselectableListView;
class LogModel;
class QLineEdit;
class QToolButton;
class Toolbar;

class AndroidExplorer : public QMainWindow
{
    Q_OBJECT

public:
    AndroidExplorer(const QString &serial, QWidget *parent = nullptr);

protected:
    void changeEvent(QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    void go(const QString &directory);
    void goUp();
    void download(const QString &path);
    void upload(const QString &path);
    void copy(const QString &src, const QString &dst);
    void move(const QString &src, const QString &dst);
    void remove(const QModelIndex &index);
    void install();

    void setClipboard(const QModelIndex &index, bool move = false);
    void retranslate();

    QString serial;
    QString currentPath;
    AndroidFileSystemModel *fileSystemModel;
    LogModel *logModel;
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

    QMenu *menuFile;
    QMenu *menuEdit;
    QMenu *menuTools;
    QMenu *menuSettings;
    QMenu *menuWindow;
    Toolbar *toolbar;
    QDockWidget *logDock;
};

#endif // ANDROIDEXPLORER_H
