# Mika's Bible App

A Windows desktop Bible reading application built with MFC (Microsoft Foundation Classes) and C++.  
All 66 books of the Bible are available in the *Bible in Basic English* (BBE) translation.

> **"Jesus is Lord!"**

---

## Features

- Full Bible text – 66 books, 31 102 verses (Bible in Basic English)
- Tree-view navigation: expand any book to browse its chapters
- Fast chapter loading via SQL Server LocalDB backend
- Read-only rich-text display with Georgia font
- **Copy** any selected text to the clipboard (Ctrl+C / Edit > Copy)
- **Print** the current chapter (Ctrl+P / File > Print)
- Title bar shows the name of the currently selected book
- English CHM help file (Help > Help Topics or F1)
- Custom book and chapter icons in the navigation tree

---

## Prerequisites

| Component | Where to get it |
|-----------|----------------|
| Windows 10 x64 or later | — |
| Visual Studio 2022 (v143 toolset) or later | [visualstudio.microsoft.com](https://visualstudio.microsoft.com/) |
| SQL Server Express LocalDB 2019+ | Bundled with VS, or [aka.ms/sqllocaldb](https://aka.ms/sqllocaldb) |
| HTML Help Workshop *(to build CHM)* | [Microsoft download](https://learn.microsoft.com/en-us/previous-versions/windows/desktop/htmlhelp/microsoft-html-help-downloads) |
| Inno Setup 6 *(to build installer)* | [jrsoftware.org/isinfo.php](https://jrsoftware.org/isinfo.php) |

---

## Getting Started

### 1. Clone the repository

```bash
git clone https://github.com/mphuttu/MikasBibleApp.git
cd MikasBibleApp
```

### 2. Set up the database

Open **PowerShell** in the repository root and run:

```powershell
.\ImportBible.ps1
```

This creates the `BibleDB` LocalDB database and imports all verses from `data\bbe.txt`.  
To force a clean reimport: `.\ImportBible.ps1 -Force`

### 3. Build the application

Open `MikasBibleApp.slnx` in Visual Studio 2022 and build in **x64 | Debug** or **x64 | Release**.

### 4. Build the help file *(optional)*

```bat
cd help
build_help.bat
```

Copy the resulting `help\MikasBibleApp.chm` to the same directory as `MikasBibleApp.exe`.

### 5. Run

Launch `x64\Release\MikasBibleApp.exe` (or press F5 in Visual Studio).

---

## Project Structure

```
MikasBibleApp/
├── data/
│   └── bbe.txt                  # Bible in Basic English source text
├── help/
│   ├── MikasBibleApp.hhp        # HTML Help project
│   ├── toc.hhc                  # Table of contents
│   ├── index.hhk                # Help index
│   ├── build_help.bat           # Build script for CHM
│   └── html/                   # Help HTML pages
├── res/
│   ├── BibleBookIcon.ico        # Tree view book icon
│   └── BibleChapterIcon.ico     # Tree view chapter icon
├── setup/
│   └── MikasBibleApp.iss        # Inno Setup installer script
├── BibleDatabase.h / .cpp       # ODBC wrapper for LocalDB
├── ContentsView.h / .cpp        # Left-panel tree view
├── MainFrm.h / .cpp             # SDI main frame + splitter
├── MikasBibleApp.h / .cpp       # Application class
├── MikasBibleAppDoc.h / .cpp    # Document class (DB + selection state)
├── MikasBibleAppView.h / .cpp   # Right-panel rich-edit view
├── ImportBible.ps1              # Database import script
└── README.md
```

---

## Keyboard Shortcuts

| Key | Action |
|-----|--------|
| Ctrl+C | Copy selected text |
| Ctrl+A | Select all text in chapter |
| Ctrl+P | Print current chapter |
| F1 | Open Help |

---

## Building the Installer

Requires [Inno Setup 6](https://jrsoftware.org/isinfo.php).

1. Build the project in **Release** configuration.
2. Build the CHM help file (see above).
3. Open `setup\MikasBibleApp.iss` in Inno Setup and click **Build > Compile**.

The installer (`MikasBibleAppSetup.exe`) will be placed in `setup\Output\`.

---

## License & Copyright

Copyright © Mika Huttunen, 2026.  
Jesus is Lord!

The *Bible in Basic English* (BBE) translation is in the public domain.
