#ifndef FILEBOX_H
#define FILEBOX_H

#include <QLineEdit>
#include <QToolButton>

class FileBox : public QWidget
{
    Q_OBJECT

public:
    FileBox(const QString &currentPath = QString(), const QString &defaultPath = QString(), bool isDirectory = false, QWidget *parent = nullptr);

    QString getCurrentPath() const;

    void setCurrentPath(const QString &path);
    void setDefaultPath(const QString &path);

    void retranslate();

signals:
    void currentPathChanged(const QString &path);

protected:
    void changeEvent(QEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

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
