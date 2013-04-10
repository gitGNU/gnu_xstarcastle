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
#include "util.h"
#include "xsc.h"
#include "sound.h"

#include "minefield.h"

Minefield::Minefield(void)
{
    //fprintf(stderr, "Minefield::Minefield()\n");
    buzzers = new Buzzer[max_number_of_mines];
    expl = new std::list<Kapow>();
    expl->clear();
} // Minefield::Minefield


Minefield::~Minefield(void)
{
    //fprintf(stderr, "Minefield::~Minefield()\n");
    delete[] buzzers;
    delete expl;
} // Minefield::~Minefield


void
Minefield::render(const bool ink) const
{
    for (int i = 0; i < max_number_of_mines; i++) {
	buzzers[i].render(ink);
    }
    for (std::list<Kapow>::iterator i = expl->begin(); i != expl->end(); i++) {
      (*i).render(ink);
    }
} // Minefield::render


void
Minefield::move(Castle *castle, Ship *ship, Stats *stats) const
{
    for (int i = 0; i < max_number_of_mines; i++) {
	buzzers[i].move(castle, ship, stats);
    }
    for (std::list<Kapow>::iterator i = expl->begin(); i != expl->end(); i++) {
      (*i).turn();
    }
    std::list<Kapow>::iterator k = expl->begin();
    while (k != expl->end()) {
      if (!(*k).alive())
        expl->erase(k++);
      else
        ++k;
    }

    int num_buzzers_not_riding_rings = 0;
    for (int i = 0; i < max_number_of_mines; i++) 
      {
        if (buzzers[i].alive() && !buzzers[i].riding_ring(castle) &&
            buzzers[i].outside())
          num_buzzers_not_riding_rings++;
      }
    if (num_buzzers_not_riding_rings == 0 && buzzer_sound_on == true)
      buzzer_sound(false);
    if (num_buzzers_not_riding_rings > 0 && buzzer_sound_on == false)
      buzzer_sound(true);
} // Minefield::move


void
Minefield::resize(const int nwidth, const int nheight) const
{
    for (int i = 0; i < max_number_of_mines; i++) {
	buzzers[i].resize(nwidth, nheight);
    }
} // Minefield::resize


bool
Minefield::hit(Laser *laser, Stats *stats) const
{
    for (int i = 0; i < max_number_of_mines; i++) {
	if (buzzers[i].hit(laser, stats)) {
                Kapow *k = new Kapow(buzzers[i].get_gc(), 0.2, 7);
                k->set_scale(150000);
                k->set_step(1.3);
                k->set_x(laser->get_x());
                k->set_y(laser->get_y());
                expl->push_back(*k);
	    return true;
	}
    }
    return false;
} // Minefield::hit


void
Minefield::launch(King *king, Castle *castle, Stats *stats) const
{
    if (!king->alive()) {
	return;
    }

    for (int i = 0; i < max_number_of_mines; i++) {
	if (!buzzers[i].alive() &&
		(Random::get() % (int)(Difficulty::avg_time_to_launch_new_buzzer_from_king * args.fps) == 0)) {
	    buzzers[i].launch_from_king(king, castle);
            stats->launched_a_buzzer();
	    return;
	}
    }
} // Minefield::launch


void
Minefield::expire() const
{
    for (int i = 0; i < max_number_of_mines; i++) 
      {
	if (buzzers[i].alive() && buzzers[i].outside() && 
		(Random::get() % (int)(Difficulty::avg_time_to_expire_outside_buzzers * args.fps) == 0)) 
          {
	    buzzers[i].snuff();
	    return;
	}
    }
} // Minefield::expire

void
Minefield::upgrade(Castle *castle) const
{
    for (int i = 0; i < max_number_of_mines; i++) {
	buzzers[i].upgrade(castle);
    }
} // Minefield::upgrade

void
Minefield::spawn_buzzers(Castle *castle, int num) 
{
  if (num < 0)
    num = max_number_of_mines;
  double mines = min (num, max_number_of_mines);
  for (int i = 0; i < int(mines); i++)
    buzzers[i].place_randomly(castle);
} // Minefield::spawn_buzzers

void
Minefield::snuff(void) const
{
    for (int i = 0; i < max_number_of_mines; i++) {
	buzzers[i].snuff();
    }
} // Minefield::snuff


void
Minefield::pause(bool pause_state) const
{
    for (int i = 0; i < max_number_of_mines; i++) {
	buzzers[i].pause(pause_state);
    }
} // Minefield::pause

void
Minefield::adjust_speed()
{
    for (int i = 0; i < max_number_of_mines; i++)
      buzzers[i].adjust_speed();
} // Minefield::adjust_speed


void
Minefield::turn(Castle *castle, Ship *ship)
{
    for (int i = 0; i < max_number_of_mines; i++)
      buzzers[i].turn(castle, ship);
}
