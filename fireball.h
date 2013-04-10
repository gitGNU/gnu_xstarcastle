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

#ifdef	XSTARCASTLE_FIREBALL_H

class Fireball;

#else	// XSTARCASTLE_FIREBALL_H
#define	XSTARCASTLE_FIREBALL_H

#include "king.h"
#include "ship.h"
#include "stats.h"
#include "xything.h"
#include "laser.h"

class Fireball : public virtual Xything {
private:
    bool flying;
    double *randomizer;
    bool paused;

    void set_randomizer(void);

public:
    Fireball(void);
    ~Fireball(void);

    void render(const bool);

    void move(Ship *, Stats *);
    void launch(King *, Stats *);
    void set_xpoints(void);

    bool hit(Laser *laser, Stats *stats);

    bool alive(void) const;
    void snuff(void);
    void pause(const bool);
};


inline bool
Fireball::alive(void) const
{
    return flying;
} // Fireball::alive


inline void
Fireball::snuff(void)
{
    flying = false;
} // Fireball::snuff


inline void
Fireball::pause(const bool pause_state)
{
    paused = pause_state;
} // Fireball::pause

#endif	// XSTARCASTLE_FIREBALL_H
