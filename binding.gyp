{
    "targets": [
        {
            "target_name": "addon",
            "sources": ["./src/addon.cc"],
            "include_dirs": ["./ext/include",   "<!@(node -p \"require('node-addon-api').include\")"],
            'defines': ['NAPI_DISABLE_CPP_EXCEPTIONS'],
            "conditions": [
                ['OS=="linux"', {
                    "libraries": [
                        "$(CURDIR)/../ext/lib/linux/ctp6.3.15/libthostmduserapi_se.so",
                        "$(CURDIR)/../ext/lib/linux/ctp6.3.15/libthosttraderapi_se.so"
                    ],
                    "link_settings": {
                        'library_dirs': ["$(CURDIR)/../ext/lib/linux/ctp6.3.15"]
                    },
                }],
                ["OS==\"win\"", {
                    "libraries": [
                        "thostmduserapi_se.lib",
                        "thosttraderapi_se.lib"
                    ],
                    "library_dirs": [
                        "./ext/lib/windows/x64/ctp6.3.15",
                    ],
                    "copies": [
                        {
                            "destination": "<(module_root_dir)/build/Release/",
                            "files": ["<(module_root_dir)/ext/lib/windows/x64/ctp6.3.15/thostmduserapi_se.dll",
                                      "<(module_root_dir)/ext/lib/windows/x64/ctp6.3.15/thosttraderapi_se.dll"]
                        }
                    ]
                }]
            ]
        }
    ],
}
