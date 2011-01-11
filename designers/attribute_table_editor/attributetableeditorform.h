#ifndef ATTRIBUTETABLEEDITORFORM_H
#define ATTRIBUTETABLEEDITORFORM_H

#include <QWidget>

namespace Ui
{
class AttributeTableEditorForm;
}

class AttributeTableEditorForm : public QWidget
{
   Q_OBJECT
public:
   AttributeTableEditorForm(QWidget* parent = 0);
   ~AttributeTableEditorForm();

protected:
   void changeEvent(QEvent* e);

private:
   Ui::AttributeTableEditorForm* ui;

private slots:
};

#endif // ATTRIBUTETABLEEDITORFORM_H
