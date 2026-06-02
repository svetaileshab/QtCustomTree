#pragma once

#include "ITreeDataSource.h"
#include "ITreeNodeData.h"
#include <QMap>
#include <QRandomGenerator>

// Тестовые данные для демонстрации
class FakeNodeData : public ITreeNodeData
{
public:
    FakeNodeData(QString id, QString name, QString status = "normal", bool onPlan = false)
        : m_id(id), m_name(name), m_status(status), m_onPlan(onPlan) {}

    QString id() const override { return m_id; }
    QString name() const override { return m_name; }
    QString status() const override { return m_status; }
    bool isOnPlan() const override { return m_onPlan; }
    QString tooltip() const override {
        return QString("ID: %1\nName: %2\nStatus: %3%4")
            .arg(m_id, m_name, m_status,
                 m_onPlan ? "\n📍 On plan" : "\n❌ Not on plan");
    }

private:
    QString m_id, m_name, m_status;
    bool m_onPlan;
};

class FakeDataSource : public ITreeDataSource
{
public:
    FakeDataSource()
    {
        // Создаём тестовую иерархию
        createBuilding("building1", "🏢 Главное здание");
        createBuilding("building2", "🏭 Производственный цех");
    }

    QList<QString> getRootNodes() const override
    {
        return {"building1", "building2"};
    }

    QList<QString> getChildren(const QString &nodeId) const override
    {
        return m_children.value(nodeId);
    }

    std::shared_ptr<ITreeNodeData> getItemData(const QString &nodeId) const override
    {
        return m_data.value(nodeId);
    }

private:
    QMap<QString, std::shared_ptr<ITreeNodeData>> m_data;
    QMap<QString, QList<QString>> m_children;

    void createBuilding(const QString &id, const QString &name)
    {
        m_data[id] = std::make_shared<FakeNodeData>(id, name);

        // Этажи
        QStringList floors;
        for (int f = 1; f <= 3; ++f) {
            QString floorId = QString("%1_floor%2").arg(id).arg(f);
            m_data[floorId] = std::make_shared<FakeNodeData>(floorId, QString("📁 Этаж %1").arg(f));
            
            QStringList devices;
            QStringList types = {"🔥 Датчик дыма", "🌡️ Датчик температуры", "🔊 Оповещатель"};
            QStringList statuses = {"normal", "warning", "alarm", "disabled"};
            
            for (int d = 1; d <= 4; ++d) {
                QString devId = QString("%1_dev%2").arg(floorId).arg(d);
                QString status = statuses[QRandomGenerator::global()->bounded(statuses.size())];
                bool onPlan = QRandomGenerator::global()->bounded(2) == 1;
                
                m_data[devId] = std::make_shared<FakeNodeData>(
                    devId,
                    QString("%1 %2").arg(types[d % types.size()]).arg(d),
                    status,
                    onPlan
                );
                devices.append(devId);
            }
            
            m_children[floorId] = devices;
            floors.append(floorId);
        }
        
        m_children[id] = floors;
    }
};
