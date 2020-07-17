@ECHO OFF
CMD /C bash -c "cd .. && make clean && make bin" || EXIT /b
CLS
CMD /C qemu-system-x86_64 -fda ../bin/os512.bin -serial stdio