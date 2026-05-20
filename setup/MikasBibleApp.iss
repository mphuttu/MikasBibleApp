; Mika's Bible App - Inno Setup Script
; Requires Inno Setup 6.x  (https://jrsoftware.org/isinfo.php)
;
; Before compiling:
;   1.  Build the project in Release (x64) configuration.
;   2.  Build help\MikasBibleApp.chm  (run help\build_help.bat).
;   3.  Place the VC++ 2022 x64 redistributable next to this file:
;         setup\VC_redist.x64.exe
;         (download from https://aka.ms/vs/17/release/vc_redist.x64.exe)
;   4.  Place the SQL Server LocalDB installer next to this file:
;         setup\SqlLocalDB.msi
;         (download from https://aka.ms/sqllocaldb)
;
; Output: setup\Output\MikasBibleAppSetup.exe

#define AppName    "Mika's Bible App"
#define AppVersion "1.0"
#define AppPublisher "Mika Huttunen"
#define AppURL     "https://github.com/mphuttu/MikasBibleApp"
#define AppExeName "MikasBibleApp.exe"
#define SrcDir     "..\x64\Release"

[Setup]
AppId={{A1B2C3D4-E5F6-7890-ABCD-EF1234567890}
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
OutputBaseFilename=MikasBibleAppSetup
Compression=lzma2
SolidCompression=yes
; Architecture
ArchitecturesInstallIn64BitMode=x64
ArchitecturesAllowed=x64
; Visual style
WizardStyle=modern
SetupIconFile=..\res\BibleBookIcon.ico
UninstallDisplayIcon={app}\{#AppExeName}
; Require admin so LocalDB can be configured
PrivilegesRequired=admin

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
; Main executable
Source: "{#SrcDir}\{#AppExeName}"; DestDir: "{app}"; Flags: ignoreversion

; Help file (compile help\build_help.bat first)
Source: "..\help\MikasBibleApp.chm"; DestDir: "{app}"; Flags: ignoreversion; Check: FileExists(ExpandConstant('..\help\MikasBibleApp.chm'))

; Icons used at runtime by the tree view
Source: "..\res\BibleBookIcon.ico";    DestDir: "{app}\res"; Flags: ignoreversion
Source: "..\res\BibleChapterIcon.ico"; DestDir: "{app}\res"; Flags: ignoreversion

; Bible data (needed only if user wants to reimport)
Source: "..\data\bbe.txt"; DestDir: "{app}\data"; Flags: ignoreversion

; PowerShell import script
Source: "..\ImportBible.ps1"; DestDir: "{app}"; Flags: ignoreversion

; VC++ 2022 redistributable (place VC_redist.x64.exe in setup\ before compiling)
Source: "VC_redist.x64.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall; Check: FileExists(ExpandConstant('{src}\VC_redist.x64.exe'))

; SQL Server LocalDB installer (place SqlLocalDB.msi in setup\ before compiling)
Source: "SqlLocalDB.msi"; DestDir: "{tmp}"; Flags: deleteafterinstall; Check: FileExists(ExpandConstant('{src}\SqlLocalDB.msi'))

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

; Install SQL Server LocalDB silently if the MSI was supplied
Filename: "msiexec.exe"; Parameters: "/i ""{tmp}\SqlLocalDB.msi"" /quiet /norestart"; \
    StatusMsg: "Installing SQL Server LocalDB..."; \
    Check: FileExists(ExpandConstant('{tmp}\SqlLocalDB.msi')); \
    Flags: waituntilterminated

; Import the Bible database after installation
Filename: "powershell.exe"; \
    Parameters: "-ExecutionPolicy Bypass -File ""{app}\ImportBible.ps1"""; \
    WorkingDir: "{app}"; \
    StatusMsg: "Importing Bible data (this may take a minute)..."; \
    Flags: waituntilterminated runhidden

; Launch app after install (optional)
Filename: "{app}\{#AppExeName}"; Description: "{cm:LaunchProgram,{#AppName}}"; Flags: nowait postinstall skipifsilent

[Code]
// If either prereq installer is missing we just skip it silently - the
// CheckFileExists() calls on the [Files] and [Run] entries handle that.
function FileExists(const AFileName: string): Boolean;
begin
  Result := SysUtils.FileExists(AFileName);
end;
