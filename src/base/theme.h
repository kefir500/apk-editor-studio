#ifndef THEME_H
#define THEME_H

#include <QColor>

class Theme
{
public:
    enum class Color {
        Success,
        Error,
        BackgroundGradientStart,
        BackgroundGradientEnd,
    };

    virtual ~Theme() = default;
    virtual QColor color(Color id) const = 0;
    void setColor();
};

class LightTheme : public Theme
{
public:
    QColor color(Color id) const override;
};

class DarkTheme : public Theme
{
public:
    QColor color(Color id) const override;
};

#endif // THEME_H
