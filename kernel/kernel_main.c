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
        k_print_at(2, 1, "QUOREMIND Q-CORE // BAREMETAL KERNEL v1.5", LIGHT_BLUE, BLACK);
        k_print_at(2, 2, "===========================================", DARK_GRAY, BLACK);

        k_print_at(2, 4, "SYSTEM STATUS:", LIGHT_GRAY, BLACK);
        if (state.stability > 80.0f) k_print_at(17, 4, "OPTIMAL [LOCK]", GREEN, BLACK);
        else if (state.stability > 40.0f) k_print_at(17, 4, "STOCHASTIC", YELLOW, BLACK);
        else k_print_at(17, 4, "CRITICAL [BREACH]", RED, BLACK);

        k_print_at(2, 5, "FLOW CONTROL:", LIGHT_GRAY, BLACK);
        if (state.shear_flow >= 9.9f) k_print_at(17, 5, "MACH 10.0 [STABLE]", CYAN, BLACK);
        else k_print_at(17, 5, "CAOTIC FLOW", MAGENTA, BLACK);

        k_print_at(2, 6, "PHOTONIC FLUX:", LIGHT_GRAY, BLACK);
        if (state.bit_stream > 10.0f) k_print_at(17, 6, "TRANSMITTING", LIGHT_BLUE, BLACK);
        else k_print_at(17, 6, "DECOHERENCE", DARK_GRAY, BLACK);

        k_print_at(2, 7, "CAUSAL REDUCTION:", LIGHT_GRAY, BLACK);
        if (state.causal_flux > 2.0f) k_print_at(20, 7, "[5V_ACTIVE]", YELLOW, BLACK);
        else k_print_at(20, 7, "[FILTERED]", DARK_GRAY, BLACK);

        // Render Z-Pinch Plasma (Text Art Column)
        int cx = 40;
        int center_y = 14;
        uint8_t p_color = (state.shear_flow >= 9.9f) ? WHITE : LIGHT_BLUE;

        for (int i = 0; i < 15; i++) {
            float y_f = (float)i / 15.0f;
            int y = center_y - 7 + i;
            
            float offset = k_sin(y_f * 4.0f + state.time) * (100.0f - state.stability) * 0.1f;
            int x = cx + (int)offset;
            
            k_putc(x, y, '#', p_color, BLACK);
            k_putc(x-1, y, '(', p_color, BLACK);
            k_putc(x+1, y, ')', p_color, BLACK);
            
            // Render Photonic Nodes (Lindblad pulses)
            // Pulse propagates based on time and node density
            float pulse_trigger = k_sin(y_f * 10.0f - state.time * 5.0f);
            if (pulse_trigger > 0.8f && state.node_density > 0.2f) {
                k_putc(x, y, '*', WHITE, BLACK); // A photonic packet
                if (pulse_trigger > 0.95f) k_putc(x, y, '@', YELLOW, BLACK); // High energy peak
            }

            // Add subtle glow tracers
            if (i % 3 == 0) {
                k_putc(x + (int)(offset*2), y, '*', DARK_GRAY, BLACK);
                k_putc(x - (int)(offset*2), y, '*', DARK_GRAY, BLACK);
            }
        }

        k_print_at(2, ROWS-2, "ENV: QEMU-I386 // MAPPED: 0xB8000 // CORE: METRIPLECTIC", DARK_GRAY, BLACK);

        if (step % 10 == 0) {
            static const char spinner[] = {'|', '/', '-', '\\'};
            serial_print("\r[QUOREMIND_CAUSAL] ");
            serial_putc(spinner[(step / 10) % 4]);
            serial_print("  FLUX: ");
            if (state.causal_flux > 1.0f) serial_print("REDUCED_BINARY ");
            else serial_print("IDLE           ");
            serial_print("  PHI_FILTER: ");
            if (state.golden_filter > 1.6f) serial_print("ACTIVE");
            else serial_print("WAIT  ");
        }
        step++;

        // Delay loop for visibility (approx 60fps in QEMU environment)
        for (volatile int i = 0; i < 4000000; i++); 
    }
}
