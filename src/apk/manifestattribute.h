#ifndef MANIFESTATTRIBUTE_H
#define MANIFESTATTRIBUTE_H

#include <QDomAttr>

class ManifestAttribute
{
public:
    ManifestAttribute() = default;
    ManifestAttribute(const QDomAttr &node) : node(node) {}

    QString getValue() const;
    QString getResourceType() const;
    QString getResourceName() const;

    void setValue(const QString &value);

private:
    QDomAttr node;
};

#endif // MANIFESTATTRIBUTE_H
