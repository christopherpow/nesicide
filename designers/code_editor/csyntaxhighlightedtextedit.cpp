#include "csyntaxhighlightedtextedit.h"

#include "pasm_lib.h"

#include "cbreakpointinfo.h"
#include "cmarker.h"

CSyntaxHighlightedTextEdit::CSyntaxHighlightedTextEdit(QWidget*)
{
#ifdef Q_WS_MAC
   this->setFont(QFont("Monaco", 11));
#endif
#ifdef Q_WS_X11
   this->setFont(QFont("Monospace", 10));
#endif
#ifdef Q_WS_WIN
   this->setFont(QFont("Consolas", 11));
#endif
   this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
   this->setWordWrapMode(QTextOption::NoWrap);
   lineNumberArea = new LineNumberArea(this);

   connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
   connect(this, SIGNAL(updateRequest(const QRect&, int)), this, SLOT(updateLineNumberArea(const QRect&, int)));
   connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

   highlighter = new CSyntaxHighlighter(this->document());
   
   executionIcon = new QIcon(":/resources/22_execution_pointer.png");
   breakEnabledIcon = new QIcon(":/resources/22_breakpoint.png");
   breakDisabledIcon = new QIcon(":/resources/22_breakpoint_disabled.png");
   breakEnabledHitIcon = new QIcon(":/resources/22_execution_break.png");
   breakDisabledHitIcon = new QIcon(":/resources/22_execution_break_disabled.png");
   
   updateLineNumberAreaWidth(0);
   highlightCurrentLine();
}

CSyntaxHighlightedTextEdit::~CSyntaxHighlightedTextEdit()
{
   delete highlighter;
   delete lineNumberArea;
   delete executionIcon;
   delete breakEnabledIcon;
   delete breakDisabledIcon;
   delete breakEnabledHitIcon;
   delete breakDisabledHitIcon;
}

int CSyntaxHighlightedTextEdit::lineNumberAreaWidth()
{
   int digits = 1;
   int max = qMax(1, blockCount());

   while (max >= 10)
   {
      max /= 10;
      ++digits;
   }

   int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

   return space + 16;
}

void CSyntaxHighlightedTextEdit::updateLineNumberAreaWidth(int /* newBlockCount */)
{
   setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CSyntaxHighlightedTextEdit::updateLineNumberArea(const QRect& rect, int dy)
{
   if (dy)
   {
      lineNumberArea->scroll(0, dy);
   }
   else
   {
      lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
   }

   if (rect.contains(viewport()->rect()))
   {
      updateLineNumberAreaWidth(0);
   }
}

void CSyntaxHighlightedTextEdit::resizeEvent(QResizeEvent* e)
{
   QPlainTextEdit::resizeEvent(e);

   QRect cr = contentsRect();
#ifndef Q_WS_MAC
   lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
#else
   lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(),
                                     cr.height() - this->horizontalScrollBar()->height()));
#endif
}

void CSyntaxHighlightedTextEdit::highlightCurrentLine()
{
   QList<QTextEdit::ExtraSelection> extraSelections;

   if (!isReadOnly())
   {
      QTextEdit::ExtraSelection selection;

      QColor lineColor = QColor(Qt::gray).lighter(150);

      selection.format.setBackground(lineColor);
      selection.format.setProperty(QTextFormat::FullWidthSelection, true);
      selection.cursor = textCursor();
      selection.cursor.clearSelection();
      extraSelections.append(selection);
   }

   setExtraSelections(extraSelections);
}

void CSyntaxHighlightedTextEdit::lineNumberAreaPaintEvent(QPaintEvent* event)
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   int idx;
   QPainter painter(lineNumberArea);
   QTextBlock block = firstVisibleBlock();
   int blockNumber = block.blockNumber();
   int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
   int bottom = top + (int) blockBoundingRect(block).height();
   int32_t addr;
   int32_t absAddr;
   CMarker* markers = nesGetExecutionMarkerDatabase();
   MarkerSetInfo* pMarker;
   QString number;

   painter.fillRect(event->rect(), this->palette().background().color());

   while (block.isValid() && top <= event->rect().bottom())
   {
      if (block.isVisible() && bottom >= event->rect().top())
      {
         addr = pasm_get_source_addr_by_linenum_and_file(blockNumber+1,this->documentTitle().toAscii().constData());
      
         absAddr = pasm_get_source_absolute_addr_by_linenum_and_file(blockNumber+1,this->documentTitle().toAscii().constData());
            
         for ( idx = 0; idx < markers->GetNumMarkers(); idx++ )
         {
            pMarker = markers->GetMarker(idx);
   
            if ( (pMarker->state == eMarkerSet_Started) ||
                  (pMarker->state == eMarkerSet_Complete) )
            {
               if ( (absAddr >= pMarker->startAbsAddr) &&
                     (absAddr <= pMarker->endAbsAddr) )
               {
                  painter.fillRect(0,top,lineNumberArea->width(),fontMetrics().height(),QColor(pMarker->red,pMarker->green,pMarker->blue));
               }
            }
         }
         
         for ( idx = 0; idx < pBreakpoints->GetNumBreakpoints(); idx++ )
         {
            BreakpointInfo* pBreakpoint = pBreakpoints->GetBreakpoint(idx);
   
            if ( (pBreakpoint->enabled) &&
                  (pBreakpoint->type == eBreakOnCPUExecution) &&
                  (pBreakpoint->item1 <= addr) &&
                  ((absAddr == -1) || (absAddr == pBreakpoint->item1Absolute)) &&
                  (pBreakpoint->item2 >= addr) )
            {
               if ( addr == nesGetCPUProgramCounterOfLastSync() )
               {
                  breakEnabledHitIcon->paint(&painter,lineNumberArea->width() - 16,top,16,fontMetrics().height());
               }
               else
               {
                  breakEnabledIcon->paint(&painter,lineNumberArea->width() - 16,top,16,fontMetrics().height());
               }
            }
            else if ( (!pBreakpoint->enabled) &&
                      (pBreakpoint->type == eBreakOnCPUExecution) &&
                      (pBreakpoint->item1 <= addr) &&
                      ((absAddr == -1) || (absAddr == pBreakpoint->item1Absolute)) &&
                      (pBreakpoint->item2 >= addr) )
            {
               if ( addr == nesGetCPUProgramCounterOfLastSync() )
               {
                  breakDisabledHitIcon->paint(&painter,lineNumberArea->width() - 16,top,16,fontMetrics().height());
               }
               else
               {
                  breakDisabledIcon->paint(&painter,lineNumberArea->width() - 16,top,16,fontMetrics().height());
               }
            }
         }
   
         if ( absAddr == nesGetAbsoluteAddressFromAddress(nesGetCPUProgramCounterOfLastSync()) )
         {
            executionIcon->paint(&painter,lineNumberArea->width() - 16,top,16,fontMetrics().height());
         }
         
         number = QString::number(blockNumber + 1);
         painter.setPen(Qt::black);
         painter.drawText(0, top, lineNumberArea->width() - 16, fontMetrics().height(),
                          Qt::AlignRight, number);
      }

      block = block.next();
      top = bottom;
      bottom = top + (int) blockBoundingRect(block).height();
      ++blockNumber;
   }
}
