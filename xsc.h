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

#ifndef	XSTARCASTLE_XSTARCASTLE_H
#define	XSTARCASTLE_XSTARCASTLE_H

#include "args.h"
#include "difficulty.h"
#include "timing.h"


extern char *program;
extern Display *display;
extern Window game_window;
extern Window stats_window;
extern int wwidth;
extern int wwidth2;
extern int gwheight;
extern int gwheight2;
extern struct Args::info args;
extern Stamp time_now;
extern GC **gcs;
extern GC **bright_gcs;
extern int display_height;
extern int display_width;

const double DEFAULT_FPS	= 30.0;
#define MAX_RINGS 3

#endif	// XSTARCASTLE_XSTARCASTLE_H
