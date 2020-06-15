#ifndef WINDOW_H
#define WINDOW_H


#include <stdlib.h>

typedef struct _BOUND {
	int x;
    int y;
    int width;
    int height;
} BOUND;

typedef struct _QNODE {
	int has_children;
	int hidden;
	BOUND bound;
	struct _QNODE * nw;
	struct _QNODE * ne;
	struct _QNODE * sw;
	struct _QNODE * se;
} QNODE;

typedef struct _FRAME {
	BOUND bound;
	char * title;
} FRAME;

typedef struct _CANVAS {
	BOUND bound;
	int * buffer;
} CANVAS;

typedef struct _VGA_WINDOW {
	int id;
	FRAME frame;
	CANVAS canvas;
    int color;
	QNODE * root;
	struct _VGA_WINDOW * next;
    struct _VGA_WINDOW * prev;
} VGA_WINDOW;

#endif /* WINDOW_H */
