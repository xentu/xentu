#!/bin/sh


# define some color constants.
Color_Off='\033[0m'       # Text Reset
Red='\033[0;91m'          # Red
Green='\033[0;92m'        # Green
Yellow='\033[0;93m'       # Yellow
White='\033[0;97m'        # White


# Make sure meson is available.
if ! command -v meson &> /dev/null; then
	echo -e "Error, meson is not available. Please install meson: ${White}python -m pip install meson${Color_Off}"
	exit
fi


# make sure the various folders are available.
mkdir -p ./build
mkdir -p ./subprojects


# Make sure the subprojects are loaded in using wrapdb.
meson wrap install zlib
meson wrap install libjpeg-turbo
meson wrap install libpng
meson wrap install freetype
meson wrap install lua
meson wrap install ogg
meson wrap install vorbis
meson wrap install sdl2_image
meson wrap install sdl2_mixer
meson wrap install sdl2_ttf
meson wrap install sdl2
meson wrap install glew


# ask meson to prepare the build
meson setup build


# All done
echo -e ""
echo -e "-------------------------------"
echo -e "Xentu Rebuild ready to compile!"