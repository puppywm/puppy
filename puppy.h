#ifndef __PUPPY__
#define __PUPPY__
#include <xcb/xcb.h>
// #include <xcb/xcb_ewmh.h>

#define WM_NAME "puppywm"

void throwerr(const char*);
void init_conn(xcb_connection_t **);
void close_conn(xcb_connection_t *);

int check_for_wm(xcb_connection_t *,xcb_screen_t *,xcb_window_t);
int spawn(const char*);

int has_focus(xcb_connection_t *);
void focus(xcb_connection_t *,xcb_window_t);

void move_window(xcb_connection_t *,xcb_window_t,int,int);

void map_request(xcb_connection_t *,xcb_screen_t *,xcb_generic_event_t *);

#endif
