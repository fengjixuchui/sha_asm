bits 64
section .text

extern print_uint32

; SHA1 transform in assembly for amd64
; rdi: state
; rsi: data block

; r8-r9-r10-r11-r12 are used for a-b-c-d-e

global sha1_transform_amd64
sha1_transform_amd64:
push r13
push r12
push rbx
; store the state buffer on the stack
sub rsp, 320

; move the state into registers
mov r8d, dword [rdi]
mov r9d, dword [rdi + 4]
mov r10d, dword [rdi + 8]
mov r11d, dword [rdi + 12]
mov r12d, dword [rdi + 16]

mov rcx, 0
.next_round:

cmp rcx, 16
jl .first_16

; last_56
mov eax, dword [rsp + (rcx - 3) * 4]
mov ebx, dword [rsp + (rcx - 8) * 4]
xor eax, ebx
mov ebx, dword [rsp + (rcx - 14) * 4]
xor eax, ebx
mov ebx, dword [rsp + (rcx - 16) * 4]
xor eax, ebx
rol eax, 1
jmp .stor_state

.first_16:
mov eax, dword [rsi + rcx * 4]
bswap eax

.stor_state:
mov dword [rsp + rcx * 4], eax

; now eax has the current round state
cmp rcx, 20
jge .1
; less than 20
mov ebx, 0x5a827999

mov edx, r9d
and edx, r10d ; b & c

mov r13d, r9d ; ~b & d
not r13d
and r13d, r11d

xor edx, r13d ; ^

jmp .end
.1:
cmp rcx, 40
jge .2
; less than 40
mov ebx, 0x6ed9eba1

mov edx, r9d
xor edx, r10d
xor edx, r11d ; b ^ c ^ d

jmp .end
.2:
cmp rcx, 60
jge .3
; less than 60
mov ebx, 0x8f1bbcdc

mov edx, r9d
and edx, r10d ; b & c

mov r13d, r9d
and r13d, r11d ; b & d
xor edx, r13d

mov r13d, r10d
and r13d, r11d ; c & d
xor edx, r13d

jmp .end
.3:
; less than 80
mov ebx, 0xca62c1d6
mov edx, r9d
xor edx, r10d
xor edx, r11d

.end:

; edx = f
; ebx = k

mov r13d, r8d
rol r13d, 5 ; t = rotl32(a, 5)
add r13d, edx ; + f
add r13d, r12d ; + e
add r13d, ebx ; + k
add r13d, eax ; + state[i]

mov r12d, r11d ; e = d
mov r11d, r10d ; d = c
mov r10d, r9d
rol r10d, 30   ; c = rotl32(b, 30)
mov r9d, r8d
mov r8d, r13d

inc rcx
cmp rcx, 80
jl .next_round

; save the state back into the array
add dword [rdi], r8d ; a
add dword [rdi + 4], r9d ; b
add dword [rdi + 8], r10d ; c
add dword [rdi + 12], r11d ; d
add dword [rdi + 16], r12d ; e

add rsp, 320
pop rbx
pop r12
pop r13
ret
