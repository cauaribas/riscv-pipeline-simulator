#include "pipeline.hpp"
#include "instruction.hpp"
#include "utils.hpp"
#include <vector>
#include <iostream>

void resolve_data_conflicts(const std::vector<Instruction> &instructions, std::vector<Instruction> &resolved_instructions)
{
    bool has_conflict = false;
    bool next_has_conflict = false;

    Instruction nop;
    nop.full_instruction = "00000000000000000000000000010011";

    for (int i = 0; i < instructions.size() - 1; i++)
    {

        const Instruction &current_inst = instructions[i];
        const Instruction &next_inst = instructions[i + 1];
        const Instruction &subsequent_inst = instructions[i + 2];

        resolved_instructions.push_back(current_inst);

        has_conflict = false;
        next_has_conflict = false;

        if (current_inst.rd != "00000")
        {
            if (next_inst.rd == subsequent_inst.rs1 || next_inst.rd == subsequent_inst.rs2)
                next_has_conflict = true;

            if (next_inst.opcode == "I")
            {
                if (current_inst.rd == next_inst.rs1)
                { // checking if the next instruction has conflict
                    has_conflict = true;
                    resolved_instructions.push_back(nop); // 2 nops to complete 2 cycles before next instruction
                    resolved_instructions.push_back(nop);
                }
                else if (!next_has_conflict && !has_conflict && current_inst.rd == subsequent_inst.rs1)
                {                                         // checking if subsequent instruction has conflict
                    resolved_instructions.push_back(nop); // 1 nop for subsequent instrucion (1 nop + 1 instruction = 2 cycles)
                }
            }
            else
            { // R etc
                if (current_inst.rd == next_inst.rs1 || current_inst.rd == next_inst.rs2)
                { // checking if the next instruction has conflict
                    has_conflict = true;
                    resolved_instructions.push_back(nop); // 2 nops to complete 2 cycles before next instruction
                    resolved_instructions.push_back(nop);
                }
                else if (!next_has_conflict && !has_conflict && (current_inst.rd == subsequent_inst.rs1 || current_inst.rd == subsequent_inst.rs2))
                {                                         // checking if subsequent instruction has conflict
                    resolved_instructions.push_back(nop); // 1 nop for subsequent instrucion (1 nop + 1 instruction = 2 cycles)
                }
            }
        }
    }
    resolved_instructions.push_back(instructions[instructions.size() - 1]);
}

void forwarding_hazard(const std::vector<Instruction> &instructions, std::vector<Instruction> &resolved_instructions)
{
    Instruction nop;
    nop.full_instruction = "00000000000000000000000000010011";

    for (int i = 0; i < instructions.size() - 1; i++)
    {

        const Instruction &current_inst = instructions[i];
        const Instruction &next_inst = instructions[i + 1];

        resolved_instructions.push_back(current_inst);

        if (current_inst.opcode == "L")
        { // Load instructions
            if (next_inst.opcode == "I" || next_inst.opcode == "L")
            {
                if (current_inst.rd == next_inst.rs1)
                {
                    resolved_instructions.push_back(nop);
                }
            }
            else if (next_inst.opcode == "B" || next_inst.opcode == "S" || next_inst.opcode == "R")
            {
                if (current_inst.rd == next_inst.rs1 || current_inst.rd == next_inst.rs2)
                {
                    resolved_instructions.push_back(nop);
                }
            }
        }
    }
    resolved_instructions.push_back(instructions[instructions.size() - 1]);
}

void reorder_with_nops(std::vector<Instruction> &instructions, std::vector<Instruction> &resolved_instructions)
{
    bool has_conflict = false;
    bool next_has_conflict = false;

    Instruction nop;
    nop.full_instruction = "00000000000000000000000000010011";

    for (int i = 0; i < instructions.size(); i++)
    {
        const Instruction &current_inst = instructions[i];
        const Instruction &next_inst = instructions[i + 1];
        const Instruction &subsequent_inst = instructions[i + 2];

        if (current_inst.full_instruction == "00000000000000000000000000010011") // NOP
            continue;

        if (current_inst.opcode == "J" || current_inst.opcode == "B")
        {
            resolved_instructions.push_back(current_inst);
            continue;
        }
        resolved_instructions.push_back(current_inst);

        has_conflict = false;
        next_has_conflict = false;

        if (current_inst.rd != "00000")
        {
            if (next_inst.rd == subsequent_inst.rs1 || next_inst.rd == subsequent_inst.rs2)
                next_has_conflict = true;

            if (current_inst.rd == next_inst.rs1 || current_inst.rd == next_inst.rs2)
            {
                next_has_conflict = true;

                for (int k = 0; k < 2; k++)
                { // 2 reordering cycles
                    bool check_for_reorder = false;

                    for (int j = i + 1; j < instructions.size(); j++)
                    {
                        Instruction &reorder_inst = instructions[j];
                        Instruction &prev_inst = instructions[i - 1];

                        if (reorder_inst.opcode == "J" || reorder_inst.opcode == "B" || (prev_inst.opcode == "I" && prev_inst.funct3 == "000"))
                        {
                            break;
                        }

                        if (current_inst.rd == reorder_inst.rs1 || current_inst.rd == reorder_inst.rs2)
                        {
                            bool has_conflict_for_reorder = false;

                            if (instructions[i - 2].rd == reorder_inst.rs1 || instructions[i - 2].rd == reorder_inst.rs2)
                            {
                                has_conflict_for_reorder = true;
                            }
                            if (prev_inst.rd == reorder_inst.rs1 || prev_inst.rd == reorder_inst.rs2)
                            {
                                has_conflict_for_reorder = true;
                            }
                            if (instructions[i + 1].rs1 == reorder_inst.rd || instructions[i + 1].rs2 == reorder_inst.rd)
                            {
                                has_conflict_for_reorder = true;
                            }
                            if (instructions[i + 2].rs1 == reorder_inst.rd || instructions[i + 2].rs2 == reorder_inst.rd)
                            {
                                has_conflict_for_reorder = true;
                            }
                            if (prev_inst.opcode == "B")
                            { // JALR (fix maybe)
                                has_conflict_for_reorder = true;
                            }
                            if (!has_conflict_for_reorder)
                            {
                                check_for_reorder = true;
                                if (i <= instructions.size())
                                {
                                    instructions.insert(instructions.begin() + i + 1, reorder_inst); // Insere na nova posição
                                }
                                if (j + 1 < instructions.size())
                                {
                                    instructions.erase(instructions.begin() + j); // Remove da posição original
                                }
                                break;
                            }
                        }
                    }
                    if (!check_for_reorder)
                    {
                        resolved_instructions.push_back(nop);
                    }
                }
            }
            else if (!next_has_conflict && !has_conflict && (current_inst.rd == subsequent_inst.rs1 || current_inst.rd == subsequent_inst.rs2))
            {
                bool check_for_reorder = false;

                for (int j = i + 1; j < instructions.size(); j++)
                {
                    Instruction &reorder_inst = instructions[j];
                    Instruction &prev_inst = instructions[i - 1];

                    if (reorder_inst.opcode == "J" || reorder_inst.opcode == "B" || (prev_inst.opcode == "I" && prev_inst.funct3 == "000"))
                    {
                        break;
                    }

                    if (current_inst.rd == reorder_inst.rs1 || current_inst.rd == reorder_inst.rs2)
                    {
                        bool has_conflict_for_reorder = false;

                        if (instructions[i - 2].rd == reorder_inst.rs1 || instructions[i - 2].rd == reorder_inst.rs2)
                        {
                            has_conflict_for_reorder = true;
                        }
                        if (prev_inst.rd == reorder_inst.rs1 || prev_inst.rd == reorder_inst.rs2)
                        {
                            has_conflict_for_reorder = true;
                        }
                        if (instructions[i + 1].rs1 == reorder_inst.rd || instructions[i + 1].rs2 == reorder_inst.rd)
                        {
                            has_conflict_for_reorder = true;
                        }
                        if (instructions[i + 2].rs1 == reorder_inst.rd || instructions[i + 2].rs2 == reorder_inst.rd)
                        {
                            has_conflict_for_reorder = true;
                        }
                        if (prev_inst.opcode == "B")
                        { // JALR (fix maybe)
                            has_conflict_for_reorder = true;
                        }
                        if (!has_conflict_for_reorder)
                        {
                            check_for_reorder = true;
                            if (i <= instructions.size())
                            {
                                instructions.insert(instructions.begin() + i + 1, reorder_inst); // Insere na nova posição
                            }
                            if (j + 1 < instructions.size())
                            {
                                instructions.erase(instructions.begin() + j); // Remove da posição original
                            }
                            break;
                        }
                    }
                }
                if (!check_for_reorder)
                {
                    resolved_instructions.push_back(nop);
                }
            }
        }
    }

    for (int i = 0; i < resolved_instructions.size(); i++)
    {
        decode_instruction(resolved_instructions[i].full_instruction);
        if (resolved_instructions[i].opcode == "JALR")
        { // JALR
            resolved_instructions.insert(resolved_instructions.begin() + i, nop);
            if (i != resolved_instructions.size() - 2)
            {
                resolved_instructions.insert(resolved_instructions.begin() + i + 2, nop);
            }
            i++;
        }
    }
}

void forwarding_and_reorder_with_nops(std::vector<Instruction> &instructions, std::vector<Instruction> &resolved_instructions)
{
    Instruction nop;
    nop.full_instruction = "00000000000000000000000000010011";

    for (int i = 0; i < instructions.size(); i++)
    {

        const Instruction &current_inst = instructions[i];
        const Instruction &next_inst = instructions[i + 1];

        resolved_instructions.push_back(current_inst);

        if (current_inst.opcode == "L")
        { // Load instructions
            if (next_inst.opcode == "I" || next_inst.opcode == "L")
            {
                if (current_inst.rd == next_inst.rs1)
                {
                    bool check_for_reorder = false;

                    for (int j = i + 1; j < instructions.size(); j++)
                    {
                        Instruction &reorder_inst = instructions[j];
                        Instruction &prev_inst = instructions[i - 1];

                        if (reorder_inst.opcode == "J" || reorder_inst.opcode == "B" || (prev_inst.opcode == "I" && prev_inst.funct3 == "000"))
                        {
                            break;
                        }

                        if (current_inst.rd == reorder_inst.rs1 || current_inst.rd == reorder_inst.rs2)
                        {
                            bool has_conflict_for_reorder = false;

                            if (instructions[i - 2].rd == reorder_inst.rs1 || instructions[i - 2].rd == reorder_inst.rs2)
                            {
                                has_conflict_for_reorder = true;
                            }
                            if (prev_inst.rd == reorder_inst.rs1 || prev_inst.rd == reorder_inst.rs2)
                            {
                                has_conflict_for_reorder = true;
                            }
                            if (instructions[i + 1].rs1 == reorder_inst.rd || instructions[i + 1].rs2 == reorder_inst.rd)
                            {
                                has_conflict_for_reorder = true;
                            }
                            if (instructions[i + 2].rs1 == reorder_inst.rd || instructions[i + 2].rs2 == reorder_inst.rd)
                            {
                                has_conflict_for_reorder = true;
                            }
                            if (prev_inst.opcode == "B" || prev_inst.opcode == "J" || (prev_inst.opcode == "I" && prev_inst.funct3 == "000"))
                            { // JALR (fix maybe)
                                has_conflict_for_reorder = true;
                            }
                            if (!has_conflict_for_reorder)
                            {
                                check_for_reorder = true;
                                if (i <= instructions.size())
                                {
                                    instructions.insert(instructions.begin() + i + 1, reorder_inst); // Insere na nova posição
                                }
                                if (j + 1 < instructions.size())
                                {
                                    instructions.erase(instructions.begin() + j); // Remove da posição original
                                }
                                break;
                            }
                        }
                    }
                    if (!check_for_reorder)
                    {
                        resolved_instructions.push_back(nop);
                    }
                }
            }
            else if (next_inst.opcode == "B" || next_inst.opcode == "S" || next_inst.opcode == "R")
            {
                if (current_inst.rd == next_inst.rs1 || current_inst.rd == next_inst.rs2)
                {
                    bool check_for_reorder = false;

                    for (int j = i + 1; j < instructions.size(); j++)
                    {
                        Instruction &reorder_inst = instructions[j];
                        Instruction &prev_inst = instructions[i - 1];

                        if (reorder_inst.opcode == "J" || reorder_inst.opcode == "B" || (prev_inst.opcode == "I" && prev_inst.funct3 == "000"))
                        {
                            break;
                        }

                        if (current_inst.rd == reorder_inst.rs1 || current_inst.rd == reorder_inst.rs2)
                        {
                            bool has_conflict_for_reorder = false;

                            if (instructions[i - 2].rd == reorder_inst.rs1 || instructions[i - 2].rd == reorder_inst.rs2)
                            {
                                has_conflict_for_reorder = true;
                            }
                            if (prev_inst.rd == reorder_inst.rs1 || prev_inst.rd == reorder_inst.rs2)
                            {
                                has_conflict_for_reorder = true;
                            }
                            if (instructions[i + 1].rs1 == reorder_inst.rd || instructions[i + 1].rs2 == reorder_inst.rd)
                            {
                                has_conflict_for_reorder = true;
                            }
                            if (instructions[i + 2].rs1 == reorder_inst.rd || instructions[i + 2].rs2 == reorder_inst.rd)
                            {
                                has_conflict_for_reorder = true;
                            }
                            if (prev_inst.opcode == "B" || prev_inst.opcode == "J" || (prev_inst.opcode == "I" && prev_inst.funct3 == "000"))
                            { // JALR (fix maybe)
                                has_conflict_for_reorder = true;
                            }
                            if (!has_conflict_for_reorder)
                            {
                                check_for_reorder = true;
                                if (i <= instructions.size())
                                {
                                    instructions.insert(instructions.begin() + i + 1, reorder_inst); // Insere na nova posição
                                }
                                if (j + 1 < instructions.size())
                                {
                                    instructions.erase(instructions.begin() + j); // Remove da posição original
                                }
                                break;
                            }
                        }
                    }
                    if (!check_for_reorder)
                    {
                        resolved_instructions.push_back(nop);
                    }
                }
            }
        }
    }
}
