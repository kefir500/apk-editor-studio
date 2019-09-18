#include "tools/javac.h"
#include "base/application.h"
#include <QRegularExpression>

Javac::Javac(QObject *parent) : Javac(app->getJavaBinaryPath("javac"), parent) {}

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
