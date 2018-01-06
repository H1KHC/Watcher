#include <Windows.h>
#include "Diagram.h"
#include <gl/GL.h>
#include "stdafx.h"

void Diagram::init(const double upboard, const int x, const int y, const int w, const int h, const double lineColor[3], const bool changeSize) {
	top = upboard;
	coord[0] = x, coord[1] = y, size[0] = w, size[1] = h;
	color[0] = lineColor[0], color[1] = lineColor[1], color[2] = lineColor[2];
	time = 0;
	changeSizeAutomatically = changeSize;
}

void Diagram::add(const double value) {
	pos[time = (time + 1) % PROCESS_BAR_WIDTH] = value;
	if (changeSizeAutomatically && pos[time] > top) top = pos[time];
}

void Diagram::display() {
	int i;
	glColor3dv(color);
	glBegin(GL_POINTS);
	for (i = 0; i < PROCESS_BAR_WIDTH; ++i) {
		glVertex2d(coord[0] + (double)i * size[0] / PROCESS_BAR_WIDTH, coord[1] + pos[(time + i + 1) % PROCESS_BAR_WIDTH] * size[1] / top);
//		if (max < pos[i]) max = i;
	}
	glVertex2d(coord[0] + PROCESS_BAR_WIDTH, coord[1] + pos[(time) % PROCESS_BAR_WIDTH] * size[1] / top);
//	if (changeSizeAutomatically && max / top < 0.8) top = max;
	glEnd();
}

Diagram::Diagram() {
	memset(pos, 0, sizeof(pos));
}


Diagram::~Diagram() {
}
