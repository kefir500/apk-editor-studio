#include "apk/project.h"
#include "apk/package.h"
#include "base/application.h"
#include "base/settings.h"
#include "base/utils.h"
#include "sheets/codesheet.h"
#include "sheets/imagesheet.h"
#include "sheets/projectsheet.h"
#include "sheets/titlesheet.h"
#include "windows/devicemanager.h"
#include "windows/dialogs.h"
#include "windows/rememberdialog.h"
#include "windows/permissioneditor.h"
#include "windows/signatureviewer.h"
#include "tools/keystore.h"
#include <QImageReader>
#include <QInputDialog>
#include <QMimeDatabase>

Project::Project(Package *package, QWidget *parent)
    : QObject(parent)
    , package(package)
{
    tabWidget = new QTabWidget(parent);
    tabWidget->setMovable(true);
    tabWidget->setTabsClosable(true);

    connect(tabWidget, &QTabWidget::currentChanged, this, [this]() {
        emit currentTabChanged(getCurrentTab());
    });
    connect(tabWidget, &QTabWidget::tabCloseRequested, this, [this](int index) {
        closeTab(static_cast<BaseSheet *>(tabWidget->widget(index)));
    });

    openProjectTab();
}

void Project::openProjectTab()
{
    const QString identifier = "project";
    auto existing = getTabByIdentifier(identifier);
    if (existing) {
        setCurrentTab(existing);
        return;
    }

    auto tab = new ProjectSheet(package, parentWidget());
    tab->setProperty("identifier", identifier);
    connect(tab, &ProjectSheet::titleEditorRequested, this, &Project::openTitlesTab);
    connect(tab, &ProjectSheet::apkSaveRequested, this, &Project::saveProject);
    connect(tab, &ProjectSheet::apkInstallRequested, this, &Project::installProject);
    addTab(tab);
}

void Project::openTitlesTab()
{
    const QString identifier = "titles";
    auto existing = getTabByIdentifier(identifier);
    if (existing) {
        setCurrentTab(existing);
        return;
    }

    auto editor = new TitleSheet(package, parentWidget());
    editor->setProperty("identifier", identifier);
    addTab(editor);
}

void Project::openResourceTab(const ResourceModelIndex &index)
{
    const QString path = index.path();
    if (path.isEmpty()) {
        return;
    }
    const QString identifier = path;
    auto existing = getTabByIdentifier(identifier);
    if (existing) {
        setCurrentTab(existing);
        return;
    }

    BaseEditableSheet *editor = nullptr;
    const auto extension = QFileInfo(path).suffix();
    if (QImageReader::supportedImageFormats().contains(extension.toUtf8())) {
        editor = new ImageSheet(index, parentWidget());
    } else if (QMimeDatabase().mimeTypeForFile(path).inherits("text/plain")) {
        editor = new CodeSheet(index, parentWidget());
    } else {
        QMessageBox::warning(parentWidget(), {}, tr("The format is not supported."));
        return;
    }
    editor->setProperty("identifier", identifier);
    addTab(editor);
}

void Project::openPermissionEditor()
{
    PermissionEditor permissionEditor(package->manifest, parentWidget());
    permissionEditor.exec();
}

void Project::openPackageRenamer()
{
    RememberDialog::say("experimental-rename-package", tr(
        "Package renaming is an experimental function which, in its current state, "
        "may lead to crashes and data loss. You can join the discussion and help us "
        "improve this feature <a href=\"%1\">here</a>."
    ).arg("https://github.com/kefir500/apk-editor-studio/discussions/39"), parentWidget());

    if (!app->settings->getDecompileSources() && !package->hasSourcesUnpacked()) {
        const QString question = tr(
            "Cloning the APK requires the source code decompilation to be turned on. "
            "Proceed?");
        if (QMessageBox::question(parentWidget(), {}, question) == QMessageBox::Yes) {
            app->settings->setDecompileSources(true);
            QMessageBox::information(parentWidget(), {}, tr("Settings have been applied. Please, reopen this APK."));
        }
        return;
    }

    if (!package->hasSourcesUnpacked()) {
        QMessageBox::warning(parentWidget(), {}, tr(
            "Please, reopen this APK in order to unpack the source code and clone the APK."));
        return;
    }

    const auto inputDialogFlags = QDialog().windowFlags() & ~Qt::WindowContextHelpButtonHint;
    const QString newPackageName = QInputDialog::getText(parentWidget(), {}, tr("Package Name:"),
                                                         QLineEdit::Normal, package->getPackageName(),
                                                         nullptr, inputDialogFlags);
    if (newPackageName.isEmpty()) {
        return;
    }

    if (!package->setPackageName(newPackageName)) {
        QMessageBox::warning(parentWidget(), {}, tr("Could not clone the APK."));
        return;
    }

    QMessageBox::information(parentWidget(), {}, tr("APK has been successfully cloned!"));
}

void Project::openSignatureViewer()
{
    SignatureViewer signatureViewer(package->getOriginalPath(), parentWidget());
    signatureViewer.exec();
}

bool Project::saveTabs()
{
    bool result = true;
    for (int index = 0; index < tabWidget->count(); ++index) {
        auto tab = qobject_cast<BaseEditableSheet *>(tabWidget->widget(index));
        if (tab && !tab->save()) {
            result = false;
        }
    }
    return result;
}

bool Project::saveProject()
{
    if (hasUnsavedTabs()) {
        const QString question = tr("Do you want to save changes before packing?");
        const int answer = QMessageBox::question(parentWidget(), QString(), question, QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        switch (answer) {
        case QMessageBox::Yes:
        case QMessageBox::Save:
            saveTabs();
            break;
        case QMessageBox::No:
        case QMessageBox::Discard:
            break;
        default:
            return false;
        }
    }

    const QString target = Dialogs::getSaveApkFilename(package, parentWidget());
    if (target.isEmpty()) {
        return false;
    }
    auto command = package->createCommandChain();
    command->add(package->createPackCommand(target), true);
    if (app->settings->getOptimizeApk()) {
        command->add(package->createZipalignCommand(target), false);
    }
    if (app->settings->getSignApk()) {
        auto keystore = Keystore::get(parentWidget());
        if (keystore) {
            command->add(package->createSignCommand(keystore.get(), target), false);
        }
    }
    command->run();
    return true;
}

bool Project::installProject()
{
    const auto device = Dialogs::getInstallDevice(parentWidget());
    if (device.isNull()) {
        return false;
    }

    QString target;
    auto command = package->createCommandChain();

    if (isUnsaved()) {
        const QString question = tr("Do you want to save changes and pack the APK before installing?");
        const int answer = QMessageBox::question(parentWidget(), QString(), question, QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        switch (answer) {
        case QMessageBox::Yes:
        case QMessageBox::Save: {
            saveTabs();
            target = Dialogs::getSaveApkFilename(package, parentWidget());
            if (target.isEmpty()) {
                delete command;
                return false;
            }
            command->add(package->createPackCommand(target), true);
            if (app->settings->getOptimizeApk()) {
                command->add(package->createZipalignCommand(target), false);
            }
            if (app->settings->getSignApk()) {
                auto keystore = Keystore::get(parentWidget());
                if (keystore) {
                    command->add(package->createSignCommand(keystore.get(), target), false);
                }
            }
            break;
        }
        case QMessageBox::No:
        case QMessageBox::Discard:
            break;
        default:
            return false;
        }
    }

    command->add(package->createInstallCommand(device.getSerial(), target));
    command->run();
    return true;
}

bool Project::exploreProject()
{
    return Utils::explore(package->getContentsPath());
}

Package *Project::getPackage() const
{
    return package;
}

BaseSheet *Project::getCurrentTab() const
{
    return static_cast<BaseSheet *>(tabWidget->currentWidget());
}

QTabWidget *Project::tabs() const
{
    return tabWidget;
}

bool Project::isUnsaved() const
{
    return package->getState().isModified() || hasUnsavedTabs();
}

int Project::addTab(BaseSheet *tab)
{
    // TODO Tab title is not retranslated on language change
    const int tabIndex = tabWidget->addTab(tab, tab->getIcon(), tab->getTitle());
    tabWidget->setCurrentIndex(tabIndex);
    auto editor = qobject_cast<BaseEditableSheet *>(tab);
    if (editor) {
        connect(editor, &BaseEditableSheet::saved, this, [this]() {
            // Project save indicator:
            const_cast<PackageState &>(package->getState()).setModified(true);
        });
        connect(editor, &BaseEditableSheet::modifiedStateChanged, this, [=](bool modified) {
            // Tab save indicator:
            const QString indicator = QString("%1 ").arg(QChar(0x2022));
            const int tabIndex = tabWidget->indexOf(editor);
            QString tabTitle = tabWidget->tabText(tabIndex);
            const bool titleHasModifiedMark = tabTitle.startsWith(indicator);
            if (modified && !titleHasModifiedMark) {
                tabTitle.prepend(indicator);
                tabWidget->setTabText(tabIndex, tabTitle);
            } else if (!modified && titleHasModifiedMark) {
                tabTitle.remove(0, indicator.length());
                tabWidget->setTabText(tabIndex, tabTitle);
            }
        });
    }
    connect(tab, &BaseSheet::titleChanged, this, [=](const QString &title) {
        tabWidget->setTabText(tabWidget->indexOf(tab), title);
    });
    connect(tab, &BaseSheet::iconChanged, this, [=](const QIcon &icon) {
        tabWidget->setTabIcon(tabWidget->indexOf(tab), icon);
    });
    return tabIndex;
}

bool Project::closeTab(BaseSheet *tab)
{
    if (!tab->finalize()) {
        return false;
    }
    delete tab;
    return true;
}

void Project::setCurrentTab(BaseSheet *tab)
{
    tabWidget->setCurrentIndex(tabWidget->indexOf(tab));
}

bool Project::hasUnsavedTabs() const
{
    for (int index = 0; index < tabWidget->count(); ++index) {
        auto editor = qobject_cast<BaseEditableSheet *>(tabWidget->widget(index));
        if (editor && editor->isModified()) {
            return true;
        }
    }
    return false;
}

BaseSheet *Project::getTabByIdentifier(const QString &identifier) const
{
    for (int index = 0; index < tabWidget->count(); ++index) {
        auto tab = static_cast<BaseSheet *>(tabWidget->widget(index));
        if (tab->property("identifier") == identifier) {
            return tab;
        }
    }
    return nullptr;
}

QWidget *Project::parentWidget() const
{
    return static_cast<QWidget *>(parent());
}
