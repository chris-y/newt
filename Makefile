ZMAKEBAS ?= ../zmakebas/zmakebas

all: NEWT
NEWT: newt.lst
	zcc +zxn -v -startup=30 -clib=sdcc_iy -SO3 --opt-code-size @newt.lst -o NEWT -subtype=dotn -create-app -DAMALLOC
%.bas: bas/%.bas
	$(ZMAKEBAS) -3 -l -o $@ $<
clean:
	rm ./NEWT
	rm ./*.bas
