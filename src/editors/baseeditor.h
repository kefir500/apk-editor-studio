#ifndef BASEEDITOR_H
#define BASEEDITOR_H

#include <QWidget>
#include <QIcon>

class BaseEditor : public QWidget
{
    Q_OBJECT

public:
    explicit BaseEditor(QWidget *parent = nullptr);

    virtual bool save(const QString &as = QString()) = 0;
    bool commit();

    bool isModified() const;

    const QString &getTitle() const;
    const QIcon &getIcon() const;

signals:
    void savedStateChanged(bool saved) const;
    void titleChanged(const QString &title) const;
    void iconChanged(const QIcon &icon) const;

protected:
    void setModified(bool value);

    QString title;
    QIcon icon;

private:
    bool modified;
};

#endif // BASEEDITOR_H
