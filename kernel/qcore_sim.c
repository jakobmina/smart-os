#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/keysym.h>
#include "qcore_metriplectic.h"
#include "hal_audio_host.h"

#define WIDTH 800
#define HEIGHT 600

void draw_ui(Display *display, Window window, GC gc, SystemState *state) {
    XClearWindow(display, window);

    // Header Decal
    XSetForeground(display, gc, 0x3b82f6); // Blue
    XDrawString(display, window, gc, 20, 30, "QUOREMIND SIM // INTEGRATED TOROIDAL-SHEAR MODEL v3.0", 54);
    
    char buf[128];
    sprintf(buf, "STABILITY: %.2f %% // SHEAR_FLOW: %.2f", state->stability, state->shear_flow);
    XDrawString(display, window, gc, 20, 50, buf, strlen(buf));

    sprintf(buf, "Bitstream: %.2f bits/s", state->bit_stream);
    XDrawString(display, window, gc, 20, 70, buf, strlen(buf));
    
    // Audio Diagnostics
    sprintf(buf, "Acoustic Load: %.2f", state->audio_energy);
    XSetForeground(display, gc, 0xa78bfa);
    XDrawString(display, window, gc, 20, 90, buf, strlen(buf));
    if (state->audio_coherence > 0.5f) {
        XSetForeground(display, gc, 0x10b981);
        XDrawString(display, window, gc, 20, 105, "RECOGNITION: LOCKED", 19);
    }
    
    sprintf(buf, "CLOCK_C: %.4f // GLOBAL_ID: %.2f rad", state->sync_clock_c, state->global_identity);
    XSetForeground(display, gc, (state->sync_clock_c > 0.5) ? 0x10b981 : 0x60a5fa);
    XDrawString(display, window, gc, 20, 125, buf, strlen(buf));

    sprintf(buf, "BUS_THROUGHPUT: %.2f // BREATHING: %.2f", state->bus.bus_throughput, state->breathing_state);
    XDrawString(display, window, gc, 20, 145, buf, strlen(buf));

    sprintf(buf, "LAUNDER V: %.1f [Duty: %.2f] // FILTER: %.2f", state->launder.last_v, state->launder.duty_cycle, state->solenoid_filter);
    XSetForeground(display, gc, (state->launder.last_v > 0.1) ? 0xfacc15 : 0x475569);
    XDrawString(display, window, gc, 20, 165, buf, strlen(buf));

    sprintf(buf, "TEMPERATURE: %.2f C // ENTROPY_RATE: %.4f", state->temperature, state->entropy_rate);
    XSetForeground(display, gc, (state->temperature > 50.0) ? 0xef4444 : 0x22d3ee);
    XDrawString(display, window, gc, 20, 185, buf, strlen(buf));

    int cx = WIDTH / 2;
    int cy = HEIGHT / 2;

    // 1. Draw the Sheared Channel (Z-Pinch)
    // Intensity modulated by solenoid filter
    XSetForeground(display, gc, (state->shear_flow > 9.0) ? 0xFFFFFF : 0x94a3b8);
    for (int i = 0; i < 20; i++) {
        float y_off = (float)i * 15.0f - 150.0f;
        float amp = (100.0f - state->stability) * 0.5f;
        int x_off = (int)(k_sin(state->time * 3.0f + (float)i * 0.5f) * amp);
        
        if (state->solenoid_filter > 0.7f) {
            XFillRectangle(display, window, gc, cx + x_off - 10, cy + (int)y_off, 20, 10);
        }
        
        // Photonic Packets (Only if unfiltered)
        if (state->breathing_state > 0.5f && (int)(state->time * 10) % 20 == i && state->solenoid_filter > 0.8f) {
            XSetForeground(display, gc, 0xfacc15); // Yellow
            XFillArc(display, window, gc, cx + x_off - 5, cy + (int)y_off, 10, 10, 0, 360*64);
            XSetForeground(display, gc, (state->shear_flow > 9.0) ? 0xFFFFFF : 0x94a3b8);
        }
    }

    // 2. Draw Toroidal Modulation (Resonance)
    XSetForeground(display, gc, (state->sync_clock_c > 0.5) ? 0x22d3ee : 0x3b82f6);
    for (int i = 0; i < TORUS_DIM; i++) {
        for (int j = 0; j < TORUS_DIM; j++) {
                float intensity = state->phi_re[i][j] * state->phi_re[i][j] + state->phi_im[i][j] * state->phi_im[i][j];
                if (intensity > 0.1 && state->solenoid_filter > 0.7f) {
                    float angle = (float)i * (2.0f * PI / TORUS_DIM) + state->global_identity;
                    float radius = 100.0f + (float)j * 15.0f + intensity * 10.0f;
                
                int px = cx + (int)(k_cos(angle) * radius);
                // Apply Nodal Bobbing to Y
                int py = cy + (int)(k_sin(angle) * radius + state->vortex_z * 15.0f);
                
                int size = (state->breathing_state > 0.5f) ? 6 : 3;
                if (state->solenoid_filter < 0.85f && size == 6) size = 3;
                
                XFillArc(display, window, gc, px - size/2, py - size/2, size, size, 0, 360*64);
            }
        }
    }

    // 3. Core Sync Meters
    for(int i=0; i<4; i++) {
        XSetForeground(display, gc, 0x475569);
        XDrawRectangle(display, window, gc, 650, 450 + i*30, 100, 20);
        int fill = (int)(state->bus.core_sync[i] * 100);
        XSetForeground(display, gc, (fill > 80) ? 0x10b981 : 0x3b82f6);
        XFillRectangle(display, window, gc, 650, 450 + i*30, fill, 20);
        sprintf(buf, "C%d", i);
        XDrawString(display, window, gc, 630, 465 + i*30, buf, 2);
    }

    XFlush(display);
}

int main() {
    Display *display;
    Window window;
    XEvent event;
    int screen;

    display = getenv("DISPLAY") ? XOpenDisplay(NULL) : NULL;
    if (display == NULL) {
        fprintf(stderr, "No DISPLAY detected. Running in HEADLESS mode for physics verification.\n");
        SystemState state;
        init_system(&state);
        hal_audio_init(); // Initialize audio even in headless mode for consistency
        for(int i=0; i<5; i++) {
            hal_audio_poll(&state); // Poll audio in headless mode
            solve_step(&state, 0.1);
            printf("[PHYSICS_TRACE] Step %d: Stability=%.2f, Flow=%.2f, Kink=%.2f\n", i, state.stability, state.shear_flow, state.kink_amplitude);
            fflush(stdout);
        }
        hal_audio_cleanup(); // Cleanup audio in headless mode
        return 0;
    }

    screen = DefaultScreen(display);
    window = XCreateSimpleWindow(display, RootWindow(display, screen), 10, 10, WIDTH, HEIGHT, 1,
                                 WhitePixel(display, screen), BlackPixel(display, screen));
    XSelectInput(display, window, ExposureMask | KeyPressMask);
    XMapWindow(display, window);

    GC gc = XCreateGC(display, window, 0, NULL);
    XSetForeground(display, gc, WhitePixel(display, screen));

    SystemState state;
    init_system(&state);
    hal_audio_init();

    while (1) {
        while (XPending(display)) {
            XNextEvent(display, &event);
            if (event.type == KeyPress) {
                KeySym key = XLookupKeysym(&event.xkey, 0);
                if (key == XK_Escape) goto cleanup;
                if (key == XK_Up) state.shear_flow = (state.shear_flow < 10.0) ? state.shear_flow + 0.1 : 10.0;
                if (key == XK_Down) state.shear_flow = (state.shear_flow > 0.0) ? state.shear_flow - 0.1 : 0.0;
            }
        }
        hal_audio_poll(&state);
        solve_step(&state, 0.016); // ~60fps logic
        draw_ui(display, window, gc, &state);
        usleep(16000);
    }

cleanup:
    hal_audio_cleanup();
    XCloseDisplay(display);
    return 0;
}
