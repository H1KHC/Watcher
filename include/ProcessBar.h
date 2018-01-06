#ifndef __H_PROCESS_BAR__
#define __H_PROCESS_BAR__

#include <map>

#ifdef _DEBUG
#define pbPrivate public
#else
#define pbPrivate private
#endif

struct Cube {
	double color[3];
	void display(const int x, const int y, const int w, const int h, const double ratio = 1.0);
};

class ProcessBar {
pbPrivate:
	bool changeSizeAutomatically;
	ProcessBar *next, *last;
	int ID, coord[2], size[2];
	double total, now, target;
	Cube foreground, background;
	friend class ProcessBarHandler;
public:
	const int getID();
	void init(const double size, const int x, const int y, const int w, const int h, const Cube &fg, const Cube &bg, const bool changeSize);
	void update(const double process);
	void reSize(const double size);
	void display();
	ProcessBar();
};

#endif