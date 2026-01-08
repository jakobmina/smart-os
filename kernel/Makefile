CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lX11 -lm

SRCS = qcore_sim.c qcore_metriplectic.c
OBJS = $(SRCS:.c=.o)
TARGET = qcore_sim

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

verify: all
	./$(TARGET)
