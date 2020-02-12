racket yarnc.scm $1
python yarnasm.py out.asm
emu/yarnemu out.asm.bin    
