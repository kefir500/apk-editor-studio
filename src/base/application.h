#ifndef APPLICATION_H
#define APPLICATION_H

#include "apk/projectitemsmodel.h"
#include "base/iconprovider.h"
#include "base/language.h"
#include "base/recent.h"
#include "base/settings.h"
#include "base/updater.h"
#include "windows/mainwindow.h"
#include <QtSingleApplication>
#include <QTranslator>
#include <QMessageBox>

class Application : public QtSingleApplication
{
    Q_OBJECT

public:
    enum Color {
        ColorLogoPrimary,
        ColorLogoSecondary,
        ColorAndroid,
        ColorBackgroundStart,
        ColorBackgroundEnd,
        ColorSuccess,
        ColorWarning,
        ColorError,
    };

    Application(int &argc, char **argv);
    ~Application() override;

    int exec();

    static QString getTitle();
    static QString getVersion();
    static QString getTitleNoSpaces();
    static QString getTitleAndVersion();

    static QString getExecutableDirectory();
    static QString getTemporaryPath(const QString &subdirectory = QString());
    static QString getLocalConfigPath(const QString &subdirectory = QString());
    static QString getSharedPath(const QString &resource = QString());
    static QString getBinaryPath(const QString &executable);
    static QString getJavaPath();
    static QString getJavaBinaryPath(const QString &executable);
    static QPixmap getLocaleFlag(const QLocale &locale);

    static QList<Language> getLanguages();
    static QColor getColor(Color color);

    int scale(int value) const;
    qreal scale(qreal value) const;
    QSize scale(int width, int height) const;

    static QString getWebPage();
    static QString getUpdatePage();
    static QString getSourcePage();
    static QString getIssuesPage();
    static QString getContactPage();
    static QString getTranslatePage();
    static QString getDonatePage();
    static QString getJrePage();
    static QString getJdkPage();
    static QString getUpdateUrl();

    Project *openApk(const QString &filename, bool unpack = true);
    bool closeApk(Project *project);
    bool installExternalApk();

    void setLanguage(const QString &locale);
    bool addToRecent(const Project *project);

    static bool associate();
    static void visitWebPage();
    static void visitUpdatePage();
    static void visitBlogPage(const QString &post);
    static void visitSourcePage();
    static void visitContactPage();
    static void visitTranslatePage();
    static void visitDonatePage();
    static void visitJrePage();
    static void visitJdkPage();

    MainWindow *window;
    ProjectItemsModel projects;
    Settings *settings;
    Recent *recent;
    IconProvider icons;
    QTranslator translator;
    QTranslator translatorQt;

protected:
    bool event(QEvent *event) override;

private:
    qreal scaleFactor;
};

#define app (static_cast<Application *>(qApp))

#endif // APPLICATION_H
