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

#include "trig.h"
#include "util.h"
#include "xsc.h"

#include "king.h"

using namespace Trig;


namespace {

const int num_animation_frames = 6;
const double total_elapsed_time_to_show_full_animation = 0.4;
const coords king_points[num_animation_frames][36] = {
    {
        {  0.28,  0.12 }, {  0.35,  0.00 }, //tip
        {  0.50,  0.05 }, { -0.12,  0.12 },
        { -0.12,  0.12 }, { -0.32,  0.50 },
        { -0.32,  0.50 }, {  0.05,  0.45 },
        {  0.05,  0.45 }, {  0.20,  0.25 },
        {  0.20,  0.25 }, { -0.50,  0.37 },
        { -0.50,  0.37 }, { -0.30,  0.00 },
        { -0.30,  0.00 }, { -0.50, -0.37 },
        { -0.50, -0.37 }, {  0.20, -0.25 },
        {  0.20, -0.25 }, {  0.05, -0.45 },
        {  0.05, -0.45 }, { -0.32, -0.50 },
        { -0.32, -0.50 }, { -0.12, -0.12 },
        { -0.12, -0.12 }, {  0.50, -0.05 },
        {  0.35,  0.00 }, {  0.28, -0.12 }, //tip
        { -0.50,  0.37 }, { -0.30,  0.00 }, //back chevron
        { -0.30,  0.00 }, { -0.50, -0.37 }, //back chevron
        { -0.02,  0.12 }, {  0.05,  0.00 }, //front chevron
        {  0.05,  0.00 }, { -0.02, -0.12 }, //front chevron
    },
    {
        { -0.50,  0.37 }, { -0.30,  0.00 }, //back chevron duplicated.
        {  0.50,  0.05 }, { -0.12,  0.12 },
        { -0.12,  0.12 }, { -0.32,  0.50 },
        { -0.32,  0.50 }, {  0.05,  0.45 },
        {  0.05,  0.45 }, {  0.20,  0.25 },
        {  0.20,  0.25 }, { -0.50,  0.37 },
        { -0.50,  0.37 }, { -0.30,  0.00 },
        { -0.30,  0.00 }, { -0.50, -0.37 },
        { -0.50, -0.37 }, {  0.20, -0.25 },
        {  0.20, -0.25 }, {  0.05, -0.45 },
        {  0.05, -0.45 }, { -0.32, -0.50 },
        { -0.32, -0.50 }, { -0.12, -0.12 },
        { -0.12, -0.12 }, {  0.50, -0.05 },
        { -0.50,  0.37 }, { -0.30,  0.00 }, //back chevron duplicated.
        { -0.45,  0.37 }, { -0.25,  0.00 }, //back chevron
        { -0.25,  0.00 }, { -0.45, -0.37 }, //back chevron
        {  0.03,  0.12 }, {  0.10,  0.00 }, //front chevron
        {  0.10,  0.00 }, {  0.03, -0.12 }, //front chevron
    },
    {
        { -0.50,  0.37 }, { -0.30,  0.00 }, //back chevron duplicated.
        {  0.50,  0.05 }, { -0.12,  0.12 },
        { -0.12,  0.12 }, { -0.32,  0.50 },
        { -0.32,  0.50 }, {  0.05,  0.45 },
        {  0.05,  0.45 }, {  0.20,  0.25 },
        {  0.20,  0.25 }, { -0.50,  0.37 },
        { -0.50,  0.37 }, { -0.30,  0.00 },
        { -0.30,  0.00 }, { -0.50, -0.37 },
        { -0.50, -0.37 }, {  0.20, -0.25 },
        {  0.20, -0.25 }, {  0.05, -0.45 },
        {  0.05, -0.45 }, { -0.32, -0.50 },
        { -0.32, -0.50 }, { -0.12, -0.12 },
        { -0.12, -0.12 }, {  0.50, -0.05 },
        { -0.50,  0.37 }, { -0.30,  0.00 }, //back chevron duplicated.
        { -0.40,  0.37 }, { -0.20,  0.00 }, //back chevron
        { -0.20,  0.00 }, { -0.40, -0.37 }, //back chevron
        {  0.08,  0.12 }, {  0.15,  0.00 }, //front chevron
        {  0.15,  0.00 }, {  0.08, -0.12 }, //front chevron
    },
    {
        { -0.50,  0.37 }, { -0.30,  0.00 }, //back chevron duplicated.
        {  0.50,  0.05 }, { -0.12,  0.12 },
        { -0.12,  0.12 }, { -0.32,  0.50 },
        { -0.32,  0.50 }, {  0.05,  0.45 },
        {  0.05,  0.45 }, {  0.20,  0.25 },
        {  0.20,  0.25 }, { -0.50,  0.37 },
        { -0.50,  0.37 }, { -0.30,  0.00 },
        { -0.30,  0.00 }, { -0.50, -0.37 },
        { -0.50, -0.37 }, {  0.20, -0.25 },
        {  0.20, -0.25 }, {  0.05, -0.45 },
        {  0.05, -0.45 }, { -0.32, -0.50 },
        { -0.32, -0.50 }, { -0.12, -0.12 },
        { -0.12, -0.12 }, {  0.50, -0.05 },
        { -0.50,  0.37 }, { -0.30,  0.00 }, //back chevron duplicated.
        { -0.35,  0.37 }, { -0.15,  0.00 }, //back chevron
        { -0.15,  0.00 }, { -0.35, -0.37 }, //back chevron
        {  0.13,  0.12 }, {  0.20,  0.00 }, //front chevron
        {  0.20,  0.00 }, {  0.13, -0.12 }, //front chevron
    },
    {
        { -0.50,  0.37 }, { -0.30,  0.00 }, //back chevron duplicated.
        {  0.50,  0.05 }, { -0.12,  0.12 },
        { -0.12,  0.12 }, { -0.32,  0.50 },
        { -0.32,  0.50 }, {  0.05,  0.45 },
        {  0.05,  0.45 }, {  0.20,  0.25 },
        {  0.20,  0.25 }, { -0.50,  0.37 },
        { -0.50,  0.37 }, { -0.30,  0.00 },
        { -0.30,  0.00 }, { -0.50, -0.37 },
        { -0.50, -0.37 }, {  0.20, -0.25 },
        {  0.20, -0.25 }, {  0.05, -0.45 },
        {  0.05, -0.45 }, { -0.32, -0.50 },
        { -0.32, -0.50 }, { -0.12, -0.12 },
        { -0.12, -0.12 }, {  0.50, -0.05 },
        { -0.50,  0.37 }, { -0.30,  0.00 }, //back chevron duplicated.
        { -0.30,  0.37 }, { -0.10,  0.00 }, //back chevron
        { -0.10,  0.00 }, { -0.30, -0.37 }, //back chevron
        {  0.18,  0.12 }, {  0.25,  0.00 }, //front chevron
        {  0.25,  0.00 }, {  0.18, -0.12 }, //front chevron
    },
    {
        { -0.50,  0.37 }, { -0.30,  0.00 }, //back chevron duplicated.
        {  0.50,  0.05 }, { -0.12,  0.12 },
        { -0.12,  0.12 }, { -0.32,  0.50 },
        { -0.32,  0.50 }, {  0.05,  0.45 },
        {  0.05,  0.45 }, {  0.20,  0.25 },
        {  0.20,  0.25 }, { -0.50,  0.37 },
        { -0.50,  0.37 }, { -0.30,  0.00 },
        { -0.30,  0.00 }, { -0.50, -0.37 },
        { -0.50, -0.37 }, {  0.20, -0.25 },
        {  0.20, -0.25 }, {  0.05, -0.45 },
        {  0.05, -0.45 }, { -0.32, -0.50 },
        { -0.32, -0.50 }, { -0.12, -0.12 },
        { -0.12, -0.12 }, {  0.50, -0.05 },
        { -0.50,  0.37 }, { -0.30,  0.00 }, //back chevron duplicated.
        { -0.25,  0.37 }, { -0.05,  0.00 }, //back chevron
        { -0.05,  0.00 }, { -0.25, -0.37 }, //back chevron
        {  0.23,  0.12 }, {  0.30,  0.00 }, //front chevron
        {  0.30,  0.00 }, {  0.23, -0.12 }, //front chevron
    },
};

} // namespace


King::King(void)
{
    //fprintf(stderr, "King::King()\n");
    set_scale(18.0);
    points = king_points[0];
    num_points = sizeof(king_points[0]) / sizeof(coords);
    xpoints = new XPoint[num_points];
    fireball = new Fireball;
    theta = 90.0;
    dtheta = 32.0 / args.fps;
    x = wwidth2;
    y = gwheight2;
    gc = fetch_gc(GC_BRIGHT_YELLOW);
    living = true;
} // King::King


King::~King(void)
{
    //fprintf(stderr, "King::~King()\n");
    delete fireball;
} // King::~King


void
King::render(const bool ink)
{
    if (alive()) {
	if (ink) {
	    set_xpoints();
	}
	paint_points(ink);
    }
    fireball->render(ink);
} // King::render

void
King::attract()
{
  animation_age += (1.0/args.fps);
  animation_age = fmod (animation_age, 
                        total_elapsed_time_to_show_full_animation);
  double frame = animation_age / total_elapsed_time_to_show_full_animation 
    * 1.0 * (double)num_animation_frames;
  points = king_points[int(frame)];
  Tthing::turn();
  set_xpoints();
} // King::attract

void
King::turn(Castle *castle, Ship *ship, Stats *stats)
{
  set_xpoints();
  animation_age += (1.0/args.fps);
  animation_age = fmod (animation_age, 
                        total_elapsed_time_to_show_full_animation);
  double frame = animation_age / total_elapsed_time_to_show_full_animation 
    * 1.0 * (double)num_animation_frames;
    points = king_points[int(frame)];
    if (alive() && ship->alive()) {
	dtheta = (wedge(theta, ship->get_angle()) * Difficulty::turning_speed_of_king) / args.fps;
	Tthing::turn();

	if (castle->opening(theta) && castle->opening(theta + 14) &&
		castle->opening(theta - 14)) {
	    fireball->launch(this, stats);
	}
    }

    fireball->move(ship, stats);
} // King::turn


void
King::resize(const int nwidth, const int nheight)
{
    Tthing::resize(nwidth, nheight);
    fireball->resize(nwidth, nheight);
} // King::resize


bool
King::hit(Castle *castle, Laser *laser, Minefield *minefield, Stats *stats)
{
    if (alive() && laser->get_radius() < diag) {
	stats->lives++;
        stats->add_score(1440);
	castle->explode(stats);
	minefield->snuff();
        fireball->snuff(); 
	living = false;
	return true;
    }
    return false;
} // King::hit


void
King::pause(const bool pause_state) const
{
    fireball->pause(pause_state);
} // King::pause
    
bool 
King::fireball_hit (Laser *laser, Stats *stats)
{
  return fireball->hit(laser, stats);
} // King::fireball_hit
