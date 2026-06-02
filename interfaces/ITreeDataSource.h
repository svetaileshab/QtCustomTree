#pragma once

#include <QString>
#include <QList>
#include <memory>
#include "ITreeNodeData.h"

class ITreeDataSource {
public:
    virtual ~ITreeDataSource() = default;
    
    virtual QList<QString> getRootNodes() const = 0;
    virtual QList<QString> getChildren(const QString& nodeId) const = 0;
    virtual std::shared_ptr<ITreeNodeData> getItemData(const QString& nodeId) const = 0;
};
