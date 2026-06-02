#pragma once

#include <QTreeView>

class QtDarkTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit QtDarkTreeView(QWidget *parent = nullptr);

protected:
    void drawBranches(QPainter *painter, const QRect &rect,
                      const QModelIndex &index) const override;
};
