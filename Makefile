ZMAKEBAS ?= ../zmakebas/zmakebas

all: newt
newt: newt.lst
	zcc +zxn -v -startup=30 -clib=sdcc_iy -SO3 --opt-code-size @newt.lst -o newt -subtype=dotn -create-app -DAMALLOC
%.bas: bas/%.bas
	$(ZMAKEBAS) -3 -l -o $@ $<
clean:
	rm ./newt
	rm ./*.bas
