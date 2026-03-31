;Declaration of Authorship:
;Sejal Goel 2401CS17
; Bubble Sort Code

        ldc 300
        a2sp ;SP=300
        ldc 5 ; A = 5
        ldc 200 ; B = 5,   A = 200
        stnl 0 ; mem[200] = 5   arr[0] = 5
        ldc 3 ; A = 3
        ldc 200 ; B = 3,   A = 200
        stnl 1 ; mem[201] = 3   arr[1] = 3
        ldc 8 ; A = 8
        ldc 200 ; B = 8,   A = 200
        stnl 2 ; mem[202] = 8   arr[2] = 8
        ldc 1 ; A = 1
        ldc 200 ; B = 1,   A = 200
        stnl 3 ; mem[203] = 1   arr[3] = 1
        ldc 4 ; A = 4
        ldc 200 ; B = 4,   A = 200
        stnl 4 ; mem[204] = 4   arr[4] = 4
        ldc 0
        stl 0 ; mem[300] = 0  (i = 0),  A = old B

outer:
        ldl 0    ; A = i
        ldc 4    ; B = i,  A = 4
        sub      ; A = i - 4
        brz done ; i == 4 -> all passes done
        ldc 0
        stl 1    ; mem[301] = 0  (j = 0),  A = old B

inner:
        ldl     1               ; A = j
        ldc     4               ; B = j,  A = 4
        sub                     ; A = j - 4
        brz     next_i          ; j == 4 -> finished inner pass
        ldc     200             ; A = 200
        ldl     1               ; B = 200,  A = j
        add                     ; A = 200 + j
        stl     4               ; mem[304] = 200+j,  A = old B (don't use)
        ldl     4               ; A = mem[304] = 200+j
        ldnl    0               ; A = mem[200+j+0] = arr[j]
        stl     2               ; mem[302] = arr[j],  A = old B (don't use)
        ldl     4               ; A = mem[304] = 200+j  (reload — stl changed A)
        ldnl    1               ; A = mem[200+j+1] = arr[j+1]
        stl     3               ; mem[303] = arr[j+1],  A = old B (don't use)
        ldl     2               ; A = arr[j]
        ldl     3               ; B = arr[j],  A = arr[j+1]
        sub                     ; A = arr[j] - arr[j+1]
        brlz    noswap          ; arr[j] < arr[j+1] -> already in order
        ldl     3               ; A = arr[j+1]
        ldl     4               ; B = arr[j+1],  A = 200+j
        stnl    0               ; mem[200+j+0] = arr[j+1]   swap part 1 done
        ldl     2               ; A = arr[j]
        ldl     4               ; B = arr[j],  A = 200+j
        stnl    1               ; mem[200+j+1] = arr[j]      swap part 2 done

noswap:
        ldl     1               ; A = j   (always reload after stl side effects)
        adc     1               ; A = j + 1
        stl     1               ; mem[301] = j+1,  A = old B (don't use)
        br      inner           ; next j

next_i:
        ldl     0               ; A = i
        adc     1               ; A = i + 1
        stl     0               ; mem[300] = i+1,  A = old B (don't use)
        br      outer           ; next pass

done:   HALT