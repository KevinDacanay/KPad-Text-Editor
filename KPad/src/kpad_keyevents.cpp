#include "kpad.h"
#include "ui_kpad.h"
#include <QClipboard>
#include <QApplication>
#include <QKeyEvent>
#include <QDebug>

// ------------
// Key Event Handler
// ------------
void Kpad::keyPressEvent(QKeyEvent *event) {
    QTextCursor cursor = textEdit->textCursor();

    // Arrow Down: move down; if on last line, move to end of line
    if (event->key() == Qt::Key_Down && event->modifiers() == Qt::NoModifier) {
        QTextBlock currentBlock = cursor.block();
        QTextBlock nextBlock = currentBlock.next();

        if (!nextBlock.isValid()) {
            // We're already on the last block
            int currentPos = cursor.positionInBlock();
            int endPos = currentBlock.length() - 1; // -1 because last char is '\n'

            if (currentPos < endPos) {
                // Move to the end of this last line
                cursor.movePosition(QTextCursor::EndOfBlock);
                textEdit->setTextCursor(cursor);
                event->accept();
                return;
            }
        }

        // Otherwise, move down normally
        cursor.movePosition(QTextCursor::Down);
        textEdit->setTextCursor(cursor);
        event->accept();
        return;
    }



    // ------------
    // De-indent with Shift+Tab
    // ------------
    if (event->key() == Qt::Key_Backtab ||
        (event->key() == Qt::Key_Tab && event->modifiers() == Qt::ShiftModifier)) {
        event->accept(); // Prevent focus change
        QTextCursor cursor = textEdit->textCursor();
        cursor.beginEditBlock();

        if (cursor.hasSelection()) {
            int start = cursor.selectionStart();
            int end = cursor.selectionEnd();
            cursor.setPosition(start);
            while (cursor.position() < end) {
                cursor.movePosition(QTextCursor::StartOfLine);
                // Try to remove a leading tab first
                cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 1);
                QString firstChar = cursor.selectedText();
                if (firstChar == "\t") {
                    cursor.removeSelectedText();
                } else {
                    // Not a tab — remove up to 4 leading spaces if present
                    cursor.clearSelection();
                    cursor.movePosition(QTextCursor::StartOfLine);
                    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 4);
                    QString startChunk = cursor.selectedText();
                    int toRemove = 0;
                    for (int i = 0; i < startChunk.size() && i < 4; ++i) {
                        if (startChunk[i] == ' ')
                            ++toRemove;
                        else
                            break;
                    }
                    if (toRemove > 0) {
                        cursor.clearSelection();
                        cursor.movePosition(QTextCursor::StartOfLine);
                        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, toRemove);
                        cursor.removeSelectedText();
                    }
                }
                cursor.movePosition(QTextCursor::Down);
            }
        } else {
            // No selection: de-indent current line and preserve cursor position
            int originalColumn = cursor.columnNumber();  // Save column position

            cursor.movePosition(QTextCursor::StartOfLine);
            cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 1);
            int charsRemoved = 0;

            if (cursor.selectedText() == "\t") {
                cursor.removeSelectedText();
                charsRemoved = 1;
            } else {
                cursor.clearSelection();
                cursor.movePosition(QTextCursor::StartOfLine);
                cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 4);
                QString chunk = cursor.selectedText();
                int toRemove = 0;
                for (int i = 0; i < chunk.size() && i < 4; ++i) {
                    if (chunk[i] == ' ')
                        ++toRemove;
                    else
                        break;
                }
                if (toRemove > 0) {
                    cursor.clearSelection();
                    cursor.movePosition(QTextCursor::StartOfLine);
                    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, toRemove);
                    cursor.removeSelectedText();
                    charsRemoved = toRemove;
                }
            }

            // Restore cursor position, adjusted for removed characters
            cursor.movePosition(QTextCursor::StartOfLine);
            int newColumn = qMax(0, originalColumn - charsRemoved);
            for (int i = 0; i < newColumn && !cursor.atBlockEnd(); ++i) {
                cursor.movePosition(QTextCursor::NextCharacter);
            }
        }

        cursor.endEditBlock();
        textEdit->setTextCursor(cursor);
        event->accept();
        return;
    }

    // ------------
    // Indent with Tab
    // ------------
    if (event->key() == Qt::Key_Tab && event->modifiers() == Qt::NoModifier) {
        cursor.beginEditBlock();

        if (cursor.hasSelection()) {
            // If there's a selection, indent all selected lines
            const int selStart = cursor.selectionStart();
            const int selEnd = cursor.selectionEnd();
            cursor.setPosition(selStart);

            // iterate per line in selection
            while (cursor.position() < selEnd) {
                cursor.movePosition(QTextCursor::StartOfLine);
                cursor.insertText("\t");
                cursor.movePosition(QTextCursor::Down);
            }
        } else {
            int columnPos = cursor.columnNumber();
            QTextBlock currentBlock = cursor.block();

            if (columnPos == 0) {
                // --- Cursor at beginning of line: indent the entire line ---
                QTextList *list = currentBlock.textList();
                if (list) {
                    QTextListFormat fmt = list->format();
                    int oldIndent = fmt.indent();
                    list->remove(currentBlock);

                    QTextListFormat newFmt = fmt;
                    newFmt.setIndent(oldIndent + 1);
                    QTextCursor blockCursor(currentBlock);
                    blockCursor.createList(newFmt);
                } else {
                    cursor.insertText("\t");
                }
            } else {
                // --- Cursor mid-line: insert tab at cursor only ---
                cursor.insertText("\t");
            }
        }

        cursor.endEditBlock();
        event->accept();
        return;
    }

    // ------------
    // Ctrl+Shift+V: paste plain text
    // ------------
    if (event->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier) && event->key() == Qt::Key_V) {
        const QClipboard *clipboard = QApplication::clipboard();
        QString plainText = clipboard->text();
        textEdit->insertPlainText(plainText);
        event->accept();
        return;
    }

    // ------------
    // Ctrl+V: default paste
    // ------------
    if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_V) {
        // Let QTextEdit's default handling occur (paste with formatting)
        QMainWindow::keyPressEvent(event);
        return;
    }

    // ------------
    // Ctrl+Shift+Enter: Create new line above current line
    // ------------
    if (event->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier) &&
        (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)) {
        cursor.movePosition(QTextCursor::StartOfLine);
        cursor.insertText("\n");
        cursor.movePosition(QTextCursor::Up);
        textEdit->setTextCursor(cursor);
        event->accept();
        return;
    }

    // ------------
    // Ctrl+Enter: Create new line below current line
    // ------------
    if (event->modifiers() == Qt::ControlModifier &&
        (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)) {
        cursor.movePosition(QTextCursor::EndOfLine);
        cursor.insertText("\n");
        textEdit->setTextCursor(cursor);
        event->accept();
        return;
    }

    // Base class (default)
    QMainWindow::keyPressEvent(event);
}

bool Kpad::eventFilter(QObject *obj, QEvent *event) {
    if (obj == textEdit && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        QTextCursor cursor = textEdit->textCursor();

        // --- Auto bullet / list detection ---
        if (keyEvent->key() == Qt::Key_Space && keyEvent->modifiers() == Qt::NoModifier) {
            QTextBlock block = cursor.block();
            QString text = block.text();

            int cursorPosInBlock = cursor.position() - block.position();
            QString textBeforeCursor = text.left(cursorPosInBlock);
            QString textAfterCursor = text.mid(cursorPosInBlock);  // Get text after cursor
            QString trimmed = textBeforeCursor.trimmed();

            if ((trimmed == "-" || trimmed == "*")) {
                int indentLevel = 0;
                for (QChar c : textBeforeCursor) {
                    if (c == ' ') indentLevel++;
                    else if (c == '\t') indentLevel += 4;
                    else break;
                }

                QTextListFormat listFmt;
                listFmt.setStyle(trimmed == "-" ? QTextListFormat::ListDisc : QTextListFormat::ListCircle);
                listFmt.setIndent(indentLevel / 4 + 1);

                cursor.beginEditBlock();

                // Save the text after the "- " or "* "
                QString contentToKeep = textAfterCursor;

                // Clear the ENTIRE line
                cursor.movePosition(QTextCursor::StartOfBlock);
                cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
                cursor.removeSelectedText();

                // Insert the content back first (so the block has content)
                if (!contentToKeep.isEmpty()) {
                    cursor.insertText(contentToKeep);
                }

                // Move cursor back to the start of the block to apply list formatting
                cursor.movePosition(QTextCursor::StartOfBlock);

                // Convert this block to a list item
                cursor.createList(listFmt);

                // Move cursor to end of the content
                cursor.movePosition(QTextCursor::EndOfBlock);

                cursor.endEditBlock();

                textEdit->setTextCursor(cursor);
                lastAutoBullet = true;
                lastBulletChar = trimmed;
                return true;
            }
        }

        // --- Undo auto-bullet with Backspace ---
        if (keyEvent->key() == Qt::Key_Backspace && keyEvent->modifiers() == Qt::NoModifier && lastAutoBullet) {
            QTextList *list = cursor.currentList();
            QTextBlock block = cursor.block();

            if (list && cursor.atBlockStart() && block.text().isEmpty()) {
                cursor.beginEditBlock();

                int listIndent = list->format().indent();
                list->remove(block);

                QTextBlockFormat fmt;
                fmt.setIndent(0);
                cursor.setBlockFormat(fmt);

                QString indent = QString(" ").repeated((listIndent - 1) * 4);
                cursor.insertText(indent + lastBulletChar + " ");

                cursor.endEditBlock();
                textEdit->setTextCursor(cursor);

                lastAutoBullet = false;
                return true;
            }
            lastAutoBullet = false;
        }

        // --- Continue or stop bullet list on Enter ---
        if ((keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) &&
            keyEvent->modifiers() == Qt::NoModifier) {

            QTextList *list = cursor.currentList();
            if (list) {
                QTextBlock block = cursor.block();

                if (block.text().isEmpty()) {
                    cursor.beginEditBlock();
                    list->remove(block);

                    QTextBlockFormat fmt;
                    fmt.setIndent(0);
                    cursor.setBlockFormat(fmt);

                    cursor.endEditBlock();
                    textEdit->setTextCursor(cursor);
                    lastAutoBullet = false;
                    return true;
                } else {
                    lastAutoBullet = false;
                }
            }
        }

        // --- Indent / Outdent bullet lists (Tab / Shift+Tab) ---
        if (keyEvent->key() == Qt::Key_Tab || keyEvent->key() == Qt::Key_Backtab) {
            const bool isBacktab =
                (keyEvent->key() == Qt::Key_Backtab) ||
                (keyEvent->key() == Qt::Key_Tab && keyEvent->modifiers() == Qt::ShiftModifier);

            if (isBacktab || keyEvent->modifiers() == Qt::NoModifier) {
                cursor.beginEditBlock();

                QList<QTextBlock> blocksToProcess;

                // Collect all blocks in selection (or just current block if no selection)
                if (cursor.hasSelection()) {
                    int selStart = cursor.selectionStart();
                    int selEnd = cursor.selectionEnd();

                    QTextBlock block = textEdit->document()->findBlock(selStart);
                    QTextBlock endBlock = textEdit->document()->findBlock(selEnd);

                    while (block.isValid()) {
                        blocksToProcess.append(block);
                        if (block == endBlock) break;
                        block = block.next();
                    }
                } else {
                    blocksToProcess.append(cursor.block());
                }

                // Process each block independently
                for (const QTextBlock &block : blocksToProcess) {
                    QTextList *list = block.textList();
                    QTextCursor blockCursor(block);

                    if (!isBacktab) {
                        // ----- INDENT (Tab) -----
                        if (list) {
                            // It's a list item: increase indent level
                            QTextListFormat fmt = list->format();
                            int oldIndent = fmt.indent();
                            list->remove(block);

                            QTextListFormat newFmt = fmt;
                            newFmt.setIndent(oldIndent + 1);
                            blockCursor.createList(newFmt);
                        } else {
                            // Not a list item: insert tab at block start
                            blockCursor.movePosition(QTextCursor::StartOfBlock);
                            blockCursor.insertText("\t");
                        }
                    } else {
                        // ----- OUTDENT (Shift+Tab) -----
                        if (list) {
                            // It's a list item: decrease indent level
                            QTextListFormat fmt = list->format();
                            int oldIndent = fmt.indent();
                            list->remove(block);

                            if (oldIndent > 1) {
                                QTextListFormat newFmt = fmt;
                                newFmt.setIndent(oldIndent - 1);
                                blockCursor.createList(newFmt);
                            } else {
                                // At base indentation → remove list formatting entirely
                                QTextBlockFormat blockFmt;
                                blockFmt.setIndent(0);
                                blockCursor.setBlockFormat(blockFmt);
                            }
                        } else {
                            // Non-list line: remove one tab or up to 4 spaces
                            blockCursor.movePosition(QTextCursor::StartOfBlock);
                            blockCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 1);
                            QString first = blockCursor.selectedText();
                            if (first == "\t") {
                                blockCursor.removeSelectedText();
                            } else {
                                blockCursor.clearSelection();
                                blockCursor.movePosition(QTextCursor::StartOfBlock);
                                blockCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 4);
                                QString chunk = blockCursor.selectedText();
                                int toRemove = 0;
                                for (int i = 0; i < chunk.size() && i < 4; ++i) {
                                    if (chunk[i] == ' ')
                                        ++toRemove;
                                    else
                                        break;
                                }
                                if (toRemove > 0) {
                                    blockCursor.clearSelection();
                                    blockCursor.movePosition(QTextCursor::StartOfBlock);
                                    blockCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, toRemove);
                                    blockCursor.removeSelectedText();
                                }
                            }
                        }
                    }
                }

                cursor.endEditBlock();
                return true;
            }
        }


        // --- ARROW DOWN behavior enhancement ---
        if (keyEvent->key() == Qt::Key_Down && keyEvent->modifiers() == Qt::NoModifier) {
            QTextCursor tempCursor = cursor;
            QTextBlock currentBlock = tempCursor.block();
            QTextBlock nextBlock = currentBlock.next();

            if (nextBlock.isValid()) {
                // Normal behavior
                tempCursor.movePosition(QTextCursor::Down);
            } else {
                // Already on last line: move to END of line
                QTextLayout *layout = currentBlock.layout();
                if (layout && layout->lineCount() > 0) {
                    QTextLine lastLine = layout->lineAt(layout->lineCount() - 1);

                    // Check if cursor is not already at the end of text
                    int endPos = currentBlock.position() + lastLine.textLength();
                    if (cursor.position() < endPos) {
                        tempCursor.movePosition(QTextCursor::EndOfBlock);
                    } else {
                        QApplication::beep(); // optional feedback
                    }
                } else {
                    tempCursor.movePosition(QTextCursor::EndOfBlock);
                }
            }

            textEdit->setTextCursor(tempCursor);
            event->accept();
            return true;
        }

        // Reset bullet flag when user types other keys
        if (lastAutoBullet && keyEvent->key() != Qt::Key_Backspace && keyEvent->key() != Qt::Key_Shift &&
            keyEvent->key() != Qt::Key_Control && keyEvent->key() != Qt::Key_Alt) {
            lastAutoBullet = false;
        }
    }

    return QMainWindow::eventFilter(obj, event);
}

