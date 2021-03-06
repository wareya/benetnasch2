#ifndef INCLUDED_FAUXMIX_API
#define INCLUDED_FAUXMIX_API

#include <SDL2/SDL_audio.h>

#define DLLEXPORT extern "C"

#ifdef GAME_MAKER
typedef double TYPE_NM;
typedef double TYPE_ID;
typedef double TYPE_EC;
typedef double TYPE_VD;
typedef double TYPE_BL;
typedef double TYPE_FT;
typedef char * TYPE_ST;
#else
typedef Sint32 TYPE_NM;
typedef Uint32 TYPE_ID;
typedef Sint32 TYPE_EC;
typedef void TYPE_VD;
typedef bool TYPE_BL;
typedef float TYPE_FT;
typedef const char * TYPE_ST;
#endif

DLLEXPORT TYPE_VD fauxmix_dll_init();

DLLEXPORT TYPE_VD fauxmix_use_float_output(TYPE_BL b);

DLLEXPORT TYPE_BL fauxmix_init(TYPE_NM samplerate, TYPE_BL mono, TYPE_NM samples);
DLLEXPORT TYPE_NM fauxmix_get_samplerate();
DLLEXPORT TYPE_NM fauxmix_get_channels();
DLLEXPORT TYPE_NM fauxmix_get_samples();
DLLEXPORT TYPE_BL fauxmix_is_ducking();

DLLEXPORT TYPE_EC fauxmix_channel(TYPE_ID id, TYPE_FT volume);

/*
 * Samples have to be loaded on a thread or else client game logic would cause synchronous disk IO
 * However, this means that the "load sample" command can't give a definitive response on the validity of a sample file
 * It has to be checked in some other way
 * The idea I just had is:
 * Sample handles can be loaded, loading, unloaded, or failed
 * and you can poll their status
 * and it's up to you to kill them if they fail or unload
 */
DLLEXPORT TYPE_ID fauxmix_sample_load(TYPE_ST filename);
DLLEXPORT TYPE_EC fauxmix_sample_status(TYPE_ID sample);
DLLEXPORT TYPE_EC fauxmix_sample_volume(TYPE_ID sample, TYPE_FT volume);
DLLEXPORT TYPE_VD fauxmix_sample_kill(TYPE_ID sample);

DLLEXPORT TYPE_ID fauxmix_emitter_create(TYPE_ID sample);
DLLEXPORT TYPE_EC fauxmix_emitter_status(TYPE_ID mine);
DLLEXPORT TYPE_EC fauxmix_emitter_volumes(TYPE_ID mine, TYPE_FT left, TYPE_FT right);
DLLEXPORT TYPE_EC fauxmix_emitter_loop(TYPE_ID mine, TYPE_BL whether);
DLLEXPORT TYPE_EC fauxmix_emitter_pitch(TYPE_ID mine, TYPE_FT ratefactor);
DLLEXPORT TYPE_EC fauxmix_emitter_fire(TYPE_ID mine);
DLLEXPORT TYPE_EC fauxmix_emitter_cease(TYPE_ID mine);
DLLEXPORT TYPE_VD fauxmix_emitter_kill(TYPE_ID mine);

#endif
