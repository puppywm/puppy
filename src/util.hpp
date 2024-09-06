#pragma once

#include <iostream>
#include <xcb/xcb.h>
#include <xcb/xproto.h>

enum annoy_t {
    error,
    warn
};

namespace util {
    void annoy(annoy_t mode,const std::string message);
    void spawn(const char *com);
    void set_focus(xcb_connection_t *c,xcb_window_t win);
	xcb_window_t get_focus(xcb_connection_t *c);
    void move_window(xcb_connection_t *c,xcb_window_t win,int x,int y);
	void list_windows(xcb_connection_t *c);
    int check_for_wm(xcb_connection_t *c,xcb_window_t root);
}

class WM {
public:
    static xcb_connection_t *dpy;
    static xcb_screen_t *scr;
    static xcb_window_t root;

    static xcb_generic_event_t *ev;

    void init_conn(xcb_connection_t **c);
    void close_conn(xcb_connection_t *c);
};

namespace events {
    void handle_map_request(xcb_connection_t *c,xcb_generic_event_t *e);
}
