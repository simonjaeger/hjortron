REM @ECHO OFF
REM CMD /C bash -c "cd .. && make clean && make iso" || EXIT /b
REM CLS
REM CMD /C qemu-system-x86_64 -boot d -cdrom ../bin/os.iso -serial stdio