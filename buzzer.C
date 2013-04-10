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

#include "buzzer.h"

using namespace Trig;


namespace {
const double time_to_wait_before_reattaching_mine_to_ring = 0.05;
const double avg_time_to_leave_a_ring_when_player_dead = 4.0;

const coords buzzer_points[] = {
    {  COS00 / 2.0, -SIN00 / 2.0 }, { -COS00 / 2.0,  SIN00 / 2.0 },
    {  COS30 / 2.0, -SIN30 / 2.0 }, { -COS30 / 2.0,  SIN30 / 2.0 },
    {  COS60 / 2.0, -SIN60 / 2.0 }, { -COS60 / 2.0,  SIN60 / 2.0 },
    {  COS90 / 2.0,  SIN90 / 2.0 }, { -COS90 / 2.0, -SIN90 / 2.0 },
    {  COS60 / 2.0,  SIN60 / 2.0 }, { -COS60 / 2.0, -SIN60 / 2.0 },
    {  COS30 / 2.0,  SIN30 / 2.0 }, { -COS30 / 2.0, -SIN30 / 2.0 },
};

} //namespace


Buzzer::Buzzer(void)
{
    //fprintf(stderr, "Buzzer::Buzzer()\n");
    set_scale(60.0);
    state = MINE_DEAD;
    paused = false;
    points = buzzer_points;
    num_points = sizeof (buzzer_points) / sizeof (coords);
    xpoints = new XPoint[num_points];
    randomizer = new double[num_points];
    riding_on = new bool[MAX_RINGS];
    fudge = new double[MAX_RINGS];
    set_gc(fetch_gc(GC_DULL_BLUE));
    adjust_speed();

    for (int i = 0; i < MAX_RINGS; i++)
      riding_on[i] = false;
    for (int i = 0; i < MAX_RINGS; i++)
      fudge[i] = 0.0;
} // Buzzer::Buzzer


Buzzer::~Buzzer(void)
{
    //fprintf(stderr, "Buzzer::~Buzzer()\n");
    delete[] randomizer;
    delete[] riding_on;
    delete[] fudge;
} // Buzzer::~Buzzer

void
Buzzer::set_randomizer(void)
{
  if (paused) {
    return;
  }

  for (int i = 0; i < num_points; i++) {
    randomizer[i] = (Random::get() % 900) / 1000.0 + 0.10;
  }
} // Buzzer::setrandomizer

void
Buzzer::render(const bool ink)
{
  if (!alive()) {
    return;
  }

  if (ink) {
      set_randomizer();
      set_xpoints();
  }
  paint_points(ink);
} // Buzzer::render

void Buzzer::launch_from_ring(Castle *castle, const int ring_idx)
{
  riding_on[ring_idx] = false;
  fudge[ring_idx] = time_to_wait_before_reattaching_mine_to_ring;
  set_gc(fetch_gc(GC_DULL_BLUE));
  theta = get_angle(); 
  dtheta = 0.0;
  update_speed();
} // Buzzer::launch_from_ring


void
Buzzer::move(Castle *castle, Ship *ship, Stats *stats)
{
    if (!alive()) {
	return;
    }

    //hey a ring segment we were on was destroyed
    for (int i = 0; i < castle->get_numrings(); i++)
      {
        if (riding_on[i] == true)
          {
            if (castle->hole(i, get_angle()))
              {
                riding_on[i] = false;
                fudge[i] = 0.0;
              }
          }
      }

    if (riding_ring(castle) == false)
      {
        update_speed();
    Xything::move();
      } else {
    for (int i = 0; i < castle->get_numrings(); i++)
      if (riding_on[i] == true)
        {
          //do we launch?
	if ((ship->alive() &&
             Random::get() % (int)(Difficulty::avg_time_for_buzzer_to_leave_a_ring * args.fps) == 0)) {

          launch_from_ring (castle, i);
        } else if (!ship->alive() && Random::get() % 
                   (int)(avg_time_to_leave_a_ring_when_player_dead * args.fps) == 0) {
	    riding_on[i] = false;
            fudge[i] = time_to_wait_before_reattaching_mine_to_ring;
            update_speed();
        } else {
          castle->seg_center(i, get_angle(), &x, &y);
        }
        }

      }
    
    // check to see if we should attach to a ring segment
    for (int i = 0; i < castle->get_numrings(); i++) {
	double ring_radius = castle->ring_size(i) / 2.0;
	bool segment_exists = !castle->hole(i, get_angle());
	bool am_outside = get_radius() > ring_radius;
	bool am_inside = get_radius() < ring_radius;
	double new_radius =
          hypot(wwidth2 - (x + dx), gwheight2 - (y + dy));
	bool going_inside = new_radius <= ring_radius;
	bool going_outside = new_radius >= ring_radius;

	if (segment_exists && ((am_outside && going_inside) ||
                               (am_inside && going_outside))) {
          if (riding_on[i] == false && fudge[i] == 0.0)
            {
              riding_on[i] = true;
              set_gc(castle->get_gc(i));
              castle->seg_center(i, get_angle(), &x, &y);
              dtheta = 0.0;
              dx = 0.0;
              dy = 0.0;
              return;
            }
        }
    }

    for (int i = 0; i < castle->get_numrings(); i++)
      {
        if (fudge[i] > 0)
          {
            fudge[i] -= 1.0/args.fps;
            if (fudge[i] < 0)
              fudge[i] = 0.0;
          }
      }

    if (ship->hit(this))
      {
        snuff();
        stats->killed_by_a_buzzer();
      }
} // Buzzer::move

void
Buzzer::upgrade(Castle *castle)
{
    const int numrings = castle->get_numrings();
    bool *new_riding = new bool[numrings];
    int i;

    for (i = 0; i < numrings; i++) {
	new_riding[i] = false;
    }
    for (i = numrings - 1; i >= 0; i--) {
	if (!riding_on[i]) {
	    continue;
	}
	if (i > 0) {
	    new_riding[i - 1] = true;
	    set_gc(castle->get_gc(i - 1));
	    castle->seg_center(i - 1, get_angle(), &x, &y);
	} else {
	    set_gc(fetch_gc(GC_DULL_BLUE));
	    double deg = get_angle();
	    double ring_radius = castle->ring_size(i) / 2.0;
	    x = wwidth2 + (xcos(deg) * ring_radius);
	    y = gwheight2 + (-xsin(deg) * ring_radius);
	}
    }
    for (i = 0; i < numrings; i++) {
	riding_on[i] = new_riding[i];
    }

    delete[] new_riding;
} // Buzzer::upgrade

void
Buzzer::launch_from_king(King *king, Castle *castle)
{
    const double deg = normalize(king->get_theta() + 180);
    const int numrings = castle->get_numrings();

    // find a location near the back end of the king
    x = wwidth2 + (xcos(deg) * (wwidth / 30.0));
    y = gwheight2 + (-xsin(deg) * (wwidth / 30.0));
    theta = deg;
    dtheta = 0.0;
    update_speed();

    for (int i = 0; i < numrings; i++) {
	riding_on[i] = false;
    }

    set_gc(fetch_gc(GC_DULL_BLUE));

    state = MINE_LIVING;
} // Buzzer::launch_from_king



bool
Buzzer::hit(Laser *laser, Stats *stats)
{
    if (alive() && hypot(laser->get_x() - x, laser->get_y() - y) <
	    (diag + laser->get_diag()) / 2.0) {
	snuff();
        stats->add_score(0); // we get no points for mines.
        play (BUZZER_EXPLODE_SOUND);
        stats->destroyed_a_buzzer();
	return true;
    }
    return false;
} // Buzzer::hit

void
Buzzer::set_xpoints(void)
{
  const double sine = xsin(theta);
  const double cosine = xcos(theta);

  for (int i = 0; i < num_points; i++) {
    double nsize = randomizer[i] * size;
    const double tx = nsize * points[i].x;
    const double ty = nsize * points[i].y;

    xpoints[i].x = (short)((tx * cosine) + (ty * sine) + x);
    xpoints[i].y = (short)(-(tx * sine) + (ty * cosine) + y);

  }
} // Buzzer::set_xpoints

void
Buzzer::resize(const int nwidth, const int nheight)
{
    speed *= (double)nwidth / wwidth;
    Xything::resize(nwidth, nheight);
} // Buzzer::resize


void
Buzzer::place_randomly(Castle *castle)
{
  const int numrings = castle->get_numrings();
  for (int i = 0; i < numrings; i++)
    riding_on[i] = false;
  int ring = Random::get() % castle->get_numrings();
  riding_on[ring] = true;
  fudge[ring] = time_to_wait_before_reattaching_mine_to_ring;
  set_gc(castle->get_gc(ring));
  double degrees = Random::get() % 360;
  castle->seg_center(ring, degrees, &x, &y);
  state = MINE_LIVING;
  theta = get_angle();
  dtheta = 0.0;
  return;
} // Buzzer::place_randomly

void
Buzzer::adjust_speed()
{
  speed = (wwidth / (Difficulty::speed_of_buzzers + (Random::get() % 6)) / args.fps);
} // Buzzer::adjust_speed

bool
Buzzer::riding_ring(Castle *c) const
{
  bool riding = false;
  const int numrings = c->get_numrings();
  for (int i = 0; i < numrings; i++)
    if (riding_on[i])
      {
            riding = true;
            break;
      }

  return riding;
} // Buzzer::riding_ring

bool
Buzzer::outside() const
{
  if (gcs[int(y)][int(x)] == fetch_gc(GC_DULL_BLUE))
    return true;
  return false;
} // Buzzer::outside

void
Buzzer::update_speed()
{
  dx = xcos(theta) * speed;
  dy = -xsin(theta) * speed;

} // Buzzer::update_speed


void
Buzzer::turn(Castle *castle, Ship *ship)
{
  if (!alive()) {
    return;
  }
  if (riding_ring(castle))
    {
      dtheta = 0;
      theta = get_angle();
      return;
    }

  //const double acceleration = 1440.0; //*2 makes it turn tighter.
  //const double delta = acceleration / sq(args.fps);
  //const double acceleration = 180;
  //const double acceleration = 70;
  const double acceleration = Difficulty::buzzer_turning_radius;
  const double delta = acceleration / args.fps;
  const double to_ship = xatan2(ship->get_y() - y, x - ship->get_x());
  const double to_castle = xatan2(gwheight2 - y, x - wwidth2);
  const double target = ship->alive() ? to_ship : to_castle;
  dtheta = 0.0;
  if (target <= theta + (delta*1) && target >= theta - (delta*1))
    dtheta = 0.0;
  else
    {
      int direction = sign(theta-target);
      if (fabs (theta-target) > 180.0)
        direction*=-1;

      dtheta += -direction * delta;
    }

  Ething::turn();
} // Buzzer::turn

