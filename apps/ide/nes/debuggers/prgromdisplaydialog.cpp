#include "prgromdisplaydialog.h"
#include "ui_prgromdisplaydialog.h"

#include "qscilexerbin.h"

#include "environmentsettingsdialog.h"

#include "codeeditorform.h"
#include "nes_emulator_core.h"

PRGROMDisplayDialog::PRGROMDisplayDialog(uint8_t* bankData,IProjectTreeViewItem* link,QWidget* parent) :
   CDesignerEditorBase(link,parent),
   m_data(0),
   ui(new Ui::PRGROMDisplayDialog)
{
   ui->setupUi(this);

   m_data = bankData;

   QString text;
   if ( m_data )
   {
      char    temp[4];

      for (int i=0; i<MEM_8KB; i++)
      {
         char l = (m_data[i]>>4)&0x0F;
         char r = m_data[i]&0x0F;
         if ( i && (!(i%16)) )
            text.append("\n");
         else if ( i )
            text.append(" " );
         sprintf(temp,"%01X%01X",l,r);
         text.append(temp);
      }
   }

   m_scintilla = new QsciScintilla();

   QObject::connect(m_scintilla, SIGNAL(linesChanged()), this, SLOT(linesChanged()));

   m_lexer = new QsciLexerBin(m_scintilla);

   m_scintilla->setLexer(m_lexer);

   m_scintilla->setFrameShape(QFrame::NoFrame);

   m_scintilla->setText(text);
   m_scintilla->setReadOnly(true);

   m_scintilla->setMarginsBackgroundColor(EnvironmentSettingsDialog::marginBackgroundColor());
   m_scintilla->setMarginsForegroundColor(EnvironmentSettingsDialog::marginForegroundColor());

   m_scintilla->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
   m_scintilla->setMarginWidth(3,0);
   m_scintilla->setMarginMarkerMask(3,0);
   m_scintilla->setMarginWidth(4,0);
   m_scintilla->setMarginMarkerMask(4,0);

   m_scintilla->setMarginWidth(2,5);
   m_scintilla->setMarginType(2,QsciScintilla::SymbolMargin);
   m_scintilla->setMarginSensitivity(2,false);
   m_scintilla->setMarginMarkerMask(2,0x00000FF0);

   m_scintilla->setMarginWidth(Margin_Decorations,22);
   m_scintilla->setMarginMarkerMask(Margin_Decorations,0x0000000F);
   m_scintilla->setMarginType(Margin_Decorations,QsciScintilla::SymbolMargin);
   m_scintilla->setMarginSensitivity(Margin_Decorations,false);

   m_scintilla->setMarginLineNumbers(Margin_LineNumbers,true);
   m_scintilla->setMarginWidth(Margin_LineNumbers,0);
   m_scintilla->setMarginMarkerMask(Margin_LineNumbers,0x00000000);
   m_scintilla->setMarginType(Margin_LineNumbers,QsciScintilla::TextMargin);
   m_scintilla->setMarginSensitivity(Margin_LineNumbers,false);

   m_scintilla->setSelectionBackgroundColor(QColor(215,215,215));
   m_scintilla->setSelectionToEol(true);

   m_scintilla->markerDefine(QsciScintilla::Background,Marker_Highlight);
   if ( EnvironmentSettingsDialog::highlightBarEnabled() )
   {
      m_scintilla->setMarkerBackgroundColor(EnvironmentSettingsDialog::highlightBarColor(),Marker_Highlight);
   }
   else
   {
      // Set the highlight bar color to background to hide it =]
      m_scintilla->setMarkerBackgroundColor(m_lexer->defaultPaper(),Marker_Highlight);
   }

   QString maxLineNum;

   maxLineNum.sprintf("%d",m_scintilla->lines()<<4);

   m_scintilla->setMarginWidth(Margin_LineNumbers,maxLineNum);
   
   setCentralWidget(m_scintilla);
}

PRGROMDisplayDialog::~PRGROMDisplayDialog()
{
   delete ui;

   delete m_lexer;
   delete m_scintilla;
}

void PRGROMDisplayDialog::changeEvent(QEvent* e)
{
   QWidget::changeEvent(e);

   switch (e->type())
   {
      case QEvent::LanguageChange:
         ui->retranslateUi(this);
         break;
      default:
         break;
   }
}

void PRGROMDisplayDialog::linesChanged()
{
   int line;

   for ( line = 0; line < m_scintilla->lines(); line++ )
   {
      m_scintilla->setMarginText(line,QString("%1").arg(line<<4,4,16,QChar('0')),QsciScintillaBase::STYLE_LINENUMBER);
   }
}

void PRGROMDisplayDialog::showEvent(QShowEvent* /*e*/)
{
}

void PRGROMDisplayDialog::applyProjectPropertiesToTab()
{
}
