#pragma once

#include <QString>
#include <QColor>

namespace QtNodes {

    class DataInfo {

    public:
        DataInfo(const QString &name, const QString &id, const QColor &color) : dataName_(name), dataId_(id),
                                                                                dataColor_(color) {

        }

        const QString &getDataName() const {
            return dataName_;
        }

        const QString &getDataId() const {
            return dataId_;
        }

        const QColor &getDataColor() const {
            return dataColor_;
        }

    private:
        QString dataName_;
        QString dataId_;
        QColor dataColor_;
    };

} // nitro