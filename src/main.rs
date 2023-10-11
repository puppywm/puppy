extern crate xcb;
use xcb::x::{
    self, 
    Cw::EventMask, 
    ConfigWindow::{X,Y,Width,Height,Sibling,StackMode}
};
use std::process::Command;

pub fn set_focus(dpy: &xcb::Connection,win: x::Window) -> () {
    dpy.send_request(&x::SetInputFocus {
        revert_to: x::InputFocus::PointerRoot,
        focus: win,
        time: x::CURRENT_TIME
    });
}

pub fn move_window(dpy: &xcb::Connection,win: x::Window,x: i32,y: i32) -> () {
    dpy.send_request_checked(&x::ConfigureWindow {
        window: win,
        value_list: &[X(x.into()),Y(y.into())]
    }); 
}


fn main() -> xcb::Result<()> {
    let Ok((dpy,_)) = xcb::Connection::connect(None) else {
        println!("unable to initialize connection with X server"); 
        panic!()
    };

    let scr = dpy.get_setup().roots().nth(0).unwrap();
    let root = scr.root();

    let rootvals = &[EventMask(x::EventMask::SUBSTRUCTURE_NOTIFY | x::EventMask::SUBSTRUCTURE_REDIRECT)];

    dpy.send_request_checked(&x::ChangeWindowAttributes {
        window: root,
        value_list: rootvals
    });

    dpy.flush()?;
    
    let _ = Command::new("/bin/sxhkd").spawn().expect("failed to launch sxhkd");

    loop {
        let event = match dpy.wait_for_event() {
            Err(xcb::Error::Connection(err)) => panic!("unexpected I/O error: {}",err),
            Err(xcb::Error::Protocol(err)) => panic!("unexpected protocol error: {:#?}",err),
            Ok(event) => event
        };
        println!("\nevent recieved : \n{:#?}",event); // for debugging
        match event {
            xcb::Event::X(x::Event::MapRequest(ev)) => {
                let win: x::Window = ev.window();
                let mapvals = &[EventMask(x::EventMask::FOCUS_CHANGE | x::EventMask::BUTTON_PRESS)];
                
                dpy.send_request_checked(&x::ChangeWindowAttributes {
                    window: win,
                    value_list: mapvals
                });

                dpy.send_request(&x::MapWindow { window: win });
                
                set_focus(&dpy,win);
                let _ = dpy.flush()?;
            },
            xcb::Event::X(x::Event::ButtonPress(ev)) => {
                let win = ev.child();
                let pressvals = &[StackMode(x::StackMode::Above)];

                dpy.send_request_checked(&x::ConfigureWindow {
                    window: win,
                    value_list: pressvals
                });

                set_focus(&dpy,win);
                let _ = dpy.flush()?;
            },
            xcb::Event::X(x::Event::ConfigureRequest(ev)) => {
                let configvals = &[X(ev.x().into()),Y(ev.y().into()),
                                   Width(ev.width().into()),Height(ev.height().into()),
                                   Sibling(ev.sibling()),StackMode(ev.stack_mode())];

                dpy.send_request_checked(&x::ConfigureWindow {
                    window: ev.window(),
                    value_list: configvals
                });

                let _ = dpy.flush()?;
            },
            _ => {}
        }
    }
}
