#pragma once

#include <QString>
#include <QPointF>
#include <memory>

class ITreeNodeData {
public:
    virtual ~ITreeNodeData() = default;
    
    virtual QString id() const = 0;
    virtual QString name() const = 0;
    virtual QString status() const { return "normal"; }
    virtual bool isOnPlan() const { return false; }
    virtual QPointF planPosition() const { return {}; }
    virtual QString tooltip() const { return name(); }
};
