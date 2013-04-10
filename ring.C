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

#include "ring.h"

#include "random.h"
using namespace Trig;


namespace {

const coords ring_points[] = {
    {  COS00 / 2.0, -SIN00 / 2.0 }, {  COS30 / 2.0, -SIN30 / 2.0 },
    {  COS30 / 2.0, -SIN30 / 2.0 }, {  COS60 / 2.0, -SIN60 / 2.0 },
    {  COS60 / 2.0, -SIN60 / 2.0 }, {  COS90 / 2.0, -SIN90 / 2.0 },
    {  COS90 / 2.0, -SIN90 / 2.0 }, { -COS60 / 2.0, -SIN60 / 2.0 },
    { -COS60 / 2.0, -SIN60 / 2.0 }, { -COS30 / 2.0, -SIN30 / 2.0 },
    { -COS30 / 2.0, -SIN30 / 2.0 }, { -COS00 / 2.0, -SIN00 / 2.0 },
    { -COS00 / 2.0, -SIN00 / 2.0 }, { -COS30 / 2.0,  SIN30 / 2.0 },
    { -COS30 / 2.0,  SIN30 / 2.0 }, { -COS60 / 2.0,  SIN60 / 2.0 },
    { -COS60 / 2.0,  SIN60 / 2.0 }, { -COS90 / 2.0,  SIN90 / 2.0 },
    { -COS90 / 2.0,  SIN90 / 2.0 }, {  COS60 / 2.0,  SIN60 / 2.0 },
    {  COS60 / 2.0,  SIN60 / 2.0 }, {  COS30 / 2.0,  SIN30 / 2.0 },
    {  COS30 / 2.0,  SIN30 / 2.0 }, {  COS00 / 2.0, -SIN00 / 2.0 },
};

} // namespace


Ring::Ring(const double nscale, const double speed, const unsigned int v,
	const GC *const gclist, const int ngcs)
{
    //fprintf(stderr, "Ring::Ring()\n");
    set_scale(nscale);
    points = ring_points;

    num_points = sizeof(ring_points) / sizeof(coords);
    xpoints = new XPoint[num_points];

    numsegments = num_points / 2;
    hits = new hitstatus[numsegments];
    refresh();

    dtheta = speed;
    theta = 0.0;

    x = wwidth2;
    y = gwheight2;

    value = v;

    gcs = new GC[ngcs];
    for (int i = 0; i < ngcs; i++) {
	gcs[i] = gclist[i];
    }
} // Ring::Ring


Ring::~Ring(void)
{
    //fprintf(stderr, "Ring::~Ring()\n");
    delete[] hits;
    delete[] gcs;
} // Ring::~Ring


void
Ring::render(const bool ink)
{
    if (ink) {
	set_xpoints();
    }

    const GC blackgc = fetch_gc(GC_BLACK);

    for (int i = 1; i < num_points; i += 2) {
        if (hits[i/2] == RING_DEAD)
            continue;
	const GC drawgc = ink ? gcs[hits[i/2]] : blackgc;
	if (!ink || drawgc != blackgc) {
	    _XDrawLine(display, window, drawgc,
		    xpoints[i - 1].x, xpoints[i - 1].y,
		    xpoints[i].x, xpoints[i].y, hits[i/2]==RING_DULL? false:true);
	}
    }
} // Ring::render


inline int
Ring::which_seg(const double degrees) const
{
    return (int)(normalize(wedge(theta, degrees)) / (360.0 / numsegments));
} // Ring::which_seg


void
Ring::seg_center(const double degrees, double *const xptr,
	double *const yptr) const
{
    const int segment = which_seg(degrees) * 2;
    *xptr = (xpoints[segment].x + xpoints[segment + 1].x) / 2.0;
    *yptr = (xpoints[segment].y + xpoints[segment + 1].y) / 2.0;
} // Ring::seg_center


bool
Ring::hole(const double degrees) const
{
    if (hits[which_seg(degrees)] < RING_DEAD) {
	return false;
    }
    return true;
} // Ring::hole


bool
Ring::hit(Laser *laser, Stats *stats, bool *dead)
{
    const int segment = which_seg(laser->get_angle());

    switch (hits[segment]) {
	case RING_DULL:
	    hits[segment] = RING_BRIGHT;
	    stats->add_score(value);
	    return true;
	case RING_BRIGHT:
	    hits[segment] = RING_DEAD;
	    stats->add_score(value);
            if (dead)
              *dead = true;
	    return true;
	case RING_DEAD:
	    return false;
    }

    return false;
} // Ring::hit


int
Ring::remaining(void) const
{
    int retval = 0;

    for (int i = 0; i < numsegments; i++) {
	if (hits[i] < RING_DEAD) {
	    retval++;
	}
    }
    return retval;
} // Ring::remaining


void
Ring::refresh(void)
{
  set_xpoints();//hackish
    for (int i = 0; i < numsegments; i++) {
	hits[i] = RING_DULL;
    }
} // Ring::refresh


void
Ring::snuff()
{
  for (int i = 0; i < numsegments; i++)
    hits[i] = RING_DEAD;
} //Ring::snuff
