#pragma once

#if defined(__cplusplus)
extern "C"
{
#endif
void config(HWND hwndParent);
void about(HWND hwndParent);
void init();
void quit();
int isourfile(const char *fn);
int play(const char *fn);
void pause();
void unpause();
int ispaused();
void stop();
int getlength();
int getoutputtime();
void setoutputtime(int time_in_ms);
void setvolume(int volume);
void setpan(int pan);
int infoDlg(const char *fn, HWND hwnd);
void getfileinfo(const char *filename, char *title, int *length_in_ms);
void eq_set(int on, char data[10], int preamp);
int get_576_samples(char *buf);
#if defined(__cplusplus)
}
#endif
