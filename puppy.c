/*
 *  hiya there! I just wanted to warn you before you start
 *  using my unstable and (possibly unsafe) window manager..
 *
 *  D:
 *
 *  keep in mind i'm only 13 and I don't know squat about
 *  using C either. (im not following a stupid tutorial)
 *
 *  I actually recommend you don't use it at all
 *  
 */

#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "puppy.h"

xcb_connection_t *dpy;
xcb_screen_t *scr;
// xcb_ewmh_connection_t ewmh;
xcb_window_t root;

xcb_generic_event_t *ev;

int main(int argc,char **argv){
    // initialization and checks
    init_conn(&dpy);

    scr = xcb_setup_roots_iterator(xcb_get_setup(dpy)).data;
    if(!scr){
        throwerr("screen problemo idk fix"); 
        xcb_disconnect(dpy);}

    root = scr->root;
    check_for_wm(dpy,scr,root);

    xcb_change_property_checked(
        dpy,XCB_PROP_MODE_REPLACE,root,
        XCB_ATOM_WM_NAME,XCB_ATOM_STRING,
        8,strlen(WM_NAME),WM_NAME);

    uint32_t val;
    //uint32_t vals[3];

    // the magic!!!
    val = XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT;
    xcb_change_window_attributes_checked(dpy,root,XCB_CW_EVENT_MASK,&val);

    // lmao hard coded moment
    if(spawn("sxhkd"))
        printf("warn: unable to spawn sxhkd, do you have it installed?\n\
                (nothing will work without it btw)[unless you have something else in mind?]");

    xcb_flush(dpy);

    // event loop
    while((ev = xcb_wait_for_event(dpy))){
        switch(ev->response_type & ~0x80){
            case XCB_EXPOSE: { // I forgot what I was using this for
                break;
            }
            case XCB_MAP_REQUEST: {
                map_request(dpy,scr,ev);
                break;
            }
            case XCB_ENTER_NOTIFY: {
                mouse_enter(dpy,ev);
                break;
            }
            case XCB_LEAVE_NOTIFY: {
                mouse_leave(dpy,ev);
                break;
            }
            default: {
                xcb_flush(dpy);
                break;
            }
        }
        free(ev);
    }

    xcb_flush(dpy);

    // xcb_ewmh_connection_wipe(&ewmh);
    close_conn(dpy);
}
