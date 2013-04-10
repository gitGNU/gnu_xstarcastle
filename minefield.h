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

#ifdef	XSTARCASTLE_MINEFIELD_H

class Minefield;

#else	// XSTARCASTLE_MINEFIELD_H
#define	XSTARCASTLE_MINEFIELD_H

#include "buzzer.h"
#include "castle.h"
#include "king.h"
#include "laser.h"
#include "ship.h"
#include "stats.h"
#include "kapow.h"
#include <list>

class Minefield {
public:
static const int max_number_of_mines = 3;
private:

    Buzzer *buzzers;
    std::list<Kapow> *expl;

public:
    Minefield(void);
    ~Minefield(void);

    void render(const bool) const;
    void draw(void) const;
    void erase(void) const;

    void move(Castle *, Ship *, Stats *) const;
    void resize(const int, const int) const;
    bool hit(Laser *, Stats *) const;
    void launch(King *, Castle *, Stats *) const;
    void expire() const;
    void upgrade(Castle *) const;
    void snuff(void) const;
    void pause(bool) const;
    void spawn_buzzers(Castle *,int);
    void adjust_speed();
    void turn(Castle *, Ship *);
};


inline void
Minefield::draw(void) const
{
    render(true);
} // Minefield::draw


inline void
Minefield::erase(void) const
{
    render(false);
} // Minefield::erase

#endif	// XSTARCASTLE_MINEFIELD_H
