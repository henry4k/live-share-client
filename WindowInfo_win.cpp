#include <assert.h>
#include <stdio.h> // DEBUG
#include <Windows.h>
#include "WindowInfo.h"


bool GetInfoOfActiveWindow(WindowInfo* info)
{
    const HWND windowHandle = GetForegroundWindow();
    if(windowHandle == NULL)
    {
        printf("GetForegroundWindow failed\n");
        return false;
    }

    {
        POINT point = {0, 0}; // translate the top-left client coordinates
        if(!ClientToScreen(windowHandle, &screenPoint))
        {
            pritnf("ClientToScreen failed\n");
            return false;
        }

        info->x = (int)point.x;
        info->y = (int)point.y;
    }

    {
        RECT clientRect;
        if(!GetClientRect(windowHandle, &clientRect))
        {
            printf("GetClientRect failed\n");
            return false;
        }

        assert(clientRect.left == 0);
        assert(clientRect.top  == 0);

        // The manual says that the bottom-right coordinates are *exclusive*.
        // This means that they lie immediately outside the rectangle.
        // To get back into the rectangle we need to substract one:
        info->w = (int)clientRect.right-1;
        info->h = (int)clientRect.bottom-1;
    }

    if(!GetWindowTextA(windowHandle, info->title, WINDOW_TITLE_SIZE))
    {
        printf("GetWindowText failed\n");
        return false;
    }

    info->dpi = (int)GetDpiForWindow(windowHandle);
    if(!info->dpi)
    {
        printf("GetDpiForWindow failed\n");
        return false;
    }

    return true;
}
