#include "editors/titleeditor.h"
#include "widgets/loadingwidget.h"
#include "windows/waitdialog.h"
#include "base/application.h"
#include <QBoxLayout>
#include <QHeaderView>
#include <QSortFilterProxyModel>

TitleEditor::TitleEditor(const Project *project, QWidget *parent) : Editor(parent)
{
    title = tr("Application Title");
    icon = app->icons.get("title.png");

    table = new QTableView(this);
    table->setAlternatingRowColors(true);
    table->setHorizontalScrollMode(QTableView::ScrollPerPixel);
    table->hide();

    auto loading = new LoadingWidget(this);
    loading->play();

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(table);
    layout->addWidget(loading);

    model = new TitleItemsModel(project, this);
    connect(model, &TitleItemsModel::initialized, this, [=]() {
        auto sortProxy = new QSortFilterProxyModel(this);
        sortProxy->setSourceModel(model);
        table->setModel(sortProxy);
        table->setSortingEnabled(true);
        table->resizeColumnsToContents();
        table->show();
        loading->stop();
    });
    connect(model, &TitleItemsModel::dataChanged, [=]() {
        setModified(true);
    });
}

bool TitleEditor::save(const QString &as)
{
    Q_UNUSED(as)
    if (!model || !model->save()) {
        return false;
    }
    setModified(false);
    emit saved();
    return true;
}
