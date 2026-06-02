#pragma once

#include "ITreeDataSource.h"
#include "ITreeNodeData.h"
#include <QMap>
#include <QRandomGenerator>

class FakeGeoNodeData : public ITreeNodeData
{
public:
    FakeGeoNodeData(QString id, QString name, QString status = "normal", bool onPlan = false)
        : m_id(id), m_name(name), m_status(status), m_onPlan(onPlan) {}

    QString id() const override { return m_id; }
    QString name() const override { return m_name; }
    QString status() const override { return m_status; }
    bool isOnPlan() const override { return m_onPlan; }
    QString tooltip() const override {
        QString info = QString("ID: %1\nName: %2\nStatus: %3").arg(m_id, m_name, m_status);
        if (m_status == "open") info += "\n✅ Open";
        else if (m_status == "low traffic") info += "\n🟡 Low traffic";
        else if (m_status == "closed") info += "\n🔴 Closed";
        else if (m_status == "restoration") info += "\n⚠️ Under restoration";
        else if (m_status == "abandoned") info += "\n👻 Abandoned";
        return info;
    }

private:
    QString m_id, m_name, m_status;
    bool m_onPlan;
};

class FakeGeoDataSource : public ITreeDataSource
{
public:
    FakeGeoDataSource()
    {
        createCountry("ru", "🇷🇺 Russia");
        createCountry("us", "🇺🇸 USA");
        createCountry("de", "🇩🇪 Germany");
    }

    QList<QString> getRootNodes() const override { return {"ru", "us", "de"}; }
    QList<QString> getChildren(const QString &nodeId) const override { return m_children.value(nodeId); }
    std::shared_ptr<ITreeNodeData> getItemData(const QString &nodeId) const override { return m_data.value(nodeId); }

private:
    QMap<QString, std::shared_ptr<ITreeNodeData>> m_data;
    QMap<QString, QList<QString>> m_children;

    void createCountry(const QString &id, const QString &name)
    {
        m_data[id] = std::make_shared<FakeGeoNodeData>(id, name);

        QMap<QString, QStringList> cities;
        cities["Moscow"] = QStringList() << "Kremlin" << "Red Square" << "Bolshoi Theatre" << "VDNKh" << "GUM";
        cities["Saint Petersburg"] = QStringList() << "Hermitage" << "Peterhof" << "Isaac Cathedral" << "Nevsky Prospekt";
        cities["Kazan"] = QStringList() << "Kazan Kremlin" << "Bauman Street" << "Qol Sharif Mosque";

        if (id == "us") {
            cities.clear();
            cities["New York"] = QStringList() << "Statue of Liberty" << "Times Square" << "Central Park" << "Empire State Building" << "Brooklyn Bridge";
            cities["San Francisco"] = QStringList() << "Golden Gate Bridge" << "Alcatraz" << "Fisherman's Wharf" << "Cable Cars";
            cities["Chicago"] = QStringList() << "Cloud Gate" << "Navy Pier" << "Millennium Park";
        } else if (id == "de") {
            cities.clear();
            cities["Berlin"] = QStringList() << "Brandenburg Gate" << "Reichstag" << "Berlin Wall" << "TV Tower" << "Museum Island";
            cities["Munich"] = QStringList() << "Marienplatz" << "BMW Museum" << "English Garden";
            cities["Hamburg"] = QStringList() << "Port of Hamburg" << "Elbphilharmonie" << "Miniatur Wunderland";
        }

        QStringList cityList;
        QStringList statuses = QStringList() << "open" << "low traffic" << "closed" << "restoration" << "abandoned";

        for (auto it = cities.begin(); it != cities.end(); ++it) {
            QString cityId = QString("%1/%2").arg(id).arg(it.key());
            m_data[cityId] = std::make_shared<FakeGeoNodeData>(cityId, "🏙️ " + it.key());
            QStringList places;
            for (int i = 0; i < it.value().size(); ++i) {
                QString placeId = QString("%1/%2").arg(cityId).arg(it.value()[i]);
                QString status = statuses[QRandomGenerator::global()->bounded(statuses.size())];
                m_data[placeId] = std::make_shared<FakeGeoNodeData>(placeId, "📍 " + it.value()[i], status, true);
                places.append(placeId);
            }
            m_children[cityId] = places;
            cityList.append(cityId);
        }
        m_children[id] = cityList;
    }
};
