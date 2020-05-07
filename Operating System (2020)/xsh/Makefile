all: xsh echo show sleep

xsh:
	gcc -o xsh xsh.c helper.c

echo:
	gcc -o _echo cmd_echo.c

show:
	gcc -o _show cmd_show.c

sleep:
	gcc -o _sleep cmd_sleep.c

clean:
	rm -rf xsh
	rm -rf _*
