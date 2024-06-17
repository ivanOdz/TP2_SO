#!/bin/bash
#./compile.sh 


#sudo chown tagc2002 Image/x64BareBonesImage.qcow2
HOST_IP=tcp:$(grep nameserver /etc/resolv.conf | awk '{print $2}');
echo $HOST_IP
DEBUG=""
SOUND=""
COMPILE=""
SNAPSHOT=""

for var in "$@"
do
    case $var in 
        -d) 
            DEBUG="-s -S"
            echo "Debug"
            ;;
        -r) 
            SNAPSHOT="-icount shift=1,sleep=on,rr=record,rrfile=replay.bin -net none"
            echo "Record"
            ;;
        -p) 
            SNAPSHOT="-icount shift=1,sleep=on,rr=replay,rrfile=replay.bin -net none"
            echo "Play"
            ;;
        -s)
            SOUND="-audiodev pa,id=snd0,server=$HOST_IP -machine pcspk-audiodev=snd0"
            echo "Sound"
            ;;
        -c)
            COMPILE=TRUE
            echo "Compile"
            ;;
    esac
done

if [ -n "$COMPILE" ]
then
    make clean
    make
fi


#Si corro el programa con el -d es porque estoy queriendo debuggear
echo qemu-system-x86_64 $DEBUG $SNAPSHOT -m 512 -drive file=Image/x64BareBonesImage.qcow2,index=0,media=disk,if=none,snapshot=on,id=img-direct -drive driver=blkreplay,if=none,image=img-direct,id=img-blkreplay -device ide-hd,drive=img-blkreplay $SOUND
qemu-system-x86_64 $DEBUG $SNAPSHOT -m 512 -drive file=Image/x64BareBonesImage.qcow2,index=0,media=disk,if=none,snapshot=on,id=img-direct -drive driver=blkreplay,if=none,image=img-direct,id=img-blkreplay -device ide-hd,drive=img-blkreplay $SOUND
