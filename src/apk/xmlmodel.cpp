#include "apk/xmlmodel.h"
#include <QTextStream>
#include <QDebug>

XmlResourceModel::XmlResourceModel(const QString &path, QObject *parent) : QAbstractItemModel(parent)
{
    file = new QFile(path, this);
    if (file->open(QFile::ReadWrite)) {
        QTextStream stream(file);
        stream.setCodec("UTF-8");
        dom.setContent(stream.readAll());
    } else {
        qWarning() << "Error: Could not read XML file";
    }

    QDomElement resources = dom.firstChildElement("resources");
    beginInsertRows(QModelIndex(), 0, resources.childNodes().count() - 1);
        root = addNode(dom.firstChildElement("resources"));
    endInsertRows();
}

XmlResourceModel::~XmlResourceModel()
{
    delete root;
}

bool XmlResourceModel::save(const QString &as) const
{
    QFile *file = this->file;
    if (!as.isEmpty()) {
        file = new QFile(as);
        file->open(QFile::WriteOnly);
    }

    bool result;
    if (file->isWritable()) {
        file->resize(0);
        QTextStream stream(file);
        stream.setCodec("UTF-8");
        dom.save(stream, 4);
        result = true;
    } else {
        qWarning() << "Error: Could not write XML file";
        result = false;
    }

    if (!as.isEmpty()) {
        delete file;
    }
    return result;
}

QString XmlResourceModel::getFilename() const
{
    return file->fileName();
}

XmlNode *XmlResourceModel::addNode(const QDomElement &node)
{
    XmlNode *resource = new XmlNode(node);
    QDomNodeList childNodes = node.childNodes();
    for (int i = 0; i < childNodes.count(); ++i) {
        QDomNode childNode = childNodes.at(i);
        if (childNode.isElement()) {
            XmlNode *child = addNode(childNode.toElement());
            resource->addChild(child);
        }
    }
    return resource;
}

bool XmlResourceModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole && index.column() == Value) {
        XmlNode *node = static_cast<XmlNode *>(index.internalPointer());
        node->setValue(value.toString());
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

QVariant XmlResourceModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid()) {
        const int column = index.column();
        XmlNode *string = static_cast<XmlNode *>(index.internalPointer());
        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            switch (column) {
                case Key:   return string->getAttribute("name");
                case Value: return string->getValue();
            }
        }
    }
    return QVariant();
}

QVariant XmlResourceModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        if (role == Qt::DisplayRole) {
            switch (section) {
                case Key:   return tr("Key");
                case Value: return tr("Value");
            }
        }
    }
    return QVariant();
}

QModelIndex XmlResourceModel::index(int row, int column, const QModelIndex &parent) const
{
    if (hasIndex(row, column, parent)) {
        XmlNode *parentItem = parent.isValid() ?  static_cast<XmlNode *>(parent.internalPointer()): root;
        XmlNode *childItem = parentItem->getChild(row);
        if (childItem) {
            return createIndex(row, column, childItem);
        }
    }
    return QModelIndex();
}

QModelIndex XmlResourceModel::parent(const QModelIndex &index) const
{
    if (index.isValid()) {
        XmlNode *childItem = static_cast<XmlNode *>(index.internalPointer());
        XmlNode *parentItem = childItem->getParent();
        if (parentItem != root) {
            return createIndex(parentItem->row(), 0, parentItem);
        }
    }
    return QModelIndex();
}

int XmlResourceModel::rowCount(const QModelIndex &parent) const
{
    XmlNode *parentItem = parent.isValid() ? static_cast<XmlNode *>(parent.internalPointer()) : root;
    return parentItem->childCount();
}

int XmlResourceModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return ColumnCount;
}

Qt::ItemFlags XmlResourceModel::flags(const QModelIndex &index) const
{
    if (index.column() == Value) {
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    } else {
        return QAbstractItemModel::flags(index);
    }
}
