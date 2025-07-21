#ifndef EXPANDABLE_CATEGORY_WIDGET_HPP
#define EXPANDABLE_CATEGORY_WIDGET_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QPropertyAnimation>
#include <QStyle>
#include <QPainter>
#include <QStyleOption>

class ExpandableCategoryWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int contentHeight READ contentHeight WRITE setContentHeight)

public:
    explicit ExpandableCategoryWidget(const QString &title, 
                                     int indentLevel = 0, 
                                     QWidget *parent = nullptr);
    
    void setContentWidget(QWidget *widget);
    QWidget *contentWidget() const;
    
    void addWidget(QWidget *widget);
    QVBoxLayout *contentLayout() const;
    
    void setExpanded(bool expanded);
    bool isExpanded() const;
    
    void setIndentLevel(int level);
    int indentLevel() const;
    
    // Styling
    void setHeaderStyle(const QString &style);
    void setArrowColor(const QColor &color);
    
signals:
    void expandedChanged(bool expanded);
    void contentHeightChanged();
    
public slots:
    void toggle();
    void expand();
    void collapse();
    void updateContentHeight();
    
protected:
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
    
    // UI elements - made protected for height calculation
    QWidget *m_headerWidget;
    QWidget *m_contentArea;
    
private:
    void setupUI();
    void updateArrow();
    int contentHeight() const;
    void setContentHeight(int height);
    
    // UI elements
    QPushButton *m_toggleButton;
    QLabel *m_titleLabel;
    QLabel *m_chevronLabel;
    QVBoxLayout *m_contentLayout;
    
    // Animation
    QPropertyAnimation *m_animation;
    
    // State
    bool m_isExpanded;
    int m_indentLevel;
    QString m_title;
    QColor m_arrowColor;
};

#endif // EXPANDABLE_CATEGORY_WIDGET_HPP