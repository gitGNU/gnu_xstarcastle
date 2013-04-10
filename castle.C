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

#include "timing.h"
#include "util.h"
#include "xsc.h"
#include "sound.h"

#include "spikes.h"
#include "kapow.h"
#include "castle.h"
#include "random.h"

const int maximum_number_of_rings = MAX_RINGS;

namespace {

const unsigned int values[maximum_number_of_rings] = { 10, 20, 30 };
const double scales[maximum_number_of_rings] = { 3.65, 5, 8 };

const int ringstates = 3;
const gc_token ringctoks[maximum_number_of_rings][ringstates] = {
    { GC_DULL_RED, GC_BRIGHT_RED, GC_BLACK },
    { GC_DULL_ORANGE, GC_BRIGHT_ORANGE, GC_BLACK },
    { GC_DULL_YELLOW, GC_BRIGHT_YELLOW, GC_BLACK },
};

} // namespace


Castle::Castle(void)
{
    //fprintf(stderr, "Castle::Castle()\n");
    numrings = maximum_number_of_rings;
    rings = new Ring *[numrings];

    state = CASTLE_STATE_NORMAL;

    GC gcs[ringstates];

    for (int i = 0; i < numrings; i++) {
	for (int j = 0; j < ringstates; j++) {
	    gcs[j] = fetch_gc(ringctoks[i][j]);
	}
	rings[i] = new Ring(scales[i], (Difficulty::ring_speed[i] / args.fps), values[i],
		gcs, ringstates);
    }
    expl = new std::list<Kapow>();
    expl->clear();
    spikes = new std::list<Spikes>();
    spikes->clear();
} // Castle::Castle


Castle::~Castle(void)
{
    //fprintf(stderr, "Castle::~Castle()\n");
    for (int i = 0; i < numrings; i++) {
	delete rings[i];
    }
    delete[] rings;
    delete expl;
    delete spikes;
} // Castle::~Castle


void
Castle::render(const bool ink)
{
    for (int i = 0; i < numrings; i++) {
	rings[i]->render(ink);
    }
    for (std::list<Kapow>::iterator i = expl->begin(); i != expl->end(); i++) {
      (*i).render(ink);
    }
    for (std::list<Spikes>::iterator i = spikes->begin(); 
         i != spikes->end(); i++) {
      (*i).render(ink);
    }
} // Castle::render


void
Castle::turn(Minefield *minefield, King *king, Stats *stats)
{
    bool go_again;

    for (int i = 0; i < numrings; i++) {
	rings[i]->turn();
    }
    for (std::list<Kapow>::iterator i = expl->begin(); i != expl->end(); i++) {
      (*i).turn();
    }
    for (std::list<Spikes>::iterator i = spikes->begin(); 
         i != spikes->end(); i++) {
      (*i).turn();
    }
    std::list<Kapow>::iterator k = expl->begin();
    while (k != expl->end()) {
      if (!(*k).alive())
        expl->erase(k++);
      else
        ++k;
    }
    std::list<Spikes>::iterator s = spikes->begin();
    while (s != spikes->end()) {
      if ((*s).alive())
        spikes->erase(s++);
      else
        ++s;
    }

    if (state == CASTLE_STATE_REGENERATING) {
	// if currently regenerating, slowly increase the size of the rings
	go_again = false;
	for (int i = 0; i < numrings; i++) {
	    double tmp = rings[i]->get_scale();

	    if (tmp > scales[i]) {
		rings[i]->set_scale(tmp -
			(((tmp - scales[i]) * 4.0) / args.fps));
	    }

	    // close enough
	    if (rings[i]->get_scale() - scales[i] < 0.01) {
		rings[i]->set_scale(scales[i]);
	    } else {
		go_again = true;
	    }
	}
	if (!go_again) {
	    state = CASTLE_STATE_NORMAL;
	}
    } else if (state == CASTLE_STATE_COLLAPSING) {
	// collapse the rings inward
        go_again = false;
	for (int i = 0; i < numrings; i++) {
	    double tmp = rings[i]->get_scale();

	    if (tmp < 1000.0) {
		rings[i]->set_scale(tmp + (tmp * 8.0) / args.fps);
	    }

	    // close enough
	    if (1000.0 - rings[i]->get_scale() < 0.01) {
		rings[i]->set_scale(1000.0);
	    } else {
		go_again = true;
	    }
	}
        if (!go_again) {
          for (int i = 0; i < numrings; i++) {
            rings[i]->set_scale(0.0);
            rings[i]->snuff();
          }
          state = CASTLE_STATE_EXPLODING;
          explosion_age=0;
          //okay let's start the first of 3 explosions.
          Kapow *explosion= new Kapow(fetch_gc(GC_DULL_RED), 1.2, 36);
          explosion->set_scale(50000);
          explosion->set_step(1.101);
          expl->push_back(*explosion);
          explosion_count++;
        }
    } else if (state == CASTLE_STATE_EXPLODING) {
	// expand rings beyond the edge of the screen
        go_again = true;
        explosion_age += 1.0/args.fps;
        if (explosion_age > 0.17)
          {
            explosion_age= 0;
            if (explosion_count < 3)
              {
                Kapow *explosion= new Kapow(fetch_gc(ringctoks[explosion_count][1]), 0.9, 28);
                explosion_count++;
                explosion->set_scale(50000);
                explosion->set_step(1.121);
                expl->push_back(*explosion);
              }
            else
              go_again = false;
          }
        if (!go_again) {
          explosion_count = 0;
          explosion_age = 0.0;
          state = CASTLE_STATE_RESTING;
          time_of_death = time_now; 
          pause_sum = 0;
        }
    } else if (state == CASTLE_STATE_RESTING) {
	// rest a bit before reincarnating
	if ((time_now - time_of_death) - pause_sum > 2000000L) {
            Difficulty::increase(stats);
            minefield->adjust_speed();
            minefield->spawn_buzzers(this, -1);
	    state = CASTLE_STATE_REGENERATING;
            play (CASTLE_REGENERATE_SOUND);
            // this is where we make the scales a little smaller so that
            // the rings can spin and grow into their proper shape.
            // we also change the spinning speeds and directions here
            // because they might have changed in Difficulty::increase.
	    for (int i = 0; i < numrings; i++) {
		if (i == numrings - 1) { 
		    rings[i]->set_scale(13);
		} else {
		    rings[i]->set_scale(scales[i + 1]);
		}
		rings[i]->refresh();
                rings[i]->set_theta(0.0);
                rings[i]->set_dtheta(Difficulty::ring_speed[i] / args.fps);

                if (king->alive() == false)
                  king->reincarnate();
	    }
	}
    } else if (!rings[0]->remaining()) {
	// enemy has destroyed entire outer ring
	Ring *tmpwall = rings[0];

	for (int i = 0; i < numrings - 1; i++) {
	    rings[i] = rings[i + 1];
	}
	rings[numrings - 1] = tmpwall;
	rings[numrings - 1]->set_scale(13);
	rings[numrings - 1]->refresh();

	for (int i = 0; i < numrings; i++) {
	    rings[i]->set_value(values[i]);
	    for (int j = 0; j < ringstates; j++) {
		rings[i]->set_gc(j, fetch_gc(ringctoks[i][j]));
	    }
	    rings[i]->set_dtheta(Difficulty::ring_speed[i] / args.fps);
	}
	state = CASTLE_STATE_REGENERATING;
        play (CASTLE_REGENERATE_SOUND);
	minefield->upgrade(this);
        stats->regenerated_a_ring();
    }
} // Castle::turn


void
Castle::resize(const int nwidth, const int nheight)
{
    for (int i = 0; i < numrings; i++) {
	rings[i]->resize(nwidth, nheight);
    }
} // Castle::resize


bool
Castle::collision(Laser *laser, Stats *stats)
{
  bool dead = false;
    for (int i = 0; i < numrings; i++) {
	if (within(laser->get_radius(), (rings[i]->get_size() / 2.0), 1.5)) {
	    if (rings[i]->hit(laser, stats, &dead)) {
              stats->hit_a_ring_segment();
              if (dead) {
                Kapow *k = new Kapow(fetch_gc(ringctoks[i][1]), 0.2, 7);
                k->set_scale(100000);
                k->set_step(1.3);
                k->set_x(laser->get_x());
                k->set_y(laser->get_y());
                expl->push_back(*k);
                play (RING_SEGMENT_EXPLODE_SOUND);
                stats->destroyed_a_ring_segment();
              }
              return true;
	    }
	}
    }
    return false;
} // Castle::collision


bool
Castle::opening(const double degrees) const
{
    for (int i = 0; i < numrings; i++) {
	if (!rings[i]->hole(degrees)) {
	    return false;
	}
    }
    return true;
} // Castle::opening


bool
Castle::hole(const int whichring, const double degrees) const
{
    return rings[whichring]->hole(degrees);
} // Castle::hole


double
Castle::ring_size(const int wallnum) const
{
    return rings[wallnum]->get_size();
} // Castle::ring_size


void
Castle::seg_center(const int ring_idx, /*const*/ double degrees,
	double * /*const*/ xptr, double * /*const*/ yptr)
{
    rings[ring_idx]->seg_center(degrees, xptr, yptr);
} // Castle::seg_center

void
Castle::explode(Stats *stats)
{
    play (CASTLE_EXPLODE_SOUND);
    stats->add_score(0);
    stats->destroyed_a_castle();
    state = CASTLE_STATE_COLLAPSING;
    Spikes *s = new Spikes(fetch_gc(ringctoks[2][1]));
    s->set_x(rings[0]->get_x());
    s->set_y(rings[0]->get_y());
    s->set_size(80);
    s->set_step(70);
    spikes->push_back(*s);
    s = new Spikes(fetch_gc(ringctoks[1][1]));
    s->set_x(rings[0]->get_x());
    s->set_y(rings[0]->get_y());
    s->set_size(50);
    s->set_step(40);
    spikes->push_back(*s);
    s = new Spikes(fetch_gc(ringctoks[0][1]));
    s->set_x(rings[0]->get_x());
    s->set_y(rings[0]->get_y());
    s->set_size(20);
    s->set_step(10);
    spikes->push_back(*s);
} // Castle::explode


GC
Castle::get_gc(const int wallnum) const
{
    return fetch_gc(ringctoks[wallnum][1]);
} // Castle::get_gc


void
Castle::refresh(void) const
{
    for (int i = 0; i < numrings; i++) {
	rings[i]->refresh();
    }
} // Castle::refresh
