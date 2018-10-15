# NES editor

![Screenshot of the editor](/screenshot.png?raw=true)

![Another screenshot of the editor](/screenshot2.png?raw=true)

## Introduction
This project is still in an early stage. Currently, the editor allows loading, editing and saving of `.chr` files. It has only been tested with graphics extracted from a `Super Mario Bros.` `ROM` file. The project has only been tested on `MacOS High Sierra`.

The full NES palette is available, and during editing the palette is constrained so as to discourage graphics the NES can't actually render.

## Next priorities

* More drawing tools
* Fix zooming controls
* Specify file to load
* Specify file to save
* Fix the samples (sub-palettes)
* Implement toggle-able grid
* Using different patterns
* Allow saving samples (sub-palettes)
* Allow specifying and saving meta-sprites
* Implement nametable editor (including meta-tiles)
* Implement attribute-table editor

## Controls

* Load -> `L` (loads a file called `data.chr`)
* Save -> `S` (saves a file called `data.chr`)
* Scroll -> zoom

## Technical details
The editor is written in `C++` using `Emacs`. A `Makefile` is supplied, so running `make` from this folder should compile the project for you. The project uses `GLFW` and `OpenGL 3.2`.
