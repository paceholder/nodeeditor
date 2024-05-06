#pragma once

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QObject>
#include <QtCore/QString>

#define X_VALUE_EXISTS(v) \
    (v.type() != QJsonValue::Undefined && v.type() != QJsonValue::Null)

#define X_STYLE_READ_COLOR(values, variable) \
    { \
        auto valueRef = values[#variable]; \
        if (X_VALUE_EXISTS(valueRef)) { \
            if (valueRef.isArray()) { \
                auto colorArray = valueRef.toArray(); \
                std::vector<int> rgb; \
                rgb.reserve(3); \
                int alpha = 255; \
                for (auto it = colorArray.begin(); it != colorArray.end(); ++it) { \
                    rgb.push_back((*it).toInt()); \
                } \
                if (colorArray.size() > 3) \
                    alpha = colorArray[3].toInt(); \
                variable = QColor(rgb[0], rgb[1], rgb[2], alpha); \
            } else { \
                variable = QColor(valueRef.toString()); \
            } \
        } \
    }

#define X_STYLE_WRITE_COLOR(values, variable) \
    { \
        values[#variable] = variable.name(QColor::HexArgb); \
    }

#define X_STYLE_READ_FLOAT(values, variable) \
    { \
        auto valueRef = values[#variable]; \
        if (X_VALUE_EXISTS(valueRef)) \
            variable = valueRef.toDouble(); \
    }

#define X_STYLE_WRITE_FLOAT(values, variable) \
    { \
        values[#variable] = variable; \
    }

#define X_STYLE_READ_BOOL(values, variable) \
    { \
        auto valueRef = values[#variable]; \
        if (X_VALUE_EXISTS(valueRef)) \
            variable = valueRef.toBool(); \
    }

#define X_STYLE_WRITE_BOOL(values, variable) \
    { \
        values[#variable] = variable; \
    }


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
