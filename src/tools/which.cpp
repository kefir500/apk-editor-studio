#include "tools/which.h"
#include <QStringList>

Result<QString> Which::find(const QString &application) const
{
    return startSync(QStringList() << application);
}
