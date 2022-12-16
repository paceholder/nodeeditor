#pragma once

#include <QtCore/QJsonObject>

namespace QtNodes {

class Serializable
{
public:
    virtual ~Serializable() = default;

    virtual QJsonObject save() const { return {}; }

    virtual void load(QJsonObject const & /*p*/) {}
};
} // namespace QtNodes
