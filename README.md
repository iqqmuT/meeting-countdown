# Countdown clock for meetings

[![Build Status](https://travis-ci.org/iqqmuT/meeting-countdown.svg?branch=master)](https://travis-ci.org/iqqmuT/meeting-countdown)

## Usage

### Examples

Countdown to 14:45 (2:45 PM):
```
meeting-countdown.exe -t 14:45
```

Countdown to 18:30 (6:30 PM), using configuration file `countdown.ini`:
```
meeting-countdown.exe -t 18:30 -c countdown.ini
```

5 minute countdown, using Comic Sans font:
```
meeting-countdown.exe -m 5 -f C:\Windows\Fonts\comic.ttf
```

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
