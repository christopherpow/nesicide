#include "qscilexerca65.h"

#include <QColor>
#include <QFont>
#include <QSettings>

#include "nes_emulator_core.h"
#include "ccc65interface.h"

static const char* CA65_keyword[] =
{
   "addr",
   "align",
   "asciiz",
   "assert",
   "autoimport",
   "bankbytes",
   "bss",
   "byt",
   "byte",
   "case",
   "charmap",
   "code",
   "condes",
   "constructor",
   "data",
   "dbyt",
   "debuginfo",
   "define",
   "def",
   "defined",
   "destructor",
   "dword",
   "else",
   "elseif",
   "end",
   "endenum",
   "endif",
   "endmac",
   "endmacro",
   "endproc",
   "endrep",
   "endrepeat",
   "endscope",
   "endstruct",
   "enum",
   "error",
   "exitmac",
   "exitmacro",
   "export",
   "exportzp",
   "feature",
   "fileopt",
   "fopt",
   "forceimport",
   "global",
   "globalzp",
   "hibytes",
   "if",
   "ifblank",
   "ifconst",
   "ifdef",
   "ifnblank",
   "ifndef",
   "ifnref",
   "ifref",
   "import",
   "importzp",
   "incbin",
   "include",
   "interruptor",
   "linecont",
   "list",
   "listbytes",
   "lobytes",
   "local",
   "localchar",
   "macpack",
   "mac",
   "macro",
   "org",
   "out",
   "p02",
   "pagelen",
   "pagelength",
   "popseg",
   "proc",
   "pushseg",
   "reloc",
   "repeat",
   "res",
   "rodata",
   "scope",
   "segment",
   "setcpu",
   "struct",
   "tag",
   "warning",
   "word",
   "zeropage",
   NULL
};

static const char* CA65_mnemonics[] =
{
   "BRK",
   "ORA",
   "KIL",
   "ASO",
   "DOP",
   "ASL",
   "PHP",
   "ANC",
   "TOP",
   "BPL",
   "CLC",
   "NOP",
   "JSR",
   "AND",
   "RLA",
   "BIT",
   "ROL",
   "PLP",
   "BMI",
   "SEC",
   "RTI",
   "EOR",
   "LSE",
   "LSR",
   "PHA",
   "ALR",
   "JMP",
   "BVC",
   "CLI",
   "RTS",
   "ADC",
   "RRA",
   "ROR",
   "PLA",
   "ARR",
   "BVS",
   "SEI",
   "STA",
   "AXS",
   "STY",
   "STX",
   "DEY",
   "TXA",
   "XAA",
   "BCC",
   "AXA",
   "TYA",
   "TXS",
   "TAS",
   "SAY",
   "XAS",
   "LDY",
   "LDA",
   "LDX",
   "LAX",
   "TAY",
   "TAX",
   "OAL",
   "BCS",
   "CLV",
   "TSX",
   "LAS",
   "CPY",
   "CMP",
   "DCM",
   "DEC",
   "INY",
   "DEX",
   "SAX",
   "BNE",
   "CLD",
   "CPX",
   "SBC",
   "INS",
   "INC",
   "INX",
   "BEQ",
   "SED",
   NULL
};

QsciLexerCA65::QsciLexerCA65(QObject */*parent*/)
{
   QString regex;
   int rc;

#if defined(Q_OS_MAC) || defined(Q_OS_MACX) || defined(Q_OS_MAC64)
   setDefaultFont(QFont("Monaco", 11));
#endif
#ifdef Q_OS_LINUX
   setDefaultFont(QFont("Monospace", 10));
#endif
#ifdef Q_OS_WIN
   setDefaultFont(QFont("Consolas", 11));
#endif

   labelRegex = QRegExp("^(\\s*)((\\.|@)?\\w+\\b:)");
   numberRegex = QRegExp("(\\b\\d+\\b)|(\\$\\b[0-9A-Fa-f]+\\b)|(\\b[0-9][0-9A-Fa-f]*h\\b)|(%\\b[01]+\\b)");

   regex = "\\b(";

   rc = 0;
   while ( CA65_mnemonics[rc] )
   {
      regex += CA65_mnemonics[rc];
      if ( CA65_mnemonics[rc+1] )
      {
         regex += '|';
      }
      rc++;
   }
   regex += ")\\b";
   opcodeRegex.setPattern(regex);
   opcodeRegex.setCaseSensitivity(Qt::CaseInsensitive);

   regex = "\\.(\\b(";
   rc = 0;
   while ( CA65_keyword[rc] )
   {
      regex += CA65_keyword[rc];
      if ( CA65_keyword[rc+1] )
      {
         regex += '|';
      }
      rc++;
   }
   regex += ")\\b)";
   keywordRegex.setPattern(regex);
   keywordRegex.setCaseSensitivity(Qt::CaseInsensitive);
}

QsciLexerCA65::~QsciLexerCA65()
{
}

QString QsciLexerCA65::description(int style) const
{
   // Note: this function MUST return a non-empty string
   //       for a style otherwise that style is ignored!
   switch ( style )
   {
      case CA65_Default:
         return "Default";
      break;
      case CA65_Comment:
         return "Comments";
      break;
      case CA65_Opcode:
         return "Instructions";
      break;
      case CA65_Label:
         return "Labels";
      break;
      case CA65_Keyword:
         return "Control commands";
      break;
      case CA65_QuotedString:
         return "Quoted strings";
      break;
      case CA65_Number:
         return "Number";
      break;
      default:
         return QString();
      break;
   }
}

// Return the list of keywords that can start a block.
const char *QsciLexerCA65::blockStartKeyword(int *style) const
{
    if (style)
        *style = CA65_Keyword;

    return ".proc";
}


// Return the list of characters that can start a block.
const char *QsciLexerCA65::blockStart(int *style) const
{
    if (style)
        *style = CA65_Keyword;

    return ".proc";
}


// Return the list of characters that can end a block.
const char *QsciLexerCA65::blockEnd(int *style) const
{
    if (style)
        *style = CA65_Keyword;

    return ".endproc";
}

void QsciLexerCA65::styleText(int start, int end)
{
   QByteArray   chars;
   QString      text;
   QStringList  lines;
   int          pos;
   int          lineLength;
   int          lineNum;
   int          index;

   // Reset line styling.
   startStyling(start,0xFF);
   setStyling(end-start,CA65_Default);

   // Get the text that is being styled.
   chars.reserve((end-start)+1);
   editor()->SendScintilla(QsciScintilla::SCI_GETTEXTRANGE,start,end,chars.data());

   // Get the starting line index from the position.
   editor()->lineIndexFromPosition(start,&lineNum,&index);

   // Break the text into line chunks.
   text = chars.constData();
   lines = text.split(QRegExp("(\r\n)|(\r)|(\n)"));

   // Style the lines.
   foreach ( const QString& line, lines )
   {
      // Get current line length.
      lineLength = editor()->lineLength(lineNum);

      if ( lineLength )
      {
         // Look for opcodes.
         pos = line.indexOf(opcodeRegex);
         if ( pos != -1 )
         {
            startStyling(start+pos,0xFF);
            setStyling(3,CA65_Opcode);
         }

         // Look for numbers.
         pos = 0;
         do
         {
            pos = line.indexOf(numberRegex,pos);
            if ( pos != -1 )
            {
               startStyling(start+pos,0xFF);
               setStyling(numberRegex.matchedLength(),CA65_Number);
               pos = pos+numberRegex.matchedLength();
            }
         } while ( pos != -1 );

         // Look for labels.
         pos = 0;
         do
         {
            pos = line.indexOf(labelRegex,pos);
            if ( pos != -1 )
            {
               QString leading_space = labelRegex.cap( 1 );
               QString label         = labelRegex.cap( 2 );
               startStyling(start+pos+leading_space.length(),0xFF);
               setStyling(label.length(),CA65_Label);
               pos = pos+labelRegex.matchedLength();

            }
         } while ( pos != -1 );

         // Look for quoted strings.
         pos = 0;
         do
         {
            pos = line.indexOf(QRegExp("\\\".*\\\""),pos);
            if ( pos != -1 )
            {
               startStyling(start+pos,0xFF);
               setStyling(line.indexOf('\"',pos+1)-pos+1,CA65_QuotedString);
               pos = line.indexOf('\"',pos+1)+1;
            }
         } while ( pos != -1 );
         pos = 0;
         do
         {
            pos = line.indexOf(QRegExp("\\\'.*\\\'"),pos);
            if ( pos != -1 )
            {
               startStyling(start+pos,0xFF);
               setStyling(line.indexOf('\'',pos+1)-pos+1,CA65_QuotedString);
               pos = line.indexOf('\'',pos+1)+1;
            }
         } while ( pos != -1 );

         // Look for keywords.
         pos = line.indexOf(keywordRegex);
         if ( pos != -1 )
         {
            startStyling(start+pos,0xFF);
            setStyling(keywordRegex.matchedLength(),CA65_Keyword);
         }

         // Look for comments. (Wash styling of comment over any previously applied).
         pos = line.indexOf(';');
         if ( pos != -1 )
         {
            startStyling(start+pos,0xFF);
            setStyling(lineLength-pos,CA65_Comment);
         }
      }

      start += lineLength;
      lineNum++;
   }
}

const char* QsciLexerCA65::wordCharacters() const
{
   const char* chars = QsciLexerCustom::wordCharacters();
   return chars;
}

QColor QsciLexerCA65::defaultColor() const
{
   return QColor(0,0,0);
}

QColor QsciLexerCA65::defaultColor(int style) const
{
   switch ( style )
   {
      case CA65_Default:
         return QColor(50,50,50);
      break;
      case CA65_Comment:
         return QColor(0,255,45);
      break;
      case CA65_Opcode:
         return QColor(0,45,255);
      break;
      case CA65_Label:
         return QColor(0,160,30);
      break;
      case CA65_Keyword:
         return QColor(120,120,120);
      break;
      case CA65_QuotedString:
         return QColor(255,120,30);
      break;
      case CA65_Number:
         return QColor(255,30,30);
      break;
      default:
         return defaultColor();
      break;
   }
}

QColor QsciLexerCA65::defaultPaper() const
{
   return QColor(255,255,255);
}

QColor QsciLexerCA65::defaultPaper(int /*style*/) const
{
   return QsciLexer::defaultPaper();
}

QColor QsciLexerCA65::paper(int /*style*/) const
{
   return QsciLexer::defaultPaper();
}

QFont QsciLexerCA65::defaultFont() const
{
#if defined(Q_OS_MAC) || defined(Q_OS_MACX) || defined(Q_OS_MAC64)
   return QFont("Monaco", 11);
#endif
#ifdef Q_OS_LINUX
   return QFont("Monospace", 10);
#endif
#ifdef Q_OS_WIN
   return QFont("Consolas", 11);
#endif
}

QFont QsciLexerCA65::defaultFont(int style) const
{
   QFont font = QsciLexerCA65::defaultFont();

   switch ( style )
   {
      case CA65_Comment:
         font.setItalic(true);
      break;
      case CA65_Opcode:
         font.setBold(true);
      break;
      case CA65_Label:
         font.setUnderline(true);
      break;
      case CA65_Keyword:
         font.setBold(true);
         font.setItalic(true);
      break;
   }

   return font;
}

bool QsciLexerCA65::readProperties(QSettings &qs,const QString &prefix)
{
   return QsciLexer::readProperties(qs,prefix);
}

bool QsciLexerCA65::writeProperties(QSettings &qs,const QString &prefix) const
{
   return QsciLexer::writeProperties(qs,prefix);
}

bool QsciLexerCA65::readSettings(QSettings &qs,const char *prefix)
{
   return QsciLexer::readSettings(qs,prefix);
}

bool QsciLexerCA65::writeSettings(QSettings &qs,const char *prefix)
{
   return QsciLexer::writeSettings(qs,prefix);
}
