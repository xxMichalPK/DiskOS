[bits 32]
[global st3Loader]
[extern st3_main]

st3Loader:
    call st3_main
    jmp $