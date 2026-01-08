#include "qcore_metriplectic.h"
#include "vga_driver.h"

// Global state for predictability in the freestanding environment
SystemState state;

// Port I/O
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
    return ret;
}

void serial_putc(char c) {
    while ((inb(0x3fd) & 0x20) == 0);
    outb(0x3f8, c);
}
void serial_print(const char *s) {
    while (*s) serial_putc(*s++);
}

void kernel_main(uint32_t magic, void* mbi) {
    (void)magic; (void)mbi;
    
    serial_print("\n--- QUOREMIND KERNEL OS BOOTED (TEXT MODE) ---\n");
    init_system(&state);
    k_clear(BLACK);

    int step = 0;
    while (1) {
        solve_step(&state, 0.05f);

        // Render UI
        k_clear(BLACK);
        
        // Header
        k_print_at(2, 1, "QUOREMIND Q-CORE // SHEARED TORUS KERNEL v3.0", LIGHT_BLUE, BLACK);
        k_print_at(2, 2, "===========================================", DARK_GRAY, BLACK);

        k_print_at(2, 4, "SYSTEM STATUS:", LIGHT_GRAY, BLACK);
        if (state.stability > 90.0f) k_print_at(17, 4, "RESONANT LOCK", GREEN, BLACK);
        else if (state.stability > 70.0f) k_print_at(17, 4, "CANAL OPEN", CYAN, BLACK);
        else k_print_at(17, 4, "DRIFTING", YELLOW, BLACK);

        k_print_at(2, 5, "SYNC CLOCK C:", LIGHT_GRAY, BLACK);
        char c_buf[16];
        int c_int = (int)(state.sync_clock_c * 1000);
        if (c_int < 0) { c_buf[0] = '-'; c_int = -c_int; } else { c_buf[0] = ' '; }
        c_buf[1] = '0' + (c_int / 1000) % 10;
        c_buf[2] = '.';
        c_buf[3] = '0' + (c_int / 100) % 10;
        c_buf[4] = '0' + (c_int / 10) % 10;
        c_buf[5] = '0' + (c_int % 10);
        c_buf[6] = '\0';
        k_print_at(17, 5, c_buf, (state.sync_clock_c > 0.5f) ? GREEN : MAGENTA, BLACK);

        k_print_at(2, 6, "BUS THROUGHPUT:", LIGHT_GRAY, BLACK);
        if (state.bus.bus_throughput > 1.0f) k_print_at(18, 6, "MAXFLOW", LIGHT_BLUE, BLACK);
        else k_print_at(18, 6, "IDLE", DARK_GRAY, BLACK);

        k_print_at(2, 7, "LASER PUMP:", LIGHT_GRAY, BLACK);
        if (state.breathing_state > 0.5f) k_print_at(17, 7, "[B_ACTIVE]", WHITE, BLACK);
        else k_print_at(17, 7, "[B_IDLE]", DARK_GRAY, BLACK);

        // Render Holistic Visualization
        // 1. Central Sheared Channel (Z-Pinch)
        int cx = 35;
        int center_y = 14;
        uint8_t chan_color = (state.shear_flow >= 9.9f) ? WHITE : LIGHT_GRAY;
        for (int i = 0; i < 15; i++) {
            float y_f = (float)i / 15.0f;
            int y = center_y - 7 + i;
            float offset = k_sin(y_f * 4.0f + state.time) * (100.0f - state.stability) * 0.05f;
            int x_val = cx + (int)offset;
            k_putc(x_val, y, '#', chan_color, BLACK);
            // Pulsing Node in Channel
            if (state.breathing_state > 0.5f && (int)(state.time * 20) % 15 == i) {
                k_putc(x_val, y, '@', YELLOW, BLACK);
            }
        }

        // 2. Toroidal Modulation (Fan around the channel)
        int tx = 55;
        uint8_t tor_color = (state.sync_clock_c > 0.5f) ? CYAN : LIGHT_BLUE;
        for (int i = 0; i < TORUS_DIM; i++) {
            for (int j = 0; j < TORUS_DIM; j++) {
                float intensity = state.phi_re[i][j]*state.phi_re[i][j] + state.phi_im[i][j]*state.phi_im[i][j];
                if (intensity > 0.3f) {
                    float angle = (float)i * (2.0f * PI / TORUS_DIM) + state.global_identity;
                    float radius = 5.0f + (float)j * 0.4f;
                    int x_pos = tx + (int)(k_cos(angle) * radius * 2.1f);
                    int y_pos = center_y + (int)(k_sin(angle) * radius);
                    k_putc(x_pos, y_pos, (state.breathing_state > 0.5f) ? '*' : '.', tor_color, BLACK);
                }
            }
        }

        // 3. Core Indicators
        for(int i=0; i<4; i++) {
            k_print_at(2, 10 + i, "CORE", LIGHT_GRAY, BLACK);
            k_putc(7, 10 + i, '0' + i, WHITE, BLACK);
            int sync_len = (int)(state.bus.core_sync[i] * 10);
            for(int s=0; s<10; s++) k_putc(9+s, 10+i, (s < sync_len) ? '>' : '-', (sync_len > 8) ? GREEN : DARK_GRAY, BLACK);
        }

        k_print_at(2, ROWS-2, "ENV: QEMU-I386 // BRIDGE: TORUS-SHEAR // CORE: MULTIPLEX", DARK_GRAY, BLACK);

        if (step % 5 == 0) {
            static const char spinner[] = {'|', '/', '-', '\\'};
            serial_print("\r[HOLISTIC_SYNC] ");
            serial_putc(spinner[(step / 5) % 4]);
            serial_print("  BUS_TP: ");
            if (state.bus.bus_throughput > 1.0f) serial_print("MAX  ");
            else serial_print("IDLE ");
            serial_print("  BRTH: ");
            if (state.breathing_state > 0.5f) serial_print("ON ");
            else serial_print("OFF");
        }
        step++;

        // Delay loop for visibility (approx 60fps in QEMU environment)
        for (volatile int i = 0; i < 4000000; i++); 
    }
}
