/*
** Copyright (c) 2007 The Khronos Group Inc.
** 
** Permission is hereby granted, free of charge, to any person obtaining a
** copy of this software and/or associated documentation files (the
** "Materials"), to deal in the Materials without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Materials, and to
** permit persons to whom the Materials are furnished to do so, subject to
** the following conditions:
** 
** The above copyright notice and this permission notice shall be included
** in all copies or substantial portions of the Materials.
** 
** THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
*/


/* -*- mode: c; tab-width: 8; -*- */
/* vi: set sw=4 ts=8: */
/* Platform-specific types and definitions for egl.h */

#ifndef __eglplatform_h_
#define __eglplatform_h_

/* Windows calling convention boilerplate */
#if defined(_WIN32) && !defined(APIENTRY) && !defined(__CYGWIN__) && !defined(__SCITECH_SNAP__)
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#endif

#include <sys/types.h>

/* Macros used in EGL function prototype declarations.
 *
 * EGLAPI return-type EGLAPIENTRY eglFunction(arguments);
 *
 * On Windows, EGLAPIENTRY can be defined like APIENTRY.
 * On most other platforms, it should be empty.
 */

#ifdef _WIN32
#ifdef EGL_DLL_EXPORTS
#define EGLAPI __declspec(dllexport)
#else
#define EGLAPI __declspec(dllimport)
#endif
#endif

#ifndef EGLAPIENTRY
#define EGLAPIENTRY
#endif
#ifndef EGLAPI
#define EGLAPI extern
#endif

/* The types NativeDisplayType, NativeWindowType, and NativePixmapType
 * are aliases of window-system-dependent types, such as X Display * or
 * Windows Device Context. They must be defined in platform-specific
 * code below. The EGL-prefixed versions of Native*Type are the same
 * types, renamed in EGL 1.3 so all types in the API start with "EGL".
 */

/* Unix (tentative)
    #include <X headers>
    typedef Display *NativeDisplayType;
      - or maybe, if encoding "hostname:display.head"
    typedef const char *NativeWindowType;
 etc.
 */

//VC03
#ifndef __int8_t_defined	// Hack: linux stdint.h defines this when typedefing int32_t
typedef signed long     int32_t;
#endif
typedef int32_t         NativeDisplayType; /*!< Native display handle type definition     */
typedef void*           NativeWindowType;   /*!< Native window handle type definition, HWND on win32 */
typedef void*           NativePixmapType;   /*!< Native pixmap handle type definition, HBITMAP on win32 */
#ifndef GL_APICALL
#define GL_APICALL
#endif
#define EGL_APICALL     EGLAPI

/* EGL 1.2 types, renamed for consistency in EGL 1.3 */
typedef NativeDisplayType EGLNativeDisplayType;
typedef NativePixmapType EGLNativePixmapType;
typedef NativeWindowType EGLNativeWindowType;

#endif /* __eglplatform_h */
