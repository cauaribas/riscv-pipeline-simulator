// Disciplina: Organização de Computadores
// Atividade: Avaliação 03 – Conflitos de dados no Pipeline
// Grupo: - Cauã Ribas
//	      - Nilson Andrade

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
// usar pragma lib

using namespace std;

struct Instruction {
    string full_instruction;
    string opcode;
    string rs1;
    string rs2;
    string rd;
    string funct3;
};

string find_opcode(string instruction){
    string op_code = instruction.substr(25, 7);
    if(op_code == "0110011"){
        return "R";
    }else if (op_code == "1110011" || op_code == "0010011" || op_code == "1100111"){
        return "I";
    }else if(op_code == "0000011"){ // Load instructions
        return "L";
    }else if (op_code == "0100011"){
        return "S";
    }else if (op_code == "1100011"){
        return "B";
    }else if (op_code == "0110111" || op_code == "0010111"){
        return "U";
    }else if (op_code == "1101111"){
        return "J";
    }else {
        return "Opcode not implemented";
    }
}

double performance_calculator(double exec_time1, double exec_time2) {
    return exec_time2 / exec_time1;
}

double cpi_calculator(int instructions) {
    return (5 + (1 * (double)(instructions - 1))) / instructions;
}

double execution_time_calculator(int instructions, double t_clock) {
    double cpi = cpi_calculator(instructions);
    cout << "Cpi: " << cpi << endl;
    cout << "Instructions: " << instructions << endl;
    cout << "Clock time: " << t_clock << endl;
    
    return instructions * cpi * t_clock;
} 

Instruction decode_instruction(string binary) { // instruction read from file
    Instruction Instruction;

    if (binary.length() == 32) {
        Instruction.opcode = find_opcode(binary);
        Instruction.full_instruction = binary;
        Instruction.rd = binary.substr(20,5);
        Instruction.funct3 = binary.substr(17,3);
        
        if (Instruction.opcode == "J" && Instruction.rd == "00000") {
            Instruction.rs1 = "00000";
            Instruction.rs2 = "00000";
        } else if (Instruction.opcode == "I") {
            if (Instruction.funct3 == "001" || Instruction.funct3 == "101") { // if Instruction is I-type and slli, srli, srai
                Instruction.rs1 = binary.substr(12,5);
                Instruction.rs2 = binary.substr(7,5);
            } else { 
                Instruction.rs1 = binary.substr(12,5);
            }
        } else {
            Instruction.rs2 = binary.substr(7,5);
            Instruction.rs1 = binary.substr(12,5);
        }
    } else {
        cout << "The instruction is not 32 bits long." << endl;
    }
    return Instruction;
}

void resolve_data_conflicts(const vector<Instruction>& instructions, vector<Instruction>& resolved_instructions) {
    bool has_conflict = false;
    bool next_has_conflict = false;

    Instruction nop;
    nop.full_instruction = "00000000000000000000000000010011";

    for (int i = 0; i < instructions.size() - 1; i++) {
        
        const Instruction& current_inst = instructions[i];
        const Instruction& next_inst = instructions[i + 1];
        const Instruction& subsequent_inst = instructions[i + 2];

        resolved_instructions.push_back(current_inst);
        
        has_conflict = false;
        next_has_conflict = false;

        if(current_inst.rd != "00000"){
            if(next_inst.rd == subsequent_inst.rs1 || next_inst.rd == subsequent_inst.rs2)
                next_has_conflict = true;

            if(next_inst.opcode == "I"){
                if (current_inst.rd == next_inst.rs1) { // checking if the next instruction has conflict
                    has_conflict = true;
                    resolved_instructions.push_back(nop); // 2 nops to complete 2 cycles before next instruction
                    resolved_instructions.push_back(nop);
                } else if (!next_has_conflict && !has_conflict && current_inst.rd == subsequent_inst.rs1) { // checking if subsequent instruction has conflict
                    resolved_instructions.push_back(nop); // 1 nop for subsequent instrucion (1 nop + 1 instruction = 2 cycles)
                }
            } else { // R etc
                if (current_inst.rd == next_inst.rs1 || current_inst.rd == next_inst.rs2) { // checking if the next instruction has conflict
                    has_conflict = true;
                    resolved_instructions.push_back(nop); // 2 nops to complete 2 cycles before next instruction
                    resolved_instructions.push_back(nop);
                } else if (!next_has_conflict && !has_conflict && (current_inst.rd == subsequent_inst.rs1 || current_inst.rd == subsequent_inst.rs2)) { // checking if subsequent instruction has conflict
                    resolved_instructions.push_back(nop); // 1 nop for subsequent instrucion (1 nop + 1 instruction = 2 cycles)
                }
            }
        }
    }
    resolved_instructions.push_back(instructions[instructions.size() - 1]);
}

void forwarding_hazard(const vector<Instruction>& instructions, vector<Instruction>& resolved_instructions) {
    
    Instruction nop;
    nop.full_instruction = "00000000000000000000000000010011";

    for (int i = 0; i < instructions.size() - 1; i++) {
        
        const Instruction& current_inst = instructions[i];
        const Instruction& next_inst = instructions[i + 1];

        resolved_instructions.push_back(current_inst);

        if(current_inst.opcode == "L"){ // Load instructions
            if(next_inst.opcode == "I" || next_inst.opcode == "L"){
                if(current_inst.rd == next_inst.rs1){
                    resolved_instructions.push_back(nop);
                }
            } else if (next_inst.opcode == "B" || next_inst.opcode == "S" || next_inst.opcode == "R"){
                if(current_inst.rd == next_inst.rs1 || current_inst.rd == next_inst.rs2){
                    resolved_instructions.push_back(nop);
                }
            }
        }
    }
    resolved_instructions.push_back(instructions[instructions.size() - 1]);
}

// void reorder_hazard(const vector<Instruction>& instructions, vector<Instruction>& resolved_instructions){
    
// }

void write_file(const vector<Instruction>& resolved_instructions) {
    ofstream output_file("output.txt");
    for (const auto& instruction : resolved_instructions) {
        output_file << instruction.full_instruction << "\n";
    }
    output_file.close();
}

void read_file(string& input_file, vector<Instruction>& instructions) {
    fstream file(input_file);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            Instruction Instruction = decode_instruction(line);
            instructions.push_back(Instruction);
        }
        file.close();
    } else {
        cout << "Unable to open the file." << input_file << endl;
    }
}

int main() {
    string input_file = "dumpfile6.txt";

    vector<Instruction> instructions;
    vector<Instruction> resolved_instructions;

    double t_clock;

    read_file(input_file, instructions);
    //resolve_data_conflicts(instructions, resolved_instructions);
    forwarding_hazard(instructions, resolved_instructions);
    write_file(resolved_instructions);
    
    cout << "Enter CPU clock time: ";
    cin >> t_clock;
    double execution_time = execution_time_calculator(instructions.size(), t_clock);
    double execution_time_nop = execution_time_calculator(resolved_instructions.size(), t_clock);

    cout << "Execution time for pipeline without NOPs (ideal pipeline): " << execution_time << endl;
    cout << "Execution time for pipeline with NOPs: " << execution_time_nop << endl;
    cout << "The pipeline without NOPs (ideal pipeline) is " << performance_calculator(execution_time, execution_time_nop) << " times faster than the pipeline with NOPs" << endl;

    return 0;
}