include ../../src/Make.inc

all: clean ex_openblas ex_mkl ex_t_openblas ex_t_mkl

LIBOPENBLAS=-Wl,-rpath /Users/viral/julia/usr/lib -L ~/julia/usr/lib -lopenblas64_ -DSGESV=sgesv_64_ -DLIBBLAS_FWD=\"/Users/viral/julia/usr/lib/libopenblas64_.${SHLIB_EXT}\"
LIBMKL=-Wl,-rpath ~/.julia/artifacts/073ff95e2c63501547247d6e1321bf4ee2a78933/lib -Wl,-rpath ~/.julia/artifacts/e76af028a823f7e7c18226c8079d03035c2e4c46/lib/ -L ~/.julia/artifacts/073ff95e2c63501547247d6e1321bf4ee2a78933/lib -lmkl_rt -DSGESV=sgesv_ -DLIBBLAS_FWD=\"/Users/viral/.julia/artifacts/073ff95e2c63501547247d6e1321bf4ee2a78933/lib/libmkl_rt.${SHLIB_EXT}\"

ex_openblas:
	gcc ${LIBOPENBLAS} example.c -o $@

ex_mkl:
	gcc ${LIBMKL} example.c -o $@

ex_t_openblas:
	-cp -f ../../src/libblastrampoline*.${SHLIB_EXT} .
	gcc -DTRAMPOLINE example.c ${LIBOPENBLAS} -L. -lblastrampoline64_ -o $@

ex_t_mkl:
	-cp -f ../../src/libblastrampoline*.${SHLIB_EXT} .
	gcc -DTRAMPOLINE example.c ${LIBMKL} -L. -lblastrampoline -o $@

clean:
	rm -fr *.dSYM ex_* *.o *~ *# *.${SHLIB_EXT}
