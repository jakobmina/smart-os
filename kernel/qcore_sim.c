#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "qcore_metriplectic.h"

#define WIDTH 800
#define HEIGHT 600

void draw_ui(Display *display, Window window, GC gc, SystemState *state) {
    XClearWindow(display, window);

    // Header Decal
    XSetForeground(display, gc, 0x3b82f6); // Blue
    XDrawString(display, window, gc, 20, 30, "QUOREMIND BAREMETAL v1.0 // METRIPLECTIC_KERNEL", 46);
    
    char buf[128];
    sprintf(buf, "STABILITY: %.2f %%", state->stability);
    XDrawString(display, window, gc, 20, 50, buf, strlen(buf));
    
    sprintf(buf, "MACH_FLOW: %.2f / 10.0", state->shear_flow);
    XSetForeground(display, gc, (state->shear_flow >= 9.9) ? 0x10b981 : 0x3b82f6);
    XDrawString(display, window, gc, 20, 70, buf, strlen(buf));

    // Draw Z-Pinch Column
    int cx = WIDTH / 2;
    int cy = HEIGHT / 2;
    int col_h = 300;
    int start_y = cy - 150;
    
    XSetForeground(display, gc, (state->shear_flow >= 9.9) ? 0xFFFFFF : 0x60a5fa);
    
    int prev_x = cx, prev_y = start_y;
    for (int i = 0; i < 100; i++) {
        double y_f = (double)i / 100.0;
        int y = start_y + (int)(y_f * col_h);
        
        double offset = sin(y_f * 5.0 + state->time * 2.0) * (100.0 - state->stability) * 0.5;
        int x = cx + (int)offset;
        
        XDrawLine(display, window, gc, prev_x, prev_y, x, y);
        prev_x = x;
        prev_y = y;
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
        for(int i=0; i<5; i++) {
            solve_step(&state, 0.1);
            printf("[PHYSICS_TRACE] Step %d: Stability=%.2f, Flow=%.2f, Kink=%.2f\n", i, state.stability, state.shear_flow, state.kink_amplitude);
            fflush(stdout);
        }
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

        solve_step(&state, 0.016); // ~60fps logic
        draw_ui(display, window, gc, &state);
        usleep(16000);
    }

cleanup:
    XCloseDisplay(display);
    return 0;
}
