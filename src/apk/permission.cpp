#include "apk/permission.h"

QString Permission::getName() const
{
    return node.attribute("android:name");
}

const QDomElement &Permission::getNode() const
{
    return node;
}
