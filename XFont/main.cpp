#include <winsys.h>

#include <iostream>
#include <cstdlib>
#include <bitset>

using std::cout;
using std::endl;

int main() {
	CreateWindow();

	XGCValues GcValues;

	GcValues.fill_style = FillSolid;
	GcValues.foreground = 0xFF0000;

	GC gc;

	gc = XCreateGC(dpy, win, GCForeground | GCFillStyle, &GcValues);

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

	XDrawString(dpy, win, gc, 20, 20, "oi", 2);

	loop = true;
	while(loop) {
		TreatEvents();
	}
	DestroyWindow();
	return 0;
}
