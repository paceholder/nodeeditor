# Qt NodeEditor

**NodeEditor** is conceived as a general-purpose Qt-based library aimed at
graph-controlled data processing.  Nodes represent algorithms with certain inputs
and outputs. Connections transfer data from the output (source) of the first node
to the input (sink) of the second one.

**NodeEditor** framework is a Visual [Dataflow
Programming](https://en.wikipedia.org/wiki/Dataflow_programming) tool.  A library
client defines models and registers them in the data model registry.  Further
work is driven by events taking place in DataModels and Nodes.  The model
computing is triggered upon arriving of any new input data. The computed result
is propagated to the output connections. Each new connection fetches available
data and propagates is further.

Each change in the source node is immediately propagated through all the
connections updating  the whole graph.

## Navigation

- [Navigation](#navigation)
- [Platforms](#platforms)
- [Dependencies](#dependencies)
- [Current state](#current-state)
- [Building](#building)
  - [Linux](#linux)
  - [Qt Creator](#qt-creator)
- [With Cmake using `vcpkg`](#with-cmake-using-vcpkg)
- [>>> Version 3 Roadmap <<<](#-version-3-roadmap-)
- [Citing](#citing)
- [Youtube video:](#youtube-video)
- [Now with styles](#now-with-styles)
- [Buy me a beer](#buy-me-a-beer)
- [Showcase](#showcase)
  - [CANdevStudio](#candevstudio)
  - [Chigraph](#chigraph)
  - [Spkgen particle engine editor](#spkgen-particle-engine-editor)

## Platforms

* OSX (Apple Clang - LLVM 3.6), Linux (x64, gcc-7.0, clang-7): [![Build Status](https://travis-ci.org/paceholder/nodeeditor.svg?branch=master)](https://travis-ci.org/paceholder/nodeeditor)
* Windows (msvc2019/Qt5/Win32, msvc2019/Qt6/x64): [![Build status](https://ci.appveyor.com/api/projects/status/wxp47wv3uyyiujjw/branch/master?svg=true)](https://ci.appveyor.com/project/paceholder/nodeeditor/branch/master)


## Dependencies

* Qt >5.15
* CMake 3.8
* Catch2

## Current state

* Model-based nodes
* Automatic data propagation
* Datatype-aware connections
* Embedded Qt widgets
* One-output to many-input connections
* JSON-based interface styles
* Saving scenes to JSON files

## Building

### Linux

~~~
git clone git@github.com:paceholder/nodeeditor.git
cd nodeeditor
mkdir build
cd build
cmake ..
make -j && make install
~~~

### Qt Creator

1. Open `CMakeLists.txt` as project.
2. If you don't have the `Catch2` library installed, go to `Build Settings`, disable the checkbox `BUILD_TESTING`.
3. `Build -> Run CMake`
4. `Build -> Build All`
5. Click the button `Run`

### With Cmake using [`vcpkg`](https://github.com/microsoft/vcpkg)

1. Install `vcpkg`
2. Add the following flag in configuration step of `CMake`
   ```bash
   -DCMAKE_TOOLCHAIN_FILE=<vcpkg_dir>/scripts/buildsystems/scripts/buildsystems/vcpkg.cmake
   ```


## >>> Version 3 Roadmap <<<

1. Headless mode. [done]
   You can create, populate, modify the derivative of AbstractGraphModel
   without adding it to the actual Flow Scene.
   The library is now designed to be general-purpose graph
   visualization and modification tool, without specialization on only
   data propagation.
2. Build data propagation on top of the graph code [done].
   - Fix old unit-tests. [in progress].
   - Fix save/restore. [done].
   - Fix CI scriptst on travis and appveyor. [not started].
3. Backward compatibility with Qt5 [not started/help needed].
3. Write improved documentation based on Sphynx platform [done].
4. Extend set of examples [partially done].
5. Undo Redo [done].
6. Python wrappring using PySide [HELP NEEDED].
7. Implement grouping nodes [not started].
8. GUI: fix scrolling for scene view window scrolling [need to check Qt6]

Any suggestions are welcome.

## Citing

    Dmitry Pinaev et al, Qt Node Editor, (2017), GitHub repository, https://github.com/paceholder/nodeeditor

BibTeX

    @misc{Pinaev2017,
      author = {Dmitry Pinaev et al},
      title = {Qt5 Node Editor},
      year = {2017},
      publisher = {GitHub},
      journal = {GitHub repository},
      howpublished = {\url{https://github.com/paceholder/nodeeditor}},
      commit = {1d1757d09b03cea0e4921bc19659465fe6e65b9b}
    }


## Youtube video:

[![Youtube demonstration](pictures/vid1.png)](https://www.youtube.com/watch?v=pxMXjSvlOFw)

## Now with styles


[![Styles](pictures/style_example.png)](https://www.youtube.com/watch?v=i_pB-Y0hCYQ)


## Buy me a beer

[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://paypal.me/DmitryPinaev)

## Showcase

### [CANdevStudio](https://github.com/GENIVI/CANdevStudio)

Cost-effective, cross-platform replacement for CAN simulation software.
CANdevStudio enables to simulate CAN signals such as ignition status, doors status or reverse gear by every automotive developer. Thanks to modularity it is easy to implement new, custom features.

[![Qt Design Studio](pictures/showcase_CANdevStudio.png)](https://youtu.be/1TfAyg6DG04?t=22)

### [Chigraph](https://github.com/chigraph/chigraph)

Chigraph is a programming language for beginners that is unique in that it is an
intuitive flow graph:

![chigraph screenshot](pictures/chigraph.png)

It features easy bindings to C/C++, package management, and a cool interface.

### [Spkgen particle engine editor](https://github.com/fredakilla/spkgen)

![spkgen screenshot](pictures/spkgen.png)

Spkgen is an editor for the SPARK particles engine using a node-based interface
to create particles effects for games.
