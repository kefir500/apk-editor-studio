#ifndef PROJECTLIST_H
#define PROJECTLIST_H

#include <QComboBox>

class Project;

class ProjectList : public QComboBox
{
    Q_OBJECT

public:
    explicit ProjectList(QWidget *parent = nullptr);
    bool setCurrentProject(Project *project);
};

#endif // PROJECTLIST_H
