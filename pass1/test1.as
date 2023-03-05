MAIN: mov S1 , LENGTH

mov S1 , r3
bne LOOP
add r2 , STR
prn #-5
sub r1 , r4
inc K
mov S1 , r3
K: .data 22
bne LOOP
LENGTH: .data 6 , -9 , 15
j: bne LOOP( r2, r7 )
STR: .string "abcdef"




