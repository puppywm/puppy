#include <iostream>
extern "C" {
#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>
#include <xcb/xproto.h>
#include <unistd.h>
#include <string.h>
}
#include "util.hpp"

// WM {
xcb_connection_t *WM::dpy;
xcb_screen_t *WM::scr;
xcb_window_t WM::root;

xcb_generic_event_t *WM::ev;

void WM::init_conn(xcb_connection_t **c){
    *c = xcb_connect(NULL,NULL);
    if (xcb_connection_has_error(*c)){
        util::annoy(error,"unable to access display");
        std::exit(1);
    }
}

void WM::close_conn(xcb_connection_t *c){
    if (c != NULL){
        xcb_disconnect(c);
        std::exit(1);
    }
    std::exit(0);
}
// }

// util {
void util::annoy(annoy_t mode,const std::string message){
    switch(mode){
        case error:
            std::cout << "\033[031merror:\033[037m " << message << std::endl;
            return;
        case warn:
            std::cout << "\033[033mwarn:\033[037m " << message << std::endl;
            return;
    }
    return;
}

void util::spawn(const char *com){
    switch (fork()){
        case -1: {
        forkerr:
            util::annoy(error,"error with forks :o");
            return;
        }
        case 0: {
            execl(com,com,NULL);
            goto forkerr;
            return;
        }
    }
}

void util::set_focus(xcb_connection_t *c, xcb_window_t win){
    if (win){
        xcb_set_input_focus(c,XCB_INPUT_FOCUS_POINTER_ROOT,win,XCB_CURRENT_TIME);
        xcb_aux_sync(c);
    }
    else if(!win) util::annoy(error,"invalid window id");
}

void util::move_window(xcb_connection_t *c,xcb_window_t win,int x,int y){
    if (win){
        uint32_t vals[2];
        vals[0] = x;
        vals[1] = y;
        xcb_configure_window(c,win,XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y,vals);
        xcb_aux_sync(c);
    }
    else if(!win) util::annoy(error,"invalid window id");
}

int util::check_for_wm(xcb_connection_t *c,xcb_window_t root){
    xcb_intern_atom_cookie_t net_wm_name_cookie;
    net_wm_name_cookie = xcb_intern_atom(c,0,
        strlen("_NET_WM_NAME"),"_NET_WM_NAME");

    xcb_intern_atom_cookie_t net_supp_wm_check_cookie;
    net_supp_wm_check_cookie = xcb_intern_atom(c,0,
        strlen("_NET_SUPPORTING_WM_CHECK"),
        "_NET_SUPPORTING_WM_CHECK");

    xcb_intern_atom_reply_t *net_wm_name_reply;
    net_wm_name_reply = xcb_intern_atom_reply(c,net_wm_name_cookie,NULL);

    xcb_intern_atom_reply_t *net_supporting_wm_check_reply;
    net_supporting_wm_check_reply = xcb_intern_atom_reply(c,
        net_supp_wm_check_cookie,NULL);

    xcb_get_property_cookie_t name_cookie;
    name_cookie = xcb_get_property(c,0,
        root,net_wm_name_reply->atom,XCB_ATOM_STRING,0,1024);

    xcb_get_property_cookie_t supporting_wm_cookie;
    supporting_wm_cookie = xcb_get_property(c,0,root,
        net_supporting_wm_check_reply->atom,
         XCB_ATOM_WINDOW,0,1);

    xcb_get_property_reply_t *name_reply = xcb_get_property_reply(c,name_cookie,NULL);
    xcb_get_property_reply_t *supporting_wm_reply = xcb_get_property_reply(c,supporting_wm_cookie,NULL);

    if (name_reply && xcb_get_property_value_length(name_reply) > 0) {
        free(name_reply);
        free(supporting_wm_reply);
        annoy(error,"another wm is already running silly");
        return 1;
    } 
    else if (supporting_wm_reply && xcb_get_property_value_length(supporting_wm_reply) > 0) {
        free(name_reply);
        free(supporting_wm_reply);
        annoy(error,"another wm is already running silly");
        return 1;
    }
    return 0;
}
// }

// events {
void events::handle_map_request(xcb_connection_t *c,xcb_generic_event_t *ev){
    xcb_map_request_event_t *e = (xcb_map_request_event_t *)ev;
    xcb_window_t win = e->window;

    uint32_t vals[2];
    vals[0] = XCB_EVENT_MASK_FOCUS_CHANGE | XCB_EVENT_MASK_ENTER_WINDOW;
            
    vals[1] = 1;

    xcb_change_window_attributes_checked(c,win,XCB_CW_EVENT_MASK | XCB_CW_OVERRIDE_REDIRECT,vals);

    xcb_map_window(c,win);
    xcb_flush(c);
}

void events::handle_button_press(xcb_connection_t *c,xcb_generic_event_t *ev){
    xcb_button_press_event_t *e = (xcb_button_press_event_t *)ev;
    xcb_window_t win = e->child;
    
    uint32_t vals[1];
    vals[0] = XCB_STACK_MODE_ABOVE;
    xcb_configure_window(c,win,XCB_CONFIG_WINDOW_STACK_MODE,vals);
}
// }
