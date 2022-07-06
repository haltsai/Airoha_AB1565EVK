
Build/lib/pisplit_cvsd_dec_position_independent.splitload.lib:     file format elf32-xtensa-le


Disassembly of section .text:

00000000 <_init-0x78>:
   0:	00000bc0 00000ad0 000002c0 000002f0     ................
  10:	00000bb0 000000a0 001fffff 00002800     .............(..
  20:	00140000 7c000000 ffe00001 7fe00000     .......|........
  30:	00000b90 000000c8 000007bc 000001c4     ................
  40:	00000814 00000b60 00007fff ffff8000     ....`...........
  50:	00000b80 00000b00 00000b20 00000b40     ........ ...@...
  60:	00000330 00000368 00000500 00000598     0...h...........
  70:	00000630 00000714                       0.......

00000078 <_init>:
  78:	008136        	entry	a1, 64
  7b:	f01d           	retw.n

0000007d <_init+0x5>:
  7d:	000000                                        ...

00000080 <_start>:
  80:	004136        	entry	a1, 32
  83:	ffe0a1        	l32r	a10, 4 (ad0 <_DYNAMIC+0x284>)
  86:	ffe0b1        	l32r	a11, 8 (2c0 <_CVSD_Decoder_Init>)
  89:	ffdd91        	l32r	a9, 0 (bc0 <__bss_start>)
  8c:	002282        	l32i	a8, a2, 0
  8f:	ffdfc1        	l32r	a12, c (2f0 <_CVSD_Decoder>)
  92:	006982        	s32i	a8, a9, 0
  95:	0008e0        	callx8	a8
  98:	ffde21        	l32r	a2, 10 (bb0 <export_parameter_array>)
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
  df:	ffd1f1        	l32r	a15, 24 (7c000000 <_end+0x7bfff43c>)
  e2:	ffd1e1        	l32r	a14, 28 (ffe00001 <_end+0xffdff43d>)
  e5:	0169           	s32i.n	a6, a1, 0
  e7:	1159           	s32i.n	a5, a1, 4
  e9:	0698           	l32i.n	a9, a6, 0
  eb:	0478           	l32i.n	a7, a4, 0
  ed:	2129           	s32i.n	a2, a1, 8
  ef:	0a0c           	movi.n	a10, 0
  f1:	ffc921        	l32r	a2, 18 (1fffff <_end+0x1ff43b>)
  f4:	0326a4        	wur.ae_bitptr	a10
  f7:	f3f1a0        	wur.ae_bithead	a10
  fa:	417570        	srli	a7, a7, 5
  fd:	637790        	minu	a7, a7, a9
 100:	21a8           	l32i.n	a10, a1, 8
 102:	1197b0        	slli	a9, a7, 5
 105:	0499           	s32i.n	a9, a4, 0
 107:	fec542        	addi	a4, a5, -2
 10a:	006672        	s32i	a7, a6, 0
 10d:	ffc351        	l32r	a5, 1c (2800 <_end+0x1c3c>)
 110:	ffc461        	l32r	a6, 20 (140000 <_end+0x13f43c>)
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
 144:	ffba71        	l32r	a7, 2c (7fe00000 <_end+0x7fdff43c>)
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
 16a:	ffb171        	l32r	a7, 30 (b90 <cvsd_BitReverseTable16>)
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
 1c9:	ff9971        	l32r	a7, 30 (b90 <cvsd_BitReverseTable16>)
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
 228:	ff7ed1        	l32r	a13, 20 (140000 <_end+0x13f43c>)
 22b:	ff7ce1        	l32r	a14, 1c (2800 <_end+0x1c3c>)
 22e:	ff7af1        	l32r	a15, 18 (1fffff <_end+0x1ff43b>)
 231:	ff7e21        	l32r	a2, 2c (7fe00000 <_end+0x7fdff43c>)
 234:	ff7c51        	l32r	a5, 24 (7c000000 <_end+0x7bfff43c>)
 237:	0178           	l32i.n	a7, a1, 0
 239:	fecbb2        	addi	a11, a11, -2
 23c:	00a082        	movi	a8, 0
 23f:	f3f180        	wur.ae_bithead	a8
 242:	032684        	wur.ae_bitptr	a8
 245:	000bf4        	ae_dbi	a11, 16
 248:	ff7831        	l32r	a3, 28 (ffe00001 <_end+0xffdff43d>)
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

000002c0 <_CVSD_Decoder_Init>:
 2c0:	004136        	entry	a1, 32
 2c3:	07d2a2        	addmi	a10, a2, 0x700
 2c6:	c83c           	movi.n	a8, 60
 2c8:	725a82        	s16i	a8, a10, 228
 2cb:	161326        	beqi	a3, 1, 2e5 <_CVSD_Decoder_Init+0x25>

000002ce <_CVSD_Decoder_Init+0xe>:
 2ce:	ff5981        	l32r	a8, 34 (c8 <cvsd_decode_init>)
 2d1:	02ad           	mov.n	a10, a2
 2d3:	b28b           	addi.n	a11, a2, 8
 2d5:	0008e0        	callx8	a8
 2d8:	ff5881        	l32r	a8, 38 (7bc <cvsd_src_dn_sampling_init>)
 2db:	a24b           	addi.n	a10, a2, 4
 2dd:	14c2b2        	addi	a11, a2, 20
 2e0:	0008e0        	callx8	a8
 2e3:	f01d           	retw.n

000002e5 <_CVSD_Decoder_Init+0x25>:
 2e5:	78a092        	movi	a9, 120
 2e8:	725a92        	s16i	a9, a10, 228
 2eb:	fff7c6        	j	2ce <_CVSD_Decoder_Init+0xe>

000002ee <_CVSD_Decoder_Init+0x2e>:
	...

000002f0 <_CVSD_Decoder>:
 2f0:	006136        	entry	a1, 48
 2f3:	02a8           	l32i.n	a10, a2, 0
 2f5:	01cd           	mov.n	a12, a1
 2f7:	e14b           	addi.n	a14, a1, 4
 2f9:	ff5081        	l32r	a8, 3c (1c4 <cvsd_decode_process>)
 2fc:	03bd           	mov.n	a11, a3
 2fe:	07d292        	addmi	a9, a2, 0x700
 301:	64c252        	addi	a5, a2, 100
 304:	e8c932        	addi	a3, a9, -24
 307:	729992        	l16si	a9, a9, 228
 30a:	05dd           	mov.n	a13, a5
 30c:	0199           	s32i.n	a9, a1, 0
 30e:	1199c0        	slli	a9, a9, 4
 311:	1199           	s32i.n	a9, a1, 4
 313:	0008e0        	callx8	a8
 316:	04cd           	mov.n	a12, a4
 318:	05bd           	mov.n	a11, a5
 31a:	7e93d2        	l16si	a13, a3, 252
 31d:	ff4881        	l32r	a8, 40 (814 <cvsd_src_dn_sampling_process>)
 320:	12a8           	l32i.n	a10, a2, 4
 322:	11ddc0        	slli	a13, a13, 4
 325:	0008e0        	callx8	a8
 328:	7e9322        	l16si	a2, a3, 252
 32b:	1122f0        	slli	a2, a2, 1
 32e:	f01d           	retw.n

00000330 <f_memset>:
 330:	004136        	entry	a1, 32
 333:	2c14a6        	blti	a4, 1, 363 <f_memset+0x33>
 336:	245040        	extui	a5, a4, 0, 3
 339:	046d           	mov.n	a6, a4
 33b:	214360        	srai	a4, a6, 3
 33e:	04a576        	loopgtz	a5, 346 <f_memset+0x16>
 341:	004232        	s8i	a3, a2, 0
 344:	221b           	addi.n	a2, a2, 1
 346:	19a476        	loopgtz	a4, 363 <f_memset+0x33>
 349:	004232        	s8i	a3, a2, 0
 34c:	014232        	s8i	a3, a2, 1
 34f:	024232        	s8i	a3, a2, 2
 352:	034232        	s8i	a3, a2, 3
 355:	044232        	s8i	a3, a2, 4
 358:	054232        	s8i	a3, a2, 5
 35b:	064232        	s8i	a3, a2, 6
 35e:	074232        	s8i	a3, a2, 7
 361:	228b           	addi.n	a2, a2, 8

00000363 <f_memset+0x33>:
 363:	f01d           	retw.n

00000365 <f_memset+0x35>:
 365:	000000                                        ...

00000368 <cvsd_src_up_sampling_mono_process_1>:
 368:	006136        	entry	a1, 48
 36b:	f3f640        	wur.ae_cbegin0	a4
 36e:	ff3571        	l32r	a7, 44 (b60 <cvsd_src_up_sampling_coef_1>)
 371:	e67c           	movi.n	a6, -2
 373:	1cc482        	addi	a8, a4, 28
 376:	f3f780        	wur.ae_cend0	a8
 379:	10a560        	and	a10, a5, a6
 37c:	c488           	l32i.n	a8, a4, 48
 37e:	446704        	ae_lp16x2f.i	aep6, a7, 0
 381:	445714        	ae_lp16x2f.i	aep5, a7, 4
 384:	444724        	ae_lp16x2f.i	aep4, a7, 8
 387:	443734        	ae_lp16x2f.i	aep3, a7, 12
 38a:	442744        	ae_lp16x2f.i	aep2, a7, 16
 38d:	441754        	ae_lp16x2f.i	aep1, a7, 20
 390:	440764        	ae_lp16x2f.i	aep0, a7, 24
 393:	028567        	bany	a5, a6, 399 <cvsd_src_up_sampling_mono_process_1+0x31>
 396:	005846        	j	4fb <cvsd_src_up_sampling_mono_process_1+0x193>
 399:	470c           	movi.n	a7, 4
 39b:	fcc332        	addi	a3, a3, -4
 39e:	fec252        	addi	a5, a2, -2
 3a1:	ff2a91        	l32r	a9, 4c (ffff8000 <_end+0xffff743c>)
 3a4:	ff29b1        	l32r	a11, 48 (7fff <_end+0x743b>)
 3a7:	364b34        	ae_cvtq48a32s	aeq1, a11
 3aa:	360934        	ae_cvtq48a32s	aeq0, a9
 3ad:	020c           	movi.n	a2, 0
 3af:	02a092        	movi	a9, 2
 3b2:	141a16        	beqz	a10, 4f7 <cvsd_src_up_sampling_mono_process_1+0x18f>
 3b5:	179a76        	loopnez	a10, 3d0 <cvsd_src_up_sampling_mono_process_1+0x68>
 3b8:	0301a0        	rsr.lend	a10
 3bb:	1300a0        	wsr.lbeg	a10
 3be:	27caa2        	addi	a10, a10, 39
 3c1:	01daa2        	addmi	a10, a10, 0x100
 3c4:	1301a0        	wsr.lend	a10
 3c7:	002000        	isync
 3ca:	0302a0        	rsr.lcount	a10
 3cd:	01caa2        	addi	a10, a10, 1
 3d0:	76e207        	bbsi	a2, 0, 44a <cvsd_src_up_sampling_mono_process_1+0xe2>
 3d3:	fc24daf87f 	{ ae_lp16x2f.c	aep7, a8, a7; nop }
 3d8:	f31f9af87f 	{ ae_lp16x2f.c	aep7, a8, a7; ae_mulzaap24s.hl.lh	aeq2, aep7, aep0 }
 3dd:	e33d9af87f 	{ ae_lp16x2f.c	aep7, a8, a7; ae_mulaap24s.hl.lh	aeq2, aep7, aep1 }
 3e2:	e35d9af87f 	{ ae_lp16x2f.c	aep7, a8, a7; ae_mulaap24s.hl.lh	aeq2, aep7, aep2 }
 3e7:	e37d9af87f 	{ ae_lp16x2f.c	aep7, a8, a7; ae_mulaap24s.hl.lh	aeq2, aep7, aep3 }
 3ec:	e39d9af87f 	{ ae_lp16x2f.c	aep7, a8, a7; ae_mulaap24s.hl.lh	aeq2, aep7, aep4 }
 3f1:	e3bd9af87f 	{ ae_lp16x2f.c	aep7, a8, a7; ae_mulaap24s.hl.lh	aeq2, aep7, aep5 }
 3f6:	05f634        	ae_mulaap24s.hl.lh	aeq2, aep7, aep6
 3f9:	35a2b4        	ae_sraiq56	aeq2, aeq2, 10
 3fc:	0196c4        	ae_minq56s	aeq2, aeq2, aeq1
 3ff:	0182c4        	ae_maxq56s	aeq2, aeq2, aeq0
 402:	632104        	ae_sq56s.i	aeq2, a1, 0
 405:	507104        	ae_lp24f.i	aep7, a1, 0
 408:	42f324        	ae_sp16f.l.iu	aep7, a3, 4
 40b:	fc24daf87f 	{ ae_lp16x2f.c	aep7, a8, a7; nop }
 410:	f1dfdaf87f 	{ ae_lp16x2f.c	aep7, a8, a7; ae_mulzaap24s.hh.ll	aeq3, aep7, aep6 }
 415:	e3bcdaf87f 	{ ae_lp16x2f.c	aep7, a8, a7; ae_mulaap24s.hh.ll	aeq3, aep7, aep5 }
 41a:	e39cdaf87f 	{ ae_lp16x2f.c	aep7, a8, a7; ae_mulaap24s.hh.ll	aeq3, aep7, aep4 }
 41f:	e37cdaf87f 	{ ae_lp16x2f.c	aep7, a8, a7; ae_mulaap24s.hh.ll	aeq3, aep7, aep3 }
 424:	e35cdaf87f 	{ ae_lp16x2f.c	aep7, a8, a7; ae_mulaap24s.hh.ll	aeq3, aep7, aep2 }
 429:	e33cdaf87f 	{ ae_lp16x2f.c	aep7, a8, a7; ae_mulaap24s.hh.ll	aeq3, aep7, aep1 }
 42e:	05f824        	ae_mulaap24s.hh.ll	aeq3, aep7, aep0
 431:	35f2b4        	ae_sraiq56	aeq3, aeq3, 10
 434:	fb8eacf51f 	{ ae_lp16f.iu	aep7, a5, 2; ae_minq56s	aeq3, aeq3, aeq1 }
 439:	fbe722f89f 	{ ae_sp16f.l.c	aep7, a8, a9; ae_maxq56s	aeq3, aeq3, aeq0 }
 43e:	633104        	ae_sq56s.i	aeq3, a1, 0
 441:	507104        	ae_lp24f.i	aep7, a1, 0
 444:	427314        	ae_sp16f.l.i	aep7, a3, 2
 447:	002a86        	j	4f5 <cvsd_src_up_sampling_mono_process_1+0x18d>

0000044a <cvsd_src_up_sampling_mono_process_1+0xe2>:
 44a:	fc24d9789f 	{ ae_lp16f.c	aep7, a8, a9; nop }
 44f:	eb1ff7011f 	{ ae_sp24x2s.i	aep0, a1, 8; ae_mulp24s.ll	aeq3, aep7, aep0 }
 454:	dc209af87f 	{ ae_lp16x2f.c	aep7, a8, a7; ae_selp24.hl	aep0, aep0, aep1 }
 459:	05f824        	ae_mulaap24s.hh.ll	aeq3, aep7, aep0
 45c:	dc449af87f 	{ ae_lp16x2f.c	aep7, a8, a7; ae_selp24.hl	aep0, aep1, aep2 }
 461:	05f824        	ae_mulaap24s.hh.ll	aeq3, aep7, aep0
 464:	dc689af87f 	{ ae_lp16x2f.c	aep7, a8, a7; ae_selp24.hl	aep0, aep2, aep3 }
 469:	05f824        	ae_mulaap24s.hh.ll	aeq3, aep7, aep0
 46c:	dc8c9af87f 	{ ae_lp16x2f.c	aep7, a8, a7; ae_selp24.hl	aep0, aep3, aep4 }
 471:	05f824        	ae_mulaap24s.hh.ll	aeq3, aep7, aep0
 474:	dcb09af87f 	{ ae_lp16x2f.c	aep7, a8, a7; ae_selp24.hl	aep0, aep4, aep5 }
 479:	05f824        	ae_mulaap24s.hh.ll	aeq3, aep7, aep0
 47c:	dcd49af87f 	{ ae_lp16x2f.c	aep7, a8, a7; ae_selp24.hl	aep0, aep5, aep6 }
 481:	e31cd9089f 	{ ae_lp16f.c	aep0, a8, a9; ae_mulaap24s.hh.ll	aeq3, aep7, aep0 }
 486:	028e04        	ae_mulap24s.hh	aeq3, aep0, aep6
 489:	35f2b4        	ae_sraiq56	aeq3, aeq3, 10
 48c:	01d7c4        	ae_minq56s	aeq3, aeq3, aeq1
 48f:	01c3c4        	ae_maxq56s	aeq3, aeq3, aeq0
 492:	633104        	ae_sq56s.i	aeq3, a1, 0
 495:	507104        	ae_lp24f.i	aep7, a1, 0
 498:	42f324        	ae_sp16f.l.iu	aep7, a3, 4
 49b:	dfb9d9089f 	{ ae_lp16f.c	aep0, a8, a9; ae_selp24.lh	aep7, aep6, aep5 }
 4a0:	e9c39a887f 	{ ae_lp16x2f.c	aep0, a8, a7; ae_mulp24s.lh	aeq2, aep0, aep6 }
 4a5:	058724        	ae_mulaap24s.hh.ll	aeq2, aep0, aep7
 4a8:	df95da887f 	{ ae_lp16x2f.c	aep0, a8, a7; ae_selp24.lh	aep7, aep5, aep4 }
 4ad:	058724        	ae_mulaap24s.hh.ll	aeq2, aep0, aep7
 4b0:	df71da887f 	{ ae_lp16x2f.c	aep0, a8, a7; ae_selp24.lh	aep7, aep4, aep3 }
 4b5:	058724        	ae_mulaap24s.hh.ll	aeq2, aep0, aep7
 4b8:	df4dda887f 	{ ae_lp16x2f.c	aep0, a8, a7; ae_selp24.lh	aep7, aep3, aep2 }
 4bd:	058724        	ae_mulaap24s.hh.ll	aeq2, aep0, aep7
 4c0:	df29da887f 	{ ae_lp16x2f.c	aep0, a8, a7; ae_selp24.lh	aep7, aep2, aep1 }
 4c5:	e3e0b0711f 	{ ae_lp24x2.i	aep7, a1, 8; ae_mulaap24s.hh.ll	aeq2, aep0, aep7 }
 4ca:	dfe5da887f 	{ ae_lp16x2f.c	aep0, a8, a7; ae_selp24.lh	aep7, aep1, aep7 }
 4cf:	e3e0b0711f 	{ ae_lp24x2.i	aep7, a1, 8; ae_mulaap24s.hh.ll	aeq2, aep0, aep7 }
 4d4:	fc24d9089f 	{ ae_lp16f.c	aep0, a8, a9; nop }
 4d9:	028714        	ae_mulap24s.hl	aeq2, aep0, aep7
 4dc:	35a2b4        	ae_sraiq56	aeq2, aeq2, 10
 4df:	fa8aacf51f 	{ ae_lp16f.iu	aep7, a5, 2; ae_minq56s	aeq2, aeq2, aeq1 }
 4e4:	fae322f89f 	{ ae_sp16f.l.c	aep7, a8, a9; ae_maxq56s	aeq2, aeq2, aeq0 }
 4e9:	632104        	ae_sq56s.i	aeq2, a1, 0
 4ec:	500104        	ae_lp24f.i	aep0, a1, 0
 4ef:	420314        	ae_sp16f.l.i	aep0, a3, 2
 4f2:	540114        	ae_lp24x2.i	aep0, a1, 8

000004f5 <cvsd_src_up_sampling_mono_process_1+0x18d>:
 4f5:	221b           	addi.n	a2, a2, 1

000004f7 <cvsd_src_up_sampling_mono_process_1+0x18f>:
 4f7:	c489           	s32i.n	a8, a4, 48
 4f9:	f01d           	retw.n

000004fb <cvsd_src_up_sampling_mono_process_1+0x193>:
 4fb:	c489           	s32i.n	a8, a4, 48
 4fd:	f01d           	retw.n

000004ff <cvsd_src_up_sampling_mono_process_1+0x197>:
	...

00000500 <cvsd_src_up_sampling_mono_process_2>:
 500:	006136        	entry	a1, 48
 503:	049d           	mov.n	a9, a4
 505:	fed281        	l32r	a8, 50 (b80 <cvsd_src_up_sampling_coef_2>)
 508:	e77c           	movi.n	a7, -2
 50a:	02ad           	mov.n	a10, a2
 50c:	26c4b2        	addi	a11, a4, 38
 50f:	20c4c2        	addi	a12, a4, 32
 512:	f3f6c0        	wur.ae_cbegin0	a12
 515:	f3f7b0        	wur.ae_cend0	a11
 518:	d428           	l32i.n	a2, a4, 52
 51a:	106570        	and	a6, a5, a7
 51d:	a03630        	addx4	a3, a6, a3
 520:	441804        	ae_lp16x2f.i	aep1, a8, 0
 523:	442814        	ae_lp16x2f.i	aep2, a8, 4
 526:	690577        	bnone	a5, a7, 593 <cvsd_src_up_sampling_mono_process_2+0x93>
 529:	fcc332        	addi	a3, a3, -4
 52c:	feca42        	addi	a4, a10, -2
 52f:	250c           	movi.n	a5, 2
 531:	fec6d1        	l32r	a13, 4c (ffff8000 <_end+0xffff743c>)
 534:	fec5e1        	l32r	a14, 48 (7fff <_end+0x743b>)
 537:	364e34        	ae_cvtq48a32s	aeq1, a14
 53a:	368d34        	ae_cvtq48a32s	aeq2, a13
 53d:	4e9676        	loopnez	a6, 58f <cvsd_src_up_sampling_mono_process_2+0x8f>
 540:	fc24d9025f 	{ ae_lp16f.c	aep0, a2, a5; nop }
 545:	e84299025f 	{ ae_lp16f.c	aep0, a2, a5; ae_mulp24s.hh	aeq0, aep0, aep2 }
 54a:	e62219025f 	{ ae_lp16f.c	aep0, a2, a5; ae_mulap24s.hl	aeq0, aep0, aep1 }
 54f:	020104        	ae_mulap24s.hh	aeq0, aep0, aep1
 552:	350174        	ae_sraiq56	aeq0, aeq0, 5
 555:	0114c4        	ae_minq56s	aeq0, aeq0, aeq1
 558:	0120c4        	ae_maxq56s	aeq0, aeq0, aeq2
 55b:	630104        	ae_sq56s.i	aeq0, a1, 0
 55e:	500104        	ae_lp24f.i	aep0, a1, 0
 561:	428324        	ae_sp16f.l.iu	aep0, a3, 4
 564:	fc24d9025f 	{ ae_lp16f.c	aep0, a2, a5; nop }
 569:	e82299025f 	{ ae_lp16f.c	aep0, a2, a5; ae_mulp24s.hh	aeq0, aep0, aep1 }
 56e:	e62219025f 	{ ae_lp16f.c	aep0, a2, a5; ae_mulap24s.hl	aeq0, aep0, aep1 }
 573:	e4422c841f 	{ ae_lp16f.iu	aep0, a4, 2; ae_mulap24s.hh	aeq0, aep0, aep2 }
 578:	fc24e2825f 	{ ae_sp16f.l.c	aep0, a2, a5; nop }
 57d:	350174        	ae_sraiq56	aeq0, aeq0, 5
 580:	0114c4        	ae_minq56s	aeq0, aeq0, aeq1
 583:	0120c4        	ae_maxq56s	aeq0, aeq0, aeq2
 586:	630104        	ae_sq56s.i	aeq0, a1, 0
 589:	500104        	ae_lp24f.i	aep0, a1, 0
 58c:	420314        	ae_sp16f.l.i	aep0, a3, 2

0000058f <cvsd_src_up_sampling_mono_process_2+0x8f>:
 58f:	d929           	s32i.n	a2, a9, 52

00000591 <cvsd_src_up_sampling_mono_process_2+0x91>:
 591:	f01d           	retw.n

00000593 <cvsd_src_up_sampling_mono_process_2+0x93>:
 593:	d429           	s32i.n	a2, a4, 52
 595:	f01d           	retw.n

00000597 <cvsd_src_up_sampling_mono_process_2+0x97>:
	...

00000598 <cvsd_src_up_sampling_mono_process_3>:
 598:	006136        	entry	a1, 48
 59b:	049d           	mov.n	a9, a4
 59d:	feac81        	l32r	a8, 50 (b80 <cvsd_src_up_sampling_coef_2>)
 5a0:	e77c           	movi.n	a7, -2
 5a2:	2ec4a2        	addi	a10, a4, 46
 5a5:	28c4b2        	addi	a11, a4, 40
 5a8:	f3f6b0        	wur.ae_cbegin0	a11
 5ab:	f3f7a0        	wur.ae_cend0	a10
 5ae:	106570        	and	a6, a5, a7
 5b1:	441804        	ae_lp16x2f.i	aep1, a8, 0
 5b4:	442814        	ae_lp16x2f.i	aep2, a8, 4
 5b7:	904620        	addx2	a4, a6, a2
 5ba:	0e2922        	l32i	a2, a9, 56
 5bd:	6a0577        	bnone	a5, a7, 62b <cvsd_src_up_sampling_mono_process_3+0x93>
 5c0:	fcc332        	addi	a3, a3, -4
 5c3:	fec442        	addi	a4, a4, -2
 5c6:	02a052        	movi	a5, 2
 5c9:	fea0c1        	l32r	a12, 4c (ffff8000 <_end+0xffff743c>)
 5cc:	fe9fd1        	l32r	a13, 48 (7fff <_end+0x743b>)
 5cf:	364d34        	ae_cvtq48a32s	aeq1, a13
 5d2:	368c34        	ae_cvtq48a32s	aeq2, a12
 5d5:	4e9676        	loopnez	a6, 627 <cvsd_src_up_sampling_mono_process_3+0x8f>
 5d8:	fc24d9025f 	{ ae_lp16f.c	aep0, a2, a5; nop }
 5dd:	e84299025f 	{ ae_lp16f.c	aep0, a2, a5; ae_mulp24s.hh	aeq0, aep0, aep2 }
 5e2:	e62219025f 	{ ae_lp16f.c	aep0, a2, a5; ae_mulap24s.hl	aeq0, aep0, aep1 }
 5e7:	020104        	ae_mulap24s.hh	aeq0, aep0, aep1
 5ea:	350174        	ae_sraiq56	aeq0, aeq0, 5
 5ed:	0114c4        	ae_minq56s	aeq0, aeq0, aeq1
 5f0:	0120c4        	ae_maxq56s	aeq0, aeq0, aeq2
 5f3:	630104        	ae_sq56s.i	aeq0, a1, 0
 5f6:	500104        	ae_lp24f.i	aep0, a1, 0
 5f9:	428324        	ae_sp16f.l.iu	aep0, a3, 4
 5fc:	fc24d9025f 	{ ae_lp16f.c	aep0, a2, a5; nop }
 601:	e82299025f 	{ ae_lp16f.c	aep0, a2, a5; ae_mulp24s.hh	aeq0, aep0, aep1 }
 606:	e62219025f 	{ ae_lp16f.c	aep0, a2, a5; ae_mulap24s.hl	aeq0, aep0, aep1 }
 60b:	e4422c841f 	{ ae_lp16f.iu	aep0, a4, 2; ae_mulap24s.hh	aeq0, aep0, aep2 }
 610:	fc24e2825f 	{ ae_sp16f.l.c	aep0, a2, a5; nop }
 615:	350174        	ae_sraiq56	aeq0, aeq0, 5
 618:	0114c4        	ae_minq56s	aeq0, aeq0, aeq1
 61b:	0120c4        	ae_maxq56s	aeq0, aeq0, aeq2
 61e:	630104        	ae_sq56s.i	aeq0, a1, 0
 621:	500104        	ae_lp24f.i	aep0, a1, 0
 624:	420314        	ae_sp16f.l.i	aep0, a3, 2

00000627 <cvsd_src_up_sampling_mono_process_3+0x8f>:
 627:	e929           	s32i.n	a2, a9, 56
 629:	f01d           	retw.n

0000062b <cvsd_src_up_sampling_mono_process_3+0x93>:
 62b:	e929           	s32i.n	a2, a9, 56
 62d:	f01d           	retw.n

0000062f <cvsd_src_up_sampling_mono_process_3+0x97>:
	...

00000630 <cvsd_src_dn_sampling_mono_process_1>:
 630:	006136        	entry	a1, 48
 633:	470c           	movi.n	a7, 4
 635:	280c           	movi.n	a8, 2
 637:	02ed           	mov.n	a14, a2
 639:	f3f640        	wur.ae_cbegin0	a4
 63c:	fe8661        	l32r	a6, 54 (b00 <cvsd_src_down_sampling_coef_1>)
 63f:	03dd           	mov.n	a13, a3
 641:	05cd           	mov.n	a12, a5
 643:	04bd           	mov.n	a11, a4
 645:	122bf2        	l32i	a15, a11, 72
 648:	4191c0        	srli	a9, a12, 1
 64b:	fe8031        	l32r	a3, 4c (ffff8000 <_end+0xffff743c>)
 64e:	14c452        	addi	a5, a4, 20
 651:	f3f750        	wur.ae_cend0	a5
 654:	368334        	ae_cvtq48a32s	aeq2, a3
 657:	fecd52        	addi	a5, a13, -2
 65a:	fcc232        	addi	a3, a2, -4
 65d:	441604        	ae_lp16x2f.i	aep1, a6, 0
 660:	442614        	ae_lp16x2f.i	aep2, a6, 4
 663:	fe7941        	l32r	a4, 48 (7fff <_end+0x743b>)
 666:	443624        	ae_lp16x2f.i	aep3, a6, 8
 669:	364434        	ae_cvtq48a32s	aeq1, a4
 66c:	fec242        	addi	a4, a2, -2
 66f:	0ca062        	movi	a6, 12
 672:	112b22        	l32i	a2, a11, 68
 675:	2e9976        	loopnez	a9, 6a7 <cvsd_src_dn_sampling_mono_process_1+0x77>
 678:	fc24da827f 	{ ae_lp16x2f.c	aep0, a2, a7; nop }
 67d:	f0239a827f 	{ ae_lp16x2f.c	aep0, a2, a7; ae_mulzaap24s.hh.ll	aeq0, aep0, aep1 }
 682:	e2409a826f 	{ ae_lp16x2f.c	aep0, a2, a6; ae_mulaap24s.hh.ll	aeq0, aep0, aep2 }
 687:	e260ac832f 	{ ae_lp16f.iu	aep0, a3, 4; ae_mulaap24s.hh.ll	aeq0, aep0, aep3 }
 68c:	fc24e2828f 	{ ae_sp16f.l.c	aep0, a2, a8; nop }
 691:	3501b4        	ae_sraiq56	aeq0, aeq0, 6
 694:	f80aac842f 	{ ae_lp16f.iu	aep0, a4, 4; ae_minq56s	aeq0, aeq0, aeq1 }
 699:	f87322828f 	{ ae_sp16f.l.c	aep0, a2, a8; ae_maxq56s	aeq0, aeq0, aeq2 }
 69e:	630104        	ae_sq56s.i	aeq0, a1, 0
 6a1:	500104        	ae_lp24f.i	aep0, a1, 0
 6a4:	428514        	ae_sp16f.l.iu	aep0, a5, 2

000006a7 <cvsd_src_dn_sampling_mono_process_1+0x77>:
 6a7:	fecd52        	addi	a5, a13, -2
 6aa:	fece42        	addi	a4, a14, -2
 6ad:	4192c0        	srli	a9, a12, 2
 6b0:	116b22        	s32i	a2, a11, 68
 6b3:	18cba2        	addi	a10, a11, 24
 6b6:	fe6431        	l32r	a3, 48 (7fff <_end+0x743b>)
 6b9:	364334        	ae_cvtq48a32s	aeq1, a3
 6bc:	f3f6a0        	wur.ae_cbegin0	a10
 6bf:	fe6321        	l32r	a2, 4c (ffff8000 <_end+0xffff743c>)
 6c2:	fe65a1        	l32r	a10, 58 (b20 <cvsd_src_down_sampling_coef_2>)
 6c5:	368234        	ae_cvtq48a32s	aeq2, a2
 6c8:	fcce32        	addi	a3, a14, -4
 6cb:	2ccb22        	addi	a2, a11, 44
 6ce:	441a04        	ae_lp16x2f.i	aep1, a10, 0
 6d1:	442a14        	ae_lp16x2f.i	aep2, a10, 4
 6d4:	443a24        	ae_lp16x2f.i	aep3, a10, 8
 6d7:	f3f720        	wur.ae_cend0	a2
 6da:	20a330        	or	a10, a3, a3
 6dd:	2e9976        	loopnez	a9, 70f <cvsd_src_dn_sampling_mono_process_1+0xdf>
 6e0:	fc24da8f7f 	{ ae_lp16x2f.c	aep0, a15, a7; nop }

000006e5 <cvsd_src_dn_sampling_mono_process_1+0xb5>:
 6e5:	f0239a8f7f 	{ ae_lp16x2f.c	aep0, a15, a7; ae_mulzaap24s.hh.ll	aeq0, aep0, aep1 }
 6ea:	e2409a8f6f 	{ ae_lp16x2f.c	aep0, a15, a6; ae_mulaap24s.hh.ll	aeq0, aep0, aep2 }
 6ef:	e260ac8a2f 	{ ae_lp16f.iu	aep0, a10, 4; ae_mulaap24s.hh.ll	aeq0, aep0, aep3 }
 6f4:	fc24e28f8f 	{ ae_sp16f.l.c	aep0, a15, a8; nop }
 6f9:	3501b4        	ae_sraiq56	aeq0, aeq0, 6
 6fc:	f80aac842f 	{ ae_lp16f.iu	aep0, a4, 4; ae_minq56s	aeq0, aeq0, aeq1 }
 701:	f873228f8f 	{ ae_sp16f.l.c	aep0, a15, a8; ae_maxq56s	aeq0, aeq0, aeq2 }
 706:	630104        	ae_sq56s.i	aeq0, a1, 0
 709:	500104        	ae_lp24f.i	aep0, a1, 0
 70c:	428514        	ae_sp16f.l.iu	aep0, a5, 2

0000070f <cvsd_src_dn_sampling_mono_process_1+0xdf>:
 70f:	126bf2        	s32i	a15, a11, 72
 712:	f01d           	retw.n

00000714 <cvsd_src_dn_sampling_mono_process_2>:
 714:	006136        	entry	a1, 48
 717:	470c           	movi.n	a7, 4
 719:	fe50f1        	l32r	a15, 5c (b40 <cvsd_src_down_sampling_coef_3>)
 71c:	30c462        	addi	a6, a4, 48
 71f:	418150        	srli	a8, a5, 1
 722:	04ad           	mov.n	a10, a4
 724:	fe4ad1        	l32r	a13, 4c (ffff8000 <_end+0xffff743c>)
 727:	fe48e1        	l32r	a14, 48 (7fff <_end+0x743b>)
 72a:	364e34        	ae_cvtq48a32s	aeq1, a14
 72d:	368d34        	ae_cvtq48a32s	aeq2, a13
 730:	132a92        	l32i	a9, a10, 76
 733:	fec352        	addi	a5, a3, -2
 736:	f3f660        	wur.ae_cbegin0	a6
 739:	fcc232        	addi	a3, a2, -4
 73c:	260c           	movi.n	a6, 2
 73e:	44c442        	addi	a4, a4, 68
 741:	441f04        	ae_lp16x2f.i	aep1, a15, 0
 744:	442f14        	ae_lp16x2f.i	aep2, a15, 4
 747:	443f24        	ae_lp16x2f.i	aep3, a15, 8
 74a:	444f34        	ae_lp16x2f.i	aep4, a15, 12
 74d:	f3f740        	wur.ae_cend0	a4
 750:	445f44        	ae_lp16x2f.i	aep5, a15, 16
 753:	fec242        	addi	a4, a2, -2
 756:	389876        	loopnez	a8, 792 <cvsd_src_dn_sampling_mono_process_2+0x7e>
 759:	fc24da897f 	{ ae_lp16x2f.c	aep0, a9, a7; nop }
 75e:	f0239a897f 	{ ae_lp16x2f.c	aep0, a9, a7; ae_mulzaap24s.hh.ll	aeq0, aep0, aep1 }
 763:	e2409a897f 	{ ae_lp16x2f.c	aep0, a9, a7; ae_mulaap24s.hh.ll	aeq0, aep0, aep2 }
 768:	e2609a897f 	{ ae_lp16x2f.c	aep0, a9, a7; ae_mulaap24s.hh.ll	aeq0, aep0, aep3 }
 76d:	e2809a897f 	{ ae_lp16x2f.c	aep0, a9, a7; ae_mulaap24s.hh.ll	aeq0, aep0, aep4 }
 772:	e2a0ac832f 	{ ae_lp16f.iu	aep0, a3, 4; ae_mulaap24s.hh.ll	aeq0, aep0, aep5 }
 777:	fc24e2896f 	{ ae_sp16f.l.c	aep0, a9, a6; nop }
 77c:	3502f4        	ae_sraiq56	aeq0, aeq0, 11
 77f:	f80aac842f 	{ ae_lp16f.iu	aep0, a4, 4; ae_minq56s	aeq0, aeq0, aeq1 }
 784:	f87322896f 	{ ae_sp16f.l.c	aep0, a9, a6; ae_maxq56s	aeq0, aeq0, aeq2 }
 789:	630104        	ae_sq56s.i	aeq0, a1, 0
 78c:	500104        	ae_lp24f.i	aep0, a1, 0
 78f:	428514        	ae_sp16f.l.iu	aep0, a5, 2

00000792 <cvsd_src_dn_sampling_mono_process_2+0x7e>:
 792:	136a92        	s32i	a9, a10, 76
 795:	f01d           	retw.n

00000797 <cvsd_src_dn_sampling_mono_process_2+0x83>:
	...

00000798 <cvsd_src_up_sampling_init>:
 798:	004136        	entry	a1, 32
 79b:	03ad           	mov.n	a10, a3
 79d:	fe3081        	l32r	a8, 60 (330 <f_memset>)
 7a0:	0b0c           	movi.n	a11, 0
 7a2:	40a0c2        	movi	a12, 64
 7a5:	0008e0        	callx8	a8
 7a8:	c339           	s32i.n	a3, a3, 48
 7aa:	20c392        	addi	a9, a3, 32
 7ad:	28c3a2        	addi	a10, a3, 40
 7b0:	e3a9           	s32i.n	a10, a3, 56
 7b2:	d399           	s32i.n	a9, a3, 52
 7b4:	0239           	s32i.n	a3, a2, 0
 7b6:	020c           	movi.n	a2, 0
 7b8:	f01d           	retw.n

000007ba <cvsd_src_up_sampling_init+0x22>:
	...

000007bc <cvsd_src_dn_sampling_init>:
 7bc:	004136        	entry	a1, 32
 7bf:	03ad           	mov.n	a10, a3
 7c1:	fe2781        	l32r	a8, 60 (330 <f_memset>)
 7c4:	0b0c           	movi.n	a11, 0
 7c6:	0c5c           	movi.n	a12, 80
 7c8:	0008e0        	callx8	a8
 7cb:	116332        	s32i	a3, a3, 68
 7ce:	18c392        	addi	a9, a3, 24
 7d1:	30c3a2        	addi	a10, a3, 48
 7d4:	1363a2        	s32i	a10, a3, 76
 7d7:	126392        	s32i	a9, a3, 72
 7da:	0239           	s32i.n	a3, a2, 0
 7dc:	020c           	movi.n	a2, 0
 7de:	f01d           	retw.n

000007e0 <cvsd_src_up_sampling_process>:
 7e0:	004136        	entry	a1, 32
 7e3:	03ad           	mov.n	a10, a3
 7e5:	04bd           	mov.n	a11, a4
 7e7:	fe1f81        	l32r	a8, 64 (368 <cvsd_src_up_sampling_mono_process_1>)
 7ea:	20c220        	or	a12, a2, a2
 7ed:	41d160        	srli	a13, a6, 1
 7f0:	0008e0        	callx8	a8
 7f3:	04ad           	mov.n	a10, a4
 7f5:	05bd           	mov.n	a11, a5
 7f7:	fe1c81        	l32r	a8, 68 (500 <cvsd_src_up_sampling_mono_process_2>)
 7fa:	02cd           	mov.n	a12, a2
 7fc:	06dd           	mov.n	a13, a6
 7fe:	0008e0        	callx8	a8
 801:	05bd           	mov.n	a11, a5
 803:	02cd           	mov.n	a12, a2
 805:	fe1981        	l32r	a8, 6c (598 <cvsd_src_up_sampling_mono_process_3>)
 808:	05ad           	mov.n	a10, a5
 80a:	11d6f0        	slli	a13, a6, 1
 80d:	0008e0        	callx8	a8
 810:	020c           	movi.n	a2, 0
 812:	f01d           	retw.n

00000814 <cvsd_src_dn_sampling_process>:
 814:	004136        	entry	a1, 32
 817:	03ad           	mov.n	a10, a3
 819:	03bd           	mov.n	a11, a3
 81b:	fe1581        	l32r	a8, 70 (630 <cvsd_src_dn_sampling_mono_process_1>)
 81e:	02cd           	mov.n	a12, a2
 820:	41d150        	srli	a13, a5, 1
 823:	0008e0        	callx8	a8
 826:	02cd           	mov.n	a12, a2
 828:	04bd           	mov.n	a11, a4
 82a:	fe1281        	l32r	a8, 74 (714 <cvsd_src_dn_sampling_mono_process_2>)
 82d:	03ad           	mov.n	a10, a3
 82f:	41d350        	srli	a13, a5, 3
 832:	0008e0        	callx8	a8
 835:	020c           	movi.n	a2, 0
 837:	f01d           	retw.n

00000839 <cvsd_src_dn_sampling_process+0x25>:
 839:	000000                                        ...

0000083c <_fini>:
 83c:	008136        	entry	a1, 64

0000083f <_fini+0x3>:
 83f:	f01d           	retw.n
