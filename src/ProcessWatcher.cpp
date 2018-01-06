#include "ProcessWatcher.h"
#include <Windows.h>
#include <GLFW\glfw3.h>
#include <Psapi.h>
#pragma comment(lib, "Psapi.lib")

double ProcessWatcher::get_cpu_usage() {
	FILETIME now;
	FILETIME creation_time;
	FILETIME exit_time;
	FILETIME kernel_time;
	FILETIME user_time;
	int64_t system_time;
	int64_t time;
	int64_t system_time_delta;
	int64_t time_delta;

	double cpu = -1;

	GetSystemTimeAsFileTime(&now);

	//判断进程是否已经退出  
	GetExitCodeProcess(hProcess, &exitcode);
	if (exitcode != STILL_ACTIVE) {
		return -1;
	}

	//计算占用CPU的百分比  
	if (!GetProcessTimes(hProcess, &creation_time, &exit_time, &kernel_time, &user_time))
	{
		return -1;
	}
	system_time = (usage.file_time_2_utc(&kernel_time) + usage.file_time_2_utc(&user_time))
		/ usage._processor;
	time = usage.file_time_2_utc(&now);

	//判断是否为首次计算  
	if ((usage._last_system_time == 0) || (usage._last_time == 0))
	{
		usage._last_system_time = system_time;
		usage._last_time = time;
		return -2;
	}

	system_time_delta = system_time - usage._last_system_time;
	time_delta = time - usage._last_time;

	if (time_delta == 0) {
		return -1;
	}

	cpu = (float)system_time_delta * 100 / (float)time_delta;
	usage._last_system_time = system_time;
	usage._last_time = time;
	return cpu_usage = cpu;
}

SIZE_T ProcessWatcher::get_memory_usage() {
	PROCESS_MEMORY_COUNTERS pmc;
	memory_usage = 0;
	if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
	{
		memory_usage = pmc.WorkingSetSize;
		return memory_usage;
	}
	return 1;
}

unsigned long long ProcessWatcher::get_IO_size() {
	static IO_COUNTERS now;
	static unsigned long long size = 0;
	return IO_size = size = now.ReadTransferCount + now.WriteTransferCount - size;
}

void ProcessWatcher::update() {
	double cpuUsage = get_cpu_usage();
	if (cpuUsage == -1) {
		extern GLFWwindow *window;
		glfwSetWindowShouldClose(window, 1);
		cpuUsage = 0;
	}
	else if (cpuUsage == -2) {
		//First run
		cpuUsage = 0;
	}

	pb[0].update(cpuUsage);
	dg[0].add(cpuUsage);
	pb[1].update(get_memory_usage());
	dg[1].add(memory_usage);
//	pb[2].update((double)get_IO_size());
//	dg[2].add((double)IO_size);
}
