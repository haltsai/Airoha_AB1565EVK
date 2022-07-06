
Build/lib/pisplit_agc_position_independent.splitload.lib:     file format elf32-xtensa-le


Disassembly of section .text:

00000000 <_init-0x68>:
   0:	000000bc 00000670 00000a8c 00000a74     ....p.......t...
  10:	00000000 00000a58 00000000 00000a90     ....X...........
  20:	00000aa8 00000a10 00000108 00000a28     ............(...
  30:	0000015c 00000a3c 00000640 00000a80     \...<...@.......
  40:	00007fff 000006a8 00000690 00000889     ................
  50:	08000000 00000110 0000036c 00000860     ........l...`...
  60:	00000a5c 00000080                       \.......

00000068 <_init>:
  68:	008136        	entry	a1, 64
  6b:	ffe581        	l32r	a8, 0 (bc <frame_dummy>)
  6e:	f03d           	nop.n
  70:	0008e0        	callx8	a8
  73:	ffe481        	l32r	a8, 4 (670 <__do_global_ctors_aux>)
  76:	f03d           	nop.n
  78:	0008e0        	callx8	a8
  7b:	f01d           	retw.n

0000007d <_init+0x15>:
  7d:	000000                                        ...

00000080 <__do_global_dtors_aux>:
  80:	004136        	entry	a1, 32
  83:	ffe131        	l32r	a3, 8 (a8c <__bss_start>)
  86:	000382        	l8ui	a8, a3, 0
  89:	d8ec           	bnez.n	a8, ba <__do_global_dtors_aux+0x3a>
  8b:	ffe021        	l32r	a2, c (a74 <p$3519_6_1>)
  8e:	02b8           	l32i.n	a11, a2, 0
  90:	0ba8           	l32i.n	a10, a11, 0
  92:	ca8c           	beqz.n	a10, a2 <__do_global_dtors_aux+0x22>

00000094 <__do_global_dtors_aux+0x14>:
  94:	9b4b           	addi.n	a9, a11, 4
  96:	0299           	s32i.n	a9, a2, 0
  98:	000ae0        	callx8	a10
  9b:	02b8           	l32i.n	a11, a2, 0
  9d:	0ba8           	l32i.n	a10, a11, 0
  9f:	ff1a56        	bnez	a10, 94 <__do_global_dtors_aux+0x14>

000000a2 <__do_global_dtors_aux+0x22>:
  a2:	ffdba1        	l32r	a10, 10 (0 <_text_start>)
  a5:	0b0c           	movi.n	a11, 0
  a7:	081ab7        	beq	a10, a11, b3 <__do_global_dtors_aux+0x33>
  aa:	ffdaa1        	l32r	a10, 14 (a58 <__EH_FRAME_BEGIN___44>)
  ad:	ffd881        	l32r	a8, 10 (0 <_text_start>)
  b0:	0008e0        	callx8	a8

000000b3 <__do_global_dtors_aux+0x33>:
  b3:	1c0c           	movi.n	a12, 1
  b5:	0043c2        	s8i	a12, a3, 0
  b8:	f01d           	retw.n

000000ba <__do_global_dtors_aux+0x3a>:
  ba:	f01d           	retw.n

000000bc <frame_dummy>:
  bc:	004136        	entry	a1, 32
  bf:	ffd681        	l32r	a8, 18 (0 <_text_start>)
  c2:	090c           	movi.n	a9, 0
  c4:	0b1897        	beq	a8, a9, d3 <frame_dummy+0x17>
  c7:	ffd3a1        	l32r	a10, 14 (a58 <__EH_FRAME_BEGIN___44>)
  ca:	ffd4b1        	l32r	a11, 1c (a90 <object$3538_8_1>)
  cd:	ffd281        	l32r	a8, 18 (0 <_text_start>)
  d0:	0008e0        	callx8	a8

000000d3 <frame_dummy+0x17>:
  d3:	f01d           	retw.n

000000d5 <frame_dummy+0x19>:
  d5:	000000                                        ...

000000d8 <_start>:
  d8:	004136        	entry	a1, 32
  db:	ffd2a1        	l32r	a10, 24 (a10 <_DYNAMIC+0x2c0>)
  de:	ffd031        	l32r	a3, 20 (aa8 <printf_ptr>)
  e1:	0288           	l32i.n	a8, a2, 0
  e3:	ffd1b1        	l32r	a11, 28 (108 <get_agc_memsize>)
  e6:	0389           	s32i.n	a8, a3, 0
  e8:	0008e0        	callx8	a8
  eb:	0388           	l32i.n	a8, a3, 0
  ed:	ffcfa1        	l32r	a10, 2c (a28 <_DYNAMIC+0x2d8>)
  f0:	ffd0b1        	l32r	a11, 30 (15c <AGC_VO_Init>)
  f3:	0008e0        	callx8	a8
  f6:	0388           	l32i.n	a8, a3, 0
  f8:	ffcfa1        	l32r	a10, 34 (a3c <_DYNAMIC+0x2ec>)
  fb:	ffcfb1        	l32r	a11, 38 (640 <AGC_Proc>)
  fe:	0008e0        	callx8	a8
 101:	ffce21        	l32r	a2, 3c (a80 <export_parameter_array>)
 104:	f01d           	retw.n

00000106 <_start+0x2e>:
	...

00000108 <get_agc_memsize>:
 108:	004136        	entry	a1, 32
 10b:	28a122        	movi	a2, 0x128
 10e:	f01d           	retw.n

00000110 <AGC_Xdiv>:
 110:	004136        	entry	a1, 32
 113:	0248           	l32i.n	a4, a2, 0
 115:	0358           	l32i.n	a5, a3, 0
 117:	04bc           	beqz.n	a4, 14b <AGC_Xdiv+0x3b>
 119:	85bc           	beqz.n	a5, 155 <AGC_Xdiv+0x45>
 11b:	60c204        	ae_lq32f.i	aeq3, a2, 0
 11e:	608304        	ae_lq32f.i	aeq2, a3, 0
 121:	366904        	ae_nsaq56s	a9, aeq2
 124:	367804        	ae_nsaq56s	a8, aeq3
 127:	438890        	min	a8, a8, a9
 12a:	f8c892        	addi	a9, a8, -8
 12d:	fc24feb90f 	{ ae_slaasq56s	aeq0, aeq3, a9; nop }
 132:	e9c882        	addi	a8, a8, -23
 135:	fc24fea84f 	{ ae_slaasq56s	aeq1, aeq2, a8; nop }
 13a:	369204        	ae_trunca32q48	a2, aeq1
 13d:	368404        	ae_trunca32q48	a4, aeq0
 140:	528c           	beqz.n	a2, 149 <AGC_Xdiv+0x39>
 142:	d22420        	quos	a2, a4, a2
 145:	a22d           	ae_clamps16	a2, a2
 147:	f01d           	retw.n

00000149 <AGC_Xdiv+0x39>:
 149:	f01d           	retw.n

0000014b <AGC_Xdiv+0x3b>:
 14b:	ffbd81        	l32r	a8, 40 (7fff <_end+0x7553>)
 14e:	020c           	movi.n	a2, 0
 150:	832850        	moveqz	a2, a8, a5
 153:	f01d           	retw.n

00000155 <AGC_Xdiv+0x45>:
 155:	ffba21        	l32r	a2, 40 (7fff <_end+0x7553>)
 158:	f01d           	retw.n

0000015a <AGC_Xdiv+0x4a>:
	...

0000015c <AGC_VO_Init>:
 15c:	004136        	entry	a1, 32
 15f:	03bd           	mov.n	a11, a3
 161:	ffb881        	l32r	a8, 44 (6a8 <xt_memcpy>)
 164:	02ad           	mov.n	a10, a2
 166:	6c2c           	movi.n	a12, 38
 168:	0008e0        	callx8	a8
 16b:	50c2a2        	addi	a10, a2, 80
 16e:	ffb681        	l32r	a8, 48 (690 <xt_memset>)
 171:	0b0c           	movi.n	a11, 0
 173:	6ca0c2        	movi	a12, 108
 176:	0008e0        	callx8	a8
 179:	0002c2        	l8ui	a12, a2, 0
 17c:	1502b2        	l8ui	a11, a2, 21
 17f:	2852c2        	s16i	a12, a2, 80
 182:	1402c2        	l8ui	a12, a2, 20
 185:	1702a2        	l8ui	a10, a2, 23
 188:	11bb80        	slli	a11, a11, 8
 18b:	20bbc0        	or	a11, a11, a12
 18e:	8bbd           	ae_sext16	a11, a11
 190:	3052b2        	s16i	a11, a2, 96
 193:	1602b2        	l8ui	a11, a2, 22
 196:	190292        	l8ui	a9, a2, 25
 199:	11aa80        	slli	a10, a10, 8
 19c:	20aab0        	or	a10, a10, a11
 19f:	8aad           	ae_sext16	a10, a10
 1a1:	3152a2        	s16i	a10, a2, 98
 1a4:	1802a2        	l8ui	a10, a2, 24
 1a7:	1b0282        	l8ui	a8, a2, 27
 1aa:	119980        	slli	a9, a9, 8
 1ad:	2099a0        	or	a9, a9, a10
 1b0:	899d           	ae_sext16	a9, a9
 1b2:	325292        	s16i	a9, a2, 100
 1b5:	1a0292        	l8ui	a9, a2, 26
 1b8:	0b0272        	l8ui	a7, a2, 11
 1bb:	118880        	slli	a8, a8, 8
 1be:	208890        	or	a8, a8, a9
 1c1:	888d           	ae_sext16	a8, a8
 1c3:	335282        	s16i	a8, a2, 102
 1c6:	0a0282        	l8ui	a8, a2, 10
 1c9:	117780        	slli	a7, a7, 8
 1cc:	207780        	or	a7, a7, a8
 1cf:	090282        	l8ui	a8, a2, 9
 1d2:	117780        	slli	a7, a7, 8
 1d5:	207780        	or	a7, a7, a8
 1d8:	080282        	l8ui	a8, a2, 8
 1db:	0f0262        	l8ui	a6, a2, 15
 1de:	117780        	slli	a7, a7, 8
 1e1:	207780        	or	a7, a7, a8
 1e4:	156272        	s32i	a7, a2, 84
 1e7:	0e0272        	l8ui	a7, a2, 14
 1ea:	0202e2        	l8ui	a14, a2, 2
 1ed:	116680        	slli	a6, a6, 8
 1f0:	206670        	or	a6, a6, a7
 1f3:	0d0272        	l8ui	a7, a2, 13
 1f6:	0302d2        	l8ui	a13, a2, 3
 1f9:	116680        	slli	a6, a6, 8
 1fc:	206670        	or	a6, a6, a7
 1ff:	0c0272        	l8ui	a7, a2, 12
 202:	130252        	l8ui	a5, a2, 19
 205:	116680        	slli	a6, a6, 8
 208:	206670        	or	a6, a6, a7
 20b:	166262        	s32i	a6, a2, 88
 20e:	120262        	l8ui	a6, a2, 18
 211:	1f0232        	l8ui	a3, a2, 31
 214:	115580        	slli	a5, a5, 8
 217:	205560        	or	a5, a5, a6
 21a:	110262        	l8ui	a6, a2, 17
 21d:	2302f2        	l8ui	a15, a2, 35
 220:	115580        	slli	a5, a5, 8
 223:	205560        	or	a5, a5, a6
 226:	100262        	l8ui	a6, a2, 16
 229:	11ff80        	slli	a15, a15, 8
 22c:	115580        	slli	a5, a5, 8
 22f:	205560        	or	a5, a5, a6
 232:	176252        	s32i	a5, a2, 92
 235:	1e0252        	l8ui	a5, a2, 30
 238:	113380        	slli	a3, a3, 8
 23b:	11dd80        	slli	a13, a13, 8
 23e:	203350        	or	a3, a3, a5
 241:	1d0252        	l8ui	a5, a2, 29
 244:	20dde0        	or	a13, a13, a14
 247:	113380        	slli	a3, a3, 8
 24a:	203350        	or	a3, a3, a5
 24d:	1c0252        	l8ui	a5, a2, 28
 250:	04e1d0        	extui	a14, a13, 1, 1
 253:	113380        	slli	a3, a3, 8
 256:	203350        	or	a3, a3, a5
 259:	1a6232        	s32i	a3, a2, 104
 25c:	220232        	l8ui	a3, a2, 34
 25f:	04d0d0        	extui	a13, a13, 0, 1
 262:	3c52e2        	s16i	a14, a2, 120
 265:	20ff30        	or	a15, a15, a3
 268:	210232        	l8ui	a3, a2, 33
 26b:	2702e2        	l8ui	a14, a2, 39
 26e:	11ff80        	slli	a15, a15, 8
 271:	20ff30        	or	a15, a15, a3
 274:	200232        	l8ui	a3, a2, 32
 277:	3d52d2        	s16i	a13, a2, 122
 27a:	11ff80        	slli	a15, a15, 8
 27d:	20ff30        	or	a15, a15, a3
 280:	1b62f2        	s32i	a15, a2, 108
 283:	2602f2        	l8ui	a15, a2, 38
 286:	2b02d2        	l8ui	a13, a2, 43
 289:	11ee80        	slli	a14, a14, 8
 28c:	20eef0        	or	a14, a14, a15
 28f:	2502f2        	l8ui	a15, a2, 37
 292:	11dd80        	slli	a13, a13, 8
 295:	11ee80        	slli	a14, a14, 8
 298:	20eef0        	or	a14, a14, a15
 29b:	2402f2        	l8ui	a15, a2, 36
 29e:	0502b2        	l8ui	a11, a2, 5
 2a1:	11ee80        	slli	a14, a14, 8
 2a4:	20eef0        	or	a14, a14, a15
 2a7:	1c62e2        	s32i	a14, a2, 112
 2aa:	2a02e2        	l8ui	a14, a2, 42
 2ad:	11bb80        	slli	a11, a11, 8
 2b0:	1a0c           	movi.n	a10, 1
 2b2:	20dde0        	or	a13, a13, a14
 2b5:	2902e2        	l8ui	a14, a2, 41
 2b8:	3c9282        	l16si	a8, a2, 120
 2bb:	11dd80        	slli	a13, a13, 8
 2be:	20dde0        	or	a13, a13, a14
 2c1:	2802e2        	l8ui	a14, a2, 40
 2c4:	ff0c           	movi.n	a15, 15
 2c6:	11dd80        	slli	a13, a13, 8
 2c9:	20dde0        	or	a13, a13, a14
 2cc:	1d62d2        	s32i	a13, a2, 116
 2cf:	751426        	beqi	a4, 1, 348 <AGC_VO_Init+0x1ec>
 2d2:	9252f2        	s16i	a15, a2, 0x124
 2d5:	ff5de1        	l32r	a14, 4c (889 <_DYNAMIC+0x139>)
 2d8:	9352e2        	s16i	a14, a2, 0x126

000002db <AGC_VO_Init+0x17f>:
 2db:	771826        	beqi	a8, 1, 356 <AGC_VO_Init+0x1fa>
 2de:	0402c2        	l8ui	a12, a2, 4
 2e1:	20bbc0        	or	a11, a11, a12
 2e4:	8bbd           	ae_sext16	a11, a11
 2e6:	2952b2        	s16i	a11, a2, 82

000002e9 <AGC_VO_Init+0x18d>:
 2e9:	8c0c           	movi.n	a12, 8
 2eb:	3e52c2        	s16i	a12, a2, 124
 2ee:	0c1c           	movi.n	a12, 16
 2f0:	02acb7        	bge	a12, a11, 2f6 <AGC_VO_Init+0x19a>
 2f3:	3e52c2        	s16i	a12, a2, 124

000002f6 <AGC_VO_Init+0x19a>:
 2f6:	0e3c           	movi.n	a14, 48
 2f8:	0c2c           	movi.n	a12, 32
 2fa:	04acb7        	bge	a12, a11, 302 <AGC_VO_Init+0x1a6>
 2fd:	8d1c           	movi.n	a13, 24
 2ff:	3e52d2        	s16i	a13, a2, 124

00000302 <AGC_VO_Init+0x1a6>:
 302:	1e2eb7        	blt	a14, a11, 324 <AGC_VO_Init+0x1c8>
 305:	9152a2        	s16i	a10, a2, 0x122
 308:	0f0c           	movi.n	a15, 0
 30a:	ff5181        	l32r	a8, 50 (8000000 <_end+0x7fff554>)
 30d:	446282        	s32i	a8, a2, 0x110
 310:	456282        	s32i	a8, a2, 0x114
 313:	436282        	s32i	a8, a2, 0x10c
 316:	466282        	s32i	a8, a2, 0x118
 319:	476282        	s32i	a8, a2, 0x11c
 31c:	216282        	s32i	a8, a2, 132
 31f:	9052f2        	s16i	a15, a2, 0x120
 322:	f01d           	retw.n

00000324 <AGC_VO_Init+0x1c8>:
 324:	3e52c2        	s16i	a12, a2, 124
 327:	9152a2        	s16i	a10, a2, 0x122
 32a:	ff49b1        	l32r	a11, 50 (8000000 <_end+0x7fff554>)
 32d:	090c           	movi.n	a9, 0
 32f:	905292        	s16i	a9, a2, 0x120
 332:	4462b2        	s32i	a11, a2, 0x110
 335:	4562b2        	s32i	a11, a2, 0x114
 338:	4362b2        	s32i	a11, a2, 0x10c
 33b:	4662b2        	s32i	a11, a2, 0x118
 33e:	4762b2        	s32i	a11, a2, 0x11c
 341:	2162b2        	s32i	a11, a2, 132
 344:	f01d           	retw.n

00000346 <AGC_VO_Init+0x1ea>:
	...

00000348 <AGC_VO_Init+0x1ec>:
 348:	44a4c2        	movi	a12, 0x444
 34b:	ed1c           	movi.n	a13, 30
 34d:	9252d2        	s16i	a13, a2, 0x124
 350:	9352c2        	s16i	a12, a2, 0x126
 353:	ffe106        	j	2db <AGC_VO_Init+0x17f>

00000356 <AGC_VO_Init+0x1fa>:
 356:	0b4c           	movi.n	a11, 64
 358:	0e4c           	movi.n	a14, 64
 35a:	3092f2        	l16si	a15, a2, 96
 35d:	3152f2        	s16i	a15, a2, 98
 360:	3252f2        	s16i	a15, a2, 100
 363:	2952e2        	s16i	a14, a2, 82
 366:	ffdfc6        	j	2e9 <AGC_VO_Init+0x18d>

00000369 <AGC_VO_Init+0x20d>:
 369:	000000                                        ...

0000036c <agc_block_proc>:
 36c:	006136        	entry	a1, 48
 36f:	189282        	l16si	a8, a2, 48
 372:	1c0c           	movi.n	a12, 1
 374:	236816        	beqz	a8, 5ae <agc_block_proc+0x242>
 377:	23e416        	beqz	a4, 5b9 <agc_block_proc+0x24d>
 37a:	080c           	movi.n	a8, 0

0000037c <agc_block_proc+0x10>:
 37c:	0b9292        	l16si	a9, a2, 22
 37f:	195282        	s16i	a8, a2, 50
 382:	0aa987        	bge	a9, a8, 390 <agc_block_proc+0x24>
 385:	0a0c           	movi.n	a10, 0
 387:	1952a2        	s16i	a10, a2, 50
 38a:	1752a2        	s16i	a10, a2, 46
 38d:	1852a2        	s16i	a10, a2, 48

00000390 <agc_block_proc+0x24>:
 390:	1592b2        	l16si	a11, a2, 42
 393:	d6a082        	movi	a8, 214
 396:	4b8c           	beqz.n	a11, 39e <agc_block_proc+0x32>
 398:	1792d2        	l16si	a13, a2, 46
 39b:	222d16        	beqz	a13, 5c1 <agc_block_proc+0x255>

0000039e <agc_block_proc+0x32>:
 39e:	428a           	add.n	a4, a2, a8
 3a0:	412284        	ae_lp16f.x	aep2, a2, a8
 3a3:	6a92a2        	l16si	a10, a2, 212
 3a6:	fc248fe33e 	{ addi	a3, a3, -2; ae_zeroq56	aeq0 }
 3ab:	fc20d3833f 	{ or	a8, a3, a3; ae_zerop48	aep1 }
 3b0:	1b1aa6        	blti	a10, 1, 3cf <agc_block_proc+0x63>
 3b3:	408814        	ae_lp16f.iu	aep0, a8, 2
 3b6:	079804        	ae_abssp24s	aep0, aep0
 3b9:	e20a0ffa9e 	{ addi	a9, a10, -1; ae_mulafp24s.hh	aeq0, aep2, aep0 }
 3be:	0aa976        	loopgtz	a9, 3cc <agc_block_proc+0x60>
 3c1:	d8066c881f 	{ ae_lp16f.iu	aep0, a8, 2; ae_maxp24s	aep1, aep1, aep0 }
 3c6:	079804        	ae_abssp24s	aep0, aep0
 3c9:	012004        	ae_mulafp24s.hh	aeq0, aep2, aep0

000003cc <agc_block_proc+0x60>:
 3cc:	071814        	ae_maxp24s	aep1, aep1, aep0

000003cf <agc_block_proc+0x63>:
 3cf:	38c282        	addi	a8, a2, 56
 3d2:	fe247d280f 	{ ae_lq32f.i	aeq1, a8, 0; ae_roundsq32asym	aeq2, aeq0 }
 3d7:	06e914        	ae_subsq56s	aeq3, aeq2, aeq1
 3da:	35f0f4        	ae_sraiq56	aeq3, aeq3, 3
 3dd:	06b934        	ae_roundsq32asym	aeq2, aeq3
 3e0:	06da04        	ae_addsq56s	aeq3, aeq1, aeq2
 3e3:	623804        	ae_sq32f.i	aeq3, a8, 0
 3e6:	82f8           	l32i.n	a15, a2, 32
 3e8:	e288           	l32i.n	a8, a2, 56
 3ea:	0a8184        	ae_cvtq48p24s.l	aeq2, aep1
 3ed:	622104        	ae_sq32f.i	aeq2, a1, 0
 3f0:	02a8f7        	bge	a8, a15, 3f6 <agc_block_proc+0x8a>
 3f3:	002346        	j	484 <agc_block_proc+0x118>
 3f6:	699282        	l16si	a8, a2, 210
 3f9:	329d           	ae_l16si.n	a9, a2, 2
 3fb:	169252        	l16si	a5, a2, 44
 3fe:	0168           	l32i.n	a6, a1, 0
 400:	c09980        	sub	a9, a9, a8
 403:	0ec956        	bnez	a9, 4f3 <agc_block_proc+0x187>
 406:	6952c2        	s16i	a12, a2, 210
 409:	f2f8           	l32i.n	a15, a2, 60
 40b:	a24b           	addi.n	a10, a2, 4
 40d:	0225e6        	bgei	a5, 2, 413 <agc_block_proc+0xa7>
 410:	008886        	j	636 <agc_block_proc+0x2ca>
 413:	40c282        	addi	a8, a2, 64
 416:	f77c           	movi.n	a7, -1
 418:	fe7c           	movi.n	a14, -1
 41a:	fd7c           	movi.n	a13, -1
 41c:	b50b           	addi.n	a11, a5, -1
 41e:	10ab76        	loopgtz	a11, 432 <agc_block_proc+0xc6>
 421:	08c8           	l32i.n	a12, a8, 0
 423:	884b           	addi.n	a8, a8, 4
 425:	23afc7        	bge	a15, a12, 44c <agc_block_proc+0xe0>
 428:	0e7d           	mov.n	a7, a14
 42a:	0ded           	mov.n	a14, a13
 42c:	0fdd           	mov.n	a13, a15
 42e:	0cfd           	mov.n	a15, a12

00000430 <agc_block_proc+0xc4>:
 430:	f03d           	nop.n

00000432 <agc_block_proc+0xc6>:
 432:	2192e0        	srai	a9, a14, 2
 435:	218270        	srai	a8, a7, 2
 438:	21c2f0        	srai	a12, a15, 2
 43b:	889a           	add.n	a8, a8, a9
 43d:	2192d0        	srai	a9, a13, 2
 440:	cc9a           	add.n	a12, a12, a9
 442:	cc8a           	add.n	a12, a12, a8
 444:	d2c9           	s32i.n	a12, a2, 52
 446:	1e25e6        	bgei	a5, 2, 468 <agc_block_proc+0xfc>
 449:	002786        	j	4eb <agc_block_proc+0x17f>

0000044c <agc_block_proc+0xe0>:
 44c:	08adc7        	bge	a13, a12, 458 <agc_block_proc+0xec>
 44f:	0e7d           	mov.n	a7, a14
 451:	0ded           	mov.n	a14, a13
 453:	0cdd           	mov.n	a13, a12
 455:	fff5c6        	j	430 <agc_block_proc+0xc4>

00000458 <agc_block_proc+0xec>:
 458:	06aec7        	bge	a14, a12, 462 <agc_block_proc+0xf6>
 45b:	0e7d           	mov.n	a7, a14
 45d:	0ced           	mov.n	a14, a12
 45f:	fff346        	j	430 <agc_block_proc+0xc4>

00000462 <agc_block_proc+0xf6>:
 462:	537c70        	max	a7, a12, a7
 465:	fff1c6        	j	430 <agc_block_proc+0xc4>

00000468 <agc_block_proc+0xfc>:
 468:	3cc282        	addi	a8, a2, 60
 46b:	0c0c           	movi.n	a12, 0
 46d:	0e8076        	loop	a0, 47f <agc_block_proc+0x113>
 470:	18d8           	l32i.n	a13, a8, 4
 472:	09e8d4        	ae_s32ip	a13, a8, 4
 475:	169252        	l16si	a5, a2, 44
 478:	cc1b           	addi.n	a12, a12, 1
 47a:	b50b           	addi.n	a11, a5, -1
 47c:	6bacb7        	bge	a12, a11, 4eb <agc_block_proc+0x17f>

0000047f <agc_block_proc+0x113>:
 47f:	fffa86        	j	46d <agc_block_proc+0x101>

00000482 <agc_block_proc+0x116>:
	...

00000484 <agc_block_proc+0x118>:
 484:	1492e2        	l16si	a14, a2, 40
 487:	16ae16        	beqz	a14, 5f5 <agc_block_proc+0x289>

0000048a <agc_block_proc+0x11e>:
 48a:	2f2282        	l32i	a8, a2, 188

0000048d <agc_block_proc+0x121>:
 48d:	360834        	ae_cvtq48a32s	aeq0, a8
 490:	10c2a2        	addi	a10, a2, 16
 493:	01d2b2        	addmi	a11, a2, 0x100
 496:	c0cbb2        	addi	a11, a11, -64
 499:	608b04        	ae_lq32f.i	aeq2, a11, 0
 49c:	fbc2ac2a0f 	{ ae_lp16f.i	aep2, a10, 0; ae_subsq56s	aeq3, aeq0, aeq2 }
 4a1:	03a0d4        	ae_mulafq32sp16s.l	aeq2, aeq3, aep2
 4a4:	312292        	l32i	a9, a2, 196
 4a7:	fd347ec93f 	{ ae_cvtq48a32s	aeq0, a9; ae_roundsq32asym	aeq1, aeq2 }
 4ac:	369804        	ae_trunca32q48	a8, aeq1
 4af:	306282        	s32i	a8, a2, 192
 4b2:	fb4aac140f 	{ ae_lp16f.i	aep1, a4, 0; ae_subsq56s	aeq3, aeq1, aeq0 }
 4b7:	d7e766a2fe 	{ l16si	a15, a2, 212; ae_mulfq32sp16s.l	aeq3, aeq3, aep1 }
 4bc:	038d           	mov.n	a8, a3
 4be:	fe3c6c881f 	{ ae_lp16f.iu	aep0, a8, 2; ae_roundsq32asym	aeq2, aeq3 }
 4c3:	221fa6        	blti	a15, 1, 4e9 <agc_block_proc+0x17d>
 4c6:	00a0c2        	movi	a12, 0
 4c9:	198076        	loop	a0, 4e6 <agc_block_proc+0x17a>
 4cc:	010ac4        	ae_addq56	aeq0, aeq0, aeq2
 4cf:	030a14        	ae_mulfq32sp16s.l	aeq1, aeq0, aep0
 4d2:	355114        	ae_slliq56	aeq1, aeq1, 4
 4d5:	1b9f34        	ae_roundsp16q48asym	aep3, aeq1
 4d8:	423804        	ae_sp16f.l.i	aep3, a8, 0
 4db:	6a92d2        	l16si	a13, a2, 212
 4de:	cc1b           	addi.n	a12, a12, 1
 4e0:	408814        	ae_lp16f.iu	aep0, a8, 2
 4e3:	02acd7        	bge	a12, a13, 4e9 <agc_block_proc+0x17d>

000004e6 <agc_block_proc+0x17a>:
 4e6:	fff7c6        	j	4c9 <agc_block_proc+0x15d>

000004e9 <agc_block_proc+0x17d>:
 4e9:	f01d           	retw.n

000004eb <agc_block_proc+0x17f>:
 4eb:	a0e520        	addx4	a14, a5, a2
 4ee:	ee69           	s32i.n	a6, a14, 56
 4f0:	000406        	j	504 <agc_block_proc+0x198>

000004f3 <agc_block_proc+0x187>:
 4f3:	881b           	addi.n	a8, a8, 1
 4f5:	695282        	s16i	a8, a2, 210
 4f8:	a08520        	addx4	a8, a5, a2
 4fb:	e8f8           	l32i.n	a15, a8, 56
 4fd:	01af67        	bge	a15, a6, 502 <agc_block_proc+0x196>
 500:	e869           	s32i.n	a6, a8, 56

00000502 <agc_block_proc+0x196>:
 502:	a24b           	addi.n	a10, a2, 4

00000504 <agc_block_proc+0x198>:
 504:	34c2b2        	addi	a11, a2, 52
 507:	fed381        	l32r	a8, 54 (110 <AGC_Xdiv>)
 50a:	302292        	l32i	a9, a2, 192
 50d:	316292        	s32i	a9, a2, 196
 510:	0008e0        	callx8	a8
 513:	208aa0        	or	a8, a10, a10
 516:	3322a2        	l32i	a10, a2, 204
 519:	118800        	slli	a8, a8, 16
 51c:	360a34        	ae_cvtq48a32s	aeq0, a10
 51f:	c0a8a0        	sub	a10, a8, a10
 522:	021ae6        	bgei	a10, 1, 528 <agc_block_proc+0x1bc>
 525:	0029c6        	j	5d0 <agc_block_proc+0x264>
 528:	36ca34        	ae_cvtq48a32s	aeq3, a10
 52b:	14c292        	addi	a9, a2, 20
 52e:	404904        	ae_lp16f.i	aep4, a9, 0
 531:	0340d4        	ae_mulafq32sp16s.l	aeq0, aeq3, aep4
 534:	fe247022de 	{ l32i	a13, a2, 8; ae_roundsq32asym	aeq2, aeq0 }
 539:	36ac04        	ae_trunca32q48	a12, aeq2
 53c:	3362c2        	s32i	a12, a2, 204
 53f:	052cd7        	blt	a12, a13, 548 <agc_block_proc+0x1dc>
 542:	20cdd0        	or	a12, a13, a13
 545:	3362d2        	s32i	a13, a2, 204

00000548 <agc_block_proc+0x1dc>:
 548:	1492b2        	l16si	a11, a2, 40
 54b:	bb0b           	addi.n	a11, a11, -1
 54d:	0ddb16        	beqz	a11, 62e <agc_block_proc+0x2c2>
 550:	3222d2        	l32i	a13, a2, 200
 553:	360d34        	ae_cvtq48a32s	aeq0, a13
 556:	c0d8d0        	sub	a13, a8, a13
 559:	021de6        	bgei	a13, 1, 55f <agc_block_proc+0x1f3>
 55c:	002b06        	j	60c <agc_block_proc+0x2a0>
 55f:	505264        	ae_lp24f.i	aep5, a2, 24
 562:	08d524        	ae_mulfq32sp24s.l	aeq2, aeq0, aep5
 565:	35a054        	ae_slliq56	aeq2, aeq2, 1
 568:	fd347022ee 	{ l32i	a14, a2, 8; ae_roundsq32asym	aeq1, aeq2 }
 56d:	369804        	ae_trunca32q48	a8, aeq1
 570:	326282        	s32i	a8, a2, 200
 573:	0428e7        	blt	a8, a14, 57b <agc_block_proc+0x20f>
 576:	0e8d           	mov.n	a8, a14
 578:	3262e2        	s32i	a14, a2, 200

0000057b <agc_block_proc+0x20f>:
 57b:	60f1d0        	abs	a15, a13
 57e:	60e1a0        	abs	a14, a10
 581:	6892a2        	l16si	a10, a2, 208
 584:	0caef7        	bge	a14, a15, 594 <agc_block_proc+0x228>
 587:	15bae6        	bgei	a10, 16, 5a0 <agc_block_proc+0x234>
 58a:	aa1b           	addi.n	a10, a10, 1
 58c:	8aad           	ae_sext16	a10, a10
 58e:	6852a2        	s16i	a10, a2, 208
 591:	0002c6        	j	5a0 <agc_block_proc+0x234>

00000594 <agc_block_proc+0x228>:
 594:	0b7c           	movi.n	a11, -16
 596:	06aba7        	bge	a11, a10, 5a0 <agc_block_proc+0x234>
 599:	aa0b           	addi.n	a10, a10, -1
 59b:	8aad           	ae_sext16	a10, a10
 59d:	6852a2        	s16i	a10, a2, 208
 5a0:	0b0c           	movi.n	a11, 0
 5a2:	72bba0        	salt	a11, a11, a10
 5a5:	938cb0        	movnez	a8, a12, a11
 5a8:	2f6282        	s32i	a8, a2, 188
 5ab:	ffb786        	j	48d <agc_block_proc+0x121>

000005ae <agc_block_proc+0x242>:
 5ae:	748c           	beqz.n	a4, 5b9 <agc_block_proc+0x24d>
 5b0:	1852c2        	s16i	a12, a2, 48
 5b3:	1752c2        	s16i	a12, a2, 46
 5b6:	ff7586        	j	390 <agc_block_proc+0x24>

000005b9 <agc_block_proc+0x24d>:
 5b9:	199282        	l16si	a8, a2, 50
 5bc:	881b           	addi.n	a8, a8, 1
 5be:	ff6e86        	j	37c <agc_block_proc+0x10>

000005c1 <agc_block_proc+0x255>:
 5c1:	fec332        	addi	a3, a3, -2
 5c4:	2f2282        	l32i	a8, a2, 188
 5c7:	01d242        	addmi	a4, a2, 0x100
 5ca:	d6c442        	addi	a4, a4, -42
 5cd:	ffaf06        	j	48d <agc_block_proc+0x121>

000005d0 <agc_block_proc+0x264>:
 5d0:	364a34        	ae_cvtq48a32s	aeq1, a10
 5d3:	12c292        	addi	a9, a2, 18
 5d6:	406904        	ae_lp16f.i	aep6, a9, 0
 5d9:	036054        	ae_mulafq32sp16s.l	aeq0, aeq1, aep6
 5dc:	ff247032de 	{ l32i	a13, a2, 12; ae_roundsq32asym	aeq3, aeq0 }
 5e1:	36bc04        	ae_trunca32q48	a12, aeq3
 5e4:	3362c2        	s32i	a12, a2, 204
 5e7:	02adc7        	bge	a13, a12, 5ed <agc_block_proc+0x281>
 5ea:	ffd686        	j	548 <agc_block_proc+0x1dc>
 5ed:	0dcd           	mov.n	a12, a13
 5ef:	3362d2        	s32i	a13, a2, 204
 5f2:	ffd486        	j	548 <agc_block_proc+0x1dc>

000005f5 <agc_block_proc+0x289>:
 5f5:	92a8           	l32i.n	a10, a2, 36
 5f7:	0228a7        	blt	a8, a10, 5fd <agc_block_proc+0x291>
 5fa:	ffa306        	j	48a <agc_block_proc+0x11e>
 5fd:	fe9481        	l32r	a8, 50 (8000000 <_end+0x7fff554>)
 600:	2f6282        	s32i	a8, a2, 188
 603:	336282        	s32i	a8, a2, 204
 606:	326282        	s32i	a8, a2, 200
 609:	ffa006        	j	48d <agc_block_proc+0x121>

0000060c <agc_block_proc+0x2a0>:
 60c:	507274        	ae_lp24f.i	aep7, a2, 28
 60f:	08fd24        	ae_mulfq32sp24s.l	aeq3, aeq0, aep7
 612:	35f054        	ae_slliq56	aeq3, aeq3, 1
 615:	fe3c7032ee 	{ l32i	a14, a2, 12; ae_roundsq32asym	aeq2, aeq3 }
 61a:	36a804        	ae_trunca32q48	a8, aeq2
 61d:	326282        	s32i	a8, a2, 200
 620:	02ae87        	bge	a14, a8, 626 <agc_block_proc+0x2ba>
 623:	ffd506        	j	57b <agc_block_proc+0x20f>
 626:	0e8d           	mov.n	a8, a14
 628:	3262e2        	s32i	a14, a2, 200
 62b:	ffd306        	j	57b <agc_block_proc+0x20f>

0000062e <agc_block_proc+0x2c2>:
 62e:	0c8d           	mov.n	a8, a12
 630:	2f62c2        	s32i	a12, a2, 188
 633:	ff9586        	j	48d <agc_block_proc+0x121>

00000636 <agc_block_proc+0x2ca>:
 636:	f77c           	movi.n	a7, -1
 638:	fe7c           	movi.n	a14, -1
 63a:	fd7c           	movi.n	a13, -1
 63c:	ff7c86        	j	432 <agc_block_proc+0xc6>

0000063f <agc_block_proc+0x2d3>:
	...

00000640 <AGC_Proc>:
 640:	004136        	entry	a1, 32
 643:	289282        	l16si	a8, a2, 80
 646:	28ac           	beqz.n	a8, 66c <AGC_Proc+0x2c>
 648:	929292        	l16si	a9, a2, 0x124
 64b:	50c272        	addi	a7, a2, 80
 64e:	020c           	movi.n	a2, 0
 650:	d25590        	quos	a5, a5, a9
 653:	1515a6        	blti	a5, 1, 66c <AGC_Proc+0x2c>
 656:	1169f0        	slli	a6, a9, 1

00000659 <AGC_Proc+0x19>:
 659:	04cd           	mov.n	a12, a4
 65b:	fe7f81        	l32r	a8, 58 (36c <agc_block_proc>)
 65e:	07ad           	mov.n	a10, a7
 660:	03bd           	mov.n	a11, a3
 662:	0008e0        	callx8	a8
 665:	336a           	add.n	a3, a3, a6
 667:	221b           	addi.n	a2, a2, 1
 669:	ec9527        	bne	a5, a2, 659 <AGC_Proc+0x19>

0000066c <AGC_Proc+0x2c>:
 66c:	f01d           	retw.n

0000066e <AGC_Proc+0x2e>:
	...

00000670 <__do_global_ctors_aux>:
 670:	004136        	entry	a1, 32
 673:	fe7a81        	l32r	a8, 5c (860 <_DYNAMIC+0x110>)
 676:	7f2882        	l32i	a8, a8, 0x1fc
 679:	fe7921        	l32r	a2, 60 (a5c <__CTOR_LIST___47>)
 67c:	0c0826        	beqi	a8, -1, 68c <__do_global_ctors_aux+0x1c>
 67f:	02a8           	l32i.n	a10, a2, 0

00000681 <__do_global_ctors_aux+0x11>:
 681:	000ae0        	callx8	a10
 684:	fcc222        	addi	a2, a2, -4
 687:	02a8           	l32i.n	a10, a2, 0
 689:	f40a66        	bnei	a10, -1, 681 <__do_global_ctors_aux+0x11>

0000068c <__do_global_ctors_aux+0x1c>:
 68c:	f01d           	retw.n

0000068e <__do_global_ctors_aux+0x1e>:
	...

00000690 <xt_memset>:
 690:	004136        	entry	a1, 32
 693:	026d           	mov.n	a6, a2
 695:	828c           	beqz.n	a2, 6a1 <xt_memset+0x11>
 697:	029476        	loopnez	a4, 69d <xt_memset+0xd>
 69a:	09c234        	ae_s16ip	a3, a2, 2

0000069d <xt_memset+0xd>:
 69d:	062d           	mov.n	a2, a6
 69f:	f01d           	retw.n

000006a1 <xt_memset+0x11>:
 6a1:	020c           	movi.n	a2, 0
 6a3:	f01d           	retw.n

000006a5 <xt_memset+0x15>:
 6a5:	000000                                        ...

000006a8 <xt_memcpy>:
 6a8:	004136        	entry	a1, 32
 6ab:	027d           	mov.n	a7, a2
 6ad:	032d           	mov.n	a2, a3
 6af:	043d           	mov.n	a3, a4
 6b1:	07c716        	beqz	a7, 731 <xt_memcpy+0x89>
 6b4:	079216        	beqz	a2, 731 <xt_memcpy+0x89>
 6b7:	25b277        	bgeu	a2, a7, 6e0 <xt_memcpy+0x38>
 6ba:	904420        	addx2	a4, a4, a2
 6bd:	1fb747        	bgeu	a7, a4, 6e0 <xt_memcpy+0x38>
 6c0:	fec422        	addi	a2, a4, -2
 6c3:	530b           	addi.n	a5, a3, -1
 6c5:	904370        	addx2	a4, a3, a7
 6c8:	fec442        	addi	a4, a4, -2
 6cb:	5e0526        	beqi	a5, -1, 72d <xt_memcpy+0x85>
 6ce:	099376        	loopnez	a3, 6db <xt_memcpy+0x33>
 6d1:	223d           	ae_l16si.n	a3, a2, 0
 6d3:	fec222        	addi	a2, a2, -2
 6d6:	fec442        	addi	a4, a4, -2
 6d9:	743d           	ae_s16i.n	a3, a4, 2

000006db <xt_memcpy+0x33>:
 6db:	072d           	mov.n	a2, a7
 6dd:	f01d           	retw.n

000006df <xt_memcpy+0x37>:
	...

000006e0 <xt_memcpy+0x38>:
 6e0:	074d           	mov.n	a4, a7
 6e2:	530b           	addi.n	a5, a3, -1
 6e4:	450526        	beqi	a5, -1, 72d <xt_memcpy+0x85>
 6e7:	248030        	extui	a8, a3, 0, 3
 6ea:	036d           	mov.n	a6, a3
 6ec:	413360        	srli	a3, a6, 3
 6ef:	069876        	loopnez	a8, 6f9 <xt_memcpy+0x51>
 6f2:	229d           	ae_l16si.n	a9, a2, 0
 6f4:	09c494        	ae_s16ip	a9, a4, 2
 6f7:	222b           	addi.n	a2, a2, 2
 6f9:	309376        	loopnez	a3, 72d <xt_memcpy+0x85>
 6fc:	223d           	ae_l16si.n	a3, a2, 0
 6fe:	09c434        	ae_s16ip	a3, a4, 2
 701:	323d           	ae_l16si.n	a3, a2, 2
 703:	09c434        	ae_s16ip	a3, a4, 2
 706:	029232        	l16si	a3, a2, 4
 709:	09c434        	ae_s16ip	a3, a4, 2
 70c:	039232        	l16si	a3, a2, 6
 70f:	09c434        	ae_s16ip	a3, a4, 2
 712:	049232        	l16si	a3, a2, 8
 715:	09c434        	ae_s16ip	a3, a4, 2
 718:	059232        	l16si	a3, a2, 10
 71b:	09c434        	ae_s16ip	a3, a4, 2
 71e:	069232        	l16si	a3, a2, 12
 721:	09c434        	ae_s16ip	a3, a4, 2
 724:	079232        	l16si	a3, a2, 14
 727:	09c434        	ae_s16ip	a3, a4, 2
 72a:	10c222        	addi	a2, a2, 16
 72d:	072d           	mov.n	a2, a7
 72f:	f01d           	retw.n

00000731 <xt_memcpy+0x89>:
 731:	020c           	movi.n	a2, 0
 733:	f01d           	retw.n

00000735 <xt_memcpy+0x8d>:
 735:	000000                                        ...

00000738 <_fini>:
 738:	008136        	entry	a1, 64
 73b:	fe4a81        	l32r	a8, 64 (80 <__do_global_dtors_aux>)
 73e:	f03d           	nop.n
 740:	0008e0        	callx8	a8

00000743 <_fini+0xb>:
 743:	f01d           	retw.n
