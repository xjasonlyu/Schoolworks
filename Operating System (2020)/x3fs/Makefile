dir := bin
src := src

bin  := x3fs
tool := mkx3fs

objects := $(wildcard src/*.c)

all: build mkx3fs

build:
	mkdir -p $(dir) && gcc -o $(dir)/$(bin) -I$(src) $(objects) shell.c

mkx3fs:
	mkdir -p $(dir) && gcc -o $(dir)/$(tool) -I$(src) mkx3fs.c

clean:
	rm -rf $(dir)
