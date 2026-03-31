;Declaration of Authorship:
;Sejal Goel 2401CS17

; Sum and Average of an Array
; array {10, 20, 30, 40, 50}  N=5
; Stack at SP=300: SP+0=i  SP+1=sum  SP+2=base
; sum stored at mem[200], average stored at mem[201]

        ldc     300
        a2sp                    ; SP = 300
        adj     3               ; allocate 3 local slots

; store array into mem[100..104]
        ldc     10
        ldc     100             ; B = 10,  A = 100
        stnl    0               ; mem[100] = 10
        ldc     20
        ldc     100             ; B = 20,  A = 100
        stnl    1               ; mem[101] = 20
        ldc     30
        ldc     100             ; B = 30,  A = 100
        stnl    2               ; mem[102] = 30
        ldc     40
        ldc     100             ; B = 40,  A = 100
        stnl    3               ; mem[103] = 40
        ldc     50
        ldc     100             ; B = 50,  A = 100
        stnl    4               ; mem[104] = 50

        ldc     100
        stl     2               ; SP+2 = base = 100
        ldc     0
        stl     1               ; SP+1 = sum = 0
        ldc     0
        stl     0               ; SP+0 = i = 0

loop:
        ldl     0               ; A = i
        adc     -5              ; A = i - N  (N=5)
        brlz    body            ; if i < N  -> continue
        br      done            ; if i >= N -> finished

body:
        ldl     2               ; A = base
        ldl     0               ; B = base,  A = i
        add                     ; A = base + i
        ldnl    0               ; A = mem[base+i] = arr[i]
        ldl     1               ; B = arr[i],  A = sum
        add                     ; A = sum + arr[i]
        stl     1               ; sum updated

        ldl     0               ; A = i
        adc     1               ; A = i + 1
        stl     0               ; i++
        br      loop

done:
; store sum at mem[200]
        ldl     1               ; A = sum
        ldc     200             ; B = sum,  A = 200
        stnl    0               ; mem[200] = sum

; average = sum / N  using repeated subtraction
; average = 0, keep subtracting N from sum until < N
        ldc     0               ; A = 0
        stl     0               ; reuse SP+0 as average = 0
        ldl     1               ; A = sum

avg_loop:
        adc     -5              ; A = A - N
        brlz    avg_store       ; if A < 0  -> done dividing
        brz     avg_exact       ; if A == 0 -> exact division
        stl     1               ; store remainder back into sum slot
        ldl     0               ; A = average
        adc     1               ; A = average + 1
        stl     0               ; average++
        ldl     1               ; A = remainder
        br      avg_loop

avg_exact:
        ldl     0               ; A = average
        adc     1               ; A = average + 1
        stl     0               ; average++

avg_store:
; store average at mem[201]
        ldl     0               ; A = average
        ldc     200             ; B = average,  A = 200
        stnl    1               ; mem[201] = average

        HALT