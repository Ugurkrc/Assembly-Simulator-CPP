#include <iostream>
#include <string>
#include <fstream> 

using namespace std;

//We have to write our own string-to-int, since atoi is banned.

int stringToInt(string str, bool& outError) {
    int sonuc = 0;
    int basamak = 1;
    bool negatif = false;
    outError = false; 

    int baslangic = 0;
    if (str.length() == 0) {
        outError = true;
        return 0;
    }

    // Check for negative numbers
    if (str[0] == '-') {
        negatif = true;
        baslangic = 1;
        if (str.length() == 1) { 
             outError = true;
             return 0;
        }
    }

    // Iterate string from end to start to build the number
    for (int i = str.length() - 1; i >= baslangic; --i) {
        if (str[i] >= '0' && str[i] <= '9') {
            sonuc += (str[i] - '0') * basamak;
            basamak *= 10;
        } else {
            // If char is not a digit, it's an error.
            outError = true;
            return 0;
        }
    }

    if (negatif) {
        return -sonuc;
    }
    return sonuc;
}

 //Converts a string to uppercase.
 //Used to parse instruction names (MOV, ADD, etc.).
 
string stringToUpper(string str) {
    string result = "";
    for (int i = 0; i < str.length(); ++i) {
        if (str[i] >= 'a' && str[i] <= 'z') {
            result += str[i] - ('a' - 'A'); // Convert lowercase to uppercase
        } else {
            result += str[i];
        }
    }
    return result;
}

 //We also need a custom line splitter, since strtok is banned.
void splitLine(string line, string parts[], int& partCount) {
    partCount = 0;
    string temp = "";
    
    // Ignore comments (;)
    int commentPos = -1;
    for(int i=0; i<line.length(); ++i) {
        if(line[i] == ';') {
            commentPos = i;
            break;
        }
    }
    if(commentPos != -1) {
        line = line.substr(0, commentPos);
    }

    // Split the line into parts
    for (int i = 0; i < line.length() && partCount < 3; ++i) {
        char c = line[i];

        // Space, tab, or comma is a delimiter
        if (c == ' ' || c == '\t' || c == ',') {
            if (temp.length() > 0) { // If a part has been accumulated
                parts[partCount] = temp;
                partCount++;
                temp = "";
            }
        } else {
            temp += c; // Add char to the current part
        }
    }

    // Add the last remaining part
    if (temp.length() > 0 && partCount < 3) {
        parts[partCount] = temp;
        partCount++;
    }
}

 //Represents the CPU's main memory.
 
class Memory {
public:
    Memory() {
        // Fill all memory locations with 0
        for (int i = 0; i < MEMORY_SIZE; ++i) {
            mem[i] = 0;
        }
    }

    unsigned char get(int address) {
        if (address < 0 || address >= MEMORY_SIZE) {
            cout << "ERROR: Invalid memory address (" << address << ")" << endl;
            return 0; 
        }
        return mem[address];
    }

    void set(int address, unsigned char value) {
        if (address < 0 || address >= MEMORY_SIZE) {
            cout << "ERROR: Invalid memory address (" << address << ")" << endl;
        } else {
            mem[address] = value;
        }
    }

    // Dumps the entire memory content to the console.
     
    void dump() {
        cout << "Memory Contents:" << endl;
        for (int i = 0; i < MEMORY_SIZE; ++i) {
            cout << "#" << i << ": " << (int)mem[i] << "\t";
            if ((i + 1) % 10 == 0) {
                cout << endl;
            }
        }
        cout << endl;
    }

private:
    // Per instructions, memory is 100 unsigned bytes.
    static const int MEMORY_SIZE = 100;
    unsigned char mem[MEMORY_SIZE];
};

// Represents the main CPU.
class CPU {
public:
    CPU() {
        // Initialize all registers to 0
        for (int i = 0; i < REGISTER_COUNT; ++i) {
            registers[i] = 0;
        }
        PC = 0; // Program counter starts at 0
        programHalted = false; 
    }

    bool isHalted() {
        return programHalted;
    }

    int getPC() {
        return PC;
    }
    
     // Line numbers are 1-based, so we adjust for our 0-based PC.
    void setPC(int newLine) {
        PC = newLine - 1; 
    }
     // Prints the contents of all registers.
     
    void dumpRegisters() {
        cout << "Registers: ";
        for (int i = 0; i < REGISTER_COUNT; ++i) {
            // Cast to (int) so it prints '80' instead of 'P'.
            cout << "R" << (i + 1) << "=" << (int)registers[i] << " ";
        }
        // Show PC as 1-based for the user.
        cout << "| PC=" << (PC + 1) << endl; 
    }

    /**
     * Executes a single line of instruction.
     */
    bool execute(string line, Memory& memory) {
        string parts[3]; 
        int partCount = 0;

        splitLine(line, parts, partCount);

        if (partCount == 0) {
            // Empty line or comment-only line, skip
            PC++; 
            return false;
        }

        string command = stringToUpper(parts[0]);
        bool hata = false; 
        // PC increments by 1 automatically, unless a JMP/JPN happens.

        if (command == "MOV") {
            hata = executeMOV(parts, partCount, memory);
        } 
        else if (command == "ADD") {
            hata = executeADD(parts, partCount, memory);
        }
        else if (command == "SUB") {
            hata = executeSUB(parts, partCount, memory);
        }
        else if (command == "PRN") {
            hata = executePRN(parts, partCount, memory);
        }
        else if (command == "JMP") {
            hata = executeJMP(parts, partCount);
        }
        else if (command == "JPN") {
            // *** DEĞİŞİKLİK 1: JPN çağırma şekli 'memory' içerecek şekilde güncellendi ***
            hata = executeJPN(parts, partCount, memory); 
        }
        else if (command == "HLT") {
            programHalted = true;
            cout << "--- Program Halted (HLT) ---" << endl;
            dumpRegisters(); 
            memory.dump();   
        }
        else {
            cout << "ERROR: Unknown command '" << command << "' on Line: " << (PC + 1) << endl;
            hata = true;
        }

        // If we didn't just jump, and we're not halted, move to the next line.
        if (command != "JMP" && !didJump && command != "HLT") {
            PC++; 
        }
        didJump = false; // Reset jump flag for the next instruction

        if (hata) {
            programHalted = true; 
            cout << "Program halted due to error on line: " << (PC + 1) << endl;
            return true;
        }

        return false; // No error
    }

private:
    // *** DEĞİŞİKLİK 2: Register sayısı 6 yapıldı (HW1 uyumu) ***
    static const int REGISTER_COUNT = 6; // R1-R6
    unsigned char registers[REGISTER_COUNT];
    int PC; // Program Counter (0-based index)
    bool programHalted;
    bool didJump = false; // Flag to stop PC from auto-incrementing after a jump

    bool isRegister(string op) {
        if (op.length() != 2) return false;
        if (op[0] != 'R' && op[0] != 'r') return false;
        // *** DEĞİŞİKLİK 3: Register kontrolü 6'ya çıkarıldı ***
        if (op[1] >= '1' && op[1] <= '6') return true; // R1-R6
        return false;
    }

    bool isAddress(string op) {
        if (op.length() < 2) return false;
        if (op[0] != '#') return false;
        bool error;
        stringToInt(op.substr(1), error); // Get the part *after* the '#' symbol.
        return !error;
    }

    bool isConstant(string op) {
        bool error;
        stringToInt(op, error);
        return !error;
    }

    /**
     * Simple char math to get the index (R1 -> 0, R2 -> 1, etc.)
     */
    int parseRegister(string op) {
        return op[1] - '1'; 
    }

    int parseAddress(string op, bool& error) {
        if (!isAddress(op)) {
            error = true;
            return 0;
        }
        return stringToInt(op.substr(1), error);
    }

    int parseConstant(string op, bool& error) {
        return stringToInt(op, error);
    }

    bool executeMOV(string parts[], int partCount, Memory& memory) {
        if (partCount != 3) return true; 

        string op1 = stringToUpper(parts[1]);
        string op2 = stringToUpper(parts[2]);
        bool error = false;

        if (isRegister(op1)) { // Destination is Register (MOV R1, ...)
            int regIndex = parseRegister(op1);

            if (isRegister(op2)) { // MOV R1, R2
                registers[regIndex] = registers[parseRegister(op2)];
            } 
            else if (isAddress(op2)) { // MOV R1, #ADRES
                int addr = parseAddress(op2, error);
                if (error) return true;
                registers[regIndex] = memory.get(addr);
            } 
            else if (isConstant(op2)) { // MOV R1, SABİT
                int val = parseConstant(op2, error);
                if (error) return true;
                registers[regIndex] = (unsigned char)val;
            } 
            else { return true; }
        } 
        else if (isAddress(op1)) { // Destination is Address (MOV #ADRES, ...)
            int addr = parseAddress(op1, error);
            if (error) return true;

            if (isRegister(op2)) { // MOV #ADRES, R1
                memory.set(addr, registers[parseRegister(op2)]);
            }
            else if (isConstant(op2)) { // MOV #ADRES, SABİT
                int val = parseConstant(op2, error);
                if (error) return true;
                memory.set(addr, (unsigned char)val);
            }
            else { return true; }
        }
        else { return true; }

        return false; 
    }

    bool executeADD(string parts[], int partCount, Memory& memory) {
        if (partCount != 3) return true; 

        string op1 = stringToUpper(parts[1]);
        string op2 = stringToUpper(parts[2]);
        bool error = false;

        if (!isRegister(op1)) return true; // First param must be a register

        int regIndex = parseRegister(op1);

        if (isRegister(op2)) { // ADD R1, R2
            registers[regIndex] += registers[parseRegister(op2)];
        }
        else if (isAddress(op2)) { // ADD R1, #ADRES
            int addr = parseAddress(op2, error);
            if (error) return true;
            registers[regIndex] += memory.get(addr);
        }
        else if (isConstant(op2)) { // ADD R1, CONSTANT
            int val = parseConstant(op2, error);
            if (error) return true;
            registers[regIndex] += (unsigned char)val;
        }
        else { return true; }

        return false; 
    }

    bool executeSUB(string parts[], int partCount, Memory& memory) {
        if (partCount != 3) return true; 

        string op1 = stringToUpper(parts[1]);
        string op2 = stringToUpper(parts[2]);
        bool error = false;

        if (!isRegister(op1)) return true; // First param must be a register

        int regIndex = parseRegister(op1);

        if (isRegister(op2)) { // SUB R1, R2
            registers[regIndex] -= registers[parseRegister(op2)];
        }
        else if (isAddress(op2)) { // SUB R1, #ADRESS
            int addr = parseAddress(op2, error);
            if (error) return true;
            registers[regIndex] -= memory.get(addr);
        }
        else if (isConstant(op2)) { // SUB R1, constant
            int val = parseConstant(op2, error);
            if (error) return true;
            registers[regIndex] -= (unsigned char)val;
        }
        else { return true; }
        
        return false; 
    }

    bool executeJMP(string parts[], int partCount) {
        bool error = false;

        if (partCount == 2) { // JMP line
            int lineNum = parseConstant(parts[1], error);
            if (error || lineNum <= 0) return true; 
            
            setPC(lineNum);
            didJump = true; // no need to increment PC
        }
        else if (partCount == 3) { // JMP R1, line
            if (!isRegister(stringToUpper(parts[1]))) return true; 
            
            int regIndex = parseRegister(stringToUpper(parts[1]));
            int lineNum = parseConstant(parts[2], error);
            
            if (error || lineNum <= 0) return true; 

            if (registers[regIndex] == 0) {
                setPC(lineNum);
                didJump = true;
            }
        }
        else { return true; }

        return false; 
    }

    // *** DEĞİŞİKLİK 4: executeJPN fonksiyonu istediğiniz mantığa göre (sabit OLMALI veya adres OLMALI) tamamen güncellendi ***
    bool executeJPN(string parts[], int partCount, Memory& memory) {
        if (partCount != 3) return true; 
        
        string op1 = stringToUpper(parts[1]);
        string op2 = stringToUpper(parts[2]); // İkinci parametre (adres VEYA sabit)
        bool error = false;

        if (!isRegister(op1)) return true; // İlk parametre register olmalı
        
        int regIndex = parseRegister(op1);
        int lineNum = 0; // Hedef satır numarası

        if (isConstant(op2)) { 
            // DURUM 1: JPN R1, 16 (Sabit)
            lineNum = parseConstant(op2, error);
            if (error) return true;
        }
        else if (isAddress(op2)) { 
            // DURUM 2: JPN R1, #16 (Adres)
            int addr = parseAddress(op2, error);
            if (error) return true;
            
            // İsteğiniz: Bellekten adresi oku, o adresteki DEĞERİ satır numarası yap
            lineNum = (int)memory.get(addr);
        }
        else {
            return true; // Hatalı ikinci parametre (ne sabit ne de adres)
        }

        // Satır numarasının geçerli olup olmadığını kontrol et
        if (lineNum <= 0) {
            cout << "ERROR: Invalid JPN line target (must be > 0): " << lineNum << endl;
            return true; 
        }

        // JPN koşulunu kontrol et
        signed char regValue = (signed char)registers[regIndex];

        if (regValue <= 0) {
            setPC(lineNum);
            didJump = true; // PC'nin otomatik artmasını engelle
        }
        
        return false; // Hata yok
    }

    bool executePRN(string parts[], int partCount, Memory& memory) {
        if (partCount != 2) return true; 

        string op1 = stringToUpper(parts[1]);
        bool error = false;

        if (isRegister(op1)) { // Check to see whether it's a register or not
            cout << (int)registers[parseRegister(op1)] << endl;
        }
        else if (isAddress(op1)) { // If it's an adress
            int addr = parseAddress(op1, error);
            if (error) return true;
            cout << (int)memory.get(addr) << endl;
        }
        else if (isConstant(op1)) { // If it's just a number
            int val = parseConstant(op1, error);
            if (error) return true;
            cout << val << endl;
        }
        else { return true; }

        return false; 
    }
};


int main(int argc, char* argv[]) {
    
    // 1. Checking to see if we have enough parameters
    if (argc != 3) {
        cout << "ERROR: Invalid usage." << endl;
        cout << "Usage: " << argv[0] << " <filename.txt> <option>" << endl;
        return 1; 
    }

    string filename = argv[1];
    string optionStr = argv[2];
    bool error;

    int option = stringToInt(optionStr, error);
    if (error || (option != 0 && option != 1 && option != 2)) {
        cout << "ERROR: Invalid option (Must be 0, 1, or 2)." << endl;
        return 1;
    }

    // 2. Read ProgramFile
    ifstream inputFile(filename.c_str());
    if (!inputFile) {
        cout << "ERROR: Could not open file: " << filename << endl;
        return 1;
    }

    string programLines[500]; // Max 500 lines
    int lineCount = 0;
    string line;

    while (lineCount < 500 && getline(inputFile, line)) {
        programLines[lineCount] = line;
        lineCount++;
    }
    inputFile.close();

    if (lineCount == 0) {
        cout << "ERROR: Program file is empty." << endl;
        return 1;
    }

    // 3. Create CPU and Memory
    CPU myCPU;
    Memory myMemory;

    // 4. Main Execution Loop
    while (!myCPU.isHalted() && myCPU.getPC() < lineCount) {
        
        int currentPC = myCPU.getPC();
        string currentInstruction = programLines[currentPC];

        if (option == 1 || option == 2) {
            cout << "Line " << (currentPC + 1) << ": " << currentInstruction << endl;
            myCPU.dumpRegisters();
        }
        
        if (option == 2) {
            myMemory.dump();
        }

        bool syntaxError = myCPU.execute(currentInstruction, myMemory);

        if (syntaxError) {
            // Error message was already printed inside execute()
            break; 
        }
    }

    // If it's option 0, we should dump the final state
    // even if the program just ran out of lines (no HLT).
    if (option == 0 && !myCPU.isHalted()) {
        cout << "--- Program End (No HLT) ---" << endl;
        myCPU.dumpRegisters();
        myMemory.dump();
    }

    return 0; // Completed successfully
}