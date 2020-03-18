#include "windows/androidexplorer.h"
#include "windows/dialogs.h"
#include "widgets/loadingwidget.h"
#include "tools/adb.h"
#include "base/application.h"
#include <QBoxLayout>
#include <QToolButton>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QInputDialog>
#include <QMenu>
#include <QShortcut>

#ifdef QT_DEBUG
    #include <QDebug>
#endif

AndroidExplorer::AndroidExplorer(const QString &serial, QWidget *parent)
    : QDialog(parent)
    , serial(serial)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    resize(app->scale(440, 400));

    auto toolbar = new QToolBar(this);

    actionDownload = new QAction(app->icons.get("download.png"), {}, this);
    actionDownload->setShortcut(QKeySequence::Save);
    toolbar->addAction(actionDownload);
    connect(actionDownload, &QAction::triggered, [=]() {
        const auto index = fileList->currentIndex();
        if (index.isValid()) {
            download(index);
        }
    });

    actionUpload = new QAction(app->icons.get("upload.png"), {}, this);
    actionUpload->setShortcut(QKeySequence("Ctrl+U"));
    toolbar->addAction(actionUpload);
    connect(actionUpload, &QAction::triggered, [=]() {
        QString path = fileSystemModel.getCurrentPath();
        const auto index = fileList->currentIndex();
        if (index.isValid()) {
            if (fileSystemModel.getItemType(index) == AndroidFileSystemItem::AndroidFSDirectory) {
                path = fileSystemModel.getItemPath(index);
            }
        }
        upload(path);
    });

    toolbar->addSeparator();

    actionCopy = new QAction(app->icons.get("copy.png"), {}, this);
    actionCopy->setShortcut(QKeySequence::Copy);
    toolbar->addAction(actionCopy);
    connect(actionCopy, &QAction::triggered, [=]() {
        const auto index = fileList->currentIndex();
        setClipboard(index, false);
    });

    actionCut = new QAction(app->icons.get("cut.png"), {}, this);
    actionCut->setShortcut(QKeySequence::Cut);
    toolbar->addAction(actionCut);
    connect(actionCut, &QAction::triggered, [=]() {
        const auto index = fileList->currentIndex();
        setClipboard(index, true);
    });

    actionPaste = new QAction(app->icons.get("paste.png"), {}, this);
    actionPaste->setEnabled(false);
    actionPaste->setShortcut(QKeySequence::Paste);
    toolbar->addAction(actionPaste);
    connect(actionPaste, &QAction::triggered, [=]() {
        if (!clipboard.data.isValid()) {
            return;
        }
        QString dst = fileSystemModel.getCurrentPath();
        const auto currentIndex = fileList->currentIndex();
        if (currentIndex.isValid()) {
            if (fileSystemModel.getItemType(currentIndex) == AndroidFileSystemItem::AndroidFSDirectory) {
                dst = fileSystemModel.getItemPath(currentIndex);
            }
        }
        if (clipboard.move) {
            move(clipboard.data, dst);
            setClipboard({});
        } else {
            copy(clipboard.data, dst);
        }
    });

    actionRename = new QAction(app->icons.get("rename.png"), {}, this);
    actionRename->setShortcut(QKeySequence("F2"));
    toolbar->addAction(actionRename);
    connect(actionRename, &QAction::triggered, [=]() {
        const auto index = fileList->currentIndex();
        if (index.isValid()) {
            fileList->edit(index);
        }
    });

    actionDelete = new QAction(app->icons.get("x-red.png"), {}, this);
    actionDelete->setShortcut(QKeySequence::Delete);
    toolbar->addAction(actionDelete);
    connect(actionDelete, &QAction::triggered, [=]() {
        const auto index = fileList->currentIndex();
        if (index.isValid()) {
            remove(index);
        }
    });

    auto fileSelectionActions = new QActionGroup(this);
    fileSelectionActions->setEnabled(false);
    fileSelectionActions->setExclusive(false);
    fileSelectionActions->addAction(actionDownload);
    fileSelectionActions->addAction(actionCopy);
    fileSelectionActions->addAction(actionCut);
    fileSelectionActions->addAction(actionRename);
    fileSelectionActions->addAction(actionDelete);

    toolbar->addSeparator();
    toolbar->addAction(app->actions.getInstallExternalApk(serial, this));
    toolbar->addSeparator();
    toolbar->addAction(app->actions.getTakeScreenshot(serial, this));

    pathUpButton = new QToolButton(this);
    pathUpButton->setToolTip(pathUpButton->text());
    pathUpButton->setIcon(app->icons.get("explorer-up.png"));
    connect(pathUpButton, &QToolButton::clicked, this, &AndroidExplorer::goUp);

    auto pathUpShortcut = new QShortcut(this);
    pathUpShortcut->setKey(QKeySequence::Back);
    connect(pathUpShortcut, &QShortcut::activated, this, &AndroidExplorer::goUp);

    pathGoButton = new QToolButton(this);
    pathGoButton->setToolTip(pathGoButton->text());
    pathGoButton->setIcon(app->icons.get("explorer-go.png"));
    connect(pathGoButton, &QToolButton::clicked, [this]() {
        go(pathInput->text());
    });

    pathInput = new QLineEdit("/", this);
    connect(pathInput, &QLineEdit::returnPressed, pathGoButton, &QToolButton::click);

    auto pathBar = new QHBoxLayout;
    pathBar->setSpacing(2);
    pathBar->addWidget(pathUpButton);
    pathBar->addWidget(pathInput);
    pathBar->addWidget(pathGoButton);

    fileList = new DeselectableListView(this);
    fileList->setModel(&fileSystemModel);
    fileList->setContextMenuPolicy(Qt::CustomContextMenu);
    fileList->setEditTriggers(QListView::SelectedClicked | QListView::EditKeyPressed);
    connect(fileList, &QListView::activated, [=](const QModelIndex &index) {
        switch (fileSystemModel.getItemType(index)) {
        case AndroidFileSystemItem::AndroidFSDirectory:
            go(fileSystemModel.getItemPath(index));
            break;
        case AndroidFileSystemItem::AndroidFSFile:
            download(index);
            break;
        }
    });
    connect(fileList, &QListView::customContextMenuRequested, [=](const QPoint &point) {
        QMenu context(this);
        context.addSeparator();
        context.addAction(actionDownload);
        context.addAction(actionUpload);
        context.addSeparator();
        context.addAction(actionCopy);
        context.addAction(actionCut);
        context.addAction(actionPaste);
        context.addAction(actionRename);
        context.addAction(actionDelete);
        context.exec(fileList->viewport()->mapToGlobal(point));
    });
    connect(fileList->selectionModel(), &QItemSelectionModel::currentChanged, [=](const QModelIndex &index) {
        fileSelectionActions->setEnabled(index.isValid());
    });

    auto loading = new LoadingWidget(fileList);

    connect(&fileSystemModel, &AndroidFileSystemModel::pathChanged, [=](const QString &path) {
        pathInput->setText(path);
        fileSelectionActions->setEnabled(false);
    });
    connect(&fileSystemModel, &AndroidFileSystemModel::modelAboutToBeReset, [=]() {
        loading->show();
        fileSelectionActions->setEnabled(false);
    });
    connect(&fileSystemModel, &AndroidFileSystemModel::modelReset, [=]() {
        loading->hide();
        fileList->scrollToTop();
    });
    connect(&fileSystemModel, &AndroidFileSystemModel::error, [=](const QString &error) {
        QMessageBox::warning(this, QString(), error);
    });

    auto layout = new QVBoxLayout(this);
    layout->addWidget(toolbar);
    layout->addLayout(pathBar);
    layout->addWidget(fileList);

    auto buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
    buttons->button(QDialogButtonBox::Close)->setAutoDefault(false);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);

    retranslate();
}

void AndroidExplorer::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        retranslate();
    }
    QDialog::changeEvent(event);
}

void AndroidExplorer::go(const QString &directory)
{
    fileSystemModel.cd(directory);
}

void AndroidExplorer::goUp()
{
    go("..");
}

void AndroidExplorer::download(const QModelIndex &index)
{
    const auto dst = Dialogs::getSaveFilename(fileSystemModel.getItemPath(index), this);
    if (dst.isEmpty()) {
        return;
    }

    fileSystemModel.download(index, dst);
}

void AndroidExplorer::upload(const QString &path)
{
    const auto src = Dialogs::getOpenFilename({}, this);
    if (src.isEmpty()) {
        return;
    }

    fileSystemModel.upload(src, path);
}

void AndroidExplorer::copy(const QModelIndex &src, const QString &dst)
{
    fileSystemModel.copy(src, dst);
}

void AndroidExplorer::move(const QModelIndex &src, const QString &dst)
{
    fileSystemModel.move(src, dst);
}

void AndroidExplorer::remove(const QModelIndex &index)
{
    QString question;
    switch (fileSystemModel.getItemType(index)) {
    case AndroidFileSystemItem::AndroidFSFile:
        question = tr("Are you sure you want to delete this file?");
        break;
    case AndroidFileSystemItem::AndroidFSDirectory:
        question = tr("Are you sure you want to delete this directory?");
        break;
    }
    if (QMessageBox::question(this, {}, question) == QMessageBox::Yes) {
        fileSystemModel.remove(index);
    }
}

void AndroidExplorer::setClipboard(const QModelIndex &index, bool move)
{
    clipboard = ClipboardEntry<QPersistentModelIndex>(index, move);
    actionPaste->setEnabled(index.isValid());
}

void AndroidExplorer::retranslate()
{
    setWindowTitle(tr("Android Explorer"));
    actionDownload->setText(tr("Download"));
    actionUpload->setText(tr("Upload"));
    actionCopy->setText(tr("Copy"));
    actionCut->setText(tr("Cut"));
    actionPaste->setText(tr("Paste"));
    actionRename->setText(tr("Rename"));
    actionDelete->setText(tr("Delete"));
    //: Navigate up one directory in a file manager hierarchy.
    pathUpButton->setText(tr("Up"));
    pathUpButton->setToolTip(tr("Up"));
    //: Navigate to a directory in a file manager.
    pathGoButton->setText(tr("Go"));
    pathGoButton->setToolTip(tr("Go"));
}
