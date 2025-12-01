#include "kpad.h"
#include "ui_kpad.h"

Kpad::Kpad(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Kpad)
    , textEdit(new QTextEdit(this))
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/icons/KpadIcon.ico"));
    resize(770, 700);
    setWindowTitle("KPad+");
    setCentralWidget(textEdit);

    // (for Dark mode):
    // Store all original icons ONCE at startup
    QList<QAction*> allActions = findChildren<QAction*>();
    for (QAction *action : allActions) {
        if (!action->icon().isNull()) {
            originalIcons[action] = action->icon(); // Save the original
        }
    }

    textEdit->installEventFilter(this);
    textEdit->viewport()->installEventFilter(this);
    textEdit->setTabChangesFocus(false);

    // ----------------  Font Size ComboBox ----------------
    fontSizeBox = new QComboBox(this);
    fontSizeBox->setEditable(true);
    QList<int> fontSizes = {8, 10, 12, 14, 16, 18, 20, 22, 28, 32};
    for(int s : fontSizes) fontSizeBox->addItem(QString::number(s));
    fontSizeBox->setCurrentText("14");

    // ----------------  Font Style ComboBox ----------------
    fontFamilyBox = new QFontComboBox(this);
    fontFamilyBox->setEditable(true);
    fontFamilyBox->setFontFilters(QFontComboBox::AllFonts);

    // Set default font for editor
    QFont defaultFont("Arial", 14);
    textEdit->setFont(defaultFont);

    // Sync ComboBoxes
    fontFamilyBox->setCurrentFont(defaultFont);
    fontSizeBox->setCurrentText(QString::number(defaultFont.pointSize()));


    // ----------------  Toolbar ----------------
    QToolBar *editToolBar = addToolBar("Edit");

    // Basic edit actions
    editToolBar->addAction(ui->actionCut);
    editToolBar->addAction(ui->actionCopy);
    editToolBar->addAction(ui->actionPaste);
    editToolBar->addSeparator();
    editToolBar->addAction(ui->actionUndo);
    editToolBar->addAction(ui->actionRedo);
    editToolBar->addSeparator();

    // Font size increase/decrease
    ui->actionDecrease_Font->setIconText("-");
    ui->actionIncrease_Font->setIconText("+");
    editToolBar->addAction(ui->actionDecrease_Font);
    editToolBar->addWidget(fontSizeBox);
    editToolBar->addAction(ui->actionIncrease_Font);
    editToolBar->addSeparator();

    // Bold / Italic / Underline
    editToolBar->addAction(ui->actionBold);
    editToolBar->addAction(ui->actionItalic);
    editToolBar->addAction(ui->actionUnderline);
    editToolBar->addSeparator();
    editToolBar->addWidget(fontFamilyBox);
    editToolBar->addSeparator();

    // Highlight
    editToolBar->addAction(ui->actionHighlight);
    editToolBar->addAction(ui->actionTextColor);
    editToolBar->addSeparator();

    // Alignment
    editToolBar->addAction(ui->actionLeft_Align);
    editToolBar->addAction(ui->actionCenter_Align);
    editToolBar->addAction(ui->actionRight_Align);

    // ----- Connect Actions -----
    // File menu
    // connect(ui->actionCode, &QAction::triggered, this, &Kpad::switchToCodeEditor);
    connect(ui->actionNewDocument, &QAction::triggered, this, &Kpad::newDocument);
    connect(ui->actionOpen, &QAction::triggered, this, &Kpad::open);
    connect(ui->actionSave, &QAction::triggered, this, &Kpad::save);
    connect(ui->actionSave_as, &QAction::triggered, this, &Kpad::saveAs);
    connect(ui->actionSave_as_HTML, &QAction::triggered, this, &Kpad::saveAsHTML);
    connect(ui->actionExit, &QAction::triggered, this, &Kpad::exit);
    connect(ui->actionAbout_me, &QAction::triggered, this, &Kpad::showAbout);

    // Edit actions
    connect(ui->actionCut, &QAction::triggered, textEdit, &QTextEdit::cut);
    connect(ui->actionCopy, &QAction::triggered, textEdit, &QTextEdit::copy);
    connect(ui->actionPaste, &QAction::triggered, textEdit, &QTextEdit::paste);
    connect(ui->actionUndo, &QAction::triggered, textEdit, &QTextEdit::undo);
    connect(ui->actionRedo, &QAction::triggered, textEdit, &QTextEdit::redo);

    // Formatting
    connect(ui->actionIncrease_Font, &QAction::triggered, this, &Kpad::increaseFontSize);
    connect(ui->actionDecrease_Font, &QAction::triggered, this, &Kpad::decreaseFontSize);
    connect(ui->actionBold, &QAction::triggered, this, &Kpad::setBold);
    connect(ui->actionItalic, &QAction::triggered, this, &Kpad::setItalic);
    connect(ui->actionUnderline, &QAction::triggered, this, &Kpad::setUnderline);
    connect(ui->actionHighlight, &QAction::toggled, this, &Kpad::toggleHighlight);
    connect(ui->actionTextColor, &QAction::triggered, this, &Kpad::toggleTextColor);
    connect(ui->actionLeft_Align, &QAction::triggered, this, &Kpad::alignLeft);
    connect(ui->actionCenter_Align, &QAction::triggered, this, &Kpad::alignCenter);
    connect(ui->actionRight_Align, &QAction::triggered, this, &Kpad::alignRight);

    connect(ui->actionDisc, &QAction::triggered, this, [=]() {
        insertBulletList("*");
    });
    connect(ui->actionCircle, &QAction::triggered, this, [=]() {
        insertBulletList("o");
    });
    connect(ui->actionSquare, &QAction::triggered, this, [=]() {
        insertBulletList("-");
    });
    connect(ui->actionDecimal, &QAction::triggered, this, [=]() {
        insertBulletList("1.");
    });
    connect(ui->actionLowerAlpha, &QAction::triggered, this, [=]() {
        insertBulletList("a.");
    });
    connect(ui->actionUpperAlpha, &QAction::triggered, this, [=]() {
        insertBulletList("A.");
    });
    connect(ui->actionLowerRoman, &QAction::triggered, this, [=]() {
        insertBulletList("i.");
    });
    connect(ui->actionUpperRoman, &QAction::triggered, this, [=]() {
        insertBulletList("I.");
    });

    // View
    connect(ui->actionZoom_Out, &QAction::triggered, this, &Kpad::zoomOut);
    connect(ui->actionZoom_In, &QAction::triggered, this, &Kpad::zoomIn);
    // Dark/Light Theme
    connect(ui->actionToggleTheme, &QAction::triggered, this, &Kpad::toggleTheme);


    // Font size and family ComboBox changes
    auto applyFont = [=](const QFont &f) {
        QTextCursor cursor = textEdit->textCursor();
        QTextCharFormat format = cursor.charFormat();
        format.setFont(f);
        if(cursor.hasSelection())
            cursor.mergeCharFormat(format);
        textEdit->mergeCurrentCharFormat(format);
    };

    // Font size change
    connect(fontSizeBox, &QComboBox::currentTextChanged, this, [=](const QString &text) {
        bool ok;
        int size = text.toInt(&ok);
        if (!ok || size <= 0) return;

        QFont f = textEdit->currentFont();
        f.setPointSize(size);
        applyFont(f);
    });

    // Font family change
    connect(fontFamilyBox, &QFontComboBox::currentFontChanged, this, [=](const QFont &font) {
        QTextCursor cursor = textEdit->textCursor();
        QTextCharFormat format;

        QFont current = cursor.charFormat().font();
        if (current.pointSize() <= 0)
            current.setPointSize(textEdit->font().pointSize());

        current.setFamily(font.family());
        current.setPointSize(fontSizeBox->currentText().toInt());

        format.setFont(current);

        if (cursor.hasSelection())
            cursor.mergeCharFormat(format);

        textEdit->mergeCurrentCharFormat(format);
    });

    // ---------------- Status Bar ----------------
    statusBar()->showMessage("Ready");

    // ----- Word and Character counter -----
    wordCountLabel = new QLabel(this);
    charCountLabel = new QLabel(this);
    statusBar()->addPermanentWidget(wordCountLabel);
    statusBar()->addPermanentWidget(charCountLabel);
    updateCounts();
    connect(textEdit, &QTextEdit::textChanged, this, &Kpad::updateCounts);
    connect(textEdit, &QTextEdit::cursorPositionChanged, this, &Kpad::updateCounts);

    // ----- Find Box -----
    findBox = new QLineEdit(this);
    findBox->setPlaceholderText("Find...");
    findBox->setMaximumWidth(150);
    statusBar()->addPermanentWidget(findBox);

    // ----- Highlight format -----
    highlightFormat.setBackground(Qt::gray);
    highlightFormat.setForeground(Qt::black);
    connect(findBox, &QLineEdit::textChanged, this, &Kpad::highlightMatches);

    // ----- Lock Window Size -----
    // Create lock button for status bar
    lockButton = new QPushButton(this);
    lockButton->setText("Unlocked");
    lockButton->setFlat(true);
    lockButton->setToolTip("Lock window size");
    lockButton->setCheckable(true);
    lockButton->setMaximumWidth(60);
    ui->statusbar->addPermanentWidget(lockButton);
    // Connect the button
    connect(lockButton, &QPushButton::toggled, this, &Kpad::toggleWindowLock);

}

Kpad::~Kpad() {
    delete ui;
}
