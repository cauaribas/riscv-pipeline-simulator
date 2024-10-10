# RISC-V Pipeline Simulator

This project is a college assignment developed in C++ that simulates the RISC-V architecture (RISCV-i32). The simulator focuses on classifying instruction types from a binary dump file. It detects and resolves data hazards by inserting NOPs (no-operation instructions) or reordering instructions to minimize conflicts. Additionally, it calculates the pipeline's performance by comparing execution time before and after conflict resolution, providing a clear view of the impact of optimizations on performance.

## Compilation

To compile the project, you can use `make` or the following `g++` command:

```sh
g++ -Iinclude -o pipeline src/main.cpp src/instruction.cpp src/utils.cpp src/pipeline.cpp
```

## Running the Simulator

After compiling, you can run the simulator using:

```sh
./pipeline.exe
```

## License

This project is licensed under the MIT License.
