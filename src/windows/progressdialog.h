#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QDialog>

class ElidedLabel;
class QDialogButtonBox;
class QLabel;
class QProgressBar;

class ProgressDialog : public QDialog
{
    Q_OBJECT

public:
    ProgressDialog(QWidget *parent = nullptr);

    void setProgressMinimum(int minimum);
    void setProgressMaximum(int maximum);
    void setProgressValue(int value);

    void setPrimaryText(const QString &text);
    void setSecondaryText(const QString &text);
    void setCancelEnabled(bool value);

private:
    QProgressBar *progressBar;
    QLabel *primaryLabel;
    ElidedLabel *secondaryLabel;
    QDialogButtonBox *buttons;
};

#endif // PROGRESSDIALOG_H
