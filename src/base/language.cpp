#include "base/language.h"
#include "base/utils.h"

Language::Language(const QString &path)
{
    code = path.split('.').at(1);
    locale = (code == "bqi")
         ? QLocale(QLocale::NorthernLuri, QLocale::Iran)
         : QLocale(code);
    flag = Utils::getLocaleFlag(locale);

    if (code == "bqi") {
        // Provide the missing language title for Luri (Bakhtiari)
        title = "لۊری بختیاری";
    } else {
        title = Utils::capitalize(locale.nativeLanguageName());
    }
}

QString Language::getTitle() const
{
    return title;
}

QString Language::getCode() const
{
    return code;
}

QIcon Language::getFlag() const
{
    return flag;
}
