#ifndef ENUMUTILS_H
#define ENUMUTILS_H

#include <QObject>
#include <QMetaEnum>
#include <QStringList>

class EnumUtils
{
public:
    template<typename T>
    static QStringList enumToStringList() {
        QMetaEnum metaEnum = QMetaEnum::fromType<T>();
        QStringList list;
        for (int i = 0; i < metaEnum.keyCount(); ++i) {
            list << QString::fromLatin1(metaEnum.key(i));
        }
        return list;
    }
};

#endif