#include "widgets/logview.h"
#include "windows/dialogs.h"
#include "base/application.h"

LogView::LogView(QWidget *parent) : QListView(parent)
{
    delegate = new LogDelegate(this);
    setItemDelegate(delegate);
    connect(delegate, &LogDelegate::updated, [this]() {
        viewport()->update();
    });
    connect(this, &LogView::clicked, [this](const QModelIndex &index) {
        clearSelection();
        const QString message = index.sibling(index.row(), LogModel::DescriptiveColumn).data().toString();
        if (!message.isEmpty()) {
            //: "Log" as in event log, message log, etc.
            Dialogs::log(tr("Log"), message, this);
        }
    });
}

LogModel *LogView::model() const
{
    return static_cast<LogModel *>(QListView::model());
}

void LogView::setModel(QAbstractItemModel *model)
{
    auto previousModel = this->model();
    if (previousModel) {
        disconnect(previousModel, &LogModel::added, this, nullptr);
        disconnect(previousModel, &LogModel::loadingStateChanged, delegate, &LogDelegate::setLoading);
    }
    if (model) {
        LogModel *logModel = qobject_cast<LogModel *>(model);
        Q_ASSERT(logModel);
        connect(logModel, &LogModel::added, [](LogEntry *entry) {
            if (entry->getType() != LogEntry::Information) {
                app->window->activateWindow();
            }
        });
        connect(logModel, &LogModel::loadingStateChanged, delegate, &LogDelegate::setLoading);
    }
    QListView::setModel(model);
}

QSize LogView::sizeHint() const
{
    return app->scale(240, 0);
}
