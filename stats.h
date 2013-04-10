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

#ifdef	XSTARCASTLE_STATS_H

class Stats;

#else	// XSTARCASTLE_STATS_H
#define	XSTARCASTLE_STATS_H

#include "tthing.h"
#include "text.h"

enum stats_states {
    STATE_NORMAL,
    STATE_SHOW_PLAYER_SCORE,
    STATE_PAUSED,
    STATE_OVER,
    STATE_HIGH_SCORES,
    STATE_ATTRACT,
    STATE_PRESS_START,
};

class Stats : public Tthing {
private:
    enum stats_states last_state;

    unsigned int high_score;

    unsigned int last_score;
    int last_lives;

    Text text_primary;
    Text text_secondary;
    Text text_tertiary;
    Text text_p1;
    Text text_p2;
    Text text_p1_score;
    Text text_p2_score;
    unsigned int score;
    unsigned int buzzers_destroyed;
    unsigned int ring_segments_destroyed;
    unsigned int ring_segments_hit;
    unsigned int castles_destroyed;
    unsigned int lasers_shot;
    unsigned int fireballs_launched;
    unsigned int rings_regenerated;
    unsigned int killed_by_buzzer;
    unsigned int killed_by_fireball;
    unsigned int buzzers_launched;

    char * get_file();
    void clear_file();
    void write_record();
public:
    Stats(void);
    ~Stats(void);

    void add_score(unsigned int);
    int lives;
    unsigned int level; //the number of times we've killed the king.
    enum stats_states state;

    void render(const bool);

    void reset(void);
    void resize(const int, const int);

    bool changed(void) const;
    void destroyed_a_buzzer();
    void destroyed_a_ring_segment();
    void destroyed_a_castle();
    void hit_a_ring_segment();
    void shot_a_laser();
    void launched_a_fireball();
    void regenerated_a_ring();
    void killed_by_a_buzzer();
    void killed_by_a_fireball();
    void launched_a_buzzer();
};

inline void
Stats::killed_by_a_buzzer()
{
  killed_by_buzzer++;
}

inline void
Stats::killed_by_a_fireball()
{
  killed_by_fireball++;
}

inline void
Stats::launched_a_buzzer()
{
  buzzers_launched++;
}

inline void
Stats::regenerated_a_ring()
{
  rings_regenerated++;
}

inline void
Stats::launched_a_fireball()
{
  fireballs_launched++;
}

inline void
Stats::shot_a_laser()
{
  lasers_shot++;
}

inline void
Stats::destroyed_a_buzzer()
{
  buzzers_destroyed++;
}

inline void
Stats::hit_a_ring_segment()
{
  ring_segments_hit++;
}

inline void
Stats::destroyed_a_ring_segment()
{
  ring_segments_destroyed++;
}

inline bool
Stats::changed(void) const
{
    return (score != last_score || lives != last_lives || state != last_state);
} // Stats::changed

#endif	// XSTARCASTLE_STATS_H
