//      xstarcastle
//      Copyright (C) 2013 Ben Asselstine
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
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

#include <X11/keysym.h>

#include "global.h"

#include "xsc.h"

#include "difficulty.h"

#include "castle.h"
#include "random.h"
//buzzers
double Difficulty::avg_time_for_buzzer_to_leave_a_ring = 2.0;
const double Difficulty::slowest_avg_time_for_buzzer_to_leave_a_ring = 2.0;
const double Difficulty::fastest_avg_time_for_buzzer_to_leave_a_ring = 0.1;

double Difficulty::speed_of_buzzers = slowest_speed_of_buzzers;
const double Difficulty::fastest_speed_of_buzzers = 2.0; //pixels per frame per second, i guess.
const double Difficulty::slowest_speed_of_buzzers = 10.0;

//king
double Difficulty::turning_speed_of_king = 4.0;
const double Difficulty::fastest_turning_speed_of_king = 10.0;
const double Difficulty::slowest_turning_speed_of_king = 4.0;

double Difficulty::avg_time_to_launch_new_buzzer_from_king = Difficulty::slowest_avg_time_to_launch_new_buzzer_from_king;
const double Difficulty::slowest_avg_time_to_launch_new_buzzer_from_king = 2;
const double Difficulty::fastest_avg_time_to_launch_new_buzzer_from_king = 0.5;

//castle
double Difficulty::ring_speed[MAX_RINGS] = { -45.0, 90.0, 90.0 };
const double Difficulty::fastest_ring_speed = 225.0;
const double Difficulty::slowest_ring_speed = 45.0;

double Difficulty::avg_time_to_expire_outside_buzzers = Difficulty::fastest_avg_time_to_expire_outside_buzzers;
const double Difficulty::slowest_avg_time_to_expire_outside_buzzers = 60.0;
const double Difficulty::fastest_avg_time_to_expire_outside_buzzers = 13.0;

//i don't know what these units of measurements are.
double Difficulty::buzzer_turning_radius = Difficulty::largest_buzzer_turning_radius;
const double Difficulty::largest_buzzer_turning_radius = 70;
const double Difficulty::smallest_buzzer_turning_radius = 180;

void
Difficulty::init()
{
  speed_of_buzzers = slowest_speed_of_buzzers;
  avg_time_for_buzzer_to_leave_a_ring = 
    slowest_avg_time_for_buzzer_to_leave_a_ring;
  turning_speed_of_king = slowest_turning_speed_of_king;
  avg_time_to_launch_new_buzzer_from_king =
   slowest_avg_time_to_launch_new_buzzer_from_king; 
  ring_speed[0] = -slowest_ring_speed;
  ring_speed[1] = slowest_ring_speed*2;
  ring_speed[2] = slowest_ring_speed*2;
}

void 
Difficulty::increase(Stats *stats)
{
  double high, low, result, range;
  stats->level++;
  double level = stats->level / 10.0;
  if (level > 1.0)
    level = 1.0;
  high = slowest_avg_time_for_buzzer_to_leave_a_ring;
  low = fastest_avg_time_for_buzzer_to_leave_a_ring;
  range = high - low;
  result = high - (level * range);
  //printf("buzzer_time = %f -> %f\n", avg_time_for_buzzer_to_leave_a_ring, result);
  avg_time_for_buzzer_to_leave_a_ring = result;

  high = slowest_speed_of_buzzers;
  low = fastest_speed_of_buzzers;
  range = high - low;
  result = high - (level * range);
  //printf("buzzer speed %f -> %f\n", speed_of_buzzers, result);
  speed_of_buzzers = result;

  high = fastest_turning_speed_of_king;
  low = slowest_turning_speed_of_king;
  range = high - low;
  result = low + (level * range);
  //printf("turning speed of king %f -> %f\n", turning_speed_of_king, result);
  turning_speed_of_king = result;

  high = slowest_avg_time_to_launch_new_buzzer_from_king;
  low = fastest_avg_time_to_launch_new_buzzer_from_king;
  range = high - low;
  result = high - (level * range);
  //printf("launching time %f -> %f\n", avg_time_to_launch_new_buzzer_from_king, result);
  avg_time_to_launch_new_buzzer_from_king = result;

  high = fastest_ring_speed-45.0;
  low = slowest_ring_speed;
  range = high - low;
  result = low + (level * range);

  for (int i = 0; i < MAX_RINGS; i++)
    {
      ring_speed[i] = 45+ (Random::get() % int(result));
      if (Random::get() % 2 == 0)
        ring_speed[i] += 45.0;
      if (Random::get() % 2 == 0)
        ring_speed[i] *= -1;
    }
  int ccw = 0;
  for (int i = 0; i < MAX_RINGS; i++)
    {
      if (ring_speed[i] < 0)
        ccw++;
    }
  //make sure at least one ring is going in the other direction.
  if (ccw == MAX_RINGS || ccw == 0)
    ring_speed[Random::get() % MAX_RINGS] *= -1;

  high = slowest_avg_time_to_expire_outside_buzzers;
  low = fastest_avg_time_to_expire_outside_buzzers;
  range = high - low;
  result = low + (level * range);
  avg_time_to_expire_outside_buzzers = result;


  high = smallest_buzzer_turning_radius;
  low = largest_buzzer_turning_radius;
  range = high - low;
  result = high - (level * range);
  buzzer_turning_radius = result;

}
