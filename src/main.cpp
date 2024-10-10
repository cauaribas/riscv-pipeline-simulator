#include "pipeline.hpp"
#include "instruction.hpp"
#include "utils.hpp"
#include <iostream>

int main()
{
    std::string input_file = "dumpfiles/dumpfile4.txt";
    std::vector<Instruction> instructions;
    std::vector<Instruction> resolved_instructions;

    double t_clock;
    std::cout << "Enter CPU clock time: ";
    std::cin >> t_clock;

    read_file(input_file, instructions);

    resolve_data_conflicts(instructions, resolved_instructions);
    // forwarding_hazard(instructions, resolved_instructions);
    // reorder_with_nops(instructions, resolved_instructions);
    // forwarding_and_reorder_with_nops(instructions, resolved_instructions);

    double execution_time = execution_time_calculator(instructions.size(), t_clock);
    double execution_time_nop = execution_time_calculator(resolved_instructions.size(), t_clock);

    std::cout << "Execution time without NOPs: " << execution_time << " cycles\n";
    std::cout << "Execution time with NOPs: " << execution_time_nop << " cycles\n";
    std::cout << "The pipeline without NOPs (ideal pipeline) is " << performance_calculator(execution_time, execution_time_nop) << " times faster than the pipeline with NOPs" << std::endl;

    // Escreva o resultado em um arquivo txt
    write_file("results/output.txt", resolved_instructions);

    return 0;
}
