# Running the trampolineblas examples

# Load LD_LIBRARY_PATH or DYLD_LIBRARY_PATH with the paths to all the relevant libraries
# Include locations of libtrampolineblas, and the BLAS that we want to forward to.

```
➜  sgesv_test git:(main) ✗ export DYLD_LIBRARY_PATH=../../src:~/julia/usr/lib:~/.julia/artifacts/6d70970ab8b5b56c585dee88f158d90d2457fe84/lib/:~/.julia/artifacts/073ff95e2c63501547247d6e1321bf4ee2a78933/lib/:~/.julia/artifacts/e76af028a823f7e7c18226c8079d03035c2e4c46/lib
```
# Forwarding to MKL
```
➜  sgesv_test git:(main) ✗ ./ex_t ~/.julia/artifacts/073ff95e2c63501547247d6e1321bf4ee2a78933/lib/libmkl_rt.dylib
Generating forwards to /Users/viral/.julia/artifacts/073ff95e2c63501547247d6e1321bf4ee2a78933/lib/libmkl_rt.dylib
1.000000e+00
1.000000e+00
1.000000e+00
```
# Forwarding to OpenBLAS with the same binary
```
➜  sgesv_test git:(main) ✗ ./ex_t ~/.julia/artifacts/6d70970ab8b5b56c585dee88f158d90d2457fe84/lib/libopenblas.0.3.9.dylib
Generating forwards to /Users/viral/.julia/artifacts/6d70970ab8b5b56c585dee88f158d90d2457fe84/lib/libopenblas.0.3.9.dylib
1.000000e+00
1.000000e+00
1.000000e+00
```
# Forwarding for trampolineblas built with 64_ prefixes to the Julia supplied OpenBLAS
```
➜  sgesv_test git:(main) ✗ ./ex_t64_ ~/julia/usr/lib/libopenblas64_.dylib
Generating forwards to /Users/viral/julia/usr/lib/libopenblas64_.dylib
1.000000e+00
1.000000e+00
1.000000e+00
```
