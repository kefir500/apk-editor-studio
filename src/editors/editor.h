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

    bool finalize() override;
    bool isModified() const;

signals:
    void saved() const;
    void modifiedStateChanged(bool modified) const;

protected:
    void setModified(bool value);
    void changeEvent(QEvent *event) override;

    QAction *actionSave;

private:
    void retranslate();

    bool modified;
};

#endif // EDITOR_H
