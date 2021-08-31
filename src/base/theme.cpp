#include "base/theme.h"
#include <QPalette>

QColor LightTheme::color(Color id) const
{
    switch (id) {
    case Color::Success:
        return QColor(235, 250, 200);
    case Color::Error:
        return QColor(255, 200, 200);
    case Color::BackgroundGradientStart:
        return QColor(250, 255, 230);
    case Color::BackgroundGradientEnd:
        return QColor(240, 245, 220);
    }
    return QColor();
}

QColor DarkTheme::color(Color id) const
{
    switch (id) {
    case Color::Success:
        return QColor(75, 150, 90);
    case Color::Error:
        return QColor(190, 95, 95);
    case Color::BackgroundGradientStart:
        return QPalette().color(QPalette::Window);
    case Color::BackgroundGradientEnd:
        return QPalette().color(QPalette::Window).lighter(120);
    }
    return QColor();
}
