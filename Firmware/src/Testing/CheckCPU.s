.include "p33EP512GM710.inc" 
                    

;*************************************************************************************
                    .section .xdata, data, near, xmemory
                    .text
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;=====================================================================================
; Функция проверки ALU, W0-W15,  регистры цикла и стека
;-------------------------------------------
;   Использование регистров:
;   W0-W1 - временное хранение данных
;-------------------------------------------
					.global _CheckCPU_run
_CheckCPU_run:
		                        ;       флаг Z
                    disi    #(4-1)                              ;Запрет прерываний
                    xor     W0, W0, W0                          ;Обнуление регистра W0
                    bra     nz, SFRFail                         ;Если флаг Z сброшен
                    dec     W0, W0                              ;Уменьшение регистра W0 на 1
                    bra     z, SFRFail                          ;Если флаг Z установлен
                    ;       флаг C
                    disi    #(4-1)                              ;Запрет прерываний
                    add     W0, W0, W0                          ;Сложение регистров W0 и W0
                    bra     nc, SFRFail                         ;Если флаг C сброшен
                    add     #0, W0                              ;Сложение регистра W0 и константы 0
                    bra     c, SFRFail                          ;Если флаг C установлен
                    ;       флаг N
                    disi    #(4-1)                              ;Запрет прерываний
                    xor     W0, W0, W0                          ;Обнуление регистра W0
                    bra     n, SFRFail                          ;Если флаг N установлен
                    dec     W0, W0                              ;Уменьшение регистра W0 на 1
                    bra     nn, SFRFail                         ;Если флаг N сброшен
                    ;       флаг OV
                    disi    #(5-1)                              ;Запрет прерываний
                    xor     W0, W0, W0                          ;Обнуление регистра W0
                    bra     ov, SFRFail                         ;Если флаг OV сброшен
                    mov     #0x7FFF, W0                         ;Запись константы 0x7FFF в регистр W0
                    inc     W0, W0                              ;Увеличение регистра W0 на 1
                    bra     nov, SFRFail                        ;Если флаг OV установлен
                    ;       регистр W0
                    mov     #0xA5, W0                           ;Запись константы 0xA5 в регистр W0
                    xor     #0xA5, W0                           ;Сравнение регистров W0 и константы 0xA5
                    bra     nz, SFRFail                         ;Если не равны
                    mov     #0xA5, W0                           ;Запись константы 0xA5 в регистр W0
                    swap.b  W0                                  ;Запись константы 0x005A в регистр W0
                    xor     #0x5A, W0                           ;Сравнение регистров W0 и константы 0x5A
                    bra     nz, SFRFail                         ;Если не равны
                    mov     #0xA500, W0                         ;Запись константы 0xA500 в регистр W0
                    swap    W0                                  ;Запись константы 0x00A5 в регистр W0
                    xor     #0xA5, W0                           ;Сравнение регистров W0 и константы 0xA5
                    bra     nz, SFRFail                         ;Если не равны
                    mov     #0x5A00, W0                         ;Запись константы 0x5A00 в регистр W0
                    swap    W0                                  ;Запись константы 0x005A в регистр W0
                    xor     #0x5A, W0                           ;Сравнение регистров W0 и константы 0x5A
                    bra     nz, SFRFail                         ;Если не равны
                    ;       регистр W1
                    mov     #0x5AA5, W0                         ;Запись константы 0x5AA5 в регистр W0
                    mov     W0, W1                              ;Запись константы 0x5AA5 в регистр W1
                    cp      W1, W0                              ;Сравнение регистров W0 и W1
                    bra     nz, SFRFail                         ;Если не равны
                    swap    W0                                  ;Запись константы 0xA55A в регистр W0
                    mov     W0, W1                              ;Запись константы 0xA55A в регистр W1
                    cp      W1, W0                              ;Сравнение регистров W0 и W1
                    bra     nz, SFRFail                         ;Если не равны

                   ;       Регистры адресации FLASH-памяти
		       ;TBLPAG
                    mov     TBLPAG,  W1                         
                    mov     #0x5AA5, W0   
		    disi    #(3-1)    
                    mov     W0, TBLPAG
                    cp.b    TBLPAG
		    mov     W1, TBLPAG 
                    bra     nz, SFRFail
                    swap    W0
		    disi    #(3-1)    
                    mov     W0, TBLPAG
                    cp.b    TBLPAG
                    bra     nz, SFRFail
                       ; DSRPAG
	            mov     DSRPAG,  W1                
                    mov     #0x02A5, W0                         
		    disi    #(3-1)    
                    mov     W0, DSRPAG
                    cp      DSRPAG
	            mov     W1, DSRPAG                
                    bra     nz, SFRFail
                    mov     #0x015A, W0
		    disi    #(3-1)    
                    mov     W0, DSRPAG
                    cp.b    DSRPAG
	            mov     W1, DSRPAG                
                    bra     nz, SFRFail
                      ;DSWPAG
	         0: mov     DSWPAG,  W1                
                    mov     #0x00A5, W0                         
		    disi    #(3-1)    
                    mov     W0, DSWPAG
                    cp      DSWPAG
                    mov     W1, DSWPAG
                    bra     nz, SFRFail
                    mov     #0x015A, W0
		    disi    #(3-1)    
                    mov     W0, DSWPAG
                    cp.b    DSWPAG
                    mov     W1, DSWPAG
                    bra     nz, SFRFail
 
		    mov     RCOUNT,  W1 
                    mov     #0x1A5A, W0                         
		    disi    #(3-1)    
                    mov     W0, RCOUNT                          
                    cp      RCOUNT                              
                    mov     W1, RCOUNT
                    bra     nz, SFRFail                              
                    mov     #0x25A5, W0                         
		    disi    #(3-1)    
                    mov     W0, RCOUNT                          
                    cp      RCOUNT                              
                    mov     W1, RCOUNT
                    bra     nz, SFRFail
		    
		    ;-------Регистр W15 -- 
                    disi    #10  
                    mov     W15, W1      ; сохранение
                    mov     #0x5AA4, W0
                    mov     W0, W15
                    cp      W15, W0
                    bra     nz, SFRFail
                    mov     #0xA55A, W0
                    mov     W0, W15
                    cp      W15, W0
                    bra     nz, SFRFail
                    mov     W1, W15      ; восстановление

		    ;-------Регистр SPLIM -- 
                    mov     SPLIM, W1      ; сохранение
                    mov     #0x5AA4, W0
                    mov     W0, SPLIM
                    cp      SPLIM
                    bra     nz, SFRFail
                    mov     #0xA55A, W0
                    mov     W0, SPLIM
                    cp      SPLIM
                    bra     nz, SFRFail
                    mov     W1, SPLIM      ; восстановление
                    ;-------Регистры W2-W14
                    mov     #0x5AA5, W0
                    mov     #0x0004, W1
                    do      #(13-1), 0f
                    push.w [W1]           ; сохранение регистра в стеке - борьба с оптимизацией
                    mov     W0, [W1]
                    cp      W0, [W1]
                    bra     nz, SFRFail
                    swap    W0
                    mov     W0, [W1]
                    cp      W0, [W1]
                    bra     nz, SFRFail
                 2: pop.w  [W1]           ; восстановление регистра из стека
                    ;                 
                 0: inc2    W1, W1
                   ;
           Ok:
                    mov     #0x0001, W0
                    return;
                    ;
           SFRFail: mov     #0x0000, W0
                    return;
.end
