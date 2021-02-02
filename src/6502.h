#ifndef _6502_H
#define _6502_H

#include <iostream>
#include <vector>

namespace m6502 {

	typedef uint8_t BYTE;	// uint8_t (1 byte)
	typedef uint16_t WORD;	// uint16_t (2 bytes)

	// computer memory struct
	struct MEMORY {
		public:
			// returns reference to data[address] (1 cycle)
			BYTE &operator[](WORD address) {
				(*cycles)--;
				return this->data[address];
			}
			
			// fills memory with 0. Does not affect cycle count (done before CPU starts)
			void init(uint32_t *nCycles) {
				for (WORD i = 0; i < MAX_MEM; i++) {
					data[i] = 0;
				}
				cycles = nCycles;
			}

			// fills memory with given byte array. Does not affect cycle count (done before CPU starts)
			void fill(std::vector<BYTE> nData) {
				for (WORD i = 0; i < nData.size(); i++) {
					data[i] = nData[i];
					if (i == MAX_MEM) {
						break;
					}
				}
			}
		private:
			static constexpr WORD MAX_MEM = 0xFFFF;
			BYTE data[MAX_MEM];	// memory data (64 KiB)
			uint32_t *cycles;	// pointer to cycle count
	}; // struct MEMORY

	// computer central processing unit struct
	struct CPU {
		public:
			static constexpr bool READ = true;
			static constexpr bool WRITE = false;

			static constexpr BYTE ins_lda_im = 0xA9;	// immediate LDA instruction (2 cycles, 2 bytes. Affects zero and negative flags)
			static constexpr BYTE ins_lda_zp = 0xA5;	// zero-page LDA instruction (3 cycles, 2 bytes. Affects zero and negative flags)
			static constexpr BYTE ins_lda_zpx = 0xB5;	// zero-page X LDA instruction (4 cycles, 2 bytes. Affects zero and negative flags)
			static constexpr BYTE ins_lda_abs = 0xAD; 	// absolute LDA instruction (4 cycles, 3 bytes. Affects zero and negative flags)
			static constexpr BYTE ins_lda_absx = 0xBD; 	// absolute X LDA instruction (4-5 cycles, 3 bytes. Affects zero and negative flags)
			static constexpr BYTE ins_lda_absy = 0xB9; 	// absolute Y LDA instruction (4-5 cycles, 3 bytes. Affects zero and negative flags)
			static constexpr BYTE ins_lda_indx = 0xA1; 	// indirect X LDA instruction (6 cycles, 2 bytes. Affects zero and negative flags)
			static constexpr BYTE ins_lda_indy = 0xB1;	// indirect Y LDA instruction (5-6 cycles, 2 bytes. Affects zero and negative flags)

			static constexpr BYTE ins_ldx_im = 0xA2;	// immediate LDX instruction (2 cycles, 2 bytes. Affects zero and negative flags)
			static constexpr BYTE ins_ldx_zp = 0xA6;	// zero-page LDX instruction (3 cycles, 2 bytes. Affects zero and negative flags)
			static constexpr BYTE ins_ldx_zpy = 0xB6;	// zero-page Y LDX instruction (4 cycles, 2 bytes. Affects zero and negative flags)
			static constexpr BYTE ins_ldx_abs = 0xAE;	// absolute LDX instruction (4 cycles, 3 bytes. Affects zero and negative flags)
			static constexpr BYTE ins_ldx_absy = 0xBE;	// absolute Y LDX instruction (4-5 cycles, 3 bytes. Affects zero and negative flags)

			static constexpr BYTE ins_ldy_im = 0xA0;	// immediate LDY instruction (2 cycles, 2 bytes. Affects zero and negative flags)
			static constexpr BYTE ins_ldy_zp = 0xA4;	// zero-page LDY instruction (3 cycles, 2 bytes. Affects zero and negative flags)
			static constexpr BYTE ins_ldy_zpx = 0xB4;	// zero-page X LDY instruction (4 cycles, 2 bytes. Affects zero and negative flags)
			static constexpr BYTE ins_ldy_abs = 0xAC;	// absolute LDY instruction (4 cycles, 3 bytes. Affects zero and negative flags)
			static constexpr BYTE ins_ldy_absx = 0xBC;	// absolute X LDY instruction (4-5 cycles, 3 bytes. Affects zero and negative flags)

			static constexpr BYTE ins_sta_zp = 0x85;	// zero-page STA instruction (3 cycles, 2 bytes. Does not affect any flag)
			static constexpr BYTE ins_sta_zpx = 0x95;	// zero-page X STA instruction (4 cycles, 2 bytes. Does not affect any flag)
			static constexpr BYTE ins_sta_abs = 0x8D;	// absolute STA instruction (4 cycles, 3 bytes. Does not affect any flag)
			static constexpr BYTE ins_sta_absx = 0x9D;	// absolute X STA instruction (5 cycles, 3 bytes. Does not affect any flag)
			static constexpr BYTE ins_sta_absy = 0x99;	// absolute Y STA instruction (5 cycles, 3 bytes. Does not affect any flag)
			static constexpr BYTE ins_sta_indx = 0x81;	// indirect X STA instruction (6 cycles, 2 bytes. Does not affect any flag)
			static constexpr BYTE ins_sta_indy = 0x91;	// indirect Y STA instruction (6 cycles, 2 bytes. Does not affect any flag)

			static constexpr BYTE ins_stx_zp = 0x86;	// zero-page STX instruction (3 cycles, 2 bytes. Does not affect any flag)
			static constexpr BYTE ins_stx_zpy = 0x96;	// zero-page Y STX instruction (4 cycles, 2 bytes. Does not affect any flag)
			static constexpr BYTE ins_stx_abs = 0x8E;	// absolute STX instruction (4 cycles, 3 bytes. Does not affect any flag)

			static constexpr BYTE ins_sty_zp = 0x86;	// zero-page STY instruction (3 cycles, 2 bytes. Does not affect any flag)
			static constexpr BYTE ins_sty_zpx = 0x96;	// zero-page X STY instruction (4 cycles, 2 bytes. Does not affect any flag)
			static constexpr BYTE ins_sty_abs = 0x8E;	// absolute STY instruction (4 cycles, 3 bytes. Does not affect any flag)

			// sends a reset signal to reset computer state (7 cycles)
			void reset(uint32_t &cycles, MEMORY &mem) {
				reg_programCounter = 0x0000;
				reg_stackPointer = 0x00;
				cycles--;
				cycles--;
				fl_carry = fl_zero = fl_interr = fl_dec = fl_break = fl_oflow = fl_neg = false;
				reg_acc = reg_x = reg_y = 0;
				// sets stack pointer register to 0xFD
				pushStack(mem, 0x00);
				pushStack(mem, 0x00);
				pushStack(mem, 0x00);
				// stores contents at 0xFFFC and 0xFFFD (little-endian) in program counter (contains location of reset routine)
				reg_programCounter = littleEndianWord(rw(mem, 0xFFFC, READ), rw(mem, 0xFFFD, READ));
			}

			// executes instructions at programCounter while cycles is greater than 0
			void execute(uint32_t &cycles, MEMORY &mem) {
				while (cycles > 0) {
					BYTE instruction = fetch(mem);
					switch (instruction) {
						case ins_lda_im:
							{
								reg_acc = fetch(mem);
								setLoadFlags(reg_acc);
							}
							break;
						case ins_lda_zp:
							{
								reg_acc = rw(mem, fetch(mem), READ);
								setLoadFlags(reg_acc);
							}
							break;
						case ins_lda_zpx:
							{
								reg_acc = rw(mem, zeroPageXAddressing(cycles, fetch(mem)), READ);
								setLoadFlags(reg_acc);
							}
							break;
						case ins_lda_abs:
							{
								BYTE addressLowByte = fetch(mem);
								reg_acc = rw(mem, littleEndianWord(addressLowByte, fetch(mem)), READ);
								setLoadFlags(reg_acc);
							}
							break;
						case ins_lda_absx:
							{
								BYTE addressLowByte = fetch(mem);
								reg_acc = rw(mem, absoluteXAddressing(mem, littleEndianWord(addressLowByte, fetch(mem))), READ);
								setLoadFlags(reg_acc);
							}
							break;
						case ins_lda_absy:
							{
								BYTE addressLowByte = fetch(mem);
								reg_acc = rw(mem, absoluteYAddressing(mem, littleEndianWord(addressLowByte, fetch(mem))), READ);
								setLoadFlags(reg_acc);
							}
							break;
						case ins_lda_indx:
							{
								reg_acc = rw(mem, indirectXAddressing(cycles, mem, fetch(mem)), READ);
								setLoadFlags(reg_acc);
							}
							break;
						case ins_lda_indy:
							{
								reg_acc = rw(mem, indirectYAddressing(mem, fetch(mem)), READ);
								setLoadFlags(reg_acc);
							}
							break;
						case ins_ldx_im:
							{
								reg_x = fetch(mem);
								setLoadFlags(reg_x);
							}
							break;
						case ins_ldx_zp:
							{
								reg_x = rw(mem, fetch(mem), READ);
								setLoadFlags(reg_x);
							}
							break;
						case ins_ldx_zpy:
							{
								reg_x = rw(mem, fetch(mem) + reg_y, READ);
								setLoadFlags(reg_x);
							}
							break;
						case ins_ldx_abs:
							{
								BYTE addressLowByte = fetch(mem);
								reg_x = rw(mem, littleEndianWord(addressLowByte, fetch(mem)), READ);
								setLoadFlags(reg_x);
							}
							break;
						case ins_ldx_absy:
							{
								BYTE addressLowByte = fetch(mem);
								reg_x = rw(mem, absoluteYAddressing(mem, littleEndianWord(addressLowByte, fetch(mem))), READ);
								setLoadFlags(reg_x);
							}
							break;
						case ins_ldy_im:
							{
								reg_y = fetch(mem);
								setLoadFlags(reg_y);
							}
							break;
						case ins_ldy_zp:
							{
								reg_y = rw(mem, fetch(mem), READ);
								setLoadFlags(reg_y);
							}
							break;
						case ins_ldy_zpx:
							{
								reg_y = rw(mem, zeroPageXAddressing(cycles, fetch(mem)), READ);
								setLoadFlags(reg_y);
							}
							break;
						case ins_ldy_abs:
							{
								BYTE addressLowByte = fetch(mem);
								reg_y = rw(mem, littleEndianWord(addressLowByte, fetch(mem)), READ);
								setLoadFlags(reg_y);
							}
							break;
						case ins_ldy_absx:
							{
								BYTE addressLowByte = fetch(mem);
								reg_y = rw(mem, absoluteXAddressing(mem, littleEndianWord(addressLowByte, fetch(mem))), READ);
							}
							break;
						case ins_sta_zp:
							{
								mem[fetch(mem)] = reg_acc;
							}
							break;
						case ins_sta_zpx:
							{
								mem[zeroPageXAddressing(cycles, fetch(mem))] = reg_acc;
							}
							break;
						case ins_sta_abs:
							{
								BYTE addressLowByte = fetch(mem);
								mem[littleEndianWord(addressLowByte, fetch(mem))] = reg_acc;
							}
							break;
						case ins_sta_absx:
							{
								BYTE addressLowByte = fetch(mem);
								mem[absoluteXAddressing(mem, littleEndianWord(addressLowByte, fetch(mem)), true)] = reg_acc;
							}
							break;
						case ins_sta_absy:
							{
								BYTE addressLowByte = fetch(mem);
								mem[absoluteYAddressing(mem, littleEndianWord(addressLowByte, fetch(mem)), true)] = reg_acc;
							}
							break;
						case ins_sta_indx:
							{
								mem[indirectXAddressing(cycles, mem, fetch(mem))] = reg_acc;
							}
							break;
						case ins_sta_indy:
							{
								mem[indirectYAddressing(mem, fetch(mem), true)] = reg_acc;
							}
							break;
					}
				}
			}

			WORD reg_programCounter;	// 16-bit program counter register
			BYTE reg_stackPointer;		// 8-bit stack pointer register
			BYTE reg_acc;				// 8-bit accumulator register
			BYTE reg_x;					// 8-bit x register
			BYTE reg_y;					// 8-bit y register

			BYTE fl_carry:1;			// 1-bit carry flag
			BYTE fl_zero:1;				// 1-bit zero flag
			BYTE fl_interr:1;			// 1-bit interrupt flag
			BYTE fl_dec:1;				// 1-bit decimal flag
			BYTE fl_break:1;			// 1-bit break flag
			BYTE fl_oflow:1;			// 1-bit overflow flag
			BYTE fl_neg:1;				// 1-bit negative flag

			// reads and returns next byte at programCounter. Increments programCounter (1 cycle)
			BYTE fetch(MEMORY &mem) {
				BYTE data = mem[reg_programCounter];
				reg_programCounter++;
				return data;
			}

			// reads/writes and returns byte at absolute address (from 0x0000 to 0xFFFF) (1 cycle)
			BYTE rw(MEMORY &mem, WORD address, bool rw, BYTE data = 0x00) {
				BYTE value;
				if (rw == READ) {
					value = mem[address];
				} else {
					mem[address] = data;
					value = data;
				}
				return value;
			}

			// pushes value to stack (address reg_stackPointer | 0x0100) and increments stack pointer (1 cycle)
			BYTE pushStack(MEMORY &mem, BYTE value) {
				mem[reg_stackPointer | 0x0100] = value;
				reg_stackPointer++;
				return value;
			}

			// pulls and returns value from stack (address reg_stackPointer | 0x0100) and decrements stack pointer (1 cycle)
			BYTE pullStack(MEMORY &mem) {
				BYTE value = mem[reg_stackPointer | 0x0100];
				reg_stackPointer--;
				return value;
			}

			// returns effective address of zero-page X addressing mode (1 cycle)
			WORD zeroPageXAddressing(uint32_t &cycles, BYTE address) {
				cycles--;
				return (BYTE)(address + reg_x);
			}

			// returns effective address of absolute X addressing mode (1 cycle in case of page cross, 0 otherwise)
			WORD absoluteXAddressing(MEMORY &mem, WORD address, bool extraCycle = false) {
				address += reg_x;
				if ((address & 0x00ff) < reg_x || extraCycle) {
					// extra cycle when page boundary is crossed
					mem[address];
				}
				return address;
			}

			// returns effective address of absolute Y addressing mode (1 cycle in case of page cross, 0 otherwise)
			WORD absoluteYAddressing(MEMORY &mem, WORD address, bool extraCycle = false) {
				address += reg_y;
				if ((address & 0x00ff) < reg_y || extraCycle) {
					// extra cycle when page boundary is crossed
					mem[address];
				}
				return address;
			}

			// returns effective address of indirect X (indexed indirect) addressing more (3 cycles)
			WORD indirectXAddressing(uint32_t &cycles, MEMORY &mem, BYTE address) {
				address += reg_x;
				cycles--;
				BYTE lowByte = rw(mem, address, READ);
				address++;
				return littleEndianWord(lowByte, rw(mem, address, READ));
			}

			// returns effective address of indirect Y (indirect indexed) addressing more (3 cycles in case of page cross, 2 otherwise)
			WORD indirectYAddressing(MEMORY &mem, BYTE address, bool extraCycle = false) {
				BYTE lowByte = rw(mem, address, READ);
				address++;
				WORD effectiveAddress = littleEndianWord(lowByte, rw(mem, address, READ));
				effectiveAddress += reg_y;
				if ((effectiveAddress & 0x00ff) < reg_y || extraCycle) {
					// extra cycle when page boundary is crossed
					mem[effectiveAddress];
				}
				return effectiveAddress;
			}

			// loads a register with a defined value and sets appropriate flags
			void setLoadFlags(BYTE reg) {
				fl_zero = (reg == 0);
				fl_neg = (0b01000000 & reg) > 0;
			}

			// returns a low endian word formed from two bytes
			WORD littleEndianWord(BYTE lowByte, BYTE highByte) {
				return lowByte | (WORD)(highByte) >> 8;
			}
	}; // struct CPU
} // namespace 6502

#endif // ifndef _6502_H