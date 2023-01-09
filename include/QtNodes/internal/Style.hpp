#pragma once

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QObject>
#include <QtCore/QString>

namespace QtNodes {

class Style // : public QObject
{
    //Q_OBJECT

public:
    virtual ~Style() = default;

public:
    virtual void loadJson(QJsonObject const &json) = 0;

    virtual QJsonObject toJson() const = 0;

    /// Loads from utf-8 byte array.
    virtual void loadJsonFromByteArray(QByteArray const &byteArray)
    {
        auto json = QJsonDocument::fromJson(byteArray).object();

        loadJson(json);
    }

    virtual void loadJsonText(QString jsonText) { loadJsonFromByteArray(jsonText.toUtf8()); }

    virtual void loadJsonFile(QString fileName)
    {
        QFile file(fileName);

        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "Couldn't open file " << fileName;

            return;
        }

        loadJsonFromByteArray(file.readAll());
    }
};

} // namespace QtNodes
