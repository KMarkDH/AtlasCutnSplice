# AtlasCutnSplice
A little tool to cut and aplice atlas


[INSTALLATION]
    I am using nothings/stb to handle png file.
    You need to install it first.

```
    clone this repo
    git submodule add git@github.com:nothings/stb.git include/stb
    git submodule update --init --recursive
    
    mkdir build
    cd build
    cmake ..
    make
    make install
```

[USAGE]

    pngcut <filepath> <width> <height>
    pngsplice <dirpath> <margin>