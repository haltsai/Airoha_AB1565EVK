
Build/lib/pisplit_peq2_position_independent.splitload.lib:     file format elf32-xtensa-le


Disassembly of section .text:

00000000 <_init-0x28>:
   0:	00000628 000005f4 00000058 00000604     (.......X.......
  10:	000003d8 00000620 000001dc 00000110     .... ...........
  20:	000002c8 00000350                       ....P...

00000028 <_init>:
  28:	008136        	entry	a1, 64
  2b:	f01d           	retw.n

0000002d <_init+0x5>:
  2d:	000000                                        ...

00000030 <_start>:
  30:	004136        	entry	a1, 32
  33:	fff4a1        	l32r	a10, 4 (5f4 <_DYNAMIC+0x1e4>)
  36:	fff231        	l32r	a3, 0 (628 <__bss_start>)
  39:	002282        	l32i	a8, a2, 0
  3c:	fff3b1        	l32r	a11, 8 (58 <PEQ2_INIT>)
  3f:	006382        	s32i	a8, a3, 0
  42:	0008e0        	callx8	a8
  45:	0388           	l32i.n	a8, a3, 0
  47:	fff1a1        	l32r	a10, c (604 <_DYNAMIC+0x1f4>)
  4a:	fff1b1        	l32r	a11, 10 (3d8 <PEQ2_PROC>)
  4d:	0008e0        	callx8	a8
  50:	fff121        	l32r	a2, 14 (620 <export_parameter_array>)
  53:	f01d           	retw.n

00000055 <_start+0x25>:
  55:	000000                                        ...

00000058 <PEQ2_INIT>:
  58:	004136        	entry	a1, 32
  5b:	229d           	ae_l16si.n	a9, a2, 0
  5d:	02c222        	addi	a2, a2, 2
  60:	639d           	ae_s16i.n	a9, a3, 0
  62:	6719a6        	blti	a9, 1, cd <PEQ2_INIT+0x75>
  65:	00a062        	movi	a6, 0
  68:	5e8076        	loop	a0, ca <PEQ2_INIT+0x72>
  6b:	0692b2        	l16si	a11, a2, 12
  6e:	071272        	l16ui	a7, a2, 14
  71:	0892c2        	l16si	a12, a2, 16
  74:	0292e2        	l16si	a14, a2, 4
  77:	031292        	l16ui	a9, a2, 6
  7a:	0492d2        	l16si	a13, a2, 8
  7d:	051282        	l16ui	a8, a2, 10
  80:	22fd           	ae_l16si.n	a15, a2, 0
  82:	099252        	l16si	a5, a2, 18
  85:	461b           	addi.n	a4, a6, 1
  87:	52ad           	ae_l16ui.n	a10, a2, 2
  89:	a06630        	addx4	a6, a6, a3
  8c:	955d           	ae_zext16	a5, a5
  8e:	0bffa0        	depbits	a10, a15, 16, 16
  91:	0bfd80        	depbits	a8, a13, 16, 16
  94:	0bfe90        	depbits	a9, a14, 16, 16
  97:	0bfc50        	depbits	a5, a12, 16, 16
  9a:	0bfb70        	depbits	a7, a11, 16, 16
  9d:	607070        	neg	a7, a7
  a0:	605050        	neg	a5, a5
  a3:	219890        	srai	a9, a9, 8
  a6:	218880        	srai	a8, a8, 8
  a9:	21a8a0        	srai	a10, a10, 8
  ac:	16a9           	s32i.n	a10, a6, 4
  ae:	1f6682        	s32i	a8, a6, 124
  b1:	106692        	s32i	a9, a6, 64
  b4:	215850        	srai	a5, a5, 8
  b7:	217870        	srai	a7, a7, 8
  ba:	2e6672        	s32i	a7, a6, 184
  bd:	3d6652        	s32i	a5, a6, 244
  c0:	239d           	ae_l16si.n	a9, a3, 0
  c2:	14c222        	addi	a2, a2, 20
  c5:	846d           	ae_sext16	a6, a4
  c7:	02a697        	bge	a6, a9, cd <PEQ2_INIT+0x75>

000000ca <PEQ2_INIT+0x72>:
  ca:	ffe686        	j	68 <PEQ2_INIT+0x10>

000000cd <PEQ2_INIT+0x75>:
  cd:	22ad           	ae_l16si.n	a10, a2, 0
  cf:	528d           	ae_l16ui.n	a8, a2, 2
  d1:	0bfa80        	depbits	a8, a10, 16, 16
  d4:	218880        	srai	a8, a8, 8
  d7:	4c6382        	s32i	a8, a3, 0x130
  da:	2d19a6        	blti	a9, 1, 10b <PEQ2_INIT+0xb3>
  dd:	020c           	movi.n	a2, 0
  df:	060c           	movi.n	a6, 0
  e1:	238076        	loop	a0, 108 <PEQ2_INIT+0xb0>
  e4:	a04230        	addx4	a4, a2, a3
  e7:	b66462        	s32i	a6, a4, 0x2d8
  ea:	a76462        	s32i	a6, a4, 0x29c
  ed:	986462        	s32i	a6, a4, 0x260
  f0:	896462        	s32i	a6, a4, 0x224
  f3:	7a6462        	s32i	a6, a4, 0x1e8
  f6:	6b6462        	s32i	a6, a4, 0x1ac
  f9:	5c6462        	s32i	a6, a4, 0x170
  fc:	4d6462        	s32i	a6, a4, 0x134
  ff:	23bd           	ae_l16si.n	a11, a3, 0
 101:	221b           	addi.n	a2, a2, 1
 103:	822d           	ae_sext16	a2, a2
 105:	02a2b7        	bge	a2, a11, 10b <PEQ2_INIT+0xb3>

00000108 <PEQ2_INIT+0xb0>:
 108:	fff546        	j	e1 <PEQ2_INIT+0x89>

0000010b <PEQ2_INIT+0xb3>:
 10b:	f01d           	retw.n

0000010d <PEQ2_INIT+0xb5>:
 10d:	000000                                        ...

00000110 <APPLY_PEQ_GAIN>:
 110:	004136        	entry	a1, 32
 113:	fcc222        	addi	a2, a2, -4
 116:	4c0304        	ae_lp24.i	aep0, a3, 0
 119:	064516        	beqz	a5, 181 <APPLY_PEQ_GAIN+0x71>
 11c:	5f14a6        	blti	a4, 1, 17f <APPLY_PEQ_GAIN+0x6f>
 11f:	730c           	movi.n	a3, 7
 121:	146040        	extui	a6, a4, 0, 2
 124:	045d           	mov.n	a5, a4
 126:	214250        	srai	a4, a5, 2
 129:	11a676        	loopgtz	a6, 13e <APPLY_PEQ_GAIN+0x2e>
 12c:	60d214        	ae_lq32f.iu	aeq3, a2, 4
 12f:	088de4        	ae_mulfq32sp24s.l	aeq3, aeq3, aep0
 132:	34b334        	ae_sllasq56s	aeq2, aeq3, a3
 135:	066934        	ae_roundsq32asym	aeq1, aeq2
 138:	060944        	ae_satq48s	aeq0, aeq1
 13b:	620204        	ae_sq32f.i	aeq0, a2, 0
 13e:	3da476        	loopgtz	a4, 17f <APPLY_PEQ_GAIN+0x6f>
 141:	600214        	ae_lq32f.i	aeq0, a2, 4
 144:	f8807d022f 	{ ae_lq32f.i	aeq0, a2, 8; ae_mulfq32sp24s.l	aeq1, aeq0, aep0 }
 149:	f9803d023f 	{ ae_lq32f.i	aeq0, a2, 12; ae_mulfq32sp24s.l	aeq2, aeq0, aep0 }
 14e:	f9807d124f 	{ ae_lq32f.iu	aeq0, a2, 16; ae_mulfq32sp24s.l	aeq3, aeq0, aep0 }
 153:	f8807e931f 	{ ae_sllasq56s	aeq0, aeq1, a3; ae_mulfq32sp24s.l	aeq1, aeq0, aep0 }
 158:	fe247ea31f 	{ ae_sllasq56s	aeq0, aeq2, a3; ae_roundsq32asym	aeq2, aeq0 }
 15d:	ff247eb31f 	{ ae_sllasq56s	aeq0, aeq3, a3; ae_roundsq32asym	aeq3, aeq0 }
 162:	fd247e931f 	{ ae_sllasq56s	aeq0, aeq1, a3; ae_roundsq32asym	aeq1, aeq0 }
 167:	060934        	ae_roundsq32asym	aeq0, aeq0
 16a:	068a44        	ae_satq48s	aeq2, aeq2
 16d:	fe84f9a2df 	{ ae_sq32f.i	aeq2, a2, -12; ae_satq48s	aeq2, aeq3 }
 172:	fd04f9a2ef 	{ ae_sq32f.i	aeq2, a2, -8; ae_satq48s	aeq1, aeq1 }
 177:	fc00f992ff 	{ ae_sq32f.i	aeq1, a2, -4; ae_satq48s	aeq0, aeq0 }
 17c:	620204        	ae_sq32f.i	aeq0, a2, 0

0000017f <APPLY_PEQ_GAIN+0x6f>:
 17f:	f01d           	retw.n

00000181 <APPLY_PEQ_GAIN+0x71>:
 181:	fa14a6        	blti	a4, 1, 17f <APPLY_PEQ_GAIN+0x6f>
 184:	330c           	movi.n	a3, 3
 186:	148040        	extui	a8, a4, 0, 2
 189:	045d           	mov.n	a5, a4
 18b:	214250        	srai	a4, a5, 2
 18e:	0ea876        	loopgtz	a8, 1a0 <APPLY_PEQ_GAIN+0x90>
 191:	609214        	ae_lq32f.iu	aeq2, a2, 4
 194:	0885a4        	ae_mulfq32sp24s.l	aeq2, aeq2, aep0
 197:	346334        	ae_sllasq56s	aeq1, aeq2, a3
 19a:	061934        	ae_roundsq32asym	aeq0, aeq1
 19d:	620204        	ae_sq32f.i	aeq0, a2, 0
 1a0:	33a476        	loopgtz	a4, 1d7 <APPLY_PEQ_GAIN+0xc7>
 1a3:	600214        	ae_lq32f.i	aeq0, a2, 4
 1a6:	f8807d022f 	{ ae_lq32f.i	aeq0, a2, 8; ae_mulfq32sp24s.l	aeq1, aeq0, aep0 }
 1ab:	f9803d023f 	{ ae_lq32f.i	aeq0, a2, 12; ae_mulfq32sp24s.l	aeq2, aeq0, aep0 }
 1b0:	f9807d124f 	{ ae_lq32f.iu	aeq0, a2, 16; ae_mulfq32sp24s.l	aeq3, aeq0, aep0 }
 1b5:	f8807e931f 	{ ae_sllasq56s	aeq0, aeq1, a3; ae_mulfq32sp24s.l	aeq1, aeq0, aep0 }
 1ba:	fe247ea31f 	{ ae_sllasq56s	aeq0, aeq2, a3; ae_roundsq32asym	aeq2, aeq0 }
 1bf:	ff247eb31f 	{ ae_sllasq56s	aeq0, aeq3, a3; ae_roundsq32asym	aeq3, aeq0 }
 1c4:	fd247e931f 	{ ae_sllasq56s	aeq0, aeq1, a3; ae_roundsq32asym	aeq1, aeq0 }
 1c9:	fc2479a2df 	{ ae_sq32f.i	aeq2, a2, -12; ae_roundsq32asym	aeq0, aeq0 }
 1ce:	6232e4        	ae_sq32f.i	aeq3, a2, -8
 1d1:	6212f4        	ae_sq32f.i	aeq1, a2, -4
 1d4:	620204        	ae_sq32f.i	aeq0, a2, 0

000001d7 <APPLY_PEQ_GAIN+0xc7>:
 1d7:	f01d           	retw.n

000001d9 <APPLY_PEQ_GAIN+0xc9>:
 1d9:	000000                                        ...

000001dc <BIQUARD_FRAMEBASE>:
 1dc:	008136        	entry	a1, 64
 1df:	4c0304        	ae_lp24.i	aep0, a3, 0
 1e2:	4c1314        	ae_lp24.i	aep1, a3, 4
 1e5:	4c2324        	ae_lp24.i	aep2, a3, 8
 1e8:	4c3334        	ae_lp24.i	aep3, a3, 12
 1eb:	4c4344        	ae_lp24.i	aep4, a3, 16
 1ee:	047d           	mov.n	a7, a4
 1f0:	60c704        	ae_lq32f.i	aeq3, a7, 0
 1f3:	608714        	ae_lq32f.i	aeq2, a7, 4
 1f6:	604724        	ae_lq32f.i	aeq1, a7, 8
 1f9:	600734        	ae_lq32f.i	aeq0, a7, 12
 1fc:	451b           	addi.n	a4, a5, 1
 1fe:	b34550        	movgez	a4, a5, a5
 201:	214140        	srai	a4, a4, 1
 204:	845d           	ae_sext16	a5, a4
 206:	fcc242        	addi	a4, a2, -4
 209:	0215e6        	bgei	a5, 1, 20f <BIQUARD_FRAMEBASE+0x33>
 20c:	002806        	j	2b0 <BIQUARD_FRAMEBASE+0xd4>
 20f:	202440        	or	a2, a4, a4
 212:	203440        	or	a3, a4, a4
 215:	632124        	ae_sq56s.i	aeq2, a1, 16
 218:	633104        	ae_sq56s.i	aeq3, a1, 0
 21b:	630114        	ae_sq56s.i	aeq0, a1, 8
 21e:	631134        	ae_sq56s.i	aeq1, a1, 24
 221:	68a576        	loopgtz	a5, 28d <BIQUARD_FRAMEBASE+0xb1>
 224:	600214        	ae_lq32f.i	aeq0, a2, 4
 227:	f8807d810f 	{ ae_lq56.i	aeq0, a1, 0; ae_mulfq32sp24s.l	aeq1, aeq0, aep0 }
 22c:	f4c6fd810f 	{ ae_lq56.i	aeq0, a1, 0; ae_mulafq32sp24s.l	aeq1, aeq0, aep1 }
 231:	f4c6fd812f 	{ ae_lq56.i	aeq0, a1, 16; ae_mulafq32sp24s.l	aeq1, aeq0, aep1 }
 236:	f4cafd813f 	{ ae_lq56.i	aeq0, a1, 24; ae_mulafq32sp24s.l	aeq1, aeq0, aep2 }
 23b:	f4cefd813f 	{ ae_lq56.i	aeq0, a1, 24; ae_mulafq32sp24s.l	aeq1, aeq0, aep3 }
 240:	f4cefd811f 	{ ae_lq56.i	aeq0, a1, 8; ae_mulafq32sp24s.l	aeq1, aeq0, aep3 }
 245:	f4d2fd031f 	{ ae_lq32f.i	aeq0, a3, 4; ae_mulafq32sp24s.l	aeq1, aeq0, aep4 }
 24a:	630124        	ae_sq56s.i	aeq0, a1, 16
 24d:	fd2c7d812f 	{ ae_lq56.i	aeq0, a1, 16; ae_roundsq32asym	aeq1, aeq1 }
 252:	631114        	ae_sq56s.i	aeq1, a1, 8
 255:	614114        	ae_lq56.i	aeq1, a1, 8
 258:	621414        	ae_sq32f.i	aeq1, a4, 4
 25b:	605224        	ae_lq32f.iu	aeq1, a2, 8
 25e:	f9813da12f 	{ ae_lq56.i	aeq1, a1, 16; ae_mulfq32sp24s.l	aeq2, aeq1, aep0 }
 263:	f5c6bd810f 	{ ae_lq56.i	aeq0, a1, 0; ae_mulafq32sp24s.l	aeq2, aeq0, aep1 }
 268:	f5c7bda11f 	{ ae_lq56.i	aeq1, a1, 8; ae_mulafq32sp24s.l	aeq2, aeq1, aep1 }
 26d:	f5cabd811f 	{ ae_lq56.i	aeq0, a1, 8; ae_mulafq32sp24s.l	aeq2, aeq0, aep2 }
 272:	f5cfbda13f 	{ ae_lq56.i	aeq1, a1, 24; ae_mulafq32sp24s.l	aeq2, aeq1, aep3 }
 277:	f5cebd132f 	{ ae_lq32f.iu	aeq0, a3, 8; ae_mulafq32sp24s.l	aeq2, aeq0, aep3 }
 27c:	f5d3ba010f 	{ ae_sq56s.i	aeq0, a1, 0; ae_mulafq32sp24s.l	aeq2, aeq1, aep4 }
 281:	062934        	ae_roundsq32asym	aeq0, aeq2
 284:	630134        	ae_sq56s.i	aeq0, a1, 24
 287:	610134        	ae_lq56.i	aeq0, a1, 24
 28a:	624424        	ae_sq32f.iu	aeq0, a4, 8

0000028d <BIQUARD_FRAMEBASE+0xb1>:
 28d:	61c104        	ae_lq56.i	aeq3, a1, 0
 290:	618124        	ae_lq56.i	aeq2, a1, 16
 293:	614134        	ae_lq56.i	aeq1, a1, 24
 296:	610114        	ae_lq56.i	aeq0, a1, 8
 299:	368804        	ae_trunca32q48	a8, aeq0
 29c:	369904        	ae_trunca32q48	a9, aeq1
 29f:	36aa04        	ae_trunca32q48	a10, aeq2
 2a2:	36bb04        	ae_trunca32q48	a11, aeq3
 2a5:	07b9           	s32i.n	a11, a7, 0
 2a7:	17a9           	s32i.n	a10, a7, 4
 2a9:	2799           	s32i.n	a9, a7, 8
 2ab:	3789           	s32i.n	a8, a7, 12
 2ad:	f01d           	retw.n

000002af <BIQUARD_FRAMEBASE+0xd3>:
	...

000002b0 <BIQUARD_FRAMEBASE+0xd4>:
 2b0:	368804        	ae_trunca32q48	a8, aeq0
 2b3:	369904        	ae_trunca32q48	a9, aeq1
 2b6:	36aa04        	ae_trunca32q48	a10, aeq2
 2b9:	36bb04        	ae_trunca32q48	a11, aeq3
 2bc:	07b9           	s32i.n	a11, a7, 0
 2be:	17a9           	s32i.n	a10, a7, 4
 2c0:	2799           	s32i.n	a9, a7, 8
 2c2:	3789           	s32i.n	a8, a7, 12
 2c4:	f01d           	retw.n

000002c6 <BIQUARD_FRAMEBASE+0xea>:
	...

000002c8 <PEQ_IIR_L>:
 2c8:	00a136        	entry	a1, 80
 2cb:	028d           	mov.n	a8, a2
 2cd:	07a376        	loopgtz	a3, 2d8 <PEQ_IIR_L+0x10>
 2d0:	0898           	l32i.n	a9, a8, 0
 2d2:	219290        	srai	a9, a9, 2
 2d5:	09e894        	ae_s32ip	a9, a8, 4

000002d8 <PEQ_IIR_L+0x10>:
 2d8:	248d           	ae_l16si.n	a8, a4, 0
 2da:	046d           	mov.n	a6, a4
 2dc:	070c           	movi.n	a7, 0
 2de:	5718a6        	blti	a8, 1, 339 <PEQ_IIR_L+0x71>

000002e1 <PEQ_IIR_L+0x19>:
 2e1:	1698           	l32i.n	a9, a6, 4
 2e3:	0199           	s32i.n	a9, a1, 0
 2e5:	102682        	l32i	a8, a6, 64
 2e8:	1189           	s32i.n	a8, a1, 4
 2ea:	ff4b81        	l32r	a8, 18 (1dc <BIQUARD_FRAMEBASE>)
 2ed:	1f26f2        	l32i	a15, a6, 124
 2f0:	21f9           	s32i.n	a15, a1, 8
 2f2:	2e26e2        	l32i	a14, a6, 184
 2f5:	31e9           	s32i.n	a14, a1, 12
 2f7:	3d26d2        	l32i	a13, a6, 244
 2fa:	41d9           	s32i.n	a13, a1, 16
 2fc:	4d26c2        	l32i	a12, a6, 0x134
 2ff:	03dd           	mov.n	a13, a3
 301:	81c9           	s32i.n	a12, a1, 32
 303:	20c1c2        	addi	a12, a1, 32
 306:	5c26b2        	l32i	a11, a6, 0x170
 309:	91b9           	s32i.n	a11, a1, 36
 30b:	6b26a2        	l32i	a10, a6, 0x1ac
 30e:	01bd           	mov.n	a11, a1
 310:	a1a9           	s32i.n	a10, a1, 40
 312:	02ad           	mov.n	a10, a2
 314:	7a2692        	l32i	a9, a6, 0x1e8
 317:	b199           	s32i.n	a9, a1, 44
 319:	0008e0        	callx8	a8
 31c:	81e8           	l32i.n	a14, a1, 32
 31e:	4d66e2        	s32i	a14, a6, 0x134
 321:	91d8           	l32i.n	a13, a1, 36
 323:	5c66d2        	s32i	a13, a6, 0x170
 326:	a1c8           	l32i.n	a12, a1, 40
 328:	6b66c2        	s32i	a12, a6, 0x1ac
 32b:	b1b8           	l32i.n	a11, a1, 44
 32d:	7a66b2        	s32i	a11, a6, 0x1e8
 330:	24ad           	ae_l16si.n	a10, a4, 0
 332:	771b           	addi.n	a7, a7, 1
 334:	664b           	addi.n	a6, a6, 4
 336:	a727a7        	blt	a7, a10, 2e1 <PEQ_IIR_L+0x19>

00000339 <PEQ_IIR_L+0x71>:
 339:	05dd           	mov.n	a13, a5
 33b:	03cd           	mov.n	a12, a3
 33d:	02ad           	mov.n	a10, a2
 33f:	ff3781        	l32r	a8, 1c (110 <APPLY_PEQ_GAIN>)
 342:	01d4b2        	addmi	a11, a4, 0x100
 345:	30cbb2        	addi	a11, a11, 48
 348:	0008e0        	callx8	a8
 34b:	f01d           	retw.n

0000034d <PEQ_IIR_L+0x85>:
 34d:	000000                                        ...

00000350 <PEQ_IIR_R>:
 350:	00a136        	entry	a1, 80
 353:	028d           	mov.n	a8, a2
 355:	07a376        	loopgtz	a3, 360 <PEQ_IIR_R+0x10>
 358:	0898           	l32i.n	a9, a8, 0
 35a:	219290        	srai	a9, a9, 2
 35d:	09e894        	ae_s32ip	a9, a8, 4

00000360 <PEQ_IIR_R+0x10>:
 360:	248d           	ae_l16si.n	a8, a4, 0
 362:	046d           	mov.n	a6, a4
 364:	070c           	movi.n	a7, 0
 366:	5718a6        	blti	a8, 1, 3c1 <PEQ_IIR_R+0x71>

00000369 <PEQ_IIR_R+0x19>:
 369:	1698           	l32i.n	a9, a6, 4
 36b:	0199           	s32i.n	a9, a1, 0
 36d:	102682        	l32i	a8, a6, 64
 370:	1189           	s32i.n	a8, a1, 4
 372:	ff2981        	l32r	a8, 18 (1dc <BIQUARD_FRAMEBASE>)
 375:	1f26f2        	l32i	a15, a6, 124
 378:	21f9           	s32i.n	a15, a1, 8
 37a:	2e26e2        	l32i	a14, a6, 184
 37d:	31e9           	s32i.n	a14, a1, 12
 37f:	3d26d2        	l32i	a13, a6, 244
 382:	41d9           	s32i.n	a13, a1, 16
 384:	8926c2        	l32i	a12, a6, 0x224
 387:	03dd           	mov.n	a13, a3
 389:	81c9           	s32i.n	a12, a1, 32
 38b:	20c1c2        	addi	a12, a1, 32
 38e:	9826b2        	l32i	a11, a6, 0x260
 391:	91b9           	s32i.n	a11, a1, 36
 393:	a726a2        	l32i	a10, a6, 0x29c
 396:	01bd           	mov.n	a11, a1
 398:	a1a9           	s32i.n	a10, a1, 40
 39a:	02ad           	mov.n	a10, a2
 39c:	b62692        	l32i	a9, a6, 0x2d8
 39f:	b199           	s32i.n	a9, a1, 44
 3a1:	0008e0        	callx8	a8
 3a4:	81e8           	l32i.n	a14, a1, 32
 3a6:	8966e2        	s32i	a14, a6, 0x224
 3a9:	91d8           	l32i.n	a13, a1, 36
 3ab:	9866d2        	s32i	a13, a6, 0x260
 3ae:	a1c8           	l32i.n	a12, a1, 40
 3b0:	a766c2        	s32i	a12, a6, 0x29c
 3b3:	b1b8           	l32i.n	a11, a1, 44
 3b5:	b666b2        	s32i	a11, a6, 0x2d8
 3b8:	24ad           	ae_l16si.n	a10, a4, 0
 3ba:	771b           	addi.n	a7, a7, 1
 3bc:	664b           	addi.n	a6, a6, 4
 3be:	a727a7        	blt	a7, a10, 369 <PEQ_IIR_R+0x19>

000003c1 <PEQ_IIR_R+0x71>:
 3c1:	05dd           	mov.n	a13, a5
 3c3:	03cd           	mov.n	a12, a3
 3c5:	02ad           	mov.n	a10, a2
 3c7:	ff1581        	l32r	a8, 1c (110 <APPLY_PEQ_GAIN>)
 3ca:	01d4b2        	addmi	a11, a4, 0x100
 3cd:	30cbb2        	addi	a11, a11, 48
 3d0:	0008e0        	callx8	a8
 3d3:	f01d           	retw.n

000003d5 <PEQ_IIR_R+0x85>:
 3d5:	000000                                        ...

000003d8 <PEQ2_PROC>:
 3d8:	004136        	entry	a1, 32
 3db:	c28c           	beqz.n	a2, 3eb <PEQ2_PROC+0x13>
 3dd:	02ad           	mov.n	a10, a2
 3df:	04bd           	mov.n	a11, a4
 3e1:	ff0f81        	l32r	a8, 20 (2c8 <PEQ_IIR_L>)
 3e4:	05cd           	mov.n	a12, a5
 3e6:	06dd           	mov.n	a13, a6
 3e8:	0008e0        	callx8	a8

000003eb <PEQ2_PROC+0x13>:
 3eb:	c38c           	beqz.n	a3, 3fb <PEQ2_PROC+0x23>
 3ed:	06dd           	mov.n	a13, a6
 3ef:	05cd           	mov.n	a12, a5
 3f1:	ff0c81        	l32r	a8, 24 (350 <PEQ_IIR_R>)
 3f4:	04bd           	mov.n	a11, a4
 3f6:	03ad           	mov.n	a10, a3
 3f8:	0008e0        	callx8	a8

000003fb <PEQ2_PROC+0x23>:
 3fb:	f01d           	retw.n

000003fd <PEQ2_PROC+0x25>:
 3fd:	000000                                        ...

00000400 <_fini>:
 400:	008136        	entry	a1, 64

00000403 <_fini+0x3>:
 403:	f01d           	retw.n
