/*
** Example Winamp .FTM input plug-in
** Copyright (c) 2015, Christopher Pow/Phrenetic Apps LLC.
**
*/

#include "in2.h"

#include "ftm_iface.h"

#include <QString>
#include "mainwindow.h"
#include <QApplication>
#include <QThread>

#include "Source/FamiTracker.h"
#include "Source/FamiTrackerView.h"
#include "Source/MainFrm.h"

extern void hideFamiTracker(QString s);
extern void qtMfcInit(QMainWindow* p);

int decode_pos_ms;		// current decoding position, in milliseconds.
HANDLE thread_handle=INVALID_HANDLE_VALUE;	// the handle to the decode thread


DWORD WINAPI DecodeThread(LPVOID b); // the decode thread procedure

int argc = 1;
char* argv[] = { "dummy.exe", NULL };
QCoreApplication* app;
QThread* thread;
int paused = 1;
int playing = 0;

// post this to the main window at end of file (after playback as stopped)
#define WM_WA_MPEG_EOF WM_USER+2

extern In_Module mod;

// FTM configuration.
#define SAMPLERATE 44100
#define BPS 16

void config(HWND hwndParent)
{
    MessageBox(hwndParent,
        "No configuration for .FTM files.",
        "Configuration",MB_OK);
    // if we had a configuration box we'd want to write it here (using DialogBox, etc)
}
void about(HWND hwndParent)
{
    MessageBox(hwndParent,"FamiTracker Module Player\nVersion 4.6.1.1\nby Christopher Pow",
        "About Phrenetic Apps LLC FTM Player",MB_OK);
}

QTextEdit* edit;
void init()
{
    CFamiTrackerApp* pApp = (CFamiTrackerApp*)AfxGetApp();
    DWORD thread_id;
    MainWindow* window;
    /* any one-time initialization goes here (configuration reading, etc) */
    app = new QApplication(argc, argv);
    window = new MainWindow();
    hideFamiTracker("WinAmp");
    qtMfcInit(window);
    theApp.InitInstance();

#if defined(USE_DEBUG)
    edit = new QTextEdit();
    window->setCentralWidget(edit);
    window->show();
#endif

    thread_handle = (HANDLE)
        CreateThread(NULL,0,(LPTHREAD_START_ROUTINE) DecodeThread,NULL,0,&thread_id);
}

void clear()
{
#if defined(USE_DEBUG)
    edit->clear();
#endif
}

void dump(const char* s)
{
#if defined(USE_DEBUG)
    edit->append(s);
#endif
}

void quit()
{
    /* one-time deinit, such as memory freeing */
//    theApp.ExitInstance();
}

int isourfile(const char *fn)
{
// used for detecting URL streams.. unused here.
// return !strncmp(fn,"http://",4); to detect HTTP streams, etc
    return 0;
}


// called when winamp wants to play a file
int play(const char *fn)
{
    CFamiTrackerApp* pApp = (CFamiTrackerApp*)AfxGetApp();
    CMainFrame* pMainFrame = (CMainFrame*)pApp->m_pMainWnd;
    CFamiTrackerDoc* pDoc = (CFamiTrackerDoc*)pMainFrame->GetActiveDocument();
    int maxlatency;

    dump("play:entered");
    dump(fn);
    pApp->StopPlayer();

    pDoc = (CFamiTrackerDoc*)openFile(fn);

    if ( pDoc )
    {
        pApp->StartPlayer(MODE_PLAY_START);

        // -1 and -1 are to specify buffer and prebuffer lengths.
        // -1 means to use the default, which all input plug-ins should
        // really do.
        maxlatency = mod.outMod->Open(SAMPLERATE,1,BPS, -1,-1);

        // maxlatency is the maxium latency between a outMod->Write() call and
        // when you hear those samples. In ms. Used primarily by the visualization
        // system.

        if (maxlatency < 0) // error opening device
        {
            return 1;
        }

        // dividing by 1000 for the first parameter of setinfo makes it
        // display 'H'... for hundred.. i.e. 14H Kbps.
        mod.SetInfo((SAMPLERATE*BPS)/1000,SAMPLERATE/1000,1,0);

        // initialize visualization stuff
        mod.SAVSAInit(maxlatency,SAMPLERATE);
        mod.VSASetInfo(SAMPLERATE,1);

        // set the output plug-ins default volume.
        // volume is 0-255, -666 is a token for
        // current volume.
        mod.outMod->SetVolume(-666);

        paused=0;
        playing = 1;
        decode_pos_ms = 0;
    //    seek_needed=-1;

        dump("play:exit = 0");
        return 0;
    }
    else
    {
        dump("play:exit = 1");
        return 1;
    }
}

// standard pause implementation
void pause()
{
    paused=1;
    mod.outMod->Pause(1);
}
void unpause()
{
    paused=0;
    mod.outMod->Pause(0);
}
int ispaused()
{
    return paused;
}


// stop playing.
void stop()
{
    dump("stop:enter");
    CFamiTrackerApp* pApp = (CFamiTrackerApp*)AfxGetApp();

    // close output system
    mod.outMod->SetVolume(0);
    mod.outMod->Close();

    playing = 0;

    // deinitialize visualization
    mod.SAVSADeInit();
    dump("stop:exit");
}


// returns length of playing track
int getlength()
{
    CFamiTrackerApp* pApp = (CFamiTrackerApp*)AfxGetApp();
    CMainFrame* pMainFrame = (CMainFrame*)pApp->m_pMainWnd;
    CFamiTrackerDoc* pDoc = (CFamiTrackerDoc*)pMainFrame->GetActiveDocument();
    int l = pDoc->GetFrameCount(pMainFrame->GetSelectedTrack())*pDoc->GetPatternLength(pMainFrame->GetSelectedTrack());
    return 180000;
}


// returns current output position, in ms.
// you could just use return mod.outMod->GetOutputTime(),
// but the dsp plug-ins that do tempo changing tend to make
// that wrong.
int getoutputtime()
{
    return mod.outMod->GetOutputTime();
}


// called when the user releases the seek scroll bar.
// usually we use it to set seek_needed to the seek
// point (seek_needed is -1 when no seek is needed)
// and the decode thread checks seek_needed.
void setoutputtime(int time_in_ms)
{
    CFamiTrackerApp* pApp = (CFamiTrackerApp*)AfxGetApp();
    CMainFrame* pMainFrame = (CMainFrame*)pApp->m_pMainWnd;
    CFamiTrackerDoc* pDoc = (CFamiTrackerDoc*)pMainFrame->GetActiveDocument();
    CFamiTrackerView* pView = (CFamiTrackerView*)pMainFrame->GetActiveView();
    pView->SelectFrame(time_in_ms/pDoc->GetPatternLength(pMainFrame->GetSelectedTrack()));
}


// standard volume/pan functions
void setvolume(int volume)
{
    mod.outMod->SetVolume(volume);
}
void setpan(int pan)
{
    mod.outMod->SetPan(pan);
}

// this gets called when the use hits Alt+3 to get the file info.
// if you need more info, ask me :)

int infoDlg(const char *fn, HWND hwnd)
{
    // CHANGEME! Write your own info dialog code here
    return 0;
}


// this is an odd function. it is used to get the title and/or
// length of a track.
// if filename is either NULL or of length 0, it means you should
// return the info of lastfn. Otherwise, return the information
// for the file in filename.
// if title is NULL, no title is copied into it.
// if length_in_ms is NULL, no length is copied into it.
void getfileinfo(const char *filename, char *title, int *length_in_ms)
{
    CFamiTrackerApp* pApp = (CFamiTrackerApp*)AfxGetApp();
    CMainFrame* pMainFrame = (CMainFrame*)pApp->m_pMainWnd;
    CFamiTrackerDoc* pDoc = (CFamiTrackerDoc*)pMainFrame->GetActiveDocument();

    if (!filename || !*filename)  // currently playing file
    {
        if (length_in_ms) *length_in_ms=getlength();
        if (title) // get non-path portion.of filename
        {
            strcpy(title,pDoc->GetFileTitle());
        }
    }
//    else // some other file
//    {
//        if (length_in_ms) // calculate length
//        {
//            HANDLE hFile;
//            hFile = CreateFile(filename,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,
//                OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
//            if (hFile != INVALID_HANDLE_VALUE)
//            {
//                *length_in_ms = (GetFileSize(hFile,NULL)*10)/(SAMPLERATE/100*(BPS/8));
//                CloseHandle(hFile);
//            }
//            else *length_in_ms=-1000; // the default is unknown file length (-1000).
//        }
//        if (title) // get non path portion of filename
//        {
//            const char *p=filename+strlen(filename);
//            while (*p != '\\' && p >= filename) p--;
//            strcpy(title,++p);
//        }
//    }
}

void eq_set(int on, char data[10], int preamp)
{
    // most plug-ins can't even do an EQ anyhow.. I'm working on writing
    // a generic PCM EQ, but it looks like it'll be a little too CPU
    // consuming to be useful :)
    // if you _CAN_ do EQ with your format, each data byte is 0-63 (+20db <-> -20db)
    // and preamp is the same.
}


// render 576 samples into buf.
// this function is only used by DecodeThread.

// note that if you adjust the size of sample_buffer, for say, 1024
// sample blocks, it will still work, but some of the visualization
// might not look as good as it could. Stick with 576 sample blocks
// if you can, and have an additional auxiliary (overflow) buffer if
// necessary..
extern "C" void SDL_FamiTracker(void* userdata, char* stream, int len);

int get_576_samples(char *buf)
{
    SDL_FamiTracker(NULL,buf,1152);
    return 1152;
}

DWORD WINAPI DecodeThread(LPVOID b)
{
    char sample_buffer[576*(BPS/8)*2];
    char zero_buffer[576*(BPS/8)*2];
    int l;

    memset(zero_buffer,0,576*(BPS/8)*2);

    while(1)
    {
        CFamiTrackerApp* pApp = (CFamiTrackerApp*)AfxGetApp();
        CMainFrame* pMainFrame = (CMainFrame*)pApp->m_pMainWnd;
        CFamiTrackerDoc* pDoc = (CFamiTrackerDoc*)pMainFrame->GetActiveDocument();

        memset(sample_buffer,0,576*(BPS/8)*2);

        if (mod.outMod && playing)
        {
            if (getoutputtime() >= getlength() )
            {
                // we're done playing, so tell Winamp and quit the thread.
                PostMessage(mod.hMainWindow,WM_WA_MPEG_EOF,0,0);
                Sleep(10);		// give a little CPU time back to the system.
            }
//            if (!pApp->GetSoundGenerator()->IsPlaying()) // done was set to TRUE during decoding, signaling eof
//            {
//                mod.outMod->CanWrite();		// some output drivers need CanWrite
//                                            // to be called on a regular basis.

//                if (!mod.outMod->IsPlaying())
//                {
//                    // we're done playing, so tell Winamp and quit the thread.
//                    PostMessage(mod.hMainWindow,WM_WA_MPEG_EOF,0,0);
//                }
//                Sleep(10);		// give a little CPU time back to the system.
//            }
//            else

            if ((l=mod.outMod->CanWrite()) >= (1152*(mod.dsp_isactive()?2:1)))
                // CanWrite() returns the number of bytes you can write, so we check that
                // to the block size. the reason we multiply the block size by two if
                // mod.dsp_isactive() is that DSP plug-ins can change it by up to a
                // factor of two (for tempo adjustment).
            {
                get_576_samples(sample_buffer);	   // retrieve samples

                // give the samples to the vis subsystems
                mod.SAAddPCMData((char *)sample_buffer,1,BPS,decode_pos_ms);
                mod.VSAAddPCMData((char *)sample_buffer,1,BPS,decode_pos_ms);

                // adjust decode position variable
                decode_pos_ms+=(576*1000)/SAMPLERATE;

                // if we have a DSP plug-in, then call it on our samples
                if (mod.dsp_isactive())
                    l=mod.dsp_dosamples(
                        (short *)sample_buffer,l/(BPS/8),BPS,1,SAMPLERATE
                      ) // dsp_dosamples
                      *(BPS/8);

                // write the pcm data to the output system
                mod.outMod->Write(sample_buffer,1152);
            }
            else
            {
                Sleep(10);
            }
        }
        else if (mod.outMod && ((l=mod.outMod->CanWrite()) >= (1152*(mod.dsp_isactive()?2:1))))
        {
            get_576_samples(sample_buffer);	   // retrieve samples
            // throw them away
            // give the samples to the vis subsystems
            mod.SAAddPCMData((char *)zero_buffer,1,BPS,decode_pos_ms);
            mod.VSAAddPCMData((char *)zero_buffer,1,BPS,decode_pos_ms);
            mod.outMod->Write(zero_buffer,1152);
        }
        else
        {
            Sleep(10);
        }
    }
    return 0;
}
