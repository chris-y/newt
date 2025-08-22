
all: newt
newt: main.c
	zcc +zxn -v -startup=30 -clib=sdcc_iy -SO3 --opt-code-size @newt.lst -o newt -subtype=dotn -create-app -DAMALLOC
clean:
	rm ./newt
