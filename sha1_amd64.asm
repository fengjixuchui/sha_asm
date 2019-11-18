bits 64
section .text

%macro round_start_16 1
mov eax, dword [rsi + %1 * 4]
bswap eax
mov dword [rsp + %1 * 4], eax
%endmacro

%macro round_start 1
mov eax, dword [rsp + (%1 - 3) * 4]
mov ebx, dword [rsp + (%1 - 8) * 4]
xor eax, ebx
mov ebx, dword [rsp + (%1 - 14) * 4]
xor eax, ebx
mov ebx, dword [rsp + (%1 - 16) * 4]
xor eax, ebx
rol eax, 1
mov dword [rsp + %1 * 4], eax
%endmacro

%macro round_end 1
; edx = f
mov r13d, r8d
rol r13d, 5 ; t = rotl32(a, 5)
add r13d, edx ; + f
add r13d, r12d ; + e
add r13d, %1 ; + k
add r13d, eax ; + state[i]
mov r12d, r11d ; e = d
mov r11d, r10d ; d = c
mov r10d, r9d
rol r10d, 30   ; c = rotl32(b, 30)
mov r9d, r8d
mov r8d, r13d
%endmacro

%macro T1 0
mov edx, r10d
xor edx, r11d
and edx, r9d
xor edx, r11d
round_end 0x5a827999
%endmacro

%macro T2 0
mov edx, r9d
xor edx, r10d
xor edx, r11d
round_end 0x6ed9eba1
%endmacro

%macro T3 0
mov edx, r11d
xor edx, r9d
and edx, r10d
mov r13d, r9d
and r13d, r11d
xor edx, r13d
round_end 0x8f1bbcdc
%endmacro

%macro T4 0
mov edx, r9d
xor edx, r10d
xor edx, r11d
round_end 0xca62c1d6
%endmacro

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

round_start_16 0
T1
round_start_16 1
T1
round_start_16 2
T1
round_start_16 3
T1
round_start_16 4
T1
round_start_16 5
T1
round_start_16 6
T1
round_start_16 7
T1
round_start_16 8
T1
round_start_16 9
T1
round_start_16 10
T1
round_start_16 11
T1
round_start_16 12
T1
round_start_16 13
T1
round_start_16 14
T1
round_start_16 15
T1

round_start 16
T1
round_start 17
T1
round_start 18
T1
round_start 19
T1

round_start 20
T2
round_start 21
T2
round_start 22
T2
round_start 23
T2
round_start 24
T2
round_start 25
T2
round_start 26
T2
round_start 27
T2
round_start 28
T2
round_start 29
T2
round_start 30
T2
round_start 31
T2
round_start 32
T2
round_start 33
T2
round_start 34
T2
round_start 35
T2
round_start 36
T2
round_start 37
T2
round_start 38
T2
round_start 39
T2


round_start 40
T3
round_start 41
T3
round_start 42
T3
round_start 43
T3
round_start 44
T3
round_start 45
T3
round_start 46
T3
round_start 47
T3
round_start 48
T3
round_start 49
T3
round_start 50
T3
round_start 51
T3
round_start 52
T3
round_start 53
T3
round_start 54
T3
round_start 55
T3
round_start 56
T3
round_start 57
T3
round_start 58
T3
round_start 59
T3


round_start 60
T4
round_start 61
T4
round_start 62
T4
round_start 63
T4
round_start 64
T4
round_start 65
T4
round_start 66
T4
round_start 67
T4
round_start 68
T4
round_start 69
T4
round_start 70
T4
round_start 71
T4
round_start 72
T4
round_start 73
T4
round_start 74
T4
round_start 75
T4
round_start 76
T4
round_start 77
T4
round_start 78
T4
round_start 79
T4

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
