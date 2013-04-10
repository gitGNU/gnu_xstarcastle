//      xstarcastle
//      Copyright (c) 1993-2005 by Mark B. Hanson (mbh@panix.com).
//      Copyright (C) 2013 Ben Asselstine
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; version 2 of the License.   
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

#include <X11/keysym.h>

#include "global.h"

#include "xsc.h"

#include "args.h"


void
Args::init(struct info *const a)
{
    a->ccw = XK_d;
    a->cw = XK_f;
    a->thrust = XK_j;
    a->fire = XK_k;
    a->pause = XK_p;
    a->iconify = XK_i;
    a->quit = XK_q;
    a->start = XK_space;
    a->fps = DEFAULT_FPS;
    a->x = -1;	// no specified position
    a->y = -1;	// no specified position
    a->percent = 90;
    a->nopause = false;
    a->bw = false;
    a->quiet = false;
    a->stats = false;
} // Args::init


void
Args::version(const char *program_name)
{
  printf("%s version %s\n", program_name, VERSION);
  printf ("Send bug reports to <xstarcastle@nym.hush.com>.\n");
}

void
Args::help(const char *program_name)
{
  printf ("\
Usage: %s [OPTION]...\n\
", program_name);
  fputs ("\
Play a clone of Star Castle.\n\
\n\
      --ccw KEYCODE      specify keycode which rotates the ship anti-clockwise\n\
      --cw KEYCODE       specify the keycode which rotates the ship clockwise\n\
      --fire KEYCODE     specify keycode which launches a bullet from the ship\n\
  -f, --fps FLOAT        try to draw this many frames per second\n\
      --greyscale        draw only in shades of grey\n\
      --iconify KEYCODE  specify the keycode which minimizes the window\n\
      --nopause          don't pause when the cursor leaves the window\n\
      --pause KEYCODE    specify keycode which suspends and resumes the game\n\
   -p, --percent INT     percent of the display the window should take up\n\
      --quiet            don't play sounds\n\
      --quit KEYCODE     change the keycode which ends the game\n\
      --start KEYCODE    change the keycode which begins the game\n\
      --stats            write gameplay statistics to ~/xstarcastle-stats.csv\n\
      --thrust KEYCODE   change the keycode which propels the ship forward\n\
  -x, --x-window-pos INT specify the starting x coordinate of the game window\n\
  -y, --y-window-pos INT specify the starting y coordinate of the game window\n\
      --help             display this help and exit\n\
      --version          display version information and exit\n\
", stdout);
  fprintf (stdout, "\nKEYCODE refers to the textual representation of an X key symbol.\nFor example, `Return' refers to the enter key, and `Left' refers to the left\narrowand so on.  The default key-bindings are:\n\n");
  fprintf (stdout, "  D turns the ship counter-clockwise\n\n\
  F turns the ship clockwise\n\n\
  J runs the thruster\n\n\
  K fires the laser\n\n\
  P toggles the pause (moving the cursor out of the window pauses the game)\n\n\
  Q quits the game\n\n\
  space starts a new game\n\n");
  fprintf(stdout, "The high score is stored in ~/.xstarcastle.\n\n");

} // Args::help

void
Args::set(struct info *const a, const int argc,
	const char *const argv[])
{
    for (int i = 1; i < argc; i++) {
	if (!strcmp(argv[i], "--help")) {
          help(basename(argv[0]));
          exit(1);
        } else if (!strcmp(argv[i], "--version")) {
          version(basename(argv[0]));
          exit(1);
        } else if (!strcmp(argv[i], "--ccw")) {
	    if (++i < argc) {
		if ((a->ccw = XStringToKeysym(argv[i])) == NoSymbol) {
		    fprintf(stderr, "%s: cannot convert `%s' to a KeySym!\n",
			    program, argv[i]);
		    exit(1);
		}
	    } else {
		fprintf(stderr, "%s: no argument for `%s'!\n",
			program, "--ccw");
		exit(1);
	    }
	} else if (!strcmp(argv[i], "--cw")) {
	    if (++i < argc) {
		if ((a->cw = XStringToKeysym(argv[i])) == NoSymbol) {
		    fprintf(stderr, "%s: cannot convert `%s' to a KeySym!\n",
			    program, argv[i]);
		    exit(1);
		}
	    } else {
		fprintf(stderr, "%s: no argument for `%s'!\n",
			program, "--cw");
		exit(1);
	    }
	} else if (!strcmp(argv[i], "--thrust")) {
	    if (++i < argc) {
		if ((a->thrust = XStringToKeysym(argv[i])) == NoSymbol) {
		    fprintf(stderr, "%s: cannot convert `%s' to a KeySym!\n",
			    program, argv[i]);
		    exit(1);
		}
	    } else {
		fprintf(stderr, "%s: no argument for `%s'!\n",
			program, "--thrust");
		exit(1);
	    }
	} else if (!strcmp(argv[i], "--fire")) {
	    if (++i < argc) {
		if ((a->fire = XStringToKeysym(argv[i])) == NoSymbol) {
		    fprintf(stderr, "%s: cannot convert `%s' to a KeySym!\n",
			    program, argv[i]);
		    exit(1);
		}
	    } else {
		fprintf(stderr, "%s: no argument for `%s'!\n",
			program, "--fire");
		exit(1);
	    }
	} else if (!strcmp(argv[i], "--pause")) {
	    if (++i < argc) {
		if ((a->pause = XStringToKeysym(argv[i])) == NoSymbol) {
		    fprintf(stderr, "%s: cannot convert `%s' to a KeySym!\n",
			    program, argv[i]);
		    exit(1);
		}
	    } else {
		fprintf(stderr, "%s: no argument for `%s'!\n",
			program, "--pause");
		exit(1);
	    }
	} else if (!strcmp(argv[i], "--iconify")) {
	    if (++i < argc) {
		if ((a->iconify = XStringToKeysym(argv[i])) == NoSymbol) {
		    fprintf(stderr, "%s: cannot convert `%s' to a KeySym!\n",
			    program, argv[i]);
		    exit(1);
		}
	    } else {
		fprintf(stderr, "%s: no argument for `%s'!\n",
			program, "--iconify");
		exit(1);
	    }
	} else if (!strcmp(argv[i], "--quiet")) {
          args.quiet = true;
	} else if (!strcmp(argv[i], "--quit")) {
	    if (++i < argc) {
		if ((a->quit = XStringToKeysym(argv[i])) == NoSymbol) {
		    fprintf(stderr, "%s: cannot convert `%s' to a KeySym!\n",
			    program, argv[i]);
		    exit(1);
		}
	    } else {
		fprintf(stderr, "%s: no argument for `%s'!\n",
			program, "--quit");
		exit(1);
	    }
	} else if (!strcmp(argv[i], "--start")) {
	    if (++i < argc) {
		if ((a->start = XStringToKeysym(argv[i])) == NoSymbol) {
		    fprintf(stderr, "%s: cannot convert `%s' to a KeySym!\n",
			    program, argv[i]);
		    exit(1);
		}
	    } else {
		fprintf(stderr, "%s: no argument for `%s'!\n",
			program, "--start");
		exit(1);
	    }
	} else if (!strcmp(argv[i], "--stats")) {
          args.stats = true;
	} else if (!strcmp(argv[i], "--fps") || !strcmp(argv[i], "-f")) {
	    if (++i < argc) {
		if ((a->fps = atoi(argv[i])) <= 0) {
		    fprintf(stderr, "%s: bad argument for `%s' to an !\n",
			    program, "--fps");
		    exit(1);
		}
	    } else {
		fprintf(stderr, "%s: no argument for `%s'!\n",
			program, "--fps");
		exit(1);
	    }
	} else if (!strcmp(argv[i], "-x") || 
                   !strcmp(argv[i], "--x-window-pos")) {
	    if (++i < argc) {
		if ((a->x = atoi(argv[i])) <= 0) {
		    fprintf(stderr, "%s: bad argument for `%s' to an !\n",
			    program, "--x-window-pos");
		    exit(1);
		}
	    } else {
		fprintf(stderr, "%s: no argument for `%s'!\n",
			program, "--x-window-pos");
		exit(1);
	    }
	} else if (!strcmp(argv[i], "-y") || 
                   !strcmp(argv[i], "--y-window-pos")) {
	    if (++i < argc) {
		if ((a->y = atoi(argv[i])) <= 0) {
		    fprintf(stderr, "%s: bad argument for `%s' to an !\n",
			    program, "--y-window-pos");
		    exit(1);
		}
	    } else {
		fprintf(stderr, "%s: no argument for `%s'!\n",
			program, "--y-window-pos");
		exit(1);
	    }
	} else if (!strcmp(argv[i], "--percent") || !strcmp(argv[1], "-p")) {
	    if (++i < argc) {
		if ((a->percent = atoi(argv[i])) <= 0 || a->percent > 100) {
		    fprintf(stderr, "%s: bad argument for `%s'!\n",
			    program, "--percent");
		    exit(1);
		}
	    } else {
		fprintf(stderr, "%s: no argument for `%s'!\n",
			program, "--percent");
		exit(1);
	    }
	} else if (!strcmp(argv[i], "--nopause")) {
	    a->nopause = true;
	} else if (!strcmp(argv[i], "--greyscale")) {
	    a->bw = true;
	} else {
	    fprintf(stderr, "%s: unknown argument `%s'!\n", program, argv[i]);
	    exit(1);
	}
    }
} // Args::set
