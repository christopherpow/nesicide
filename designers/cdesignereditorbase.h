#ifndef CDESIGNEREDITORBASE_H
#define CDESIGNEREDITORBASE_H

#include <QWidget>

#include "icenterwidgetitem.h"
#include "iprojecttreeviewitem.h"

class CDesignerEditorBase : public QWidget, public ICenterWidgetItem
{
    Q_OBJECT
public:
    explicit CDesignerEditorBase(IProjectTreeViewItem* link,QWidget *parent = 0);
    virtual ~CDesignerEditorBase() {};

    virtual bool isModified() { return m_isModified; }
    virtual void setModified(bool modified) { m_isModified = modified; emit editor_modified(modified);}

    // ICenterWidgetItem Interface Implmentation
    virtual bool onCloseQuery();
    virtual void onClose();
    virtual bool onSaveQuery();
    virtual void onSave();

protected:
    void keyPressEvent(QKeyEvent *e);

signals:
    void editor_modified(bool m);

public slots:

protected:
    bool m_isModified;

};

#endif // CDESIGNEREDITORBASE_H
