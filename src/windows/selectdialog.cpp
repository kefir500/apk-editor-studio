#include "windows/selectdialog.h"
#include <QLabel>
#include <QBoxLayout>
#include <QDialogButtonBox>

SelectDialog::SelectDialog(const QString &title, const QString &text, QWidget *parent) : QDialog(parent)
{
    setWindowTitle(title);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    auto layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel(text, this));

    combo = new QComboBox(this);
    layout->addWidget(combo);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, this, &SelectDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &SelectDialog::reject);
    layout->addWidget(buttons);
}

int SelectDialog::select(const QStringList &items, int selected, QWidget *parent)
{
    return select(QString(), QString(), items, selected, parent);
}

int SelectDialog::select(const QString &text, const QStringList &items, int selected, QWidget *parent)
{
    return select(QString(), text, items, selected, parent);
}

int SelectDialog::select(const QString &title, const QString &text, const QStringList &items, int selected, QWidget *parent)
{
    auto widget = new SelectDialog(title, text, parent);
    widget->addItems(items);
    widget->setCurrentIndex(selected);
    if (widget->exec() == QDialog::Accepted) {
        return widget->getCurrentIndex();
    }
    return -1;
}

void SelectDialog::addItem(const QString &item)
{
    combo->addItem(item);
}

void SelectDialog::addItems(const QStringList &items)
{
    combo->addItems(items);
}

void SelectDialog::clearItems()
{
    combo->clear();
}

int SelectDialog::getCurrentIndex() const
{
    return combo->currentIndex();
}

QString SelectDialog::getCurrentText() const
{
    return combo->currentText();
}

void SelectDialog::setCurrentIndex(int index)
{
    combo->setCurrentIndex(index);
}
