#include "kpad.h"
#include "ui_kpad.h"

// --------------------
// About
// --------------------
void Kpad::showAbout() {
    QString aboutText =
        "<b>KPad</b><br>"
        "</b>a C++/Qt Text Editor</b><br><br>"
        "KPad is a project I created to learn C++ and Qt. It started as a way for me "
        "to explore these powerful tools while building something practical. Inspired by "
        "the simplicity of Notepad++, I wanted to create a lightweight, fast, and minimal text editor "
        "that’s both functional and easy to use. This journey has allowed me to dive deeper into C++ "
        "and Qt, refine my coding skills, and develop a clean, intuitive tool for writing.<br><br>"

        "Working on KPad, I got to apply what I learned, experiment with UI development,"
        "and bring a simple idea to life. I’m excited to share it with others who "
        "appreciate minimalism and efficiency in their writing tools.<br><br>"

        "<b>Thank you for using KPad+</b><br><br>";

    QMessageBox::about(this, "About KPad+", aboutText);
}

