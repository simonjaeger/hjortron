@ECHO OFF
CMD /C bash -c "cd .. && make clean && make bin" || EXIT /b
CLS
CMD /C qemu-system-x86_64 -drive file=../bin/os512.bin,if=ide,format=raw -serial stdio