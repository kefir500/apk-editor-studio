#include "widgets/logdelegate.h"
#include "base/application.h"
#include <QPainter>

LogDelegate::LogDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
    spinnerAngle = 0;
    iconExpand = app->icons.get("more.png");
    spinnerTimer.setInterval(25);
    connect(&spinnerTimer, &QTimer::timeout, [=]() {
        spinnerAngle = (spinnerAngle > 0) ? spinnerAngle - 160 : 5760;
        emit updated();
    });
}

void LogDelegate::setLoading(bool loading)
{
    loading
        ? spinnerTimer.start()
        : spinnerTimer.stop();
}

void LogDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);
    painter->setPen(QPen(QPalette().windowText(), 1.5));
    painter->setRenderHint(QPainter::Antialiasing);

    const int h = option.rect.height() - 8;
    const int w = h;
    const int x = option.rect.right() - w - 2;
    const int y = option.rect.center().y() - h / 2;

    const auto model = static_cast<const LogModel *>(index.model());
    const bool loading = (index.row() == model->rowCount() - 1) ? model->getLoadingState() : false;
    const QString message = index.sibling(index.row(), LogModel::LogDescriptive).data().toString();

    if (loading) {
        painter->drawArc(x, y, w, h, spinnerAngle, 12 * 360);
    } else if (!message.isEmpty()) {
        iconExpand.paint(painter, x, y, w, h);
    }
}
