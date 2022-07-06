
Build/lib/pisplit_cvsd_enc_position_independent.splitload.lib:     file format elf32-xtensa-le


Disassembly of section .text:

00000000 <_init-0x78>:
   0:	00000bd0 00000ae0 000002c0 000002f0     ................
  10:	00000bc0 000000a0 001fffff 00002800     .............(..
  20:	00140000 7c000000 ffe00001 7fe00000     .......|........
  30:	00000ba0 000000b4 000007a0 000007e8     ................
  40:	000000dc 00000b70 00007fff ffff8000     ....p...........
  50:	00000b90 00000b10 00000b30 00000b50     ........0...P...
  60:	00000338 00000370 00000508 000005a0     8...p...........
  70:	00000638 0000071c                       8.......

00000078 <_init>:
  78:	008136        	entry	a1, 64
  7b:	f01d           	retw.n

0000007d <_init+0x5>:
  7d:	000000                                        ...

00000080 <_start>:
  80:	004136        	entry	a1, 32
  83:	ffe0a1        	l32r	a10, 4 (ae0 <_DYNAMIC+0x28c>)
  86:	ffe0b1        	l32r	a11, 8 (2c0 <_CVSD_Encoder_Init>)
  89:	ffdd91        	l32r	a9, 0 (bd0 <__bss_start>)
  8c:	002282        	l32i	a8, a2, 0
  8f:	ffdfc1        	l32r	a12, c (2f0 <_CVSD_Encoder>)
  92:	006982        	s32i	a8, a9, 0
  95:	0008e0        	callx8	a8
  98:	ffde21        	l32r	a2, 10 (bc0 <export_parameter_array>)
  9b:	f01d           	retw.n

0000009d <_start+0x1d>:
  9d:	000000                                        ...

000000a0 <cvsd_codec_init>:
  a0:	004136        	entry	a1, 32
  a3:	080c           	movi.n	a8, 0
  a5:	a30c           	movi.n	a3, 10
  a7:	2239           	s32i.n	a3, a2, 8
  a9:	0289           	s32i.n	a8, a2, 0
  ab:	1289           	s32i.n	a8, a2, 4
  ad:	082d           	mov.n	a2, a8
  af:	f01d           	retw.n

000000b1 <cvsd_codec_init+0x11>:
  b1:	000000                                        ...

000000b4 <cvsd_encode_init>:
  b4:	004136        	entry	a1, 32
  b7:	ffd781        	l32r	a8, 14 (a0 <cvsd_codec_init>)
  ba:	20a330        	or	a10, a3, a3
  bd:	0008e0        	callx8	a8
  c0:	0239           	s32i.n	a3, a2, 0
  c2:	0a2d           	mov.n	a2, a10
  c4:	f01d           	retw.n

000000c6 <cvsd_encode_init+0x12>:
	...

000000c8 <cvsd_decode_init>:
  c8:	004136        	entry	a1, 32
  cb:	ffd281        	l32r	a8, 14 (a0 <cvsd_codec_init>)
  ce:	03ad           	mov.n	a10, a3
  d0:	0008e0        	callx8	a8
  d3:	0239           	s32i.n	a3, a2, 0
  d5:	0a2d           	mov.n	a2, a10
  d7:	f01d           	retw.n

000000d9 <cvsd_decode_init+0x11>:
  d9:	000000                                        ...

000000dc <cvsd_encode_process>:
  dc:	006136        	entry	a1, 48
  df:	ffd1f1        	l32r	a15, 24 (7c000000 <_end+0x7bfff42c>)
  e2:	ffd1e1        	l32r	a14, 28 (ffe00001 <_end+0xffdff42d>)
  e5:	0169           	s32i.n	a6, a1, 0
  e7:	1159           	s32i.n	a5, a1, 4
  e9:	0698           	l32i.n	a9, a6, 0
  eb:	0478           	l32i.n	a7, a4, 0
  ed:	2129           	s32i.n	a2, a1, 8
  ef:	0a0c           	movi.n	a10, 0
  f1:	ffc921        	l32r	a2, 18 (1fffff <_end+0x1ff42b>)
  f4:	0326a4        	wur.ae_bitptr	a10
  f7:	f3f1a0        	wur.ae_bithead	a10
  fa:	417570        	srli	a7, a7, 5
  fd:	637790        	minu	a7, a7, a9
 100:	21a8           	l32i.n	a10, a1, 8
 102:	1197b0        	slli	a9, a7, 5
 105:	0499           	s32i.n	a9, a4, 0
 107:	fec542        	addi	a4, a5, -2
 10a:	006672        	s32i	a7, a6, 0
 10d:	ffc351        	l32r	a5, 1c (2800 <_end+0x1c2c>)
 110:	ffc461        	l32r	a6, 20 (140000 <_end+0x13f42c>)
 113:	002ab2        	l32i	a11, a10, 0
 116:	012a92        	l32i	a9, a10, 4
 119:	022aa2        	l32i	a10, a10, 8
 11c:	1177c0        	slli	a7, a7, 4
 11f:	439776        	loopnez	a7, 166 <cvsd_encode_process+0x8a>
 122:	21a1a0        	srai	a10, a10, 1
 125:	23cd           	ae_l16si.n	a12, a3, 0
 127:	11dbf0        	slli	a13, a11, 1
 12a:	b2ddf0        	mulsh	a13, a13, a15
 12d:	11cca0        	slli	a12, a12, 6
 130:	c0ccd0        	sub	a12, a12, a13
 133:	05bfc0        	extui	a11, a12, 31, 1
 136:	117bd0        	slli	a7, a11, 3
 139:	20aa70        	or	a10, a10, a7
 13c:	f70c           	movi.n	a7, 15
 13e:	701a77        	beq	a10, a7, 1b2 <cvsd_encode_process+0xd6>
 141:	06da16        	beqz	a10, 1b2 <cvsd_encode_process+0xd6>
 144:	ffba71        	l32r	a7, 2c (7fe00000 <_end+0x7fdff42c>)
 147:	1189f0        	slli	a8, a9, 1
 14a:	b28870        	mulsh	a8, a8, a7

0000014d <cvsd_encode_process+0x71>:
 14d:	1114b4        	ae_sbi	a4, a11, 1
 150:	332b           	addi.n	a3, a3, 2
 152:	439860        	min	a9, a8, a6
 155:	539950        	max	a9, a9, a5
 158:	05cc96        	bltz	a12, 1b8 <cvsd_encode_process+0xdc>
 15b:	218490        	srai	a8, a9, 4
 15e:	8d8a           	add.n	a8, a13, a8

00000160 <cvsd_encode_process+0x84>:
 160:	43b820        	min	a11, a8, a2
 163:	53bbe0        	max	a11, a11, a14

00000166 <cvsd_encode_process+0x8a>:
 166:	0138           	l32i.n	a3, a1, 0
 168:	0338           	l32i.n	a3, a3, 0
 16a:	ffb171        	l32r	a7, 30 (ba0 <cvsd_BitReverseTable16>)
 16d:	1128           	l32i.n	a2, a1, 4
 16f:	339376        	loopnez	a3, 1a6 <cvsd_encode_process+0xca>
 172:	423d           	ae_l16ui.n	a3, a2, 0
 174:	344430        	extui	a4, a3, 4, 4
 177:	345830        	extui	a5, a3, 8, 4
 17a:	346030        	extui	a6, a3, 0, 4
 17d:	413c30        	srli	a3, a3, 12
 180:	474a           	add.n	a4, a7, a4
 182:	575a           	add.n	a5, a7, a5
 184:	373a           	add.n	a3, a7, a3
 186:	676a           	add.n	a6, a7, a6
 188:	000552        	l8ui	a5, a5, 0
 18b:	000442        	l8ui	a4, a4, 0
 18e:	000332        	l8ui	a3, a3, 0
 191:	000662        	l8ui	a6, a6, 0
 194:	ca7540        	depbits	a4, a5, 12, 8
 197:	113380        	slli	a3, a3, 8
 19a:	1156c0        	slli	a5, a6, 4
 19d:	203430        	or	a3, a4, a3
 1a0:	203350        	or	a3, a3, a5
 1a3:	09c234        	ae_s16ip	a3, a2, 2
 1a6:	2178           	l32i.n	a7, a1, 8
 1a8:	020c           	movi.n	a2, 0
 1aa:	07b9           	s32i.n	a11, a7, 0
 1ac:	1799           	s32i.n	a9, a7, 4
 1ae:	27a9           	s32i.n	a10, a7, 8
 1b0:	f01d           	retw.n

000001b2 <cvsd_encode_process+0xd6>:
 1b2:	28d982        	addmi	a8, a9, 0x2800
 1b5:	ffe506        	j	14d <cvsd_encode_process+0x71>

000001b8 <cvsd_encode_process+0xdc>:
 1b8:	218490        	srai	a8, a9, 4
 1bb:	c08d80        	sub	a8, a13, a8
 1be:	ffe786        	j	160 <cvsd_encode_process+0x84>

000001c1 <cvsd_encode_process+0xe5>:
 1c1:	000000                                        ...

000001c4 <cvsd_decode_process>:
 1c4:	006136        	entry	a1, 48
 1c7:	05cd           	mov.n	a12, a5
 1c9:	ff9971        	l32r	a7, 30 (ba0 <cvsd_BitReverseTable16>)
 1cc:	03bd           	mov.n	a11, a3
 1ce:	1129           	s32i.n	a2, a1, 4
 1d0:	06f8           	l32i.n	a15, a6, 0
 1d2:	0428           	l32i.n	a2, a4, 0
 1d4:	11a8           	l32i.n	a10, a1, 4
 1d6:	41f5f0        	srli	a15, a15, 5
 1d9:	63ff20        	minu	a15, a15, a2
 1dc:	01f9           	s32i.n	a15, a1, 0
 1de:	04f9           	s32i.n	a15, a4, 0
 1e0:	0b2d           	mov.n	a2, a11
 1e2:	11ffb0        	slli	a15, a15, 5
 1e5:	06f9           	s32i.n	a15, a6, 0
 1e7:	1a98           	l32i.n	a9, a10, 4
 1e9:	0ad8           	l32i.n	a13, a10, 0
 1eb:	0438           	l32i.n	a3, a4, 0
 1ed:	21d9           	s32i.n	a13, a1, 8
 1ef:	2aa8           	l32i.n	a10, a10, 8
 1f1:	339376        	loopnez	a3, 228 <cvsd_decode_process+0x64>
 1f4:	423d           	ae_l16ui.n	a3, a2, 0
 1f6:	344430        	extui	a4, a3, 4, 4
 1f9:	345830        	extui	a5, a3, 8, 4
 1fc:	346030        	extui	a6, a3, 0, 4
 1ff:	413c30        	srli	a3, a3, 12
 202:	474a           	add.n	a4, a7, a4
 204:	575a           	add.n	a5, a7, a5
 206:	373a           	add.n	a3, a7, a3
 208:	676a           	add.n	a6, a7, a6
 20a:	000552        	l8ui	a5, a5, 0
 20d:	000442        	l8ui	a4, a4, 0
 210:	000332        	l8ui	a3, a3, 0
 213:	000662        	l8ui	a6, a6, 0
 216:	ca7540        	depbits	a4, a5, 12, 8
 219:	113380        	slli	a3, a3, 8
 21c:	1156c0        	slli	a5, a6, 4
 21f:	203430        	or	a3, a4, a3
 222:	203350        	or	a3, a3, a5
 225:	09c234        	ae_s16ip	a3, a2, 2

00000228 <cvsd_decode_process+0x64>:
 228:	ff7ed1        	l32r	a13, 20 (140000 <_end+0x13f42c>)
 22b:	ff7ce1        	l32r	a14, 1c (2800 <_end+0x1c2c>)
 22e:	ff7af1        	l32r	a15, 18 (1fffff <_end+0x1ff42b>)
 231:	ff7e21        	l32r	a2, 2c (7fe00000 <_end+0x7fdff42c>)
 234:	ff7c51        	l32r	a5, 24 (7c000000 <_end+0x7bfff42c>)
 237:	0178           	l32i.n	a7, a1, 0
 239:	fecbb2        	addi	a11, a11, -2
 23c:	00a082        	movi	a8, 0
 23f:	f3f180        	wur.ae_bithead	a8
 242:	032684        	wur.ae_bitptr	a8
 245:	000bf4        	ae_dbi	a11, 16
 248:	ff7831        	l32r	a3, 28 (ffe00001 <_end+0xffdff42d>)
 24b:	1177c0        	slli	a7, a7, 4
 24e:	000bf4        	ae_dbi	a11, 16
 251:	4b9776        	loopnez	a7, 2a0 <cvsd_decode_process+0xdc>
 254:	2178           	l32i.n	a7, a1, 8
 256:	1167f0        	slli	a6, a7, 1
 259:	b26650        	mulsh	a6, a6, a5
 25c:	21a1a0        	srai	a10, a10, 1
 25f:	214660        	srai	a4, a6, 6
 262:	0077d6        	bgez	a7, 26d <cvsd_decode_process+0xa9>
 265:	548060        	extui	a8, a6, 0, 6
 268:	741b           	addi.n	a7, a4, 1
 26a:	934780        	movnez	a4, a7, a8

0000026d <cvsd_decode_process+0xa9>:
 26d:	09cc44        	ae_s16ip	a4, a12, 2
 270:	034f14        	ae_lbi	a4, 1
 273:	001bf4        	ae_dbi	a11, 1
 276:	1174d0        	slli	a7, a4, 3
 279:	20aa70        	or	a10, a10, a7
 27c:	0fa072        	movi	a7, 15
 27f:	2d1a77        	beq	a10, a7, 2b0 <cvsd_decode_process+0xec>
 282:	aaac           	beqz.n	a10, 2b0 <cvsd_decode_process+0xec>
 284:	1189f0        	slli	a8, a9, 1
 287:	b28820        	mulsh	a8, a8, a2

0000028a <cvsd_decode_process+0xc6>:
 28a:	4398d0        	min	a9, a8, a13
 28d:	5399e0        	max	a9, a9, a14
 290:	44ac           	beqz.n	a4, 2b8 <cvsd_decode_process+0xf4>
 292:	214490        	srai	a4, a9, 4
 295:	c04640        	sub	a4, a6, a4

00000298 <cvsd_decode_process+0xd4>:
 298:	537430        	max	a7, a4, a3
 29b:	4377f0        	min	a7, a7, a15
 29e:	2179           	s32i.n	a7, a1, 8

000002a0 <cvsd_decode_process+0xdc>:
 2a0:	1188           	l32i.n	a8, a1, 4
 2a2:	020c           	movi.n	a2, 0
 2a4:	21b8           	l32i.n	a11, a1, 8
 2a6:	08b9           	s32i.n	a11, a8, 0
 2a8:	1899           	s32i.n	a9, a8, 4
 2aa:	28a9           	s32i.n	a10, a8, 8
 2ac:	f01d           	retw.n

000002ae <cvsd_decode_process+0xea>:
	...

000002b0 <cvsd_decode_process+0xec>:
 2b0:	28d982        	addmi	a8, a9, 0x2800
 2b3:	fff4c6        	j	28a <cvsd_decode_process+0xc6>

000002b6 <cvsd_decode_process+0xf2>:
	...

000002b8 <cvsd_decode_process+0xf4>:
 2b8:	214490        	srai	a4, a9, 4
 2bb:	464a           	add.n	a4, a6, a4
 2bd:	fff5c6        	j	298 <cvsd_decode_process+0xd4>

000002c0 <_CVSD_Encoder_Init>:
 2c0:	004136        	entry	a1, 32
 2c3:	09d2a2        	addmi	a10, a2, 0x900
 2c6:	c83c           	movi.n	a8, 60
 2c8:	5a5a82        	s16i	a8, a10, 180
 2cb:	161326        	beqi	a3, 1, 2e5 <_CVSD_Encoder_Init+0x25>

000002ce <_CVSD_Encoder_Init+0xe>:
 2ce:	ff5981        	l32r	a8, 34 (b4 <cvsd_encode_init>)
 2d1:	02ad           	mov.n	a10, a2
 2d3:	b28b           	addi.n	a11, a2, 8
 2d5:	0008e0        	callx8	a8
 2d8:	ff5881        	l32r	a8, 38 (7a0 <cvsd_src_up_sampling_init>)
 2db:	a24b           	addi.n	a10, a2, 4
 2dd:	14c2b2        	addi	a11, a2, 20
 2e0:	0008e0        	callx8	a8
 2e3:	f01d           	retw.n

000002e5 <_CVSD_Encoder_Init+0x25>:
 2e5:	78a092        	movi	a9, 120
 2e8:	5a5a92        	s16i	a9, a10, 180
 2eb:	fff7c6        	j	2ce <_CVSD_Encoder_Init+0xe>

000002ee <_CVSD_Encoder_Init+0x2e>:
	...

000002f0 <_CVSD_Encoder>:
 2f0:	006136        	entry	a1, 48
 2f3:	12a8           	l32i.n	a10, a2, 4
 2f5:	54c2c2        	addi	a12, a2, 84
 2f8:	ff5181        	l32r	a8, 3c (7e8 <cvsd_src_up_sampling_process>)
 2fb:	20b330        	or	a11, a3, a3
 2fe:	09d2e2        	addmi	a14, a2, 0x900
 301:	02d252        	addmi	a5, a2, 0x200
 304:	b8ce32        	addi	a3, a14, -72
 307:	5a9ee2        	l16si	a14, a14, 180
 30a:	34c552        	addi	a5, a5, 52
 30d:	20d550        	or	a13, a5, a5
 310:	11eef0        	slli	a14, a14, 1
 313:	0008e0        	callx8	a8
 316:	04dd           	mov.n	a13, a4
 318:	05bd           	mov.n	a11, a5
 31a:	02a8           	l32i.n	a10, a2, 0
 31c:	01cd           	mov.n	a12, a1
 31e:	e14b           	addi.n	a14, a1, 4
 320:	7e9392        	l16si	a9, a3, 252
 323:	ff4781        	l32r	a8, 40 (dc <cvsd_encode_process>)
 326:	1199           	s32i.n	a9, a1, 4
 328:	1199c0        	slli	a9, a9, 4
 32b:	0199           	s32i.n	a9, a1, 0
 32d:	0008e0        	callx8	a8
 330:	7e9322        	l16si	a2, a3, 252
 333:	f01d           	retw.n

00000335 <_CVSD_Encoder+0x45>:
 335:	000000                                        ...

00000338 <f_memset>:
 338:	004136        	entry	a1, 32
 33b:	2c14a6        	blti	a4, 1, 36b <f_memset+0x33>
 33e:	245040        	extui	a5, a4, 0, 3
 341:	046d           	mov.n	a6, a4
 343:	214360        	srai	a4, a6, 3
 346:	04a576        	loopgtz	a5, 34e <f_memset+0x16>
 349:	004232        	s8i	a3, a2, 0
 34c:	221b           	addi.n	a2, a2, 1
 34e:	19a476        	loopgtz	a4, 36b <f_memset+0x33>
 351:	004232        	s8i	a3, a2, 0
 354:	014232        	s8i	a3, a2, 1
 357:	024232        	s8i	a3, a2, 2
 35a:	034232        	s8i	a3, a2, 3
 35d:	044232        	s8i	a3, a2, 4
 360:	054232        	s8i	a3, a2, 5
 363:	064232        	s8i	a3, a2, 6
 366:	074232        	s8i	a3, a2, 7
 369:	228b           	addi.n	a2, a2, 8

0000036b <f_memset+0x33>:
 36b:	f01d           	retw.n

0000036d <f_memset+0x35>:
 36d:	000000                                        ...

00000370 <cvsd_src_up_sampling_mono_process_1>:
 370:	006136        	entry	a1, 48
 373:	f3f640        	wur.ae_cbegin0	a4
 376:	ff3371        	l32r	a7, 44 (b70 <cvsd_src_up_sampling_coef_1>)
 379:	e67c           	movi.n	a6, -2
 37b:	1cc482        	addi	a8, a4, 28
 37e:	f3f780        	wur.ae_cend0	a8
 381:	10a560        	and	a10, a5, a6
 384:	c488           	l32i.n	a8, a4, 48
 386:	446704        	ae_lp16x2f.i	aep6, a7, 0
 389:	445714        	ae_lp16x2f.i	aep5, a7, 4
 38c:	444724        	ae_lp16x2f.i	aep4, a7, 8
 38f:	443734        	ae_lp16x2f.i	aep3, a7, 12
 392:	442744        	ae_lp16x2f.i	aep2, a7, 16
 395:	441754        	ae_lp16x2f.i	aep1, a7, 20
 398:	440764        	ae_lp16x2f.i	aep0, a7, 24
 39b:	028567        	bany	a5, a6, 3a1 <cvsd_src_up_sampling_mono_process_1+0x31>
 39e:	005846        	j	503 <cvsd_src_up_sampling_mono_process_1+0x193>
 3a1:	470c           	movi.n	a7, 4
 3a3:	fcc332        	addi	a3, a3, -4
 3a6:	fec252        	addi	a5, a2, -2
 3a9:	ff2891        	l32r	a9, 4c (ffff8000 <_end+0xffff742c>)
 3ac:	ff27b1        	l32r	a11, 48 (7fff <_end+0x742b>)
 3af:	364b34        	ae_cvtq48a32s	aeq1, a11
 3b2:	360934        	ae_cvtq48a32s	aeq0, a9
 3b5:	020c           	movi.n	a2, 0
 3b7:	02a092        	movi	a9, 2
 3ba:	141a16        	beqz	a10, 4ff <cvsd_src_up_sampling_mono_process_1+0x18f>
 3bd:	179a76        	loopnez	a10, 3d8 <cvsd_src_up_sampling_mono_process_1+0x68>
 3c0:	0301a0        	rsr.lend	a10
 3c3:	1300a0        	wsr.lbeg	a10
 3c6:	27caa2        	addi	a10, a10, 39
 3c9:	01daa2        	addmi	a10, a10, 0x100
 3cc:	1301a0        	wsr.lend	a10
 3cf:	002000        	isync
 3d2:	0302a0        	rsr.lcount	a10
 3d5:	01caa2        	addi	a10, a10, 1
 3d8:	76e207        	bbsi	a2, 0, 452 <cvsd_src_up_sampling_mono_process_1+0xe2>
 3db:	fc24daf87f 	{ ae_lp16x2f.c	aep7, a8, a7; nop }
 3e0:	f31f9af87f 	{ ae_lp16x2f.c	aep7, a8, a7; ae_mulzaap24s.hl.lh	aeq2, aep7, aep0 }
 3e5:	e33d9af87f 	{ ae_lp16x2f.c	aep7, a8, a7; ae_mulaap24s.hl.lh	aeq2, aep7, aep1 }
 3ea:	e35d9af87f 	{ ae_lp16x2f.c	aep7, a8, a7; ae_mulaap24s.hl.lh	aeq2, aep7, aep2 }
 3ef:	e37d9af87f 	{ ae_lp16x2f.c	aep7, a8, a7; ae_mulaap24s.hl.lh	aeq2, aep7, aep3 }
 3f4:	e39d9af87f 	{ ae_lp16x2f.c	aep7, a8, a7; ae_mulaap24s.hl.lh	aeq2, aep7, aep4 }
 3f9:	e3bd9af87f 	{ ae_lp16x2f.c	aep7, a8, a7; ae_mulaap24s.hl.lh	aeq2, aep7, aep5 }
 3fe:	05f634        	ae_mulaap24s.hl.lh	aeq2, aep7, aep6
 401:	35a2b4        	ae_sraiq56	aeq2, aeq2, 10
 404:	0196c4        	ae_minq56s	aeq2, aeq2, aeq1
 407:	0182c4        	ae_maxq56s	aeq2, aeq2, aeq0
 40a:	632104        	ae_sq56s.i	aeq2, a1, 0
 40d:	507104        	ae_lp24f.i	aep7, a1, 0
 410:	42f324        	ae_sp16f.l.iu	aep7, a3, 4
 413:	fc24daf87f 	{ ae_lp16x2f.c	aep7, a8, a7; nop }
 418:	f1dfdaf87f 	{ ae_lp16x2f.c	aep7, a8, a7; ae_mulzaap24s.hh.ll	aeq3, aep7, aep6 }
 41d:	e3bcdaf87f 	{ ae_lp16x2f.c	aep7, a8, a7; ae_mulaap24s.hh.ll	aeq3, aep7, aep5 }
 422:	e39cdaf87f 	{ ae_lp16x2f.c	aep7, a8, a7; ae_mulaap24s.hh.ll	aeq3, aep7, aep4 }
 427:	e37cdaf87f 	{ ae_lp16x2f.c	aep7, a8, a7; ae_mulaap24s.hh.ll	aeq3, aep7, aep3 }
 42c:	e35cdaf87f 	{ ae_lp16x2f.c	aep7, a8, a7; ae_mulaap24s.hh.ll	aeq3, aep7, aep2 }
 431:	e33cdaf87f 	{ ae_lp16x2f.c	aep7, a8, a7; ae_mulaap24s.hh.ll	aeq3, aep7, aep1 }
 436:	05f824        	ae_mulaap24s.hh.ll	aeq3, aep7, aep0
 439:	35f2b4        	ae_sraiq56	aeq3, aeq3, 10
 43c:	fb8eacf51f 	{ ae_lp16f.iu	aep7, a5, 2; ae_minq56s	aeq3, aeq3, aeq1 }
 441:	fbe722f89f 	{ ae_sp16f.l.c	aep7, a8, a9; ae_maxq56s	aeq3, aeq3, aeq0 }
 446:	633104        	ae_sq56s.i	aeq3, a1, 0
 449:	507104        	ae_lp24f.i	aep7, a1, 0
 44c:	427314        	ae_sp16f.l.i	aep7, a3, 2
 44f:	002a86        	j	4fd <cvsd_src_up_sampling_mono_process_1+0x18d>

00000452 <cvsd_src_up_sampling_mono_process_1+0xe2>:
 452:	fc24d9789f 	{ ae_lp16f.c	aep7, a8, a9; nop }
 457:	eb1ff7011f 	{ ae_sp24x2s.i	aep0, a1, 8; ae_mulp24s.ll	aeq3, aep7, aep0 }
 45c:	dc209af87f 	{ ae_lp16x2f.c	aep7, a8, a7; ae_selp24.hl	aep0, aep0, aep1 }
 461:	05f824        	ae_mulaap24s.hh.ll	aeq3, aep7, aep0
 464:	dc449af87f 	{ ae_lp16x2f.c	aep7, a8, a7; ae_selp24.hl	aep0, aep1, aep2 }
 469:	05f824        	ae_mulaap24s.hh.ll	aeq3, aep7, aep0
 46c:	dc689af87f 	{ ae_lp16x2f.c	aep7, a8, a7; ae_selp24.hl	aep0, aep2, aep3 }
 471:	05f824        	ae_mulaap24s.hh.ll	aeq3, aep7, aep0
 474:	dc8c9af87f 	{ ae_lp16x2f.c	aep7, a8, a7; ae_selp24.hl	aep0, aep3, aep4 }
 479:	05f824        	ae_mulaap24s.hh.ll	aeq3, aep7, aep0
 47c:	dcb09af87f 	{ ae_lp16x2f.c	aep7, a8, a7; ae_selp24.hl	aep0, aep4, aep5 }
 481:	05f824        	ae_mulaap24s.hh.ll	aeq3, aep7, aep0
 484:	dcd49af87f 	{ ae_lp16x2f.c	aep7, a8, a7; ae_selp24.hl	aep0, aep5, aep6 }
 489:	e31cd9089f 	{ ae_lp16f.c	aep0, a8, a9; ae_mulaap24s.hh.ll	aeq3, aep7, aep0 }
 48e:	028e04        	ae_mulap24s.hh	aeq3, aep0, aep6
 491:	35f2b4        	ae_sraiq56	aeq3, aeq3, 10
 494:	01d7c4        	ae_minq56s	aeq3, aeq3, aeq1
 497:	01c3c4        	ae_maxq56s	aeq3, aeq3, aeq0
 49a:	633104        	ae_sq56s.i	aeq3, a1, 0
 49d:	507104        	ae_lp24f.i	aep7, a1, 0
 4a0:	42f324        	ae_sp16f.l.iu	aep7, a3, 4
 4a3:	dfb9d9089f 	{ ae_lp16f.c	aep0, a8, a9; ae_selp24.lh	aep7, aep6, aep5 }
 4a8:	e9c39a887f 	{ ae_lp16x2f.c	aep0, a8, a7; ae_mulp24s.lh	aeq2, aep0, aep6 }
 4ad:	058724        	ae_mulaap24s.hh.ll	aeq2, aep0, aep7
 4b0:	df95da887f 	{ ae_lp16x2f.c	aep0, a8, a7; ae_selp24.lh	aep7, aep5, aep4 }
 4b5:	058724        	ae_mulaap24s.hh.ll	aeq2, aep0, aep7
 4b8:	df71da887f 	{ ae_lp16x2f.c	aep0, a8, a7; ae_selp24.lh	aep7, aep4, aep3 }
 4bd:	058724        	ae_mulaap24s.hh.ll	aeq2, aep0, aep7
 4c0:	df4dda887f 	{ ae_lp16x2f.c	aep0, a8, a7; ae_selp24.lh	aep7, aep3, aep2 }
 4c5:	058724        	ae_mulaap24s.hh.ll	aeq2, aep0, aep7
 4c8:	df29da887f 	{ ae_lp16x2f.c	aep0, a8, a7; ae_selp24.lh	aep7, aep2, aep1 }
 4cd:	e3e0b0711f 	{ ae_lp24x2.i	aep7, a1, 8; ae_mulaap24s.hh.ll	aeq2, aep0, aep7 }
 4d2:	dfe5da887f 	{ ae_lp16x2f.c	aep0, a8, a7; ae_selp24.lh	aep7, aep1, aep7 }
 4d7:	e3e0b0711f 	{ ae_lp24x2.i	aep7, a1, 8; ae_mulaap24s.hh.ll	aeq2, aep0, aep7 }
 4dc:	fc24d9089f 	{ ae_lp16f.c	aep0, a8, a9; nop }
 4e1:	028714        	ae_mulap24s.hl	aeq2, aep0, aep7
 4e4:	35a2b4        	ae_sraiq56	aeq2, aeq2, 10
 4e7:	fa8aacf51f 	{ ae_lp16f.iu	aep7, a5, 2; ae_minq56s	aeq2, aeq2, aeq1 }
 4ec:	fae322f89f 	{ ae_sp16f.l.c	aep7, a8, a9; ae_maxq56s	aeq2, aeq2, aeq0 }
 4f1:	632104        	ae_sq56s.i	aeq2, a1, 0
 4f4:	500104        	ae_lp24f.i	aep0, a1, 0
 4f7:	420314        	ae_sp16f.l.i	aep0, a3, 2
 4fa:	540114        	ae_lp24x2.i	aep0, a1, 8

000004fd <cvsd_src_up_sampling_mono_process_1+0x18d>:
 4fd:	221b           	addi.n	a2, a2, 1

000004ff <cvsd_src_up_sampling_mono_process_1+0x18f>:
 4ff:	c489           	s32i.n	a8, a4, 48
 501:	f01d           	retw.n

00000503 <cvsd_src_up_sampling_mono_process_1+0x193>:
 503:	c489           	s32i.n	a8, a4, 48
 505:	f01d           	retw.n

00000507 <cvsd_src_up_sampling_mono_process_1+0x197>:
	...

00000508 <cvsd_src_up_sampling_mono_process_2>:
 508:	006136        	entry	a1, 48
 50b:	049d           	mov.n	a9, a4
 50d:	fed081        	l32r	a8, 50 (b90 <cvsd_src_up_sampling_coef_2>)
 510:	e77c           	movi.n	a7, -2
 512:	02ad           	mov.n	a10, a2
 514:	26c4b2        	addi	a11, a4, 38
 517:	20c4c2        	addi	a12, a4, 32
 51a:	f3f6c0        	wur.ae_cbegin0	a12
 51d:	f3f7b0        	wur.ae_cend0	a11
 520:	d428           	l32i.n	a2, a4, 52
 522:	106570        	and	a6, a5, a7
 525:	a03630        	addx4	a3, a6, a3
 528:	441804        	ae_lp16x2f.i	aep1, a8, 0
 52b:	442814        	ae_lp16x2f.i	aep2, a8, 4
 52e:	690577        	bnone	a5, a7, 59b <cvsd_src_up_sampling_mono_process_2+0x93>
 531:	fcc332        	addi	a3, a3, -4
 534:	feca42        	addi	a4, a10, -2
 537:	250c           	movi.n	a5, 2
 539:	fec4d1        	l32r	a13, 4c (ffff8000 <_end+0xffff742c>)
 53c:	fec3e1        	l32r	a14, 48 (7fff <_end+0x742b>)
 53f:	364e34        	ae_cvtq48a32s	aeq1, a14
 542:	368d34        	ae_cvtq48a32s	aeq2, a13
 545:	4e9676        	loopnez	a6, 597 <cvsd_src_up_sampling_mono_process_2+0x8f>
 548:	fc24d9025f 	{ ae_lp16f.c	aep0, a2, a5; nop }
 54d:	e84299025f 	{ ae_lp16f.c	aep0, a2, a5; ae_mulp24s.hh	aeq0, aep0, aep2 }
 552:	e62219025f 	{ ae_lp16f.c	aep0, a2, a5; ae_mulap24s.hl	aeq0, aep0, aep1 }
 557:	020104        	ae_mulap24s.hh	aeq0, aep0, aep1
 55a:	350174        	ae_sraiq56	aeq0, aeq0, 5
 55d:	0114c4        	ae_minq56s	aeq0, aeq0, aeq1
 560:	0120c4        	ae_maxq56s	aeq0, aeq0, aeq2
 563:	630104        	ae_sq56s.i	aeq0, a1, 0
 566:	500104        	ae_lp24f.i	aep0, a1, 0
 569:	428324        	ae_sp16f.l.iu	aep0, a3, 4
 56c:	fc24d9025f 	{ ae_lp16f.c	aep0, a2, a5; nop }
 571:	e82299025f 	{ ae_lp16f.c	aep0, a2, a5; ae_mulp24s.hh	aeq0, aep0, aep1 }
 576:	e62219025f 	{ ae_lp16f.c	aep0, a2, a5; ae_mulap24s.hl	aeq0, aep0, aep1 }
 57b:	e4422c841f 	{ ae_lp16f.iu	aep0, a4, 2; ae_mulap24s.hh	aeq0, aep0, aep2 }
 580:	fc24e2825f 	{ ae_sp16f.l.c	aep0, a2, a5; nop }
 585:	350174        	ae_sraiq56	aeq0, aeq0, 5
 588:	0114c4        	ae_minq56s	aeq0, aeq0, aeq1
 58b:	0120c4        	ae_maxq56s	aeq0, aeq0, aeq2
 58e:	630104        	ae_sq56s.i	aeq0, a1, 0
 591:	500104        	ae_lp24f.i	aep0, a1, 0
 594:	420314        	ae_sp16f.l.i	aep0, a3, 2

00000597 <cvsd_src_up_sampling_mono_process_2+0x8f>:
 597:	d929           	s32i.n	a2, a9, 52

00000599 <cvsd_src_up_sampling_mono_process_2+0x91>:
 599:	f01d           	retw.n

0000059b <cvsd_src_up_sampling_mono_process_2+0x93>:
 59b:	d429           	s32i.n	a2, a4, 52
 59d:	f01d           	retw.n

0000059f <cvsd_src_up_sampling_mono_process_2+0x97>:
	...

000005a0 <cvsd_src_up_sampling_mono_process_3>:
 5a0:	006136        	entry	a1, 48
 5a3:	049d           	mov.n	a9, a4
 5a5:	feaa81        	l32r	a8, 50 (b90 <cvsd_src_up_sampling_coef_2>)
 5a8:	e77c           	movi.n	a7, -2
 5aa:	2ec4a2        	addi	a10, a4, 46
 5ad:	28c4b2        	addi	a11, a4, 40
 5b0:	f3f6b0        	wur.ae_cbegin0	a11
 5b3:	f3f7a0        	wur.ae_cend0	a10
 5b6:	106570        	and	a6, a5, a7
 5b9:	441804        	ae_lp16x2f.i	aep1, a8, 0
 5bc:	442814        	ae_lp16x2f.i	aep2, a8, 4
 5bf:	904620        	addx2	a4, a6, a2
 5c2:	0e2922        	l32i	a2, a9, 56
 5c5:	6a0577        	bnone	a5, a7, 633 <cvsd_src_up_sampling_mono_process_3+0x93>
 5c8:	fcc332        	addi	a3, a3, -4
 5cb:	fec442        	addi	a4, a4, -2
 5ce:	02a052        	movi	a5, 2
 5d1:	fe9ec1        	l32r	a12, 4c (ffff8000 <_end+0xffff742c>)
 5d4:	fe9dd1        	l32r	a13, 48 (7fff <_end+0x742b>)
 5d7:	364d34        	ae_cvtq48a32s	aeq1, a13
 5da:	368c34        	ae_cvtq48a32s	aeq2, a12
 5dd:	4e9676        	loopnez	a6, 62f <cvsd_src_up_sampling_mono_process_3+0x8f>
 5e0:	fc24d9025f 	{ ae_lp16f.c	aep0, a2, a5; nop }
 5e5:	e84299025f 	{ ae_lp16f.c	aep0, a2, a5; ae_mulp24s.hh	aeq0, aep0, aep2 }
 5ea:	e62219025f 	{ ae_lp16f.c	aep0, a2, a5; ae_mulap24s.hl	aeq0, aep0, aep1 }
 5ef:	020104        	ae_mulap24s.hh	aeq0, aep0, aep1
 5f2:	350174        	ae_sraiq56	aeq0, aeq0, 5
 5f5:	0114c4        	ae_minq56s	aeq0, aeq0, aeq1
 5f8:	0120c4        	ae_maxq56s	aeq0, aeq0, aeq2
 5fb:	630104        	ae_sq56s.i	aeq0, a1, 0
 5fe:	500104        	ae_lp24f.i	aep0, a1, 0
 601:	428324        	ae_sp16f.l.iu	aep0, a3, 4
 604:	fc24d9025f 	{ ae_lp16f.c	aep0, a2, a5; nop }
 609:	e82299025f 	{ ae_lp16f.c	aep0, a2, a5; ae_mulp24s.hh	aeq0, aep0, aep1 }
 60e:	e62219025f 	{ ae_lp16f.c	aep0, a2, a5; ae_mulap24s.hl	aeq0, aep0, aep1 }
 613:	e4422c841f 	{ ae_lp16f.iu	aep0, a4, 2; ae_mulap24s.hh	aeq0, aep0, aep2 }
 618:	fc24e2825f 	{ ae_sp16f.l.c	aep0, a2, a5; nop }
 61d:	350174        	ae_sraiq56	aeq0, aeq0, 5
 620:	0114c4        	ae_minq56s	aeq0, aeq0, aeq1
 623:	0120c4        	ae_maxq56s	aeq0, aeq0, aeq2
 626:	630104        	ae_sq56s.i	aeq0, a1, 0
 629:	500104        	ae_lp24f.i	aep0, a1, 0
 62c:	420314        	ae_sp16f.l.i	aep0, a3, 2

0000062f <cvsd_src_up_sampling_mono_process_3+0x8f>:
 62f:	e929           	s32i.n	a2, a9, 56
 631:	f01d           	retw.n

00000633 <cvsd_src_up_sampling_mono_process_3+0x93>:
 633:	e929           	s32i.n	a2, a9, 56
 635:	f01d           	retw.n

00000637 <cvsd_src_up_sampling_mono_process_3+0x97>:
	...

00000638 <cvsd_src_dn_sampling_mono_process_1>:
 638:	006136        	entry	a1, 48
 63b:	470c           	movi.n	a7, 4
 63d:	280c           	movi.n	a8, 2
 63f:	02ed           	mov.n	a14, a2
 641:	f3f640        	wur.ae_cbegin0	a4
 644:	fe8461        	l32r	a6, 54 (b10 <cvsd_src_down_sampling_coef_1>)
 647:	03dd           	mov.n	a13, a3
 649:	05cd           	mov.n	a12, a5
 64b:	04bd           	mov.n	a11, a4
 64d:	122bf2        	l32i	a15, a11, 72
 650:	4191c0        	srli	a9, a12, 1
 653:	fe7e31        	l32r	a3, 4c (ffff8000 <_end+0xffff742c>)
 656:	14c452        	addi	a5, a4, 20
 659:	f3f750        	wur.ae_cend0	a5
 65c:	368334        	ae_cvtq48a32s	aeq2, a3
 65f:	fecd52        	addi	a5, a13, -2
 662:	fcc232        	addi	a3, a2, -4
 665:	441604        	ae_lp16x2f.i	aep1, a6, 0
 668:	442614        	ae_lp16x2f.i	aep2, a6, 4
 66b:	fe7741        	l32r	a4, 48 (7fff <_end+0x742b>)
 66e:	443624        	ae_lp16x2f.i	aep3, a6, 8
 671:	364434        	ae_cvtq48a32s	aeq1, a4
 674:	fec242        	addi	a4, a2, -2
 677:	0ca062        	movi	a6, 12
 67a:	112b22        	l32i	a2, a11, 68
 67d:	2e9976        	loopnez	a9, 6af <cvsd_src_dn_sampling_mono_process_1+0x77>
 680:	fc24da827f 	{ ae_lp16x2f.c	aep0, a2, a7; nop }
 685:	f0239a827f 	{ ae_lp16x2f.c	aep0, a2, a7; ae_mulzaap24s.hh.ll	aeq0, aep0, aep1 }
 68a:	e2409a826f 	{ ae_lp16x2f.c	aep0, a2, a6; ae_mulaap24s.hh.ll	aeq0, aep0, aep2 }
 68f:	e260ac832f 	{ ae_lp16f.iu	aep0, a3, 4; ae_mulaap24s.hh.ll	aeq0, aep0, aep3 }
 694:	fc24e2828f 	{ ae_sp16f.l.c	aep0, a2, a8; nop }
 699:	3501b4        	ae_sraiq56	aeq0, aeq0, 6
 69c:	f80aac842f 	{ ae_lp16f.iu	aep0, a4, 4; ae_minq56s	aeq0, aeq0, aeq1 }
 6a1:	f87322828f 	{ ae_sp16f.l.c	aep0, a2, a8; ae_maxq56s	aeq0, aeq0, aeq2 }
 6a6:	630104        	ae_sq56s.i	aeq0, a1, 0
 6a9:	500104        	ae_lp24f.i	aep0, a1, 0
 6ac:	428514        	ae_sp16f.l.iu	aep0, a5, 2

000006af <cvsd_src_dn_sampling_mono_process_1+0x77>:
 6af:	fecd52        	addi	a5, a13, -2
 6b2:	fece42        	addi	a4, a14, -2
 6b5:	4192c0        	srli	a9, a12, 2
 6b8:	116b22        	s32i	a2, a11, 68
 6bb:	18cba2        	addi	a10, a11, 24
 6be:	fe6231        	l32r	a3, 48 (7fff <_end+0x742b>)
 6c1:	364334        	ae_cvtq48a32s	aeq1, a3
 6c4:	f3f6a0        	wur.ae_cbegin0	a10
 6c7:	fe6121        	l32r	a2, 4c (ffff8000 <_end+0xffff742c>)
 6ca:	fe63a1        	l32r	a10, 58 (b30 <cvsd_src_down_sampling_coef_2>)
 6cd:	368234        	ae_cvtq48a32s	aeq2, a2
 6d0:	fcce32        	addi	a3, a14, -4
 6d3:	2ccb22        	addi	a2, a11, 44
 6d6:	441a04        	ae_lp16x2f.i	aep1, a10, 0
 6d9:	442a14        	ae_lp16x2f.i	aep2, a10, 4
 6dc:	443a24        	ae_lp16x2f.i	aep3, a10, 8
 6df:	f3f720        	wur.ae_cend0	a2
 6e2:	20a330        	or	a10, a3, a3
 6e5:	2e9976        	loopnez	a9, 717 <cvsd_src_dn_sampling_mono_process_1+0xdf>
 6e8:	fc24da8f7f 	{ ae_lp16x2f.c	aep0, a15, a7; nop }

000006ed <cvsd_src_dn_sampling_mono_process_1+0xb5>:
 6ed:	f0239a8f7f 	{ ae_lp16x2f.c	aep0, a15, a7; ae_mulzaap24s.hh.ll	aeq0, aep0, aep1 }
 6f2:	e2409a8f6f 	{ ae_lp16x2f.c	aep0, a15, a6; ae_mulaap24s.hh.ll	aeq0, aep0, aep2 }
 6f7:	e260ac8a2f 	{ ae_lp16f.iu	aep0, a10, 4; ae_mulaap24s.hh.ll	aeq0, aep0, aep3 }
 6fc:	fc24e28f8f 	{ ae_sp16f.l.c	aep0, a15, a8; nop }
 701:	3501b4        	ae_sraiq56	aeq0, aeq0, 6
 704:	f80aac842f 	{ ae_lp16f.iu	aep0, a4, 4; ae_minq56s	aeq0, aeq0, aeq1 }
 709:	f873228f8f 	{ ae_sp16f.l.c	aep0, a15, a8; ae_maxq56s	aeq0, aeq0, aeq2 }
 70e:	630104        	ae_sq56s.i	aeq0, a1, 0
 711:	500104        	ae_lp24f.i	aep0, a1, 0
 714:	428514        	ae_sp16f.l.iu	aep0, a5, 2

00000717 <cvsd_src_dn_sampling_mono_process_1+0xdf>:
 717:	126bf2        	s32i	a15, a11, 72
 71a:	f01d           	retw.n

0000071c <cvsd_src_dn_sampling_mono_process_2>:
 71c:	006136        	entry	a1, 48
 71f:	470c           	movi.n	a7, 4
 721:	fe4ef1        	l32r	a15, 5c (b50 <cvsd_src_down_sampling_coef_3>)
 724:	30c462        	addi	a6, a4, 48
 727:	418150        	srli	a8, a5, 1
 72a:	04ad           	mov.n	a10, a4
 72c:	fe48d1        	l32r	a13, 4c (ffff8000 <_end+0xffff742c>)
 72f:	fe46e1        	l32r	a14, 48 (7fff <_end+0x742b>)
 732:	364e34        	ae_cvtq48a32s	aeq1, a14
 735:	368d34        	ae_cvtq48a32s	aeq2, a13
 738:	132a92        	l32i	a9, a10, 76
 73b:	fec352        	addi	a5, a3, -2
 73e:	f3f660        	wur.ae_cbegin0	a6
 741:	fcc232        	addi	a3, a2, -4
 744:	260c           	movi.n	a6, 2
 746:	44c442        	addi	a4, a4, 68
 749:	441f04        	ae_lp16x2f.i	aep1, a15, 0
 74c:	442f14        	ae_lp16x2f.i	aep2, a15, 4
 74f:	443f24        	ae_lp16x2f.i	aep3, a15, 8
 752:	444f34        	ae_lp16x2f.i	aep4, a15, 12
 755:	f3f740        	wur.ae_cend0	a4
 758:	445f44        	ae_lp16x2f.i	aep5, a15, 16
 75b:	fec242        	addi	a4, a2, -2
 75e:	389876        	loopnez	a8, 79a <cvsd_src_dn_sampling_mono_process_2+0x7e>
 761:	fc24da897f 	{ ae_lp16x2f.c	aep0, a9, a7; nop }
 766:	f0239a897f 	{ ae_lp16x2f.c	aep0, a9, a7; ae_mulzaap24s.hh.ll	aeq0, aep0, aep1 }
 76b:	e2409a897f 	{ ae_lp16x2f.c	aep0, a9, a7; ae_mulaap24s.hh.ll	aeq0, aep0, aep2 }
 770:	e2609a897f 	{ ae_lp16x2f.c	aep0, a9, a7; ae_mulaap24s.hh.ll	aeq0, aep0, aep3 }
 775:	e2809a897f 	{ ae_lp16x2f.c	aep0, a9, a7; ae_mulaap24s.hh.ll	aeq0, aep0, aep4 }
 77a:	e2a0ac832f 	{ ae_lp16f.iu	aep0, a3, 4; ae_mulaap24s.hh.ll	aeq0, aep0, aep5 }
 77f:	fc24e2896f 	{ ae_sp16f.l.c	aep0, a9, a6; nop }
 784:	3502f4        	ae_sraiq56	aeq0, aeq0, 11
 787:	f80aac842f 	{ ae_lp16f.iu	aep0, a4, 4; ae_minq56s	aeq0, aeq0, aeq1 }
 78c:	f87322896f 	{ ae_sp16f.l.c	aep0, a9, a6; ae_maxq56s	aeq0, aeq0, aeq2 }
 791:	630104        	ae_sq56s.i	aeq0, a1, 0
 794:	500104        	ae_lp24f.i	aep0, a1, 0
 797:	428514        	ae_sp16f.l.iu	aep0, a5, 2

0000079a <cvsd_src_dn_sampling_mono_process_2+0x7e>:
 79a:	136a92        	s32i	a9, a10, 76
 79d:	f01d           	retw.n

0000079f <cvsd_src_dn_sampling_mono_process_2+0x83>:
	...

000007a0 <cvsd_src_up_sampling_init>:
 7a0:	004136        	entry	a1, 32
 7a3:	03ad           	mov.n	a10, a3
 7a5:	fe2e81        	l32r	a8, 60 (338 <f_memset>)
 7a8:	0b0c           	movi.n	a11, 0
 7aa:	40a0c2        	movi	a12, 64
 7ad:	0008e0        	callx8	a8
 7b0:	c339           	s32i.n	a3, a3, 48
 7b2:	20c392        	addi	a9, a3, 32
 7b5:	28c3a2        	addi	a10, a3, 40
 7b8:	e3a9           	s32i.n	a10, a3, 56
 7ba:	d399           	s32i.n	a9, a3, 52
 7bc:	0239           	s32i.n	a3, a2, 0
 7be:	020c           	movi.n	a2, 0
 7c0:	f01d           	retw.n

000007c2 <cvsd_src_up_sampling_init+0x22>:
	...

000007c4 <cvsd_src_dn_sampling_init>:
 7c4:	004136        	entry	a1, 32
 7c7:	03ad           	mov.n	a10, a3
 7c9:	fe2581        	l32r	a8, 60 (338 <f_memset>)
 7cc:	0b0c           	movi.n	a11, 0
 7ce:	0c5c           	movi.n	a12, 80
 7d0:	0008e0        	callx8	a8
 7d3:	116332        	s32i	a3, a3, 68
 7d6:	18c392        	addi	a9, a3, 24
 7d9:	30c3a2        	addi	a10, a3, 48
 7dc:	1363a2        	s32i	a10, a3, 76
 7df:	126392        	s32i	a9, a3, 72
 7e2:	0239           	s32i.n	a3, a2, 0
 7e4:	020c           	movi.n	a2, 0
 7e6:	f01d           	retw.n

000007e8 <cvsd_src_up_sampling_process>:
 7e8:	004136        	entry	a1, 32
 7eb:	03ad           	mov.n	a10, a3
 7ed:	04bd           	mov.n	a11, a4
 7ef:	fe1d81        	l32r	a8, 64 (370 <cvsd_src_up_sampling_mono_process_1>)
 7f2:	20c220        	or	a12, a2, a2
 7f5:	41d160        	srli	a13, a6, 1
 7f8:	0008e0        	callx8	a8
 7fb:	04ad           	mov.n	a10, a4
 7fd:	05bd           	mov.n	a11, a5
 7ff:	fe1a81        	l32r	a8, 68 (508 <cvsd_src_up_sampling_mono_process_2>)
 802:	02cd           	mov.n	a12, a2
 804:	06dd           	mov.n	a13, a6
 806:	0008e0        	callx8	a8
 809:	05bd           	mov.n	a11, a5
 80b:	02cd           	mov.n	a12, a2
 80d:	fe1781        	l32r	a8, 6c (5a0 <cvsd_src_up_sampling_mono_process_3>)
 810:	05ad           	mov.n	a10, a5
 812:	11d6f0        	slli	a13, a6, 1
 815:	0008e0        	callx8	a8
 818:	020c           	movi.n	a2, 0
 81a:	f01d           	retw.n

0000081c <cvsd_src_dn_sampling_process>:
 81c:	004136        	entry	a1, 32
 81f:	03ad           	mov.n	a10, a3
 821:	03bd           	mov.n	a11, a3
 823:	fe1381        	l32r	a8, 70 (638 <cvsd_src_dn_sampling_mono_process_1>)
 826:	02cd           	mov.n	a12, a2
 828:	41d150        	srli	a13, a5, 1
 82b:	0008e0        	callx8	a8
 82e:	02cd           	mov.n	a12, a2
 830:	04bd           	mov.n	a11, a4
 832:	fe1081        	l32r	a8, 74 (71c <cvsd_src_dn_sampling_mono_process_2>)
 835:	03ad           	mov.n	a10, a3
 837:	41d350        	srli	a13, a5, 3
 83a:	0008e0        	callx8	a8
 83d:	020c           	movi.n	a2, 0
 83f:	f01d           	retw.n

00000841 <cvsd_src_dn_sampling_process+0x25>:
 841:	000000                                        ...

00000844 <_fini>:
 844:	008136        	entry	a1, 64

00000847 <_fini+0x3>:
 847:	f01d           	retw.n
