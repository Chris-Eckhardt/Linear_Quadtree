#include <CUnit/Basic.h>
#include "CUnit/CUnit.h"
#include "CUnit/Automated.h"
#include <assert.h>
#include "window.h"

/* holds our testing values */
int test_data[100][20];

/* test for bound_contains() function */
void bound_test1()
{
    int pass_test_num = test_data[0][0];
    int fail_test_num = test_data[0][1];
    /* bound attributes */
    int x = test_data[1][0];
    int y = test_data[1][1];
    int w = test_data[1][2];
    int h = test_data[1][3];
    BOUND b = {x,y,w,h};
    /* run tests expected to pass */
    for(int i = 0; i < pass_test_num*2; i+=2) {
        int t_x = test_data[2][i];
        int t_y = test_data[2][i+1];
        CU_ASSERT_TRUE(bound_contains(&b, t_x, t_y));
    }
    /* run tests expected to fail */
    for(int i = 0; i < fail_test_num*2; i+=2) {
        int t_x = test_data[3][i];
        int t_y = test_data[3][i+1];
        CU_ASSERT_FALSE(bound_contains(&b, t_x, t_y));
    }
}

/* test for bounds_intersects() function */
void bound_test2()
{
    /* bound 1 attributes */
    int x1 = test_data[5][0];
    int y1 = test_data[5][1];
    int w1 = test_data[5][2];
    int h1 = test_data[5][3];
    
    /* bound 2 attributes */
    int x2 = test_data[6][0];
    int y2 = test_data[6][1];
    int w2 = test_data[6][2];
    int h2 = test_data[6][3];

    /* bound 3 attributes */
    int x3 = test_data[7][0];
    int y3 = test_data[7][1];
    int w3 = test_data[7][2];
    int h3 = test_data[7][3];

    BOUND b1 = {x1,y1,w1,h1};
    BOUND b2 = {x2,y2,w2,h2};
    BOUND b3 = {x3,y3,w3,h3};

    CU_ASSERT_TRUE(bound_intersects(&b1, &b2));
    CU_ASSERT_TRUE(bound_intersects(&b2, &b1));
    CU_ASSERT_FALSE(bound_intersects(&b1, &b3));
    CU_ASSERT_FALSE(bound_intersects(&b3, &b2));
}

/* test for creating and destroying QNODE's */
void qnode_test1()
{
    int x = test_data[9][0];
    int y = test_data[9][1];
    int w = test_data[9][2];
    int h = test_data[9][3];
    BOUND b = {x,y,w,h};
    QNODE * qn = NULL;
    qn = create_qnode(x,y,w,h);
    CU_ASSERT_NOT_EQUAL(qn, NULL);
    CU_ASSERT_EQUAL(qn->hidden, 0);
    CU_ASSERT_EQUAL(qn->has_children, 0);
    CU_ASSERT_EQUAL(qn->bound.x, x);
    CU_ASSERT_EQUAL(qn->bound.y, y);
    CU_ASSERT_EQUAL(qn->bound.width, w);
    CU_ASSERT_EQUAL(qn->bound.height, h);
    qn = destroy_qnode(qn);
    CU_ASSERT_EQUAL(qn, NULL);
}

/* test subdivision of parent node into children, then destroy the children */
void qnode_test2()
{
    int x1 = test_data[11][0];
    int y1 = test_data[11][1];
    int w1 = test_data[11][2];
    int h1 = test_data[11][3];
    int x2 = test_data[12][0];
    int y2 = test_data[12][1];
    int w2 = test_data[12][2];
    int h2 = test_data[12][3];
    BOUND b = {x1,y1,w1,h1};
    QNODE * qn = create_qnode(x2,y2,w2,h2);
    subdivide(qn, b);
    CU_ASSERT_NOT_EQUAL(qn->ne, NULL);
    CU_ASSERT_NOT_EQUAL(qn->nw, NULL);
    CU_ASSERT_NOT_EQUAL(qn->se, NULL);
    CU_ASSERT_NOT_EQUAL(qn->sw, NULL);
    destroy_children(qn);
    CU_ASSERT_EQUAL(qn->ne, NULL);
    CU_ASSERT_EQUAL(qn->nw, NULL);
    CU_ASSERT_EQUAL(qn->se, NULL);
    CU_ASSERT_EQUAL(qn->sw, NULL);
    destroy_qnode(qn);
}

/* test create_window() */
void qtree_test1()
{
    int x = test_data[9][0];
    int y = test_data[9][1];
    int w = test_data[9][2];
    int h = test_data[9][3];
    WINDOW * wnd = NULL;
    wnd = create_window(x,y,w,h);
    CU_ASSERT_NOT_EQUAL(wnd, NULL);
    CU_ASSERT_NOT_EQUAL(wnd->id, 0);
    CU_ASSERT_EQUAL(wnd->next, wnd);
}

/* test build_quadtrees() */
void qtree_test2()
{
    int x1 = test_data[11][0];
    int y1 = test_data[11][1];
    int w1 = test_data[11][2];
    int h1 = test_data[11][3];
    int x2 = test_data[12][0];
    int y2 = test_data[12][1];
    int w2 = test_data[12][2];
    int h2 = test_data[12][3];
    WINDOW * wnd1 = create_window(x1,y1,w1,h1);
    WINDOW * wnd2 = create_window(x2,y2,w2,h2);
    CU_ASSERT_EQUAL(wnd1->id, 2);
    CU_ASSERT_EQUAL(wnd2->id, 3);
    CU_ASSERT_EQUAL(WINDOW_LIST, wnd2);
    CU_ASSERT_EQUAL(WINDOW_LIST->next, wnd1);
    /* skip window from previous test, window destructor not implemented */
    CU_ASSERT_EQUAL(WINDOW_LIST->next->next->next, wnd2);
    /* build quadtrees */
    build_quadtrees();
    /* the top window's qtree root (wnd2->root) will have no children, 
    wnd1->root will have children */
    CU_ASSERT_EQUAL(wnd2->root->has_children, 0);
    CU_ASSERT_EQUAL(wnd1->root->has_children, 1);
    /* check the final window for children (window from previous test) */
    CU_ASSERT_EQUAL(wnd1->next->root->has_children, 1);
    /* the next window should be the top (wnd2) and should again, have NO children */
    CU_ASSERT_EQUAL(wnd1->next->next->root->has_children, 0);
}

/* now to test that the nodes are being created properly (ie match the overlapping regions of windows)
to test this we will call search_tree */
void qtree_test3()
{
    int x1 = test_data[14][0];
    int y1 = test_data[14][1];
    int w1 = test_data[14][2];
    int h1 = test_data[14][3];
    int x2 = test_data[15][0];
    int y2 = test_data[15][1];
    int w2 = test_data[15][2];
    int h2 = test_data[15][3];

    /* the BOUND definition the intersecting region 
    of these windows is: { 20, 20, 60, 30 } */

    BOUND b = { 20, 20, 60, 30 }; // the search will be tested agains this bound
    WINDOW * wnd1 = create_window(x1,y1,w1,h1);
    WINDOW * wnd2 = create_window(x2,y2,w2,h2);

    /* REMEMBER TO BUILD THE QTREES */
    build_quadtrees();

    /* wnd1's qtree will be just the root with no 
    children because it is on top. we will search 
    wnd2's tree for the nodes coresponding to the 
    instersecting bound */

    for(int y = b.y; y < b.y+b.height; y++) {
        for(int x = b.x; x < b.x+b.width; x++) {
            CU_ASSERT_EQUAL(search_qtree(wnd1->root, x, y), 1);
        }
    }

    /* now we will test a few that should lie outside 
    the intersecting region */
    int fail_test_num = test_data[16][0];
    for(int i = 0; i < fail_test_num*2; i+=2) {
        int t_x = test_data[17][i];
        int t_y = test_data[17][i+1];
        CU_ASSERT_FALSE(search_qtree(wnd1->root, t_x, t_y));
    }
}

int read_data()
{
    FILE *fd = fopen("data.csv", "r");
    int row = 0;

    if (!fd) {
        printf("Can't open file\n");
        return 0;
    }

    char buf[1024];

    while (fgets(buf, 1024, fd)) {
        int col = 0;

        char * i = strtok(buf, ",");
        while (i) {
            test_data[row][col] = atoi(i);
            i = strtok(NULL, ",");
            col++;
        }
        row++;
    }

    fclose(fd);

    return 1;
}

int suite_success_init(void) { return 0; }
int suite_success_clean(void) { return 0; }

CU_TestInfo bound_testcases[] = {
        {"Test1:", bound_test1},
        {"Test2:", bound_test2},
        CU_TEST_INFO_NULL
};

CU_TestInfo qnode_testcases[] = {
        {"Test1:", qnode_test1},
        {"Test2:", qnode_test2},
        CU_TEST_INFO_NULL
};

CU_TestInfo qtree_testcases[] = {
        {"Test1:", qtree_test1},
        {"Test2:", qtree_test2},
        {"Test3:", qtree_test3},
        CU_TEST_INFO_NULL
};

CU_SuiteInfo suites[] = {
        {"Testing bound functions: ", suite_success_init, suite_success_clean, NULL, NULL, bound_testcases},
        {"Testing qnode functions: ",suite_success_init, suite_success_clean, NULL, NULL, qnode_testcases},
        {"Testing quadtree: ",suite_success_init, suite_success_clean, NULL, NULL, qtree_testcases},
        CU_SUITE_INFO_NULL
};

void add_tests()
{
    assert(NULL != CU_get_registry());
    assert(!CU_is_test_running());

    if(CUE_SUCCESS != CU_register_suites(suites)){
            fprintf(stderr, "Register suites failed - %s ", CU_get_error_msg());
            exit(1);
    }
}

int main()
{
    if(!read_data())
        return 0;
    
    if(CU_initialize_registry()) {
        fprintf(stderr, " Initialization of Test Registry failed. ");
        exit(1);
    } else {
        add_tests();
        CU_set_output_filename("TestResults");
        CU_list_tests_to_file();
        CU_automated_run_tests();
        CU_cleanup_registry();
    }
    return 0;

}