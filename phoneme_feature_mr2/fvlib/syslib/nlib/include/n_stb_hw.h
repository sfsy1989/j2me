#ifndef __HW_H__
#define __HW_H__




typedef enum {
    F_HW_NONE = 0,
    F_HW_150,       /*zg board*/
    F_HW_151,       /*his demo board*/
    F_HW_152,       /*fonsview board1*/
    F_HW_154,       /*fonsview board2*/
    F_HW_155,       /*fiber sample*/
} f_hw_e;

int n_load_hw_config();
int n_get_hw_version();

#endif
