#include <winsys.h>

#include <iostream>
#include <cstdlib>
#include <cerrno>
#include <ctime>
#include <cstring>
#include <sys/time.h>

// Define se a janela deve ser fullscreen
//#define		Full

using std::cout;
using std::endl;

KEY_PRESS(TreatKeyPressStub) {}
BUTTON_PRESS(TreatButtonPressStub) {}

void (*TreatConfigureNotify)(XEvent *xev);
KeyPressFn *TreatKeyPress = TreatKeyPressStub;
ButtonPressFn *TreatButtonPress = TreatButtonPressStub;

int debug;
int verbose;
XSetWindowAttributes	swa;
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
//	GLX_SAMPLE_BUFFERS,	1,
//	GLX_SAMPLES,		8,
	None
};

GraphicsContext *createGraphicsContext() {
	return new GraphicsContext{0};
}

void CreateWindow(GraphicsContext *ctx){
	if (ctx->major == 0 && ctx->minor == 0) {
		ctx->major = 4;
		ctx->minor = 1;
	}

	ctx->dpy = XOpenDisplay(NULL);
	
	ctx->root = DefaultRootWindow(ctx->dpy);
	ctx->screen = DefaultScreen(ctx->dpy);
	
	int count;
	GLXFBConfig *p_fbc = glXChooseFBConfig(ctx->dpy, ctx->screen, att, &count);
	
	if (count < 1){
		cout << "No FBConfig found" << endl;
		cout << count << endl;
		
		/*for (int i = 0; i < count; i++) {
			std::cout << p_fbc[i].
		}*/
		
		XCloseDisplay(ctx->dpy);
		exit(1);
	}
	
	fbc = p_fbc[0];
	XFree(p_fbc); // Can I free it here?
	
	ctx->vi = glXGetVisualFromFBConfig(ctx->dpy, fbc);
	
	ctx->cmap = XCreateColormap(ctx->dpy, ctx->root, ctx->vi->visual, AllocNone);
	
	swa.background_pixel = 0xBCBCBC;
	swa.colormap = ctx->cmap;
	swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | PointerMotionMask | ButtonMotionMask | ButtonPressMask | StructureNotifyMask;
	
	ctx->win = XCreateWindow(ctx->dpy, ctx->root, 0, 0, 600, 600, 0, ctx->vi->depth, InputOutput, ctx->vi->visual, CWBackPixel | CWColormap | CWEventMask, &swa);
	
	delete_event = XInternAtom(ctx->dpy, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(ctx->dpy, ctx->win, &delete_event, 1);
	
	XMapWindow(ctx->dpy, ctx->win);
	XStoreName(ctx->dpy, ctx->win, "Simple X Window");
	

	int context_attribs[] = {
		GLX_CONTEXT_MAJOR_VERSION_ARB,	ctx->major,
		GLX_CONTEXT_MINOR_VERSION_ARB,	ctx->minor,
		GLX_CONTEXT_FLAGS_ARB,			GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		GLX_CONTEXT_PROFILE_MASK_ARB,	GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
		GLX_RENDER_TYPE,				GLX_RGBA_TYPE,
		None
	};

	glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC) glXGetProcAddressARB( (const GLubyte *) "glXCreateContextAttribsARB");
	
	if (!glXCreateContextAttribsARB){
		cout << "Error getting gl functions address" << endl;
		
		XCloseDisplay(ctx->dpy);
		exit(1);
	}
	
	glc = glXCreateContextAttribsARB(ctx->dpy, fbc, 0, True, context_attribs);
	//glc = glXCreateContext(ctx->dpy, vi, NULL, GL_TRUE);
	
	if (glc == NULL){
		cout << "Failed to create a context" << endl;
		
		XCloseDisplay(ctx->dpy);
		exit(1);
	}
	
	if (verbose) {
		if (glXIsDirect(ctx->dpy, glc)){
			cout << "Direct GLX rendering context obtained" << endl;
		}else{
			cout << "Indirect GLX rendering context obtained" << endl;
		}
	}
	
	glXMakeCurrent(ctx->dpy, ctx->win, glc);

	if (debug >= 2) {
		GLint ext, n;
		const GLubyte *extension = NULL;

		extension = glGetString(GL_RENDERER);
		cout << extension << endl;

		glGetIntegerv(GL_NUM_EXTENSIONS, &n);
		cout << "Found " << n << " extensions" << endl;

		for (ext = 0; ext < n; ext++) {
			extension = glGetStringi(GL_EXTENSIONS, ext);
			cout << extension << endl;
		}
	}
}

void DestroyWindow(GraphicsContext *ctx) {
	glXMakeCurrent(ctx->dpy, None, NULL);
	glXDestroyContext(ctx->dpy, glc);
	
	XDestroyWindow(ctx->dpy, ctx->win);
	XCloseDisplay(ctx->dpy);
}

void EndDraw(GraphicsContext *ctx) {
	glXSwapBuffers(ctx->dpy, ctx->win);
}

void TreatEvents(GraphicsContext *ctx) {
	while (XPending(ctx->dpy) > 0){
		XNextEvent(ctx->dpy, &xev);
		
		switch (xev.type){
			case KeyPress:
				TreatKeyPress(&xev.xkey);
				break;
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
				TreatButtonPress(&xev.xbutton);
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

				if (TreatConfigureNotify != NULL) {
					TreatConfigureNotify(&xev);
				}
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

void NonFullscreen(GraphicsContext *ctx) {
	//Create a XEvent to tell the Window Manager to show the window as non-fullscreen
	memset(&xev, 0, sizeof(xev));
	xev.type = ClientMessage;
	xev.xclient.window = ctx->win;
	xev.xclient.message_type = XInternAtom(ctx->dpy, "_NET_WM_STATE", False);
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = 0;
	xev.xclient.data.l[1] = XInternAtom(ctx->dpy, "_NET_WM_STATE_FULLSCREEN", False);
	xev.xclient.data.l[2] = xev.xclient.data.l[3] = xev.xclient.data.l[4] = 0;
	
	//Send the XEvent just created
	XSendEvent (ctx->dpy, ctx->root, False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);
}

void Fullscreen(GraphicsContext *ctx) {
	//Create a XEvent to tell the Window Manager to show the window as fullscreen
	memset(&xev, 0, sizeof(xev));
	xev.type = ClientMessage;
	xev.xclient.window = ctx->win;
	xev.xclient.message_type = XInternAtom(ctx->dpy, "_NET_WM_STATE", False);
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = 1;
	xev.xclient.data.l[1] = XInternAtom(ctx->dpy, "_NET_WM_STATE_FULLSCREEN", False);
	xev.xclient.data.l[2] = xev.xclient.data.l[3] = xev.xclient.data.l[4] = 0;
	
	//Send the XEvent that was just created
	XSendEvent (ctx->dpy, ctx->root, False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);
}

FrameCounter *initFrameCounter() {
	FrameCounter * frameCounter = new FrameCounter{0};

	frameCounter->lastTime = getTime();
}

void updateFrameCounter(FrameCounter *frameCounter) {
	unsigned int currTime;
	unsigned int elapsedTime;

	currTime = getTime();
	elapsedTime = currTime - frameCounter->lastTime;

	frameCounter->lastTime = currTime;

	frameCounter->elapsedTime += elapsedTime;
	frameCounter->elapsedTimeTotal += elapsedTime;

	frameCounter->frameCount += 1;
	frameCounter->frameCountTotal += 1;
}
