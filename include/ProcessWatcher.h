#ifndef __H_PROCESSWATCHER__
#define __H_PROCESSWATCHER__

#include <Windows.h>
#include <GLFW\glfw3.h>
#include "ProcessBar.h"
#include "Diagram.h"
#include "stdafx.h"

typedef long long          int64_t;
typedef unsigned long long uint64_t;

struct CPUdata {
	int _processor;    //cpu数量    
	int64_t _last_time;         //上一次的时间    
	int64_t _last_system_time;

	uint64_t file_time_2_utc(const FILETIME* ftime) {
		LARGE_INTEGER li;
		li.LowPart = ftime->dwLowDateTime;
		li.HighPart = ftime->dwHighDateTime;
		return li.QuadPart;
	}

	int get_processor_number() {
		SYSTEM_INFO info;
		GetSystemInfo(&info);
		return info.dwNumberOfProcessors;
	}

	void init() {
		_last_system_time = 0;
		_last_time = 0;
	}

	CPUdata(DWORD ProcessID) {
		init();
		_processor = get_processor_number();
	}
	CPUdata() { init(); _processor = get_processor_number(); }
};

struct ProcessWatcher {
	GLFWwindow *window;
	int mx, my, wx, wy;

	int iProcess;
	HANDLE hProcess;

	DWORD exitcode;

	CPUdata usage;
	ProcessBar pb[NUM_OF_PROCESS_BARS];
	Diagram dg[NUM_OF_PROCESS_BARS];

	double cpu_usage;
	SIZE_T memory_usage;
	unsigned long long IO_size;

	double get_cpu_usage();
	SIZE_T get_memory_usage();
	unsigned long long get_IO_size();
	void update();
};

#endif
