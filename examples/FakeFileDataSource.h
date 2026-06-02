#pragma once

#include "ITreeDataSource.h"
#include "ITreeNodeData.h"
#include <QMap>
#include <QRandomGenerator>

class FakeFileNodeData : public ITreeNodeData
{
public:
    FakeFileNodeData(QString id, QString name, QString status = "normal", bool onPlan = false)
        : m_id(id), m_name(name), m_status(status), m_onPlan(onPlan) {}

    QString id() const override { return m_id; }
    QString name() const override { return m_name; }
    QString status() const override { return m_status; }
    bool isOnPlan() const override { return m_onPlan; }
    QString tooltip() const override {
        QString info = QString("📄 %1\nPath: %2\nStatus: %3").arg(m_name, m_id, m_status);
        if (m_status == "locked") info += "\n🔴 Locked by system";
        else if (m_status == "modified") info += "\n🟡 Modified recently";
        else if (m_status == "corrupted") info += "\n⚠️ File corrupted";
        else if (m_status == "hidden") info += "\n👻 Hidden file";
        else info += "\n✅ Accessible";
        return info;
    }

private:
    QString m_id, m_name, m_status;
    bool m_onPlan;
};

class FakeFileDataSource : public ITreeDataSource
{
public:
    FakeFileDataSource()
    {
        createDrive("C:", "C:");
        createDrive("D:", "D:");
    }

    QList<QString> getRootNodes() const override { return {"C:", "D:"}; }
    QList<QString> getChildren(const QString &nodeId) const override { return m_children.value(nodeId); }
    std::shared_ptr<ITreeNodeData> getItemData(const QString &nodeId) const override { return m_data.value(nodeId); }

private:
    QMap<QString, std::shared_ptr<ITreeNodeData>> m_data;
    QMap<QString, QList<QString>> m_children;

    void createDrive(const QString &id, const QString &name)
    {
        m_data[id] = std::make_shared<FakeFileNodeData>(id, name);

        QMap<QString, QList<QPair<QString, QString>>> structure;
        structure["Windows"] = {
            {"System32", "ntoskrnl.exe|kernel32.dll|user32.dll|gdi32.dll|shell32.dll|cmd.exe|regedit.exe|notepad.exe|calc.exe|mspaint.exe"},
            {"Fonts", "arial.ttf|times.ttf|cour.ttf|segoe.ttf|webdings.ttf|wingding.ttf"},
            {"Logs", "system.log|app.log|security.log|setup.log"},
            {"Temp", ""}
        };
        structure["Users"] = {
            {"Admin", "resume.pdf|budget.xlsx|photo.jpg|notes.txt|config.ini"},
            {"Guest", "readme.txt"}
        };
        structure["Program Files"] = {
            {"Qt", "Qt5Core.dll|Qt5Widgets.dll|Qt5Gui.dll|Qt5Network.dll"},
            {"CMake", "cmake.exe|cmake-gui.exe|cpack.exe|ctest.exe"},
            {"Git", "git.exe|git-bash.exe|git-gui.exe"}
        };

        QStringList statuses = {"accessible", "modified", "locked", "corrupted", "hidden"};
        QStringList rootFolders;

        for (auto it = structure.begin(); it != structure.end(); ++it) {
            QString folderId = QString("%1/%2").arg(id).arg(it.key());
            m_data[folderId] = std::make_shared<FakeFileNodeData>(folderId, "📁 " + it.key());
            QStringList subFolders;
            for (const auto &sub : it.value()) {
                QString subId = QString("%1/%2").arg(folderId).arg(sub.first);
                m_data[subId] = std::make_shared<FakeFileNodeData>(subId, "📁 " + sub.first);
                QStringList files;
                if (!sub.second.isEmpty()) {
                    QStringList fileNames = sub.second.split("|");
                    for (int i = 0; i < fileNames.size(); ++i) {
                        QString fileId = QString("%1/%2").arg(subId).arg(fileNames[i]);
                        QString status = statuses[QRandomGenerator::global()->bounded(statuses.size())];
                        m_data[fileId] = std::make_shared<FakeFileNodeData>(fileId, "📄 " + fileNames[i], status);
                        files.append(fileId);
                    }
                }
                m_children[subId] = files;
                subFolders.append(subId);
            }
            m_children[folderId] = subFolders;
            rootFolders.append(folderId);
        }
        m_children[id] = rootFolders;
    }
};
