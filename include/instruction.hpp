#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include <string>

struct Instruction
{
    std::string full_instruction;
    std::string opcode;
    std::string rs1;
    std::string rs2;
    std::string rd;
    std::string funct3;
};

Instruction decode_instruction(const std::string &binary);
std::string find_opcode(const std::string &instruction);

#endif