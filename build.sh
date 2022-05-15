libdragon make -j
echo Disasm...
cd build_n64/
libdragon disasm > rom.asm
echo Done
cd ..