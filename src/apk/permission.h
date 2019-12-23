#ifndef PERMISSION_H
#define PERMISSION_H

#include <QDomElement>

class Permission
{
public:
    explicit Permission(const QDomElement &node) : node(node) {}

    bool operator==(const Permission &permission) const;

    QString getName() const;
    const QDomElement &getNode() const;

private:
    QDomElement node;
};

#endif // PERMISSION_H
