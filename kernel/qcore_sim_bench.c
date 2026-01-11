#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/keysym.h>
#include "qcore_metriplectic.h"
#include "hal_audio_host.h"

#define WIDTH 1024
#define HEIGHT 600

void draw_ui(Display *display, Window window, GC gc, SystemState *state) {
    XClearWindow(display, window);

    // Header
    XSetForeground(display, gc, 0xFFFFFF);
    XDrawString(display, window, gc, WIDTH/2 - 100, 30, "PROTOCOL ALPHA: TAYLOR-COUETTE BENCHMARK", 40);
    XDrawLine(display, window, gc, WIDTH/2, 50, WIDTH/2, HEIGHT - 50);

    // Labels
    XSetForeground(display, gc, 0x94a3b8);
    XDrawString(display, window, gc, 100, 60, "CLASSICAL NAVIER-STOKES (BASELINE)", 34);
    XDrawString(display, window, gc, WIDTH/2 + 100, 60, "SENTIENT METRIPLECTIC ENGINE", 28);

    char buf[128];
    // Stats HUD (Center)
    XSetForeground(display, gc, 0xFFFFFF);
    sprintf(buf, "L2 ERROR: %.6f", state->l2_error);
    XDrawString(display, window, gc, WIDTH/2 - 60, 440, buf, strlen(buf));
    
    sprintf(buf, "EFFICIENCY: %.2f x", state->thermal_eff / 50.0f);
    XSetForeground(display, gc, (state->thermal_eff > 100.0) ? 0x10b981 : 0xfacc15);
    XDrawString(display, window, gc, WIDTH/2 - 60, 460, buf, strlen(buf));
    
    sprintf(buf, "TEMP: %.2f C", state->temperature);
    XSetForeground(display, gc, (state->temperature > 50.0) ? 0xef4444 : 0x22d3ee);
    XDrawString(display, window, gc, WIDTH/2 - 60, 480, buf, strlen(buf));

    sprintf(buf, "LASALLE V: %.4f [dV: %.4f]", state->lyapunov_v, state->lyapunov_dot);
    XSetForeground(display, gc, (state->lyapunov_dot <= 0.001f) ? 0x10b981 : 0xfbbf24);
    XDrawString(display, window, gc, WIDTH/2 - 60, 500, buf, strlen(buf));
    
    // Acoustic HUD
    sprintf(buf, "ACOUSTIC LOAD: %.2f", state->audio_energy);
    XSetForeground(display, gc, 0xa78bfa);
    XDrawString(display, window, gc, WIDTH/2 - 60, 540, buf, strlen(buf));
    
    int bar_w = (int)(state->audio_energy * 200.0f);
    XFillRectangle(display, window, gc, WIDTH/2 - 60, 550, bar_w, 10);

    if (state->audio_coherence > 0.5f) {
        XSetForeground(display, gc, 0x10b981);
        XDrawString(display, window, gc, WIDTH/2 - 60, 570, "RECOGNITION: RESONANT LOCK", 27);
    }
    
    if (state->is_lasalle_locked) {
        XSetForeground(display, gc, 0x10b981);
        XDrawString(display, window, gc, WIDTH/2 - 60, 590, "BARBASHIN-LASALLE: LOCKED INVARIANT", 35);
    }

    // 1. Classical Side (Static/Analytical)
    int left_cx = WIDTH / 4;
    int right_cx = 3 * WIDTH / 4;
    int cy = HEIGHT / 2;

    XSetForeground(display, gc, 0x475569);
    for (int i = 0; i < 15; i++) {
        int y = cy - 100 + i * 14;
        XDrawLine(display, window, gc, left_cx - 50, y, left_cx + 50, y); // Laminar flow lines
    }

    // 2. Metriplectic Side (Dynamic)
    XSetForeground(display, gc, (state->stability > 90.0) ? 0x22d3ee : 0x3b82f6);
    for (int i = 0; i < TORUS_DIM; i++) {
        for (int j = 0; j < TORUS_DIM; j++) {
            float intensity = state->phi_re[i][j] * state->phi_re[i][j] + state->phi_im[i][j] * state->phi_im[i][j];
            if (intensity > 0.1 && state->solenoid_filter > 0.7f) {
                float angle = (float)i * (2.0f * PI / TORUS_DIM) + state->global_identity;
                float radius = 80.0f + (float)j * 10.0f;
                int px = right_cx + (int)(k_cos(angle) * radius);
                int py = cy + (int)(k_sin(angle) * radius);
                XFillArc(display, window, gc, px-2, py-2, 4, 4, 0, 360*64);
            }
        }
    }

    // Solenoid Strobe Visual on Right
    if (state->launder.last_v > 0.1f) {
        XSetForeground(display, gc, 0xfacc15);
        XDrawArc(display, window, gc, right_cx - 150, cy - 150, 300, 300, 0, 360*64);
    }

    XFlush(display);
}

int main() {
    Display *display;
    Window window;
    XEvent event;
    int screen;

    display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "Cannot open display\n");
        return 1;
    }

    screen = DefaultScreen(display);
    window = XCreateSimpleWindow(display, RootWindow(display, screen), 10, 10, WIDTH, HEIGHT, 1,
                                 WhitePixel(display, screen), BlackPixel(display, screen));
    XSelectInput(display, window, ExposureMask | KeyPressMask);
    XMapWindow(display, window);
    GC gc = XCreateGC(display, window, 0, NULL);

    SystemState state;
    init_system(&state);
    hal_audio_init();

    while (1) {
        while (XPending(display)) {
            XNextEvent(display, &event);
            if (event.type == KeyPress) {
                if (XLookupKeysym(&event.xkey, 0) == XK_Escape) goto cleanup;
            }
        }
        hal_audio_poll(&state);
        solve_step(&state, 0.016);
        draw_ui(display, window, gc, &state);
        usleep(16000);
    }

cleanup:
    hal_audio_cleanup();
    XCloseDisplay(display);
    return 0;
}
