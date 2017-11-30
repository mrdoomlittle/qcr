C_IFLAGS="-Ierr/inc"
sh clean.sh
gcc -c -std=c11 $C_IFLAGS -o qcr.o qcr.c
gcc -c -std=c11 $C_IFLAGS -o vec.o vec.c
gcc -c -std=c11 $C_IFLAGS -o buff.o buff.c
gcc -c -std=c11 $C_IFLAGS -o map.o map.c
ar rcs lib/libmdl-qcr.a map.o qcr.o vec.o buff.o
cp qcr.h inc/mdl
gcc -Wall -std=c11 -Iinc -Llib $C_IFLAGS -o main main.c -lmdl-qcr
