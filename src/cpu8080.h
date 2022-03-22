#ifndef CPU_8080_H
#define CPU_8080_H

/// 8080 structure declaration
struct _8080;
typedef struct _8080 cpu8080;

/// Constructor
cpu8080* cpu8080Init();

/// Destructor
void cpu8080Free(cpu8080*);

/**
* Emulates the next operation to be executed in the CPU according to its program counter.
* @param cpu    pointer to a 8080 cpu object.
* @returns      the number of clock cycles consumed by the operation.
*/
int cpu8080EmulateOp(cpu8080* cpu);

#endif // CPU_8080_H
