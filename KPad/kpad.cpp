#include "kpad.h"
#include "ui_kpad.h"

Kpad::Kpad(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Kpad)   // Make sure `ui` is initialized
    , textEdit(new QTextEdit(this)) // Create QTextEdit as central widget
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/icons/KpadIcon.ico"));
    setMinimumSize(650, 600);   // Prevent shrinking
    setWindowTitle("KPad+");
    setCentralWidget(textEdit); // Set QTextEdit as main editing area
    textEdit->installEventFilter(this);
    textEdit->viewport()->installEventFilter(this);

    // ----------------  Font Size ComboBox ----------------
    fontSizeBox = new QComboBox(this);
    fontSizeBox->setEditable(true); // Allow custom font size input
    QList<int> fontSizes = {8, 10, 12, 14, 16, 18, 20, 22, 28, 32};
    for(int s : fontSizes) fontSizeBox->addItem(QString::number(s));
    fontSizeBox->setCurrentText("14"); // Default font size

    // ----------------  Font Style ComboBox ----------------
    fontFamilyBox = new QFontComboBox(this);
    fontFamilyBox->setEditable(true);
    fontFamilyBox->setFontFilters(QFontComboBox::AllFonts); // show all available fonts

    // Set default font for editor
    QFont defaultFont("Consolas", 14); // font family + size
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
    editToolBar->addWidget(fontSizeBox); // Add ComboBox for custom size
    editToolBar->addAction(ui->actionIncrease_Font);
    editToolBar->addSeparator();
    // Bold / Italic / Underline
    editToolBar->addAction(ui->actionBold);
    editToolBar->addAction(ui->actionItalic);
    editToolBar->addAction(ui->actionUnderline);
    editToolBar->addSeparator();
    editToolBar->addWidget(fontFamilyBox);
    editToolBar->addSeparator();
    // Alignment
    editToolBar->addAction(ui->actionLeft_Align);
    editToolBar->addAction(ui->actionCenter_Align);
    editToolBar->addAction(ui->actionRight_Align);

    // ----- Shortcuts -----
    ui->actionIncrease_Font->setShortcut(QKeySequence("Ctrl+="));
    ui->actionDecrease_Font->setShortcut(QKeySequence("Ctrl+-"));
    ui->actionBold->setShortcut(QKeySequence("Ctrl+B"));
    ui->actionItalic->setShortcut(QKeySequence("Ctrl+I"));
    ui->actionUnderline->setShortcut(QKeySequence("Ctrl+U"));
    ui->actionLeft_Align->setShortcut(QKeySequence("Ctrl+Shift+L"));
    ui->actionCenter_Align->setShortcut(QKeySequence("Ctrl+Shift+C"));
    ui->actionRight_Align->setShortcut(QKeySequence("Ctrl+Shift+R"));


    // ----- Connect Actions -----
    // File menu
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

    // Font size and family ComboBox changes
    // Font size
    // Apply font changes to selected text or typing cursor
    auto applyFont = [=](const QFont &f) {
        QTextCursor cursor = textEdit->textCursor();
        QTextCharFormat format = cursor.charFormat();
        format.setFont(f);               // merge all font properties
        if(cursor.hasSelection())
            cursor.mergeCharFormat(format); // change selected text
        textEdit->mergeCurrentCharFormat(format); // affect future typing
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

        // Preserve existing style attributes
        QFont current = cursor.charFormat().font();
        if (current.pointSize() <= 0)
            current.setPointSize(textEdit->font().pointSize());

        current.setFamily(font.family()); // only change family
        current.setPointSize(fontSizeBox->currentText().toInt()); // keep size from box

        format.setFont(current);

        if (cursor.hasSelection())
            cursor.mergeCharFormat(format);

        textEdit->mergeCurrentCharFormat(format); // apply to future typing
    });




    // ---------------- Status Bar ----------------
    statusBar()->showMessage("Ready"); // Show ready message in status bar

    // ----- Word and Character counter -----
    wordCountLabel = new QLabel(this);
    charCountLabel = new QLabel(this);

    // Add to status bar:
    statusBar()->addPermanentWidget(wordCountLabel);
    statusBar()->addPermanentWidget(charCountLabel);
    updateCounts(); // Initial update
    connect(textEdit, &QTextEdit::textChanged, this, &Kpad::updateCounts);
    connect(textEdit, &QTextEdit::cursorPositionChanged, this, &Kpad::updateCounts);

    // ----- Find Box -----
    findBox = new QLineEdit(this);
    findBox->setPlaceholderText("Find...");
    findBox->setMaximumWidth(150); // Optional: limit size
    statusBar()->addPermanentWidget(findBox);

    // Highlight format
    highlightFormat.setBackground(Qt::gray);
    highlightFormat.setForeground(Qt::black);

    // Connect signal for real-time search
    connect(findBox, &QLineEdit::textChanged, this, &Kpad::highlightMatches);
}

Kpad::~Kpad() { delete ui; }

// -------------------- File Actions --------------------
void Kpad::exit() { close(); }

void Kpad::closeEvent(QCloseEvent *event) {
    if (maybeSave()) {
        event->accept();  // Allow the application to close
    } else {
        event->ignore();  // Cancel the close operation
    }
}

bool Kpad::maybeSave() {
    if (!hasUnsavedChanges()) {
        return true;  // No unsaved changes, safe to close
    }

    QString fileName = currentFile.isEmpty() ? "Untitled" : QFileInfo(currentFile).fileName();

    QMessageBox::StandardButton result = QMessageBox::question(
        this,
        "KPad+ - Unsaved Changes",
        QString("The document '%1' has been modified.\n\n"
                "Do you want to save your changes before closing?").arg(fileName),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
        QMessageBox::Save  // Default button
        );

    switch (result) {
    case QMessageBox::Save:
        save();  // Use your existing save method
        return true;  // Allow closing after save
    case QMessageBox::Discard:
        return true;  // Close without saving
    case QMessageBox::Cancel:
    default:
        return false;  // Cancel the close operation
    }
}

bool Kpad::hasUnsavedChanges() {
    // Check if the document has been modified
    return textEdit->document()->isModified();
}

void Kpad::open() {
    if (!maybeSave()) {
        return;  // User cancelled, don't open new file
    }

    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Open File",
        "",
        "Text Files (*.txt);;HTML Files (*.html *.htm);;All Files (*.*)"
        );

    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot open file: " + file.errorString());
        return;
    }

    QTextStream in(&file);
    QString text = in.readAll();
    file.close();

    currentFile = fileName;
    setWindowTitle(QFileInfo(fileName).fileName() + " - KPad+");

    if (fileName.endsWith(".html", Qt::CaseInsensitive) || fileName.endsWith(".htm", Qt::CaseInsensitive)) {
        textEdit->setHtml(text);   // load with formatting
    } else {
        textEdit->setPlainText(text); // load as raw text
    }

    textEdit->document()->setModified(false);  // Mark as not modified since we just loaded
}


void Kpad::newDocument() {
    if (!maybeSave()) {
        return;  // User cancelled, don't create new document
    }

    currentFile.clear();
    textEdit->clear();
    textEdit->document()->setModified(false);  // Mark as not modified
    setWindowTitle("KPad+");
}

void Kpad::save() {
    // If currentFile is empty, ask the user for a file name
    QString fileName = currentFile.isEmpty() ? QFileDialog::getSaveFileName(
        this,
        "Save",
        "",
        "Text Files (*.txt);;All Files (*.*)"  // default filter for .txt
        ) : currentFile;

    if(fileName.isEmpty()) return;

    // Enforce .txt extension if saving a new file and user didn't provide one
    if (currentFile.isEmpty() && !fileName.endsWith(".txt", Qt::CaseInsensitive)) {
        fileName += ".txt";
    }

    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QFile::Text)){
        QMessageBox::warning(this,"Warning","Cannot save file: "+file.errorString());
        return;
    }

    currentFile = fileName;  // update the current file path
    QTextStream out(&file);
    out << textEdit->toPlainText();  // save plain text
    file.close();

    textEdit->document()->setModified(false);  // Mark as saved
    setWindowTitle(QFileInfo(fileName).fileName() + " - KPad+");
}



void Kpad::saveAs() {
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Save As",
        "",
        "Text Files (*.txt);;All Files (*.*)"
        );
    if(fileName.isEmpty()) return;

    if (!fileName.endsWith(".txt", Qt::CaseInsensitive)) {
        fileName += ".txt";
    }

    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QFile::Text)){
        QMessageBox::warning(this,"Warning","Cannot save file: "+file.errorString());
        return;
    }

    currentFile = fileName;
    QTextStream out(&file);
    out << textEdit->toPlainText();
    file.close();

    textEdit->document()->setModified(false);  // Mark as saved
    setWindowTitle(QFileInfo(fileName).fileName() + " - KPad+");
}

void Kpad::saveAsHTML() {
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Save as HTML",
        "",
        "HTML Files (*.html *.htm)"
        );

    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot save file: " + file.errorString());
        return;
    }

    QTextStream out(&file);
    out << textEdit->toHtml();
    file.close();

    currentFile = fileName;
    textEdit->document()->setModified(false);  // Mark as saved
    setWindowTitle(QFileInfo(fileName).fileName() + " - KPad+");
}

// -------------------- About --------------------
void Kpad::showAbout() {
    QString aboutText =
        "<b>KPad - a C++/Qt Text Editor</b><br><br>"
        "KPad is a lightweight and efficient text editor built with C++ and Qt, "
        "designed as a modern, minimal alternative inspired by Notepad++. "
        "It combines speed, simplicity, and essential features to provide a "
        "clean and intuitive writing experience."

        "<br><br><b>Features:</b><br>"
        "- Bold, Italic, Underline formatting<br>"
        "- Adjustable font size and font family<br>"
        "- Left, Center, Right text alignment<br>"
        "- Bullet lists<br>"
        "- Zoom In / Zoom Out via buttons or Ctrl + Mouse Wheel<br>"
        "- Find functionality <br>"
        "- Word and Character counter<br>"
        "- Save as plain text or HTML with formatting preserved<br>"
        "- Undo / Redo, Cut / Copy / Paste<br>"
        "- Keyboard shortcuts for common actions<br>"
        "- New, Open, Save, Save As,, About dialog<br><br>"

        "<br><b>Thank you for using KPad!</b>"
        ;

    QMessageBox::about(this, "About KPad+", aboutText);
}


// -------------------- Edit / Format --------------------
void Kpad::applyFont(const QString &font) {
    QTextCharFormat fmt;
    fmt.setFontFamily(font);
    applyFormatToSelection(fmt);
}

void Kpad::changeFontSizeDelta(int delta) {
    QTextCursor cursor = textEdit->textCursor();
    if (!cursor.hasSelection()) {
        // No selection → apply to typing cursor only
        QTextCharFormat format;
        int currentSize = textEdit->currentCharFormat().fontPointSize();
        if (currentSize <= 0)
            currentSize = textEdit->font().pointSize();

        int newSize = std::max(1, currentSize + delta);
        format.setFontPointSize(newSize);
        textEdit->mergeCurrentCharFormat(format);

        fontSizeBox->setCurrentText(QString::number(newSize));
        return;
    }

    // With selection → apply to whole range
    int minSize = INT_MAX, maxSize = 0;

    QTextCursor tempCursor = cursor;
    int start = cursor.selectionStart();
    int end   = cursor.selectionEnd();
    tempCursor.setPosition(start);

    while (tempCursor.position() < end) {
        QTextCharFormat fmt = tempCursor.charFormat();
        int size = fmt.fontPointSize();
        if (size > 0) {
            minSize = std::min(minSize, size);
            maxSize = std::max(maxSize, size);
        }
        tempCursor.movePosition(QTextCursor::NextCharacter);
    }

    // Pick a baseline size (e.g., use max size to keep things growing)
    int baseline = (maxSize > 0 ? maxSize : textEdit->font().pointSize());
    int newSize = std::max(1, baseline + delta);

    QTextCharFormat format;
    format.setFontPointSize(newSize);
    cursor.mergeCharFormat(format);

    fontSizeBox->setCurrentText(QString::number(newSize));
}

void Kpad::increaseFontSize() {
    changeFontSizeDelta(+1);
}

void Kpad::decreaseFontSize() {
    changeFontSizeDelta(-1);
}

// Toggle Bold for entire selection
void Kpad::setBold() {
    QTextCharFormat fmt;
    bool isBold = textEdit->fontWeight() == QFont::Bold;
    fmt.setFontWeight(isBold ? QFont::Normal : QFont::Bold);

    if (!extraCursors.isEmpty()) {
        applyFormatToCursors(fmt);
    } else {
        applyFormatToSelection(fmt);
    }
}

// Toggle Italic
void Kpad::setItalic() {
    QTextCharFormat fmt;
    bool isItalic = textEdit->fontItalic();
    fmt.setFontItalic(!isItalic);

    if (!extraCursors.isEmpty()) {
        applyFormatToCursors(fmt);
    } else {
        applyFormatToSelection(fmt);
    }
}

// Toggle Underline
void Kpad::setUnderline() {
    QTextCharFormat fmt;
    bool isUnderlined = textEdit->fontUnderline();
    fmt.setFontUnderline(!isUnderlined);

    if (!extraCursors.isEmpty()) {
        applyFormatToCursors(fmt);
    } else {
        applyFormatToSelection(fmt);
    }

}

void Kpad::applyFormatToSelection(const QTextCharFormat &format) {
    QTextCursor cursor = textEdit->textCursor();
    if (cursor.hasSelection()) {
        cursor.mergeCharFormat(format);  // applies only to highlighted text
    } else {
        textEdit->mergeCurrentCharFormat(format);  // applies to future typing
    }
}


void Kpad::alignLeft() { textEdit->setAlignment(Qt::AlignLeft); }
void Kpad::alignCenter() { textEdit->setAlignment(Qt::AlignCenter); }
void Kpad::alignRight() { textEdit->setAlignment(Qt::AlignRight); }

void Kpad::insertBulletList(const QString &style) {
    QTextCursor cursor = textEdit->textCursor();
    QTextListFormat listFormat;

    // Select list style
    if (style == "*")
        listFormat.setStyle(QTextListFormat::ListDisc);       // solid circle
    else if (style == "o")
        listFormat.setStyle(QTextListFormat::ListCircle);     // hollow circle
    else if (style == "-")
        listFormat.setStyle(QTextListFormat::ListSquare);     // square bullet
    else if (style == "1.")
        listFormat.setStyle(QTextListFormat::ListDecimal);    // 1, 2, 3...
    else if (style == "a.")
        listFormat.setStyle(QTextListFormat::ListLowerAlpha); // a, b, c...
    else if (style == "A.")
        listFormat.setStyle(QTextListFormat::ListUpperAlpha); // A, B, C...
    else if (style == "i.")
        listFormat.setStyle(QTextListFormat::ListLowerRoman); // i, ii, iii...
    else if (style == "I.")
        listFormat.setStyle(QTextListFormat::ListUpperRoman); // I, II, III...


    listFormat.setIndent(1);           // list level

    // Optional: adjust spacing between bullet and text
    QTextBlockFormat blockFormat = cursor.blockFormat();
    blockFormat.setTextIndent(10);    // space between bullet and text
    cursor.setBlockFormat(blockFormat);

    cursor.insertList(listFormat);
}


// -------------------- Word and Char Counter --------------------
void Kpad::updateCounts() {
    QTextCursor cursor = textEdit->textCursor();
    QString text;

    if (cursor.hasSelection()) {
        text = cursor.selectedText();
    } else {
        text = textEdit->toPlainText();
    }
    // Count characters:
    int charCount = text.length();
    // Count words (split by whitespace);
    int wordCount = text.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts).count();

    // Update labels:
    wordCountLabel->setText(QString("Words: %1").arg(wordCount));
    charCountLabel->setText(QString("Chars: %1").arg(charCount));
}

// -------------------- Zoom In/Out --------------------
void Kpad::zoomIn() {
    textEdit->zoomIn(1);  // argument = step (1 = +1 point size)
}

void Kpad::zoomOut() {
    textEdit->zoomOut(1); // argument = step
}

void Kpad::wheelEvent(QWheelEvent *event) {
    if(event->modifiers() & Qt::ControlModifier) {
        if(event->angleDelta().y() > 0) zoomIn();
        else zoomOut();
        event->accept();
    } else {
        QMainWindow::wheelEvent(event);
    }
}

// -------------------- Find Box --------------------
void Kpad::highlightMatches(const QString &pattern) {
    // First, clear all existing highlights
    QTextCursor cursor(textEdit->document());
    QTextCharFormat clearFormat;
    clearFormat.setBackground(Qt::transparent);
    cursor.select(QTextCursor::Document);
    cursor.mergeCharFormat(clearFormat);

    if (pattern.isEmpty()) return; // nothing to highlight

    QTextDocument *doc = textEdit->document();
    QTextCursor highlightCursor(doc);
    while (!highlightCursor.isNull() && !highlightCursor.atEnd()) {
        highlightCursor = doc->find(pattern, highlightCursor);
        if (!highlightCursor.isNull()) {
            highlightCursor.mergeCharFormat(highlightFormat);
        }
    }
}
// Replace your existing keyPressEvent method with this:
void Kpad::keyPressEvent(QKeyEvent *event) {
    QTextCursor mainCursor = textEdit->textCursor();

    // Multi-cursor: Alt + Up/Down
    if (event->modifiers() == Qt::AltModifier) {
        if (event->key() == Qt::Key_Up) {
            QTextCursor newCursor = mainCursor;
            if (newCursor.movePosition(QTextCursor::Up, QTextCursor::MoveAnchor)) {
                // Position cursor at same column as main cursor
                int column = mainCursor.columnNumber();
                newCursor.movePosition(QTextCursor::StartOfLine);
                for (int i = 0; i < column && !newCursor.atBlockEnd(); i++) {
                    newCursor.movePosition(QTextCursor::Right);
                }
                extraCursors.append(newCursor);
                textEdit->viewport()->update(); // Force repaint to show cursors
            }
            event->accept();
            return;
        }
        else if (event->key() == Qt::Key_Down) {
            QTextCursor newCursor = mainCursor;
            if (newCursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor)) {
                // Position cursor at same column as main cursor
                int column = mainCursor.columnNumber();
                newCursor.movePosition(QTextCursor::StartOfLine);
                for (int i = 0; i < column && !newCursor.atBlockEnd(); i++) {
                    newCursor.movePosition(QTextCursor::Right);
                }
                extraCursors.append(newCursor);
                textEdit->viewport()->update(); // Force repaint to show cursors
            }
            event->accept();
            return;
        }
    }

    // Escape to clear extra cursors
    if (event->key() == Qt::Key_Escape && !extraCursors.isEmpty()) {
        clearExtraCursors();
        event->accept();
        return;
    }

    // Handle text input for all cursors
    if (!extraCursors.isEmpty() && !event->text().isEmpty() && event->text().at(0).isPrint()) {
        insertTextAtCursors(event->text());
        event->accept();
        return;
    }

    // Handle special keys for all cursors
    if (!extraCursors.isEmpty()) {
        switch (event->key()) {
        case Qt::Key_Backspace:
            handleBackspaceAtCursors();
            event->accept();
            return;
        case Qt::Key_Delete:
            handleDeleteAtCursors();
            event->accept();
            return;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            insertTextAtCursors("\n");
            event->accept();
            return;
        case Qt::Key_Tab:
            insertTextAtCursors("\t");
            event->accept();
            return;
        }
    }

    // Handle Ctrl+V paste for multi-cursors
    if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_V) {
        const QClipboard *clipboard = QApplication::clipboard();
        QString plainText = clipboard->text();
        if (!extraCursors.isEmpty()) {
            insertTextAtCursors(plainText);
            event->accept();
            return;
        }
    }

    // Existing Ctrl+Shift+V paste without formatting
    if (event->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier) && event->key() == Qt::Key_V) {
        const QClipboard *clipboard = QApplication::clipboard();
        QString plainText = clipboard->text();
        if (!extraCursors.isEmpty()) {
            insertTextAtCursors(plainText);
        } else {
            textEdit->insertPlainText(plainText);
        }
        event->accept();
        return;
    }

    // For any other key that might clear cursors (arrow keys, etc.)
    if (!extraCursors.isEmpty() &&
        (event->key() == Qt::Key_Left || event->key() == Qt::Key_Right ||
         (event->key() == Qt::Key_Up && event->modifiers() != Qt::AltModifier) ||
         (event->key() == Qt::Key_Down && event->modifiers() != Qt::AltModifier) ||
         (event->modifiers() & Qt::ControlModifier))) {
        clearExtraCursors();
    }

    // Default behavior
    QMainWindow::keyPressEvent(event);
}

// Replace your existing insertTextAtCursors method:
void Kpad::insertTextAtCursors(const QString &text) {
    if (extraCursors.isEmpty()) {
        // No extra cursors, insert normally
        QTextCursor mainCursor = textEdit->textCursor();
        mainCursor.insertText(text);
        textEdit->setTextCursor(mainCursor);
        return;
    }

    // Sort cursors by position (backwards) to avoid position shifts
    QVector<QTextCursor> allCursors = extraCursors;
    allCursors.append(textEdit->textCursor());

    std::sort(allCursors.begin(), allCursors.end(),
              [](const QTextCursor &a, const QTextCursor &b) {
                  return a.position() > b.position();
              });

    // Insert text at each cursor position
    for (QTextCursor &cursor : allCursors) {
        cursor.insertText(text);
    }

    // Update main cursor to the last one (original position)
    textEdit->setTextCursor(allCursors.last());

    // Update extra cursors positions
    extraCursors.clear();
    for (int i = 0; i < allCursors.size() - 1; i++) {
        extraCursors.append(allCursors[i]);
    }

    textEdit->viewport()->update();
}

// Replace your existing applyFormatToCursors method:
void Kpad::applyFormatToCursors(const QTextCharFormat &format) {
    if (extraCursors.isEmpty()) {
        // No extra cursors, apply normally
        QTextCursor mainCursor = textEdit->textCursor();
        if (mainCursor.hasSelection()) {
            mainCursor.mergeCharFormat(format);
        } else {
            textEdit->mergeCurrentCharFormat(format);
        }
        return;
    }

    // Apply to main cursor
    QTextCursor mainCursor = textEdit->textCursor();
    if (mainCursor.hasSelection()) {
        mainCursor.mergeCharFormat(format);
    } else {
        textEdit->mergeCurrentCharFormat(format);
    }

    // Apply to extra cursors
    for (QTextCursor &cursor : extraCursors) {
        if (cursor.hasSelection()) {
            cursor.mergeCharFormat(format);
        } else {
            // For non-selection cursors, apply to future typing
            QTextCharFormat currentFormat = cursor.charFormat();
            currentFormat.merge(format);
            cursor.setCharFormat(currentFormat);
        }
    }
}

// Add these new methods to your .cpp file:
void Kpad::clearExtraCursors() {
    extraCursors.clear();
    textEdit->viewport()->update();
}

void Kpad::handleBackspaceAtCursors() {
    QVector<QTextCursor> allCursors = extraCursors;
    allCursors.append(textEdit->textCursor());

    // Sort by position (backwards) to avoid position shifts
    std::sort(allCursors.begin(), allCursors.end(),
              [](const QTextCursor &a, const QTextCursor &b) {
                  return a.position() > b.position();
              });

    for (QTextCursor &cursor : allCursors) {
        if (cursor.hasSelection()) {
            cursor.removeSelectedText();
        } else if (!cursor.atStart()) {
            cursor.deletePreviousChar();
        }
    }

    // Update cursors
    textEdit->setTextCursor(allCursors.last());
    extraCursors.clear();
    for (int i = 0; i < allCursors.size() - 1; i++) {
        extraCursors.append(allCursors[i]);
    }

    textEdit->viewport()->update();
}

void Kpad::handleDeleteAtCursors() {
    QVector<QTextCursor> allCursors = extraCursors;
    allCursors.append(textEdit->textCursor());

    // Sort by position (backwards)
    std::sort(allCursors.begin(), allCursors.end(),
              [](const QTextCursor &a, const QTextCursor &b) {
                  return a.position() > b.position();
              });

    for (QTextCursor &cursor : allCursors) {
        if (cursor.hasSelection()) {
            cursor.removeSelectedText();
        } else if (!cursor.atEnd()) {
            cursor.deleteChar();
        }
    }

    // Update cursors
    textEdit->setTextCursor(allCursors.last());
    extraCursors.clear();
    for (int i = 0; i < allCursors.size() - 1; i++) {
        extraCursors.append(allCursors[i]);
    }

    textEdit->viewport()->update();
}

// Add this eventFilter method to draw the extra cursors:
bool Kpad::eventFilter(QObject *obj, QEvent *event) {
    if (obj == textEdit->viewport() && event->type() == QEvent::Paint && !extraCursors.isEmpty()) {
        // Let the original paint event happen first
        bool result = QMainWindow::eventFilter(obj, event);

        // Now draw extra cursors
        QPainter painter(textEdit->viewport());
        painter.setPen(QPen(Qt::black, 2)); // Black cursor, 2px thick

        for (const QTextCursor &cursor : extraCursors) {
            QRect cursorRect = textEdit->cursorRect(cursor);
            if (cursorRect.isValid()) {
                // Draw a vertical line for the cursor
                painter.drawLine(cursorRect.topLeft(), cursorRect.bottomLeft());
            }
        }
        return result;
    }
    return QMainWindow::eventFilter(obj, event);
}
