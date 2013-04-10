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
#include "timing.h"
#include "trig.h"
#include "util.h"
#include "xsc.h"

#include "ship.h"

using namespace Trig;


const coords ship_points[2][NUM_SHIP_POINTS] = {
    {
    {  0.50, -0.10 }, { -0.50, -0.18 },
    { -0.50, -0.18 }, {  0.30, -0.03 },
    {  0.30, -0.03 }, { -0.45, -0.50 },
    { -0.45, -0.50 }, { -0.05,  0.00 },
    { -0.05,  0.00 }, { -0.45,  0.50 },
    { -0.45,  0.50 }, {  0.30,  0.03 },
    {  0.30,  0.03 }, { -0.50,  0.18 },
    { -0.50,  0.18 }, {  0.50,  0.10 },
    { -0.50,  0.18 }, {  0.50,  0.10 }, //dup
    { -0.50,  0.18 }, {  0.50,  0.10 }, //dup
    { -0.50,  0.18 }, {  0.50,  0.10 }, //dup
    },
    {
    {  0.50, -0.10 }, { -0.50, -0.18 },
    { -0.50, -0.18 }, {  0.30, -0.03 },
    {  0.30, -0.03 }, { -0.45, -0.50 },
    { -0.45, -0.50 }, { -0.05,  0.00 },
    { -0.05,  0.00 }, { -0.45,  0.50 },
    { -0.45,  0.50 }, {  0.30,  0.03 },
    {  0.30,  0.03 }, { -0.50,  0.18 },
    { -0.50,  0.18 }, {  0.50,  0.10 },
    { -0.10,  0.00 }, { -0.53,  0.00 },
    { -0.10,  0.02 }, { -0.43,  0.02 },
    { -0.10,  -0.02 }, { -0.43,  -0.02 },
    }
};

const double max_turn_rate = 540.0;


Ship::Ship(void)
{
    int i;

    //fprintf(stderr, "Ship::Ship()\n");
    set_scale(25.0);
    user_rotating_cw = false;
    user_rotating_ccw = false;
    user_thrusting = false;

    theta = 180.0;

    points = ship_points[user_thrusting];
    num_points = NUM_SHIP_POINTS;

    xpoints = new XPoint[num_points];
    Tthing::set_xpoints();

    x = initial_x();
    y = initial_y();

    max_shots = 3;
    lasers = new Laser[max_shots];
    for (i = 0; i < max_shots; i++) {
	lasers[i].set_ship(this);
    }

    state = SHIP_DEAD;

    explosion = NULL;
} // Ship::Ship


Ship::~Ship(void)
{
    //fprintf(stderr, "Ship::~Ship()\n");
    delete[] lasers;
} // Ship::~Ship



double
Ship::initial_x(void)
{
    return wwidth * 0.9;
} // Ship::initial_x


double
Ship::initial_y(void)
{
    return (Random::get() % (int)(gwheight * 0.8)) + (gwheight * 0.1);
} // Ship::initial_y


void
Ship::render(const bool ink)
{
    for (int i = 0; i < max_shots; i++) {
	lasers[i].render(ink);
    }

    if (state == SHIP_EXPLODING) {
      if (explosion)
        explosion->render(ink);
    }

    if (!alive()) {
	return;
    }


    if (ink) {
	Tthing::set_xpoints();
    }

    paint_points(ink);

} // Ship::render


inline void
Ship::bounce(Castle *castle)
{
    const double deg = get_angle();
    const double dist = (castle->ring_size(CASTLE_RING_OUTER) / 2.0) + diag;

    x = (xcos(deg) * dist) + wwidth2;
    y = (-xsin(deg) * dist) + gwheight2;

    Xything::bounce(normalize(deg + 90.0));

    // bounce speed penalty
    dx /= 4.0;
    dy /= 4.0; 

    const double left = wedge(theta, deg - 90.0);
    const double right = wedge(theta, deg + 90.0); 
    const double which_way = (fabs(left) < fabs(right)) ? left : right;

    double fudge = 1.5; //add fudge so that the ship turns 180 degrees for head on collisions with the castle.
    dtheta = ((max_turn_rate*fudge) / args.fps) * (which_way / 90.0); 
} // Ship::bounce


void
Ship::move(Castle *castle, King *king, Minefield *minefield, Stats *stats)
{
    int i;
    points = ship_points[user_thrusting];
    Tthing::set_xpoints();

    for (i = 0; i < max_shots; i++) {
	lasers[i].move(castle, king, minefield, stats);
    }

    if (state == SHIP_EXPLODING) {
      if ((time_now - time_of_death) - pause_sum > 2500000L) {
        state = SHIP_RESTING;
        stats->lives--;
      }

      if (explosion)
        {
          if (explosion->alive())
            explosion->turn();
          else
            {
              delete explosion;
              explosion = NULL;
            }
        }
    }

    if (state == SHIP_RESTING &&
	    (time_now - time_of_death) - pause_sum > 4000000L) {
	if (stats->lives < 0)
	    state = SHIP_DEAD;
        else
          reincarnate(); 
        // not a fan of this call to reincarnate being here,
        // but if we move it to state_change it will break the showing
        // of player scores when the ship dies.
    }

    if (!alive()) {
	return;
    }

    if (user_thrusting) {
	const double acceleration = wwidth / 0.75;
	const double delta = acceleration / sq(args.fps);

	dx += xcos(theta) * delta;
	dy += -xsin(theta) * delta;

	const double speed = hypot(dx, dy);
	const double max_speed = (wwidth / 2.25) / args.fps;

	if (speed > max_speed) {
	    const double limit_factor = max_speed / speed;

	    dx *= limit_factor;
	    dy *= limit_factor;
	}
    } else {
	const double acceleration = wwidth / 11.0;
	const double delta = -acceleration / sq(args.fps);
	double speed = hypot(dx, dy) + delta;

	if (speed < 0.0) {
	    speed = 0.0;
	}

	const double drift_angle = xatan2(dy, -dx);

	dx = xcos(drift_angle) * speed;
	dy = -xsin(drift_angle) * speed;
    }

    if (king->alive()) {
	for (i = 0; i < num_points; i++) {
	    if (hypot(wwidth2 - (xpoints[i].x + dx),
		    gwheight2 - (xpoints[i].y + dy)) <
		    castle->ring_size(CASTLE_RING_OUTER) / 2.0) {
		bounce(castle);
		break;
	    }
	}
    }

    Xything::move();
} // Ship::move


void
Ship::turn(void)
{

    if (!alive()) {
	return;
    }

    if ((user_rotating_cw || user_rotating_ccw) &&
	    !(user_rotating_cw && user_rotating_ccw)) {
	const double acceleration = 720.0;
	const double delta = acceleration / sq(args.fps);

	if (user_rotating_cw) {
	    dtheta -= delta;
	} else if (user_rotating_ccw) {
	    dtheta += delta;
	}

	const double max_rate = max_turn_rate / args.fps;

	if (fabs(dtheta) > max_rate) {
	    dtheta = sign(dtheta) * max_rate;
	}
    } else {
	const double acceleration = 1440.0;
	const double delta = acceleration / sq(args.fps);
	const int direction = sign(dtheta);

	dtheta += -direction * delta;
	if (sign(dtheta) != direction) {
	    dtheta = 0.0;
	}
    }

    Ething::turn();
} // Ship::turn


void
Ship::fire(Stats *stats)
{
    if (!alive()) {
	return;
    }

    for (int i = 0; i < max_shots; i++) {
	if (!lasers[i].alive()) {
	    lasers[i].launch();
            stats->shot_a_laser();
	    return;
	}
    }
} // Ship::fire


void
Ship::resize(const int nwidth, const int nheight)
{
    int i;

    Xything::resize(nwidth, nheight);
    for (i = 0; i < max_shots; i++) {
	lasers[i].resize(nwidth, nheight);
    }
    if (explosion)
      explosion->resize(nwidth, nheight);
} // Ship::resize

bool
Ship::hit(Xything *badguy)
{
    if (!alive() || hypot(badguy->get_x() - x, badguy->get_y() - y) >=
	    (diag + badguy->get_diag()) / 2.0) {
	return false;
    }

    for (int i = 0; i < max_shots; i++) {
	lasers[i].snuff();
    }

    state = SHIP_EXPLODING;

    explosion = new Kapow(fetch_gc(GC_DULL_BLUE), 0.45, 14);
    explosion->set_x(get_x());
    explosion->set_y(get_y());
                explosion->set_scale(100000);
                explosion->set_step(1.28);

    x = wwidth2;		// send the buzzers back home
    y = gwheight2;
    time_of_death = time_now;
    pause_sum = 0;

    play(SHIP_EXPLODE_SOUND);
    return true;
} // Ship::hit


void
Ship::reincarnate(void)
{
    theta = 180.0;
    x = initial_x();
    y = initial_y();

    dtheta = 0.0;
    dx = 0.0;
    dy = 0.0;
    Tthing::set_xpoints();

    state = SHIP_LIVING;
} // Ship::reincarnate
