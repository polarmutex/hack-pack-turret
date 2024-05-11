SHELL=/usr/bin/env bash
SERIAL=/dev/ttyUSB0  # This may need to be changed, based on the output of `arduino-cli board list`.
BAUDRATE=115200  # This should be in sync with whatever you set in the sketch via `Serial.begin(xxx);` (if you set it at all).

.PHONY: all compile upload monitor

all: compile upload

# Optional: provide a git hash so the running program can report what version it was built from.
#
# The arduino-cli executable runs in an FHS environment, but it downloads its own Python that does not work inside the `nix develop` shell.
# So we override it with a working Python from our nix development shell.
compile:
	arduino-cli compile -v \
	--fqbn arduino:avr:nano \
	--build-property "compiler.cpp.extra_flags=\"-DGIT_VERSION=\"$(shell git rev-parse HEAD)\"\""

upload:
	[ -e $(SERIAL) ] && \
	arduino-cli upload -v -p $(SERIAL)  --fqbn arduino:avr:nano || \
	{ echo "==> $(SERIAL) is not available"; exit 1; }

# Monitor the serial output.
# The --imap option maps '\n' to '\r\n' so newlines are newlines.
monitor:
	picocom -b $(BAUDRATE) --imap lfcrlf $(SERIAL)
