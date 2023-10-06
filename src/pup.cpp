#include <iostream>
extern "C" {
#include <string.h>
#include <xcb/xcb.h>
}
#include "util.hpp"

int main(int argc,char *argv[]){
    WM nc;

    nc.init_conn(&nc.dpy);

    nc.scr = xcb_setup_roots_iterator(xcb_get_setup(nc.dpy)).data;
    nc.root = nc.scr->root;
    
    switch(argc){
        case 1:
            util::annoy(error,"no command specified");
            nc.close_conn(nc.dpy);
            std::exit(1);
        case 2:
            break;
    }

    if (strncmp(argv[1],"mv",2)==0){
        if (!(argv[2] && argv[3])){
            util::annoy(error,"invalid args");
            nc.close_conn(nc.dpy);
            std::exit(1);
        }

        if (!argv[4]){
            util::annoy(error,"invalid args");
            nc.close_conn(nc.dpy);
            std::exit(1);
        }

        int a = atoi(argv[2]);
        int b = atoi(argv[3]);

        xcb_window_t win = strtoul(argv[4],NULL,16);
        xcb_get_geometry_reply_t *geo = xcb_get_geometry_reply(nc.dpy,xcb_get_geometry(nc.dpy,win),NULL);

        int x = geo->x + a;
        int y = geo->y + b;

        util::move_window(nc.dpy,strtoul(argv[4],NULL,16),x,y);
        free(geo);
    }

    if (strncmp(argv[1],"rs",2)==0){
        if (!(argv[2] && argv[3])){
            util::annoy(error,"invalid args");
            nc.close_conn(nc.dpy);
            std::exit(1);
        }

        if (!argv[4]){
            util::annoy(error,"invalid args");
            nc.close_conn(nc.dpy);
            std::exit(1);
        }

        int a = atoi(argv[2]);
        int b = atoi(argv[3]);

        xcb_window_t win = strtoul(argv[4],NULL,16);
        xcb_get_geometry_reply_t *geo = xcb_get_geometry_reply(nc.dpy,xcb_get_geometry(nc.dpy,win),NULL);

        int width = geo->width + a;
        int height = geo->height + b;

        util::resize_window(nc.dpy,strtoul(argv[4],NULL,16),width,height);
        free(geo);
    }


    if (strncmp(argv[1],"fw",2)==0){
        xcb_get_input_focus_reply_t *focus;
        focus = xcb_get_input_focus_reply(nc.dpy,xcb_get_input_focus(nc.dpy),NULL);

        if (focus == NULL){
        focuserr:
            util::annoy(warn,"focus not set");
            nc.close_conn(nc.dpy);
            std::exit(1);
        } else if (focus->focus == XCB_NONE) goto focuserr;

        printf("0x%08x\n",focus->focus);
        free(focus);
    }

    if (strncmp(argv[1],"fc",2)==0){
        if (!argv[2]){
            util::annoy(error,"invalid args");
            nc.close_conn(nc.dpy);
            std::exit(1);
        }

        util::set_focus(nc.dpy,strtoul(argv[2],NULL,16));
    }

    nc.close_conn(nc.dpy);
}
