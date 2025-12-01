#ifndef KPAD_H
#define KPAD_H

#include <QMainWindow>
#include <QIcon>
#include <QTextEdit>
#include <QFileDialog>
#include <QFile>
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
#include <QVector>
#include <QPainter>
#include <QDebug>
#include <QProcess>
#include <QCloseEvent>
#include <QColorDialog>
#include <QMap>

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
    bool saveFile(const QString &filePath, QTextEdit *editor);

    // About Dialog
    void showAbout();

    // Edit / Format Actions
    void applyFont(const QString &font);
    void increaseFontSize();
    void decreaseFontSize();
    void setBold();
    void setItalic();
    void setUnderline();
    void toggleHighlight(bool enabled);
    void toggleTextColor(bool enabled);
    void alignLeft();
    void alignCenter();
    void alignRight();
    void insertBulletList(const QString &style);
    void applyFormatToSelection(const QTextCharFormat &format);
    void changeFontSizeDelta(int delta);

    void updateCounts();                            // Word and Char count:
    void zoomIn();
    void zoomOut();
    void highlightMatches(const QString &pattern);  // Find Text Box
    void toggleWindowLock(bool locked);
    void toggleTheme();
    void updateIconColors();                        // for icon color changes in dark mode


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
    QLabel *charCountLabel;         // For char counter
    QPushButton *zoomInButton;
    QPushButton *zoomOutButton;
    QLineEdit *findBox;
    QTextCharFormat highlightFormat;
    QLineEdit *findLineEdit;
    QPushButton *findNextButton;
    QPushButton *findPrevButton;
    QTextCursor lastCursor;         // remember last match for navigation
    QString lastBulletChar = "";    // For automatic bullet points
    QToolButton *lockSizeButton;    // For Window Size Lock
    QSize lockedSize;
    QPushButton *lockButton;
    QMap<QAction*, QIcon> originalIcons; // Store original icons

    bool maybeSave();               // Helper function to handle save logic
    bool hasUnsavedChanges();       // Check if document has unsaved changes
    bool darkMode = false;
    bool lastAutoBullet = false;    // For automatic bullet points
    bool isWindowLocked;

};

#endif // KPAD_H
