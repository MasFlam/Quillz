# Quillz
A [Processing](https://processing.org)-inspired, [Wren](https://wren.io)-based graphical sketching framework.

# Usage
See the [examples](examples/) directory for example programs.
```
quillz path/to/sketch/file.wren
```

# Building
You will need GLFW and OpenGL. On Debian the packages are `libglfw3-dev` and `libgl-dev`.
Wren is bundled as a git submodule, so when cloning the repo use `git clone --recursive <url...>`.
Once you have the dependencies, run `make` to compile everything. The final executable lands in
the repo root. Currently only POSIX systems are supported.
