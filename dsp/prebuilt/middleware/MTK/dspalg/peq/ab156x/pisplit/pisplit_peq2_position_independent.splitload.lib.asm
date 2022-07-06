
Build/lib/pisplit_peq2_position_independent.splitload.lib:     file format elf32-xtensa-le


Disassembly of section .text:

00000000 <_init-0x28>:
   0:	000005b8 00000580 00000058 00000590     ........X.......
  10:	00000368 000005b0 00000174 00000110     h.......t.......
  20:	00000260 000002e4                       `.......

00000028 <_init>:
  28:	008136        	entry	a1, 64
  2b:	f01d           	retw.n

0000002d <_init+0x5>:
  2d:	000000                                        ...

00000030 <_start>:
  30:	004136        	entry	a1, 32
  33:	fff4a1        	l32r	a10, 4 (580 <_DYNAMIC+0x1e4>)
  36:	fff231        	l32r	a3, 0 (5b8 <__bss_start>)
  39:	002282        	l32i	a8, a2, 0
  3c:	fff3b1        	l32r	a11, 8 (58 <PEQ2_INIT>)
  3f:	006382        	s32i	a8, a3, 0
  42:	0008e0        	callx8	a8
  45:	0388           	l32i.n	a8, a3, 0
  47:	fff1a1        	l32r	a10, c (590 <_DYNAMIC+0x1f4>)
  4a:	fff1b1        	l32r	a11, 10 (368 <PEQ2_PROC>)
  4d:	0008e0        	callx8	a8
  50:	fff121        	l32r	a2, 14 (5b0 <export_parameter_array>)
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
 113:	4c0304        	ae_lp24.i	aep0, a3, 0
 116:	fcc222        	addi	a2, a2, -4
 119:	5414a6        	blti	a4, 1, 171 <APPLY_PEQ_GAIN+0x61>
 11c:	03a032        	movi	a3, 3
 11f:	206440        	or	a6, a4, a4
 122:	145040        	extui	a5, a4, 0, 2
 125:	0ea576        	loopgtz	a5, 137 <APPLY_PEQ_GAIN+0x27>
 128:	609214        	ae_lq32f.iu	aeq2, a2, 4
 12b:	0885a4        	ae_mulfq32sp24s.l	aeq2, aeq2, aep0
 12e:	346334        	ae_sllasq56s	aeq1, aeq2, a3
 131:	061934        	ae_roundsq32asym	aeq0, aeq1
 134:	620204        	ae_sq32f.i	aeq0, a2, 0

00000137 <APPLY_PEQ_GAIN+0x27>:
 137:	214260        	srai	a4, a6, 2
 13a:	33a476        	loopgtz	a4, 171 <APPLY_PEQ_GAIN+0x61>
 13d:	600214        	ae_lq32f.i	aeq0, a2, 4

00000140 <APPLY_PEQ_GAIN+0x30>:
 140:	f8807d022f 	{ ae_lq32f.i	aeq0, a2, 8; ae_mulfq32sp24s.l	aeq1, aeq0, aep0 }
 145:	f9803d023f 	{ ae_lq32f.i	aeq0, a2, 12; ae_mulfq32sp24s.l	aeq2, aeq0, aep0 }
 14a:	f9807d124f 	{ ae_lq32f.iu	aeq0, a2, 16; ae_mulfq32sp24s.l	aeq3, aeq0, aep0 }
 14f:	f8807e931f 	{ ae_sllasq56s	aeq0, aeq1, a3; ae_mulfq32sp24s.l	aeq1, aeq0, aep0 }
 154:	fe247ea31f 	{ ae_sllasq56s	aeq0, aeq2, a3; ae_roundsq32asym	aeq2, aeq0 }
 159:	ff247eb31f 	{ ae_sllasq56s	aeq0, aeq3, a3; ae_roundsq32asym	aeq3, aeq0 }
 15e:	fd247e931f 	{ ae_sllasq56s	aeq0, aeq1, a3; ae_roundsq32asym	aeq1, aeq0 }
 163:	fc2479a2df 	{ ae_sq32f.i	aeq2, a2, -12; ae_roundsq32asym	aeq0, aeq0 }
 168:	6232e4        	ae_sq32f.i	aeq3, a2, -8
 16b:	6212f4        	ae_sq32f.i	aeq1, a2, -4
 16e:	620204        	ae_sq32f.i	aeq0, a2, 0

00000171 <APPLY_PEQ_GAIN+0x61>:
 171:	f01d           	retw.n

00000173 <APPLY_PEQ_GAIN+0x63>:
	...

00000174 <BIQUARD_FRAMEBASE>:
 174:	008136        	entry	a1, 64
 177:	4c0304        	ae_lp24.i	aep0, a3, 0
 17a:	4c1314        	ae_lp24.i	aep1, a3, 4
 17d:	4c2324        	ae_lp24.i	aep2, a3, 8
 180:	4c3334        	ae_lp24.i	aep3, a3, 12
 183:	4c4344        	ae_lp24.i	aep4, a3, 16
 186:	047d           	mov.n	a7, a4
 188:	60c704        	ae_lq32f.i	aeq3, a7, 0
 18b:	608714        	ae_lq32f.i	aeq2, a7, 4
 18e:	604724        	ae_lq32f.i	aeq1, a7, 8
 191:	600734        	ae_lq32f.i	aeq0, a7, 12
 194:	451b           	addi.n	a4, a5, 1
 196:	b34550        	movgez	a4, a5, a5
 199:	214140        	srai	a4, a4, 1
 19c:	845d           	ae_sext16	a5, a4
 19e:	fcc242        	addi	a4, a2, -4
 1a1:	0215e6        	bgei	a5, 1, 1a7 <BIQUARD_FRAMEBASE+0x33>
 1a4:	002806        	j	248 <BIQUARD_FRAMEBASE+0xd4>
 1a7:	202440        	or	a2, a4, a4
 1aa:	203440        	or	a3, a4, a4
 1ad:	632124        	ae_sq56s.i	aeq2, a1, 16
 1b0:	633104        	ae_sq56s.i	aeq3, a1, 0
 1b3:	630114        	ae_sq56s.i	aeq0, a1, 8
 1b6:	631134        	ae_sq56s.i	aeq1, a1, 24
 1b9:	68a576        	loopgtz	a5, 225 <BIQUARD_FRAMEBASE+0xb1>
 1bc:	600214        	ae_lq32f.i	aeq0, a2, 4
 1bf:	f8807d810f 	{ ae_lq56.i	aeq0, a1, 0; ae_mulfq32sp24s.l	aeq1, aeq0, aep0 }
 1c4:	f4c6fd810f 	{ ae_lq56.i	aeq0, a1, 0; ae_mulafq32sp24s.l	aeq1, aeq0, aep1 }
 1c9:	f4c6fd812f 	{ ae_lq56.i	aeq0, a1, 16; ae_mulafq32sp24s.l	aeq1, aeq0, aep1 }
 1ce:	f4cafd813f 	{ ae_lq56.i	aeq0, a1, 24; ae_mulafq32sp24s.l	aeq1, aeq0, aep2 }
 1d3:	f4cefd813f 	{ ae_lq56.i	aeq0, a1, 24; ae_mulafq32sp24s.l	aeq1, aeq0, aep3 }
 1d8:	f4cefd811f 	{ ae_lq56.i	aeq0, a1, 8; ae_mulafq32sp24s.l	aeq1, aeq0, aep3 }
 1dd:	f4d2fd031f 	{ ae_lq32f.i	aeq0, a3, 4; ae_mulafq32sp24s.l	aeq1, aeq0, aep4 }
 1e2:	630124        	ae_sq56s.i	aeq0, a1, 16
 1e5:	fd2c7d812f 	{ ae_lq56.i	aeq0, a1, 16; ae_roundsq32asym	aeq1, aeq1 }
 1ea:	631114        	ae_sq56s.i	aeq1, a1, 8
 1ed:	614114        	ae_lq56.i	aeq1, a1, 8
 1f0:	621414        	ae_sq32f.i	aeq1, a4, 4
 1f3:	605224        	ae_lq32f.iu	aeq1, a2, 8
 1f6:	f9813da12f 	{ ae_lq56.i	aeq1, a1, 16; ae_mulfq32sp24s.l	aeq2, aeq1, aep0 }
 1fb:	f5c6bd810f 	{ ae_lq56.i	aeq0, a1, 0; ae_mulafq32sp24s.l	aeq2, aeq0, aep1 }
 200:	f5c7bda11f 	{ ae_lq56.i	aeq1, a1, 8; ae_mulafq32sp24s.l	aeq2, aeq1, aep1 }
 205:	f5cabd811f 	{ ae_lq56.i	aeq0, a1, 8; ae_mulafq32sp24s.l	aeq2, aeq0, aep2 }
 20a:	f5cfbda13f 	{ ae_lq56.i	aeq1, a1, 24; ae_mulafq32sp24s.l	aeq2, aeq1, aep3 }
 20f:	f5cebd132f 	{ ae_lq32f.iu	aeq0, a3, 8; ae_mulafq32sp24s.l	aeq2, aeq0, aep3 }
 214:	f5d3ba010f 	{ ae_sq56s.i	aeq0, a1, 0; ae_mulafq32sp24s.l	aeq2, aeq1, aep4 }
 219:	062934        	ae_roundsq32asym	aeq0, aeq2
 21c:	630134        	ae_sq56s.i	aeq0, a1, 24
 21f:	610134        	ae_lq56.i	aeq0, a1, 24
 222:	624424        	ae_sq32f.iu	aeq0, a4, 8

00000225 <BIQUARD_FRAMEBASE+0xb1>:
 225:	61c104        	ae_lq56.i	aeq3, a1, 0
 228:	618124        	ae_lq56.i	aeq2, a1, 16
 22b:	614134        	ae_lq56.i	aeq1, a1, 24
 22e:	610114        	ae_lq56.i	aeq0, a1, 8
 231:	368804        	ae_trunca32q48	a8, aeq0
 234:	369904        	ae_trunca32q48	a9, aeq1
 237:	36aa04        	ae_trunca32q48	a10, aeq2
 23a:	36bb04        	ae_trunca32q48	a11, aeq3
 23d:	07b9           	s32i.n	a11, a7, 0
 23f:	17a9           	s32i.n	a10, a7, 4
 241:	2799           	s32i.n	a9, a7, 8
 243:	3789           	s32i.n	a8, a7, 12
 245:	f01d           	retw.n

00000247 <BIQUARD_FRAMEBASE+0xd3>:
	...

00000248 <BIQUARD_FRAMEBASE+0xd4>:
 248:	368804        	ae_trunca32q48	a8, aeq0
 24b:	369904        	ae_trunca32q48	a9, aeq1
 24e:	36aa04        	ae_trunca32q48	a10, aeq2
 251:	36bb04        	ae_trunca32q48	a11, aeq3
 254:	07b9           	s32i.n	a11, a7, 0
 256:	17a9           	s32i.n	a10, a7, 4
 258:	2799           	s32i.n	a9, a7, 8
 25a:	3789           	s32i.n	a8, a7, 12
 25c:	f01d           	retw.n

0000025e <BIQUARD_FRAMEBASE+0xea>:
	...

00000260 <PEQ_IIR_L>:
 260:	00a136        	entry	a1, 80
 263:	028d           	mov.n	a8, a2
 265:	07a376        	loopgtz	a3, 270 <PEQ_IIR_L+0x10>
 268:	0898           	l32i.n	a9, a8, 0
 26a:	219290        	srai	a9, a9, 2
 26d:	09e894        	ae_s32ip	a9, a8, 4

00000270 <PEQ_IIR_L+0x10>:
 270:	248d           	ae_l16si.n	a8, a4, 0
 272:	045d           	mov.n	a5, a4
 274:	060c           	movi.n	a6, 0
 276:	5718a6        	blti	a8, 1, 2d1 <PEQ_IIR_L+0x71>

00000279 <PEQ_IIR_L+0x19>:
 279:	1598           	l32i.n	a9, a5, 4
 27b:	0199           	s32i.n	a9, a1, 0
 27d:	102582        	l32i	a8, a5, 64
 280:	1189           	s32i.n	a8, a1, 4
 282:	ff6581        	l32r	a8, 18 (174 <BIQUARD_FRAMEBASE>)
 285:	1f25f2        	l32i	a15, a5, 124
 288:	21f9           	s32i.n	a15, a1, 8
 28a:	2e25e2        	l32i	a14, a5, 184
 28d:	31e9           	s32i.n	a14, a1, 12
 28f:	3d25d2        	l32i	a13, a5, 244
 292:	41d9           	s32i.n	a13, a1, 16
 294:	4d25c2        	l32i	a12, a5, 0x134
 297:	03dd           	mov.n	a13, a3
 299:	81c9           	s32i.n	a12, a1, 32
 29b:	20c1c2        	addi	a12, a1, 32
 29e:	5c25b2        	l32i	a11, a5, 0x170
 2a1:	91b9           	s32i.n	a11, a1, 36
 2a3:	6b25a2        	l32i	a10, a5, 0x1ac
 2a6:	01bd           	mov.n	a11, a1
 2a8:	a1a9           	s32i.n	a10, a1, 40
 2aa:	02ad           	mov.n	a10, a2
 2ac:	7a2592        	l32i	a9, a5, 0x1e8
 2af:	b199           	s32i.n	a9, a1, 44
 2b1:	0008e0        	callx8	a8
 2b4:	81e8           	l32i.n	a14, a1, 32
 2b6:	4d65e2        	s32i	a14, a5, 0x134
 2b9:	91d8           	l32i.n	a13, a1, 36
 2bb:	5c65d2        	s32i	a13, a5, 0x170
 2be:	a1c8           	l32i.n	a12, a1, 40
 2c0:	6b65c2        	s32i	a12, a5, 0x1ac
 2c3:	b1b8           	l32i.n	a11, a1, 44
 2c5:	7a65b2        	s32i	a11, a5, 0x1e8
 2c8:	24ad           	ae_l16si.n	a10, a4, 0
 2ca:	661b           	addi.n	a6, a6, 1
 2cc:	554b           	addi.n	a5, a5, 4
 2ce:	a726a7        	blt	a6, a10, 279 <PEQ_IIR_L+0x19>

000002d1 <PEQ_IIR_L+0x71>:
 2d1:	03cd           	mov.n	a12, a3
 2d3:	02ad           	mov.n	a10, a2
 2d5:	ff5181        	l32r	a8, 1c (110 <APPLY_PEQ_GAIN>)
 2d8:	01d4b2        	addmi	a11, a4, 0x100
 2db:	30cbb2        	addi	a11, a11, 48
 2de:	0008e0        	callx8	a8
 2e1:	f01d           	retw.n

000002e3 <PEQ_IIR_L+0x83>:
	...

000002e4 <PEQ_IIR_R>:
 2e4:	00a136        	entry	a1, 80
 2e7:	028d           	mov.n	a8, a2
 2e9:	07a376        	loopgtz	a3, 2f4 <PEQ_IIR_R+0x10>
 2ec:	0898           	l32i.n	a9, a8, 0
 2ee:	219290        	srai	a9, a9, 2
 2f1:	09e894        	ae_s32ip	a9, a8, 4

000002f4 <PEQ_IIR_R+0x10>:
 2f4:	248d           	ae_l16si.n	a8, a4, 0
 2f6:	045d           	mov.n	a5, a4
 2f8:	060c           	movi.n	a6, 0
 2fa:	5718a6        	blti	a8, 1, 355 <PEQ_IIR_R+0x71>

000002fd <PEQ_IIR_R+0x19>:
 2fd:	1598           	l32i.n	a9, a5, 4
 2ff:	0199           	s32i.n	a9, a1, 0
 301:	102582        	l32i	a8, a5, 64
 304:	1189           	s32i.n	a8, a1, 4
 306:	ff4481        	l32r	a8, 18 (174 <BIQUARD_FRAMEBASE>)
 309:	1f25f2        	l32i	a15, a5, 124
 30c:	21f9           	s32i.n	a15, a1, 8
 30e:	2e25e2        	l32i	a14, a5, 184
 311:	31e9           	s32i.n	a14, a1, 12
 313:	3d25d2        	l32i	a13, a5, 244
 316:	41d9           	s32i.n	a13, a1, 16
 318:	8925c2        	l32i	a12, a5, 0x224
 31b:	03dd           	mov.n	a13, a3
 31d:	81c9           	s32i.n	a12, a1, 32
 31f:	20c1c2        	addi	a12, a1, 32
 322:	9825b2        	l32i	a11, a5, 0x260
 325:	91b9           	s32i.n	a11, a1, 36
 327:	a725a2        	l32i	a10, a5, 0x29c
 32a:	01bd           	mov.n	a11, a1
 32c:	a1a9           	s32i.n	a10, a1, 40
 32e:	02ad           	mov.n	a10, a2
 330:	b62592        	l32i	a9, a5, 0x2d8
 333:	b199           	s32i.n	a9, a1, 44
 335:	0008e0        	callx8	a8
 338:	81e8           	l32i.n	a14, a1, 32
 33a:	8965e2        	s32i	a14, a5, 0x224
 33d:	91d8           	l32i.n	a13, a1, 36
 33f:	9865d2        	s32i	a13, a5, 0x260
 342:	a1c8           	l32i.n	a12, a1, 40
 344:	a765c2        	s32i	a12, a5, 0x29c
 347:	b1b8           	l32i.n	a11, a1, 44
 349:	b665b2        	s32i	a11, a5, 0x2d8
 34c:	24ad           	ae_l16si.n	a10, a4, 0
 34e:	661b           	addi.n	a6, a6, 1
 350:	554b           	addi.n	a5, a5, 4
 352:	a726a7        	blt	a6, a10, 2fd <PEQ_IIR_R+0x19>

00000355 <PEQ_IIR_R+0x71>:
 355:	03cd           	mov.n	a12, a3
 357:	02ad           	mov.n	a10, a2
 359:	ff3081        	l32r	a8, 1c (110 <APPLY_PEQ_GAIN>)
 35c:	01d4b2        	addmi	a11, a4, 0x100
 35f:	30cbb2        	addi	a11, a11, 48
 362:	0008e0        	callx8	a8
 365:	f01d           	retw.n

00000367 <PEQ_IIR_R+0x83>:
	...

00000368 <PEQ2_PROC>:
 368:	004136        	entry	a1, 32
 36b:	b28c           	beqz.n	a2, 37a <PEQ2_PROC+0x12>
 36d:	02ad           	mov.n	a10, a2
 36f:	ff2c81        	l32r	a8, 20 (260 <PEQ_IIR_L>)
 372:	04bd           	mov.n	a11, a4
 374:	20c550        	or	a12, a5, a5
 377:	0008e0        	callx8	a8

0000037a <PEQ2_PROC+0x12>:
 37a:	a38c           	beqz.n	a3, 388 <PEQ2_PROC+0x20>
 37c:	05cd           	mov.n	a12, a5
 37e:	ff2981        	l32r	a8, 24 (2e4 <PEQ_IIR_R>)
 381:	04bd           	mov.n	a11, a4
 383:	03ad           	mov.n	a10, a3
 385:	0008e0        	callx8	a8

00000388 <PEQ2_PROC+0x20>:
 388:	f01d           	retw.n

0000038a <PEQ2_PROC+0x22>:
	...

0000038c <_fini>:
 38c:	008136        	entry	a1, 64

0000038f <_fini+0x3>:
 38f:	f01d           	retw.n
