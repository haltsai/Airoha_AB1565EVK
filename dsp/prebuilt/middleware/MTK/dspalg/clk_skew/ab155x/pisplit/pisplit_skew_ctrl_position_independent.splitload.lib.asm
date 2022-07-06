
Build/lib/pisplit_skew_ctrl_position_independent.splitload.lib:     file format elf32-xtensa-le


Disassembly of section .text:

00000000 <_init-0x4c>:
   0:	0000109c 0000102c 00000078 000000ac     ....,...x.......
  10:	000005b0 00001090 00000bf8 c0000000     ................
  20:	000008e0 00000a70 00000154 00000388     ....p...T.......
  30:	40000000 0000a000 2aaaaaaa 00004000     ...@.......*.@..
  40:	55555555 00000ccd 00002000              UUUU..... ..

0000004c <_init>:
  4c:	008136        	entry	a1, 64
  4f:	f01d           	retw.n

00000051 <_init+0x5>:
  51:	00000000 36000000                                .......

00000058 <_start>:
  58:	004136        	entry	a1, 32
  5b:	ffeaa1        	l32r	a10, 4 (102c <_DYNAMIC+0x204>)
  5e:	ffeab1        	l32r	a11, 8 (78 <skew_ctrl_init>)
  61:	ffeac1        	l32r	a12, c (ac <skew_ctrl_set_input_framesize>)
  64:	ffe791        	l32r	a9, 0 (109c <__bss_start>)
  67:	0288           	l32i.n	a8, a2, 0
  69:	ffe9d1        	l32r	a13, 10 (5b0 <skew_ctrl_process>)
  6c:	0989           	s32i.n	a8, a9, 0
  6e:	0008e0        	callx8	a8
  71:	ffe821        	l32r	a2, 14 (1090 <export_parameter_array>)
  74:	f01d           	retw.n

00000076 <_start+0x1e>:
	...

00000078 <skew_ctrl_init>:
  78:	004136        	entry	a1, 32
  7b:	1a0c           	movi.n	a10, 1
  7d:	724d           	ae_s16i.n	a4, a2, 2
  7f:	623d           	ae_s16i.n	a3, a2, 0
  81:	070c           	movi.n	a7, 0
  83:	431c           	movi.n	a3, 20
  85:	2f5272        	s16i	a7, a2, 94
  88:	315272        	s16i	a7, a2, 98
  8b:	2e5272        	s16i	a7, a2, 92
  8e:	024d           	mov.n	a4, a2
  90:	305462        	s16i	a6, a4, 96
  93:	325452        	s16i	a5, a4, 100
  96:	2d54a2        	s16i	a10, a4, 90
  99:	072d           	mov.n	a2, a7
  9b:	025d           	mov.n	a5, a2
  9d:	088376        	loop	a3, a9 <skew_ctrl_init+0x31>
  a0:	a03240        	addx4	a3, a2, a4
  a3:	221b           	addi.n	a2, a2, 1
  a5:	2359           	s32i.n	a5, a3, 8
  a7:	922d           	ae_zext16	a2, a2

000000a9 <skew_ctrl_init+0x31>:
  a9:	f01d           	retw.n

000000ab <skew_ctrl_init+0x33>:
	...

000000ac <skew_ctrl_set_input_framesize>:
  ac:	004136        	entry	a1, 32
  af:	2c1282        	l16ui	a8, a2, 88
  b2:	621837        	beq	a8, a3, 118 <skew_ctrl_set_input_framesize+0x6c>
  b5:	2c5232        	s16i	a3, a2, 88
  b8:	0d0c           	movi.n	a13, 0
  ba:	00a062        	movi	a6, 0
  bd:	3212a2        	l16ui	a10, a2, 100
  c0:	525d           	ae_l16ui.n	a5, a2, 2
  c2:	429d           	ae_l16ui.n	a9, a2, 0
  c4:	00a072        	movi	a7, 0
  c7:	e0c552        	addi	a5, a5, -32
  ca:	4c2926        	beqi	a9, 2, 11a <skew_ctrl_set_input_framesize+0x6e>
  cd:	419130        	srli	a9, a3, 1

000000d0 <skew_ctrl_set_input_framesize+0x24>:
  d0:	2d1232        	l16ui	a3, a2, 90
  d3:	414190        	srli	a4, a9, 1
  d6:	839450        	moveqz	a9, a4, a5
  d9:	823390        	mull	a3, a3, a9
  dc:	1a6272        	s32i	a7, a2, 104
  df:	531b           	addi.n	a5, a3, 1
  e1:	430b           	addi.n	a4, a3, -1
  e3:	3a4a26        	beqi	a10, 4, 121 <skew_ctrl_set_input_framesize+0x75>
  e6:	05cd           	mov.n	a12, a5
  e8:	ffcc81        	l32r	a8, 18 (bf8 <__udivdi3>)
  eb:	015320        	slli	a5, a3, 30
  ee:	05ad           	mov.n	a10, a5
  f0:	413230        	srli	a3, a3, 2
  f3:	03bd           	mov.n	a11, a3
  f5:	0008e0        	callx8	a8
  f8:	06dd           	mov.n	a13, a6
  fa:	04cd           	mov.n	a12, a4
  fc:	1c62a2        	s32i	a10, a2, 112
  ff:	03bd           	mov.n	a11, a3
 101:	ffc581        	l32r	a8, 18 (bf8 <__udivdi3>)
 104:	0a3d           	mov.n	a3, a10
 106:	05ad           	mov.n	a10, a5
 108:	0008e0        	callx8	a8
 10b:	9a0b           	addi.n	a9, a10, -1
 10d:	b30b           	addi.n	a11, a3, -1
 10f:	1c62b2        	s32i	a11, a2, 112
 112:	1d6292        	s32i	a9, a2, 116
 115:	f01d           	retw.n

00000117 <skew_ctrl_set_input_framesize+0x6b>:
	...

00000118 <skew_ctrl_set_input_framesize+0x6c>:
 118:	f01d           	retw.n

0000011a <skew_ctrl_set_input_framesize+0x6e>:
 11a:	419230        	srli	a9, a3, 2
 11d:	ffebc6        	j	d0 <skew_ctrl_set_input_framesize+0x24>

00000120 <skew_ctrl_set_input_framesize+0x74>:
	...

00000121 <skew_ctrl_set_input_framesize+0x75>:
 121:	01a420        	slli	a10, a4, 30
 124:	41b240        	srli	a11, a4, 2
 127:	ffbc81        	l32r	a8, 18 (bf8 <__udivdi3>)
 12a:	07dd           	mov.n	a13, a7
 12c:	03cd           	mov.n	a12, a3
 12e:	0008e0        	callx8	a8
 131:	07dd           	mov.n	a13, a7
 133:	41b250        	srli	a11, a5, 2
 136:	1c62a2        	s32i	a10, a2, 112
 139:	019520        	slli	a9, a5, 30
 13c:	03cd           	mov.n	a12, a3
 13e:	ffb681        	l32r	a8, 18 (bf8 <__udivdi3>)
 141:	0a3d           	mov.n	a3, a10
 143:	09ad           	mov.n	a10, a9
 145:	0008e0        	callx8	a8
 148:	aa0b           	addi.n	a10, a10, -1
 14a:	b30b           	addi.n	a11, a3, -1
 14c:	1c62b2        	s32i	a11, a2, 112
 14f:	1d62a2        	s32i	a10, a2, 116
 152:	f01d           	retw.n

00000154 <skew_ctrl_16b>:
 154:	008136        	entry	a1, 64
 157:	3139           	s32i.n	a3, a1, 12
 159:	0159           	s32i.n	a5, a1, 0
 15b:	4169           	s32i.n	a6, a1, 16
 15d:	428d           	ae_l16ui.n	a8, a2, 0
 15f:	ab0c           	movi.n	a11, 10
 161:	590c           	movi.n	a9, 5
 163:	fec8c2        	addi	a12, a8, -2
 166:	839bc0        	moveqz	a9, a11, a12
 169:	2199           	s32i.n	a9, a1, 8
 16b:	90d920        	addx2	a13, a9, a2
 16e:	11a9f0        	slli	a10, a9, 1
 171:	11a9           	s32i.n	a10, a1, 4
 173:	bd8b           	addi.n	a11, a13, 8
 175:	0b7d           	mov.n	a7, a11
 177:	049916        	beqz	a9, 1c4 <skew_ctrl_16b+0x70>
 17a:	098d           	mov.n	a8, a9
 17c:	24f090        	extui	a15, a9, 0, 3
 17f:	069f76        	loopnez	a15, 189 <skew_ctrl_16b+0x35>
 182:	239d           	ae_l16si.n	a9, a3, 0
 184:	09c794        	ae_s16ip	a9, a7, 2
 187:	332b           	addi.n	a3, a3, 2

00000189 <skew_ctrl_16b+0x35>:
 189:	419380        	srli	a9, a8, 3
 18c:	f03d           	nop.n
 18e:	309976        	loopnez	a9, 1c2 <skew_ctrl_16b+0x6e>
 191:	239d           	ae_l16si.n	a9, a3, 0
 193:	09c794        	ae_s16ip	a9, a7, 2
 196:	339d           	ae_l16si.n	a9, a3, 2
 198:	09c794        	ae_s16ip	a9, a7, 2
 19b:	029392        	l16si	a9, a3, 4
 19e:	09c794        	ae_s16ip	a9, a7, 2
 1a1:	039392        	l16si	a9, a3, 6
 1a4:	09c794        	ae_s16ip	a9, a7, 2
 1a7:	049392        	l16si	a9, a3, 8
 1aa:	09c794        	ae_s16ip	a9, a7, 2
 1ad:	059392        	l16si	a9, a3, 10
 1b0:	09c794        	ae_s16ip	a9, a7, 2
 1b3:	069392        	l16si	a9, a3, 12
 1b6:	09c794        	ae_s16ip	a9, a7, 2
 1b9:	079392        	l16si	a9, a3, 14
 1bc:	09c794        	ae_s16ip	a9, a7, 2
 1bf:	10c332        	addi	a3, a3, 16
 1c2:	428d           	ae_l16ui.n	a8, a2, 0

000001c4 <skew_ctrl_16b+0x70>:
 1c4:	3112a2        	l16ui	a10, a2, 98
 1c7:	ff9561        	l32r	a6, 1c (c0000000 <_end+0xbfffef60>)
 1ca:	0b3d           	mov.n	a3, a11
 1cc:	fecaa2        	addi	a10, a10, -2
 1cf:	0dda16        	beqz	a10, 2b0 <skew_ctrl_16b+0x15c>

000001d2 <skew_ctrl_16b+0x7e>:
 1d2:	fec8c2        	addi	a12, a8, -2
 1d5:	0fdc16        	beqz	a12, 2d6 <skew_ctrl_16b+0x182>
 1d8:	550c           	movi.n	a5, 5

000001da <skew_ctrl_16b+0x86>:
 1da:	047d           	mov.n	a7, a4
 1dc:	040c           	movi.n	a4, 0

000001de <skew_ctrl_16b+0x8a>:
 1de:	1b22e2        	l32i	a14, a2, 108
 1e1:	1a22d2        	l32i	a13, a2, 104
 1e4:	f32b           	addi.n	a15, a3, 2
 1e6:	80dde0        	add	a13, a13, a14
 1e9:	10ed60        	and	a14, a13, a6
 1ec:	c0ede0        	sub	a14, a13, a14
 1ef:	1a62e2        	s32i	a14, a2, 104
 1f2:	020d67        	bnone	a13, a6, 1f8 <skew_ctrl_16b+0xa4>
 1f5:	933f40        	movnez	a3, a15, a4
 1f8:	02ad           	mov.n	a10, a2
 1fa:	ff8981        	l32r	a8, 20 (8e0 <intp_filter_16b>)
 1fd:	03bd           	mov.n	a11, a3
 1ff:	1c0c           	movi.n	a12, 1
 201:	0008e0        	callx8	a8
 204:	09c7a4        	ae_s16ip	a10, a7, 2
 207:	441b           	addi.n	a4, a4, 1
 209:	944d           	ae_zext16	a4, a4
 20b:	cf9547        	bne	a5, a4, 1de <skew_ctrl_16b+0x8a>
 20e:	3148           	l32i.n	a4, a1, 12
 210:	1138           	l32i.n	a3, a1, 4
 212:	4188           	l32i.n	a8, a1, 16
 214:	334a           	add.n	a3, a3, a4
 216:	fec332        	addi	a3, a3, -2
 219:	054d           	mov.n	a4, a5
 21b:	2db587        	bgeu	a5, a8, 24c <skew_ctrl_16b+0xf8>

0000021e <skew_ctrl_16b+0xca>:
 21e:	02ad           	mov.n	a10, a2
 220:	1c0c           	movi.n	a12, 1
 222:	1b22b2        	l32i	a11, a2, 108
 225:	1a2292        	l32i	a9, a2, 104
 228:	ff7e81        	l32r	a8, 20 (8e0 <intp_filter_16b>)
 22b:	d32b           	addi.n	a13, a3, 2
 22d:	99ba           	add.n	a9, a9, a11
 22f:	10b960        	and	a11, a9, a6
 232:	933db0        	movnez	a3, a13, a11
 235:	c099b0        	sub	a9, a9, a11
 238:	1a6292        	s32i	a9, a2, 104
 23b:	03bd           	mov.n	a11, a3
 23d:	0008e0        	callx8	a8
 240:	09c7a4        	ae_s16ip	a10, a7, 2
 243:	41c8           	l32i.n	a12, a1, 16
 245:	441b           	addi.n	a4, a4, 1
 247:	944d           	ae_zext16	a4, a4
 249:	d19c47        	bne	a12, a4, 21e <skew_ctrl_16b+0xca>

0000024c <skew_ctrl_16b+0xf8>:
 24c:	728b           	addi.n	a7, a2, 8
 24e:	428d           	ae_l16ui.n	a8, a2, 0
 250:	3138           	l32i.n	a3, a1, 12
 252:	1198           	l32i.n	a9, a1, 4
 254:	762826        	beqi	a8, 2, 2ce <skew_ctrl_16b+0x17a>
 257:	0188           	l32i.n	a8, a1, 0
 259:	1188f0        	slli	a8, a8, 1

0000025c <skew_ctrl_16b+0x108>:
 25c:	383a           	add.n	a3, a8, a3
 25e:	c03390        	sub	a3, a3, a9
 261:	2198           	l32i.n	a9, a1, 8
 263:	045916        	beqz	a9, 2ac <skew_ctrl_16b+0x158>
 266:	098d           	mov.n	a8, a9
 268:	249090        	extui	a9, a9, 0, 3
 26b:	069976        	loopnez	a9, 275 <skew_ctrl_16b+0x121>
 26e:	23ad           	ae_l16si.n	a10, a3, 0
 270:	09c7a4        	ae_s16ip	a10, a7, 2
 273:	332b           	addi.n	a3, a3, 2

00000275 <skew_ctrl_16b+0x121>:
 275:	419380        	srli	a9, a8, 3
 278:	309976        	loopnez	a9, 2ac <skew_ctrl_16b+0x158>
 27b:	239d           	ae_l16si.n	a9, a3, 0
 27d:	09c794        	ae_s16ip	a9, a7, 2
 280:	339d           	ae_l16si.n	a9, a3, 2
 282:	09c794        	ae_s16ip	a9, a7, 2
 285:	029392        	l16si	a9, a3, 4
 288:	09c794        	ae_s16ip	a9, a7, 2
 28b:	039392        	l16si	a9, a3, 6
 28e:	09c794        	ae_s16ip	a9, a7, 2
 291:	049392        	l16si	a9, a3, 8
 294:	09c794        	ae_s16ip	a9, a7, 2
 297:	059392        	l16si	a9, a3, 10
 29a:	09c794        	ae_s16ip	a9, a7, 2
 29d:	069392        	l16si	a9, a3, 12
 2a0:	09c794        	ae_s16ip	a9, a7, 2
 2a3:	079392        	l16si	a9, a3, 14
 2a6:	09c794        	ae_s16ip	a9, a7, 2
 2a9:	10c332        	addi	a3, a3, 16

000002ac <skew_ctrl_16b+0x158>:
 2ac:	f01d           	retw.n

000002ae <skew_ctrl_16b+0x15a>:
	...

000002b0 <skew_ctrl_16b+0x15c>:
 2b0:	2e12c2        	l16ui	a12, a2, 92
 2b3:	cc0b           	addi.n	a12, a12, -1
 2b5:	f19c56        	bnez	a12, 1d2 <skew_ctrl_16b+0x7e>
 2b8:	fec8a2        	addi	a10, a8, -2
 2bb:	490c           	movi.n	a9, 4
 2bd:	230c           	movi.n	a3, 2
 2bf:	8339a0        	moveqz	a3, a9, a10
 2c2:	3d3a           	add.n	a3, a13, a3
 2c4:	338b           	addi.n	a3, a3, 8
 2c6:	0b8a16        	beqz	a10, 382 <skew_ctrl_16b+0x22e>
 2c9:	450c           	movi.n	a5, 4
 2cb:	ffc2c6        	j	1da <skew_ctrl_16b+0x86>

000002ce <skew_ctrl_16b+0x17a>:
 2ce:	0188           	l32i.n	a8, a1, 0
 2d0:	1188e0        	slli	a8, a8, 2
 2d3:	ffe146        	j	25c <skew_ctrl_16b+0x108>

000002d6 <skew_ctrl_16b+0x182>:
 2d6:	550c           	movi.n	a5, 5

000002d8 <skew_ctrl_16b+0x184>:
 2d8:	047d           	mov.n	a7, a4
 2da:	932b           	addi.n	a9, a3, 2
 2dc:	5199           	s32i.n	a9, a1, 20
 2de:	040c           	movi.n	a4, 0

000002e0 <skew_ctrl_16b+0x18c>:
 2e0:	1b22b2        	l32i	a11, a2, 108
 2e3:	1a22a2        	l32i	a10, a2, 104
 2e6:	aaba           	add.n	a10, a10, a11
 2e8:	10ba60        	and	a11, a10, a6
 2eb:	c0bab0        	sub	a11, a10, a11
 2ee:	1a62b2        	s32i	a11, a2, 104
 2f1:	090a67        	bnone	a10, a6, 2fe <skew_ctrl_16b+0x1aa>
 2f4:	648c           	beqz.n	a4, 2fe <skew_ctrl_16b+0x1aa>
 2f6:	51c8           	l32i.n	a12, a1, 20
 2f8:	334b           	addi.n	a3, a3, 4
 2fa:	cc4b           	addi.n	a12, a12, 4
 2fc:	51c9           	s32i.n	a12, a1, 20
 2fe:	02ad           	mov.n	a10, a2
 300:	ff4881        	l32r	a8, 20 (8e0 <intp_filter_16b>)
 303:	03bd           	mov.n	a11, a3
 305:	2c0c           	movi.n	a12, 2
 307:	0008e0        	callx8	a8
 30a:	51b8           	l32i.n	a11, a1, 20
 30c:	67ad           	ae_s16i.n	a10, a7, 0
 30e:	ff4481        	l32r	a8, 20 (8e0 <intp_filter_16b>)
 311:	2c0c           	movi.n	a12, 2
 313:	02ad           	mov.n	a10, a2
 315:	0008e0        	callx8	a8
 318:	77ad           	ae_s16i.n	a10, a7, 2
 31a:	441b           	addi.n	a4, a4, 1
 31c:	774b           	addi.n	a7, a7, 4
 31e:	944d           	ae_zext16	a4, a4
 320:	bc9547        	bne	a5, a4, 2e0 <skew_ctrl_16b+0x18c>
 323:	31a8           	l32i.n	a10, a1, 12
 325:	1198           	l32i.n	a9, a1, 4
 327:	054d           	mov.n	a4, a5
 329:	4188           	l32i.n	a8, a1, 16
 32b:	99aa           	add.n	a9, a9, a10
 32d:	fcc932        	addi	a3, a9, -4
 330:	fec992        	addi	a9, a9, -2
 333:	056192        	s32i	a9, a1, 20
 336:	023587        	bltu	a5, a8, 33c <skew_ctrl_16b+0x1e8>
 339:	ffc3c6        	j	24c <skew_ctrl_16b+0xf8>

0000033c <skew_ctrl_16b+0x1e8>:
 33c:	1b22c2        	l32i	a12, a2, 108
 33f:	1a22b2        	l32i	a11, a2, 104
 342:	51d8           	l32i.n	a13, a1, 20
 344:	bbca           	add.n	a11, a11, a12
 346:	10cb60        	and	a12, a11, a6
 349:	c0cbc0        	sub	a12, a11, a12
 34c:	1a62c2        	s32i	a12, a2, 104
 34f:	050b67        	bnone	a11, a6, 358 <skew_ctrl_16b+0x204>
 352:	334b           	addi.n	a3, a3, 4
 354:	dd4b           	addi.n	a13, a13, 4
 356:	51d9           	s32i.n	a13, a1, 20

00000358 <skew_ctrl_16b+0x204>:
 358:	02ad           	mov.n	a10, a2
 35a:	ff3181        	l32r	a8, 20 (8e0 <intp_filter_16b>)
 35d:	03bd           	mov.n	a11, a3
 35f:	2c0c           	movi.n	a12, 2
 361:	0008e0        	callx8	a8
 364:	51b8           	l32i.n	a11, a1, 20
 366:	67ad           	ae_s16i.n	a10, a7, 0
 368:	ff2e81        	l32r	a8, 20 (8e0 <intp_filter_16b>)
 36b:	2c0c           	movi.n	a12, 2
 36d:	02ad           	mov.n	a10, a2
 36f:	0008e0        	callx8	a8
 372:	441b           	addi.n	a4, a4, 1
 374:	77ad           	ae_s16i.n	a10, a7, 2
 376:	4198           	l32i.n	a9, a1, 16
 378:	774b           	addi.n	a7, a7, 4
 37a:	944d           	ae_zext16	a4, a4
 37c:	bc9947        	bne	a9, a4, 33c <skew_ctrl_16b+0x1e8>
 37f:	ffb246        	j	24c <skew_ctrl_16b+0xf8>

00000382 <skew_ctrl_16b+0x22e>:
 382:	450c           	movi.n	a5, 4
 384:	ffd406        	j	2d8 <skew_ctrl_16b+0x184>

00000387 <skew_ctrl_16b+0x233>:
	...

00000388 <skew_ctrl_32b>:
 388:	008136        	entry	a1, 64
 38b:	3139           	s32i.n	a3, a1, 12
 38d:	0159           	s32i.n	a5, a1, 0
 38f:	4169           	s32i.n	a6, a1, 16
 391:	428d           	ae_l16ui.n	a8, a2, 0
 393:	ab0c           	movi.n	a11, 10
 395:	590c           	movi.n	a9, 5
 397:	fec8c2        	addi	a12, a8, -2
 39a:	839bc0        	moveqz	a9, a11, a12
 39d:	2199           	s32i.n	a9, a1, 8
 39f:	a0d920        	addx4	a13, a9, a2
 3a2:	11a9e0        	slli	a10, a9, 2
 3a5:	11a9           	s32i.n	a10, a1, 4
 3a7:	bd8b           	addi.n	a11, a13, 8
 3a9:	0b7d           	mov.n	a7, a11
 3ab:	041916        	beqz	a9, 3f0 <skew_ctrl_32b+0x68>
 3ae:	098d           	mov.n	a8, a9
 3b0:	24f090        	extui	a15, a9, 0, 3
 3b3:	069f76        	loopnez	a15, 3bd <skew_ctrl_32b+0x35>
 3b6:	0398           	l32i.n	a9, a3, 0
 3b8:	09e794        	ae_s32ip	a9, a7, 4
 3bb:	334b           	addi.n	a3, a3, 4

000003bd <skew_ctrl_32b+0x35>:
 3bd:	419380        	srli	a9, a8, 3
 3c0:	2a9976        	loopnez	a9, 3ee <skew_ctrl_32b+0x66>
 3c3:	0398           	l32i.n	a9, a3, 0
 3c5:	09e794        	ae_s32ip	a9, a7, 4
 3c8:	1398           	l32i.n	a9, a3, 4
 3ca:	09e794        	ae_s32ip	a9, a7, 4
 3cd:	2398           	l32i.n	a9, a3, 8
 3cf:	09e794        	ae_s32ip	a9, a7, 4
 3d2:	3398           	l32i.n	a9, a3, 12
 3d4:	09e794        	ae_s32ip	a9, a7, 4
 3d7:	4398           	l32i.n	a9, a3, 16
 3d9:	09e794        	ae_s32ip	a9, a7, 4
 3dc:	5398           	l32i.n	a9, a3, 20
 3de:	09e794        	ae_s32ip	a9, a7, 4
 3e1:	6398           	l32i.n	a9, a3, 24
 3e3:	09e794        	ae_s32ip	a9, a7, 4
 3e6:	7398           	l32i.n	a9, a3, 28
 3e8:	09e794        	ae_s32ip	a9, a7, 4
 3eb:	20c332        	addi	a3, a3, 32
 3ee:	428d           	ae_l16ui.n	a8, a2, 0

000003f0 <skew_ctrl_32b+0x68>:
 3f0:	3112a2        	l16ui	a10, a2, 98
 3f3:	ff0a61        	l32r	a6, 1c (c0000000 <_end+0xbfffef60>)
 3f6:	0b3d           	mov.n	a3, a11
 3f8:	fecaa2        	addi	a10, a10, -2
 3fb:	0d9a16        	beqz	a10, 4d8 <skew_ctrl_32b+0x150>

000003fe <skew_ctrl_32b+0x76>:
 3fe:	fec8c2        	addi	a12, a8, -2
 401:	0f9c16        	beqz	a12, 4fe <skew_ctrl_32b+0x176>
 404:	550c           	movi.n	a5, 5

00000406 <skew_ctrl_32b+0x7e>:
 406:	047d           	mov.n	a7, a4
 408:	00a042        	movi	a4, 0

0000040b <skew_ctrl_32b+0x83>:
 40b:	1b22e2        	l32i	a14, a2, 108
 40e:	1a22d2        	l32i	a13, a2, 104
 411:	f34b           	addi.n	a15, a3, 4
 413:	ddea           	add.n	a13, a13, a14
 415:	10ed60        	and	a14, a13, a6
 418:	c0ede0        	sub	a14, a13, a14
 41b:	1a62e2        	s32i	a14, a2, 104
 41e:	020d67        	bnone	a13, a6, 424 <skew_ctrl_32b+0x9c>
 421:	933f40        	movnez	a3, a15, a4
 424:	02ad           	mov.n	a10, a2
 426:	feff81        	l32r	a8, 24 (a70 <intp_filter_32b>)
 429:	03bd           	mov.n	a11, a3
 42b:	1c0c           	movi.n	a12, 1
 42d:	0008e0        	callx8	a8
 430:	09e7a4        	ae_s32ip	a10, a7, 4
 433:	441b           	addi.n	a4, a4, 1
 435:	944d           	ae_zext16	a4, a4
 437:	d09547        	bne	a5, a4, 40b <skew_ctrl_32b+0x83>
 43a:	3148           	l32i.n	a4, a1, 12
 43c:	1138           	l32i.n	a3, a1, 4
 43e:	4188           	l32i.n	a8, a1, 16
 440:	334a           	add.n	a3, a3, a4
 442:	fcc332        	addi	a3, a3, -4
 445:	054d           	mov.n	a4, a5
 447:	2db587        	bgeu	a5, a8, 478 <skew_ctrl_32b+0xf0>

0000044a <skew_ctrl_32b+0xc2>:
 44a:	02ad           	mov.n	a10, a2
 44c:	1c0c           	movi.n	a12, 1
 44e:	1b22b2        	l32i	a11, a2, 108
 451:	1a2292        	l32i	a9, a2, 104
 454:	fef481        	l32r	a8, 24 (a70 <intp_filter_32b>)
 457:	d34b           	addi.n	a13, a3, 4
 459:	99ba           	add.n	a9, a9, a11
 45b:	10b960        	and	a11, a9, a6
 45e:	933db0        	movnez	a3, a13, a11
 461:	c099b0        	sub	a9, a9, a11
 464:	1a6292        	s32i	a9, a2, 104
 467:	03bd           	mov.n	a11, a3
 469:	0008e0        	callx8	a8
 46c:	09e7a4        	ae_s32ip	a10, a7, 4
 46f:	41c8           	l32i.n	a12, a1, 16
 471:	441b           	addi.n	a4, a4, 1
 473:	944d           	ae_zext16	a4, a4
 475:	d19c47        	bne	a12, a4, 44a <skew_ctrl_32b+0xc2>

00000478 <skew_ctrl_32b+0xf0>:
 478:	728b           	addi.n	a7, a2, 8
 47a:	428d           	ae_l16ui.n	a8, a2, 0
 47c:	3138           	l32i.n	a3, a1, 12
 47e:	1198           	l32i.n	a9, a1, 4
 480:	722826        	beqi	a8, 2, 4f6 <skew_ctrl_32b+0x16e>
 483:	0188           	l32i.n	a8, a1, 0
 485:	1188e0        	slli	a8, a8, 2

00000488 <skew_ctrl_32b+0x100>:
 488:	383a           	add.n	a3, a8, a3
 48a:	c03390        	sub	a3, a3, a9
 48d:	2198           	l32i.n	a9, a1, 8
 48f:	041916        	beqz	a9, 4d4 <skew_ctrl_32b+0x14c>
 492:	098d           	mov.n	a8, a9
 494:	249090        	extui	a9, a9, 0, 3
 497:	069976        	loopnez	a9, 4a1 <skew_ctrl_32b+0x119>
 49a:	03a8           	l32i.n	a10, a3, 0
 49c:	09e7a4        	ae_s32ip	a10, a7, 4
 49f:	334b           	addi.n	a3, a3, 4

000004a1 <skew_ctrl_32b+0x119>:
 4a1:	419380        	srli	a9, a8, 3
 4a4:	f03d           	nop.n
 4a6:	2a9976        	loopnez	a9, 4d4 <skew_ctrl_32b+0x14c>
 4a9:	0398           	l32i.n	a9, a3, 0
 4ab:	09e794        	ae_s32ip	a9, a7, 4
 4ae:	1398           	l32i.n	a9, a3, 4
 4b0:	09e794        	ae_s32ip	a9, a7, 4
 4b3:	2398           	l32i.n	a9, a3, 8
 4b5:	09e794        	ae_s32ip	a9, a7, 4
 4b8:	3398           	l32i.n	a9, a3, 12
 4ba:	09e794        	ae_s32ip	a9, a7, 4
 4bd:	4398           	l32i.n	a9, a3, 16
 4bf:	09e794        	ae_s32ip	a9, a7, 4
 4c2:	5398           	l32i.n	a9, a3, 20
 4c4:	09e794        	ae_s32ip	a9, a7, 4
 4c7:	6398           	l32i.n	a9, a3, 24
 4c9:	09e794        	ae_s32ip	a9, a7, 4
 4cc:	7398           	l32i.n	a9, a3, 28
 4ce:	09e794        	ae_s32ip	a9, a7, 4
 4d1:	20c332        	addi	a3, a3, 32

000004d4 <skew_ctrl_32b+0x14c>:
 4d4:	f01d           	retw.n

000004d6 <skew_ctrl_32b+0x14e>:
	...

000004d8 <skew_ctrl_32b+0x150>:
 4d8:	2e12c2        	l16ui	a12, a2, 92
 4db:	cc0b           	addi.n	a12, a12, -1
 4dd:	f1dc56        	bnez	a12, 3fe <skew_ctrl_32b+0x76>
 4e0:	fec8a2        	addi	a10, a8, -2
 4e3:	890c           	movi.n	a9, 8
 4e5:	430c           	movi.n	a3, 4
 4e7:	8339a0        	moveqz	a3, a9, a10
 4ea:	3d3a           	add.n	a3, a13, a3
 4ec:	338b           	addi.n	a3, a3, 8
 4ee:	0b8a16        	beqz	a10, 5aa <skew_ctrl_32b+0x222>
 4f1:	450c           	movi.n	a5, 4
 4f3:	ffc3c6        	j	406 <skew_ctrl_32b+0x7e>

000004f6 <skew_ctrl_32b+0x16e>:
 4f6:	0188           	l32i.n	a8, a1, 0
 4f8:	1188d0        	slli	a8, a8, 3
 4fb:	ffe246        	j	488 <skew_ctrl_32b+0x100>

000004fe <skew_ctrl_32b+0x176>:
 4fe:	550c           	movi.n	a5, 5

00000500 <skew_ctrl_32b+0x178>:
 500:	047d           	mov.n	a7, a4
 502:	934b           	addi.n	a9, a3, 4
 504:	5199           	s32i.n	a9, a1, 20
 506:	040c           	movi.n	a4, 0

00000508 <skew_ctrl_32b+0x180>:
 508:	1b22b2        	l32i	a11, a2, 108
 50b:	1a22a2        	l32i	a10, a2, 104
 50e:	aaba           	add.n	a10, a10, a11
 510:	10ba60        	and	a11, a10, a6
 513:	c0bab0        	sub	a11, a10, a11
 516:	1a62b2        	s32i	a11, a2, 104
 519:	090a67        	bnone	a10, a6, 526 <skew_ctrl_32b+0x19e>
 51c:	648c           	beqz.n	a4, 526 <skew_ctrl_32b+0x19e>
 51e:	51c8           	l32i.n	a12, a1, 20
 520:	338b           	addi.n	a3, a3, 8
 522:	cc8b           	addi.n	a12, a12, 8
 524:	51c9           	s32i.n	a12, a1, 20
 526:	02ad           	mov.n	a10, a2
 528:	febf81        	l32r	a8, 24 (a70 <intp_filter_32b>)
 52b:	03bd           	mov.n	a11, a3
 52d:	2c0c           	movi.n	a12, 2
 52f:	0008e0        	callx8	a8
 532:	51b8           	l32i.n	a11, a1, 20
 534:	07a9           	s32i.n	a10, a7, 0
 536:	febb81        	l32r	a8, 24 (a70 <intp_filter_32b>)
 539:	2c0c           	movi.n	a12, 2
 53b:	02ad           	mov.n	a10, a2
 53d:	0008e0        	callx8	a8
 540:	17a9           	s32i.n	a10, a7, 4
 542:	441b           	addi.n	a4, a4, 1
 544:	778b           	addi.n	a7, a7, 8
 546:	944d           	ae_zext16	a4, a4
 548:	bc9547        	bne	a5, a4, 508 <skew_ctrl_32b+0x180>
 54b:	31a8           	l32i.n	a10, a1, 12
 54d:	1198           	l32i.n	a9, a1, 4
 54f:	054d           	mov.n	a4, a5
 551:	4188           	l32i.n	a8, a1, 16
 553:	99aa           	add.n	a9, a9, a10
 555:	f8c932        	addi	a3, a9, -8
 558:	fcc992        	addi	a9, a9, -4
 55b:	056192        	s32i	a9, a1, 20
 55e:	023587        	bltu	a5, a8, 564 <skew_ctrl_32b+0x1dc>
 561:	ffc4c6        	j	478 <skew_ctrl_32b+0xf0>

00000564 <skew_ctrl_32b+0x1dc>:
 564:	1b22c2        	l32i	a12, a2, 108
 567:	1a22b2        	l32i	a11, a2, 104
 56a:	51d8           	l32i.n	a13, a1, 20
 56c:	bbca           	add.n	a11, a11, a12
 56e:	10cb60        	and	a12, a11, a6
 571:	c0cbc0        	sub	a12, a11, a12
 574:	1a62c2        	s32i	a12, a2, 104
 577:	050b67        	bnone	a11, a6, 580 <skew_ctrl_32b+0x1f8>
 57a:	338b           	addi.n	a3, a3, 8
 57c:	dd8b           	addi.n	a13, a13, 8
 57e:	51d9           	s32i.n	a13, a1, 20

00000580 <skew_ctrl_32b+0x1f8>:
 580:	02ad           	mov.n	a10, a2
 582:	fea881        	l32r	a8, 24 (a70 <intp_filter_32b>)
 585:	03bd           	mov.n	a11, a3
 587:	2c0c           	movi.n	a12, 2
 589:	0008e0        	callx8	a8
 58c:	51b8           	l32i.n	a11, a1, 20
 58e:	07a9           	s32i.n	a10, a7, 0
 590:	fea581        	l32r	a8, 24 (a70 <intp_filter_32b>)
 593:	2c0c           	movi.n	a12, 2
 595:	02ad           	mov.n	a10, a2
 597:	0008e0        	callx8	a8
 59a:	441b           	addi.n	a4, a4, 1
 59c:	17a9           	s32i.n	a10, a7, 4
 59e:	4198           	l32i.n	a9, a1, 16
 5a0:	778b           	addi.n	a7, a7, 8
 5a2:	944d           	ae_zext16	a4, a4
 5a4:	bc9947        	bne	a9, a4, 564 <skew_ctrl_32b+0x1dc>
 5a7:	ffb346        	j	478 <skew_ctrl_32b+0xf0>

000005aa <skew_ctrl_32b+0x222>:
 5aa:	450c           	movi.n	a5, 4
 5ac:	ffd406        	j	500 <skew_ctrl_32b+0x178>

000005af <skew_ctrl_32b+0x227>:
	...

000005b0 <skew_ctrl_process>:
 5b0:	004136        	entry	a1, 32
 5b3:	2c12b2        	l16ui	a11, a2, 88
 5b6:	2e12a2        	l16ui	a10, a2, 92
 5b9:	0b8d           	mov.n	a8, a11
 5bb:	099a16        	beqz	a10, 658 <skew_ctrl_process+0xa8>
 5be:	311272        	l16ui	a7, a2, 98
 5c1:	01caa2        	addi	a10, a10, 1
 5c4:	9aad           	ae_zext16	a10, a10
 5c6:	2e52a2        	s16i	a10, a2, 92

000005c9 <skew_ctrl_process+0x19>:
 5c9:	2c0c           	movi.n	a12, 2
 5cb:	c7ac           	beqz.n	a7, 5fb <skew_ctrl_process+0x4b>
 5cd:	490c           	movi.n	a9, 4
 5cf:	52fd           	ae_l16ui.n	a15, a2, 2
 5d1:	42ed           	ae_l16ui.n	a14, a2, 0
 5d3:	0cdd           	mov.n	a13, a12
 5d5:	e0cff2        	addi	a15, a15, -32
 5d8:	fecee2        	addi	a14, a14, -2
 5db:	83d9e0        	moveqz	a13, a9, a14
 5de:	321292        	l16ui	a9, a2, 100
 5e1:	11edf0        	slli	a14, a13, 1
 5e4:	83def0        	moveqz	a13, a14, a15
 5e7:	fbc992        	addi	a9, a9, -5
 5ea:	1c7916        	beqz	a9, 7b5 <skew_ctrl_process+0x205>
 5ed:	f70b           	addi.n	a15, a7, -1
 5ef:	1e5f16        	beqz	a15, 7d8 <skew_ctrl_process+0x228>
 5f2:	ea0b           	addi.n	a14, a10, -1
 5f4:	9bda           	add.n	a9, a11, a13
 5f6:	999d           	ae_zext16	a9, a9
 5f8:	8389e0        	moveqz	a8, a9, a14

000005fb <skew_ctrl_process+0x4b>:
 5fb:	1d0c           	movi.n	a13, 1
 5fd:	648d           	ae_s16i.n	a8, a4, 0
 5ff:	66bd           	ae_s16i.n	a11, a6, 0
 601:	52fd           	ae_l16ui.n	a15, a2, 2
 603:	0a0c           	movi.n	a10, 0
 605:	424d           	ae_l16ui.n	a4, a2, 0
 607:	e0cfe2        	addi	a14, a15, -32
 60a:	83ade0        	moveqz	a10, a13, a14
 60d:	672426        	beqi	a4, 2, 678 <skew_ctrl_process+0xc8>
 610:	41b1b0        	srli	a11, a11, 1
 613:	f48180        	extui	a8, a8, 1, 16
 616:	419180        	srli	a9, a8, 1
 619:	9389a0        	movnez	a8, a9, a10

0000061c <skew_ctrl_process+0x6c>:
 61c:	3112d2        	l16ui	a13, a2, 98
 61f:	9bed           	ae_zext16	a14, a11
 621:	4191e0        	srli	a9, a14, 1
 624:	93e9a0        	movnez	a14, a9, a10
 627:	9eed           	ae_zext16	a14, a14
 629:	05bd16        	beqz	a13, 688 <skew_ctrl_process+0xd8>
 62c:	98dd           	ae_zext16	a13, a8
 62e:	e0cfa2        	addi	a10, a15, -32
 631:	193a16        	beqz	a10, 7c8 <skew_ctrl_process+0x218>
 634:	05cd           	mov.n	a12, a5
 636:	fe7c81        	l32r	a8, 28 (154 <skew_ctrl_16b>)
 639:	03bd           	mov.n	a11, a3
 63b:	02ad           	mov.n	a10, a2
 63d:	0008e0        	callx8	a8

00000640 <skew_ctrl_process+0x90>:
 640:	2e12a2        	l16ui	a10, a2, 92
 643:	2d1292        	l16ui	a9, a2, 90
 646:	0b0c           	movi.n	a11, 0
 648:	0119a7        	beq	a9, a10, 64d <skew_ctrl_process+0x9d>
 64b:	f01d           	retw.n

0000064d <skew_ctrl_process+0x9d>:
 64d:	2e52b2        	s16i	a11, a2, 92
 650:	3152b2        	s16i	a11, a2, 98
 653:	1a62b2        	s32i	a11, a2, 104
 656:	f01d           	retw.n

00000658 <skew_ctrl_process+0xa8>:
 658:	1c22d2        	l32i	a13, a2, 112
 65b:	1d22c2        	l32i	a12, a2, 116
 65e:	315272        	s16i	a7, a2, 98
 661:	e70b           	addi.n	a14, a7, -1
 663:	83cde0        	moveqz	a12, a13, a14
 666:	1b62c2        	s32i	a12, a2, 108
 669:	f5c716        	beqz	a7, 5c9 <skew_ctrl_process+0x19>
 66c:	aa1b           	addi.n	a10, a10, 1
 66e:	9aad           	ae_zext16	a10, a10
 670:	2e52a2        	s16i	a10, a2, 92
 673:	ffd486        	j	5c9 <skew_ctrl_process+0x19>

00000676 <skew_ctrl_process+0xc6>:
	...

00000678 <skew_ctrl_process+0xc8>:
 678:	41b2b0        	srli	a11, a11, 2
 67b:	f48280        	extui	a8, a8, 2, 16
 67e:	41d180        	srli	a13, a8, 1
 681:	938da0        	movnez	a8, a13, a10
 684:	ffe506        	j	61c <skew_ctrl_process+0x6c>

00000687 <skew_ctrl_process+0xd7>:
	...

00000688 <skew_ctrl_process+0xd8>:
 688:	301282        	l16ui	a8, a2, 96
 68b:	1a0c           	movi.n	a10, 1
 68d:	fac892        	addi	a9, a8, -6
 690:	155916        	beqz	a9, 7e9 <skew_ctrl_process+0x239>
 693:	f9c8b2        	addi	a11, a8, -7
 696:	83cab0        	moveqz	a12, a10, a11
 699:	0c8d           	mov.n	a8, a12

0000069b <skew_ctrl_process+0xeb>:
 69b:	11d8f0        	slli	a13, a8, 1
 69e:	fec4a2        	addi	a10, a4, -2
 6a1:	190c           	movi.n	a9, 1
 6a3:	0b0c           	movi.n	a11, 0
 6a5:	11cef0        	slli	a12, a14, 1
 6a8:	83b9a0        	moveqz	a11, a9, a10
 6ab:	e0cf92        	addi	a9, a15, -32
 6ae:	93ecb0        	movnez	a14, a12, a11
 6b1:	938db0        	movnez	a8, a13, a11
 6b4:	aa0c           	movi.n	a10, 10
 6b6:	5d0c           	movi.n	a13, 5
 6b8:	93dab0        	movnez	a13, a10, a11
 6bb:	c0ce80        	sub	a12, a14, a8
 6be:	8ccd           	ae_sext16	a12, a12
 6c0:	88ed           	ae_sext16	a14, a8
 6c2:	b28b           	addi.n	a11, a2, 8
 6c4:	128916        	beqz	a9, 7f0 <skew_ctrl_process+0x240>
 6c7:	116df0        	slli	a6, a13, 1
 6ca:	114ef0        	slli	a4, a14, 1
 6cd:	908db0        	addx2	a8, a13, a11
 6d0:	c08840        	sub	a8, a8, a4
 6d3:	431ea6        	blti	a14, 1, 71a <skew_ctrl_process+0x16a>
 6d6:	2490e0        	extui	a9, a14, 0, 3
 6d9:	06a976        	loopgtz	a9, 6e3 <skew_ctrl_process+0x133>
 6dc:	28ad           	ae_l16si.n	a10, a8, 0
 6de:	09c5a4        	ae_s16ip	a10, a5, 2
 6e1:	882b           	addi.n	a8, a8, 2

000006e3 <skew_ctrl_process+0x133>:
 6e3:	2193e0        	srai	a9, a14, 3
 6e6:	30a976        	loopgtz	a9, 71a <skew_ctrl_process+0x16a>
 6e9:	289d           	ae_l16si.n	a9, a8, 0
 6eb:	09c594        	ae_s16ip	a9, a5, 2
 6ee:	389d           	ae_l16si.n	a9, a8, 2
 6f0:	09c594        	ae_s16ip	a9, a5, 2
 6f3:	029892        	l16si	a9, a8, 4
 6f6:	09c594        	ae_s16ip	a9, a5, 2
 6f9:	039892        	l16si	a9, a8, 6
 6fc:	09c594        	ae_s16ip	a9, a5, 2
 6ff:	049892        	l16si	a9, a8, 8
 702:	09c594        	ae_s16ip	a9, a5, 2
 705:	059892        	l16si	a9, a8, 10
 708:	09c594        	ae_s16ip	a9, a5, 2
 70b:	069892        	l16si	a9, a8, 12
 70e:	09c594        	ae_s16ip	a9, a5, 2
 711:	079892        	l16si	a9, a8, 14
 714:	09c594        	ae_s16ip	a9, a5, 2
 717:	10c882        	addi	a8, a8, 16

0000071a <skew_ctrl_process+0x16a>:
 71a:	431ca6        	blti	a12, 1, 761 <skew_ctrl_process+0x1b1>
 71d:	24e0c0        	extui	a14, a12, 0, 3
 720:	06ae76        	loopgtz	a14, 72a <skew_ctrl_process+0x17a>
 723:	23fd           	ae_l16si.n	a15, a3, 0
 725:	09c5f4        	ae_s16ip	a15, a5, 2
 728:	332b           	addi.n	a3, a3, 2

0000072a <skew_ctrl_process+0x17a>:
 72a:	2193c0        	srai	a9, a12, 3
 72d:	30a976        	loopgtz	a9, 761 <skew_ctrl_process+0x1b1>
 730:	239d           	ae_l16si.n	a9, a3, 0
 732:	09c594        	ae_s16ip	a9, a5, 2
 735:	339d           	ae_l16si.n	a9, a3, 2
 737:	09c594        	ae_s16ip	a9, a5, 2
 73a:	029392        	l16si	a9, a3, 4
 73d:	09c594        	ae_s16ip	a9, a5, 2
 740:	039392        	l16si	a9, a3, 6
 743:	09c594        	ae_s16ip	a9, a5, 2
 746:	049392        	l16si	a9, a3, 8
 749:	09c594        	ae_s16ip	a9, a5, 2
 74c:	059392        	l16si	a9, a3, 10
 74f:	09c594        	ae_s16ip	a9, a5, 2
 752:	069392        	l16si	a9, a3, 12
 755:	09c594        	ae_s16ip	a9, a5, 2
 758:	079392        	l16si	a9, a3, 14
 75b:	09c594        	ae_s16ip	a9, a5, 2
 75e:	10c332        	addi	a3, a3, 16

00000761 <skew_ctrl_process+0x1b1>:
 761:	343a           	add.n	a3, a4, a3
 763:	c03360        	sub	a3, a3, a6
 766:	021de6        	bgei	a13, 1, 76c <skew_ctrl_process+0x1bc>
 769:	ffb4c6        	j	640 <skew_ctrl_process+0x90>
 76c:	2480d0        	extui	a8, a13, 0, 3
 76f:	06a876        	loopgtz	a8, 779 <skew_ctrl_process+0x1c9>
 772:	239d           	ae_l16si.n	a9, a3, 0
 774:	09cb94        	ae_s16ip	a9, a11, 2
 777:	332b           	addi.n	a3, a3, 2

00000779 <skew_ctrl_process+0x1c9>:
 779:	2193d0        	srai	a9, a13, 3
 77c:	f03d           	nop.n
 77e:	30a976        	loopgtz	a9, 7b2 <skew_ctrl_process+0x202>
 781:	239d           	ae_l16si.n	a9, a3, 0
 783:	09cb94        	ae_s16ip	a9, a11, 2
 786:	339d           	ae_l16si.n	a9, a3, 2
 788:	09cb94        	ae_s16ip	a9, a11, 2
 78b:	029392        	l16si	a9, a3, 4
 78e:	09cb94        	ae_s16ip	a9, a11, 2
 791:	039392        	l16si	a9, a3, 6
 794:	09cb94        	ae_s16ip	a9, a11, 2
 797:	049392        	l16si	a9, a3, 8
 79a:	09cb94        	ae_s16ip	a9, a11, 2
 79d:	059392        	l16si	a9, a3, 10
 7a0:	09cb94        	ae_s16ip	a9, a11, 2
 7a3:	069392        	l16si	a9, a3, 12
 7a6:	09cb94        	ae_s16ip	a9, a11, 2
 7a9:	079392        	l16si	a9, a3, 14
 7ac:	09cb94        	ae_s16ip	a9, a11, 2
 7af:	10c332        	addi	a3, a3, 16
 7b2:	ffa286        	j	640 <skew_ctrl_process+0x90>

000007b5 <skew_ctrl_process+0x205>:
 7b5:	e70b           	addi.n	a14, a7, -1
 7b7:	111e16        	beqz	a14, 8cc <skew_ctrl_process+0x31c>
 7ba:	9a0b           	addi.n	a9, a10, -1
 7bc:	c0fbd0        	sub	a15, a11, a13
 7bf:	9ffd           	ae_zext16	a15, a15
 7c1:	83bf90        	moveqz	a11, a15, a9
 7c4:	ff8cc6        	j	5fb <skew_ctrl_process+0x4b>

000007c7 <skew_ctrl_process+0x217>:
	...

000007c8 <skew_ctrl_process+0x218>:
 7c8:	05cd           	mov.n	a12, a5
 7ca:	fe1881        	l32r	a8, 2c (388 <skew_ctrl_32b>)
 7cd:	03bd           	mov.n	a11, a3
 7cf:	02ad           	mov.n	a10, a2
 7d1:	0008e0        	callx8	a8
 7d4:	ff9a06        	j	640 <skew_ctrl_process+0x90>

000007d7 <skew_ctrl_process+0x227>:
	...

000007d8 <skew_ctrl_process+0x228>:
 7d8:	2d12e2        	l16ui	a14, a2, 90
 7db:	c09bd0        	sub	a9, a11, a13
 7de:	999d           	ae_zext16	a9, a9
 7e0:	c0eea0        	sub	a14, a14, a10
 7e3:	8389e0        	moveqz	a8, a9, a14
 7e6:	ff8446        	j	5fb <skew_ctrl_process+0x4b>

000007e9 <skew_ctrl_process+0x239>:
 7e9:	080c           	movi.n	a8, 0
 7eb:	ffab06        	j	69b <skew_ctrl_process+0xeb>

000007ee <skew_ctrl_process+0x23e>:
	...

000007f0 <skew_ctrl_process+0x240>:
 7f0:	116de0        	slli	a6, a13, 2
 7f3:	114ee0        	slli	a4, a14, 2
 7f6:	a08db0        	addx4	a8, a13, a11
 7f9:	c08840        	sub	a8, a8, a4
 7fc:	3d1ea6        	blti	a14, 1, 83d <skew_ctrl_process+0x28d>
 7ff:	2490e0        	extui	a9, a14, 0, 3
 802:	06a976        	loopgtz	a9, 80c <skew_ctrl_process+0x25c>
 805:	08a8           	l32i.n	a10, a8, 0
 807:	09e5a4        	ae_s32ip	a10, a5, 4
 80a:	884b           	addi.n	a8, a8, 4

0000080c <skew_ctrl_process+0x25c>:
 80c:	2193e0        	srai	a9, a14, 3
 80f:	2aa976        	loopgtz	a9, 83d <skew_ctrl_process+0x28d>
 812:	0898           	l32i.n	a9, a8, 0
 814:	09e594        	ae_s32ip	a9, a5, 4
 817:	1898           	l32i.n	a9, a8, 4
 819:	09e594        	ae_s32ip	a9, a5, 4
 81c:	2898           	l32i.n	a9, a8, 8
 81e:	09e594        	ae_s32ip	a9, a5, 4
 821:	3898           	l32i.n	a9, a8, 12
 823:	09e594        	ae_s32ip	a9, a5, 4
 826:	4898           	l32i.n	a9, a8, 16
 828:	09e594        	ae_s32ip	a9, a5, 4
 82b:	5898           	l32i.n	a9, a8, 20
 82d:	09e594        	ae_s32ip	a9, a5, 4
 830:	6898           	l32i.n	a9, a8, 24
 832:	09e594        	ae_s32ip	a9, a5, 4
 835:	7898           	l32i.n	a9, a8, 28
 837:	09e594        	ae_s32ip	a9, a5, 4
 83a:	20c882        	addi	a8, a8, 32

0000083d <skew_ctrl_process+0x28d>:
 83d:	3d1ca6        	blti	a12, 1, 87e <skew_ctrl_process+0x2ce>
 840:	24e0c0        	extui	a14, a12, 0, 3
 843:	06ae76        	loopgtz	a14, 84d <skew_ctrl_process+0x29d>
 846:	03f8           	l32i.n	a15, a3, 0
 848:	09e5f4        	ae_s32ip	a15, a5, 4
 84b:	334b           	addi.n	a3, a3, 4

0000084d <skew_ctrl_process+0x29d>:
 84d:	2193c0        	srai	a9, a12, 3
 850:	2aa976        	loopgtz	a9, 87e <skew_ctrl_process+0x2ce>
 853:	0398           	l32i.n	a9, a3, 0
 855:	09e594        	ae_s32ip	a9, a5, 4
 858:	1398           	l32i.n	a9, a3, 4
 85a:	09e594        	ae_s32ip	a9, a5, 4
 85d:	2398           	l32i.n	a9, a3, 8
 85f:	09e594        	ae_s32ip	a9, a5, 4
 862:	3398           	l32i.n	a9, a3, 12
 864:	09e594        	ae_s32ip	a9, a5, 4
 867:	4398           	l32i.n	a9, a3, 16
 869:	09e594        	ae_s32ip	a9, a5, 4
 86c:	5398           	l32i.n	a9, a3, 20
 86e:	09e594        	ae_s32ip	a9, a5, 4
 871:	6398           	l32i.n	a9, a3, 24
 873:	09e594        	ae_s32ip	a9, a5, 4
 876:	7398           	l32i.n	a9, a3, 28
 878:	09e594        	ae_s32ip	a9, a5, 4
 87b:	20c332        	addi	a3, a3, 32

0000087e <skew_ctrl_process+0x2ce>:
 87e:	343a           	add.n	a3, a4, a3
 880:	c03360        	sub	a3, a3, a6
 883:	021de6        	bgei	a13, 1, 889 <skew_ctrl_process+0x2d9>
 886:	ff6d86        	j	640 <skew_ctrl_process+0x90>
 889:	2480d0        	extui	a8, a13, 0, 3
 88c:	f03d           	nop.n
 88e:	06a876        	loopgtz	a8, 898 <skew_ctrl_process+0x2e8>
 891:	0398           	l32i.n	a9, a3, 0
 893:	09eb94        	ae_s32ip	a9, a11, 4
 896:	334b           	addi.n	a3, a3, 4

00000898 <skew_ctrl_process+0x2e8>:
 898:	2193d0        	srai	a9, a13, 3
 89b:	2aa976        	loopgtz	a9, 8c9 <skew_ctrl_process+0x319>
 89e:	0398           	l32i.n	a9, a3, 0
 8a0:	09eb94        	ae_s32ip	a9, a11, 4
 8a3:	1398           	l32i.n	a9, a3, 4
 8a5:	09eb94        	ae_s32ip	a9, a11, 4
 8a8:	2398           	l32i.n	a9, a3, 8
 8aa:	09eb94        	ae_s32ip	a9, a11, 4
 8ad:	3398           	l32i.n	a9, a3, 12
 8af:	09eb94        	ae_s32ip	a9, a11, 4
 8b2:	4398           	l32i.n	a9, a3, 16
 8b4:	09eb94        	ae_s32ip	a9, a11, 4
 8b7:	5398           	l32i.n	a9, a3, 20
 8b9:	09eb94        	ae_s32ip	a9, a11, 4
 8bc:	6398           	l32i.n	a9, a3, 24
 8be:	09eb94        	ae_s32ip	a9, a11, 4
 8c1:	7398           	l32i.n	a9, a3, 28
 8c3:	09eb94        	ae_s32ip	a9, a11, 4
 8c6:	20c332        	addi	a3, a3, 32
 8c9:	ff5cc6        	j	640 <skew_ctrl_process+0x90>

000008cc <skew_ctrl_process+0x31c>:
 8cc:	2d12f2        	l16ui	a15, a2, 90
 8cf:	ebda           	add.n	a14, a11, a13
 8d1:	9eed           	ae_zext16	a14, a14
 8d3:	c0ffa0        	sub	a15, a15, a10
 8d6:	83bef0        	moveqz	a11, a14, a15
 8d9:	ff4786        	j	5fb <skew_ctrl_process+0x4b>

000008dc <skew_ctrl_process+0x32c>:
 8dc:	00000000                                ....

000008e0 <intp_filter_16b>:
 8e0:	008136        	entry	a1, 64
 8e3:	1a2272        	l32i	a7, a2, 104
 8e6:	301262        	l16ui	a6, a2, 96
 8e9:	1184f0        	slli	a8, a4, 1
 8ec:	882d           	ae_sext16	a2, a8
 8ee:	fac652        	addi	a5, a6, -6
 8f1:	1177f0        	slli	a7, a7, 1
 8f4:	0179           	s32i.n	a7, a1, 0
 8f6:	500104        	ae_lp24f.i	aep0, a1, 0
 8f9:	153516        	beqz	a5, a50 <intp_filter_16b+0x170>
 8fc:	fdcd71        	l32r	a7, 30 (40000000 <_end+0x3fffef60>)
 8ff:	f9c692        	addi	a9, a6, -7
 902:	fdcc61        	l32r	a6, 34 (a000 <_end+0x8f60>)
 905:	0ec916        	beqz	a9, 9f5 <intp_filter_16b+0x115>
 908:	60f020        	neg	a15, a2
 90b:	fdce51        	l32r	a5, 44 (ccd <__udivdi3+0xd5>)
 90e:	fdcaa1        	l32r	a10, 38 (2aaaaaaa <_end+0x2aaa9a0a>)
 911:	fdca91        	l32r	a9, 3c (4000 <_end+0x2f60>)
 914:	368734        	ae_cvtq48a32s	aeq2, a7
 917:	fdca81        	l32r	a8, 40 (55555555 <_end+0x555544b5>)
 91a:	36c834        	ae_cvtq48a32s	aeq3, a8
 91d:	356054        	ae_slliq56	aeq1, aeq2, 1
 920:	fc1458b99f 	{ ae_cvtp24a16x2.ll	aep3, a9, a9; ae_movq56	aeq0, aeq2 }
 925:	ee619421af 	{ s32i.n	a10, a1, 8; ae_mulsfp24s.ll	aeq0, aep0, aep3 }
 92a:	e363ae212f 	{ ae_lp24f.i	aep2, a1, 8; ae_mulafp24s.ll	aeq2, aep0, aep3 }
 92f:	018a34        	ae_mulafp24s.ll	aeq3, aep0, aep2
 932:	1bac74        	ae_roundsp24q48sym	aep7, aeq2
 935:	1bbc54        	ae_roundsp24q48sym	aep5, aeq3
 938:	ee41ff11bf 	{ ae_movq56	aeq2, aeq1; ae_mulsfp24s.ll	aeq1, aep0, aep2 }
 93d:	0183b4        	ae_mulsfp24s.ll	aeq2, aep0, aep3
 940:	faefba801f 	{ ae_slliq56	aeq0, aeq0, 1; ae_roundsp24q48sym	aep4, aeq1 }
 945:	1b8c34        	ae_roundsp24q48sym	aep3, aeq0
 948:	010d74        	ae_mulfp24s.ll	aeq1, aep0, aep5
 94b:	1bac14        	ae_roundsp24q48sym	aep1, aeq2
 94e:	01c974        	ae_mulfp24s.ll	aeq3, aep4, aep1
 951:	011374        	ae_mulfp24s.ll	aeq0, aep1, aep3
 954:	01f574        	ae_mulfp24s.ll	aeq2, aep7, aep5
 957:	1b8c64        	ae_roundsp24q48sym	aep6, aeq0
 95a:	1bac24        	ae_roundsp24q48sym	aep2, aeq2
 95d:	fbfff7712f 	{ ae_sp24x2s.i	aep7, a1, 16; ae_roundsp24q48sym	aep7, aeq3 }
 962:	012074        	ae_mulfp24s.ll	aeq0, aep2, aep0
 965:	01fb74        	ae_mulfp24s.ll	aeq3, aep7, aep3
 968:	1b8c54        	ae_roundsp24q48sym	aep5, aeq0
 96b:	1bbc14        	ae_roundsp24q48sym	aep1, aeq3
 96e:	019a74        	ae_mulfp24s.ll	aeq3, aep1, aep2
 971:	017574        	ae_mulfp24s.ll	aeq0, aep7, aep5
 974:	35b054        	ae_slliq56	aeq2, aeq3, 1
 977:	fbfa3a809f 	{ ae_slliq56	aeq2, aeq0, 1; ae_addq56	aeq3, aeq3, aeq2 }
 97c:	010ac4        	ae_addq56	aeq0, aeq0, aeq2
 97f:	1bbc74        	ae_roundsp24q48sym	aep7, aeq3
 982:	eb71f0212f 	{ ae_lp24x2.i	aep2, a1, 16; ae_mulfp24s.ll	aeq3, aep4, aep3 }
 987:	018274        	ae_mulfp24s.ll	aeq2, aep0, aep2
 98a:	1b9c34        	ae_roundsp24q48sym	aep3, aeq1
 98d:	016d74        	ae_mulfp24s.ll	aeq1, aep6, aep5
 990:	1bac24        	ae_roundsp24q48sym	aep2, aeq2
 993:	1bbc44        	ae_roundsp24q48sym	aep4, aeq3
 996:	01cd74        	ae_mulfp24s.ll	aeq3, aep4, aep5
 999:	019374        	ae_mulfp24s.ll	aeq2, aep1, aep3
 99c:	1b9c54        	ae_roundsp24q48sym	aep5, aeq1
 99f:	1bac34        	ae_roundsp24q48sym	aep3, aeq2
 9a2:	fbff98c66f 	{ ae_cvtp24a16x2.ll	aep4, a6, a6; ae_roundsp24q48sym	aep6, aeq3 }
 9a7:	016c74        	ae_mulfp24s.ll	aeq1, aep6, aep4
 9aa:	eb4598e55f 	{ ae_cvtp24a16x2.ll	aep6, a5, a5; ae_mulfp24s.ll	aeq2, aep1, aep2 }
 9af:	01de74        	ae_mulfp24s.ll	aeq3, aep5, aep6
 9b2:	1b9c24        	ae_roundsp24q48sym	aep2, aeq1
 9b5:	f8ffd1e43f 	{ addx2	a14, a4, a3; ae_roundsp24q48sym	aep1, aeq3 }
 9ba:	eb8dda5eff 	{ ae_lp16f.xu	aep5, a14, a15; ae_mulfp24s.ll	aeq3, aep3, aep4 }
 9bf:	ea35da1eff 	{ ae_lp16f.xu	aep1, a14, a15; ae_mulfp24s.ll	aeq1, aep5, aep1 }
 9c4:	011a34        	ae_mulafp24s.ll	aeq1, aep1, aep2
 9c7:	1bac54        	ae_roundsp24q48sym	aep5, aeq2
 9ca:	1b8c24        	ae_roundsp24q48sym	aep2, aeq0
 9cd:	01d674        	ae_mulfp24s.ll	aeq2, aep5, aep6
 9d0:	faffda1eff 	{ ae_lp16f.xu	aep1, a14, a15; ae_roundsp24q48sym	aep5, aeq3 }
 9d5:	011a34        	ae_mulafp24s.ll	aeq1, aep1, aep2
 9d8:	f9f7da6eff 	{ ae_lp16f.xu	aep6, a14, a15; ae_roundsp24q48sym	aep3, aeq2 }
 9dd:	e2fbda4eff 	{ ae_lp16f.xu	aep4, a14, a15; ae_mulafp24s.ll	aeq1, aep6, aep7 }
 9e2:	e2b3da2eff 	{ ae_lp16f.xu	aep2, a14, a15; ae_mulafp24s.ll	aeq1, aep4, aep5 }
 9e7:	012b34        	ae_mulafp24s.ll	aeq1, aep2, aep3
 9ea:	1b9c14        	ae_roundsp24q48sym	aep1, aeq1
 9ed:	421124        	ae_sp16f.l.i	aep1, a1, 4
 9f0:	029122        	l16si	a2, a1, 4
 9f3:	f01d           	retw.n

000009f5 <intp_filter_16b+0x115>:
 9f5:	fd94c1        	l32r	a12, 48 (2000 <_end+0xf60>)
 9f8:	301cc4        	ae_cvtp24a16x2.ll	aep1, a12, a12
 9fb:	019874        	ae_mulfp24s.ll	aeq3, aep1, aep0
 9fe:	11b4d0        	slli	a11, a4, 3
 a01:	1bbc74        	ae_roundsp24q48sym	aep7, aeq3
 a04:	017874        	ae_mulfp24s.ll	aeq1, aep7, aep0
 a07:	368734        	ae_cvtq48a32s	aeq2, a7
 a0a:	f83efaa09f 	{ ae_slliq56	aeq2, aeq2, 1; ae_subq56	aeq0, aeq3, aeq1 }
 a0f:	fa36d8e66f 	{ ae_cvtp24a16x2.ll	aep6, a6, a6; ae_subq56	aeq2, aeq2, aeq1 }
 a14:	01e034        	ae_mulafp24s.ll	aeq2, aep6, aep0
 a17:	01dfc4        	ae_subq56	aeq3, aeq1, aeq3
 a1a:	faffd5b3bf 	{ sub	a11, a3, a11; ae_roundsp24q48sym	aep5, aeq3 }
 a1f:	f9f7da4b2f 	{ ae_lp16f.xu	aep4, a11, a2; ae_roundsp24q48sym	aep3, aeq2 }
 a24:	01cd74        	ae_mulfp24s.ll	aeq3, aep4, aep5
 a27:	0a4084        	ae_cvtq48p24s.l	aeq1, aep0
 a2a:	0158c4        	ae_addq56	aeq1, aeq1, aeq0
 a2d:	f8efda2b2f 	{ ae_lp16f.xu	aep2, a11, a2; ae_roundsp24q48sym	aep1, aeq1 }
 a32:	01ab34        	ae_mulafp24s.ll	aeq3, aep2, aep3
 a35:	060824        	ae_negq56	aeq0, aeq0
 a38:	fbe79a7b2f 	{ ae_lp16f.xu	aep7, a11, a2; ae_roundsp24q48sym	aep6, aeq0 }
 a3d:	e33fda5b2f 	{ ae_lp16f.xu	aep5, a11, a2; ae_mulafp24s.ll	aeq3, aep7, aep1 }
 a42:	01de34        	ae_mulafp24s.ll	aeq3, aep5, aep6
 a45:	1bbc44        	ae_roundsp24q48sym	aep4, aeq3
 a48:	424124        	ae_sp16f.l.i	aep4, a1, 4
 a4b:	029122        	l16si	a2, a1, 4
 a4e:	f01d           	retw.n

00000a50 <intp_filter_16b+0x170>:
 a50:	11d4e0        	slli	a13, a4, 2
 a53:	c0d3d0        	sub	a13, a3, a13
 a56:	41cd24        	ae_lp16f.xu	aep4, a13, a2
 a59:	0ac484        	ae_cvtq48p24s.l	aeq3, aep4
 a5c:	ef11da3d2f 	{ ae_lp16f.xu	aep3, a13, a2; ae_mulsfp24s.ll	aeq3, aep4, aep0 }
 a61:	01b834        	ae_mulafp24s.ll	aeq3, aep3, aep0
 a64:	1bbc24        	ae_roundsp24q48sym	aep2, aeq3
 a67:	422124        	ae_sp16f.l.i	aep2, a1, 4
 a6a:	029122        	l16si	a2, a1, 4
 a6d:	f01d           	retw.n

00000a6f <intp_filter_16b+0x18f>:
	...

00000a70 <intp_filter_32b>:
 a70:	008136        	entry	a1, 64
 a73:	301262        	l16ui	a6, a2, 96
 a76:	1a2272        	l32i	a7, a2, 104
 a79:	1124e0        	slli	a2, a4, 2
 a7c:	fac652        	addi	a5, a6, -6
 a7f:	1177f0        	slli	a7, a7, 1
 a82:	0179           	s32i.n	a7, a1, 0
 a84:	500104        	ae_lp24f.i	aep0, a1, 0
 a87:	151516        	beqz	a5, bdc <intp_filter_32b+0x16c>
 a8a:	fd6971        	l32r	a7, 30 (40000000 <_end+0x3fffef60>)
 a8d:	f9c682        	addi	a8, a6, -7
 a90:	fd6961        	l32r	a6, 34 (a000 <_end+0x8f60>)
 a93:	0eb816        	beqz	a8, b82 <intp_filter_32b+0x112>
 a96:	60e020        	neg	a14, a2
 a99:	a0d430        	addx4	a13, a4, a3
 a9c:	fd6af1        	l32r	a15, 44 (ccd <__udivdi3+0xd5>)
 a9f:	fd6691        	l32r	a9, 38 (2aaaaaaa <_end+0x2aaa9a0a>)
 aa2:	fd6681        	l32r	a8, 3c (4000 <_end+0x2f60>)
 aa5:	360734        	ae_cvtq48a32s	aeq0, a7
 aa8:	fd6651        	l32r	a5, 40 (55555555 <_end+0x555544b5>)
 aab:	364534        	ae_cvtq48a32s	aeq1, a5
 aae:	35c054        	ae_slliq56	aeq3, aeq0, 1
 ab1:	fe0458e88f 	{ ae_cvtp24a16x2.ll	aep6, a8, a8; ae_movq56	aeq2, aeq0 }
 ab6:	efc194219f 	{ s32i.n	a9, a1, 8; ae_mulsfp24s.ll	aeq2, aep0, aep6 }
 abb:	e2c3ae512f 	{ ae_lp24f.i	aep5, a1, 8; ae_mulafp24s.ll	aeq0, aep0, aep6 }
 ac0:	010d34        	ae_mulafp24s.ll	aeq1, aep0, aep5
 ac3:	fbe7faa09f 	{ ae_slliq56	aeq2, aeq2, 1; ae_roundsp24q48sym	aep7, aeq0 }
 ac8:	1bac14        	ae_roundsp24q48sym	aep1, aeq2
 acb:	efa1ff313f 	{ ae_movq56	aeq0, aeq3; ae_mulsfp24s.ll	aeq3, aep0, aep5 }
 ad0:	1b9c24        	ae_roundsp24q48sym	aep2, aeq1
 ad3:	0106b4        	ae_mulsfp24s.ll	aeq0, aep0, aep6
 ad6:	010a74        	ae_mulfp24s.ll	aeq1, aep0, aep2
 ad9:	1bbc44        	ae_roundsp24q48sym	aep4, aeq3
 adc:	1b9c54        	ae_roundsp24q48sym	aep5, aeq1
 adf:	01c174        	ae_mulfp24s.ll	aeq2, aep4, aep1
 ae2:	1b8c34        	ae_roundsp24q48sym	aep3, aeq0
 ae5:	013174        	ae_mulfp24s.ll	aeq0, aep3, aep1
 ae8:	01cb74        	ae_mulfp24s.ll	aeq3, aep4, aep3
 aeb:	eb5dba212f 	{ ae_sq56s.i	aeq2, a1, 16; ae_mulfp24s.ll	aeq2, aep7, aep2 }
 af0:	1bbc34        	ae_roundsp24q48sym	aep3, aeq3
 af3:	01b974        	ae_mulfp24s.ll	aeq3, aep3, aep1
 af6:	1bac64        	ae_roundsp24q48sym	aep6, aeq2
 af9:	016874        	ae_mulfp24s.ll	aeq1, aep6, aep0
 afc:	1bbc44        	ae_roundsp24q48sym	aep4, aeq3
 aff:	1b9c14        	ae_roundsp24q48sym	aep1, aeq1
 b02:	014e74        	ae_mulfp24s.ll	aeq1, aep4, aep6
 b05:	01b974        	ae_mulfp24s.ll	aeq3, aep3, aep1
 b08:	f9e7ba909f 	{ ae_slliq56	aeq2, aeq1, 1; ae_roundsp24q48sym	aep2, aeq0 }
 b0d:	f9ea3ab09f 	{ ae_slliq56	aeq2, aeq3, 1; ae_addq56	aeq1, aeq1, aeq2 }
 b12:	fbfa3dc12f 	{ ae_lq56.i	aeq2, a1, 16; ae_addq56	aeq3, aeq3, aeq2 }
 b17:	1bac64        	ae_roundsp24q48sym	aep6, aeq2
 b1a:	01a174        	ae_mulfp24s.ll	aeq2, aep2, aep1
 b1d:	016174        	ae_mulfp24s.ll	aeq0, aep6, aep1
 b20:	1bac24        	ae_roundsp24q48sym	aep2, aeq2
 b23:	f9e7d8966f 	{ ae_cvtp24a16x2.ll	aep1, a6, a6; ae_roundsp24q48sym	aep3, aeq0 }
 b28:	eb2d98bfff 	{ ae_cvtp24a16x2.ll	aep3, a15, a15; ae_mulfp24s.ll	aeq2, aep3, aep1 }
 b2d:	012374        	ae_mulfp24s.ll	aeq0, aep2, aep3
 b30:	1bac64        	ae_roundsp24q48sym	aep6, aeq2
 b33:	f9e7bc4def 	{ ae_lq32f.xu	aeq2, a13, a14; ae_roundsp24q48sym	aep2, aeq0 }
 b38:	08a5a4        	ae_mulfq32sp24s.l	aeq2, aeq2, aep2
 b3b:	f9ffbc0def 	{ ae_lq32f.xu	aeq0, a13, a14; ae_roundsp24q48sym	aep2, aeq3 }
 b40:	08e324        	ae_mulafq32sp24s.l	aeq2, aeq0, aep6
 b43:	ebe1fc0def 	{ ae_lq32f.xu	aeq0, a13, a14; ae_mulfp24s.ll	aeq3, aep0, aep7 }
 b48:	08a324        	ae_mulafq32sp24s.l	aeq2, aeq0, aep2
 b4b:	1bbc64        	ae_roundsp24q48sym	aep6, aeq3
 b4e:	014674        	ae_mulfp24s.ll	aeq0, aep4, aep6
 b51:	01cd74        	ae_mulfp24s.ll	aeq3, aep4, aep5
 b54:	fbefbc2def 	{ ae_lq32f.xu	aeq1, a13, a14; ae_roundsp24q48sym	aep6, aeq1 }
 b59:	08e364        	ae_mulafq32sp24s.l	aeq2, aeq1, aep6
 b5c:	1bbc74        	ae_roundsp24q48sym	aep7, aeq3
 b5f:	1b8c24        	ae_roundsp24q48sym	aep2, aeq0
 b62:	012374        	ae_mulfp24s.ll	aeq0, aep2, aep3
 b65:	01f974        	ae_mulfp24s.ll	aeq3, aep7, aep1
 b68:	1b8c44        	ae_roundsp24q48sym	aep4, aeq0
 b6b:	fafffc2def 	{ ae_lq32f.xu	aeq1, a13, a14; ae_roundsp24q48sym	aep5, aeq3 }
 b70:	f5d7bc6def 	{ ae_lq32f.xu	aeq3, a13, a14; ae_mulafq32sp24s.l	aeq2, aeq1, aep5 }
 b75:	08c3e4        	ae_mulafq32sp24s.l	aeq2, aeq3, aep4
 b78:	066834        	ae_roundsq32sym	aeq1, aeq2
 b7b:	621114        	ae_sq32f.i	aeq1, a1, 4
 b7e:	1128           	l32i.n	a2, a1, 4
 b80:	f01d           	retw.n

00000b82 <intp_filter_32b+0x112>:
 b82:	fd31b1        	l32r	a11, 48 (2000 <_end+0xf60>)
 b85:	306bb4        	ae_cvtp24a16x2.ll	aep6, a11, a11
 b88:	016874        	ae_mulfp24s.ll	aeq1, aep6, aep0
 b8b:	11a4c0        	slli	a10, a4, 4
 b8e:	1b9c54        	ae_roundsp24q48sym	aep5, aeq1
 b91:	01d074        	ae_mulfp24s.ll	aeq2, aep5, aep0
 b94:	36c734        	ae_cvtq48a32s	aeq3, a7
 b97:	f8aafab0df 	{ ae_slliq56	aeq3, aeq3, 1; ae_subq56	aeq0, aeq1, aeq2 }
 b9c:	fbbad8c66f 	{ ae_cvtp24a16x2.ll	aep4, a6, a6; ae_subq56	aeq3, aeq3, aeq2 }
 ba1:	01c834        	ae_mulafp24s.ll	aeq3, aep4, aep0
 ba4:	f936d5a3af 	{ sub	a10, a3, a10; ae_subq56	aeq1, aeq2, aeq1 }
 ba9:	f9effc4a2f 	{ ae_lq32f.xu	aeq2, a10, a2; ae_roundsp24q48sym	aep3, aeq1 }
 bae:	08b5a4        	ae_mulfq32sp24s.l	aeq2, aeq2, aep3
 bb1:	1bbc24        	ae_roundsp24q48sym	aep2, aeq3
 bb4:	ff0c3c2a2f 	{ ae_lq32f.xu	aeq1, a10, a2; ae_cvtq48p24s.l	aeq3, aep0 }
 bb9:	08a364        	ae_mulafq32sp24s.l	aeq2, aeq1, aep2
 bbc:	01f8c4        	ae_addq56	aeq3, aeq3, aeq0
 bbf:	1bbc14        	ae_roundsp24q48sym	aep1, aeq3
 bc2:	060824        	ae_negq56	aeq0, aeq0
 bc5:	fbe7fc2a2f 	{ ae_lq32f.xu	aeq1, a10, a2; ae_roundsp24q48sym	aep7, aeq0 }
 bca:	f5c7bc6a2f 	{ ae_lq32f.xu	aeq3, a10, a2; ae_mulafq32sp24s.l	aeq2, aeq1, aep1 }
 bcf:	08f3e4        	ae_mulafq32sp24s.l	aeq2, aeq3, aep7
 bd2:	066834        	ae_roundsq32sym	aeq1, aeq2
 bd5:	621114        	ae_sq32f.i	aeq1, a1, 4
 bd8:	1128           	l32i.n	a2, a1, 4
 bda:	f01d           	retw.n

00000bdc <intp_filter_32b+0x16c>:
 bdc:	11c4d0        	slli	a12, a4, 3
 bdf:	c0c3c0        	sub	a12, a3, a12
 be2:	607c24        	ae_lq32f.xu	aeq1, a12, a2
 be5:	f8c1fc4c2f 	{ ae_lq32f.xu	aeq2, a12, a2; ae_mulsfq32sp24s.l	aeq1, aeq1, aep0 }
 bea:	080ba4        	ae_mulafq32sp24s.l	aeq1, aeq2, aep0
 bed:	061834        	ae_roundsq32sym	aeq0, aeq1
 bf0:	620114        	ae_sq32f.i	aeq0, a1, 4
 bf3:	1128           	l32i.n	a2, a1, 4
 bf5:	f01d           	retw.n

00000bf7 <intp_filter_32b+0x187>:
	...

00000bf8 <__udivdi3>:
 bf8:	008136        	entry	a1, 64
 bfb:	049d           	mov.n	a9, a4
 bfd:	02ad           	mov.n	a10, a2
 bff:	038d           	mov.n	a8, a3
 c01:	0b2c           	movi.n	a11, 32
 c03:	09b516        	beqz	a5, ca2 <__udivdi3+0xaa>
 c06:	06b357        	bgeu	a3, a5, c10 <__udivdi3+0x18>
 c09:	020c           	movi.n	a2, 0
 c0b:	030c           	movi.n	a3, 0
 c0d:	f01d           	retw.n

00000c0f <__udivdi3+0x17>:
	...

00000c10 <__udivdi3+0x18>:
 c10:	40f540        	nsau	a4, a5
 c13:	1d4416        	beqz	a4, deb <__udivdi3+0x1f3>
 c16:	c02b40        	sub	a2, a11, a4
 c19:	400200        	ssr	a2
 c1c:	91f030        	srl	a15, a3
 c1f:	401400        	ssl	a4
 c22:	81c3a0        	src	a12, a3, a10
 c25:	81b590        	src	a11, a5, a9
 c28:	9b8d           	ae_zext16	a8, a11
 c2a:	f5d0b0        	extui	a13, a11, 16, 16
 c2d:	404010        	ssai	16
 c30:	c2efd0        	quou	a14, a15, a13
 c33:	0e5d           	mov.n	a5, a14
 c35:	8238e0        	mull	a3, a8, a14
 c38:	e2ffd0        	remu	a15, a15, a13
 c3b:	81ffc0        	src	a15, a15, a12
 c3e:	0f2d           	mov.n	a2, a15
 c40:	0ebf37        	bgeu	a15, a3, c52 <__udivdi3+0x5a>
 c43:	5e0b           	addi.n	a5, a14, -1
 c45:	2bfa           	add.n	a2, a11, a15
 c47:	0732b7        	bltu	a2, a11, c52 <__udivdi3+0x5a>
 c4a:	04b237        	bgeu	a2, a3, c52 <__udivdi3+0x5a>
 c4d:	2b2a           	add.n	a2, a11, a2
 c4f:	fece52        	addi	a5, a14, -2
 c52:	116500        	slli	a6, a5, 16
 c55:	401400        	ssl	a4
 c58:	c03230        	sub	a3, a2, a3
 c5b:	c2e3d0        	quou	a14, a3, a13
 c5e:	0e2d           	mov.n	a2, a14
 c60:	a15900        	sll	a5, a9
 c63:	8288e0        	mull	a8, a8, a14
 c66:	e233d0        	remu	a3, a3, a13
 c69:	0bf3c0        	depbits	a12, a3, 16, 16
 c6c:	0c3d           	mov.n	a3, a12
 c6e:	0ebc87        	bgeu	a12, a8, c80 <__udivdi3+0x88>
 c71:	2e0b           	addi.n	a2, a14, -1
 c73:	3bca           	add.n	a3, a11, a12
 c75:	0733b7        	bltu	a3, a11, c80 <__udivdi3+0x88>
 c78:	04b387        	bgeu	a3, a8, c80 <__udivdi3+0x88>
 c7b:	3b3a           	add.n	a3, a11, a3
 c7d:	fece22        	addi	a2, a14, -2
 c80:	c03380        	sub	a3, a3, a8
 c83:	202260        	or	a2, a2, a6
 c86:	828250        	mull	a8, a2, a5
 c89:	a25250        	muluh	a5, a2, a5
 c8c:	0b3357        	bltu	a3, a5, c9b <__udivdi3+0xa3>
 c8f:	c09350        	sub	a9, a3, a5
 c92:	07f956        	bnez	a9, d15 <__udivdi3+0x11d>
 c95:	a1ba00        	sll	a11, a10
 c98:	79bb87        	bgeu	a11, a8, d15 <__udivdi3+0x11d>

00000c9b <__udivdi3+0xa3>:
 c9b:	220b           	addi.n	a2, a2, -1
 c9d:	030c           	movi.n	a3, 0
 c9f:	f01d           	retw.n

00000ca1 <__udivdi3+0xa9>:
	...

00000ca2 <__udivdi3+0xaa>:
 ca2:	75b347        	bgeu	a3, a4, d1b <__udivdi3+0x123>
 ca5:	40f450        	nsau	a5, a4
 ca8:	a58c           	beqz.n	a5, cb6 <__udivdi3+0xbe>
 caa:	401500        	ssl	a5
 cad:	a14400        	sll	a4, a4
 cb0:	813320        	src	a3, a3, a2
 cb3:	a12200        	sll	a2, a2

00000cb6 <__udivdi3+0xbe>:
 cb6:	945d           	ae_zext16	a5, a4
 cb8:	404010        	ssai	16
 cbb:	f58040        	extui	a8, a4, 16, 16
 cbe:	e2c380        	remu	a12, a3, a8
 cc1:	c2b380        	quou	a11, a3, a8
 cc4:	0bad           	mov.n	a10, a11
 cc6:	81cc20        	src	a12, a12, a2
 cc9:	8295b0        	mull	a9, a5, a11
 ccc:	0c3d           	mov.n	a3, a12
 cce:	0ebc97        	bgeu	a12, a9, ce0 <__udivdi3+0xe8>
 cd1:	ab0b           	addi.n	a10, a11, -1
 cd3:	3c4a           	add.n	a3, a12, a4
 cd5:	073347        	bltu	a3, a4, ce0 <__udivdi3+0xe8>
 cd8:	04b397        	bgeu	a3, a9, ce0 <__udivdi3+0xe8>
 cdb:	343a           	add.n	a3, a4, a3
 cdd:	fecba2        	addi	a10, a11, -2
 ce0:	c0d390        	sub	a13, a3, a9
 ce3:	c29d80        	quou	a9, a13, a8
 ce6:	093d           	mov.n	a3, a9
 ce8:	825590        	mull	a5, a5, a9
 ceb:	e2dd80        	remu	a13, a13, a8
 cee:	0bfd20        	depbits	a2, a13, 16, 16
 cf1:	16b257        	bgeu	a2, a5, d0b <__udivdi3+0x113>
 cf4:	390b           	addi.n	a3, a9, -1
 cf6:	224a           	add.n	a2, a2, a4
 cf8:	0f3247        	bltu	a2, a4, d0b <__udivdi3+0x113>
 cfb:	0cb257        	bgeu	a2, a5, d0b <__udivdi3+0x113>
 cfe:	112a00        	slli	a2, a10, 16
 d01:	fec932        	addi	a3, a9, -2
 d04:	202320        	or	a2, a3, a2
 d07:	030c           	movi.n	a3, 0
 d09:	f01d           	retw.n

00000d0b <__udivdi3+0x113>:
 d0b:	112a00        	slli	a2, a10, 16
 d0e:	202320        	or	a2, a3, a2
 d11:	030c           	movi.n	a3, 0
 d13:	f01d           	retw.n

00000d15 <__udivdi3+0x11d>:
 d15:	030c           	movi.n	a3, 0
 d17:	f01d           	retw.n

00000d19 <__udivdi3+0x121>:
	...

00000d1b <__udivdi3+0x123>:
 d1b:	130c           	movi.n	a3, 1
 d1d:	c24340        	quou	a4, a3, a4
 d20:	839490        	moveqz	a9, a4, a9
 d23:	094d           	mov.n	a4, a9
 d25:	40f950        	nsau	a5, a9
 d28:	0dc516        	beqz	a5, e08 <__udivdi3+0x210>
 d2b:	401500        	ssl	a5
 d2e:	c0eb50        	sub	a14, a11, a5
 d31:	a14900        	sll	a4, a9
 d34:	400e00        	ssr	a14
 d37:	91d080        	srl	a13, a8
 d3a:	401500        	ssl	a5
 d3d:	813820        	src	a3, a8, a2
 d40:	945d           	ae_zext16	a5, a4
 d42:	a12200        	sll	a2, a2
 d45:	f58040        	extui	a8, a4, 16, 16
 d48:	404010        	ssai	16
 d4b:	c2cd80        	quou	a12, a13, a8
 d4e:	0cbd           	mov.n	a11, a12
 d50:	82a5c0        	mull	a10, a5, a12
 d53:	e2dd80        	remu	a13, a13, a8
 d56:	81dd30        	src	a13, a13, a3
 d59:	0d9d           	mov.n	a9, a13
 d5b:	0ebda7        	bgeu	a13, a10, d6d <__udivdi3+0x175>
 d5e:	bc0b           	addi.n	a11, a12, -1
 d60:	9d4a           	add.n	a9, a13, a4
 d62:	073947        	bltu	a9, a4, d6d <__udivdi3+0x175>
 d65:	04b9a7        	bgeu	a9, a10, d6d <__udivdi3+0x175>
 d68:	949a           	add.n	a9, a4, a9
 d6a:	feccb2        	addi	a11, a12, -2
 d6d:	c0d9a0        	sub	a13, a9, a10
 d70:	e2ed80        	remu	a14, a13, a8
 d73:	0bfe30        	depbits	a3, a14, 16, 16
 d76:	c2dd80        	quou	a13, a13, a8
 d79:	0dad           	mov.n	a10, a13
 d7b:	82c5d0        	mull	a12, a5, a13
 d7e:	039d           	mov.n	a9, a3
 d80:	0eb3c7        	bgeu	a3, a12, d92 <__udivdi3+0x19a>
 d83:	ad0b           	addi.n	a10, a13, -1
 d85:	934a           	add.n	a9, a3, a4
 d87:	073947        	bltu	a9, a4, d92 <__udivdi3+0x19a>
 d8a:	04b9c7        	bgeu	a9, a12, d92 <__udivdi3+0x19a>
 d8d:	949a           	add.n	a9, a4, a9
 d8f:	fecda2        	addi	a10, a13, -2
 d92:	c039c0        	sub	a3, a9, a12
 d95:	11fb00        	slli	a15, a11, 16
 d98:	20aaf0        	or	a10, a10, a15

00000d9b <__udivdi3+0x1a3>:
 d9b:	404010        	ssai	16
 d9e:	e2d380        	remu	a13, a3, a8
 da1:	c2c380        	quou	a12, a3, a8
 da4:	0cbd           	mov.n	a11, a12
 da6:	8295c0        	mull	a9, a5, a12
 da9:	81dd20        	src	a13, a13, a2
 dac:	0d3d           	mov.n	a3, a13
 dae:	0ebd97        	bgeu	a13, a9, dc0 <__udivdi3+0x1c8>
 db1:	bc0b           	addi.n	a11, a12, -1
 db3:	3d4a           	add.n	a3, a13, a4
 db5:	073347        	bltu	a3, a4, dc0 <__udivdi3+0x1c8>
 db8:	04b397        	bgeu	a3, a9, dc0 <__udivdi3+0x1c8>
 dbb:	343a           	add.n	a3, a4, a3
 dbd:	feccb2        	addi	a11, a12, -2
 dc0:	c0e390        	sub	a14, a3, a9
 dc3:	c29e80        	quou	a9, a14, a8
 dc6:	093d           	mov.n	a3, a9
 dc8:	825590        	mull	a5, a5, a9
 dcb:	e2ee80        	remu	a14, a14, a8
 dce:	0bfe20        	depbits	a2, a14, 16, 16
 dd1:	23b257        	bgeu	a2, a5, df8 <__udivdi3+0x200>
 dd4:	390b           	addi.n	a3, a9, -1
 dd6:	224a           	add.n	a2, a2, a4
 dd8:	1c3247        	bltu	a2, a4, df8 <__udivdi3+0x200>
 ddb:	19b257        	bgeu	a2, a5, df8 <__udivdi3+0x200>
 dde:	113b00        	slli	a3, a11, 16
 de1:	fec922        	addi	a2, a9, -2
 de4:	202230        	or	a2, a2, a3
 de7:	0a3d           	mov.n	a3, a10
 de9:	f01d           	retw.n

00000deb <__udivdi3+0x1f3>:
 deb:	023537        	bltu	a5, a3, df1 <__udivdi3+0x1f9>
 dee:	103297        	bltu	a2, a9, e02 <__udivdi3+0x20a>

00000df1 <__udivdi3+0x1f9>:
 df1:	120c           	movi.n	a2, 1
 df3:	030c           	movi.n	a3, 0
 df5:	f01d           	retw.n

00000df7 <__udivdi3+0x1ff>:
	...

00000df8 <__udivdi3+0x200>:
 df8:	112b00        	slli	a2, a11, 16
 dfb:	202320        	or	a2, a3, a2
 dfe:	0a3d           	mov.n	a3, a10
 e00:	f01d           	retw.n

00000e02 <__udivdi3+0x20a>:
 e02:	020c           	movi.n	a2, 0
 e04:	030c           	movi.n	a3, 0
 e06:	f01d           	retw.n

00000e08 <__udivdi3+0x210>:
 e08:	03ad           	mov.n	a10, a3
 e0a:	995d           	ae_zext16	a5, a9
 e0c:	c03890        	sub	a3, a8, a9
 e0f:	f58090        	extui	a8, a9, 16, 16
 e12:	ffe146        	j	d9b <__udivdi3+0x1a3>

00000e15 <__udivdi3+0x21d>:
 e15:	000000                                        ...

00000e18 <_fini>:
 e18:	008136        	entry	a1, 64

00000e1b <_fini+0x3>:
 e1b:	f01d           	retw.n
