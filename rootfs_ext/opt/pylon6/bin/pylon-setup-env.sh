#!/bin/sh
# this script sets up the environment to
#   - build pylon applications (for samples only when pylon is not installed in the standard directory /opt/pylon) or
#   - run pylon applications using GenTL Producers
#
# use dot sourcing when calling this script
# note that there is no exit in error cases, to not destroy the outer shell


if [ $# != 1 ] ; then
    echo "Usage:"
    echo "  source pylon-setup-env.sh <path to pylon install dir>"
    echo ""
    echo "     By sourcing this script, the current environment is modified to"
    echo "     be able to run and build pylon applications."
else
    # we can't deduce the pylon root from $0 or $BASH_SOURCE because it is not available on busybox.
    NEW_PYLON_ROOT=`readlink -f "$1"`

    if [ ! -d "$NEW_PYLON_ROOT" ]; then
        echo "Error: The directory '$NEW_PYLON_ROOT' (PYLON_ROOT) does not exist" 1>&2
    else
        if [ -n "$PYLON_ROOT" -a "$PYLON_ROOT" != "$NEW_PYLON_ROOT" ]; then
            echo "Notice: PYLON_ROOT was already set. It got replaced with '$NEW_PYLON_ROOT'" 1>&2
        fi

        # to build pylon applications, PYLON_ROOT has to be set
        export PYLON_ROOT=$NEW_PYLON_ROOT

        # set default path for GenTL Producers.
        gentlpath=$PYLON_ROOT/lib/gentlproducer/gtl
        # determine arch for GenTL.
        ARCH=`uname -m`
        case "$ARCH" in
            x86_64)
                systembits=64
                # on linux 64 CXP is supported too.
                gentlpath=$gentlpath:$PYLON_ROOT/lib/pylonCXP/bin
                ;;
            aarch64)
                systembits=64
                ;;
            *)
                systembits=32
                ;;
        esac

        # export GENICAM_GENTL path according to bitness
        if [ "$systembits" = "64" ]; then
            if [ -n "$GENICAM_GENTL64_PATH" -a "$GENICAM_GENTL64_PATH" != "$gentlpath" ]; then
                echo "Notice: GENICAM_GENTL64_PATH was already set. It got replaced with '$gentlpath'" 1>&2
            fi
            export GENICAM_GENTL64_PATH="$gentlpath"
        else
            if [ -n "$GENICAM_GENTL32_PATH" -a "$GENICAM_GENTL32_PATH" != "$gentlpath" ]; then
                echo "Notice: GENICAM_GENTL32_PATH was already set. It got extended with '$gentlpath'" 1>&2
            fi
            export GENICAM_GENTL32_PATH="$gentlpath"
        fi
    fi
fi
