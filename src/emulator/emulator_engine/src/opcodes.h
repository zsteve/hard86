/**
 * @file Opcode implementations
 * x86 emulator engine for hard86
 * Stephen Zhang, 2014
 */

#ifndef OPCODES_H
#define OPCODES_H

/* opcode function body template:
int op_??(){

	return 0;
}
*/

void process_instr_prefixes();

int op_0xeb();
int op_0xb8();
int op_0x04();
int op_0x05();
int op_0x80();

#endif