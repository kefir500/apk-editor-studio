#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QToolBar>
#include <QMap>

class Toolbar : public QToolBar {

public:
    explicit Toolbar(QWidget *parent = 0);
    void reinitialize();
};

#endif // TOOLBAR_H
