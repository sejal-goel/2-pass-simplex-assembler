# Custom 8-bit Opcode Assembler & Emulator

A complete development toolchain for a custom ISA (Instruction Set Architecture), featuring a **Two-Pass Assembler** and a **Register-based Emulator**.

**Author:** Sejal Goel  
**Roll No:** 2401CS17  
**Institution:** IIT Patna  

---

## Features

### 1. Assembler (`assembler.cpp`)
Converts `.asm` source files into binary object code.
* **Two-Pass Logic:** * **Pass 1:** Defines symbols, handles `SET` directives, and checks for label errors.
    * **Pass 2:** Generates 32-bit machine code and calculates relative offsets for branching.
* **Error Logging:** Generates a `.log` file detailing syntax errors or warnings (e.g., unused labels).
* **Listing File:** Generates a `.lst` file mapping Memory Addresses and Machine Code to the original Source Code.

### 2. Emulator (`emulator.cpp`)
Simulates the hardware execution of the assembled `.o` binary.
* **Architecture:** 4 core registers (A, B, SP, PC) and a 16MB memory space.
* **Execution Modes:**
    * **Trace:** Continuous execution with real-time register updates.
    * **Step:** Manual "Press Enter" execution for granular debugging.
* **Memory Dumps:** Exports the final state of the data region (stack/arrays) to a `_data.txt` file.
* **Safety Guards:** Detects infinite loops ($>10^6$ cycles), integer overflows, and segmentation faults.

---

## Getting Started

### Compilation
Use any C++11 (or later) compatible compiler:
```bash
g++ assembler.cpp -o assembler
g++ emulator.cpp -o emulator
