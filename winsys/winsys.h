#ifndef	__WINSYS_H__
#define	__WINSYS_H__

#define GL_GLEXT_PROTOTYPES
#include <GL/glcorearb.h>
#include <GL/glx.h>
#include <X11/Xlib.h>

extern int debug;
extern int verbose;
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

#define KEY_PRESS(name)		void name(XKeyEvent *xkey)
typedef KEY_PRESS(KeyPressFn);
#define BUTTON_PRESS(name)		void name(XButtonEvent *xbutton)
typedef BUTTON_PRESS(ButtonPressFn);

extern void (*TreatConfigureNotify)(XEvent *xev);
extern KeyPressFn *TreatKeyPress;
extern ButtonPressFn *TreatButtonPress;

#endif //__WINSYS_H__
