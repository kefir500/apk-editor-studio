#include "apk/permission.h"

bool Permission::operator==(const Permission &permission) const
{
    return node == permission.node;
}

QString Permission::getName() const
{
    return node.attribute("android:name");
}

const QDomElement &Permission::getNode() const
{
    return node;
}
