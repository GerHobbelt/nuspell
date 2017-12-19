<!-- add following also to nuspell.org
[![Code Coverage Codecov](https://codecov.io/gh/hunspell/nuspell/branch/master/graph/badge.svg)](https://codecov.io/gh/hunspell/nuspell)
[![Build Status Travis](https://travis-ci.org/hunspell/nuspell.svg?branch=master)](https://travis-ci.org/hunspell/nuspell)
[![Build Status AppVeyor]([https://ci.appveyor.com/api/projects/status/ecxsq3s4j2b9n965?svg=true](https://ci.appveyor.com/api/projects/status/ecxsq3s4j2b9n965?svg=true)](https://ci.appveyor.com/project/hunspell-bot/nuspell-n4uof)
[![Translation Status](https://hosted.weblate.org/widgets/hunspell/-/translations/svg-badge.svg)](https://hosted.weblate.org/engage/hunspell/?utm_source=widget)

see also https://shields.io/
-->

# About Nuspell

NOTICE: Nuspell is currently under development. For contributing see [version 2
specification](https://github.com/hunspell/hunspell/wiki/Version-2-Specification).

Nuspell is a spell checker and morphological analyzer library and
program designed for languages with rich morphology and complex word
compounding or character encoding. Nuspell interfaces: Ispell-like
terminal interface using Curses library, Ispell pipe interface, C++
class and C functions. Nuspell is a pure C++ reimplementation of Hunspell.

Hunspell's code base comes from the OpenOffice.org MySpell
(http://lingucomponent.openoffice.org/MySpell-3.zip). See
README.MYSPELL, AUTHORS.MYSPELL and license.myspell files. Hunspell is
designed to eventually replace Myspell in OpenOffice.org.

Main features of Nuspell spell checker and morphological analyzer:

  - Unicode support (affix rules work only with the first 65535 Unicode
    characters)
  - Morphological analysis (in custom item and arrangement style) and
    stemming
  - Max. 65535 affix classes and twofold affix stripping (for
    agglutinative languages, like Azeri, Basque, Estonian, Finnish,
    Hungarian, Turkish, etc.)
  - Support complex compounds (for example, Hungarian and German)
  - Support language specific features (for example, special casing of
    Azeri and Turkish dotted i, or German sharp s)
  - Handle conditional affixes, circumfixes, fogemorphemes, forbidden
    words, pseudoroots and homonyms.
  - Free software. Versions 1.x are licenced under LGPL, GPL, MPL
    tri-license. Version 2 is licenced only under GNU LGPL.

# Dependencies

Build only dependencies:

    g++ make autoconf automake autopoint libtool wget catch

Runtime dependencies:

|                 | Mandatory        |Optional          |
|-----------------|------------------|------------------|
| libnuspell 1    |                  |                  |
| cmd line tool 1 | libiconv gettext | ncurses readline |
| libnuspell 2    | boost-locale     |                  |
| cmd line tool 2 |                  |                  |
    
Recommended tools for developers:

    vim qtcreator clang-format cppcheck gdb libtool-bin doxygen plantuml

# Compiling on GNU/Linux and Unixes

We first need to download the dependencies. On Linux, `gettext` and
`libiconv` are part of the standard library. On other Unixes we
need to manually install them.

For Ubuntu:

    sudo apt install autoconf automake autopoint libtool libboost-locale-dev \
                     libboost-system-dev

Then run the following commands:

    autoreconf -vfi
    ./configure
    make
    sudo make install
    sudo ldconfig

For dictionary development, use the `--with-warnings` option of
configure.

For interactive user interface of Nuspell executable, use the
`--with-ui option`.

Optional developer packages:

  - ncurses (need for --with-ui), eg. libncursesw5 for UTF-8
  - readline (for fancy input line editing, configure parameter:
    --with-readline)

In Ubuntu, the packages are:

    libncurses5-dev libreadline-dev

# Compiling on OSX and macOS

On macOS for compiler always use `clang` and not `g++` because Homebrew
dependencies are build with that.

    brew install autoconf automake libtool gettext boost
    brew link gettext --force

Then run the standard trio: autoreconf, configure, make. See above.

# Compiling on Windows

## 1\. Compiling with Mingw64 and MSYS2

Download Msys2, update everything and install the following
    packages:

    pacman -S base-devel mingw-w64-x86_64-toolchain mingw-w64-x86_64-libtool \
              mingw-w64-x86_64-boost

Open Mingw-w64 Win64 prompt and compile the same way as on Linux, see
above.

## 2\. Compiling in Cygwin environment

Download and install Cygwin environment for Windows with the following
extra packages:

  - make
  - automake
  - autoconf
  - libtool
  - gcc-g++ development package
  - boost
  - ncurses, readline (for user interface)
  - iconv (character conversion)

Then compile the same way as on Linux. Cygwin builds depend on
Cygwin1.dll.

# Debugging

It is recomended to install a debug build of the standard library:

    libstdc++6-6-dbg

For debugging we need to create a debug build and then we need to start
`gdb`.

    ./configure CXXFLAGS='-g -O0 -Wall -Wextra'
    make
    ./libtool --mode=execute gdb src/tools/nuspell

You can also pass the `CXXFLAGS` directly to `make` without calling
`./configure`, but we don't recommend this way during long development
sessions.

If you like to develop and debug with an IDE, see documentation at
https://github.com/hunspell/hunspell/wiki/IDE-Setup

# Testing

Testing Nuspell (see tests in tests/ subdirectory):

    make check

or with Valgrind debugger:

    make check
    VALGRIND=[Valgrind_tool] make check

For example:

    make check
    VALGRIND=memcheck make check

# Documentation

features and dictionary format:

    man 5 nuspell
    man nuspell
    nuspell -h

http://nuspell.org/

Documentation for developers can be generated from the source files by running:

    doxygen

The result can be viewed by opening `doxygen/html/index.html` in a web browser.
Doxygen will use Graphviz for generating all sorts of graphs and PlantUML
for generating UML diagrams. Make sure that the packages plantuml and graphviz are installed before running Doxygen. The latter is usually installed automatically
when installing Doxygen.

# Usage

The src/tools directory contains ten executables after compiling.

  - The main executable:
      - nuspell: main program for spell checking and others (see
        manual)
  - Example tools:
      - analyze: example of spell checking, stemming and morphological
        analysis
      - chmorph: example of automatic morphological generation and
        conversion
      - example: example of spell checking and suggestion
  - Tools for dictionary development:
      - affixcompress: dictionary generation from large (millions of
        words) vocabularies
      - makealias: alias compression (Nuspell only, not back compatible
        with MySpell)
      - wordforms: word generation (Nuspell version of unmunch)
      - ~~hunzip: decompressor of hzip format~~ (DEPRECATED)
      - ~~hzip: compressor of hzip format~~ (DEPRECATED)
      - munch (DEPRECATED, use affixcompress): dictionary generation
        from vocabularies (it needs an affix file, too).
      - unmunch (DEPRECATED, use wordforms): list all recognized words
        of a MySpell dictionary

After compiling and installing (see INSTALL) you can run the Nuspell
spell checker (compiled with user interface) with a Nuspell, Hunspell or Myspell
dictionary:

    nuspell -d en_US text.txt

or without interface:

    nuspell
    nuspell -d en_UK -l <text.txt

Dictionaries consist of an affix and dictionary file, see tests/ or
http://wiki.services.openoffice.org/wiki/Dictionaries.

# Using Nuspell library with GCC

Including in your program:

    #include <nuspell.hxx>

Linking with Nuspell static library:

    g++ -lnuspell-1.6 example.cxx
    # or better, use pkg-config
    g++ $(pkg-config --cflags --libs nuspell) example.cxx

## Dictionaries

Myspell, Hunspell and Nuspell dictionaries:

  - https://github.com/hunspell/hunspell/wiki/Dictionaries-and-Contacts
  - https://github.com/hunspell/hunspell/wiki/Dictionary-Packages
  - http://extensions.libreoffice.org
  - http://cgit.freedesktop.org/libreoffice/dictionaries
  - http://extensions.openoffice.org
  - http://wiki.services.openoffice.org/wiki/Dictionaries

Aspell dictionaries (need some conversion):

  - ftp://ftp.gnu.org/gnu/aspell/dict

Conversion steps: see relevant feature request at
http://nuspell.org/ .

László Németh, nemeth at numbertext org

