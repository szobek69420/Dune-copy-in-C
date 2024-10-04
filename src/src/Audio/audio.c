#include "audio.h"

#include <stdio.h>
#include <stdlib.h>
#include <miniaudio.h>
#include <seqtor.h>

//---------------------------------------

struct sound {
	ma_sound* data;
	sound_id_t id;
};


static sound_id_t currentId = 1;

static ma_engine* engine = NULL;
static int currentEngineState = AUDIO_NONE;

//sound initializers
struct SoundInfo {
	ma_sound* sound;
	char* pathToSound;
};
typedef struct SoundInfo SoundInfo;

static seqtor_of(SoundInfo) LOADED_SOUNDS;

//active sounds
static sound activeSounds[AUDIO_MAX_COUNT_OF_SIMULTANEOUS_SOUNDS];

void audio_unloadSounds(int);

//returns 0 if problemlos
int audio_init(int engineState)
{
	if (engine != NULL)
		return;

	seqtor_init(LOADED_SOUNDS, 1);

	ma_result result;

	engine = malloc(sizeof(ma_engine));

	result = ma_engine_init(NULL, engine);
	if (result != MA_SUCCESS) {
		printf("Failed to initialize audio engine.");
		free(engine);
		engine = NULL;
		return -1;
	}

	currentEngineState = engineState;

	for (unsigned int i = 0; i < AUDIO_MAX_COUNT_OF_SIMULTANEOUS_SOUNDS; i++)
	{
		activeSounds[i].data = NULL;
		activeSounds[i].id = 0;//0 means unused
	}

	return 0;
}

void audio_destroy()
{
	if (engine == NULL)
		return;

	for (unsigned int i = 0; i < AUDIO_MAX_COUNT_OF_SIMULTANEOUS_SOUNDS; i++)
	{
		if (activeSounds[i].id == 0)
			continue;

		ma_sound_uninit(activeSounds[i].data);
		free(activeSounds[i].data);
		activeSounds[i].id = 0;//0 means unused
	}

	audio_unloadSounds(currentEngineState);
	ma_engine_uninit(engine);
	free(engine);
	engine = NULL;
	currentEngineState = 0;

	seqtor_destroy(LOADED_SOUNDS);
}

void audio_cleanupUnused()
{
	if (engine == NULL)
		return;

	for (int i = 0; i < AUDIO_MAX_COUNT_OF_SIMULTANEOUS_SOUNDS; i++)
	{
		if (activeSounds[i].id != 0 && ma_sound_at_end(activeSounds[i].data) != 0)
		{
			ma_sound_uninit(activeSounds[i].data);
			free(activeSounds[i].data);
			activeSounds[i].id = 0;
		}
	}
}

sound_id_t audio_playSound(const char* pathToFile)
{
	if (engine == NULL)
	{
		return 0;
	}

	sound s;
	s.data = malloc(sizeof(ma_sound));

	ma_result result = MA_SUCCESS;
	result = ma_sound_init_from_file(engine, pathToFile, 0, NULL, NULL, s.data);

	if (result != MA_SUCCESS)
		return 0;

	//actually play the sound
	int index = -1;
	for (int i = 0; i < AUDIO_MAX_COUNT_OF_SIMULTANEOUS_SOUNDS; i++)//search for unused sounds
	{
		if (activeSounds[i].id == 0)
		{
			index = i;
			break;
		}
	}

	for (int i = 0; i < AUDIO_MAX_COUNT_OF_SIMULTANEOUS_SOUNDS; i++)//search for not freed but finished sounds
	{
		if (activeSounds[i].id != 0 && ma_sound_at_end(activeSounds[i].data) != 0)
		{
			ma_sound_uninit(activeSounds[i].data);
			free(activeSounds[i].data);
			index = i;
			break;
		}
	}

	if (index == -1)//no place for a new sound
	{
		free(s.data);
		return 0;
	}

	result = ma_sound_start(s.data);
	if (result != MA_SUCCESS)
	{
		free(s.data);
		return 0;
	}

	//add sound to the activeSounds
	s.id = currentId++;
	activeSounds[index] = s;

	//add to the registry, if it has not been loaded yet
	int index = -1;
	for (int i = 0; i < seqtor_size(LOADED_SOUNDS); i++)
	{
		if (strcmp(pathToFile, seqtor_at(LOADED_SOUNDS, i).pathToSound) == 0)
		{
			index = i;
			break;
		}
	}

	if (index == -1)
	{
		ma_sound* globus = malloc(sizeof(ma_sound));
		memcpy(globus, s.data, sizeof(ma_sound));
		
		SoundInfo si;
		si.pathToSound = malloc((strlen(pathToFile) + 1) * sizeof(char));
		strcpy(si.pathToSound, pathToFile);
		si.sound = globus;

		seqtor_push_back(LOADED_SOUNDS, si);
	}

	return s.id;
}

int audio_soundAtEnd(sound_id_t soundId)
{
	if (engine == NULL || soundId == 0)
		return 69;


	int index = -1;
	for (int i = 0; i < AUDIO_MAX_COUNT_OF_SIMULTANEOUS_SOUNDS; i++)
	{
		if (activeSounds[i].id == soundId)
		{
			index = i;
			break;
		}
	}

	if (index == -1 || ma_sound_at_end(activeSounds[index].data) != 0)
	{
		return 69;
	}

	return 0;
}

void audio_stopSound(sound_id_t soundId)
{
	if (engine == NULL || soundId == 0)
		return;


	int index = -1;
	for (int i = 0; i < AUDIO_MAX_COUNT_OF_SIMULTANEOUS_SOUNDS; i++)
	{
		if (activeSounds[i].id == soundId)
		{
			index = i;
			break;
		}
	}

	if (index != -1)
	{
		ma_sound_stop(activeSounds[index].data);
		free(activeSounds[index].data);
		activeSounds[index].data = NULL;
		activeSounds[index].id = 0;
	}
}

void audio_unloadSounds(int currentState)
{
	if (engine == NULL)
		return;


	for (int i = 0; i < seqtor_size(LOADED_SOUNDS); i++)
	{
		ma_sound_uninit(seqtor_at(LOADED_SOUNDS, i).sound);
		free(seqtor_at(LOADED_SOUNDS, i).pathToSound);
		free(seqtor_at(LOADED_SOUNDS, i).sound);
	}

	seqtor_clear(LOADED_SOUNDS);
}