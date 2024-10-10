#include "utils.hpp"
#include <fstream>
#include <iostream>
#include <vector>

void read_file(const std::string &input_file, std::vector<Instruction> &instructions)
{
    std::fstream file(input_file);
    if (file.is_open())
    {
        std::string line;
        while (std::getline(file, line))
        {
            Instruction instruction = decode_instruction(line);
            instructions.push_back(instruction);
        }
        file.close();
    }
    else
    {
        std::cout << "Unable to open the file: " << input_file << std::endl;
    }
}

void write_file(const std::string &output_file, const std::vector<Instruction> &resolved_instructions)
{
    std::ofstream out(output_file);
    for (const auto &instruction : resolved_instructions)
    {
        out << instruction.full_instruction << "\n";
    }
    out.close();
}

double performance_calculator(double exec_time1, double exec_time2)
{
    return exec_time2 / exec_time1;
}

double cpi_calculator(int instructions)
{
    return (5 + (1 * (double)(instructions - 1))) / instructions;
}

double execution_time_calculator(int instructions, double t_clock)
{
    double cpi = cpi_calculator(instructions);
    std::cout << "Cpi: " << cpi << std::endl;
    std::cout << "Instructions: " << instructions << std::endl;
    std::cout << "Clock time: " << t_clock << std::endl;

    return instructions * cpi * t_clock;
}
