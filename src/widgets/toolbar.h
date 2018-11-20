#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QToolBar>
#include <QMap>

class Toolbar : public QToolBar
{
public:
    explicit Toolbar(QWidget *parent = nullptr);

    void reinitialize();

    static QMap<QString, QAction *> all();
    static void addToPool(const QString &identifier, QAction *action);

private:
    static QMap<QString, QAction *> pool;
};

#endif // TOOLBAR_H
