#include "base/language.h"
#include "base/application.h"
#include "base/utils.h"

Language::Language(const QString &path)
{
    code = path.split('.').at(1);

    locale = QLocale(code);
    title = Utils::capitalize(locale.nativeLanguageName());

    const QLocale::Language localeLanguage = locale.language();
    const QLocale::Country localeCountry = locale.country();
    const QStringList localeSegments = QLocale(localeLanguage, localeCountry).name().split('_');
    if (localeSegments.count() > 1) {
        flag = QPixmap(QString(app->getSharedPath("resources/flags/%1.png")).arg(localeSegments.at(1).toLower()));
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

QPixmap Language::getFlag() const
{
    return flag;
}
