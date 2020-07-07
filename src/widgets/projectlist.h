#ifndef PROJECTLIST_H
#define PROJECTLIST_H

#include <QComboBox>

class Project;

class ProjectList : public QComboBox
{
public:
    explicit ProjectList(QWidget *parent = nullptr);
    bool setCurrentProject(Project *project);
};

#endif // PROJECTLIST_H
