#include <winsys.h>

#include <iostream>
#include <cstdlib>
#include <bitset>

using std::cout;
using std::endl;

int main() {
	CreateWindow();

	Font font = XLoadFont(dpy, "-*-fixed-*-*-*-*-20-*-*-*-*-*-*-*");

	XGCValues GcValues;

	GcValues.fill_style = FillSolid;
	GcValues.foreground = 0xFF0000;
	GcValues.font = font;

	GC gc;

	gc = XCreateGC(dpy, win, GCForeground | GCFillStyle | GCFont, &GcValues);

	if (XGetGCValues(dpy, gc, GCFont, &GcValues) != 0) {
		std::bitset<32> font(GcValues.font);
		cout << font << endl;
	} else {
		exit(1);
	}

	/*
	XTextItem item;

	item.chars = "oi";
	item.nchars = 2;
	item.delta = 0;
	item.font = None;
	*/

	XClearWindow(dpy, win);

	XDrawString(dpy, win, gc, 20, 20, "X11 Font Testing", 16);

	loop = true;
	while(loop) {
		TreatEvents();
	}

	XFreeGC(dpy, gc);
	XUnloadFont(dpy, font);
	DestroyWindow();
	return 0;
}
