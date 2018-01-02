#ifndef WINDOWINFO_H
#define WINDOWINFO_H

struct WindowInfo
{
    int x, y, w, h; // inner size
#if defined(_WIN32)
    char title[128];
#else
    char* displayName;
#endif
};

bool GetInfoOfActiveWindow(WindowInfo* info);

#endif
