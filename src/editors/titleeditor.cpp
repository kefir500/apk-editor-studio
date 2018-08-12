#include "editors/titleeditor.h"
#include "windows/waitdialog.h"
#include "base/application.h"
#include <QBoxLayout>
#include <QHeaderView>

TitleEditor::TitleEditor(const Project *project, QWidget *parent) : BaseEditor(QString(), QPixmap(), parent)
{
    title = tr("Application Title");
    icon = app->loadIcon("title.png");

    WAIT
    model = new TitlesModel(project, this);

    table = new QTableView(this);
    table->setAlternatingRowColors(true);
    table->setHorizontalScrollMode(QTableView::ScrollPerPixel);
    table->setModel(model);
    table->setSortingEnabled(true);
    table->resizeColumnsToContents();

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(table);

    connect(model, &TitlesModel::dataChanged, [=]() {
        setModified(true);
    });
}

bool TitleEditor::load()
{
    Q_UNUSED(filename)
    return true;
}

bool TitleEditor::save(const QString &as)
{
    Q_UNUSED(as);
    const bool result = model->save();
    setModified(!result);
    return result;
}
