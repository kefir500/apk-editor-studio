#include "tools/javac.h"
#include <QRegularExpression>

QString Javac::version() const
{
    QStringList arguments;
    arguments << "-version";
    auto result = startSync(arguments);
    if (!result.success) {
        return QString();
    }
    QRegularExpression regex("javac (.+)");
    const QString version = regex.match(result.value).captured(1);
    return version;
}
