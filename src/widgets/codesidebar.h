#ifndef CODESIDEBAR_H
#define CODESIDEBAR_H

#include <QWidget>

class CodeEditor;

class CodeSideBar : public QWidget
{
    Q_OBJECT

public:
    CodeSideBar(CodeEditor *parent);

    void setCurrentLine(int line);
    void updateSidebarGeometry();

    QSize sizeHint() const override;

protected:
    void changeEvent(QEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    void updateSidebarWidth(int blocks);

    CodeEditor *editor;
    int sidebarWidth = 0;
    int sidebarPadding = 7;
    int currentLineNumber;
};

#endif // CODESIDEBAR_H
