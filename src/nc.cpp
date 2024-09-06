#include <iostream>
#include <string.h>
#include <xcb/xcb.h>

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

	if (strncmp(argv[1],"-h",2)==0){
		printf("nc mv <+x> <+y> <win_id> : add x and y to the position of win_id\n");
		printf("nc fc <win_id> : focus <win_id>\n");
		printf("nc gf : prints the id of the currently focused window\n");
		printf("nc ls : lists all window ids\n");
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

    if (strncmp(argv[1],"fc",2)==0){
        if (!argv[2]){
            util::annoy(error,"invalid args");
            nc.close_conn(nc.dpy);
            std::exit(1);
        }

        util::set_focus(nc.dpy,strtoul(argv[2],NULL,16));
    }

	if (strncmp(argv[1],"gf",2)==0){
		xcb_window_t focused_win = util::get_focus(nc.dpy);
		printf("0x%08x\n",focused_win);
	}

	if (strncmp(argv[1],"ls",2)==0){
		util::list_windows(nc.dpy);
	}

    nc.close_conn(nc.dpy);
}
