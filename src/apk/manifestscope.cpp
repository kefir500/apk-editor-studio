#include "apk/manifestscope.h"

QString ManifestScope::name() const
{
    return node.attribute("android:name");
}

ManifestAttribute ManifestScope::label()
{
    return ManifestAttribute(node.attributeNode("android:label"));
}

ManifestAttribute ManifestScope::icon()
{
    return ManifestAttribute(node.attributeNode("android:icon"));
}

ManifestAttribute ManifestScope::roundIcon()
{
    return ManifestAttribute(node.attributeNode("android:roundIcon"));
}

ManifestAttribute ManifestScope::banner()
{
    return ManifestAttribute(node.attributeNode("android:banner"));
}

ManifestScope::Type ManifestScope::type() const
{
    if (node.tagName() == "application") {
        return Type::Application;
    } else if (node.tagName() == "activity") {
        return Type::Activity;
    }
    return Type::Invalid;
}
