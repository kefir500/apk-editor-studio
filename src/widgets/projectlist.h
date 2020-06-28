#ifndef PROJECTLIST_H
#define PROJECTLIST_H

#include <QComboBox>

class Project;

class ProjectList : public QComboBox
{
public:
    explicit ProjectList(QWidget *parent = nullptr);

    void setModel(QAbstractItemModel *model);
    bool setCurrentProject(Project *project);

private:
    void onRowsInserted(const QModelIndex &parent, int first, int last);
};

#endif // PROJECTLIST_H
