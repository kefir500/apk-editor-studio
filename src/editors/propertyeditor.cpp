#include "editors/propertyeditor.h"
#include <QBoxLayout>
#include <QHeaderView>

PropertyEditor::PropertyEditor(const QString &filename, QWidget *parent) : BaseEditor(filename, QPixmap(), parent)
{
    this->model = new XmlResourceModel(filename, this);

    table = new QTableView(this);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    table->setHorizontalScrollMode(QTableView::ScrollPerPixel);
    table->setAlternatingRowColors(true);
    table->setModel(model);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(table);

    connect(model, &XmlResourceModel::dataChanged, [=]() {
        setModified(true);
    });
}

PropertyEditor::~PropertyEditor()
{
    if (model) {
        delete model;
    }
}

bool PropertyEditor::save(const QString &as)
{
    if (model) {
        if (as.isEmpty()) {
            const bool result = model->save();
            setModified(!result);
            return result;
        } else {
            return model->save(as);
        }
    }
    return false;
}
