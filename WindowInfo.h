#ifndef WINDOWINFO_H
#define WINDOWINFO_H

#define WINDOW_TITLE_SIZE 128

struct WindowInfo
{
    int x, y, w, h; // inner size
    int dpi;
#if defined(_WIN32)
    char title[WINDOW_TITLE_SIZE];
#else
    char* displayName;
#endif
};

bool GetInfoOfActiveWindow(WindowInfo* info);

#endif
