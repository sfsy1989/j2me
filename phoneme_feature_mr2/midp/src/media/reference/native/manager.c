#include <sni.h>
#include <stdio.h>

KNIEXPORT  KNI_RETURNTYPE_VOID
KNIDECL(javax_microedition_media_Manager_createPlayer0) {
        printf("=====enter javax_microedition_media_manager_creatPlayer0\n");
        int test = KNI_GetParameterAsInt(2);
        printf("=====createPlayer0 test = %d \n",test);

        
        KNI_ReturnVoid();
}

