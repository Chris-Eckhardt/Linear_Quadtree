#include "window.h"


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
    if(a->x > b->x)
        intr.x = a->x;
    else
        intr.x = b->x;

    // find y
    if(a->y > b->y)
        intr.y = a->y;
    else
        intr.y = b->y;

    // find width
    if((a->x+a->width) < (b->x+b->width))
        intr.width = (a->x+a->width)-intr.x;
    else
        intr.width = (b->x+b->width)-intr.x;

    // find height
    if((a->y+a->height) < (b->y+b->height))
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
    q->bound = create_bound(x,y,width,height);
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
    else if (q->has_children) {
        if (bound_contains(&(q->nw->bound), x, y))
            return search_qtree(q->nw, x, y);
        else if (bound_contains(&(q->ne->bound), x, y))
            return search_qtree(q->ne, x, y);
        else if (bound_contains(&(q->sw->bound), x, y))
            return search_qtree(q->sw, x, y);
        else if (bound_contains(&(q->se->bound), x, y))
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

    int offsetx = 0;
    int offsety = 0;
    if(w%2 == 1)
        offsetx = 1;
    if(h%2 == 1)
        offsety = 1;

    q->nw = create_qnode(x,y,w/2+offsetx,h/2+offsety);
    if(bound_intersects(&(q->nw->bound),&b))
        check_qnode(q->nw, b);

    q->ne = create_qnode(x+(w/2)+offsetx,y+offsety,w/2,h/2+offsety);
    if(bound_intersects(&(q->ne->bound),&b))
        check_qnode(q->ne, b);

    q->sw = create_qnode(x,y+(h/2)+offsety,w/2+offsetx,h/2);
    if(bound_intersects(&(q->sw->bound),&b))
        check_qnode(q->sw, b);

    q->se = create_qnode(x+(w/2)+offsetx,y+(h/2)+offsety,w/2,h/2);
    if(bound_intersects(&(q->se->bound),&b))
        check_qnode(q->se, b);

}

/* this is the equivalent of 'node insertion' but with 
one extra step to see if the node should be subdivided */
void check_qnode(QNODE * q, BOUND b)
{
    if(q->bound.width <= 1 || q->bound.height <= 1) {
        q->hidden = 1;
    }
    else if(bound_intersects(&(q->bound), &b)) {
        subdivide(q, b);
    }

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

/* helper function for adding a new window to the 
front of the circular linked-list */
WINDOW * get_last_window()
{
    if(!WINDOW_LIST)
        return NULL;
    WINDOW * ptr = WINDOW_LIST;
    while(ptr->next != WINDOW_LIST) {
        ptr = ptr->next;
    }
    return ptr;
}

/* used to add a new window to the global window list */
void add_window_to_list(WINDOW * w)
{
    if(!WINDOW_LIST) {
        WINDOW_LIST = w;
        w->next = w;
    } else {
        WINDOW * last = get_last_window();
        last->next = w;
        w->next = WINDOW_LIST;
        WINDOW_LIST = w;
    }
}

WINDOW * create_window(int x, int y, int width, int height)
{
    WINDOW * w = malloc( sizeof(WINDOW) );
    w->id = ++G_WINDOW_ID;
    w->root = create_qnode(x, y, width, height);
    add_window_to_list(w);
    return w;
}

/*************** END WINDOW *************/

/*************** TREE MANAGEMENT *************/

void build_quadtrees()
{
    if(!WINDOW_LIST)
        return;

    WINDOW * w_ptr = WINDOW_LIST->next;

    while(w_ptr != WINDOW_LIST) {
        reset_root(w_ptr->root);

        WINDOW * f_ptr = WINDOW_LIST;
        while(f_ptr != w_ptr) {
            check_qnode(w_ptr->root, f_ptr->root->bound);
            f_ptr = f_ptr->next;
        }
        w_ptr = w_ptr->next;
    }
}

/*************** END TREE MANAGEMENT *********/