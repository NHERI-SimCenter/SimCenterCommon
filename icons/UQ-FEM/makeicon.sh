#!/bin/bash

if [ $# != 1 ]
then
   echo "usage: ./makeicon.sh MARSTER_ICON_NAME"
   exit 1
fi

BASENAME=${1%.*}

ICONDIR=${BASENAME}.iconset

if [ -d $ICONDIR ]
then
   echo "$ICONDIR already exists. ABORT"
   exit 2 
fi

# create the folder to hold the image stack

mkdir $ICONDIR

# create the scaled image stack

convert $1 -scale 1024x1024 $ICONDIR/icon_512x512@2x.png
convert $1 -scale 512x512 $ICONDIR/icon_512x512.png
convert $1 -scale 512x512 $ICONDIR/icon_256x256@2x.png
convert $1 -scale 256x256 $ICONDIR/icon_256x256.png
convert $1 -scale 256x256 $ICONDIR/icon_128x128@2x.png
convert $1 -scale 128x128 $ICONDIR/icon_128x128.png
convert $1 -scale 128x128 $ICONDIR/icon_64x64@2x.png
convert $1 -scale 64x64 $ICONDIR/icon_64x64.png
convert $1 -scale 64x64 $ICONDIR/icon_32x32@2x.png
convert $1 -scale 32x32 $ICONDIR/icon_32x32.png
convert $1 -scale 32x32 $ICONDIR/icon_16x16@2x.png
convert $1 -scale 16x16 $ICONDIR/icon_16x16.png

# create the mac icon

iconutil -c icns -o ${BASENAME}.icns $ICONDIR

if [ -f ${BASENAME}.icns ]
then
   exit 0
else
   echo "failed to create mac icns file"
   exit 3
fi



