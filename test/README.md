# Simple libblastrampoline test

This test:

* Downloads OpenBLAS and CompilerSupportLibraries
* Builds `dgemm_test.c` linking directly against `OpenBLAS`, runs it to verify this can run at all.
* Builds `libblastrampoline`
* Re-builds `dgemm_test.c`, this time linking only against `libblastrampoline`, and finally runs `dgemm_test` while defining `LIBBLAS_NAME` to point at `libopenblas`, and showing the same result.