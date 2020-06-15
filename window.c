#include "window.h"



int g_window_id 	= 0;

VGA_WINDOW * window_list_head;
VGA_WINDOW * window_list_tail;

/* global var used for defining a windows id */
int G_WINDOW_ID = 0;

/************** BOUND ****************/

BOUND create_bound(int x, int y, int width, int height)
{
    BOUND bound;
    bound.x = x;
    bound.y = y;
    bound.width = width;
    bound.height = height;
    return bound;
}

/* returns 1 if a bound contains a point,
0 if it does not */
int bound_contains(BOUND * b, int x, int y)
{
    if(x >= b->x && x < b->x+b->width
        && y >= b->y && y < b->y+b->height) {
        return 1;
    }
    return 0;
}

/* returns 1 if two bounds intersect at any point, 
0 if they do not*/
int bound_intersects(BOUND * a, BOUND * b)
{
    if(a->x >= (b->x+b->width) || b->x >= (a->x+a->width))
        return 0;

    if(a->y >= (b->y+b->height) || b->y >= (a->y+a->height))
        return 0;

    return 1;
}

/* not currently used but is useful for testing/debug */
BOUND get_intersection(BOUND * a, BOUND * b)
{
    BOUND intr = {0,0,0,0};

    // find x
    if(a->x >= b->x)
        intr.x = a->x;
    else
        intr.x = b->x;

    // find y
    if(a->y >= b->y)
        intr.y = a->y;
    else
        intr.y = b->y;

    // find width
    if((a->x+a->width) <= (b->x+b->width))
        intr.width = (a->x+a->width)-intr.x;
    else
        intr.width = (b->x+b->width)-intr.x;

    // find height
    if((a->y+a->height) <= (b->y+b->height))
        intr.height = (a->y+a->height)-intr.y;
    else
        intr.height = (b->y+b->height)-intr.y;

    // ret
    return intr;
}

/************** END BOUND ****************/

/************** QNODE ****************/

QNODE * create_qnode(int x, int y, int width, int height)
{
    QNODE * q = malloc( sizeof(QNODE) );
    q->hidden = 0;
    q->has_children = 0;
    q->bound.x = x;
	q->bound.y = y;
	q->bound.width = width;
	q->bound.height = height;
    q->ne = NULL;
    q->nw = NULL;
    q->se = NULL;
    q->sw = NULL;
    return q;
}

/* this is called before the quadtrees are rebuild */
void reset_root(QNODE * root)
{
    destroy_children(root);
    root->hidden = 0;
}

/* remove a node from the tree, 
must remove children first */
QNODE * destroy_qnode(QNODE * q)
{
    if(q) {
        destroy_children(q);
        free(q);
        q = NULL;
    }
    return q;
}

/* remove child nodes from a parent */
void destroy_children(QNODE * q)
{
    if(q->has_children) {
        if (q->nw)
            q->nw = destroy_qnode(q->nw);
        if (q->ne)
            q->ne = destroy_qnode(q->ne);
        if (q->se)
            q->se = destroy_qnode(q->se);
        if (q->sw)
            q->sw = destroy_qnode(q->sw);
        q->has_children = 0;
    }
}

/* recursive search through a quad tree and return 1 
if a node is 'hidden' and 0 if it is visible to the user */
int search_qtree(QNODE * q, int x, int y)
{
    if (q->hidden == 1) {
        return 1;
    }
    else if (q->has_children)
        {
            if (q->nw && bound_contains(&(q->nw->bound), x, y))
                return search_qtree(q->nw, x, y);
            if (q->ne && bound_contains(&(q->ne->bound), x, y))
                return search_qtree(q->ne, x, y);
            if (q->sw && bound_contains(&(q->sw->bound), x, y))
                return search_qtree(q->sw, x, y);
            if (q->se && bound_contains(&(q->se->bound), x, y))
                return search_qtree(q->se, x, y);
        }
    return 0;
}

/* subdivide a parent node into children and 
offset child bounds for intiger division */
void subdivide(QNODE * q, BOUND b)
{
    q->has_children = 1;
    int x = q->bound.x;
    int y = q->bound.y;
    int w = q->bound.width;
    int h = q->bound.height;

    BOUND nw = create_bound(x,y,(w/2),(h/2));
    BOUND ne = create_bound(x+(w/2),y,(w/2),(h/2));
    BOUND sw = create_bound(x,y+(h/2),(w/2),(h/2));
    BOUND se = create_bound(x+(w/2),y+(h/2),(w/2),(h/2));

    if(bound_intersects(&(nw),&b)) {
        q->nw = create_qnode(nw.x,nw.y,nw.width,nw.height);
        check_qnode(q->nw, b);
    }

    if(bound_intersects(&(ne),&b)) {
        q->ne = create_qnode(ne.x,ne.y,ne.width,ne.height);
        check_qnode(q->ne, b);
    }

    if(bound_intersects(&(sw),&b)) {
        q->sw = create_qnode(sw.x,sw.y,sw.width,sw.height);
        check_qnode(q->sw, b);
    }

    if(bound_intersects(&(se),&b)) {
        q->se = create_qnode(se.x,se.y,se.width,se.height);
        check_qnode(q->se, b);
    }

}

/* this is the equivalent of 'node insertion' but with 
one extra step to see if the node should be subdivided */
void check_qnode(QNODE * q, BOUND b)
{
    if(q->bound.width <= 1 || q->bound.height <= 1) {
        q->hidden = 1;
    }
    else if(!q->has_children && bound_intersects(&(q->bound), &b)) {
        subdivide(q, b);
    }
    else {
    if(!q->nw->hidden)
        if(bound_intersects(&(q->nw->bound), &b))
            check_qnode(q->nw, b);
    if(!q->ne->hidden)       
        if(bound_intersects(&(q->ne->bound), &b))
            check_qnode(q->ne, b);
    if(!q->sw->hidden)
        if(bound_intersects(&(q->sw->bound), &b))
            check_qnode(q->sw, b);
    if(!q->se->hidden)
        if(bound_intersects(&(q->se->bound), &b))
            check_qnode(q->se, b);
    }

    // after return from recursive calls
    
    if(q->has_children) {
        if(q->nw->hidden == 1 &&
            q->ne->hidden == 1 &&
            q->sw->hidden == 1 &&
            q->se->hidden == 1)
        {
            destroy_children(q);
            q->hidden = 1;
        }
    }
}

/************** END QNODE ****************/

/************ WINDOW ******************/

/* used to add a new window to the global window list */
void add_window_to_list(VGA_WINDOW * w)
{
    if(!window_list_head) {
        window_list_head = w;
        window_list_tail = w;
    } else {
        w->next = window_list_head;
        window_list_head->prev = w;
        window_list_head = w;
    }
}

void create_window ( PARAM_VGA_CREATE_WINDOW * params)
{
	VGA_WINDOW * window = malloc( sizeof(VGA_WINDOW) );
	window->id = g_window_id++;
	params->window_id = window->id;
	window->frame.bound.x = params->x-1;
	window->frame.bound.y = params->y-10;
	window->frame.bound.width = params->width+2;
	window->frame.bound.height = params->height+11;
	window->canvas.bound.x = params->x;
	window->canvas.bound.y = params->y;
	window->canvas.bound.width = params->width;
	window->canvas.bound.height = params->height;
    window->color = current_color++;

    int bound_size = 1;
    while (bound_size < window->frame.bound.width &&
        bound_size < window->frame.bound.height) 
        {
            bound_size *= 2;
        }

	window->root = create_qnode(
        window->frame.bound.x, 
        window->frame.bound.y, 
        bound_size, 
        bound_size);

    window->next = NULL;
    window->prev = NULL;
	add_window_to_list(window);
}

/*************** END WINDOW *************/

/*************** TREE MANAGEMENT *************/

void reset_qtrees()
{

    VGA_WINDOW * w_ptr = window_list_head;

    while(w_ptr != NULL){
        reset_root(w_ptr->root);
        w_ptr = w_ptr->next;
    }
}

void build_quadtrees()
{
    if(!window_list_tail)
        return;

    reset_qtrees();

    VGA_WINDOW * w_ptr = window_list_tail;
    VGA_WINDOW * f_ptr;

    while(w_ptr != NULL) {

        f_ptr = w_ptr->prev;
        while(f_ptr != NULL) {
            check_qnode(w_ptr->root, get_intersection(&(w_ptr->frame.bound), &(f_ptr->frame.bound)));
            f_ptr = f_ptr->prev;
        }
        w_ptr = w_ptr->prev;
    }
}
/*************** END TREE MANAGEMENT *********/
