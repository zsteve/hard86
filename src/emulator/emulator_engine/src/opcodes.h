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
int op_0x88();
int op_0x89();
int op_0x8a();
int op_0x8b();
int op_0x8c();
int op_0x8e();
int op_0xa0();
int op_0xa1();
int op_0xa2();
int op_0xa3();
int op_0xb0();
int op_0xff();
int op_0x50();
int op_0x6a();
int op_0x68();
int op_0x8f();
int op_0x58();
int op_0xe6();
int op_0xe7();
int op_0xee();
int op_0xef();
int op_0xe4();
int op_0xe5();
int op_0xec();
int op_0xed();
#endif