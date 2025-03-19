#!/bin/bash

make clean
make debug_build
make run

rm -rf builddir
meson setup builddir --buildtype=debug -Db_coverage=true
meson compile -C builddir
meson test -C builddir
ninja coverage -C builddir
