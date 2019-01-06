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
        ColorHighlight,
        ColorBackgroundStart,
        ColorBackgroundEnd,
        ColorSuccess,
        ColorWarning,
        ColorError,
    };

    Application(int &argc, char **argv);
    ~Application() override;

    int exec();

    QString getTitle() const;
    QString getVersion() const;
    QString getTitleNoSpaces() const;
    QString getTitleAndVersion() const;

    QString getExecutableDirectory() const;
    QString getTemporaryPath(const QString &subdirectory = QString()) const;
    QString getLocalConfigPath(const QString &subdirectory = QString()) const;
    QString getSharedPath(const QString &resource = QString()) const;
    QString getBinaryPath(const QString &executable) const;
    QPixmap getLocaleFlag(const QLocale &locale) const;

    QList<Language> getLanguages() const;
    QColor getColor(Color color) const;

    int scale(int value) const;
    QSize scale(int width, int height) const;

    QPixmap loadPixmap(const QString &filename) const;

    QString getWebPage() const;
    QString getSourcePage() const;
    QString getIssuesPage() const;
    QString getContactPage() const;
    QString getTranslatePage() const;
    QString getDonatePage() const;
    QString getJrePage() const;
    QString getJdkPage() const;
    QString getUpdateUrl() const;

    Project *openApk(const QString &filename, bool unpack = true);
    bool closeApk(Project *project);
    bool installExternalApk();

    void setLanguage(const QString &locale);
    bool addToRecent(const Project *project);

    bool associate() const;
    void visitWebPage() const;
    void visitContactPage() const;
    void visitTranslatePage() const;
    void visitDonatePage() const;
    void visitJrePage() const;
    void visitJdkPage() const;

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
