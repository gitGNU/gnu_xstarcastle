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

#ifndef	XSTARCASTLE_FONT_H
#define	XSTARCASTLE_FONT_H

#include "global.h"

const int font_digit_offset = 0;
const int font_comma_offset = 10;
const int font_upper_offset = 11;
const int font_lower_offset = 37;
const int font_copyright_offset = 63;

extern const struct coords *const cinematronics_points[];
extern const int cinematronics_count[];

#endif	// XSTARCASTLE_FONT_H
