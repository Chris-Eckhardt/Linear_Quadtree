#ifndef WINDOW_H
#define WINDOW_H


#include <stdlib.h>

/****************** BOUND ************/

typedef struct _BOUND {
	int x;
    int y;
    int width;
    int height;
} BOUND;

BOUND create_bound(int x, int y, int width, int height);

int bound_contains(BOUND * b, int x, int y);

int bound_intersects(BOUND * a, BOUND * b);

/*********** END BOUND ***************/

/************ QNODE ***************/

typedef struct _QNODE {
	int has_children;
	int hidden;
	BOUND bound;
	struct _QNODE * nw;
	struct _QNODE * ne;
	struct _QNODE * sw;
	struct _QNODE * se;
} QNODE;

QNODE * create_qnode(int x, int y, int width, int height);

QNODE * destroy_qnode(QNODE * q);

void destroy_children(QNODE * q);

void check_qnode(QNODE * q, BOUND b);

void subdivide(QNODE * q, BOUND b);

int search_qtree(QNODE * q, int x, int y);

/********** END QNODE *************/

/***************** WINDOW **************/

typedef struct _WINDOW {
	int id;
	QNODE * root;
	struct _WINDOW * next;
} WINDOW;

WINDOW * WINDOW_LIST;

WINDOW * create_window(int x, int y, int width, int height);

/************** END WINDOW *************/

/*************** TREE MANAGEMENT *************/

BOUND get_intersection(BOUND * a, BOUND * b);

void build_quadtrees();

/*************** END TREE MANAGEMENT *********/

#endif /* WINDOW_H */