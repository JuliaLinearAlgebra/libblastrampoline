all: clean openblas trampoline

openblas:
	gcc -Wl,-rpath /Users/viral/julia/usr/lib  example.c -L ~/julia/usr/lib -lopenblas64_ -o example_openblas

trampoline:
	-cp -f ../src/libblastrampoline*{so,dylib,dll} .
	gcc -DTRAMPOLINE example.c -L. -lblastrampoline -o example_trampoline

clean:
	rm -fr *.dSYM example_* *.o *~ *# *.dylib
