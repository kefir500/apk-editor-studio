#ifndef APPLICATION_H
#define APPLICATION_H

#include "apk/projectsmodel.h"
#include "base/formats.h"
#include "base/language.h"
#include "base/recent.h"
#include "base/settings.h"
#include "base/updater.h"
#include "windows/mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QMessageBox>

class Application : public QApplication
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
    ~Application() Q_DECL_OVERRIDE;

    int exec();

    QString getTitle() const;
    QString getVersion() const;
    QString getTitleNoSpaces() const;
    QString getTitleAndVersion() const;

    QString getDirectory() const;
    QString getTemporaryPath() const;
    QString getOutputPath() const;
    QString getFrameworksPath() const;
    QString getLocalConfigPath(QString subdirectory = QString()) const;
    QString getSharedPath(const QString &resource = QString()) const;
    QString getToolPath(const QString &tool) const;
    QPixmap getLocaleFlag(const QLocale &locale) const;

    QList<Language> getLanguages() const;
    QColor getColor(Color color) const;

    int scale(int value) const;
    QSize scale(int width, int height) const;

    QIcon loadIcon(const QString &filename) const;
    QPixmap loadPixmap(const QString &filename) const;
    bool replaceImage(const QString &targetPath, const QString &sourcePath) const;

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
    bool explore(const QString &path);
    void visitWebPage() const;
    void visitContactPage() const;
    void visitTranslatePage() const;
    void visitDonatePage() const;
    void visitJrePage() const;
    void visitJdkPage() const;

    MainWindow *window;
    ProjectsModel projects;
    Formats formats;
    Settings *settings; // TODO Check on macOS: See https://github.com/kefir500/apk-icon-editor/commit/6ac051d
    Recent *recent;
    QMap<QString, QAction *> toolbar;
    QTranslator translator;
    QTranslator translatorQt;

protected:
    bool event(QEvent *event) Q_DECL_OVERRIDE;

private:
    qreal scaleFactor;
};

#define app (static_cast<Application *>(qApp))

#endif // APPLICATION_H
