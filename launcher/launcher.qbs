import qbs

DynamicLibrary {
    name: "LauncherApi"
    Depends { name: "cpp" }
    Depends { name: "CommonApi" }
    Depends { name: "InternalApi" }

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: ["../include"]
    }

    destinationDirectory: '.'

    cpp.dynamicLibraries: [
        "dl",
        "pthread"
    ]

    cpp.includePaths: [
        "../core",
        "../include",
    ]

    files: [
        "../core/core_extern_resources.h",
        "../core/main.c",
        "dynamic_loader.h",
        "dynamic_loader.c",
        "launcher_mainloop.c",
        "ta_extern_resources.h",
        "ta_internal_thread.h",
        "ta_internal_thread.c",
        "ta_io_thread.h",
        "ta_io_thread.c",
        "ta_process.h",
        "ta_process.c",
    ]
}
