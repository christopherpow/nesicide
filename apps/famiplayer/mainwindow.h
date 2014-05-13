#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "cqtmfc.h"

namespace Ui {
    class MainWindow;
}

class CWndMFC : public CDialog
{
   DECLARE_DYNAMIC(CWndMFC)

protected:
   int GetTimeLimit();

public:   
   virtual BOOL OnInitDialog();
   afx_msg void OnDeltaposSpinTime(NMHDR *pNMHDR, LRESULT *pResult);
   
   DECLARE_MESSAGE_MAP()
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
   void dragEnterEvent(QDragEnterEvent *event);
   void dragMoveEvent(QDragMoveEvent *event);
   void dropEvent(QDropEvent *event);
   void closeEvent(QCloseEvent *);
   void changeFolder(QString newFolderPath);
   void changeSong(QString newSongPath);
   void updateSubtuneText();
   void loadFile(QString file);
   void createShuffleLists();
   bool eventFilter(QObject *object, QEvent *event);

private:
    Ui::MainWindow *ui;
    bool m_bPlaying;
    bool m_bChangeSong;
    bool m_bCheck;
    bool m_bDraggingPosition;
    QTimer* m_pTimer;
    QTimer* m_pSettleTimer;
    CWndMFC* m_pWndMFC;
    QStringList m_shuffleListFolder;
    QStringList m_shuffleListSong;
    int m_iCurrentShuffleIndex;

private slots:
    void onIdleSlot();
    void settleTimer_timeout();
    void current_returnPressed();
    void documentClosed();
    void on_playStop_clicked();
    void on_browse_clicked();
    void on_next_clicked();
    void on_previous_clicked();
    void on_paths_currentIndexChanged(const QString &arg1);
    void on_current_currentIndexChanged(const QString &arg1);
    void on_subtune_currentIndexChanged(int index);
    void on_visuals_toggled(bool checked);
    void on_playOnStart_toggled(bool checked);
    void on_repeat_toggled(bool checked);
    void on_shuffle_toggled(bool checked);
    void on_timeLimit_toggled(bool checked);
};

#endif // MAINWINDOW_H
