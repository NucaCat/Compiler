.model small 
INCLUDE dat.asm 
.stack 100h 
.code 
.386 
start: 
mov ax, @data 
mov   ds, ax 
; LET() 
mov   di, 0 
push  di 
mov ax, 0
push ax 
pop   ax 
pop   di 
mov   k[di], ax 
; DO() 
@28: 
; WRITE() 
; WRITE LITERAL 
lea   dx, @30p 
mov   ah, 9 
int   21h 
lea   dx, clrf 
mov   ah, 9 
int   21h 
mov di, 0 
mov   ax,k[di] 
push ax 
; WRITE ARIPHMETICS 
pop   ax 
mov   bx, 10 
mov   di, 0 
mov   si, ax 
cmp   ax, 0 
jns @33
neg   si 
mov   ah, 2 
mov   dl, "-" 
int   21h 
mov   ax, si 
@33: 
mov   dx, 0 
div   bx 
add   dl, 30h 
mov   output[di], dl 
inc   di 
cmp   al, 0 
jnz @33 
mov   cx, di 
dec   di 
mov   ah, 2 
@34: 
 mov   dl, output[di] 
dec   di 
int   21h 
loop @34 
lea   dx, clrf 
mov   ah, 9 
int   21h 
; LET() 
mov   di, 0 
push  di 
mov di, 0 
mov   ax,k[di] 
push ax 
mov ax, 1
push ax 
pop bx 
pop ax 
add ax, bx 
push ax 
pop   ax 
pop   di 
mov   k[di], ax 
mov di, 0 
mov   ax,k[di] 
push ax 
mov ax, 5
push ax 
pop bx 
pop ax 
cmp ax, bx 
jne @35
push 0 
jmp @36
@35: 
push 1 
@36: 
pop   ax 
cmp   ax, 0 
jz @29
jmp @28
@29: 
; WRITE() 
; WRITE LITERAL 
lea   dx, @37p 
mov   ah, 9 
int   21h 
lea   dx, clrf 
mov   ah, 9 
int   21h 
; READ() 
push  0 
; READ BOOLEAN 
@38: 
     mov   ah, 0Ah 
     lea   dx, @buffer 
     int   21h 
     cmp   blength, 4 
     je    @41 
     cmp   blength, 5 
     je    @41 
     jmp   @45 
@41: 
     cmp   @buf[0], "t" 
     je    @42 
     push  0 
     jmp   @44 
@42: 
     push  1 
     jmp   @44 
@45: 
     lea   dx, err_msg 
     mov   ah, 9 
     int   21h 
     jmp   @38 
@44: 
     pop   ax 
     pop   di 
     mov  l[di], al 
; IF() 
mov di, 0 
xor ah, ah 
mov al, l[di] 
push ax 
pop   ax 
cmp   ax, 0 
jz @46
; IF THEN 
; WRITE() 
; WRITE LITERAL 
lea   dx, @48p 
mov   ah, 9 
int   21h 
lea   dx, clrf 
mov   ah, 9 
int   21h 
jmp @47
@46: 
; IF ELSE 
; WRITE() 
; WRITE LITERAL 
lea   dx, @49p 
mov   ah, 9 
int   21h 
lea   dx, clrf 
mov   ah, 9 
int   21h 
@47: 
; WRITE() 
; WRITE LITERAL 
lea   dx, @50p 
mov   ah, 9 
int   21h 
lea   dx, clrf 
mov   ah, 9 
int   21h 
; READ() 
push  0 
; READ CHAR 
     mov   ah, 0Ah 
     lea   dx, @buffer 
     int   21h 
     xor   dx, dx 
     mov   dl, @buf[0] 
     pop   di 
     mov  h[di], dl 
push  0 
mov ax, 1
push ax 
; READ INTEGER 
@53: 
     mov   ah, 0Ah 
     lea   dx, @buffer 
     int   21h 
     mov   ax, 0 
     mov   cx, 0 
     mov   cl, byte ptr[blength] 
     mov   bx, cx 
@51: 
     dec   bx 
     mov   al, @buf[bx] 
     cmp   al, "9" 
     ja    @52 
     cmp   al, "0" 
     jb    @52 
     loop  @51 
     mov   cl, byte ptr[blength] 
     mov   di, 0 
     mov   ax, 0 
@55: 
     mov   bl, @buf[di] 
     inc   di 
     sub   bl, 30h 
     add   ax, bx 
     mov   si, ax 
     mov   bx, 10 
     mul   bx 
     loop  @55 
     mov   ax, si 
     pop   di 
     shl   di, 1 
     mov  z[di],ax 
     jmp   @54 
@52: 
     lea   dx, err_msg 
     mov   ah, 9 
     int   21h 
     jmp   @53 
@54: 
; WRITE() 
mov di, 0 
mov ax, 1
push ax 
pop di 
shl di, 1 
mov   ax,z[di] 
push ax 
; WRITE ARIPHMETICS 
pop   ax 
mov   bx, 10 
mov   di, 0 
mov   si, ax 
cmp   ax, 0 
jns @58
neg   si 
mov   ah, 2 
mov   dl, "-" 
int   21h 
mov   ax, si 
@58: 
mov   dx, 0 
div   bx 
add   dl, 30h 
mov   output[di], dl 
inc   di 
cmp   al, 0 
jnz @58 
mov   cx, di 
dec   di 
mov   ah, 2 
@59: 
 mov   dl, output[di] 
dec   di 
int   21h 
loop @59 
lea   dx, clrf 
mov   ah, 9 
int   21h 
     mov   di, 0 
; WRITE CHAR 
     mov   ax, 0 
     mov   al, h[di] 
     mov   dl, al 
     mov   ah, 2 
     int   21h 
lea   dx, clrf 
mov   ah, 9 
int   21h 
; LET() 
mov   di, 0 
push  di 
mov ax, 5
push ax 
pop   ax 
pop   di 
mov   p1[di], ax 
; LET() 
mov   di, 0 
push  di 
mov ax, 8
push ax 
pop   ax 
pop   di 
mov   p2[di], ax 
; WHILE() 
@60: 
mov di, 0 
mov   ax,p1[di] 
push ax 
mov di, 0 
mov   ax,p2[di] 
push ax 
pop bx 
pop ax 
cmp ax, bx 
jb @61
push 0 
jmp @62
@61: 
push 1 
@62: 
pop   ax 
cmp   ax, 0 
jz @63
; WRITE() 
mov di, 0 
mov   ax,p1[di] 
push ax 
; WRITE ARIPHMETICS 
pop   ax 
mov   bx, 10 
mov   di, 0 
mov   si, ax 
cmp   ax, 0 
jns @66
neg   si 
mov   ah, 2 
mov   dl, "-" 
int   21h 
mov   ax, si 
@66: 
mov   dx, 0 
div   bx 
add   dl, 30h 
mov   output[di], dl 
inc   di 
cmp   al, 0 
jnz @66 
mov   cx, di 
dec   di 
mov   ah, 2 
@67: 
 mov   dl, output[di] 
dec   di 
int   21h 
loop @67 
lea   dx, clrf 
mov   ah, 9 
int   21h 
; LET() 
mov   di, 0 
push  di 
mov di, 0 
mov   ax,p1[di] 
push ax 
mov ax, 1
push ax 
pop bx 
pop ax 
add ax, bx 
push ax 
pop   ax 
pop   di 
mov   p1[di], ax 
jmp @60
@63: 
; LET() 
mov   di, 0 
push  di 
mov ax, 4
push ax 
pop   ax 
pop   di 
mov   x[di], ax 
; IF() 
mov di, 0 
mov   ax,p1[di] 
push ax 
mov di, 0 
mov   ax,p2[di] 
push ax 
pop bx 
pop ax 
cmp ax, bx 
je @68
push 0 
jmp @69
@68: 
push 1 
@69: 
mov di, 0 
mov   ax,x[di] 
push ax 
mov ax, 4
push ax 
pop bx 
pop ax 
cmp ax, bx 
je @70
push 0 
jmp @71
@70: 
push 1 
@71: 
pop bx 
pop ax 
and ax, bx 
push ax 
pop   ax 
cmp   ax, 0 
jz @72
; IF THEN 
; WRITE() 
; WRITE LITERAL 
lea   dx, @74p 
mov   ah, 9 
int   21h 
lea   dx, clrf 
mov   ah, 9 
int   21h 
jmp @73
@72: 
; IF ELSE 
; WRITE() 
; WRITE LITERAL 
lea   dx, @75p 
mov   ah, 9 
int   21h 
lea   dx, clrf 
mov   ah, 9 
int   21h 
@73: 
; IF() 
mov di, 0 
mov   ax,x[di] 
push ax 
mov ax, 5
push ax 
pop bx 
pop ax 
cmp ax, bx 
je @76
push 0 
jmp @77
@76: 
push 1 
@77: 
pop ax 
not ax 
push ax 
pop   ax 
cmp   ax, 0 
jz @78
; IF THEN 
; WRITE() 
; WRITE LITERAL 
lea   dx, @80p 
mov   ah, 9 
int   21h 
lea   dx, clrf 
mov   ah, 9 
int   21h 
jmp @79
@78: 
@79: 
; IF() 
mov di, 0 
mov   ax,x[di] 
push ax 
mov ax, 5
push ax 
pop bx 
pop ax 
cmp ax, bx 
je @81
push 0 
jmp @82
@81: 
push 1 
@82: 
mov di, 0 
mov   ax,x[di] 
push ax 
mov ax, 0
push ax 
pop bx 
pop ax 
cmp ax, bx 
ja @83
push 0 
jmp @84
@83: 
push 1 
@84: 
pop bx 
pop ax 
or ax, bx 
push ax 
pop   ax 
cmp   ax, 0 
jz @85
; IF THEN 
; WRITE() 
; WRITE LITERAL 
lea   dx, @87p 
mov   ah, 9 
int   21h 
lea   dx, clrf 
mov   ah, 9 
int   21h 
jmp @86
@85: 
@86: 
error: 
mov ax, 4C00h 
int 21h 
end start 
