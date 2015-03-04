#ifndef	_WINSYS_H
#define	_WINSYS_H

void CreateWindow();
void DestroyWindow();
void EndDraw();
void TreatEvents();
void msleep(unsigned int msec);
unsigned int getTime();
void NonFullscreen();

#endif //_WINSYS_H
