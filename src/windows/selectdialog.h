#ifndef SELECTDIALOG_H
#define SELECTDIALOG_H

#include <QDialog>
#include <QComboBox>

class SelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectDialog(const QString &title, const QString &text, QWidget *parent = nullptr);

    static int select(const QStringList &items, int selected = 0, QWidget *parent = nullptr);
    static int select(const QString &text, const QStringList &items, int selected = 0, QWidget *parent = nullptr);
    static int select(const QString &title, const QString &text, const QStringList &items, int selected = 0, QWidget *parent = nullptr);

    void addItem(const QString &item);
    void addItems(const QStringList &items);
    void clearItems();

    int getCurrentIndex() const;
    QString getCurrentText() const;
    void setCurrentIndex(int index);

private:
    QComboBox *combo;
};

#endif // SELECTDIALOG_H
