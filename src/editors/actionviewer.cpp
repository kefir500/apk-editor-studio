#include "editors/actionviewer.h"
#include "base/application.h"
#include "base/utils.h"
#include "widgets/gradientwidget.h"
#include <QBoxLayout>
#include <QPainter>
#include <QPaintEvent>

#ifdef Q_OS_LINUX
    #include <QFontDatabase>
#endif

ActionViewer::ActionViewer(QWidget *parent) : Viewer(parent)
{
    background = new GradientWidget(this);
    background->resize(size());

    label = new ElidedLabel(this);
#if defined(Q_OS_WIN)
    label->setFont(QFont("Segoe UI", 14));
#elif defined(Q_OS_OSX)
    label->setFont(QFont(".SF NS Text", 18));
#elif defined(Q_OS_LINUX)
    QFont font = QFontDatabase::systemFont(QFontDatabase::TitleFont);
    font.setPointSize(14);
    label->setFont(font);
#endif
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("margin-bottom: 6px;");

    layout = new QVBoxLayout(this);
    layout->setMargin(64);
    layout->setSpacing(6);
    layout->addStretch();
    layout->addWidget(label);
    layout->addStretch();
}

void ActionViewer::setTitle(const QString &title)
{
    label->setText(title);
}

void ActionViewer::addWidget(QWidget *widget)
{
    layout->insertWidget(layout->count() - 1, widget);
}

QPushButton *ActionViewer::addButton(const QString &title)
{
    QPushButton *button = new QPushButton(this);
    button->setText(title);
    button->setMinimumHeight(Utils::scale(34));
    if (!Utils::isDarkTheme()) {
        const QString btnStyle(
            "QPushButton { background: rgb(225, 240, 190); border: none; padding: 0 20px } "
            "QPushButton:hover { background: rgb(215, 230, 180); }"
            "QPushButton:pressed { background: rgb(205, 220, 170); }"
        );
        button->setStyleSheet(btnStyle);
    }
    addWidget(button);
    return button;
}

void ActionViewer::resizeEvent(QResizeEvent *event)
{
    background->resize(event->size());
}
