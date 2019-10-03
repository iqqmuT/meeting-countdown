# Countdown clock for meetings

[![Build Status](https://travis-ci.org/iqqmuT/meeting-countdown.svg?branch=master)](https://travis-ci.org/iqqmuT/meeting-countdown)

## Building

* Install Docker
* Run this command on cloned repository:

```
docker run -v $PWD:/code iqqmut/meeting-countdown-builder
```

The command mounts current directory and builds the app inside Docker container and if everything works, you should find `bin/meeting-countdown.exe`. Required libraries are statically linked so no external DLL files are needed to run the application. Docker image uses Mingw-w64 to cross compile the application to be run on Windows.

To run `make clean`, run this command:

```
docker run -v $PWD:/code iqqmut/meeting-countdown-builder make clean
```

To open a shell inside Docker container:

```
docker run --rm -it -v $PWD:/code iqqmut/meeting-countdown-builder /bin/bash
```

## Coding Style

Vaguely using [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html).
