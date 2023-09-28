#include <xcb/xcb.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "puppy.h"

xcb_connection_t *dpy;
xcb_screen_t *scr;

int main(int argc,char *argv[]){

    dpy = xcb_connect(NULL,NULL);
    if (xcb_connection_has_error(dpy)){
        printf("error while starting");
        return 1;
    }

    scr = xcb_setup_roots_iterator(xcb_get_setup(dpy)).data;

    switch (argc){
        case 0:
        case 1:
            printf("no arguments supplied, see -h for help\n");
            return 1;
        case 2:
            break;
    }

    if (!strcmp(argv[1],"mv")){
        if (!(argv[2] && argv[3])){
        mverror:
            printf("invalid arguments\n");
            return 1;
        }

        int newX = atoi(argv[2]);
        int newY = atoi(argv[3]);

        if (!argv[4])
            goto mverror;

        xcb_window_t wid = atoi(argv[4]);
       
        move_window(dpy,wid,newX,newY);
        return 0;
    }
}
