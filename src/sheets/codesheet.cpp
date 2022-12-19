#include "sheets/codesheet.h"
#include "widgets/codeeditor.h"
#include "widgets/codesearchbar.h"
#include "windows/rememberdialog.h"
#include "windows/progressdialog.h"
#include "base/application.h"
#include <KSyntaxHighlighting/Definition>
#include <KSyntaxHighlighting/DefinitionDownloader>
#include <QAbstractButton>
#include <QAction>
#include <QBoxLayout>
#include <QFileInfo>
#include <QPushButton>
#include <QRegularExpression>
#include <QScrollBar>
#include <QTextCodec>

CodeSheet::CodeSheet(const ResourceModelIndex &index, QWidget *parent)
    : BaseFileSheet(index, parent)
{
    filePath = index.path();
    QString sheetTitle = filePath.section('/', -2);
    QRegularExpression guid("^{\\w{8}-\\w{4}-\\w{4}-\\w{4}-\\w{12}}");
    if (guid.match(sheetTitle).hasMatch()) {
        sheetTitle = sheetTitle.split('/').last();
    }
    setSheetTitle(sheetTitle);

    btnDownloadDefinitions = new QPushButton(this);
    btnDownloadDefinitions->hide();
    connect(btnDownloadDefinitions, &QPushButton::clicked, this, &CodeSheet::downloadDefinitions);

    editor = new CodeEditor(this);
    editor->setCenterOnScroll(true);
    const auto definition = app->highlightingRepository.definitionForFileName(filePath);
    if (definition.isValid()) {
        editor->setDefinition(definition);
    } else {
        checkFormatSupport();
    }

    searchBar = new CodeSearchBar(editor);
    connect(editor, &CodeEditor::searchFinished, searchBar, &CodeSearchBar::setResults);

    auto statusBar = new QHBoxLayout;
    statusBar->addWidget(btnDownloadDefinitions);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(editor);
    layout->addWidget(searchBar);
    layout->addLayout(statusBar);
    layout->setMargin(0);
    layout->setSpacing(0);

    load();
    connect(editor, &QPlainTextEdit::modificationChanged, this, &CodeSheet::setModified);

    // Initialize actions:

    addActionSeparator();

    auto actionFind = app->actions.getFind(this);
    addAction(actionFind);
    connect(actionFind, &QAction::triggered, this, &CodeSheet::findSelectedText);

    auto actionFindNext = app->actions.getFindNext(this);
    addAction(actionFindNext);
    connect(actionFindNext, &QAction::triggered, this, [=]() {
        searchBar->show();
        editor->nextSearchQuery();
    });

    auto actionFindPrevious = app->actions.getFindPrevious(this);
    addAction(actionFindPrevious);
    connect(actionFindPrevious, &QAction::triggered, this, [=]() {
        searchBar->show();
        editor->prevSearchQuery();
    });

    auto actionReplace = app->actions.getReplace(this);
    addAction(actionReplace);
    connect(actionReplace, &QAction::triggered, this, &CodeSheet::replaceSelectedText);

    retranslate();
}

bool CodeSheet::load()
{
    QFile file(index.path());
    if (file.open(QFile::ReadOnly)) {
        QTextStream stream(&file);
        stream.setCodec("UTF-8"); // Fallback if no codec is detected on further read
        auto cursor = editor->textCursor();
        const int selectionStart = cursor.selectionStart();
        const int selectionEnd = cursor.selectionEnd();
        const int scrollPosition = editor->verticalScrollBar()->value();
        editor->setPlainText(stream.readAll());
        codec = stream.codec(); // Qt automatically deletes the codec on exit
        cursor.setPosition(selectionStart);
        cursor.setPosition(selectionEnd, QTextCursor::KeepAnchor);
        editor->setTextCursor(cursor);
        editor->verticalScrollBar()->setValue(scrollPosition);
        setModified(false);
        return true;
    }
    qWarning() << "Error: Could not open code resource file";
    return false;
}

bool CodeSheet::save(const QString &as)
{
    QFile file(as.isEmpty() ? index.path() : as);
    if (file.open(QFile::WriteOnly)) {
        file.resize(0);
        QTextStream stream(&file);
        stream.setCodec(codec);
        stream.setGenerateByteOrderMark(codec->name() != "UTF-8");
        stream << editor->toPlainText();
        if (as.isEmpty()) {
            setModified(false);
            emit saved();
        }
        return true;
    }
    qWarning() << "Error: Could not save code resource file";
    return false;
}

void CodeSheet::setTextCursor(int lineNumber, int columnNumber, int selectionLength)
{
    auto cursor = editor->textCursor();
    cursor.movePosition(QTextCursor::Start);
    cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, lineNumber - 1);
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, columnNumber);
    if (selectionLength > 0) {
        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, selectionLength);
    }
    editor->setTextCursor(cursor);
}

void CodeSheet::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        retranslate();
    }
    BaseSheet::changeEvent(event);
}

void CodeSheet::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        searchBar->hide();
    }
    BaseFileSheet::keyPressEvent(event);
}

void CodeSheet::findSelectedText()
{
    auto cursor = editor->textCursor();
    if (!cursor.hasSelection()) {
        cursor.select(QTextCursor::WordUnderCursor);
    }
    const auto selection(cursor.selectedText());
    cursor.setPosition(cursor.selectionStart()); // Move to the beginning of the selection and clear it
    editor->setTextCursor(cursor);
    searchBar->setFindText(selection);
    searchBar->show();
    searchBar->focusFindBar();
}

void CodeSheet::replaceSelectedText()
{
    const auto selection = editor->textCursor().selectedText();
    searchBar->show();
    searchBar->showReplaceBar();
    if (selection.isEmpty()) {
        searchBar->focusFindBar();
    } else {
        searchBar->setFindText(selection);
        searchBar->focusReplaceBar();
    }
}

void CodeSheet::checkFormatSupport()
{
    const auto fileName = QFileInfo(filePath).fileName();
    for (const auto &extension : supportedExtensions()) {
        auto regex = QRegularExpression::wildcardToRegularExpression(extension);
        if (QRegularExpression(regex).match(fileName).hasMatch()) {
            const QString question(tr("Would you like to download syntax definitions for this and other formats?"));
            if (RememberDialog::ask("download-syntax-definitions", question, this)) {
                downloadDefinitions();
            } else {
                btnDownloadDefinitions->show();
            }
            break;
        }
    }
}

void CodeSheet::downloadDefinitions()
{
    auto progressDialog = new ProgressDialog(this);
    progressDialog->setPrimaryText(tr("Downloading syntax definitions..."));
    progressDialog->setCancelEnabled(false);
    progressDialog->show();

    auto downloader = new KSyntaxHighlighting::DefinitionDownloader(&app->highlightingRepository);
    connect(downloader, &KSyntaxHighlighting::DefinitionDownloader::informationMessage, [progressDialog](const QString &message) {
        qDebug() << message;
    });
    connect(downloader, &KSyntaxHighlighting::DefinitionDownloader::done, this, [=]() {
        editor->setDefinition(app->highlightingRepository.definitionForFileName(filePath));
        btnDownloadDefinitions->hide();
        downloader->deleteLater();
        progressDialog->deleteLater();
    });
    downloader->start();
}

void CodeSheet::retranslate()
{
    btnDownloadDefinitions->setText(tr("Download Syntax Definitions"));
}

QStringList CodeSheet::supportedExtensions() const
{
    return {
        "*.mustache", "*.handlebars", "*.hbs", "*.ractive", "*.hogan", "*.hulk", "*.html.mst", "*.html.mu", "*.html.rac",
        "*.kbasic",
        "*.per", "*.PER", "*.per.err",
        "*.csv",
        "*.xul", "*.xbl",
        "*.bat",
        "*.c++", "*.cxx", "*.cpp", "*.cc", "*.C", "*.h", "*.hh", "*.H", "*.h++", "*.hxx", "*.hpp", "*.hcc",
        "*.clist", "*.CLIST",
        "*.mll",
        "*.mtt",
        "*.k",
        "*.dtd",
        "*.reg",
        "*.pb", "*.pbi",
        "*.sql", "*.SQL", "*.ddl", "*.DDL",
        "*.supp",
        "*.asm",
        "*.ll",
        "*.prg", "*.PRG", "*.ch",
        "*.abc", "*.ABC",
        "*.scss",
        "*.graphql",
        "*.dat",
        "*.jira",
        "*.html",
        "*.sa",
        "*.oors",
        "*.stl",
        "*.cfg",
        "*.kt", "*.kts",
        "*.vhdl", "*.vhd",
        "*.dart",
        "*.jsp", "*.JSP",
        "*.eml", "*.email", "*.emlx", "*.mbox", "*.mbx",
        "*.c", "*.C", "*.h",
        "*.mod", "*.def",
        "*.idr",
        "*.clj", "*.cljs", "*.cljc",
        "*.php", "*.php3", "*.wml", "*.phtml", "*.phtm", "*.inc", "*.ctp",
        "*.cfg",
        "*.mab", "*.MAB", "*.Mab",
        "*.pro", "*.pri", "*.prf",
        "*.vtc",
        "*.e",
        "*.sv", "*.svh",
        "*.asm",
        "*.tsx",
        "*.nsi",
        "Jam*", "*.jam",
        "*.lgt", ".logtalk",
        "*.d", "*.D", "*.di", "*.DI",
        "*.lua", "*.rockspec",
        "changelog",
        "*.curry",
        "*.raku", "*.rakumod", "*.rakudoc", "*.rakutest", "*.pl6", "*.PL6", "*.p6", "*.pm6", "*.pod6",
        "*.java",
        "*.desktop", "*.kdelnk", "*.desktop.cmake",
        "Kconfig*", "Config.*",
        "*.t2t",
        "fstab", "mtab",
        "*.ad", "*.adoc", "*.asciidoc",
        "*.tex", "*.ltx", "*.dtx", "*.sty", "*.cls", "*.bbx", "*.cbx", "*.lbx", "*.tikz", "*.pgf",
        "*.sql", "*.SQL", "*.ddl", "*.DDL",
        "*.go",
        "*.csv",
        "*.css",
        "*.aff",
        "*.lcurry",
        "*.js", "*.mjs", "*.cjs", "*.kwinscript", "*.julius",
        "*.btm",
        "*.vala",
        "*.4th", "*.4TH", "*.f", "*.F", "*.frt", "*.FRT", "*.fs", "*.FS", "*.fth", "*.FTH", "*.seq", "*.SEQ",
        "*.adb", "*.ads", "*.ada", "*.a",
        "*.rng", "*.RNG",
        "*.erl",
        "*.asm", "*.inc", "*.ASM", "*.INC",
        "*.mml",
        "*.yang",
        "*.replicode",
        "*.w", "*.nw",
        "*.as",
        "*.mm", "*.M", "*.h",
        "*.f", "*.F", "*.for", "*.FOR", "*.fpp", "*.FPP",
        "*.m3", "*.i3", "*.ig", "*.mg",
        "*.s", "*.i", "*.S", "*.I",
        "*.sql", "*.SQL", "*.ddl", "*.DDL", "*.trg", "*.TRG", "*.spc", "*.SPC", "*.bdy", "*.DBY",
        "*.purs",
        "*.do", "*.ado", "*.doh", "*.DO", "*.ADO", "*.DOH",
        "*.mod", "*.def",
        "*.m3u",
        "*.sh", "*.zsh", ".zshrc", ".zprofile", ".zlogin", ".zlogout", ".profile",
        "*.mod", "*.def",
        "*.inf", "*.h",
        "*.dic",
        "*.tig",
        "*.obj",
        ".kdesrc-buildrc", "kdesrc-buildrc",
        "*.rtf",
        "*.gdf",
        "*.pug", "*.jade",
        "*.properties",
        "*.sed",
        "*.p", "*.w", "*.i", "*.cls",
        "*.c++", "*.cxx", "*.cpp", "*.cc", "*.C", "*.h", "*.hh", "*.H", "*.h++", "*.hxx", "*.hpp", "*.hcc", "*.moc",
        "*.mup", "*.not",
        "*.v", "*.V", "*.vl",
        "*.logcat",
        "*.s",
        "*.vcl",
        "*.sci", "*.sce",
        "QRPG*.*", "qrpg*.*",
        "*.tf",
        "*.mod", "*.def",
        "*.prolog", "*.dcg", "*.pro",
        "*.pp",
        "*.n",
        "*.znn",
        "*.dot",
        "*.p", "*.pas", "*.pp",
        "*.1", "*.2", "*.3", "*.4", "*.5", "*.6", "*.7", "*.8", "*.1m", "*.3x", "*.tmac",
        "*.kcrash", "*.crash", "*.bt",
        "*.proto",
        "*.s", "*.S",
        "*.elm",
        "*.bib",
        "*.feature",
        "*.gdb",
        "*.texi",
        "*.cg", "*.cgfx",
        "*.hs", "*.chs", "*.hs-boot",
        "*.rib",
        "*.rs",
        "*.crk",
        "*.sp", "*.hsp",
        "*.nb",
        "*.tt*",
        "*.org",
        "*.ample", "*.menu", "*.startup",
        "*.less",
        "*.bb", "*.bbappend", "*.bbclass", "*.inc",
        "*.boo",
        "*.idl",
        "*.fe", "*.feh",
        "*.te", "*.if", "*.spt", "policy.conf", "access_vectors", "mls", "mcs", "mls_macros", "te_macros", "policy_capabilities", "seapp_contexts", "port_contexts",
        "*.haml",
        "*.pike",
        "*.qml", "*.qmltypes",
        "GNUmakefile", "Makefile", "makefile", "GNUmakefile.*", "Makefile.*", "makefile.*", "*.mk",
        "*.mako", "*.mak",
        "*.awk",
        "Dockerfile",
        "*.cfg", "*.pbl", "*.CFG", "*.PBL",
        "*.ninja",
        "*.htm", "*.html",
        "*.m4",
        "*.rkt", "*.scrbl",
        "*.cis",
        "*.mediawiki",
        "*.textile",
        "*.jl",
        "*.ldif",
        "*.hex", "*.ihx",
        "*.csv",
        "*.diff", "*patch", "*.rej",
        "*.g",
        "*.Rd",
        "Cakefile", "*.coffee", "*.coco", "*.cson",
        "gitolite.conf",
        "*.iss",
        "*.mch", "*.imp", "*.ref",
        "Doxyfile", "Doxyfile.*",
        "*.py", "*.pyw", "*.pyi", "SConstruct", "SConscript", "*.FCMacro",
        "*.iCal", "*.iCalendar", ".ics", "*.ifb", "*.iFBf", "*.vcal", "*.vcalendar", "*.vcard", "*.vcf",
        "*.asn", "*.asn1",
        "*.ftl",
        "*.pgn", "*.PGN",
        "*.md", "*.mmd", "*.markdown",
        "*.hamlet",
        "*.vb",
        "*.sst", "*.ssm", "*.ssi", "*._sst", "*.-sst",
        "*.ys",
        "*.rqb",
        "*.glsl", "*.vert", "*.vs", "*.frag", "*.fs", "*.geom", "*.gs", "*.tcs", "*.tes",
        "*.swift",
        "*.sgml",
        "*.nc",
        "*.fbs",
        "*.toml",
        "*.cp", "*.bro",
        "*.inc", "*.pov",
        "*.pl", "*.PL", "*.pm",
        "meson.build", "meson_options.txt",
        "*.vr", "*.vri", "*.vrh",
        "*.agda",
        "*.rnc",
        "*.c++", "*.cxx", "*.cpp", "*.cc", "*.C", "*.cu", "*.h", "*.hh", "*.H", "*.h++", "*.hxx", "*.hpp", "*.hcc", "*.cuh", "*.inl", "*.ino", "*.pde", "*.moc",
        "*.gnuplot", "*.gp", "*.gplt", "*.plt",
        "*.ml", "*.mli",
        "*.cfg",
        "*.nim", "*.nims", "*.nimble",
        "*.jsx",
        "*.c++", "*.cxx", "*.cpp", "*.cc", "*.C", "*.h", "*.hh", "*.H", "*.h++", "*.hxx", "*.hpp", "*.hcc", "*.moc",
        "ChangeLog",
        "*.sass",
        "git-rebase-todo",
        "*.ctx", "*.mkiv", "*.mkvi", "*.mkxl", "*.mklx",
        "*.nix",
        "*.idx",
        "*.cs", "*.ashx",
        "*.vm",
        "*.m", "*.h",
        "*.fastq", "*.fq", "*.fastq.gz", "*.fq.gz",
        "*.abap", "*.ABAP",
        "*.vcc",
        "*.fish", "fishd.*",
        "*.l", "*.lex", "*.flex",
        "*.srt",
        "*.stan", "*.stanfunctions",
        "*.ini", "*.pls", "*.kcfgc", ".gitattributes*", ".gitconfig*", ".gitmodules*", ".editorconfig*",
        "*.octave", "*.m", "*.M",
        "*.bas", "*.bi", "*.BAS", "*.BI",
        "*.cl",
        "*.rules", "*.snort", "*.suricata",
        "*.prg", "*.PRG", "*.ch",
        "*.csh", "*.tcsh", "csh.cshrc", "csh.login", ".tcshrc", ".cshrc", ".login",
        "*.asm", "*.ASM", "*.asm-avr",
        "*.cfg",
        "*.fc", "file_contexts", "file_contexts_*", "file_contexts.local", "file_contexts.homedirs", "file_contexts.template", "homedir_template", "property_contexts", "service_contexts", "hwservice_contexts", "initial_sid_contexts", "genfs_contexts", "fs_use",
        "*.sh", "*.bash", "*.ebuild", "*.eclass", "*.exlib", "*.exheres-0", ".bashrc", ".bash_profile", ".bash_login", ".profile", "PKGBUILD", "APKBUILD",
        "*.dox", "*.doxygen",
        "*.cue",
        "*.yar", "*.yara",
        "usr.bin.*", "usr.sbin.*", "bin.*", "sbin.*", "usr.lib.*", "usr.lib64.*", "usr.lib32.*", "usr.libx32.*", "usr.libexec.*", "usr.local.bin.*", "usr.local.sbin.*", "usr.local.lib*", "opt.*", "etc.cron.*", "snap.*", "snap-update-ns.*", "snap-confine.*",
        "*.brs",
        "*.ahk", "*.iahk",
        "todo.txt",
        "*.automount", "*.device", "*.mount", "*.path", "*.service", "*.slice", "*.socket", "*.swap", "*.target", "*.timer",
        "xorg.conf",
        "*.asm", "*.inc", "*.fasm",
        "*.mapcss",
        "*.lsl",
        "*.lhs",
        "*.Praat", "*.praat", "*.psc", "*.praat-script", "*.praatscript", "*.proc",
        "*.tcl", "*.tk",
        "*.gd", "*.gi", "*.g",
        "*.retro",
        "*.ps1", "*.psm1", "*.psd1",
        "*.rmd", "*.Rmd", "*.RMD",
        "*.ahdl", "*.tdf",
        "*.sml", "*.ml",
        "*.ijs", "*.ijt", "*.IJS", "*.IJT",
        "*.overpassql",
        "*.rpy",
        "*.sol",
        "*.gd",
        "*.4gl", "*.4GL", "*.err",
        "*.mm",
        "*.mp", "*.mps", "*.mpost", "*.mf",
        "*.fs", "*.fsi", "*.fsx",
        "*.groovy", "*.gradle", "*.gvy", "Jenkinsfile",
        "*.po", "*.pot",
        "*.hx", "*.Hx", "*.hX", "*.HX",
        "*.ngc", "*.gcode",
        "*.mib",
        "*.pro",
        "*.scm", "*.ss", "*.scheme", "*.guile", "*.chicken", "*.meta",
        "*.wrl",
        "*.json", ".kateproject", ".arcconfig", "*.geojson", "*.gltf", "*.theme",
        "*.tjp", "*.tji",
        "*.pony",
        "*.gdl", "*.vcg", "*.GDL", "*.VCG",
        "*.qdocconf",
        "*.cil",
        "*.pig",
        "*.src", "*.SRC", "*.asm", "*.ASM", "*.pic", "*.PIC",
        "*.ps", "*.ai", "*.eps",
        "*.f90", "*.F90", "*.f95", "*.F95", "*.f03", "*.F03", "*.f08", "*.F08",
        "*.dats", "*.sats", "*.hats",
        "*.uc",
        "*.spec",
        "*.t", "*.h",
        "*.ftl",
        "*.mel",
        "*.mss",
        "*.sql", "*.SQL", "*.ddl", "*.DDL",
        "*.cr",
        "*.q",
        "*.ex", "*.exs", "*.eex", "*.xml.eex", "*.js.eex",
        "*.tsv", "*.tab",
        "*.mo",
        "*.ccss",
        "*.ppd",
        "*.mac", "*.MAC", "*.dem", "*.DEM",
        "*.cfm", "*.cfc", "*.cfml", "*.dbm",
        "*.ts", "*.mts", "*.cts",
        "*.csv",
        ".gitignore*",
        "*.R", "*.r", "*.S", "*.s", "*.q",
        "*.ly", "*.LY", "*.ily", "*.ILY", "*.lyi", "*.LYI",
        "*.mly",
        "*.gdbinit",
        "control",
        "*.ld",
        "*.xsl", "*.xslt",
        "*.rhtml", "*.RHTML", "*.html.erb",
        "*.pure",
        "*.pli", "*.pl1",
        "httpd.conf", "httpd2.conf", "apache.conf", "apache2.conf", ".htaccess*", ".htpasswd*",
        "*.rst",
        "*.e", "*.ex", "*.exw", "*.exu",
        "*.rex", "*.rx", "*.rexx",
        "*.rb", "*.rjs", "*.rxml", "*.xml.erb", "*.js.erb", "*.rake", "Rakefile", "Gemfile", "*.gemspec", "Vagrantfile",
        "*.e",
        "*.JCL", "*.jcl",
        "*.ply",
        "*.ans",
        "*.c", "*.h", "*.inc", "*.o",
        "*.m", "*.M",
        "*.asp",
        "*.m", "*.mag",
        "*.y", "*.yy", "*.ypp", "*.y++",
        "*.lisp", "*.cl", "*.lsp", "*.el",
        "*.il",
        "CMakeLists.txt", "*.cmake", "*.cmake.in",
        "*.cgis",
        "*.c", "*.C", "*.h",
        "*.siv", "*.sieve",
        "*.scad",
        "*.scala", "*.sbt",
        "*.impl", "*.sign",
    };
}
