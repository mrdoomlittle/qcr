C_IFLAGS="-Ierr/inc"
gcc -c -std=c11 $C_IFLAGS -o qcr.o qcr.c
gcc -c -std=c11 $C_IFLAGS -o vec.o vec.c
gcc -c -std=c11 $C_IFLAGS -o buff.o buff.c
gcc -c -std=c11 $C_IFLAGS -o map.o map.c
gcc -std=c11 $C_IFLAGS -o main main.c map.o qcr.o vec.o buff.c
