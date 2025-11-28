# Virtual CPU & Assembly Interpreter

## ⚡ Overview
This project is a standalone **System Emulator** designed to parse and execute a custom Assembly-like language. It simulates a complete **Von Neumann architecture**, featuring a distinct **CPU**, **Registers**, and **RAM (Memory)**.

Unlike typical school projects that rely on `strtok` or `atoi`, this interpreter implements **custom string parsing algorithms** to tokenize instructions and convert data types, demonstrating a deep understanding of low-level algorithmic logic and C++ memory manipulation.

## 🛠️ Architecture
The system is built on an Object-Oriented design pattern:

* **CPU Class:** Manages the Fetch-Decode-Execute cycle, updates the Program Counter (PC), and handles 6 general-purpose registers (`R1`-`R6`).
* **Memory Class:** Simulates a 100-byte addressable memory space for data storage and retrieval.
* **Instruction Parser:** A custom-built lexer that processes raw text files into executable commands, handling whitespace, comments, and case insensitivity manually.

## 💻 Supported Instruction Set (ISA)

| Command | Usage | Description |
| :--- | :--- | :--- |
| **MOV** | `MOV R1, 10` <br> `MOV R1, R2` <br> `MOV R1, #50` | Moves a constant, register value, or **memory value (from address #50)** into the target. |
| **ADD** | `ADD R1, 5` <br> `ADD R1, #10` | Adds a constant or memory value to the register. |
| **SUB** | `SUB R1, R3` | Subtracts a register value from the target register. |
| **PRN** | `PRN R1` <br> `PRN #20` | Prints the value of a register or the content of **memory address #20**. |
| **JMP** | `JMP 5` <br> `JMP R1, 10` | **Unconditional:** Jumps to line 5. <br> **Conditional:** Jumps to line 10 if `R1 == 0`. |
| **JPN** | `JPN R1, 15` | **Jump if Negative:** Jumps to line 15 if `R1 <= 0`. |
| **HLT** | `HLT` | Halts the processor and dumps the registers & memory state. |

## 🚀 How to Run

Compile the source code using any standard C++ compiler:

```bash
g++ main.cpp -o cpu_sim
