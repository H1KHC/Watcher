#include "ProcessBar.h"
#include <Windows.h>
#include <gl\GL.h>

#pragma comment(lib, "opengl32.lib")

ProcessBar::ProcessBar() : last(0), next(0), ID(0), now(0) {
}

const int ProcessBar::getID() {
	return ID;
}

void ProcessBar::init(const double size, const int x, const int y, const int w, const int h, const Cube &fg, const Cube &bg, const bool changeSize) {
	this->total = size ? size : 1;
	this->now = 0;
	this->coord[0] = x;
	this->coord[1] = y;
	this->size[0] = w;
	this->size[1] = h;
	this->foreground = fg;
	this->background = bg;
	changeSizeAutomatically = changeSize;
}
void ProcessBar::update(const double process) {
	target = process;
	if (changeSizeAutomatically && target > total) reSize(target);
}
void ProcessBar::reSize(const double size) {
	this->total = size ? size : 1;
}

void Cube::display(const int x, const int y, const int w, const int h, const double ratio) {
	glColor3dv(color);
	glRecti(x, y, x + w, y + h);
}
void ProcessBar::display() {
	extern bool smooth;
	now = smooth ? now * 0.9 + target * 0.1 : target;
	glPushMatrix();
	background.display(coord[0], coord[1], size[0], size[1]);
	foreground.display(coord[0], coord[1], (const int)((double)size[0] * now / total), size[1], (const int)((double)now / total));
	glPopMatrix();
}
