#include <Windows.h>
#include <GLFW\glfw3.h>
#include "ProcessBar.h"
#include "ProcessWatcher.h"
#include "stdafx.h"
#include <stdio.h>

bool ld, move;

void mouseReceiver(GLFWwindow* window, int button, int action, int mods) {
	extern ProcessWatcher *pw;
	static bool toClose = false;
	#ifdef _DEBUG
	printf("Cursor Click Receiver:\tBotton = %2d Action = %2d\n", button, action);
	fflush(stdout);
	#endif
	switch (button) {
		case GLFW_MOUSE_BUTTON_LEFT:
			switch (action) {
				case GLFW_PRESS:
					ld = true;
					if (pw->mx > NUM_OF_PROCESS_BARS * PROCESS_BAR_WIDTH) {
						if (pw->my > PROCESS_BAR_HEIGHT / 2) {
							move = true;
						} else {
							toClose = true;
						}
					}
					break;
				case GLFW_RELEASE:
					ld = false;
					if (move) move = false;
					else if (toClose) {
						if (pw->mx > NUM_OF_PROCESS_BARS * PROCESS_BAR_WIDTH && pw->my <= PROCESS_BAR_HEIGHT / 2) {
							glfwSetWindowShouldClose(window, 1);
						}
						toClose = false;
					}
					break;
			}
	}
}

void cursorMoveReceiver(GLFWwindow* window, double xpos, double ypos) {
	extern ProcessWatcher *pw;
	if (move) {
		pw->wx += (int)xpos - pw->mx;
		pw->wy += (int)ypos - pw->my;
		glfwSetWindowPos(window, pw->wx, pw->wy);
	} else {
		pw->mx = (int)xpos, pw->my = (int)ypos;
		if(pw->mx >= (PROCESS_BAR_WIDTH * NUM_OF_PROCESS_BARS)){
			if(WINDOW_WIDTH <= pw->mx + 2 || pw->my <= 2 || WINDOW_HEIGHT <= pw->my + 2)
				pw->mx = pw->my = 0;
		}
		#ifdef _DEBUG
		printf("Cursor Move Receiver:\tmx = %4d my = %4d\n", pw->mx, pw->my);
		fflush(stdout);
		#endif
	}
}
