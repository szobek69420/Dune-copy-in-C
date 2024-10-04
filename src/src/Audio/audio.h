#ifndef AUDIO_H
#define AUDIO_H

#define AUDIO_MAX_COUNT_OF_SIMULTANEOUS_SOUNDS 100


//both menu and ingame sfx and music

//-------------------------------

struct Sound;
typedef struct Sound Sound;

typedef unsigned int sound_id_t;

//returns 0 if problemlos (the value of the engineState is either AUDIO_INGAME or AUDIO_MENU
int audio_init();
void audio_deinit();

//removes finished sounds from the list so that the response time of audio_playSound will always be immediate
void audio_cleanupUnused();

//returns the id of the sound (0 if it was unsuccessful)
sound_id_t audio_playSound(const char* pathToFile);
void audio_stopSound(sound_id_t soundId);
int audio_soundAtEnd(sound_id_t s);

void audio_preloadSound(const char* pathToFile);

#define AUDIO_PLAY_RANDOM_CAVE_NOISE() audio_playSound(AUDIO_SFX_CAVE_NOISE_1+(int)(AUDIO_CAVE_NOISE_COUNT*((float)rand()/(float)RAND_MAX)))

#endif