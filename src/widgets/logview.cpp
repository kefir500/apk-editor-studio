#include "widgets/logview.h"
#include "widgets/logdelegate.h"
#include "windows/dialogs.h"
#include "base/application.h"
#include <QPainter>

LogView::LogView(QWidget *parent) : QListView(parent)
{
    setItemDelegate(new LogDelegate(this));

    loading = new QVariantAnimation(this);
    loading->setDuration(1000);
    loading->setLoopCount(-1);
    loading->setKeyValueAt(0, QPalette().color(QPalette::Base));
    loading->setKeyValueAt(0.5, app->getColor(Application::ColorLogoPrimary));
    loading->setKeyValueAt(1, QPalette().color(QPalette::Base));
    loading->start();

    connect(this, &LogView::clicked, [this](const QModelIndex &index) {
        clearSelection();
        const QString message = index.sibling(index.row(), LogModel::LogDescriptive).data().toString();
        if (!message.isEmpty()) {
            //: "Log" as in event log, message log, etc.
            Dialogs::log(tr("Log"), message, this);
        }
    });
}

void LogView::setModel(QAbstractItemModel *model)
{
    disconnect(loading, &QVariantAnimation::valueChanged, nullptr, nullptr);

    if (model) {
        LogModel *logModel = qobject_cast<LogModel *>(model);
        Q_ASSERT(logModel);
        connect(logModel, &LogModel::added, [](LogEntry *entry) {
            if (entry->getType() != LogEntry::Information) {
                app->window->activateWindow();
            }
        });
        connect(loading, &QVariantAnimation::valueChanged, [=]() {
            if (logModel->getLoadingState()) {
                viewport()->update();
            }
        });
    }

    QListView::setModel(model);
}

QSize LogView::sizeHint() const
{
    return app->scale(240, 0);
}

void LogView::paintEvent(QPaintEvent *event)
{
    QListView::paintEvent(event);
    LogModel *logModel = qobject_cast<LogModel *>(model());
    if (logModel && logModel->getLoadingState()) {
        QPainter painter(viewport());
        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(loading->currentValue().value<QColor>()));
        const int height = 4;
        painter.drawRect(0, viewport()->height() - height, viewport()->width(), height);
    }
}
