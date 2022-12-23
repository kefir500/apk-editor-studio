#include "windows/frameworkmanager.h"
#include "windows/dialogs.h"
#include "base/utils.h"
#include "tools/apktool.h"
#include <QBoxLayout>
#include <QDragEnterEvent>
#include <QListView>
#include <QMimeData>
#include <QPushButton>

FrameworkManager::FrameworkManager(QWidget *parent) : QDialog{parent}
{
    //: This string refers to multiple frameworks (as in "Manager of frameworks").
    setWindowTitle(tr("Framework Manager"));
    setWindowIcon(QIcon::fromTheme("tool-frameworkmanager"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setAcceptDrops(true);
    resize(Utils::scale(440, 300));

    filesystemModel.setRootPath(Apktool::getFrameworksPath());

    frameworkList = new QListView(this);
    frameworkList->setModel(&filesystemModel);
    frameworkList->setRootIndex(filesystemModel.index(filesystemModel.rootPath()));
    frameworkList->setSelectionMode(QAbstractItemView::ExtendedSelection);

    rubberBand = new QRubberBand(QRubberBand::Rectangle, this);

    //: The "&" is a shortcut key prefix, not an "and" conjunction. Details: https://github.com/kefir500/apk-editor-studio/wiki/Translation-Guide#shortcuts
    auto btnInstall = new QPushButton(tr("&Install"), this);
    connect(btnInstall, &QPushButton::clicked, this, [this]() {
        auto frameworks = Dialogs::getOpenApkFilenames(this);
        for (const auto &framework : qAsConst(frameworks)) {
            installFramework(framework);
        }
    });

    auto btnRemove = new QPushButton(tr("Remove"), this);
    btnRemove->setEnabled(false);
    connect(btnRemove, &QPushButton::clicked, this, [this]() {
        auto selectedIndexes = frameworkList->selectionModel()->selectedIndexes();
        if (selectedIndexes.isEmpty()) {
            return;
        }
        const QString question("Are you sure you want to remove the selected framework(s)?");
        if (QMessageBox::question(this, {}, question) != QMessageBox::Yes) {
            return;
        }
        for (const auto &index : qAsConst(selectedIndexes)) {
            const auto framework = filesystemModel.data(index).toString();
            if (!framework.isEmpty()) {
                removeFramework(framework);
            }
        }
    });

    connect(frameworkList->selectionModel(), &QItemSelectionModel::selectionChanged, this, [this, btnRemove]() {
        btnRemove->setEnabled(!frameworkList->selectionModel()->selectedIndexes().isEmpty());
    });

    auto btnBrowse = new QPushButton(tr("Open Directory"), this);
    connect(btnBrowse, &QPushButton::clicked, this, [this]() {
        Utils::explore(Apktool::getFrameworksPath());
    });

    auto buttonsLayout = new QVBoxLayout;
    buttonsLayout->addWidget(btnInstall);
    buttonsLayout->addWidget(btnRemove);
    buttonsLayout->addWidget(btnBrowse);
    buttonsLayout->addStretch();

    auto layout = new QHBoxLayout(this);
    layout->addWidget(frameworkList);
    layout->addLayout(buttonsLayout);
}

void FrameworkManager::dragEnterEvent(QDragEnterEvent *event)
{
    event->accept();
    rubberBand->setVisible(event->mimeData()->hasUrls());
}

void FrameworkManager::dragLeaveEvent(QDragLeaveEvent *event)
{
    Q_UNUSED(event)
    rubberBand->hide();
}

void FrameworkManager::dropEvent(QDropEvent *event)
{
    const auto mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        event->acceptProposedAction();
        const auto urls = mimeData->urls();
        for (const auto &url : urls) {
            installFramework(url.toLocalFile());
        }
    }
    rubberBand->hide();
}

void FrameworkManager::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    rubberBand->setGeometry(frameworkList->geometry());
}

void FrameworkManager::installFramework(const QString &path)
{
    auto apktool = new Apktool::InstallFramework(path, Apktool::getFrameworksPath(), this);
    connect(apktool, &Apktool::InstallFramework::finished, this, [=](bool success) {
        if (!success) {
            const auto filename = QFileInfo(path).fileName();
            //: "%1" will be replaced with a framework file name.
            Dialogs::detailed(tr("Could not install the \"%1\" framework.").arg(filename),
                              apktool->output(), QMessageBox::Warning, this);
        }
        apktool->deleteLater();
    });
    apktool->run();
}

void FrameworkManager::removeFramework(const QString &name)
{
    if (!QFile::remove(QString("%1/%2").arg(Apktool::getFrameworksPath(), name))) {
        //: "%1" will be replaced with a framework file name.
        const QString message(tr("Could not remove the \"%1\" framework."));
        QMessageBox::warning(this, {}, message.arg(name));
    }
}
