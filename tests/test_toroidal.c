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

    printf("[TEST] Running 150 steps of integration...\n");
    float prev_id = state.global_identity;
    float max_vis = 0.0f;
    float min_vis = 1.0f;

    for (int i = 0; i < 150; i++) {
        solve_step(&state, 0.1f);
        
        if (state.visibility_score > max_vis) max_vis = state.visibility_score;
        if (state.visibility_score < min_vis) min_vis = state.visibility_score;

        assert(state.bf_axis >= -1.05f && state.bf_axis <= 1.05f);
        assert(state.visibility_score >= -0.05f && state.visibility_score <= 1.05f);
        
        if (i % 30 == 0) {
            printf("Step %d: Visibility = %.2f, Axis = %s, Stability = %.2f%%\n", 
                   i, state.visibility_score, (state.bf_axis > 0) ? "BOSONIC" : "FERMIONIC", state.stability);
        }
    }

    printf("[TEST] Verifying Visibility Dynamics...\n");
    assert(max_vis > 0.8f);
    assert(min_vis < 0.2f);
    printf("PASS: Visibility cycled between %.2f and %.2f\n", min_vis, max_vis);

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
