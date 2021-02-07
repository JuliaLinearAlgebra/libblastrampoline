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
            image: "julia:"+version+(if alpine then "-alpine" else ""),
            commands: [
                "julia --project=. --check-bounds=yes --color=yes -e 'using InteractiveUtils; versioninfo(verbose=true)'",
                "julia --project=. --check-bounds=yes --color=yes -e 'using Pkg; Pkg.instantiate()'",
                "julia --project=. --check-bounds=yes --color=yes test/runtests.jl"
            ]
        }
    ],
    trigger: {
        branch: ["master"]
    }
};

[
    Pipeline("linux", "arm64", "1.6"),
]
