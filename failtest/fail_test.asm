; test_errors.asm
; Test error handling - fibonacci sequence attempt
; Each line demonstrates a different assembler error

start:
start:          ; duplicate label definition
        br      nowhere         ; no such label
        ldc     09xyz           ; not a number (invalid octal)
        stl                     ; missing operand
        add     3               ; unexpected operand
        ldc     10, 20          ; extra on end of line
3bad:                           ; bogus label name
        fib                     ; bogus mnemonic
        ldc     next next       ; extra on end of line