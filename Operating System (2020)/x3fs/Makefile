bin := bin
cmd := cmd
src := src

x3fs  := x3fs
mkx3fs := mkx3fs

objects := $(wildcard $(src)/*.c)

all: x3fs mkx3fs

x3fs:
	mkdir -p $(bin) && gcc -o $(bin)/$(x3fs) -I$(src) $(objects) $(cmd)/shell.c

mkx3fs:
	mkdir -p $(bin) && gcc -o $(bin)/$(mkx3fs) -I$(src) $(cmd)/mkx3fs.c

clean:
	rm -rf $(bin)
