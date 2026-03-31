;Declaration of Authorship:
;Sejal Goel 2401CS17

; Kadane Maximum Sub-array Algorithm
; array {-2, 1, -3, 4, -1, 2}
; N=6
; Stack: SP+0=i  SP+1=max_ending  SP+2=max_so_far  SP+3=cur  SP+4=base
; Result in register A on HALT

arr_0: data -2
arr_1: data  1
arr_2: data -3
arr_3: data  4
arr_4: data -1
arr_5: data  2

        ldc     0
        a2sp                    ; SP = 0
        adj     5               ; allocate 5 local slots

        ldc     arr_0
        stl     4               ; SP+4 = base

        ldc     arr_0
        ldnl    0               ; A = arr[0]
        stl     1               ; max_ending = arr[0]
        stl     2               ; max_so_far = arr[0]

        ldc     1
        stl     0               ; i = 1

loop:
        ldl     0               ; A = i
        adc     -6              ; A = i - N  (N=6)
        brlz    body            ; if i < N  -> continue
        br      done            ; if i >= N -> finished

body:
        ldl     4               ; A = base
        ldl     0               ; B = base,  A = i
        add                     ; A = base + i
        ldnl    0               ; A = arr[i]
        stl     3               ; SP+3 = cur

        ldl     1               ; A = max_ending
        ldl     3               ; B = max_ending,  A = cur
        add                     ; A = max_ending + cur
        stl     1               ; max_ending updated

        ldl     1               ; A = new max_ending
        ldl     3               ; B = new max_ending,  A = cur
        sub                     ; A = cur - new max_ending
        brlz    no_reset        ; if old max_ending >= 0 -> keep
        brz     no_reset        ; if old max_ending == 0 -> keep
        ldl     3               ; A = cur
        stl     1               ; reset: max_ending = cur

no_reset:
        ldl     2               ; A = max_so_far
        ldl     1               ; B = max_so_far,  A = max_ending
        sub                     ; A = max_ending - max_so_far
        brlz    no_update       ; if max_ending < max_so_far -> skip
        brz     no_update       ; if max_ending == max_so_far -> skip
        ldl     1               ; A = max_ending
        stl     2               ; max_so_far = max_ending

no_update:
        ldl     0               ; A = i
        adc     1               ; A = i + 1
        stl     0               ; i++
        br      loop

done:
        ldl     2               ; A = max_so_far  <- result in A
        HALT
