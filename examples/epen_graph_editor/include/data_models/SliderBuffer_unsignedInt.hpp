#pragma once
#include "UIBufferBase.hpp"
class SliderBuffer_unsignedInt : public UIBufferBase
{
    Q_OBJECT
public:
    QString caption() const override { return QStringLiteral("unsigned int Slider Input Buffer"); }

    QString name() const override { return QStringLiteral("SliderBuffer_unsignedInt"); }
};