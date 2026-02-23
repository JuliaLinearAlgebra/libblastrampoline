using Libdl, Test

# Taken from AppleAccelerate.jl to avoid a dependency on it
const libacc = "/System/Library/Frameworks/Accelerate.framework/Accelerate"
const libacc_info_plist = "/System/Library/Frameworks/Accelerate.framework/Versions/Current/Resources/Info.plist"

function get_macos_version(normalize=true)
    @static if !Sys.isapple()
        return nothing
    end

    plist_lines = split(String(read("/System/Library/CoreServices/SystemVersion.plist")), "\n")
    vers_idx = findfirst(l -> occursin("ProductVersion", l), plist_lines)
    if vers_idx === nothing
        return nothing
    end

    m = match(r">([\d\.]+)<", plist_lines[vers_idx+1])
    if m === nothing
        return nothing
    end

    ver = VersionNumber(only(m.captures))
    if normalize && ver.major == 16
        return VersionNumber(26, ver.minor, ver.patch)
    end
    return ver
end


# Load the Accelerate library
libacc_handle = dlopen(libacc)
@testset "Accelerate ILP64 loading" begin
    # ILP64 requires macOS 13.3+
    if get_macos_version() >= v"13.3"
        # Load the ILP64 interface
        lbt_forward(lbt_handle, libacc; clear=true, suffix_hint="\x1a\$NEWLAPACK\$ILP64")

        # Test that we have only one library loaded
        config = lbt_get_config(lbt_handle)
        libs = unpack_loaded_libraries(config)
        @test length(libs) == 1

        # Test that it's Accelerate and it's correctly identified
        @test libs[1].libname == libacc
        @test libs[1].interface == LBT_INTERFACE_ILP64

        # Test that `dgemm` forwards to `dgemm_` within the Accelerate library
        acc_dgemm = dlsym(libacc_handle, "dgemm\$NEWLAPACK\$ILP64")
        @test lbt_get_forward(lbt_handle, "dgemm_", LBT_INTERFACE_ILP64) == acc_dgemm
    end
end

@testset "Accelerate LP64 loading" begin
    # New LAPACK interface requires macOS 13.3+
    if get_macos_version() >= v"13.3"
        # Load the LP64 interface
        lbt_forward(lbt_handle, libacc; clear=true, suffix_hint="\x1a\$NEWLAPACK")

        # Test that we have only one library loaded
        config = lbt_get_config(lbt_handle)
        libs = unpack_loaded_libraries(config)
        @test length(libs) == 1

        # Test that it's Accelerate and it's correctly identified
        @test libs[1].libname == libacc
        @test libs[1].interface == LBT_INTERFACE_LP64

        # Test that `dgemm` forwards to `dgemm_` within the Accelerate library
        acc_dgemm = dlsym(libacc_handle, "dgemm\$NEWLAPACK")
        @test lbt_get_forward(lbt_handle, "dgemm_", LBT_INTERFACE_LP64) == acc_dgemm
    end
end

@testset "Accelerate threading" begin
    # This threading API will only work on v15 and above
    if get_macos_version() >= v"15"
        lbt_forward(lbt_handle, libacc; clear=true)

        # Set to single-threaded
        lbt_set_num_threads(lbt_handle, 1)
        @test lbt_get_num_threads(lbt_handle) == 1

        # Set to multi-threaded
        # Accelerate doesn't actually let us say how many threads, so we must test for greater than
        lbt_set_num_threads(lbt_handle, 2)
        @test lbt_get_num_threads(lbt_handle) > 1

        # Set back to single-threaded
        lbt_set_num_threads(lbt_handle, 1)
        @test lbt_get_num_threads(lbt_handle) == 1
    end
end

@testset "Accelerate info" begin
    lbt_forward(lbt_handle, libacc; clear=true)

    # Get the first loaded library
    config = lbt_get_config(lbt_handle)
    lib = unsafe_load(config.loaded_libs, 1)

    info_str = lbt_get_library_info(lbt_handle, lib)

    @test occursin("Apple", info_str)
end
