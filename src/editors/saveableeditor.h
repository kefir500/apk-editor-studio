#ifndef SAVEABLEEDITOR_H
#define SAVEABLEEDITOR_H

#include "editors/baseeditor.h"
#include <QAction>

class SaveableEditor : public BaseEditor
{
    Q_OBJECT

public:
    SaveableEditor(QWidget *parent = nullptr);
    virtual bool save(const QString &as = QString()) = 0;

    bool commit();
    bool isModified() const;

signals:
    void savedStateChanged(bool saved) const;

protected:
    void setModified(bool value);
    void changeEvent(QEvent *event) Q_DECL_OVERRIDE;

    QAction *actionSave;

private:
    void retranslate();

    bool modified;

};

#endif // SAVEABLEEDITOR_H
