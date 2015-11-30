#ifndef __F_UTEST_H__
#define __F_UTEST_H__

#include <stdio.h>

#define u_assert(expr) \
do { \
    if (!(expr)) { \
        printf("error at line %d in %s!\n", __LINE__, __FUNCTION__); \
        exit(-1); \
    } \
} while(0)

#define u_assert_yes(string)  \
do { \
	printf("%s (y/n):", (string)); \
	int ch = getchar(); \
	getchar(); \
	u_assert(ch == 'Y' || ch == 'y'); \
} while(0)

typedef int (*f_tst_case_func) (void *arg);

typedef struct {
	char path[1024];
	void *func;
	void *arg;
} f_test_case_s;

#define F_MAX_TEST_CASES 100

int f_test_init(int argc, char **argv);
int f_test_run();
int f_test_add_data_func(char *path, f_tst_case_func case_func, void *arg);
unsigned int f_test_timer_last();

#endif

