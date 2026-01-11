#include <stdio.h>
#include <assert.h>
#include "../kernel/qcore_metriplectic.h"

int main() {
    SystemState state;
    init_system(&state);

    printf("[TEST] Initializing Thermal Dynamics Test...\n");
    assert(state.temperature == 22.0f);
    assert(state.entropy_rate == 0.0f);

    printf("[TEST] Running 1000 steps to simulate heating...\n");
    // Force some heating by giving the launder a fake high duty cycle or just running pulses
    float dt = 0.05f;
    for (int i = 0; i < 1000; i++) {
        solve_step(&state, dt);
        
        if (i % 200 == 0) {
            printf("Step %d: Temp = %.2f C, Entropy Rate = %.4f, Stability = %.2f\n", 
                   i, state.temperature, state.entropy_rate, state.stability);
        }
    }

    printf("[TEST] Verifying temperature rise...\n");
    assert(state.temperature > 22.0f);
    printf("PASS: Temperature rose to %.2f C\n", state.temperature);

    printf("[TEST] Verifying cooling behavior (No V_RMS)...\n");
    // Shutdown the launder target to force zero pulse
    state.launder.target_phi = 0.0f; 
    float high_temp = state.temperature;
    
    for (int i = 0; i < 1000; i++) {
        solve_step(&state, dt);
    }
    
    assert(state.temperature < high_temp);
    printf("PASS: Temperature decayed from %.2f C to %.2f C\n", high_temp, state.temperature);

    printf("ALL TESTS PASSED\n");
    return 0;
}
