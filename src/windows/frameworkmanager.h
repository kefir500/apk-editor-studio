#ifndef FRAMEWORKMANAGER_H
#define FRAMEWORKMANAGER_H

#include <QDialog>
#include <QFileSystemModel>

class QListView;
class QRubberBand;

class FrameworkManager : public QDialog
{
    Q_OBJECT

public:
    explicit FrameworkManager(QWidget *parent = nullptr);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void installFramework(const QString &path);
    void removeFramework(const QString &name);

    QFileSystemModel filesystemModel;
    QListView *frameworkList;
    QRubberBand *rubberBand;
};

#endif // FRAMEWORKMANAGER_H
