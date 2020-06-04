#ifndef APPLICATION_H
#define APPLICATION_H

#include "apk/projectitemsmodel.h"
#include "base/actionprovider.h"
#include "base/iconprovider.h"
#include "base/language.h"
#include "base/recent.h"
#include "base/settings.h"
#include "base/theme.h"
#include "windows/mainwindow.h"
#include <QtSingleApplication>
#include <QTranslator>

class Application : public QtSingleApplication
{
    Q_OBJECT

public:
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

    const Theme *theme() const;

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


    void setLanguage(const QString &locale);
    MainWindow *window;
    Settings *settings;
    Recent *recent;
    IconProvider icons;
    ActionProvider actions;
    ProjectItemsModel projects;

protected:
    bool event(QEvent *event) override;

private:
    void processArguments(const QStringList &arguments);

    Theme *theme_;
    qreal scaleFactor;
    QTranslator translator;
    QTranslator translatorQt;
};

#define app (static_cast<Application *>(qApp))

#endif // APPLICATION_H
