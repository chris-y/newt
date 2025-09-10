ZMAKEBAS ?= ../zmakebas/zmakebas

all: newt
newt: main.c
	zcc +zxn -v -startup=30 -clib=sdcc_iy -SO3 --opt-code-size @newt.lst -o newt -subtype=dotn -create-app -DAMALLOC
clock.bas: bas/clock.bas
	$(ZMAKEBAS) -3 -o clock.bas bas/clock.bas
clean:
	rm ./newt
	rm ./clock.bas
