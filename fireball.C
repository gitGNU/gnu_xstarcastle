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

#include "global.h"

#include "random.h"
#include "trig.h"
#include "util.h"
#include "xsc.h"
#include "sound.h"

#include "fireball.h"

using namespace Trig;


namespace {

const coords fireball_points[] = {
    { COS00 / 2.0, -SIN00 / 2.0 }, { -COS00 / 2.0,  SIN00 / 2.0 },
    { COS15 / 2.0, -SIN15 / 2.0 }, { -COS15 / 2.0,  SIN15 / 2.0 },
    { COS30 / 2.0, -SIN30 / 2.0 }, { -COS30 / 2.0,  SIN30 / 2.0 },
    { COS45 / 2.0, -SIN45 / 2.0 }, { -COS45 / 2.0,  SIN45 / 2.0 },
    { COS60 / 2.0, -SIN60 / 2.0 }, { -COS60 / 2.0,  SIN60 / 2.0 },
    { COS75 / 2.0, -SIN75 / 2.0 }, { -COS75 / 2.0,  SIN75 / 2.0 },
    { COS90 / 2.0,  SIN90 / 2.0 }, { -COS90 / 2.0, -SIN90 / 2.0 },
    { COS75 / 2.0,  SIN75 / 2.0 }, { -COS75 / 2.0, -SIN75 / 2.0 },
    { COS60 / 2.0,  SIN60 / 2.0 }, { -COS60 / 2.0, -SIN60 / 2.0 },
    { COS45 / 2.0,  SIN45 / 2.0 }, { -COS45 / 2.0, -SIN45 / 2.0 },
    { COS30 / 2.0,  SIN30 / 2.0 }, { -COS30 / 2.0, -SIN30 / 2.0 },
    { COS15 / 2.0,  SIN15 / 2.0 }, { -COS15 / 2.0, -SIN15 / 2.0 },
};

} // namespace


Fireball::Fireball(void)
{
    //fprintf(stderr, "Fireball::Fireball()\n");
    set_scale(25.0);
    flying = false;
    paused = false;
    points = fireball_points;
    num_points = sizeof(fireball_points) / sizeof(coords);
    xpoints = new XPoint[num_points];
    randomizer = new double[num_points];
    gc = fetch_gc(GC_BRIGHT_BLUE);
} // Fireball::Fireball


Fireball::~Fireball(void)
{
    //fprintf(stderr, "Fireball::~Fireball()\n");
    delete[] randomizer;
} // Fireball::~Fireball


void
Fireball::set_randomizer(void)
{
    if (paused) {
	return;
    }

    for (int i = 0; i < num_points; i++) {
	randomizer[i] = (Random::get() % 500) / 1000.0 + 0.50;
    }
} // Fireball::set_randomizer


void
Fireball::render(const bool ink)
{
    if (!alive()) {
	return;
    }

    if (ink) {
	set_randomizer();
	set_xpoints();
    }
    paint_points(ink);
} // Fireball::render


void
Fireball::move(Ship *ship, Stats *stats)
{
    if (!alive()) {
	return;
    }

    x += dx;
    y += dy;
    if ((x >= wwidth) || (x < 0) || (y >= gwheight) || (y < 0) ||
	    ship->hit(this)) {
      stats->killed_by_a_fireball();
	snuff();
    }
} // Fireball::move


void
Fireball::launch(King *king, Stats *stats)
{
    if (alive()) {
	return;
    }

    x = wwidth2;
    y = gwheight2;

    const double deg = king->get_theta();
    const double mag = (wwidth / 2.5) / args.fps;
    dx = xcos(deg) * mag;
    dy = -xsin(deg) * mag;

    flying = true;
    play(FIREBALL_SOUND);
    stats->launched_a_fireball();
} // Fireball::launch


void
Fireball::set_xpoints(void)
{
    for (int i = 0; i < num_points; i++) {
	double nsize = randomizer[i] * size;

	xpoints[i].x = (short)((nsize * points[i].x) + x);
	xpoints[i].y = (short)((nsize * points[i].y) + y);
    }
} // Fireball::set_xpoints

bool
Fireball::hit(Laser *laser, Stats *stats)
{
  if (!alive())
    return false;
  double delta_x = fabs(laser->get_x() - get_x());
  double delta_y = fabs(laser->get_y() - get_y());
  if (delta_x < diag && delta_y < diag)
    {
      stats->add_score(0);
      return true;
    }
  return false;
} // Fireball::hit
