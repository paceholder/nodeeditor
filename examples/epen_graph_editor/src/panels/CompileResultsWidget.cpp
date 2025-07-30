#include "panels/CompileResultsWidget.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QSplitter>
#include <QLabel>
#include <QStyle>
#include <QTabWidget>

CompileResultsWidget::CompileResultsWidget(QWidget *parent)
    : QWidget(parent)
    , m_tabWidget(nullptr)
    , m_rawOutputEdit(nullptr)
    , m_messageList(nullptr)
    , m_clearButton(nullptr)
    , m_closeButton(nullptr)
{
    setupUI();
}

CompileResultsWidget::~CompileResultsWidget() = default;

void CompileResultsWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    
    // Title bar
    QHBoxLayout *titleLayout = new QHBoxLayout();
    QLabel *titleLabel = new QLabel("Compilation Results");
    titleLabel->setStyleSheet("font-weight: bold; font-size: 12px;");
    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();
    
    m_clearButton = new QPushButton("Clear");
    m_clearButton->setMaximumWidth(60);
    connect(m_clearButton, &QPushButton::clicked, this, &CompileResultsWidget::clear);
    titleLayout->addWidget(m_clearButton);
    
    m_closeButton = new QPushButton("×");
    m_closeButton->setMaximumWidth(20);
    m_closeButton->setMaximumHeight(20);
    m_closeButton->setStyleSheet(
        "QPushButton {"
        "   border: none;"
        "   font-weight: bold;"
        "   font-size: 16px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #ff4444;"
        "   color: white;"
        "}"
    );
    connect(m_closeButton, &QPushButton::clicked, this, &CompileResultsWidget::closeRequested);
    titleLayout->addWidget(m_closeButton);
    
    mainLayout->addLayout(titleLayout);
    
    // Create tab widget
    m_tabWidget = new QTabWidget();
    
    // Message list tab
    m_messageList = new QListWidget();
    m_messageList->setAlternatingRowColors(true);
    m_messageList->setStyleSheet(
        "QListWidget {"
        "   border: 1px solid #ccc;"
        "   border-radius: 3px;"
        "}"
        "QListWidget::item {"
        "   padding: 4px;"
        "   border-bottom: 1px solid #eee;"
        "}"
        "QListWidget::item:selected {"
        "   background-color: #4285f4;"
        "   color: white;"
        "}"
        "QListWidget::item:hover {"
        "   background-color: #e3f2fd;"
        "}"
    );
    connect(m_messageList, &QListWidget::itemClicked, 
            this, &CompileResultsWidget::onItemClicked);
    
    // Raw output tab
    m_rawOutputEdit = new QTextEdit();
    m_rawOutputEdit->setReadOnly(true);
    m_rawOutputEdit->setFont(QFont("Consolas", 9));
    m_rawOutputEdit->setStyleSheet(
        "QTextEdit {"
        "   background-color: #f5f5f5;"
        "   border: 1px solid #ccc;"
        "   border-radius: 3px;"
        "}"
    );
    
    m_tabWidget->addTab(m_messageList, "Messages");
    m_tabWidget->addTab(m_rawOutputEdit, "Build Log");
    
    mainLayout->addWidget(m_tabWidget);
}

void CompileResultsWidget::clear()
{
    m_messages.clear();
    m_messageList->clear();
    m_rawOutputEdit->clear();
}

void CompileResultsWidget::addMessage(const CompileMessage &message)
{
    m_messages.append(message);
    
    // Create list item
    QListWidgetItem *item = new QListWidgetItem();
    
    // Format message
    QString formattedMessage;
    if (!message.file.isEmpty()) {
        formattedMessage = QString("%1:%2:%3: %4")
            .arg(message.file)
            .arg(message.line)
            .arg(message.column)
            .arg(message.message);
    } else {
        formattedMessage = QString("Line %1, Col %2: %3")
            .arg(message.line)
            .arg(message.column)
            .arg(message.message);
    }
    
    item->setText(formattedMessage);
    item->setData(Qt::UserRole, m_messages.size() - 1);
    
    // Set icon and color based on type
    QColor color = getColorForType(message.type);
    item->setForeground(color);
    
    // Set icon
    switch (message.type) {
    case CompileMessage::Error:
        item->setIcon(style()->standardIcon(QStyle::SP_DialogCancelButton));
        break;
    case CompileMessage::Warning:
        item->setIcon(style()->standardIcon(QStyle::SP_MessageBoxWarning));
        break;
    case CompileMessage::Info:
        item->setIcon(style()->standardIcon(QStyle::SP_MessageBoxInformation));
        break;
    }
    
    m_messageList->addItem(item);
}

void CompileResultsWidget::addMessages(const QVector<CompileMessage> &messages)
{
    for (const auto &message : messages) {
        addMessage(message);
    }
}

void CompileResultsWidget::setRawOutput(const QString &output)
{
    m_rawOutputEdit->setPlainText(output);
    
    // Auto-switch to build log tab if there's output but no messages
    if (!output.isEmpty() && m_messages.isEmpty()) {
        m_tabWidget->setCurrentIndex(1);
    }
}

void CompileResultsWidget::setCompilationStatus(bool success, const QString &summary)
{
    CompileMessage statusMessage;
    statusMessage.type = success ? CompileMessage::Info : CompileMessage::Error;
    statusMessage.message = summary.isEmpty() 
        ? (success ? "Compilation successful" : "Compilation failed")
        : summary;
    statusMessage.line = 0;
    statusMessage.column = 0;
    
    // Add as first item
    m_messages.prepend(statusMessage);
    
    QListWidgetItem *item = new QListWidgetItem();
    item->setText(statusMessage.message);
    item->setData(Qt::UserRole, -1); // Special index for status
    
    if (success) {
        item->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
        item->setForeground(QColor(0, 128, 0));
        item->setBackground(QColor(240, 255, 240));
    } else {
        item->setIcon(style()->standardIcon(QStyle::SP_DialogCancelButton));
        item->setForeground(QColor(128, 0, 0));
        item->setBackground(QColor(255, 240, 240));
    }
    
    m_messageList->insertItem(0, item);
    
    // Auto-switch to messages tab
    m_tabWidget->setCurrentIndex(0);
}

void CompileResultsWidget::onItemClicked(QListWidgetItem *item)
{
    int index = item->data(Qt::UserRole).toInt();
    if (index >= 0 && index < m_messages.size()) {
        const CompileMessage &message = m_messages[index];
        emit messageClicked(message.line, message.column);
    }
}

QColor CompileResultsWidget::getColorForType(CompileMessage::Type type) const
{
    switch (type) {
    case CompileMessage::Error:
        return QColor(204, 0, 0);
    case CompileMessage::Warning:
        return QColor(255, 140, 0);
    case CompileMessage::Info:
        return QColor(0, 0, 204);
    default:
        return QColor(0, 0, 0);
    }
}