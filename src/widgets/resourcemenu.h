#ifndef RESOURCEMENU_H
#define RESOURCEMENU_H

#include <QMenu>

class ResourceMenu : public QMenu
{
    Q_OBJECT

public:
    ResourceMenu(QWidget *parent = nullptr);

    QAction *getEditAction();
    QAction *getReplaceAction();
    QAction *getSaveAction();
    QAction *getSaveAsAction();
    QAction *getExploreAction();

    void setEnabled(bool enabled);

    void retranslate();

signals:
    void editClicked() const;
    void replaceClicked() const;
    void saveClicked() const;
    void saveAsClicked() const;
    void exploreClicked() const;

protected:
    void changeEvent(QEvent *event) Q_DECL_OVERRIDE;

private:
    QAction *actionEdit;
    QAction *actionReplace;
    QAction *actionSave;
    QAction *actionSaveAs;
    QAction *actionExplore;
    QAction *separatorReplace;
    QAction *separatorEdit;
    QAction *separatorSave;
};

#endif // RESOURCEMENU_H
