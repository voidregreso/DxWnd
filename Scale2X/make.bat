
gcc -DUSE_ASM_INLINE -DUSE_SCALE2X_SSE2 -c scale2x.c
gcc -DUSE_ASM_INLINE -DUSE_SCALE2X_SSE2 -c scale3x.c
gcc -DUSE_ASM_INLINE -DUSE_SCALE2X_SSE2 -c scalebit.c
gcc -shared -o scale2x.dll scale2x.o scale3x.o scalebit.o -Wl,--out-implib,scale2x.dll
move scale2x.dll ../Release
