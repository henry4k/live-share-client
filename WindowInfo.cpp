#include <assert.h>
#include <stdio.h> // DEBUG
#include "WindowInfo.h"

#if defined(_WIN32)
#include "Windows.h"
WindowInfo GetInfoOfActiveWindow()
{
    const HWND windowHandle = GetForegroundWindow();
    if(windowHandle == NULL)
    {
        // :(
    }
}
#else // X11
#include <X11/Xlib.h>
#include <X11/Xatom.h> // XA_WINDOW

static Window GetWindowWithInputFocus(Display* display)
{
    Window focusedWindow;
    int revertToState; // unused

    const int result = XGetInputFocus(display, &focusedWindow, &revertToState);
    if(result == BadMatch ||
       result == BadWindow ||
       result == BadValue)
    {
        printf("XGetInputFocus failed\n");
        return None;
    }

    if(focusedWindow == None ||
       focusedWindow == PointerRoot)
    {
        printf("invalid focusedWindow\n");
        return None;
    }

    return focusedWindow;
}

static Window GetWindowWithActiveTag(Display* display)
{
    const Atom activeWindowProperty =
        XInternAtom(display, "_NET_ACTIVE_WINDOW", True);
    if(activeWindowProperty == None ||
       activeWindowProperty == BadAlloc ||
       activeWindowProperty == BadValue)
    {
        printf("activeWindowProperty not there (%lu)\n", activeWindowProperty);
        return None;
    }

    Atom actualType;
    int actualFormat = 0;
    unsigned long itemCount = 0;
    unsigned long bytesLeft = 0; // bytes after return
    unsigned char* items = NULL;
    if(XGetWindowProperty(display,
                          DefaultRootWindow(display),
                          activeWindowProperty,
                          0, // long_offset,
                          1, // long_length
                          False, // don't delete
                          XA_WINDOW, // property type
                          &actualType,
                          &actualFormat,
                          &itemCount,
                          &bytesLeft,
                          &items) != Success)
    {
        printf("XGetWindowProperty failed\n");
        return None;
    }

    if(actualType != XA_WINDOW) // Not found
    {
        printf("actualType != XA_WINDOW\n");
        return None;
    }

    assert(actualFormat == 32);
    assert(itemCount == 1);
    assert(bytesLeft == 0);
    assert(items != NULL);

    Window window = *(Window*)items;

    XFree(items);

    return window;
}

static Window GetActiveWindow(Display* display)
{
    const Window window = GetWindowWithActiveTag(display);
    if(window == None)
        return GetWindowWithInputFocus(display);
    return window;
}

bool GetInfoOfActiveWindow(WindowInfo* info)
{
    char* displayName = XDisplayName(NULL);
    Display* display = XOpenDisplay(displayName);
    assert(display);

    Window window = GetActiveWindow(display);
    if(window == None)
        return false;

    int x, y;
    {
        Window child; // unused
        const int result = XTranslateCoordinates(display,
                                                window, // target
                                                DefaultRootWindow(display), // source
                                                0, // source x
                                                0, // source y
                                                &x,
                                                &y,
                                                &child);
        if(result == BadWindow)
        {
            printf("XTranslateCoordinates failed\n");
            return false;
        }
    }

    unsigned int width, height;
    {
        Window root; // unused
        int x, y; // unused
        unsigned int border_width; // unused
        unsigned int depth; // unused
        if(XGetGeometry(display, (Drawable)window, &root, &x, &y, &width, &height, &border_width, &depth) == BadDrawable)
        {
            printf("XGetGeometry failed\n");
            return false;
        }
    }

    XCloseDisplay(display);

    info->x = x;
    info->y = y;
    info->w = (int)width;
    info->h = (int)height;
    info->displayName = displayName;
    return true;
}
#endif
