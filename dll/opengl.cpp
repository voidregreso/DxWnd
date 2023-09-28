#define _CRT_SECURE_NO_WARNINGS

#define STRETCHDRAWPIXELS
#define STRETCHBITMAPS
//#define SCALEDUMP
//#define TRACEALL

#include "stdio.h"
#include "dxwnd.h"
#include "dxwcore.hpp"
#include "syslibs.h"
#include "dxhook.h"
#include "dxhelper.h"
#include "gl\gl.h"
#include "gl\wglext.h"
#include "gl\glext.h"
#ifdef STRETCHBITMAPS
#include <math.h>
#endif

#define DXWDECLARATIONS TRUE
#include "glhook.h"
#undef DXWDECLARATIONS

#ifndef COMPRESSED_RGB_S3TC_DXT1_EXT
#define COMPRESSED_RGB_S3TC_DXT1_EXT                   0x83F0
#define COMPRESSED_RGBA_S3TC_DXT1_EXT                  0x83F1
#define COMPRESSED_RGBA_S3TC_DXT3_EXT                  0x83F2
#define COMPRESSED_RGBA_S3TC_DXT5_EXT                  0x83F3
#endif

//#include "logall.h"

// glut + wgl
typedef void (WINAPI *glutFullScreen_Type)(void);
glutFullScreen_Type pglutFullScreen;
void WINAPI extglutFullScreen(void);
typedef void (WINAPI *glutInitWindowSize_Type)(int, int);
glutInitWindowSize_Type pglutInitWindowSize;
void extglutInitWindowSize(int, int);
typedef void (WINAPI *glutInitWindowPosition_Type)(int, int);
glutInitWindowPosition_Type pglutInitWindowPosition;
void extglutInitWindowPosition(int, int);
typedef void (WINAPI *glutSetWindow_Type)(HWND);
glutSetWindow_Type pglutSetWindow;
void WINAPI extglutSetWindow(HWND);
typedef const GLubyte *(WINAPI *glGetString_Type)(GLenum);
glGetString_Type pglGetString;
const GLubyte *WINAPI extglGetString(GLenum);
typedef char *(WINAPI *wglGetExtensionsStringEXT_Type)(void);
wglGetExtensionsStringEXT_Type pwglGetExtensionsStringEXT;
char *WINAPI extwglGetExtensionsStringEXT(void);
typedef const GLubyte *(WINAPI *gluGetString_Type)(GLenum);
gluGetString_Type pgluGetString;
const GLubyte *WINAPI extgluGetString(GLenum);
#ifdef TRACEALL
typedef void (WINAPI *glutSwapBuffers_Type)(void);
glutSwapBuffers_Type pglutSwapBuffers;
void WINAPI extglutSwapBuffers(void);
typedef GLint (WINAPI *gluBuild2DMipmaps_Type)(GLenum, GLint, GLsizei, GLsizei, GLenum, GLenum, const void *);
gluBuild2DMipmaps_Type pgluBuild2DMipmaps;
GLint WINAPI extgluBuild2DMipmaps(GLenum, GLint, GLsizei, GLsizei, GLenum, GLenum, const void *);
typedef void (WINAPI *glGenTextures_Type)(GLsizei, GLuint *);
glGenTextures_Type pglGenTextures;
void WINAPI extglGenTextures(GLsizei, GLuint *);
typedef void (WINAPI *gluOrtho2D_Type)(GLdouble, GLdouble, GLdouble, GLdouble);
gluOrtho2D_Type pgluOrtho2D;
void WINAPI extgluOrtho2D(GLdouble, GLdouble, GLdouble, GLdouble);
typedef void (WINAPI *glHint_Type)(GLenum, GLenum);
glHint_Type pglHint;
void WINAPI extglHint(GLenum, GLenum);
typedef void (WINAPI *glFrontFace_Type)(GLenum);
glFrontFace_Type pglFrontFace;
void WINAPI extglFrontFace(GLenum);
typedef void (WINAPI *glEnableClientState_Type)(GLenum);
glEnableClientState_Type pglEnableClientState;
void WINAPI extglEnableClientState(GLenum);
typedef void (WINAPI *glMateriali_Type)(GLenum, GLenum, GLint);
glMateriali_Type pglMateriali;
void WINAPI extglMateriali(GLenum, GLenum, GLint);
typedef void (WINAPI *glMaterialfv_Type)(GLenum, GLenum, const GLfloat *);
glMaterialfv_Type pglMaterialfv;
void WINAPI extglMaterialfv(GLenum, GLenum, const GLfloat *);
typedef void (WINAPI *glLightf_Type)(GLenum, GLenum, GLfloat);
glLightf_Type pglLightf;
void WINAPI extglLightf(GLenum, GLenum, GLfloat);
typedef void (WINAPI *glLighti_Type)(GLenum, GLenum, GLint);
glLighti_Type pglLighti;
void WINAPI extglLighti(GLenum, GLenum, GLint);
typedef void (WINAPI *glLightfv_Type)(GLenum, GLenum, const GLfloat *);
glLightfv_Type pglLightfv;
void WINAPI extglLightfv(GLenum, GLenum, const GLfloat *);
typedef void (WINAPI *glLightiv_Type)(GLenum, GLenum, const GLint *);
glLightiv_Type pglLightiv;
void WINAPI extglLightiv(GLenum, GLenum, const GLint *);
typedef void (WINAPI *glPushMatrix_Type)();
glPushMatrix_Type pglPushMatrix;
void WINAPI extglPushMatrix();
typedef void (WINAPI *glRotated_Type)(GLdouble, GLdouble, GLdouble, GLdouble);
glRotated_Type pglRotated;
void WINAPI extglRotated(GLdouble, GLdouble, GLdouble, GLdouble);
typedef void (WINAPI *glRotatef_Type)(GLfloat, GLfloat, GLfloat, GLfloat);
glRotatef_Type pglRotatef;
void WINAPI extglRotatef(GLfloat, GLfloat, GLfloat, GLfloat);
typedef void (WINAPI *glArrayElement_Type)(GLint);
glArrayElement_Type pglArrayElement;
void WINAPI extglArrayElement(GLint);
typedef void (WINAPI *glRasterPos4fv_Type)(const GLfloat *);
glRasterPos4fv_Type pglRasterPos4fv;
void WINAPI extglRasterPos4fv(const GLfloat *);
typedef void (WINAPI *glRasterPos2i_Type)(GLint, GLint);
glRasterPos2i_Type pglRasterPos2i;
void WINAPI extglRasterPos2i(GLint, GLint);
typedef void (WINAPI *glRasterPos2d_Type)(GLdouble, GLdouble);
glRasterPos2d_Type pglRasterPos2d;
void WINAPI extglRasterPos2d(GLdouble, GLdouble);
typedef void (WINAPI *glRasterPos2f_Type)(GLfloat, GLfloat);
glRasterPos2f_Type pglRasterPos2f;
void WINAPI extglRasterPos2f(GLfloat, GLfloat);
#endif

typedef void (WINAPI *glBitmap_Type)(GLsizei, GLsizei, GLfloat, GLfloat, GLfloat, GLfloat, const GLubyte *);
glBitmap_Type pglBitmap;
void WINAPI extglBitmap(GLsizei, GLsizei, GLfloat, GLfloat, GLfloat, GLfloat, const GLubyte *);
typedef void (WINAPI *glTexSubImage2D_Type)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *);
glTexSubImage2D_Type pglTexSubImage2D;
void WINAPI extglTexSubImage2D(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *);
typedef void (WINAPI *glTexParameteri_Type)(GLenum, GLenum, GLint);
glTexParameteri_Type pglTexParameteri, pglTexEnvi;
void WINAPI extglTexParameteri(GLenum, GLenum, GLint);
typedef void (WINAPI *glTexParameterf_Type)(GLenum, GLenum, GLfloat);
glTexParameterf_Type pglTexParameterf, pglTexEnvf;
void WINAPI extglTexParameterf(GLenum, GLenum, GLfloat);

#ifdef TRACEALL
typedef void (WINAPI *glVertex2f_Type)(GLfloat, GLfloat);
glVertex2f_Type pglVertex2f;
void WINAPI extglVertex2f(GLfloat, GLfloat);
typedef void (WINAPI *glVertex3f_Type)(GLfloat, GLfloat, GLfloat);
glVertex3f_Type pglVertex3f;
void WINAPI extglVertex3f(GLfloat, GLfloat, GLfloat);
typedef void (WINAPI *glFrustum_Type)(GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble);
glFrustum_Type pglFrustum;
void WINAPI extglFrustum(GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble);
typedef void (WINAPI *glDrawArrays_Type)(GLenum mode, GLint first, GLsizei count);
glDrawArrays_Type pglDrawArrays;
void WINAPI extglDrawArrays(GLenum mode, GLint first, GLsizei count);
typedef void (WINAPI *glTranslatef_Type)(GLfloat, GLfloat, GLfloat);
glTranslatef_Type pglTranslatef;
void WINAPI extglTranslatef(GLfloat, GLfloat, GLfloat);
typedef void (WINAPI *glScalef_Type)(GLfloat, GLfloat, GLfloat);
glScalef_Type pglScalef;
void WINAPI extglScalef(GLfloat, GLfloat, GLfloat);
typedef void (WINAPI *glOrtho_Type)(GLdouble,  GLdouble,  GLdouble,  GLdouble,  GLdouble,  GLdouble);
glOrtho_Type pglOrtho;
void WINAPI extglOrtho(GLdouble,  GLdouble,  GLdouble,  GLdouble,  GLdouble,  GLdouble);
typedef void (WINAPI *glEnable_Type)(GLenum);
glEnable_Type pglEnable, pglDisable;
void WINAPI extglEnable(GLenum);
void WINAPI extglDisable(GLenum);
void WINAPI extglTexEnvi(GLenum, GLenum, GLint);
void WINAPI extglTexEnvf(GLenum, GLenum, GLfloat);
typedef void (WINAPI *glDeleteTextures_Type)(GLsizei, const GLuint *);
glDeleteTextures_Type pglDeleteTextures;
void WINAPI extglDeleteTextures(GLsizei, const GLuint *);
typedef void (WINAPI *glMatrixMode_Type)(GLenum);
glMatrixMode_Type pglMatrixMode;
void WINAPI extglMatrixMode(GLenum);
typedef void (WINAPI *glLoadIdentity_Type)(void);
glLoadIdentity_Type pglLoadIdentity;
void WINAPI extglLoadIdentity(void);
#endif

typedef HGLRC (WINAPI *wglCreateLayerContext_Type)(HDC, int);
wglCreateLayerContext_Type pwglCreateLayerContext;
HGLRC WINAPI extwglCreateLayerContext(HDC, int);
typedef BOOL (WINAPI *wglDeleteContext_Type)(HGLRC);
wglDeleteContext_Type pwglDeleteContext;
BOOL WINAPI extwglDeleteContext(HGLRC);
typedef HGLRC (WINAPI *wglGetCurrentContext_Type)(void);
wglGetCurrentContext_Type pwglGetCurrentContext;
HGLRC WINAPI extwglGetCurrentContext(void);
typedef BOOL (WINAPI *wglCopyContext_Type)(HGLRC, HGLRC, UINT);
wglCopyContext_Type pwglCopyContext;
BOOL WINAPI extwglCopyContext(HGLRC, HGLRC, UINT);

#ifdef TRACEALL
typedef void (WINAPI *glViewportArrayv_Type)(GLuint, GLsizei, const GLfloat *);
glViewportArrayv_Type pglViewportArrayv;
void WINAPI extglViewportArrayv(GLuint, GLsizei, const GLfloat *);

typedef void (WINAPI *glViewportIndexedf_Type)(GLuint, GLfloat, GLfloat, GLfloat, GLfloat);
glViewportIndexedf_Type pglViewportIndexedf;
void WINAPI extglViewportIndexedf(GLuint, GLfloat, GLfloat, GLfloat, GLfloat);

typedef void (WINAPI *glViewportIndexedfv_Type)(GLuint, const GLfloat *);
glViewportIndexedfv_Type pglViewportIndexedfv;
void WINAPI extglViewportIndexedfv(GLuint, const GLfloat *);

typedef void (WINAPI *glScissorArrayv_Type)(GLuint, GLsizei, const GLfloat *);
glScissorArrayv_Type pglScissorArrayv;
void WINAPI extglScissorArrayv(GLuint, GLsizei, const GLfloat *);

typedef void (WINAPI *glScissorIndexed_Type)(GLuint, GLint, GLint, GLsizei, GLsizei);
glScissorIndexed_Type pglScissorIndexed;
void WINAPI extglScissorIndexed(GLuint, GLint, GLint, GLsizei, GLsizei);

typedef void (WINAPI *glScissorIndexedf_Type)(GLuint, GLfloat, GLfloat, GLfloat, GLfloat);
glScissorIndexedf_Type pglScissorIndexedf;
void WINAPI extglScissorIndexedf(GLuint, GLfloat, GLfloat, GLfloat, GLfloat);

typedef void (WINAPI *glScissorIndexedv_Type)(GLuint, const GLint *);
glScissorIndexedv_Type pglScissorIndexedv;
void WINAPI extglScissorIndexedv(GLuint, const GLint *);

typedef void (WINAPI *glScissorIndexedfv_Type)(GLuint, const GLfloat *);
glScissorIndexedfv_Type pglScissorIndexedfv;
void WINAPI extglScissorIndexedfv(GLuint, const GLfloat *);

typedef void (WINAPI *glWindowPos2s_Type)(GLshort, GLshort);
glWindowPos2s_Type pglWindowPos2s;
void WINAPI extglWindowPos2s(GLshort, GLshort);

typedef void (WINAPI *glWindowPos2i_Type)(GLint, GLint);
glWindowPos2i_Type pglWindowPos2i;
void WINAPI extglWindowPos2i(GLint, GLint);

typedef void (WINAPI *glWindowPos2f_Type)(GLfloat, GLfloat);
glWindowPos2f_Type pglWindowPos2f;
void WINAPI extglWindowPos2f(GLfloat, GLfloat);
#endif

#ifdef STRETCHDRAWPIXELS
void WINAPI extglDrawPixels(GLsizei, GLsizei, GLenum, GLenum, const GLvoid *);
typedef void (WINAPI *glDrawPixels_Type)(GLsizei, GLsizei, GLenum, GLenum, const GLvoid *);
glDrawPixels_Type pglDrawPixels = NULL;
#endif

static HookEntryEx_Type Hooks[] = {
    {HOOK_IAT_CANDIDATE, 0, "glGetError", NULL, (FARPROC *) &pglGetError, (FARPROC)extglGetError},
    {HOOK_IAT_CANDIDATE, 0, "glViewport", NULL, (FARPROC *) &pglViewport, (FARPROC)extglViewport},
#ifdef TRACEALL
    {HOOK_IAT_CANDIDATE, 0, "glViewportArrayv", NULL, (FARPROC *) &pglViewportArrayv, (FARPROC)extglViewportArrayv},
    {HOOK_IAT_CANDIDATE, 0, "glViewportIndexedf", NULL, (FARPROC *) &pglViewportIndexedf, (FARPROC)extglViewportIndexedf},
    {HOOK_IAT_CANDIDATE, 0, "glViewportIndexedfv", NULL, (FARPROC *) &pglViewportIndexedfv, (FARPROC)extglViewportIndexedfv},
#endif
    {HOOK_IAT_CANDIDATE, 0, "glScissor", NULL, (FARPROC *) &pglScissor, (FARPROC)extglScissor},
#ifdef TRACEALL
    {HOOK_IAT_CANDIDATE, 0, "glScissorArrayv", NULL, (FARPROC *) &pglScissorArrayv, (FARPROC)extglScissorArrayv},
    {HOOK_IAT_CANDIDATE, 0, "glScissorIndexed", NULL, (FARPROC *) &pglScissorIndexed, (FARPROC)extglScissorIndexed},
    {HOOK_IAT_CANDIDATE, 0, "glScissorIndexedf", NULL, (FARPROC *) &pglScissorIndexedf, (FARPROC)extglScissorIndexedf},
    {HOOK_IAT_CANDIDATE, 0, "glScissorIndexedfv", NULL, (FARPROC *) &pglScissorIndexedfv, (FARPROC)extglScissorIndexedfv},
    {HOOK_IAT_CANDIDATE, 0, "glScissorIndexedv", NULL, (FARPROC *) &pglScissorIndexedv, (FARPROC)extglScissorIndexedv},
    {HOOK_IAT_CANDIDATE, 0, "glScissorIndexedfv", NULL, (FARPROC *) &pglScissorIndexedfv, (FARPROC)extglScissorIndexedfv},
    {HOOK_IAT_CANDIDATE, 0, "glWindowPos2s", NULL, (FARPROC *) &pglWindowPos2s, (FARPROC)extglWindowPos2s},
    {HOOK_IAT_CANDIDATE, 0, "glWindowPos2i", NULL, (FARPROC *) &pglWindowPos2i, (FARPROC)extglWindowPos2i},
    {HOOK_IAT_CANDIDATE, 0, "glWindowPos2f", NULL, (FARPROC *) &pglWindowPos2f, (FARPROC)extglWindowPos2f},
#endif
    {HOOK_IAT_CANDIDATE, 0, "glGetIntegerv", NULL, (FARPROC *) &pglGetIntegerv, (FARPROC) &extglGetIntegerv},
    {HOOK_IAT_CANDIDATE, 0, "glDrawBuffer", NULL, (FARPROC *) &pglDrawBuffer, (FARPROC)extglDrawBuffer},
    {HOOK_IAT_CANDIDATE, 0, "glPolygonMode", NULL, (FARPROC *) &pglPolygonMode, (FARPROC)extglPolygonMode},
    {HOOK_IAT_CANDIDATE, 0, "glGetFloatv", NULL, (FARPROC *) &pglGetFloatv, (FARPROC)extglGetFloatv},
    {HOOK_IAT_CANDIDATE, 0, "glClear", NULL, (FARPROC *) &pglClear, (FARPROC)extglClear},
    {HOOK_IAT_CANDIDATE, 0, "glTexImage2D", NULL, (FARPROC *) &pglTexImage2D, (FARPROC)extglTexImage2D},
    {HOOK_IAT_CANDIDATE, 0, "glTexSubImage2D", NULL, (FARPROC *) &pglTexSubImage2D, (FARPROC)extglTexSubImage2D},
#ifdef STRETCHDRAWPIXELS
    {HOOK_IAT_CANDIDATE, 0, "glDrawPixels", NULL, (FARPROC *) &pglDrawPixels, (FARPROC)extglDrawPixels},
#endif
    {HOOK_IAT_CANDIDATE, 0, "glPixelZoom", NULL, (FARPROC *) &pglPixelZoom, (FARPROC)extglPixelZoom},
    {HOOK_IAT_CANDIDATE, 0, "glBindTexture", NULL, (FARPROC *) &pglBindTexture, (FARPROC)extglBindTexture},
    {HOOK_IAT_CANDIDATE, 0, "glBitmap", NULL, (FARPROC *) &pglBitmap, (FARPROC)extglBitmap},
    //{HOOK_IAT_CANDIDATE, 0, "glCopyTexImage2D", NULL, (FARPROC *)&pglCopyTexImage2D, (FARPROC)extglCopyTexImage2D},
    {HOOK_IAT_CANDIDATE, 0, "glGetString", NULL, (FARPROC *) &pglGetString, (FARPROC)extglGetString},
    {HOOK_IAT_CANDIDATE, 0, "glTexParameteri", NULL, (FARPROC *) &pglTexParameteri, (FARPROC)extglTexParameteri},
    {HOOK_IAT_CANDIDATE, 0, "glTexParameterf", NULL, (FARPROC *) &pglTexParameterf, (FARPROC)extglTexParameterf},
#ifdef TRACEALL
    {HOOK_IAT_CANDIDATE, 0, "glBegin", NULL, (FARPROC *) &pglBegin, (FARPROC)extglBegin},
    {HOOK_IAT_CANDIDATE, 0, "glEnd", NULL, (FARPROC *) &pglEnd, (FARPROC)extglEnd},
    {HOOK_IAT_CANDIDATE, 0, "glVertex2f", NULL, (FARPROC *) &pglVertex2f, (FARPROC)extglVertex2f},
    {HOOK_IAT_CANDIDATE, 0, "glVertex3f", NULL, (FARPROC *) &pglVertex3f, (FARPROC)extglVertex3f},
    {HOOK_IAT_CANDIDATE, 0, "glFrustum", NULL, (FARPROC *) &pglFrustum, (FARPROC)extglFrustum},
    {HOOK_IAT_CANDIDATE, 0, "glDrawArrays", NULL, (FARPROC *) &pglDrawArrays, (FARPROC)extglDrawArrays},
    {HOOK_IAT_CANDIDATE, 0, "glTranslatef", NULL, (FARPROC *) &pglTranslatef, (FARPROC)extglTranslatef},
    {HOOK_IAT_CANDIDATE, 0, "glScalef", NULL, (FARPROC *) &pglScalef, (FARPROC)extglScalef},
    {HOOK_IAT_CANDIDATE, 0, "glOrtho", NULL, (FARPROC *) &pglOrtho, (FARPROC)extglOrtho},
    {HOOK_IAT_CANDIDATE, 0, "glPixelStorei", NULL, (FARPROC *) &pglPixelStorei, (FARPROC)extglPixelStorei},
    {HOOK_IAT_CANDIDATE, 0, "glEnable", NULL, (FARPROC *) &pglEnable, (FARPROC)extglEnable},
    {HOOK_IAT_CANDIDATE, 0, "glDisable", NULL, (FARPROC *) &pglDisable, (FARPROC)extglDisable},
    {HOOK_IAT_CANDIDATE, 0, "glGenTextures", NULL, (FARPROC *) &pglGenTextures, (FARPROC)extglGenTextures},
    {HOOK_IAT_CANDIDATE, 0, "glTexEnvi", NULL, (FARPROC *) &pglTexEnvi, (FARPROC)extglTexEnvi},
    {HOOK_IAT_CANDIDATE, 0, "glTexEnvf", NULL, (FARPROC *) &pglTexEnvf, (FARPROC)extglTexEnvf},
    {HOOK_IAT_CANDIDATE, 0, "glDeleteTextures", NULL, (FARPROC *) &pglDeleteTextures, (FARPROC)extglDeleteTextures},
    {HOOK_IAT_CANDIDATE, 0, "glMatrixMode", NULL, (FARPROC *) &pglMatrixMode, (FARPROC)extglMatrixMode},
    {HOOK_IAT_CANDIDATE, 0, "glLoadIdentity", NULL, (FARPROC *) &pglLoadIdentity, (FARPROC)extglLoadIdentity},
    {HOOK_IAT_CANDIDATE, 0, "glHint", NULL, (FARPROC *) &pglHint, (FARPROC)extglHint},
    {HOOK_IAT_CANDIDATE, 0, "glFrontFace", NULL, (FARPROC *) &pglFrontFace, (FARPROC)extglFrontFace},
    {HOOK_IAT_CANDIDATE, 0, "glEnableClientState", NULL, (FARPROC *) &pglEnableClientState, (FARPROC)extglEnableClientState},
    {HOOK_IAT_CANDIDATE, 0, "glMateriali", NULL, (FARPROC *) &pglMateriali, (FARPROC)extglMateriali},
    {HOOK_IAT_CANDIDATE, 0, "glMaterialfv", NULL, (FARPROC *) &pglMaterialfv, (FARPROC)extglMaterialfv},
    {HOOK_IAT_CANDIDATE, 0, "glLightf", NULL, (FARPROC *) &pglLightf, (FARPROC)extglLightf},
    {HOOK_IAT_CANDIDATE, 0, "glLighti", NULL, (FARPROC *) &pglLighti, (FARPROC)extglLighti},
    {HOOK_IAT_CANDIDATE, 0, "glLightfv", NULL, (FARPROC *) &pglLightfv, (FARPROC)extglLightfv},
    {HOOK_IAT_CANDIDATE, 0, "glLightiv", NULL, (FARPROC *) &pglLightiv, (FARPROC)extglLightiv},
    {HOOK_IAT_CANDIDATE, 0, "glPushMatrix", NULL, (FARPROC *) &pglPushMatrix, (FARPROC)extglPushMatrix},
    {HOOK_IAT_CANDIDATE, 0, "glRotated", NULL, (FARPROC *) &pglRotated, (FARPROC)extglRotated},
    {HOOK_IAT_CANDIDATE, 0, "glRotatef", NULL, (FARPROC *) &pglRotatef, (FARPROC)extglRotatef},
    {HOOK_IAT_CANDIDATE, 0, "glArrayElement", NULL, (FARPROC *) &pglArrayElement, (FARPROC)extglArrayElement},
    {HOOK_IAT_CANDIDATE, 0, "glRasterPos2i", NULL, (FARPROC *) &pglRasterPos2i, (FARPROC)extglRasterPos2i},
    {HOOK_IAT_CANDIDATE, 0, "glRasterPos2d", NULL, (FARPROC *) &pglRasterPos2d, (FARPROC)extglRasterPos2d},
    {HOOK_IAT_CANDIDATE, 0, "glRasterPos2f", NULL, (FARPROC *) &pglRasterPos2f, (FARPROC)extglRasterPos2f}, // impacts on glBitmap
    {HOOK_IAT_CANDIDATE, 0, "glRasterPos4fv", NULL, (FARPROC *) &pglRasterPos4fv, (FARPROC)extglRasterPos4fv},
#endif
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type WiggieHooks[] = {
    {HOOK_IAT_CANDIDATE, 0, "wglCreateContext", NULL, (FARPROC *) &pwglCreateContext, (FARPROC)extwglCreateContext},
    {HOOK_IAT_CANDIDATE, 0, "wglCreateLayerContext", NULL, (FARPROC *) &pwglCreateLayerContext, (FARPROC)extwglCreateLayerContext},
    {HOOK_IAT_CANDIDATE, 0, "wglDeleteContext", NULL, (FARPROC *) &pwglDeleteContext, (FARPROC)extwglDeleteContext},
    {HOOK_IAT_CANDIDATE, 0, "wglGetCurrentContext", NULL, (FARPROC *) &pwglGetCurrentContext, (FARPROC)extwglGetCurrentContext},
    {HOOK_IAT_CANDIDATE, 0, "wglCopyContext", NULL, (FARPROC *) &pwglCopyContext, (FARPROC)extwglCopyContext},
    {HOOK_IAT_CANDIDATE, 0, "wglMakeCurrent", NULL, (FARPROC *) &pwglMakeCurrent, (FARPROC)extwglMakeCurrent},
    {HOOK_IAT_CANDIDATE, 0, "wglGetProcAddress", NULL, (FARPROC *) &pwglGetProcAddress, (FARPROC)extwglGetProcAddress},
    {HOOK_IAT_CANDIDATE, 0, "wglGetExtensionsStringEXT", NULL, (FARPROC *) &pwglGetExtensionsStringEXT, (FARPROC)extwglGetExtensionsStringEXT},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

// to do:
//	glutInitDisplayMode
//  glutCreateWindow,  glutCreateSubWindow
//	glutPositionWindow,  glutReshapeWindow
//	glGetFloatv ( GL_SCISSOR_BOX - GL_VIEWPORT )

static HookEntryEx_Type GlutHooks[] = {
    {HOOK_IAT_CANDIDATE, 0, "glutFullScreen", NULL, (FARPROC *) &pglutFullScreen, (FARPROC)extglutFullScreen},
    {HOOK_IAT_CANDIDATE, 0, "glutInitWindowSize", NULL, (FARPROC *) &pglutInitWindowSize, (FARPROC)extglutInitWindowSize},
    {HOOK_IAT_CANDIDATE, 0, "glutInitWindowPosition", NULL, (FARPROC *) &pglutInitWindowPosition, (FARPROC)extglutInitWindowPosition},
    {HOOK_IAT_CANDIDATE, 0, "glutSetWindow", NULL, (FARPROC *) &pglutSetWindow, (FARPROC)extglutSetWindow},
#ifdef TRACEALL
    {HOOK_IAT_CANDIDATE, 0, "glutSwapBuffers", NULL, (FARPROC *) &pglutSwapBuffers, (FARPROC)extglutSwapBuffers},
    {HOOK_IAT_CANDIDATE, 0, "gluBuild2DMipmaps", NULL, (FARPROC *) &pgluBuild2DMipmaps, (FARPROC)extgluBuild2DMipmaps},
#endif // TRACEALL
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type GluHooks[] = {
    {HOOK_IAT_CANDIDATE, 0, "gluGetString", NULL, (FARPROC *) &pgluGetString, (FARPROC)extgluGetString},
#ifdef TRACEALL
    {HOOK_IAT_CANDIDATE, 0, "gluOrtho2D", NULL, (FARPROC *) &pgluOrtho2D, (FARPROC)extgluOrtho2D},
#endif // TRACEALL
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

FARPROC Remap_gl_ProcAddress(LPCSTR proc, HMODULE hModule) {
    FARPROC addr;
    if(!(dxw.dwFlags2 & HOOKOPENGL)) return NULL;
    if (addr = RemapLibraryEx(proc, hModule, Hooks)) return addr;
    if (dxw.dwFlags9 & HOOKWGLCONTEXT) if(addr = RemapLibraryEx(proc, hModule, WiggieHooks)) return addr;
    if (dxw.dwFlags7 & HOOKGLUT32) if(addr = RemapLibraryEx(proc, hModule, GlutHooks)) return addr;
    // NULL -> keep the original call address
    return NULL;
}

PROC Remap_wgl_ProcAddress(LPCSTR proc) {
    int i;
    HookEntryEx_Type *Hook;
    if(!(dxw.dwFlags2 & HOOKOPENGL)) return NULL;
    for(i = 0; WiggieHooks[i].APIName; i++) {
        Hook = &WiggieHooks[i];
        if (!strcmp(proc, Hook->APIName)) {
            if (Hook->StoreAddress) { // avoid clearing function pointers
                PROC Addr = (*pwglGetProcAddress)(proc);
                if(Addr) *(Hook->StoreAddress) = Addr;
            }
            OutTraceDW("GetProcAddress: hooking proc=%s at addr=%#x\n", ProcToString(proc), (Hook->StoreAddress) ? * (Hook->StoreAddress) : 0);
            return Hook->HookerAddress;
        }
    }
    // v2.04.39: SDL2 game bstone (window port of "Blak Stone") loads all gl* methods through wglGetProcAddress !!
    for(i = 0; Hooks[i].APIName; i++) {
        Hook = &Hooks[i];
        if (!strcmp(proc, Hook->APIName)) {
            if (Hook->StoreAddress) { // avoid clearing function pointers
                PROC Addr = (*pwglGetProcAddress)(proc);
                if(Addr) *(Hook->StoreAddress) = Addr;
            }
            OutTraceDW("GetProcAddress: hooking proc=%s at addr=%#x\n", ProcToString(proc), (Hook->StoreAddress) ? * (Hook->StoreAddress) : 0);
            return Hook->HookerAddress;
        }
    }
    // NULL -> keep the original call address
    return NULL;
}

void ForceHookOpenGL(HMODULE base) { // to test .....
    HMODULE hGlLib;
    static int DoOnce = FALSE;
    if(DoOnce) return;
    DoOnce = TRUE;
    hGlLib = (*pLoadLibraryA)("OpenGL32.dll");
    OutTraceDW("ForceHookOpenGL: hGlLib=%#x\n", hGlLib);
    if(!hGlLib) {
        OutTraceE("LoadLibrary(\"OpenGL32.dll\") ERROR: err=%d at %d\n", GetLastError(), __LINE__);
        return;
    }
    int i;
    HookEntryEx_Type *Hook;
    for(i = 0; Hooks[i].APIName; i++) {
        Hook = &Hooks[i];
        Hook->OriginalAddress = GetProcAddress(hGlLib, Hook->APIName);
        if(Hook->OriginalAddress)
            HookAPI(base, "opengl32", Hook->StoreAddress, Hook->APIName, Hook->HookerAddress);
    }
    if(dxw.dwFlags9 & HOOKWGLCONTEXT) {
        for(i = 0; WiggieHooks[i].APIName; i++) {
            Hook = &WiggieHooks[i];
            Hook->OriginalAddress = GetProcAddress(hGlLib, Hook->APIName);
            if(Hook->OriginalAddress)
                HookAPI(base, "opengl32", Hook->StoreAddress, Hook->APIName, Hook->HookerAddress);
        }
    }
    return;
}

void HookOpenGL(HMODULE module) {
    if(!(dxw.dwFlags2 & HOOKOPENGL)) return;
    //customlib = SysLibsTable[SYSLIBIDX_OPENGL].name;
    char *customlib = "OpenGL32.dll"; // v2.04.66: the .dll suffix is REQUIRED!!
    // Beware: if not defined, the char *dxw.CustomOpenGLLib is set to NULL at initialization
    if (dxw.CustomOpenGLLib) customlib = dxw.CustomOpenGLLib;
    OutTraceDW("HookOpenGL module=%#x lib=\"%s\" forced=%#x\n", module, customlib, (dxw.dwFlags3 & FORCEHOOKOPENGL) ? 1 : 0);
    if (dxw.dwFlags3 & FORCEHOOKOPENGL)
        ForceHookOpenGL(module);
    else {
        HookLibraryEx(module, Hooks, customlib);
        if(dxw.dwFlags9 & HOOKWGLCONTEXT) HookLibraryEx(module, WiggieHooks, customlib);
    }
    if(dxw.dwFlags7 & HOOKGLUT32) HookLibraryEx(module, GlutHooks, "glut32.dll");
    if(dxw.dwFlags7 & HOOKGLUT32) HookLibraryEx(module, GluHooks, "glu32.dll");
    return;
}

#ifndef DXW_NOTRACES
static char *sglEnum(GLint t) {
    char *p;
    switch(t) {
    case GL_TEXTURE_1D:
        p = "GL_TEXTURE_1D";
        break;
    case GL_TEXTURE_2D:
        p = "GL_TEXTURE_2D";
        break;
    case GL_TEXTURE_3D:
        p = "GL_TEXTURE_3D";
        break;
    case GL_PROXY_TEXTURE_2D:
        p = "GL_PROXY_TEXTURE_2D";
        break;
    case GL_TEXTURE_1D_ARRAY:
        p = "GL_TEXTURE_1D_ARRAY";
        break;
    case GL_PROXY_TEXTURE_1D_ARRAY:
        p = "GL_PROXY_TEXTURE_1D_ARRAY";
        break;
    case GL_TEXTURE_RECTANGLE:
        p = "GL_TEXTURE_RECTANGLE";
        break;
    case GL_PROXY_TEXTURE_RECTANGLE:
        p = "GL_PROXY_TEXTURE_RECTANGLE";
        break;
    case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
        p = "GL_TEXTURE_CUBE_MAP_POSITIVE_X";
        break;
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
        p = "GL_TEXTURE_CUBE_MAP_NEGATIVE_X";
        break;
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
        p = "GL_TEXTURE_CUBE_MAP_POSITIVE_Y";
        break;
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
        p = "GL_TEXTURE_CUBE_MAP_NEGATIVE_Y";
        break;
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
        p = "GL_TEXTURE_CUBE_MAP_POSITIVE_Z";
        break;
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
        p = "GL_TEXTURE_CUBE_MAP_NEGATIVE_Z";
        break;
    case GL_TEXTURE_CUBE_MAP:
        p = "GL_TEXTURE_CUBE_MAP";
        break;
    case GL_RGB4:
        p = "GL_RGB4";
        break;
    case GL_RGB5:
        p = "GL_RGB5";
        break;
    case GL_RGB8:
        p = "GL_RGB8";
        break;
    case GL_RGB10:
        p = "GL_RGB10";
        break;
    case GL_RGB12:
        p = "GL_RGB12";
        break;
    case GL_RGB16:
        p = "GL_RGB16";
        break;
    case GL_RGBA2:
        p = "GL_RGBA2";
        break;
    case GL_RGBA4:
        p = "GL_RGBA4";
        break;
    case GL_RGB5_A1:
        p = "GL_RGB5_A1";
        break;
    case GL_RGBA8:
        p = "GL_RGBA8";
        break;
    case GL_RGB10_A2:
        p = "GL_RGB10_A2";
        break;
    case GL_RGBA12:
        p = "GL_RGBA12";
        break;
    case GL_RGBA16:
        p = "GL_RGBA16";
        break;
    case GL_TEXTURE_MIN_FILTER:
        p = "GL_TEXTURE_MIN_FILTER";
        break;
    case GL_TEXTURE_MAG_FILTER:
        p = "GL_TEXTURE_MAG_FILTER";
        break;
    case GL_TEXTURE_WRAP_S:
        p = "GL_TEXTURE_WRAP_S";
        break;
    case GL_TEXTURE_WRAP_T:
        p = "GL_TEXTURE_WRAP_T";
        break;
    case GL_TEXTURE_ENV_MODE:
        p = "GL_TEXTURE_ENV_MODE";
        break;
    case GL_ALPHA_TEST:
        p = "GL_ALPHA_TEST";
        break;
    case GL_AUTO_NORMAL:
        p = "GL_AUTO_NORMAL";
        break;
    case GL_BLEND:
        p = "GL_BLEND";
        break;
    case GL_CLIP_PLANE0:
        p = "GL_CLIP_PLANE0";
        break;
    case GL_CLIP_PLANE1:
        p = "GL_CLIP_PLANE1";
        break;
    case GL_CLIP_PLANE2:
        p = "GL_CLIP_PLANE2";
        break;
    case GL_CLIP_PLANE3:
        p = "GL_CLIP_PLANE3";
        break;
    case GL_CLIP_PLANE4:
        p = "GL_CLIP_PLANE4";
        break;
    case GL_CLIP_PLANE5:
        p = "GL_CLIP_PLANE5";
        break;
    case GL_COLOR_LOGIC_OP:
        p = "GL_COLOR_LOGIC_OP";
        break;
    case GL_COLOR_SUM:
        p = "GL_COLOR_SUM";
        break;
    case GL_COLOR_TABLE:
        p = "GL_COLOR_TABLE";
        break;
    case GL_CONVOLUTION_1D:
        p = "GL_CONVOLUTION_1D";
        break;
    case GL_CONVOLUTION_2D:
        p = "GL_CONVOLUTION_2D";
        break;
    case GL_DEPTH_TEST:
        p = "GL_DEPTH_TEST";
        break;
    case GL_DITHER:
        p = "GL_DITHER";
        break;
    case GL_FOG:
        p = "GL_FOG";
        break;
    case GL_HISTOGRAM:
        p = "GL_HISTOGRAM";
        break;
    case GL_INDEX_LOGIC_OP:
        p = "GL_INDEX_LOGIC_OP";
        break;
    case GL_LINE_SMOOTH:
        p = "GL_LINE_SMOOTH";
        break;
    case GL_LINE_STIPPLE:
        p = "GL_LINE_STIPPLE";
        break;
    case GL_MAP1_COLOR_4:
        p = "GL_MAP1_COLOR_4";
        break;
    case GL_MAP1_INDEX:
        p = "GL_MAP1_INDEX";
        break;
    case GL_MAP1_NORMAL:
        p = "GL_MAP1_NORMAL";
        break;
    case GL_MAP1_TEXTURE_COORD_1:
        p = "GL_MAP1_TEXTURE_COORD_1";
        break;
    case GL_MAP1_TEXTURE_COORD_2:
        p = "GL_MAP1_TEXTURE_COORD_2";
        break;
    case GL_MAP1_TEXTURE_COORD_3:
        p = "GL_MAP1_TEXTURE_COORD_3";
        break;
    case GL_MAP1_TEXTURE_COORD_4:
        p = "GL_MAP1_TEXTURE_COORD_4";
        break;
    case GL_MAP1_VERTEX_3:
        p = "GL_MAP1_VERTEX_3";
        break;
    case GL_MAP1_VERTEX_4:
        p = "GL_MAP1_VERTEX_4";
        break;
    case GL_MAP2_COLOR_4:
        p = "GL_MAP2_COLOR_4";
        break;
    case GL_MAP2_INDEX:
        p = "GL_MAP2_INDEX";
        break;
    case GL_MAP2_NORMAL:
        p = "GL_MAP2_NORMAL";
        break;
    case GL_MAP2_TEXTURE_COORD_1:
        p = "GL_MAP2_TEXTURE_COORD_1";
        break;
    case GL_MAP2_TEXTURE_COORD_2:
        p = "GL_MAP2_TEXTURE_COORD_2";
        break;
    case GL_MAP2_TEXTURE_COORD_3:
        p = "GL_MAP2_TEXTURE_COORD_3";
        break;
    case GL_MAP2_TEXTURE_COORD_4:
        p = "GL_MAP2_TEXTURE_COORD_4";
        break;
    case GL_MAP2_VERTEX_3:
        p = "GL_MAP2_VERTEX_3";
        break;
    case GL_MAP2_VERTEX_4:
        p = "GL_MAP2_VERTEX_4";
        break;
    case GL_MINMAX:
        p = "GL_MINMAX";
        break;
    case GL_MULTISAMPLE:
        p = "GL_MULTISAMPLE";
        break;
    case GL_POINT_SMOOTH:
        p = "GL_POINT_SMOOTH";
        break;
    case GL_POINT_SPRITE:
        p = "GL_POINT_SPRITE";
        break;
    case GL_POST_COLOR_MATRIX_COLOR_TABLE:
        p = "GL_POST_COLOR_MATRIX_COLOR_TABLE";
        break;
    case GL_POST_CONVOLUTION_COLOR_TABLE:
        p = "GL_POST_CONVOLUTION_COLOR_TABLE";
        break;
    case GL_RESCALE_NORMAL:
        p = "GL_RESCALE_NORMAL";
        break;
    case GL_SAMPLE_ALPHA_TO_COVERAGE:
        p = "GL_SAMPLE_ALPHA_TO_COVERAGE";
        break;
    case GL_SAMPLE_ALPHA_TO_ONE:
        p = "GL_SAMPLE_ALPHA_TO_ONE";
        break;
    case GL_SAMPLE_COVERAGE:
        p = "GL_SAMPLE_COVERAGE";
        break;
    case GL_SEPARABLE_2D:
        p = "GL_SEPARABLE_2D";
        break;
    case GL_SCISSOR_TEST:
        p = "GL_SCISSOR_TEST";
        break;
    case GL_STENCIL_TEST:
        p = "GL_STENCIL_TEST";
        break;
    case GL_TEXTURE_GEN_Q:
        p = "GL_TEXTURE_GEN_Q";
        break;
    case GL_TEXTURE_GEN_R:
        p = "GL_TEXTURE_GEN_R";
        break;
    case GL_TEXTURE_GEN_S:
        p = "GL_TEXTURE_GEN_S";
        break;
    case GL_TEXTURE_GEN_T:
        p = "GL_TEXTURE_GEN_T";
        break;
    case GL_VERTEX_PROGRAM_POINT_SIZE:
        p = "GL_VERTEX_PROGRAM_POINT_SIZE";
        break;
    case GL_VERTEX_PROGRAM_TWO_SIDE:
        p = "GL_VERTEX_PROGRAM_TWO_SIDE";
        break;
    /* Matrix Mode */
    case GL_MATRIX_MODE:
        p = "GL_MATRIX_MODE";
        break;
    case GL_MODELVIEW:
        p = "GL_MODELVIEW";
        break;
    case GL_PROJECTION:
        p = "GL_PROJECTION";
        break;
    case GL_TEXTURE:
        p = "GL_TEXTURE";
        break;
    /* Hints */
    case GL_FOG_HINT:
        p = "GL_FOG_HINT";
        break;
    case GL_LINE_SMOOTH_HINT:
        p = "GL_LINE_SMOOTH_HINT";
        break;
    case GL_PERSPECTIVE_CORRECTION_HINT:
        p = "GL_PERSPECTIVE_CORRECTION_HINT";
        break;
    case GL_POINT_SMOOTH_HINT:
        p = "GL_POINT_SMOOTH_HINT";
        break;
    case GL_POLYGON_SMOOTH_HINT:
        p = "GL_POLYGON_SMOOTH_HINT";
        break;
    case GL_DONT_CARE:
        p = "GL_DONT_CARE";
        break;
    case GL_FASTEST:
        p = "GL_FASTEST";
        break;
    case GL_NICEST:
        p = "GL_NICEST";
        break;
    /* Polygons */
    case GL_POINT:
        p = "GL_POINT";
        break;
    case GL_LINE:
        p = "GL_LINE";
        break;
    case GL_FILL:
        p = "GL_FILL";
        break;
    case GL_CW:
        p = "GL_CW";
        break;
    case GL_CCW:
        p = "GL_CCW";
        break;
    case GL_FRONT:
        p = "GL_FRONT";
        break;
    case GL_BACK:
        p = "GL_BACK";
        break;
    case GL_POLYGON_MODE:
        p = "GL_POLYGON_MODE";
        break;
    case GL_POLYGON_SMOOTH:
        p = "GL_POLYGON_SMOOTH";
        break;
    case GL_POLYGON_STIPPLE:
        p = "GL_POLYGON_STIPPLE";
        break;
    case GL_EDGE_FLAG:
        p = "GL_EDGE_FLAG";
        break;
    case GL_CULL_FACE:
        p = "GL_CULL_FACE";
        break;
    case GL_CULL_FACE_MODE:
        p = "GL_CULL_FACE_MODE";
        break;
    case GL_FRONT_FACE:
        p = "GL_FRONT_FACE";
        break;
    case GL_POLYGON_OFFSET_FACTOR:
        p = "GL_POLYGON_OFFSET_FACTOR";
        break;
    case GL_POLYGON_OFFSET_UNITS:
        p = "GL_POLYGON_OFFSET_UNITS";
        break;
    case GL_POLYGON_OFFSET_POINT:
        p = "GL_POLYGON_OFFSET_POINT";
        break;
    case GL_POLYGON_OFFSET_LINE:
        p = "GL_POLYGON_OFFSET_LINE";
        break;
    case GL_POLYGON_OFFSET_FILL:
        p = "GL_POLYGON_OFFSET_FILL";
        break;
    /* Vertex Arrays */
    case GL_VERTEX_ARRAY:
        p = "GL_VERTEX_ARRAY";
        break;
    case GL_NORMAL_ARRAY:
        p = "GL_NORMAL_ARRAY";
        break;
    case GL_COLOR_ARRAY:
        p = "GL_COLOR_ARRAY";
        break;
    case GL_INDEX_ARRAY:
        p = "GL_INDEX_ARRAY";
        break;
    case GL_TEXTURE_COORD_ARRAY:
        p = "GL_TEXTURE_COORD_ARRAY";
        break;
    case GL_EDGE_FLAG_ARRAY:
        p = "GL_EDGE_FLAG_ARRAY";
        break;
    case GL_VERTEX_ARRAY_SIZE:
        p = "GL_VERTEX_ARRAY_SIZE";
        break;
    case GL_VERTEX_ARRAY_TYPE:
        p = "GL_VERTEX_ARRAY_TYPE";
        break;
    case GL_VERTEX_ARRAY_STRIDE:
        p = "GL_VERTEX_ARRAY_STRIDE";
        break;
    case GL_NORMAL_ARRAY_TYPE:
        p = "GL_NORMAL_ARRAY_TYPE";
        break;
    case GL_NORMAL_ARRAY_STRIDE:
        p = "GL_NORMAL_ARRAY_STRIDE";
        break;
    case GL_COLOR_ARRAY_SIZE:
        p = "GL_COLOR_ARRAY_SIZE";
        break;
    case GL_COLOR_ARRAY_TYPE:
        p = "GL_COLOR_ARRAY_TYPE";
        break;
    case GL_COLOR_ARRAY_STRIDE:
        p = "GL_COLOR_ARRAY_STRIDE";
        break;
    case GL_INDEX_ARRAY_TYPE:
        p = "GL_INDEX_ARRAY_TYPE";
        break;
    case GL_INDEX_ARRAY_STRIDE:
        p = "GL_INDEX_ARRAY_STRIDE";
        break;
    case GL_TEXTURE_COORD_ARRAY_SIZE:
        p = "GL_TEXTURE_COORD_ARRAY_SIZE";
        break;
    case GL_TEXTURE_COORD_ARRAY_TYPE:
        p = "GL_TEXTURE_COORD_ARRAY_TYPE";
        break;
    case GL_TEXTURE_COORD_ARRAY_STRIDE:
        p = "GL_TEXTURE_COORD_ARRAY_STRIDE";
        break;
    case GL_EDGE_FLAG_ARRAY_STRIDE:
        p = "GL_EDGE_FLAG_ARRAY_STRIDE";
        break;
    case GL_VERTEX_ARRAY_POINTER:
        p = "GL_VERTEX_ARRAY_POINTER";
        break;
    case GL_NORMAL_ARRAY_POINTER:
        p = "GL_NORMAL_ARRAY_POINTER";
        break;
    case GL_COLOR_ARRAY_POINTER:
        p = "GL_COLOR_ARRAY_POINTER";
        break;
    case GL_INDEX_ARRAY_POINTER:
        p = "GL_INDEX_ARRAY_POINTER";
        break;
    case GL_TEXTURE_COORD_ARRAY_POINTER:
        p = "GL_TEXTURE_COORD_ARRAY_POINTER";
        break;
    case GL_EDGE_FLAG_ARRAY_POINTER:
        p = "GL_EDGE_FLAG_ARRAY_POINTER";
        break;
    case GL_V2F:
        p = "GL_V2F";
        break;
    case GL_V3F:
        p = "GL_V3F";
        break;
    case GL_C4UB_V2F:
        p = "GL_C4UB_V2F";
        break;
    case GL_C4UB_V3F:
        p = "GL_C4UB_V3F";
        break;
    case GL_C3F_V3F:
        p = "GL_C3F_V3F";
        break;
    case GL_N3F_V3F:
        p = "GL_N3F_V3F";
        break;
    case GL_C4F_N3F_V3F:
        p = "GL_C4F_N3F_V3F";
        break;
    case GL_T2F_V3F:
        p = "GL_T2F_V3F";
        break;
    case GL_T4F_V4F:
        p = "GL_T4F_V4F";
        break;
    case GL_T2F_C4UB_V3F:
        p = "GL_T2F_C4UB_V3F";
        break;
    case GL_T2F_C3F_V3F:
        p = "GL_T2F_C3F_V3F";
        break;
    case GL_T2F_N3F_V3F:
        p = "GL_T2F_N3F_V3F";
        break;
    case GL_T2F_C4F_N3F_V3F:
        p = "GL_T2F_C4F_N3F_V3F";
        break;
    case GL_T4F_C4F_N3F_V4F:
        p = "GL_T4F_C4F_N3F_V4F";
        break;
    /* Lighting */
    case GL_LIGHTING:
        p = "GL_LIGHTING";
        break;
    case GL_LIGHT0:
        p = "GL_LIGHT0";
        break;
    case GL_LIGHT1:
        p = "GL_LIGHT1";
        break;
    case GL_LIGHT2:
        p = "GL_LIGHT2";
        break;
    case GL_LIGHT3:
        p = "GL_LIGHT3";
        break;
    case GL_LIGHT4:
        p = "GL_LIGHT4";
        break;
    case GL_LIGHT5:
        p = "GL_LIGHT5";
        break;
    case GL_LIGHT6:
        p = "GL_LIGHT6";
        break;
    case GL_LIGHT7:
        p = "GL_LIGHT7";
        break;
    case GL_SPOT_EXPONENT:
        p = "GL_SPOT_EXPONENT";
        break;
    case GL_SPOT_CUTOFF:
        p = "GL_SPOT_CUTOFF";
        break;
    case GL_CONSTANT_ATTENUATION:
        p = "GL_CONSTANT_ATTENUATION";
        break;
    case GL_LINEAR_ATTENUATION:
        p = "GL_LINEAR_ATTENUATION";
        break;
    case GL_QUADRATIC_ATTENUATION:
        p = "GL_QUADRATIC_ATTENUATION";
        break;
    case GL_AMBIENT:
        p = "GL_AMBIENT";
        break;
    case GL_DIFFUSE:
        p = "GL_DIFFUSE";
        break;
    case GL_SPECULAR:
        p = "GL_SPECULAR";
        break;
    case GL_SHININESS:
        p = "GL_SHININESS";
        break;
    case GL_EMISSION:
        p = "GL_EMISSION";
        break;
    case GL_POSITION:
        p = "GL_POSITION";
        break;
    case GL_SPOT_DIRECTION:
        p = "GL_SPOT_DIRECTION";
        break;
    case GL_AMBIENT_AND_DIFFUSE:
        p = "GL_AMBIENT_AND_DIFFUSE";
        break;
    case GL_COLOR_INDEXES:
        p = "GL_COLOR_INDEXES";
        break;
    case GL_LIGHT_MODEL_TWO_SIDE:
        p = "GL_LIGHT_MODEL_TWO_SIDE";
        break;
    case GL_LIGHT_MODEL_LOCAL_VIEWER:
        p = "GL_LIGHT_MODEL_LOCAL_VIEWER";
        break;
    case GL_LIGHT_MODEL_AMBIENT:
        p = "GL_LIGHT_MODEL_AMBIENT";
        break;
    case GL_FRONT_AND_BACK:
        p = "GL_FRONT_AND_BACK";
        break;
    case GL_SHADE_MODEL:
        p = "GL_SHADE_MODEL";
        break;
    case GL_FLAT:
        p = "GL_FLAT";
        break;
    case GL_SMOOTH:
        p = "GL_SMOOTH";
        break;
    case GL_COLOR_MATERIAL:
        p = "GL_COLOR_MATERIAL";
        break;
    case GL_COLOR_MATERIAL_FACE:
        p = "GL_COLOR_MATERIAL_FACE";
        break;
    case GL_COLOR_MATERIAL_PARAMETER:
        p = "GL_COLOR_MATERIAL_PARAMETER";
        break;
    case GL_NORMALIZE:
        p = "GL_NORMALIZE";
        break;
    /* unknown */
    default:
        p = "unknown";
        break;
    }
    return p;
}
#endif // DXW_NOTRACES

GLenum WINAPI extglGetError() {
    // to avoid dependencies on opengl32.dll
    if (pglGetError) return (*pglGetError)();
    return GL_NO_ERROR;
}

void WINAPI extglViewport(GLint  x,  GLint  y,  GLsizei  width,  GLsizei  height) {
    ApiName("glViewport");
    OutTraceOGL("%s: pos=(%d,%d) size=(%d,%d)\n", ApiRef, x, y, width, height);
    BOOL bViewportBypass;
    if(!(dxw.dwFlags11 & SCALEMAINVIEWPORT))
        bViewportBypass = TRUE;
    else {
        bViewportBypass = (width == (int)dxw.GetScreenWidth()) && (height == (int)dxw.GetScreenHeight());
        OutTraceDW("%s: BYPASS=%#x\n", ApiRef, bViewportBypass);
    }
    if(dxw.Windowize && bViewportBypass) {
        if(x == CW_USEDEFAULT) x = 0;
        if(y == CW_USEDEFAULT) y = 0;
        // v2.04.89: casted to int type to handle negative x,y values like in SW:KOTOR
        x = (x * (int)dxw.iSizX) / (int)dxw.GetScreenWidth();
        y = (y * (int)dxw.iSizY) / (int)dxw.GetScreenHeight();
        width = (width * (int)dxw.iSizX) / (int)dxw.GetScreenWidth();
        height = (height * (int)dxw.iSizY) / (int)dxw.GetScreenHeight();
        OutTraceDW("%s: remapped pos=(%d,%d) size=(%d,%d)\n", ApiRef, x, y, width, height);
    }
    (*pglViewport)(x, y, width, height);
}

#ifdef TRACEALL
void WINAPI extglViewportArrayv(GLuint first, GLsizei count, const GLfloat *v) {
    ApiName("glViewportArrayv");
    OutTraceOGL("%s: first=%d count=%d\n", ApiRef, first, count);
    (*pglViewportArrayv)(first, count, v);
}

void WINAPI extglViewportIndexedf(GLuint index, GLfloat x, GLfloat y, GLfloat w, GLfloat h) {
    ApiName("glViewportIndexedf");
    OutTraceOGL("%s: index=%d x=%f y=%f w=%f h=%f\n", ApiRef, index, x, y, w, h);
    (*pglViewportIndexedf)(index, x, y, w, h);
}

void WINAPI extglViewportIndexedfv(GLuint index, const GLfloat *v) {
    ApiName("glViewportIndexedfv");
    OutTraceOGL("%s: index=%d x=%f y=%f w=%f h=%f\n", ApiRef, index, v[0], v[1], v[2], v[3]);
    (*pglViewportIndexedfv)(index, v);
}
#endif

void WINAPI extglScissor(GLint  x,  GLint  y,  GLsizei  width,  GLsizei  height) {
    ApiName("glScissor");
    OutTraceOGL("%s: pos=(%d,%d) size=(%d,%d)\n", ApiRef, x, y, width, height);
    if(dxw.Windowize) {
        // v2.04.89: casted to int type to handle negative x,y values like in SW:KOTOR
        x = (x * (int)dxw.iSizX) / (int)dxw.GetScreenWidth();
        y = (y * (int)dxw.iSizY) / (int)dxw.GetScreenHeight();
        width = (width * (int)dxw.iSizX) / (int)dxw.GetScreenWidth();
        height = (height * (int)dxw.iSizY) / (int)dxw.GetScreenHeight();
        OutTraceDW("%s: remapped pos=(%d,%d) size=(%d,%d)\n", ApiRef, x, y, width, height);
    }
    (*pglScissor)(x, y, width, height);
    if((dxw.dwFlags12 & LOCKGLVIEWPORT) && pglViewport) {
        OutTraceDW("%s: SYNC glViewport pos=(%d,%d) size=(%d,%d)\n", ApiRef, 0, 0, dxw.iSiz0X, dxw.iSiz0Y);
        (*pglViewport)(0, 0, dxw.iSiz0X, dxw.iSiz0Y);
    }
    if(dxw.dwFlags12 & PROJECTBUFFER) dxw.Project();
}

#ifdef TRACEALL
void WINAPI extglScissorArrayv(GLuint first, GLsizei count, const GLfloat *v) {
    ApiName("glScissorArrayv");
    OutTraceOGL("%s: first=%d count=%d\n", ApiRef, first, count);
    (*pglScissorArrayv)(first, count, v);
}

void WINAPI extglScissorIndexed(GLuint index, GLint x, GLint y, GLsizei w, GLsizei h) {
    ApiName("glScissorIndexedf");
    OutTraceOGL("%s: index=%d x=%d y=%d w=%d h=%d\n", ApiRef, index, x, y, w, h);
    (*pglScissorIndexed)(index, x, y, w, h);
}

void WINAPI extglScissorIndexedf(GLuint index, GLfloat x, GLfloat y, GLfloat w, GLfloat h) {
    ApiName("glScissorIndexedf");
    OutTraceOGL("%s: index=%d x=%f y=%f w=%f h=%f\n", ApiRef, index, x, y, w, h);
    (*pglScissorIndexedf)(index, x, y, w, h);
}

void WINAPI extglScissorIndexedv(GLuint index, const GLint *v) {
    ApiName("glScissorIndexedv");
    OutTraceOGL("%s: index=%d x=%d y=%d w=%d h=%d\n", ApiRef, index, v[0], v[1], v[2], v[3]);
    (*pglScissorIndexedv)(index, v);
}

void WINAPI extglScissorIndexedfv(GLuint index, const GLfloat *v) {
    ApiName("glScissorIndexedfv");
    OutTraceOGL("%s: index=%d x=%f y=%f w=%f h=%f\n", ApiRef, index, v[0], v[1], v[2], v[3]);
    (*pglScissorIndexedfv)(index, v);
}
#endif

#ifdef TRACEALL
void WINAPI extglWindowPos2s(GLshort x, GLshort y) {
    ApiName("glWindowPos2s");
    OutTraceOGL("%s: x=%d y=%d\n", ApiRef, x, y);
    (*pglWindowPos2s)(x, y);
}

void WINAPI extglWindowPos2i(GLint x, GLint y) {
    ApiName("glWindowPos2i");
    OutTraceOGL("%s: x=%d y=%d\n", ApiRef, x, y);
    (*pglWindowPos2i)(x, y);
}

void WINAPI extglWindowPos2f(GLfloat x, GLfloat y) {
    ApiName("glWindowPos2f");
    OutTraceOGL("%s: x=%f y=%f\n", ApiRef, x, y);
    (*pglWindowPos2f)(x, y);
}

/*
    void WINAPI extglWindowPos2d(GLdouble x, GLdouble y)

    void WINAPI extglWindowPos3s(GLshort x, GLshort y, GLshort z)

    void WINAPI extglWindowPos3i(GLint x, GLint y, GLint z)

    void WINAPI extglWindowPos3f(GLfloat x, GLfloat y, GLfloat z)

    void WINAPI extglWindowPos3d(GLdouble x, GLdouble y, GLdouble z)
*/
#endif

void WINAPI extglGetIntegerv(GLenum pname, GLint *params) {
    (*pglGetIntegerv)(pname, params);
    OutDebugOGL("glGetIntegerv: pname=%#x\n", pname);
    if(pname == GL_VIEWPORT) {
        BOOL bViewportBypass;
        if(!(dxw.dwFlags11 & SCALEMAINVIEWPORT))
            bViewportBypass = TRUE;
        else {
            bViewportBypass = (
                                  (params[0] == 0) &&
                                  (params[1] == 0) &&
                                  (params[2] == dxw.iSizX) &&
                                  (params[3] == dxw.iSizY));
            OutTraceDW("glGetIntegerv(GL_VIEWPORT): BYPASS=%#x\n", bViewportBypass);
        }
        if(dxw.Windowize && bViewportBypass) {
            OutTraceDW("glGetIntegerv(GL_VIEWPORT): pos=(%i,%i) siz=(%i,%i)\n", params[0], params[1], params[2], params[3]);
            if(dxw.iSizX && dxw.iSizY) {
                // v2.04.89: casted to int type to handle negative x,y values like in SW:KOTOR
                params[0] = (params[0] * (int)dxw.GetScreenWidth()) / (int)dxw.iSizX;
                params[1] = (params[1] * (int)dxw.GetScreenHeight()) / (int)dxw.iSizY;
                params[2] = (params[2] * (int)dxw.GetScreenWidth()) / (int)dxw.iSizX;
                params[3] = (params[3] * (int)dxw.GetScreenHeight()) / (int)dxw.iSizY;
            }
            OutTraceDW("glGetIntegerv(GL_VIEWPORT): FIXED pos=(%i,%i) siz=(%i,%i)\n", params[0], params[1], params[2], params[3]);
        }
    }
}

void WINAPI extglDrawBuffer(GLenum mode) {
    OutDebugOGL("glDrawBuffer: mode=%#x\n", mode);
    if(dxw.dwFlags2 & WIREFRAME) (*pglClear)(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT) ; // clear screen for wireframe mode....
    // handle FPS only to backbuffer updates (if stereo, on left backbuffer...)
    // using the frontbuffer seems less reliable: Return to Castle Wolfenstein doesn't use it at all!
    if (dxw.dwFlags2 & HANDLEFPS) {
        switch (mode) {
        //case GL_FRONT_LEFT:
        case GL_BACK_LEFT:
        //case GL_FRONT:
        case GL_BACK:
        case GL_LEFT:
        case GL_FRONT_AND_BACK:
            if(dxw.HandleFPS()) return;
        }
    }
    (*pglDrawBuffer)(mode);
    dxw.ShowOverlay();
}

void WINAPI extglPolygonMode(GLenum face, GLenum mode) {
    OutTraceOGL("glPolygonMode: face=%#x mode=%#x\n", face, mode);
    //OutTraceDW("glPolygonMode: extglPolygonMode=%#x pglPolygonMode=%#x\n", extglPolygonMode, pglPolygonMode);
    if(dxw.dwFlags2 & WIREFRAME) {
        OutTraceDW("glPolygonMode: WIREFRAME forcind mode=GL_LINE\n");
        mode = GL_LINE; // trick to set wireframe mode....
    }
    (*pglPolygonMode)(face, mode);
    return;
}

void WINAPI extglGetFloatv(GLenum pname, GLboolean *params) {
    OutTraceOGL("glGetFloatv: pname=%#x\n", pname);
    (*pglGetFloatv)(pname, params);
    return;
}

void WINAPI extglClear(GLbitfield mask) {
    (*pglClear)(mask);
    return;
}

//BEWARE: SetPixelFormat must be issued on the same hdc used by OpenGL wglCreateContext, otherwise
// a failure err=2000 ERROR INVALID PIXEL FORMAT occurs!!

HGLRC WINAPI extwglCreateContext(HDC hdc) {
    HGLRC ret;
    OutTraceOGL("wglCreateContext: hdc=%#x\n", hdc);
    BOOL bRemappedDC = FALSE;
    // v2.02.31: don't let it use desktop hdc
    if(dxw.Windowize && dxw.IsRealDesktop(WindowFromDC(hdc))) {
        HDC oldhdc = hdc;
        hdc = (*pGDIGetDC)(dxw.GethWnd());
        bRemappedDC = TRUE;
        OutTraceDW("wglCreateContext: remapped desktop hdc=%#x->%#x hWnd=%#x\n", oldhdc, hdc, dxw.GethWnd());
    }
    // v2.04.73: FORCECLIPCHILDREN on wgl context
    if(dxw.dwFlags4 & FORCECLIPCHILDREN) {
        HWND hwnd = WindowFromDC(hdc);
        LONG dwStyle = (*pGetWindowLong)(hwnd, GWL_STYLE);
        if(!(dwStyle & WS_CLIPCHILDREN)) {
            OutTraceDW("wglCreateContext: fixed style +WS_CLIPCHILDREN hdc=%#x hwnd=%#x style=%#x(%s)\n",
                       hdc, hwnd, dwStyle, ExplainStyle(dwStyle));
            (*pSetWindowLong)(hwnd, GWL_STYLE, dwStyle | WS_CLIPCHILDREN);
        }
    }
    // v2.05.23: added wglCreateContext in emulation mode, doesn't apply the selected pixel format
    // but adapts to the current one.
    if(dxw.IsEmulated) {
        int kount = 0;
        PIXELFORMATDESCRIPTOR pfd;
        int iPixelFormat;
        do {
            kount++;
            iPixelFormat = ChoosePixelFormat(hdc, &pfd);
            OutTraceOGL("wglCreateContext: pixel format = %d\n", iPixelFormat);
            //if(!(*pGDISetPixelFormat)(hdc, iPixelFormat, &pfd)){
            if(!SetPixelFormat(hdc, iPixelFormat, &pfd))
                OutTraceOGL("wglCreateContext: SetPixelFormat failed err=%d at=%d\n", GetLastError(), __LINE__);
            ret = (*pwglCreateContext)(hdc);
            if(!ret && (kount > 5)) {
                OutTraceOGL("wglCreateContext: ERROR glCreateContext failed err=%d at=%d\n", GetLastError(), __LINE__);
                break;
            }
            if(ret) break;
            Sleep(20);
        } while(TRUE);
    } else
        ret = (*pwglCreateContext)(hdc);
#ifndef DXW_NOTRACES
    if(!ret) OutTraceE("wglCreateContext: ERROR err=%d\n", GetLastError());
#endif // DXW_NOTRACES
    if(bRemappedDC) (*pGDIReleaseDC)(dxw.GethWnd(), hdc); // v2.04.97: fixed DC leakage
    return ret;
}

HGLRC WINAPI extwglCreateLayerContext(HDC hdc, int iLayerPlane) {
    HGLRC ret;
    BOOL bRemappedDC = FALSE;
    OutTraceOGL("wglCreateLayerContext: hdc=%#x layer=%d\n", hdc, iLayerPlane);
    // v2.02.31: don't let it use desktop hdc
    if(dxw.Windowize && dxw.IsRealDesktop(WindowFromDC(hdc))) {
        HDC oldhdc = hdc;
        hdc = (*pGDIGetDC)(dxw.GethWnd());
        bRemappedDC = TRUE;
        OutTraceDW("wglCreateContext: remapped desktop hdc=%#x->%#x hWnd=%#x\n", oldhdc, hdc, dxw.GethWnd());
    }
    ret = (*pwglCreateLayerContext)(hdc, iLayerPlane);
#ifndef DXW_NOTRACES
    if(!ret) OutTraceE("wglCreateLayerContext: ERROR err=%d\n", GetLastError());
#endif // DXW_NOTRACES
    if(bRemappedDC) (*pGDIReleaseDC)(dxw.GethWnd(), hdc); // v2.04.97: fixed DC leakage
    return ret;
}

BOOL WINAPI extwglDeleteContext(HGLRC hglrc) {
    BOOL ret;
    OutTraceOGL("wglDeleteContext: hglrc=%#x\n", hglrc);
    ret = (*pwglDeleteContext)(hglrc);
#ifndef DXW_NOTRACES
    if(!ret) OutTraceE("wglDeleteContext: ERROR err=%d\n", GetLastError());
#endif // DXW_NOTRACES
    return ret;
}

HGLRC WINAPI extwglGetCurrentContext(void) {
    HGLRC ret;
    ret = (*pwglGetCurrentContext)();
    OutTraceOGL("wglGetCurrentContext: hglrc=%#x\n", ret);
    return ret;
}

BOOL WINAPI extwglCopyContext(HGLRC hglrcSrc, HGLRC hglrcDst, UINT mask) {
    BOOL ret;
    ApiName("wglCopyContext");
    OutTraceOGL("%s: src=%#x dst=%#x mask=%#x\n", ApiRef, hglrcSrc, hglrcDst, mask);
    ret = (*pwglCopyContext)(hglrcSrc, hglrcDst, mask);
#ifndef DXW_NOTRACES
    if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
#endif // DXW_NOTRACES
    return ret;
}

PROC WINAPI extwglGetProcAddress(LPCSTR proc) {
    PROC procaddr;
    OutTraceOGL("wglGetProcAddress: proc=%s\n", proc);
    procaddr = Remap_wgl_ProcAddress(proc);
    if (!procaddr) procaddr = (*pwglGetProcAddress)(proc);
    return procaddr;
}

BOOL WINAPI extwglMakeCurrent(HDC hdc, HGLRC hglrc) {
    BOOL ret;
    BOOL bRemappedDC = FALSE;
    OutTraceOGL("wglMakeCurrent: hdc=%#x hglrc=%#x\n", hdc, hglrc);
    // v2.02.31: don't let it use desktop hdc
    if(dxw.Windowize && dxw.IsRealDesktop(WindowFromDC(hdc))) {
        HDC oldhdc = hdc;
        hdc = (*pGDIGetDC)(dxw.GethWnd());
        bRemappedDC = TRUE;
        OutTraceDW("wglMakeCurrent: remapped desktop hdc=%#x->%#x\n", oldhdc, hdc);
    }
    ret = (*pwglMakeCurrent)(hdc, hglrc);
    // v2.05.23: it seems that OpenGL/wglMakeCurrent doesn't like the hdc if it has a clipping
    // region set. In this case, you can try clearing the clipper. Helps in "Sid Meier's SimGolf".
    if(!ret && (ERROR_CLIPPING_NOT_SUPPORTED == GetLastError())) {
        (*pSelectClipRgn)(hdc, NULL);
        ret = (*pwglMakeCurrent)(hdc, hglrc);
    }
    if(ret) {
        HWND hWnd;
        hWnd = WindowFromDC(hdc);
        DWORD dwStyle = (*pGetWindowLong)(hWnd, GWL_STYLE);
        if((hWnd != dxw.GethWnd()) && !((dwStyle & WS_CHILD))) {
            OutTraceDW("wglMakeCurrent: setting hwnd=%#x\n", hWnd);
            dxw.SethWnd(hWnd);
            // v2.04.28 addition: fixes "18 Wheels of Steel Across America" window positioning
            // v2.04.34: moved from wglCreateContext wrapper to here
            if (hWnd && dxw.Windowize && dxw.IsFullScreen()) {
                dxw.FixWindowFrame(hWnd);
                dxw.AdjustWindowPos(hWnd, dxw.iSizX, dxw.iSizY);
            }
            // v2.04.28, v2.04.34 end
        }
    } else
        OutTraceE("wglMakeCurrent: ERROR err=%d\n", GetLastError());
    if(bRemappedDC) (*pGDIReleaseDC)(dxw.GethWnd(), hdc); // v2.04.97: fixed DC leakage
    return ret;
}

static unsigned int Hash(BYTE *buf, int len) {
    unsigned int b    = 378551;
    unsigned int a    = 63689;
    DWORD hash = 0;
    for(int i = 0; i < len; i++) {
        hash = hash * a + buf[i];
        a    = a * b;
    }
    return hash;
}

static void glTextureDump(GLint internalFormat, GLenum Format, GLsizei w, GLsizei h, GLenum type, const GLvoid *data) {
    int iSurfaceSize, iScanLineSize;
    static int MinTexX, MinTexY, MaxTexX, MaxTexY;
    static BOOL DoOnce = TRUE;
    char pszFile[MAX_PATH];
    WORD dwRGBBitCount;
    DWORD dwRBitMask, dwGBitMask, dwBBitMask, dwABitMask;
    char *sType;
    OutTraceDW("glTextureDump: formats=%#x,%#x size=(%dx%d) type=%#x\n", internalFormat, Format, w, h, type);
    if(DoOnce) {
        char sProfilePath[MAX_PATH];
        sprintf_s(sProfilePath, MAX_PATH, "%s\\dxwnd.ini", GetDxWndPath());
        MinTexX = (*pGetPrivateProfileIntA)("Texture", "MinTexX", 0, sProfilePath);
        MaxTexX = (*pGetPrivateProfileIntA)("Texture", "MaxTexX", 0, sProfilePath);
        MinTexY = (*pGetPrivateProfileIntA)("Texture", "MinTexY", 0, sProfilePath);
        MaxTexY = (*pGetPrivateProfileIntA)("Texture", "MaxTexY", 0, sProfilePath);
        OutTraceOGL("TextureDump: size min=(%dx%d) max=(%dx%d)\n", MinTexX, MinTexY, MaxTexX, MaxTexY);
        sprintf_s(pszFile, MAX_PATH, "%s\\texture.out", GetDxWndPath());
        CreateDirectory(pszFile, NULL);
        DoOnce = FALSE;
    }
    if((MinTexX && (w < MinTexX)) || (MinTexY && (h < MinTexY))) {
        OutTraceOGL("TextureDump: SKIP small texture\n");
        return;
    }
    if((MaxTexX && (w > MaxTexX)) || (MaxTexY && (h > MaxTexY))) {
        OutTraceOGL("TextureDump: SKIP big texture\n");
        return;
    }
    if(h == 1) {
        OutTraceOGL("TextureDump: SKIP 1 row texture\n");
        return;
    }
    switch(internalFormat) {
    case GL_LINE_STRIP:
        break; // ??? used by SimGolf
    case GL_TRIANGLES:
        break; // the only safe for now ....
    case GL_RGB:
        break; // from "Alone in the Dark the new nightmare" .....
    case GL_RGB16:
        break;
    case GL_RGBA8:
        break; // from "GhostMaster"
    case GL_RGBA:
        break; // ????
    case GL_LUMINANCE:
        break; // "Blak Stone"
    case GL_QUADS:
    default:
        OutTraceOGL("TextureDump: unsupported int. format %#x\n", internalFormat);
        return;
    }
    // format: GL_RED?, GL_GREEN?, GL_BLUE?, GL_RG?, GL_RGB?, GL_BGR?, GL_RGBA?, GL_BGRA
    // type: GL_UNSIGNED_BYTE?, GL_BYTE?, GL_UNSIGNED_SHORT?, GL_SHORT?, GL_UNSIGNED_INT?, GL_INT?,
    // GL_FLOAT?, GL_UNSIGNED_BYTE_3_3_2?, GL_UNSIGNED_BYTE_2_3_3_REV?, GL_UNSIGNED_SHORT_5_6_5?, GL_UNSIGNED_SHORT_5_6_5_REV?,
    // GL_UNSIGNED_SHORT_4_4_4_4?, GL_UNSIGNED_SHORT_4_4_4_4_REV?, GL_UNSIGNED_SHORT_5_5_5_1?, GL_UNSIGNED_SHORT_1_5_5_5_REV?,
    // GL_UNSIGNED_INT_8_8_8_8?, GL_UNSIGNED_INT_8_8_8_8_REV?, GL_UNSIGNED_INT_10_10_10_2?, GL_UNSIGNED_INT_2_10_10_10_REV
    sType = "unknown";
    dwRGBBitCount = 0;
    switch(Format) {
    case GL_RGB:
        switch(type) {
        case GL_BYTE:
        case GL_UNSIGNED_BYTE: // Daikatana, Doom 3
            dwRBitMask = 0x0FF000000;
            dwGBitMask = 0x000FF0000;
            dwBBitMask = 0x00000FF00;
            dwABitMask = 0x000000000;
            dwRGBBitCount = 24;
            sType = "RGB888";
            break;
        }
        break;
    case GL_RGBA:
        switch(type) {
        case GL_BYTE:
        case GL_UNSIGNED_BYTE: // Daikatana, Doom 3
            dwRBitMask = 0x000000FF;
            dwGBitMask = 0x0000FF00;
            dwBBitMask = 0x00FF0000;
            dwABitMask = 0xFF000000;
            dwRGBBitCount = 32;
            sType = "RGBA888";
            break;
        }
        break;
    case GL_BGR:
        switch(type) {
        case GL_BYTE:
        case GL_UNSIGNED_BYTE: // ???
            dwRBitMask = 0xFF000000;
            dwGBitMask = 0x00FF0000;
            dwBBitMask = 0x0000FF00;
            dwABitMask = 0x00000000;
            dwRGBBitCount = 24;
            sType = "BGR888";
            break;
        }
        break;
    case GL_BGRA:
        switch(type) {
        case GL_BYTE:
        case GL_UNSIGNED_BYTE: // ???
            dwRBitMask = 0x00FF0000;
            dwGBitMask = 0x0000FF00;
            dwBBitMask = 0x000000FF;
            dwABitMask = 0xFF000000;
            dwRGBBitCount = 32;
            sType = "BGRA888";
            break;
        }
        break;
    case GL_RGB16:
        switch(type) {
        case GL_BYTE:
        case GL_UNSIGNED_BYTE: // ???
            dwRBitMask = 0x0000F800;
            dwGBitMask = 0x000007E0;
            dwBBitMask = 0x0000001F;
            dwABitMask = 0x00000000;
            dwRGBBitCount = 16;
            sType = "RGB16";
            break;
        }
        break;
    case GL_RGBA8:
        switch(type) {
        case GL_BYTE:
        case GL_UNSIGNED_BYTE: // ???
            dwRBitMask = 0x00000000;
            dwGBitMask = 0x00000000;
            dwBBitMask = 0x00000000;
            dwABitMask = 0x00000000;
            dwRGBBitCount = 8;
            sType = "RGBA8";
            break;
        }
        break;
    case GL_LUMINANCE:
        //switch(type){
        //	default:
        dwRBitMask = 0x0000FFFF;
        dwGBitMask = 0x00000000;
        dwBBitMask = 0x00000000;
        dwABitMask = 0x00000000;
        dwRGBBitCount = 16;
        sType = "LUMINANCE";
        //		break;
        //}
        break;
    }
    if(!dwRGBBitCount) {
        OutTraceOGL("TextureDump: unsupported 0 RGB count\n");
        return;
    }
    OutTraceOGL("TextureDump: format=%s bpp=%d RGBA=(%08.8X:%08.8X:%08.8X:%08.8X)\n",
                sType, dwRGBBitCount, dwRBitMask, dwGBitMask, dwBBitMask, dwABitMask);
    iSurfaceSize = w * h * (dwRGBBitCount / 8);
    while (TRUE) {
        FILE *hf;
        BITMAPFILEHEADER hdr;       // bitmap file-header
        BITMAPV4HEADER pbi;			// bitmap info-header
        DWORD hash;
        memset((void *)&pbi, 0, sizeof(BITMAPV4HEADER));
        pbi.bV4Size = sizeof(BITMAPV4HEADER);
        pbi.bV4Width = w;
        pbi.bV4Height = h;
        pbi.bV4BitCount = dwRGBBitCount;
        pbi.bV4SizeImage = ((w * dwRGBBitCount + 0x1F) & ~0x1F) / 8 * h;
        pbi.bV4Height = - h;
        pbi.bV4Planes = 1;
        //pbi.bV4V4Compression = BI_BITFIELDS;
        pbi.bV4V4Compression = BI_RGB; // more portable
        //if(pbi.bV4BitCount == 8) pbi.bV4V4Compression = BI_RGB;
        pbi.bV4XPelsPerMeter = 1;
        pbi.bV4YPelsPerMeter = 1;
        pbi.bV4ClrUsed = 0;
        if(pbi.bV4BitCount == 8) pbi.bV4ClrUsed = 256;
        pbi.bV4ClrImportant = 0;
        pbi.bV4RedMask = dwRBitMask;
        pbi.bV4GreenMask = dwGBitMask;
        pbi.bV4BlueMask = dwBBitMask;
        pbi.bV4AlphaMask = dwABitMask;
        pbi.bV4CSType = LCS_CALIBRATED_RGB;
        iScanLineSize = ((w * dwRGBBitCount + 0x1F) & ~0x1F) / 8;
        // calculate the bitmap hash
        OutDebugOGL("glTextureDump: hash linesize=%d h=%d\n", iScanLineSize, h);
        __try {
            hash = Hash((BYTE *)data, iSurfaceSize);
        } __except(EXCEPTION_EXECUTE_HANDLER) {
            OutTraceE("glTextureDump: hash exception\n");
            return;
        }
        // Create the .BMP file.
        //sprintf_s(pszFile, MAX_PATH, "%s\\texture.out\\texture.%03d.%03d.%s.%08X.bmp",
        //	GetDxWndPath(), w, h, sType, hash);
        sprintf_s(pszFile, MAX_PATH, "%s\\texture.out\\texture.I%x.F%x.T%x.%03d.%03d.%08X.bmp",
                  GetDxWndPath(), internalFormat, Format, type, w, h, hash);
        OutDebugOGL("writing to %s\n", pszFile);
        hf = fopen(pszFile, "wb");
        if(!hf) break;
        hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"
        // Compute the size of the entire file.
        hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + pbi.bV4Size + pbi.bV4ClrUsed * sizeof(RGBQUAD) + pbi.bV4SizeImage);
        hdr.bfReserved1 = 0;
        hdr.bfReserved2 = 0;
        // Compute the offset to the array of color indices.
        hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + pbi.bV4Size + pbi.bV4ClrUsed * sizeof (RGBQUAD);
        // Copy the BITMAPFILEHEADER into the .BMP file.
        fwrite((LPVOID)&hdr, sizeof(BITMAPFILEHEADER), 1, hf);
        // Copy the BITMAPINFOHEADER array into the file.
        fwrite((LPVOID)&pbi, sizeof(BITMAPV4HEADER), 1, hf);
        // Copy the array of color indices into the .BMP file.
        __try {
            for(int y = 0; y < h; y++)
                fwrite((BYTE *)data + (y * iScanLineSize), iScanLineSize, 1, hf);
        } __except(EXCEPTION_EXECUTE_HANDLER) {}
        //fwrite((BYTE *)data, iScanLineSize, h, hf);
        // Close the .BMP file.
        fclose(hf);
        break;
    }
}

void WINAPI extglTexImage2D(
    GLenum target,
    GLint level,
    GLint internalFormat,
    GLsizei width,
    GLsizei height,
    GLint border,
    GLenum format,
    GLenum type,
    const GLvoid *data) {
    OutTraceOGL("glTexImage2D: TEXTURE target=%#x(%s) level=%#x internalformat=%#x format=%#x type=%#x size=(%dx%d)\n",
                target, sglEnum(target), level, internalFormat, format, type, width, height);
    switch(target) {
    //case GL_PROXY_TEXTURE_RECTANGLE:
    //case GL_PROXY_TEXTURE_2D:
    case GL_TEXTURE_2D:
    case GL_TEXTURE_RECTANGLE:
        switch(dxw.dwFlags5 & TEXTUREMASK) {
        default:
        case TEXTUREHIGHLIGHT:
            //glTextureHighlight(s);
            break;
        case TEXTUREDUMP:
            glTextureDump(internalFormat, format, width, height, type, data);
            break;
        case TEXTUREHACK:
            //glTextureHack(...);
            break;
        case TEXTURETRANSP:
            //glTextureTransp(...);
            break;
        }
        break;
    }
    if(dxw.dwFlags4 & NOTEXTURES) return;
    return (*pglTexImage2D)(target, level, internalFormat, width, height, border, format, type, data);
}

void WINAPI extglTexSubImage2D(
    GLenum target,
    GLint level,
    GLint xoffset, GLint yoffset,
    GLsizei width, GLsizei height,
    GLenum format,
    GLenum type,
    const GLvoid *pixels) {
    OutTraceOGL("glTexSubImage2D: target=%#x level=%d offset=(%d,%d) size=(%dx%d) format=%#x type=%#x \n",
                target, level, xoffset, yoffset, width, height, format, type);
    switch(target) {
    //case GL_PROXY_TEXTURE_RECTANGLE:
    //case GL_PROXY_TEXTURE_2D:
    case GL_TEXTURE_2D:
    case GL_TEXTURE_RECTANGLE:
        switch(dxw.dwFlags5 & TEXTUREMASK) {
        default:
        case TEXTUREHIGHLIGHT:
            //glTextureHighlight(s);
            break;
        case TEXTUREDUMP:
            glTextureDump(format, format, width, height, type, pixels);
            break;
        case TEXTUREHACK:
            //glTextureHack(...);
            break;
        case TEXTURETRANSP:
            //glTextureTransp(...);
            break;
        }
        break;
    }
    if(dxw.dwFlags4 & NOTEXTURES) return;
    (*pglTexSubImage2D)(target, level, xoffset, yoffset, width, height, format, type, pixels);
}

void WINAPI extglCopyTexImage2D(
    GLenum target,
    GLint level,
    GLenum internalFormat,
    GLint x,
    GLint y,
    GLsizei width,
    GLsizei height,
    GLint border) {
    OutTraceOGL("glCopyTexImage2D: TEXTURE target=%#x(%s) level=%#x internalformat=%#x pos=(%d,%d) size=(%dx%d) border=%d\n",
                target, sglEnum(target), level, internalFormat, x, y, width, height, border);
    switch(target) {
    //case GL_PROXY_TEXTURE_RECTANGLE:
    //case GL_PROXY_TEXTURE_2D:
    case GL_TEXTURE_2D:
    case GL_TEXTURE_RECTANGLE:
        switch(dxw.dwFlags5 & TEXTUREMASK) {
        default:
        case TEXTUREHIGHLIGHT:
            //glTextureHighlight(s);
            break;
        case TEXTUREDUMP:
            //glTextureDump(internalFormat, 0, width, height, 0, glGet(GL_READ_BUFFER));
            break;
        case TEXTUREHACK:
            //glTextureHack(...);
            break;
        case TEXTURETRANSP:
            //glTextureTransp(...);
            break;
        }
        break;
    }
    if(dxw.dwFlags4 & NOTEXTURES) return;
    return (*pglCopyTexImage2D)(target, level, internalFormat, x, y, width, height, border);
}

#ifdef STRETCHDRAWPIXELS
char *ExplainDrawPixelsFormat(DWORD c) {
    static char *eb;
    switch(c) {
    case GL_COLOR_INDEX:
        eb = "GL_COLOR_INDEX";
        break;
    case GL_STENCIL_INDEX:
        eb = "GL_STENCIL_INDEX";
        break;
    case GL_DEPTH_COMPONENT:
        eb = "GL_DEPTH_COMPONENT";
        break;
    case GL_RGB:
        eb = "GL_RGB";
        break;
    case GL_BGR:
        eb = "GL_BGR";
        break;
    case GL_RGBA:
        eb = "GL_RGBA";
        break;
    case GL_BGRA:
        eb = "GL_BGRA";
        break;
    case GL_RED:
        eb = "GL_RED";
        break;
    case GL_GREEN:
        eb = "GL_GREEN";
        break;
    case GL_BLUE:
        eb = "GL_BLUE";
        break;
    case GL_ALPHA:
        eb = "GL_ALPHA";
        break;
    case GL_LUMINANCE:
        eb = "GL_LUMINANCE";
        break;
    case GL_LUMINANCE_ALPHA:
        eb = "GL_LUMINANCE_ALPHA";
        break;
    default:
        eb = "unknown";
        break;
    }
    return eb;
}

// v2.04.28: Scaling in glDrawPixels is useful to stretch the "Crazy Marble" initial splash screen
void WINAPI extglDrawPixels(GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *data) {
    OutTraceOGL("glDrawPixels: size=(%dx%d) format=%#x(%s) type=%d data=%#x\n",
                width, height, format, ExplainDrawPixelsFormat(format), type, data);
    if(dxw.dwFlags6 & FIXPIXELZOOM) {
        GLfloat xfactor, yfactor;
        RECT desktop;
        (*pGetClientRect)(dxw.GethWnd(), &desktop);
        xfactor = (GLfloat)desktop.right / (GLfloat)dxw.GetScreenWidth();
        yfactor = (GLfloat)desktop.bottom / (GLfloat)dxw.GetScreenHeight();
        if(!pglPixelZoom) {
            HMODULE hGlLib;
            hGlLib = (*pLoadLibraryA)("OpenGL32.dll"); // Beware: not compatible with custom OpenGL specfication....
            pglPixelZoom = (glPixelZoom_Type)(*GetProcAddress)(hGlLib, "glPixelZoom");
        }
        if (pglPixelZoom) (*pglPixelZoom)(xfactor, yfactor);
        OutTraceDW("glDrawPixels: glPixelZoom FIXED x,y factor=(%f,%f)\n", xfactor, yfactor);
    }
    (*pglDrawPixels)(width, height, format, type, data);
#ifndef DXW_NOTRACES
    GLenum glerr;
    if ((glerr = extglGetError()) != GL_NO_ERROR) OutTraceE("glDrawPixels: GLERR %d at %d\n", glerr, __LINE__);
#endif // DXW_NOTRACES
    return;
}
#endif

void WINAPI extglPixelZoom(GLfloat xfactor, GLfloat yfactor) {
    OutTraceOGL("glPixelZoom: x,y factor=(%f,%f)\n", xfactor, yfactor);
    if(dxw.dwFlags6 & FIXPIXELZOOM) {
        RECT desktop;
        (*pGetClientRect)(dxw.GethWnd(), &desktop);
        xfactor = (xfactor * desktop.right) / dxw.GetScreenWidth();
        yfactor = (yfactor * desktop.bottom) / dxw.GetScreenHeight();
        OutTraceDW("glPixelZoom: FIXED x,y factor=(%f,%f)\n", xfactor, yfactor);
    }
    (*pglPixelZoom)(xfactor, yfactor);
#ifndef DXW_NOTRACES
    GLenum glerr;
    if ((glerr = extglGetError()) != GL_NO_ERROR) OutTraceE("GLERR %d at %d\n", glerr, __LINE__);
#endif // DXW_NOTRACES
    return;
}

#ifndef DXW_NOTRACES
static char *sglmode(GLenum m) {
    char *labels[] = {
        "POINTS", "LINES", "LINE_LOOP", "LINE_STRIP",
        "TRIANGLES", "TRIANGLE_STRIP", "TRIANGLE_FAN", "QUADS",
        "QUAD_STRIP", "POLYGON", "unknown"
    };
    if(m > GL_POLYGON) m = GL_POLYGON + 1;
    return labels[m];
}
#endif //DXW_NOTRACES

#ifdef TRACEALL
void WINAPI extglBegin(GLenum mode) {
    GLenum glerr;
    OutTraceOGL("glBegin: mode=%#x(%s)\n", mode, sglmode(mode));
    (*pglBegin)(mode);
    if ((glerr = extglGetError()) != GL_NO_ERROR) OutTraceE("GLERR %d at %d\n", glerr, __LINE__);
    return;
}

void WINAPI extglEnd(void) {
    OutTraceOGL("glEnd\n");
    (*pglEnd)();
}
#endif // TRACEALL

void WINAPI extglBindTexture(GLenum target, GLuint texture) {
    OutTraceOGL("glBindTexture: target=%#x(%s) texture=%#x\n", target, sglEnum(target), texture);
    if(dxw.dwFlags7 & FIXBINDTEXTURE) {
        static GLuint uiLastTex = 0;
        if(uiLastTex) (*pglBindTexture)(target, 0);
        uiLastTex = texture;
    }
    extglGetError(); // clears the error code
    (*pglBindTexture)(target, texture);
#ifndef DXW_NOTRACES
    GLenum glerr;
    if ((glerr = extglGetError()) != GL_NO_ERROR) OutTraceE("GLERR %d at %d\n", glerr, __LINE__);
#endif // DXW_NOTRACES
    return;
}

#ifdef TRACEALL
void WINAPI extglPixelStorei(GLenum pname,  GLint param) {
    GLenum glerr;
    OutTraceOGL("glPixelStorei: pname=%#x param=%#x\n", pname, param);
    (*pglPixelStorei)(pname, param);
    if ((glerr = extglGetError()) != GL_NO_ERROR) OutTraceE("GLERR %d at %d\n", glerr, __LINE__);
    return;
}
#endif

void WINAPI extglutFullScreen(void) {
    OutTraceOGL("glutFullScreen: void\n");
    if(!dxw.Windowize) return (*pglutFullScreen)();
    OutTraceDW("glutFullScreen BYPASS\n");
    dxw.SetFullScreen(TRUE);
}

void extglutInitWindowSize(int width, int height) {
    int dummy1, dummy2;
    OutTraceOGL("glutInitWindowSize: size=(%dx%d)\n", width, height);
    if(dxw.Windowize) {
        dummy1 = 0;
        dummy2 = 0;
        dxw.MapWindow(&dummy1, &dummy2, &width, &height);
        OutTraceDW("glutInitWindowSize: FIXED width=%d height=%d\n", width, height);
    }
    (*pglutInitWindowSize)(width, height);
}

void extglutInitWindowPosition(int x, int y) {
    int dummy1, dummy2;
    OutTraceOGL("glutInitWindowPosition: pos=(%d,%d)\n", x, y);
    if(dxw.Windowize) {
        dummy1 = 0;
        dummy2 = 0;
        dxw.MapWindow(&x, &y, &dummy1, &dummy2);
        OutTraceDW("glutInitWindowPosition: FIXED pos=(%d,%d)\n", x, y);
    }
    (*pglutInitWindowPosition)(x, y);
}

void WINAPI extglutSetWindow(HWND win) {
    OutTraceOGL("glutSetWindow: win=%#x\n", win);
    if(dxw.Windowize && dxw.IsRealDesktop(win)) win = dxw.GethWnd();
    (*pglutSetWindow)(win);
}

static char *glStringName(GLenum name) {
    char *ret;
    switch(name) {
    case GL_VENDOR:
        ret = "GL_VENDOR";
        break;
    case GL_RENDERER:
        ret = "GL_RENDERER";
        break;
    case GL_VERSION:
        ret = "GL_VERSION";
        break;
    case GL_SHADING_LANGUAGE_VERSION:
        ret = "GL_SHADING_LANGUAGE_VERSION";
        break;
    case GL_EXTENSIONS:
        ret = "GL_EXTENSIONS";
        break;
    default:
        ret = "unknown";
        break;
    }
    return ret;
}

const  GLubyte *WINAPI extglGetString(GLenum name) {
    const GLubyte *ret;
    ApiName("glGetString");
    OutTraceOGL("%s: name=%#x(%s)\n", ApiRef, name, glStringName(name));
    ret = (*pglGetString)(name);
#ifndef DXW_NOTRACES
    if(IsDebugOGL) {
        if(strlen((const char *)ret) < 80)
            OutTrace("%s: name=%#x(%s) ret=\"%.80s\"\n", ApiRef, name, glStringName(name), ret);
        else {
            const GLubyte *p = ret;
            OutTrace("%s: name=%#x(%s) ret=(%d)\n", ApiRef, name, glStringName(name), strlen((const char *)ret));
            while(strlen((const char *)p) > 80) {
                OutTrace("%s: \"%.80s\" +\n", ApiRef, p);
                p += 80;
            }
            OutTrace("%s: \"%.80s\"\n", ApiRef, p);
        }
    }
#endif // DXW_NOTRACES
    if((name == GL_EXTENSIONS) && (dxw.dwFlags12 & GLEXTENSIONSLIE)) {
        // returns a dummy, unknown extension
        OutTraceOGL("%s: returning GL_EXT_LIE\n", ApiRef);
        return (GLubyte *)"GL_EXT_LIE";
    }
    return ret;
}

char *WINAPI extwglGetExtensionsStringEXT(void) {
    char *ret;
    ApiName("wglGetExtensionsStringEXT");
    OutTraceOGL("%s: void\n", ApiRef);
    ret = (*pwglGetExtensionsStringEXT)();
#ifndef DXW_NOTRACES
    if(IsDebugOGL) {
        if(strlen((const char *)ret) < 80)
            OutTrace("%s: ret=\"%.80s\"\n", ApiRef, ret);
        else {
            const char *p = ret;
            OutTrace("%s: ret=(%d)\n", ApiRef, strlen((const char *)ret));
            while(strlen((const char *)p) > 80) {
                OutTrace("%s: \"%.80s\" +\n", ApiRef, p);
                p += 80;
            }
            OutTrace("%s: \"%.80s\"\n", ApiRef, p);
        }
    }
#endif // DXW_NOTRACES
    if(dxw.dwFlags12 & GLEXTENSIONSLIE) {
        // returns a dummy, unknown extension
        OutTraceOGL("%s: returning GL_EXT_LIE\n", ApiRef);
        return "GL_EXT_LIE";
    }
    return ret;
}

const GLubyte *WINAPI extgluGetString(GLenum name) {
    const GLubyte *ret;
    ApiName("gluGetString");
    OutTraceOGL("%s: name=%#x(%s)\n", ApiRef, name, glStringName(name));
    ret = (*pgluGetString)(name);
#ifndef DXW_NOTRACES
    if(IsDebugOGL) {
        if(strlen((const char *)ret) < 80)
            OutTrace("%s: name=%#x(%s) ret=\"%.80s\"\n", ApiRef, name, glStringName(name), ret);
        else {
            const GLubyte *p = ret;
            OutTrace("%s: name=%#x(%s) ret=(%d)\n", ApiRef, name, glStringName(name), strlen((const char *)ret));
            while(strlen((const char *)p) > 80) {
                OutTrace("%s: \"%.80s\" +\n", ApiRef, p);
                p += 80;
            }
            OutTrace("%s: \"%.80s\"\n", ApiRef, p);
        }
    }
#endif // DXW_NOTRACES
    if((name == GL_EXTENSIONS) && (dxw.dwFlags12 & GLEXTENSIONSLIE)) {
        // returns a dummy, unknown extension
        OutTraceOGL("%s: returning GL_EXT_LIE\n", ApiRef);
        return (GLubyte *)"GL_EXT_LIE";
    }
    return ret;
}

#ifdef TRACEALL
void WINAPI extgluOrtho2D(GLdouble left, GLdouble right, GLdouble top, GLdouble bottom) {
    OutTraceOGL("gluOrtho2D: rect=(%f,%f)-(%f,%f)\n", left, right, top, bottom);
    (*pgluOrtho2D)(left, right, top, bottom);
}

GLint WINAPI extgluBuild2DMipmaps(GLenum target, GLint intFormat, GLsizei w, GLsizei h, GLenum format, GLenum type, const void *data) {
    GLint ret;
    OutTraceOGL("gluBuild2DMipmaps: size=%dx%d\n", w, h);
    ret = (*pgluBuild2DMipmaps)(target, intFormat, w, h, format, type, data);
    OutTraceOGL("gluBuild2DMipmaps: ret=%d\n", ret);
    return ret;
}

void WINAPI extglRasterPos4fv(const GLfloat *v) {
    OutTraceOGL("glRasterPos4fv: x=%f y=%f z=%f w=%f\n", v[0], v[1], v[2], v[3]);
    //v2.05.29 fix: no scaling - the coordinates are in window logical values!!
    //dxw.UnmapClient(&w[0], &w[1]);
    (*pglRasterPos4fv)((const GLfloat *)v);
}

void WINAPI extglRasterPos2i(GLint x, GLint y) {
    OutTraceOGL("glRasterPos2i: x=%d y=%d\n", x, y);
    //v2.05.29 fix: no scaling - the coordinates are in window logical values!!
    //dxw.UnmapClient(&x, &y);
    (*pglRasterPos2i)(x, y);
}

void WINAPI extglRasterPos2d(GLdouble x, GLdouble y) {
    OutTraceOGL("glRasterPos2d: x=%f y=%f\n", (float)x, (float)y);
    //v2.05.29 fix: no scaling - the coordinates are in window logical values!!
    //dxw.UnmapClient(&x, &y);
    (*pglRasterPos2d)(x, y);
}

void WINAPI extglRasterPos2f(GLfloat x, GLfloat y) {
    OutTraceOGL("glRasterPos2f: x=%f y=%f\n", x, y);
    //v2.05.29 fix: no scaling - the coordinates are in window logical values!!
    //dxw.UnmapClient(&x, &y);
    (*pglRasterPos2f)(x, y);
}
#endif // TRACEALL

// for glBitmap, see http://www.dei.isep.ipp.pt/~matos/cg/docs/OpenGL_PG/ch09.html
// from http://www.dei.isep.ipp.pt/~matos/cg/docs/OpenGL_PG/index.html

#ifdef STRETCHBITMAPS

// undocumented, but true: MS glBitmaps are padded to DWORD boundaries!
#define padlen(x) (((x+31)/32) * 4)

static int getbit(BYTE *map, int y, int x, int h, int w) {
    BYTE mask = 0x1 << (7 - (x % 8));
    BYTE byte;
    int linelen = padlen(w);
    byte = map[((h - (y + 1)) * linelen) + (x / 8)];
    return (byte & mask);
}

static void putbit(BYTE *map, int y, int x, int h, int w, int val) {
    BYTE mask = 0x1 << (7 - (x % 8));
    BYTE *byte;
    int linelen = padlen(w);
    byte = &map[((h - (y + 1)) * linelen) + (x / 8)];
    if(val)
        *byte |= mask;
    // following lines useless if the array is initialized to 0s
    //else
    //	*byte &= ~mask;
}

void resample(GLubyte *bitmap, GLubyte *scaledmap, int oldw, int oldh, int neww, int newh) {
    OutTraceDW("resample: scaling (%dx%d)->(%dx%d)\n", oldw, oldh, neww,  newh);
#ifdef SCALEDUMP
    for(int y = 0; y < oldh; y++) {
        for(int x = 0; x < oldw; x++)
            OutTrace("%c", getbit((BYTE *)bitmap, y, x, oldh, oldw) ? 'X' : '_');
        OutTrace("\n");
    }
#endif
    double xscale = (neww + 0.0) / oldw;
    double yscale = (newh + 0.0) / oldh;
    double threshold = 0.5 / (xscale * yscale);
    double yend = 0.0;
    for (int f = 0; f < newh; f++) { // y on output
        double ystart = yend;
        yend = (f + 1) / yscale;
        if (yend >= oldh) yend = oldh - 0.000001;
        double xend = 0.0;
        for (int g = 0; g < neww; g++) { // x on output
            double xstart = xend;
            xend = (g + 1) / xscale;
            if (xend >= oldw) xend = oldw - 0.000001;
            double sum = 0.0;
            for (int y = (int)ystart; y <= (int)yend; ++y) {
                double yportion = 1.0;
                if (y == (int)ystart) yportion -= ystart - y;
                if (y == (int)yend) yportion -= y + 1 - yend;
                for (int x = (int)xstart; x <= (int)xend; ++x) {
                    double xportion = 1.0;
                    if (x == (int)xstart) xportion -= xstart - x;
                    if (x == (int)xend) xportion -= x + 1 - xend;
                    sum += (double)getbit((BYTE *)bitmap, y, x, oldh, oldw) * yportion * xportion;
                }
            }
            //scaledmap[f][g] = (sum > threshold) ? 1 : 0;
            putbit((BYTE *)scaledmap, f, g, newh, neww, (sum > threshold) ? 1 : 0);
        }
    }
#ifdef SCALEDUMP
    for(int y = 0; y < newh; y++) {
        for(int x = 0; x < neww; x++)
            OutTrace("%c", getbit((BYTE *)scaledmap, y, x, newh, neww) ? 'X' : '_');
        OutTrace("\n");
    }
#endif
}
#endif

void WINAPI extglBitmap(GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap) {
    OutTraceOGL("glBitmap: size=(%dx%d) orig=(%f,%f) move=(%f,%f)\n", width, height, xorig, yorig, xmove, ymove);
#ifdef STRETCHBITMAPS
    if(dxw.dwFlags9 & SCALEGLBITMAPS) {
        int scaledw, scaledh, bufsize;
        GLubyte *scaledmap;
        scaledw = width;
        scaledh = height;
        dxw.MapClient(&scaledw, &scaledh);
        dxw.MapClient(&xorig, &yorig);
        dxw.MapClient(&xmove, &ymove);
        bufsize = padlen(scaledw) * scaledh;
        scaledmap = (GLubyte *)malloc(bufsize);
        memset(scaledmap, 0, bufsize);
        resample((GLubyte *)bitmap, (GLubyte *)scaledmap, width, height, scaledw, scaledh);
        (*pglBitmap)(scaledw, scaledh, xorig, yorig, xmove, ymove, scaledmap);
        free(scaledmap);
    } else
        (*pglBitmap)(width, height, xorig, yorig, xmove, ymove, bitmap);
#else
    (*pglBitmap)(width, height, xorig, yorig, xmove, ymove, bitmap);
#endif
}

void WINAPI extglTexParameteri(GLenum target, GLenum pname, GLint param) {
    ApiName("glTexParameteri");
    OutTraceOGL("%s: target=%#x(%s) pname=%#x(%s) param=%d\n", ApiRef, target, sglEnum(target), pname, sglEnum(pname), param);
    if((dxw.dwFlags13 & GLFIXCLAMP) && (target == GL_TEXTURE_2D)) {
        if ((pname == GL_TEXTURE_WRAP_T) || (pname == GL_TEXTURE_WRAP_S)) {
            if(param == GL_CLAMP) {
                OutTraceOGL("%s: forced GL_CLAMP_TO_EDGE\n", ApiRef);
                param = GL_CLAMP_TO_EDGE;
            }
        }
    }
    (*pglTexParameteri)(target, pname, param);
}

void WINAPI extglTexParameterf(GLenum target, GLenum pname, GLfloat param) {
    ApiName("glTexParameterf");
    OutTraceOGL("%s: target=%#x(%s) pname=%#x(%s) param=%f\n", ApiRef, target, sglEnum(target), pname, sglEnum(pname), param);
    if((dxw.dwFlags13 & GLFIXCLAMP) && (target == GL_TEXTURE_2D)) {
        if ((pname == GL_TEXTURE_WRAP_T) || (pname == GL_TEXTURE_WRAP_S)) {
            if(param == (float)GL_CLAMP) {
                OutTraceOGL("%s: forced GL_CLAMP_TO_EDGE\n", ApiRef);
                param = (float)GL_CLAMP_TO_EDGE;
            }
        }
    }
    (*pglTexParameterf)(target, pname, param);
}

#ifdef TRACEALL

void WINAPI extglVertex2f(GLfloat x, GLfloat y) {
    OutTraceOGL("glVertex2f: x=%f y=%f\n", x, y);
    (*pglVertex2f)(x, y);
}
void WINAPI extglVertex3f(GLfloat x, GLfloat y, GLfloat z) {
    OutTraceOGL("glVertex3f: x=%f y=%f z=%f\n", x, y, z);
    (*pglVertex3f)(x, y, z);
}

void WINAPI extglFrustum(GLdouble left,  GLdouble right,  GLdouble bottom,  GLdouble top,  GLdouble nearVal,  GLdouble farVal) {
    OutTraceOGL("glFrustum: rect=(%f,%f)-(%f,%f) near=%f far=%f\n",
                (float)left, (float)top, (float)right, (float)bottom, (float)nearVal, (float)farVal);
    (*pglFrustum)(left, right, bottom, top, nearVal, farVal);
}

void WINAPI extglDrawArrays(GLenum mode, GLint first, GLsizei count) {
    OutTraceOGL("glDrawArrays: mode=%#x first=%d count=%d\n", mode, first, count);
    (*pglDrawArrays)(mode, first, count);
}

void WINAPI extglTranslatef(GLfloat x, GLfloat y, GLfloat z) {
    OutTraceOGL("glTranslatef: xyz=(%f:%f:%f)\n", x, y, z);
    (*pglTranslatef)(x, y, z);
}

void WINAPI extglScalef(GLfloat x, GLfloat y, GLfloat z) {
    OutTraceOGL("glScalef: xyz=(%f:%f:%f)\n", x, y, z);
    (*pglScalef)(x, y, z);
}

void WINAPI extglOrtho(GLdouble left,  GLdouble right,  GLdouble bottom,  GLdouble top,  GLdouble nearVal,  GLdouble farVal) {
    OutTraceOGL("glOrtho: rect=(%f,%f)-(%f,%f) near=%f far=%f\n", left, right, top, bottom, nearVal, farVal);
    (*pglOrtho)(left, right, bottom, top, nearVal, farVal);
}

void WINAPI extglEnable(GLenum cap) {
    OutTraceOGL("glEnable: cap=%#x(%s)\n", cap, sglEnum(cap));
    (*pglEnable)(cap);
}

void WINAPI extglDisable(GLenum cap) {
    OutTraceOGL("glDisable: cap=%#x(%s)\n", cap, sglEnum(cap));
    (*pglDisable)(cap);
}

void WINAPI extglGenTextures(GLsizei n, GLuint *textures) {
    OutTraceOGL("glGenTextures: n=%d tex=%#x\n", n, textures);
    (*pglGenTextures)(n, textures);
}

void WINAPI extglTexEnvi(GLenum target, GLenum pname, GLint param) {
    OutTraceOGL("glTexEnvi: target=%#x(%s) pname=%#x(%s) param=%d\n", target, sglEnum(target), pname, sglEnum(pname), param);
    (*pglTexEnvi)(target, pname, param);
}

void WINAPI extglTexEnvf(GLenum target, GLenum pname, GLfloat param) {
    OutTraceOGL("glTexEnvf: target=%#x(%s) pname=%#x(%s) param=%f\n", target, sglEnum(target), pname, sglEnum(pname), param);
    (*pglTexEnvf)(target, pname, param);
}

void WINAPI extglDeleteTextures(GLsizei n, const GLuint *textures) {
    OutTraceOGL("glDeleteTextures: n=%d tex=%#x\n", n, textures);
    (*pglDeleteTextures)(n, textures);
}

void WINAPI extglMatrixMode(GLenum mode) {
    OutTraceOGL("glMatrixMode: mode=%#x(%s)\n", mode, sglEnum(mode));
    (*pglMatrixMode)(mode);
}

void WINAPI extglLoadIdentity(void) {
    OutTraceOGL("glLoadIdentity:\n");
    (*pglLoadIdentity)();
}

void WINAPI extglutSwapBuffers() {
    OutTraceOGL("glutSwapBuffers:\n");
    (*pglutSwapBuffers)();
}

void WINAPI extglHint(GLenum target, GLenum mode) {
    OutTraceOGL("glHint: target=%#x(%s) mode=%#x(%s)\n", target, sglEnum(target), mode, sglEnum(mode));
    (*pglHint)(target, mode);
}

void WINAPI extglFrontFace(GLenum mode) {
    OutTraceOGL("glFrontFace: mode=%#x(%s)\n", mode, sglEnum(mode));
    (*pglFrontFace)(mode);
}

void WINAPI extglEnableClientState(GLenum cap) {
    OutTraceOGL("glEnableClientState: cap=%#x(%s)\n", cap, sglEnum(cap));
    (*pglEnableClientState)(cap);
}

void WINAPI extglMateriali(GLenum face, GLenum pname, GLint param) {
    OutTraceOGL("glMateriali: face=%#x(%s) pname=%#x(%s) param=%d\n",
                face, sglEnum(face), pname, sglEnum(pname), param);
    (*pglMateriali)(face, pname, param);
}

void WINAPI extglMaterialfv(GLenum face, GLenum pname, const GLfloat *params) {
    OutTraceOGL("glMaterialfv: face=%#x(%s) pname=%#x(%s)\n",
                face, sglEnum(face), pname, sglEnum(pname));
    (*pglMaterialfv)(face, pname, params);
}

void WINAPI extglLightf(GLenum light, GLenum pname, GLfloat param) {
    OutTraceOGL("glLightf: light=%#x(%s) pname=%#x(%s) param=%f\n",
                light, sglEnum(light), pname, sglEnum(pname), param);
    (*pglLightf)(light, pname, param);
}

void WINAPI extglLighti(GLenum light, GLenum pname, GLint param) {
    OutTraceOGL("glLighti: light=%#x(%s) pname=%#x(%s) param=%d\n",
                light, sglEnum(light), pname, sglEnum(pname), param);
    (*pglLighti)(light, pname, param);
}

void WINAPI extglLightfv(GLenum light, GLenum pname, const GLfloat *params) {
    OutTraceOGL("glLightfv: light=%#x(%s) pname=%#x(%s)\n",
                light, sglEnum(light), pname, sglEnum(pname));
    (*pglLightfv)(light, pname, params);
}

void WINAPI extglLightiv(GLenum light, GLenum pname, const GLint *params) {
    OutTraceOGL("glLightiv: light=%#x(%s) pname=%#x(%s)\n",
                light, sglEnum(light), pname, sglEnum(pname));
    (*pglLightiv)(light, pname, params);
}

void WINAPI extglPushMatrix() {
    OutTraceOGL("glPushMatrix:\n");
    (*pglPushMatrix)();
}

void WINAPI extglRotated(GLdouble angle, GLdouble x, GLdouble y, GLdouble z) {
    OutTraceOGL("glRotated: angle=%f x=%f y=%f z=%f\n", angle, x, y, z);
    (*pglRotated)(angle, x, y, z);
}

void WINAPI extglRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z) {
    OutTraceOGL("glRotatef: angle=%f x=%f y=%f z=%f\n", angle, x, y, z);
    (*pglRotatef)(angle, x, y, z);
}

void WINAPI extglArrayElement(GLint i) {
    OutTraceOGL("glArrayElement: i=%d\n", i);
    (*pglArrayElement)(i);
}
#endif // TRACEALL
