#ifndef	__WINSYS_H__
#define	__WINSYS_H__

#include <GL/gl.h>
#include <GL/glx.h>
#include <X11/Xlib.h>

extern Display					*dpy;
extern Window					root;
extern XVisualInfo				*vi;
extern Colormap				cmap;
extern XSetWindowAttributes	swa;
extern Window					win;
extern GLXContext				glc;
extern XWindowAttributes		gwa;
extern XEvent					xev;
extern GLXFBConfig				fbc;
extern Atom					delete_event;
extern Bool					loop;

void CreateWindow();
void DestroyWindow();
void EndDraw();
void TreatEvents();
void msleep(unsigned int msec);
unsigned int getTime();
void NonFullscreen();

extern void (*TreatConfigureNotify)(XEvent *xev);

#endif //__WINSYS_H__
