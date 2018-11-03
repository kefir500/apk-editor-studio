#ifndef BASEEDITOR_H
#define BASEEDITOR_H

#include <QWidget>
#include <QFileSystemWatcher>
#include <QIcon>

class BaseEditor : public QWidget
{
    Q_OBJECT

public:
    explicit BaseEditor(const QString &filename = QString(), const QPixmap &icon = QPixmap(), QWidget *parent = nullptr);

    virtual bool load() = 0;
    virtual bool save(const QString &as = QString()) = 0;
    bool saveAs();
    bool commit();
    bool replace();
    void explore();

    bool isModified() const;
    bool isEditable() const;

    const QString &getTitle() const;
    const QIcon &getIcon() const;

    static QStringList supportedFormats();

signals:
    void savedStateChanged(bool saved) const;
    void titleChanged(const QString &title) const;
    void iconChanged(const QIcon &icon) const;

protected:
    void setModified(bool value);

    QString filename;
    QString title;
    QIcon icon;

private:
    QFileSystemWatcher watcher;
    bool modified;
};

#endif // BASEEDITOR_H
