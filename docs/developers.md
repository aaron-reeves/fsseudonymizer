[Table of contents](_tableOfContents.md)  
Previous: [Using the application from the command line](commandline.md) | Next: [Customizing the application](customizing.md)

# _Fsseudonymizer:_ Information for developers

## Language and platform requirements

_Fsseudonymizer_ is written in C++ using the cross-platform development toolkit [Qt](https://www.qt.io/download-open-source).  _Fsseudonymizer_ is freely available as open source software, released under the terms of the [GNU General Public License (GPL) version 2](license.md).

Although _Fsseudonymizer_ is currently released only for the _Microsoft Windows_ platform, it should be possible to build on any other operating system supported by Qt.

## The source code

Complete source code for _Fsseudonymizer_ is available at <https://github.com/aaron-reeves/pseudonymizer>.

## Required toolkits and libraries

The following toolkits and libraries are required to build _Fsseudonymizer_:

  - [Qt](https://www.qt.io/download-qt-installer) version 5.x (5.12 or higher is recommended)
  - A suitable C++ compiler (we currently use the [MinGW](http://www.mingw.org) toolchain provided with Qt)
  - [libmagic](http://www.darwinsys.com/file/), a library for determining file types based on contents, as described [here](https://filemagic.readthedocs.io/en/latest/guide.html)
    - libmagic is included in virtually all Linux distributions
    - Several versions of libmagic for _Windows_ are available:
      - The 64-bit version of _Fsseudonymizer_ uses [libmagicwin64](https://github.com/aaron-reeves/libmagicwin64), which is based on <https://github.com/pidydx/libmagicwin64>.
      - The 32-bit version of _Fsseudonymizer_ uses [File for Windows version 5.03](http://gnuwin32.sourceforge.net/packages/file.htm).
  - libxls, an open-source library for reading _Microsoft Excel 97-2003_-formatted files. _Fsseudonymizer_ uses the version at <https://github.com/aaron-reeves/libxls>, which is based on <https://github.com/libxls/libxls>.
  - QtXlsxWriter, an open-source library for reading and writing _Microsoft Excel 2007_-formatted files.  _Fsseudonymizer_ uses the version at <https://github.com/aaron-reeves/QtXlsxWriter>, which is derived from an original version at <https://github.com/dbzhang800/QtXlsxWriter>.

## Building the application

The process of building _Fsseudonymizer_ is similar to the process for building any other Qt-based application.

Once all required libraries are installed and configured, building is a relatively simple matter of compiling the program, either from within [Qt Creator](https://www.qt.io/development-tools) or via ```qmake``` and ```make``` as described [here](https://wiki.qt.io/Getting_Started_on_the_Commandline).

It may be necessary to modify the project file ```Fsseudonymizer.pro``` to specify application options, including:

  - The locations of libraries and included files
  - Whether to compile the application as a 32-bit or 64-bit application
  - Whether to compile the application as a graphical or a command line application

## Acknowledgments

*We gratefully acknowledge the efforts of the many developers who have contributed to the open-source platforms, tools, and libraries used for the development of _Fsseudonymizer_.*

