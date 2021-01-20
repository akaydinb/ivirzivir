.286
.model small
.code
 org 100h
 

 
basla:          mov     si,offset metin1
                call    printtext
                push    cs
                pop     ax
                call    printregister
                mov     si,offset metin2
                call    printtext
                call    nextline
nextline:       pop     ax
                call    printregister
                call    prcrlf
                
                push    ds
                xor     ax,ax
                mov     bx,4
                mov     ds,ax
                mov     ax,ds:[bx]
                mov     oldvectoroffs,ax
                mov     ax,ds:[bx+02]
                mov     oldvectorseg,ax
                cli
                push    cs
                pop     ax
                mov     ds:[bx+02],ax
                mov     ax,offset debugr
                mov     ds:[bx],ax
                pop     ds
                sti
                call    printvectt
                
                pushf
                mov     bp,sp
                or      word ptr [bp], 0100h
                popf
                
                nop
                inc     di
                mov     ax,0123h

                pushf
                mov     bp,sp
                and     word ptr [bp], 0FEFFh
                popf
                
                xor     ax,ax
                int     20h

alterint01      equ     this word
newint01:       cli
                push    ax
                mov     ax,0E2Eh
                int     10h
                pop     ax
                sti
                iret

debugr:         cli
                push    bp
                mov     bp,sp
                pusha
                push    cs
                pop     ds
                push    si
                mov     si,offset RAX
                call    printtext
                call    printregister
                mov     si,offset RBX
                call    printtext
                mov     ax,bx
                call    printregister
                mov     si,offset RCX
                call    printtext
                mov     ax,cx
                call    printregister
                mov     si,offset RDX
                call    printtext
                mov     ax,dx
                call    printregister
                mov     si,offset RSP
                call    printtext
                mov     ax,sp
                call    printregister
                mov     si,offset RBP
                call    printtext
                mov     ax,bp
                call    printregister
                mov     si,offset RSI
                call    printtext
                pop     ax
                call    printregister
                push    ax
                mov     si,offset RDI
                call    printtext
                mov     ax,di
                call    printregister
                call    prcrlf
                mov     si,offset RDS
                call    printtext
                push    ds
                pop     ax
                call    printregister
                mov     si,offset RES
                call    printtext
                push    es
                pop     ax
                call    printregister
                mov     si,offset RSS
                call    printtext
                push    ss
                pop     ax
                call    printregister
                mov     si,offset RCS
                call    printtext
                mov     ax,[bp+04]
                call    printregister
                mov     si,offset RIP
                call    printtext
                mov     ax,[bp+02]
                call    printregister
                mov     si,offset RFLAGS
                call    printtext
                mov     ax,[bp+06]
                call    printregister
                call    prcrlf
                call    prcrlf
                pop     si
                popa
                pop     bp
                sti
                iret
                
                
printregister:  push    bp
                mov     bp,sp
                push    ax
                push    bx
                mov     bx,offset xtable
                shr     ax,12
                xlat
                mov     ah,0Eh
                int     10h
                mov     ax,[bp-02]
                shr     ax,8
                and     al,0Fh
                xlat
                mov     ah,0Eh
                int     10h
                mov     ax,[bp-02]
                shr     ax,4
                and     al,0Fh
                xlat
                mov     ah,0Eh
                int     10h                
                mov     ax,[bp-02]
                and     al,0Fh
                xlat
                mov     ah,0Eh
                int     10h
                mov     al,20h
                int     10h
                pop     bx
                pop     ax
                pop     bp
                ret
printtext:      push    ax
                push    si
                mov     ah,0Eh
nextchar:       lodsb
                test    al,al
                jnz     printtext1
                pop     si
                pop     ax
                ret
printtext1:     int     10h
                jmp     nextchar

printvectt:     push    ds
                push    es
                push    ax
                push    bx
                push    cx
                push    di
                push    cs
                pop     ds
                xor     ax,ax
                mov     es,ax
                xor     di,di
                mov     bx,offset xtable
                mov     cx,0010h
printvectt1:    mov     al,es:[di]
                push    ax
                shr     al,4
                xlat
                mov     ah,0Eh
                int     10h
                pop     ax
                and     al,0Fh
                xlat
                mov     ah,0Eh
                int     10h
                mov     al,20h
                int     10h
                inc     di
                loop    printvectt1
                call    prcrlf
                pop     si
                pop     cx
                pop     bx
                pop     ax
                pop     es
                pop     ds
                ret

prcrlf:         push    ax
                mov     ax,0E0Dh
                int     10h
                mov     ax,0E0Ah
                int     10h
                pop     ax
                ret

xtable          db      '0123456789ABCDEF'
metin1          db      'CS = ',0
metin2          db      0Dh, 0Ah, 'IP = ',0
RAX             db      'AX=',0
RBX             db      'BX=',0
RCX             db      'CX=',0
RDX             db      'DX=',0
RSP             db      'SP=',0
RBP             db      'BP=',0
RSI             db      'SI=',0
RDI             db      'DI=',0
RDS             db      'DS=',0
RES             db      'ES=',0
RSS             db      'SS=',0
RCS             db      'CS=',0
RIP             db      'IP=',0
RFLAGS          db      'FLAGS=',0

oldvectorseg    dw      ?
oldvectoroffs   dw      ?

end basla