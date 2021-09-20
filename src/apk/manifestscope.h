#ifndef MANIFESTSCOPE_H
#define MANIFESTSCOPE_H

#include "apk/manifestattribute.h"

class ManifestScope
{
public:
    enum class Type {
        Unknown,
        Application,
        Activity
    };

    ManifestScope(QDomElement node) : node(node) {}

    QString name() const;
    ManifestAttribute label();
    ManifestAttribute icon();
    ManifestAttribute roundIcon();
    ManifestAttribute banner();
    Type type() const;

private:
    QDomElement node;
};

#endif // MANIFESTSCOPE_H
