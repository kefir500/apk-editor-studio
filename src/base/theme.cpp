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

QTextCharFormat LightTheme::text(Theme::Text id) const
{
    QTextCharFormat format;
    switch (id) {
    case Text::XmlDefault:
        format.setForeground(QPalette().windowText());
        format.setFontWeight(QFont::Bold);
        break;
    case Text::XmlTagBracket:
        format.setForeground(Qt::blue);
        break;
    case Text::XmlTagName:
        format.setForeground(Qt::blue);
        break;
    case Text::XmlAttribute:
        format.setForeground(Qt::red);
        break;
    case Text::XmlValue:
        format.setForeground(Qt::darkMagenta);
        break;
    case Text::XmlComment:
        format.setForeground(Qt::gray);
        break;
    case Text::YamlDefault:
        format.setForeground(QPalette().windowText());
        break;
    case Text::YamlKey:
        format.setForeground(Qt::darkBlue);
        format.setFontWeight(QFont::Bold);
        break;
    case Text::YamlValueDefault:
        format.setForeground(Qt::darkGreen);
        format.setFontWeight(QFont::Bold);
        break;
    case Text::YamlValueNumber:
        format.setForeground(QColor(210, 120, 20));
        format.setFontWeight(QFont::Bold);
        break;
    case Text::YamlComment:
        format.setForeground(Qt::gray);
        break;
    }
    return format;
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

QTextCharFormat DarkTheme::text(Theme::Text id) const
{
    const QColor blue(110, 180, 240);
    const QColor beige(230, 175, 150);
    const QColor green(105, 155, 85);
    QTextCharFormat format;
    switch (id) {
    case Text::XmlDefault:
        format.setForeground(QPalette().windowText());
        break;
    case Text::XmlTagBracket:
        format.setForeground(blue);
        break;
    case Text::XmlTagName:
        format.setForeground(blue);
        break;
    case Text::XmlAttribute:
        format.setForeground(QColor(155, 220, 255));
        break;
    case Text::XmlValue:
        format.setForeground(beige);
        break;
    case Text::XmlComment:
        format.setForeground(green);
        break;
    case Text::YamlDefault:
        format.setForeground(QPalette().windowText());
        break;
    case Text::YamlKey:
        format.setForeground(blue);
        break;
    case Text::YamlValueDefault:
        format.setForeground(beige);
        break;
    case Text::YamlValueNumber:
        format.setForeground(QColor(180, 205, 170));
        break;
    case Text::YamlComment:
        format.setForeground(green);
        break;
    }
    return format;
}
