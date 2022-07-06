
Build/lib/pisplit_msbc_enc_position_independent.splitload.lib:     file format elf32-xtensa-le


Disassembly of section .text:

00000000 <_init-0x5c>:
   0:	00000d10 000009b4 00000550 00000544     ........P...D...
  10:	0000043c 00000500 00000a20 00000a2c     <....... ...,...
  20:	000001f0 00000bec 00000b70 00000cf0     ........p.......
  30:	00000090 00008000 00800000 00000240     ............@...
  40:	00000580 00000801 00003801 000001d0     .........8......
  50:	000003d8 0000ffff 00000f00              ............

0000005c <_init>:
  5c:	008136        	entry	a1, 64
  5f:	f01d           	retw.n

00000061 <_init+0x5>:
  61:	00000000 36000000                                .......

00000068 <_start>:
  68:	004136        	entry	a1, 32
  6b:	ffe6a1        	l32r	a10, 4 (9b4 <_DYNAMIC+0x268>)
  6e:	ffe6b1        	l32r	a11, 8 (550 <SBC_Encoder_Init>)
  71:	ffe6c1        	l32r	a12, c (544 <mSBC_SW_SN_init>)
  74:	ffe7d1        	l32r	a13, 10 (43c <mSBC_Encoder_Process>)
  77:	ffe291        	l32r	a9, 0 (d10 <__bss_start>)
  7a:	0288           	l32i.n	a8, a2, 0
  7c:	ffe6e1        	l32r	a14, 14 (500 <mSBC_SW_SN>)
  7f:	0989           	s32i.n	a8, a9, 0
  81:	0008e0        	callx8	a8
  84:	ffe521        	l32r	a2, 18 (a20 <export_parameter_array>)
  87:	f01d           	retw.n

00000089 <_start+0x21>:
  89:	00000000 36000000                                .......

00000090 <SbcAnalysisFilter8>:
  90:	008136        	entry	a1, 64
  93:	026d           	mov.n	a6, a2
  95:	03d252        	addmi	a5, a2, 0x300
  98:	5b2582        	l32i	a8, a5, 0x16c
  9b:	c89542        	l16si	a4, a5, 0x190
  9e:	602572        	l32i	a7, a5, 0x180
  a1:	888d           	ae_sext16	a8, a8
  a3:	80c552        	addi	a5, a5, -128
  a6:	0159           	s32i.n	a5, a1, 0
  a8:	3189           	s32i.n	a8, a1, 12
  aa:	0218e6        	bgei	a8, 1, b0 <SbcAnalysisFilter8+0x20>
  ad:	0045c6        	j	1c8 <SbcAnalysisFilter8+0x138>
  b0:	90a082        	movi	a8, 144
  b3:	ffdad1        	l32r	a13, 1c (a2c <export_parameter_array+0xc>)
  b6:	02d2e2        	addmi	a14, a2, 0x200
  b9:	0c0c           	movi.n	a12, 0
  bb:	41c9           	s32i.n	a12, a1, 16
  bd:	c0a022        	movi	a2, 192
  c0:	21d9           	s32i.n	a13, a1, 8
  c2:	c08840        	sub	a8, a8, a4
  c5:	fccee2        	addi	a14, a14, -4
  c8:	11e9           	s32i.n	a14, a1, 4
  ca:	888d           	ae_sext16	a8, a8
  cc:	90d860        	addx2	a13, a8, a6
  cf:	001486        	j	125 <SbcAnalysisFilter8+0x95>

000000d2 <SbcAnalysisFilter8+0x42>:
  d2:	990c           	movi.n	a9, 9
  d4:	11a8           	l32i.n	a10, a1, 4
  d6:	218140        	srai	a8, a4, 1
  d9:	884b           	addi.n	a8, a8, 4
  db:	888d           	ae_sext16	a8, a8
  dd:	1188e0        	slli	a8, a8, 2
  e0:	c08a80        	sub	a8, a10, a8
  e3:	218976        	loop	a9, 108 <SbcAnalysisFilter8+0x78>
  e6:	0898           	l32i.n	a9, a8, 0
  e8:	ffdab2        	addmi	a11, a10, 0xffffff00
  eb:	ffd8c2        	addmi	a12, a8, 0xffffff00
  ee:	0a99           	s32i.n	a9, a10, 0
  f0:	f0caa2        	addi	a10, a10, -16
  f3:	f0c882        	addi	a8, a8, -16
  f6:	3f2c92        	l32i	a9, a12, 252
  f9:	3f6b92        	s32i	a9, a11, 252
  fc:	3e2c92        	l32i	a9, a12, 248
  ff:	3e6b92        	s32i	a9, a11, 248
 102:	3d2c92        	l32i	a9, a12, 244
 105:	3d6b92        	s32i	a9, a11, 244

00000108 <SbcAnalysisFilter8+0x78>:
 108:	040c           	movi.n	a4, 0

0000010a <SbcAnalysisFilter8+0x7a>:
 10a:	3198           	l32i.n	a9, a1, 12
 10c:	41a8           	l32i.n	a10, a1, 16
 10e:	90a082        	movi	a8, 144
 111:	c08840        	sub	a8, a8, a4
 114:	888d           	ae_sext16	a8, a8
 116:	90d860        	addx2	a13, a8, a6
 119:	ba1b           	addi.n	a11, a10, 1
 11b:	8bad           	ae_sext16	a10, a11
 11d:	41a9           	s32i.n	a10, a1, 16
 11f:	c099a0        	sub	a9, a9, a10
 122:	0a2916        	beqz	a9, 1c8 <SbcAnalysisFilter8+0x138>

00000125 <SbcAnalysisFilter8+0x95>:
 125:	27cd           	ae_l16si.n	a12, a7, 0
 127:	275dc2        	s16i	a12, a13, 78
 12a:	37bd           	ae_l16si.n	a11, a7, 2
 12c:	265db2        	s16i	a11, a13, 76
 12f:	0297a2        	l16si	a10, a7, 4
 132:	21b8           	l32i.n	a11, a1, 8
 134:	255da2        	s16i	a10, a13, 74
 137:	039792        	l16si	a9, a7, 6
 13a:	245d92        	s16i	a9, a13, 72
 13d:	091c           	movi.n	a9, 16
 13f:	049732        	l16si	a3, a7, 8
 142:	235d32        	s16i	a3, a13, 70
 145:	0597f2        	l16si	a15, a7, 10
 148:	225df2        	s16i	a15, a13, 68
 14b:	0697e2        	l16si	a14, a7, 12
 14e:	a0a0f2        	movi	a15, 160
 151:	215de2        	s16i	a14, a13, 66
 154:	80a0e2        	movi	a14, 128
 157:	0797c2        	l16si	a12, a7, 14
 15a:	205dc2        	s16i	a12, a13, 64
 15d:	10c772        	addi	a7, a7, 16
 160:	0c0c           	movi.n	a12, 0
 162:	448976        	loop	a9, 1aa <SbcAnalysisFilter8+0x11a>
 165:	909860        	addx2	a9, a8, a6
 168:	40c9a2        	addi	a10, a9, 64
 16b:	400a04        	ae_lp16f.i	aep0, a10, 0
 16e:	600b14        	ae_lq32f.i	aeq0, a11, 4
 171:	f8e00609ae 	{ addi	a10, a9, 96; ae_mulrfq32sp24s.l	aeq0, aeq0, aep0 }
 176:	400a04        	ae_lp16f.i	aep0, a10, 0
 179:	604b24        	ae_lq32f.i	aeq1, a11, 8
 17c:	d4e19989ef 	{ ae_lp16f.x	aep0, a9, a14; ae_mularfq32sp24s.l	aeq0, aeq1, aep0 }
 181:	604b34        	ae_lq32f.i	aeq1, a11, 12
 184:	d4e19989ff 	{ ae_lp16f.x	aep0, a9, a15; ae_mularfq32sp24s.l	aeq0, aeq1, aep0 }
 189:	604b44        	ae_lq32f.i	aeq1, a11, 16
 18c:	d4e199892f 	{ ae_lp16f.x	aep0, a9, a2; ae_mularfq32sp24s.l	aeq0, aeq1, aep0 }
 191:	605b54        	ae_lq32f.iu	aeq1, a11, 20
 194:	d4e1801c9e 	{ addi	a9, a12, 1; ae_mularfq32sp24s.l	aeq0, aeq1, aep0 }
 199:	a0ac60        	addx4	a10, a12, a6
 19c:	350074        	ae_sraiq56	aeq0, aeq0, 1
 19f:	368c04        	ae_trunca32q48	a12, aeq0
 1a2:	0ac9           	s32i.n	a12, a10, 0
 1a4:	89cd           	ae_sext16	a12, a9
 1a6:	981b           	addi.n	a9, a8, 1
 1a8:	898d           	ae_sext16	a8, a9

000001aa <SbcAnalysisFilter8+0x11a>:
 1aa:	ff9d81        	l32r	a8, 20 (1f0 <SBC_FastIDCT8>)
 1ad:	06ad           	mov.n	a10, a6
 1af:	05bd           	mov.n	a11, a5
 1b1:	0008e0        	callx8	a8
 1b4:	20c552        	addi	a5, a5, 32
 1b7:	8fa092        	movi	a9, 143
 1ba:	02a947        	bge	a9, a4, 1c0 <SbcAnalysisFilter8+0x130>
 1bd:	ffc446        	j	d2 <SbcAnalysisFilter8+0x42>
 1c0:	448b           	addi.n	a4, a4, 8
 1c2:	844d           	ae_sext16	a4, a4
 1c4:	ffd086        	j	10a <SbcAnalysisFilter8+0x7a>

000001c7 <SbcAnalysisFilter8+0x137>:
	...

000001c8 <SbcAnalysisFilter8+0x138>:
 1c8:	0188           	l32i.n	a8, a1, 0
 1ca:	846842        	s32i	a4, a8, 0x210
 1cd:	f01d           	retw.n

000001cf <SbcAnalysisFilter8+0x13f>:
	...

000001d0 <SbcAnalysisInit>:
 1d0:	004136        	entry	a1, 32
 1d3:	030c           	movi.n	a3, 0
 1d5:	206220        	or	a6, a2, a2
 1d8:	70a042        	movi	a4, 112
 1db:	40c222        	addi	a2, a2, 64
 1de:	04d682        	addmi	a8, a6, 0x400
 1e1:	028476        	loop	a4, 1e7 <SbcAnalysisInit+0x17>
 1e4:	09e234        	ae_s32ip	a3, a2, 4

000001e7 <SbcAnalysisInit+0x17>:
 1e7:	246832        	s32i	a3, a8, 144
 1ea:	f01d           	retw.n

000001ec <SbcAnalysisInit+0x1c>:
 1ec:	00000000                                ....

000001f0 <SBC_FastIDCT8>:
 1f0:	004136        	entry	a1, 32
 1f3:	06a042        	movi	a4, 6
 1f6:	206330        	or	a6, a3, a3
 1f9:	20c372        	addi	a7, a3, 32
 1fc:	fcc282        	addi	a8, a2, -4
 1ff:	ff8921        	l32r	a2, 24 (bec <sbc_enc_as16Offset8+0x7c>)
 202:	203880        	or	a3, a8, a8

00000205 <SBC_FastIDCT8+0x15>:
 205:	605314        	ae_lq32f.iu	aeq1, a3, 4
 208:	449214        	ae_lp16x2f.iu	aep1, a2, 4
 20b:	609314        	ae_lq32f.iu	aeq2, a3, 4
 20e:	f8256d821f 	{ ae_lp16x2f.iu	aep0, a2, 4; ae_mulq32sp16s.h	aeq1, aeq1, aep1 }
 213:	f6e6fd031f 	{ ae_lq32f.i	aeq0, a3, 4; ae_mulaq32sp16s.l	aeq1, aeq2, aep1 }
 218:	f462fd132f 	{ ae_lq32f.iu	aeq0, a3, 8; ae_mulaq32sp16s.h	aeq1, aeq0, aep0 }
 21d:	0c8476        	loop	a4, 22d <SBC_FastIDCT8+0x3d>
 220:	f4e2ed821f 	{ ae_lp16x2f.iu	aep0, a2, 4; ae_mulaq32sp16s.l	aeq1, aeq0, aep0 }
 225:	600314        	ae_lq32f.i	aeq0, a3, 4
 228:	f462fd132f 	{ ae_lq32f.iu	aeq0, a3, 8; ae_mulaq32sp16s.h	aeq1, aeq0, aep0 }

0000022d <SBC_FastIDCT8+0x3d>:
 22d:	f4e2c0064f 	{ movi	a4, 6; ae_mulaq32sp16s.l	aeq1, aeq0, aep0 }
 232:	083d           	mov.n	a3, a8
 234:	369904        	ae_trunca32q48	a9, aeq1
 237:	09e694        	ae_s32ip	a9, a6, 4
 23a:	c79677        	bne	a6, a7, 205 <SBC_FastIDCT8+0x15>
 23d:	f01d           	retw.n

0000023f <SBC_FastIDCT8+0x4f>:
	...

00000240 <sbc_enc_bit_alloc_mono>:
 240:	004136        	entry	a1, 32
 243:	02ed           	mov.n	a14, a2
 245:	02d222        	addmi	a2, a2, 0x200
 248:	02dd           	mov.n	a13, a2
 24a:	9c2232        	l32i	a3, a2, 0x270
 24d:	6cc2f2        	addi	a15, a2, 108
 250:	9a22c2        	l32i	a12, a2, 0x268
 253:	330b           	addi.n	a3, a3, -1
 255:	16f316        	beqz	a3, 3c8 <sbc_enc_bit_alloc_mono+0x188>
 258:	982282        	l32i	a8, a2, 0x260
 25b:	ff7351        	l32r	a5, 28 (b70 <sbc_enc_as16Offset8>)
 25e:	1188b0        	slli	a8, a8, 5
 261:	558a           	add.n	a5, a5, a8
 263:	021ce6        	bgei	a12, 1, 269 <sbc_enc_bit_alloc_mono+0x29>
 266:	0059c6        	j	3d1 <sbc_enc_bit_alloc_mono+0x191>
 269:	0e4d           	mov.n	a4, a14
 26b:	b97c           	movi.n	a9, -5
 26d:	1cac76        	loopgtz	a12, 28d <sbc_enc_bit_alloc_mono+0x4d>
 270:	902462        	l32i	a6, a4, 0x240
 273:	030c           	movi.n	a3, 0
 275:	444b           	addi.n	a4, a4, 4
 277:	869c           	beqz.n	a6, 293 <sbc_enc_bit_alloc_mono+0x53>
 279:	05a8           	l32i.n	a10, a5, 0
 27b:	c0a6a0        	sub	a10, a6, a10
 27e:	7233a0        	salt	a3, a3, a10
 281:	21b1a0        	srai	a11, a10, 1
 284:	93ab30        	movnez	a10, a11, a3
 287:	02a9           	s32i.n	a10, a2, 0

00000289 <sbc_enc_bit_alloc_mono+0x49>:
 289:	224b           	addi.n	a2, a2, 4
 28b:	554b           	addi.n	a5, a5, 4

0000028d <sbc_enc_bit_alloc_mono+0x4d>:
 28d:	071ce6        	bgei	a12, 1, 298 <sbc_enc_bit_alloc_mono+0x58>
 290:	004f46        	j	3d1 <sbc_enc_bit_alloc_mono+0x191>

00000293 <sbc_enc_bit_alloc_mono+0x53>:
 293:	0299           	s32i.n	a9, a2, 0
 295:	fffc06        	j	289 <sbc_enc_bit_alloc_mono+0x49>

00000298 <sbc_enc_bit_alloc_mono+0x58>:
 298:	0d2d           	mov.n	a2, a13
 29a:	040c           	movi.n	a4, 0
 29c:	24b0c0        	extui	a11, a12, 0, 3
 29f:	06ab76        	loopgtz	a11, 2a9 <sbc_enc_bit_alloc_mono+0x69>
 2a2:	0238           	l32i.n	a3, a2, 0
 2a4:	224b           	addi.n	a2, a2, 4
 2a6:	534340        	max	a4, a3, a4

000002a9 <sbc_enc_bit_alloc_mono+0x69>:
 2a9:	2133c0        	srai	a3, a12, 3
 2ac:	f03d           	nop.n
 2ae:	2aa376        	loopgtz	a3, 2dc <sbc_enc_bit_alloc_mono+0x9c>
 2b1:	0238           	l32i.n	a3, a2, 0
 2b3:	1258           	l32i.n	a5, a2, 4
 2b5:	2268           	l32i.n	a6, a2, 8
 2b7:	3278           	l32i.n	a7, a2, 12
 2b9:	4288           	l32i.n	a8, a2, 16
 2bb:	5298           	l32i.n	a9, a2, 20
 2bd:	62a8           	l32i.n	a10, a2, 24
 2bf:	72b8           	l32i.n	a11, a2, 28
 2c1:	533340        	max	a3, a3, a4
 2c4:	20c222        	addi	a2, a2, 32
 2c7:	533530        	max	a3, a5, a3
 2ca:	533630        	max	a3, a6, a3
 2cd:	533730        	max	a3, a7, a3
 2d0:	533830        	max	a3, a8, a3
 2d3:	533930        	max	a3, a9, a3
 2d6:	533a30        	max	a3, a10, a3
 2d9:	534b30        	max	a4, a11, a3

000002dc <sbc_enc_bit_alloc_mono+0x9c>:
 2dc:	822f92        	l32i	a9, a15, 0x208
 2df:	641b           	addi.n	a6, a4, 1
 2e1:	00a052        	movi	a5, 0

000002e4 <sbc_enc_bit_alloc_mono+0xa4>:
 2e4:	c09950        	sub	a9, a9, a5
 2e7:	660b           	addi.n	a6, a6, -1
 2e9:	0d2d           	mov.n	a2, a13
 2eb:	021ce6        	bgei	a12, 1, 2f1 <sbc_enc_bit_alloc_mono+0xb1>
 2ee:	003046        	j	3b3 <sbc_enc_bit_alloc_mono+0x173>
 2f1:	050c           	movi.n	a5, 0
 2f3:	13ac76        	loopgtz	a12, 30a <sbc_enc_bit_alloc_mono+0xca>
 2f6:	0248           	l32i.n	a4, a2, 0
 2f8:	224b           	addi.n	a2, a2, 4
 2fa:	c04460        	sub	a4, a4, a6
 2fd:	07b4e6        	bgei	a4, 16, 308 <sbc_enc_bit_alloc_mono+0xc8>
 300:	0414a6        	blti	a4, 1, 308 <sbc_enc_bit_alloc_mono+0xc8>
 303:	0e1426        	beqi	a4, 1, 315 <sbc_enc_bit_alloc_mono+0xd5>
 306:	551b           	addi.n	a5, a5, 1

00000308 <sbc_enc_bit_alloc_mono+0xc8>:
 308:	f03d           	nop.n

0000030a <sbc_enc_bit_alloc_mono+0xca>:
 30a:	c08950        	sub	a8, a9, a5
 30d:	d318e6        	bgei	a8, 1, 2e4 <sbc_enc_bit_alloc_mono+0xa4>
 310:	79cc           	bnez.n	a9, 31b <sbc_enc_bit_alloc_mono+0xdb>
 312:	002a86        	j	3c0 <sbc_enc_bit_alloc_mono+0x180>

00000315 <sbc_enc_bit_alloc_mono+0xd5>:
 315:	552b           	addi.n	a5, a5, 2
 317:	fffb46        	j	308 <sbc_enc_bit_alloc_mono+0xc8>

0000031a <sbc_enc_bit_alloc_mono+0xda>:
	...

0000031b <sbc_enc_bit_alloc_mono+0xdb>:
 31b:	0d2d           	mov.n	a2, a13
 31d:	862b           	addi.n	a8, a6, 2
 31f:	0b0c           	movi.n	a11, 0
 321:	02dea2        	addmi	a10, a14, 0x200
 324:	60caa2        	addi	a10, a10, 96
 327:	0a7d           	mov.n	a7, a10
 329:	1dac76        	loopgtz	a12, 34a <sbc_enc_bit_alloc_mono+0x10a>
 32c:	0248           	l32i.n	a4, a2, 0
 32e:	224b           	addi.n	a2, a2, 4
 330:	04a487        	bge	a4, a8, 338 <sbc_enc_bit_alloc_mono+0xf8>
 333:	07b9           	s32i.n	a11, a7, 0
 335:	0003c6        	j	348 <sbc_enc_bit_alloc_mono+0x108>

00000338 <sbc_enc_bit_alloc_mono+0xf8>:
 338:	c04460        	sub	a4, a4, a6
 33b:	05b4e6        	bgei	a4, 16, 344 <sbc_enc_bit_alloc_mono+0x104>
 33e:	844d           	ae_sext16	a4, a4
 340:	000086        	j	346 <sbc_enc_bit_alloc_mono+0x106>

00000343 <sbc_enc_bit_alloc_mono+0x103>:
	...

00000344 <sbc_enc_bit_alloc_mono+0x104>:
 344:	041c           	movi.n	a4, 16

00000346 <sbc_enc_bit_alloc_mono+0x106>:
 346:	0749           	s32i.n	a4, a7, 0

00000348 <sbc_enc_bit_alloc_mono+0x108>:
 348:	774b           	addi.n	a7, a7, 4

0000034a <sbc_enc_bit_alloc_mono+0x10a>:
 34a:	0d2d           	mov.n	a2, a13
 34c:	0a7d           	mov.n	a7, a10
 34e:	3919a6        	blti	a9, 1, 38b <sbc_enc_bit_alloc_mono+0x14b>
 351:	361ca6        	blti	a12, 1, 38b <sbc_enc_bit_alloc_mono+0x14b>
 354:	661b           	addi.n	a6, a6, 1
 356:	040c           	movi.n	a4, 0
 358:	280c           	movi.n	a8, 2
 35a:	198076        	loop	a0, 377 <sbc_enc_bit_alloc_mono+0x137>
 35d:	0758           	l32i.n	a5, a7, 0
 35f:	441b           	addi.n	a4, a4, 1
 361:	1525a6        	blti	a5, 2, 37a <sbc_enc_bit_alloc_mono+0x13a>
 364:	12b5e6        	bgei	a5, 16, 37a <sbc_enc_bit_alloc_mono+0x13a>
 367:	990b           	addi.n	a9, a9, -1
 369:	b51b           	addi.n	a11, a5, 1
 36b:	07b9           	s32i.n	a11, a7, 0

0000036d <sbc_enc_bit_alloc_mono+0x12d>:
 36d:	774b           	addi.n	a7, a7, 4
 36f:	224b           	addi.n	a2, a2, 4
 371:	1619a6        	blti	a9, 1, 38b <sbc_enc_bit_alloc_mono+0x14b>
 374:	13a4c7        	bge	a4, a12, 38b <sbc_enc_bit_alloc_mono+0x14b>

00000377 <sbc_enc_bit_alloc_mono+0x137>:
 377:	fff7c6        	j	35a <sbc_enc_bit_alloc_mono+0x11a>

0000037a <sbc_enc_bit_alloc_mono+0x13a>:
 37a:	02d8           	l32i.n	a13, a2, 0
 37c:	ed9d67        	bne	a13, a6, 36d <sbc_enc_bit_alloc_mono+0x12d>
 37f:	ea29a6        	blti	a9, 2, 36d <sbc_enc_bit_alloc_mono+0x12d>
 382:	0789           	s32i.n	a8, a7, 0
 384:	fec992        	addi	a9, a9, -2
 387:	fff886        	j	36d <sbc_enc_bit_alloc_mono+0x12d>

0000038a <sbc_enc_bit_alloc_mono+0x14a>:
	...

0000038b <sbc_enc_bit_alloc_mono+0x14b>:
 38b:	0a7d           	mov.n	a7, a10
 38d:	1f19a6        	blti	a9, 1, 3b0 <sbc_enc_bit_alloc_mono+0x170>
 390:	1c1ca6        	blti	a12, 1, 3b0 <sbc_enc_bit_alloc_mono+0x170>
 393:	040c           	movi.n	a4, 0
 395:	148076        	loop	a0, 3ad <sbc_enc_bit_alloc_mono+0x16d>
 398:	0758           	l32i.n	a5, a7, 0
 39a:	441b           	addi.n	a4, a4, 1
 39c:	05b5e6        	bgei	a5, 16, 3a5 <sbc_enc_bit_alloc_mono+0x165>
 39f:	990b           	addi.n	a9, a9, -1
 3a1:	e51b           	addi.n	a14, a5, 1
 3a3:	07e9           	s32i.n	a14, a7, 0

000003a5 <sbc_enc_bit_alloc_mono+0x165>:
 3a5:	774b           	addi.n	a7, a7, 4
 3a7:	0519a6        	blti	a9, 1, 3b0 <sbc_enc_bit_alloc_mono+0x170>
 3aa:	02a4c7        	bge	a4, a12, 3b0 <sbc_enc_bit_alloc_mono+0x170>

000003ad <sbc_enc_bit_alloc_mono+0x16d>:
 3ad:	fff906        	j	395 <sbc_enc_bit_alloc_mono+0x155>

000003b0 <sbc_enc_bit_alloc_mono+0x170>:
 3b0:	f01d           	retw.n

000003b2 <sbc_enc_bit_alloc_mono+0x172>:
	...

000003b3 <sbc_enc_bit_alloc_mono+0x173>:
 3b3:	050c           	movi.n	a5, 0
 3b5:	0219a6        	blti	a9, 1, 3bb <sbc_enc_bit_alloc_mono+0x17b>
 3b8:	ffca06        	j	2e4 <sbc_enc_bit_alloc_mono+0xa4>
 3bb:	050c           	movi.n	a5, 0
 3bd:	f5a956        	bnez	a9, 31b <sbc_enc_bit_alloc_mono+0xdb>

000003c0 <sbc_enc_bit_alloc_mono+0x180>:
 3c0:	c09950        	sub	a9, a9, a5
 3c3:	660b           	addi.n	a6, a6, -1
 3c5:	ffd486        	j	31b <sbc_enc_bit_alloc_mono+0xdb>

000003c8 <sbc_enc_bit_alloc_mono+0x188>:
 3c8:	40c2d2        	addi	a13, a2, 64
 3cb:	021ca6        	blti	a12, 1, 3d1 <sbc_enc_bit_alloc_mono+0x191>
 3ce:	ffb186        	j	298 <sbc_enc_bit_alloc_mono+0x58>

000003d1 <sbc_enc_bit_alloc_mono+0x191>:
 3d1:	040c           	movi.n	a4, 0
 3d3:	ffc146        	j	2dc <sbc_enc_bit_alloc_mono+0x9c>

000003d6 <sbc_enc_bit_alloc_mono+0x196>:
	...

000003d8 <mybit_pack>:
 3d8:	004136        	entry	a1, 32
 3db:	937d           	ae_zext16	a7, a3
 3dd:	0298           	l32i.n	a9, a2, 0
 3df:	081c           	movi.n	a8, 16
 3e1:	021232        	l16ui	a3, a2, 4
 3e4:	c08890        	sub	a8, a8, a9
 3e7:	11a487        	bge	a4, a8, 3fc <mybit_pack+0x24>
 3ea:	401400        	ssl	a4
 3ed:	b94a           	add.n	a11, a9, a4
 3ef:	02b9           	s32i.n	a11, a2, 0
 3f1:	a1a300        	sll	a10, a3
 3f4:	20a7a0        	or	a10, a7, a10
 3f7:	0252a2        	s16i	a10, a2, 4
 3fa:	f01d           	retw.n

000003fc <mybit_pack+0x24>:
 3fc:	32f8           	l32i.n	a15, a2, 12
 3fe:	22e8           	l32i.n	a14, a2, 8
 400:	ff0bc1        	l32r	a12, 2c (cf0 <MASK_bitp>)
 403:	c0d480        	sub	a13, a4, a8
 406:	401800        	ssl	a8
 409:	a16300        	sll	a6, a3
 40c:	90cdc0        	addx2	a12, a13, a12
 40f:	ee1b           	addi.n	a14, a14, 1
 411:	400d00        	ssr	a13
 414:	b15070        	sra	a5, a7
 417:	205560        	or	a5, a5, a6
 41a:	955d           	ae_zext16	a5, a5
 41c:	025252        	s16i	a5, a2, 4
 41f:	416850        	srli	a6, a5, 8
 422:	115580        	slli	a5, a5, 8
 425:	205560        	or	a5, a5, a6
 428:	6f5d           	ae_s16i.n	a5, a15, 0
 42a:	22e9           	s32i.n	a14, a2, 8
 42c:	02d9           	s32i.n	a13, a2, 0
 42e:	2ccd           	ae_l16si.n	a12, a12, 0
 430:	ff2b           	addi.n	a15, a15, 2
 432:	32f9           	s32i.n	a15, a2, 12
 434:	10cc70        	and	a12, a12, a7
 437:	0252c2        	s16i	a12, a2, 4
 43a:	f01d           	retw.n

0000043c <mSBC_Encoder_Process>:
 43c:	006136        	entry	a1, 48
 43f:	1149           	s32i.n	a4, a1, 4
 441:	04ad           	mov.n	a10, a4
 443:	fefb81        	l32r	a8, 30 (90 <SbcAnalysisFilter8>)
 446:	04d492        	addmi	a9, a4, 0x400
 449:	216932        	s32i	a3, a9, 132
 44c:	206922        	s32i	a2, a9, 128
 44f:	84c9b2        	addi	a11, a9, -124
 452:	01b9           	s32i.n	a11, a1, 0
 454:	1b2922        	l32i	a2, a9, 108
 457:	00a032        	movi	a3, 0
 45a:	276932        	s32i	a3, a9, 156
 45d:	4c5932        	s16i	a3, a9, 152
 460:	256932        	s32i	a3, a9, 148
 463:	0008e0        	callx8	a8
 466:	0e0c           	movi.n	a14, 0
 468:	fef3c1        	l32r	a12, 34 (8000 <_end+0x72a0>)
 46b:	fef351        	l32r	a5, 38 (800000 <_end+0x7ff2a0>)
 46e:	1168           	l32i.n	a6, a1, 4
 470:	940c           	movi.n	a4, 9
 472:	e70c           	movi.n	a7, 14
 474:	02d6f2        	addmi	a15, a6, 0x200
 477:	40cff2        	addi	a15, a15, 64
 47a:	03d662        	addmi	a6, a6, 0x300
 47d:	80c6d2        	addi	a13, a6, -128
 480:	a0c662        	addi	a6, a6, -96

00000483 <mSBC_Encoder_Process+0x47>:
 483:	0d8d           	mov.n	a8, a13
 485:	7112a6        	blti	a2, 1, 4fa <mSBC_Encoder_Process+0xbe>
 488:	0a0c           	movi.n	a10, 0
 48a:	0a6207        	bbci	a2, 0, 498 <mSBC_Encoder_Process+0x5c>
 48d:	0d98           	l32i.n	a9, a13, 0
 48f:	20cd82        	addi	a8, a13, 32
 492:	609190        	abs	a9, a9
 495:	53a9a0        	max	a10, a9, a10

00000498 <mSBC_Encoder_Process+0x5c>:
 498:	219120        	srai	a9, a2, 1
 49b:	12a976        	loopgtz	a9, 4b1 <mSBC_Encoder_Process+0x75>
 49e:	0898           	l32i.n	a9, a8, 0
 4a0:	88b8           	l32i.n	a11, a8, 32
 4a2:	40c882        	addi	a8, a8, 64
 4a5:	609190        	abs	a9, a9
 4a8:	60b1b0        	abs	a11, a11
 4ab:	5399a0        	max	a9, a9, a10
 4ae:	53ab90        	max	a10, a11, a9

000004b1 <mSBC_Encoder_Process+0x75>:
 4b1:	dd4b           	addi.n	a13, a13, 4
 4b3:	72b5a0        	salt	a11, a5, a10
 4b6:	208330        	or	a8, a3, a3
 4b9:	9384b0        	movnez	a8, a4, a11
 4bc:	132787        	blt	a7, a8, 4d3 <mSBC_Encoder_Process+0x97>
 4bf:	0fa092        	movi	a9, 15
 4c2:	c09980        	sub	a9, a9, a8
 4c5:	0aa976        	loopgtz	a9, 4d3 <mSBC_Encoder_Process+0x97>
 4c8:	401800        	ssl	a8
 4cb:	a1bc00        	sll	a11, a12
 4ce:	01aba7        	bge	a11, a10, 4d3 <mSBC_Encoder_Process+0x97>
 4d1:	881b           	addi.n	a8, a8, 1
 4d3:	53e8e0        	max	a14, a8, a14
 4d6:	889d           	ae_sext16	a9, a8
 4d8:	09ef94        	ae_s32ip	a9, a15, 4
 4db:	a49d67        	bne	a13, a6, 483 <mSBC_Encoder_Process+0x47>
 4de:	0198           	l32i.n	a9, a1, 0
 4e0:	fed781        	l32r	a8, 3c (240 <sbc_enc_bit_alloc_mono>)
 4e3:	11a8           	l32i.n	a10, a1, 4
 4e5:	3e69e2        	s32i	a14, a9, 248
 4e8:	0008e0        	callx8	a8
 4eb:	fed581        	l32r	a8, 40 (580 <EncPacking>)
 4ee:	0128           	l32i.n	a2, a1, 0
 4f0:	11a8           	l32i.n	a10, a1, 4
 4f2:	0008e0        	callx8	a8
 4f5:	422222        	l32i	a2, a2, 0x108
 4f8:	f01d           	retw.n

000004fa <mSBC_Encoder_Process+0xbe>:
 4fa:	0a0c           	movi.n	a10, 0
 4fc:	ffec46        	j	4b1 <mSBC_Encoder_Process+0x75>

000004ff <mSBC_Encoder_Process+0xc3>:
	...

00000500 <mSBC_SW_SN>:
 500:	004136        	entry	a1, 32
 503:	235d           	ae_l16si.n	a5, a3, 0
 505:	1f3526        	beqi	a5, 3, 528 <mSBC_SW_SN+0x28>
 508:	651b           	addi.n	a6, a5, 1
 50a:	636d           	ae_s16i.n	a6, a3, 0
 50c:	a6ac           	beqz.n	a6, 53a <mSBC_SW_SN+0x3a>
 50e:	211626        	beqi	a6, 1, 533 <mSBC_SW_SN+0x33>
 511:	650b           	addi.n	a6, a5, -1
 513:	c8a042        	movi	a4, 200
 516:	f8a032        	movi	a3, 248
 519:	833460        	moveqz	a3, a4, a6
 51c:	140c           	movi.n	a4, 1
 51e:	113380        	slli	a3, a3, 8
 521:	203340        	or	a3, a3, a4
 524:	623d           	ae_s16i.n	a3, a2, 0
 526:	f01d           	retw.n

00000528 <mSBC_SW_SN+0x28>:
 528:	fec781        	l32r	a8, 44 (801 <_DYNAMIC+0xb5>)
 52b:	090c           	movi.n	a9, 0
 52d:	639d           	ae_s16i.n	a9, a3, 0
 52f:	628d           	ae_s16i.n	a8, a2, 0
 531:	f01d           	retw.n

00000533 <mSBC_SW_SN+0x33>:
 533:	fec5a1        	l32r	a10, 48 (3801 <_end+0x2aa1>)
 536:	62ad           	ae_s16i.n	a10, a2, 0
 538:	f01d           	retw.n

0000053a <mSBC_SW_SN+0x3a>:
 53a:	fec2b1        	l32r	a11, 44 (801 <_DYNAMIC+0xb5>)
 53d:	62bd           	ae_s16i.n	a11, a2, 0
 53f:	f01d           	retw.n

00000541 <mSBC_SW_SN+0x41>:
 541:	000000                                        ...

00000544 <mSBC_SW_SN_init>:
 544:	004136        	entry	a1, 32
 547:	030c           	movi.n	a3, 0
 549:	623d           	ae_s16i.n	a3, a2, 0
 54b:	f01d           	retw.n

0000054d <mSBC_SW_SN_init+0x9>:
 54d:	000000                                        ...

00000550 <SBC_Encoder_Init>:
 550:	004136        	entry	a1, 32
 553:	02ad           	mov.n	a10, a2
 555:	febd81        	l32r	a8, 4c (1d0 <SbcAnalysisInit>)
 558:	192c           	movi.n	a9, 33
 55a:	ac1c           	movi.n	a12, 26
 55c:	fd0c           	movi.n	a13, 15
 55e:	8e0c           	movi.n	a14, 8
 560:	02d2b2        	addmi	a11, a2, 0x200
 563:	0f0c           	movi.n	a15, 0
 565:	986bf2        	s32i	a15, a11, 0x260
 568:	996bf2        	s32i	a15, a11, 0x264
 56b:	9c6bf2        	s32i	a15, a11, 0x270
 56e:	9a6be2        	s32i	a14, a11, 0x268
 571:	9b6bd2        	s32i	a13, a11, 0x26c
 574:	9d6bc2        	s32i	a12, a11, 0x274
 577:	a26b92        	s32i	a9, a11, 0x288
 57a:	0008e0        	callx8	a8
 57d:	f01d           	retw.n

0000057f <SBC_Encoder_Init+0x2f>:
	...

00000580 <EncPacking>:
 580:	008136        	entry	a1, 64
 583:	8c0c           	movi.n	a12, 8
 585:	04d2b2        	addmi	a11, a2, 0x400
 588:	03d282        	addmi	a8, a2, 0x300
 58b:	05d272        	addmi	a7, a2, 0x500
 58e:	94c772        	addi	a7, a7, -108
 591:	6128a2        	l32i	a10, a8, 0x184
 594:	84c892        	addi	a9, a8, -124
 597:	0199           	s32i.n	a9, a1, 0
 599:	286ba2        	s32i	a10, a11, 160
 59c:	5a2882        	l32i	a8, a8, 0x168
 59f:	4189           	s32i.n	a8, a1, 16
 5a1:	feab81        	l32r	a8, 50 (3d8 <mybit_pack>)
 5a4:	ada0b2        	movi	a11, 173
 5a7:	07ad           	mov.n	a10, a7
 5a9:	0008e0        	callx8	a8
 5ac:	07ad           	mov.n	a10, a7
 5ae:	fea881        	l32r	a8, 50 (3d8 <mybit_pack>)
 5b1:	0b0c           	movi.n	a11, 0
 5b3:	8c0c           	movi.n	a12, 8
 5b5:	0008e0        	callx8	a8
 5b8:	07ad           	mov.n	a10, a7
 5ba:	fea581        	l32r	a8, 50 (3d8 <mybit_pack>)
 5bd:	0b0c           	movi.n	a11, 0
 5bf:	8c0c           	movi.n	a12, 8
 5c1:	0008e0        	callx8	a8
 5c4:	07ad           	mov.n	a10, a7
 5c6:	fea281        	l32r	a8, 50 (3d8 <mybit_pack>)
 5c9:	0b0c           	movi.n	a11, 0
 5cb:	8c0c           	movi.n	a12, 8
 5cd:	0008e0        	callx8	a8
 5d0:	4198           	l32i.n	a9, a1, 16
 5d2:	02d252        	addmi	a5, a2, 0x200
 5d5:	40c552        	addi	a5, a5, 64
 5d8:	3159           	s32i.n	a5, a1, 12
 5da:	093d           	mov.n	a3, a9
 5dc:	2d19a6        	blti	a9, 1, 60d <EncPacking+0x8d>
 5df:	090c           	movi.n	a9, 0
 5e1:	040c           	movi.n	a4, 0
 5e3:	000146        	j	5ec <EncPacking+0x6c>

000005e6 <EncPacking+0x66>:
 5e6:	190c           	movi.n	a9, 1

000005e8 <EncPacking+0x68>:
 5e8:	330b           	addi.n	a3, a3, -1
 5ea:	f39c           	beqz.n	a3, 60d <EncPacking+0x8d>

000005ec <EncPacking+0x6c>:
 5ec:	0588           	l32i.n	a8, a5, 0
 5ee:	1144c0        	slli	a4, a4, 4
 5f1:	554b           	addi.n	a5, a5, 4
 5f3:	204840        	or	a4, a8, a4
 5f6:	744040        	extui	a4, a4, 0, 8
 5f9:	e91966        	bnei	a9, 1, 5e6 <EncPacking+0x66>
 5fc:	07ad           	mov.n	a10, a7
 5fe:	fe9481        	l32r	a8, 50 (3d8 <mybit_pack>)
 601:	04bd           	mov.n	a11, a4
 603:	8c0c           	movi.n	a12, 8
 605:	0008e0        	callx8	a8
 608:	090c           	movi.n	a9, 0
 60a:	fff686        	j	5e8 <EncPacking+0x68>

0000060d <EncPacking+0x8d>:
 60d:	01a8           	l32i.n	a10, a1, 0
 60f:	7a2aa2        	l32i	a10, a10, 0x1e8
 612:	03d242        	addmi	a4, a2, 0x300
 615:	80c442        	addi	a4, a4, -128
 618:	ba0b           	addi.n	a11, a10, -1
 61a:	51b9           	s32i.n	a11, a1, 20
 61c:	6d1aa6        	blti	a10, 1, 68d <EncPacking+0x10d>
 61f:	fe8d61        	l32r	a6, 54 (ffff <_end+0xf29f>)
 622:	02d282        	addmi	a8, a2, 0x200
 625:	41a8           	l32i.n	a10, a1, 16
 627:	60c882        	addi	a8, a8, 96
 62a:	1189           	s32i.n	a8, a1, 4
 62c:	aa0b           	addi.n	a10, a10, -1
 62e:	21a9           	s32i.n	a10, a1, 8

00000630 <EncPacking+0xb0>:
 630:	2138           	l32i.n	a3, a1, 8
 632:	41b8           	l32i.n	a11, a1, 16
 634:	3128           	l32i.n	a2, a1, 12
 636:	1158           	l32i.n	a5, a1, 4
 638:	481ba6        	blti	a11, 1, 684 <EncPacking+0x104>

0000063b <EncPacking+0xbb>:
 63b:	05c8           	l32i.n	a12, a5, 0
 63d:	554b           	addi.n	a5, a5, 4
 63f:	8cbc           	beqz.n	a12, 67b <EncPacking+0xfb>
 641:	07ad           	mov.n	a10, a7
 643:	fe8381        	l32r	a8, 50 (3d8 <mybit_pack>)
 646:	04b8           	l32i.n	a11, a4, 0
 648:	1e0c           	movi.n	a14, 1
 64a:	02d8           	l32i.n	a13, a2, 0
 64c:	091c           	movi.n	a9, 16
 64e:	c099c0        	sub	a9, a9, a12
 651:	fdfb           	addi.n	a15, a13, 15
 653:	401f00        	ssl	a15
 656:	a1ee00        	sll	a14, a14
 659:	400900        	ssr	a9
 65c:	bbea           	add.n	a11, a11, a14
 65e:	b1f060        	sra	a15, a6
 661:	11ff10        	slli	a15, a15, 15
 664:	82ebf0        	mull	a14, a11, a15
 667:	b2bbf0        	mulsh	a11, a11, a15
 66a:	404f10        	ssai	31
 66d:	81bbe0        	src	a11, a11, a14
 670:	400d00        	ssr	a13
 673:	b1b0b0        	sra	a11, a11
 676:	9bbd           	ae_zext16	a11, a11
 678:	0008e0        	callx8	a8

0000067b <EncPacking+0xfb>:
 67b:	224b           	addi.n	a2, a2, 4
 67d:	444b           	addi.n	a4, a4, 4
 67f:	330b           	addi.n	a3, a3, -1
 681:	b60366        	bnei	a3, -1, 63b <EncPacking+0xbb>

00000684 <EncPacking+0x104>:
 684:	51a8           	l32i.n	a10, a1, 20
 686:	aa0b           	addi.n	a10, a10, -1
 688:	51a9           	s32i.n	a10, a1, 20
 68a:	a20a66        	bnei	a10, -1, 630 <EncPacking+0xb0>

0000068d <EncPacking+0x10d>:
 68d:	01b8           	l32i.n	a11, a1, 0
 68f:	862b52        	l32i	a5, a11, 0x218
 692:	842ba2        	l32i	a10, a11, 0x210
 695:	1155f0        	slli	a5, a5, 1
 698:	826b52        	s32i	a5, a11, 0x208
 69b:	7a9c           	beqz.n	a10, 6b6 <EncPacking+0x136>
 69d:	0b0c           	movi.n	a11, 0
 69f:	0c1c           	movi.n	a12, 16
 6a1:	fe6b81        	l32r	a8, 50 (3d8 <mybit_pack>)
 6a4:	c0cca0        	sub	a12, a12, a10
 6a7:	07ad           	mov.n	a10, a7
 6a9:	0008e0        	callx8	a8
 6ac:	01d8           	l32i.n	a13, a1, 0
 6ae:	822d52        	l32i	a5, a13, 0x208
 6b1:	551b           	addi.n	a5, a5, 1
 6b3:	826d52        	s32i	a5, a13, 0x208

000006b6 <EncPacking+0x136>:
 6b6:	0168           	l32i.n	a6, a1, 0
 6b8:	802662        	l32i	a6, a6, 0x200
 6bb:	41a8           	l32i.n	a10, a1, 16
 6bd:	d87c           	movi.n	a8, -3
 6bf:	26ed           	ae_l16si.n	a14, a6, 0
 6c1:	21a1a0        	srai	a10, a10, 1
 6c4:	06fd           	mov.n	a15, a6
 6c6:	7448e0        	extui	a4, a14, 8, 8
 6c9:	5ba8a7        	bge	a8, a10, 728 <EncPacking+0x1a8>
 6cc:	2a4b           	addi.n	a2, a10, 4
 6ce:	130c           	movi.n	a3, 1
 6d0:	fa0c           	movi.n	a10, 15
 6d2:	0001c6        	j	6dd <EncPacking+0x15d>

000006d5 <EncPacking+0x155>:
 6d5:	7448e0        	extui	a4, a14, 8, 8

000006d8 <EncPacking+0x158>:
 6d8:	331b           	addi.n	a3, a3, 1
 6da:	341237        	beq	a2, a3, 712 <EncPacking+0x192>

000006dd <EncPacking+0x15d>:
 6dd:	243326        	beqi	a3, 3, 705 <EncPacking+0x185>
 6e0:	780c           	movi.n	a8, 7
 6e2:	08a092        	movi	a9, 8
 6e5:	1c8976        	loop	a9, 705 <EncPacking+0x185>
 6e8:	400800        	ssr	a8
 6eb:	21b7a0        	srai	a11, a10, 7
 6ee:	11aaf0        	slli	a10, a10, 1
 6f1:	b19040        	sra	a9, a4
 6f4:	880b           	addi.n	a8, a8, -1
 6f6:	30bb90        	xor	a11, a11, a9
 6f9:	04b0b0        	extui	a11, a11, 0, 1
 6fc:	f09bb0        	subx8	a9, a11, a11
 6ff:	a0b9b0        	addx4	a11, a9, a11
 702:	30aba0        	xor	a10, a11, a10

00000705 <EncPacking+0x185>:
 705:	cc6307        	bbci	a3, 0, 6d5 <EncPacking+0x155>
 708:	3fed           	ae_l16si.n	a14, a15, 2
 70a:	ff2b           	addi.n	a15, a15, 2
 70c:	7440e0        	extui	a4, a14, 0, 8
 70f:	fff146        	j	6d8 <EncPacking+0x158>

00000712 <EncPacking+0x192>:
 712:	0198           	l32i.n	a9, a1, 0
 714:	36bd           	ae_l16si.n	a11, a6, 2
 716:	908560        	addx2	a8, a5, a6
 719:	11ca80        	slli	a12, a10, 8
 71c:	20bbc0        	or	a11, a11, a12
 71f:	76bd           	ae_s16i.n	a11, a6, 2
 721:	806982        	s32i	a8, a9, 0x200
 724:	f01d           	retw.n

00000726 <EncPacking+0x1a6>:
	...

00000728 <EncPacking+0x1a8>:
 728:	fe4c81        	l32r	a8, 58 (f00 <_end+0x1a0>)
 72b:	36fd           	ae_l16si.n	a15, a6, 2
 72d:	01e8           	l32i.n	a14, a1, 0
 72f:	90d560        	addx2	a13, a5, a6
 732:	20ff80        	or	a15, a15, a8
 735:	76fd           	ae_s16i.n	a15, a6, 2
 737:	806ed2        	s32i	a13, a14, 0x200
 73a:	f01d           	retw.n

0000073c <_fini>:
 73c:	008136        	entry	a1, 64

0000073f <_fini+0x3>:
 73f:	f01d           	retw.n
