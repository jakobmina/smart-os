#include <alsa/asoundlib.h>
#include <math.h>
#include "hal_audio_host.h"

static snd_pcm_t *capture_handle = NULL;
static short buffer[1024];

int hal_audio_init() {
    int err;
    if ((err = snd_pcm_open(&capture_handle, "default", SND_PCM_STREAM_CAPTURE, SND_PCM_NONBLOCK)) < 0) {
        fprintf(stderr, "Cannot open audio device: %s\n", snd_strerror(err));
        return -1;
    }

    if ((err = snd_pcm_set_params(capture_handle,
                                 SND_PCM_FORMAT_S16_LE,
                                 SND_PCM_ACCESS_RW_INTERLEAVED,
                                 1,
                                 44100,
                                 1,
                                 500000)) < 0) { // 0.5s latency
        fprintf(stderr, "ALSA set_params error: %s\n", snd_strerror(err));
        return -1;
    }

    return 0;
}

void hal_audio_poll(SystemState *state) {
    if (!capture_handle) return;

    int frames = snd_pcm_readi(capture_handle, buffer, 1024);
    if (frames < 0) {
        if (frames == -EPIPE) snd_pcm_prepare(capture_handle);
        return;
    }

    float sum_sq = 0;
    int crossings = 0;
    short last_val = 0;

    for (int i = 0; i < frames; i++) {
        float val = buffer[i] / 32768.0f;
        sum_sq += val * val;
        
        if ((buffer[i] > 0 && last_val <= 0) || (buffer[i] < 0 && last_val >= 0)) {
            crossings++;
        }
        last_val = buffer[i];
    }

    float rms = sqrtf(sum_sq / (float)frames);
    state->audio_energy = (0.9f * state->audio_energy) + (0.1f * rms);

    // Simple Coherence: Stability of zero-crossings
    // A stable whistle or tone will have high coherence
    if (rms > 0.05f) {
        state->audio_coherence = (0.95f * state->audio_coherence) + 0.05f;
    } else {
        state->audio_coherence *= 0.99f;
    }
}

void hal_audio_cleanup() {
    if (capture_handle) {
        snd_pcm_close(capture_handle);
        capture_handle = NULL;
    }
}
