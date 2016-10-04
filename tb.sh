#!/bin/sh

T_DIR="test"
T_FILE="test"

if [ "$#" -eq 1 ]; then
    case $1 in
        0)
            ./build.sh tizen_tm1 USR ;;
        1)
            scripts/mkmodimg.sh
            tar cf IMAGE.tar -C arch/arm/boot dzImage -C ../../../usr/tmp-mod modules.img ;;
        2)
            sudo lthor IMAGE.tar ;;
        3)
            make -C $T_DIR
            sdb push $T_DIR/$T_FILE /home/developer/$T_FILE
            sdb shell "cd ~; ./$T_FILE"
    esac
else
    echo "Usage: ./tb <build stage>"
fi
