#include "banner.h"
#include "vga_driver.h"

static const char *banner_lines[] = {
    "  @@@@@@ @@@@@@@@@@   @@@@@@  @@@@@@@   @@@@@@ @@@ @@@  @@@@@@",
    " !@@     @@! @@! @@! @@!  @@@ @@!  @@@ !@@     @@! !@@ !@@    ",
    "  !@@!!  @!! !!@ @!@ @!@  !@! @!@@!@!   !@@!!   !@!@!   !@@!! ",
    "     !:! !!:     !!: !!:  !!! !!:          !:!   !!:       !:!",
    " ::.: :   :      :    : :. :   :       ::.: :    .:    ::.: : "
};

void render_banner(const SystemState *state) {
    int start_x = 9; // Centered (80-62)/2
    int start_y = 1;
    
    // Calculate scanline position based on time
    int scanline_x = (int)(state->time * 40.0f) % 80;
    
    // Determine base colors based on stability
    uint8_t base_fg = CYAN;
    uint8_t alt_fg = LIGHT_BLUE;
    
    if (state->stability < 40.0f) {
        base_fg = RED;
        alt_fg = DARK_GRAY;
    } else if (state->stability < 70.0f) {
        base_fg = CYAN;
        alt_fg = MAGENTA;
    }

    for (int y = 0; y < 5; y++) {
        const char *line = banner_lines[y];
        for (int x = 0; line[x] != '\0'; x++) {
            int screen_x = start_x + x;
            int screen_y = start_y + y;
            
            char c = line[x];
            uint8_t fg = (x % 2 == 0) ? base_fg : alt_fg;
            
            // Hologram Scanline Effect
            if (screen_x == scanline_x || screen_x == scanline_x - 1 || screen_x == scanline_x + 1) {
                fg = WHITE;
            }
            
            // Glitch Effect for low stability
            if (state->stability < 50.0f) {
                // Use higher frequency time for flicker
                int flicker = (int)(state->time * 100.0f) % 10;
                if (flicker == y) {
                    fg = (state->stability < 30.0f) ? RED : YELLOW;
                    if (flicker % 2 == 0) c = (c == ' ') ? '.' : '?';
                }
            }

            if (c != ' ') {
                k_putc(screen_x, screen_y, c, fg, BLACK);
            }
        }
    }
}
