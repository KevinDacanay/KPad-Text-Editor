#include "kpad.h"
#include "ui_kpad.h"

// --------------------
// Zoom In/Out
// --------------------
void Kpad::zoomIn() {
    textEdit->zoomIn(1);
}

void Kpad::zoomOut() {
    textEdit->zoomOut(1);
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

// --------------------
// Dark/Light Theme
// --------------------
void Kpad::toggleTheme() {
    darkMode = !darkMode;
    // Get current cursor and format
    QTextCursor cursor = textEdit->textCursor();

    if (darkMode) {
        QString darkStyle = R"(
            QMainWindow, QWidget {
                background-color: #2b2b2b;
                color: #ffffff;
            }
            QMenuBar {
                background-color: #353535;
                color: #ffffff;
            }
            QMenuBar::item:selected {
                background: #3e3e3e;
            }
            QMenu {
                background-color: #2b2b2b;
                color: #ffffff;
            }
            QMenu::item:selected {
                background: #3e3e3e;
            }
            QTextEdit {
                background-color: #1e1e1e;
                color: #ffffff;
                selection-background-color: #0078d7;
            }
            QLineEdit, QComboBox, QSpinBox {
                background-color: #3c3c3c;
                color: #ffffff;
                border: 1px solid #555;
            }
            QPushButton {
                background-color: #3c3c3c;
                color: #ffffff;
                border: 1px solid #555;
                padding: 4px;
            }
            QPushButton:hover {
                background-color: #444444;
            }
            QMessageBox {
                background-color: #2b2b2b;
                color: #ffffff;
            }
            QStatusBar {
                background-color: #353535;
                color: #ffffff;
            }
        )";
        qApp->setStyleSheet(darkStyle);

        // Set default text color to white for dark mode
        textEdit->setTextColor(QColor("#ffffff"));

    } else {
        qApp->setStyleSheet(""); // Reset to default light theme

        // Set default text color to black for light mode
        textEdit->setTextColor(QColor("#000000"));
    }

    // Update the current character format for future typing
    QTextCharFormat fmt;
    fmt.setForeground(darkMode ? QColor("#ffffff") : QColor("#000000"));
    textEdit->mergeCurrentCharFormat(fmt);

    updateIconColors();
}

void Kpad::updateIconColors() {
    QList<QAction*> allActions = findChildren<QAction*>();

    for (QAction *action : allActions) {
        if (originalIcons.contains(action)) {
            if (darkMode) {
                // Dark mode: invert the ORIGINAL icon
                QIcon originalIcon = originalIcons[action];
                QPixmap pixmap = originalIcon.pixmap(QSize(16, 16));
                QImage image = pixmap.toImage();
                image.invertPixels();
                action->setIcon(QIcon(QPixmap::fromImage(image)));
            } else {
                // Light mode: restore the ORIGINAL icon
                action->setIcon(originalIcons[action]);
            }
        }
    }
}
