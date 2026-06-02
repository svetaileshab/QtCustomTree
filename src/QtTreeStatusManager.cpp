#include "QtCustomTree/QtTreeStatusManager.h"
#include "ITreeNodeData.h"

#include <QAbstractItemModel>
#include <QColor>

QtTreeStatusManager::QtTreeStatusManager(QTreeView *treeView, QAbstractItemModel *model, QObject *parent)
    : QObject(parent), m_treeView(treeView), m_model(model) {}

QtTreeStatusManager::~QtTreeStatusManager() {}

void QtTreeStatusManager::updateItemStatus(const QString &itemId, const QString &status)
{
    Q_UNUSED(itemId)
    Q_UNUSED(status)
    // Статусы обновляются через модель
}

void QtTreeStatusManager::updateAllStatuses(std::function<QString(const QString&)> statusProvider)
{
    Q_UNUSED(statusProvider)
    // Массовое обновление статусов
}

void QtTreeStatusManager::updateItemTooltip(const QModelIndex &index, std::shared_ptr<ITreeNodeData> data)
{
    if (!index.isValid() || !data) return;
    
    QString tooltip = QString("ID: %1\nName: %2\nStatus: %3")
                          .arg(data->id(), data->name(), data->status());
    
    if (data->isOnPlan()) {
        tooltip += QString("\n📍 On plan: X=%1, Y=%2")
                       .arg(data->planPosition().x())
                       .arg(data->planPosition().y());
    } else {
        tooltip += "\n❌ Not on plan";
    }
    
    m_model->setData(index, tooltip, Qt::ToolTipRole);
}

QColor QtTreeStatusManager::statusToColor(const QString &status)
{
    if (status == "alarm")    return QColor("#ef4444");
    if (status == "warning")  return QColor("#f59e0b");
    if (status == "fault")    return QColor("#f97316");
    if (status == "disabled") return QColor("#6b7280");
    return QColor("#10b981");
}
