#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

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

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    void addPage(const QString &title, QLayout *layout, bool stretch = true);
    void load();
    void save();

private:
    QListWidgetItem *createToolbarSeparatorItem() const;
    QListWidgetItem *createToolbarSpacerItem() const;

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

#endif // SETTINGSDIALOG_H
