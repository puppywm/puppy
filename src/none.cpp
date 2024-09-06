#include <iostream>
#include <xcb/xcb.h>
#include <xcb/xproto.h>

#include "util.hpp"

int main(void){
    WM none;

    none.init_conn(&none.dpy);

    none.scr = xcb_setup_roots_iterator(xcb_get_setup(none.dpy)).data;
    none.root = none.scr->root;

    if (util::check_for_wm(none.dpy,none.root))
        none.close_conn(none.dpy);

    uint32_t vals[4];

    vals[0] = XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY;
    xcb_change_window_attributes_checked(none.dpy,none.root,XCB_CW_EVENT_MASK,vals);

    util::spawn("/bin/sxhkd");

    xcb_flush(none.dpy);

    while((none.ev = xcb_wait_for_event(none.dpy))){
        switch(none.ev->response_type & ~0x80){
            case XCB_EXPOSE: {
                break;
            }
            case XCB_MAP_REQUEST: {
                events::handle_map_request(none.dpy,none.ev);
                break;
            }
            default: {
                xcb_flush(none.dpy);
                break;
            }
        }
        free(none.ev);
    }

    xcb_flush(none.dpy);

    none.close_conn(none.dpy);
}
