@ECHO OFF
CMD /C bash -c "cd .. && make clean && make fdd" || EXIT /b
CLS
CMD /C qemu-system-x86_64 -fda ../bin/fdd.bin -serial stdio