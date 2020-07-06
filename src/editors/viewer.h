#ifndef VIEWER_H
#define VIEWER_H

#include <QWidget>
#include <QIcon>

class Viewer : public QWidget
{
    Q_OBJECT

public:
    explicit Viewer(QWidget *parent = nullptr) : QWidget(parent) {}

    const QString &getTitle() const;
    const QIcon &getIcon() const;

    virtual bool finalize();

signals:
    void titleChanged(const QString &title);
    void iconChanged(const QIcon &icon);

protected:
    QString title;
    QIcon icon;
};

#endif // VIEWER_H
