#ifndef GRAPHICSBANKEDITORFORM_H
#define GRAPHICSBANKEDITORFORM_H

#include <QList>

#include "cdesignereditorbase.h"
#include "cchrromitemtabledisplaymodel.h"
#include "cchrrombankitemdelegate.h"
#include "ichrrombankitem.h"
#include "iprojecttreeviewitem.h"

#include "panzoomrenderer.h"
#include "tilificationthread.h"

// CHR bank 'sides'
#define LEFT 0
#define RIGHT 1

namespace Ui
{
class GraphicsBankEditorForm;
}

class GraphicsBankEditorForm : public CDesignerEditorBase
{
   Q_OBJECT
public:
   GraphicsBankEditorForm(QList<IChrRomBankItem*> leftBankItems,QList<IChrRomBankItem*> rightBankItems,IProjectTreeViewItem* link = 0,QWidget* parent = 0);
   virtual ~GraphicsBankEditorForm();
   void updateChrRomBankItemList(QList<IChrRomBankItem*> leftBankItems,QList<IChrRomBankItem*> rightBankItems);

   // Member getters.
   QList<IChrRomBankItem*> bankItems(int side);

protected:
   void changeEvent(QEvent* event);
   void keyPressEvent(QKeyEvent *event);
   void showEvent(QShowEvent *event);

private:
   Ui::GraphicsBankEditorForm* ui;
   CChrRomItemTableDisplayModel* leftModel;
   CChrRomItemTableDisplayModel* rightModel;
   PanZoomRenderer* renderer;
   CChrRomBankItemDelegate* delegate;
   char* imgData;
   TilificationThread* pLeftThread;
   TilificationThread* pRightThread;
   QByteArray tilifiedData;

private slots:
   void renderData();
   void renderData(int side,QByteArray output);
   void updateUi();
   void snapTo(QString item);
   void applyChangesToTab(QString uuid);
   void applyProjectPropertiesToTab();

signals:
   void prepareToTilify(int side);
   void addToTilificator(int side,IChrRomBankItem* item);
   void tilify(int side);
};

#endif // GRAPHICSBANKEDITORFORM_H
