local Pipeline(os, arch, version, alpine=false) = {
    kind: "pipeline",
    name: os+" - "+arch+" - Julia "+version+(if alpine then " (Alpine)" else ""),
    platform: {
        os: os,
        arch: arch
    },
    steps: [
        {
            name: "Run tests",
            # Use Alpine 3.13 because 3.14 has some troubles with Docker due to
            # https://wiki.alpinelinux.org/wiki/Release_Notes_for_Alpine_3.14.0#faccessat2
            image: "julia:"+version+(if alpine then "-alpine3.13" else ""),
            commands: [
                (if alpine then "" else "apt-get update -y"),
                (if alpine then "apk add build-base linux-headers" else "apt-get install -y gcc build-essential"),
                "julia --project=test --check-bounds=yes --color=yes -e 'using InteractiveUtils; versioninfo(verbose=true)'",
                "julia --project=test --check-bounds=yes --color=yes -e 'using Pkg; Pkg.instantiate()'",
                "julia --project=test --check-bounds=yes --color=yes test/runtests.jl"
            ]
        }
    ],
    trigger: {
        branch: ["main"]
    }
};

[
    Pipeline("linux", "arm", "1.6.1"),
    Pipeline("linux", "arm64", "1.6"),
    Pipeline("linux", "amd64", "1.6", true),
]
