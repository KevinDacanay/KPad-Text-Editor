#include "kpad.h"
#include "ui_kpad.h"

// --------------------
// File Actions
// --------------------
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

bool Kpad::saveFile(const QString &filePath, QTextEdit *editor) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);
    out << editor->toPlainText();
    editor->document()->setModified(false);
    return true;
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

