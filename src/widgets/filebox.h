#ifndef FILEBOX_H
#define FILEBOX_H

#include <QLineEdit>
#include <QToolButton>

class FileBox : public QWidget
{
    Q_OBJECT

public:
    FileBox(const QString &currentPath = QString(), const QString &defaultPath = QString(), bool isDirectory = false, QWidget *parent = 0);

    QString getCurrentPath() const;

    void setCurrentPath(const QString &path);
    void setDefaultPath(const QString &path);

    void retranslate();

signals:
    void currentPathChanged(const QString &path);

protected:
    void changeEvent(QEvent *event) Q_DECL_OVERRIDE;
    void dragEnterEvent(QDragEnterEvent *event) Q_DECL_OVERRIDE;
    void dragLeaveEvent(QDragLeaveEvent *event) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent *event) Q_DECL_OVERRIDE;

private:
    void openPath();
    void checkPath();
    void resetPath();

    bool isDirectory;
    QString defaultPath;

    QLineEdit *input;
    QToolButton *btnReset;
    QToolButton *btnOpen;
};

#endif // FILEBOX_H
