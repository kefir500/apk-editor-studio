#include "windows/androidexplorer.h"
#include "windows/dialogs.h"
#include "widgets/loadingwidget.h"
#include "widgets/toolbar.h"
#include "tools/adb.h"
#include "base/application.h"
#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QShortcut>
#include <QToolButton>

#ifdef QT_DEBUG
    #include <QDebug>
#endif

AndroidExplorer::AndroidExplorer(const QString &serial, QWidget *parent)
    : QDialog(parent)
    , serial(serial)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    resize(app->scale(500, 400));

    actionDownload = new QAction(app->icons.get("download.png"), {}, this);
    actionDownload->setShortcut(QKeySequence::Save);
    connect(actionDownload, &QAction::triggered, [=]() {
        const auto index = fileList->currentIndex();
        if (index.isValid()) {
            const auto path = fileSystemModel.getItemPath(index);
            download(path);
        }
    });

    actionUpload = new QAction(app->icons.get("upload.png"), {}, this);
    actionUpload->setShortcut(QKeySequence("Ctrl+U"));
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

    actionCopy = new QAction(app->icons.get("copy.png"), {}, this);
    actionCopy->setShortcut(QKeySequence::Copy);
    connect(actionCopy, &QAction::triggered, [=]() {
        const auto index = fileList->currentIndex();
        setClipboard(index, false);
    });

    actionCut = new QAction(app->icons.get("cut.png"), {}, this);
    actionCut->setShortcut(QKeySequence::Cut);
    connect(actionCut, &QAction::triggered, [=]() {
        const auto index = fileList->currentIndex();
        setClipboard(index, true);
    });

    actionPaste = new QAction(app->icons.get("paste.png"), {}, this);
    actionPaste->setEnabled(false);
    actionPaste->setShortcut(QKeySequence::Paste);
    connect(actionPaste, &QAction::triggered, [=]() {
        const QString src = clipboard.data;
        QString dst = fileSystemModel.getCurrentPath();
        const auto index = fileList->currentIndex();
        if (index.isValid()) {
            if (fileSystemModel.getItemType(index) == AndroidFileSystemItem::AndroidFSDirectory) {
                dst = fileSystemModel.getItemPath(index);
            }
        }
        if (clipboard.move) {
            move(src, dst);
            setClipboard({});
        } else {
            copy(src, dst);
        }
    });

    actionRename = new QAction(app->icons.get("rename.png"), {}, this);
    actionRename->setShortcut(QKeySequence("F2"));
    connect(actionRename, &QAction::triggered, [=]() {
        const auto index = fileList->currentIndex();
        if (index.isValid()) {
            fileList->edit(index);
        }
    });

    actionDelete = new QAction(app->icons.get("x-red.png"), {}, this);
    actionDelete->setShortcut(QKeySequence::Delete);
    connect(actionDelete, &QAction::triggered, [=]() {
        remove(fileList->currentIndex());
    });

    auto actionInstall = app->actions.getInstallApk(serial, this);
    auto actionScreenshot = app->actions.getTakeScreenshot(serial, this);

    auto toolbar = new Toolbar(this);
    toolbar->addActionToPool("download", actionDownload);
    toolbar->addActionToPool("upload", actionUpload);
    toolbar->addActionToPool("copy", actionCopy);
    toolbar->addActionToPool("cut", actionCut);
    toolbar->addActionToPool("paste", actionPaste);
    toolbar->addActionToPool("rename", actionRename);
    toolbar->addActionToPool("delete", actionDelete);
    toolbar->addActionToPool("install", actionInstall);
    toolbar->addActionToPool("screenshot", actionScreenshot);
    toolbar->initialize(app->settings->getAndroidExplorerToolbar());
    connect(toolbar, &Toolbar::updated, app->settings, &Settings::setAndroidExplorerToolbar);

    auto fileSelectionActions = new QActionGroup(this);
    fileSelectionActions->setEnabled(false);
    fileSelectionActions->setExclusive(false);
    fileSelectionActions->addAction(actionDownload);
    fileSelectionActions->addAction(actionCopy);
    fileSelectionActions->addAction(actionCut);
    fileSelectionActions->addAction(actionRename);
    fileSelectionActions->addAction(actionDelete);

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
        const auto type = fileSystemModel.getItemType(index);
        const auto path = fileSystemModel.getItemPath(index);
        switch (type) {
        case AndroidFileSystemItem::AndroidFSDirectory:
            go(path);
            break;
        case AndroidFileSystemItem::AndroidFSFile:
            download(path);
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

void AndroidExplorer::download(const QString &path)
{
    const auto dst = Dialogs::getSaveFilename(path, this);
    if (dst.isEmpty()) {
        return;
    }

    fileSystemModel.download(path, dst);
}

void AndroidExplorer::upload(const QString &path)
{
    const auto src = Dialogs::getOpenFilename({}, this);
    if (src.isEmpty()) {
        return;
    }

    fileSystemModel.upload(src, path);
}

void AndroidExplorer::copy(const QString &src, const QString &dst)
{
    fileSystemModel.copy(src, dst);
}

void AndroidExplorer::move(const QString &src, const QString &dst)
{
    fileSystemModel.move(src, dst);
}

void AndroidExplorer::remove(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }

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
        const auto path = fileSystemModel.getItemPath(index);
        fileSystemModel.remove(path);
    }
}

void AndroidExplorer::setClipboard(const QModelIndex &index, bool move)
{
    const bool isValid = index.isValid();
    clipboard.data = isValid ? fileSystemModel.getItemPath(index) : QString();
    clipboard.move = move;
    actionPaste->setEnabled(isValid);
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
