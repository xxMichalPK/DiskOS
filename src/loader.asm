[BITS 32]
[global loader]
[extern kMain]

loader:
    call kMain
    jmp $