#include "winsys.h"

#define		GL_GLEXT_PROTOTYPES

#include <iostream>
#include <cstdlib>
#include <cerrno>
#include <ctime>
#include <sys/time.h>
#include <GL/glx.h>
#include <X11/Xlib.h>

// Define se a janela deve ser fullscreen
//#define		Full

using std::cout;
using std::endl;

Display					*dpy;
Window					root;
XVisualInfo				*vi;
Colormap				cmap;
XSetWindowAttributes	swa;
Window					win;
GLXContext				glc;
XWindowAttributes		gwa;
XEvent					xev;
GLXFBConfig				fbc;
Atom					delete_event;
Bool					loop;

PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB;

GLint att[] = {
	GLX_X_RENDERABLE,	True,
	GLX_RENDER_TYPE,	GLX_RGBA_BIT,
	GLX_DRAWABLE_TYPE,	GLX_WINDOW_BIT,
	GLX_X_VISUAL_TYPE,	GLX_TRUE_COLOR,
	GLX_RED_SIZE,		8,
	GLX_GREEN_SIZE,		8,
	GLX_BLUE_SIZE,		8,
	GLX_DEPTH_SIZE,		24,
	GLX_STENCIL_SIZE,	8,
	GLX_DOUBLEBUFFER,	True,
	GLX_SAMPLE_BUFFERS,	1,
	GLX_SAMPLES,		8,
	None
};

void CreateWindow(){
	dpy = XOpenDisplay(NULL);
	
	root = DefaultRootWindow(dpy);
	
	int count;
	GLXFBConfig *p_fbc = glXChooseFBConfig(dpy, DefaultScreen(dpy), att, &count);
	
	if (count < 1){
		cout << "No FBConfig found" << endl;
		cout << count << endl;
		
		/*for (int i = 0; i < count; i++) {
			std::cout << p_fbc[i].
		}*/
		
		XCloseDisplay(dpy);
		exit(1);
	}
	
	fbc = p_fbc[0];
	XFree(p_fbc); // Can I free it here?
	
	vi = glXGetVisualFromFBConfig(dpy, fbc);
	
	cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
	
	swa.background_pixel = 0xBCBCBC;
	swa.colormap = cmap;
	swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | PointerMotionMask | ButtonMotionMask | ButtonPressMask | StructureNotifyMask;
	
	win = XCreateWindow(dpy, root, 0, 0, 600, 600, 0, vi->depth, InputOutput, vi->visual, CWBackPixel | CWColormap | CWEventMask, &swa);
	
	delete_event = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(dpy, win, &delete_event, 1);
	
	XMapWindow(dpy, win);
	XStoreName(dpy, win, "Simple X Window");
	
	//Create a XEvent to tell the Window Manager to show the window as fullscreen
#ifdef Full
	memset(&xev, 0, sizeof(xev));
	xev.type = ClientMessage;
	xev.xclient.window = win;
	xev.xclient.message_type = XInternAtom(dpy, "_NET_WM_STATE", False);
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = 1;
	xev.xclient.data.l[1] = XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", False);
	xev.xclient.data.l[2] = xev.xclient.data.l[3] = xev.xclient.data.l[4] = 0;
	
	//Send the XEvent that was just created
	XSendEvent (dpy, root, False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);
#endif

	int context_attribs[] = {
		GLX_CONTEXT_MAJOR_VERSION_ARB,	4,
		GLX_CONTEXT_MINOR_VERSION_ARB,	1,
		GLX_CONTEXT_FLAGS_ARB,			GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		GLX_CONTEXT_PROFILE_MASK_ARB,	GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
		GLX_RENDER_TYPE,				GLX_RGBA_TYPE,
		None
	};

	glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC) glXGetProcAddressARB( (const GLubyte *) "glXCreateContextAttribsARB");
	
	if (!glXCreateContextAttribsARB){
		cout << "Error getting gl functions address" << endl;
		
		XCloseDisplay(dpy);
		exit(1);
	}
	
	glc = glXCreateContextAttribsARB(dpy, fbc, 0, True, context_attribs);
	//glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
	
	if (glc == NULL){
		cout << "Failed to create a context" << endl;
		
		XCloseDisplay(dpy);
		exit(1);
	}
	
	if (glXIsDirect(dpy, glc)){
		cout << "Direct GLX rendering context obtained" << endl;
	}else{
		cout << "Indirect GLX rendering context obtained" << endl;
	}
	
	glXMakeCurrent(dpy, win, glc);
}

void DestroyWindow() {
	glXMakeCurrent(dpy, None, NULL);
	glXDestroyContext(dpy, glc);
	
	XDestroyWindow(dpy, win);	
	XCloseDisplay(dpy);
}

void EndDraw() {
	glXSwapBuffers(dpy, win);
}

void TreatEvents() {
	while (XPending(dpy) > 0){
		XNextEvent(dpy, &xev);
		
		switch (xev.type){
			case KeyPress:
				/*switch (XLookupKeysym(&xev.xkey, 0)){
					case (XK_space):
						//modelview.translate(0, 0, 1);
						
						//if (quant == size)
						//	quant = 0;
						//
						//quant += 600;
						break;
					case (XK_w):
						if (xev.xkey.state & ShiftMask)
							vert = -Run;
						else
							vert = -Walk;
						break;
					case (XK_s):
						if (xev.xkey.state & ShiftMask)
							vert = Run;
						else
							vert = Walk;
						break;
					case (XK_a):
						if (xev.xkey.state & ShiftMask)
							hor = Run;
						else
							hor = Walk;
						break;
					case (XK_d):
						if (xev.xkey.state & ShiftMask)
							hor = -Run;
						else
							hor = -Walk;
						break;
					case (XK_Up):
						vertRot++;
						break;
					case (XK_Down):
						vertRot--;
						break;
					case (XK_Left):
						horRot++;
						break;
					case (XK_Right):
						horRot--;
						break;
					case (XK_Escape):
						loop = false;
						break;
				}
				//cout << (xev.xkey.state & Mod1Mask) << endl;
				break;*/
			case KeyRelease:
				/*switch (XLookupKeysym(&xev.xkey, 0)){
					case (XK_space):
						//modelview.translate(0, 0, 1);
						
						//if (quant == size)
						//	quant = 0;
						//
						//quant += 600;
						break;
					case (XK_w):
						vert = 0;
						break;
					case (XK_s):
						vert = 0;
						break;
					case (XK_a):
						hor = 0;
						break;
					case (XK_d):
						hor = 0;
						break;
				}
				break;*/
			case ButtonPress:
				/*cout << xev.xbutton.time << endl;
				cout << xev.xbutton.button << endl;
				cout << xev.xbutton.x << ", " << xev.xbutton.y << endl << endl;*/
				break;
			case MotionNotify:
				/*static int lastX = 0;
				
				cout << xev.xmotion.time << endl;
				cout << xev.xmotion.x << ", " << xev.xmotion.y << endl << endl;*/
				break;
			case ClientMessage:
				if (xev.xclient.data.l[0] == delete_event)
					loop = false;
				break;
			case ConfigureNotify:					
				glViewport(0, 0, xev.xconfigure.width, xev.xconfigure.height);
				//projection.setPerspective(60, static_cast<float>(xev.xconfigure.width) / xev.xconfigure.height, 1, 1000);
				break;
			default:
				break;
		}
	}
}

void msleep(unsigned int msec) {
	struct timespec req, rem;
	int err;

	req.tv_sec = msec / 1000;
	req.tv_nsec = (msec % 1000) * 1000000;

	while ((req.tv_sec != 0) || (req.tv_nsec != 0)) {
		if (nanosleep(&req, &rem) == 0) {
			break;
		}
		err = errno;
		if (err = EINTR) {
			req.tv_sec = rem.tv_sec;
			req.tv_nsec = rem.tv_nsec;
			continue;
		}
		break;
	}
}

unsigned int getTime() {
	struct timeval tv;
	
	gettimeofday(&tv, NULL);
	
	return tv.tv_sec * 1000 + tv.tv_usec /1000;
}

void NonFullscreen() {
	//Create a XEvent to tell the Window Manager to show the window as non-fullscreen
#ifdef Full
	memset(&xev, 0, sizeof(xev));
	xev.type = ClientMessage;
	xev.xclient.window = win;
	xev.xclient.message_type = XInternAtom(dpy, "_NET_WM_STATE", False);
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = 0;
	xev.xclient.data.l[1] = XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", False);
	xev.xclient.data.l[2] = xev.xclient.data.l[3] = xev.xclient.data.l[4] = 0;
	
	//Send the XEvent just created
	XSendEvent (dpy, root, False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);
#endif
}
