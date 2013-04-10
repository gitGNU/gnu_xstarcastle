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

#ifdef	XSTARCASTLE_SHIP_H

class Ship;

#else	// XSTARCASTLE_SHIP_H
#define	XSTARCASTLE_SHIP_H

#include "castle.h"
#include "ething.h"
#include "king.h"
#include "laser.h"
#include "minefield.h"
#include "stats.h"
#include "xything.h"
#include "kapow.h"
#include "sound.h"

const int NUM_SHIP_POINTS = 22;

extern const struct coords ship_points[2][NUM_SHIP_POINTS];


class Ship : public virtual Ething {
private:
    enum ship_states {
	SHIP_LIVING,
	SHIP_EXPLODING,
	SHIP_RESTING,
	SHIP_DEAD
    };
    enum ship_states state;

    bool user_rotating_cw;
    bool user_rotating_ccw;
    bool user_thrusting;
    int max_shots;
    Laser *lasers;
    Kapow *explosion;
    Stamp time_of_death;
    Stamp pause_start;
    Stamp pause_sum;

    double initial_x(void);
    double initial_y(void);

    void bounce(Castle *);

public:
    Ship(void);
    ~Ship(void);

    void render(const bool);

    void move(Castle *, King *, Minefield *, Stats *);
    void turn(void);
    void fire(Stats*);
    void resize(const int, const int);
    bool hit(Xything *);
    void reincarnate(void);

    void rotate_cw(const enum key_state);
    void rotate_ccw(const key_state);
    void thrust(const key_state);
    bool rotating_cw(void) const;
    bool rotating_ccw(void) const;
    bool rotating(void) const;
    bool thrusting(void) const;
    bool alive(void) const;
    void pause(const bool);
    bool resting() const;
};


inline bool
Ship::resting() const
{
  if (state == SHIP_RESTING)
    return true;
  return false;
}

inline void
Ship::rotate_cw(const enum key_state n)
{
    user_rotating_cw = (n == KEY_DOWN);
} // Ship::rotate_cw


inline void
Ship::rotate_ccw(const key_state n)
{
    user_rotating_ccw = (n == KEY_DOWN);
} // Ship::rotate_ccw


inline void
Ship::thrust(const key_state n)
{
    user_thrusting = (n == KEY_DOWN);
} // Ship::thrust


inline bool
Ship::rotating_cw(void) const
{
    return user_rotating_cw;
} // Ship::rotating_cw


inline bool
Ship::rotating_ccw(void) const
{
    return user_rotating_ccw;
} // Ship::rotating_ccw


inline bool
Ship::thrusting(void) const
{
    return user_thrusting;
} // Ship::thrusting


inline bool
Ship::alive(void) const
{
    return (state == SHIP_LIVING);
} // Ship::alive


inline void
Ship::pause(const bool pause_state)
{
    if (pause_state) {
	pause_start = time_now;
    } else {
	pause_sum += time_now - pause_start;
    }
} // Ship::pause

#endif	// XSTARCASTLE_SHIP_H
