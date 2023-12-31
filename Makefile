# Uncomment lines below if you have problems with $PATH
#SHELL := /bin/bash
#PATH := /usr/local/bin:$(PATH)

all:
	pio -f -c vim run

upload:
	pio -f -c vim run --target upload

clean:
	pio -f -c vim run --target clean

program:
	pio -f -c vim run --target program

uploadfs:
	pio -f -c vim run --target uploadfs

update:
	pio -f -c vim update

monitor:
	pio -f -c vim device monitor

compile_commands.json: FORCE
	pio -f -c vim run --target compiledb

FORCE: ;

# vim: syntax=make ts=4 sw=4 sts=4 sr noet
