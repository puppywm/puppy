#include <iostream>
extern "C"{
#include <xcb/xcb.h>
#include <xcb/xproto.h>
}
#include "util.hpp"

int main(void){
    WM puppy;

    puppy.init_conn(&puppy.dpy);

    puppy.scr = xcb_setup_roots_iterator(xcb_get_setup(puppy.dpy)).data;
    puppy.root = puppy.scr->root;

    if (util::check_for_wm(puppy.dpy,puppy.root))
        puppy.close_conn(puppy.dpy);

    uint32_t vals[4];

    vals[0] = XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY   | XCB_EVENT_MASK_STRUCTURE_NOTIFY |
              XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | XCB_EVENT_MASK_PROPERTY_CHANGE;
    xcb_change_window_attributes(puppy.dpy,puppy.root,XCB_CW_EVENT_MASK,vals);

    util::spawn("/bin/sxhkd");

    xcb_grab_button(puppy.dpy,0,puppy.root,XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE,
                    XCB_GRAB_MODE_ASYNC,XCB_GRAB_MODE_ASYNC,puppy.root,XCB_NONE,1,XCB_NONE);

    xcb_flush(puppy.dpy);

    while((puppy.ev = xcb_wait_for_event(puppy.dpy))){
        switch(puppy.ev->response_type & ~0x80){
            case XCB_MAP_REQUEST: {
                events::handle_map_request(puppy.dpy,puppy.ev);
                break;
            }
            case XCB_BUTTON_PRESS: {
                events::handle_button_press(puppy.dpy, puppy.ev);
                break;
            }
            case XCB_CONFIGURE_REQUEST: {
                events::handle_configure_request(puppy.dpy,puppy.ev);
                break;
            }
            default: {
                util::annoy(warn,"unhandled event type, break event loop..");
                break;
            }
        }
        free(puppy.ev);
    }

    xcb_flush(puppy.dpy);

    puppy.close_conn(puppy.dpy);
}
