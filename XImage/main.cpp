#include <winsys.h>

#include <sys/mman.h>
#include <X11/Xlibint.h>

#include <iostream>
#include <cstdlib>
#include <bitset>

using std::cout;
using std::endl;

void resize(XEvent *xev);
void resizeImage(XImage *image, unsigned int width, unsigned int height);

static GC gc;
static XImage image;
static unsigned int width, height;

int main() {
	CreateWindow();

	XGCValues GcValues;

	GcValues.fill_style = FillSolid;
	GcValues.foreground = 0xFF0000;

	gc = XCreateGC(dpy, win, GCForeground | GCFillStyle, &GcValues);

	XWindowAttributes winAttrs;

	cout << "Trying to get Windows Geometry" << endl;
	XGetWindowAttributes(dpy, win, &winAttrs);
	width = winAttrs.width;
	height = winAttrs.height;

	cout << "Visual masks" << endl << std::hex << vi->red_mask << endl << vi->green_mask << endl << vi->blue_mask << endl << std::dec;
	cout << "width: " << width << ", height: " << height << endl;
	cout << "byte_order: " << (dpy->byte_order == LSBFirst ? "LSBFirst" : "MSBFirst") << endl;
	cout << "bitmap_unit: " << (dpy->bitmap_unit) << endl;
	cout << "bitmap_bit_order: " << (dpy->bitmap_bit_order == LSBFirst ? "LSBFirst" : "MSBFirst") << endl;
	cout << "TreatConfigureNotify: " << (TreatConfigureNotify == NULL ? "NULL" : "Undefined") << endl;

	TreatConfigureNotify = resize;

	image.width = width;
	image.height = height;
	image.xoffset = 0;
	image.format = ZPixmap;
	image.data = NULL;
	image.byte_order = MSBFirst;
	image.bitmap_unit = 32;
	image.bitmap_bit_order = LSBFirst;
	image.bitmap_pad = 32;
	image.depth = 24;
	image.bytes_per_line = 0;
	image.bits_per_pixel = 32;
	image.red_mask = vi->red_mask;
	image.green_mask = vi->green_mask;
	image.blue_mask = vi->blue_mask;
	image.obdata = NULL;

	XClearWindow(dpy, win);

	resizeImage(&image, width, height);

	int offset = 0;

	loop = true;
	while(loop) {
		TreatEvents();

		char *pixel = image.data;

		for (int x = 0; x < height; x++) {
			for (int y = 0; y < width; y++) {
				pixel[0] = 0;
				pixel[1] = 0;
				pixel[2] = (char) x;
				pixel[3] = (char) y;

				pixel += 4;
			}
		}

		offset += 1;

		XPutImage(dpy, win, gc, &image, 0, 0, 0, 0, width, height);
	}

	munmap(image.data, sizeof(char) * image.width * image.height * 4);
	//free(image.data);

	XFreeGC(dpy, gc);
	DestroyWindow();
	return 0;
}

void resize(XEvent *xev) {
	width = xev->xconfigure.width;
	height = xev->xconfigure.height;

	resizeImage(&image, width, height);
}

void resizeImage(XImage *image, unsigned int width, unsigned int height) {
	char *data;

	if (image->data != NULL) {
		munmap(image->data, sizeof(char) * image->width * image->height * 4);
		//free(image->data);
	}

	data = (char *) mmap(NULL, sizeof(char) * width * height * 4, PROT_READ | PROT_WRITE,
						 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	//data = (char *) malloc(sizeof(char) * width * height * 4);

	if (data == NULL) {
		cout << "Unable to allocate memory!";
		exit(1);
	}

	image->width = width;
	image->height = height;
	image->data = data;
	image->bytes_per_line = 0;

	XInitImage(image);
}
