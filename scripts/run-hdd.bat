@ECHO OFF
REM CMD /C bash -c "cd .. && make clean && make bin2" || EXIT /b
REM CLS
CMD /C qemu-system-x86_64 -drive file=../bin/os512.bin,if=ide,format=raw -serial stdio