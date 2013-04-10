//      xstarcastle
//      Copyright (c) 1993-2005 by Mark B. Hanson (mbh@panix.com).
//      Copyright (C) 2013 Ben Asselstine
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; version 2 of the License, or 
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
//  02110-1301, USA.
//

#include <signal.h>
#include <sys/param.h>
#include <stdio.h>
#include <GL/glx.h>

#include <X11/Xatom.h>

#include "global.h"

#include "random.h"
#include "args.h"
#include "difficulty.h"
#include "timing.h"
#include "trig.h"
#include "util.h"

#include "xsc.h"

#include "castle.h"
#include "king.h"
#include "minefield.h"
#include "ship.h"
#include "starfield.h"
#include "stats.h"
#include "sound.h"

#include "icon.xbm"


// extern
struct Args::info	args;
char			*program;
Window			game_window;
Display			*display;
int			wwidth;
int			wwidth2;
int			gwheight;
int			gwheight2;
Stamp			time_now;
int                     display_width, display_height;


namespace {

Ship		*ship;
King		*king;
Castle		*castle;
Minefield	*minefield;
Stats		*stats;
Starfield	*light_starfield;

bool		iconified = false;
XEvent		event;
const double	aspect = (4.0 / 3.0);
int		screen_number;
XWMHints	wm_hints;
Atom		delete_atom;

const double total_elapsed_time_to_show_player_scores = 2.0;
const double total_elapsed_time_to_show_game_over = 3.07;
const double total_elapsed_time_to_show_attract = 7.0;
const double total_elapsed_time_to_show_high_scores = 5.0;

void
key_repeat(const bool on)
{
    XKeyboardControl kbcontrol;
    static XKeyboardState origkbstate =
	    (XGetKeyboardControl(display, &origkbstate), origkbstate);

    kbcontrol.auto_repeat_mode = (on ?
	    origkbstate.global_auto_repeat : AutoRepeatModeOff);
    XChangeKeyboardControl(display, KBAutoRepeatMode, &kbcontrol);
} // ::key_repeat


void
pause_stuff(const bool pause_state)
{
    castle->pause(pause_state);
    king->pause(pause_state);
    minefield->pause(pause_state);
    ship->pause(pause_state);
    sound_pause(pause_state);
} // :: pause_stuff


void
quit(const int code)
{
    key_repeat(true);

    delete ship;
    delete king;
    delete castle;
    delete minefield;
    delete stats;
    delete light_starfield;


    XDestroyWindow(display, game_window);
    free_all_gcs();
    XCloseDisplay(display);

    exit(code);
} // ::quit


extern "C" RETSIGTYPE
catcher(int sig)
{
    fprintf(stderr, "xstarcastle: caught signal %d -- shutting down\n", sig);
    quit(EXIT_FAILURE);
} // ::catcher




inline bool
handle_event(void)
{
    if (!XPending(display)) {
	// no events to handle
	return false;
    }

    XNextEvent(display, &event);
    switch (event.type) {
	case Expose:
	    if (event.xexpose.count != 0) {
		// more exposure events on the way, we can ignore this one
		break;
	    }

	    if (stats->state != STATE_PAUSED) {
		stats->erase();
		king->erase();
		minefield->erase();
		// the other stuff doesn't change shape,
		// so doesn't really need to be erased
	    }

	    stats->draw();
	    light_starfield->draw();
            if (stats->state != STATE_HIGH_SCORES)
              {
	    ship->draw();
	    castle->draw();
	    king->draw();
	    minefield->draw();
              }

	    // flush events to avoid flickering
	    XSync(display, False);

	    break;
	case ConfigureNotify: {
	    int nw = event.xconfigure.width;
	    int nh = event.xconfigure.height;
	    int ngh = (int)(nh);

	    if (nw == wwidth && nh == gwheight) {
		// window not resized, ignore event
		break;
	    }

	    stats->resize(nw, nh - ngh);
	    ship->resize(nw, ngh);
	    king->resize(nw, ngh);
	    castle->resize(nw, ngh);
	    minefield->resize(nw, ngh);
	    light_starfield->resize(nw, ngh);

	    wwidth = nw;
	    gwheight = nh;
	    gwheight = ngh;

	    wwidth2 = wwidth / 2;
	    gwheight2 = gwheight / 2;
            fill_gcs();

	    //XMoveResizeWindow(display, game_window,
		    //0, 0,
		    //wwidth, gwheight);
	}
	    break;
	case KeyPress: {
	    KeySym ks = XLookupKeysym(&event.xkey, 0);

	    if (ks == args.ccw) {
		if (stats->state == STATE_NORMAL ||
                    stats->state == STATE_SHOW_PLAYER_SCORE ) {
                  if (ship->alive())
		    ship->rotate_ccw(KEY_DOWN);
		}
	    } 
            if (ks == args.cw) {
		if (stats->state == STATE_NORMAL ||
                    stats->state == STATE_SHOW_PLAYER_SCORE) {
                  if (ship->alive())
		    ship->rotate_cw(KEY_DOWN);
		}
	    } 
            if (ks == args.thrust) {
		if (stats->state == STATE_NORMAL ||
                    stats->state == STATE_SHOW_PLAYER_SCORE) {
                  if (ship->alive())
		    ship->thrust(KEY_DOWN);
		}
	    } 
            if (ks == args.fire) {
		if (stats->state == STATE_NORMAL || 
                    stats->state == STATE_SHOW_PLAYER_SCORE) {
                  if (ship->alive() && king->alive())
		    ship->fire(stats);
		}
	    } 
            if (ks == args.pause) {
		if (stats->state == STATE_OVER || stats->state == STATE_HIGH_SCORES || stats->state == STATE_ATTRACT || stats->state == STATE_PRESS_START || iconified) {
                  ;
		} else {
		    if (stats->state == STATE_PAUSED) {
			stats->state = STATE_NORMAL;
			pause_stuff(false);
			key_repeat(false);
		    } else {
			stats->state = STATE_PAUSED;
			pause_stuff(true);
			key_repeat(true);
		    }
		}
	    } 
            if (ks == args.iconify) {
		if (iconified) {
		    XWithdrawWindow(display, game_window,
			    screen_number);
		    XSetWMHints(display, game_window, &wm_hints);
		    XMapWindow(display, game_window);
		    iconified = false;
		} else {
		    XIconifyWindow(display, game_window, screen_number);
		    iconified = true;
		    if (stats->state == STATE_NORMAL) {
			stats->state = STATE_PAUSED;
			pause_stuff(true);
		    }
		    key_repeat(true);
		}
	    } 
            if (ks == args.start) {
		if (stats->state == STATE_HIGH_SCORES ||
                    stats->state == STATE_ATTRACT ||
                    stats->state == STATE_PRESS_START) {
                  if (stats->state == STATE_PRESS_START) {
		    stats->reset();
		    castle->refresh();
		    minefield->erase();
		    minefield->snuff();
                    minefield->spawn_buzzers(castle, -1);
		    ship->reincarnate();
                  } else {
                    stats->state = STATE_PRESS_START;
                    king->erase();
                    castle->erase();
                    ship->erase();
                    minefield->erase();
                  }
		}
	    } 
            if (ks == args.quit) {
		quit(EXIT_SUCCESS);
	    }
	}
	    break;
	case KeyRelease: {
	    KeySym ks = XLookupKeysym(&event.xkey, 0);

	    if (ks == args.ccw) {
		if (stats->state == STATE_NORMAL ||
                    stats->state == STATE_SHOW_PLAYER_SCORE) {
                  ship->rotate_ccw(KEY_UP);
		}
	    } 
            if (ks == args.cw) {
		if (stats->state == STATE_NORMAL ||
                    stats->state == STATE_SHOW_PLAYER_SCORE) {
                  ship->rotate_cw(KEY_UP);
		}
	    } 
            if (ks == args.thrust) {
		if (stats->state == STATE_NORMAL ||
                    stats->state == STATE_SHOW_PLAYER_SCORE) {
                  ship->thrust(KEY_UP);
		}
	    }
	}
	    break;
	case LeaveNotify:
	    if (stats->state == STATE_NORMAL) {
                if (args.nopause == false)
                  {
                    stats->state = STATE_PAUSED;
                    pause_stuff(true);
                  }
	    }
	    key_repeat(true);
	    break;
	case EnterNotify:
	    if (stats->state == STATE_OVER) {
		key_repeat(false);
	    }
	    break;
	case MapNotify:
	    iconified = false;
	    break;
	case UnmapNotify:
	    iconified = true;
	    if (stats->state == STATE_NORMAL) {
		stats->state = STATE_PAUSED;
		pause_stuff(true);
	    }
	    key_repeat(true);
	    break;
	case ClientMessage:
	    if (event.xclient.format == 32 &&
		    (Atom)event.xclient.data.l[0] == delete_atom) {
		quit(EXIT_SUCCESS);
	    }
	    break;
	default:
	    // ignore it.
	    break;
    }

    return true;
} // ::handle_event


bool
change_states()
{
  if (stats->state == STATE_NORMAL && castle->resting() && !king->alive())
    {
      /*the time of death gets set after the explosion starts. */
    stats->state = STATE_SHOW_PLAYER_SCORE;
      buzzer_sound(true);
            //ship->erase();
    return false;
    }
  else if (stats->state == STATE_NORMAL && !ship->alive() &&
           ship->resting() && stats->lives > -1)
    {
      /*the time of death gets set when ship is hit. */
      stats->state = STATE_SHOW_PLAYER_SCORE;
      buzzer_sound(false);
      king->erase();
      castle->erase();
      ship->erase();
      minefield->erase();
      return false;
    }
  else if (stats->state == STATE_NORMAL && !ship->alive() && stats->lives < 0)
    {
      stats->state = STATE_OVER;
      return false;
    }
  else if (stats->state == STATE_SHOW_PLAYER_SCORE)
      {
        static double age;
        age += (1.0/args.fps);
        if (age > total_elapsed_time_to_show_player_scores)
          {
            age = 0.0;
            stats->state = STATE_NORMAL;
            return true;
            /*
             *this return of true looks out of place, but it has the side
             effect of causing the dead ship to respawn.
             this breaks us out of a loop that makes the player score
             show indefinitely.
             */
          }
        if (ship->alive() == true)
          return true;
        return false;
      }
  else if (stats->state == STATE_OVER)
      {
        static double age;
        age += (1.0/args.fps);
        if (age > total_elapsed_time_to_show_game_over)
          {
            age = 0.0;
            stats->state = STATE_HIGH_SCORES;
            king->erase();
            castle->erase();
            ship->erase();
            minefield->erase();
            return false;
          }
      }
  else if (stats->state == STATE_HIGH_SCORES)
      {
        static double age;
        age += (1.0/args.fps);
        if (age > total_elapsed_time_to_show_high_scores)
          {
            age = 0.0;
            castle->refresh();
            minefield->erase();
            minefield->snuff();
            stats->state = STATE_ATTRACT;
            king->set_dtheta(32.0 / args.fps);
            minefield->spawn_buzzers(castle, -1);
            castle->refresh();
          }
      }
  else if (stats->state == STATE_ATTRACT)
      {
        static double age;
        age += (1.0/args.fps);
        if (age > total_elapsed_time_to_show_attract)
          {
            age = 0.0;
            stats->state = STATE_HIGH_SCORES;
            buzzer_sound(false);
            king->erase();
            castle->erase();
            ship->erase();
            minefield->erase();
            return false;
          }
      }

    if (stats->state == STATE_PAUSED ||
        stats->state == STATE_HIGH_SCORES ||
        stats->state == STATE_PRESS_START) {
      return false;
    }
    return true;
}

inline void
animate(void)
{

    // erase the last frame
    light_starfield->erase();
    king->erase();
    castle->erase();
    ship->erase();
    minefield->erase();

    // calculate new positions
    ship->turn();
    ship->move(castle, king, minefield, stats);
    if (stats->state == STATE_ATTRACT)
      king->attract();
    else
      king->turn(castle, ship, stats);
    castle->turn(minefield, king, stats);
    minefield->turn(castle, ship);
    minefield->move(castle, ship, stats);
    minefield->launch(king, castle, stats);	// maybe launch another buzzer
    minefield->expire(); //maybe snuff a mine

    // draw new frame
    light_starfield->draw();
    king->draw();
    castle->draw();
    ship->draw();
    minefield->draw();

    // flush events
    XSync(display, False);
} // ::animate


void
snooze(void)
{
  static const long desired = (long)((1.0 / args.fps) * 1000000);
  static Stamp tv_begin = (XSCTime::now(&tv_begin), tv_begin);
  static long sleepfor;

  XSCTime::now(&time_now);
  sleepfor += desired - (time_now - tv_begin).micros();

  if (sleepfor > 0) {
    // sleep if we have the time to spare
    XSCTime::sleep(sleepfor);
  } else if (sleepfor < desired * -2L) {
    // keep the game from running wild if we have a sleep deficit
    sleepfor = desired * -2L;
  }

  tv_begin = time_now;
} // ::snooze

} // namespace



int
main(const int argc, char **const argv)
{
  char *display_name = NULL;
  char title[] = "xstarcastle";
  char *window_title = title;
  char *icon_title = title;
  Pixmap icon;
  XSizeHints size_hints;
  XClassHint class_hints;
  XTextProperty windowName, iconName;


  program = argv[0];

  //printf("%s version %s\n", title, VERSION);

  XSCTime::now(&time_now);
  Random::seed((unsigned int)time_now.get_usec());

  Trig::init();

  Args::init(&args);
  Args::set(&args, argc, argv);

  Difficulty::init();

  if (!args.quiet)
    {
      sound_init();
    }

  if ((display = XOpenDisplay(display_name)) == NULL) {
    fprintf(stderr, "%s: cannot connect to X server %s\n",
            program, XDisplayName(display_name));
    exit(1);
  }

  screen_number = DefaultScreen(display);

  display_width = DisplayWidth(display, screen_number);
  display_height = DisplayHeight(display, screen_number);

  gwheight = (int)(display_height * args.percent / 100.0);
  wwidth = (int)(gwheight * aspect);

  gwheight2 = gwheight / 2;
  wwidth2 = wwidth / 2;

  //XSetWindowAttributes atts;

  game_window = XCreateSimpleWindow(display,
                                    RootWindow(display, screen_number),
                                    0, 0,
                                    wwidth, gwheight,
                                    0,
                                    WhitePixel(display, screen_number),
                                    BlackPixel(display, screen_number));

  icon = XCreateBitmapFromData(display, game_window,
                               (char *)icon_bits, icon_width, icon_height);
  if (XStringListToTextProperty(&window_title, 1, &windowName) == 0) {
    fprintf(stderr, "%s: structure allocation for windowName failed.\n",
            program);
    exit(1);
  }

  if (XStringListToTextProperty(&icon_title, 1, &iconName) == 0) {
    fprintf(stderr, "%s: structure allocation for iconName failed.\n",
            program);
    exit(1);
  }

  size_hints.flags = PSize | PAspect | PMinSize | PMaxSize;
  size_hints.min_aspect.x =
    size_hints.max_aspect.x =
    size_hints.min_width = (int)(100.0 * aspect);
  size_hints.min_aspect.y =
    size_hints.max_aspect.y =
    size_hints.min_height = 100;
  size_hints.max_width = display_width;
  size_hints.max_height = display_height;

  wm_hints.flags = StateHint | IconPixmapHint | InputHint;
  wm_hints.initial_state = NormalState;
  wm_hints.input = True;
  wm_hints.icon_pixmap = icon;

  class_hints.res_name = program;
  class_hints.res_class = window_title;

  XSetWMProperties(display, game_window, &windowName, &iconName, argv, argc,
                   &size_hints, &wm_hints, &class_hints);

  delete_atom = XInternAtom(display, "WM_DELETE_WINDOW", False);

  XSetWMProtocols(display, game_window, &delete_atom, 1);

  XSelectInput(display, game_window, (ExposureMask | KeyPressMask |
                                      KeyReleaseMask | StructureNotifyMask | LeaveWindowMask |
                                      EnterWindowMask));


  XMapWindow(display, game_window);
  XNextEvent(display, &event);
  XSync(display, True);

  init_gc(args.bw);

  if (args.x != -1 && args.x != -1)
    {
      XWindowChanges changes;
      changes.x = args.x;
      changes.y = args.y;
      XConfigureWindow  (display, game_window, CWX | CWY, &changes);
    }

  ship = new Ship;
  king = new King;
  castle = new Castle;
  stats = new Stats;
  stats->state = STATE_ATTRACT;
  minefield = new Minefield;
  light_starfield = new Starfield(-1, true);

  fill_gcs();

  key_repeat(false);

  signal(SIGHUP, catcher);
  signal(SIGINT, catcher);
  signal(SIGQUIT, catcher);
  //signal(SIGSEGV, catcher);
  signal(SIGTERM, catcher);

  bool first = true;
  for (;;) {
    while (handle_event());
    bool do_animation = change_states();
    if (stats->changed()) {
	stats->erase();
	stats->draw();
    }
    if (do_animation)
      animate();
    if (stats->state == STATE_SHOW_PLAYER_SCORE)
      if (ship->alive())
        stats->draw();
    snooze();
    if (first)
      {
        //very hackish.
        minefield->spawn_buzzers(castle, -1);
        first = false;
      }
  }

  quit(EXIT_FAILURE);
} // ::main
