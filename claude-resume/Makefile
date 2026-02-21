CC      = cc
CFLAGS  = -std=c11 -O2 -Wall -Wextra
TARGET  = claude-resume

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $@ $<

install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

clean:
	rm -f $(TARGET)
