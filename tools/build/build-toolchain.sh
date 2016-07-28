#!/usr/bin/env sh

BASEDIR=$(cd $(dirname $0)/../.. && pwd)
OUTDIR=$BASEDIR/out
OBJDIR=$OUTDIR/obj
BASEAVRDIR=$BASEDIR/tools/avr
NUMCORES=$(getconf _NPROCESSORS_ONLN)

mkdir -p $OBJDIR > /dev/null 2>&1

# binutils
echo "Building binutils..."
sleep 1
mkdir $OBJDIR/binutils > /dev/null 2>&1
cd $OBJDIR/binutils
$BASEAVRDIR/binutils-gdb/configure --target=avr --prefix=$OUTDIR/usr/local/avr --disable-nls --enable-install-libbfd && make -j$NUMCORES && make install
echo "Done"

# gcc
echo "Building gcc..."
sleep 1
mkdir $OBJDIR/gcc > /dev/null 2>&1
cd $OBJDIR/gcc
$BASEAVRDIR/gcc/configure --target=avr --prefix=$OUTDIR/usr/local/avr --disable-nls --enable-languages=c,c++ --disable-libssp && make -j$NUMCORES && make install
echo "Done"

# TODO: avr-libc, avrdude, gdb, simulavrxx?
# See: http://www.mbeckler.org/microcontrollers/avrgcc_build/
