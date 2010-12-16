#ifndef PRGROMDISPLAYDIALOG_H
#define PRGROMDISPLAYDIALOG_H

#include <QDialog>

namespace Ui
{
class PRGROMDisplayDialog;
}

class PRGROMDisplayDialog : public QDialog
{
   Q_OBJECT
public:
   PRGROMDisplayDialog(QWidget* parent = 0);
   ~PRGROMDisplayDialog();
   void setRomData(unsigned char* data)
   {
      m_data = data;
   }

protected:
   void changeEvent(QEvent* e);
   void showEvent(QShowEvent* e);

protected:
   unsigned char* m_data;

private:
   Ui::PRGROMDisplayDialog* ui;
};

#endif // PRGROMDISPLAYDIALOG_H
