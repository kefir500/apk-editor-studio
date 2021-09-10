#include "base/themes.h"
#include "base/utils.h"
#include <DarkStyle/DarkStyle.h>

ThemeRepository::ThemeRepository()
{
    themes.insert(QStringLiteral("system"), new SystemTheme());
    themes.insert(QStringLiteral("darkstyle"), new DarkStyleTheme());
    themes.insert(QStringLiteral("fusion"), new FusionTheme());
}

ThemeRepository::~ThemeRepository()
{
    qDeleteAll(themes);
}

ITheme *ThemeRepository::getTheme(const QString &name)
{
    return themes.value(name, themes.value(QStringLiteral("system")));
}

QStringList ThemeRepository::getThemeNames() const
{
    QStringList themeNames = themes.keys();
    themeNames.swapItemsAt(themeNames.indexOf(QStringLiteral("system")), 0);
    return themeNames;
}

// SystemTheme

QString SystemTheme::title() const
{
    return qApp->translate("SystemTheme", "System Theme");
}

void SystemTheme::initialize()
{
    QIcon::setThemeName(Utils::isLightTheme()
        ? QStringLiteral("apk-editor-studio")
        : QStringLiteral("apk-editor-studio-dark"));
}

// DarkStyleTheme

QString DarkStyleTheme::title() const
{
    return QStringLiteral("DarkStyle");
}

void DarkStyleTheme::initialize()
{
    qApp->setStyle(new DarkStyle());
    QIcon::setThemeName(QStringLiteral("apk-editor-studio-dark"));
}

// FusionTheme

QString FusionTheme::title() const
{
    return QStringLiteral("Fusion");
}

void FusionTheme::initialize()
{
    qApp->setStyle(QStringLiteral("Fusion"));
    QIcon::setThemeName(QStringLiteral("apk-editor-studio"));
}
