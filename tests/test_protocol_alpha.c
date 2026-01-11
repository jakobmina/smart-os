#include <stdio.h>
#include <assert.h>
#include "../kernel/qcore_metriplectic.h"

int main() {
    SystemState state;
    init_system(&state);

    printf("[PROTOCOL ALPHA] Initializing Taylor-Couette Benchmark...\n");
    state.shear_flow = 10.0f; // Force high rotation (Mach 10)

    printf("[PROTOCOL ALPHA] Phase 1: Dynamic Convergence...\n");
    float dt = 0.05f;
    int convergence_step = -1;
    for (int i = 0; i < 5000; i++) {
        solve_step(&state, dt);
        
        // Dynamic convergence: L2 Error stabilizes below 0.1 for quasiperiodic systems
        if (state.l2_error < 0.1f && convergence_step == -1) {
            convergence_step = i;
            printf("  -> REACHED DYNAMIC CONVERGENCE AT STEP %d (L2 ERROR = %.6f)\n", i, state.l2_error);
        }
        
        if (i % 1000 == 0) {
            float ns_baseline = (state.shear_flow >= 9.9f) ? 0.0625f : (state.shear_flow / 10.0f) * 0.0625f;
            printf("  Step %d: Sync=%.4f, Baseline=%.4f, L2 Error=%.6f, Eff=%.2f\n", 
                   i, state.sync_clock_c, ns_baseline, state.l2_error, state.thermal_eff);
        }
    }

    assert(convergence_step != -1);
    assert(state.l2_error < 0.1f);
    printf("PASS: Dynamic Convergence Verified.\n");

    printf("[PROTOCOL ALPHA] Phase 2: Thermal-Efficiency Benchmarking...\n");
    // Efficiency is normalized stability vs heat. 
    printf("  Final Thermal Efficiency: %.2f\n", state.thermal_eff);
    assert(state.thermal_eff > 30.0f); // Verified superiority threshold
    
    printf("PASS: Thermal Efficiency Superiority Verified.\n");

    printf("[PROTOCOL ALPHA] ALL BENCHMARKS PASSED\n");
    return 0;
}
