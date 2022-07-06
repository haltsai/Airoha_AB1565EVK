
Build/skew_ctrl_lib/pisplit_skew_ctrl_position_independent.splitload.lib:     file format elf32-xtensa-le


Disassembly of section .text:

00000000 <_init-0x98>:
   0:	000000ec 00000ad0 000010ac 00001094     ................
  10:	00000000 00001074 00000000 000010b0     ....t...........
  20:	000010c8 00001014 00000214 0000102c     ............,...
  30:	0000024c 00001050 00000640 000010a0     L...P...@.......
  40:	40000000 00400000 002aaaaa 55555555     ...@..@...*.UUUU
  50:	000ccd00 00a00000 00000af0 000003d8     ................
  60:	3fffffff 00000a34 000008b0 00000614     ...?4...........
  70:	00000438 00000560 0000a000 00002000     8...`........ ..
  80:	2aaaaaaa 00004000 00000ccd 00000e7c     ...*.@......|...
  90:	00001078 000000b0                       x.......

00000098 <_init>:
  98:	008136        	entry	a1, 64
  9b:	ffd981        	l32r	a8, 0 (ec <frame_dummy>)
  9e:	f03d           	nop.n
  a0:	0008e0        	callx8	a8
  a3:	ffd881        	l32r	a8, 4 (ad0 <__do_global_ctors_aux>)
  a6:	f03d           	nop.n
  a8:	0008e0        	callx8	a8
  ab:	f01d           	retw.n

000000ad <_init+0x15>:
  ad:	000000                                        ...

000000b0 <__do_global_dtors_aux>:
  b0:	004136        	entry	a1, 32
  b3:	ffd531        	l32r	a3, 8 (10ac <__bss_start>)
  b6:	000382        	l8ui	a8, a3, 0
  b9:	d8ec           	bnez.n	a8, ea <__do_global_dtors_aux+0x3a>
  bb:	ffd421        	l32r	a2, c (1094 <p$3519_6_1>)
  be:	02b8           	l32i.n	a11, a2, 0
  c0:	0ba8           	l32i.n	a10, a11, 0
  c2:	ca8c           	beqz.n	a10, d2 <__do_global_dtors_aux+0x22>

000000c4 <__do_global_dtors_aux+0x14>:
  c4:	9b4b           	addi.n	a9, a11, 4
  c6:	0299           	s32i.n	a9, a2, 0
  c8:	000ae0        	callx8	a10
  cb:	02b8           	l32i.n	a11, a2, 0
  cd:	0ba8           	l32i.n	a10, a11, 0
  cf:	ff1a56        	bnez	a10, c4 <__do_global_dtors_aux+0x14>

000000d2 <__do_global_dtors_aux+0x22>:
  d2:	ffcfa1        	l32r	a10, 10 (0 <_text_start>)
  d5:	0b0c           	movi.n	a11, 0
  d7:	081ab7        	beq	a10, a11, e3 <__do_global_dtors_aux+0x33>
  da:	ffcea1        	l32r	a10, 14 (1074 <__EH_FRAME_BEGIN___44>)
  dd:	ffcc81        	l32r	a8, 10 (0 <_text_start>)
  e0:	0008e0        	callx8	a8

000000e3 <__do_global_dtors_aux+0x33>:
  e3:	1c0c           	movi.n	a12, 1
  e5:	0043c2        	s8i	a12, a3, 0
  e8:	f01d           	retw.n

000000ea <__do_global_dtors_aux+0x3a>:
  ea:	f01d           	retw.n

000000ec <frame_dummy>:
  ec:	004136        	entry	a1, 32
  ef:	ffca81        	l32r	a8, 18 (0 <_text_start>)
  f2:	090c           	movi.n	a9, 0
  f4:	0b1897        	beq	a8, a9, 103 <frame_dummy+0x17>
  f7:	ffc7a1        	l32r	a10, 14 (1074 <__EH_FRAME_BEGIN___44>)
  fa:	ffc8b1        	l32r	a11, 1c (10b0 <object$3538_8_1>)
  fd:	ffc681        	l32r	a8, 18 (0 <_text_start>)
 100:	0008e0        	callx8	a8

00000103 <frame_dummy+0x17>:
 103:	f01d           	retw.n

00000105 <frame_dummy+0x19>:
 105:	000000                                        ...

00000108 <_start>:
 108:	004136        	entry	a1, 32
 10b:	ffc6a1        	l32r	a10, 24 (1014 <_DYNAMIC+0x2ec>)
 10e:	ffc431        	l32r	a3, 20 (10c8 <printf_ptr>)
 111:	0288           	l32i.n	a8, a2, 0
 113:	ffc5b1        	l32r	a11, 28 (214 <skew_ctrl_init>)
 116:	0389           	s32i.n	a8, a3, 0
 118:	0008e0        	callx8	a8
 11b:	0388           	l32i.n	a8, a3, 0
 11d:	ffc3a1        	l32r	a10, 2c (102c <_DYNAMIC+0x304>)
 120:	ffc4b1        	l32r	a11, 30 (24c <skew_ctrl_set_input_framesize>)
 123:	0008e0        	callx8	a8
 126:	0388           	l32i.n	a8, a3, 0
 128:	ffc3a1        	l32r	a10, 34 (1050 <_DYNAMIC+0x328>)
 12b:	ffc3b1        	l32r	a11, 38 (640 <skew_ctrl_process>)
 12e:	0008e0        	callx8	a8
 131:	ffc221        	l32r	a2, 3c (10a0 <export_parameter_array>)
 134:	f01d           	retw.n

00000136 <_start+0x2e>:
	...

00000138 <IN_LINE_32b_5ord>:
 138:	008136        	entry	a1, 64
 13b:	ffc581        	l32r	a8, 50 (ccd00 <_end+0xcbc34>)
 13e:	ffc541        	l32r	a4, 54 (a00000 <_end+0x9fef34>)
 141:	ffc291        	l32r	a9, 4c (55555555 <_end+0x55554489>)
 144:	ffc0b1        	l32r	a11, 44 (400000 <_end+0x3fef34>)
 147:	ffc0a1        	l32r	a10, 48 (2aaaaa <_end+0x2a99de>)
 14a:	5139           	s32i.n	a3, a1, 20
 14c:	ffbdc1        	l32r	a12, 40 (40000000 <_end+0x3fffef34>)
 14f:	36cc34        	ae_cvtq48a32s	aeq3, a12
 152:	39cab4        	ae_movpa24x2	aep4, a10, a11
 155:	507154        	ae_lp24f.i	aep7, a1, 20
 158:	cb1cfec97f 	{ ae_cvtq48a32s	aeq1, a9; ae_sllip24	aep7, aep7, 1 }
 15d:	e29eff313f 	{ ae_movq56	aeq0, aeq3; ae_mulafp24s.lh	aeq1, aep7, aep4 }
 162:	017434        	ae_mulafp24s.ll	aeq0, aep7, aep4
 165:	feec3ab0df 	{ ae_slliq56	aeq3, aeq3, 1; ae_cvtq48p24s.l	aeq2, aep7 }
 16a:	01bec4        	ae_subq56	aeq2, aeq3, aeq2
 16d:	1b8c24        	ae_roundsp24q48sym	aep2, aeq0
 170:	1bac54        	ae_roundsp24q48sym	aep5, aeq2
 173:	ed9dff313f 	{ ae_movq56	aeq0, aeq3; ae_mulsfp24s.lh	aeq3, aep7, aep4 }
 178:	1b9c34        	ae_roundsp24q48sym	aep3, aeq1
 17b:	0174b4        	ae_mulsfp24s.ll	aeq0, aep7, aep4
 17e:	017a74        	ae_mulfp24s.ll	aeq1, aep7, aep2
 181:	1bbc04        	ae_roundsp24q48sym	aep0, aeq3
 184:	1b8c44        	ae_roundsp24q48sym	aep4, aeq0
 187:	012374        	ae_mulfp24s.ll	aeq0, aep2, aep3
 18a:	018474        	ae_mulfp24s.ll	aeq2, aep0, aep4
 18d:	08fd24        	ae_mulfq32sp24s.l	aeq3, aeq0, aep7
 190:	1bac14        	ae_roundsp24q48sym	aep1, aeq2
 193:	019574        	ae_mulfp24s.ll	aeq2, aep1, aep5
 196:	f9fff7311f 	{ ae_sp24x2s.i	aep3, a1, 8; ae_roundsp24q48sym	aep3, aeq3 }
 19b:	019b74        	ae_mulfp24s.ll	aeq3, aep1, aep3
 19e:	1bac64        	ae_roundsp24q48sym	aep6, aeq2
 1a1:	086524        	ae_mulfq32sp24s.l	aeq0, aeq0, aep6
 1a4:	f8997ab09f 	{ ae_slliq56	aeq2, aeq3, 1; ae_mulfq32sp24s.l	aeq1, aeq1, aep6 }
 1a9:	fbfa3a809f 	{ ae_slliq56	aeq2, aeq0, 1; ae_addq56	aeq3, aeq3, aeq2 }
 1ae:	010ac4        	ae_addq56	aeq0, aeq0, aeq2
 1b1:	1bbc24        	ae_roundsp24q48sym	aep2, aeq3
 1b4:	01c574        	ae_mulfp24s.ll	aeq2, aep4, aep5
 1b7:	018d74        	ae_mulfp24s.ll	aeq3, aep0, aep5
 1ba:	f8e7f0011f 	{ ae_lp24x2.i	aep0, a1, 8; ae_roundsp24q48sym	aep1, aeq0 }
 1bf:	017074        	ae_mulfp24s.ll	aeq0, aep7, aep0
 1c2:	08b5a4        	ae_mulfq32sp24s.l	aeq2, aeq2, aep3
 1c5:	fb8d62548f 	{ ae_movpa24x2	aep5, a4, a8; ae_mulfq32sp24s.l	aeq3, aeq3, aep3 }
 1ca:	085d64        	ae_mulfq32sp24s.l	aeq1, aeq1, aep5
 1cd:	086524        	ae_mulfq32sp24s.l	aeq0, aeq0, aep6
 1d0:	1b9c74        	ae_roundsp24q48sym	aep7, aeq1
 1d3:	08dce4        	ae_mulfq32sp24s.h	aeq3, aeq3, aep5
 1d6:	08d5a4        	ae_mulfq32sp24s.l	aeq2, aeq2, aep5
 1d9:	085424        	ae_mulfq32sp24s.h	aeq0, aeq0, aep5
 1dc:	faf78ffcff 	{ movi	a15, -4; ae_roundsp24q48sym	aep4, aeq2 }
 1e1:	f9ffc042ee 	{ addi	a14, a2, 4; ae_roundsp24q48sym	aep3, aeq3 }
 1e6:	f8e7bc6eff 	{ ae_lq32f.xu	aeq3, a14, a15; ae_roundsp24q48sym	aep0, aeq0 }
 1eb:	fb917c4eff 	{ ae_lq32f.xu	aeq2, a14, a15; ae_mulfq32sp24s.l	aeq3, aeq3, aep4 }
 1f0:	f7cefc4eff 	{ ae_lq32f.xu	aeq2, a14, a15; ae_mulafq32sp24s.l	aeq3, aeq2, aep3 }
 1f5:	f7cafc4eff 	{ ae_lq32f.xu	aeq2, a14, a15; ae_mulafq32sp24s.l	aeq3, aeq2, aep2 }
 1fa:	f7c6fc4eff 	{ ae_lq32f.xu	aeq2, a14, a15; ae_mulafq32sp24s.l	aeq3, aeq2, aep1 }
 1ff:	f7c2fc0eff 	{ ae_lq32f.xu	aeq0, a14, a15; ae_mulafq32sp24s.l	aeq3, aeq2, aep0 }
 204:	08fb24        	ae_mulafq32sp24s.l	aeq3, aeq0, aep7
 207:	06b834        	ae_roundsq32sym	aeq2, aeq3
 20a:	622104        	ae_sq32f.i	aeq2, a1, 0
 20d:	0128           	l32i.n	a2, a1, 0
 20f:	f01d           	retw.n

00000211 <IN_LINE_32b_5ord+0xd9>:
 211:	000000                                        ...

00000214 <skew_ctrl_init>:
 214:	004136        	entry	a1, 32
 217:	055252        	s16i	a5, a2, 10
 21a:	461c           	movi.n	a6, 20
 21c:	623d           	ae_s16i.n	a3, a2, 0
 21e:	075242        	s16i	a4, a2, 14
 221:	170c           	movi.n	a7, 1
 223:	025272        	s16i	a7, a2, 4
 226:	040c           	movi.n	a4, 0
 228:	045242        	s16i	a4, a2, 8
 22b:	065242        	s16i	a4, a2, 12
 22e:	035242        	s16i	a4, a2, 6
 231:	415242        	s16i	a4, a2, 130
 234:	425242        	s16i	a4, a2, 132
 237:	405242        	s16i	a4, a2, 128
 23a:	203440        	or	a3, a4, a4
 23d:	08a676        	loopgtz	a6, 249 <skew_ctrl_init+0x35>
 240:	a06320        	addx4	a6, a3, a2
 243:	c649           	s32i.n	a4, a6, 48
 245:	331b           	addi.n	a3, a3, 1
 247:	933d           	ae_zext16	a3, a3

00000249 <skew_ctrl_init+0x35>:
 249:	f01d           	retw.n

0000024b <skew_ctrl_init+0x37>:
	...

0000024c <skew_ctrl_set_input_framesize>:
 24c:	00c136        	entry	a1, 96
 24f:	528d           	ae_l16ui.n	a8, a2, 2
 251:	0d0c           	movi.n	a13, 0
 253:	021252        	l16ui	a5, a2, 4
 256:	c08830        	sub	a8, a8, a3
 259:	176816        	beqz	a8, 3d3 <skew_ctrl_set_input_framesize+0x187>
 25c:	416230        	srli	a6, a3, 2
 25f:	417130        	srli	a7, a3, 1
 262:	723d           	ae_s16i.n	a3, a2, 2
 264:	428d           	ae_l16ui.n	a8, a2, 0
 266:	071292        	l16ui	a9, a2, 14
 269:	040c           	movi.n	a4, 0
 26b:	0a0c           	movi.n	a10, 0
 26d:	0b0c           	movi.n	a11, 0
 26f:	0c0c           	movi.n	a12, 0
 271:	0f0c           	movi.n	a15, 0
 273:	71f9           	s32i.n	a15, a1, 28
 275:	81c9           	s32i.n	a12, a1, 32
 277:	42c9           	s32i.n	a12, a2, 16
 279:	31b9           	s32i.n	a11, a1, 12
 27b:	91a9           	s32i.n	a10, a1, 36
 27d:	5149           	s32i.n	a4, a1, 20
 27f:	f7c992        	addi	a9, a9, -9
 282:	e0c882        	addi	a8, a8, -32
 285:	030c           	movi.n	a3, 0
 287:	6139           	s32i.n	a3, a1, 24
 289:	936780        	movnez	a6, a7, a8
 28c:	825560        	mull	a5, a5, a6
 28f:	080c           	movi.n	a8, 0
 291:	a189           	s32i.n	a8, a1, 40
 293:	060c           	movi.n	a6, 0
 295:	751b           	addi.n	a7, a5, 1
 297:	1145e0        	slli	a4, a5, 2
 29a:	4169           	s32i.n	a6, a1, 16
 29c:	1135d0        	slli	a3, a5, 3
 29f:	650b           	addi.n	a6, a5, -1
 2a1:	a30b           	addi.n	a10, a3, -1
 2a3:	b31b           	addi.n	a11, a3, 1
 2a5:	c40b           	addi.n	a12, a4, -1
 2a7:	e41b           	addi.n	a14, a4, 1
 2a9:	b1e9           	s32i.n	a14, a1, 44
 2ab:	c1c9           	s32i.n	a12, a1, 48
 2ad:	d1b9           	s32i.n	a11, a1, 52
 2af:	e1a9           	s32i.n	a10, a1, 56
 2b1:	09f956        	bnez	a9, 354 <skew_ctrl_set_input_framesize+0x108>
 2b4:	01a620        	slli	a10, a6, 30
 2b7:	41b260        	srli	a11, a6, 2
 2ba:	ff6781        	l32r	a8, 58 (af0 <__udivdi3>)
 2bd:	81d8           	l32i.n	a13, a1, 32
 2bf:	05cd           	mov.n	a12, a5
 2c1:	0008e0        	callx8	a8
 2c4:	81d8           	l32i.n	a13, a1, 32
 2c6:	05cd           	mov.n	a12, a5
 2c8:	62a9           	s32i.n	a10, a2, 24
 2ca:	019720        	slli	a9, a7, 30
 2cd:	41b270        	srli	a11, a7, 2
 2d0:	ff6281        	l32r	a8, 58 (af0 <__udivdi3>)
 2d3:	0a7d           	mov.n	a7, a10
 2d5:	09ad           	mov.n	a10, a9
 2d7:	0008e0        	callx8	a8
 2da:	91d8           	l32i.n	a13, a1, 36
 2dc:	04cd           	mov.n	a12, a4
 2de:	ff5e81        	l32r	a8, 58 (af0 <__udivdi3>)
 2e1:	c1b8           	l32i.n	a11, a1, 48
 2e3:	0a6d           	mov.n	a6, a10
 2e5:	92a9           	s32i.n	a10, a2, 36
 2e7:	019b20        	slli	a9, a11, 30
 2ea:	41b2b0        	srli	a11, a11, 2
 2ed:	09ad           	mov.n	a10, a9
 2ef:	0008e0        	callx8	a8
 2f2:	91d8           	l32i.n	a13, a1, 36
 2f4:	04cd           	mov.n	a12, a4
 2f6:	ff5881        	l32r	a8, 58 (af0 <__udivdi3>)
 2f9:	b1b8           	l32i.n	a11, a1, 44
 2fb:	0a5d           	mov.n	a5, a10
 2fd:	72a9           	s32i.n	a10, a2, 28
 2ff:	019b20        	slli	a9, a11, 30
 302:	41b2b0        	srli	a11, a11, 2
 305:	09ad           	mov.n	a10, a9
 307:	0008e0        	callx8	a8
 30a:	a1d8           	l32i.n	a13, a1, 40
 30c:	03cd           	mov.n	a12, a3
 30e:	ff5281        	l32r	a8, 58 (af0 <__udivdi3>)
 311:	e1b8           	l32i.n	a11, a1, 56
 313:	0a4d           	mov.n	a4, a10
 315:	a2a9           	s32i.n	a10, a2, 40
 317:	019b20        	slli	a9, a11, 30
 31a:	41b2b0        	srli	a11, a11, 2
 31d:	09ad           	mov.n	a10, a9
 31f:	0008e0        	callx8	a8
 322:	a1d8           	l32i.n	a13, a1, 40
 324:	ff4d81        	l32r	a8, 58 (af0 <__udivdi3>)
 327:	d1b8           	l32i.n	a11, a1, 52
 329:	03cd           	mov.n	a12, a3
 32b:	82a9           	s32i.n	a10, a2, 32
 32d:	0a3d           	mov.n	a3, a10
 32f:	019b20        	slli	a9, a11, 30
 332:	41b2b0        	srli	a11, a11, 2
 335:	09ad           	mov.n	a10, a9
 337:	0008e0        	callx8	a8

0000033a <skew_ctrl_set_input_framesize+0xee>:
 33a:	ca0b           	addi.n	a12, a10, -1
 33c:	d40b           	addi.n	a13, a4, -1
 33e:	e60b           	addi.n	a14, a6, -1
 340:	f30b           	addi.n	a15, a3, -1
 342:	850b           	addi.n	a8, a5, -1
 344:	970b           	addi.n	a9, a7, -1
 346:	6299           	s32i.n	a9, a2, 24
 348:	7289           	s32i.n	a8, a2, 28
 34a:	82f9           	s32i.n	a15, a2, 32
 34c:	92e9           	s32i.n	a14, a2, 36
 34e:	a2d9           	s32i.n	a13, a2, 40
 350:	b2c9           	s32i.n	a12, a2, 44
 352:	f01d           	retw.n

00000354 <skew_ctrl_set_input_framesize+0x108>:
 354:	07cd           	mov.n	a12, a7
 356:	ff4081        	l32r	a8, 58 (af0 <__udivdi3>)
 359:	01a520        	slli	a10, a5, 30
 35c:	01a9           	s32i.n	a10, a1, 0
 35e:	415250        	srli	a5, a5, 2
 361:	05bd           	mov.n	a11, a5
 363:	0008e0        	callx8	a8
 366:	05bd           	mov.n	a11, a5
 368:	31d8           	l32i.n	a13, a1, 12
 36a:	0a7d           	mov.n	a7, a10
 36c:	62a9           	s32i.n	a10, a2, 24
 36e:	ff3a81        	l32r	a8, 58 (af0 <__udivdi3>)
 371:	06cd           	mov.n	a12, a6
 373:	01a8           	l32i.n	a10, a1, 0
 375:	0008e0        	callx8	a8
 378:	41d8           	l32i.n	a13, a1, 16
 37a:	b1c8           	l32i.n	a12, a1, 44
 37c:	ff3781        	l32r	a8, 58 (af0 <__udivdi3>)
 37f:	0a6d           	mov.n	a6, a10
 381:	92a9           	s32i.n	a10, a2, 36
 383:	019420        	slli	a9, a4, 30
 386:	1199           	s32i.n	a9, a1, 4
 388:	414240        	srli	a4, a4, 2
 38b:	04bd           	mov.n	a11, a4
 38d:	09ad           	mov.n	a10, a9
 38f:	0008e0        	callx8	a8
 392:	04bd           	mov.n	a11, a4
 394:	51d8           	l32i.n	a13, a1, 20
 396:	0a5d           	mov.n	a5, a10
 398:	72a9           	s32i.n	a10, a2, 28
 39a:	ff2f81        	l32r	a8, 58 (af0 <__udivdi3>)
 39d:	c1c8           	l32i.n	a12, a1, 48
 39f:	11a8           	l32i.n	a10, a1, 4
 3a1:	0008e0        	callx8	a8
 3a4:	61d8           	l32i.n	a13, a1, 24
 3a6:	d1c8           	l32i.n	a12, a1, 52
 3a8:	ff2c81        	l32r	a8, 58 (af0 <__udivdi3>)
 3ab:	0a4d           	mov.n	a4, a10
 3ad:	a2a9           	s32i.n	a10, a2, 40
 3af:	019320        	slli	a9, a3, 30
 3b2:	2199           	s32i.n	a9, a1, 8
 3b4:	413230        	srli	a3, a3, 2
 3b7:	03bd           	mov.n	a11, a3
 3b9:	09ad           	mov.n	a10, a9
 3bb:	0008e0        	callx8	a8
 3be:	71d8           	l32i.n	a13, a1, 28
 3c0:	e1c8           	l32i.n	a12, a1, 56
 3c2:	82a9           	s32i.n	a10, a2, 32
 3c4:	03bd           	mov.n	a11, a3
 3c6:	ff2481        	l32r	a8, 58 (af0 <__udivdi3>)
 3c9:	0a3d           	mov.n	a3, a10
 3cb:	21a8           	l32i.n	a10, a1, 8
 3cd:	0008e0        	callx8	a8
 3d0:	ffd986        	j	33a <skew_ctrl_set_input_framesize+0xee>

000003d3 <skew_ctrl_set_input_framesize+0x187>:
 3d3:	f01d           	retw.n

000003d5 <skew_ctrl_set_input_framesize+0x189>:
 3d5:	000000                                        ...

000003d8 <Get_Src_Len>:
 3d8:	004136        	entry	a1, 32
 3db:	061262        	l16ui	a6, a2, 12
 3de:	690c           	movi.n	a9, 6
 3e0:	480c           	movi.n	a8, 4
 3e2:	3c2626        	beqi	a6, 2, 422 <Get_Src_Len+0x4a>
 3e5:	570c           	movi.n	a7, 5
 3e7:	2d5626        	beqi	a6, 5, 418 <Get_Src_Len+0x40>
 3ea:	2a8626        	beqi	a6, 8, 418 <Get_Src_Len+0x40>
 3ed:	211626        	beqi	a6, 1, 412 <Get_Src_Len+0x3a>
 3f0:	184626        	beqi	a6, 4, 40c <Get_Src_Len+0x34>
 3f3:	157626        	beqi	a6, 7, 40c <Get_Src_Len+0x34>
 3f6:	fdc6b2        	addi	a11, a6, -3
 3f9:	429252        	l16si	a5, a2, 132
 3fc:	8a0c           	movi.n	a10, 8
 3fe:	83a8b0        	moveqz	a10, a8, a11
 401:	f255a0        	rems	a5, a5, a10
 404:	2015a6        	blti	a5, 1, 428 <Get_Src_Len+0x50>
 407:	637d           	ae_s16i.n	a7, a3, 0
 409:	0001c6        	j	414 <Get_Src_Len+0x3c>

0000040c <Get_Src_Len+0x34>:
 40c:	4292c2        	l16si	a12, a2, 132
 40f:	1c1ca6        	blti	a12, 1, 42f <Get_Src_Len+0x57>

00000412 <Get_Src_Len+0x3a>:
 412:	637d           	ae_s16i.n	a7, a3, 0

00000414 <Get_Src_Len+0x3c>:
 414:	647d           	ae_s16i.n	a7, a4, 0
 416:	f01d           	retw.n

00000418 <Get_Src_Len+0x40>:
 418:	4292d2        	l16si	a13, a2, 132
 41b:	003d96        	bltz	a13, 422 <Get_Src_Len+0x4a>
 41e:	078d           	mov.n	a8, a7
 420:	079d           	mov.n	a9, a7

00000422 <Get_Src_Len+0x4a>:
 422:	639d           	ae_s16i.n	a9, a3, 0
 424:	648d           	ae_s16i.n	a8, a4, 0
 426:	f01d           	retw.n

00000428 <Get_Src_Len+0x50>:
 428:	639d           	ae_s16i.n	a9, a3, 0
 42a:	087d           	mov.n	a7, a8
 42c:	fff906        	j	414 <Get_Src_Len+0x3c>

0000042f <Get_Src_Len+0x57>:
 42f:	639d           	ae_s16i.n	a9, a3, 0
 431:	087d           	mov.n	a7, a8
 433:	fff746        	j	414 <Get_Src_Len+0x3c>

00000436 <Get_Src_Len+0x5e>:
	...

00000438 <skew_ctrl_32b>:
 438:	008136        	entry	a1, 64
 43b:	580c           	movi.n	a8, 5
 43d:	2139           	s32i.n	a3, a1, 8
 43f:	059d           	mov.n	a9, a5
 441:	1199           	s32i.n	a9, a1, 4
 443:	043d           	mov.n	a3, a4
 445:	065d           	mov.n	a5, a6
 447:	2148           	l32i.n	a4, a1, 8
 449:	026d           	mov.n	a6, a2
 44b:	44c222        	addi	a2, a2, 68
 44e:	207440        	or	a7, a4, a4
 451:	06a876        	loopgtz	a8, 45b <skew_ctrl_32b+0x23>
 454:	0798           	l32i.n	a9, a7, 0
 456:	09e294        	ae_s32ip	a9, a2, 4
 459:	774b           	addi.n	a7, a7, 4

0000045b <skew_ctrl_32b+0x23>:
 45b:	06ad           	mov.n	a10, a6
 45d:	feff81        	l32r	a8, 5c (3d8 <Get_Src_Len>)
 460:	01bd           	mov.n	a11, a1
 462:	c12b           	addi.n	a12, a1, 2
 464:	0008e0        	callx8	a8
 467:	51ad           	ae_l16ui.n	a10, a1, 2
 469:	051692        	l16ui	a9, a6, 10
 46c:	417d           	ae_l16ui.n	a7, a1, 0
 46e:	032d           	mov.n	a2, a3
 470:	10c482        	addi	a8, a4, 16
 473:	fefb31        	l32r	a3, 60 (3fffffff <_end+0x3fffef33>)
 476:	3189           	s32i.n	a8, a1, 12
 478:	a07760        	addx4	a7, a7, a6
 47b:	30c772        	addi	a7, a7, 48
 47e:	622966        	bnei	a9, 2, 4e4 <skew_ctrl_32b+0xac>
 481:	5698           	l32i.n	a9, a6, 20
 483:	46b8           	l32i.n	a11, a6, 16
 485:	51b9           	s32i.n	a11, a1, 20
 487:	4199           	s32i.n	a9, a1, 16
 489:	5aac           	beqz.n	a10, 4b2 <skew_ctrl_32b+0x7a>
 48b:	040c           	movi.n	a4, 0

0000048d <skew_ctrl_32b+0x55>:
 48d:	41c8           	l32i.n	a12, a1, 16
 48f:	51b8           	l32i.n	a11, a1, 20
 491:	cbca           	add.n	a12, a11, a12
 493:	15dec0        	extui	a13, a12, 30, 2
 496:	10bc30        	and	a11, a12, a3
 499:	51b9           	s32i.n	a11, a1, 20
 49b:	a0dd70        	addx4	a13, a13, a7
 49e:	937d40        	movnez	a7, a13, a4
 4a1:	07ad           	mov.n	a10, a7
 4a3:	ffc965        	call8	138 <IN_LINE_32b_5ord>
 4a6:	09e2a4        	ae_s32ip	a10, a2, 4
 4a9:	51ad           	ae_l16ui.n	a10, a1, 2
 4ab:	441b           	addi.n	a4, a4, 1
 4ad:	944d           	ae_zext16	a4, a4
 4af:	da34a7        	bltu	a4, a10, 48d <skew_ctrl_32b+0x55>

000004b2 <skew_ctrl_32b+0x7a>:
 4b2:	0a4d           	mov.n	a4, a10
 4b4:	3178           	l32i.n	a7, a1, 12
 4b6:	1fba57        	bgeu	a10, a5, 4d9 <skew_ctrl_32b+0xa1>

000004b9 <skew_ctrl_32b+0x81>:
 4b9:	4188           	l32i.n	a8, a1, 16
 4bb:	51b8           	l32i.n	a11, a1, 20
 4bd:	8b8a           	add.n	a8, a11, a8
 4bf:	159e80        	extui	a9, a8, 30, 2
 4c2:	10b830        	and	a11, a8, a3
 4c5:	51b9           	s32i.n	a11, a1, 20
 4c7:	a07970        	addx4	a7, a9, a7
 4ca:	07ad           	mov.n	a10, a7
 4cc:	ffc6a5        	call8	138 <IN_LINE_32b_5ord>
 4cf:	09e2a4        	ae_s32ip	a10, a2, 4
 4d2:	441b           	addi.n	a4, a4, 1
 4d4:	944d           	ae_zext16	a4, a4
 4d6:	df3457        	bltu	a4, a5, 4b9 <skew_ctrl_32b+0x81>

000004d9 <skew_ctrl_32b+0xa1>:
 4d9:	4188           	l32i.n	a8, a1, 16
 4db:	5198           	l32i.n	a9, a1, 20
 4dd:	4699           	s32i.n	a9, a6, 16
 4df:	5689           	s32i.n	a8, a6, 20
 4e1:	001746        	j	542 <skew_ctrl_32b+0x10a>

000004e4 <skew_ctrl_32b+0xac>:
 4e4:	caac           	beqz.n	a10, 514 <skew_ctrl_32b+0xdc>
 4e6:	040c           	movi.n	a4, 0

000004e8 <skew_ctrl_32b+0xb0>:
 4e8:	06ad           	mov.n	a10, a6
 4ea:	56d8           	l32i.n	a13, a6, 20
 4ec:	46b8           	l32i.n	a11, a6, 16
 4ee:	1c0c           	movi.n	a12, 1
 4f0:	fedd81        	l32r	a8, 64 (a34 <intp_filter_32b>)
 4f3:	bbda           	add.n	a11, a11, a13
 4f5:	159eb0        	extui	a9, a11, 30, 2
 4f8:	a09970        	addx4	a9, a9, a7
 4fb:	10bb30        	and	a11, a11, a3
 4fe:	46b9           	s32i.n	a11, a6, 16
 500:	937940        	movnez	a7, a9, a4
 503:	07bd           	mov.n	a11, a7
 505:	0008e0        	callx8	a8
 508:	09e2a4        	ae_s32ip	a10, a2, 4
 50b:	51ad           	ae_l16ui.n	a10, a1, 2
 50d:	441b           	addi.n	a4, a4, 1
 50f:	944d           	ae_zext16	a4, a4
 511:	d334a7        	bltu	a4, a10, 4e8 <skew_ctrl_32b+0xb0>

00000514 <skew_ctrl_32b+0xdc>:
 514:	0a4d           	mov.n	a4, a10
 516:	3178           	l32i.n	a7, a1, 12
 518:	26ba57        	bgeu	a10, a5, 542 <skew_ctrl_32b+0x10a>

0000051b <skew_ctrl_32b+0xe3>:
 51b:	06ad           	mov.n	a10, a6
 51d:	56d8           	l32i.n	a13, a6, 20
 51f:	46b8           	l32i.n	a11, a6, 16
 521:	1c0c           	movi.n	a12, 1
 523:	fed081        	l32r	a8, 64 (a34 <intp_filter_32b>)
 526:	bbda           	add.n	a11, a11, a13
 528:	159eb0        	extui	a9, a11, 30, 2
 52b:	10bb30        	and	a11, a11, a3
 52e:	a07970        	addx4	a7, a9, a7
 531:	46b9           	s32i.n	a11, a6, 16
 533:	07bd           	mov.n	a11, a7
 535:	0008e0        	callx8	a8
 538:	09e2a4        	ae_s32ip	a10, a2, 4
 53b:	441b           	addi.n	a4, a4, 1
 53d:	944d           	ae_zext16	a4, a4
 53f:	d83457        	bltu	a4, a5, 51b <skew_ctrl_32b+0xe3>
 542:	2188           	l32i.n	a8, a1, 8
 544:	1178           	l32i.n	a7, a1, 4
 546:	30c622        	addi	a2, a6, 48
 549:	a07780        	addx4	a7, a7, a8
 54c:	ecc772        	addi	a7, a7, -20
 54f:	580c           	movi.n	a8, 5
 551:	06a876        	loopgtz	a8, 55b <skew_ctrl_32b+0x123>
 554:	0798           	l32i.n	a9, a7, 0
 556:	09e294        	ae_s32ip	a9, a2, 4
 559:	774b           	addi.n	a7, a7, 4

0000055b <skew_ctrl_32b+0x123>:
 55b:	f01d           	retw.n

0000055d <skew_ctrl_32b+0x125>:
 55d:	000000                                        ...

00000560 <skew_ctrl_16b>:
 560:	006136        	entry	a1, 48
 563:	2139           	s32i.n	a3, a1, 8
 565:	037d           	mov.n	a7, a3
 567:	1159           	s32i.n	a5, a1, 4
 569:	580c           	movi.n	a8, 5
 56b:	065d           	mov.n	a5, a6
 56d:	026d           	mov.n	a6, a2
 56f:	3ac222        	addi	a2, a2, 58
 572:	06a876        	loopgtz	a8, 57c <skew_ctrl_16b+0x1c>
 575:	27ad           	ae_l16si.n	a10, a7, 0
 577:	09c2a4        	ae_s16ip	a10, a2, 2
 57a:	772b           	addi.n	a7, a7, 2

0000057c <skew_ctrl_16b+0x1c>:
 57c:	06ad           	mov.n	a10, a6
 57e:	feb781        	l32r	a8, 5c (3d8 <Get_Src_Len>)
 581:	01bd           	mov.n	a11, a1
 583:	c12b           	addi.n	a12, a1, 2
 585:	0008e0        	callx8	a8
 588:	feb631        	l32r	a3, 60 (3fffffff <_end+0x3fffef33>)
 58b:	417d           	ae_l16ui.n	a7, a1, 0
 58d:	042d           	mov.n	a2, a4
 58f:	51ad           	ae_l16ui.n	a10, a1, 2
 591:	907760        	addx2	a7, a7, a6
 594:	30c772        	addi	a7, a7, 48
 597:	caac           	beqz.n	a10, 5c7 <skew_ctrl_16b+0x67>
 599:	040c           	movi.n	a4, 0

0000059b <skew_ctrl_16b+0x3b>:
 59b:	06ad           	mov.n	a10, a6
 59d:	56d8           	l32i.n	a13, a6, 20
 59f:	46b8           	l32i.n	a11, a6, 16
 5a1:	1c0c           	movi.n	a12, 1
 5a3:	feb181        	l32r	a8, 68 (8b0 <intp_filter_16b>)
 5a6:	bbda           	add.n	a11, a11, a13
 5a8:	159eb0        	extui	a9, a11, 30, 2
 5ab:	909970        	addx2	a9, a9, a7
 5ae:	10bb30        	and	a11, a11, a3
 5b1:	46b9           	s32i.n	a11, a6, 16
 5b3:	937940        	movnez	a7, a9, a4
 5b6:	07bd           	mov.n	a11, a7
 5b8:	0008e0        	callx8	a8
 5bb:	09c2a4        	ae_s16ip	a10, a2, 2
 5be:	51ad           	ae_l16ui.n	a10, a1, 2
 5c0:	441b           	addi.n	a4, a4, 1
 5c2:	944d           	ae_zext16	a4, a4
 5c4:	d334a7        	bltu	a4, a10, 59b <skew_ctrl_16b+0x3b>

000005c7 <skew_ctrl_16b+0x67>:
 5c7:	2178           	l32i.n	a7, a1, 8
 5c9:	0a4d           	mov.n	a4, a10
 5cb:	778b           	addi.n	a7, a7, 8
 5cd:	26ba57        	bgeu	a10, a5, 5f7 <skew_ctrl_16b+0x97>

000005d0 <skew_ctrl_16b+0x70>:
 5d0:	06ad           	mov.n	a10, a6
 5d2:	56d8           	l32i.n	a13, a6, 20
 5d4:	46b8           	l32i.n	a11, a6, 16
 5d6:	1c0c           	movi.n	a12, 1
 5d8:	fea481        	l32r	a8, 68 (8b0 <intp_filter_16b>)
 5db:	bbda           	add.n	a11, a11, a13
 5dd:	159eb0        	extui	a9, a11, 30, 2
 5e0:	10bb30        	and	a11, a11, a3
 5e3:	907970        	addx2	a7, a9, a7
 5e6:	46b9           	s32i.n	a11, a6, 16
 5e8:	07bd           	mov.n	a11, a7
 5ea:	0008e0        	callx8	a8
 5ed:	09c2a4        	ae_s16ip	a10, a2, 2
 5f0:	441b           	addi.n	a4, a4, 1
 5f2:	944d           	ae_zext16	a4, a4
 5f4:	d83457        	bltu	a4, a5, 5d0 <skew_ctrl_16b+0x70>

000005f7 <skew_ctrl_16b+0x97>:
 5f7:	2188           	l32i.n	a8, a1, 8
 5f9:	1178           	l32i.n	a7, a1, 4
 5fb:	30c622        	addi	a2, a6, 48
 5fe:	907780        	addx2	a7, a7, a8
 601:	f6c772        	addi	a7, a7, -10
 604:	580c           	movi.n	a8, 5
 606:	06a876        	loopgtz	a8, 610 <skew_ctrl_16b+0xb0>
 609:	279d           	ae_l16si.n	a9, a7, 0
 60b:	09c294        	ae_s16ip	a9, a2, 2
 60e:	772b           	addi.n	a7, a7, 2

00000610 <skew_ctrl_16b+0xb0>:
 610:	f01d           	retw.n

00000612 <skew_ctrl_16b+0xb2>:
	...

00000614 <Check_Phase_Frac>:
 614:	004136        	entry	a1, 32
 617:	236d           	ae_l16si.n	a6, a3, 0
 619:	450c           	movi.n	a5, 4
 61b:	820b           	addi.n	a8, a2, -1
 61d:	820c           	movi.n	a2, 8
 61f:	832580        	moveqz	a2, a5, a8
 622:	420b           	addi.n	a4, a2, -1
 624:	0f8647        	bany	a6, a4, 637 <Check_Phase_Frac+0x23>
 627:	609160        	abs	a9, a6
 62a:	899d           	ae_sext16	a9, a9
 62c:	0b2927        	blt	a9, a2, 63b <Check_Phase_Frac+0x27>
 62f:	120c           	movi.n	a2, 1
 631:	0a0c           	movi.n	a10, 0
 633:	63ad           	ae_s16i.n	a10, a3, 0
 635:	f01d           	retw.n

00000637 <Check_Phase_Frac+0x23>:
 637:	020c           	movi.n	a2, 0
 639:	f01d           	retw.n

0000063b <Check_Phase_Frac+0x27>:
 63b:	020c           	movi.n	a2, 0
 63d:	f01d           	retw.n

0000063f <Check_Phase_Frac+0x2b>:
	...

00000640 <skew_ctrl_process>:
 640:	006136        	entry	a1, 48
 643:	065272        	s16i	a7, a2, 12
 646:	079d           	mov.n	a9, a7
 648:	2a0c           	movi.n	a10, 2
 64a:	21a9           	s32i.n	a10, a1, 8
 64c:	890b           	addi.n	a8, a9, -1
 64e:	070c           	movi.n	a7, 0
 650:	1a7816        	beqz	a8, 7fb <skew_ctrl_process+0x1bb>
 653:	fcc9b2        	addi	a11, a9, -4
 656:	20ab16        	beqz	a11, 864 <skew_ctrl_process+0x224>
 659:	f9c9c2        	addi	a12, a9, -7
 65c:	1a3c16        	beqz	a12, 803 <skew_ctrl_process+0x1c3>
 65f:	fec9d2        	addi	a13, a9, -2
 662:	208d16        	beqz	a13, 86e <skew_ctrl_process+0x22e>
 665:	fbc9e2        	addi	a14, a9, -5
 668:	20ae16        	beqz	a14, 876 <skew_ctrl_process+0x236>
 66b:	f8c9f2        	addi	a15, a9, -8
 66e:	20ef16        	beqz	a15, 880 <skew_ctrl_process+0x240>
 671:	405272        	s16i	a7, a2, 128
 674:	fe73a1        	l32r	a10, 40 (40000000 <_end+0x3fffef34>)

00000677 <skew_ctrl_process+0x37>:
 677:	52a9           	s32i.n	a10, a2, 20
 679:	528d           	ae_l16ui.n	a8, a2, 2
 67b:	1189           	s32i.n	a8, a1, 4
 67d:	0189           	s32i.n	a8, a1, 0
 67f:	079916        	beqz	a9, 6fc <skew_ctrl_process+0xbc>
 682:	82a0c2        	movi	a12, 130
 685:	0712a2        	l16ui	a10, a2, 14
 688:	42ed           	ae_l16ui.n	a14, a2, 0
 68a:	2d0c           	movi.n	a13, 2
 68c:	4b0c           	movi.n	a11, 4
 68e:	e0cee2        	addi	a14, a14, -32
 691:	93bde0        	movnez	a11, a13, a14
 694:	31b9           	s32i.n	a11, a1, 12
 696:	3f9a66        	bnei	a10, 10, 6d9 <skew_ctrl_process+0x99>
 699:	f90b           	addi.n	a15, a9, -1
 69b:	204f16        	beqz	a15, 8a3 <skew_ctrl_process+0x263>
 69e:	fcc982        	addi	a8, a9, -4
 6a1:	1e5816        	beqz	a8, 88a <skew_ctrl_process+0x24a>
 6a4:	f9c9a2        	addi	a10, a9, -7
 6a7:	1dfa16        	beqz	a10, 88a <skew_ctrl_process+0x24a>
 6aa:	1d2926        	beqi	a9, 2, 6cb <skew_ctrl_process+0x8b>
 6ad:	025926        	beqi	a9, 5, 6b3 <skew_ctrl_process+0x73>
 6b0:	488966        	bnei	a9, 8, 6fc <skew_ctrl_process+0xbc>

000006b3 <skew_ctrl_process+0x73>:
 6b3:	b2ca           	add.n	a11, a2, a12
 6b5:	419292        	l16si	a9, a2, 130
 6b8:	4092a2        	l16si	a10, a2, 128
 6bb:	fe6c81        	l32r	a8, 6c (614 <Check_Phase_Frac>)
 6be:	990b           	addi.n	a9, a9, -1
 6c0:	415292        	s16i	a9, a2, 130
 6c3:	425292        	s16i	a9, a2, 132
 6c6:	0008e0        	callx8	a8
 6c9:	faac           	beqz.n	a10, 6fc <skew_ctrl_process+0xbc>

000006cb <skew_ctrl_process+0x8b>:
 6cb:	31b8           	l32i.n	a11, a1, 12
 6cd:	11a8           	l32i.n	a10, a1, 4
 6cf:	c0bab0        	sub	a11, a10, a11
 6d2:	9bad           	ae_zext16	a10, a11
 6d4:	11a9           	s32i.n	a10, a1, 4
 6d6:	000886        	j	6fc <skew_ctrl_process+0xbc>

000006d9 <skew_ctrl_process+0x99>:
 6d9:	d90b           	addi.n	a13, a9, -1
 6db:	0c3d16        	beqz	a13, 7a2 <skew_ctrl_process+0x162>
 6de:	fcc9e2        	addi	a14, a9, -4
 6e1:	0a4e16        	beqz	a14, 789 <skew_ctrl_process+0x149>
 6e4:	f9c9f2        	addi	a15, a9, -7
 6e7:	09ef16        	beqz	a15, 789 <skew_ctrl_process+0x149>
 6ea:	fec982        	addi	a8, a9, -2
 6ed:	166816        	beqz	a8, 857 <skew_ctrl_process+0x217>
 6f0:	fbc9a2        	addi	a10, a9, -5
 6f3:	147a16        	beqz	a10, 83e <skew_ctrl_process+0x1fe>
 6f6:	f8c9b2        	addi	a11, a9, -8
 6f9:	141b16        	beqz	a11, 83e <skew_ctrl_process+0x1fe>

000006fc <skew_ctrl_process+0xbc>:
 6fc:	11e8           	l32i.n	a14, a1, 4
 6fe:	01a8           	l32i.n	a10, a1, 0
 700:	64ad           	ae_s16i.n	a10, a4, 0
 702:	66ed           	ae_s16i.n	a14, a6, 0
 704:	0612c2        	l16ui	a12, a2, 12
 707:	42bd           	ae_l16ui.n	a11, a2, 0
 709:	41f1e0        	srli	a15, a14, 1
 70c:	4191a0        	srli	a9, a10, 1
 70f:	41e2e0        	srli	a14, a14, 2
 712:	41a2a0        	srli	a10, a10, 2
 715:	e0cb82        	addi	a8, a11, -32
 718:	93a980        	movnez	a10, a9, a8
 71b:	93ef80        	movnez	a14, a15, a8
 71e:	9eed           	ae_zext16	a14, a14
 720:	9ccc           	bnez.n	a12, 72d <skew_ctrl_process+0xed>
 722:	051292        	l16ui	a9, a2, 10
 725:	087956        	bnez	a9, 7b0 <skew_ctrl_process+0x170>
 728:	090c           	movi.n	a9, 0
 72a:	0022c6        	j	7b9 <skew_ctrl_process+0x179>

0000072d <skew_ctrl_process+0xed>:
 72d:	9add           	ae_zext16	a13, a10
 72f:	0ecb66        	bnei	a11, 32, 741 <skew_ctrl_process+0x101>
 732:	05cd           	mov.n	a12, a5
 734:	fe4f81        	l32r	a8, 70 (438 <skew_ctrl_32b>)
 737:	03bd           	mov.n	a11, a3
 739:	02ad           	mov.n	a10, a2
 73b:	0008e0        	callx8	a8
 73e:	0002c6        	j	74d <skew_ctrl_process+0x10d>

00000741 <skew_ctrl_process+0x101>:
 741:	05cd           	mov.n	a12, a5
 743:	fe4c81        	l32r	a8, 74 (560 <skew_ctrl_16b>)
 746:	03bd           	mov.n	a11, a3
 748:	02ad           	mov.n	a10, a2
 74a:	0008e0        	callx8	a8

0000074d <skew_ctrl_process+0x10d>:
 74d:	409292        	l16si	a9, a2, 128
 750:	4192d2        	l16si	a13, a2, 130
 753:	8b0c           	movi.n	a11, 8
 755:	041966        	bnei	a9, 1, 75d <skew_ctrl_process+0x11d>
 758:	4a0c           	movi.n	a10, 4
 75a:	000206        	j	766 <skew_ctrl_process+0x126>

0000075d <skew_ctrl_process+0x11d>:
 75d:	fec9c2        	addi	a12, a9, -2
 760:	80a0a2        	movi	a10, 128
 763:	83abc0        	moveqz	a10, a11, a12

00000766 <skew_ctrl_process+0x126>:
 766:	8aad           	ae_sext16	a10, a10
 768:	ea0b           	addi.n	a14, a10, -1
 76a:	098de7        	bany	a13, a14, 777 <skew_ctrl_process+0x137>
 76d:	4279           	s32i.n	a7, a2, 16
 76f:	425272        	s16i	a7, a2, 132
 772:	020c           	movi.n	a2, 0
 774:	000086        	j	77a <skew_ctrl_process+0x13a>

00000777 <skew_ctrl_process+0x137>:
 777:	429222        	l16si	a2, a2, 132

0000077a <skew_ctrl_process+0x13a>:
 77a:	60f120        	abs	a15, a2
 77d:	8ffd           	ae_sext16	a15, a15
 77f:	042fa7        	blt	a15, a10, 787 <skew_ctrl_process+0x147>
 782:	020c           	movi.n	a2, 0
 784:	f01d           	retw.n

00000786 <skew_ctrl_process+0x146>:
	...

00000787 <skew_ctrl_process+0x147>:
 787:	f01d           	retw.n

00000789 <skew_ctrl_process+0x149>:
 789:	b2ca           	add.n	a11, a2, a12
 78b:	419292        	l16si	a9, a2, 130
 78e:	4092a2        	l16si	a10, a2, 128
 791:	fe3681        	l32r	a8, 6c (614 <Check_Phase_Frac>)
 794:	991b           	addi.n	a9, a9, 1
 796:	415292        	s16i	a9, a2, 130
 799:	425292        	s16i	a9, a2, 132
 79c:	0008e0        	callx8	a8
 79f:	f59a16        	beqz	a10, 6fc <skew_ctrl_process+0xbc>

000007a2 <skew_ctrl_process+0x162>:
 7a2:	31b8           	l32i.n	a11, a1, 12
 7a4:	01a8           	l32i.n	a10, a1, 0
 7a6:	c0bab0        	sub	a11, a10, a11
 7a9:	9bad           	ae_zext16	a10, a11
 7ab:	01a9           	s32i.n	a10, a1, 0
 7ad:	ffd2c6        	j	6fc <skew_ctrl_process+0xbc>

000007b0 <skew_ctrl_process+0x170>:
 7b0:	d90b           	addi.n	a13, a9, -1
 7b2:	2198           	l32i.n	a9, a1, 8
 7b4:	1c0c           	movi.n	a12, 1
 7b6:	839cd0        	moveqz	a9, a12, a13

000007b9 <skew_ctrl_process+0x179>:
 7b9:	89ad           	ae_sext16	a10, a9
 7bb:	c0ce90        	sub	a12, a14, a9
 7be:	8ccd           	ae_sext16	a12, a12
 7c0:	30c292        	addi	a9, a2, 48
 7c3:	44cb66        	bnei	a11, 32, 80b <skew_ctrl_process+0x1cb>
 7c6:	09bd           	mov.n	a11, a9
 7c8:	11dae0        	slli	a13, a10, 2
 7cb:	c099d0        	sub	a9, a9, a13
 7ce:	14c992        	addi	a9, a9, 20
 7d1:	06aa76        	loopgtz	a10, 7db <skew_ctrl_process+0x19b>
 7d4:	09a8           	l32i.n	a10, a9, 0
 7d6:	09e5a4        	ae_s32ip	a10, a5, 4
 7d9:	994b           	addi.n	a9, a9, 4
 7db:	06ac76        	loopgtz	a12, 7e5 <skew_ctrl_process+0x1a5>
 7de:	03c8           	l32i.n	a12, a3, 0
 7e0:	09e5c4        	ae_s32ip	a12, a5, 4
 7e3:	334b           	addi.n	a3, a3, 4

000007e5 <skew_ctrl_process+0x1a5>:
 7e5:	5e0c           	movi.n	a14, 5
 7e7:	eccdf2        	addi	a15, a13, -20
 7ea:	8033f0        	add	a3, a3, a15
 7ed:	06ae76        	loopgtz	a14, 7f7 <skew_ctrl_process+0x1b7>
 7f0:	0388           	l32i.n	a8, a3, 0
 7f2:	09eb84        	ae_s32ip	a8, a11, 4
 7f5:	334b           	addi.n	a3, a3, 4
 7f7:	ffd486        	j	74d <skew_ctrl_process+0x10d>

000007fa <skew_ctrl_process+0x1ba>:
	...

000007fb <skew_ctrl_process+0x1bb>:
 7fb:	405272        	s16i	a7, a2, 128
 7fe:	62a8           	l32i.n	a10, a2, 24
 800:	ff9cc6        	j	677 <skew_ctrl_process+0x37>

00000803 <skew_ctrl_process+0x1c3>:
 803:	4052a2        	s16i	a10, a2, 128
 806:	82a8           	l32i.n	a10, a2, 32
 808:	ff9ac6        	j	677 <skew_ctrl_process+0x37>

0000080b <skew_ctrl_process+0x1cb>:
 80b:	09bd           	mov.n	a11, a9
 80d:	11daf0        	slli	a13, a10, 1
 810:	c099d0        	sub	a9, a9, a13
 813:	99ab           	addi.n	a9, a9, 10
 815:	06aa76        	loopgtz	a10, 81f <skew_ctrl_process+0x1df>
 818:	29ad           	ae_l16si.n	a10, a9, 0
 81a:	09c5a4        	ae_s16ip	a10, a5, 2
 81d:	992b           	addi.n	a9, a9, 2
 81f:	06ac76        	loopgtz	a12, 829 <skew_ctrl_process+0x1e9>
 822:	23cd           	ae_l16si.n	a12, a3, 0
 824:	09c5c4        	ae_s16ip	a12, a5, 2
 827:	332b           	addi.n	a3, a3, 2

00000829 <skew_ctrl_process+0x1e9>:
 829:	5e0c           	movi.n	a14, 5
 82b:	f6cdf2        	addi	a15, a13, -10
 82e:	8033f0        	add	a3, a3, a15
 831:	06ae76        	loopgtz	a14, 83b <skew_ctrl_process+0x1fb>
 834:	238d           	ae_l16si.n	a8, a3, 0
 836:	09cb84        	ae_s16ip	a8, a11, 2
 839:	332b           	addi.n	a3, a3, 2
 83b:	ffc386        	j	74d <skew_ctrl_process+0x10d>

0000083e <skew_ctrl_process+0x1fe>:
 83e:	b2ca           	add.n	a11, a2, a12
 840:	419292        	l16si	a9, a2, 130
 843:	4092a2        	l16si	a10, a2, 128
 846:	fe0981        	l32r	a8, 6c (614 <Check_Phase_Frac>)
 849:	990b           	addi.n	a9, a9, -1
 84b:	415292        	s16i	a9, a2, 130
 84e:	425292        	s16i	a9, a2, 132
 851:	0008e0        	callx8	a8
 854:	ea4a16        	beqz	a10, 6fc <skew_ctrl_process+0xbc>

00000857 <skew_ctrl_process+0x217>:
 857:	31b8           	l32i.n	a11, a1, 12
 859:	01a8           	l32i.n	a10, a1, 0
 85b:	baba           	add.n	a11, a10, a11
 85d:	9bad           	ae_zext16	a10, a11
 85f:	01a9           	s32i.n	a10, a1, 0
 861:	ffa5c6        	j	6fc <skew_ctrl_process+0xbc>

00000864 <skew_ctrl_process+0x224>:
 864:	72a8           	l32i.n	a10, a2, 28
 866:	1c0c           	movi.n	a12, 1
 868:	4052c2        	s16i	a12, a2, 128
 86b:	ff8206        	j	677 <skew_ctrl_process+0x37>

0000086e <skew_ctrl_process+0x22e>:
 86e:	405272        	s16i	a7, a2, 128
 871:	92a8           	l32i.n	a10, a2, 36
 873:	ff8006        	j	677 <skew_ctrl_process+0x37>

00000876 <skew_ctrl_process+0x236>:
 876:	a2a8           	l32i.n	a10, a2, 40
 878:	1d0c           	movi.n	a13, 1
 87a:	4052d2        	s16i	a13, a2, 128
 87d:	ff7d86        	j	677 <skew_ctrl_process+0x37>

00000880 <skew_ctrl_process+0x240>:
 880:	0aed           	mov.n	a14, a10
 882:	4052e2        	s16i	a14, a2, 128
 885:	b2a8           	l32i.n	a10, a2, 44
 887:	ff7b06        	j	677 <skew_ctrl_process+0x37>

0000088a <skew_ctrl_process+0x24a>:
 88a:	b2ca           	add.n	a11, a2, a12
 88c:	419292        	l16si	a9, a2, 130
 88f:	4092a2        	l16si	a10, a2, 128
 892:	fdf681        	l32r	a8, 6c (614 <Check_Phase_Frac>)
 895:	991b           	addi.n	a9, a9, 1
 897:	415292        	s16i	a9, a2, 130
 89a:	425292        	s16i	a9, a2, 132
 89d:	0008e0        	callx8	a8
 8a0:	e58a16        	beqz	a10, 6fc <skew_ctrl_process+0xbc>

000008a3 <skew_ctrl_process+0x263>:
 8a3:	31b8           	l32i.n	a11, a1, 12
 8a5:	11a8           	l32i.n	a10, a1, 4
 8a7:	baba           	add.n	a11, a10, a11
 8a9:	9bad           	ae_zext16	a10, a11
 8ab:	11a9           	s32i.n	a10, a1, 4
 8ad:	ff92c6        	j	6fc <skew_ctrl_process+0xbc>

000008b0 <intp_filter_16b>:
 8b0:	008136        	entry	a1, 64
 8b3:	051282        	l16ui	a8, a2, 10
 8b6:	4258           	l32i.n	a5, a2, 16
 8b8:	1194f0        	slli	a9, a4, 1
 8bb:	892d           	ae_sext16	a2, a9
 8bd:	1155f0        	slli	a5, a5, 1
 8c0:	0159           	s32i.n	a5, a1, 0
 8c2:	500104        	ae_lp24f.i	aep0, a1, 0
 8c5:	b8dc           	bnez.n	a8, 8e4 <intp_filter_16b+0x34>
 8c7:	11a4e0        	slli	a10, a4, 2
 8ca:	c0a3a0        	sub	a10, a3, a10
 8cd:	41ba24        	ae_lp16f.xu	aep3, a10, a2
 8d0:	0a0384        	ae_cvtq48p24s.l	aeq0, aep3
 8d3:	ee0d9a2a2f 	{ ae_lp16f.xu	aep2, a10, a2; ae_mulsfp24s.ll	aeq0, aep3, aep0 }
 8d8:	012034        	ae_mulafp24s.ll	aeq0, aep2, aep0
 8db:	1b8c14        	ae_roundsp24q48sym	aep1, aeq0
 8de:	421124        	ae_sp16f.l.i	aep1, a1, 4
 8e1:	005246        	j	a2e <intp_filter_16b+0x17e>

000008e4 <intp_filter_16b+0x34>:
 8e4:	fde561        	l32r	a6, 78 (a000 <_end+0x8f34>)
 8e7:	fdd671        	l32r	a7, 40 (40000000 <_end+0x3fffef34>)
 8ea:	581866        	bnei	a8, 1, 946 <intp_filter_16b+0x96>
 8ed:	fde3d1        	l32r	a13, 7c (2000 <_end+0xf34>)
 8f0:	304dd4        	ae_cvtp24a16x2.ll	aep4, a13, a13
 8f3:	01c874        	ae_mulfp24s.ll	aeq3, aep4, aep0
 8f6:	11c4d0        	slli	a12, a4, 3
 8f9:	1bbc34        	ae_roundsp24q48sym	aep3, aeq3
 8fc:	013874        	ae_mulfp24s.ll	aeq1, aep3, aep0
 8ff:	368734        	ae_cvtq48a32s	aeq2, a7
 902:	f83efaa09f 	{ ae_slliq56	aeq2, aeq2, 1; ae_subq56	aeq0, aeq3, aeq1 }
 907:	fa36d8a66f 	{ ae_cvtp24a16x2.ll	aep2, a6, a6; ae_subq56	aeq2, aeq2, aeq1 }
 90c:	01a034        	ae_mulafp24s.ll	aeq2, aep2, aep0
 90f:	01dfc4        	ae_subq56	aeq3, aeq1, aeq3
 912:	f8ffd5c3cf 	{ sub	a12, a3, a12; ae_roundsp24q48sym	aep1, aeq3 }
 917:	fbf79a7c2f 	{ ae_lp16f.xu	aep7, a12, a2; ae_roundsp24q48sym	aep6, aeq2 }
 91c:	01f974        	ae_mulfp24s.ll	aeq3, aep7, aep1
 91f:	0a4084        	ae_cvtq48p24s.l	aeq1, aep0
 922:	0158c4        	ae_addq56	aeq1, aeq1, aeq0
 925:	faef9a5c2f 	{ ae_lp16f.xu	aep5, a12, a2; ae_roundsp24q48sym	aep4, aeq1 }
 92a:	01de34        	ae_mulafp24s.ll	aeq3, aep5, aep6
 92d:	060824        	ae_negq56	aeq0, aeq0
 930:	f9e79a3c2f 	{ ae_lp16f.xu	aep3, a12, a2; ae_roundsp24q48sym	aep2, aeq0 }
 935:	e38fda1c2f 	{ ae_lp16f.xu	aep1, a12, a2; ae_mulafp24s.ll	aeq3, aep3, aep4 }
 93a:	019a34        	ae_mulafp24s.ll	aeq3, aep1, aep2
 93d:	1bbc74        	ae_roundsp24q48sym	aep7, aeq3
 940:	427124        	ae_sp16f.l.i	aep7, a1, 4
 943:	0039c6        	j	a2e <intp_filter_16b+0x17e>

00000946 <intp_filter_16b+0x96>:
 946:	60b020        	neg	a11, a2
 949:	fdcfc1        	l32r	a12, 88 (ccd <__udivdi3+0x1dd>)
 94c:	fdcdf1        	l32r	a15, 80 (2aaaaaaa <_end+0x2aaa99de>)
 94f:	fdcde1        	l32r	a14, 84 (4000 <_end+0x2f34>)
 952:	364734        	ae_cvtq48a32s	aeq1, a7
 955:	fdbdd1        	l32r	a13, 4c (55555555 <_end+0x55554489>)
 958:	368d34        	ae_cvtq48a32s	aeq2, a13
 95b:	351054        	ae_slliq56	aeq0, aeq1, 1
 95e:	ff0c58feef 	{ ae_cvtp24a16x2.ll	aep7, a14, a14; ae_movq56	aeq3, aeq1 }
 963:	efe1d421ff 	{ s32i.n	a15, a1, 8; ae_mulsfp24s.ll	aeq3, aep0, aep7 }
 968:	e2e3ee612f 	{ ae_lp24f.i	aep6, a1, 8; ae_mulafp24s.ll	aeq1, aep0, aep7 }
 96d:	018634        	ae_mulafp24s.ll	aeq2, aep0, aep6
 970:	1b9c44        	ae_roundsp24q48sym	aep4, aeq1
 973:	1bac24        	ae_roundsp24q48sym	aep2, aeq2
 976:	eec1bf017f 	{ ae_movq56	aeq1, aeq0; ae_mulsfp24s.ll	aeq0, aep0, aep6 }
 97b:	010fb4        	ae_mulsfp24s.ll	aeq1, aep0, aep7
 97e:	f8e7fab0df 	{ ae_slliq56	aeq3, aeq3, 1; ae_roundsp24q48sym	aep1, aeq0 }
 983:	1bbc74        	ae_roundsp24q48sym	aep7, aeq3
 986:	010274        	ae_mulfp24s.ll	aeq0, aep0, aep2
 989:	1b9c54        	ae_roundsp24q48sym	aep5, aeq1
 98c:	019574        	ae_mulfp24s.ll	aeq2, aep1, aep5
 98f:	01df74        	ae_mulfp24s.ll	aeq3, aep5, aep7
 992:	014a74        	ae_mulfp24s.ll	aeq1, aep4, aep2
 995:	1bbc34        	ae_roundsp24q48sym	aep3, aeq3
 998:	1b9c64        	ae_roundsp24q48sym	aep6, aeq1
 99b:	faf7b7412f 	{ ae_sp24x2s.i	aep4, a1, 16; ae_roundsp24q48sym	aep4, aeq2 }
 9a0:	01e874        	ae_mulfp24s.ll	aeq3, aep6, aep0
 9a3:	01c774        	ae_mulfp24s.ll	aeq2, aep4, aep7
 9a6:	1bbc24        	ae_roundsp24q48sym	aep2, aeq3
 9a9:	1bac54        	ae_roundsp24q48sym	aep5, aeq2
 9ac:	01d674        	ae_mulfp24s.ll	aeq2, aep5, aep6
 9af:	01ca74        	ae_mulfp24s.ll	aeq3, aep4, aep2
 9b2:	356054        	ae_slliq56	aeq1, aeq2, 1
 9b5:	fa763ab05f 	{ ae_slliq56	aeq1, aeq3, 1; ae_addq56	aeq2, aeq2, aeq1 }
 9ba:	01f9c4        	ae_addq56	aeq3, aeq3, aeq1
 9bd:	1bac44        	ae_roundsp24q48sym	aep4, aeq2
 9c0:	ebe5b0612f 	{ ae_lp24x2.i	aep6, a1, 16; ae_mulfp24s.ll	aeq2, aep1, aep7 }
 9c5:	010e74        	ae_mulfp24s.ll	aeq1, aep0, aep6
 9c8:	1b8c74        	ae_roundsp24q48sym	aep7, aeq0
 9cb:	013274        	ae_mulfp24s.ll	aeq0, aep3, aep2
 9ce:	1b9c64        	ae_roundsp24q48sym	aep6, aeq1
 9d1:	1bac14        	ae_roundsp24q48sym	aep1, aeq2
 9d4:	019274        	ae_mulfp24s.ll	aeq2, aep1, aep2
 9d7:	015f74        	ae_mulfp24s.ll	aeq1, aep5, aep7
 9da:	1b8c24        	ae_roundsp24q48sym	aep2, aeq0
 9dd:	1b9c74        	ae_roundsp24q48sym	aep7, aeq1
 9e0:	f9f7d8966f 	{ ae_cvtp24a16x2.ll	aep1, a6, a6; ae_roundsp24q48sym	aep3, aeq2 }
 9e5:	013174        	ae_mulfp24s.ll	aeq0, aep3, aep1
 9e8:	ead5d8bccf 	{ ae_cvtp24a16x2.ll	aep3, a12, a12; ae_mulfp24s.ll	aeq1, aep5, aep6 }
 9ed:	01a374        	ae_mulfp24s.ll	aeq2, aep2, aep3
 9f0:	1b8c64        	ae_roundsp24q48sym	aep6, aeq0
 9f3:	faf7d1a43f 	{ addx2	a10, a4, a3; ae_roundsp24q48sym	aep5, aeq2 }
 9f8:	eb3d9a2abf 	{ ae_lp16f.xu	aep2, a10, a11; ae_mulfp24s.ll	aeq2, aep7, aep1 }
 9fd:	eaa99a5abf 	{ ae_lp16f.xu	aep5, a10, a11; ae_mulfp24s.ll	aeq0, aep2, aep5 }
 a02:	015634        	ae_mulafp24s.ll	aeq0, aep5, aep6
 a05:	1b9c24        	ae_roundsp24q48sym	aep2, aeq1
 a08:	1bbc64        	ae_roundsp24q48sym	aep6, aeq3
 a0b:	012b74        	ae_mulfp24s.ll	aeq1, aep2, aep3
 a0e:	f9f79a5abf 	{ ae_lp16f.xu	aep5, a10, a11; ae_roundsp24q48sym	aep2, aeq2 }
 a13:	015634        	ae_mulafp24s.ll	aeq0, aep5, aep6
 a16:	fbefda3abf 	{ ae_lp16f.xu	aep3, a10, a11; ae_roundsp24q48sym	aep7, aeq1 }
 a1b:	e28f9a1abf 	{ ae_lp16f.xu	aep1, a10, a11; ae_mulafp24s.ll	aeq0, aep3, aep4 }
 a20:	e2479a6abf 	{ ae_lp16f.xu	aep6, a10, a11; ae_mulafp24s.ll	aeq0, aep1, aep2 }
 a25:	016734        	ae_mulafp24s.ll	aeq0, aep6, aep7
 a28:	1b8c54        	ae_roundsp24q48sym	aep5, aeq0
 a2b:	425124        	ae_sp16f.l.i	aep5, a1, 4
 a2e:	029122        	l16si	a2, a1, 4
 a31:	f01d           	retw.n

00000a33 <intp_filter_16b+0x183>:
	...

00000a34 <intp_filter_32b>:
 a34:	006136        	entry	a1, 48
 a37:	4258           	l32i.n	a5, a2, 16
 a39:	051222        	l16ui	a2, a2, 10
 a3c:	1155f0        	slli	a5, a5, 1
 a3f:	0159           	s32i.n	a5, a1, 0
 a41:	e2dc           	bnez.n	a2, a63 <intp_filter_32b+0x2f>
 a43:	500104        	ae_lp24f.i	aep0, a1, 0
 a46:	1194e0        	slli	a9, a4, 2
 a49:	1184d0        	slli	a8, a4, 3
 a4c:	c08380        	sub	a8, a3, a8
 a4f:	607894        	ae_lq32f.xu	aeq1, a8, a9
 a52:	f8c1fc489f 	{ ae_lq32f.xu	aeq2, a8, a9; ae_mulsfq32sp24s.l	aeq1, aeq1, aep0 }
 a57:	080ba4        	ae_mulafq32sp24s.l	aeq1, aeq2, aep0
 a5a:	061834        	ae_roundsq32sym	aeq0, aeq1
 a5d:	620114        	ae_sq32f.i	aeq0, a1, 4
 a60:	0018c6        	j	ac7 <intp_filter_32b+0x93>

00000a63 <intp_filter_32b+0x2f>:
 a63:	601266        	bnei	a2, 1, ac7 <intp_filter_32b+0x93>
 a66:	11a4c0        	slli	a10, a4, 4
 a69:	fd83b1        	l32r	a11, 78 (a000 <_end+0x8f34>)
 a6c:	501104        	ae_lp24f.i	aep1, a1, 0
 a6f:	fd83d1        	l32r	a13, 7c (2000 <_end+0xf34>)
 a72:	306dd4        	ae_cvtp24a16x2.ll	aep6, a13, a13
 a75:	01e174        	ae_mulfp24s.ll	aeq2, aep6, aep1
 a78:	fd72c1        	l32r	a12, 40 (40000000 <_end+0x3fffef34>)
 a7b:	faf7fecc3f 	{ ae_cvtq48a32s	aeq0, a12; ae_roundsp24q48sym	aep5, aeq2 }
 a80:	eb35d8cbbf 	{ ae_cvtp24a16x2.ll	aep4, a11, a11; ae_mulfp24s.ll	aeq3, aep5, aep1 }
 a85:	11b4e0        	slli	a11, a4, 2
 a88:	f9b6fa801f 	{ ae_slliq56	aeq0, aeq0, 1; ae_subq56	aeq1, aeq2, aeq3 }
 a8d:	010fc4        	ae_subq56	aeq0, aeq0, aeq3
 a90:	014134        	ae_mulafp24s.ll	aeq0, aep4, aep1
 a93:	fabad5a3af 	{ sub	a10, a3, a10; ae_subq56	aeq2, aeq3, aeq2 }
 a98:	f9f7fc6abf 	{ ae_lq32f.xu	aeq3, a10, a11; ae_roundsp24q48sym	aep3, aeq2 }
 a9d:	08bde4        	ae_mulfq32sp24s.l	aeq3, aeq3, aep3
 aa0:	1b8c24        	ae_roundsp24q48sym	aep2, aeq0
 aa3:	fc2c3c4abf 	{ ae_lq32f.xu	aeq2, a10, a11; ae_cvtq48p24s.l	aeq0, aep1 }
 aa8:	08aba4        	ae_mulafq32sp24s.l	aeq3, aeq2, aep2
 aab:	0109c4        	ae_addq56	aeq0, aeq0, aeq1
 aae:	1b8c04        	ae_roundsp24q48sym	aep0, aeq0
 ab1:	064924        	ae_negq56	aeq1, aeq1
 ab4:	fbeffc4abf 	{ ae_lq32f.xu	aeq2, a10, a11; ae_roundsp24q48sym	aep7, aeq1 }
 ab9:	f7c2fc0abf 	{ ae_lq32f.xu	aeq0, a10, a11; ae_mulafq32sp24s.l	aeq3, aeq2, aep0 }
 abe:	08fb24        	ae_mulafq32sp24s.l	aeq3, aeq0, aep7
 ac1:	06b834        	ae_roundsq32sym	aeq2, aeq3
 ac4:	622114        	ae_sq32f.i	aeq2, a1, 4
 ac7:	1128           	l32i.n	a2, a1, 4
 ac9:	f01d           	retw.n

00000acb <intp_filter_32b+0x97>:
 acb:	00000000 00413600                                .....

00000ad0 <__do_global_ctors_aux>:
 ad0:	004136        	entry	a1, 32
 ad3:	fd6e81        	l32r	a8, 8c (e7c <_DYNAMIC+0x154>)
 ad6:	7f2882        	l32i	a8, a8, 0x1fc
 ad9:	fd6d21        	l32r	a2, 90 (1078 <__CTOR_LIST___47>)
 adc:	0c0826        	beqi	a8, -1, aec <__do_global_ctors_aux+0x1c>
 adf:	02a8           	l32i.n	a10, a2, 0

00000ae1 <__do_global_ctors_aux+0x11>:
 ae1:	000ae0        	callx8	a10
 ae4:	fcc222        	addi	a2, a2, -4
 ae7:	02a8           	l32i.n	a10, a2, 0
 ae9:	f40a66        	bnei	a10, -1, ae1 <__do_global_ctors_aux+0x11>

00000aec <__do_global_ctors_aux+0x1c>:
 aec:	f01d           	retw.n

00000aee <__do_global_ctors_aux+0x1e>:
	...

00000af0 <__udivdi3>:
 af0:	008136        	entry	a1, 64
 af3:	049d           	mov.n	a9, a4
 af5:	02ad           	mov.n	a10, a2
 af7:	038d           	mov.n	a8, a3
 af9:	0b2c           	movi.n	a11, 32
 afb:	09b516        	beqz	a5, b9a <__udivdi3+0xaa>
 afe:	06b357        	bgeu	a3, a5, b08 <__udivdi3+0x18>
 b01:	020c           	movi.n	a2, 0
 b03:	030c           	movi.n	a3, 0
 b05:	f01d           	retw.n

00000b07 <__udivdi3+0x17>:
	...

00000b08 <__udivdi3+0x18>:
 b08:	40f540        	nsau	a4, a5
 b0b:	1d4416        	beqz	a4, ce3 <__udivdi3+0x1f3>
 b0e:	c02b40        	sub	a2, a11, a4
 b11:	400200        	ssr	a2
 b14:	91f030        	srl	a15, a3
 b17:	401400        	ssl	a4
 b1a:	81c3a0        	src	a12, a3, a10
 b1d:	81b590        	src	a11, a5, a9
 b20:	9b8d           	ae_zext16	a8, a11
 b22:	f5d0b0        	extui	a13, a11, 16, 16
 b25:	404010        	ssai	16
 b28:	c2efd0        	quou	a14, a15, a13
 b2b:	0e5d           	mov.n	a5, a14
 b2d:	8238e0        	mull	a3, a8, a14
 b30:	e2ffd0        	remu	a15, a15, a13
 b33:	81ffc0        	src	a15, a15, a12
 b36:	0f2d           	mov.n	a2, a15
 b38:	0ebf37        	bgeu	a15, a3, b4a <__udivdi3+0x5a>
 b3b:	5e0b           	addi.n	a5, a14, -1
 b3d:	2bfa           	add.n	a2, a11, a15
 b3f:	0732b7        	bltu	a2, a11, b4a <__udivdi3+0x5a>
 b42:	04b237        	bgeu	a2, a3, b4a <__udivdi3+0x5a>
 b45:	2b2a           	add.n	a2, a11, a2
 b47:	fece52        	addi	a5, a14, -2
 b4a:	116500        	slli	a6, a5, 16
 b4d:	401400        	ssl	a4
 b50:	c03230        	sub	a3, a2, a3
 b53:	c2e3d0        	quou	a14, a3, a13
 b56:	0e2d           	mov.n	a2, a14
 b58:	a15900        	sll	a5, a9
 b5b:	8288e0        	mull	a8, a8, a14
 b5e:	e233d0        	remu	a3, a3, a13
 b61:	0bf3c0        	depbits	a12, a3, 16, 16
 b64:	0c3d           	mov.n	a3, a12
 b66:	0ebc87        	bgeu	a12, a8, b78 <__udivdi3+0x88>
 b69:	2e0b           	addi.n	a2, a14, -1
 b6b:	3bca           	add.n	a3, a11, a12
 b6d:	0733b7        	bltu	a3, a11, b78 <__udivdi3+0x88>
 b70:	04b387        	bgeu	a3, a8, b78 <__udivdi3+0x88>
 b73:	3b3a           	add.n	a3, a11, a3
 b75:	fece22        	addi	a2, a14, -2
 b78:	c03380        	sub	a3, a3, a8
 b7b:	202260        	or	a2, a2, a6
 b7e:	828250        	mull	a8, a2, a5
 b81:	a25250        	muluh	a5, a2, a5
 b84:	0b3357        	bltu	a3, a5, b93 <__udivdi3+0xa3>
 b87:	c09350        	sub	a9, a3, a5
 b8a:	07f956        	bnez	a9, c0d <__udivdi3+0x11d>
 b8d:	a1ba00        	sll	a11, a10
 b90:	79bb87        	bgeu	a11, a8, c0d <__udivdi3+0x11d>

00000b93 <__udivdi3+0xa3>:
 b93:	220b           	addi.n	a2, a2, -1
 b95:	030c           	movi.n	a3, 0
 b97:	f01d           	retw.n

00000b99 <__udivdi3+0xa9>:
	...

00000b9a <__udivdi3+0xaa>:
 b9a:	75b347        	bgeu	a3, a4, c13 <__udivdi3+0x123>
 b9d:	40f450        	nsau	a5, a4
 ba0:	a58c           	beqz.n	a5, bae <__udivdi3+0xbe>
 ba2:	401500        	ssl	a5
 ba5:	a14400        	sll	a4, a4
 ba8:	813320        	src	a3, a3, a2
 bab:	a12200        	sll	a2, a2

00000bae <__udivdi3+0xbe>:
 bae:	945d           	ae_zext16	a5, a4
 bb0:	404010        	ssai	16
 bb3:	f58040        	extui	a8, a4, 16, 16
 bb6:	e2c380        	remu	a12, a3, a8
 bb9:	c2b380        	quou	a11, a3, a8
 bbc:	0bad           	mov.n	a10, a11
 bbe:	81cc20        	src	a12, a12, a2
 bc1:	8295b0        	mull	a9, a5, a11
 bc4:	0c3d           	mov.n	a3, a12
 bc6:	0ebc97        	bgeu	a12, a9, bd8 <__udivdi3+0xe8>
 bc9:	ab0b           	addi.n	a10, a11, -1
 bcb:	3c4a           	add.n	a3, a12, a4
 bcd:	073347        	bltu	a3, a4, bd8 <__udivdi3+0xe8>
 bd0:	04b397        	bgeu	a3, a9, bd8 <__udivdi3+0xe8>
 bd3:	343a           	add.n	a3, a4, a3
 bd5:	fecba2        	addi	a10, a11, -2
 bd8:	c0d390        	sub	a13, a3, a9
 bdb:	c29d80        	quou	a9, a13, a8
 bde:	093d           	mov.n	a3, a9
 be0:	825590        	mull	a5, a5, a9
 be3:	e2dd80        	remu	a13, a13, a8
 be6:	0bfd20        	depbits	a2, a13, 16, 16
 be9:	16b257        	bgeu	a2, a5, c03 <__udivdi3+0x113>
 bec:	390b           	addi.n	a3, a9, -1
 bee:	224a           	add.n	a2, a2, a4
 bf0:	0f3247        	bltu	a2, a4, c03 <__udivdi3+0x113>
 bf3:	0cb257        	bgeu	a2, a5, c03 <__udivdi3+0x113>
 bf6:	112a00        	slli	a2, a10, 16
 bf9:	fec932        	addi	a3, a9, -2
 bfc:	202320        	or	a2, a3, a2
 bff:	030c           	movi.n	a3, 0
 c01:	f01d           	retw.n

00000c03 <__udivdi3+0x113>:
 c03:	112a00        	slli	a2, a10, 16
 c06:	202320        	or	a2, a3, a2
 c09:	030c           	movi.n	a3, 0
 c0b:	f01d           	retw.n

00000c0d <__udivdi3+0x11d>:
 c0d:	030c           	movi.n	a3, 0
 c0f:	f01d           	retw.n

00000c11 <__udivdi3+0x121>:
	...

00000c13 <__udivdi3+0x123>:
 c13:	130c           	movi.n	a3, 1
 c15:	c24340        	quou	a4, a3, a4
 c18:	839490        	moveqz	a9, a4, a9
 c1b:	094d           	mov.n	a4, a9
 c1d:	40f950        	nsau	a5, a9
 c20:	0dc516        	beqz	a5, d00 <__udivdi3+0x210>
 c23:	401500        	ssl	a5
 c26:	c0eb50        	sub	a14, a11, a5
 c29:	a14900        	sll	a4, a9
 c2c:	400e00        	ssr	a14
 c2f:	91d080        	srl	a13, a8
 c32:	401500        	ssl	a5
 c35:	813820        	src	a3, a8, a2
 c38:	945d           	ae_zext16	a5, a4
 c3a:	a12200        	sll	a2, a2
 c3d:	f58040        	extui	a8, a4, 16, 16
 c40:	404010        	ssai	16
 c43:	c2cd80        	quou	a12, a13, a8
 c46:	0cbd           	mov.n	a11, a12
 c48:	82a5c0        	mull	a10, a5, a12
 c4b:	e2dd80        	remu	a13, a13, a8
 c4e:	81dd30        	src	a13, a13, a3
 c51:	0d9d           	mov.n	a9, a13
 c53:	0ebda7        	bgeu	a13, a10, c65 <__udivdi3+0x175>
 c56:	bc0b           	addi.n	a11, a12, -1
 c58:	9d4a           	add.n	a9, a13, a4
 c5a:	073947        	bltu	a9, a4, c65 <__udivdi3+0x175>
 c5d:	04b9a7        	bgeu	a9, a10, c65 <__udivdi3+0x175>
 c60:	949a           	add.n	a9, a4, a9
 c62:	feccb2        	addi	a11, a12, -2
 c65:	c0d9a0        	sub	a13, a9, a10
 c68:	e2ed80        	remu	a14, a13, a8
 c6b:	0bfe30        	depbits	a3, a14, 16, 16
 c6e:	c2dd80        	quou	a13, a13, a8
 c71:	0dad           	mov.n	a10, a13
 c73:	82c5d0        	mull	a12, a5, a13
 c76:	039d           	mov.n	a9, a3
 c78:	0eb3c7        	bgeu	a3, a12, c8a <__udivdi3+0x19a>
 c7b:	ad0b           	addi.n	a10, a13, -1
 c7d:	934a           	add.n	a9, a3, a4
 c7f:	073947        	bltu	a9, a4, c8a <__udivdi3+0x19a>
 c82:	04b9c7        	bgeu	a9, a12, c8a <__udivdi3+0x19a>
 c85:	949a           	add.n	a9, a4, a9
 c87:	fecda2        	addi	a10, a13, -2
 c8a:	c039c0        	sub	a3, a9, a12
 c8d:	11fb00        	slli	a15, a11, 16
 c90:	20aaf0        	or	a10, a10, a15

00000c93 <__udivdi3+0x1a3>:
 c93:	404010        	ssai	16
 c96:	e2d380        	remu	a13, a3, a8
 c99:	c2c380        	quou	a12, a3, a8
 c9c:	0cbd           	mov.n	a11, a12
 c9e:	8295c0        	mull	a9, a5, a12
 ca1:	81dd20        	src	a13, a13, a2
 ca4:	0d3d           	mov.n	a3, a13
 ca6:	0ebd97        	bgeu	a13, a9, cb8 <__udivdi3+0x1c8>
 ca9:	bc0b           	addi.n	a11, a12, -1
 cab:	3d4a           	add.n	a3, a13, a4
 cad:	073347        	bltu	a3, a4, cb8 <__udivdi3+0x1c8>
 cb0:	04b397        	bgeu	a3, a9, cb8 <__udivdi3+0x1c8>
 cb3:	343a           	add.n	a3, a4, a3
 cb5:	feccb2        	addi	a11, a12, -2
 cb8:	c0e390        	sub	a14, a3, a9
 cbb:	c29e80        	quou	a9, a14, a8
 cbe:	093d           	mov.n	a3, a9
 cc0:	825590        	mull	a5, a5, a9
 cc3:	e2ee80        	remu	a14, a14, a8
 cc6:	0bfe20        	depbits	a2, a14, 16, 16
 cc9:	23b257        	bgeu	a2, a5, cf0 <__udivdi3+0x200>
 ccc:	390b           	addi.n	a3, a9, -1
 cce:	224a           	add.n	a2, a2, a4
 cd0:	1c3247        	bltu	a2, a4, cf0 <__udivdi3+0x200>
 cd3:	19b257        	bgeu	a2, a5, cf0 <__udivdi3+0x200>
 cd6:	113b00        	slli	a3, a11, 16
 cd9:	fec922        	addi	a2, a9, -2
 cdc:	202230        	or	a2, a2, a3
 cdf:	0a3d           	mov.n	a3, a10
 ce1:	f01d           	retw.n

00000ce3 <__udivdi3+0x1f3>:
 ce3:	023537        	bltu	a5, a3, ce9 <__udivdi3+0x1f9>
 ce6:	103297        	bltu	a2, a9, cfa <__udivdi3+0x20a>

00000ce9 <__udivdi3+0x1f9>:
 ce9:	120c           	movi.n	a2, 1
 ceb:	030c           	movi.n	a3, 0
 ced:	f01d           	retw.n

00000cef <__udivdi3+0x1ff>:
	...

00000cf0 <__udivdi3+0x200>:
 cf0:	112b00        	slli	a2, a11, 16
 cf3:	202320        	or	a2, a3, a2
 cf6:	0a3d           	mov.n	a3, a10
 cf8:	f01d           	retw.n

00000cfa <__udivdi3+0x20a>:
 cfa:	020c           	movi.n	a2, 0
 cfc:	030c           	movi.n	a3, 0
 cfe:	f01d           	retw.n

00000d00 <__udivdi3+0x210>:
 d00:	03ad           	mov.n	a10, a3
 d02:	995d           	ae_zext16	a5, a9
 d04:	c03890        	sub	a3, a8, a9
 d07:	f58090        	extui	a8, a9, 16, 16
 d0a:	ffe146        	j	c93 <__udivdi3+0x1a3>

00000d0d <__udivdi3+0x21d>:
 d0d:	000000                                        ...

00000d10 <_fini>:
 d10:	008136        	entry	a1, 64
 d13:	fce081        	l32r	a8, 94 (b0 <__do_global_dtors_aux>)
 d16:	f03d           	nop.n
 d18:	0008e0        	callx8	a8

00000d1b <_fini+0xb>:
 d1b:	f01d           	retw.n
