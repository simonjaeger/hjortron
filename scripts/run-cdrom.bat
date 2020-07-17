@ECHO OFF
CMD /C bash -c "cd .. && make clean && make iso" || EXIT /b
CLS
CMD /C qemu-system-x86_64 -boot d -cdrom ../bin/os.iso -serial stdio