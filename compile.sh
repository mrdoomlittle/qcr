C_IFLAGS="-Ierr/inc"
gcc -c -std=c11 $C_IFLAGS -o qcr.o qcr.c
gcc -c -std=c11 $C_IFLAGS -o vec.o vec.c
gcc -c -std=c11 $C_IFLAGS -o buff.o buff.c
gcc -std=c11 $C_IFLAGS -o main main.c qcr.o vec.o buff.c
