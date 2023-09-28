
gcc -DUSE_ASM_INLINE -DUSE_SCALE2X_SSE2 -c scale2k.c
gcc -DUSE_ASM_INLINE -DUSE_SCALE2X_SSE2 -c scale3k.c
gcc -DUSE_ASM_INLINE -DUSE_SCALE2X_SSE2 -c scale4k.c
gcc -DUSE_ASM_INLINE -DUSE_SCALE2X_SSE2 -c interp.c
gcc -DUSE_ASM_INLINE -DUSE_SCALE2X_SSE2 -c scalebit.c
gcc -DUSE_ASM_INLINE -DUSE_SCALE2X_SSE2 -c rgb.c
gcc -shared -o scale2k.dll scale2k.o scale3k.o scale4k.o interp.o scalebit.o rgb.o -Wl,--out-implib,scale2k.dll
move scale2k.dll ../Release
