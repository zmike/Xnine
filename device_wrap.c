/* SPDX-License-Identifier: LGPL-2.1-or-later */
/*
 * Copyright 2016 Axel Davy
 */
#include <stdio.h>
#include <stdlib.h>
#include <d3d9.h>

#include <d3dadapter/d3dadapter9.h>

#define ALT_WINAPI
#define ALT_DECLSPEC_HOTPATCH 
typedef struct common_vtable {
	/* IUnknown */
	HRESULT (WINAPI *QueryInterface)(void *This, REFIID riid, void **ppvObject);
	ULONG (WINAPI *AddRef)(void *This);
	ULONG (WINAPI *Release)(void *This);
	/* IDirect3DResource9 */
	HRESULT (WINAPI *GetDevice)(void *This, IDirect3DDevice9 **ppDevice);
	HRESULT (WINAPI *SetPrivateData)(IDirect3DIndexBuffer9 *This, REFGUID refguid, const void *pData, DWORD SizeOfData, DWORD Flags);
	HRESULT (WINAPI *GetPrivateData)(IDirect3DIndexBuffer9 *This, REFGUID refguid, void *pData, DWORD *pSizeOfData);
	HRESULT (WINAPI *FreePrivateData)(IDirect3DIndexBuffer9 *This, REFGUID refguid);
	DWORD (WINAPI *SetPriority)(IDirect3DIndexBuffer9 *This, DWORD PriorityNew);
	DWORD (WINAPI *GetPriority)(IDirect3DIndexBuffer9 *This);
	void (WINAPI *PreLoad)(IDirect3DIndexBuffer9 *This);
	D3DRESOURCETYPE (WINAPI *GetType)(IDirect3DIndexBuffer9 *This);
} common_vtable;

typedef struct common_wrap {
   common_vtable *lpVtbl;
   common_vtable *lpVtbl_internal;
} common_wrap;

#define COMMON_WRAP0(ret, func)  \
    ret ALT_WINAPI Nine_ ## func(void *This) \
    { \
        return ((common_wrap *)This)->lpVtbl_internal->func(This); \
    }

#define COMMON_WRAP1(ret, func, type1)  \
    ret ALT_WINAPI Nine_ ## func(void *This, type1 arg1) \
    { \
        return ((common_wrap *)This)->lpVtbl_internal->func(This, arg1); \
    }

#define COMMON_WRAP2(ret, func, type1, type2)  \
    ret ALT_WINAPI Nine_ ## func(void *This, type1 arg1, type2 arg2) \
    { \
        return ((common_wrap *)This)->lpVtbl_internal->func(This, arg1, arg2); \
    }

#define COMMON_WRAP3(ret, func, type1, type2, type3)  \
    ret ALT_WINAPI Nine_ ## func(void *This, type1 arg1, type2 arg2, type3 arg3) \
    { \
        return ((common_wrap *)This)->lpVtbl_internal->func(This, arg1, arg2, arg3); \
    }

#define COMMON_WRAP4(ret, func, type1, type2, type3, type4)  \
    ret ALT_WINAPI Nine_ ## func(void *This, type1 arg1, type2 arg2, type3 arg3, type4 arg4) \
    { \
        return ((common_wrap *)This)->lpVtbl_internal->func(This, arg1, arg2, arg3, arg4); \
    }

COMMON_WRAP2(HRESULT, QueryInterface, REFIID, void **)
COMMON_WRAP0(ULONG, AddRef)
COMMON_WRAP0(ULONG, Release)
COMMON_WRAP1(HRESULT, GetDevice, IDirect3DDevice9 **)
COMMON_WRAP4(HRESULT, SetPrivateData, REFGUID, const void *, DWORD, DWORD)
COMMON_WRAP3(HRESULT, GetPrivateData, REFGUID, void *, DWORD*)
COMMON_WRAP1(HRESULT, FreePrivateData, REFGUID)
COMMON_WRAP1(DWORD, SetPriority, DWORD)
COMMON_WRAP0(DWORD, GetPriority)
COMMON_WRAP0(void, PreLoad)
COMMON_WRAP0(D3DRESOURCETYPE, GetType)

struct IDirect3DSurface9_Minor1
{
        IDirect3DSurface9Vtbl *lpVtbl;
        IDirect3DSurface9Vtbl *lpVtbl_internal;
};
typedef struct IDirect3DSurface9_Minor1 IDirect3DSurface9_Minor1;

#define SURFACE_WRAP0(ret, func)  \
    ret ALT_WINAPI NineSurface9_ ## func(void *This) \
    { \
        return ((IDirect3DSurface9_Minor1 *)This)->lpVtbl_internal->func(This); \
    }

#define SURFACE_WRAP1(ret, func, type1)  \
    ret ALT_WINAPI NineSurface9_ ## func(void *This, type1 arg1) \
    { \
        return ((IDirect3DSurface9_Minor1 *)This)->lpVtbl_internal->func(This, arg1); \
    }

#define SURFACE_WRAP2(ret, func, type1, type2)  \
    ret ALT_WINAPI NineSurface9_ ## func(void *This, type1 arg1, type2 arg2) \
    { \
        return ((IDirect3DSurface9_Minor1 *)This)->lpVtbl_internal->func(This, arg1, arg2); \
    }

#define SURFACE_WRAP3(ret, func, type1, type2, type3)  \
    ret ALT_WINAPI NineSurface9_ ## func(void *This, type1 arg1, type2 arg2, type3 arg3) \
    { \
        return ((IDirect3DSurface9_Minor1 *)This)->lpVtbl_internal->func(This, arg1, arg2, arg3); \
    }

SURFACE_WRAP2(HRESULT, GetContainer, REFIID, void**) /* Parent interfaces are already wrapped */
SURFACE_WRAP1(HRESULT, GetDesc, D3DSURFACE_DESC*)
SURFACE_WRAP3(HRESULT, LockRect, D3DLOCKED_RECT *, const RECT *, DWORD)
SURFACE_WRAP0(HRESULT, UnlockRect)
SURFACE_WRAP1(HRESULT, GetDC, HDC*)
SURFACE_WRAP1(HRESULT, ReleaseDC, HDC)

static IDirect3DSurface9Vtbl NineSurface9_vtable = {
    (void *)Nine_QueryInterface,
    (void *)Nine_AddRef,
    (void *)Nine_Release,
    (void *)Nine_GetDevice, /* actually part of  iface */
    (void *)Nine_SetPrivateData,
    (void *)Nine_GetPrivateData,
    (void *)Nine_FreePrivateData,
    (void *)Nine_SetPriority,
    (void *)Nine_GetPriority,
    (void *)Nine_PreLoad, /* nop */
    (void *)Nine_GetType, /* immutable */
    (void *)NineSurface9_GetContainer,
    (void *)NineSurface9_GetDesc, /* immutable */
    (void *)NineSurface9_LockRect,
    (void *)NineSurface9_UnlockRect,
    (void *)NineSurface9_GetDC,
    (void *)NineSurface9_ReleaseDC
};

struct IDirect3DVolume9_Minor1
{
        IDirect3DVolume9Vtbl *lpVtbl;
        IDirect3DVolume9Vtbl *lpVtbl_internal;
};
typedef struct IDirect3DVolume9_Minor1 IDirect3DVolume9_Minor1;

#define VOLUME_WRAP0(ret, func)  \
    ret ALT_WINAPI NineVolume9_ ## func(void *This) \
    { \
        return ((IDirect3DVolume9_Minor1 *)This)->lpVtbl_internal->func(This); \
    }

#define VOLUME_WRAP1(ret, func, type1)  \
    ret ALT_WINAPI NineVolume9_ ## func(void *This, type1 arg1) \
    { \
        return ((IDirect3DVolume9_Minor1 *)This)->lpVtbl_internal->func(This, arg1); \
    }

#define VOLUME_WRAP2(ret, func, type1, type2)  \
    ret ALT_WINAPI NineVolume9_ ## func(void *This, type1 arg1, type2 arg2) \
    { \
        return ((IDirect3DVolume9_Minor1 *)This)->lpVtbl_internal->func(This, arg1, arg2); \
    }

#define VOLUME_WRAP3(ret, func, type1, type2, type3)  \
    ret ALT_WINAPI NineVolume9_ ## func(void *This, type1 arg1, type2 arg2, type3 arg3) \
    { \
        return ((IDirect3DVolume9_Minor1 *)This)->lpVtbl_internal->func(This, arg1, arg2, arg3); \
    }

VOLUME_WRAP2(HRESULT, GetContainer, REFIID, void**) /* Parent interfaces are already wrapped */
VOLUME_WRAP1(HRESULT, GetDesc, D3DVOLUME_DESC*)
VOLUME_WRAP3(HRESULT, LockBox, D3DLOCKED_BOX *, const D3DBOX *, DWORD)
VOLUME_WRAP0(HRESULT, UnlockBox)

static IDirect3DVolume9Vtbl NineVolume9_vtable = {
    (void *)Nine_QueryInterface,
    (void *)Nine_AddRef,
    (void *)Nine_Release,
    (void *)Nine_GetDevice, /* actually part of  iface */
    (void *)Nine_SetPrivateData,
    (void *)Nine_GetPrivateData,
    (void *)Nine_FreePrivateData,
    (void *)NineVolume9_GetContainer,
    (void *)NineVolume9_GetDesc, /* immutable */
    (void *)NineVolume9_LockBox,
    (void *)NineVolume9_UnlockBox
};

struct IDirect3DStateBlock9_Minor1
{
        IDirect3DStateBlock9Vtbl *lpVtbl;
        IDirect3DStateBlock9Vtbl *lpVtbl_internal;
};

typedef struct IDirect3DStateBlock9_Minor1 IDirect3DStateBlock9_Minor1;

#define STATEBLOCK_WRAP0(ret, func)  \
    ret ALT_WINAPI NineStateBlock9_ ## func(void *This) \
    { \
        return ((IDirect3DStateBlock9_Minor1 *)This)->lpVtbl_internal->func(This); \
    }

STATEBLOCK_WRAP0(HRESULT, Capture)
STATEBLOCK_WRAP0(HRESULT, Apply)

static IDirect3DStateBlock9Vtbl NineStateBlock9_vtable = {
    (void *)Nine_QueryInterface,
    (void *)Nine_AddRef,
    (void *)Nine_Release,
    (void *)Nine_GetDevice, /* actually part of StateBlock9 iface */
    (void *)NineStateBlock9_Capture,
    (void *)NineStateBlock9_Apply
};


struct IDirect3DDevice9Ex_Minor1
{
        IDirect3DDevice9ExVtbl *lpVtbl;
        IDirect3DDevice9ExVtbl *lpVtbl_internal;
};

struct IDirect3DSwapChain9Ex_Minor1
{
        IDirect3DSwapChain9ExVtbl *lpVtbl;
        IDirect3DSwapChain9ExVtbl *lpVtbl_internal;
};


typedef struct IDirect3DDevice9Ex_Minor1 IDirect3DDevice9Ex_Minor1;
typedef struct IDirect3DSwapChain9Ex_Minor1 IDirect3DSwapChain9Ex_Minor1;
typedef struct IDirect3DVertexDeclaration9_Minor1 {
   IDirect3DVertexDeclaration9Vtbl *lpVtbl;
   IDirect3DVertexDeclaration9Vtbl *lpVtbl_internal;
} IDirect3DVertexDeclaration9_Minor1;

#define SWAPCHAIN_WRAP0(ret, func)  \
    ret ALT_WINAPI NineSwapChain9_ ## func(IDirect3DSwapChain9Ex *This) \
    { \
        return ((IDirect3DSwapChain9Ex_Minor1 *)This)->lpVtbl_internal->func(This); \
    }

#define SWAPCHAIN_WRAP1(ret, func, type1)  \
    ret ALT_WINAPI NineSwapChain9_ ## func(IDirect3DSwapChain9Ex *This, type1 arg1) \
    { \
        return ((IDirect3DSwapChain9Ex_Minor1 *)This)->lpVtbl_internal->func(This, arg1); \
    }

#define SWAPCHAIN_WRAP2(ret, func, type1, type2)  \
    ret ALT_WINAPI NineSwapChain9_ ## func(IDirect3DSwapChain9Ex *This, type1 arg1, type2 arg2) \
    { \
        return ((IDirect3DSwapChain9Ex_Minor1 *)This)->lpVtbl_internal->func(This, arg1, arg2); \
    }

#define SWAPCHAIN_WRAP3(ret, func, type1, type2, type3)  \
    ret ALT_WINAPI NineSwapChain9_ ## func(IDirect3DSwapChain9Ex *This, type1 arg1, type2 arg2, type3 arg3) \
    { \
        return ((IDirect3DSwapChain9Ex_Minor1 *)This)->lpVtbl_internal->func(This, arg1, arg2, arg3); \
    }

#define SWAPCHAIN_H_WRAP5(ret, func, type1, type2, type3, type4, type5)  \
    ret ALT_WINAPI ALT_DECLSPEC_HOTPATCH NineSwapChain9_ ## func(IDirect3DSwapChain9Ex *This, type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5) \
    { \
        return ((IDirect3DSwapChain9Ex_Minor1 *)This)->lpVtbl_internal->func(This, arg1, arg2, arg3, arg4, arg5); \
    }

#define DEVICE_WRAP0(ret, func)  \
    ret ALT_WINAPI NineDevice9_ ## func(IDirect3DDevice9Ex *This) \
    { \
        return ((IDirect3DDevice9Ex_Minor1 *)This)->lpVtbl_internal->func(This); \
    }

#define DEVICE_WRAP1(ret, func, type1)  \
    ret ALT_WINAPI NineDevice9_ ## func(IDirect3DDevice9Ex *This, type1 arg1) \
    { \
        return ((IDirect3DDevice9Ex_Minor1 *)This)->lpVtbl_internal->func(This, arg1); \
    }

#define DEVICE_WRAP2(ret, func, type1, type2)  \
    ret ALT_WINAPI NineDevice9_ ## func(IDirect3DDevice9Ex *This, type1 arg1, type2 arg2) \
    { \
        return ((IDirect3DDevice9Ex_Minor1 *)This)->lpVtbl_internal->func(This, arg1, arg2); \
    }

#define DEVICE_WRAP3(ret, func, type1, type2, type3)  \
    ret ALT_WINAPI NineDevice9_ ## func(IDirect3DDevice9Ex *This, type1 arg1, type2 arg2, type3 arg3) \
    { \
        return ((IDirect3DDevice9Ex_Minor1 *)This)->lpVtbl_internal->func(This, arg1, arg2, arg3); \
    }

#define DEVICE_WRAP4(ret, func, type1, type2, type3, type4)  \
    ret ALT_WINAPI NineDevice9_ ## func(IDirect3DDevice9Ex *This, type1 arg1, type2 arg2, type3 arg3, type4 arg4) \
    { \
        return ((IDirect3DDevice9Ex_Minor1 *)This)->lpVtbl_internal->func(This, arg1, arg2, arg3, arg4); \
    }

#define DEVICE_WRAP5(ret, func, type1, type2, type3, type4, type5)  \
    ret ALT_WINAPI NineDevice9_ ## func(IDirect3DDevice9Ex *This, type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5) \
    { \
        return ((IDirect3DDevice9Ex_Minor1 *)This)->lpVtbl_internal->func(This, arg1, arg2, arg3, arg4, arg5); \
    }

#define DEVICE_WRAP6(ret, func, type1, type2, type3, type4, type5, type6)  \
    ret ALT_WINAPI NineDevice9_ ## func(IDirect3DDevice9Ex *This, type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5, type6 arg6) \
    { \
        return ((IDirect3DDevice9Ex_Minor1 *)This)->lpVtbl_internal->func(This, arg1, arg2, arg3, arg4, arg5, arg6); \
    }

#define DEVICE_WRAP7(ret, func, type1, type2, type3, type4, type5, type6, type7)  \
    ret ALT_WINAPI NineDevice9_ ## func(IDirect3DDevice9Ex *This, type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5, type6 arg6, type7 arg7) \
    { \
        return ((IDirect3DDevice9Ex_Minor1 *)This)->lpVtbl_internal->func(This, arg1, arg2, arg3, arg4, arg5, arg6, arg7); \
    }

#define DEVICE_WRAP8(ret, func, type1, type2, type3, type4, type5, type6, type7, type8)  \
    ret ALT_WINAPI NineDevice9_ ## func(IDirect3DDevice9Ex *This, type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5, type6 arg6, type7 arg7, type8 arg8) \
    { \
        return ((IDirect3DDevice9Ex_Minor1 *)This)->lpVtbl_internal->func(This, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8); \
    }

#define DEVICE_WRAP9(ret, func, type1, type2, type3, type4, type5, type6, type7, type8, type9)  \
    ret ALT_WINAPI NineDevice9_ ## func(IDirect3DDevice9Ex *This, type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5, type6 arg6, type7 arg7, type8 arg8, type9 arg9) \
    { \
        return ((IDirect3DDevice9Ex_Minor1 *)This)->lpVtbl_internal->func(This, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9); \
    }

#define DEVICE_H_WRAP0(ret, func)  \
    ret ALT_WINAPI ALT_DECLSPEC_HOTPATCH NineDevice9_ ## func(IDirect3DDevice9Ex *This) \
    { \
        return ((IDirect3DDevice9Ex_Minor1 *)This)->lpVtbl_internal->func(This); \
    }

#define DEVICE_H_WRAP1(ret, func, type1)  \
    ret ALT_WINAPI ALT_DECLSPEC_HOTPATCH NineDevice9_ ## func(IDirect3DDevice9Ex *This, type1 arg1) \
    { \
        return ((IDirect3DDevice9Ex_Minor1 *)This)->lpVtbl_internal->func(This, arg1); \
    }

#define DEVICE_H_WRAP2(ret, func, type1, type2)  \
    ret ALT_WINAPI ALT_DECLSPEC_HOTPATCH NineDevice9_ ## func(IDirect3DDevice9Ex *This, type1 arg1, type2 arg2) \
    { \
        return ((IDirect3DDevice9Ex_Minor1 *)This)->lpVtbl_internal->func(This, arg1, arg2); \
    }

#define DEVICE_H_WRAP3(ret, func, type1, type2, type3)  \
    ret ALT_WINAPI ALT_DECLSPEC_HOTPATCH NineDevice9_ ## func(IDirect3DDevice9Ex *This, type1 arg1, type2 arg2, type3 arg3) \
    { \
        return ((IDirect3DDevice9Ex_Minor1 *)This)->lpVtbl_internal->func(This, arg1, arg2, arg3); \
    }

#define DEVICE_H_WRAP4(ret, func, type1, type2, type3, type4)  \
    ret ALT_WINAPI ALT_DECLSPEC_HOTPATCH NineDevice9_ ## func(IDirect3DDevice9Ex *This, type1 arg1, type2 arg2, type3 arg3, type4 arg4) \
    { \
        return ((IDirect3DDevice9Ex_Minor1 *)This)->lpVtbl_internal->func(This, arg1, arg2, arg3, arg4); \
    }

#define DEVICE_H_WRAP5(ret, func, type1, type2, type3, type4, type5)  \
    ret ALT_WINAPI ALT_DECLSPEC_HOTPATCH NineDevice9_ ## func(IDirect3DDevice9Ex *This, type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5) \
    { \
        return ((IDirect3DDevice9Ex_Minor1 *)This)->lpVtbl_internal->func(This, arg1, arg2, arg3, arg4, arg5); \
    }


static HRESULT ALT_WINAPI
NineSwapChain9_GetBackBuffer(IDirect3DSwapChain9Ex *This, UINT Level, D3DBACKBUFFER_TYPE type, IDirect3DSurface9 **ppSurface)
{    HRESULT hr;

    hr = ((IDirect3DSwapChain9Ex_Minor1 *)This)->lpVtbl_internal->GetBackBuffer(This, Level, type, ppSurface);
    if (FAILED(hr))
        return hr;

    (*ppSurface)->lpVtbl = (IDirect3DSurface9Vtbl *)&NineSurface9_vtable;

    return hr;
}



SWAPCHAIN_WRAP2(HRESULT, QueryInterface, REFIID, void **)
SWAPCHAIN_WRAP0(ULONG, AddRef)
SWAPCHAIN_WRAP0(ULONG, Release)
SWAPCHAIN_H_WRAP5(HRESULT, Present, const RECT *, const RECT *, HWND, const RGNDATA *, DWORD)
SWAPCHAIN_WRAP1(HRESULT, GetFrontBufferData, IDirect3DSurface9 *)
SWAPCHAIN_WRAP1(HRESULT, GetRasterStatus, D3DRASTER_STATUS *)
SWAPCHAIN_WRAP1(HRESULT, GetDisplayMode, D3DDISPLAYMODE *)
SWAPCHAIN_WRAP1(HRESULT, GetDevice, IDirect3DDevice9 **)
SWAPCHAIN_WRAP1(HRESULT, GetPresentParameters, D3DPRESENT_PARAMETERS *)
SWAPCHAIN_WRAP1(HRESULT, GetLastPresentCount, UINT *)
SWAPCHAIN_WRAP1(HRESULT, GetPresentStats, D3DPRESENTSTATS *)
SWAPCHAIN_WRAP2(HRESULT, GetDisplayModeEx, D3DDISPLAYMODEEX *, D3DDISPLAYROTATION *)

static HRESULT ALT_WINAPI
NineDevice9_GetBackBuffer(IDirect3DDevice9Ex *This, UINT Level, UINT arg, D3DBACKBUFFER_TYPE type, IDirect3DSurface9 **ppSurface)
{    HRESULT hr;

    hr = ((IDirect3DDevice9Ex_Minor1 *)This)->lpVtbl_internal->GetBackBuffer(This, Level, arg, type, ppSurface);
    if (FAILED(hr))
        return hr;

    (*ppSurface)->lpVtbl = (IDirect3DSurface9Vtbl *)&NineSurface9_vtable;

    return hr;
}

static HRESULT ALT_WINAPI
NineDevice9_CreateRenderTarget(IDirect3DDevice9Ex *This, UINT arg1, UINT arg2, D3DFORMAT arg3, D3DMULTISAMPLE_TYPE arg4, DWORD arg5, BOOL arg6, IDirect3DSurface9 **ppSurface, HANDLE *arg7)
{    HRESULT hr;

    hr = ((IDirect3DDevice9Ex_Minor1 *)This)->lpVtbl_internal->CreateRenderTarget(This, arg1, arg2, arg3, arg4, arg5, arg6, ppSurface, arg7);
    if (FAILED(hr))
        return hr;

    (*ppSurface)->lpVtbl = (IDirect3DSurface9Vtbl *)&NineSurface9_vtable;

    return hr;
}

static HRESULT ALT_WINAPI
NineDevice9_CreateRenderTargetEx(IDirect3DDevice9Ex *This, UINT arg1, UINT arg2, D3DFORMAT arg3, D3DMULTISAMPLE_TYPE arg4, DWORD arg5, BOOL arg6, IDirect3DSurface9 **ppSurface, HANDLE *arg7, DWORD arg8)
{    HRESULT hr;

    hr = ((IDirect3DDevice9Ex_Minor1 *)This)->lpVtbl_internal->CreateRenderTargetEx(This, arg1, arg2, arg3, arg4, arg5, arg6, ppSurface, arg7, arg8);
    if (FAILED(hr))
        return hr;

    (*ppSurface)->lpVtbl = (IDirect3DSurface9Vtbl *)&NineSurface9_vtable;

    return hr;
}

static HRESULT ALT_WINAPI
NineDevice9_CreateDepthStencilSurface(IDirect3DDevice9Ex *This, UINT arg1, UINT arg2, D3DFORMAT arg3, D3DMULTISAMPLE_TYPE arg4, DWORD arg5, BOOL arg6, IDirect3DSurface9 **ppSurface, HANDLE *arg7)
{    HRESULT hr;

    hr = ((IDirect3DDevice9Ex_Minor1 *)This)->lpVtbl_internal->CreateDepthStencilSurface(This, arg1, arg2, arg3, arg4, arg5, arg6, ppSurface, arg7);
    if (FAILED(hr))
        return hr;

    (*ppSurface)->lpVtbl = (IDirect3DSurface9Vtbl *)&NineSurface9_vtable;

    return hr;
}

static HRESULT ALT_WINAPI
NineDevice9_CreateDepthStencilSurfaceEx(IDirect3DDevice9Ex *This, UINT arg1, UINT arg2, D3DFORMAT arg3, D3DMULTISAMPLE_TYPE arg4, DWORD arg5, BOOL arg6, IDirect3DSurface9 **ppSurface, HANDLE *arg7, DWORD arg8)
{    HRESULT hr;

    hr = ((IDirect3DDevice9Ex_Minor1 *)This)->lpVtbl_internal->CreateDepthStencilSurfaceEx(This, arg1, arg2, arg3, arg4, arg5, arg6, ppSurface, arg7, arg8);
    if (FAILED(hr))
        return hr;

    (*ppSurface)->lpVtbl = (IDirect3DSurface9Vtbl *)&NineSurface9_vtable;

    return hr;
}

static HRESULT ALT_WINAPI
NineDevice9_CreateOffscreenPlainSurface(IDirect3DDevice9Ex *This, UINT arg1, UINT arg2, D3DFORMAT arg3, D3DPOOL arg4, IDirect3DSurface9 **ppSurface, HANDLE *arg5)
{    HRESULT hr;

    hr = ((IDirect3DDevice9Ex_Minor1 *)This)->lpVtbl_internal->CreateOffscreenPlainSurface(This, arg1, arg2, arg3, arg4, ppSurface, arg5);
    if (FAILED(hr))
        return hr;

    (*ppSurface)->lpVtbl = (IDirect3DSurface9Vtbl *)&NineSurface9_vtable;

    return hr;
}

static HRESULT ALT_WINAPI
NineDevice9_CreateOffscreenPlainSurfaceEx(IDirect3DDevice9Ex *This, UINT arg1, UINT arg2, D3DFORMAT arg3, D3DPOOL arg4, IDirect3DSurface9 **ppSurface, HANDLE *arg5, DWORD arg6)
{    HRESULT hr;

    hr = ((IDirect3DDevice9Ex_Minor1 *)This)->lpVtbl_internal->CreateOffscreenPlainSurfaceEx(This, arg1, arg2, arg3, arg4, ppSurface, arg5, arg6);
    if (FAILED(hr))
        return hr;

    (*ppSurface)->lpVtbl = (IDirect3DSurface9Vtbl *)&NineSurface9_vtable;

    return hr;
}

static HRESULT ALT_WINAPI
NineDevice9_GetDirect3D(struct NineDevice9 *This,
                         IDirect3D9 **ppD3D9)
{
    void **vtable_mirror;
    HRESULT hr = ((IDirect3DDevice9Ex_Minor1 *)This)->lpVtbl_internal->GetDirect3D(This, &vtable_mirror);
    /* ppD3D9 is the IDirect3D9 version. Let's get the _alt */
    ppD3D9 = (void*)(vtable_mirror-1);
}

static HRESULT ALT_WINAPI
NineDevice9_GetRenderTarget(IDirect3DDevice9Ex *This, DWORD s, IDirect3DSurface9 **ppSurface)
{   
    HRESULT hr;

    hr = ((IDirect3DDevice9Ex_Minor1 *)This)->lpVtbl_internal->GetRenderTarget(This, s, ppSurface);
    if (FAILED(hr))
        return hr;

    (*ppSurface)->lpVtbl = (IDirect3DSurface9Vtbl *)&NineSurface9_vtable;

    return hr;
}

static HRESULT ALT_WINAPI
NineDevice9_GetDepthStencilSurface(IDirect3DDevice9Ex *This, IDirect3DSurface9 **ppSurface)
{   
    HRESULT hr;

    hr = ((IDirect3DDevice9Ex_Minor1 *)This)->lpVtbl_internal->GetDepthStencilSurface(This, ppSurface);
    if (FAILED(hr))
        return hr;

    (*ppSurface)->lpVtbl = (IDirect3DSurface9Vtbl *)&NineSurface9_vtable;

    return hr;
}

static HRESULT ALT_WINAPI
NineDevice9_CreateStateBlock(IDirect3DDevice9Ex *This, D3DSTATEBLOCKTYPE type, IDirect3DStateBlock9 **ppSB)
{   
    HRESULT hr;

    hr = ((IDirect3DDevice9Ex_Minor1 *)This)->lpVtbl_internal->CreateStateBlock(This, type, ppSB);
    if (FAILED(hr))
        return hr;

    (*ppSB)->lpVtbl = (IDirect3DStateBlock9Vtbl *)&NineStateBlock9_vtable;

    return hr;
}

static HRESULT ALT_WINAPI
NineDevice9_EndStateBlock(IDirect3DDevice9Ex *This, IDirect3DStateBlock9 **ppSB)
{   
    HRESULT hr;

    hr = ((IDirect3DDevice9Ex_Minor1 *)This)->lpVtbl_internal->EndStateBlock(This, ppSB);
    if (FAILED(hr))
        return hr;

    (*ppSB)->lpVtbl = (IDirect3DStateBlock9Vtbl *)&NineStateBlock9_vtable;

    return hr;
}

DEVICE_WRAP2(HRESULT, QueryInterface, REFIID, void **)
DEVICE_WRAP0(ULONG, AddRef)
DEVICE_H_WRAP0(ULONG, Release)
DEVICE_WRAP0(HRESULT, TestCooperativeLevel)
DEVICE_WRAP0(UINT, GetAvailableTextureMem)
DEVICE_WRAP0(HRESULT, EvictManagedResources)
DEVICE_WRAP1(HRESULT, GetDeviceCaps, D3DCAPS9 *)
DEVICE_WRAP2(HRESULT, GetDisplayMode, UINT, D3DDISPLAYMODE*)
DEVICE_WRAP1(HRESULT, GetCreationParameters, D3DDEVICE_CREATION_PARAMETERS *)
DEVICE_WRAP3(HRESULT, SetCursorProperties, UINT, UINT, IDirect3DSurface9 *)
DEVICE_WRAP3(void, SetCursorPosition, int, int, DWORD)
DEVICE_WRAP1(BOOL, ShowCursor, BOOL)
/*DEVICE_H_WRAP2(HRESULT, CreateAdditionalSwapChain, D3DPRESENT_PARAMETERS *, IDirect3DSwapChain9 **)*/
/*DEVICE_H_WRAP2(HRESULT, GetSwapChain, UINT, IDirect3DSwapChain9 **)*/
DEVICE_WRAP0(UINT, GetNumberOfSwapChains)
DEVICE_H_WRAP1(HRESULT, Reset, D3DPRESENT_PARAMETERS *)
DEVICE_H_WRAP4(HRESULT, Present, const RECT *, const RECT *, HWND, const RGNDATA *)
DEVICE_WRAP2(HRESULT, GetRasterStatus, UINT, D3DRASTER_STATUS *)
DEVICE_WRAP1(HRESULT, SetDialogBoxMode, BOOL)
DEVICE_H_WRAP3(void, SetGammaRamp, UINT, DWORD, const D3DGAMMARAMP *)
DEVICE_WRAP2(void, GetGammaRamp, UINT, D3DGAMMARAMP *)
DEVICE_WRAP4(HRESULT, UpdateSurface, IDirect3DSurface9 *, const RECT *, IDirect3DSurface9 *, const POINT *)
DEVICE_WRAP2(HRESULT, UpdateTexture, IDirect3DBaseTexture9 *, IDirect3DBaseTexture9 *)
DEVICE_WRAP2(HRESULT, GetRenderTargetData, IDirect3DSurface9 *, IDirect3DSurface9 *)
DEVICE_WRAP2(HRESULT, GetFrontBufferData, UINT, IDirect3DSurface9 *)
DEVICE_WRAP5(HRESULT, StretchRect, IDirect3DSurface9 *, const RECT *, IDirect3DSurface9 *, const RECT *, D3DTEXTUREFILTERTYPE)
DEVICE_WRAP3(HRESULT, ColorFill, IDirect3DSurface9 *, const RECT *, D3DCOLOR)
DEVICE_WRAP2(HRESULT, SetRenderTarget, DWORD, IDirect3DSurface9 *)
//DEVICE_WRAP2(HRESULT, GetRenderTarget, DWORD, IDirect3DSurface9 **)
//DEVICE_WRAP1(HRESULT, GetDepthStencilSurface, IDirect3DSurface9 **)
DEVICE_WRAP1(HRESULT, SetDepthStencilSurface, IDirect3DSurface9 *)
DEVICE_WRAP0(HRESULT, BeginScene)
DEVICE_H_WRAP0(HRESULT, EndScene)
DEVICE_WRAP6(HRESULT, Clear, DWORD, const D3DRECT *, DWORD, D3DCOLOR, float, DWORD)
DEVICE_WRAP2(HRESULT, SetTransform, D3DTRANSFORMSTATETYPE, const D3DMATRIX *)
DEVICE_WRAP2(HRESULT, GetTransform, D3DTRANSFORMSTATETYPE, D3DMATRIX *)
DEVICE_WRAP2(HRESULT, MultiplyTransform, D3DTRANSFORMSTATETYPE, const D3DMATRIX *)
DEVICE_WRAP1(HRESULT, SetViewport, const D3DVIEWPORT9 *)
DEVICE_WRAP1(HRESULT, GetViewport, D3DVIEWPORT9 *)
DEVICE_WRAP1(HRESULT, SetMaterial, const D3DMATERIAL9 *)
DEVICE_WRAP1(HRESULT, GetMaterial, D3DMATERIAL9 *)
DEVICE_WRAP2(HRESULT, SetLight, DWORD, const D3DLIGHT9 *)
DEVICE_WRAP2(HRESULT, GetLight, DWORD, D3DLIGHT9 *)
DEVICE_WRAP2(HRESULT, LightEnable, DWORD, BOOL)
DEVICE_WRAP2(HRESULT, GetLightEnable, DWORD, BOOL *)
DEVICE_WRAP2(HRESULT, SetClipPlane, DWORD, const float *)
DEVICE_WRAP2(HRESULT, GetClipPlane, DWORD, float *)
DEVICE_H_WRAP2(HRESULT, SetRenderState, D3DRENDERSTATETYPE, DWORD)
DEVICE_WRAP2(HRESULT, GetRenderState, D3DRENDERSTATETYPE, DWORD *)
//DEVICE_WRAP2(HRESULT, CreateStateBlock, D3DSTATEBLOCKTYPE, IDirect3DStateBlock9 **)
DEVICE_WRAP0(HRESULT, BeginStateBlock)
//DEVICE_WRAP1(HRESULT, EndStateBlock, IDirect3DStateBlock9 **)
DEVICE_WRAP1(HRESULT, SetClipStatus, const D3DCLIPSTATUS9 *)
DEVICE_WRAP1(HRESULT, GetClipStatus, D3DCLIPSTATUS9 *)
DEVICE_WRAP2(HRESULT, GetTexture, DWORD, IDirect3DBaseTexture9 **)
DEVICE_WRAP2(HRESULT, SetTexture, DWORD, IDirect3DBaseTexture9 *)
DEVICE_WRAP3(HRESULT, GetTextureStageState, DWORD, D3DTEXTURESTAGESTATETYPE, DWORD *)
DEVICE_WRAP3(HRESULT, SetTextureStageState, DWORD, D3DTEXTURESTAGESTATETYPE, DWORD)
DEVICE_WRAP3(HRESULT, GetSamplerState, DWORD, D3DSAMPLERSTATETYPE, DWORD *)
DEVICE_H_WRAP3(HRESULT, SetSamplerState, DWORD, D3DSAMPLERSTATETYPE, DWORD)
DEVICE_WRAP1(HRESULT, ValidateDevice, DWORD *)
DEVICE_WRAP2(HRESULT, SetPaletteEntries, UINT, const PALETTEENTRY *)
DEVICE_WRAP2(HRESULT, GetPaletteEntries, UINT, PALETTEENTRY *)
DEVICE_WRAP1(HRESULT, SetCurrentTexturePalette, UINT)
DEVICE_WRAP1(HRESULT, GetCurrentTexturePalette, UINT *)
DEVICE_WRAP1(HRESULT, SetScissorRect, const RECT *)
DEVICE_WRAP1(HRESULT, GetScissorRect, RECT *)
DEVICE_WRAP1(HRESULT, SetSoftwareVertexProcessing, BOOL)
DEVICE_WRAP0(BOOL, GetSoftwareVertexProcessing)
DEVICE_WRAP1(HRESULT, SetNPatchMode, float)
DEVICE_WRAP0(float, GetNPatchMode)
DEVICE_WRAP3(HRESULT, DrawPrimitive, D3DPRIMITIVETYPE, UINT, UINT)
DEVICE_WRAP6(HRESULT, DrawIndexedPrimitive, D3DPRIMITIVETYPE, INT, UINT, UINT, UINT, UINT)
DEVICE_WRAP4(HRESULT, DrawPrimitiveUP, D3DPRIMITIVETYPE, UINT, const void *, UINT)
DEVICE_WRAP8(HRESULT, DrawIndexedPrimitiveUP, D3DPRIMITIVETYPE, UINT, UINT, UINT, const void *, D3DFORMAT, const void *, UINT)
DEVICE_WRAP6(HRESULT, ProcessVertices, UINT, UINT, UINT, IDirect3DVertexBuffer9 *, IDirect3DVertexDeclaration9 *, DWORD)
DEVICE_WRAP1(HRESULT, SetVertexDeclaration, IDirect3DVertexDeclaration9 *)
DEVICE_WRAP1(HRESULT, GetVertexDeclaration, IDirect3DVertexDeclaration9 **)
DEVICE_WRAP1(HRESULT, SetFVF, DWORD)
DEVICE_WRAP1(HRESULT, GetFVF, DWORD *)
DEVICE_WRAP1(HRESULT, SetVertexShader, IDirect3DVertexShader9 *)
DEVICE_WRAP1(HRESULT, GetVertexShader, IDirect3DVertexShader9 **)
DEVICE_WRAP3(HRESULT, SetVertexShaderConstantF, UINT, const float *, UINT)
DEVICE_WRAP3(HRESULT, GetVertexShaderConstantF, UINT, float *, UINT)
DEVICE_WRAP3(HRESULT, SetVertexShaderConstantI, UINT, const int *, UINT)
DEVICE_WRAP3(HRESULT, GetVertexShaderConstantI, UINT, int *, UINT)
DEVICE_WRAP3(HRESULT, SetVertexShaderConstantB, UINT, const BOOL *, UINT)
DEVICE_WRAP3(HRESULT, GetVertexShaderConstantB, UINT, BOOL *, UINT)
DEVICE_WRAP4(HRESULT, SetStreamSource, UINT, IDirect3DVertexBuffer9 *, UINT, UINT)
DEVICE_WRAP4(HRESULT, GetStreamSource, UINT, IDirect3DVertexBuffer9 **, UINT *, UINT *)
DEVICE_WRAP2(HRESULT, SetStreamSourceFreq, UINT, UINT)
DEVICE_WRAP2(HRESULT, GetStreamSourceFreq, UINT, UINT *)
DEVICE_WRAP1(HRESULT, SetIndices, IDirect3DIndexBuffer9 *)
DEVICE_WRAP2(HRESULT, GetIndices, IDirect3DIndexBuffer9 **, UINT*) //this should only have 1 param?
DEVICE_WRAP1(HRESULT, SetPixelShader, IDirect3DPixelShader9 *)
DEVICE_WRAP1(HRESULT, GetPixelShader, IDirect3DPixelShader9 **)
DEVICE_WRAP3(HRESULT, SetPixelShaderConstantF, UINT, const float *, UINT)
DEVICE_WRAP3(HRESULT, GetPixelShaderConstantF, UINT, float *, UINT)
DEVICE_WRAP3(HRESULT, SetPixelShaderConstantI, UINT, const int *, UINT)
DEVICE_WRAP3(HRESULT, GetPixelShaderConstantI, UINT, int *, UINT)
DEVICE_WRAP3(HRESULT, SetPixelShaderConstantB, UINT, const BOOL *, UINT)
DEVICE_WRAP3(HRESULT, GetPixelShaderConstantB, UINT, BOOL *, UINT)
DEVICE_WRAP3(HRESULT, DrawRectPatch, UINT, const float *, const D3DRECTPATCH_INFO *)
DEVICE_WRAP3(HRESULT, DrawTriPatch, UINT, const float *, const D3DTRIPATCH_INFO *)
DEVICE_WRAP1(HRESULT, DeletePatch, UINT)
DEVICE_WRAP4(HRESULT, SetConvolutionMonoKernel, UINT, UINT, float *, float *)
DEVICE_WRAP8(HRESULT, ComposeRects, IDirect3DSurface9 *, IDirect3DSurface9 *, IDirect3DVertexBuffer9 *, UINT, IDirect3DVertexBuffer9 *, D3DCOMPOSERECTSOP, int, int)
DEVICE_H_WRAP5(HRESULT, PresentEx, const RECT *, const RECT *, HWND, const RGNDATA *, DWORD)
DEVICE_WRAP1(HRESULT, GetGPUThreadPriority, INT *)
DEVICE_WRAP1(HRESULT, SetGPUThreadPriority, INT)
DEVICE_WRAP1(HRESULT, WaitForVBlank, UINT)
DEVICE_WRAP2(HRESULT, CheckResourceResidency, IDirect3DResource9 **, UINT32)
DEVICE_WRAP1(HRESULT, SetMaximumFrameLatency, UINT)
DEVICE_WRAP1(HRESULT, GetMaximumFrameLatency, UINT *)
DEVICE_WRAP1(HRESULT, CheckDeviceState, HWND)
DEVICE_H_WRAP2(HRESULT, ResetEx, D3DPRESENT_PARAMETERS *, D3DDISPLAYMODEEX *)
DEVICE_WRAP3(HRESULT, GetDisplayModeEx, UINT, D3DDISPLAYMODEEX *, D3DDISPLAYROTATION *)

IDirect3DSwapChain9ExVtbl NineSwapChain9Ex_vtable = {
    (void *)NineSwapChain9_QueryInterface,
    (void *)NineSwapChain9_AddRef,
    (void *)NineSwapChain9_Release,
    (void *)NineSwapChain9_Present,
    (void *)NineSwapChain9_GetFrontBufferData,
    (void *)NineSwapChain9_GetBackBuffer,
    (void *)NineSwapChain9_GetRasterStatus,
    (void *)NineSwapChain9_GetDisplayMode,
    (void *)NineSwapChain9_GetDevice,
    (void *)NineSwapChain9_GetPresentParameters,
    (void *)NineSwapChain9_GetLastPresentCount,
    (void *)NineSwapChain9_GetPresentStats,
    (void *)NineSwapChain9_GetDisplayModeEx
};

HRESULT ALT_WINAPI ALT_DECLSPEC_HOTPATCH NineDevice9_CreateAdditionalSwapChain(IDirect3DDevice9Ex *This, D3DPRESENT_PARAMETERS *pPresentationParameters, IDirect3DSwapChain9 **pSwapChain)
{
    HRESULT hr;

    hr = ((IDirect3DDevice9Ex_Minor1 *)This)->lpVtbl_internal->CreateAdditionalSwapChain(This, pPresentationParameters, pSwapChain);
    if (FAILED(hr))
        return hr;

    (*pSwapChain)->lpVtbl = (IDirect3DSwapChain9Vtbl *)&NineSwapChain9Ex_vtable;

    return hr;
}

HRESULT ALT_WINAPI ALT_DECLSPEC_HOTPATCH NineDevice9_GetSwapChain(IDirect3DDevice9Ex *This, UINT iSwapChain, IDirect3DSwapChain9 **pSwapChain)
{
    HRESULT hr;

    hr = ((IDirect3DDevice9Ex_Minor1 *)This)->lpVtbl_internal->GetSwapChain(This, iSwapChain, pSwapChain);
    if (FAILED(hr))
        return hr;

    (*pSwapChain)->lpVtbl = (IDirect3DSwapChain9Vtbl *)&NineSwapChain9Ex_vtable;

    return hr;
}



#define VDECL_WRAP2(ret, func, type1, type2)  \
    ret ALT_WINAPI NineVertexDeclaration9_ ## func(IDirect3DVertexDeclaration9 *This, type1 arg1, type2 arg2) \
    { \
        return ((IDirect3DVertexDeclaration9_Minor1 *)This)->lpVtbl_internal->func(This, arg1, arg2); \
    }


VDECL_WRAP2(HRESULT, GetDeclaration, D3DVERTEXELEMENT9 *, UINT *)

static IDirect3DVertexDeclaration9Vtbl NineVertexDeclaration9_vtable = {
    (void *)Nine_QueryInterface,
    (void *)Nine_AddRef,
    (void *)Nine_Release,
    (void *)Nine_GetDevice,
    (void *)NineVertexDeclaration9_GetDeclaration
};

static HRESULT ALT_WINAPI ALT_DECLSPEC_HOTPATCH NineDevice9_CreateVertexDeclaration(IDirect3DDevice9Ex *This, const D3DVERTEXELEMENT9 *pVertexElements, IDirect3DVertexDeclaration9 **ppDecl)
{
    HRESULT hr;
    hr = ((IDirect3DDevice9Ex_Minor1 *)This)->lpVtbl_internal->CreateVertexDeclaration(This, pVertexElements, ppDecl);
    if (FAILED(hr))
        return hr;

    (*ppDecl)->lpVtbl = (IDirect3DVertexDeclaration9Vtbl *)&NineVertexDeclaration9_vtable;

    return hr;
}


struct IDirect3DTexture9_Minor1
{
        IDirect3DTexture9Vtbl *lpVtbl;
        IDirect3DTexture9Vtbl *lpVtbl_internal;
};
typedef struct IDirect3DTexture9_Minor1 IDirect3DTexture9_Minor1;
#define TEX_WRAP0(ret, func)  \
    ret ALT_WINAPI NineTexture9_ ## func(void *This) \
    { \
        return ((IDirect3DTexture9_Minor1 *)This)->lpVtbl_internal->func(This); \
    }

#define TEX_WRAP1(ret, func, type1)  \
    ret ALT_WINAPI NineTexture9_ ## func(void *This, type1 arg1) \
    { \
        return ((IDirect3DTexture9_Minor1 *)This)->lpVtbl_internal->func(This, arg1); \
    }

#define TEX_WRAP2(ret, func, type1, type2)  \
    ret ALT_WINAPI NineTexture9_ ## func(void *This, type1 arg1, type2 arg2) \
    { \
        return ((IDirect3DTexture9_Minor1 *)This)->lpVtbl_internal->func(This, arg1, arg2); \
    }

#define TEX_WRAP4(ret, func, type1, type2, type3, type4)  \
    ret ALT_WINAPI NineTexture9_ ## func(void *This, type1 arg1, type2 arg2, type3 arg3, type4 arg4) \
    { \
        return ((IDirect3DTexture9_Minor1 *)This)->lpVtbl_internal->func(This, arg1, arg2, arg3, arg4); \
    }

TEX_WRAP1(DWORD, SetLOD, DWORD)
TEX_WRAP0(DWORD, GetLOD)
TEX_WRAP0(DWORD, GetLevelCount)
TEX_WRAP1(HRESULT, SetAutoGenFilterType, D3DTEXTUREFILTERTYPE)
TEX_WRAP0(D3DTEXTUREFILTERTYPE, GetAutoGenFilterType)
TEX_WRAP0(void, GenerateMipSubLevels)
TEX_WRAP2(HRESULT, GetLevelDesc, UINT, D3DSURFACE_DESC*)
TEX_WRAP4(HRESULT, LockRect, UINT, D3DLOCKED_RECT *, const RECT *, DWORD)
TEX_WRAP1(HRESULT, UnlockRect, UINT)
TEX_WRAP1(HRESULT, AddDirtyRect, const RECT *)

static HRESULT ALT_WINAPI ALT_DECLSPEC_HOTPATCH 
NineTexture9_GetSurfaceLevel(IDirect3DTexture9 *This, UINT Level, IDirect3DSurface9 **ppSurfaceLevel)
{    HRESULT hr;

    hr = ((IDirect3DTexture9_Minor1 *)This)->lpVtbl_internal->GetSurfaceLevel(This, Level, ppSurfaceLevel);
    if (FAILED(hr))
        return hr;

    (*ppSurfaceLevel)->lpVtbl = (IDirect3DSurface9Vtbl *)&NineSurface9_vtable;

    return hr;
}

static IDirect3DTexture9Vtbl NineTexture9_vtable = {
    (void *)Nine_QueryInterface,
    (void *)Nine_AddRef,
    (void *)Nine_Release,
    (void *)Nine_GetDevice,
    (void *)Nine_SetPrivateData,
    (void *)Nine_GetPrivateData,
    (void *)Nine_FreePrivateData,
    (void *)Nine_SetPriority,
    (void *)Nine_GetPriority,
    (void *)Nine_PreLoad,
    (void *)Nine_GetType,
    (void *)NineTexture9_SetLOD,
    (void *)NineTexture9_GetLOD,
    (void *)NineTexture9_GetLevelCount,
    (void *)NineTexture9_SetAutoGenFilterType,
    (void *)NineTexture9_GetAutoGenFilterType,
    (void *)NineTexture9_GenerateMipSubLevels,
    (void *)NineTexture9_GetLevelDesc, /* immutable */
    (void *)NineTexture9_GetSurfaceLevel, /* AddRef */
    (void *)NineTexture9_LockRect,
    (void *)NineTexture9_UnlockRect,
    (void *)NineTexture9_AddDirtyRect
};

static HRESULT ALT_WINAPI ALT_DECLSPEC_HOTPATCH 
NineDevice9_CreateTexture( IDirect3DDevice9Ex *This,
                           UINT Width,
                           UINT Height,
                           UINT Levels,
                           DWORD Usage,
                           D3DFORMAT Format,
                           D3DPOOL Pool,
                           IDirect3DTexture9 **ppTexture,
                           HANDLE *pSharedHandle )
{
    HRESULT hr;

    hr = ((IDirect3DDevice9Ex_Minor1 *)This)->lpVtbl_internal->CreateTexture(This, Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
    if (FAILED(hr))
        return hr;

    (*ppTexture)->lpVtbl = (IDirect3DTexture9Vtbl *)&NineTexture9_vtable;

    return hr;
}


struct IDirect3DQuery9_Minor1
{
        IDirect3DQuery9Vtbl *lpVtbl;
        IDirect3DQuery9Vtbl *lpVtbl_internal;
};
typedef struct IDirect3DQuery9_Minor1 IDirect3DQuery9_Minor1;
#define QUERY_WRAP0(ret, func)  \
    ret ALT_WINAPI NineQuery9_ ## func(void *This) \
    { \
        return ((IDirect3DQuery9_Minor1 *)This)->lpVtbl_internal->func(This); \
    }

#define QUERY_WRAP1(ret, func, type1)  \
    ret ALT_WINAPI NineQuery9_ ## func(void *This, type1 arg1) \
    { \
        return ((IDirect3DQuery9_Minor1 *)This)->lpVtbl_internal->func(This, arg1); \
    }

#define QUERY_WRAP3(ret, func, type1, type2, type3)  \
    ret ALT_WINAPI NineQuery9_ ## func(void *This, type1 arg1, type2 arg2, type3 arg3) \
    { \
        return ((IDirect3DQuery9_Minor1 *)This)->lpVtbl_internal->func(This, arg1, arg2, arg3); \
    }

QUERY_WRAP0(D3DQUERYTYPE, GetType)
QUERY_WRAP0(DWORD, GetDataSize)
QUERY_WRAP1(HRESULT, Issue, DWORD)
QUERY_WRAP3(HRESULT, GetData, void *, DWORD, DWORD)
static IDirect3DQuery9Vtbl NineQuery9_vtable = {
    (void *)Nine_QueryInterface,
    (void *)Nine_AddRef,
    (void *)Nine_Release,
    (void *)Nine_GetDevice, /* actually part of Query9 iface */
    (void *)NineQuery9_GetType, /* immutable */
    (void *)NineQuery9_GetDataSize, /* immutable */
    (void *)NineQuery9_Issue,
    (void *)NineQuery9_GetData
};
static HRESULT ALT_WINAPI
NineDevice9_CreateQuery(IDirect3DDevice9Ex *This, D3DQUERYTYPE Type, IDirect3DQuery9 **ppQuery)
{
    HRESULT hr;

    hr = ((IDirect3DDevice9Ex_Minor1 *)This)->lpVtbl_internal->CreateQuery(This, Type, ppQuery);
    if (FAILED(hr))
        return hr;

    (*ppQuery)->lpVtbl = (IDirect3DQuery9Vtbl *)&NineQuery9_vtable;

    return hr;
}

struct IDirect3DPixelShader9_Minor1
{
        IDirect3DPixelShader9Vtbl *lpVtbl;
        IDirect3DPixelShader9Vtbl *lpVtbl_internal;
};
typedef struct IDirect3DPixelShader9_Minor1 IDirect3DPixelShader9_Minor1;
#define PS_WRAP2(ret, func, type1, type2)  \
    ret ALT_WINAPI NinePixelShader9_ ## func(void *This, type1 arg1, type2 arg2) \
    { \
        return ((IDirect3DPixelShader9_Minor1 *)This)->lpVtbl_internal->func(This, arg1, arg2); \
    }

PS_WRAP2(HRESULT, GetFunction, void *, UINT*)
static IDirect3DPixelShader9Vtbl NinePixelShader9_vtable = {
    (void *)Nine_QueryInterface,
    (void *)Nine_AddRef,
    (void *)Nine_Release,
    (void *)Nine_GetDevice, /* actually part of PixelShader9 iface */
    (void *)NinePixelShader9_GetFunction
};
static HRESULT ALT_WINAPI
NineDevice9_CreatePixelShader(IDirect3DDevice9Ex *This, const DWORD *pFunction, IDirect3DPixelShader9 **ppShader)
{
    HRESULT hr;

    hr = ((IDirect3DDevice9Ex_Minor1 *)This)->lpVtbl_internal->CreatePixelShader(This, pFunction, ppShader);
    if (FAILED(hr))
        return hr;

    (*ppShader)->lpVtbl = (IDirect3DPixelShader9Vtbl *)&NinePixelShader9_vtable;

    return hr;
}

struct IDirect3DVertexShader9_Minor1
{
        IDirect3DVertexShader9Vtbl *lpVtbl;
        IDirect3DVertexShader9Vtbl *lpVtbl_internal;
};
typedef struct IDirect3DVertexShader9_Minor1 IDirect3DVertexShader9_Minor1;
#define VS_WRAP2(ret, func, type1, type2)  \
    ret ALT_WINAPI NineVertexShader9_ ## func(void *This, type1 arg1, type2 arg2) \
    { \
        return ((IDirect3DVertexShader9_Minor1 *)This)->lpVtbl_internal->func(This, arg1, arg2); \
    }

VS_WRAP2(HRESULT, GetFunction, void *, UINT*)
static IDirect3DVertexShader9Vtbl NineVertexShader9_vtable = {
    (void *)Nine_QueryInterface,
    (void *)Nine_AddRef,
    (void *)Nine_Release,
    (void *)Nine_GetDevice, /* actually part of VertexShader9 iface */
    (void *)NineVertexShader9_GetFunction
};
static HRESULT ALT_WINAPI
NineDevice9_CreateVertexShader(IDirect3DDevice9Ex *This, const DWORD *pFunction, IDirect3DVertexShader9 **ppShader)
{
    HRESULT hr;

    hr = ((IDirect3DDevice9Ex_Minor1 *)This)->lpVtbl_internal->CreateVertexShader(This, pFunction, ppShader);
    if (FAILED(hr))
        return hr;

    (*ppShader)->lpVtbl = (IDirect3DVertexShader9Vtbl *)&NineVertexShader9_vtable;

    return hr;
}

struct IDirect3DVolumeTexture9_Minor1
{
        IDirect3DVolumeTexture9Vtbl *lpVtbl;
        IDirect3DVolumeTexture9Vtbl *lpVtbl_internal;
};
typedef struct IDirect3DVolumeTexture9_Minor1 IDirect3DVolumeTexture9_Minor1;
#define TEX3D_WRAP2(ret, func, type1, type2)  \
    ret ALT_WINAPI NineVolumeTexture9_ ## func(void *This, type1 arg1, type2 arg2) \
    { \
        return ((IDirect3DVolumeTexture9_Minor1 *)This)->lpVtbl_internal->func(This, arg1, arg2); \
    }

#define TEX3D_WRAP1(ret, func, type1)  \
    ret ALT_WINAPI NineVolumeTexture9_ ## func(void *This, type1 arg1) \
    { \
        return ((IDirect3DVolumeTexture9_Minor1 *)This)->lpVtbl_internal->func(This, arg1); \
    }

#define TEX3D_WRAP4(ret, func, type1, type2, type3, type4)  \
    ret ALT_WINAPI NineVolumeTexture9_ ## func(void *This, type1 arg1, type2 arg2, type3 arg3, type4 arg4) \
    { \
        return ((IDirect3DVolumeTexture9_Minor1 *)This)->lpVtbl_internal->func(This, arg1, arg2, arg3, arg4); \
    }

//TEX3D_WRAP2(HRESULT, GetVolumeLevel, UINT, IDirect3DVolume9 **)
TEX3D_WRAP4(HRESULT, LockBox, UINT, D3DLOCKED_BOX *, const D3DBOX *, DWORD)
TEX3D_WRAP1(HRESULT, UnlockBox, UINT)
TEX3D_WRAP1(HRESULT, AddDirtyBox, const D3DBOX *)

static HRESULT ALT_WINAPI ALT_DECLSPEC_HOTPATCH 
NineVolumeTexture9_GetVolumeLevel(IDirect3DVolumeTexture9 *This, UINT Level, IDirect3DVolume9 **ppVolumeLevel)
{    HRESULT hr;

    hr = ((IDirect3DVolumeTexture9_Minor1 *)This)->lpVtbl_internal->GetVolumeLevel(This, Level, ppVolumeLevel);
    if (FAILED(hr))
        return hr;

    (*ppVolumeLevel)->lpVtbl = (IDirect3DVolume9Vtbl *)&NineVolume9_vtable;

    return hr;
}

IDirect3DVolumeTexture9Vtbl NineVolumeTexture9_vtable = {
    (void *)Nine_QueryInterface,
    (void *)Nine_AddRef,
    (void *)Nine_Release,
    (void *)Nine_GetDevice,
    (void *)Nine_SetPrivateData,
    (void *)Nine_GetPrivateData,
    (void *)Nine_FreePrivateData,
    (void *)Nine_SetPriority,
    (void *)Nine_GetPriority,
    (void *)Nine_PreLoad,
    (void *)Nine_GetType,
    (void *)NineTexture9_SetLOD,
    (void *)NineTexture9_GetLOD,
    (void *)NineTexture9_GetLevelCount,
    (void *)NineTexture9_SetAutoGenFilterType,
    (void *)NineTexture9_GetAutoGenFilterType,
    (void *)NineTexture9_GenerateMipSubLevels,
    (void *)NineTexture9_GetLevelDesc, /* immutable */
    (void *)NineVolumeTexture9_GetVolumeLevel, /* AddRef */
    (void *)NineVolumeTexture9_LockBox,
    (void *)NineVolumeTexture9_UnlockBox,
    (void *)NineVolumeTexture9_AddDirtyBox
};
static HRESULT ALT_WINAPI
NineDevice9_CreateVolumeTexture(IDirect3DDevice9Ex *This, UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9 **ppVolumeTexture, HANDLE *pSharedHandle)
{
    HRESULT hr;

    hr = ((IDirect3DDevice9Ex_Minor1 *)This)->lpVtbl_internal->CreateVolumeTexture(This, Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture, pSharedHandle);
    if (FAILED(hr))
        return hr;

    (*ppVolumeTexture)->lpVtbl = (IDirect3DVolumeTexture9Vtbl *)&NineVolumeTexture9_vtable;

    return hr;
}

struct IDirect3DCubeTexture9_Minor1
{
        IDirect3DCubeTexture9Vtbl *lpVtbl;
        IDirect3DCubeTexture9Vtbl *lpVtbl_internal;
};
typedef struct IDirect3DCubeTexture9_Minor1 IDirect3DCubeTexture9_Minor1;
#define CUBE_WRAP2(ret, func, type1, type2)  \
    ret ALT_WINAPI NineCubeTexture9_ ## func(void *This, type1 arg1, type2 arg2) \
    { \
        return ((IDirect3DCubeTexture9_Minor1 *)This)->lpVtbl_internal->func(This, arg1, arg2); \
    }
#define CUBE_WRAP3(ret, func, type1, type2, type3)  \
    ret ALT_WINAPI NineCubeTexture9_ ## func(void *This, type1 arg1, type2 arg2, type3 arg3) \
    { \
        return ((IDirect3DCubeTexture9_Minor1 *)This)->lpVtbl_internal->func(This, arg1, arg2, arg3); \
    }
typedef struct IDirect3DCubeTexture9_Minor1 IDirect3DCubeTexture9_Minor1;
#define CUBE_WRAP5(ret, func, type1, type2, type3, type4, type5)  \
    ret ALT_WINAPI NineCubeTexture9_ ## func(void *This, type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5) \
    { \
        return ((IDirect3DCubeTexture9_Minor1 *)This)->lpVtbl_internal->func(This, arg1, arg2, arg3, arg4, arg5); \
    }
CUBE_WRAP2(HRESULT, GetLevelDesc, UINT, D3DSURFACE_DESC*)
//CUBE_WRAP3(HRESULT, GetCubeMapSurface, D3DCUBEMAP_FACES, UINT, IDirect3DSurface9 **)
CUBE_WRAP5(HRESULT, LockRect, D3DCUBEMAP_FACES, UINT, D3DLOCKED_RECT *, const RECT *, DWORD)
CUBE_WRAP2(HRESULT, UnlockRect, D3DCUBEMAP_FACES, UINT)
CUBE_WRAP2(HRESULT, AddDirtyRect, D3DCUBEMAP_FACES, const RECT *)

static HRESULT ALT_WINAPI ALT_DECLSPEC_HOTPATCH 
NineCubeTexture9_GetCubeMapSurface(IDirect3DCubeTexture9 *This, D3DCUBEMAP_FACES FaceType, UINT Level, IDirect3DSurface9 **ppCubeMapSurface)
{    HRESULT hr;

    hr = ((IDirect3DCubeTexture9_Minor1 *)This)->lpVtbl_internal->GetCubeMapSurface(This, FaceType, Level, ppCubeMapSurface);
    if (FAILED(hr))
        return hr;

    (*ppCubeMapSurface)->lpVtbl = (IDirect3DSurface9Vtbl *)&NineSurface9_vtable;

    return hr;
}

static IDirect3DCubeTexture9Vtbl NineCubeTexture9_vtable = {
    (void *)Nine_QueryInterface,
    (void *)Nine_AddRef,
    (void *)Nine_Release,
    (void *)Nine_GetDevice,
    (void *)Nine_SetPrivateData,
    (void *)Nine_GetPrivateData,
    (void *)Nine_FreePrivateData,
    (void *)Nine_SetPriority,
    (void *)Nine_GetPriority,
    (void *)Nine_PreLoad,
    (void *)Nine_GetType,
    (void *)NineTexture9_SetLOD,
    (void *)NineTexture9_GetLOD,
    (void *)NineTexture9_GetLevelCount,
    (void *)NineTexture9_SetAutoGenFilterType,
    (void *)NineTexture9_GetAutoGenFilterType,
    (void *)NineTexture9_GenerateMipSubLevels,
    (void *)NineCubeTexture9_GetLevelDesc, /* immutable */
    (void *)NineCubeTexture9_GetCubeMapSurface, /* AddRef */
    (void *)NineCubeTexture9_LockRect,
    (void *)NineCubeTexture9_UnlockRect,
    (void *)NineCubeTexture9_AddDirtyRect
};

static HRESULT ALT_WINAPI ALT_DECLSPEC_HOTPATCH 
NineDevice9_CreateCubeTexture(IDirect3DDevice9Ex *This, UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9 **ppCubeTexture, HANDLE *pSharedHandle)
{
    HRESULT hr;

    hr = ((IDirect3DDevice9Ex_Minor1 *)This)->lpVtbl_internal->CreateCubeTexture(This, EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture, pSharedHandle);
    if (FAILED(hr))
        return hr;

    (*ppCubeTexture)->lpVtbl = (IDirect3DCubeTexture9Vtbl *)&NineCubeTexture9_vtable;

    return hr;
}

struct IDirect3DIndexBuffer9_Minor1
{
        IDirect3DIndexBuffer9Vtbl *lpVtbl;
        IDirect3DIndexBuffer9Vtbl *lpVtbl_internal;
};
typedef struct IDirect3DIndexBuffer9_Minor1 IDirect3DIndexBuffer9_Minor1;
#define IB_WRAP0(ret, func)  \
    ret ALT_WINAPI NineIndexBuffer9_ ## func(void *This) \
    { \
        return ((IDirect3DIndexBuffer9_Minor1 *)This)->lpVtbl_internal->func(This); \
    }

#define IB_WRAP1(ret, func, type1)  \
    ret ALT_WINAPI NineIndexBuffer9_ ## func(void *This, type1 arg1) \
    { \
        return ((IDirect3DIndexBuffer9_Minor1 *)This)->lpVtbl_internal->func(This, arg1); \
    }

#define IB_WRAP4(ret, func, type1, type2, type3, type4)  \
    ret ALT_WINAPI NineIndexBuffer9_ ## func(void *This, type1 arg1, type2 arg2, type3 arg3, type4 arg4) \
    { \
        return ((IDirect3DIndexBuffer9_Minor1 *)This)->lpVtbl_internal->func(This, arg1, arg2, arg3, arg4); \
    }
IB_WRAP4(HRESULT, Lock, UINT, UINT, void **, DWORD)
IB_WRAP0(HRESULT, Unlock)
IB_WRAP1(HRESULT, GetDesc, D3DINDEXBUFFER_DESC *)

IDirect3DIndexBuffer9Vtbl NineIndexBuffer9_vtable = {
    (void *)Nine_QueryInterface,
    (void *)Nine_AddRef,
    (void *)Nine_Release,
    (void *)Nine_GetDevice, /* actually part of  iface */
    (void *)Nine_SetPrivateData,
    (void *)Nine_GetPrivateData,
    (void *)Nine_FreePrivateData,
    (void *)Nine_SetPriority,
    (void *)Nine_GetPriority,
    (void *)Nine_PreLoad, /* nop */
    (void *)Nine_GetType, /* immutable */
    (void *)NineIndexBuffer9_Lock,
    (void *)NineIndexBuffer9_Unlock,
    (void *)NineIndexBuffer9_GetDesc /* immutable */
};

static HRESULT ALT_WINAPI
NineDevice9_CreateIndexBuffer(IDirect3DDevice9Ex *This, UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9 **ppIndexBuffer, HANDLE *pSharedHandle)
{
    HRESULT hr;

    hr = ((IDirect3DDevice9Ex_Minor1 *)This)->lpVtbl_internal->CreateIndexBuffer(This, Length, Usage, Format, Pool, ppIndexBuffer, pSharedHandle);
    if (FAILED(hr))
        return hr;

    (*ppIndexBuffer)->lpVtbl = (IDirect3DIndexBuffer9Vtbl *)&NineIndexBuffer9_vtable;

    return hr;
}

static HRESULT ALT_WINAPI
NineDevice9_CreateVertexBuffer(IDirect3DDevice9Ex *This, UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9 **ppVertexBuffer, HANDLE *pSharedHandle)
{
    HRESULT hr;

    hr = ((IDirect3DDevice9Ex_Minor1 *)This)->lpVtbl_internal->CreateVertexBuffer(This, Length, Usage, FVF, Pool, ppVertexBuffer, pSharedHandle);
    if (FAILED(hr))
        return hr;

    (*ppVertexBuffer)->lpVtbl = (IDirect3DVertexBuffer9Vtbl *)&NineIndexBuffer9_vtable;

    return hr;
}

static IDirect3DDevice9ExVtbl NineDevice9_vtable = {
    (void *)NineDevice9_QueryInterface,
    (void *)NineDevice9_AddRef,
    (void *)NineDevice9_Release,
    (void *)NineDevice9_TestCooperativeLevel,
    (void *)NineDevice9_GetAvailableTextureMem,
    (void *)NineDevice9_EvictManagedResources,
    (void *)NineDevice9_GetDirect3D,
    (void *)NineDevice9_GetDeviceCaps,
    (void *)NineDevice9_GetDisplayMode,
    (void *)NineDevice9_GetCreationParameters,
    (void *)NineDevice9_SetCursorProperties,
    (void *)NineDevice9_SetCursorPosition,
    (void *)NineDevice9_ShowCursor,
    (void *)NineDevice9_CreateAdditionalSwapChain,
    (void *)NineDevice9_GetSwapChain,
    (void *)NineDevice9_GetNumberOfSwapChains,
    (void *)NineDevice9_Reset,
    (void *)NineDevice9_Present,
    (void *)NineDevice9_GetBackBuffer,
    (void *)NineDevice9_GetRasterStatus,
    (void *)NineDevice9_SetDialogBoxMode,
    (void *)NineDevice9_SetGammaRamp,
    (void *)NineDevice9_GetGammaRamp,
    (void *)NineDevice9_CreateTexture,
    (void *)NineDevice9_CreateVolumeTexture,
    (void *)NineDevice9_CreateCubeTexture,
    (void *)NineDevice9_CreateVertexBuffer,
    (void *)NineDevice9_CreateIndexBuffer,
    (void *)NineDevice9_CreateRenderTarget,
    (void *)NineDevice9_CreateDepthStencilSurface,
    (void *)NineDevice9_UpdateSurface,
    (void *)NineDevice9_UpdateTexture,
    (void *)NineDevice9_GetRenderTargetData,
    (void *)NineDevice9_GetFrontBufferData,
    (void *)NineDevice9_StretchRect,
    (void *)NineDevice9_ColorFill,
    (void *)NineDevice9_CreateOffscreenPlainSurface,
    (void *)NineDevice9_SetRenderTarget,
    (void *)NineDevice9_GetRenderTarget,
    (void *)NineDevice9_SetDepthStencilSurface,
    (void *)NineDevice9_GetDepthStencilSurface,
    (void *)NineDevice9_BeginScene,
    (void *)NineDevice9_EndScene,
    (void *)NineDevice9_Clear,
    (void *)NineDevice9_SetTransform,
    (void *)NineDevice9_GetTransform,
    (void *)NineDevice9_MultiplyTransform,
    (void *)NineDevice9_SetViewport,
    (void *)NineDevice9_GetViewport,
    (void *)NineDevice9_SetMaterial,
    (void *)NineDevice9_GetMaterial,
    (void *)NineDevice9_SetLight,
    (void *)NineDevice9_GetLight,
    (void *)NineDevice9_LightEnable,
    (void *)NineDevice9_GetLightEnable,
    (void *)NineDevice9_SetClipPlane,
    (void *)NineDevice9_GetClipPlane,
    (void *)NineDevice9_SetRenderState,
    (void *)NineDevice9_GetRenderState,
    (void *)NineDevice9_CreateStateBlock,
    (void *)NineDevice9_BeginStateBlock,
    (void *)NineDevice9_EndStateBlock,
    (void *)NineDevice9_SetClipStatus,
    (void *)NineDevice9_GetClipStatus,
    (void *)NineDevice9_GetTexture,
    (void *)NineDevice9_SetTexture,
    (void *)NineDevice9_GetTextureStageState,
    (void *)NineDevice9_SetTextureStageState,
    (void *)NineDevice9_GetSamplerState,
    (void *)NineDevice9_SetSamplerState,
    (void *)NineDevice9_ValidateDevice,
    (void *)NineDevice9_SetPaletteEntries,
    (void *)NineDevice9_GetPaletteEntries,
    (void *)NineDevice9_SetCurrentTexturePalette,
    (void *)NineDevice9_GetCurrentTexturePalette,
    (void *)NineDevice9_SetScissorRect,
    (void *)NineDevice9_GetScissorRect,
    (void *)NineDevice9_SetSoftwareVertexProcessing,
    (void *)NineDevice9_GetSoftwareVertexProcessing,
    (void *)NineDevice9_SetNPatchMode,
    (void *)NineDevice9_GetNPatchMode,
    (void *)NineDevice9_DrawPrimitive,
    (void *)NineDevice9_DrawIndexedPrimitive,
    (void *)NineDevice9_DrawPrimitiveUP,
    (void *)NineDevice9_DrawIndexedPrimitiveUP,
    (void *)NineDevice9_ProcessVertices,
    (void *)NineDevice9_CreateVertexDeclaration,
    (void *)NineDevice9_SetVertexDeclaration,
    (void *)NineDevice9_GetVertexDeclaration,
    (void *)NineDevice9_SetFVF,
    (void *)NineDevice9_GetFVF,
    (void *)NineDevice9_CreateVertexShader,
    (void *)NineDevice9_SetVertexShader,
    (void *)NineDevice9_GetVertexShader,
    (void *)NineDevice9_SetVertexShaderConstantF,
    (void *)NineDevice9_GetVertexShaderConstantF,
    (void *)NineDevice9_SetVertexShaderConstantI,
    (void *)NineDevice9_GetVertexShaderConstantI,
    (void *)NineDevice9_SetVertexShaderConstantB,
    (void *)NineDevice9_GetVertexShaderConstantB,
    (void *)NineDevice9_SetStreamSource,
    (void *)NineDevice9_GetStreamSource,
    (void *)NineDevice9_SetStreamSourceFreq,
    (void *)NineDevice9_GetStreamSourceFreq,
    (void *)NineDevice9_SetIndices,
    (void *)NineDevice9_GetIndices,
    (void *)NineDevice9_CreatePixelShader,
    (void *)NineDevice9_SetPixelShader,
    (void *)NineDevice9_GetPixelShader,
    (void *)NineDevice9_SetPixelShaderConstantF,
    (void *)NineDevice9_GetPixelShaderConstantF,
    (void *)NineDevice9_SetPixelShaderConstantI,
    (void *)NineDevice9_GetPixelShaderConstantI,
    (void *)NineDevice9_SetPixelShaderConstantB,
    (void *)NineDevice9_GetPixelShaderConstantB,
    (void *)NineDevice9_DrawRectPatch,
    (void *)NineDevice9_DrawTriPatch,
    (void *)NineDevice9_DeletePatch,
    (void *)NineDevice9_CreateQuery,
    (void *)NineDevice9_SetConvolutionMonoKernel,
    (void *)NineDevice9_ComposeRects,
    (void *)NineDevice9_PresentEx,
    (void *)NineDevice9_GetGPUThreadPriority,
    (void *)NineDevice9_SetGPUThreadPriority,
    (void *)NineDevice9_WaitForVBlank,
    (void *)NineDevice9_CheckResourceResidency,
    (void *)NineDevice9_SetMaximumFrameLatency,
    (void *)NineDevice9_GetMaximumFrameLatency,
    (void *)NineDevice9_CheckDeviceState,
    (void *)NineDevice9_CreateRenderTargetEx,
    (void *)NineDevice9_CreateOffscreenPlainSurfaceEx,
    (void *)NineDevice9_CreateDepthStencilSurfaceEx,
    (void *)NineDevice9_ResetEx,
    (void *)NineDevice9_GetDisplayModeEx
};

void *get_device_vtable()
{
    printf("WRAPPED DEVICE TABLE\n");
    return &NineDevice9_vtable;
}
