# Countdown clock for meetings

## Building

* Install Docker
* Run this command on cloned repository:

```
docker run -v $PWD:/code iqqmut/meeting-countdown-builder
```

The command mounts current directory and builds the app inside Docker container and if everything works, you should find `bin/meeting-countdown.exe`. Required libraries are statically linked so no external DLL files are needed.
