## IR remote controlled motorized audio volume ##

### Cloning the repo

To clone the repo, don't forget to clone the submodules:

```
git clone https://github.com/danngreen/ir-remote
cd ir-remote
git submodule update --init
```

### Building

To build:

```
cmake -B build
cmake --build build
```

There is also a Makefile if you prefer that syntax (it just wraps the cmake calls):

```
make
```

