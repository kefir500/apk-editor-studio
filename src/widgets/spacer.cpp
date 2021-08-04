#include "widgets/spacer.h"

Spacer::Spacer(QWidget *parent) : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}
