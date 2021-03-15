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
    Pipeline("linux", "arm64", "1.6"),
    Pipeline("linux", "amd64", "1.6", true),
]
