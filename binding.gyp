{
    "targets": [
        {
            "target_name": "addon",
            "sources": ["./src/addon.cc"],
            "include_dirs": ["./ext/include"],
            "library_dirs": [
                "./ext/lib/windows/x64/ctp6.3.15",
            ],
            "libraries": [
                "thostmduserapi_se.lib", "thosttraderapi_se.lib"
            ],
            'msvs_settings': {
                'VCCLCompilerTool': {
                    'ExceptionHandling': 1,  # /EHsc
                    'AdditionalOptions': ['/m']
                }
            },
            "conditions": [
                ["OS==\"win\"", {
                    'msvs_settings': {
                        'VCCLCompilerTool': {
                            'AdditionalOptions': [
                                # disable Thread-Safe "Magic" for local static variables
                                '/m',
                            ],
                        },
                    },
                }]
            ]
        }
    ],
}
