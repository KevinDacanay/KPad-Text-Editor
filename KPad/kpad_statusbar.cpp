#include "kpad.h"
#include "ui_kpad.h"

// --------------------
// Find Box
// --------------------
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

// --------------------
// Word and Char Counter
// --------------------
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

// --------------------
// Toggle Window Lock
// --------------------
void Kpad::toggleWindowLock(bool locked)
{
    isWindowLocked = locked;

    if (locked) {
        // Lock the window size
        lockedSize = size(); // Save current size
        setFixedSize(lockedSize); // Prevent resizing

        // Update button appearance
        // lockButton->setIcon(":/icons/locked.png"); // Locked emoji (or use "Locked")
        lockButton->setText("Locked");
        lockButton->setToolTip("Unlock window size");

    } else {
        // Unlock the window size
        setMinimumSize(0, 0); // Remove minimum size constraint
        setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX); // Remove maximum size constraint

        // Update button appearance
        // lockButton->setIcon(":/icons/unlocked.png"); // Unlocked emoji (or use "Unlock")
        lockButton->setText("Unlocked");
        lockButton->setToolTip("Lock window size");
    }
}
