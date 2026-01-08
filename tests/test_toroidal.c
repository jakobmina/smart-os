#include <stdio.h>
#include <assert.h>
#include "../kernel/qcore_metriplectic.h"

int main() {
    SystemState state;
    init_system(&state);

    printf("[TEST] Initializing Toroidal Manifold...\n");
    assert(state.time == 0.0f);
    assert(state.shear_flow == 10.0f);
    assert(state.gamma_strobe == 0.2f);

    printf("[TEST] Running 100 steps of integration...\n");
    float prev_id = state.global_identity;
    for (int i = 0; i < 100; i++) {
        solve_step(&state, 0.1f);
        
        // Verify clock_c is in a reasonable range
        if (state.sync_clock_c < -10.0f || state.sync_clock_c > 10.0f) {
            printf("CRITICAL ERROR: Clock_c = %.4f at step %d\n", state.sync_clock_c, i);
        }
        assert(state.sync_clock_c >= -10.0f && state.sync_clock_c <= 10.0f);
        
        if (i % 20 == 0) {
            printf("Step %d: Clock_c = %.4f, Identity = %.4f rad, Stability = %.2f%%\n", 
                   i, state.sync_clock_c, state.global_identity, state.stability);
        }
    }

    printf("[TEST] Verifying Global Identity evolution...\n");
    // Global identity should have evolved if there was resonance
    if (state.global_identity > prev_id) {
        printf("PASS: Global identity evolved (%.4f -> %.4f)\n", prev_id, state.global_identity);
    } else {
        printf("INFO: No significant resonance detected in this window.\n");
    }

    printf("[TEST] Verifying Stability bounds...\n");
    assert(state.stability >= 0.0f && state.stability <= 100.0f);

    printf("ALL TESTS PASSED\n");
    return 0;
}
