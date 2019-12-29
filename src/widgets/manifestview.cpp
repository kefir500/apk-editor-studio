#include "widgets/manifestview.h"
#include "widgets/itembuttondelegate.h"
#include "windows/selectdialog.h"
#include "base/application.h"
#include "base/utils.h"
#include <QEvent>
#include <QHeaderView>

ManifestView::ManifestView(QWidget *parent) : QTableView(parent)
{
    setMouseTracking(true);
    viewport()->setAttribute(Qt::WA_Hover, true);
    horizontalHeader()->hide();
    horizontalHeader()->setStretchLastSection(true);
    verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::AllEditTriggers);
    setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    ItemButtonDelegate *buttonDelegate = new ItemButtonDelegate(this);
    setItemDelegate(buttonDelegate);
    connect(buttonDelegate, &ItemButtonDelegate::clicked, [=](int row) {
        switch (row) {
        case ManifestModel::MinimumSdkRow: {
            const QString dialogTitle = model()->headerData(ManifestModel::MinimumSdkRow, Qt::Vertical).toString();
            const int currentApi = model()->getMinimumSdk();
            const int api = selectAndroidApi(dialogTitle, currentApi);
            if (api) {
                model()->setMinimumSdk(api);
            }
            break;
        }
        case ManifestModel::TargetSdkRow: {
            const QString dialogTitle = model()->headerData(ManifestModel::TargetSdkRow, Qt::Vertical).toString();
            const int currentApi = model()->getTargetSdk();
            const int api = selectAndroidApi(dialogTitle, currentApi);
            if (api) {
                model()->setTargetSdk(api);
            }
            break;
        }
        default:
            emit titleEditorRequested(static_cast<ManifestModel::Row>(row));
        }
    });
}

ManifestModel *ManifestView::model() const
{
    return static_cast<ManifestModel *>(QTableView::model());
}

void ManifestView::setModel(QAbstractItemModel *model)
{
    if (model) {
        Q_ASSERT(qobject_cast<ManifestModel *>(model));
        QTableView::setModel(model);
    }
}

QSize ManifestView::sizeHint() const
{
    return QSize(app->scale(240), 117);
}

int ManifestView::selectAndroidApi(const QString &dialogTitle, int defaultApi)
{
    QList<int> apiLevels;
    QStringList apiCodenames;
    int selection = 0;
    for (int api = Utils::ANDROID_3; api < Utils::ANDROID_SDK_COUNT; ++api) {
        apiLevels.append(api);
        apiCodenames.append(QString("%1 (%2)").arg(api).arg(Utils::getAndroidCodename(api)));
        if (api == defaultApi) {
            selection = apiLevels.count() - 1;
        }
    }
    selection = SelectDialog::select(dialogTitle, apiCodenames, selection, this);
    return (selection != -1) ? apiLevels.at(selection) : 0;
}
