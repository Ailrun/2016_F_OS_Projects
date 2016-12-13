#!/bin/sh

if [ "$#" -eq 1 ]; then
    case $1 in
        0)
            ./build.sh tizen_tm1 USR 2> error.log
	    echo "Compile Error Lists:"
	    cat error.log | grep "error:";;
        1)
	    sudo ls
            scripts/mkmodimg.sh
            tar cf IMAGE.tar -C arch/arm/boot dzImage -C ../../../usr/tmp-mod modules.img ;;
        2)
            sudo lthor IMAGE.tar ;;
    esac
else
    echo "Usage: ./tb <build stage>"
fi
