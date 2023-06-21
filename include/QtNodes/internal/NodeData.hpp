#pragma once

#include <memory>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QColor>
#include <set>

#include "Export.hpp"

#include <QDebug>
namespace QtNodes {

/**
 * `id` represents an internal unique data type for the given port.
 * `name` is a normal text description.
 */
struct NODE_EDITOR_PUBLIC NodeDataType
{
    QString id;
    QString name;
    QColor color;

    // TODO: this is pretty hacky, but seems to be the easiest to do without a major rewrite
    [[nodiscard]] bool allowConversionFrom(const QString& otherId) const {
        return allowedFromConversions.count(otherId) > 0;
    }

    std::set<QString> allowedFromConversions;
};

/**
 * Class represents data transferred between nodes.
 * @param type is used for comparing the types
 * The actual data is stored in subtypes
 */
class NODE_EDITOR_PUBLIC NodeData
{
public:
    virtual ~NodeData() = default;

    [[nodiscard]] virtual QString getDescription() const {
        return type().name;
    }

    /// Type for inner use
    virtual NodeDataType type() const = 0;

    virtual bool empty() const = 0;

    virtual void allowConversionFrom(const QString& id) = 0;
};

} // namespace QtNodes
Q_DECLARE_METATYPE(QtNodes::NodeDataType)
Q_DECLARE_METATYPE(std::shared_ptr<QtNodes::NodeData>)
