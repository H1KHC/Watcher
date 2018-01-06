#include <Windows.h>
#include <tlhelp32.h>
#include <GLFW\glfw3.h>
#include <stdio.h>
#include "ProcessWatcher.h"
#include "stdafx.h"

#define GET_EXIT_CODE_FAILED	0xFFFFFFFD
#define CREATE_WINDOW_FAILED	0xFFFFFFFE
#define OPEN_PROCESS_FAILED		0xFFFFFFFF

#ifdef _DEBUG
#pragma comment(lib, "GLFW3D.lib")
#else
#pragma comment(lib, "GLFW3.lib")
#endif

ProcessWatcher *pw;
GLuint lists;
GLFWwindow *window;
HWND hWnd;
bool NT50, smooth = FRAMES_PER_SECOND > 10;

inline bool TestVersion() {
    OSVERSIONINFO osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    if (!GetVersionEx(&osvi)) {
        return false;
    }
	NT50 = osvi.dwMajorVersion == 5;
	return true;
}

BOOL EnableDebugPrivilege() {
	HANDLE hToken;
	BOOL fOk = FALSE;
	if(OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES,&hToken)) { //Get Token
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount=1;
		if(!LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&tp.Privileges[0].Luid))//Get Luid
			printf("Can't lookup privilege value.\n"),
			fflush(stdout);
		tp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
		if(!AdjustTokenPrivileges(hToken,FALSE,&tp,sizeof(tp),NULL,NULL))//Adjust Token
			printf("Can't adjust privilege value.\n"),
			fflush(stdout);
		fOk=(GetLastError()==ERROR_SUCCESS);
		CloseHandle(hToken);
	}
	return fOk;
}

inline int initProcessWatcher(const int procID) {
	Cube fg, bg;
	pw = new ProcessWatcher;
	pw->window = window;
	pw->iProcess = procID;
	pw->hProcess = 
		OpenProcess(
		NT50 ? PROCESS_QUERY_INFORMATION : PROCESS_QUERY_LIMITED_INFORMATION,
		false,
		procID);
		
	if (!pw->hProcess) return OPEN_PROCESS_FAILED;
	else{
		DWORD exitcode = 0;
		if(!GetExitCodeProcess(pw->hProcess, &exitcode) || exitcode != STILL_ACTIVE) {
			return GET_EXIT_CODE_FAILED;
		}
	}

	double Color[][3][3] = {
		{ { 0.8, 0.4, 0.0 },{ 0.4, 0.2, 0.0 },{ 1.0, 0.5, 0.0 } },
		{ { 0.0, 0.8, 0.0 },{ 0.0, 0.4, 0.0 },{ 0.0, 1.0, 0.0 } },
		{ { 0.0, 0.4, 0.8 },{ 0.0, 0.2, 0.4 },{ 0.0, 0.5, 1.0 } }
	};
	double size[] = { 100, 50 << 20, 1 << 20 };
	bool changeSize[][2] = {
		{false, false},
		{true, true},
		{true, true}
	};

	fg.color[0] = 0.8; fg.color[1] = 0.4; fg.color[2] = 0.0;
	bg.color[0] = 0.4; bg.color[1] = 0.2; bg.color[2] = 0.0;
	pw->pb[0].init(size[0], 0, 0, PROCESS_BAR_WIDTH, PROCESS_BAR_HEIGHT, fg, bg, changeSize[0][0]);
	pw->dg[0].init(size[0], 0, 0, PROCESS_BAR_WIDTH, PROCESS_BAR_HEIGHT, Color[0][2], changeSize[0][1]);
	fg.color[0] = 0.0;
	fg.color[1] = 0.8;
	fg.color[2] = 0.0;
	bg.color[0] = Color[1][1][0];
	bg.color[1] = Color[1][1][1];
	bg.color[2] = Color[1][1][2];
	pw->pb[1].init(size[1], PROCESS_BAR_WIDTH, 0, PROCESS_BAR_WIDTH, PROCESS_BAR_HEIGHT, fg, bg, changeSize[1][0]);
	pw->dg[1].init(size[1], PROCESS_BAR_WIDTH, 0, PROCESS_BAR_WIDTH, PROCESS_BAR_HEIGHT, Color[1][2], changeSize[1][1]);
	return 0;
}

inline void initOpenGL() {
	void mouseReceiver(GLFWwindow* window, int button, int action, int mods);
	void cursorMoveReceiver(GLFWwindow* window, double xpos, double ypos);
	glfwMakeContextCurrent(window);

	glfwSetMouseButtonCallback(window, mouseReceiver);
	glfwSetCursorPosCallback(window, cursorMoveReceiver);

	glfwSwapInterval(60 / FRAMES_PER_SECOND);

	glLoadIdentity();
	glOrtho(0, NUM_OF_PROCESS_BARS * PROCESS_BAR_WIDTH + PROCESS_BAR_HEIGHT / 2, 0, PROCESS_BAR_HEIGHT, -1, 1);

	glfwSetWindowPos(window, pw->wx = 200, pw->wy = 200);
	lists = glGenLists(127);
	wglUseFontBitmaps(wglGetCurrentDC(), 0, 127, lists);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glHint(GL_LINE_SMOOTH, GL_NICEST);
}

inline bool initWindowStyle(const int windowAlpha) {
	SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW);
	if(NT50) return true;
	LONG nRet = GetWindowLong(hWnd, GWL_EXSTYLE);
	nRet = nRet | WS_EX_LAYERED;
	SetWindowLong(hWnd, GWL_EXSTYLE, nRet);
	return SetLayeredWindowAttributes(hWnd, 0, windowAlpha, LWA_ALPHA);
}

int watcher(const int procID, const int windowAlpha) {
	void display();
	int i;
	char WindowName[16];
	sprintf(WindowName, "Watcher-%5d", procID);

	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WindowName, NULL, NULL);

	if (!window) return CREATE_WINDOW_FAILED;
	
	if(i = initProcessWatcher(procID)) return i;
	initOpenGL();
	hWnd = FindWindow(0, WindowName);
	if(initWindowStyle(windowAlpha)) {
		DWORD err = GetLastError();
		printf("WARN:SET_WINDOW_STYLE_FAILED\n");
		if(err) printf("ERRNO:%d\n", err);
		fflush(stdout);
	}

	i = FRAMES_PER_SECOND - 1;
	while (!glfwWindowShouldClose(window)) {
		#ifdef _DEBUG
		printf("%d\n", i);
		#endif
		if(++i == FRAMES_PER_SECOND)
			i = 0, pw->update();
		display();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	return 0;
}

inline bool procNameCompare(const char *a, const char *b) {
	while (true) {
		if ((*a & 0xdf) != (*b & 0xdf)) return false;
		if (*(++a) == 0) {
			if (*(++b) == 0) return true;
			else return false;
		}
		if (*(++b) == 0) {
			return !strcmp(a, ".exe");
		}
	}
}

DWORD GetProcessIdByName(LPCTSTR pName) {
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot) {
		return -1;
	}
	PROCESSENTRY32 pe = { sizeof(pe) };
	BOOL fOk;
	for (fOk = Process32First(hSnapshot, &pe); fOk; fOk = Process32Next(hSnapshot, &pe)) {
		if (procNameCompare(pe.szExeFile, pName)) {
			CloseHandle(hSnapshot);
			return pe.th32ProcessID;
		}
	}
	return -1;
}

int main(int argc, char *argv[]) {
	int i = -1, alpha = 192;
	char *buf = argv[1];
	
	if(!TestVersion()) {
		printf("WARN:TestVersion failed, issues may occur.");
		fflush(stdout);
	}
	if(NT50 && !EnableDebugPrivilege()) {
		printf("WARN:EnableDebugPrivilege failed, issues may occur.");
		fflush(stdout);
	}

#ifdef _DEBUG
	i = GetCurrentProcessId();
#else
	if (argc == 3) {
		if (!sscanf(argv[2], "%d", &alpha)) alpha = 192;
	}
	if (argc == 1) {
		buf = new char[128];
		printf("Please input the id or name of the process you'll watch:");
		fflush(stdout);
		scanf("%[^\n]s", buf);
	}
	if(!strcmp(buf, "help")) {
		puts(
		"Usage: watcher PID/NAME [Alpha]\n"
		);
		fflush(stdout);
	}
	if(!strcmp(buf, "this")) i = GetCurrentProcessId();
	else {
		for(i = strlen(buf) - 1; i >= 0; --i){
			if((unsigned)(buf[i] - '0') > 9){
				i = GetProcessIdByName(buf);
				buf[0] = '\0';
				break;
			}
		}
		if(buf[0]) sscanf(buf, "%d", &i);
	}
#if HIDE_COMMAND_WINDOW == 1
	if(argc == 1)ShowWindow(FindWindow("ConsoleWindowClass", NULL), SW_HIDE);
#endif
#endif
	if (i == -1 || !glfwInit()) {
		exit(EXIT_FAILURE);
	}
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_DECORATED, GL_FALSE);
	glfwWindowHint(GLFW_DEPTH_BITS, 0);
	
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

	if(i = watcher(i, alpha)) {
		switch(i) {
		case GET_EXIT_CODE_FAILED:
			printf("ERRN:GET_EXIT_CODE_FAILED\n");
			MessageBox(0, __TEXT("GET_EXIT_CODE_FAILED"), __TEXT("ERRN"), MB_ICONERROR | MB_OK);
		break;
		case CREATE_WINDOW_FAILED:
			printf("ERRN:CREATE_WINDOW_FAILED\n");
			MessageBox(0, __TEXT("CREATE_WINDOW_FAILED"), __TEXT("ERRN"), MB_ICONERROR | MB_OK);
		break;
		case OPEN_PROCESS_FAILED:
			printf("ERRN:OPEN_PROCESS_FAILED\n");
			MessageBox(0, __TEXT("OPEN_PROCESS_FAILED"), __TEXT("ERRN"), MB_ICONERROR | MB_OK);
		break;
		}
	}
	
	DWORD err = GetLastError();
	if(err) printf("ERRNO:%d\n", err);

	glfwTerminate();

	return 0;
}
