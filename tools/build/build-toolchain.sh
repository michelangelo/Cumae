#!/usr/bin/env sh

BASEDIR=$(cd $(dirname $0)/../.. && pwd)
OUTDIR=$BASEDIR/out
OBJDIR=$OUTDIR/obj
BASEAVRDIR=$BASEDIR/tools/avr
NUMCORES=$(getconf _NPROCESSORS_ONLN)
AVRDUDE_VERSION=6.3

mkdir -p $OBJDIR > /dev/null 2>&1

# binutils
build_binutils() {
    echo "Building binutils..."
    sleep 1
    mkdir $OBJDIR/binutils > /dev/null 2>&1
    cd $OBJDIR/binutils
    $BASEAVRDIR/binutils-gdb/configure --target=avr --prefix=$OUTDIR/usr/local/avr --disable-nls --enable-install-libbfd && make -j$NUMCORES && make install
}

# gcc
build_gcc() {
    echo "Building gcc..."
    sleep 1
    mkdir $OBJDIR/gcc > /dev/null 2>&1
    cd $OBJDIR/gcc
    $BASEAVRDIR/gcc/configure --target=avr --prefix=$OUTDIR/usr/local/avr --disable-nls --enable-languages=c,c++ --disable-libssp && make -j$NUMCORES && make install
}

# avrdude (plain download)
build_avrdude() {
    echo "Building avrdude..."
    sleep 1
    mkdir $OBJDIR/avrdude > /dev/null 2>&1 # We use the OBJ dir for the temp file,
                                           # not optimal, I know. ;)
    cd $OBJDIR/avrdude
    wget http://download.savannah.gnu.org/releases/avrdude/avrdude-$AVRDUDE_VERSION.tar.gz
    tar xfz avrdude-$AVRDUDE_VERSION.tar.gz
    cd avrdude-$AVRDUDE_VERSION
    ./configure --prefix=$OUTDIR/usr/local/avr && make -j$NUMCORES && make install
}

# avr-libc
build_avrlibc() {
    echo "Building avr-libc"
    sleep 1
    cd $BASEDIR/external/avr-libc-2.0.0
    export PATH=$OUTDIR/usr/local/avr/bin:$PATH
    export CC=avr-gcc
    ./configure --build=`./config.guess` --host=avr --prefix=$OUTDIR/usr/local/avr && make && make install
}

build_binutils && build_gcc && build_avrdude && build_avrlibc && echo "*** ALL DONE!"

# TODO: avr-libc, simulavrxx?
# See: http://www.mbeckler.org/microcontrollers/avrgcc_build/
