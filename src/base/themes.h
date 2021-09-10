#ifndef THEMES_H
#define THEMES_H

#include <QMap>

class ITheme {
public:
    virtual QString title() const = 0;
    virtual void initialize() = 0;
    virtual ~ITheme() = default;
};

class SystemTheme : public ITheme {
public:
    virtual QString title() const;
    virtual void initialize();
};

class DarkStyleTheme : public ITheme {
public:
    virtual QString title() const;
    virtual void initialize();
};

class FusionTheme : public ITheme {
public:
    virtual QString title() const;
    virtual void initialize();
};

class ThemeRepository
{
public:
    ThemeRepository();
    ~ThemeRepository();

    ITheme *getTheme(const QString &name);
    QStringList getThemeNames() const;

private:
    QMap<QString, ITheme *> themes;
};

#endif // THEMES_H
