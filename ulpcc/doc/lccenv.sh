#!/bin/bash

# Change LCCGITDIR if you put the source somewhere else
LCCGITDIR=~/lcc

export BUILDDIR=$LCCGITDIR/build 
export ULPCCDIR=$LCCGITDIR/ulpcc
export LCCDIR=$BUILDDIR

function buildlcc {
    cd $LCCGITDIR
    make HOSTFILE=etc/linux.c all
}

function ulpcc {
    $BUILDDIR/lcc -I$IDF_PATH/components/soc/soc/esp32/include -I$IDF_PATH/components/soc/soc/esp32/include/soc -I$IDF_PATH/components/esp_common/include -I$ULPCCDIR/inc -Wf-target=ulp -S "$1" -o `basename "$1" .c`.S
}


function ulpccsym {
    $BUILDDIR/lcc -I$IDF_PATH/components/soc/soc/esp32/include -I$IDF_PATH/components/soc/soc/esp32/include/soc -I$IDF_PATH/components/esp_common/include -I$ULPCCDIR/inc -Wf-target=symbolic -S "$1" -o `basename "$1" .c`.sym
}

