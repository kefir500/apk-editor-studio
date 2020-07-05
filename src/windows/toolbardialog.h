#ifndef TOOLBARDIALOG_H
#define TOOLBARDIALOG_H

#include <QDialog>

class QListWidgetItem;
class Toolbar;

class ToolbarDialog : public QDialog
{
    Q_OBJECT

public:
    ToolbarDialog(const Toolbar &toolbar, QWidget *parent = nullptr);

signals:
    void actionsUpdated(const QStringList &actions) const;

private:
    QListWidgetItem *createSeparatorItem() const;
    QListWidgetItem *createSpacerItem() const;
};

#endif // TOOLBARDIALOG_H
