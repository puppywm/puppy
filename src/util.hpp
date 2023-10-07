#pragma once

#include <iostream>
extern "C"{
#include <xcb/xcb.h>
#include <xcb/xproto.h>
}

// classes
class WM {
public:
    static xcb_connection_t *dpy;
    static xcb_screen_t *scr;
    static xcb_window_t focused_win;
    static xcb_window_t root;

    static xcb_generic_event_t *ev;

    void init_conn(xcb_connection_t **c);
    void close_conn(xcb_connection_t *c);
};

// disregard
enum annoy_t {
    error,
    warn
};

// namespaces
namespace util {
    void annoy(annoy_t mode,const std::string message);
    void spawn(const char *com);
    void set_focus(xcb_connection_t *c,xcb_window_t win);
    void move_window(xcb_connection_t *c,xcb_window_t win,int x,int y);
    void resize_window(xcb_connection_t *c,xcb_window_t win,int width,int height);
    void raise_window(xcb_connection_t *c,xcb_window_t win);
    int check_for_wm(xcb_connection_t *c,xcb_window_t root);
}

namespace events {
    void handle_configure_request(xcb_connection_t *c,xcb_generic_event_t *ev);
    void handle_map_request(xcb_connection_t *c,xcb_generic_event_t *e);
    void handle_enter_notify(xcb_connection_t *c,xcb_generic_event_t *e);
}
