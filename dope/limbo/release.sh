#!/bin/bash

#configuration
BUILDDIR=/tmp
PREFIX=/tmp/usr
export CXX=g++-3.2
export CXXFLAGS="-Wall -ansi -pedantic -Wno-long-long -Os -DNDEBUG"
PACKAGE=dope
SETUPCROSS=~/develop/cross/config
DOPE_CROSS_CONFIGURE_OPTIONS="--with-sigc-prefix=/home/jens/develop/cross --with-xml-prefix=/home/jens/develop/cross --prefix=/home/jens/develop/cross --disable-shared"
#end of configuration

set -e

cd $BUILDDIR

#check that $PACKAGE directory does not already exist
if test -e $PACKAGE; then
	echo Warning $PACKAGE directory already exists - abort or I will remove it
	read
	rm -rf $PACKAGE
fi

#get source
cvs -d:pserver:anonymous:@cvs.$PACKAGE.berlios.de:/cvsroot/$PACKAGE login
cvs -z3 -d:pserver:anonymous:@cvs.$PACKAGE.berlios.de:/cvsroot/$PACKAGE co $PACKAGE

#build distribution tarball
cd $PACKAGE
./bootstrap.sh
./configure
make distcheck
DISTFILE=`ls -1 $PACKAGE-*.tar.gz`
DIR=${DISTFILE%.tar.gz}
if [ "x$DIR" = "x" ]; then
    echo Fatal error
    exit 1
fi
cd $BUILDDIR

if test -e $DIR; then
	echo Warning $DIR directory already exists - abort or I will remove it
	read
	rm -rf $DIR
fi
tar xzvf $PACKAGE/$DISTFILE

#build in seperate directory
mkdir -f build
cd build

#native arch
mkdir -f arch-native
cd arch-native
if test -e $DIR; then
	echo Warning $DIR directory already exists - abort or I will remove it
	read
	rm -rf $DIR
fi
cd $DIR
$BUILDDIR/$DIR/configure --disable-shared --prefix=$PREFIX
make install

#cross compile
if test -e $SETUPCROSS; then
    source $SETUPCROSS
    cd $BUILDDIR/build
    mkdir -f arch-cross
    cd arch-cross
    if test -e $DIR; then
	echo Warning $DIR directory already exists - abort or I will remove it
	read
	rm -rf $DIR
    fi
    cd $DIR
    $BUILDDIR/$DIR/configure $CROSS_CONFIGURE_OPTIONS $DOPE_CROSS_CONFIGURE_OPTIONS --prefix=$PREFIX/cross
    make install
else
    echo cross compiler config script not found
fi
