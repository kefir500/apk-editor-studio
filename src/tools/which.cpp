#include "tools/which.h"
#include <QStringList>

Result<QString> Which::find(const QString &application)
{
    Which which;
    return which.startSync({application});
}
