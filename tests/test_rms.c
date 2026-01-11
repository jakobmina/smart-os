#include <stdio.h>
#include <assert.h>
#include "../kernel/qcore_metriplectic.h"

int main() {
    SystemState state;
    init_system(&state);

    printf("[TEST] Initializing GoldenLaunder HAL Test...\n");
    assert(state.launder.target_phi == PHI);
    assert(state.launder.current_rms == 0.0f);

    printf("[TEST] Running 20000 steps to verify PI-Control convergence...\n");
    float dt = 0.05f;
    for (int i = 0; i < 20000; i++) {
        solve_step(&state, dt);
        
        if (i % 4000 == 0) {
            printf("Step %d: Launder V = %.1f, V_RMS = %.4f (Target: %.4f), Duty = %.4f\n", 
                   i, state.launder.last_v, state.launder.current_rms, PHI, state.launder.duty_cycle);
        }
    }

    float final_rms = state.launder.current_rms;
    printf("[TEST] Final V_RMS = %.4f\n", final_rms);

    // Delta check: Allow 1.5% error due to quasiperiodic variance
    float error = (final_rms > PHI) ? (final_rms - PHI) : (PHI - final_rms);
    float rel_error = error / PHI;

    if (rel_error < 0.015f) {
        printf("PASS: Final RMS is within 1.5%% of PHI (Stable HAL Lock).\n");
    } else {
        printf("FAIL: Final RMS error is %.2f%%\n", rel_error * 100.0f);
        return 1;
    }

    printf("ALL TESTS PASSED\n");
    return 0;
}
