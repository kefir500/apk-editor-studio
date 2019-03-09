#ifndef FILEBOX_H
#define FILEBOX_H

#include <QLineEdit>
#include <QToolButton>
#include <QRubberBand>

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
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    class LineEditWithoutDrops : public QLineEdit
    {
    public:
        LineEditWithoutDrops(QWidget *parent) : QLineEdit(parent) {}
    private:
        void dragEnterEvent(QDragEnterEvent *) override {}
        void dragLeaveEvent(QDragLeaveEvent *) override {}
        void dropEvent(QDropEvent *) override {}
    };

    void openPath();
    void checkPath();
    void resetPath();

    bool isDirectory;
    QString defaultPath;

    LineEditWithoutDrops *input;
    QToolButton *btnReset;
    QToolButton *btnOpen;
    QRubberBand *rubberBand;
};

#endif // FILEBOX_H
