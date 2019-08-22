#!/usr/bin/env bash

path-remove ()
{
    local IFS=':';
    local NEWPATH;
    local DIR;
    local PATHVARIABLE=${2:-PATH};
    for DIR in ${!PATHVARIABLE};
    do
        if [ "$DIR" != "$1" ]; then
            NEWPATH=${NEWPATH:+$NEWPATH:}$DIR;
        fi;
    done;
    export $PATHVARIABLE="$NEWPATH"
}

path-append ()
{
    path-remove "$1" "$2";
    local PATHVARIABLE="${2:-PATH}";
    export $PATHVARIABLE="${!PATHVARIABLE:+${!PATHVARIABLE}:}$1"
}

source /cvmfs/uboone.opensciencegrid.org/products/setup_uboone.sh
#setup uboonecode v08_00_00_16 -q e17:prof

echo "setup wcp"
product_dir=$(dirname "$(readlink -f ${BASH_SOURCE[0]})")
path-append $product_dir PRODUCTS
setup wcp v00_10_00 -q e17:prof

