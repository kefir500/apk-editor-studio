#ifndef BASEEDITABLESHEET_H
#define BASEEDITABLESHEET_H

#include "sheets/basesheet.h"
#include <QAction>

class BaseEditableSheet : public BaseSheet
{
    Q_OBJECT

public:
    BaseEditableSheet(QWidget *parent = nullptr);
    virtual bool save(const QString &as = QString()) = 0;

    bool finalize() override;
    bool isModified() const;

signals:
    void saved();
    void modifiedStateChanged(bool modified);

protected:
    void setModified(bool value);
    void changeEvent(QEvent *event) override;

    QAction *actionSave;

private:
    void retranslate();

    bool modified;
};

#endif // BASEEDITABLESHEET_H
