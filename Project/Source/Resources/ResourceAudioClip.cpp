#include "ResourceAudioClip.h"

#include "Globals.h"
#include "Application.h"
#include "Modules/ModuleFiles.h"
#include "Utils/Buffer.h"

#include "Utils/MSTimer.h"
#include <inttypes.h>
#include "AL/alext.h"
#include <sndfile.h>
#include "Utils/Logging.h"

#include "rapidjson/error/en.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"

#define JSON_TAG_AUDIO "Audio"

void ResourceAudioClip::Load() {
	MSTimer timer;
	timer.Start();
	std::string filePath = GetResourceFilePath();
	LOG("Loading audio from path: \"%s\".", filePath);

	Buffer<char> audioBuffer = App->files->Load(filePath.c_str());
	if (audioBuffer.Size() == 0) return;

	rapidjson::Document document;
	document.ParseInsitu<rapidjson::kParseNanAndInfFlag>(audioBuffer.Data());
	if (document.HasParseError()) {
		LOG("Error parsing JSON: %s (offset: %u)", rapidjson::GetParseError_En(document.GetParseError()), document.GetErrorOffset());
		return;
	}

	// Load cubemap
	JsonValue jFile(document, document);
	JsonValue jAudio = jFile[JSON_TAG_AUDIO];

	ALenum err, format;
	SNDFILE* sndfile;
	SF_INFO sfinfo;
	short* membuf;
	sf_count_t num_frames;
	ALsizei num_bytes;

	/* Open the audio file and check that it's usable. */
	filePath = jAudio[0];
	sndfile = sf_open(filePath.c_str(), SFM_READ, &sfinfo);
	if (!sndfile) {
		LOG("Could not open audio in %s: %s\n", filePath, sf_strerror(sndfile));
		/*fprintf(stderr, "Could not open audio in %s: %s\n", filePath, sf_strerror(sndfile));*/
		return;
	}
	if (sfinfo.frames < 1 || sfinfo.frames > (sf_count_t)(INT_MAX / sizeof(short)) / sfinfo.channels) {
		LOG("Bad sample count in %s (%" PRId64 ")\n", filePath, sfinfo.frames);
		/*fprintf(stderr, "Bad sample count in %s (%" PRId64 ")\n", filePath, sfinfo.frames);*/
		sf_close(sndfile);
		return;
	}

	/* Get the sound format, and figure out the OpenAL format */
	format = AL_NONE;
	if (sfinfo.channels == 1)
		format = AL_FORMAT_MONO16;
	else if (sfinfo.channels == 2)
		format = AL_FORMAT_STEREO16;
	else if (sfinfo.channels == 3) {
		if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
			format = AL_FORMAT_BFORMAT2D_16;
	} else if (sfinfo.channels == 4) {
		if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
			format = AL_FORMAT_BFORMAT3D_16;
	}
	if (!format) {
		LOG("Unsupported channel count: %d\n", sfinfo.channels);
		/*fprintf(stderr, "Unsupported channel count: %d\n", sfinfo.channels);*/
		sf_close(sndfile);
		return;
	}

	/* Decode the whole audio file to a buffer. */
	membuf = static_cast<short*>(malloc((size_t)(sfinfo.frames * sfinfo.channels) * sizeof(short)));

	num_frames = sf_readf_short(sndfile, membuf, sfinfo.frames);
	if (num_frames < 1) {
		free(membuf);
		sf_close(sndfile);
		LOG("Failed to read samples in %s (%" PRId64 ")\n", filePath, num_frames);
		return;
	}
	num_bytes = (ALsizei)(num_frames * sfinfo.channels) * (ALsizei) sizeof(short);

	/* Buffer the audio data into a new buffer object, then free the data and
	 * close the file.
	 */
	buffer = 0;
	alGenBuffers(1, &buffer);
	alBufferData(buffer, format, membuf, num_bytes, sfinfo.samplerate);

	free(membuf);
	sf_close(sndfile);

	/* Check if an error occured, and clean up if so. */
	err = alGetError();
	if (err != AL_NO_ERROR) {
		fprintf(stderr, "OpenAL Error: %s\n", alGetString(err));
		if (buffer && alIsBuffer(buffer))
			alDeleteBuffers(1, &buffer);
		return;
	}
}

void ResourceAudioClip::Unload() {
}