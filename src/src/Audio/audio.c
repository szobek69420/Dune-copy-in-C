#define _CRT_SECURE_NO_WARNINGS

#include "audio.h"

#include <stdio.h>
#include <stdlib.h>
#include <miniaudio.h>
#include <seqtor.h>

//---------------------------------------

struct Sound {
	ma_sound* data;
	sound_id_t id;
};


static sound_id_t currentId = 1;

static ma_engine* engine = NULL;

//sound initializers
struct SoundInfo {
	ma_sound* data;
	char* path;
};
typedef struct SoundInfo SoundInfo;

static seqtor_of(SoundInfo) LOADED_SOUNDS;

//active sounds
static Sound activeSounds[AUDIO_MAX_COUNT_OF_SIMULTANEOUS_SOUNDS];

void audio_unloadSounds();

//returns 0 if problemlos
int audio_init()
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

	for (unsigned int i = 0; i < AUDIO_MAX_COUNT_OF_SIMULTANEOUS_SOUNDS; i++)
	{
		activeSounds[i].data = NULL;
		activeSounds[i].id = 0;//0 means unused
	}

	return 0;
}

void audio_deinit()
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

	audio_unloadSounds();
	ma_engine_uninit(engine);
	free(engine);
	engine = NULL;

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

	audio_preloadSound(pathToFile);//if it is loaded already, then nichts passiert

	Sound s;
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

void audio_preloadSound(const char* pathToFile)
{
	for (int i = 0; i < seqtor_size(LOADED_SOUNDS); i++)
	{
		if (strcmp(seqtor_at(LOADED_SOUNDS, i).path, pathToFile) == 0)
			return;
	}

	SoundInfo si;
	si.path = malloc((strlen(pathToFile) + 1) * sizeof(char));
	if (si.path == NULL)
	{
		printf("Audio: File could not be loaded\n");
		return;
	}
	strcpy(si.path, pathToFile);

	si.data = malloc(sizeof(ma_sound));
	if (si.data == NULL)
	{
		printf("Audio: File could not be loaded\n");
		free(si.path);
		return;
	}

	ma_result result = MA_SUCCESS;
	result = ma_sound_init_from_file(engine, pathToFile, 0, NULL, NULL, si.data);

	if (result != MA_SUCCESS)
	{
		printf("Audio: File could not be loaded\n");
		free(si.path);
		free(si.data);
		return;
	}

	seqtor_push_back(LOADED_SOUNDS, si);
}

void audio_unloadSounds()
{
	if (engine == NULL)
		return;


	for (int i = 0; i < seqtor_size(LOADED_SOUNDS); i++)
	{
		ma_sound_uninit(seqtor_at(LOADED_SOUNDS, i).data);
		free(seqtor_at(LOADED_SOUNDS, i).path);
		free(seqtor_at(LOADED_SOUNDS, i).data);
	}

	seqtor_clear(LOADED_SOUNDS);
}