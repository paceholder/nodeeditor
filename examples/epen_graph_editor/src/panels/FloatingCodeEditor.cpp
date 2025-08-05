#include "panels/FloatingCodeEditor.hpp"
#include "CodeEditor.hpp"
#include "GraphEditorMainWindow.hpp"
#include "compilers/CUDACompiler.hpp"
#include "compilers/MetalCompiler.hpp"
#include "compilers/OpenCLCompiler.hpp"
#include "compilers/SimpleGPUCompiler.hpp"
#include "panels/CompileResultsWidget.hpp"
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QMoveEvent>
#include <QPushButton>
#include <QSplitter>
#include <QStandardItemModel>
#include <QTimer>
#include <QVBoxLayout>

FloatingCodeEditor::FloatingCodeEditor(GraphEditorWindow *parent)
    : FloatingPanelBase(parent, "Code Editor")
    , m_languageCombo(nullptr)
    , m_codeEditor(nullptr)
    , m_compileButton(nullptr)
    , m_darkModeCheckBox(nullptr)
    , m_resultsWidget(nullptr)
    , m_maximizeButton(nullptr)
    , m_isMaximized(false)
    , m_preMaximizeDockPosition(FloatingPanelBase::Floating)
    , m_preMaximizeDockedHeight(450)
    , m_preMaximizeDockedWidth(700)
    , m_preMaximizeFloatHeight(500)
{
    // Set panel-specific dimensions
    setFloatingWidth(600);
    setDockedWidth(700);
    setDockedHeight(450); // Larger default height for code editor

    // Set higher priority for code editor
    setPanelPriority(2); // Higher priority than other panels (default is 1)

    // Support bottom docking in addition to left/right
    setDockingDistance(40);
    
    // Enable resizing for code editor panel
    setResizable(true);

    // Initialize UI
    setupUI();
    connectSignals();

    // Initialize compilers
    initializeCompilers();

    // Initial floating size and position
    setMinimumSize(400, 300); // Set minimum size for resizing
    resize(floatingWidth(), 500);
    if (parent) {
        m_floatingGeometry = QRect((parent->width() - floatingWidth()) / 2,
                                   (parent->height() - 500) / 2,
                                   floatingWidth(),
                                   500);
        setGeometry(m_floatingGeometry);
    }

    // Ensure editor is on top
    raise();
    m_floatHeight = height();

    // Start docked to bottom after a short delay to ensure proper initialization
    QTimer::singleShot(300, this, [this]() { setDockPosition(FloatingPanelBase::DockedBottom); });
}

FloatingCodeEditor::~FloatingCodeEditor() = default;

void FloatingCodeEditor::setupUI()
{
    // Call base class setup
    setupBaseUI("Code Editor");

    QVBoxLayout *layout = getContentLayout();
    layout->setSpacing(5);

    // Top controls section
    QWidget *topWidget = new QWidget();
    QHBoxLayout *topLayout = new QHBoxLayout(topWidget);
    topLayout->setContentsMargins(0, 0, 0, 0);

    // Language selection
    QLabel *langLabel = new QLabel("Language:");
    langLabel->setStyleSheet("font-weight: bold; color: #333;");
    topLayout->addWidget(langLabel);

    m_languageCombo = new QComboBox();
    m_languageCombo->setMinimumWidth(100);

    // Compilers will be added by updateCompilerAvailability()

    topLayout->addWidget(m_languageCombo);

    // Add some spacing
    topLayout->addSpacing(20);

    // Compile button
    m_compileButton = new QPushButton("Compile");
    m_compileButton->setStyleSheet("QPushButton {"
                                   "   padding: 5px 16px;"
                                   "   background-color: #4285f4;"
                                   "   color: white;"
                                   "   border: none;"
                                   "   border-radius: 3px;"
                                   "   font-weight: bold;"
                                   "}"
                                   "QPushButton:hover {"
                                   "   background-color: #3367d6;"
                                   "}"
                                   "QPushButton:pressed {"
                                   "   background-color: #2851a3;"
                                   "}"
                                   "QPushButton:disabled {"
                                   "   background-color: #ccc;"
                                   "   color: #666;"
                                   "}");
    topLayout->addWidget(m_compileButton);

    topLayout->addStretch();

    // Dark mode toggle
    m_darkModeCheckBox = new QCheckBox("Dark Mode");
    m_darkModeCheckBox->setChecked(m_codeEditor ? m_codeEditor->isDarkMode() : false);
    topLayout->addWidget(m_darkModeCheckBox);

    // Add spacing before maximize button
    topLayout->addSpacing(10);

    // Maximize button
    m_maximizeButton = new QPushButton();
    m_maximizeButton->setFixedSize(24, 24);
    m_maximizeButton->setToolTip("Maximize");
    m_maximizeButton->setStyleSheet("QPushButton {"
                                    "   background-color: transparent;"
                                    "   border: 1px solid #ccc;"
                                    "   border-radius: 3px;"
                                    "   font-family: monospace;"
                                    "   font-size: 16px;"
                                    "   padding: 0px;"
                                    "}"
                                    "QPushButton:hover {"
                                    "   background-color: #e0e0e0;"
                                    "   border-color: #999;"
                                    "}"
                                    "QPushButton:pressed {"
                                    "   background-color: #d0d0d0;"
                                    "}");
    m_maximizeButton->setText("□"); // Unicode square symbol for maximize
    topLayout->addWidget(m_maximizeButton);

    layout->addWidget(topWidget);

    // Create splitter for code editor and results
    QSplitter *splitter = new QSplitter(Qt::Vertical);

    // Create code editor widget
    m_codeEditor = new CodeEditor();

    // Create results widget
    m_resultsWidget = new CompileResultsWidget();
    m_resultsWidget->hide();
    connect(m_resultsWidget,
            &CompileResultsWidget::closeRequested,
            this,
            &FloatingCodeEditor::onResultsCloseRequested);
    connect(m_resultsWidget,
            &CompileResultsWidget::messageClicked,
            this,
            &FloatingCodeEditor::onMessageClicked);

    splitter->addWidget(m_codeEditor);
    splitter->addWidget(m_resultsWidget);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 1);

    layout->addWidget(splitter, 1); // Give maximum space to editor

    // Initial size
    getContentWidget()->adjustSize();
    adjustSize();
}

void FloatingCodeEditor::connectSignals()
{
    connect(m_languageCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            &FloatingCodeEditor::onLanguageChanged);

    connect(m_compileButton, &QPushButton::clicked, this, &FloatingCodeEditor::onCompileClicked);

    connect(m_codeEditor, &CodeEditor::codeChanged, this, &FloatingCodeEditor::onCodeChanged);

    connect(m_codeEditor,
            &CodeEditor::languageChanged,
            this,
            &FloatingCodeEditor::onEditorLanguageChanged);

    connect(m_darkModeCheckBox, &QCheckBox::toggled, this, &FloatingCodeEditor::onThemeToggled);

    connect(m_maximizeButton, &QPushButton::clicked, this, &FloatingCodeEditor::onMaximizeClicked);
}

void FloatingCodeEditor::onLanguageChanged(int index)
{
    Q_UNUSED(index)

    QString language = getCurrentLanguage();
    m_codeEditor->setLanguage(language);

    emit languageChanged(language);
}

void FloatingCodeEditor::onCompileClicked()
{
    performCompilation();
    emit compileRequested(getCode(), getCurrentLanguage());
}

void FloatingCodeEditor::onCodeChanged()
{
    emit codeChanged();
}

void FloatingCodeEditor::onThemeToggled(bool checked)
{
    m_codeEditor->setDarkMode(checked);
}

void FloatingCodeEditor::onMaximizeClicked()
{
    toggleMaximize();
}

void FloatingCodeEditor::onEditorLanguageChanged(const QString &language)
{
    // Sync combo box with editor's language
    int index = -1;
    for (int i = 0; i < m_languageCombo->count(); ++i) {
        QString itemText = m_languageCombo->itemText(i);
        if (itemText == language || itemText.startsWith(language + " ")) {
            index = i;
            break;
        }
    }

    if (index >= 0 && m_languageCombo->currentIndex() != index) {
        m_languageCombo->setCurrentIndex(index);
    }
}

void FloatingCodeEditor::toggleMaximize()
{
    if (m_isMaximized) {
        restoreFromMaximized();
    } else {
        maximizePanel();
    }
}

void FloatingCodeEditor::maximizePanel()
{
    if (!parentWidget())
        return;

    // Store current state
    m_preMaximizeDockPosition = dockPosition();
    m_preMaximizeDockedHeight = dockedHeight();
    m_preMaximizeDockedWidth = dockedWidth();

    if (isDocked()) {
        m_preMaximizeGeometry = m_floatingGeometry;
        m_preMaximizeFloatHeight = m_floatHeight;
    } else {
        m_preMaximizeGeometry = geometry();
        m_preMaximizeFloatHeight = height();
    }

    // Get parent dimensions
    QRect parentRect = parentWidget()->rect();

    // If docked, undock first
    if (isDocked()) {
        m_isMaximized = true;
        setDockPosition(FloatingPanelBase::Floating);
        QTimer::singleShot(0, this, [this, parentRect]() {
            setGeometry(0, 0, parentRect.width(), parentRect.height());
            setMinimumSize(parentRect.size());
            setMaximumSize(parentRect.size());
            m_maximizeButton->setText("◱");
            m_maximizeButton->setToolTip("Restore");
            raise();
        });
    } else {
        setGeometry(0, 0, parentRect.width(), parentRect.height());
        setMinimumSize(parentRect.size());
        setMaximumSize(parentRect.size());
        m_maximizeButton->setText("◱");
        m_maximizeButton->setToolTip("Restore");
        raise();
        m_isMaximized = true;
    }
}

void FloatingCodeEditor::restoreFromMaximized()
{
    // Restore size constraints
    setMinimumSize(QSize(0, 0));
    setMaximumSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX));

    // Unset fixed size
    setFixedSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);

    // Restore previous dock state
    if (m_preMaximizeDockPosition != FloatingPanelBase::Floating) {
        // Restore docked dimensions first
        setDockedHeight(m_preMaximizeDockedHeight);
        setDockedWidth(m_preMaximizeDockedWidth);

        // Set appropriate size before docking
        if (m_preMaximizeDockPosition == FloatingPanelBase::DockedLeft
            || m_preMaximizeDockPosition == FloatingPanelBase::DockedRight) {
            setFixedWidth(m_preMaximizeDockedWidth);
        } else if (m_preMaximizeDockPosition == FloatingPanelBase::DockedBottom) {
            setFixedHeight(m_preMaximizeDockedHeight);
        }

        // Restore dock position
        setDockPosition(m_preMaximizeDockPosition);
    } else {
        // Restore floating state
        // First restore the proper floating size
        setFixedWidth(floatingWidth());
        setFixedHeight(m_preMaximizeGeometry.height());

        // Then restore position and size
        setGeometry(m_preMaximizeGeometry);

        // Update the stored floating geometry
        m_floatingGeometry = m_preMaximizeGeometry;
    }

    // Update button appearance
    m_maximizeButton->setText("□"); // Unicode maximize symbol
    m_maximizeButton->setToolTip("Maximize");

    m_isMaximized = false;
}

void FloatingCodeEditor::setMaximized(bool maximized)
{
    if (maximized != m_isMaximized) {
        toggleMaximize();
    }
}

void FloatingCodeEditor::setCode(const QString &code)
{
    m_codeEditor->setCode(code);
}

QString FloatingCodeEditor::getCode() const
{
    return m_codeEditor->getCode();
}

void FloatingCodeEditor::setLanguage(const QString &language)
{
    m_codeEditor->setLanguage(language);
}

QString FloatingCodeEditor::getCurrentLanguage() const
{
    QString text = m_languageCombo->currentText();
    // Remove "(Not Available)" suffix if present
    if (text.endsWith(" (Not Available)")) {
        return text.left(text.indexOf(" (Not Available)"));
    }
    return text;
}

void FloatingCodeEditor::setReadOnly(bool readOnly)
{
    m_codeEditor->setReadOnly(readOnly);
    m_compileButton->setEnabled(!readOnly);
}

void FloatingCodeEditor::moveEvent(QMoveEvent *event)
{
    FloatingPanelBase::moveEvent(event);

    // If we're maximized, prevent movement away from (0,0)
    if (m_isMaximized && (x() != 0 || y() != 0)) {
        move(0, 0);
    }
}

void FloatingCodeEditor::initializeCompilers()
{
    // Always try to create all compilers
    // They will check availability at runtime

    // OpenCL Compiler
    {
        auto openclCompiler = std::make_unique<OpenCLCompiler>();
        if (openclCompiler->isAvailable()) {
            m_compilers["OpenCL"] = std::move(openclCompiler);
            qDebug() << "OpenCL compiler initialized successfully";
        } else {
            qDebug() << "OpenCL compiler not available:" << openclCompiler->getAvailabilityError();
            // Store unavailable compiler for error messages
            m_compilers["OpenCL"] = std::move(openclCompiler);
        }
    }

    // CUDA Compiler
    {
        auto cudaCompiler = std::make_unique<CUDACompiler>();
        if (cudaCompiler->isAvailable()) {
            m_compilers["CUDA"] = std::move(cudaCompiler);
            qDebug() << "CUDA compiler initialized successfully";
        } else {
            qDebug() << "CUDA compiler not available:" << cudaCompiler->getAvailabilityError();
            // Store unavailable compiler for error messages
            m_compilers["CUDA"] = std::move(cudaCompiler);
        }
    }

    // Metal Compiler
    {
        auto metalCompiler = std::make_unique<MetalCompiler>();
        if (metalCompiler->isAvailable()) {
            m_compilers["Metal"] = std::move(metalCompiler);
            qDebug() << "Metal compiler initialized successfully";
        } else {
            qDebug() << "Metal compiler not available:" << metalCompiler->getAvailabilityError();
            // Store unavailable compiler for error messages
            m_compilers["Metal"] = std::move(metalCompiler);
        }
    }

    // Update UI to show compiler availability
    updateCompilerAvailability();
}

void FloatingCodeEditor::updateCompilerAvailability()
{
    // Store current selection
    QString currentSelection = m_languageCombo->currentText();

    // Clear and rebuild combo box
    m_languageCombo->clear();

    bool hasAnyCompiler = false;
    QStringList supportedLanguages = m_codeEditor->getSupportedLanguages();

    for (const QString &lang : supportedLanguages) {
        auto it = m_compilers.find(lang);
        if (it != m_compilers.end() && it->second) {
            if (it->second->isAvailable()) {
                m_languageCombo->addItem(lang);
                hasAnyCompiler = true;
            } else {
                // Add disabled item to show what's not available
                QString itemText = lang + " (Not Available)";
                m_languageCombo->addItem(itemText);
                int index = m_languageCombo->count() - 1;

                // Get the model and disable the item
                QStandardItemModel *model = qobject_cast<QStandardItemModel *>(
                    m_languageCombo->model());
                if (model) {
                    QStandardItem *item = model->item(index);
                    if (item) {
                        item->setEnabled(false);
                        item->setToolTip(it->second->getAvailabilityError());
                    }
                }
            }
        }
    }

    // Restore selection if possible
    int index = m_languageCombo->findText(currentSelection);
    if (index >= 0 && m_languageCombo->itemText(index) == currentSelection) {
        m_languageCombo->setCurrentIndex(index);
    }

    // Enable/disable compile button
    m_compileButton->setEnabled(hasAnyCompiler);

    if (!hasAnyCompiler) {
        m_compileButton->setToolTip(
            "No GPU compilers are available. Please install appropriate drivers.");
    } else {
        m_compileButton->setToolTip("");
    }
}

void FloatingCodeEditor::performCompilation()
{
    QString language = getCurrentLanguage();
    QString code = getCode();

    // Clear previous results
    m_compiledBinary.clear();

    // Find compiler
    auto it = m_compilers.find(language);
    if (it == m_compilers.end() || !it->second) {
        m_resultsWidget->clear();
        m_resultsWidget->setCompilationStatus(false, "No compiler available for " + language);
        showCompileResults(true);
        return;
    }

    // Double-check availability
    if (!it->second->isAvailable()) {
        m_resultsWidget->clear();
        m_resultsWidget->setCompilationStatus(false, "Compiler not available");
        m_resultsWidget->setRawOutput(it->second->getAvailabilityError());
        showCompileResults(true);
        return;
    }

    // Disable compile button during compilation
    m_compileButton->setEnabled(false);
    m_compileButton->setText("Compiling...");

    // Perform compilation
    SimpleGPUCompiler::CompileResult result = it->second->compile(code);

    // Store compiled binary if successful
    if (result.success) {
        m_compiledBinary = result.compiledBinary;
    }

    // Update results widget
    m_resultsWidget->clear();
    m_resultsWidget->setCompilationStatus(result.success);
    m_resultsWidget->setRawOutput(result.buildLog);
    m_resultsWidget->addMessages(result.messages);

    // Show results
    showCompileResults(true);

    // Re-enable compile button
    m_compileButton->setEnabled(true);
    m_compileButton->setText("Compile");
}

void FloatingCodeEditor::showCompileResults(bool show)
{
    if (show) {
        m_resultsWidget->show();
        // Adjust splitter sizes if needed
        if (isDocked() && dockPosition() == DockedBottom && !m_isMaximized) {
            setDockedHeight(600); // Increase height when showing results
            updatePosition();
        }
    } else {
        m_resultsWidget->hide();
        // Restore original size
        if (isDocked() && dockPosition() == DockedBottom && !m_isMaximized) {
            setDockedHeight(450);
            updatePosition();
        }
    }
}

void FloatingCodeEditor::resizeEvent(QResizeEvent *event)
{
    FloatingPanelBase::resizeEvent(event);

    // If we're maximized and parent resized, update our size
    if (m_isMaximized && parentWidget()) {
        QRect parentRect = parentWidget()->rect();
        setGeometry(parentRect);
        setMinimumSize(parentRect.size());
        setMaximumSize(parentRect.size());
    }
}

void FloatingCodeEditor::onResultsCloseRequested()
{
    showCompileResults(false);
}

void FloatingCodeEditor::onMessageClicked(int line, int column)
{
    // Delegate to CodeEditor
    m_codeEditor->setCursorPosition(line, column);
    m_codeEditor->ensureLineVisible(line);
    m_codeEditor->highlightLine(line);
    m_codeEditor->setFocus();
}

void FloatingCodeEditor::dockChanged(bool isFloat)
{
    if (isFloat && !m_isMaximized) {
        m_maximizeButton->hide();
    } else {
        m_maximizeButton->show();
    }
}