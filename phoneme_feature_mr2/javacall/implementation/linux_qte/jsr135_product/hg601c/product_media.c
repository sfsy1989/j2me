
#ifdef __cplusplus
extern "C" {
#endif
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int product_media_initialize(void) {
    printf("product_media_initialize\n");
    return 0;
}

int product_media_finalize(void) {
    printf("product_media_finalize\n");
    return 0;
}


int product_media_start(char* media_file) {
    printf("product_media_start\n");
    return 0;
}


int product_media_stop(void) {
    printf("product_media_stop\n");
    return 0;
}

#ifdef __cplusplus
}
#endif 
 

