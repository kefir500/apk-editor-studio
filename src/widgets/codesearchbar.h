#ifndef CODESEARCHBAR_H
#define CODESEARCHBAR_H

#include <QWidget>

class CodeEditor;
class QLabel;
class QLineEdit;
class QToolButton;

class CodeSearchBar : public QWidget
{
    Q_OBJECT

public:
    CodeSearchBar(CodeEditor *parent = nullptr);

    void setResults(int totalResults, int currentResult = 0);
    void setFindText(const QString &text);
    void focusFindBar();
    void focusReplaceBar();
    void showReplaceBar();
    void hideReplaceBar();

protected:
    void changeEvent(QEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    void updateResults();
    void retranslate();

    int totalResults = 0;
    int currentResult = 0;

    CodeEditor *editor;

    QWidget *findBar;
    QLabel *findLabel;
    QLineEdit *findInput;
    QLabel *resultsLabel;
    QToolButton *btnOpenReplaceBar;
    QToolButton *btnClose;

    QWidget *replaceBar;
    QLabel *replaceLabel;
    QLineEdit *replaceInput;
    QToolButton *btnReplace;
    QToolButton *btnReplaceAll;
    QToolButton *btnCloseReplaceBar;
};

#endif // CODESEARCHBAR_H
