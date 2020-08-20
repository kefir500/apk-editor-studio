#include "widgets/readonlycheckbox.h"

ReadOnlyCheckBox::ReadOnlyCheckBox(QWidget *parent) : QCheckBox(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setFocusPolicy(Qt::NoFocus);
}

ReadOnlyCheckBox::ReadOnlyCheckBox(const QString &text, QWidget *parent) : ReadOnlyCheckBox(parent)
{
    setText(text);
}
