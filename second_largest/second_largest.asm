;Declaration of Authorship:
;Sejal Goel 2401CS17

; Second Largest Element in an Array
; array {3, 1, 4, 1, 5, 9}
; N=6
; Stack: SP+0=i  SP+1=largest  SP+2=second  SP+3=cur  SP+4=base
; Result in register A on HALT

arr_0: data 3
arr_1: data 1
arr_2: data 4
arr_3: data 1
arr_4: data 5
arr_5: data 9

        ldc     0
        a2sp                    ; SP = 0
        adj     5               ; allocate 5 local slots

        ldc     arr_0
        stl     4               ; SP+4 = base

        ldc     arr_0
        ldnl    0               ; A = arr[0]
        stl     1               ; largest = arr[0]

        ldc     arr_0
        ldnl    1               ; A = arr[1]
        stl     2               ; second = arr[1]

; ensure largest >= second (swap if needed)
        ldl     1               ; A = largest
        ldl     2               ; B = largest,  A = second
        sub                     ; A = second - largest
        brlz    seed_ok         ; if second < largest -> ok
        brz     seed_ok         ; if second == largest -> ok
        ldl     1               ; A = largest
        stl     3               ; temp = largest
        ldl     2               ; A = second
        stl     1               ; largest = second
        ldl     3               ; A = temp
        stl     2               ; second = old largest

seed_ok:
        ldc     2
        stl     0               ; i = 2

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

; case 1: if cur > largest -> second = largest, largest = cur
        ldl     1               ; A = largest
        ldl     3               ; B = largest,  A = cur
        sub                     ; A = cur - largest
        brlz    chk_second      ; if cur < largest -> check second
        brz     chk_second      ; if cur == largest -> check second
        ldl     1               ; A = old largest
        stl     2               ; second = old largest
        ldl     3               ; A = cur
        stl     1               ; largest = cur
        br      next_i

; case 2: if second < cur < largest -> second = cur
chk_second:
        ldl     2               ; A = second
        ldl     3               ; B = second,  A = cur
        sub                     ; A = cur - second
        brlz    next_i          ; if cur <= second -> skip
        brz     next_i          ; if cur == second -> skip
        ldl     1               ; A = largest
        ldl     3               ; B = largest,  A = cur
        sub                     ; A = cur - largest
        brz     next_i          ; if cur == largest -> skip (duplicate max)
        ldl     3               ; A = cur
        stl     2               ; second = cur

next_i:
        ldl     0               ; A = i
        adc     1               ; A = i + 1
        stl     0               ; i++
        br      loop

done:
        ldl     2               ; A = second largest  <- result in A
        HALT