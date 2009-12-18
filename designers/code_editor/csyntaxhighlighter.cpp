#include "csyntaxhighlighter.h"

CSyntaxHighlighter::CSyntaxHighlighter(QTextDocument *parent)
     : QSyntaxHighlighter(parent)
 {
     HighlightingRule rule;
     keywordFormat.setForeground(Qt::darkBlue);
     keywordFormat.setFontWeight(QFont::Bold);
     QStringList keywordPatterns;
     keywordPatterns
             << "\\bbne\\b"
             << "\\bbpl\\b"
             << "\\bcpx\\b"
             << "\\blda\\b"
             << "\\binx\\b"
             << "\\bjmp\\b"
             << "\\bsta\\b"

             ;
     foreach (const QString &pattern, keywordPatterns) {
         rule.pattern = QRegExp(pattern);
         rule.format = keywordFormat;
         highlightingRules.append(rule);
     }

     classFormat.setFontWeight(QFont::Bold);
     classFormat.setForeground(Qt::blue);
     rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
     rule.format = classFormat;
     highlightingRules.append(rule);

     labelFormat.setForeground(Qt::darkCyan);
     labelFormat.setFontWeight(QFont::Bold);
     rule.pattern = QRegExp("[^:\\s]*:");
     rule.format = labelFormat;
     highlightingRules.append(rule);

     dotPreprocessorFormat.setForeground(Qt::darkYellow);
     rule.pattern = QRegExp("[.][a-zA-Z]*");
     rule.format = dotPreprocessorFormat;
     highlightingRules.append(rule);

     quotationFormat.setForeground(Qt::darkGreen);
     rule.pattern = QRegExp("\".*\"");
     rule.format = quotationFormat;
     highlightingRules.append(rule);

     singleLineCommentFormat.setForeground(Qt::red);
     rule.pattern = QRegExp(";.*$");
     rule.format = singleLineCommentFormat;
     highlightingRules.append(rule);

 }

 void CSyntaxHighlighter::highlightBlock(const QString &text)
 {
     foreach (const HighlightingRule &rule, highlightingRules) {
         QRegExp expression(rule.pattern);
         int index = expression.indexIn(text);
         while (index >= 0) {
             int length = expression.matchedLength();
             setFormat(index, length, rule.format);
             index = expression.indexIn(text, index + length);
         }
     }
 }
