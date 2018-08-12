#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <QLocale>
#include <QPixmap>

class Language {

public:
    Language(const QString &path);

    QString getTitle() const;
    QString getCode() const;
    QPixmap getFlag() const;

private:
    QLocale locale;
    QString title;
    QString code;
    QPixmap flag;
};

#endif // LANGUAGE_H
