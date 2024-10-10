#include "instruction.hpp"
#include <iostream>
#include <vector>

Instruction decode_instruction(const std::string &binary)
{
    Instruction instruction;
    if (binary.length() == 32)
    {
        instruction.opcode = find_opcode(binary);
        instruction.full_instruction = binary;
        instruction.rd = binary.substr(20, 5);
        instruction.funct3 = binary.substr(17, 3);

        if (instruction.opcode == "J" && instruction.rd == "00000")
        {
            instruction.rs1 = "00000";
            instruction.rs2 = "00000";
        }
        else if (instruction.opcode == "I" || instruction.opcode == "L")
        {
            instruction.rs1 = binary.substr(12, 5);
        }
        else
        {
            instruction.rs2 = binary.substr(7, 5);
            instruction.rs1 = binary.substr(12, 5);
        }
    }
    else
    {
        std::cout << "The instruction is not 32 bits long." << std::endl;
    }
    return instruction;
}

std::string find_opcode(const std::string &instruction)
{
    std::string op_code = instruction.substr(25, 7);
    if (op_code == "0110011")
    {
        return "R";
    }
    else if (op_code == "1110011" || op_code == "0010011")
    {
        return "I";
    }
    else if (op_code == "0000011")
    { // Load instructions
        return "L";
    }
    else if (op_code == "0100011")
    {
        return "S";
    }
    else if (op_code == "1100011")
    {
        return "B";
    }
    else if (op_code == "0110111" || op_code == "0010111")
    {
        return "U";
    }
    else if (op_code == "1101111")
    {
        return "J";
    }
    else if (op_code == "1100111")
    {
        return "JALR";
    }
    else
    {
        return "Opcode not implemented";
    }
}
