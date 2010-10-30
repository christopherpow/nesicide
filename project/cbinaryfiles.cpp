#include "cbinaryfiles.h"

CBinaryFiles::CBinaryFiles()
{
    m_pBinaryFileList = new QList<CBinaryFile *>();
}

CBinaryFiles::~CBinaryFiles()
{
    if (m_pBinaryFileList)
    {
        delete m_pBinaryFileList;
    }
}

QList<CBinaryFile *> *CBinaryFiles::getBinaryFileList()
{
    return m_pBinaryFileList;
}

void CBinaryFiles::setBinaryFileList(QList<CBinaryFile *> * newBinaryFileList)
{
    m_pBinaryFileList = newBinaryFileList;
}

bool CBinaryFiles::serialize(QDomDocument &doc, QDomNode &node)
{
    QDomElement binaryFilesElement = addElement( doc, node, "binaryfiles" );

    for (int sourceItemIdx = 0; sourceItemIdx < m_pBinaryFileList->count(); sourceItemIdx++)
    {
        if (!m_pBinaryFileList->at(sourceItemIdx))
            return false;
        if (!m_pBinaryFileList->at(sourceItemIdx)->serialize(doc, binaryFilesElement))
            return false;
    }
    return true;
}

bool CBinaryFiles::deserialize(QDomDocument &doc, QDomNode &node)
{
    if (m_pBinaryFileList)
    {
        for (int indexOfSourceItem=0; indexOfSourceItem<m_pBinaryFileList->count(); indexOfSourceItem++)
        {
            if (m_pBinaryFileList->at(indexOfSourceItem))
            {
                this->removeChild(m_pBinaryFileList->at(indexOfSourceItem));
                delete m_pBinaryFileList->at(indexOfSourceItem);
            }
        }
        delete m_pBinaryFileList;
    }

    m_pBinaryFileList = new QList<CBinaryFile *>();

    QDomNode childNode = node.firstChild();
    if (!childNode.isNull()) do
    {
        if (childNode.nodeName() == "binaryitem") {

            CBinaryFile *pNewBinaryFile = new CBinaryFile();
            pNewBinaryFile->InitTreeItem(this);
            m_pBinaryFileList->append(pNewBinaryFile);
            this->appendChild(pNewBinaryFile);
            if (!pNewBinaryFile->deserialize(doc, childNode))
                return false;

        } else
            return false;
    } while (!(childNode = childNode.nextSibling()).isNull());

    return true;
}

QString CBinaryFiles::caption() const
{
    return QString("Binary Files");
}


void CBinaryFiles::contextMenuEvent(QContextMenuEvent* event, QTreeView* parent)
{
    QMenu menu(parent);
    menu.addAction("&Import Binary File...");

    QAction *ret = menu.exec(event->globalPos());
    if (ret)
    {
        if (ret->text() == "&Import Binary File...")
        {

            QString fileName = QFileDialog::getOpenFileName(parent, "Import Binary File", "", "All Files (*.*)");
            if (QFile::exists(fileName))
            {
                CBinaryFile *pNewBinaryFile = new CBinaryFile();
                QFile file(fileName);
                if (file.open(QFile::ReadOnly))
                {
                    pNewBinaryFile->setBinaryData(new QByteArray(file.readAll()));
                    pNewBinaryFile->setBinaryName(QFileInfo(file).fileName());
                    file.close();
                }
                pNewBinaryFile->InitTreeItem(this);
                m_pBinaryFileList->append(pNewBinaryFile);
                this->appendChild(pNewBinaryFile);
                ((CProjectTreeViewModel *)parent->model())->layoutChangedEvent();
            }
        }
    }
}
