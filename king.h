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

#ifdef	XSTARCASTLE_KING_H

class King;

#else	// XSTARCASTLE_KING_H
#define	XSTARCASTLE_KING_H

#include "castle.h"
#include "fireball.h"
#include "laser.h"
#include "minefield.h"
#include "ship.h"
#include "stats.h"
#include "tthing.h"

class King : public virtual Tthing {
private:
    Fireball *fireball;
    bool living;
    double animation_age;

public:
    King(void);
    ~King(void);

    void render(const bool);

    void turn(Castle *, Ship *, Stats *);
    void attract();
    void resize(const int, const int);
    bool hit(Castle *, Laser *, Minefield *, Stats *);
    bool fireball_hit (Laser *, Stats *);
    void reincarnate(void);
    void pause(const bool) const;

    bool alive(void) const;
};


inline void
King::reincarnate(void)
{
    living = true;
} // King::reincarnate


inline bool
King::alive(void) const
{
    return living;
} // King::alive

#endif	// XSTARCASTLE_KING_H
