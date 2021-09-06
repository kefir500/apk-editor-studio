#ifndef CODESIDEBAR_H
#define CODESIDEBAR_H

#include <QWidget>

class CodeSideBar : public QWidget
{
    Q_OBJECT
    friend class CodeEditor;

public:
    CodeSideBar(CodeEditor *parent);
    QSize sizeHint() const override;

protected:
    void changeEvent(QEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    void highlightCurrentLine();
    void updateSidebarWidth(int blocks);
    void updateSidebarGeometry();

    CodeEditor *editor;
    int sidebarWidth = 0;
    int sidebarPadding = 7;
    int currentLineNumber;
};

#endif // CODESIDEBAR_H
