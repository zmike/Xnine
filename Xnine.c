/*
 * Copyright © 2014 Axel Davy
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */ 

#include <stdio.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/time.h> 
#include <assert.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <d3d9.h>
#include <d3dadapter/d3dadapter9.h>
#include <d3dadapter/drm.h>
#include "Xnine.h"
#include "dri3.h"

#define IsEqualGUID(rguid1, rguid2) (!memcmp(rguid1, rguid2, sizeof(GUID)))


struct Xnine_window {
    HWND win;
    int width;
    int height;
    int border_width_left;
    int border_width_right;
    int border_height_top;
    int border_height_bottom;
    BOOL fullscreen;
    int depth;
    Window Xwindow;
    struct Xnine_window *next;
};

struct Xnine_private {
    int scrnum;
    Display *dpy;
    int screen_width;
    int screen_height;
    struct D3DAdapter9DRM *d3d9_drm;
#if D3DADAPTER9_WITHDRI2
    int is_dri2_fallback;
#endif
    struct Xnine_window *window_list;
};

static struct Xnine_window *find_window(struct Xnine_private *priv, HWND win)
{
    struct Xnine_window *w = priv->window_list;

    while (w) {
        if (w->win == win)
            break;
        w = w->next;
    }
    return w;
}

static void remove_window(struct Xnine_private *priv, HWND win)
{
    struct Xnine_window *o = NULL, *w = priv->window_list;

    if (w->win == win) {
        priv->window_list = w->next;
        return;
    }

    o = w;
    w = w->next;

    while (w) {
        if (w->win == win)
            break;
        o = w;
        w = w->next;
    }
    if (w)
        o->next = w->next;
}

static HWND add_window(struct Xnine_private *priv, struct Xnine_window *Xnine_window)
{
    struct Xnine_window *w = priv->window_list;

    if (!w) {
        priv->window_list = Xnine_window;
        Xnine_window->win = (HWND)1;
        return (HWND)1;
    }

    while (w->next) {
        int index = (int)(uintptr_t) w->win;
        w = w->next;
    }
    /* indices grow */
    Xnine_window->win = (HWND) ((uintptr_t) w->win + 1);
    w->next = Xnine_window;
    return (HWND)Xnine_window->win;
}


///// X Helper functions

BOOL Xnine_create_window(struct Xnine_private *priv, int width, int height, BOOL fullscreen, HWND *hwnd)
{
    Window Xwindow;
    struct Xnine_window *Xnine_win;
    XSetWindowAttributes attr;

    assert(!fullscreen);
    attr.background_pixel = 0;
    attr.border_pixel = 0;
    attr.event_mask = 0;
    
    Xwindow = XCreateWindow(priv->dpy,
                            RootWindow(priv->dpy, priv->scrnum),
                            0, 0, width, height, 0, 24, InputOutput,
                            CopyFromParent, CWBorderPixel, &attr);
    if (!Xwindow)
        return FALSE;

    Xnine_win = calloc(1, sizeof(struct Xnine_window));
    if (!Xnine_win) {
        XDestroyWindow(priv->dpy, Xwindow);
        return FALSE;
    }

    Xnine_win->width = width;
    Xnine_win->height = height;
    Xnine_win->border_width_left = 0;
    Xnine_win->border_width_right = 0;
    Xnine_win->border_height_top = 0;
    Xnine_win->border_height_bottom = 0;
    Xnine_win->fullscreen = FALSE;
    Xnine_win->depth = 24;
    Xnine_win->Xwindow = Xwindow;
    XMapWindow(priv->dpy, Xwindow);
    *hwnd = add_window(priv, Xnine_win);
    return TRUE;
}

BOOL Xnine_resize_window(struct Xnine_private *priv, HWND win, int width, int height, BOOL fullscreen)
{
    struct Xnine_window *Xnine_win = find_window(priv, win);

    if (!Xnine_win || fullscreen)
        return FALSE;
    XResizeWindow(priv->dpy, Xnine_win->Xwindow, width, height);
    return TRUE;
}

void Xnine_destroy_window(struct Xnine_private *priv, HWND win)
{
    struct Xnine_window *Xnine_win = find_window(priv, win);

    if (!Xnine_win)
        return;
    remove_window(priv, win);
    XDestroyWindow(priv->dpy, Xnine_win->Xwindow);
    free(Xnine_win);
}

///// ID3DPresent

struct D3DPresent_Xnine {
    void *vtable;
    int32_t refs;

    struct Xnine_private *priv;
    HWND focus_wnd;

    D3DPRESENT_PARAMETERS present_params;
    PRESENTpriv *present_priv;
#if D3DADAPTER9_WITHDRI2
    struct DRI2priv *dri2_priv;
#endif
};

const GUID IID_IUnknown = { 0x00000000, 0x0000, 0x0000, { 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 } };
const GUID IID_ID3DPresent = { 0x77D60E80, 0xF1E6, 0x11DF, { 0x9E, 0x39, 0x95, 0x0C, 0xDF, 0xD7, 0x20, 0x85 } };
const GUID IID_ID3DPresentGroup = { 0xB9C3016E, 0xF32A, 0x11DF, { 0x9C, 0x18, 0x92, 0xEA, 0xDE, 0xD7, 0x20, 0x85 } };
const GUID IID_IDirect3D9 = { 0x81BDCBCA, 0x64D4, 0x426D, { 0xAE, 0x8D, 0xAD, 0x1, 0x47, 0xF4, 0x27, 0x5C } };
const GUID IID_IDirect3D9Ex = { 0x02177241, 0x69FC, 0x400C, { 0x8F, 0xF1, 0x93, 0xA4, 0x4D, 0xF6, 0x86, 0x1D } };

static ULONG WINAPI Xnine_AddRef(struct D3DPresent_Xnine *This);

static HRESULT WINAPI
Xnine_QueryInterface(struct D3DPresent_Xnine *This,
                     REFIID riid,
                     void **ppvObject)
{
    if (!ppvObject) { return E_POINTER; }

    if (IsEqualGUID(&IID_ID3DPresent, riid) ||
        IsEqualGUID(&IID_IUnknown, riid)) {
        *ppvObject = This;
        Xnine_AddRef(This);
        return S_OK;
    }
    *ppvObject = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI
Xnine_AddRef(struct D3DPresent_Xnine *This)
{
    uint32_t refs = __sync_add_and_fetch(&This->refs, 1);
    return refs;
}

static ULONG WINAPI
Xnine_Release(struct D3DPresent_Xnine *This)
{
    uint32_t refs =  __sync_sub_and_fetch(&This->refs, 1);
    if (refs == 0) {
        PRESENTDestroy(This->priv->dpy, This->present_priv);
#if D3DADAPTER9_WITHDRI2
        if (This->priv->is_dri2_fallback)
            DRI2FallbackDestroy(This->dri2_priv);
#endif
        free(This);
    }
    return refs;
}

static HRESULT WINAPI
Xnine_SetPresentParameters(struct D3DPresent_Xnine *This,
                           D3DPRESENT_PARAMETERS *pPresentationParameters,
                           D3DDISPLAYMODEEX *pFullscreenDisplayMode)
{
    HWND draw_window_hwnd = pPresentationParameters->hDeviceWindow ?
                            pPresentationParameters->hDeviceWindow :
                            This->focus_wnd;
    struct Xnine_window *draw_window = find_window(This->priv, draw_window_hwnd);
    // TODO: if pFullscreenDisplayMode is set, use it to set fullscreen mode

    if (pPresentationParameters->BackBufferWidth == 0)
        pPresentationParameters->BackBufferWidth = draw_window->width;
    if (pPresentationParameters->BackBufferHeight == 0)
        pPresentationParameters->BackBufferHeight = draw_window->height;

    if (!pPresentationParameters->Windowed) {
        //TODO: set fullscreen
    } else if (!This->present_params.Windowed) {
        //TODO: unset fullscreen
    }

    This->present_params = *pPresentationParameters;
    return D3D_OK;
}

struct D3DWindowBuffer
{
    PRESENTPixmapPriv *present_pixmap_priv;
};

static HRESULT WINAPI
Xnine_D3DWindowBufferFromDmaBuf(struct D3DPresent_Xnine *This, int dmaBufFd,
                                int width, int height, int stride, int depth,
                                int bpp, struct D3DWindowBuffer **out)
{
    Pixmap pixmap;

#if D3DADAPTER9_WITHDRI2
    if (This->priv->is_dri2_fallback) {
        *out = calloc(1, sizeof(struct D3DWindowBuffer));
        DRI2FallbackPRESENTPixmap(This->present_priv, This->dri2_priv,
                                  dmaBufFd, width, height, stride, depth,
                                  bpp,
                                  &((*out)->present_pixmap_priv));
        return D3D_OK;
    }
#endif
    if (!DRI3PixmapFromDmaBuf(This->priv->dpy, DefaultScreen(This->priv->dpy),
                              dmaBufFd, width, height, stride, depth,
                              bpp, &pixmap ))
        return D3DERR_DRIVERINTERNALERROR;

    *out = calloc(1, sizeof(struct D3DWindowBuffer));
    PRESENTPixmapInit(This->present_priv, pixmap, &((*out)->present_pixmap_priv));
    return D3D_OK;
}

static HRESULT WINAPI
Xnine_DestroyD3DWindowBuffer(struct D3DPresent_Xnine *This,
                             struct D3DWindowBuffer *buffer)
{
    (void) This;
    PRESENTTryFreePixmap(This->priv->dpy, buffer->present_pixmap_priv);
    free(buffer);
    return D3D_OK;
}

static HRESULT WINAPI
Xnine_WaitBufferReleased(struct D3DPresent_Xnine *This,
                         struct D3DWindowBuffer *buffer)
{
    (void) This;
    PRESENTWaitPixmapReleased(buffer->present_pixmap_priv);
    return D3D_OK;
}

static HRESULT WINAPI
Xnine_FrontBufferCopy(struct D3DPresent_Xnine *This,
                      struct D3DWindowBuffer *buffer)
{
#if D3DADAPTER9_WITHDRI2
    if (This->priv->is_dri2_fallback)
        return D3DERR_DRIVERINTERNALERROR;
#endif
    if (PRESENTHelperCopyFront(This->priv->dpy, buffer->present_pixmap_priv))
        return D3D_OK;
    else
        return D3DERR_DRIVERINTERNALERROR;
}

static HRESULT WINAPI
Xnine_PresentBuffer(struct D3DPresent_Xnine *This,
                    struct D3DWindowBuffer *buffer,
                    HWND hWndOverride,
                    const RECT *pSourceRect,
                    const RECT *pDestRect,
                    const RGNDATA *pDirtyRegion,
                    DWORD Flags)
{
    RECT dest_translate;
    HWND dest_window_hwnd = hWndOverride ?
                              hWndOverride :
                              This->present_params.hDeviceWindow ?
                                This->present_params.hDeviceWindow :
                                This->focus_wnd;
    struct Xnine_window *dest_window = find_window(This->priv, dest_window_hwnd);

    assert(dest_window);

    if (dest_window->border_height_top || dest_window->border_width_left) {
        dest_translate.top = dest_window->border_height_top;
        dest_translate.left = dest_window->border_width_left;
        dest_translate.bottom = dest_window->border_height_top +
                                dest_window->height;
        dest_translate.right = dest_window->border_width_left +
                               dest_window->width;
        if (pDestRect) {
            dest_translate.top += pDestRect->top;
            dest_translate.left += pDestRect->left;
            dest_translate.bottom = dest_window->border_height_top +
                                    pDestRect->bottom;
            dest_translate.right = dest_window->border_width_left +
                                   pDestRect->right;
        }
        pDestRect = (const RECT *) &dest_translate;
    }

    if (!PRESENTPixmap(This->priv->dpy, dest_window->Xwindow,
                       buffer->present_pixmap_priv, &This->present_params,
                       pSourceRect, pDestRect, pDirtyRegion))
        return D3DERR_DRIVERINTERNALERROR;

    return D3D_OK;
}

static HRESULT WINAPI
Xnine_GetRasterStatus(struct D3DPresent_Xnine *This,
                      D3DRASTER_STATUS *pRasterStatus)
{
    return D3DERR_INVALIDCALL;
}

static HRESULT WINAPI
Xnine_GetDisplayMode(struct D3DPresent_Xnine *This,
                     D3DDISPLAYMODEEX *pMode,
                     D3DDISPLAYROTATION *pRotation)
{
    struct Xnine_private *priv = This->priv;

    pMode->Width = priv->screen_width;
    pMode->Height = priv->screen_height;
    pMode->RefreshRate = 60; // 60000 ?
    pMode->Format = D3DFMT_X8R8G8B8;
    pMode->ScanLineOrdering = D3DSCANLINEORDERING_PROGRESSIVE;
    *pRotation = D3DDISPLAYROTATION_IDENTITY;

    return D3D_OK;
}

static HRESULT WINAPI
Xnine_GetPresentStats(struct D3DPresent_Xnine *This,
                      D3DPRESENTSTATS *pStats)
{
    return D3DERR_INVALIDCALL;
}

static HRESULT WINAPI
Xnine_GetCursorPos(struct D3DPresent_Xnine *This,
                   POINT *pPoint)
{
    //TODO
    return D3DERR_DRIVERINTERNALERROR;
}

static HRESULT WINAPI
Xnine_SetCursorPos(struct D3DPresent_Xnine *This,
                   POINT *pPoint)
{
    //TODO
    return D3DERR_DRIVERINTERNALERROR;
}

static HRESULT WINAPI
Xnine_SetCursor(struct D3DPresent_Xnine *This,
                void *pBitmap,
                POINT *pHotspot,
                BOOL bShow)
{
   //TODO
    return D3DERR_DRIVERINTERNALERROR;
}

static HRESULT WINAPI
Xnine_SetGammaRamp(struct D3DPresent_Xnine *This,
                   const D3DGAMMARAMP *pRamp,
                   HWND hWndOverride)
{
    return D3DERR_DRIVERINTERNALERROR; 
}

static HRESULT WINAPI
Xnine_GetWindowInfo(struct D3DPresent_Xnine *This, HWND hWnd,
                    int *width, int *height, int *depth )
{
    struct Xnine_window *window;
    HRESULT hr;

    if (!hWnd)
        hWnd = This->focus_wnd;
    window = find_window(This->priv, hWnd);
    *width = window->width;
    *height = window->height;
    *depth = window->depth;
    return D3D_OK;
}

static BOOL WINAPI
Xnine_GetWindowOccluded(struct D3DPresent_Xnine *This)
{
    return FALSE;
}

static BOOL WINAPI
Xnine_ResolutionMismatch(struct D3DPresent_Xnine *This)
{
    return FALSE;
}

static HANDLE WINAPI
Xnine_CreateThread(struct D3DPresent_Xnine *This, void *pThreadfunc, void *pParam)
{
    return NULL;
}

static BOOL WINAPI
Xnine_WaitForThread(struct D3DPresent_Xnine *This, HANDLE thread)
{
    return TRUE;
}

static HRESULT WINAPI
Xnine_SetPresentParameters2(struct D3DPresent_Xnine *This, D3DPRESENT_PARAMETERS2 *pParameters)
{
    return D3D_OK;
}

static BOOL WINAPI
Xnine_IsBufferReleased(struct D3DPresent_Xnine *This, D3DWindowBuffer *buffer)
{
    return TRUE;
}

static HRESULT WINAPI
Xnine_WaitBufferReleaseEvent(struct D3DPresent_Xnine *This)
{
    return D3D_OK;
}

static HRESULT WINAPI
Xnine_GetWindowHWND(struct D3DPresent_Xnine *This, HWND hWnd, void **pDisplay, void **pWindow, int *type)
{
    struct Xnine_window *window;

    window = find_window(This->priv, hWnd);
    if (!window)
        return D3DERR_DRIVERINTERNALERROR;
    *pWindow = (uintptr_t*)window->Xwindow;
    *pDisplay = XGetXCBConnection(This->priv->dpy);
    return D3D_OK;
}

static ID3DPresentVtbl Xnine_vtable = {
    (void *)Xnine_QueryInterface,
    (void *)Xnine_AddRef,
    (void *)Xnine_Release,
    (void *)Xnine_SetPresentParameters,
    (void *)Xnine_D3DWindowBufferFromDmaBuf,
    (void *)Xnine_DestroyD3DWindowBuffer,
    (void *)Xnine_WaitBufferReleased,
    (void *)Xnine_FrontBufferCopy,
    (void *)Xnine_PresentBuffer,
    (void *)Xnine_GetRasterStatus,
    (void *)Xnine_GetDisplayMode,
    (void *)Xnine_GetPresentStats,
    (void *)Xnine_GetCursorPos,
    (void *)Xnine_SetCursorPos,
    (void *)Xnine_SetCursor,
    (void *)Xnine_SetGammaRamp,
    (void *)Xnine_GetWindowInfo,
    (void *)Xnine_GetWindowOccluded,
    (void *)Xnine_ResolutionMismatch,
    (void *)Xnine_CreateThread,
    (void *)Xnine_WaitForThread,
    (void *)Xnine_SetPresentParameters2,
    (void *)Xnine_IsBufferReleased,
    (void *)Xnine_WaitBufferReleaseEvent,
    (void *)Xnine_GetWindowHWND,
};


struct D3DPresent_Xnine *D3DPresent_Xnine_create(struct Xnine_private *priv, HWND focus_wnd)
{
    struct D3DPresent_Xnine *This = calloc(1, sizeof(struct D3DPresent_Xnine));

    if (!This)
        return NULL;

    This->vtable = &Xnine_vtable;
    This->refs = 1;
    This->focus_wnd = focus_wnd;
    This->priv = priv;
    PRESENTInit(priv->dpy, &(This->present_priv));
#if D3DADAPTER9_WITHDRI2
    if (priv->is_dri2_fallback)
        DRI2FallbackInit(priv->dpy, &(This->dri2_priv));
#endif
    return This;
}
///// ID3DPresentGroup

struct D3DPresentGroup_Xnine
{
    void *vtable;

    int32_t refs;

    struct D3DPresent_Xnine *present_backend; /* only one display */
};

static ULONG WINAPI
XnineGroup_AddRef(struct D3DPresentGroup_Xnine *This);

static HRESULT WINAPI
XnineGroup_QueryInterface(struct D3DPresentGroup_Xnine *This,
                          REFIID riid, void **ppvObject)
{
    if (!ppvObject) { return E_POINTER; }
    if (IsEqualGUID(&IID_ID3DPresentGroup, riid) ||
        IsEqualGUID(&IID_IUnknown, riid)) {
        *ppvObject = This;
        XnineGroup_AddRef(This);
        return S_OK;
    }

    *ppvObject = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI
XnineGroup_AddRef(struct D3DPresentGroup_Xnine *This)
{
    uint32_t refs = __sync_add_and_fetch(&This->refs, 1);
    return refs;
}

static ULONG WINAPI
XnineGroup_Release(struct D3DPresentGroup_Xnine *This)
{
    uint32_t refs =  __sync_sub_and_fetch(&This->refs, 1);
    if (refs == 0) {
        Xnine_Release(This->present_backend);
        free(This);
    }
    return refs;
}

static UINT WINAPI
XnineGroup_GetMultiheadCount(struct D3DPresentGroup_Xnine *This)
{
    return 1;
}

static HRESULT WINAPI
XnineGroup_GetPresent(struct D3DPresentGroup_Xnine *This,
                      UINT Index, ID3DPresent **ppPresent)
{
    if (Index >= 1)
        return D3DERR_INVALIDCALL;

    Xnine_AddRef(This->present_backend);
    *ppPresent = (ID3DPresent *)This->present_backend;

    return D3D_OK;
}

static HRESULT WINAPI
XnineGroup_CreateAdditionalPresent(struct D3DPresentGroup_Xnine *This,
                                   D3DPRESENT_PARAMETERS *pPresentationParameters,
                                   ID3DPresent **ppPresent)
{
    struct D3DPresent_Xnine *res = D3DPresent_Xnine_create(This->present_backend->priv, 0);
    (void) pPresentationParameters;
    if (!res)
        return D3DERR_DRIVERINTERNALERROR;
    *ppPresent = (ID3DPresent *)res;
    return D3D_OK;
}

static void WINAPI
XnineGroup_GetVersion(struct D3DPresentGroup_Xnine *This,
                      int *major, int *minor)
{
    *major = 1;
    *minor = 5;
}

static ID3DPresentGroupVtbl XnineGroup_vtable = {
    (void *)XnineGroup_QueryInterface,
    (void *)XnineGroup_AddRef,
    (void *)XnineGroup_Release,
    (void *)XnineGroup_GetMultiheadCount,
    (void *)XnineGroup_GetPresent,
    (void *)XnineGroup_CreateAdditionalPresent,
    (void *)XnineGroup_GetVersion
};

struct D3DPresentGroup_Xnine *D3DPresentGroup_Xnine_create(struct Xnine_private *priv, HWND focus_wnd)
{
    struct D3DPresentGroup_Xnine *This = calloc(1, sizeof(struct D3DPresentGroup_Xnine));

    if (!This)
        return NULL;

    This->vtable = &XnineGroup_vtable;
    This->refs = 1;
    This->present_backend = D3DPresent_Xnine_create(priv, focus_wnd);
    if (!This->present_backend) {
        free(This);
        return NULL;
    }
    return This;
}

// IDirect3D9Ex

struct Direct3D9Ex_Xnine {
    void *vtable;
    uint32_t refs;
    struct Xnine_private *priv;
    ID3DAdapter9 *adapter9;
    BOOL is_ex;
};

static ULONG WINAPI
XnineD3D_AddRef(struct Direct3D9Ex_Xnine *This);

static HRESULT WINAPI
XnineD3D_QueryInterface(struct Direct3D9Ex_Xnine *This,
                        REFIID riid, void **ppvObject)
{
    if (!ppvObject) { return E_POINTER; }
    if ((IsEqualGUID(&IID_IDirect3D9Ex, riid) && This->is_ex) ||
         IsEqualGUID(&IID_IDirect3D9, riid) ||
         IsEqualGUID(&IID_IUnknown, riid)) {
        *ppvObject = This;
        XnineD3D_AddRef(This);
        return S_OK;
    }

    *ppvObject = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI
XnineD3D_AddRef(struct Direct3D9Ex_Xnine *This)
{
    uint32_t refs = __sync_add_and_fetch(&This->refs, 1);
    return refs;
}

static ULONG WINAPI
XnineD3D_Release(struct Direct3D9Ex_Xnine *This)
{
    uint32_t refs =  __sync_sub_and_fetch(&This->refs, 1);
    if (refs == 0) {
        ID3DAdapter9_Release(This->adapter9);
        free(This);
    }
    return refs;
}

static HRESULT WINAPI
XnineD3D_RegisterSoftwareDevice(struct Direct3D9Ex_Xnine *This,
                                void *pInitializeFunction)
{
    return D3DERR_INVALIDCALL;
}

static UINT WINAPI
XnineD3D_GetAdapterCount(struct Direct3D9Ex_Xnine *This)
{
    return 1;
}

static HRESULT WINAPI
XnineD3D_GetAdapterIdentifier(struct Direct3D9Ex_Xnine *This,
                              UINT Adapter, DWORD Flags,
                              D3DADAPTER_IDENTIFIER9 *pIdentifier)
{
    if (Adapter > 0)
        return D3DERR_INVALIDCALL;

    return ID3DAdapter9_GetAdapterIdentifier(This->adapter9, Flags, pIdentifier);
}

static UINT WINAPI
XnineD3D_GetAdapterModeCount(struct Direct3D9Ex_Xnine *This,
                             UINT Adapter, D3DFORMAT Format)
{
    if (Adapter > 0)
        return 0;

    if (FAILED(ID3DAdapter9_CheckDeviceFormat(This->adapter9, D3DDEVTYPE_HAL,
                                              Format,
                                              D3DUSAGE_RENDERTARGET,
                                              D3DRTYPE_SURFACE, Format))) {
        return 0;
    }

    return 1;
}

static HRESULT WINAPI
XnineD3D_EnumAdapterModes(struct Direct3D9Ex_Xnine *This,
                          UINT Adapter, D3DFORMAT Format,
                          UINT Mode, D3DDISPLAYMODE *pMode)
{
    HRESULT hr;

    if (Adapter > 0 || Mode > 0)
        return D3DERR_INVALIDCALL;

    hr = ID3DAdapter9_CheckDeviceFormat(This->adapter9, D3DDEVTYPE_HAL,
                                       Format, D3DUSAGE_RENDERTARGET,
                                       D3DRTYPE_SURFACE, Format);
    if (FAILED(hr)) {
        return hr;
    }

    pMode->Width = This->priv->screen_width;
    pMode->Height = This->priv->screen_height;
    pMode->RefreshRate = 60; // 60000 ?
    pMode->Format = Format;

    return D3D_OK;
}

static HRESULT WINAPI
XnineD3D_GetAdapterDisplayMode(struct Direct3D9Ex_Xnine *This,
                                  UINT Adapter,
                                  D3DDISPLAYMODE *pMode)
{

    if (Adapter > 0)
        return D3DERR_INVALIDCALL;

    pMode->Width = This->priv->screen_width;
    pMode->Height = This->priv->screen_height;
    pMode->RefreshRate = 60; // 60000 ?
    pMode->Format = D3DFMT_X8R8G8B8;

    return D3D_OK;
}

static HRESULT WINAPI
XnineD3D_CheckDeviceType(struct Direct3D9Ex_Xnine *This,
                         UINT Adapter, D3DDEVTYPE DevType,
                         D3DFORMAT AdapterFormat,
                         D3DFORMAT BackBufferFormat,
                         BOOL bWindowed)
{
    if (Adapter > 0)
        return D3DERR_INVALIDCALL;

    return ID3DAdapter9_CheckDeviceType(This->adapter9, DevType,
                                        AdapterFormat, BackBufferFormat,
                                        bWindowed);
}

static HRESULT WINAPI
XnineD3D_CheckDeviceFormat(struct Direct3D9Ex_Xnine *This,
                           UINT Adapter,
                           D3DDEVTYPE DeviceType,
                           D3DFORMAT AdapterFormat,
                           DWORD Usage,
                           D3DRESOURCETYPE RType,
                           D3DFORMAT CheckFormat)
{
    if (Adapter > 0)
        return D3DERR_INVALIDCALL;

    return ID3DAdapter9_CheckDeviceFormat(This->adapter9, DeviceType,
                                          AdapterFormat, Usage, RType,
                                          CheckFormat);
}

static HRESULT WINAPI
XnineD3D_CheckDeviceMultiSampleType(struct Direct3D9Ex_Xnine *This,
                                    UINT Adapter,
                                    D3DDEVTYPE DeviceType,
                                    D3DFORMAT SurfaceFormat,
                                    BOOL Windowed,
                                    D3DMULTISAMPLE_TYPE MultiSampleType,
                                    DWORD *pQualityLevels)
{
    if (Adapter > 0)
        return D3DERR_INVALIDCALL;
    
    return ID3DAdapter9_CheckDeviceMultiSampleType(This->adapter9,
                                                   DeviceType,
                                                   SurfaceFormat,
                                                   Windowed,
                                                   MultiSampleType,
                                                   pQualityLevels);
}

static HRESULT WINAPI
XnineD3D_CheckDepthStencilMatch(struct Direct3D9Ex_Xnine *This,
                                UINT Adapter,
                                D3DDEVTYPE DeviceType,
                                D3DFORMAT AdapterFormat,
                                D3DFORMAT RenderTargetFormat,
                                D3DFORMAT DepthStencilFormat)
{
    if (Adapter > 0)
        return D3DERR_INVALIDCALL;

    return ID3DAdapter9_CheckDepthStencilMatch(This->adapter9, DeviceType,
                                               AdapterFormat,
                                               RenderTargetFormat,
                                               DepthStencilFormat);
}

static HRESULT WINAPI
XnineD3D_CheckDeviceFormatConversion(struct Direct3D9Ex_Xnine *This,
                                     UINT Adapter,
                                     D3DDEVTYPE DeviceType,
                                     D3DFORMAT SourceFormat,
                                     D3DFORMAT TargetFormat)
{
    if (Adapter > 0)
        return D3DERR_INVALIDCALL;

    return ID3DAdapter9_CheckDeviceFormatConversion(This->adapter9,
                                                    DeviceType, SourceFormat,
                                                    TargetFormat);
}

static HRESULT WINAPI
XnineD3D_GetDeviceCaps(struct Direct3D9Ex_Xnine *This,
                       UINT Adapter, D3DDEVTYPE DeviceType,
                       D3DCAPS9 *pCaps)
{
    HRESULT hr;

    if (Adapter > 0)
        return D3DERR_INVALIDCALL;

    hr = ID3DAdapter9_GetDeviceCaps(This->adapter9, DeviceType, pCaps);
    if (FAILED(hr))
        return hr;

    pCaps->MasterAdapterOrdinal = 0;
    pCaps->AdapterOrdinalInGroup = 0;
    pCaps->NumberOfAdaptersInGroup = 1;

    return hr;
}

static HMONITOR WINAPI
XnineD3D_GetAdapterMonitor(struct Direct3D9Ex_Xnine *This, UINT Adapter)
{
    return 0;
}

static UINT WINAPI
XnineD3D_GetAdapterModeCountEx(struct Direct3D9Ex_Xnine *This,
                               UINT Adapter, const D3DDISPLAYMODEFILTER *pFilter)
{
    return 1;
}

static HRESULT WINAPI
XnineD3D_EnumAdapterModesEx(struct Direct3D9Ex_Xnine *This,
                            UINT Adapter,
                            const D3DDISPLAYMODEFILTER *pFilter,
                            UINT Mode, D3DDISPLAYMODEEX *pMode )
{
    return D3DERR_INVALIDCALL;
}

static HRESULT WINAPI
XnineD3D_GetAdapterDisplayModeEx(struct Direct3D9Ex_Xnine *This,
                                 UINT Adapter,
                                 D3DDISPLAYMODEEX *pMode,
                                 D3DDISPLAYROTATION *pRotation)
{
    return D3DERR_INVALIDCALL;
}

static HRESULT WINAPI
XnineD3D_GetAdapterLUID(struct Direct3D9Ex_Xnine *This,
                        UINT Adapter, LUID *pLUID)
{
    return D3DERR_INVALIDCALL;
}

static HRESULT WINAPI
XnineD3D_CreateDeviceEx(struct Direct3D9Ex_Xnine *This,
                        UINT Adapter,
                        D3DDEVTYPE DeviceType,
                        HWND hFocusWindow,
                        DWORD BehaviorFlags,
                        D3DPRESENT_PARAMETERS *pPresentationParameters,
                        D3DDISPLAYMODEEX *pFullscreenDisplayMode,
                        IDirect3DDevice9Ex **ppReturnedDeviceInterface)
{
    ID3DPresentGroup *present;
    HRESULT hr;

    if (Adapter > 0)
        return D3DERR_INVALIDCALL;

    present = (ID3DPresentGroup *)D3DPresentGroup_Xnine_create(This->priv,
                                                               hFocusWindow);

    if (This->is_ex) {
        hr = ID3DAdapter9_CreateDeviceEx(This->adapter9, Adapter, DeviceType,
                                         hFocusWindow, BehaviorFlags,
                                         pPresentationParameters,
                                         pFullscreenDisplayMode,
                                         (IDirect3D9Ex *)This, present,
                                         ppReturnedDeviceInterface);
    } else {
        hr = ID3DAdapter9_CreateDevice(This->adapter9, Adapter, DeviceType,
                                       hFocusWindow, BehaviorFlags,
                                       pPresentationParameters,
                                       (IDirect3D9 *)This, present,
                                       (IDirect3DDevice9 **)
                                        ppReturnedDeviceInterface);
    }
    if (FAILED(hr))
        ID3DPresentGroup_Release(present);

    return hr;
}

static HRESULT WINAPI
XnineD3D_CreateDevice(struct Direct3D9Ex_Xnine *This,
                      UINT Adapter, D3DDEVTYPE DeviceType,
                      HWND hFocusWindow, DWORD BehaviorFlags,
                      D3DPRESENT_PARAMETERS *pPresentationParameters,
                      IDirect3DDevice9 **ppReturnedDeviceInterface )
{
    HRESULT hr;
    hr = XnineD3D_CreateDeviceEx(This, Adapter, DeviceType, hFocusWindow,
                                    BehaviorFlags, pPresentationParameters,
                                    NULL,
                                    (IDirect3DDevice9Ex **)
                                     ppReturnedDeviceInterface);
    if (FAILED(hr))
        return hr;
    return D3D_OK;
}

static IDirect3D9ExVtbl XnineD3d9_vtable = {
    (void *)XnineD3D_QueryInterface,
    (void *)XnineD3D_AddRef,
    (void *)XnineD3D_Release,
    (void *)XnineD3D_RegisterSoftwareDevice,
    (void *)XnineD3D_GetAdapterCount,
    (void *)XnineD3D_GetAdapterIdentifier,
    (void *)XnineD3D_GetAdapterModeCount,
    (void *)XnineD3D_EnumAdapterModes,
    (void *)XnineD3D_GetAdapterDisplayMode,
    (void *)XnineD3D_CheckDeviceType,
    (void *)XnineD3D_CheckDeviceFormat,
    (void *)XnineD3D_CheckDeviceMultiSampleType,
    (void *)XnineD3D_CheckDepthStencilMatch,
    (void *)XnineD3D_CheckDeviceFormatConversion,
    (void *)XnineD3D_GetDeviceCaps,
    (void *)XnineD3D_GetAdapterMonitor,
    (void *)XnineD3D_CreateDevice,
    (void *)XnineD3D_GetAdapterModeCountEx,
    (void *)XnineD3D_EnumAdapterModesEx,
    (void *)XnineD3D_GetAdapterDisplayModeEx,
    (void *)XnineD3D_CreateDeviceEx,
    (void *)XnineD3D_GetAdapterLUID
};

ID3DAdapter9 *create_adapter9(struct Xnine_private *priv)
{
    int fd = -1;
    ID3DAdapter9 *out;
    BOOL dri3_fail = FALSE;
    BOOL dri2_fail = FALSE;

#if D3DADAPTER9_WITHDRI2
    if (!priv->is_dri2_fallback && !DRI3Open(priv->dpy, DefaultScreen(priv->dpy), &fd)) {
#else
    if (!DRI3Open(priv->dpy, DefaultScreen(priv->dpy), &fd)) {
#endif
        dri3_fail = TRUE;
    }
#if D3DADAPTER9_WITHDRI2
    if (priv->is_dri2_fallback && !DRI2FallbackOpen(priv->dpy, DefaultScreen(priv->dpy), &fd)) {
        dri2_fail = TRUE;
    }
#endif

    if (priv->d3d9_drm->create_adapter(fd, &out) != D3D_OK) {
        if (dri3_fail)
            fprintf(stderr, "Failed to use DRI3\n");
        else if (dri2_fail)
            fprintf(stderr, "Failed to use DRI2\n");
        fprintf(stderr, "Failed to load the driver\n");
        return NULL;
    }

    return out;
}

struct IDirect3D9Ex *Xnine_get_D3D9Ex(struct Xnine_private *priv)
{
    struct Direct3D9Ex_Xnine *res = calloc(1, sizeof(struct Direct3D9Ex_Xnine));
    ID3DAdapter9 *adapter9 = create_adapter9(priv);

    if (!res || !adapter9)
        return NULL;

    res->vtable = &XnineD3d9_vtable;
    res->refs = 1;
    res->priv = priv;
    res->adapter9 = adapter9;
    res->is_ex = TRUE;
    return (struct IDirect3D9Ex*)res;
}

struct IDirect3D9 *Xnine_get_D3D9(struct Xnine_private *priv)
{
    struct Direct3D9Ex_Xnine *res = calloc(1, sizeof(struct Direct3D9Ex_Xnine));
    ID3DAdapter9 *adapter9 = create_adapter9(priv);

    if (!res)
        return NULL;

    res->vtable = &XnineD3d9_vtable;
    res->refs = 1;
    res->priv = priv;
    res->adapter9 = adapter9;
    res->is_ex = FALSE;
    return (struct IDirect3D9*)res;
}

BOOL Xnine_init(int screen_num, struct Xnine_private **priv)
{
    static void * WINAPI (*pD3DAdapter9GetProc)(const char *);
    struct Xnine_private *res;
    struct D3DAdapter9DRM *d3d9_drm;
    BOOL is_dri2_fallback = 0;
    Display *dpy = XOpenDisplay(NULL);
    int scrn_num = screen_num == -1 ? DefaultScreen(dpy) : screen_num;
    static void *handle = NULL;

    if (!PRESENTCheckExtension(dpy, 1, 0)) {
        fprintf(stderr, "Requirements not met: no PRESENT\n");
        return FALSE;
    }

    if (!DRI3CheckExtension(dpy, 1, 0)) {
#if !D3DADAPTER9_WITHDRI2
        fprintf(stderr, "Requirements not met: no DRI3\n");
#else
        fprintf(stderr, "Unable to query DRI3. Trying DRI2 fallback (slower performance).\n");
        is_dri2_fallback = 1;
        if (!DRI2FallbackCheckSupport(dpy)) {
            fprintf(stderr, "Requirements not met: no DRI3 nor DRI2 fallback\n");
            is_dri2_fallback = 0;
        }
#endif
    }

    handle = dlopen(D3DADAPTERPATH, RTLD_LOCAL | RTLD_NOW);
    if (!handle) {
        fprintf(stderr, "Failed to load d3d9 lib: %s\n", dlerror());
        return FALSE;
    }

    pD3DAdapter9GetProc = dlsym(handle, "D3DAdapter9GetProc");
    if (!pD3DAdapter9GetProc) {
        fprintf(stderr, "Failed to load d3d9 lib symbols\n");
        return FALSE;
    }

    d3d9_drm = pD3DAdapter9GetProc(D3DADAPTER9DRM_NAME);
    if (!d3d9_drm) {
        fprintf(stderr, "Failed to load d3d9 drm backend\n");
        return FALSE;
    }

    /*if (d3d9_drm->major_version != 1) {
        fprintf(stderr, "D3d9 version mismatch.\n");
        return FALSE;
    }*/

    res = calloc(1, sizeof(struct Xnine_private));
    if (!res)
        return FALSE;

    res->scrnum = scrn_num;
    res->dpy = dpy;
    res->screen_width = DisplayWidth(dpy, scrn_num);
    res->screen_height = DisplayHeight(dpy, scrn_num);
    res->d3d9_drm = d3d9_drm;
#if D3DADAPTER9_WITHDRI2
    res->is_dri2_fallback = is_dri2_fallback;
#endif
    *priv = res;
    return TRUE;
}

void Xnine_close(struct Xnine_private *priv)
{
    while (priv->window_list) {
        Xnine_destroy_window(priv, (priv->window_list)->win);
    }
    free(priv);
}
