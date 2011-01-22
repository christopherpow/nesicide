#include "csyntaxhighlighter.h"
#include "csourceassembler.h"

CSyntaxHighlighter::CSyntaxHighlighter(QTextDocument* parent)
   : QSyntaxHighlighter(parent)
{
   HighlightingRule rule;
   QStringList keywordPatterns;
   QStringList preprocessorPatterns;

#if 1
   // Assembler mnemonics
   for (int inx = 0; ; inx++)
   {
      if (AssemblerInstructionItems[inx].isEmpty())
      {
         break;
      }

      keywordPatterns << ("\\b" + AssemblerInstructionItems[inx] + "\\b");
   }
   keywordFormat.setForeground(Qt::blue);
   keywordFormat.setFontWeight(QFont::Bold);
   foreach (const QString &pattern, keywordPatterns)
   {
      rule.pattern = QRegExp(pattern);
      rule.pattern.setCaseSensitivity(Qt::CaseInsensitive);
      rule.format = keywordFormat;
      highlightingRules.append(rule);
   }

   // Normal Numbers
   classFormat.setFontWeight(QFont::Normal);
   classFormat.setForeground(Qt::darkCyan);
   rule.pattern = QRegExp("\\b[0-9]+\\b");
   rule.format = classFormat;
   highlightingRules.append(rule);

   // Hexadecimal Numbers
   classFormat.setFontWeight(QFont::Normal);
   classFormat.setForeground(Qt::darkMagenta);
   rule.pattern = QRegExp("0[xX][0-9a-fA-F]+\\b");
   rule.format = classFormat;
   highlightingRules.append(rule);
   rule.pattern = QRegExp("[0-9a-fA-F]+\\h\\b");
   highlightingRules.append(rule);
   rule.pattern = QRegExp("\\$[0-9a-fA-F]+\\b");
   highlightingRules.append(rule);

   // Labels
   labelFormat.setForeground(Qt::darkCyan);
   labelFormat.setFontWeight(QFont::Bold);
   rule.pattern = QRegExp("[^:\\s]*:");
   rule.format = labelFormat;
   highlightingRules.append(rule);

   // Preprocessor Commands
   // Assembler directives
   for (int inx = 0; ; inx++)
   {
      if (AssemblerPreprocessorDirectives[inx].isEmpty())
      {
         break;
      }

      preprocessorPatterns << ("\\b" + AssemblerPreprocessorDirectives[inx] + "\\b");
      preprocessorPatterns << ("\\b." + AssemblerPreprocessorDirectives[inx] + "\\b");
   }
   dotPreprocessorFormat.setForeground(Qt::darkYellow);
   foreach (const QString &pattern, preprocessorPatterns)
   {
      rule.pattern = QRegExp(pattern);
      rule.pattern.setCaseSensitivity(Qt::CaseInsensitive);
      rule.format = dotPreprocessorFormat;
      highlightingRules.append(rule);
   }

   // Quoted Text
   quotationFormat.setForeground(Qt::darkGreen);
   rule.pattern = QRegExp("\".*\"|'.*'");
   rule.format = quotationFormat;
   highlightingRules.append(rule);

   // Single Line Comments
   // This rule has to be last or the other rules will overpaint
   singleLineCommentFormat.setForeground(Qt::gray);
   singleLineCommentFormat.setFontWeight(QFont::Normal);
   rule.pattern = QRegExp(";.*$|//.*$");
   rule.format = singleLineCommentFormat;
   highlightingRules.append(rule);
#endif
}

void CSyntaxHighlighter::highlightBlock(const QString& text)
{
   if ( (currentBlock().isVisible()) && (!text.isEmpty()) )
   {
      foreach (const HighlightingRule &rule, highlightingRules)
      {
#if 1
         QRegExp expression(rule.pattern);
         int index = expression.indexIn(text);
   
         while (index >= 0)
         {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
         }
#else
         int index = text.indexOf(rule.pattern);
   
         while (index >= 0)
         {
            int length = rule.pattern.matchedLength();
            setFormat(index, length, rule.format);
            index = text.indexOf(rule.pattern,index+length);
         }
#endif
      }
   }
}
