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

#include "util.h"
#include "xsc.h"

#include "ship.h"	// ship data points

#include "stats.h"

const char *save_file = ".xstarcastle";

Stats::Stats(void)
{
    //fprintf(stderr, "Stats::Stats()\n");
    reset();
    last_score = ULONG_MAX;	// ensure we get displayed first time

    set_window(game_window);
    text_primary.set_window(game_window);
    text_secondary.set_window(game_window);
    text_p1_score.set_window(game_window);
    text_p2_score.set_window(game_window);
    text_p1.set_window(game_window);
    text_p2.set_window(game_window);
    text_tertiary.set_window(game_window);

    set_scale(8.0);
    text_primary.set_scale(scale);
    text_secondary.set_scale(scale);
    text_p1_score.set_scale(scale);
    text_p2_score.set_scale(scale);
    text_p1.set_scale(scale);
    text_p2.set_scale(scale);
    text_tertiary.set_scale(scale);

    set_gc(fetch_gc(GC_BRIGHT_GREY));
    text_primary.set_gc(fetch_gc(GC_DULL_BLUE));
    text_secondary.set_gc(fetch_gc(GC_DULL_BLUE));
    text_p1.set_gc(fetch_gc(GC_DULL_BLUE));
    text_p2.set_gc(fetch_gc(GC_DULL_BLUE));
    text_p1_score.set_gc(fetch_gc(GC_DULL_BLUE));
    text_p2_score.set_gc(fetch_gc(GC_DULL_BLUE));
    text_tertiary.set_gc(fetch_gc(GC_DULL_BLUE));

    set_theta(0.0);

    char *file = NULL;
    if (asprintf (&file, "%s/%s", getenv ("HOME"), save_file) != -1)
      {
        char *line = NULL;
        size_t n;
        FILE *fp = fopen (file, "r");
        if (fp)
          {
            if (getline (&line, &n, fp) != -1)
              sscanf (line, "%u", &high_score);
            free (line);
            fclose (fp);
          }
        free (file);
      }
} // Stats::Stats


Stats::~Stats(void)
{
    //fprintf(stderr, "Stats::~Stats()\n");
} // Stats::~Stats


char *
Stats::get_file()
{
  char *file = NULL;
  if (asprintf(&file, "%s/%s", getenv("HOME"), "xstarcastle-stats.csv") == -1)
    return NULL;
  return file;
}
void
Stats::clear_file()
{
  char *file = get_file();
  if (file)
    {
      FILE *fp = fopen (file, "w");
      if (fp)
        {
          fprintf (fp, "time, score, lives left, buzzers destroyed, ring segments destroyed, ring segments hit, castles destroyed, lasers shot, fireballs launched, rings regenerated, kills by buzzers, kills by fireballs, buzzers launched\n");
          fclose (fp);
        }
      free (file);
    }
}
void
Stats::write_record()
{
  char *file = get_file();
  if (file)
    {
      FILE *fp = fopen (file, "a");
      if (fp)
        {
          Stamp now;
          XSCTime::now(&now);

          fprintf (fp, 
                   "%lf, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\n",
                   double(now),
                   score, lives, buzzers_destroyed,
                   ring_segments_destroyed, ring_segments_hit, 
                   castles_destroyed, lasers_shot, fireballs_launched, 
                   rings_regenerated, killed_by_buzzer, killed_by_fireball, 
                   buzzers_launched);
          fclose (fp);
        }
      free (file);
    }
}
          

void
Stats::reset(void)
{
  if (args.stats)
    clear_file();
  score = 0;
  lives = 2;
  lasers_shot = 0;
  ring_segments_destroyed = 0;
  castles_destroyed = 0;
  ring_segments_hit = 0;
  buzzers_destroyed = 0;
  fireballs_launched = 0;
  rings_regenerated = 0;
  killed_by_buzzer = 0;
  killed_by_fireball = 0; 
  buzzers_launched = 0;
  state = STATE_NORMAL;
} // Stats::reset


void
Stats::render(const bool ink)
{
    //const double charwidth = text_primary.get_size() / 6.0;

    if (ink) {
      char buf[32];
      static char scorestr[20 + 1];	// 64 bits worth + terminator

	last_score = score;


	switch (state) {
	    case STATE_NORMAL:
		text_secondary.set_message("");
		text_primary.set_message("");
                text_p1_score.set_message("");
                text_p1.set_message("");
                text_p2_score.set_message("");
                text_p2.set_message("");
                text_tertiary.set_message("");
		break;
            case STATE_SHOW_PLAYER_SCORE:
                if (lives < last_lives) {
                  if (lives != 0) {
                    snprintf (buf, sizeof (buf), "SHIPS LEFT  %d", lives);
                    text_secondary.set_message(buf);
                  } else {
                    strcpy (buf, "LAST SHIP");
                  }
                  text_secondary.set_message(buf);
                }
                  sprintf(scorestr, "%u", score);
                  text_primary.set_message(scorestr);
                text_p1_score.set_message("");
                text_p1.set_message("");
                text_p2_score.set_message("");
                text_p2.set_message("");
                text_tertiary.set_message("");
                break;
	    case STATE_PAUSED:
		text_primary.set_message("PAUSED");
		break;
	    case STATE_OVER:
                if (args.stats)
                  write_record();
                if (score > high_score)
                  {
                    high_score = score;
                    char *file = NULL;
                    if (asprintf (&file, "%s/%s", getenv ("HOME"), 
                                  save_file) != -1)
                      {
                        FILE *fp = fopen (file, "w");
                        if (fp)
                          {
                            fprintf (fp, "%u\n", high_score);
                            fclose (fp);
                          }
                        free (file);
                      }
                  }
		text_primary.set_message("GAME OVER");
                text_p1_score.set_message("");
                text_p1.set_message("");
                text_p2_score.set_message("");
                text_p2.set_message("");
                text_tertiary.set_message("");
		break;
            case STATE_HIGH_SCORES:
                //okay we have some text that says HIGH SCORE
                //and a number beneath it.
                //the high score is centered top.
                text_primary.set_message("HIGH SCORE");
                snprintf (buf, sizeof (buf), "%d", high_score);
		text_secondary.set_message(buf);
                  sprintf(scorestr, "%u", score);
                text_p1.set_message("PLAYER ONE");
                text_p1_score.set_message(scorestr);
                text_p2.set_message("PLAYER TWO");
                text_p2_score.set_message("0");
                text_tertiary.set_message("@ 1980 CINEMATRONICS");
                break;
            case STATE_ATTRACT:
		text_primary.set_message("");
		text_secondary.set_message("");
                text_p1_score.set_message("");
                text_p1.set_message("");
                text_p2_score.set_message("");
                text_p2.set_message("");
                text_tertiary.set_message("@ 1980 CINEMATRONICS");
                 break;
            case STATE_PRESS_START:
		text_primary.set_message("PRESS");
		text_secondary.set_message("ONE PLAYER START");
                text_p1_score.set_message("");
                text_p1.set_message("");
                text_p2_score.set_message("");
                text_p2.set_message("");
                text_tertiary.set_message("CREDITS 1");
                 break;
	    default:
		break;
	}
	last_lives = lives;
	last_state = state;
    }

    // display the score

    y = 0.0 / 2.0;

    text_primary.set_position(-1.0, text_primary.get_size()*0.21);
    text_primary.render(ink);

    text_secondary.set_position(-1.0, (text_primary.get_size() * 0.21) + (text_secondary.get_size()/3.0));
    text_secondary.render(ink);

    text_tertiary.set_position(-1.0, gwheight-(text_tertiary.get_size()/5.0));
    text_tertiary.render(ink);

    // display the p1 and p2 score (during high score state)
    text_p1.set_position((wwidth2 + ((text_p1.get_size()/text_p1.get_scale())/ 2.0) - (text_p1.calculate_width() / 2.0))/4.0, (gwheight-(text_p1.get_size()*1)) / 2);
    text_p1.render(ink);
    text_p1_score.set_position(((wwidth2+ ((text_p1.get_size()/text_p1.get_scale())/ 2.0) - (text_p1.calculate_width() / 2.0))/4.0)+(text_p1.calculate_width()/2.0)-(text_p1_score.calculate_width()/2.0), ((gwheight-(text_p1_score.get_size()*1)) / 2) + (text_p1_score.get_size()/3.0));
    text_p1_score.render(ink);
    text_p2.set_position((wwidth2 + ((text_p2.get_size()/text_p2.get_scale())/ 2.0) - (text_p2.calculate_width() / 2.0))*1.75, (gwheight-(text_p2.get_size()*1)) / 2);
    text_p2.render(ink);
    text_p2_score.set_position(((wwidth2 + ((text_p2.get_size()/text_p2.get_scale())/ 2.0) - (text_p2.calculate_width() / 2.0))*1.75) + (text_p2.calculate_width()/2.0)- (text_p2_score.calculate_width()/2.0), ((gwheight-(text_p2.get_size()*1.0)) / 2) + (text_p2_score.get_size()/3.0));
    text_p2_score.render(ink);
   
    if (lives >= 0 || !ink) {

	GC save_gc = gc;
	set_gc(save_gc);
	set_scale(10.0);
	set_theta(0.0);
    }
} // Stats::render


void
Stats::resize(const int nwidth, const int nheight)
{
    Thing::resize(nwidth, nheight);
    text_primary.resize(nwidth, nheight);
    text_secondary.resize(nwidth, nheight);
    text_tertiary.resize(nwidth, nheight);
    text_p1_score.resize(nwidth, nheight);
    text_p1.resize(nwidth, nheight);
    text_p2_score.resize(nwidth, nheight);
    text_p2.resize(nwidth, nheight);
} // Stats::resize
    
void 
Stats::add_score(unsigned int s)
{
  score += s;
  score %= 1000000;
} // Stats::add_score

void
Stats::destroyed_a_castle()
{
  castles_destroyed++;
  if (args.stats)
    write_record();
}

