CC = gcc
CFLAGS = -Wall -Wextra -O2 -I.
LDFLAGS = -lX11 -lm -lasound

SRCS = qcore_sim.c qcore_sim_bench.c qcore_metriplectic.c hal_golden_launder.c hal_audio_host.c
OBJS = $(SRCS:.c=.o)
all: qcore_sim qcore_sim_bench

qcore_sim: qcore_sim.o qcore_metriplectic.o hal_golden_launder.o hal_audio_host.o
	$(CC) qcore_sim.o qcore_metriplectic.o hal_golden_launder.o hal_audio_host.o -o qcore_sim $(LDFLAGS)

qcore_sim_bench: qcore_sim_bench.o qcore_metriplectic.o hal_golden_launder.o hal_audio_host.o
	$(CC) qcore_sim_bench.o qcore_metriplectic.o hal_golden_launder.o hal_audio_host.o -o qcore_sim_bench $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

verify: all
	./$(TARGET)
