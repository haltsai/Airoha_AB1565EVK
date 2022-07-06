
Build/lib/pisplit_fir_eq_position_independent.splitload.lib:     file format elf32-xtensa-le


Disassembly of section .text:

00000000 <_init-0x58>:
   0:	000000ac 000001a8 00000550 00000534     ........P...4...
  10:	00000000 00000518 00000000 00000554     ............T...
  20:	000000f0 0000056c 000004a0 000000c8     ....l...........
  30:	000004bc 000000d0 000004d4 00000154     ............T...
  40:	000004ec 000000f0 00000540 00000320     ........@... ...
  50:	0000051c 00000070                       ....p...

00000058 <_init>:
  58:	008136        	entry	a1, 64
  5b:	ffe981        	l32r	a8, 0 (ac <frame_dummy>)
  5e:	f03d           	nop.n
  60:	0008e0        	callx8	a8
  63:	ffe881        	l32r	a8, 4 (1a8 <__do_global_ctors_aux>)
  66:	f03d           	nop.n
  68:	0008e0        	callx8	a8
  6b:	f01d           	retw.n

0000006d <_init+0x15>:
  6d:	000000                                        ...

00000070 <__do_global_dtors_aux>:
  70:	004136        	entry	a1, 32
  73:	ffe531        	l32r	a3, 8 (550 <__bss_start>)
  76:	000382        	l8ui	a8, a3, 0
  79:	d8ec           	bnez.n	a8, aa <__do_global_dtors_aux+0x3a>
  7b:	ffe421        	l32r	a2, c (534 <p$3519_6_1>)
  7e:	02b8           	l32i.n	a11, a2, 0
  80:	0ba8           	l32i.n	a10, a11, 0
  82:	ca8c           	beqz.n	a10, 92 <__do_global_dtors_aux+0x22>

00000084 <__do_global_dtors_aux+0x14>:
  84:	9b4b           	addi.n	a9, a11, 4
  86:	0299           	s32i.n	a9, a2, 0
  88:	000ae0        	callx8	a10
  8b:	02b8           	l32i.n	a11, a2, 0
  8d:	0ba8           	l32i.n	a10, a11, 0
  8f:	ff1a56        	bnez	a10, 84 <__do_global_dtors_aux+0x14>

00000092 <__do_global_dtors_aux+0x22>:
  92:	ffdfa1        	l32r	a10, 10 (0 <_text_start>)
  95:	0b0c           	movi.n	a11, 0
  97:	081ab7        	beq	a10, a11, a3 <__do_global_dtors_aux+0x33>
  9a:	ffdea1        	l32r	a10, 14 (518 <__EH_FRAME_BEGIN___44>)
  9d:	ffdc81        	l32r	a8, 10 (0 <_text_start>)
  a0:	0008e0        	callx8	a8

000000a3 <__do_global_dtors_aux+0x33>:
  a3:	1c0c           	movi.n	a12, 1
  a5:	0043c2        	s8i	a12, a3, 0
  a8:	f01d           	retw.n

000000aa <__do_global_dtors_aux+0x3a>:
  aa:	f01d           	retw.n

000000ac <frame_dummy>:
  ac:	004136        	entry	a1, 32
  af:	ffda81        	l32r	a8, 18 (0 <_text_start>)
  b2:	090c           	movi.n	a9, 0
  b4:	0b1897        	beq	a8, a9, c3 <frame_dummy+0x17>
  b7:	ffd7a1        	l32r	a10, 14 (518 <__EH_FRAME_BEGIN___44>)
  ba:	ffd8b1        	l32r	a11, 1c (554 <object$3538_8_1>)
  bd:	ffd681        	l32r	a8, 18 (0 <_text_start>)
  c0:	0008e0        	callx8	a8

000000c3 <frame_dummy+0x17>:
  c3:	f01d           	retw.n

000000c5 <frame_dummy+0x19>:
  c5:	000000                                        ...

000000c8 <get_fir_wbeq_memsize>:
  c8:	004136        	entry	a1, 32
  cb:	70a122        	movi	a2, 0x170
  ce:	f01d           	retw.n

000000d0 <Voice_TxEQ_init>:
  d0:	004136        	entry	a1, 32
  d3:	060c           	movi.n	a6, 0
  d5:	524b           	addi.n	a5, a2, 4
  d7:	b45c           	movi.n	a4, 91
  d9:	01d292        	addmi	a9, a2, 0x100
  dc:	bac992        	addi	a9, a9, -70
  df:	09a476        	loopgtz	a4, ec <Voice_TxEQ_init+0x1c>
  e2:	338d           	ae_l16si.n	a8, a3, 2
  e4:	332b           	addi.n	a3, a3, 2
  e6:	09c584        	ae_s16ip	a8, a5, 2
  e9:	5a5562        	s16i	a6, a5, 180

000000ec <Voice_TxEQ_init+0x1c>:
  ec:	0299           	s32i.n	a9, a2, 0
  ee:	f01d           	retw.n

000000f0 <Voice_TxEQ_Prcs_Length>:
  f0:	004136        	entry	a1, 32
  f3:	270c           	movi.n	a7, 2
  f5:	da2c           	movi.n	a10, 45
  f7:	934b           	addi.n	a9, a3, 4
  f9:	fec282        	addi	a8, a2, -2
  fc:	01d362        	addmi	a6, a3, 0x100
  ff:	0328           	l32i.n	a2, a3, 0
 101:	70c652        	addi	a5, a6, 112
 104:	fe248ba66e 	{ addi	a6, a6, -70; ae_zeroq56	aeq2 }
 109:	f3f750        	wur.ae_cend0	a5
 10c:	f3f660        	wur.ae_cbegin0	a6
 10f:	fc1453699f 	{ or	a6, a9, a9; ae_movq56	aeq0, aeq2 }

00000114 <Voice_TxEQ_Prcs_Length+0x24>:
 114:	408814        	ae_lp16f.iu	aep0, a8, 2
 117:	fc24e2827f 	{ ae_sp16f.l.c	aep0, a2, a7; nop }
 11c:	440314        	ae_lp16x2f.i	aep0, a3, 4
 11f:	0eaa76        	loopgtz	a10, 131 <Voice_TxEQ_Prcs_Length+0x41>
 122:	fc24d9227f 	{ ae_lp16f.c	aep2, a2, a7; nop }
 127:	e24319127f 	{ ae_lp16f.c	aep1, a2, a7; ae_mulafp24s.hl	aeq0, aep0, aep2 }
 12c:	e223ad861f 	{ ae_lp16x2f.iu	aep0, a6, 4; ae_mulafp24s.ll	aeq0, aep0, aep1 }

00000131 <Voice_TxEQ_Prcs_Length+0x41>:
 131:	fc24d9427f 	{ ae_lp16f.c	aep4, a2, a7; nop }
 136:	e28313699f 	{ or	a6, a9, a9; ae_mulafp24s.hl	aeq0, aep0, aep4 }
 13b:	da2c           	movi.n	a10, 45
 13d:	3640d4        	ae_sllisq56s	aeq1, aeq0, 3
 140:	f9ebcff44e 	{ addi	a4, a4, -1; ae_roundsp16q48sym	aep3, aeq1 }
 145:	fc1472380f 	{ ae_sp16f.l.i	aep3, a8, 0; ae_movq56	aeq0, aeq2 }
 14a:	c614e6        	bgei	a4, 1, 114 <Voice_TxEQ_Prcs_Length+0x24>
 14d:	0329           	s32i.n	a2, a3, 0
 14f:	f01d           	retw.n

00000151 <Voice_TxEQ_Prcs_Length+0x61>:
 151:	000000                                        ...

00000154 <Voice_TxEQ_Prcs>:
 154:	004136        	entry	a1, 32
 157:	03bd           	mov.n	a11, a3
 159:	ffb181        	l32r	a8, 20 (f0 <Voice_TxEQ_Prcs_Length>)
 15c:	02ad           	mov.n	a10, a2
 15e:	f0a0c2        	movi	a12, 240
 161:	0008e0        	callx8	a8
 164:	f01d           	retw.n

00000166 <Voice_TxEQ_Prcs+0x12>:
	...

00000168 <_start>:
 168:	004136        	entry	a1, 32
 16b:	ffafa1        	l32r	a10, 28 (4a0 <_DYNAMIC+0x2c0>)
 16e:	ffad31        	l32r	a3, 24 (56c <printf_ptr>)
 171:	0288           	l32i.n	a8, a2, 0
 173:	ffaeb1        	l32r	a11, 2c (c8 <get_fir_wbeq_memsize>)
 176:	0389           	s32i.n	a8, a3, 0
 178:	0008e0        	callx8	a8
 17b:	0388           	l32i.n	a8, a3, 0
 17d:	ffaca1        	l32r	a10, 30 (4bc <_DYNAMIC+0x2dc>)
 180:	ffadb1        	l32r	a11, 34 (d0 <Voice_TxEQ_init>)
 183:	0008e0        	callx8	a8
 186:	0388           	l32i.n	a8, a3, 0
 188:	ffaca1        	l32r	a10, 38 (4d4 <_DYNAMIC+0x2f4>)
 18b:	ffacb1        	l32r	a11, 3c (154 <Voice_TxEQ_Prcs>)
 18e:	0008e0        	callx8	a8
 191:	0388           	l32i.n	a8, a3, 0
 193:	ffaba1        	l32r	a10, 40 (4ec <_DYNAMIC+0x30c>)
 196:	ffabb1        	l32r	a11, 44 (f0 <Voice_TxEQ_Prcs_Length>)
 199:	0008e0        	callx8	a8
 19c:	ffab21        	l32r	a2, 48 (540 <export_parameter_array>)
 19f:	f01d           	retw.n

000001a1 <_start+0x39>:
 1a1:	00000000 36000000                                .......

000001a8 <__do_global_ctors_aux>:
 1a8:	004136        	entry	a1, 32
 1ab:	ffa881        	l32r	a8, 4c (320 <_DYNAMIC+0x140>)
 1ae:	7f2882        	l32i	a8, a8, 0x1fc
 1b1:	ffa721        	l32r	a2, 50 (51c <__CTOR_LIST___47>)
 1b4:	0c0826        	beqi	a8, -1, 1c4 <__do_global_ctors_aux+0x1c>
 1b7:	02a8           	l32i.n	a10, a2, 0

000001b9 <__do_global_ctors_aux+0x11>:
 1b9:	000ae0        	callx8	a10
 1bc:	fcc222        	addi	a2, a2, -4
 1bf:	02a8           	l32i.n	a10, a2, 0
 1c1:	f40a66        	bnei	a10, -1, 1b9 <__do_global_ctors_aux+0x11>

000001c4 <__do_global_ctors_aux+0x1c>:
 1c4:	f01d           	retw.n

000001c6 <__do_global_ctors_aux+0x1e>:
	...

000001c8 <_fini>:
 1c8:	008136        	entry	a1, 64
 1cb:	ffa281        	l32r	a8, 54 (70 <__do_global_dtors_aux>)
 1ce:	f03d           	nop.n
 1d0:	0008e0        	callx8	a8

000001d3 <_fini+0xb>:
 1d3:	f01d           	retw.n
