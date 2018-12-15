#ifndef PROJECTLIST_H
#define PROJECTLIST_H

#include "apk/projectitemsmodel.h"
#include <QComboBox>

class ProjectList : public QComboBox
{
    Q_OBJECT

public:
    explicit ProjectList(QWidget *parent = nullptr);

    bool setCurrentProject(Project *project);

    ProjectItemsModel *model() const;
    void setModel(ProjectItemsModel *model);

signals:
    void currentProjectChanged(Project *project) const;
};

#endif // PROJECTLIST_H
