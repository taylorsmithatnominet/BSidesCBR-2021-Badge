# device ports - modify as needed or specify on the CLI
ifndef ESP_PORT
ESP_PORT=/dev/ttyUSB0
endif
ifndef SAMD_PORT
SAMD_PORT=/dev/ttyACM0
endif
$(info SAMD port: $(SAMD_PORT) ESP port: $(ESP_PORT))

# FIXME: nproc is quite linux-specific
EXTRA_FLAGS=-j$(shell nproc) --samd21-port=$(SAMD_PORT) --esp32-port=$(ESP_PORT)
ESP_TARGET=--target=esp32
SAMD_TARGET=--target=samd21
default: build flash monitor

clean:
	./build.sh --clean $(EXTRA_FLAGS)
clean-samd:
	./build.sh --clean $(EXTRA_FLAGS) $(SAMD_TARGET)
clean-esp:
	./build.sh --clean $(EXTRA_FLAGS) $(ESP_TARGET)
.PHONY: clean clean-samd clean-esp

build:
	./build.sh --build $(EXTRA_FLAGS)
build-samd:
	./build.sh --build $(EXTRA_FLAGS) $(SAMD_TARGET)
build-esp:
	./build.sh --build $(EXTRA_FLAGS) $(ESP_TARGET)
.PHONY: build build-samd build-esp

flash:
	./build.sh --flash $(EXTRA_FLAGS)
flash-samd:
	./build.sh --flash $(EXTRA_FLAGS) $(SAMD_TARGET)
flash-esp:
	./build.sh --flash $(EXTRA_FLAGS) $(ESP_TARGET)
.PHONY: flash flash-samd flash-esp


monitor: 
	./build.sh --monitor $(EXTRA_FLAGS)
monitor-samd: 
	./build.sh --monitor $(EXTRA_FLAGS) $(SAMD_TARGET)
monitor-esp: 
	./build.sh --monitor $(EXTRA_FLAGS) $(ESP_TARGET)
.PHONY: monitor monitor-samd monitor-esp
