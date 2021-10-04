#ifndef BASESHEET_H
#define BASESHEET_H

#include <QWidget>
#include <QIcon>

class BaseSheet : public QWidget
{
    Q_OBJECT

public:
    explicit BaseSheet(QWidget *parent = nullptr) : QWidget(parent) {}

    const QString &getTitle() const;
    const QIcon &getIcon() const;

    virtual bool finalize();

signals:
    void titleChanged(const QString &title);
    void iconChanged(const QIcon &icon);

protected:
    void setSheetTitle(const QString &title);
    void setSheetIcon(const QIcon &icon);
    void addSeparator();

private:
    QString title;
    QIcon icon;
};

#endif // BASESHEET_H
