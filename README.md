# KPad-Text-Editor

Kpad is a lightweight, Notepad-style text editor built with **C++ and Qt**.  
It is designed to be simple, fast, and easy to use while offering essential features such as text formatting and file management.

---
### ✨ Features
- Open, edit, and save plain text files (`.txt` and others).
- Basic text formatting: **bold**, *italic*, underline, font selection, and font size adjustment.
- "Paste without formatting" option (Ctrl+Shift+V).
- Save As functionality.
- Cross-platform (Windows, Linux, macOS — requires Qt).

---

### 📥 Installation

### Option 1: Download Prebuilt Executable
1. Go to the [Releases](../../releases) page.
2. Download the latest `.zip` package.
3. Extract the contents to any folder.
4. Run `Kpad.exe`.

> Note: All required Qt DLLs are included. No need to install Qt separately.

---

### Option 2: Build From Source (Developers)

#### Requirements
- C++17 compiler (MSVC, GCC, or Clang).
- [Qt 6.x or later](https://www.qt.io/download).
- [CMake](https://cmake.org/) (if using CMake build system).
- Git.

#### Steps

# Clone the repository
```bash
git clone https://github.com/yourusername/kpad.git
cd kpad

# Configure and build (CMake example)
cmake -B build -S .
cmake --build build --config Release
```
The executable will be available in the `build/` folder.

If using Qt Creator, simply open the project file and click **Run**.

---

### 🚀 Usage

Launch Kpad.

Use the menu bar to:

- **File → Open** to open a text file.
- **File → Save/Save As** to save changes.
- **Edit → Format** to change font size, style, and family.
- `Ctrl+Alt+↑` / `Ctrl+Alt+↓` to add multiple cursors.
- `Ctrl+Shift+V` to paste text without formatting.

---

### 🤝 Contributing

Contributions are welcome! To get started:

1. Fork the repository.
2. Create a new branch:
```bash
git checkout -b feature/my-feature
```
Push to your fork and open a Pull Request.

Please make sure your code builds and follows the project’s style.

---

### 🗂️ Project Structure
```
kpad/
├── src/             # Source code (C++/Qt files)
├── include/         # Header files
├── resources/       # Icons, .qrc resource file
├── build/           # Build output (ignored in git)
├── CMakeLists.txt   # Build configuration
└── README.md        # This file
```

---

### 📜 License

This project is licensed under the MIT License.  
You are free to use, modify, and distribute it under the terms of the license.

---

### 📌 Notes

- For Windows deployment, we use `windeployqt` to bundle required Qt DLLs with the `.exe`.
- Each GitHub Release includes a zipped folder with the `.exe` and dependencies.
- Future updates will be uploaded to the Releases page.
