#ifndef PROPERTYEDITOR_H
#define PROPERTYEDITOR_H

#include "editors/baseeditor.h"
#include "apk/xmlmodel.h"
#include <QTableView>

class PropertyEditor : public BaseEditor
{
    Q_OBJECT

public:
    explicit PropertyEditor(const QString &filename = QString(), QWidget *parent = nullptr);
    ~PropertyEditor() Q_DECL_OVERRIDE;

    bool save(const QString &as = QString()) Q_DECL_OVERRIDE;

private:
    QTableView *table;
    XmlResourceModel *model;
};

#endif // PROPERTYEDITOR_H
