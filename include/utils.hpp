#ifndef UTILS_HPP
#define UTILS_HPP

#include "instruction.hpp"
#include <vector>
#include <string>

// Declarações de funções utilitárias
void read_file(const std::string &input_file, std::vector<Instruction> &instructions);
void write_file(const std::string &output_file, const std::vector<Instruction> &resolved_instructions);
double performance_calculator(double exec_time1, double exec_time2);
double cpi_calculator(int instructions);
double execution_time_calculator(int instructions, double t_clock);

#endif
