#include <winsys.h>

#include <X11/Xlibint.h>

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

	XWindowAttributes winAttrs;
	unsigned int width, height;

	cout << "Trying to get Windows Geometry" << endl;
	XGetWindowAttributes(dpy, win, &winAttrs);
	width = winAttrs.width;
	height = winAttrs.height;

	char *data;

	data = (char *) malloc(sizeof(char) * width * height * 4);

	if (data == NULL) {
		cout << "Unable to allocate memory!";
		exit(1);
	}

	cout << "Visual masks" << endl << std::hex << vi->red_mask << endl << vi->green_mask << endl << vi->blue_mask << endl << std::dec;
	cout << "width: " << width << ", height: " << height << endl;
	cout << "byte_order: " << (dpy->byte_order == LSBFirst ? "LSBFirst" : "MSBFirst") << endl;
	cout << "bitmap_unit: " << (dpy->bitmap_unit) << endl;
	cout << "bitmap_bit_order: " << (dpy->bitmap_bit_order == LSBFirst ? "LSBFirst" : "MSBFirst") << endl;

	XImage image;

	image.width = width;
	image.height = height;
	image.xoffset = 0;
	image.format = ZPixmap;
	image.data = data;
	image.byte_order = MSBFirst;
	image.bitmap_unit = 32;
	image.bitmap_bit_order = MSBFirst;
	image.bitmap_pad = 32;
	image.depth = 24;
	image.bytes_per_line = 0;
	image.bits_per_pixel = 32;
	image.red_mask = vi->red_mask;
	image.green_mask = vi->green_mask;
	image.blue_mask = vi->blue_mask;
	image.obdata = NULL;

	XInitImage(&image);

	char *pixel = data;

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			pixel[0] = 0;
			pixel[1] = 0;
			pixel[2] = (char) x;
			pixel[3] = (char) y;

			pixel += 4;
		}
	}

	XClearWindow(dpy, win);

	XPutImage(dpy, win, gc, &image, 0, 0, 0, 0, width, height);

	loop = true;
	while(loop) {
		TreatEvents();
	}

	free(data);

	XFreeGC(dpy, gc);
	DestroyWindow();
	return 0;
}
