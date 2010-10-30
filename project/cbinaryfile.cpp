#include "cbinaryfile.h"
#include "cnesicideproject.h"

CBinaryFile::CBinaryFile()
{
    m_binaryData = new QByteArray();
}

CBinaryFile::~CBinaryFile()
{
    if (m_binaryData)
        delete m_binaryData;
}


QByteArray *CBinaryFile::getBinaryData()
{
    return m_binaryData;
}

void CBinaryFile::setBinaryName(QString newName)
{
    m_binaryName = newName;
}

bool CBinaryFile::onNameChanged(QString newName)
{
    m_binaryName = newName;
    return true;
}

void CBinaryFile::setBinaryData(QByteArray *newBinaryData)
{
    m_binaryData = newBinaryData;
}

bool CBinaryFile::serialize(QDomDocument &doc, QDomNode &node)
{
    QDomElement binaryElement = addElement( doc, node, "binaryitem" );
    binaryElement.setAttribute("binaryname", m_binaryName);
    QDomCDATASection dataSect = doc.createCDATASection(m_binaryData->toBase64());
    binaryElement.appendChild(dataSect);
    return true;
}

bool CBinaryFile::deserialize(QDomDocument &doc, QDomNode &node)
{
    QDomElement element = node.toElement();

    if (element.isNull())
        return false;

    if (!element.hasAttribute("binaryname"))
        return false;

    m_binaryName = element.attribute("binaryname");
    QDomCDATASection cdata = element.firstChild().toCDATASection();
    if (cdata.isNull())
        return false;

    m_binaryData = new QByteArray(QByteArray::fromBase64(cdata.data().toUtf8()));

    return true;
}

QString CBinaryFile::caption() const
{
    return m_binaryName;
}

void CBinaryFile::contextMenuEvent(QContextMenuEvent* event, QTreeView* parent)
{
    QMenu menu(parent);
    menu.addAction("&Delete");

    QAction *ret = menu.exec(event->globalPos());
    if (ret)
    {
        if (ret->text() == "&Delete")
        {
            if (QMessageBox::question(parent, "Delete Binary File", "Are you sure you want to delete " + m_binaryName,
                                  QMessageBox::Yes, QMessageBox::No) != QMessageBox::Yes)
                return;

            // TODO: Fix this logic so the memory doesn't get lost.
            nesicideProject->getProject()->getBinaryFiles()->removeChild(this);
            nesicideProject->getProject()->getBinaryFiles()->getBinaryFileList()->removeAll(this);
            ((CProjectTreeViewModel *)parent->model())->layoutChangedEvent();
        }
    }
}

int CBinaryFile::getChrRomBankItemSize()
{
   return this->getBinaryData()->size();
}

QByteArray* CBinaryFile::getChrRomBankItemData()
{
   return this->getBinaryData();
}

QIcon CBinaryFile::getChrRomBankItemIcon()
{
   return QIcon(":/resources/22_binary_file.png");
}
