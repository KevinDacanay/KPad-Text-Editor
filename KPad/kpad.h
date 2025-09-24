#ifndef KPAD_H
#define KPAD_H

#include <QMainWindow>
#include <QIcon>
#include <QTextEdit>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QToolBar>
#include <QComboBox>
#include <QStatusBar>
#include <QFont>
#include <QFontComboBox>
#include <QLabel>
#include <QRegularExpression>
#include <QWheelEvent>
#include <QTextCharFormat>
#include <QPushButton>
#include <QKeyEvent>
#include <QTextCursor>
#include <QTextList>
#include <QTextListFormat>
#include <QTextBlockFormat>
#include <QLineEdit>
#include <QClipboard>
#include <QTextCharFormat>
#include <QVector>
#include <QPainter>
#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui { class Kpad; }
QT_END_NAMESPACE

class Kpad : public QMainWindow
{
    Q_OBJECT

public:
    explicit Kpad(QWidget *parent = nullptr);
    ~Kpad();

private slots:
    // File Actions
    void open();
    void newDocument();
    void save();
    void saveAs();
    void saveAsHTML();
    void exit();

    // About Dialog
    void showAbout();

    // Edit / Format Actions
    void applyFont(const QString &font);
    void increaseFontSize();
    void decreaseFontSize();
    void setBold();
    void setItalic();
    void setUnderline();
    void alignLeft();
    void alignCenter();
    void alignRight();
    void insertBulletList(const QString &style);
    void applyFormatToSelection(const QTextCharFormat &format);
    void changeFontSizeDelta(int delta);


    // Word and Char count:
    void updateCounts();

    // Zoom In/Out:
    void zoomIn();
    void zoomOut();

    // Find Box:
    void highlightMatches(const QString &pattern);

    // Multi Cursor:
    void applyFormatToCursors(const QTextCharFormat &format);
    void insertTextAtCursors(const QString &text);
    void clearExtraCursors();
    void handleBackspaceAtCursors();
    void handleDeleteAtCursors();


protected:
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;


private:
    Ui::Kpad *ui;                   // Pointer to the Qt UI
    QString currentFile;            // Stores current file path
    QComboBox *fontSizeBox;         // Dropdown for font sizes
    QTextEdit *textEdit;            // Main text editing area
    QFontComboBox *fontFamilyBox;   // Dropdown for font styles
    QLabel *wordCountLabel;         // For word counter
    QLabel *charCountLabel;         // Fpr char counter
    QPushButton *zoomInButton;
    QPushButton *zoomOutButton;
    QLineEdit *findBox;
    QTextCharFormat highlightFormat;
    QLineEdit *findLineEdit;
    QPushButton *findNextButton;
    QPushButton *findPrevButton;
    QTextCursor lastCursor;         // remember last match for navigation
    QVector<QTextCursor> extraCursors;  // multicursor storage

    bool maybeSave();  // Helper function to handle save logic
    bool hasUnsavedChanges();  // Check if document has unsaved changes

};

#endif // KPAD_H
