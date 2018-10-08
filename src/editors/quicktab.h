#ifndef QUICKTAB_H
#define QUICKTAB_H

#include "editors/baseeditor.h"
#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <functional>

class QuickTab : public BaseEditor
{
    Q_OBJECT

public:
    explicit QuickTab(QWidget *parent = nullptr);
    void setTitle(const QString &title);
    bool load() Q_DECL_OVERRIDE;
    bool save(const QString &as = QString()) Q_DECL_OVERRIDE;

protected:
    QPushButton *addButton(const QString &title = QString());
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    QVBoxLayout *layout;
    QLabel *label;
};

#endif // QUICKTAB_H
