include ../../src/Make.inc

all: clean ex_openblas64_ ex_mkl ex_t ex_t64_

LIBOPENBLAS=-Wl,-rpath /Users/viral/julia/usr/lib -L ~/julia/usr/lib -lopenblas64_
LIBMKL=-Wl,-rpath ~/.julia/artifacts/073ff95e2c63501547247d6e1321bf4ee2a78933/lib -Wl,-rpath ~/.julia/artifacts/e76af028a823f7e7c18226c8079d03035c2e4c46/lib/ -L ~/.julia/artifacts/073ff95e2c63501547247d6e1321bf4ee2a78933/lib -lmkl_rt

ex_openblas64_:
	gcc ${LIBOPENBLAS} -DSGESV=sgesv_64_ example.c -o $@

ex_mkl:
	gcc ${LIBMKL} -DSGESV=sgesv_ example.c -o $@

ex_t64_:
	gcc -DTRAMPOLINE -DSGESV=sgesv_64_ example.c -L../../src -lblastrampoline64_ -o $@

ex_t:
	gcc -DTRAMPOLINE -DSGESV=sgesv_ example.c -L../../src -lblastrampoline -o $@

clean:
	rm -fr *.dSYM ex_* *.o *~ *# *.${SHLIB_EXT}
