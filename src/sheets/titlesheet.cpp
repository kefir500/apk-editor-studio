#include "sheets/titlesheet.h"
#include "widgets/loadingwidget.h"
#include "apk/titleitemsmodel.h"
#include <QBoxLayout>
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QTableView>

TitleSheet::TitleSheet(const Package *package, QWidget *parent) : BaseEditableSheet(parent)
{
    setSheetIcon(QIcon::fromTheme("tool-titleeditor"));

    table = new QTableView(this);
    table->setAlternatingRowColors(true);
    table->setHorizontalScrollMode(QTableView::ScrollPerPixel);

    auto loading = new LoadingWidget(this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(table);

    model = new TitleItemsModel(package, this);
    connect(model, &TitleItemsModel::initialized, this, [=]() {
        auto sortProxy = new QSortFilterProxyModel(this);
        sortProxy->setSourceModel(model);
        table->setModel(sortProxy);
        table->setSortingEnabled(true);
        table->resizeColumnsToContents();
        loading->hide();
    });
    connect(model, &TitleItemsModel::dataChanged, this, [this]() {
        setModified(true);
    });

    retranslate();
}

bool TitleSheet::save(const QString &as)
{
    Q_UNUSED(as)
    if (!model) {
        return false;
    }
    model->save();
    setModified(false);
    emit saved();
    return true;
}

void TitleSheet::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        retranslate();
    }
    BaseEditableSheet::changeEvent(event);
}

void TitleSheet::retranslate()
{
    setSheetTitle(tr("Application Title"));
}
