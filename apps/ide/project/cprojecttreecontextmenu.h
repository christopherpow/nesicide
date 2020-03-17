#ifndef CPROJECTTREECONTEXTMENU_H
#define CPROJECTTREECONTEXTMENU_H

#include "model/iuuidvisitor.h"
#include <QPoint>
#include <QWidget>
#include <QMenu>

class CProjectModel;

class CProjectTreeContextMenu : public QObject, public IUuidVisitor
{
   Q_OBJECT
public:
   CProjectTreeContextMenu(QWidget* menuParent, QPoint pos, CProjectModel* project);

   virtual void visit(CUuid&);
   virtual void visit(CAttributeUuid&);
   virtual void visit(CBinaryFileUuid&);
   virtual void visit(CFilterUuid&);
   virtual void visit(CGraphicsBankUuid&);
   virtual void visit(CSourceFileUuid&);
   virtual void visit(CTileStampUuid&);
   virtual void visit(CMusicFileUuid&);

   virtual void visit(CChrRomUuid&);
   virtual void visit(CPrgRomUuid&);

private:
   QPoint         m_position;
   QWidget*       m_parent;
   CProjectModel* m_project;
   QUuid          m_targetUuid;

   // Popup dialogs that instruct the user to do something.
   bool           confirmChoice(const QString& caption, const QString& text);
   QList<QString> selectExistingFiles(const QString& caption);
   QString        selectNewFileName();
   QString        selectNewItemName(const QString& caption, const QString& text);

   void appendGlobalMenuItems(QMenu* menu);

private slots:
   void newGraphicsBank();
   void newPalette();
   void newSourceFile();
   void newTile();
   void newScreen();
   void newMusicFile();

   void fileProperties();

   void addBinaryFile();
   void addSourceFile();
   void addMusicFile();

   void deleteGraphicsBank();
   void deletePalette();
   void deleteTile();
   void removeBinaryFile();
   void removeSourceFile();
   void removeMusicFile();
};

#endif // CPROJECTTREECONTEXTMENU_H
