#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include "widgets/filebox.h"
#include "widgets/poollistwidget.h"
#include <QDialog>
#include <QBoxLayout>
#include <QListWidget>
#include <QStackedWidget>
#include <QGroupBox>
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>

class OptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionsDialog(QWidget *parent = nullptr);
    void addPage(const QString &title, QLayout *layout, bool stretch = true);
    void load();
    void save();

signals:
    void loaded() const;
    void saved() const;

protected:
    void changeEvent(QEvent *event) Q_DECL_OVERRIDE;

private:
    void initialize();
    QListWidgetItem *createToolbarSeparatorItem() const;
    QListWidgetItem *createToolbarSpacerItem() const;

    QWidget *widget;
    QVBoxLayout *layout;

    QListWidget *pageList;
    QStackedWidget *pageStack;

    QCheckBox *checkboxUpdates;
    QPushButton *btnAssociate;
    QSpinBox *spinboxRecent;
    QComboBox *comboLanguages;

    FileBox *fileboxApktool;
    FileBox *fileboxOutput;
    FileBox *fileboxFrameworks;
    QCheckBox *checkboxSources;

    QGroupBox *groupSign;
    QGroupBox *groupZipalign;
    FileBox *fileboxApksigner;

    FileBox *fileboxZipalign;

    FileBox *fileboxAdb;

    QListWidget *listToolbarUsed;
    PoolListWidget *listToolbarUnused;
};

#endif // OPTIONSDIALOG_H
