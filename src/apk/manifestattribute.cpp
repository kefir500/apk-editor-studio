#include "apk/manifestattribute.h"
#include <QStringList>

QString ManifestAttribute::getValue() const
{
    return node.value();
}

QString ManifestAttribute::getResourceType() const
{
    return node.value().split('/').value(0).mid(1);
}

QString ManifestAttribute::getResourceName() const
{
    return node.value().split('/').value(1);
}

void ManifestAttribute::setValue(const QString value)
{
    node.setValue(value);
}
