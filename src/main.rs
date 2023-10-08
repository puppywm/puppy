extern crate xcb;
use xcb::x::{self, Cw::EventMask};
use std::process::Command;

pub fn set_focus(dpy: &xcb::Connection,win: x::Window) -> () {
    dpy.send_request(&x::SetInputFocus {
        revert_to: x::InputFocus::PointerRoot,
        focus: win,
        time: x::CURRENT_TIME
    });
    let _ = dpy.flush();
}

fn main() -> xcb::Result<()> {
    let Ok((dpy,_)) = xcb::Connection::connect(None) else {
        println!("unable to initialize connection with X server"); 
        panic!()};

    let scr = dpy.get_setup().roots().nth(0).unwrap();
    let root = scr.root();

    let rootvals = &[EventMask(x::EventMask::SUBSTRUCTURE_NOTIFY | x::EventMask::SUBSTRUCTURE_REDIRECT)];

    dpy.send_request_checked(&x::ChangeWindowAttributes {
        window: root,
        value_list: rootvals
    });

    dpy.flush()?;
    
    let _ = Command::new("/bin/sxhkd").spawn();

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

                dpy.send_request(&x::MapWindow { window: ev.window() });
                
                let _ = dpy.flush()?;
                set_focus(&dpy,win);
            },
            /*
            xcb::Event::X(x::Event::ConfigureRequest(ev)) => {
            },
            */
            _ => {}
        }
    }
}
