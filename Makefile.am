## Process this file with automake to produce Makefile.in
#      xstarcastle
#      Copyright (c) 1993-2002 by Mark B. Hanson (mbh@panix.com).
#      Copyright (C) 2013 Ben Asselstine
# 
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; version 2 of the License, or
#   (at your option) any later version.
# 
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
# 
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
#   02110-1301, USA.

SUBDIRS=sounds
PACKAGE = xstarcastle
VERSION = 1.6

# for development
#AM_CXXFLAGS = -g -DDEBUG -Wall -W -Wshadow -Wcast-qual -Wcast-align \
#-Wconversion -Winline -Werror
#AM_CXXFLAGS = -std=c++11
AM_CXXFLAGS=${SDL_CFLAGS} -Wall -Werror
INCLUDES=-DXSTARCASTLE_DATADIR='"$(XSTARCASTLE_DATADIR)"'

bin_PROGRAMS = xstarcastle
xstarcastle_SOURCES = \
	args.C \
	buzzer.C \
	castle.C \
	difficulty.C \
	ething.C \
	fireball.C \
	kapow.C \
	king.C \
	laser.C \
	minefield.C \
	ring.C \
	cinematronics.C \
	ship.C \
	spikes.C \
	spikes.h \
	stamp.C \
	starfield.C \
	stats.C \
	text.C \
	thing.C \
	trig.C \
	tthing.C \
	util.C \
	xsc.C \
	xything.C \
	sound.C

xstarcastle_LDADD=${SDL_LIBS}
#this man page was generated with help2man, but i don't want to add it as a 
#build dependency.
#this means you're stuck with manually regenerating it when the command-line
#options change.
#regenerate with: help2man --no-info ./xstarcastle
man1_MANS = xstarcastle.1

EXTRA_DIST = \
	${man1_MANS} \
	args.h \
	buzzer.h \
	castle.h \
	config.h \
	difficulty.h \
	ething.h \
	fireball.h \
	font.h \
	global.h \
	icon.xbm \
	kapow.h \
	king.h \
	laser.h \
	minefield.h \
	random.h \
	ring.h \
	ship.h \
	stamp.h \
	starfield.h \
	stats.h \
	text.h \
	thing.h \
	timing.h \
	trig.h \
	tthing.h \
	util.h \
	xsc.h \
	xything.h \
	sound.h
