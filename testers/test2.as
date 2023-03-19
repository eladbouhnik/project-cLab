MAIN: mov r3 ,LENGTH
LOOP: jmp L1(#-1,r6)
prn #-5
bne LOOP(r4,r5)
sub r1, r4
bne END
L1: inc LENGTH
bne LOOP(LENGTH,r3) 
bne LOOP

END: stop
 .string "abcdef"
LENGTH: .data 6,-9,15
.data 22
