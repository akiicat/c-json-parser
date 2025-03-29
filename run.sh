#!/bin/bash

make clean
make debug_build
make run

rm -rf build
meson setup build --buildtype=debug -Db_coverage=true
meson compile -C build
meson test -C build
ninja coverage -C build

# push build
# gcovr -r .. --decisions --calls --html --html-details --html-block-ids --html-theme github.blue -o coverage.html
# pop
