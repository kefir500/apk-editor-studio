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

    QCheckBox *checkboxSingleInstance;
    QCheckBox *checkboxUpdates;
    QSpinBox *spinboxRecent;
    QComboBox *comboLanguages;
#ifdef Q_OS_WIN
    QGroupBox *groupAssociate;
    QCheckBox *checkboxExplorerOpen;
    QCheckBox *checkboxExplorerInstall;
    QCheckBox *checkboxExplorerOptimize;
    QCheckBox *checkboxExplorerSign;
#endif

    FileBox *fileboxJava;
    QSpinBox *spinboxMinHeapSize;
    QSpinBox *spinboxMaxHeapSize;

    FileBox *fileboxApktool;
    FileBox *fileboxOutput;
    FileBox *fileboxFrameworks;
    QCheckBox *checkboxAapt;
    QCheckBox *checkboxDebuggable;
    QCheckBox *checkboxSources;
    QCheckBox *checkboxBrokenResources;

    QGroupBox *groupSign;
    QGroupBox *groupZipalign;
    FileBox *fileboxApksigner;

    FileBox *fileboxZipalign;

    FileBox *fileboxAdb;
};

#endif // OPTIONSDIALOG_H
