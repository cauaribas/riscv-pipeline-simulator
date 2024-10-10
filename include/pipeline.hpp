#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include <vector>
#include "instruction.hpp"

void resolve_data_conflicts(const std::vector<Instruction> &instructions, std::vector<Instruction> &resolved_instructions);
void forwarding_hazard(const std::vector<Instruction> &instructions, std::vector<Instruction> &resolved_instructions);
void reorder_with_nops(std::vector<Instruction> &instructions, std::vector<Instruction> &resolved_instructions);
void forwarding_and_reorder_with_nops(std::vector<Instruction> &instructions, std::vector<Instruction> &resolved_instructions);

#endif // PIPELINE_HPP