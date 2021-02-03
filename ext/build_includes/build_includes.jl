using BinaryBuilder, SHA

LBT_ROOT = abspath(dirname(dirname(@__DIR__)))

# This script heavily borroed from the OpenBLAS build recipe in Yggdrasil
# X-ref: https://github.com/JuliaPackaging/Yggdrasil/blob/master/O/OpenBLAS/common.jl
version = v"0.3.13"
for openblas32 in (true, false)
    sources = [
        ArchiveSource("https://github.com/xianyi/OpenBLAS/archive/v$(version).tar.gz",
                    "79197543b17cc314b7e43f7a33148c308b0807cd6381ee77f77e15acf3e6459e")
    ]

    script = """
    # We always want threading
    flags=(USE_THREAD=1 GEMM_MULTITHREADING_THRESHOLD=50 NO_AFFINITY=1)

    # We are cross-compiling
    flags+=(CROSS=1 PREFIX=/ "CROSS_SUFFIX=\${target}-")

    if [[ \${nbits} == 64 && "$(openblas32)" != "true" ]]; then
        # We're building an ILP64 BLAS with 64-bit BlasInt
        flags+=("LIBPREFIX=libopenblas64_" INTERFACE64=1 SYMBOLSUFFIX=64_)
    else
        flags+=("LIBPREFIX=libopenblas" INTERFACE64=1)
    fi
    flags+=(BINARY=\${nbits})

    # "build" the project, but only install the headers
    cd \${WORKSPACE}/srcdir/OpenBLAS*/

    # Fake out the buildsystem, make it think the libraries have been built
    touch lib.grd
    make "\${flags[@]}" "PREFIX=\${prefix}" dummy
    cp config.h config_last.h

    # Call `make install`, fully expecting this to blow up in our faces.
    make "\${flags[@]}" "PREFIX=\${prefix}" install || true
    """

    products = [
        FileProduct("include/cblas.h", :cblas_h),
        FileProduct("include/f77blas.h", :f77blas_h),
        FileProduct("include/lapack.h", :lapack_h),
        FileProduct("include/lapacke.h", :lapack_h),
        FileProduct("include/openblas_config.h", :openblas_config_h),
    ]
    name = "OpenBLASHeaders$(openblas32 ? "32" : "64")"
    build_tarballs(ARGS, name, version, sources, script, supported_platforms(;experimental=false), products, Dependency[], lock_microarchitecture=false)
end

# Unpack each set of headers into its own directory inside of `include/`:
rm(joinpath(LBT_ROOT, "include"), recursive=true, force=true)
for tarball_path in readdir("products")
    interface = startswith(tarball_path, "OpenBLASHeaders32") ? "LP64" : "ILP64"
    triplet = split(tarball_path, ".v$(version).")[end][1:end-7]
    tarball_path = joinpath("products", tarball_path)
    target_path = joinpath(LBT_ROOT, "include", interface, triplet)

    mkpath(target_path)
    run(`tar -C $(target_path) -xzf $(tarball_path) --strip-components=1 include/`)
end

# de-duplicate headers that share the exact same contents
targets = unique(vcat([readdir(joinpath(LBT_ROOT, "include", interface)) for interface in ("LP64", "ILP64")]...))
function collect_hashes(fname, interfaces=("LP64", "ILP64"))
    return vec([open(sha256, joinpath(LBT_ROOT, "include", interface, target, fname)) for interface in interfaces, target in targets])
end
function is_file_identical_across_targets(fname; interfaces=("LP64", "ILP64"))
    hashes = collect_hashes(fname, interfaces)
    return all(Ref(hashes[1]) .== hashes)
end
function symlink_common_file(fname; interfaces = ("LP64", "ILP64"), common_dir = joinpath(LBT_ROOT, "include", "common"))
    mkpath(common_dir)
    # Copy the file out to `common`
    common_path = joinpath(common_dir, fname)
    rm(common_path; force=true)
    mv(joinpath(LBT_ROOT, "include", interfaces[1], targets[1], fname), common_path)

    for interface in interfaces
        for target in targets
            src_path = joinpath(LBT_ROOT, "include", interface, target, fname)
            if islink(src_path)
                continue
            end
            rm(src_path; force=true)
            symlink(relpath(common_path, dirname(src_path)), src_path)
        end
    end
end

function cleanup_common_headers()
    @info("Cleaning up identical headers across targets/interfaces")
    common_filenames = unique(vcat([readdir(joinpath(LBT_ROOT, "include", interface, target)) for target in targets, interface in ("LP64", "ILP64")]...))
    for fname in common_filenames
        if is_file_identical_across_targets(fname; interfaces=("LP64", "ILP64"))
            @info(" -> $(fname)")
            symlink_common_file(fname; interfaces=("LP64", "ILP64"), common_dir=joinpath(LBT_ROOT, "include", "common"))
        end
    end
    @info("Cleaning up identical headers across target but not interface")
    for interface in ("LP64", "ILP64")
        common_filenames = unique(vcat([readdir(joinpath(LBT_ROOT, "include", interface, target)) for target in targets]...))
        for fname in common_filenames
            if is_file_identical_across_targets(fname; interfaces=(interface,))
                @info(" -> $(interface)/$(fname)")
                symlink_common_file(fname; interfaces=(interface,), common_dir=joinpath(LBT_ROOT, "include", interface, "common"))
            end
        end
    end
end

cleanup_common_headers()