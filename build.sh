#!/bin/bash

set -e

OSDIST=`lsb_release -i |awk -F: '{print tolower($2)}' | tr -d ' \t'`
BUILDDIR=$(readlink -f $(dirname ${BASH_SOURCE[0]}))
CORE=`grep -c ^processor /proc/cpuinfo`
CMAKE=cmake

usage()
{
    echo "Usage: build.sh [releaseions]"
    echo
    echo "[-help]                    List this help"
    echo "[clean|-clean]             Remove build directories"
    echo "[-debug]                   Build debug library only (default)"
    echo "[-release]                 Build release library only (default)"
    echo "[-j <n>]                   Compile parallel (default: system cores)"
    echo "[-verbose]                 Turn on verbosity when compiling"

    exit 1
}

clean=0
verbose=""
jcore=$CORE
debug=1
release=1
while [ $# -gt 0 ]; do
    case "$1" in
        -help)
            usage
            ;;
        clean|-clean)
            clean=1
            shift
            ;;
        -debug)
            debug=1
            release=0
            shift
            ;;
        -release)
            debug=0
            release=1
            shift
            ;;
        -j)
            shift
            jcore=$1
            shift
            ;;
        -verbose)
            verbose="VERBOSE=1"
            shift
            ;;
        *)
            echo "unknown option"
            usage
            ;;
    esac
done

debug_dir=${DEBUG_DIR:-Debug}
release_dir=${REL_DIR:-Release}

here=$PWD
cd $BUILDDIR

if [[ $clean == 1 ]]; then
    echo $PWD
    echo "/bin/rm -rf $debug_dir $release_dir"
    /bin/rm -rf $debug_dir $release_dir
    exit 0
fi

if [[ $debug == 1 ]]; then
    mkdir -p $debug_dir
    cd $debug_dir
    $CMAKE -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=/opt/xilinx ..;
    echo "make -j $jcore $verbose DESTDIR=$PWD install"
    time make -j $jcore $verbose DESTDIR=$PWD install
    cd $BUILDDIR
fi

if [[ $release == 1 ]]; then
    mkdir -p $release_dir
    cd $release_dir
    $CMAKE -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/opt/xilinx ..;
    echo "make -j $jcore $verbose DESTDIR=$PWD install"
    time make -j $jcore $verbose DESTDIR=$PWD install
    time make package
    cd $BUILDDIR
fi

cd $here
