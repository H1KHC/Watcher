#ifndef __H_DIAGRAM__
#define __H_DIAGRAM__
#include "stdafx.h"
class Diagram {
	bool changeSizeAutomatically;
	int coord[2], size[2], time;
	double top, pos[PROCESS_BAR_WIDTH], color[3];
public:
	void init(const double upboard, const int x, const int y, const int w, const int h, const double lineColor[3], const bool changeSize);
	void add(const double value);
	void display();
	Diagram();
	~Diagram();
};

#endif // __H_DIAGRAM__