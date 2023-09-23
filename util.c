/*
 * 
 * i'm never annotating this code, so if anyone actually wants
 * to learn from this, don't, use something else, save yourself
 * from goofy ass code that might even be redundant.
 *
 */

#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "puppy.h"

void throwerr(const char* error) {
    fprintf(stderr,"\033[31merr\033[0m: %s\n",error);
    exit(1);
}

int spawn(const char *com){
    if(fork() < 0){
        return 1;
    } else if(fork() == 0){
        execlp((char *)com,(char *)com,NULL);
        return 0;
    }
    return 1;
}

void init_conn(xcb_connection_t **c){
    *c = xcb_connect(NULL,NULL);
    if(xcb_connection_has_error(*c)){
        throwerr("unable to access display");
        exit(1);
    }
}

void close_conn(xcb_connection_t *c){
    if(c != NULL){
        xcb_disconnect(c);
        exit(1);
    }
    exit(1);
}

int check_for_wm(xcb_connection_t *c,xcb_screen_t *scr,xcb_window_t root){
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
        throwerr("another wm is already running silly");
        return 1;
    } 
    else if (supporting_wm_reply && xcb_get_property_value_length(supporting_wm_reply) > 0) {
        free(name_reply);
        free(supporting_wm_reply);
        throwerr("another wm is already running silly");
        return 1;
    }
    return 0;
}

void focus(xcb_connection_t *dpy,xcb_window_t win){
    if (win){
        xcb_set_input_focus(
                dpy,
                XCB_INPUT_FOCUS_POINTER_ROOT,
                win,XCB_CURRENT_TIME);
    }
    else return;
}

void unfocus(xcb_connection_t *dpy,xcb_window_t win){
    if (win){
        xcb_set_input_focus(
            dpy,
            XCB_INPUT_FOCUS_NONE,
            win,XCB_CURRENT_TIME
        );
    }
    else return;
}

int has_focus(xcb_connection_t *dpy){
    xcb_get_input_focus_reply_t *reply;
    reply = xcb_get_input_focus_reply(dpy,xcb_get_input_focus(dpy),NULL);
    if (!reply)
        return 0;
    return 1;
}

int win_has_focus(xcb_connection_t *dpy,xcb_window_t win){
    return 0;
}

void move_window(xcb_connection_t *dpy,xcb_window_t win,int x,int y){
    const uint32_t coords[] = { x, y };
    xcb_configure_window(dpy,win,XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y,coords);
}

void map_request(xcb_connection_t *dpy,xcb_screen_t *scr,xcb_generic_event_t *ev){
    xcb_map_request_event_t *e;
    e = (xcb_map_request_event_t *)ev;

    xcb_map_window(dpy,e->window);

    xcb_get_geometry_reply_t *wingeom;
    wingeom = xcb_get_geometry_reply(dpy,xcb_get_geometry(dpy,e->window),NULL);

    int newX = scr->width_in_pixels / 2 - wingeom->width / 2;
    int newY = scr->height_in_pixels / 2 - wingeom->height / 2;

    uint32_t val = XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_LEAVE_WINDOW | 
                   XCB_EVENT_MASK_KEY_PRESS    | XCB_EVENT_MASK_KEY_RELEASE  | 
                   XCB_EVENT_MASK_EXPOSURE;

    xcb_change_window_attributes_checked(dpy,e->window,XCB_CW_EVENT_MASK,&val);

    move_window(dpy,e->window,newX,newY);
    focus(dpy,e->window);

    free(wingeom);
    xcb_flush(dpy);
}

void mouse_enter(xcb_connection_t *dpy,xcb_generic_event_t *ev){
    xcb_enter_notify_event_t *e = (xcb_enter_notify_event_t *)ev;
    focus(dpy,e->event);
}

void mouse_leave(xcb_connection_t *dpy,xcb_generic_event_t *ev){
    xcb_leave_notify_event_t *e = (xcb_leave_notify_event_t *)ev;
    unfocus(dpy,e->event);
}

void key_press(xcb_connection_t *dpy,xcb_generic_event_t *ev){
    //xcb_key_press_event_t *e = (xcb_key_press_event_t *)ev;
}
