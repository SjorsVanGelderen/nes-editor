# NES editor

![Screenshot of the editor](/assets/screenshot3.png?raw=true)

## Introduction

This project is still in an early stage. Currently, the editor allows loading, editing and saving of `.chr` files. It has only been tested with graphics extracted from a `Super Mario Bros.` `ROM` file. The project has only been tested on `MacOS High Sierra`.

The full NES palette is available, and during editing the palette is constrained so as to discourage graphics the NES can't actually render.

This editor aims to replace [YY-CHR](http://www.geocities.jp/yy_6502/yychr/index.html) at least for NES graphics.

![Another screenshot of the editor](/assets/screenshot2.png?raw=true)

## Next priorities

* More drawing tools
* Selecting and sliding
* Cutting / copying and pasting
* Specify file to load
* Specify file to save
* Implement toggle-able grid
* Using different character data patterns
* Allow specifying and saving meta-sprites
* Implement nametable editor (including meta-tiles)
* Implement attribute-table editor
* Support for other operating systems (`Windows`, `Linux`, possibly `BSD`)

![Yet another screenshot of the editor](/assets/screenshot.png?raw=true)

## Controls

* Load character -> `L` (loads a file called `data.chr`)
* Save character -> `S` (saves a file called `data.chr`)
* Load samples -> `Z` (loads a file called `samples.sam`)
* Save samples -> `X` (saves a file called `samples.sam`)
* `1` and `2` -> Switch between character / sample editing mode and nametable editing mode(this mode is currently not usable)
* Scroll -> zoom

## Technical details

The editor is written in `C++` using `Emacs`. A `Makefile` is supplied, so running `make` from this folder should compile the project for you. The project uses `GLFW` and `OpenGL 3.2`.
