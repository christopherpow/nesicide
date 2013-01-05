#include "musiceditorform.h"
#include "ui_musiceditorform.h"

MusicEditorForm::MusicEditorForm(IProjectTreeViewItem* link,QWidget *parent) :
   CDesignerEditorBase(link,parent),
   ui(new Ui::MusicEditorForm)
{
   ui->setupUi(this);
}

MusicEditorForm::~MusicEditorForm()
{
   delete ui;
}
