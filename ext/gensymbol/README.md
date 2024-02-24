# gensymbol

This uses [the `gensymbol` script from OpenBLAS](https://github.com/xianyi/OpenBLAS/blob/develop/exports/gensymbol) to generate a list of BLAS/LAPACK symbols from first principles.
To use this, run `./generate_func_list.sh` and it will automatically populate [the `src/exported_funcs.inc` file](../../src/exported_funcs.inc).
