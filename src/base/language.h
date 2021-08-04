#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <QLocale>
#include <QIcon>

class Language
{
public:
    Language(const QString &path);

    QString getTitle() const;
    QString getCode() const;
    QIcon getFlag() const;

private:
    QLocale locale;
    QString title;
    QString code;
    QIcon flag;
};

#endif // LANGUAGE_H
