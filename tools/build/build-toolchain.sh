#!/usr/bin/env sh

BASEDIR=$(cd $(dirname $0)/../.. && pwd)
OUTDIR=$BASEDIR/out
OBJDIR=$OUTDIR/obj
BASEAVRDIR=$BASEDIR/tools/avr

mkdir -p $OBJDIR > /dev/null 2>&1

# binutils
echo "Building binutils..."
sleep 1
mkdir $OBJDIR/binutils > /dev/null 2>&1
cd $OBJDIR/binutils
$BASEAVRDIR/binutils-gdb/configure --target=avr --prefix=$OUTDIR/usr/local/avr --disable-nls --enable-install-libbfd && make && make install
echo "Done"

# TODO: gcc, avr-libc, avrdude, gdb, simulavrxx?
# See: http://www.mbeckler.org/microcontrollers/avrgcc_build/
