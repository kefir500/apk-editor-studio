#ifndef PROJECTLIST_H
#define PROJECTLIST_H

#include "apk/projectsmodel.h"
#include <QComboBox>

class ProjectList : public QComboBox
{
    Q_OBJECT

public:
    explicit ProjectList(QWidget *parent = nullptr);

    bool setCurrentProject(Project *project);

    ProjectsModel *model() const;
    void setModel(ProjectsModel *model);

signals:
    void currentProjectChanged(Project *project) const;
};

#endif // PROJECTLIST_H
