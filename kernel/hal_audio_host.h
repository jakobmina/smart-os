#ifndef HAL_AUDIO_HOST_H
#define HAL_AUDIO_HOST_H

#include "qcore_metriplectic.h"

/**
 * @brief Initialize ALSA audio capture on the host.
 * @return 0 on success, -1 on failure.
 */
int hal_audio_init();

/**
 * @brief Poll audio device for latest samples and update system state.
 * @param state Pointer to the SystemState to update.
 */
void hal_audio_poll(SystemState *state);

/**
 * @brief Cleanup ALSA resources.
 */
void hal_audio_cleanup();

#endif // HAL_AUDIO_HOST_H
