//      xstarcastle
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
#include "xsc.h"
#include "sound.h"
#include <SDL.h>
#include <SDL_mixer.h>

Mix_Chunk * sounds[MAX_SOUNDS];
bool buzzer_sound_on = false;

const char * files[MAX_SOUNDS] = {
  "radar_fail.wav",
  "bombexplosion.wav",
  "flak_gun_sound.wav",
  "radar_ready.wav",
  "bodyslam.wav",
  "bodyslam.wav",
  "explosionBoss.wav",
  "tesla_tower.wav",
};

Mix_Music *buzzer;

void play(int snd)
{
  if (args.quiet)
    return;
  Mix_PlayChannel(-1, sounds[snd], 0);
}

void buzzer_sound(bool on)
{
  buzzer_sound_on = on;
  if (on)
    {
      if (Mix_PausedMusic())
        Mix_ResumeMusic();
      else
        Mix_FadeInMusic(buzzer, 1, 0);
    }
  else
    Mix_PauseMusic();
}

void repeat_music()
{
  Mix_FadeInMusic(buzzer, 1, 0);
}

void sound_init()
{
  if (args.quiet)
    return;
  SDL_Init(SDL_INIT_AUDIO);
  Mix_OpenAudio(44100, AUDIO_S16, 2, 1024); 
  for (int i = 0; i < MAX_SOUNDS; i++)
    {
      char *file = NULL;
      if (asprintf(&file, "%s/%s", XSTARCASTLE_DATADIR, files[i]) != -1)
        {
          sounds[i] = Mix_LoadWAV(file);
          free (file);
        }
    }
  char *file = NULL;
  if (asprintf(&file, "%s/%s", XSTARCASTLE_DATADIR, files[BUZZER_SOUND]) != -1)
    {
      buzzer = Mix_LoadMUS(file);
      Mix_HookMusicFinished(repeat_music);
      buzzer_sound_on = false;
    }
}

void sound_pause(bool pause)
{
  if (pause)
    Mix_PauseMusic();
  else
    Mix_ResumeMusic();
}

