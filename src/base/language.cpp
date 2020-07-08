#include "base/language.h"
#include "base/utils.h"

Language::Language(const QString &path)
{
    code = path.split('.').at(1);
    locale = QLocale(code);
    title = Utils::capitalize(locale.nativeLanguageName());
    flag = QPixmap(Utils::getLocaleFlag(locale));
}

QString Language::getTitle() const
{
    return title;
}

QString Language::getCode() const
{
    return code;
}

QPixmap Language::getFlag() const
{
    return flag;
}
