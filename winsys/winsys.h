#ifndef	__WINSYS_H__
#define	__WINSYS_H__

#include <glcommon.h>

#include <unistd.h>

#include <GL/glcorearb.h>
#include <GL/glx.h>
#include <X11/Xlib.h>

struct GraphicsContext {
	Display			*dpy;
	XVisualInfo		*vi;
	Colormap		cmap;
	Window			root;
	Window			win;
	int				screen;

	int				major;
	int				minor;
};

struct FrameCounter {
	unsigned int lastTime;
	unsigned int elapsedTime;
	unsigned int frameCount;

	unsigned int elapsedTimeTotal;
	unsigned int frameCountTotal;
};

extern int					debug;
extern int					verbose;
extern XSetWindowAttributes	swa;
extern GLXContext			glc;
extern XWindowAttributes	gwa;
extern XEvent				xev;
extern GLXFBConfig			fbc;
extern Atom					delete_event;
extern Bool					loop;

GraphicsContext *createGraphicsContext();
void CreateWindow(GraphicsContext *ctx);
void DestroyWindow(GraphicsContext *ctx);
void EndDraw(GraphicsContext *ctx);
void TreatEvents(GraphicsContext *ctx);
void msleep(unsigned int msec);
unsigned int getTime();
void Fullscreen(GraphicsContext *ctx);
void NonFullscreen(GraphicsContext *ctx);
FrameCounter *initFrameCounter();
void updateFrameCounter(FrameCounter *);
void saveImage(const char *filename = NULL);

#define KEY_PRESS(name)		void name(XKeyEvent *xkey)
typedef KEY_PRESS(KeyPressFn);
#define BUTTON_PRESS(name)		void name(XButtonEvent *xbutton)
typedef BUTTON_PRESS(ButtonPressFn);

extern void (*TreatConfigureNotify)(XEvent *xev);
extern KeyPressFn *TreatKeyPress;
extern ButtonPressFn *TreatButtonPress;

#endif //__WINSYS_H__
