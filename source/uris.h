/*
  This file taken from the LV2 Sampler Example Plugin
  Copyright 2011-2012 David Robillard <d@drobilla.net>

  Permission to use, copy, modify, and/or distribute this software for any
  purpose with or without fee is hereby granted, provided that the above
  copyright notice and this permission notice appear in all copies.

  THIS SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#ifndef CABSIM_URIS_H
#define CABSIM_URIS_H

#include "lv2/lv2plug.in/ns/ext/log/log.h"
#include "lv2/lv2plug.in/ns/ext/midi/midi.h"
#include "lv2/lv2plug.in/ns/ext/state/state.h"
#include "lv2/lv2plug.in/ns/ext/parameters/parameters.h"

#define CABSIM_URI          "http://moddevices.com/plugins/mod-devel/cabsim-IR-loader"
#define CABSIM__sample      CABSIM_URI "#sample"
#define CABSIM__applySample CABSIM_URI "#applySample"
#define CABSIM__freeSample  CABSIM_URI "#freeSample"

typedef struct {
	LV2_URID atom_Float;
	LV2_URID atom_Path;
	LV2_URID atom_Resource;
	LV2_URID atom_Sequence;
	LV2_URID atom_URID;
	LV2_URID atom_eventTransfer;
	LV2_URID eg_applySample;
	LV2_URID eg_sample;
	LV2_URID eg_freeSample;
	LV2_URID midi_Event;
	LV2_URID param_gain;
	LV2_URID patch_Get;
	LV2_URID patch_Set;
	LV2_URID patch_property;
	LV2_URID patch_value;
} CabsimURIs;

static inline void
map_cabsim_uris(LV2_URID_Map* map, CabsimURIs* uris)
{
	uris->atom_Float         = map->map(map->handle, LV2_ATOM__Float);
	uris->atom_Path          = map->map(map->handle, LV2_ATOM__Path);
	uris->atom_Resource      = map->map(map->handle, LV2_ATOM__Resource);
	uris->atom_Sequence      = map->map(map->handle, LV2_ATOM__Sequence);
	uris->atom_URID          = map->map(map->handle, LV2_ATOM__URID);
	uris->atom_eventTransfer = map->map(map->handle, LV2_ATOM__eventTransfer);
	uris->eg_applySample     = map->map(map->handle, CABSIM__applySample);
	uris->eg_freeSample      = map->map(map->handle, CABSIM__freeSample);
	uris->eg_sample          = map->map(map->handle, CABSIM__sample);
	uris->midi_Event         = map->map(map->handle, LV2_MIDI__MidiEvent);
	uris->param_gain         = map->map(map->handle, LV2_PARAMETERS__gain);
	uris->patch_Get          = map->map(map->handle, LV2_PATCH__Get);
	uris->patch_Set          = map->map(map->handle, LV2_PATCH__Set);
	uris->patch_property     = map->map(map->handle, LV2_PATCH__property);
	uris->patch_value        = map->map(map->handle, LV2_PATCH__value);
}

/**
 * Write a message like the following to @p forge:
 * []
 *     a patch:Set ;
 *     patch:property eg:sample ;
 *     patch:value </home/me/foo.wav> .
 */
static inline LV2_Atom*
write_set_file(LV2_Atom_Forge*    forge,
               const CabsimURIs* uris,
               const char*        filename,
               const uint32_t     filename_len)
{
	LV2_Atom_Forge_Frame frame;
	LV2_Atom* set = (LV2_Atom*)lv2_atom_forge_object(
		forge, &frame, 0, uris->patch_Set);

	lv2_atom_forge_key(forge, uris->patch_property);
	lv2_atom_forge_urid(forge, uris->eg_sample);
	lv2_atom_forge_key(forge, uris->patch_value);
	lv2_atom_forge_path(forge, filename, filename_len);

	lv2_atom_forge_pop(forge, &frame);

	return set;
}

static inline const LV2_Atom*
read_set_file(const CabsimURIs*     uris,
              const LV2_Atom_Object* obj)
{
	if (obj->body.otype != uris->patch_Set) {
		fprintf(stderr, "Ignoring unknown message type %d\n", obj->body.otype);
		return NULL;
	}

	/* Get property URI. */
	const LV2_Atom* property = NULL;
	lv2_atom_object_get(obj, uris->patch_property, &property, 0);
	if (!property) {
		fprintf(stderr, "Malformed set message has no body.\n");
		return NULL;
	} else if (property->type != uris->atom_URID) {
		fprintf(stderr, "Malformed set message has non-URID property.\n");
		return NULL;
	} else if (((const LV2_Atom_URID*)property)->body != uris->eg_sample) {
		fprintf(stderr, "Set message for unknown property.\n");
		return NULL;
	}

	/* Get value. */
	const LV2_Atom* file_path = NULL;
	lv2_atom_object_get(obj, uris->patch_value, &file_path, 0);
	if (!file_path) {
		fprintf(stderr, "Malformed set message has no value.\n");
		return NULL;
	} else if (file_path->type != uris->atom_Path) {
		fprintf(stderr, "Set message value is not a Path.\n");
		return NULL;
	}

	return file_path;
}

#endif  /* CABSIM_URIS_H */
