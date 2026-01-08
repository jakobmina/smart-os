#include "qcore_metriplectic.h"

float k_mod_2pi(float x) {
    while (x > 2.0f * PI) x -= 2.0f * PI;
    while (x < 0.0f) x += 2.0f * PI;
    return x;
}

float k_sin(float x) {
    x = k_mod_2pi(x);
    if (x > PI) return -k_sin(x - PI);
    if (x > PI/2.0f) return k_cos(x - PI/2.0f);
    float x2 = x * x;
    return x * (1.0f - x2 * (1.0f/6.0f - x2 * (1.0f/120.0f - x2 * (1.0f/5040.0f))));
}

float k_cos(float x) {
    x = k_mod_2pi(x);
    if (x > PI) return -k_cos(x - PI);
    if (x > PI/2.0f) return -k_sin(x - PI/2.0f);
    float x2 = x * x;
    return 1.0f - x2 * (0.5f - x2 * (1.0f/24.0f - x2 * (1.0f/720.0f - x2 * (1.0f/40320.0f))));
}

float k_exp(float x) {
    // Basic e^x approximation
    return 1.0f + x + (x*x)/2.0f + (x*x*x)/6.0f;
}

float k_sqrt(float x) {
    if (x <= 0) return 0;
    float res = x;
    for(int i=0; i<4; i++) res = 0.5f * (res + x/res);
    return res;
}

float golden_operator(float n) {
    return k_cos(PI * n) * k_cos(PI * PHI * n);
}

void init_system(SystemState *state) {
    state->time = 0.0f;
    state->kink_amplitude = 10.0f;
    state->stability = 50.0f;
    state->shear_flow = 10.0f; // Default to Mach 10 "Canal Open"
    
    state->sync_clock_c = 0.0f;
    state->global_identity = 0.0f;
    state->gamma_strobe = 0.2f;
    state->breathing_state = 0.0f;
    
    state->node_density = 0.0f;
    state->bit_stream = 0.0f;
    state->causal_flux = 0.0f;
    state->golden_filter = PHI;

    // Initialize Bus
    for(int i=0; i<4; i++) state->bus.core_sync[i] = 0.0f;
    state->bus.bus_throughput = 0.0f;
    state->bus.packet_loss = 0.0f;

    // Initialize Φ on T^2 with a quasiperiodic wave
    for (int i = 0; i < TORUS_DIM; i++) {
        for (int j = 0; j < TORUS_DIM; j++) {
            float theta = (float)i * 2.0f * PI / TORUS_DIM;
            float phi = (float)j * 2.0f * PI / TORUS_DIM;
            state->phi_re[i][j] = k_cos(theta);
            state->phi_im[i][j] = k_sin(phi * PHI);
        }
    }
}

float compute_sync_clock(SystemState *state) {
    float sum = 0.0f;
    float On = golden_operator(state->time);
    
    for (int i = 0; i < TORUS_DIM; i++) {
        for (int j = 0; j < TORUS_DIM; j++) {
            float intensity = state->phi_re[i][j]*state->phi_re[i][j] + 
                             state->phi_im[i][j]*state->phi_im[i][j];
            sum += intensity * On;
        }
    }
    return sum / (float)(TORUS_DIM * TORUS_DIM);
}

void apply_breathing_projector(SystemState *state, float dt) {
    float On = golden_operator(state->time);
    float phase = PI * On;
    float e_re = k_cos(phase);
    float e_im = k_sin(phase);
    
    float factor_re = 1.0f - e_re;
    float factor_im = -e_im;
    
    // Breathing state is the "Strobe Light" laser pumping
    state->breathing_state = (k_cos(state->time * PI * 2.0f) > 0.0f) ? 1.0f : 0.0f;

    for (int i = 0; i < TORUS_DIM; i++) {
        for (int j = 0; j < TORUS_DIM; j++) {
            float phi_r = state->phi_re[i][j];
            float phi_i = state->phi_im[i][j];
            
            // Interaction only during Strobe ON phase
            float strobe_factor = state->gamma_strobe * state->breathing_state;
            
            float dphi_re = -strobe_factor * (phi_r * factor_re - phi_i * factor_im);
            float dphi_im = -strobe_factor * (phi_r * factor_im + phi_i * factor_re);
            
            state->phi_re[i][j] += dphi_re * dt;
            state->phi_im[i][j] += dphi_im * dt;
        }
    }
}

void solve_step(SystemState *state, float dt) {
    state->time += dt;

    // 1. Classical Canal (Shear Flow)
    float target_stability = (state->shear_flow >= 9.9f) ? 100.0f : (state->shear_flow * 8.0f);
    
    // 2. Toroidal Modulation
    apply_breathing_projector(state, dt);
    state->sync_clock_c = compute_sync_clock(state);
    
    if (state->sync_clock_c > 0.5f) {
        state->global_identity += state->sync_clock_c * dt * 0.1f;
    }

    // 3. Metriplectic Coupling
    // Stability is the product of Classical Shear and Toroidal Resonance
    float tor_boost = (state->sync_clock_c > 0.0f) ? state->sync_clock_c * 10.0f : 0.0f;
    float d_metr = (target_stability - state->stability) * 0.1f + tor_boost;
    state->stability += d_metr * dt;

    // 4. Information Pump (The Breathing Laser)
    // Causal flux is restored: it pulses with the breathing strobe
    if (state->breathing_state > 0.5f && state->stability > 90.0f) {
        state->causal_flux = state->sync_clock_c * 5.0f; // 5V signal pulse
        state->node_density += state->causal_flux * dt;
    } else {
        state->causal_flux *= 0.8f; // Decay
        state->node_density *= 0.99f;
    }
    
    // PHI filter resets with the parity dagger
    if (k_cos(PI * state->time * 2.0f) < -0.99f) {
        state->golden_filter = PHI;
    } else {
        state->golden_filter = (PHI * state->golden_filter + 1.0f) * 0.5f;
    }

    // 5. Inter-core Interaction
    for(int i=0; i<4; i++) {
        // Each core synchronizes based on the golden operator phase shift
        float core_phase = state->time + (float)i * (PI / 2.0f);
        float core_op = k_cos(PI * core_phase) * k_cos(PI * PHI * core_phase);
        state->bus.core_sync[i] = (state->stability / 100.0f) * (core_op * 0.5f + 0.5f);
    }
    
    // Bus throughput is maximized when core_sync is balanced and stability is high
    state->bus.bus_throughput = state->node_density * state->bus.core_sync[0];
    state->bus.packet_loss = (100.0f - state->stability) / 100.0f;

    if (state->stability < 0) state->stability = 0;
    if (state->stability > 100) state->stability = 100;
}
