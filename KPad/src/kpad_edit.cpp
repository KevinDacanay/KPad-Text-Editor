#include "kpad.h"
#include "ui_kpad.h"
#include <climits>

// --------------------
// Edit / Format
// --------------------
void Kpad::applyFont(const QString &font) {
    QTextCharFormat fmt;
    fmt.setFontFamily(font);
    applyFormatToSelection(fmt);
}

void Kpad::changeFontSizeDelta(int delta) {
    QTextCursor cursor = textEdit->textCursor();
    if (!cursor.hasSelection()) {
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

    int start = cursor.selectionStart();
    int end = cursor.selectionEnd();
    QTextCursor tempCursor = textEdit->textCursor();

    tempCursor.beginEditBlock();
    tempCursor.setPosition(start);

    while (tempCursor.position() < end) {
        tempCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
        QTextCharFormat fmt = tempCursor.charFormat();

        int size = fmt.fontPointSize();
        if (size <= 0)
            size = textEdit->font().pointSize();

        fmt.setFontPointSize(std::max(1, size + delta));
        tempCursor.mergeCharFormat(fmt);

        tempCursor.clearSelection();
    }

    tempCursor.endEditBlock();

    int displaySize = textEdit->currentCharFormat().fontPointSize();
    if (displaySize <= 0)
        displaySize = textEdit->font().pointSize();
    fontSizeBox->setCurrentText(QString::number(displaySize));
}

void Kpad::increaseFontSize() {
    changeFontSizeDelta(+1);
}

void Kpad::decreaseFontSize() {
    changeFontSizeDelta(-1);
}

// Toggle Bold
void Kpad::setBold() {
    QTextCharFormat fmt;
    bool isBold = textEdit->fontWeight() == QFont::Bold;
    fmt.setFontWeight(isBold ? QFont::Normal : QFont::Bold);
    applyFormatToSelection(fmt);
}

// Toggle Italic
void Kpad::setItalic() {
    QTextCharFormat fmt;
    bool isItalic = textEdit->fontItalic();
    fmt.setFontItalic(!isItalic);
    applyFormatToSelection(fmt);
}

// Toggle Underline
void Kpad::setUnderline() {
    QTextCharFormat fmt;
    bool isUnderlined = textEdit->fontUnderline();
    fmt.setFontUnderline(!isUnderlined);
    applyFormatToSelection(fmt);
}

// Apply formatting to current selection or typing position
void Kpad::applyFormatToSelection(const QTextCharFormat &format) {
    QTextCursor cursor = textEdit->textCursor();
    if (cursor.hasSelection()) {
        cursor.mergeCharFormat(format);
    } else {
        textEdit->mergeCurrentCharFormat(format);
    }
}

// Highlight text with color picker:
void Kpad::toggleHighlight(bool enabled)
{
    QTextCursor cursor = textEdit->textCursor();

    // -----------------------------
    // CASE 1: Selected text
    // -----------------------------
    if (cursor.hasSelection()) {
        if (enabled) {
            // Open color picker dialog
            QColor color = QColorDialog::getColor(QColor("#fff59d"), this, "Choose Highlight Color");

            // If user cancelled, uncheck the button and return
            if (!color.isValid()) {
                ui->actionHighlight->setChecked(false);
                return;
            }

            // Get the current format and modify only the background
            QTextCharFormat currentFmt = cursor.charFormat();
            currentFmt.setBackground(color);

            // Apply format to the selected text
            cursor.setCharFormat(currentFmt);
        } else {
            // Remove highlight
            QTextCharFormat currentFmt = cursor.charFormat();
            currentFmt.setBackground(Qt::transparent);
            cursor.setCharFormat(currentFmt);
        }

        // Clear the selection to see the highlight
        int pos = cursor.selectionEnd();
        cursor.clearSelection();
        cursor.setPosition(pos);
        textEdit->setTextCursor(cursor);

        // DO NOT keep highlight mode enabled after selection
        ui->actionHighlight->setChecked(false);
        return;
    }

    // -----------------------------
    // CASE 2: No selection → typing mode highlight
    // -----------------------------
    if (enabled) {
        // Open color picker dialog
        QColor color = QColorDialog::getColor(QColor("#fff59d"), this, "Choose Highlight Color");

        // If user cancelled, uncheck the button and return
        if (!color.isValid()) {
            ui->actionHighlight->setChecked(false);
            return;
        }

        QTextCharFormat fmt;
        fmt.setBackground(color);
        textEdit->mergeCurrentCharFormat(fmt);
    } else {
        // Turn off highlight for typing
        QTextCharFormat clearFmt;
        clearFmt.setBackground(Qt::transparent);
        textEdit->mergeCurrentCharFormat(clearFmt);
    }
}

// Change font color with color picker:
void Kpad::toggleTextColor(bool enabled)
{
    QTextCursor cursor = textEdit->textCursor();

    // -----------------------------
    // CASE 1: Selected text
    // -----------------------------
    if (cursor.hasSelection()) {
        if (enabled) {
            // Open color picker dialog
            QColor color = QColorDialog::getColor(QColor("#000000"), this, "Choose Text Color");

            // If user cancelled, uncheck the button and return
            if (!color.isValid()) {
                ui->actionTextColor->setChecked(false);
                return;
            }

            // Get the current format and modify only the foreground color
            QTextCharFormat currentFmt = cursor.charFormat();
            currentFmt.setForeground(color);

            // Apply format to the selected text
            cursor.setCharFormat(currentFmt);
        } else {
            // Reset to default black
            QTextCharFormat currentFmt = cursor.charFormat();
            currentFmt.setForeground(QColor("#000000"));
            cursor.setCharFormat(currentFmt);
        }

        // Clear the selection to see the color change
        int pos = cursor.selectionEnd();
        cursor.clearSelection();
        cursor.setPosition(pos);
        textEdit->setTextCursor(cursor);

        // DO NOT keep font color mode enabled after selection
        ui->actionTextColor->setChecked(false);
        return;
    }

    // -----------------------------
    // CASE 2: No selection → typing mode font color
    // -----------------------------
    if (enabled) {
        // Open color picker dialog
        QColor color = QColorDialog::getColor(QColor("#000000"), this, "Choose Font Color");

        // If user cancelled, uncheck the button and return
        if (!color.isValid()) {
            ui->actionTextColor->setChecked(false);
            return;
        }

        QTextCharFormat fmt;
        fmt.setForeground(color);
        textEdit->mergeCurrentCharFormat(fmt);
    } else {
        // Turn off custom color for typing (reset to black)
        QTextCharFormat clearFmt;
        clearFmt.setForeground(QColor("#000000"));
        textEdit->mergeCurrentCharFormat(clearFmt);
    }
}

// Alignment functions
void Kpad::alignLeft()   { textEdit->setAlignment(Qt::AlignLeft); }
void Kpad::alignCenter() { textEdit->setAlignment(Qt::AlignCenter); }
void Kpad::alignRight()  { textEdit->setAlignment(Qt::AlignRight); }

// Insert bullet or numbered list
void Kpad::insertBulletList(const QString &style) {
    QTextCursor cursor = textEdit->textCursor();
    QTextListFormat listFormat;

    if (style == "*")
        listFormat.setStyle(QTextListFormat::ListDisc);
    else if (style == "o")
        listFormat.setStyle(QTextListFormat::ListCircle);
    else if (style == "-")
        listFormat.setStyle(QTextListFormat::ListSquare);
    else if (style == "1.")
        listFormat.setStyle(QTextListFormat::ListDecimal);
    else if (style == "a.")
        listFormat.setStyle(QTextListFormat::ListLowerAlpha);
    else if (style == "A.")
        listFormat.setStyle(QTextListFormat::ListUpperAlpha);
    else if (style == "i.")
        listFormat.setStyle(QTextListFormat::ListLowerRoman);
    else if (style == "I.")
        listFormat.setStyle(QTextListFormat::ListUpperRoman);

    listFormat.setIndent(1);

    QTextBlockFormat blockFormat = cursor.blockFormat();
    blockFormat.setTextIndent(10); // space between bullet and text
    cursor.setBlockFormat(blockFormat);

    cursor.insertList(listFormat);
}
