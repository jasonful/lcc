#!/bin/sh

lcc -Wo-lccdir=$ULPCCDIR/bin/darwin -I$IDF_PATH/components/soc/esp32/include -I$IDF_PATH/components/soc/esp32/include/soc -I$ULPCCDIR/inc -Wf-target=ulp -S $1 -o $2
