#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>

class FileBox;
class QGroupBox;
class QCheckBox;
class QComboBox;
class QListWidget;
class QSpinBox;
class QStackedWidget;
class QVBoxLayout;

class OptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionsDialog(QWidget *parent = nullptr);
    void addPage(const QString &title, QLayout *layout, bool stretch = true);
    void load();
    void save();

protected:
    void changeEvent(QEvent *event) override;

private:
    void initialize();

    QWidget *widget;
    QVBoxLayout *layout;

    QListWidget *pageList;
    QStackedWidget *pageStack;

    // General

    QCheckBox *checkboxSingleInstance;
    QCheckBox *checkboxUpdates;
    QSpinBox *spinboxRecent;
#ifdef Q_OS_WIN
    QGroupBox *groupAssociate;
    QCheckBox *checkboxExplorerOpen;
    QCheckBox *checkboxExplorerInstall;
    QCheckBox *checkboxExplorerOptimize;
    QCheckBox *checkboxExplorerSign;
#endif

    // Appearance

    QComboBox *comboLanguages;
    QComboBox *comboThemes;

    // Java

    FileBox *fileboxJava;
    QSpinBox *spinboxMinHeapSize;
    QSpinBox *spinboxMaxHeapSize;

    // Apktool

    FileBox *fileboxApktool;
    FileBox *fileboxOutput;
    FileBox *fileboxFrameworks;
    QCheckBox *checkboxAapt2;
    QCheckBox *checkboxDebuggable;
    QCheckBox *checkboxSources;
    QCheckBox *checkboxOnlyMainClasses;
    QCheckBox *checkboxBrokenResources;

    // Apksigner

    QCheckBox *checkboxSign;
    FileBox *fileboxApksigner;

    // Zipalign

    QCheckBox *checkboxZipalign;
    FileBox *fileboxZipalign;

    // ADB

    FileBox *fileboxAdb;
};

#endif // OPTIONSDIALOG_H
