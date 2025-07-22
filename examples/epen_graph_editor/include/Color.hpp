#ifndef COLOR_HPP
#define COLOR_HPP

#include <algorithm>
#include <QColor>
#include <QMetaType>
#include <QObject>

class Color : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float red READ red WRITE setRed NOTIFY redChanged)
    Q_PROPERTY(float green READ green WRITE setGreen NOTIFY greenChanged)
    Q_PROPERTY(float blue READ blue WRITE setBlue NOTIFY blueChanged)
    Q_PROPERTY(float alpha READ alpha WRITE setAlpha NOTIFY alphaChanged)

public:
    explicit Color(QObject *parent = nullptr)
        : QObject(parent)
        , m_red(0.0f)
        , m_green(0.0f)
        , m_blue(0.0f)
        , m_alpha(1.0f)
    {}

    Color(float r, float g, float b, float a = 1.0f, QObject *parent = nullptr)
        : QObject(parent)
        , m_red(clamp(r))
        , m_green(clamp(g))
        , m_blue(clamp(b))
        , m_alpha(clamp(a))
    {}

    // Copy constructor
    Color(const Color &other, QObject *parent = nullptr)
        : QObject(parent)
        , m_red(other.m_red)
        , m_green(other.m_green)
        , m_blue(other.m_blue)
        , m_alpha(other.m_alpha)
    {}

    // Constructor from QColor
    Color(const QColor &qcolor, QObject *parent = nullptr)
        : QObject(parent)
    {
        setFromQColor(qcolor);
    }

    // Getters
    float red() const { return m_red; }
    float green() const { return m_green; }
    float blue() const { return m_blue; }
    float alpha() const { return m_alpha; }

    // Setters with clamping to [0.0, 1.0]
    void setRed(float red)
    {
        float clamped = clamp(red);

        m_red = clamped;
        emit redChanged(m_red);
        emit colorChanged();
    }

    void setGreen(float green)
    {
        float clamped = clamp(green);

        m_green = clamped;
        emit greenChanged(m_green);
        emit colorChanged();
    }

    void setBlue(float blue)
    {
        float clamped = clamp(blue);

        m_blue = clamped;
        emit blueChanged(m_blue);
        emit colorChanged();
    }

    void setAlpha(float alpha)
    {
        float clamped = clamp(alpha);
        if (qFuzzyCompare(m_alpha, clamped))
            return;
        m_alpha = clamped;
        emit alphaChanged(m_alpha);
        emit colorChanged();
    }

    // Convenience method to set all values at once
    void setColor(float r, float g, float b, float a = 1.0f)
    {
        bool changed = false;

        float clampedR = clamp(r);
        float clampedG = clamp(g);
        float clampedB = clamp(b);
        float clampedA = clamp(a);

        if (!qFuzzyCompare(m_red, clampedR)) {
            m_red = clampedR;
            emit redChanged(m_red);
            changed = true;
        }

        if (!qFuzzyCompare(m_green, clampedG)) {
            m_green = clampedG;
            emit greenChanged(m_green);
            changed = true;
        }

        if (!qFuzzyCompare(m_blue, clampedB)) {
            m_blue = clampedB;
            emit blueChanged(m_blue);
            changed = true;
        }

        if (!qFuzzyCompare(m_alpha, clampedA)) {
            m_alpha = clampedA;
            emit alphaChanged(m_alpha);
            changed = true;
        }

        if (changed) {
            emit colorChanged();
        }
    }

    // Convert to/from QColor
    QColor toQColor() const { return QColor::fromRgbF(m_red, m_green, m_blue, m_alpha); }

    void setFromQColor(const QColor &qcolor)
    {
        setColor(qcolor.redF(), qcolor.greenF(), qcolor.blueF(), qcolor.alphaF());
    }

    // Convert to/from integer values (0-255)
    void setFromRgba255(int r, int g, int b, int a = 255)
    {
        setColor(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
    }

    int red255() const { return static_cast<int>(m_red * 255.0f + 0.5f); }
    int green255() const { return static_cast<int>(m_green * 255.0f + 0.5f); }
    int blue255() const { return static_cast<int>(m_blue * 255.0f + 0.5f); }
    int alpha255() const { return static_cast<int>(m_alpha * 255.0f + 0.5f); }

    // Convert to hex string
    QString toHex() const
    {
        return QString("#%1%2%3%4")
            .arg(red255(), 2, 16, QChar('0'))
            .arg(green255(), 2, 16, QChar('0'))
            .arg(blue255(), 2, 16, QChar('0'))
            .arg(alpha255(), 2, 16, QChar('0'));
    }

    // Assignment operator
    Color &operator=(const Color &other)
    {
        if (this != &other) {
            setColor(other.m_red, other.m_green, other.m_blue, other.m_alpha);
        }
        return *this;
    }

    // Comparison operators
    bool operator==(const Color &other) const
    {
        return qFuzzyCompare(m_red, other.m_red) && qFuzzyCompare(m_green, other.m_green)
               && qFuzzyCompare(m_blue, other.m_blue) && qFuzzyCompare(m_alpha, other.m_alpha);
    }

    bool operator!=(const Color &other) const { return !(*this == other); }

    // Useful color operations
    void invert() { setColor(1.0f - m_red, 1.0f - m_green, 1.0f - m_blue, m_alpha); }

    void makeGrayscale()
    {
        // Using luminance formula
        float gray = 0.299f * m_red + 0.587f * m_green + 0.114f * m_blue;
        setColor(gray, gray, gray, m_alpha);
    }

    float luminance() const { return 0.299f * m_red + 0.587f * m_green + 0.114f * m_blue; }

    // Blend with another color
    void blend(const Color &other, float factor = 0.5f)
    {
        factor = clamp(factor);
        float invFactor = 1.0f - factor;

        setColor(m_red * invFactor + other.m_red * factor,
                 m_green * invFactor + other.m_green * factor,
                 m_blue * invFactor + other.m_blue * factor,
                 m_alpha * invFactor + other.m_alpha * factor);
    }

signals:
    void redChanged(float red);
    void greenChanged(float green);
    void blueChanged(float blue);
    void alphaChanged(float alpha);
    void colorChanged(); // Emitted when any component changes

private:
    // Helper function to clamp values to [0.0, 1.0]
    static float clamp(float value) { return std::max(0.0f, std::min(1.0f, value)); }

    float m_red;
    float m_green;
    float m_blue;
    float m_alpha;
};

// Register the type for use with Qt's meta-type system
Q_DECLARE_METATYPE(Color *)

#endif // COLOR_HPP