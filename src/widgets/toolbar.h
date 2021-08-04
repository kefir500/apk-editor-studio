#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QToolBar>
#include <QMap>

class Toolbar : public QToolBar
{
    Q_OBJECT

public:
    explicit Toolbar(QWidget *parent = nullptr);

    static QString getIdentifier(const QAction *action);

    const QList<QAction *> &getCurrentActions() const;
    const QMap<QString, QAction *> &getAvailableActions() const;

    void addActionToPool(const QString &getIdentifier, QAction *action);
    void initialize(const QStringList &actions);

signals:
    void updated(const QStringList &actions);

private:
    static void setIdentifier(QAction *action, const QString &getIdentifier);

    QAction *addSeparator();
    QAction *addSpacer();

    void changeEvent(QEvent *event) override;

    QList<QAction *> currentActions;
    QMap<QString, QAction *> availableActions;
    QAction *actionCustomize;
};

#endif // TOOLBAR_H
