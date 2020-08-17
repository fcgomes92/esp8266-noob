.PHONY: build monitor upload

build:
	@python3 ./scripts/build.py --upload_flags=$(UPLOAD_FLAGS) --upload_port=$(UPLOAD_PORT) --monitor_port=$(MONITOR_PORT)

monitor:
	@pio device monitor

upload: build
	@pio run --target upload