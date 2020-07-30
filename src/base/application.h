#ifndef APPLICATION_H
#define APPLICATION_H

#include "apk/projectitemsmodel.h"
#include "base/actionprovider.h"
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

    static QList<Language> getLanguages();
    const Theme *theme() const;
    void setLanguage(const QString &locale);

    Settings *settings;
    Recent *recent;
    ActionProvider actions{projects};

protected:
    bool event(QEvent *event) override;

private:
    void processArguments(const QStringList &arguments, MainWindow *window = nullptr);

    QList<MainWindow *> instances;
    ProjectItemsModel projects;
    Theme *theme_;
    QTranslator translator;
    QTranslator translatorQt;
};

#define app (static_cast<Application *>(qApp))

#endif // APPLICATION_H
