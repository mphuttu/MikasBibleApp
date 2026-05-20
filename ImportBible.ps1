# ImportBible.ps1
# Imports bbe.txt into SQL Server LocalDB (MSSQLLocalDB instance, database BibleDB).
# Run once before starting the application.
#
# Usage:  .\ImportBible.ps1
#         .\ImportBible.ps1 -BbeTxtPath "C:\path\to\bbe.txt"

param(
    [string]$BbeTxtPath  = "$PSScriptRoot\data\bbe.txt",
    [string]$InstanceName = "MSSQLLocalDB",
    [string]$DatabaseName = "BibleDB",
    [switch]$Force   # Clears existing data and reimports from scratch
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

# ── Book abbreviation → full name mapping ───────────────────────────────────
$bookNames = [ordered]@{
    # Old Testament
    "Gen" = "Genesis";           "Exo" = "Exodus";            "Lev" = "Leviticus"
    "Num" = "Numbers";           "Deu" = "Deuteronomy";       "Jos" = "Joshua"
    "Jdg" = "Judges";            "Rut" = "Ruth";              "1Sa" = "1 Samuel"
    "2Sa" = "2 Samuel";          "1Ki" = "1 Kings";           "2Ki" = "2 Kings"
    "1Ch" = "1 Chronicles";      "2Ch" = "2 Chronicles";      "Ezr" = "Ezra"
    "Neh" = "Nehemiah";          "Est" = "Esther";            "Job" = "Job"
    "Psa" = "Psalms";            "Pro" = "Proverbs";          "Ecc" = "Ecclesiastes"
    "Son" = "Song of Solomon";   "Sng" = "Song of Solomon";   "Sos" = "Song of Solomon"
    "Isa" = "Isaiah";            "Jer" = "Jeremiah";          "Lam" = "Lamentations"
    "Eze" = "Ezekiel";           "Ezk" = "Ezekiel";           "Dan" = "Daniel"
    "Hos" = "Hosea";             "Joe" = "Joel";              "Jol" = "Joel"
    "Amo" = "Amos";              "Oba" = "Obadiah";           "Jon" = "Jonah"
    "Mic" = "Micah";             "Nah" = "Nahum";             "Hab" = "Habakkuk"
    "Zep" = "Zephaniah";         "Hag" = "Haggai";            "Zec" = "Zechariah"
    "Zch" = "Zechariah";         "Mal" = "Malachi"
    # New Testament
    "Mat" = "Matthew";           "Mrk" = "Mark";              "Mar" = "Mark"
    "Luk" = "Luke";              "Jhn" = "John";              "Joh" = "John"
    "Act" = "Acts";              "Rom" = "Romans";            "1Co" = "1 Corinthians"
    "2Co" = "2 Corinthians";     "Gal" = "Galatians";         "Eph" = "Ephesians"
    "Php" = "Philippians";       "Phi" = "Philippians";       "Col" = "Colossians"
    "1Th" = "1 Thessalonians";   "2Th" = "2 Thessalonians";   "1Ti" = "1 Timothy"
    "2Ti" = "2 Timothy";         "Tit" = "Titus";             "Phm" = "Philemon"
    "Heb" = "Hebrews";           "Jas" = "James";             "Jam" = "James"
    "1Pe" = "1 Peter";           "2Pe" = "2 Peter";           "1Jn" = "1 John"
    "2Jn" = "2 John";            "3Jn" = "3 John";            "1Jo" = "1 John"
    "2Jo" = "2 John";            "3Jo" = "3 John";            "Jud" = "Jude"
    "Rev" = "Revelation"
}

# ── Verify source file ───────────────────────────────────────────────────────
if (-not (Test-Path $BbeTxtPath)) {
    Write-Error "File not found: $BbeTxtPath"
    exit 1
}

# ── Ensure LocalDB instance is running ──────────────────────────────────────
Write-Host "Starting LocalDB instance '$InstanceName'..."
$state = (sqllocaldb info $InstanceName 2>&1) -join " "
if ($state -match "No such instance") {
    sqllocaldb create $InstanceName | Out-Null
}
sqllocaldb start $InstanceName 2>&1 | Out-Null
$pipeName = (sqllocaldb info $InstanceName | Select-String "Instance pipe name").ToString().Split(":",2)[1].Trim()
Write-Host "Pipe: $pipeName"

# ── Helper: run SQL via sqlcmd (uses a temp file to avoid command-line length limits) ──
function Invoke-Sql {
    param([string]$Sql, [string]$Database = "master")
    # Write SQL to a temp file so we avoid the Windows command-line length limit.
    $tmpFile = [System.IO.Path]::GetTempFileName()
    $sqlFile = $tmpFile + ".sql"
    Rename-Item $tmpFile $sqlFile
    try {
        # Write as UTF-8 with BOM so sqlcmd handles NVARCHAR literals correctly.
        [System.IO.File]::WriteAllText($sqlFile, $Sql, [System.Text.Encoding]::UTF8)
        $result = sqlcmd -S "(localdb)\$InstanceName" -d $Database -i $sqlFile -b 2>&1
        if ($LASTEXITCODE -ne 0) { throw "sqlcmd error in [$Database]: $($result -join ' ')" }
        $result
    }
    finally {
        if (Test-Path $sqlFile) { Remove-Item $sqlFile -Force }
    }
}

# ── Create database ──────────────────────────────────────────────────────────
Write-Host "Creating database '$DatabaseName'..."
Invoke-Sql @"
IF NOT EXISTS (SELECT name FROM sys.databases WHERE name = N'$DatabaseName')
    CREATE DATABASE [$DatabaseName];
"@

# ── Create tables ────────────────────────────────────────────────────────────
Write-Host "Creating tables..."
Invoke-Sql @"
IF NOT EXISTS (SELECT * FROM sys.tables WHERE name = 'Books')
BEGIN
    CREATE TABLE Books (
        BookId    INT           PRIMARY KEY IDENTITY(1,1),
        ShortName NVARCHAR(10)  NOT NULL,
        FullName  NVARCHAR(100) NOT NULL,
        BookOrder INT           NOT NULL
    );
END;

IF NOT EXISTS (SELECT * FROM sys.tables WHERE name = 'Verses')
BEGIN
    CREATE TABLE Verses (
        VerseId   INT            PRIMARY KEY IDENTITY(1,1),
        BookId    INT            NOT NULL REFERENCES Books(BookId),
        Chapter   INT            NOT NULL,
        VerseNum  INT            NOT NULL,
        VerseText NVARCHAR(2000) NOT NULL
    );
    CREATE INDEX IX_Verses_BookChapter ON Verses (BookId, Chapter, VerseNum);
END;
"@ -Database $DatabaseName

# ── Check if already imported ────────────────────────────────────────────────
$count = (Invoke-Sql "SELECT COUNT(*) FROM Books" -Database $DatabaseName | Where-Object { $_ -match '^\s*\d+' } | Select-Object -First 1).Trim()
if ([int]$count -gt 0) {
    if ($Force) {
        Write-Host "Force mode: clearing existing data..."
        Invoke-Sql "DELETE FROM Verses; DELETE FROM Books; DBCC CHECKIDENT ('Books', RESEED, 0); DBCC CHECKIDENT ('Verses', RESEED, 0);" -Database $DatabaseName | Out-Null
        Write-Host "Tables cleared."
    }
    else {
        Write-Host "Database already contains $count books. Skipping import."
        Write-Host "Tip: use -Force to clear and reimport."
        Write-Host "Done."
        exit 0
    }
}

# ── Parse bbe.txt ─────────────────────────────────────────────────────────────
Write-Host "Parsing $BbeTxtPath ..."
$versePattern = '^([A-Za-z0-9]{2,4})\s+(\d+):(\d+)\s+(.+)$'

$bookOrder  = @{}   # abbrev -> order index
$bookOrder2 = [System.Collections.Generic.List[string]]::new()
$verses     = [System.Collections.Generic.List[hashtable]]::new()

Get-Content -Path $BbeTxtPath -Encoding UTF8 | ForEach-Object {
    if ($_ -match $versePattern) {
        $abbrev  = $Matches[1]
        $chapter = [int]$Matches[2]
        $verseN  = [int]$Matches[3]
        $text    = $Matches[4].Replace("'", "''")   # escape single quotes

        if (-not $bookOrder.ContainsKey($abbrev)) {
            $bookOrder[$abbrev] = $bookOrder2.Count
            $bookOrder2.Add($abbrev) | Out-Null
        }
        $verses.Add(@{ A=$abbrev; C=$chapter; V=$verseN; T=$text })
    }
}

Write-Host "Found $($bookOrder2.Count) books and $($verses.Count) verses."

# ── Insert books ──────────────────────────────────────────────────────────────
Write-Host "Inserting books..."
$bookIdMap = @{}   # abbrev -> BookId (from DB)

foreach ($abbrev in $bookOrder2) {
    $fullName = if ($bookNames.Contains($abbrev)) { $bookNames[$abbrev] } else { $abbrev }
    $order    = $bookOrder[$abbrev]
    $fullName = $fullName.Replace("'", "''")
    $sql = "INSERT INTO Books (ShortName, FullName, BookOrder) VALUES (N'$abbrev', N'$fullName', $order); SELECT SCOPE_IDENTITY();"
    $rows = Invoke-Sql $sql -Database $DatabaseName
    $newId = ($rows | Where-Object { $_ -match '^\s*[\d\.]+\s*$' } | Select-Object -First 1).Trim()
    $bookIdMap[$abbrev] = [int][decimal]$newId
}

# ── Insert verses in batches of 500 ──────────────────────────────────────────
Write-Host "Inserting verses (this may take a minute)..."
$batchSize = 100
$total = $verses.Count
$inserted = 0

for ($i = 0; $i -lt $total; $i += $batchSize) {
    $batch = $verses | Select-Object -Skip $i -First $batchSize
    $rows = $batch | ForEach-Object {
        $bid = $bookIdMap[$_.A]
        "($bid, $($_.C), $($_.V), N'$($_.T)')"
    }
    $sql = "INSERT INTO Verses (BookId, Chapter, VerseNum, VerseText) VALUES " + ($rows -join ",`n") + ";"
    Invoke-Sql $sql -Database $DatabaseName | Out-Null
    $inserted += $batch.Count
    Write-Progress -Activity "Inserting verses" -PercentComplete ([int]($inserted * 100 / $total)) -Status "$inserted / $total"
}
Write-Progress -Activity "Inserting verses" -Completed

Write-Host "Import complete: $($bookOrder2.Count) books, $total verses inserted into [$DatabaseName]."
