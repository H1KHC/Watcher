#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <GLFW\glfw3.h>
#include <gl\GL.h>
#include <stdio.h>
#include "ProcessBar.h"
#include "ProcessWatcher.h"
#include "stdafx.h"

inline void sSizeput(char * buf, double d) {
	if (d > 1024) {
		d /= 1024.0;
		if (d > 1024) {
			d /= 1024.0;
			if (d > 1024) {
				d /= 1024.0;
				sprintf(buf, "%6.2lfG", d);
			}
			else sprintf(buf, "%6.2lfM", d);
		}
		else sprintf(buf, "%6.2lfK", d);
	}
	else sprintf(buf, "%6.2lfB", d);
}

void display() {
	extern GLuint lists;
	extern ProcessWatcher *pw;
	char buf[16] = { 0 };
	int i;
	bool mouseOnCloseButton = false, mouseOnMovingButton = false;
	if (pw->mx > (NUM_OF_PROCESS_BARS * PROCESS_BAR_WIDTH)) {
		mouseOnCloseButton = pw->my <= (PROCESS_BAR_HEIGHT / 2);
		mouseOnMovingButton = !mouseOnCloseButton;
	}
	for (int i = 0; i < NUM_OF_PROCESS_BARS; ++i) {
		pw->pb[i].display();
		pw->dg[i].display();
	}
	glColor3d(0.5 * (mouseOnMovingButton + 1), 0.5 * (mouseOnMovingButton + 1), 0.0);
	glRectd(NUM_OF_PROCESS_BARS * PROCESS_BAR_WIDTH, 0, NUM_OF_PROCESS_BARS * PROCESS_BAR_WIDTH + PROCESS_BAR_HEIGHT / 2, PROCESS_BAR_HEIGHT / 2);
	glColor3d(0.5 * (mouseOnCloseButton + 1), 0.0, 0.5 * (mouseOnCloseButton + 1));
	glRectd(NUM_OF_PROCESS_BARS * PROCESS_BAR_WIDTH, PROCESS_BAR_HEIGHT / 2, NUM_OF_PROCESS_BARS * PROCESS_BAR_WIDTH + PROCESS_BAR_HEIGHT / 2, PROCESS_BAR_HEIGHT);
	glColor3d(1.0, 1.0, 1.0);
	glRasterPos2f(2.0f, PROCESS_BAR_HEIGHT - 12.0f);
	glCallList(lists + 'C');
	glRasterPos2f(PROCESS_BAR_WIDTH + 2.0f, PROCESS_BAR_HEIGHT - 12.0f);
	glCallList(lists + 'M');
//	glRasterPos2f(PROCESS_BAR_WIDTH * 2 + 2.0f, PROCESS_BAR_HEIGHT - 12.0f);
//	drawString("D");

	if (pw->cpu_usage >= 0) {
		sprintf(buf, "%6.2lf%%", pw->cpu_usage);
	} else sprintf(buf, "%6.2lf%%", 0.0);
	glRasterPos2f(3.0f, 2.0f);
	for(i = 0; i < 7; ++i)
		glCallList(lists + buf[i]);
		
	sSizeput(buf, pw->memory_usage);
	glRasterPos2f(81.0f, 2.0f);
	for(i = 0; i < 7; ++i)
		glCallList(lists + buf[i]);

//	sSizeput(buf, (double)pw->IO_size);
//	glRasterPos2f(163.0f, 2.0f);
//	for(i = 0; i < 7; ++i)
//		glCallList(lists + buf[i]);

	glRasterPos2f(NUM_OF_PROCESS_BARS * PROCESS_BAR_WIDTH + 6.0f, PROCESS_BAR_HEIGHT - 13.0f);glCallList(lists + 'x');
	glRasterPos2f(NUM_OF_PROCESS_BARS * PROCESS_BAR_WIDTH + 6.0f, 5.0f); glCallList(lists + '+');
}
