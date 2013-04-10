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

#ifndef	XSTARCASTLE_ARGS_H
#define	XSTARCASTLE_ARGS_H

#include <X11/Xlib.h>
namespace Args {

struct info {
    KeySym ccw;		// the key which turns the ship counterclockwise
    KeySym cw;		// the key which turns the ship clockwise
    KeySym thrust;	// the key which thrusts the ship
    KeySym fire;	// the key which fires the ship's laser
    KeySym pause;	// the key which pauses the game
    KeySym iconify;	// the key which iconifies the window
    KeySym quit;	// the key which quits the game
    KeySym start;	// the key which starts the game
    double fps;		// frames per second
    int x;		// x offset of the window
    int y;		// y offset of the window
    int percent;	// percent of the display the window should take up
    bool nopause;	// don't pause when the cursor leaves the window
    bool quiet;         // don't play sounds
    bool bw;		// black and white display
    bool stats;         // save stats to a file.
};

void init(struct info *const);
void set(struct info *const, const int, const char *const []);
void help(const char *);
void version(const char *);

} // namespace Args

#endif	// XSTARCASTLE_ARGS_H
