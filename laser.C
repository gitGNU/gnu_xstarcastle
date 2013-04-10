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

#include "global.h"

#include "trig.h"
#include "util.h"
#include "xsc.h"

#include "sound.h"
#include "laser.h"

using namespace Trig;


Laser::Laser(void)
{
    //fprintf(stderr, "Laser::Laser()\n");
    set_scale(175.0);
    lifespan = wwidth * 0.70;
    age = lifespan;
    mag = (wwidth / 2.0) / args.fps;
} // Laser::Laser


Laser::~Laser(void)
{
    //fprintf(stderr, "Laser::~Laser()\n");
} // Laser::~Laser


void
Laser::resize(const int nwidth, const int nheight)
{
    const double nlifespan = nwidth * 0.75;
    const double deg = ship->get_theta();

    if (alive()) {
	age *= nlifespan / lifespan;
    } else {
	age = nlifespan;
    }
    lifespan = nlifespan;

    mag = (nwidth / 2.0) / args.fps;
    dx = xcos(deg) * 1.2 /*+ ship->get_dx() / mag*/;
    dy = -xsin(deg) * 1.2/* + ship->get_dy() / mag*/;
    Thing::resize(nwidth, nheight);
    //here we say the laser goes 1.1 times faster than the ship.

    if (size < 2.0) {
	size = 2.0;
    }
} // Laser::resize


void
Laser::render(const bool ink)
{
    if (!alive()) {
	return;
    }

    _XFillArc(display, window, (ink ? gc : fetch_gc(GC_BLACK)),
	    (int)(x - (size / 2.0)),
	    (int)(y - (size / 2.0)),
	    (int)size, (int)size,
	    0, 360*64, true);
} // Laser::render


void
Laser::move(Castle *castle, King *king, Minefield *minefield, Stats *stats)
{
    for (int i = 0; i < mag && alive(); i++) {
	Xything::move();
	if (castle->collision(this, stats) ||
		king->hit(castle, this, minefield, stats) ||
		minefield->hit(this, stats) ||
                king->fireball_hit(this, stats)) {
	    snuff();
	} else {
	    age += hypot(dx, dy);
	}
    }
} // Laser::move


void
Laser::launch(void)
{
    const double deg = ship->get_theta();

    x = ship->get_x();
    y = ship->get_y();

    dx = xcos(deg) * 1.1 /*+ ship->get_dx() / mag*/;
    dy = -xsin(deg) * 1.1/* + ship->get_dy() / mag*/;

    age = 0;
    play(BULLET_SOUND);
} // Laser::launch
