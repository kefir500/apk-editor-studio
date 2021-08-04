#ifndef THEME_H
#define THEME_H

#include <QTextCharFormat>

class Theme
{
public:
    enum class Color {
        Success,
        Error,
        BackgroundGradientStart,
        BackgroundGradientEnd,
    };

    enum class Text {
        XmlDefault,
        XmlTagBracket,
        XmlTagName,
        XmlAttribute,
        XmlValue,
        XmlComment,
        YamlDefault,
        YamlKey,
        YamlValueDefault,
        YamlValueNumber,
        YamlComment,
    };

    virtual ~Theme() = default;
    virtual QColor color(Color id) const = 0;
    virtual QTextCharFormat text(Text id) const = 0;
};

class LightTheme : public Theme
{
public:
    QColor color(Color id) const override;
    QTextCharFormat text(Text id) const override;
};

class DarkTheme : public Theme
{
public:
    QColor color(Color id) const override;
    QTextCharFormat text(Text id) const override;
};

#endif // THEME_H
