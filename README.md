# AfterRomu

An unofficial source port of the RomuStrike v140 version.

## Building

Build the docker:
```bash
$ docker build -t afterromu:latest .
```

Run the Docker:
```bash
# Windows
$ docker run -v ${PWD}:/opt --rm -it --entrypoint bash afterromu
# Bash / Linux
$ docker run -v $PWD:/opt --rm -it --entrypoint bash afterromu
```

Build it in Docker:
```bash
$ mkdir build
$ cd build
$ cmake .. && make -j
```

## License

The source code for RomuStrike includes code from the following sources:
- Half-Life, from Valve Software/ID Software (all rights reserved)
- The "HL Rendering Engine", a minimal engine inspired by Half-Life, created
  by Bart Sekura

Additionally, the repository includes utility libraries provided by Nate Miller,
which are licensed under the GPLv2+ license.
