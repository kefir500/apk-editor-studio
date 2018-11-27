#ifndef EDITOR_H
#define EDITOR_H

#include "editors/viewer.h"
#include <QAction>

class Editor : public Viewer
{
    Q_OBJECT

public:
    Editor(QWidget *parent = nullptr);
    virtual bool save(const QString &as = QString()) = 0;

    bool finalize() Q_DECL_OVERRIDE;
    bool isModified() const;

signals:
    void saved() const;
    void savedStateChanged(bool saved) const;

protected:
    void setModified(bool value);
    void changeEvent(QEvent *event) Q_DECL_OVERRIDE;

    QAction *actionSave;

private:
    void retranslate();

    bool modified;

};

#endif // EDITOR_H
