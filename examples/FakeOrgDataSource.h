#pragma once

#include "ITreeDataSource.h"
#include "ITreeNodeData.h"
#include <QMap>
#include <QRandomGenerator>

class FakeOrgNodeData : public ITreeNodeData
{
public:
    FakeOrgNodeData(QString id, QString name, QString status = "normal", bool onPlan = false)
        : m_id(id), m_name(name), m_status(status), m_onPlan(onPlan) {}

    QString id() const override { return m_id; }
    QString name() const override { return m_name; }
    QString status() const override { return m_status; }
    bool isOnPlan() const override { return m_onPlan; }
    QString tooltip() const override {
        QString info = QString("ID: %1\nName: %2\nStatus: %3").arg(m_id, m_name, m_status);
        if (m_status == "online") info += "\n🟢 Available";
        else if (m_status == "away") info += "\n🟡 Away";
        else if (m_status == "offline") info += "\n🔴 Offline";
        else if (m_status == "busy") info += "\n🟠 Do Not Disturb";
        return info;
    }

private:
    QString m_id, m_name, m_status;
    bool m_onPlan;
};

class FakeOrgDataSource : public ITreeDataSource
{
public:
    FakeOrgDataSource()
    {
        createDepartment("dept_eng", "Engineering");
        createDepartment("dept_sales", "Sales");
        createDepartment("dept_hr", "Human Resources");
    }

    QList<QString> getRootNodes() const override
    {
        return {"dept_eng", "dept_sales", "dept_hr"};
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

    void createDepartment(const QString &id, const QString &name)
    {
        m_data[id] = std::make_shared<FakeOrgNodeData>(id, "🏢 " + name);

        QMap<QString, QStringList> teams;
        teams["Frontend Team"] = QStringList() << "Alice Johnson" << "Bob Smith" << "Charlie Brown" << "Diana Ross" << "Eve Wilson";
        teams["Backend Team"] = QStringList() << "Frank Miller" << "Grace Lee" << "Henry Davis" << "Ivy Chen" << "Jack White";
        teams["QA Team"] = QStringList() << "Kate Taylor" << "Leo Martinez" << "Mia Anderson";
        
        if (id == "dept_sales") {
            teams.clear();
            teams["Enterprise"] = QStringList() << "Nathan Scott" << "Olivia Green" << "Peter Black";
            teams["SMB"] = QStringList() << "Quinn Roberts" << "Rachel Adams" << "Sam Turner";
            teams["Support"] = QStringList() << "Tom Harris" << "Uma Patel" << "Victor Kim" << "Wendy Zhou";
        } else if (id == "dept_hr") {
            teams.clear();
            teams["Recruitment"] = QStringList() << "Xavier Brown" << "Yuki Tanaka" << "Zoe Garcia";
            teams["Training"] = QStringList() << "Adam Wright" << "Beth Lopez" << "Carlos Ruiz";
        }

        QStringList teamList;
        QStringList statuses = QStringList() << "online" << "away" << "offline" << "busy";
        
        for (auto it = teams.begin(); it != teams.end(); ++it) {
            QString teamId = QString("%1/%2").arg(id).arg(it.key());
            m_data[teamId] = std::make_shared<FakeOrgNodeData>(teamId, "👥 " + it.key());
            
            QStringList members;
            for (const QString &person : it.value()) {
                QString personId = QString("%1/%2").arg(teamId).arg(person);
                QString status = statuses[QRandomGenerator::global()->bounded(statuses.size())];
                m_data[personId] = std::make_shared<FakeOrgNodeData>(personId, "👤 " + person, status);
                members.append(personId);
            }
            m_children[teamId] = members;
            teamList.append(teamId);
        }
        m_children[id] = teamList;
    }
};
