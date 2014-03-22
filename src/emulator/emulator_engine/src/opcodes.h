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

/* JMP */
int op_0xeb();
int op_0xe9();

/* MOV */
int op_0xb8();
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

/* VARIOUS */
int op_0xc6();
int op_0xc7();
int op_0x80();
int op_0x81();
int op_0x83();
int op_0xff();
int op_0x8f();

/* ADD */
int op_0x04();
int op_0x05();
int op_0x00();
int op_0x01();
int op_0x02();
int op_0x03();

/* ADC */
int op_0x14();
int op_0x15();
int op_0x10();
int op_0x11();
int op_0x12();
int op_0x13();

/* PUSH */
int op_0x50();
int op_0x6a();
int op_0x68();

/* POP */
int op_0x58();
int op_0xe6();

/* OUT */
int op_0xe7();
int op_0xee();
int op_0xef();

/* IN */
int op_0xe4();
int op_0xe5();
int op_0xec();
int op_0xed();

/* SUB */
int op_0x2c();
int op_0x2d();
int op_0x28();
int op_0x29();
int op_0x2a();

/* CMP */
int op_0x38();
int op_0x39();
int op_0x3a();
int op_0x3b();
int op_0x3c();
int op_0x3d();

#endif