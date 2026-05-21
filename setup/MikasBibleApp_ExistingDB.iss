; Mika's Bible App - Inno Setup Script (Existing-DB variant)
;
; Use this script when BibleDB already exists in SQL Server LocalDB
; (i.e. ImportBible.ps1 has already been run on the target machine).
; Only the application executable and the help file are installed.
;
; Requires Inno Setup 6.x  (https://jrsoftware.org/isinfo.php)
;
; Before compiling:
;   1.  Build the project in Release (x64) configuration.
;   2.  Build help\MikasBibleApp.chm  (run help\build_help.bat).
;   3.  Place the VC++ 2022 x64 redistributable next to this file:
;         setup\VC_redist.x64.exe
;         (download from https://aka.ms/vs/17/release/vc_redist.x64.exe)
;
; Output: setup\Output\MikasBibleAppSetup_ExistingDB.exe

#define AppName    "Mika's Bible App"
#define AppVersion "1.0"
#define AppPublisher "Mika Huttunen"
#define AppURL     "https://github.com/mphuttu/MikasBibleApp"
#define AppExeName "MikasBibleApp.exe"
#define SrcDir     "..\x64\Release"

[Setup]
AppId={{B2C3D4E5-F6A7-8901-BCDE-F12345678901}
AppName={#AppName}
AppVersion={#AppVersion}
AppPublisher={#AppPublisher}
AppPublisherURL={#AppURL}
AppSupportURL={#AppURL}
AppUpdatesURL={#AppURL}
AppCopyright=Copyright © Mika Huttunen, 2026
DefaultDirName={autopf}\{#AppName}
DefaultGroupName={#AppName}
AllowNoIcons=yes
; Output
OutputDir=Output
OutputBaseFilename=MikasBibleAppSetup_ExistingDB
Compression=lzma2
SolidCompression=yes
; Architecture
ArchitecturesInstallIn64BitMode=x64
ArchitecturesAllowed=x64
; Visual style
WizardStyle=modern
SetupIconFile=..\res\BibleBookIcon.ico
UninstallDisplayIcon={app}\{#AppExeName}
; Admin rights needed for VC++ redistributable
PrivilegesRequired=admin

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
; Main executable
Source: "{#SrcDir}\{#AppExeName}"; DestDir: "{app}"; Flags: ignoreversion

; Help file (compile help\build_help.bat first)
Source: "..\help\MikasBibleApp.chm"; DestDir: "{app}"; Flags: ignoreversion

; Icons used at runtime by the tree view
Source: "..\res\BibleBookIcon.ico";    DestDir: "{app}\res"; Flags: ignoreversion
Source: "..\res\BibleChapterIcon.ico"; DestDir: "{app}\res"; Flags: ignoreversion

; VC++ 2022 redistributable (place VC_redist.x64.exe in setup\ before compiling)
Source: "VC_redist.x64.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall; Check: FileExists(ExpandConstant('{src}\VC_redist.x64.exe'))

[Icons]
Name: "{group}\{#AppName}";           Filename: "{app}\{#AppExeName}"
Name: "{group}\Uninstall {#AppName}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\{#AppName}";   Filename: "{app}\{#AppExeName}"; Tasks: desktopicon

[Run]
; Install VC++ redistributable silently if the file was supplied
Filename: "{tmp}\VC_redist.x64.exe"; Parameters: "/install /quiet /norestart"; \
    StatusMsg: "Installing Visual C++ Redistributable..."; \
    Check: FileExists(ExpandConstant('{tmp}\VC_redist.x64.exe')); \
    Flags: waituntilterminated

; Launch app after install (optional)
Filename: "{app}\{#AppExeName}"; Description: "{cm:LaunchProgram,{#AppName}}"; Flags: nowait postinstall skipifsilent
