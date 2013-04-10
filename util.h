//      xstarcastle
//      Copyright (c) 1993-2005 by Mark B. Hanson (mbh@panix.com).
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

#ifndef	XSTARCASTLE_UTIL_H
#define	XSTARCASTLE_UTIL_H

typedef enum _gc_token {
    GC_BLACK,

    GC_BRIGHT_RED,
    GC_DULL_RED,

    GC_BRIGHT_ORANGE,
    GC_DULL_ORANGE,

    GC_BRIGHT_YELLOW,
    GC_DULL_YELLOW,

    GC_BRIGHT_GREY,
    GC_DULL_GREY,

    GC_GREEN,

    GC_BRIGHT_BLUE,
    GC_DULL_BLUE,

    GC_TOKEN_COUNT
} gc_token;

extern void init_gc(bool);
extern void free_all_gcs(void);
extern GC fetch_gc(const gc_token);

void _XDrawLine(Display * display, Window window, GC thisgc, int nx0, int ny0, int nx1, int ny1, bool);
void _XFillArc(Display *display, Window window, GC thisgc, int x, int y, int xsize, int ysize, int b, int d, bool);
void fill_gcs();

#endif	// XSTARCASTLE_UTIL_H
