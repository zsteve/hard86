/**
 * @file Opcode implementations
 * x86 emulator engine for hard86
 * Stephen Zhang, 2014
 */

#ifndef OPCODES_H
#define OPCODES_H

/* opcode function body template:
void op_??(){

	return 0;
}
*/

void process_instr_prefixes();

/* JMP */
void op_0xeb();
void op_0xe9();

/* JCC */
void op_0x74();
void op_0x7e();
void op_0x75();
void op_0x7c();
void op_0x72();
void op_0x76();
void op_0x7a();
void op_0x70();
void op_0x78();
void op_0x7d();
void op_0x7f();
void op_0x73();
void op_0x77();
void op_0x7b();
void op_0x71();
void op_0x79();

/* MOV */
void op_0xb8();
void op_0x88();
void op_0x89();
void op_0x8a();
void op_0x8b();
void op_0x8c();
void op_0x8e();
void op_0xa0();
void op_0xa1();
void op_0xa2();
void op_0xa3();
void op_0xb0();

/* VARIOUS */
void op_0xc6();
void op_0xc7();
void op_0x80();
void op_0x81();
void op_0x83();
void op_0xff();
void op_0x8f();

/* ADD */
void op_0x04();
void op_0x05();
void op_0x00();
void op_0x01();
void op_0x02();
void op_0x03();

/* ADC */
void op_0x14();
void op_0x15();
void op_0x10();
void op_0x11();
void op_0x12();
void op_0x13();

/* PUSH */
void op_0x50();
void op_0x6a();
void op_0x68();

/* POP */
void op_0x58();
void op_0xe6();

/* OUT */
void op_0xe7();
void op_0xee();
void op_0xef();

/* IN */
void op_0xe4();
void op_0xe5();
void op_0xec();
void op_0xed();

/* SUB */
void op_0x2c();
void op_0x2d();
void op_0x28();
void op_0x29();
void op_0x2a();

/* CMP */
void op_0x38();
void op_0x39();
void op_0x3a();
void op_0x3b();
void op_0x3c();
void op_0x3d();

/* INC */
void op_0xfe();
void op_0x40();

/* DEC */
void op_0x48();

/* NEG */
void op_0xf6();
void op_0xf7();

/* SHL/SAL/SHR/SAR */
void op_0xd0();
void op_0xd1();
void op_0xd2();
void op_0xd3();

void op_0xc0();
void op_0xc1();

/* NOT */

/* AND */
void op_0x20();
void op_0x21();
void op_0x22();
void op_0x23();
void op_0x24();
void op_0x25();

void op_0x0c();
void op_0x0d();
void op_0x08();
void op_0x09();
void op_0x0a();
void op_0x0b();
void op_0x34();
void op_0x35();
void op_0x30();
void op_0x31();
void op_0x32();
void op_0x33();

/* CALL */
void op_0xe8();
void op_0x9a();

/* RET */
void op_0xc3();
void op_0xcb();
void op_0xc2();
void op_0xca();

#endif
