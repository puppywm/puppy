#ifndef __PUPPY__
#define __PUPPY__
#include <xcb/xcb.h>
// #include <xcb/xcb_ewmh.h>

#define WM_NAME "puppywm"
#define DISTANCE_FROM_CORNER 10

void throwerr(const char* error);
void init_conn(xcb_connection_t **c);
void close_conn(xcb_connection_t *c);

int check_for_wm(xcb_connection_t *c,xcb_screen_t *scr,xcb_window_t root);
int spawn(const char*);

int has_focus(xcb_connection_t *dpy);

void focus(xcb_connection_t *dpy,xcb_window_t win);
void unfocus(xcb_connection_t *dpy,xcb_window_t win);

void move_window(xcb_connection_t *dpy,xcb_window_t win,int x,int y);

void key_press(xcb_connection_t *dpy,xcb_generic_event_t *ev);
void mouse_click(xcb_connection_t *dpy,xcb_generic_event_t *ev);

void map_request(xcb_connection_t *dpy,xcb_screen_t *scr,xcb_generic_event_t *ev);

#endif
