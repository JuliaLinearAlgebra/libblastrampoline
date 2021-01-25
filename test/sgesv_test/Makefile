include ../../src/Make.inc

all: clean ex_openblas64_ ex_mkl ex_trampoline ex_trampoline64_

LIBOPENBLAS=-Wl,-rpath ~/julia-1.5.3/lib/julia -L ~/julia-1.5.3/lib/julia -lopenblas64_
LIBMKL=-Wl,-rpath ~/.julia/artifacts/a8e009985328801a84c9af6610f94f77a7c12852/lib -Wl,-rpath ~/.julia/artifacts/947793e42b663bacd09f00d96aa96a47095f3b1c/lib/ -L ~/.julia/artifacts/a8e009985328801a84c9af6610f94f77a7c12852/lib -lmkl_rt

ex_openblas64_:
	gcc ${LIBOPENBLAS} -DSGESV=sgesv_64_ example.c -o $@

ex_mkl:
	gcc ${LIBMKL} -DSGESV=sgesv_ example.c -o $@

ex_trampoline:
	gcc -DTRAMPOLINE -DSGESV=sgesv_ example.c -L../../src -lblastrampoline -o $@

ex_trampoline64_:
	gcc -DTRAMPOLINE -DSGESV=sgesv_64_ example.c -L../../src -lblastrampoline -o $@

clean:
	rm -fr *.dSYM ex_* *.o *~ *# *.${SHLIB_EXT}
