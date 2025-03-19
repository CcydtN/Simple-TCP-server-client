setup build_type:
    mkdir -p builddir/{{build_type}}
    cd builddir/{{build_type}}; cmake -DCMAKE_BUILD_TYPE={{build_type}} ../..
    
clean:
    rm -r builddir

[working-directory: 'builddir/debug']
debug: (setup "debug")
    cmake --build .

[working-directory: 'builddir/release']
release: (setup "release")
    cmake --build .
