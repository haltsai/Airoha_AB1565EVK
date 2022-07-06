
Build/lib/pisplit_msbc_enc_position_independent.splitload.lib:     file format elf32-xtensa-le


Disassembly of section .text:

00000000 <_init-0x90>:
   0:	000000e4 000007e0 00001000 00000c04     ................
  10:	00000000 00000be8 00000000 00001004     ................
  20:	00000480 000003d8 0000101c 00000b7c     ............|...
  30:	00000480 00000b94 00000368 00000bac     ........h.......
  40:	000003d8 00000bc8 00000330 00000c10     ........0.......
  50:	00000c1c 000002e8 00000dd8 00000fe0     ................
  60:	00000188 00008000 00800000 000006c0     ................
  70:	000004b0 000002c8 00000374 0000ffff     ........t.......
  80:	00000d60 000009f0 00000bec 000000a8     `...............

00000090 <_init>:
  90:	008136        	entry	a1, 64
  93:	ffdb81        	l32r	a8, 0 (e4 <frame_dummy>)
  96:	f03d           	nop.n
  98:	0008e0        	callx8	a8
  9b:	ffda81        	l32r	a8, 4 (7e0 <__do_global_ctors_aux>)
  9e:	f03d           	nop.n
  a0:	0008e0        	callx8	a8
  a3:	f01d           	retw.n

000000a5 <_init+0x15>:
  a5:	000000                                        ...

000000a8 <__do_global_dtors_aux>:
  a8:	004136        	entry	a1, 32
  ab:	ffd731        	l32r	a3, 8 (1000 <__bss_start>)
  ae:	000382        	l8ui	a8, a3, 0
  b1:	d8ec           	bnez.n	a8, e2 <__do_global_dtors_aux+0x3a>
  b3:	ffd621        	l32r	a2, c (c04 <p$3519_6_1>)
  b6:	02b8           	l32i.n	a11, a2, 0
  b8:	0ba8           	l32i.n	a10, a11, 0
  ba:	ca8c           	beqz.n	a10, ca <__do_global_dtors_aux+0x22>

000000bc <__do_global_dtors_aux+0x14>:
  bc:	9b4b           	addi.n	a9, a11, 4
  be:	0299           	s32i.n	a9, a2, 0
  c0:	000ae0        	callx8	a10
  c3:	02b8           	l32i.n	a11, a2, 0
  c5:	0ba8           	l32i.n	a10, a11, 0
  c7:	ff1a56        	bnez	a10, bc <__do_global_dtors_aux+0x14>

000000ca <__do_global_dtors_aux+0x22>:
  ca:	ffd1a1        	l32r	a10, 10 (0 <_text_start>)
  cd:	0b0c           	movi.n	a11, 0
  cf:	081ab7        	beq	a10, a11, db <__do_global_dtors_aux+0x33>
  d2:	ffd0a1        	l32r	a10, 14 (be8 <__EH_FRAME_BEGIN___44>)
  d5:	ffce81        	l32r	a8, 10 (0 <_text_start>)
  d8:	0008e0        	callx8	a8

000000db <__do_global_dtors_aux+0x33>:
  db:	1c0c           	movi.n	a12, 1
  dd:	0043c2        	s8i	a12, a3, 0
  e0:	f01d           	retw.n

000000e2 <__do_global_dtors_aux+0x3a>:
  e2:	f01d           	retw.n

000000e4 <frame_dummy>:
  e4:	004136        	entry	a1, 32
  e7:	ffcc81        	l32r	a8, 18 (0 <_text_start>)
  ea:	090c           	movi.n	a9, 0
  ec:	0b1897        	beq	a8, a9, fb <frame_dummy+0x17>
  ef:	ffc9a1        	l32r	a10, 14 (be8 <__EH_FRAME_BEGIN___44>)
  f2:	ffcab1        	l32r	a11, 1c (1004 <object$3538_8_1>)
  f5:	ffc881        	l32r	a8, 18 (0 <_text_start>)
  f8:	0008e0        	callx8	a8

000000fb <frame_dummy+0x17>:
  fb:	f01d           	retw.n

000000fd <frame_dummy+0x19>:
  fd:	000000                                        ...

00000100 <SBC_GSENC_INIT>:
 100:	004136        	entry	a1, 32
 103:	ffc781        	l32r	a8, 20 (480 <SBC_Encoder_Init>)
 106:	04ad           	mov.n	a10, a4
 108:	0008e0        	callx8	a8
 10b:	193c           	movi.n	a9, 49
 10d:	0b1c           	movi.n	a11, 16
 10f:	1c0c           	movi.n	a12, 1
 111:	03d4a2        	addmi	a10, a4, 0x300
 114:	656a22        	s32i	a2, a10, 0x194
 117:	90c3c0        	addx2	a12, a3, a12
 11a:	716ac2        	s32i	a12, a10, 0x1c4
 11d:	636ab2        	s32i	a11, a10, 0x18c
 120:	6a6a92        	s32i	a9, a10, 0x1a8
 123:	f01d           	retw.n

00000125 <SBC_GSENC_INIT+0x25>:
 125:	000000                                        ...

00000128 <mSBC_GSENC_Process>:
 128:	004136        	entry	a1, 32
 12b:	03bd           	mov.n	a11, a3
 12d:	02ad           	mov.n	a10, a2
 12f:	04cd           	mov.n	a12, a4
 131:	03d452        	addmi	a5, a4, 0x300
 134:	ffbc81        	l32r	a8, 24 (3d8 <mSBC_Encoder_Process>)
 137:	bcc562        	addi	a6, a5, -68
 13a:	712552        	l32i	a5, a5, 0x1c4
 13d:	0008e0        	callx8	a8
 140:	826652        	s32i	a5, a6, 0x208
 143:	7c2622        	l32i	a2, a6, 0x1f0
 146:	f01d           	retw.n

00000148 <_start>:
 148:	004136        	entry	a1, 32
 14b:	ffb8a1        	l32r	a10, 2c (b7c <_DYNAMIC+0x364>)
 14e:	ffb631        	l32r	a3, 28 (101c <printf_ptr>)
 151:	0288           	l32i.n	a8, a2, 0
 153:	ffb7b1        	l32r	a11, 30 (480 <SBC_Encoder_Init>)
 156:	0389           	s32i.n	a8, a3, 0
 158:	0008e0        	callx8	a8
 15b:	0388           	l32i.n	a8, a3, 0
 15d:	ffb5a1        	l32r	a10, 34 (b94 <_DYNAMIC+0x37c>)
 160:	ffb6b1        	l32r	a11, 38 (368 <mSBC_SW_SN_init>)
 163:	0008e0        	callx8	a8
 166:	0388           	l32i.n	a8, a3, 0
 168:	ffb5a1        	l32r	a10, 3c (bac <_DYNAMIC+0x394>)
 16b:	ffb5b1        	l32r	a11, 40 (3d8 <mSBC_Encoder_Process>)
 16e:	0008e0        	callx8	a8
 171:	0388           	l32i.n	a8, a3, 0
 173:	ffb4a1        	l32r	a10, 44 (bc8 <_DYNAMIC+0x3b0>)
 176:	ffb4b1        	l32r	a11, 48 (330 <mSBC_SW_SN>)
 179:	0008e0        	callx8	a8
 17c:	ffb421        	l32r	a2, 4c (c10 <export_parameter_array>)
 17f:	f01d           	retw.n

00000181 <_start+0x39>:
 181:	00000000 36000000                                .......

00000188 <SbcAnalysisFilter8>:
 188:	008136        	entry	a1, 64
 18b:	03d272        	addmi	a7, a2, 0x300
 18e:	d89762        	l16si	a6, a7, 0x1b0
 191:	632782        	l32i	a8, a7, 0x18c
 194:	6827a2        	l32i	a10, a7, 0x1a0
 197:	a0c7b2        	addi	a11, a7, -96
 19a:	51b9           	s32i.n	a11, a1, 20
 19c:	41a9           	s32i.n	a10, a1, 16
 19e:	888d           	ae_sext16	a8, a8
 1a0:	80c772        	addi	a7, a7, -128
 1a3:	2189           	s32i.n	a8, a1, 8
 1a5:	0218e6        	bgei	a8, 1, 1ab <SbcAnalysisFilter8+0x23>
 1a8:	0044c6        	j	2bf <SbcAnalysisFilter8+0x137>
 1ab:	80a052        	movi	a5, 128
 1ae:	a0a042        	movi	a4, 160
 1b1:	c0a032        	movi	a3, 192
 1b4:	02d2c2        	addmi	a12, a2, 0x200
 1b7:	0e0c           	movi.n	a14, 0
 1b9:	ffa5f1        	l32r	a15, 50 (c1c <export_parameter_array+0xc>)
 1bc:	11f9           	s32i.n	a15, a1, 4
 1be:	31e9           	s32i.n	a14, a1, 12
 1c0:	fcccc2        	addi	a12, a12, -4
 1c3:	01c9           	s32i.n	a12, a1, 0
 1c5:	90a0e2        	movi	a14, 144
 1c8:	c0be60        	sub	a11, a14, a6

000001cb <SbcAnalysisFilter8+0x43>:
 1cb:	4198           	l32i.n	a9, a1, 16
 1cd:	8bbd           	ae_sext16	a11, a11
 1cf:	90cb20        	addx2	a12, a11, a2
 1d2:	29dd           	ae_l16si.n	a13, a9, 0
 1d4:	275cd2        	s16i	a13, a12, 78
 1d7:	39ad           	ae_l16si.n	a10, a9, 2
 1d9:	265ca2        	s16i	a10, a12, 76
 1dc:	029982        	l16si	a8, a9, 4
 1df:	255c82        	s16i	a8, a12, 74
 1e2:	0399d2        	l16si	a13, a9, 6
 1e5:	245cd2        	s16i	a13, a12, 72
 1e8:	0499a2        	l16si	a10, a9, 8
 1eb:	235ca2        	s16i	a10, a12, 70
 1ee:	059982        	l16si	a8, a9, 10
 1f1:	225c82        	s16i	a8, a12, 68
 1f4:	081c           	movi.n	a8, 16
 1f6:	0699d2        	l16si	a13, a9, 12
 1f9:	215cd2        	s16i	a13, a12, 66
 1fc:	0799a2        	l16si	a10, a9, 14
 1ff:	205ca2        	s16i	a10, a12, 64
 202:	10c992        	addi	a9, a9, 16
 205:	4199           	s32i.n	a9, a1, 16
 207:	0121a2        	l32i	a10, a1, 4
 20a:	00a092        	movi	a9, 0
 20d:	44a876        	loopgtz	a8, 255 <SbcAnalysisFilter8+0xcd>
 210:	a0d920        	addx4	a13, a9, a2
 213:	908b20        	addx2	a8, a11, a2
 216:	605a14        	ae_lq32f.iu	aeq1, a10, 4
 219:	609a14        	ae_lq32f.iu	aeq2, a10, 4
 21c:	60c8c2        	addi	a12, a8, 96
 21f:	415834        	ae_lp16f.x	aep5, a8, a3
 222:	416844        	ae_lp16f.x	aep6, a8, a4
 225:	417854        	ae_lp16f.x	aep7, a8, a5
 228:	bb1b           	addi.n	a11, a11, 1
 22a:	8bbd           	ae_sext16	a11, a11
 22c:	40c882        	addi	a8, a8, 64
 22f:	401804        	ae_lp16f.i	aep1, a8, 0
 232:	f8e56c0c0f 	{ ae_lp16f.i	aep0, a12, 0; ae_mulrfq32sp24s.l	aeq1, aeq1, aep1 }
 237:	d6e0fd1a1f 	{ ae_lq32f.iu	aeq0, a10, 4; ae_mularfq32sp24s.l	aeq1, aeq2, aep0 }
 23c:	d4fcfd7a1f 	{ ae_lq32f.iu	aeq3, a10, 4; ae_mularfq32sp24s.l	aeq1, aeq0, aep7 }
 241:	d6f9fd5a1f 	{ ae_lq32f.iu	aeq2, a10, 4; ae_mularfq32sp24s.l	aeq1, aeq3, aep6 }
 246:	d6f4c0199e 	{ addi	a9, a9, 1; ae_mularfq32sp24s.l	aeq1, aeq2, aep5 }
 24b:	899d           	ae_sext16	a9, a9
 24d:	355074        	ae_sraiq56	aeq1, aeq1, 1
 250:	369c04        	ae_trunca32q48	a12, aeq1
 253:	0dc9           	s32i.n	a12, a13, 0

00000255 <SbcAnalysisFilter8+0xcd>:
 255:	ff7f81        	l32r	a8, 54 (2e8 <SBC_FastIDCT8>)
 258:	02ad           	mov.n	a10, a2
 25a:	07bd           	mov.n	a11, a7
 25c:	0008e0        	callx8	a8
 25f:	20c772        	addi	a7, a7, 32
 262:	8fa0f2        	movi	a15, 143
 265:	3caf67        	bge	a15, a6, 2a5 <SbcAnalysisFilter8+0x11d>
 268:	9b0c           	movi.n	a11, 9
 26a:	219160        	srai	a9, a6, 1
 26d:	01a8           	l32i.n	a10, a1, 0
 26f:	00a062        	movi	a6, 0
 272:	04c992        	addi	a9, a9, 4
 275:	899d           	ae_sext16	a9, a9
 277:	1199e0        	slli	a9, a9, 2
 27a:	c09a90        	sub	a9, a10, a9
 27d:	21ab76        	loopgtz	a11, 2a2 <SbcAnalysisFilter8+0x11a>
 280:	ffd9b2        	addmi	a11, a9, 0xffffff00
 283:	ffdac2        	addmi	a12, a10, 0xffffff00
 286:	0988           	l32i.n	a8, a9, 0
 288:	0a89           	s32i.n	a8, a10, 0
 28a:	f0caa2        	addi	a10, a10, -16
 28d:	f0c992        	addi	a9, a9, -16
 290:	3f2be2        	l32i	a14, a11, 252
 293:	3f6ce2        	s32i	a14, a12, 252
 296:	3e2bd2        	l32i	a13, a11, 248
 299:	3e6cd2        	s32i	a13, a12, 248
 29c:	3d2bb2        	l32i	a11, a11, 244
 29f:	3d6cb2        	s32i	a11, a12, 244
 2a2:	0000c6        	j	2a9 <SbcAnalysisFilter8+0x121>

000002a5 <SbcAnalysisFilter8+0x11d>:
 2a5:	668b           	addi.n	a6, a6, 8
 2a7:	866d           	ae_sext16	a6, a6

000002a9 <SbcAnalysisFilter8+0x121>:
 2a9:	2188           	l32i.n	a8, a1, 8
 2ab:	3198           	l32i.n	a9, a1, 12
 2ad:	90a0e2        	movi	a14, 144
 2b0:	c0be60        	sub	a11, a14, a6
 2b3:	a91b           	addi.n	a10, a9, 1
 2b5:	8a9d           	ae_sext16	a9, a10
 2b7:	3199           	s32i.n	a9, a1, 12
 2b9:	c08890        	sub	a8, a8, a9
 2bc:	f0b856        	bnez	a8, 1cb <SbcAnalysisFilter8+0x43>

000002bf <SbcAnalysisFilter8+0x137>:
 2bf:	51b8           	l32i.n	a11, a1, 20
 2c1:	846b62        	s32i	a6, a11, 0x210
 2c4:	f01d           	retw.n

000002c6 <SbcAnalysisFilter8+0x13e>:
	...

000002c8 <SbcAnalysisInit>:
 2c8:	004136        	entry	a1, 32
 2cb:	40c242        	addi	a4, a2, 64
 2ce:	050c           	movi.n	a5, 0
 2d0:	04d282        	addmi	a8, a2, 0x400
 2d3:	70a032        	movi	a3, 112
 2d6:	02a376        	loopgtz	a3, 2dc <SbcAnalysisInit+0x14>
 2d9:	09e454        	ae_s32ip	a5, a4, 4

000002dc <SbcAnalysisInit+0x14>:
 2dc:	2c6852        	s32i	a5, a8, 176
 2df:	f01d           	retw.n

000002e1 <SbcAnalysisInit+0x19>:
 2e1:	00000000 36000000                                .......

000002e8 <SBC_FastIDCT8>:
 2e8:	004136        	entry	a1, 32
 2eb:	fcc272        	addi	a7, a2, -4
 2ee:	ff5a21        	l32r	a2, 58 (dd8 <sbc_enc_as16Offset8+0x78>)
 2f1:	20c362        	addi	a6, a3, 32
 2f4:	205770        	or	a5, a7, a7

000002f7 <SBC_FastIDCT8+0xf>:
 2f7:	548214        	ae_lp24x2.iu	aep0, a2, 8
 2fa:	c8403d151f 	{ ae_lq32f.iu	aeq0, a5, 4; ae_sllip24	aep0, aep0, 8 }
 2ff:	f8203d351f 	{ ae_lq32f.iu	aeq1, a5, 4; ae_mulq32sp16s.h	aeq0, aeq0, aep0 }
 304:	f4e380074f 	{ movi	a4, 7; ae_mulaq32sp16s.l	aeq0, aeq1, aep0 }
 309:	0fa476        	loopgtz	a4, 31c <SBC_FastIDCT8+0x34>
 30c:	549214        	ae_lp24x2.iu	aep1, a2, 8
 30f:	c8447d551f 	{ ae_lq32f.iu	aeq2, a5, 4; ae_sllip24	aep1, aep1, 8 }
 314:	f666bd351f 	{ ae_lq32f.iu	aeq1, a5, 4; ae_mulaq32sp16s.h	aeq0, aeq2, aep1 }
 319:	031154        	ae_mulaq32sp16s.l	aeq0, aeq1, aep1

0000031c <SBC_FastIDCT8+0x34>:
 31c:	368504        	ae_trunca32q48	a5, aeq0
 31f:	09e354        	ae_s32ip	a5, a3, 4
 322:	075d           	mov.n	a5, a7
 324:	cf9367        	bne	a3, a6, 2f7 <SBC_FastIDCT8+0xf>
 327:	f01d           	retw.n

00000329 <SBC_FastIDCT8+0x41>:
 329:	00000000 36000000                                .......

00000330 <mSBC_SW_SN>:
 330:	004136        	entry	a1, 32
 333:	234d           	ae_l16si.n	a4, a3, 0
 335:	1a0c           	movi.n	a10, 1
 337:	050c           	movi.n	a5, 0
 339:	fdc462        	addi	a6, a4, -3
 33c:	441b           	addi.n	a4, a4, 1
 33e:	935460        	movnez	a5, a4, a6
 341:	635d           	ae_s16i.n	a5, a3, 0
 343:	35cc           	bnez.n	a5, 34a <mSBC_SW_SN+0x1a>
 345:	830c           	movi.n	a3, 8
 347:	0004c6        	j	35e <mSBC_SW_SN+0x2e>

0000034a <mSBC_SW_SN+0x1a>:
 34a:	041566        	bnei	a5, 1, 352 <mSBC_SW_SN+0x22>
 34d:	833c           	movi.n	a3, 56
 34f:	0002c6        	j	35e <mSBC_SW_SN+0x2e>

00000352 <mSBC_SW_SN+0x22>:
 352:	fec582        	addi	a8, a5, -2
 355:	c8a062        	movi	a6, 200
 358:	f8a032        	movi	a3, 248
 35b:	833680        	moveqz	a3, a6, a8
 35e:	119380        	slli	a9, a3, 8
 361:	2099a0        	or	a9, a9, a10
 364:	629d           	ae_s16i.n	a9, a2, 0
 366:	f01d           	retw.n

00000368 <mSBC_SW_SN_init>:
 368:	004136        	entry	a1, 32
 36b:	030c           	movi.n	a3, 0
 36d:	623d           	ae_s16i.n	a3, a2, 0
 36f:	f01d           	retw.n

00000371 <mSBC_SW_SN_init+0x9>:
 371:	000000                                        ...

00000374 <mybit_pack>:
 374:	004136        	entry	a1, 32
 377:	021272        	l16ui	a7, a2, 4
 37a:	02a8           	l32i.n	a10, a2, 0
 37c:	938d           	ae_zext16	a8, a3
 37e:	091c           	movi.n	a9, 16
 380:	c099a0        	sub	a9, a9, a10
 383:	11a497        	bge	a4, a9, 398 <mybit_pack+0x24>
 386:	3a4a           	add.n	a3, a10, a4
 388:	401400        	ssl	a4
 38b:	a1b700        	sll	a11, a7
 38e:	20b8b0        	or	a11, a8, a11
 391:	0252b2        	s16i	a11, a2, 4

00000394 <mybit_pack+0x20>:
 394:	0239           	s32i.n	a3, a2, 0
 396:	f01d           	retw.n

00000398 <mybit_pack+0x24>:
 398:	32d8           	l32i.n	a13, a2, 12
 39a:	ff30c1        	l32r	a12, 5c (fe0 <MASK_bitp>)
 39d:	c03490        	sub	a3, a4, a9
 3a0:	401900        	ssl	a9
 3a3:	a1f700        	sll	a15, a7
 3a6:	90c3c0        	addx2	a12, a3, a12
 3a9:	400300        	ssr	a3
 3ac:	b1e080        	sra	a14, a8
 3af:	20eef0        	or	a14, a14, a15
 3b2:	9eed           	ae_zext16	a14, a14
 3b4:	0252e2        	s16i	a14, a2, 4
 3b7:	41f8e0        	srli	a15, a14, 8
 3ba:	11ee80        	slli	a14, a14, 8
 3bd:	20eef0        	or	a14, a14, a15
 3c0:	6ded           	ae_s16i.n	a14, a13, 0
 3c2:	dd2b           	addi.n	a13, a13, 2
 3c4:	2ccd           	ae_l16si.n	a12, a12, 0
 3c6:	32d9           	s32i.n	a13, a2, 12
 3c8:	22d8           	l32i.n	a13, a2, 8
 3ca:	10cc80        	and	a12, a12, a8
 3cd:	0252c2        	s16i	a12, a2, 4
 3d0:	dd1b           	addi.n	a13, a13, 1
 3d2:	22d9           	s32i.n	a13, a2, 8
 3d4:	ffef06        	j	394 <mybit_pack+0x20>

000003d7 <mybit_pack+0x63>:
	...

000003d8 <mSBC_Encoder_Process>:
 3d8:	006136        	entry	a1, 48
 3db:	ff2181        	l32r	a8, 60 (188 <SbcAnalysisFilter8>)
 3de:	04d492        	addmi	a9, a4, 0x400
 3e1:	286922        	s32i	a2, a9, 160
 3e4:	296932        	s32i	a3, a9, 164
 3e7:	a4c9a2        	addi	a10, a9, -92
 3ea:	232932        	l32i	a3, a9, 140
 3ed:	01a9           	s32i.n	a10, a1, 0
 3ef:	020c           	movi.n	a2, 0
 3f1:	2f6922        	s32i	a2, a9, 188
 3f4:	5c5922        	s16i	a2, a9, 184
 3f7:	2d6922        	s32i	a2, a9, 180
 3fa:	04ad           	mov.n	a10, a4
 3fc:	0008e0        	callx8	a8
 3ff:	0e0c           	movi.n	a14, 0
 401:	ff18c1        	l32r	a12, 64 (8000 <_end+0x6fa0>)
 404:	ff1951        	l32r	a5, 68 (800000 <_end+0x7fefa0>)
 407:	e70c           	movi.n	a7, 14
 409:	03d462        	addmi	a6, a4, 0x300
 40c:	02d4f2        	addmi	a15, a4, 0x200
 40f:	40cff2        	addi	a15, a15, 64
 412:	80c6d2        	addi	a13, a6, -128
 415:	a0c662        	addi	a6, a6, -96

00000418 <mSBC_Encoder_Process+0x40>:
 418:	0d9d           	mov.n	a9, a13
 41a:	02bd           	mov.n	a11, a2
 41c:	dd4b           	addi.n	a13, a13, 4
 41e:	1213a6        	blti	a3, 1, 434 <mSBC_Encoder_Process+0x5c>
 421:	0a0c           	movi.n	a10, 0
 423:	0aa376        	loopgtz	a3, 431 <mSBC_Encoder_Process+0x59>
 426:	0988           	l32i.n	a8, a9, 0
 428:	20c992        	addi	a9, a9, 32
 42b:	608180        	abs	a8, a8
 42e:	53a8a0        	max	a10, a8, a10

00000431 <mSBC_Encoder_Process+0x59>:
 431:	000046        	j	436 <mSBC_Encoder_Process+0x5e>

00000434 <mSBC_Encoder_Process+0x5c>:
 434:	0a0c           	movi.n	a10, 0

00000436 <mSBC_Encoder_Process+0x5e>:
 436:	7285a0        	salt	a8, a5, a10
 439:	990c           	movi.n	a9, 9
 43b:	93b980        	movnez	a11, a9, a8
 43e:	1227b7        	blt	a7, a11, 454 <mSBC_Encoder_Process+0x7c>
 441:	f90c           	movi.n	a9, 15
 443:	c099b0        	sub	a9, a9, a11
 446:	0aa976        	loopgtz	a9, 454 <mSBC_Encoder_Process+0x7c>
 449:	401b00        	ssl	a11
 44c:	a18c00        	sll	a8, a12
 44f:	01a8a7        	bge	a8, a10, 454 <mSBC_Encoder_Process+0x7c>
 452:	bb1b           	addi.n	a11, a11, 1
 454:	53ebe0        	max	a14, a11, a14
 457:	8b9d           	ae_sext16	a9, a11
 459:	09ef94        	ae_s32ip	a9, a15, 4
 45c:	b89d67        	bne	a13, a6, 418 <mSBC_Encoder_Process+0x40>
 45f:	0198           	l32i.n	a9, a1, 0
 461:	ff0281        	l32r	a8, 6c (6c0 <sbc_enc_bit_alloc_mono>)
 464:	04ad           	mov.n	a10, a4
 466:	3e69e2        	s32i	a14, a9, 248
 469:	0008e0        	callx8	a8
 46c:	ff0181        	l32r	a8, 70 (4b0 <EncPacking>)
 46f:	04ad           	mov.n	a10, a4
 471:	0138           	l32i.n	a3, a1, 0
 473:	0008e0        	callx8	a8
 476:	486322        	s32i	a2, a3, 0x120
 479:	422322        	l32i	a2, a3, 0x108
 47c:	f01d           	retw.n

0000047e <mSBC_Encoder_Process+0xa6>:
	...

00000480 <SBC_Encoder_Init>:
 480:	004136        	entry	a1, 32
 483:	02ad           	mov.n	a10, a2
 485:	fefb81        	l32r	a8, 74 (2c8 <SbcAnalysisInit>)
 488:	192c           	movi.n	a9, 33
 48a:	ac1c           	movi.n	a12, 26
 48c:	fd0c           	movi.n	a13, 15
 48e:	8e0c           	movi.n	a14, 8
 490:	03d2b2        	addmi	a11, a2, 0x300
 493:	0f0c           	movi.n	a15, 0
 495:	606bf2        	s32i	a15, a11, 0x180
 498:	616bf2        	s32i	a15, a11, 0x184
 49b:	646bf2        	s32i	a15, a11, 0x190
 49e:	626be2        	s32i	a14, a11, 0x188
 4a1:	636bd2        	s32i	a13, a11, 0x18c
 4a4:	656bc2        	s32i	a12, a11, 0x194
 4a7:	6a6b92        	s32i	a9, a11, 0x1a8
 4aa:	0008e0        	callx8	a8
 4ad:	f01d           	retw.n

000004af <SBC_Encoder_Init+0x2f>:
	...

000004b0 <EncPacking>:
 4b0:	008136        	entry	a1, 64
 4b3:	0129           	s32i.n	a2, a1, 0
 4b5:	04d2c2        	addmi	a12, a2, 0x400
 4b8:	03d2a2        	addmi	a10, a2, 0x300
 4bb:	a8ca72        	addi	a7, a10, -88
 4be:	712a92        	l32i	a9, a10, 0x1c4
 4c1:	692ab2        	l32i	a11, a10, 0x1a4
 4c4:	306cb2        	s32i	a11, a12, 192
 4c7:	622aa2        	l32i	a10, a10, 0x188
 4ca:	41a9           	s32i.n	a10, a1, 16
 4cc:	3b3966        	bnei	a9, 3, 50b <EncPacking+0x5b>
 4cf:	9ca0b2        	movi	a11, 156
 4d2:	8c0c           	movi.n	a12, 8
 4d4:	05d222        	addmi	a2, a2, 0x500
 4d7:	fee881        	l32r	a8, 78 (374 <mybit_pack>)
 4da:	b4c222        	addi	a2, a2, -76
 4dd:	02ad           	mov.n	a10, a2
 4df:	0008e0        	callx8	a8
 4e2:	02ad           	mov.n	a10, a2
 4e4:	fee581        	l32r	a8, 78 (374 <mybit_pack>)
 4e7:	8027b2        	l32i	a11, a7, 0x200
 4ea:	8c0c           	movi.n	a12, 8
 4ec:	0008e0        	callx8	a8
 4ef:	02ad           	mov.n	a10, a2
 4f1:	fee181        	l32r	a8, 78 (374 <mybit_pack>)
 4f4:	7b27b2        	l32i	a11, a7, 0x1ec
 4f7:	8c0c           	movi.n	a12, 8
 4f9:	0008e0        	callx8	a8
 4fc:	02ad           	mov.n	a10, a2
 4fe:	fede81        	l32r	a8, 78 (374 <mybit_pack>)
 501:	0b0c           	movi.n	a11, 0
 503:	8c0c           	movi.n	a12, 8
 505:	0008e0        	callx8	a8
 508:	872792        	l32i	a9, a7, 0x21c

0000050b <EncPacking+0x5b>:
 50b:	79fc           	bnez.n	a9, 546 <EncPacking+0x96>
 50d:	ada0b2        	movi	a11, 173
 510:	0128           	l32i.n	a2, a1, 0
 512:	8c0c           	movi.n	a12, 8
 514:	fed981        	l32r	a8, 78 (374 <mybit_pack>)
 517:	05d222        	addmi	a2, a2, 0x500
 51a:	b4c222        	addi	a2, a2, -76
 51d:	02ad           	mov.n	a10, a2
 51f:	0008e0        	callx8	a8
 522:	02ad           	mov.n	a10, a2
 524:	fed581        	l32r	a8, 78 (374 <mybit_pack>)
 527:	0b0c           	movi.n	a11, 0
 529:	8c0c           	movi.n	a12, 8
 52b:	0008e0        	callx8	a8
 52e:	02ad           	mov.n	a10, a2
 530:	fed281        	l32r	a8, 78 (374 <mybit_pack>)
 533:	0b0c           	movi.n	a11, 0
 535:	8c0c           	movi.n	a12, 8
 537:	0008e0        	callx8	a8
 53a:	02ad           	mov.n	a10, a2
 53c:	fecf81        	l32r	a8, 78 (374 <mybit_pack>)
 53f:	0b0c           	movi.n	a11, 0
 541:	8c0c           	movi.n	a12, 8
 543:	0008e0        	callx8	a8

00000546 <EncPacking+0x96>:
 546:	4198           	l32i.n	a9, a1, 16
 548:	0168           	l32i.n	a6, a1, 0
 54a:	093d           	mov.n	a3, a9
 54c:	02d662        	addmi	a6, a6, 0x200
 54f:	40c662        	addi	a6, a6, 64
 552:	3169           	s32i.n	a6, a1, 12
 554:	3319a6        	blti	a9, 1, 58b <EncPacking+0xdb>
 557:	0128           	l32i.n	a2, a1, 0
 559:	090c           	movi.n	a9, 0
 55b:	040c           	movi.n	a4, 0
 55d:	05d222        	addmi	a2, a2, 0x500
 560:	b4c222        	addi	a2, a2, -76

00000563 <EncPacking+0xb3>:
 563:	0688           	l32i.n	a8, a6, 0
 565:	1144c0        	slli	a4, a4, 4
 568:	664b           	addi.n	a6, a6, 4
 56a:	204840        	or	a4, a8, a4
 56d:	744040        	extui	a4, a4, 0, 8
 570:	101966        	bnei	a9, 1, 584 <EncPacking+0xd4>
 573:	02ad           	mov.n	a10, a2
 575:	fec081        	l32r	a8, 78 (374 <mybit_pack>)
 578:	04bd           	mov.n	a11, a4
 57a:	8c0c           	movi.n	a12, 8
 57c:	0008e0        	callx8	a8
 57f:	090c           	movi.n	a9, 0
 581:	000046        	j	586 <EncPacking+0xd6>

00000584 <EncPacking+0xd4>:
 584:	190c           	movi.n	a9, 1

00000586 <EncPacking+0xd6>:
 586:	330b           	addi.n	a3, a3, -1
 588:	fd7356        	bnez	a3, 563 <EncPacking+0xb3>

0000058b <EncPacking+0xdb>:
 58b:	7927a2        	l32i	a10, a7, 0x1e4
 58e:	0158           	l32i.n	a5, a1, 0
 590:	6179           	s32i.n	a7, a1, 24
 592:	ba0b           	addi.n	a11, a10, -1
 594:	03d552        	addmi	a5, a5, 0x300
 597:	80c552        	addi	a5, a5, -128
 59a:	51b9           	s32i.n	a11, a1, 20
 59c:	751aa6        	blti	a10, 1, 615 <EncPacking+0x165>
 59f:	41a8           	l32i.n	a10, a1, 16
 5a1:	0188           	l32i.n	a8, a1, 0
 5a3:	feb671        	l32r	a7, 7c (ffff <_end+0xef9f>)
 5a6:	aa0b           	addi.n	a10, a10, -1
 5a8:	05d822        	addmi	a2, a8, 0x500
 5ab:	b4c222        	addi	a2, a2, -76
 5ae:	21a9           	s32i.n	a10, a1, 8
 5b0:	02d882        	addmi	a8, a8, 0x200
 5b3:	60c882        	addi	a8, a8, 96
 5b6:	1189           	s32i.n	a8, a1, 4

000005b8 <EncPacking+0x108>:
 5b8:	2138           	l32i.n	a3, a1, 8
 5ba:	4188           	l32i.n	a8, a1, 16
 5bc:	3148           	l32i.n	a4, a1, 12
 5be:	1168           	l32i.n	a6, a1, 4
 5c0:	4818a6        	blti	a8, 1, 60c <EncPacking+0x15c>

000005c3 <EncPacking+0x113>:
 5c3:	06c8           	l32i.n	a12, a6, 0
 5c5:	664b           	addi.n	a6, a6, 4
 5c7:	8cbc           	beqz.n	a12, 603 <EncPacking+0x153>
 5c9:	02ad           	mov.n	a10, a2
 5cb:	feab81        	l32r	a8, 78 (374 <mybit_pack>)
 5ce:	05b8           	l32i.n	a11, a5, 0
 5d0:	1e0c           	movi.n	a14, 1
 5d2:	04d8           	l32i.n	a13, a4, 0
 5d4:	091c           	movi.n	a9, 16
 5d6:	c099c0        	sub	a9, a9, a12
 5d9:	fdfb           	addi.n	a15, a13, 15
 5db:	401f00        	ssl	a15
 5de:	a1ee00        	sll	a14, a14
 5e1:	400900        	ssr	a9
 5e4:	bbea           	add.n	a11, a11, a14
 5e6:	b1f070        	sra	a15, a7
 5e9:	11ff10        	slli	a15, a15, 15
 5ec:	82ebf0        	mull	a14, a11, a15
 5ef:	b2bbf0        	mulsh	a11, a11, a15
 5f2:	404f10        	ssai	31
 5f5:	81bbe0        	src	a11, a11, a14
 5f8:	400d00        	ssr	a13
 5fb:	b1b0b0        	sra	a11, a11
 5fe:	9bbd           	ae_zext16	a11, a11
 600:	0008e0        	callx8	a8

00000603 <EncPacking+0x153>:
 603:	444b           	addi.n	a4, a4, 4
 605:	554b           	addi.n	a5, a5, 4
 607:	330b           	addi.n	a3, a3, -1
 609:	b60366        	bnei	a3, -1, 5c3 <EncPacking+0x113>

0000060c <EncPacking+0x15c>:
 60c:	51a8           	l32i.n	a10, a1, 20
 60e:	aa0b           	addi.n	a10, a10, -1
 610:	51a9           	s32i.n	a10, a1, 20
 612:	a20a66        	bnei	a10, -1, 5b8 <EncPacking+0x108>

00000615 <EncPacking+0x165>:
 615:	6178           	l32i.n	a7, a1, 24
 617:	8527d2        	l32i	a13, a7, 0x214
 61a:	8327b2        	l32i	a11, a7, 0x20c
 61d:	11ddf0        	slli	a13, a13, 1
 620:	8167d2        	s32i	a13, a7, 0x204
 623:	bb9c           	beqz.n	a11, 642 <EncPacking+0x192>
 625:	fe9481        	l32r	a8, 78 (374 <mybit_pack>)
 628:	0c1c           	movi.n	a12, 16
 62a:	01a8           	l32i.n	a10, a1, 0
 62c:	c0ccb0        	sub	a12, a12, a11
 62f:	0b0c           	movi.n	a11, 0
 631:	05daa2        	addmi	a10, a10, 0x500
 634:	b4caa2        	addi	a10, a10, -76
 637:	0008e0        	callx8	a8
 63a:	8127d2        	l32i	a13, a7, 0x204
 63d:	dd1b           	addi.n	a13, a13, 1
 63f:	8167d2        	s32i	a13, a7, 0x204

00000642 <EncPacking+0x192>:
 642:	7f2722        	l32i	a2, a7, 0x1fc
 645:	df7c           	movi.n	a15, -3
 647:	41a8           	l32i.n	a10, a1, 16
 649:	025d           	mov.n	a5, a2
 64b:	22cd           	ae_l16si.n	a12, a2, 0
 64d:	02ed           	mov.n	a14, a2
 64f:	21a1a0        	srai	a10, a10, 1
 652:	7448c0        	extui	a4, a12, 8, 8
 655:	44afa7        	bge	a15, a10, 69d <EncPacking+0x1ed>
 658:	130c           	movi.n	a3, 1
 65a:	fa4b           	addi.n	a15, a10, 4
 65c:	fa0c           	movi.n	a10, 15

0000065e <EncPacking+0x1ae>:
 65e:	233326        	beqi	a3, 3, 685 <EncPacking+0x1d5>
 661:	790c           	movi.n	a9, 7
 663:	8b0c           	movi.n	a11, 8
 665:	1cab76        	loopgtz	a11, 685 <EncPacking+0x1d5>
 668:	400900        	ssr	a9
 66b:	21b7a0        	srai	a11, a10, 7
 66e:	990b           	addi.n	a9, a9, -1
 670:	11aaf0        	slli	a10, a10, 1
 673:	b18040        	sra	a8, a4
 676:	30bb80        	xor	a11, a11, a8
 679:	04b0b0        	extui	a11, a11, 0, 1
 67c:	f08bb0        	subx8	a8, a11, a11
 67f:	a088b0        	addx4	a8, a8, a11
 682:	30a8a0        	xor	a10, a8, a10

00000685 <EncPacking+0x1d5>:
 685:	05e307        	bbsi	a3, 0, 68e <EncPacking+0x1de>
 688:	7448c0        	extui	a4, a12, 8, 8
 68b:	000186        	j	695 <EncPacking+0x1e5>

0000068e <EncPacking+0x1de>:
 68e:	3ecd           	ae_l16si.n	a12, a14, 2
 690:	ee2b           	addi.n	a14, a14, 2
 692:	7440c0        	extui	a4, a12, 0, 8

00000695 <EncPacking+0x1e5>:
 695:	331b           	addi.n	a3, a3, 1
 697:	c39f37        	bne	a15, a3, 65e <EncPacking+0x1ae>
 69a:	000046        	j	69f <EncPacking+0x1ef>

0000069d <EncPacking+0x1ed>:
 69d:	fa0c           	movi.n	a10, 15

0000069f <EncPacking+0x1ef>:
 69f:	872792        	l32i	a9, a7, 0x21c
 6a2:	013926        	beqi	a9, 3, 6a7 <EncPacking+0x1f7>
 6a5:	e9cc           	bnez.n	a9, 6b7 <EncPacking+0x207>

000006a7 <EncPacking+0x1f7>:
 6a7:	7f2752        	l32i	a5, a7, 0x1fc
 6aa:	329d           	ae_l16si.n	a9, a2, 2
 6ac:	8127d2        	l32i	a13, a7, 0x204
 6af:	11ba80        	slli	a11, a10, 8
 6b2:	2099b0        	or	a9, a9, a11
 6b5:	729d           	ae_s16i.n	a9, a2, 2

000006b7 <EncPacking+0x207>:
 6b7:	90cd50        	addx2	a12, a13, a5
 6ba:	7f67c2        	s32i	a12, a7, 0x1fc
 6bd:	f01d           	retw.n

000006bf <EncPacking+0x20f>:
	...

000006c0 <sbc_enc_bit_alloc_mono>:
 6c0:	004136        	entry	a1, 32
 6c3:	024d           	mov.n	a4, a2
 6c5:	fe6e61        	l32r	a6, 80 (d60 <sbc_enc_as16Offset8>)
 6c8:	02d2a2        	addmi	a10, a2, 0x200
 6cb:	03d292        	addmi	a9, a2, 0x300
 6ce:	602982        	l32i	a8, a9, 0x180
 6d1:	0a5d           	mov.n	a5, a10
 6d3:	8cc9b2        	addi	a11, a9, -116
 6d6:	1188b0        	slli	a8, a8, 5
 6d9:	622992        	l32i	a9, a9, 0x188
 6dc:	668a           	add.n	a6, a6, a8
 6de:	fbaf82        	movi	a8, -5
 6e1:	1ca976        	loopgtz	a9, 701 <sbc_enc_bit_alloc_mono+0x41>
 6e4:	902472        	l32i	a7, a4, 0x240
 6e7:	444b           	addi.n	a4, a4, 4
 6e9:	37cc           	bnez.n	a7, 6f0 <sbc_enc_bit_alloc_mono+0x30>
 6eb:	0589           	s32i.n	a8, a5, 0
 6ed:	000306        	j	6fd <sbc_enc_bit_alloc_mono+0x3d>

000006f0 <sbc_enc_bit_alloc_mono+0x30>:
 6f0:	06c8           	l32i.n	a12, a6, 0
 6f2:	c077c0        	sub	a7, a7, a12
 6f5:	0217a6        	blti	a7, 1, 6fb <sbc_enc_bit_alloc_mono+0x3b>
 6f8:	217170        	srai	a7, a7, 1

000006fb <sbc_enc_bit_alloc_mono+0x3b>:
 6fb:	0579           	s32i.n	a7, a5, 0

000006fd <sbc_enc_bit_alloc_mono+0x3d>:
 6fd:	554b           	addi.n	a5, a5, 4
 6ff:	664b           	addi.n	a6, a6, 4

00000701 <sbc_enc_bit_alloc_mono+0x41>:
 701:	0a5d           	mov.n	a5, a10
 703:	1019a6        	blti	a9, 1, 717 <sbc_enc_bit_alloc_mono+0x57>
 706:	00a042        	movi	a4, 0
 709:	06a976        	loopgtz	a9, 713 <sbc_enc_bit_alloc_mono+0x53>
 70c:	05d8           	l32i.n	a13, a5, 0
 70e:	554b           	addi.n	a5, a5, 4
 710:	534d40        	max	a4, a13, a4
 713:	000086        	j	719 <sbc_enc_bit_alloc_mono+0x59>

00000716 <sbc_enc_bit_alloc_mono+0x56>:
	...

00000717 <sbc_enc_bit_alloc_mono+0x57>:
 717:	040c           	movi.n	a4, 0

00000719 <sbc_enc_bit_alloc_mono+0x59>:
 719:	641b           	addi.n	a6, a4, 1
 71b:	822b42        	l32i	a4, a11, 0x208
 71e:	0a5d           	mov.n	a5, a10

00000720 <sbc_enc_bit_alloc_mono+0x60>:
 720:	04bd           	mov.n	a11, a4
 722:	660b           	addi.n	a6, a6, -1
 724:	1f19a6        	blti	a9, 1, 747 <sbc_enc_bit_alloc_mono+0x87>
 727:	070c           	movi.n	a7, 0
 729:	17a976        	loopgtz	a9, 744 <sbc_enc_bit_alloc_mono+0x84>
 72c:	0548           	l32i.n	a4, a5, 0
 72e:	554b           	addi.n	a5, a5, 4
 730:	c04460        	sub	a4, a4, a6
 733:	0bb4e6        	bgei	a4, 16, 742 <sbc_enc_bit_alloc_mono+0x82>
 736:	0814a6        	blti	a4, 1, 742 <sbc_enc_bit_alloc_mono+0x82>
 739:	c40b           	addi.n	a12, a4, -1
 73b:	871b           	addi.n	a8, a7, 1
 73d:	772b           	addi.n	a7, a7, 2
 73f:	9378c0        	movnez	a7, a8, a12
 742:	f03d           	nop.n

00000744 <sbc_enc_bit_alloc_mono+0x84>:
 744:	000046        	j	749 <sbc_enc_bit_alloc_mono+0x89>

00000747 <sbc_enc_bit_alloc_mono+0x87>:
 747:	070c           	movi.n	a7, 0

00000749 <sbc_enc_bit_alloc_mono+0x89>:
 749:	0a5d           	mov.n	a5, a10
 74b:	c04b70        	sub	a4, a11, a7
 74e:	ce14e6        	bgei	a4, 1, 720 <sbc_enc_bit_alloc_mono+0x60>
 751:	2bcc           	bnez.n	a11, 757 <sbc_enc_bit_alloc_mono+0x97>
 753:	04bd           	mov.n	a11, a4
 755:	660b           	addi.n	a6, a6, -1

00000757 <sbc_enc_bit_alloc_mono+0x97>:
 757:	0a5d           	mov.n	a5, a10
 759:	862b           	addi.n	a8, a6, 2
 75b:	0d0c           	movi.n	a13, 0
 75d:	02d2c2        	addmi	a12, a2, 0x200
 760:	60ccc2        	addi	a12, a12, 96
 763:	0c2d           	mov.n	a2, a12
 765:	1ca976        	loopgtz	a9, 785 <sbc_enc_bit_alloc_mono+0xc5>
 768:	0578           	l32i.n	a7, a5, 0
 76a:	554b           	addi.n	a5, a5, 4
 76c:	04a787        	bge	a7, a8, 774 <sbc_enc_bit_alloc_mono+0xb4>
 76f:	02d9           	s32i.n	a13, a2, 0
 771:	000386        	j	783 <sbc_enc_bit_alloc_mono+0xc3>

00000774 <sbc_enc_bit_alloc_mono+0xb4>:
 774:	c04760        	sub	a4, a7, a6
 777:	04b4e6        	bgei	a4, 16, 77f <sbc_enc_bit_alloc_mono+0xbf>
 77a:	844d           	ae_sext16	a4, a4
 77c:	000046        	j	781 <sbc_enc_bit_alloc_mono+0xc1>

0000077f <sbc_enc_bit_alloc_mono+0xbf>:
 77f:	041c           	movi.n	a4, 16

00000781 <sbc_enc_bit_alloc_mono+0xc1>:
 781:	0249           	s32i.n	a4, a2, 0

00000783 <sbc_enc_bit_alloc_mono+0xc3>:
 783:	224b           	addi.n	a2, a2, 4

00000785 <sbc_enc_bit_alloc_mono+0xc5>:
 785:	0a5d           	mov.n	a5, a10
 787:	0c2d           	mov.n	a2, a12
 789:	280c           	movi.n	a8, 2
 78b:	761b           	addi.n	a7, a6, 1
 78d:	060c           	movi.n	a6, 0

0000078f <sbc_enc_bit_alloc_mono+0xcf>:
 78f:	291ba6        	blti	a11, 1, 7bc <sbc_enc_bit_alloc_mono+0xfc>
 792:	26a697        	bge	a6, a9, 7bc <sbc_enc_bit_alloc_mono+0xfc>
 795:	0248           	l32i.n	a4, a2, 0
 797:	661b           	addi.n	a6, a6, 1
 799:	0b24a6        	blti	a4, 2, 7a8 <sbc_enc_bit_alloc_mono+0xe8>
 79c:	08b4e6        	bgei	a4, 16, 7a8 <sbc_enc_bit_alloc_mono+0xe8>
 79f:	bb0b           	addi.n	a11, a11, -1
 7a1:	d41b           	addi.n	a13, a4, 1
 7a3:	02d9           	s32i.n	a13, a2, 0
 7a5:	000306        	j	7b5 <sbc_enc_bit_alloc_mono+0xf5>

000007a8 <sbc_enc_bit_alloc_mono+0xe8>:
 7a8:	05e8           	l32i.n	a14, a5, 0
 7aa:	079e77        	bne	a14, a7, 7b5 <sbc_enc_bit_alloc_mono+0xf5>
 7ad:	042ba6        	blti	a11, 2, 7b5 <sbc_enc_bit_alloc_mono+0xf5>
 7b0:	0289           	s32i.n	a8, a2, 0
 7b2:	fecbb2        	addi	a11, a11, -2
 7b5:	224b           	addi.n	a2, a2, 4
 7b7:	554b           	addi.n	a5, a5, 4
 7b9:	fff486        	j	78f <sbc_enc_bit_alloc_mono+0xcf>

000007bc <sbc_enc_bit_alloc_mono+0xfc>:
 7bc:	0c2d           	mov.n	a2, a12
 7be:	060c           	movi.n	a6, 0

000007c0 <sbc_enc_bit_alloc_mono+0x100>:
 7c0:	141ba6        	blti	a11, 1, 7d8 <sbc_enc_bit_alloc_mono+0x118>
 7c3:	11a697        	bge	a6, a9, 7d8 <sbc_enc_bit_alloc_mono+0x118>
 7c6:	0248           	l32i.n	a4, a2, 0
 7c8:	661b           	addi.n	a6, a6, 1
 7ca:	05b4e6        	bgei	a4, 16, 7d3 <sbc_enc_bit_alloc_mono+0x113>
 7cd:	bb0b           	addi.n	a11, a11, -1
 7cf:	f41b           	addi.n	a15, a4, 1
 7d1:	02f9           	s32i.n	a15, a2, 0

000007d3 <sbc_enc_bit_alloc_mono+0x113>:
 7d3:	224b           	addi.n	a2, a2, 4
 7d5:	fff9c6        	j	7c0 <sbc_enc_bit_alloc_mono+0x100>

000007d8 <sbc_enc_bit_alloc_mono+0x118>:
 7d8:	f01d           	retw.n

000007da <sbc_enc_bit_alloc_mono+0x11a>:
 7da:	00000000 41360000                                ......

000007e0 <__do_global_ctors_aux>:
 7e0:	004136        	entry	a1, 32
 7e3:	fe2881        	l32r	a8, 84 (9f0 <_DYNAMIC+0x1d8>)
 7e6:	7f2882        	l32i	a8, a8, 0x1fc
 7e9:	fe2721        	l32r	a2, 88 (bec <__CTOR_LIST___47>)
 7ec:	0c0826        	beqi	a8, -1, 7fc <__do_global_ctors_aux+0x1c>
 7ef:	02a8           	l32i.n	a10, a2, 0

000007f1 <__do_global_ctors_aux+0x11>:
 7f1:	000ae0        	callx8	a10
 7f4:	fcc222        	addi	a2, a2, -4
 7f7:	02a8           	l32i.n	a10, a2, 0
 7f9:	f40a66        	bnei	a10, -1, 7f1 <__do_global_ctors_aux+0x11>

000007fc <__do_global_ctors_aux+0x1c>:
 7fc:	f01d           	retw.n

000007fe <__do_global_ctors_aux+0x1e>:
	...

00000800 <_fini>:
 800:	008136        	entry	a1, 64
 803:	fe2281        	l32r	a8, 8c (a8 <__do_global_dtors_aux>)
 806:	f03d           	nop.n
 808:	0008e0        	callx8	a8

0000080b <_fini+0xb>:
 80b:	f01d           	retw.n
