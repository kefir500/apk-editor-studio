#ifndef BASEEDITOR_H
#define BASEEDITOR_H

#include <QWidget>
#include <QIcon>

class BaseEditor : public QWidget
{
    Q_OBJECT

public:
    explicit BaseEditor(QWidget *parent = nullptr) : QWidget(parent) {}

    const QString &getTitle() const;
    const QIcon &getIcon() const;

signals:
    void titleChanged(const QString &title) const;
    void iconChanged(const QIcon &icon) const;

protected:
    QString title;
    QIcon icon;
};

#endif // BASEEDITOR_H
