
Build/lib/pisplit_msbc_dec_position_independent.splitload.lib:     file format elf32-xtensa-le


Disassembly of section .text:

00000000 <_init-0xa0>:
       0:	00002950 000018e0 0000028c 000000c8     P)..............
      10:	00002940 00000240 0000029c 000003e4     @)..@...........
      20:	0000036c 000009b4 00000b90 00000cf8     l...............
      30:	00001190 00000250 00000a10 00002830     ....P.......0(..
      40:	00009c00 0000ad00 00000304 000013b0     ................
      50:	00000c10 000007d8 000005b8 000009e8     ................
      60:	00000500 00005a00 00001600 00001a60     .....Z......`...
      70:	00001de0 00001a20 00002c00 00001be0     .... ....,......
      80:	00002150 00001ae0 00000acc 00001930     P!..........0...
      90:	00001910 000019d0 00001168 0000117c     ........h...|...

000000a0 <_init>:
      a0:	008136        	entry	a1, 64
      a3:	f01d           	retw.n

000000a5 <_init+0x5>:
      a5:	000000                                        ...

000000a8 <_start>:
      a8:	004136        	entry	a1, 32
      ab:	ffd6a1        	l32r	a10, 4 (18e0 <_DYNAMIC+0x318>)
      ae:	ffd6b1        	l32r	a11, 8 (28c <msbc_dec_init>)
      b1:	ffd391        	l32r	a9, 0 (2950 <__bss_start>)
      b4:	002282        	l32i	a8, a2, 0
      b7:	ffd5c1        	l32r	a12, c (c8 <msbc_dec_process>)
      ba:	006982        	s32i	a8, a9, 0
      bd:	0008e0        	callx8	a8
      c0:	ffd421        	l32r	a2, 10 (2940 <export_parameter_array>)
      c3:	f01d           	retw.n

000000c5 <_start+0x1d>:
      c5:	000000                                        ...

000000c8 <msbc_dec_process>:
      c8:	006136        	entry	a1, 48
      cb:	049d           	mov.n	a9, a4
      cd:	060c           	movi.n	a6, 0
      cf:	054d           	mov.n	a4, a5
      d1:	006192        	s32i	a9, a1, 0
      d4:	205220        	or	a5, a2, a2
      d7:	1da022        	movi	a2, 29

000000da <msbc_dec_process+0x12>:
      da:	ffce81        	l32r	a8, 14 (240 <msbc_change_endian16>)
      dd:	907630        	addx2	a7, a6, a3
      e0:	37ad           	ae_l16si.n	a10, a7, 2
      e2:	0008e0        	callx8	a8
      e5:	67ad           	ae_s16i.n	a10, a7, 0
      e7:	661b           	addi.n	a6, a6, 1
      e9:	966d           	ae_zext16	a6, a6
      eb:	eb9627        	bne	a6, a2, da <msbc_dec_process+0x12>
      ee:	260c           	movi.n	a6, 2

000000f0 <msbc_dec_process+0x28>:
      f0:	ffc981        	l32r	a8, 14 (240 <msbc_change_endian16>)
      f3:	907630        	addx2	a7, a6, a3
      f6:	27ad           	ae_l16si.n	a10, a7, 0
      f8:	0008e0        	callx8	a8
      fb:	67ad           	ae_s16i.n	a10, a7, 0
      fd:	661b           	addi.n	a6, a6, 1
      ff:	966d           	ae_zext16	a6, a6
     101:	eb9627        	bne	a6, a2, f0 <msbc_dec_process+0x28>
     104:	870c           	movi.n	a7, 8
     106:	23bd           	ae_l16si.n	a11, a3, 0
     108:	538d           	ae_l16ui.n	a8, a3, 2
     10a:	c32b           	addi.n	a12, a3, 2
     10c:	38c3a2        	addi	a10, a3, 56
     10f:	04d592        	addmi	a9, a5, 0x400
     112:	020c           	movi.n	a2, 0
     114:	032624        	wur.ae_bitptr	a2
     117:	84c962        	addi	a6, a9, -124
     11a:	83a340        	moveqz	a10, a3, a4
     11d:	1b69c2        	s32i	a12, a9, 108
     120:	2769a2        	s32i	a10, a9, 156
     123:	0bfb80        	depbits	a8, a11, 16, 16
     126:	f3f180        	wur.ae_bithead	a8
     129:	10b416        	beqz	a4, 238 <msbc_dec_process+0x170>
     12c:	0138           	l32i.n	a3, a1, 0
     12e:	ea2b           	addi.n	a14, a10, 2
     130:	4add           	ae_l16ui.n	a13, a10, 0
     132:	2769e2        	s32i	a14, a9, 156
     135:	757d           	ae_s16i.n	a7, a5, 2
     137:	11dd80        	slli	a13, a13, 8
     13a:	65dd           	ae_s16i.n	a13, a5, 0

0000013c <msbc_dec_process+0x74>:
     13c:	ffb781        	l32r	a8, 18 (29c <msbc_get_byte>)
     13f:	05ad           	mov.n	a10, a5
     141:	0008e0        	callx8	a8
     144:	78a042        	movi	a4, 120
     147:	ada092        	movi	a9, 173
     14a:	7a9a97        	bne	a10, a9, 1c8 <msbc_dec_process+0x100>
     14d:	ffb281        	l32r	a8, 18 (29c <msbc_get_byte>)
     150:	05ad           	mov.n	a10, a5
     152:	0008e0        	callx8	a8
     155:	ffb081        	l32r	a8, 18 (29c <msbc_get_byte>)
     158:	0955a2        	s16i	a10, a5, 18
     15b:	05ad           	mov.n	a10, a5
     15d:	0008e0        	callx8	a8
     160:	ffae81        	l32r	a8, 18 (29c <msbc_get_byte>)
     163:	0a55a2        	s16i	a10, a5, 20
     166:	05ad           	mov.n	a10, a5
     168:	0008e0        	callx8	a8
     16b:	ffac81        	l32r	a8, 1c (3e4 <sbc_extract_scale_factors_non_joint>)
     16e:	e39f10        	rur.ae_bithead	a9
     171:	055522        	s16i	a2, a5, 10
     174:	045522        	s16i	a2, a5, 8
     177:	025522        	s16i	a2, a5, 4
     17a:	065572        	s16i	a7, a5, 12
     17d:	9b3c           	movi.n	a11, 57
     17f:	fc0c           	movi.n	a12, 15
     181:	1d0c           	movi.n	a13, 1
     183:	1655a2        	s16i	a10, a5, 44
     186:	ae1c           	movi.n	a14, 26
     188:	00a2c4        	rur.ae_bitptr	a10
     18b:	0855e2        	s16i	a14, a5, 16
     18e:	0755d2        	s16i	a13, a5, 14
     191:	0355c2        	s16i	a12, a5, 6
     194:	8a56b2        	s16i	a11, a6, 0x114
     197:	1b5542        	s16i	a4, a5, 54
     19a:	1a5572        	s16i	a7, a5, 52
     19d:	3c66a2        	s32i	a10, a6, 240
     1a0:	3b6692        	s32i	a9, a6, 236
     1a3:	05ad           	mov.n	a10, a5
     1a5:	0008e0        	callx8	a8
     1a8:	ff9e81        	l32r	a8, 20 (36c <sbc_crc8>)
     1ab:	12c5a2        	addi	a10, a5, 18
     1ae:	1515b2        	l16ui	a11, a5, 42
     1b1:	0008e0        	callx8	a8
     1b4:	161592        	l16ui	a9, a5, 44
     1b7:	6819a7        	beq	a9, a10, 223 <msbc_dec_process+0x15b>
     1ba:	ff9a81        	l32r	a8, 24 (9b4 <sbc_zero_sb_samples>)
     1bd:	05ad           	mov.n	a10, a5
     1bf:	0008e0        	callx8	a8
     1c2:	120c           	movi.n	a2, 1
     1c4:	000c86        	j	1fa <msbc_dec_process+0x132>

000001c7 <msbc_dec_process+0xff>:
	...

000001c8 <msbc_dec_process+0x100>:
     1c8:	ff9781        	l32r	a8, 24 (9b4 <sbc_zero_sb_samples>)
     1cb:	05ad           	mov.n	a10, a5
     1cd:	0008e0        	callx8	a8
     1d0:	120c           	movi.n	a2, 1
     1d2:	065572        	s16i	a7, a5, 12
     1d5:	9a3c           	movi.n	a10, 57
     1d7:	ab1c           	movi.n	a11, 26
     1d9:	1c0c           	movi.n	a12, 1
     1db:	fd0c           	movi.n	a13, 15
     1dd:	0e0c           	movi.n	a14, 0
     1df:	0255e2        	s16i	a14, a5, 4
     1e2:	0455e2        	s16i	a14, a5, 8
     1e5:	0555e2        	s16i	a14, a5, 10
     1e8:	0355d2        	s16i	a13, a5, 6
     1eb:	0755c2        	s16i	a12, a5, 14
     1ee:	0855b2        	s16i	a11, a5, 16
     1f1:	8a56a2        	s16i	a10, a6, 0x114
     1f4:	1a5572        	s16i	a7, a5, 52
     1f7:	1b5542        	s16i	a4, a5, 54

000001fa <msbc_dec_process+0x132>:
     1fa:	05cd           	mov.n	a12, a5
     1fc:	0a0c           	movi.n	a10, 0
     1fe:	03bd           	mov.n	a11, a3
     200:	05d592        	addmi	a9, a5, 0x500
     203:	a0c992        	addi	a9, a9, -96
     206:	0bd532        	addmi	a3, a5, 0xb00
     209:	531382        	l16ui	a8, a3, 166
     20c:	525382        	s16i	a8, a3, 164
     20f:	ff8681        	l32r	a8, 28 (b90 <SBC_synthesis8_per_channel>)
     212:	286392        	s32i	a9, a3, 160
     215:	a4c332        	addi	a3, a3, -92
     218:	0008e0        	callx8	a8
     21b:	8013a2        	l16ui	a10, a3, 0x100
     21e:	8153a2        	s16i	a10, a3, 0x102
     221:	f01d           	retw.n

00000223 <msbc_dec_process+0x15b>:
     223:	ff8281        	l32r	a8, 2c (cf8 <sbc_bit_allocation>)
     226:	05ad           	mov.n	a10, a5
     228:	0008e0        	callx8	a8
     22b:	ff8181        	l32r	a8, 30 (1190 <sbc_reconstruction>)
     22e:	05ad           	mov.n	a10, a5
     230:	0008e0        	callx8	a8
     233:	020c           	movi.n	a2, 0
     235:	fff046        	j	1fa <msbc_dec_process+0x132>

00000238 <msbc_dec_process+0x170>:
     238:	0138           	l32i.n	a3, a1, 0
     23a:	752d           	ae_s16i.n	a2, a5, 2
     23c:	ffbf06        	j	13c <msbc_dec_process+0x74>

0000023f <msbc_dec_process+0x177>:
	...

00000240 <msbc_change_endian16>:
     240:	004136        	entry	a1, 32
     243:	743820        	extui	a3, a2, 8, 8
     246:	8a7230        	depbits	a3, a2, 8, 8
     249:	832d           	ae_sext16	a2, a3
     24b:	f01d           	retw.n

0000024d <msbc_change_endian16+0xd>:
     24d:	000000                                        ...

00000250 <mSBC_memory_clear>:
     250:	004136        	entry	a1, 32
     253:	090c           	movi.n	a9, 0
     255:	a0a462        	movi	a6, 0x4a0
     258:	208220        	or	a8, a2, a2
     25b:	00a132        	movi	a3, 0x100
     25e:	00a022        	movi	a2, 0
     261:	098376        	loop	a3, 26e <mSBC_memory_clear+0x1e>
     264:	a03280        	addx4	a3, a2, a8
     267:	221b           	addi.n	a2, a2, 1
     269:	166392        	s32i	a9, a3, 88
     26c:	922d           	ae_zext16	a2, a2

0000026e <mSBC_memory_clear+0x1e>:
     26e:	020c           	movi.n	a2, 0
     270:	a0a032        	movi	a3, 160
     273:	f03d           	nop.n
     275:	0a8376        	loop	a3, 283 <mSBC_memory_clear+0x33>
     278:	a03280        	addx4	a3, a2, a8
     27b:	221b           	addi.n	a2, a2, 1
     27d:	336a           	add.n	a3, a3, a6
     27f:	922d           	ae_zext16	a2, a2
     281:	0399           	s32i.n	a9, a3, 0
     283:	0bd832        	addmi	a3, a8, 0xb00
     286:	535392        	s16i	a9, a3, 166
     289:	f01d           	retw.n

0000028b <mSBC_memory_clear+0x3b>:
	...

0000028c <msbc_dec_init>:
     28c:	004136        	entry	a1, 32
     28f:	ff6981        	l32r	a8, 34 (250 <mSBC_memory_clear>)
     292:	20a220        	or	a10, a2, a2
     295:	0008e0        	callx8	a8
     298:	f01d           	retw.n

0000029a <msbc_dec_init+0xe>:
	...

0000029c <msbc_get_byte>:
     29c:	004136        	entry	a1, 32
     29f:	850c           	movi.n	a5, 8
     2a1:	02d242        	addmi	a4, a2, 0x200
     2a4:	9b2432        	l32i	a3, a4, 0x26c
     2a7:	032754        	ae_lb	a2, a5
     2aa:	1c1354        	ae_db	a3, a5
     2ad:	922d           	ae_zext16	a2, a2
     2af:	9b6432        	s32i	a3, a4, 0x26c
     2b2:	f01d           	retw.n

000002b4 <msbc_get_byte+0x18>:
     2b4:	00000000                                ....

000002b8 <sbc_init>:
     2b8:	004136        	entry	a1, 32
     2bb:	090c           	movi.n	a9, 0
     2bd:	e4a362        	movi	a6, 0x3e4
     2c0:	208220        	or	a8, a2, a2
     2c3:	00a132        	movi	a3, 0x100
     2c6:	00a022        	movi	a2, 0
     2c9:	098376        	loop	a3, 2d6 <sbc_init+0x1e>
     2cc:	a03280        	addx4	a3, a2, a8
     2cf:	221b           	addi.n	a2, a2, 1
     2d1:	166392        	s32i	a9, a3, 88
     2d4:	922d           	ae_zext16	a2, a2

000002d6 <sbc_init+0x1e>:
     2d6:	020c           	movi.n	a2, 0
     2d8:	0bd8a2        	addmi	a10, a8, 0xb00
     2db:	a0a032        	movi	a3, 160
     2de:	0e8376        	loop	a3, 2f0 <sbc_init+0x38>
     2e1:	a03280        	addx4	a3, a2, a8
     2e4:	221b           	addi.n	a2, a2, 1
     2e6:	336a           	add.n	a3, a3, a6
     2e8:	922d           	ae_zext16	a2, a2
     2ea:	2f6392        	s32i	a9, a3, 188
     2ed:	cf6392        	s32i	a9, a3, 0x33c
     2f0:	04d832        	addmi	a3, a8, 0x400
     2f3:	00a222        	movi	a2, 0x200
     2f6:	789d           	ae_s16i.n	a9, a8, 2
     2f8:	535a92        	s16i	a9, a10, 166
     2fb:	545a92        	s16i	a9, a10, 168
     2fe:	4c5322        	s16i	a2, a3, 152
     301:	f01d           	retw.n

00000303 <sbc_init+0x4b>:
	...

00000304 <sbc_get_bytes>:
     304:	004136        	entry	a1, 32
     307:	028d           	mov.n	a8, a2
     309:	430b           	addi.n	a4, a3, -1
     30b:	090c           	movi.n	a9, 0
     30d:	0cd262        	addmi	a6, a2, 0xc00
     310:	04d2a2        	addmi	a10, a2, 0x400
     313:	10c662        	addi	a6, a6, 16
     316:	276a62        	s32i	a6, a10, 156
     319:	ff47a1        	l32r	a10, 38 (a10 <SBC_synthesis4_per_channel+0x28>)
     31c:	729d           	ae_s16i.n	a9, a2, 2
     31e:	049030        	extui	a9, a3, 0, 1
     321:	933490        	movnez	a3, a4, a9
     324:	933d           	ae_zext16	a3, a3
     326:	63ac           	beqz.n	a3, 350 <sbc_get_bytes+0x4c>
     328:	0ad872        	addmi	a7, a8, 0xa00
     32b:	01c322        	addi	a2, a3, 1
     32e:	212120        	srai	a2, a2, 1
     331:	1b9276        	loopnez	a2, 350 <sbc_get_bytes+0x4c>
     334:	832722        	l32i	a2, a7, 0x20c
     337:	000232        	l8ui	a3, a2, 0
     33a:	421b           	addi.n	a4, a2, 1
     33c:	522b           	addi.n	a5, a2, 2
     33e:	836742        	s32i	a4, a7, 0x20c
     341:	010222        	l8ui	a2, a2, 1
     344:	113380        	slli	a3, a3, 8
     347:	836752        	s32i	a5, a7, 0x20c
     34a:	202230        	or	a2, a2, a3
     34d:	09c624        	ae_s16ip	a2, a6, 2

00000350 <sbc_get_bytes+0x4c>:
     350:	499c           	beqz.n	a9, 368 <sbc_get_bytes+0x64>
     352:	98aa           	add.n	a9, a8, a10
     354:	7f2942        	l32i	a4, a9, 0x1fc
     357:	020c           	movi.n	a2, 0
     359:	000432        	l8ui	a3, a4, 0
     35c:	441b           	addi.n	a4, a4, 1
     35e:	7f6942        	s32i	a4, a9, 0x1fc
     361:	113380        	slli	a3, a3, 8
     364:	663d           	ae_s16i.n	a3, a6, 0
     366:	f01d           	retw.n

00000368 <sbc_get_bytes+0x64>:
     368:	020c           	movi.n	a2, 0
     36a:	f01d           	retw.n

0000036c <sbc_crc8>:
     36c:	004136        	entry	a1, 32
     36f:	070c           	movi.n	a7, 0
     371:	025d           	mov.n	a5, a2
     373:	038d           	mov.n	a8, a3
     375:	413330        	srli	a3, a3, 3
     378:	05f316        	beqz	a3, 3db <sbc_crc8+0x6f>
     37b:	ff3061        	l32r	a6, 3c (2830 <SBC_crc_table>)
     37e:	f40c           	movi.n	a4, 15
     380:	020c           	movi.n	a2, 0
     382:	249376        	loopnez	a3, 3aa <sbc_crc8+0x3e>
     385:	903250        	addx2	a3, a2, a5
     388:	433d           	ae_l16ui.n	a3, a3, 0
     38a:	221b           	addi.n	a2, a2, 1
     38c:	922d           	ae_zext16	a2, a2
     38e:	303340        	xor	a3, a3, a4
     391:	414130        	srli	a4, a3, 1
     394:	043030        	extui	a3, a3, 0, 1
     397:	1133d0        	slli	a3, a3, 3
     39a:	904460        	addx2	a4, a4, a6
     39d:	444d           	ae_l16ui.n	a4, a4, 0
     39f:	933d           	ae_zext16	a3, a3
     3a1:	400300        	ssr	a3
     3a4:	b13040        	sra	a3, a4
     3a7:	744030        	extui	a4, a3, 0, 8

000003aa <sbc_crc8+0x3e>:
     3aa:	023d           	mov.n	a3, a2

000003ac <sbc_crc8+0x40>:
     3ac:	80a062        	movi	a6, 128
     3af:	242080        	extui	a2, a8, 0, 3
     3b2:	903350        	addx2	a3, a3, a5
     3b5:	433d           	ae_l16ui.n	a3, a3, 0
     3b7:	d81c           	movi.n	a8, 29
     3b9:	1aa276        	loopgtz	a2, 3d7 <sbc_crc8+0x6b>
     3bc:	072d           	mov.n	a2, a7
     3be:	305340        	xor	a5, a3, a4
     3c1:	644040        	extui	a4, a4, 0, 7
     3c4:	1144f0        	slli	a4, a4, 1
     3c7:	105560        	and	a5, a5, a6
     3ca:	1133f0        	slli	a3, a3, 1
     3cd:	855d           	ae_sext16	a5, a5
     3cf:	932850        	movnez	a2, a8, a5
     3d2:	302240        	xor	a2, a2, a4
     3d5:	924d           	ae_zext16	a4, a2

000003d7 <sbc_crc8+0x6b>:
     3d7:	042d           	mov.n	a2, a4
     3d9:	f01d           	retw.n

000003db <sbc_crc8+0x6f>:
     3db:	f40c           	movi.n	a4, 15
     3dd:	030c           	movi.n	a3, 0
     3df:	fff246        	j	3ac <sbc_crc8+0x40>

000003e2 <sbc_crc8+0x76>:
	...

000003e4 <sbc_extract_scale_factors_non_joint>:
     3e4:	004136        	entry	a1, 32
     3e7:	1a1272        	l16ui	a7, a2, 52
     3ea:	02d252        	addmi	a5, a2, 0x200
     3ed:	20a220        	or	a10, a2, a2
     3f0:	38ca42        	addi	a4, a10, 56
     3f3:	16ca62        	addi	a6, a10, 22
     3f6:	9d2592        	l32i	a9, a5, 0x274
     3f9:	9c2582        	l32i	a8, a5, 0x270
     3fc:	f3f180        	wur.ae_bithead	a8
     3ff:	032694        	wur.ae_bitptr	a9
     402:	74c582        	addi	a8, a5, 116
     405:	417170        	srli	a7, a7, 1
     408:	370b           	addi.n	a3, a7, -1
     40a:	832d           	ae_sext16	a2, a3
     40c:	043f30        	extui	a3, a3, 15, 1
     40f:	0d5356        	bnez	a3, 4e8 <sbc_extract_scale_factors_non_joint+0x104>
     412:	08a092        	movi	a9, 8
     415:	01c222        	addi	a2, a2, 1
     418:	1d6207        	bbci	a2, 0, 439 <sbc_extract_scale_factors_non_joint+0x55>
     41b:	03b794        	ae_lb	a11, a9
     41e:	3cca42        	addi	a4, a10, 60
     421:	9b25e2        	l32i	a14, a5, 0x26c
     424:	34c4b0        	extui	a12, a11, 4, 4
     427:	34d0b0        	extui	a13, a11, 0, 4
     42a:	1c1e94        	ae_db	a14, a9
     42d:	9b65e2        	s32i	a14, a5, 0x26c
     430:	1d5ad2        	s16i	a13, a10, 58
     433:	1c5ac2        	s16i	a12, a10, 56
     436:	09c6b4        	ae_s16ip	a11, a6, 2

00000439 <sbc_extract_scale_factors_non_joint+0x55>:
     439:	215120        	srai	a5, a2, 1
     43c:	08a516        	beqz	a5, 4ca <sbc_extract_scale_factors_non_joint+0xe6>
     43f:	03d794        	ae_lb	a13, a9
     442:	7e2822        	l32i	a2, a8, 0x1f8
     445:	34f4d0        	extui	a15, a13, 4, 4
     448:	1c1294        	ae_db	a2, a9
     44b:	34e0d0        	extui	a14, a13, 0, 4
     44e:	7e6822        	s32i	a2, a8, 0x1f8
     451:	032794        	ae_lb	a2, a9
     454:	74ed           	ae_s16i.n	a14, a4, 2
     456:	64fd           	ae_s16i.n	a15, a4, 0
     458:	09c6d4        	ae_s16ip	a13, a6, 2
     45b:	7e28c2        	l32i	a12, a8, 0x1f8
     45e:	34b420        	extui	a11, a2, 4, 4
     461:	343020        	extui	a3, a2, 0, 4
     464:	1c1c94        	ae_db	a12, a9
     467:	7e68c2        	s32i	a12, a8, 0x1f8
     46a:	035432        	s16i	a3, a4, 6
     46d:	0254b2        	s16i	a11, a4, 4
     470:	09c624        	ae_s16ip	a2, a6, 2
     473:	250b           	addi.n	a2, a5, -1
     475:	35a276        	loopgtz	a2, 4ae <sbc_extract_scale_factors_non_joint+0xca>
     478:	7e2822        	l32i	a2, a8, 0x1f8
     47b:	033794        	ae_lb	a3, a9
     47e:	448b           	addi.n	a4, a4, 8
     480:	1c1294        	ae_db	a2, a9
     483:	7e6822        	s32i	a2, a8, 0x1f8
     486:	342030        	extui	a2, a3, 0, 4
     489:	037794        	ae_lb	a7, a9
     48c:	345430        	extui	a5, a3, 4, 4
     48f:	742d           	ae_s16i.n	a2, a4, 2
     491:	645d           	ae_s16i.n	a5, a4, 0
     493:	09c634        	ae_s16ip	a3, a6, 2
     496:	7e2822        	l32i	a2, a8, 0x1f8
     499:	343470        	extui	a3, a7, 4, 4
     49c:	345070        	extui	a5, a7, 0, 4
     49f:	1c1294        	ae_db	a2, a9
     4a2:	7e6822        	s32i	a2, a8, 0x1f8
     4a5:	025432        	s16i	a3, a4, 4
     4a8:	035452        	s16i	a5, a4, 6
     4ab:	09c674        	ae_s16ip	a7, a6, 2

000004ae <sbc_extract_scale_factors_non_joint+0xca>:
     4ae:	1a1ac2        	l16ui	a12, a10, 52
     4b1:	00b2c4        	rur.ae_bitptr	a11
     4b4:	e39f10        	rur.ae_bithead	a9
     4b7:	41c1c0        	srli	a12, a12, 1
     4ba:	cc2b           	addi.n	a12, a12, 2
     4bc:	11ccd0        	slli	a12, a12, 3
     4bf:	155ac2        	s16i	a12, a10, 42
     4c2:	8068b2        	s32i	a11, a8, 0x200
     4c5:	7f6892        	s32i	a9, a8, 0x1fc
     4c8:	f01d           	retw.n

000004ca <sbc_extract_scale_factors_non_joint+0xe6>:
     4ca:	1a1ac2        	l16ui	a12, a10, 52
     4cd:	e39f10        	rur.ae_bithead	a9
     4d0:	00b2c4        	rur.ae_bitptr	a11
     4d3:	41c1c0        	srli	a12, a12, 1
     4d6:	cc2b           	addi.n	a12, a12, 2
     4d8:	11ccd0        	slli	a12, a12, 3
     4db:	155ac2        	s16i	a12, a10, 42
     4de:	8068b2        	s32i	a11, a8, 0x200
     4e1:	7f6892        	s32i	a9, a8, 0x1fc
     4e4:	f01d           	retw.n

000004e6 <sbc_extract_scale_factors_non_joint+0x102>:
	...

000004e8 <sbc_extract_scale_factors_non_joint+0x104>:
     4e8:	e3df10        	rur.ae_bithead	a13
     4eb:	00e2c4        	rur.ae_bitptr	a14
     4ee:	f72b           	addi.n	a15, a7, 2
     4f0:	11ffd0        	slli	a15, a15, 3
     4f3:	155af2        	s16i	a15, a10, 42
     4f6:	9d65e2        	s32i	a14, a5, 0x274
     4f9:	9c65d2        	s32i	a13, a5, 0x270
     4fc:	f01d           	retw.n

000004fe <sbc_extract_scale_factors_non_joint+0x11a>:
	...

00000500 <sbc_extract_scale_factors_joint_ch4>:
     500:	004136        	entry	a1, 32
     503:	02bd           	mov.n	a11, a2
     505:	1a1b52        	l16ui	a5, a11, 52
     508:	02d222        	addmi	a2, a2, 0x200
     50b:	74c292        	addi	a9, a2, 116
     50e:	9d2272        	l32i	a7, a2, 0x274
     511:	9c2262        	l32i	a6, a2, 0x270
     514:	f3f160        	wur.ae_bithead	a6
     517:	032674        	wur.ae_bitptr	a7
     51a:	38cb62        	addi	a6, a11, 56
     51d:	16cb72        	addi	a7, a11, 22
     520:	415150        	srli	a5, a5, 1
     523:	350b           	addi.n	a3, a5, -1
     525:	834d           	ae_sext16	a4, a3
     527:	043f30        	extui	a3, a3, 15, 1
     52a:	06f356        	bnez	a3, 59d <sbc_extract_scale_factors_joint_ch4+0x9d>
     52d:	9b22c2        	l32i	a12, a2, 0x26c
     530:	8a0c           	movi.n	a10, 8
     532:	0387a4        	ae_lb	a8, a10
     535:	1c1ca4        	ae_db	a12, a10
     538:	9b62c2        	s32i	a12, a2, 0x26c
     53b:	34c480        	extui	a12, a8, 4, 4
     53e:	0b1b22        	l16ui	a2, a11, 22
     541:	348080        	extui	a8, a8, 0, 4
     544:	1c5bc2        	s16i	a12, a11, 56
     547:	1d5b82        	s16i	a8, a11, 58
     54a:	1188c0        	slli	a8, a8, 4
     54d:	24a476        	loopgtz	a4, 575 <sbc_extract_scale_factors_joint_ch4+0x75>
     550:	0337a4        	ae_lb	a3, a10
     553:	664b           	addi.n	a6, a6, 4
     555:	2022c0        	or	a2, a2, a12
     558:	345030        	extui	a5, a3, 0, 4
     55b:	09c724        	ae_s16ip	a2, a7, 2
     55e:	7e2922        	l32i	a2, a9, 0x1f8
     561:	678d           	ae_s16i.n	a8, a7, 0
     563:	34c430        	extui	a12, a3, 4, 4
     566:	1c12a4        	ae_db	a2, a10
     569:	7e6922        	s32i	a2, a9, 0x1f8
     56c:	765d           	ae_s16i.n	a5, a6, 2
     56e:	1185c0        	slli	a8, a5, 4
     571:	66cd           	ae_s16i.n	a12, a6, 0
     573:	472d           	ae_l16ui.n	a2, a7, 0

00000575 <sbc_extract_scale_factors_joint_ch4+0x75>:
     575:	073d           	mov.n	a3, a7
     577:	2042c0        	or	a4, a2, a12
     57a:	00e2c4        	rur.ae_bitptr	a14
     57d:	09c344        	ae_s16ip	a4, a3, 2
     580:	638d           	ae_s16i.n	a8, a3, 0
     582:	1a1bf2        	l16ui	a15, a11, 52
     585:	e3df10        	rur.ae_bithead	a13
     588:	420c           	movi.n	a2, 4
     58a:	41f1f0        	srli	a15, a15, 1
     58d:	ff2b           	addi.n	a15, a15, 2
     58f:	b0ff20        	addx8	a15, a15, a2
     592:	155bf2        	s16i	a15, a11, 42
     595:	8069e2        	s32i	a14, a9, 0x200
     598:	7f69d2        	s32i	a13, a9, 0x1fc
     59b:	f01d           	retw.n

0000059d <sbc_extract_scale_factors_joint_ch4+0x9d>:
     59d:	e38f10        	rur.ae_bithead	a8
     5a0:	0092c4        	rur.ae_bitptr	a9
     5a3:	4c0c           	movi.n	a12, 4
     5a5:	a52b           	addi.n	a10, a5, 2
     5a7:	b0aac0        	addx8	a10, a10, a12
     5aa:	155ba2        	s16i	a10, a11, 42
     5ad:	9d6292        	s32i	a9, a2, 0x274
     5b0:	9c6282        	s32i	a8, a2, 0x270
     5b3:	f01d           	retw.n

000005b5 <sbc_extract_scale_factors_joint_ch4+0xb5>:
     5b5:	000000                                        ...

000005b8 <sbc_extract_scale_factors_joint_ch8>:
     5b8:	004136        	entry	a1, 32
     5bb:	1a1272        	l16ui	a7, a2, 52
     5be:	02d252        	addmi	a5, a2, 0x200
     5c1:	02ad           	mov.n	a10, a2
     5c3:	38ca42        	addi	a4, a10, 56
     5c6:	18ca62        	addi	a6, a10, 24
     5c9:	9d2592        	l32i	a9, a5, 0x274
     5cc:	9c2582        	l32i	a8, a5, 0x270
     5cf:	f3f180        	wur.ae_bithead	a8
     5d2:	032694        	wur.ae_bitptr	a9
     5d5:	74c582        	addi	a8, a5, 116
     5d8:	417170        	srli	a7, a7, 1
     5db:	370b           	addi.n	a3, a7, -1
     5dd:	832d           	ae_sext16	a2, a3
     5df:	043f30        	extui	a3, a3, 15, 1
     5e2:	0d2356        	bnez	a3, 6b8 <sbc_extract_scale_factors_joint_ch8+0x100>
     5e5:	890c           	movi.n	a9, 8
     5e7:	221b           	addi.n	a2, a2, 1
     5e9:	1d6207        	bbci	a2, 0, 60a <sbc_extract_scale_factors_joint_ch8+0x52>
     5ec:	03b794        	ae_lb	a11, a9
     5ef:	3cca42        	addi	a4, a10, 60
     5f2:	9b25e2        	l32i	a14, a5, 0x26c
     5f5:	34c4b0        	extui	a12, a11, 4, 4
     5f8:	34d0b0        	extui	a13, a11, 0, 4
     5fb:	1c1e94        	ae_db	a14, a9
     5fe:	9b65e2        	s32i	a14, a5, 0x26c
     601:	1d5ad2        	s16i	a13, a10, 58
     604:	1c5ac2        	s16i	a12, a10, 56
     607:	09c6b4        	ae_s16ip	a11, a6, 2

0000060a <sbc_extract_scale_factors_joint_ch8+0x52>:
     60a:	215120        	srai	a5, a2, 1
     60d:	08a516        	beqz	a5, 69b <sbc_extract_scale_factors_joint_ch8+0xe3>
     610:	03d794        	ae_lb	a13, a9
     613:	7e2822        	l32i	a2, a8, 0x1f8
     616:	34f4d0        	extui	a15, a13, 4, 4
     619:	1c1294        	ae_db	a2, a9
     61c:	34e0d0        	extui	a14, a13, 0, 4
     61f:	7e6822        	s32i	a2, a8, 0x1f8
     622:	032794        	ae_lb	a2, a9
     625:	74ed           	ae_s16i.n	a14, a4, 2
     627:	64fd           	ae_s16i.n	a15, a4, 0
     629:	09c6d4        	ae_s16ip	a13, a6, 2
     62c:	7e28c2        	l32i	a12, a8, 0x1f8
     62f:	34b420        	extui	a11, a2, 4, 4
     632:	343020        	extui	a3, a2, 0, 4
     635:	1c1c94        	ae_db	a12, a9
     638:	7e68c2        	s32i	a12, a8, 0x1f8
     63b:	035432        	s16i	a3, a4, 6
     63e:	0254b2        	s16i	a11, a4, 4
     641:	09c624        	ae_s16ip	a2, a6, 2
     644:	250b           	addi.n	a2, a5, -1
     646:	35a276        	loopgtz	a2, 67f <sbc_extract_scale_factors_joint_ch8+0xc7>
     649:	7e2822        	l32i	a2, a8, 0x1f8
     64c:	033794        	ae_lb	a3, a9
     64f:	448b           	addi.n	a4, a4, 8
     651:	1c1294        	ae_db	a2, a9
     654:	7e6822        	s32i	a2, a8, 0x1f8
     657:	342030        	extui	a2, a3, 0, 4
     65a:	037794        	ae_lb	a7, a9
     65d:	345430        	extui	a5, a3, 4, 4
     660:	742d           	ae_s16i.n	a2, a4, 2
     662:	645d           	ae_s16i.n	a5, a4, 0
     664:	09c634        	ae_s16ip	a3, a6, 2
     667:	7e2822        	l32i	a2, a8, 0x1f8
     66a:	343470        	extui	a3, a7, 4, 4
     66d:	345070        	extui	a5, a7, 0, 4
     670:	1c1294        	ae_db	a2, a9
     673:	7e6822        	s32i	a2, a8, 0x1f8
     676:	025432        	s16i	a3, a4, 4
     679:	035452        	s16i	a5, a4, 6
     67c:	09c674        	ae_s16ip	a7, a6, 2

0000067f <sbc_extract_scale_factors_joint_ch8+0xc7>:
     67f:	1a1ac2        	l16ui	a12, a10, 52
     682:	00b2c4        	rur.ae_bitptr	a11
     685:	e39f10        	rur.ae_bithead	a9
     688:	41c1c0        	srli	a12, a12, 1
     68b:	cc3b           	addi.n	a12, a12, 3
     68d:	11ccd0        	slli	a12, a12, 3
     690:	155ac2        	s16i	a12, a10, 42
     693:	8068b2        	s32i	a11, a8, 0x200
     696:	7f6892        	s32i	a9, a8, 0x1fc
     699:	f01d           	retw.n

0000069b <sbc_extract_scale_factors_joint_ch8+0xe3>:
     69b:	1a1ac2        	l16ui	a12, a10, 52
     69e:	e39f10        	rur.ae_bithead	a9
     6a1:	00b2c4        	rur.ae_bitptr	a11
     6a4:	41c1c0        	srli	a12, a12, 1
     6a7:	cc3b           	addi.n	a12, a12, 3
     6a9:	11ccd0        	slli	a12, a12, 3
     6ac:	155ac2        	s16i	a12, a10, 42
     6af:	8068b2        	s32i	a11, a8, 0x200
     6b2:	7f6892        	s32i	a9, a8, 0x1fc
     6b5:	f01d           	retw.n

000006b7 <sbc_extract_scale_factors_joint_ch8+0xff>:
	...

000006b8 <sbc_extract_scale_factors_joint_ch8+0x100>:
     6b8:	e3df10        	rur.ae_bithead	a13
     6bb:	00e2c4        	rur.ae_bitptr	a14
     6be:	f73b           	addi.n	a15, a7, 3
     6c0:	11ffd0        	slli	a15, a15, 3
     6c3:	155af2        	s16i	a15, a10, 42
     6c6:	9d65e2        	s32i	a14, a5, 0x274
     6c9:	9c65d2        	s32i	a13, a5, 0x270
     6cc:	f01d           	retw.n

000006ce <sbc_extract_scale_factors_joint_ch8+0x116>:
	...

000006d0 <sbc_get_frame_length>:
     6d0:	004136        	entry	a1, 32
     6d3:	04d222        	addmi	a2, a2, 0x400
     6d6:	4c1222        	l16ui	a2, a2, 152
     6d9:	f01d           	retw.n

000006db <sbc_get_frame_length+0xb>:
	...

000006dc <sbc_get_sample_freq>:
     6dc:	004136        	entry	a1, 32
     6df:	021222        	l16ui	a2, a2, 4
     6e2:	f01d           	retw.n

000006e4 <sbc_frame_header_decode>:
     6e4:	004136        	entry	a1, 32
     6e7:	fe5641        	l32r	a4, 40 (9c00 <_end+0x72ac>)
     6ea:	fe5651        	l32r	a5, 44 (ad00 <_end+0x83ac>)
     6ed:	027d           	mov.n	a7, a2
     6ef:	0bd362        	addmi	a6, a3, 0xb00
     6f2:	14c662        	addi	a6, a6, 20
     6f5:	020c           	movi.n	a2, 0
     6f7:	672d           	ae_s16i.n	a2, a7, 0

000006f9 <sbc_frame_header_decode+0x15>:
     6f9:	fe5381        	l32r	a8, 48 (304 <sbc_get_bytes>)
     6fc:	03ad           	mov.n	a10, a3
     6fe:	1b0c           	movi.n	a11, 1
     700:	0008e0        	callx8	a8
     703:	cacc           	bnez.n	a10, 713 <sbc_frame_header_decode+0x2f>
     705:	7e16a2        	l16ui	a10, a6, 252
     708:	0c1a47        	beq	a10, a4, 718 <sbc_frame_header_decode+0x34>
     70b:	191a57        	beq	a10, a5, 728 <sbc_frame_header_decode+0x44>
     70e:	021c           	movi.n	a2, 16
     710:	fff946        	j	6f9 <sbc_frame_header_decode+0x15>

00000713 <sbc_frame_header_decode+0x2f>:
     713:	0a2d           	mov.n	a2, a10
     715:	f01d           	retw.n

00000717 <sbc_frame_header_decode+0x33>:
	...

00000718 <sbc_frame_header_decode+0x34>:
     718:	fe4c81        	l32r	a8, 48 (304 <sbc_get_bytes>)
     71b:	03ad           	mov.n	a10, a3
     71d:	1b0c           	movi.n	a11, 1
     71f:	0008e0        	callx8	a8
     722:	2a9c           	beqz.n	a10, 738 <sbc_frame_header_decode+0x54>
     724:	0a2d           	mov.n	a2, a10
     726:	f01d           	retw.n

00000728 <sbc_frame_header_decode+0x44>:
     728:	fe4881        	l32r	a8, 48 (304 <sbc_get_bytes>)
     72b:	03ad           	mov.n	a10, a3
     72d:	2b0c           	movi.n	a11, 2
     72f:	0008e0        	callx8	a8
     732:	ca9c           	beqz.n	a10, 752 <sbc_frame_header_decode+0x6e>
     734:	0a2d           	mov.n	a2, a10
     736:	f01d           	retw.n

00000738 <sbc_frame_header_decode+0x54>:
     738:	03ad           	mov.n	a10, a3
     73a:	7e1622        	l16ui	a2, a6, 252
     73d:	2b0c           	movi.n	a11, 2
     73f:	fe4281        	l32r	a8, 48 (304 <sbc_get_bytes>)
     742:	742820        	extui	a2, a2, 8, 8
     745:	095322        	s16i	a2, a3, 18
     748:	0008e0        	callx8	a8
     74b:	061a16        	beqz	a10, 7b0 <sbc_frame_header_decode+0xcc>
     74e:	0a2d           	mov.n	a2, a10
     750:	f01d           	retw.n

00000752 <sbc_frame_header_decode+0x6e>:
     752:	78a092        	movi	a9, 120
     755:	04d3d2        	addmi	a13, a3, 0x400
     758:	9c3c           	movi.n	a12, 57
     75a:	050c           	movi.n	a5, 0
     75c:	af1c           	movi.n	a15, 26
     75e:	fe0c           	movi.n	a14, 15
     760:	7e1682        	l16ui	a8, a6, 252
     763:	8a0c           	movi.n	a10, 8
     765:	1b0c           	movi.n	a11, 1
     767:	748880        	extui	a8, a8, 8, 8
     76a:	095382        	s16i	a8, a3, 18
     76d:	fe3681        	l32r	a8, 48 (304 <sbc_get_bytes>)
     770:	fc0642        	l8ui	a4, a6, 252
     773:	0753b2        	s16i	a11, a3, 14
     776:	0653a2        	s16i	a10, a3, 12
     779:	0353e2        	s16i	a14, a3, 6
     77c:	0853f2        	s16i	a15, a3, 16
     77f:	0a5342        	s16i	a4, a3, 20
     782:	025352        	s16i	a5, a3, 4
     785:	045352        	s16i	a5, a3, 8
     788:	055352        	s16i	a5, a3, 10
     78b:	4c5dc2        	s16i	a12, a13, 152
     78e:	1a53a2        	s16i	a10, a3, 52
     791:	1b5392        	s16i	a9, a3, 54
     794:	03ad           	mov.n	a10, a3
     796:	0008e0        	callx8	a8
     799:	3a8c           	beqz.n	a10, 7a0 <sbc_frame_header_decode+0xbc>
     79b:	0a2d           	mov.n	a2, a10
     79d:	f01d           	retw.n

0000079f <sbc_frame_header_decode+0xbb>:
	...

000007a0 <sbc_frame_header_decode+0xbc>:
     7a0:	7e16a2        	l16ui	a10, a6, 252
     7a3:	190c           	movi.n	a9, 1
     7a5:	74a8a0        	extui	a10, a10, 8, 8
     7a8:	1653a2        	s16i	a10, a3, 44
     7ab:	679d           	ae_s16i.n	a9, a7, 0
     7ad:	f01d           	retw.n

000007af <sbc_frame_header_decode+0xcb>:
	...

000007b0 <sbc_frame_header_decode+0xcc>:
     7b0:	03cd           	mov.n	a12, a3
     7b2:	fe2681        	l32r	a8, 4c (13b0 <sbc_header_parse>)
     7b5:	7e16d2        	l16ui	a13, a6, 252
     7b8:	20a240        	or	a10, a2, a4
     7bb:	9aad           	ae_zext16	a10, a10
     7bd:	0dbd           	mov.n	a11, a13
     7bf:	7490d0        	extui	a9, a13, 0, 8
     7c2:	165392        	s16i	a9, a3, 44
     7c5:	41d8d0        	srli	a13, a13, 8
     7c8:	0a53d2        	s16i	a13, a3, 20
     7cb:	0008e0        	callx8	a8
     7ce:	2a8c           	beqz.n	a10, 7d4 <sbc_frame_header_decode+0xf0>
     7d0:	0a2d           	mov.n	a2, a10
     7d2:	f01d           	retw.n

000007d4 <sbc_frame_header_decode+0xf0>:
     7d4:	020c           	movi.n	a2, 0
     7d6:	f01d           	retw.n

000007d8 <change_endian16x>:
     7d8:	004136        	entry	a1, 32
     7db:	743820        	extui	a3, a2, 8, 8
     7de:	8a7230        	depbits	a3, a2, 8, 8
     7e1:	832d           	ae_sext16	a2, a3
     7e3:	f01d           	retw.n

000007e5 <change_endian16x+0xd>:
     7e5:	000000                                        ...

000007e8 <sbc_frame_decode>:
     7e8:	006136        	entry	a1, 48
     7eb:	039d           	mov.n	a9, a3
     7ed:	04d232        	addmi	a3, a2, 0x400
     7f0:	4c1382        	l16ui	a8, a3, 152
     7f3:	0199           	s32i.n	a9, a1, 0
     7f5:	84c332        	addi	a3, a3, -124
     7f8:	fdc8a2        	addi	a10, a8, -3
     7fb:	fec882        	addi	a8, a8, -2
     7fe:	b38aa0        	movgez	a8, a10, a10
     801:	218180        	srai	a8, a8, 1
     804:	2938a6        	blti	a8, 3, 831 <sbc_frame_decode+0x49>
     807:	250c           	movi.n	a5, 2
     809:	fe1171        	l32r	a7, 50 (c10 <SBC_synthesis8_per_channel+0x80>)

0000080c <sbc_frame_decode+0x24>:
     80c:	906520        	addx2	a6, a5, a2
     80f:	fe1181        	l32r	a8, 54 (7d8 <change_endian16x>)
     812:	667a           	add.n	a6, a6, a7
     814:	26ad           	ae_l16si.n	a10, a6, 0
     816:	0008e0        	callx8	a8
     819:	66ad           	ae_s16i.n	a10, a6, 0
     81b:	8a1382        	l16ui	a8, a3, 0x114
     81e:	551b           	addi.n	a5, a5, 1
     820:	955d           	ae_zext16	a5, a5
     822:	fdc892        	addi	a9, a8, -3
     825:	fec882        	addi	a8, a8, -2
     828:	b38990        	movgez	a8, a9, a9
     82b:	218180        	srai	a8, a8, 1
     82e:	da2587        	blt	a5, a8, 80c <sbc_frame_decode+0x24>

00000831 <sbc_frame_decode+0x49>:
     831:	041282        	l16ui	a8, a2, 8
     834:	0cd2a2        	addmi	a10, a2, 0xc00
     837:	0bd2c2        	addmi	a12, a2, 0xb00
     83a:	090c           	movi.n	a9, 0
     83c:	032694        	wur.ae_bitptr	a9
     83f:	dccc62        	addi	a6, a12, -36
     842:	891cb2        	l16ui	a11, a12, 0x112
     845:	12caa2        	addi	a10, a10, 18
     848:	881cc2        	l16ui	a12, a12, 0x110
     84b:	fdc882        	addi	a8, a8, -3
     84e:	3a63a2        	s32i	a10, a3, 232
     851:	0bfcb0        	depbits	a11, a12, 16, 16
     854:	f3f1b0        	wur.ae_bithead	a11
     857:	117816        	beqz	a8, 972 <sbc_frame_decode+0x18a>
     85a:	175292        	s16i	a9, a2, 46

0000085d <sbc_frame_decode+0x75>:
     85d:	f9bc           	beqz.n	a9, 8a0 <sbc_frame_decode+0xb8>
     85f:	1a0c           	movi.n	a10, 1
     861:	0612c2        	l16ui	a12, a2, 12
     864:	04d2d2        	addmi	a13, a2, 0x400
     867:	58cdd2        	addi	a13, a13, 88
     86a:	3963d2        	s32i	a13, a3, 228
     86d:	ffccb2        	addi	a11, a12, -1
     870:	401b00        	ssl	a11
     873:	a1aa00        	sll	a10, a10
     876:	9aad           	ae_zext16	a10, a10
     878:	4cac           	beqz.n	a12, 8a0 <sbc_frame_decode+0xb8>
     87a:	00a052        	movi	a5, 0
     87d:	1c8076        	loop	a0, 89d <sbc_frame_decode+0xb5>
     880:	1712e2        	l16ui	a14, a2, 46
     883:	0c0ae7        	bnone	a10, a14, 893 <sbc_frame_decode+0xab>
     886:	3923f2        	l32i	a15, a3, 228
     889:	6d5d           	ae_s16i.n	a5, a13, 0
     88b:	0612c2        	l16ui	a12, a2, 12
     88e:	df2b           	addi.n	a13, a15, 2
     890:	3963d2        	s32i	a13, a3, 228

00000893 <sbc_frame_decode+0xab>:
     893:	41a1a0        	srli	a10, a10, 1
     896:	551b           	addi.n	a5, a5, 1
     898:	955d           	ae_zext16	a5, a5
     89a:	02b5c7        	bgeu	a5, a12, 8a0 <sbc_frame_decode+0xb8>

0000089d <sbc_frame_decode+0xb5>:
     89d:	fff706        	j	87d <sbc_frame_decode+0x95>

000008a0 <sbc_frame_decode+0xb8>:
     8a0:	e39f10        	rur.ae_bithead	a9
     8a3:	00a2c4        	rur.ae_bitptr	a10
     8a6:	041282        	l16ui	a8, a2, 8
     8a9:	3c63a2        	s32i	a10, a3, 240
     8ac:	3b6392        	s32i	a9, a3, 236
     8af:	153866        	bnei	a8, 3, 8c8 <sbc_frame_decode+0xe0>
     8b2:	0612b2        	l16ui	a11, a2, 12
     8b5:	fccbb2        	addi	a11, a11, -4
     8b8:	0dfb16        	beqz	a11, 99b <sbc_frame_decode+0x1b3>
     8bb:	fde781        	l32r	a8, 58 (5b8 <sbc_extract_scale_factors_joint_ch8>)
     8be:	02ad           	mov.n	a10, a2
     8c0:	0008e0        	callx8	a8
     8c3:	000246        	j	8d0 <sbc_frame_decode+0xe8>

000008c6 <sbc_frame_decode+0xde>:
	...

000008c8 <sbc_frame_decode+0xe0>:
     8c8:	fdd581        	l32r	a8, 1c (3e4 <sbc_extract_scale_factors_non_joint>)
     8cb:	02ad           	mov.n	a10, a2
     8cd:	0008e0        	callx8	a8

000008d0 <sbc_frame_decode+0xe8>:
     8d0:	fdd481        	l32r	a8, 20 (36c <sbc_crc8>)
     8d3:	12c2a2        	addi	a10, a2, 18
     8d6:	1512b2        	l16ui	a11, a2, 42
     8d9:	0008e0        	callx8	a8
     8dc:	161292        	l16ui	a9, a2, 44
     8df:	0a19a7        	beq	a9, a10, 8ed <sbc_frame_decode+0x105>
     8e2:	fdd081        	l32r	a8, 24 (9b4 <sbc_zero_sb_samples>)
     8e5:	02ad           	mov.n	a10, a2
     8e7:	0008e0        	callx8	a8
     8ea:	0003c6        	j	8fd <sbc_frame_decode+0x115>

000008ed <sbc_frame_decode+0x105>:
     8ed:	fdcf81        	l32r	a8, 2c (cf8 <sbc_bit_allocation>)
     8f0:	02ad           	mov.n	a10, a2
     8f2:	0008e0        	callx8	a8
     8f5:	fdce81        	l32r	a8, 30 (1190 <sbc_reconstruction>)
     8f8:	02ad           	mov.n	a10, a2
     8fa:	0008e0        	callx8	a8

000008fd <sbc_frame_decode+0x115>:
     8fd:	061292        	l16ui	a9, a2, 12
     900:	05d2a2        	addmi	a10, a2, 0x500
     903:	6516b2        	l16ui	a11, a6, 202
     906:	6456b2        	s16i	a11, a6, 200
     909:	a0caa2        	addi	a10, a10, -96
     90c:	3166a2        	s32i	a10, a6, 196
     90f:	794926        	beqi	a9, 4, 98c <sbc_frame_decode+0x1a4>
     912:	01b8           	l32i.n	a11, a1, 0
     914:	fdc581        	l32r	a8, 28 (b90 <SBC_synthesis8_per_channel>)
     917:	00a0a2        	movi	a10, 0
     91a:	20c220        	or	a12, a2, a2
     91d:	0008e0        	callx8	a8

00000920 <sbc_frame_decode+0x138>:
     920:	071292        	l16ui	a9, a2, 14
     923:	6416a2        	l16ui	a10, a6, 200
     926:	6556a2        	s16i	a10, a6, 202
     929:	032926        	beqi	a9, 2, 930 <sbc_frame_decode+0x148>
     92c:	020c           	movi.n	a2, 0
     92e:	f01d           	retw.n

00000930 <sbc_frame_decode+0x148>:
     930:	0612b2        	l16ui	a11, a2, 12
     933:	07d2c2        	addmi	a12, a2, 0x700
     936:	6616d2        	l16ui	a13, a6, 204
     939:	6456d2        	s16i	a13, a6, 200
     93c:	20ccc2        	addi	a12, a12, 32
     93f:	3166c2        	s32i	a12, a6, 196
     942:	164b66        	bnei	a11, 4, 95c <sbc_frame_decode+0x174>
     945:	04bd           	mov.n	a11, a4
     947:	fdc581        	l32r	a8, 5c (9e8 <SBC_synthesis4_per_channel>)
     94a:	02cd           	mov.n	a12, a2
     94c:	1a0c           	movi.n	a10, 1
     94e:	0008e0        	callx8	a8
     951:	020c           	movi.n	a2, 0
     953:	641692        	l16ui	a9, a6, 200
     956:	665692        	s16i	a9, a6, 204
     959:	f01d           	retw.n

0000095b <sbc_frame_decode+0x173>:
	...

0000095c <sbc_frame_decode+0x174>:
     95c:	02cd           	mov.n	a12, a2
     95e:	fdb281        	l32r	a8, 28 (b90 <SBC_synthesis8_per_channel>)
     961:	04bd           	mov.n	a11, a4
     963:	1a0c           	movi.n	a10, 1
     965:	0008e0        	callx8	a8
     968:	020c           	movi.n	a2, 0
     96a:	641692        	l16ui	a9, a6, 200
     96d:	665692        	s16i	a9, a6, 204
     970:	f01d           	retw.n

00000972 <sbc_frame_decode+0x18a>:
     972:	0612b2        	l16ui	a11, a2, 12
     975:	03c7b4        	ae_lb	a12, a11
     978:	1752c2        	s16i	a12, a2, 46
     97b:	1c1ab4        	ae_db	a10, a11
     97e:	3a63a2        	s32i	a10, a3, 232
     981:	9c9d           	ae_zext16	a9, a12
     983:	214b26        	beqi	a11, 4, 9a8 <sbc_frame_decode+0x1c0>
     986:	0b5292        	s16i	a9, a2, 22
     989:	ffb406        	j	85d <sbc_frame_decode+0x75>

0000098c <sbc_frame_decode+0x1a4>:
     98c:	01b8           	l32i.n	a11, a1, 0
     98e:	fdb381        	l32r	a8, 5c (9e8 <SBC_synthesis4_per_channel>)
     991:	0a0c           	movi.n	a10, 0
     993:	02cd           	mov.n	a12, a2
     995:	0008e0        	callx8	a8
     998:	ffe106        	j	920 <sbc_frame_decode+0x138>

0000099b <sbc_frame_decode+0x1b3>:
     99b:	fdb181        	l32r	a8, 60 (500 <sbc_extract_scale_factors_joint_ch4>)
     99e:	02ad           	mov.n	a10, a2
     9a0:	0008e0        	callx8	a8
     9a3:	ffca46        	j	8d0 <sbc_frame_decode+0xe8>

000009a6 <sbc_frame_decode+0x1be>:
	...

000009a8 <sbc_frame_decode+0x1c0>:
     9a8:	34a0c0        	extui	a10, a12, 0, 4
     9ab:	11aac0        	slli	a10, a10, 4
     9ae:	0b52a2        	s16i	a10, a2, 22
     9b1:	ffaa06        	j	85d <sbc_frame_decode+0x75>

000009b4 <sbc_zero_sb_samples>:
     9b4:	004136        	entry	a1, 32
     9b7:	050c           	movi.n	a5, 0
     9b9:	00a132        	movi	a3, 0x100
     9bc:	024d           	mov.n	a4, a2
     9be:	060c           	movi.n	a6, 0
     9c0:	062d           	mov.n	a2, a6
     9c2:	098376        	loop	a3, 9cf <sbc_zero_sb_samples+0x1b>
     9c5:	a03240        	addx4	a3, a2, a4
     9c8:	221b           	addi.n	a2, a2, 1
     9ca:	166352        	s32i	a5, a3, 88
     9cd:	922d           	ae_zext16	a2, a2

000009cf <sbc_zero_sb_samples+0x1b>:
     9cf:	f01d           	retw.n

000009d1 <sbc_zero_sb_samples+0x1d>:
     9d1:	000000                                        ...

000009d4 <pseudo_stereo>:
     9d4:	004136        	entry	a1, 32
     9d7:	f01d           	retw.n

000009d9 <pseudo_stereo+0x5>:
     9d9:	000000                                        ...

000009dc <pseudo_stereo_setup>:
     9dc:	004136        	entry	a1, 32
     9df:	f01d           	retw.n

000009e1 <pseudo_stereo_setup+0x5>:
     9e1:	00000000 36000000                                .......

000009e8 <SBC_synthesis4_per_channel>:
     9e8:	006136        	entry	a1, 48
     9eb:	04dd           	mov.n	a13, a4
     9ed:	fec3a2        	addi	a10, a3, -2
     9f0:	031462        	l16ui	a6, a4, 6
     9f3:	490c           	movi.n	a9, 4
     9f5:	932920        	movnez	a2, a9, a2
     9f8:	a08240        	addx4	a8, a2, a4
     9fb:	58c882        	addi	a8, a8, 88
     9fe:	0216e6        	bgei	a6, 1, a04 <SBC_synthesis4_per_channel+0x1c>
     a01:	006146        	j	b8a <SBC_synthesis4_per_channel+0x1a2>
     a04:	0b0c           	movi.n	a11, 0
     a06:	fd98c1        	l32r	a12, 68 (1600 <_DYNAMIC+0x38>)
     a09:	090c           	movi.n	a9, 0
     a0b:	fd96f1        	l32r	a15, 64 (5a00 <_end+0x30ac>)
     a0e:	fd9761        	l32r	a6, 6c (1a60 <SBC_cos_coef_4+0x40>)
     a11:	0bd4e2        	addmi	a14, a4, 0xb00
     a14:	a4cee2        	addi	a14, a14, -92
     a17:	0169           	s32i.n	a6, a1, 0
     a19:	303ff4        	ae_cvtp24a16x2.ll	aep3, a15, a15
     a1c:	fe248008ff 	{ movi	a15, 8; ae_zeroq56	aeq2 }
     a21:	004d46        	j	b5a <SBC_synthesis4_per_channel+0x172>

00000a24 <SBC_synthesis4_per_channel+0x3c>:
     a24:	0138           	l32i.n	a3, a1, 0
     a26:	01d992        	addmi	a9, a9, 0x100
     a29:	a0c992        	addi	a9, a9, -96

00000a2c <SBC_synthesis4_per_channel+0x44>:
     a2c:	5c5304        	ae_lp24x2f.i	aep5, a3, 0
     a2f:	fc24ddc2ff 	{ ae_lp24x2f.c	aep4, a2, a15; nop }
     a34:	fc24dd92ff 	{ ae_lp24x2f.c	aep1, a2, a15; nop }
     a39:	f8c332        	addi	a3, a3, -8
     a3c:	e2b02fa32f 	{ ae_lp24x2f.iu	aep2, a3, 16; ae_mulaafp24s.hh.ll	aeq0, aep4, aep5 }
     a41:	051204        	ae_mulaafp24s.hh.ll	aeq0, aep1, aep2
     a44:	f8e780076f 	{ movi	a6, 7; ae_roundsp24q48sym	aep0, aeq0 }
     a49:	ff1474040f 	{ ae_sp24f.l.i	aep0, a4, 0; ae_movq56	aeq3, aeq2 }
     a4e:	1c8676        	loop	a6, a6e <SBC_synthesis4_per_channel+0x86>
     a51:	fc145d82ff 	{ ae_lp24x2f.c	aep0, a2, a15; ae_movq56	aeq0, aeq2 }
     a56:	5c1314        	ae_lp24x2f.i	aep1, a3, 8
     a59:	e2202f832f 	{ ae_lp24x2f.iu	aep0, a3, 16; ae_mulaafp24s.hh.ll	aeq0, aep0, aep1 }
     a5e:	fc24dd92ff 	{ ae_lp24x2f.c	aep1, a2, a15; nop }
     a63:	e20402844e 	{ addi	a4, a4, 40; ae_mulaafp24s.hh.ll	aeq0, aep1, aep0 }
     a68:	1b8c04        	ae_roundsp24q48sym	aep0, aeq0
     a6b:	520404        	ae_sp24f.l.i	aep0, a4, 0

00000a6e <SBC_synthesis4_per_channel+0x86>:
     a6e:	5c2934        	ae_lp24x2f.i	aep2, a9, 24
     a71:	5c6924        	ae_lp24x2f.i	aep6, a9, 16
     a74:	1148           	l32i.n	a4, a1, 4
     a76:	5c1914        	ae_lp24x2f.i	aep1, a9, 8
     a79:	160c           	movi.n	a6, 1
     a7b:	412140        	srli	a2, a4, 1
     a7e:	304460        	xor	a4, a4, a6
     a81:	112280        	slli	a2, a2, 8
     a84:	37d2c4        	ae_truncp24a32x2	aep5, a2, a12
     a87:	eab4af790f 	{ ae_lp24x2f.i	aep7, a9, 0; ae_mulfp24s.hl	aeq0, aep5, aep5 }
     a8c:	fd7961        	l32r	a6, 70 (1de0 <SBC_proto_coef_4>)
     a8f:	350414        	ae_slliq56	aeq0, aeq0, 16
     a92:	368504        	ae_trunca32q48	a5, aeq0
     a95:	044040        	extui	a4, a4, 0, 1
     a98:	f8c922        	addi	a2, a9, -8
     a9b:	a05560        	addx4	a5, a5, a6
     a9e:	a04450        	addx4	a4, a4, a5
     aa1:	fcc432        	addi	a3, a4, -4
     aa4:	505434        	ae_lp24f.i	aep5, a4, 12
     aa7:	504414        	ae_lp24f.i	aep4, a4, 4
     aaa:	500404        	ae_lp24f.i	aep0, a4, 0

00000aad <SBC_synthesis4_per_channel+0xc5>:
     aad:	dc81ae442f 	{ ae_lp24f.i	aep4, a4, 8; ae_selp24.lh	aep0, aep0, aep4 }
     ab2:	e31c6e045f 	{ ae_lp24f.i	aep0, a4, 20; ae_mulaafp24s.hh.ll	aeq3, aep7, aep0 }
     ab7:	deb1ae744f 	{ ae_lp24f.i	aep7, a4, 16; ae_selp24.lh	aep4, aep4, aep5 }
     abc:	e3846ec37f 	{ ae_lp24f.iu	aep4, a3, 28; ae_mulaafp24s.hh.ll	aeq3, aep1, aep4 }
     ac1:	501334        	ae_lp24f.i	aep1, a3, 12
     ac4:	df1dee032f 	{ ae_lp24f.i	aep0, a3, 8; ae_selp24.lh	aep7, aep7, aep0 }
     ac9:	e3f86e531f 	{ ae_lp24f.i	aep5, a3, 4; ae_mulaafp24s.hh.ll	aeq3, aep6, aep7 }
     ace:	deb1aff25f 	{ ae_lp24x2f.iu	aep7, a2, 40; ae_selp24.lh	aep4, aep4, aep5 }
     ad3:	e3887e012f 	{ ae_selp24.lh	aep0, aep0, aep1; ae_mulaafp24s.hh.ll	aeq3, aep2, aep4 }
     ad8:	e31c401bbe 	{ addi	a11, a11, 1; ae_mulaafp24s.hh.ll	aeq3, aep7, aep0 }
     add:	090c           	movi.n	a9, 0
     adf:	fd5c40035f 	{ movi	a5, 3; ae_roundsq32sym	aeq1, aeq3 }
     ae4:	f8ad6ee34f 	{ ae_lp24f.iu	aep6, a3, 16; ae_mulq32sp16s.l	aeq1, aeq1, aep3 }
     ae9:	f03d           	nop.n
     aeb:	f03d           	nop.n
     aed:	548576        	loop	a5, b45 <SBC_synthesis4_per_channel+0x15d>
     af0:	fc147ad06f 	{ ae_sllisq56s	aeq1, aeq1, 2; ae_movq56	aeq0, aeq2 }
     af5:	1b9e24        	ae_roundsp16q48sym	aep2, aeq1
     af8:	42aa14        	ae_sp16f.l.iu	aep2, a10, 2
     afb:	502324        	ae_lp24f.i	aep2, a3, 8
     afe:	501314        	ae_lp24f.i	aep1, a3, 4
     b01:	dc45ee234f 	{ ae_lp24f.i	aep2, a3, 16; ae_selp24.lh	aep1, aep1, aep2 }
     b06:	5c0214        	ae_lp24x2f.i	aep0, a2, 8
     b09:	e2202e133f 	{ ae_lp24f.i	aep1, a3, 12; ae_mulaafp24s.hh.ll	aeq0, aep0, aep1 }
     b0e:	dc45ef222f 	{ ae_lp24x2f.i	aep2, a2, 16; ae_selp24.lh	aep1, aep1, aep2 }
     b13:	e2282e235f 	{ ae_lp24f.i	aep2, a3, 20; ae_mulaafp24s.hh.ll	aeq0, aep2, aep1 }
     b18:	501364        	ae_lp24f.i	aep1, a3, 24
     b1b:	dc29eea37f 	{ ae_lp24f.iu	aep2, a3, 28; ae_selp24.lh	aep1, aep2, aep1 }
     b20:	5c0234        	ae_lp24x2f.i	aep0, a2, 24
     b23:	e2202e131f 	{ ae_lp24f.i	aep1, a3, 4; ae_mulaafp24s.hh.ll	aeq0, aep0, aep1 }
     b28:	dc29ef224f 	{ ae_lp24x2f.i	aep2, a2, 32; ae_selp24.lh	aep1, aep2, aep1 }
     b2d:	e2282e232f 	{ ae_lp24f.i	aep2, a3, 8; ae_mulaafp24s.hh.ll	aeq0, aep2, aep1 }
     b32:	501334        	ae_lp24f.i	aep1, a3, 12
     b35:	dc29efa25f 	{ ae_lp24x2f.iu	aep2, a2, 40; ae_selp24.lh	aep1, aep2, aep1 }
     b3a:	e2282ea34f 	{ ae_lp24f.iu	aep2, a3, 16; ae_mulaafp24s.hh.ll	aeq0, aep2, aep1 }
     b3f:	064834        	ae_roundsq32sym	aeq1, aeq0
     b42:	033b54        	ae_mulq32sp16s.l	aeq1, aeq1, aep3

00000b45 <SBC_synthesis4_per_channel+0x15d>:
     b45:	365094        	ae_sllisq56s	aeq1, aeq1, 2
     b48:	1b9e64        	ae_roundsp16q48sym	aep6, aeq1
     b4b:	42ea14        	ae_sp16f.l.iu	aep6, a10, 2
     b4e:	1a1d72        	l16ui	a7, a13, 52
     b51:	031d62        	l16ui	a6, a13, 6
     b54:	a08780        	addx4	a8, a7, a8
     b57:	2fab67        	bge	a11, a6, b8a <SBC_synthesis4_per_channel+0x1a2>

00000b5a <SBC_synthesis4_per_channel+0x172>:
     b5a:	f3f680        	wur.ae_cbegin0	a8
     b5d:	10c862        	addi	a6, a8, 16
     b60:	801e42        	l16ui	a4, a14, 0x100
     b63:	1149           	s32i.n	a4, a1, 4
     b65:	f3f760        	wur.ae_cend0	a6
     b68:	f7c422        	addi	a2, a4, -9
     b6b:	641b           	addi.n	a6, a4, 1
     b6d:	836920        	moveqz	a6, a9, a2
     b70:	3f2e92        	l32i	a9, a14, 252
     b73:	805e62        	s16i	a6, a14, 0x100
     b76:	fc1453288f 	{ or	a2, a8, a8; ae_movq56	aeq0, aeq2 }
     b7b:	046040        	extui	a6, a4, 0, 1
     b7e:	a04490        	addx4	a4, a4, a9
     b81:	e9f656        	bnez	a6, a24 <SBC_synthesis4_per_channel+0x3c>
     b84:	fd3c31        	l32r	a3, 74 (1a20 <SBC_cos_coef_4>)
     b87:	ffa846        	j	a2c <SBC_synthesis4_per_channel+0x44>

00000b8a <SBC_synthesis4_per_channel+0x1a2>:
     b8a:	f01d           	retw.n

00000b8c <SBC_synthesis4_per_channel+0x1a4>:
     b8c:	00000000                                ....

00000b90 <SBC_synthesis8_per_channel>:
     b90:	006136        	entry	a1, 48
     b93:	fec3a2        	addi	a10, a3, -2
     b96:	04dd           	mov.n	a13, a4
     b98:	58c442        	addi	a4, a4, 88
     b9b:	031de2        	l16ui	a14, a13, 6
     b9e:	78cd82        	addi	a8, a13, 120
     ba1:	838420        	moveqz	a8, a4, a2
     ba4:	148e16        	beqz	a14, cf0 <SBC_synthesis8_per_channel+0x160>
     ba7:	0bddf2        	addmi	a15, a13, 0xb00
     baa:	fd33c1        	l32r	a12, 78 (2c00 <_end+0x2ac>)
     bad:	090c           	movi.n	a9, 0
     baf:	fd3361        	l32r	a6, 7c (1be0 <SBC_cos_coef_8+0x100>)
     bb2:	fd2cb1        	l32r	a11, 64 (5a00 <_end+0x30ac>)
     bb5:	305bb4        	ae_cvtp24a16x2.ll	aep5, a11, a11
     bb8:	0169           	s32i.n	a6, a1, 0
     bba:	0b0c           	movi.n	a11, 0
     bbc:	004006        	j	cc0 <SBC_synthesis8_per_channel+0x130>

00000bbf <SBC_synthesis8_per_channel+0x2f>:
     bbf:	00000000                                ....

00000bc3 <SBC_synthesis8_per_channel+0x33>:
     bc3:	0138           	l32i.n	a3, a1, 0
     bc5:	01d992        	addmi	a9, a9, 0x100
     bc8:	40c992        	addi	a9, a9, 64

00000bcb <SBC_synthesis8_per_channel+0x3b>:
     bcb:	5c4324        	ae_lp24x2f.i	aep4, a3, 16
     bce:	5c7314        	ae_lp24x2f.i	aep7, a3, 8
     bd1:	5c1304        	ae_lp24x2f.i	aep1, a3, 0
     bd4:	fc24dd825f 	{ ae_lp24x2f.c	aep0, a2, a5; nop }
     bd9:	fc24dde25f 	{ ae_lp24x2f.c	aep6, a2, a5; nop }
     bde:	fc24ddb25f 	{ ae_lp24x2f.c	aep3, a2, a5; nop }
     be3:	f0229d925f 	{ ae_lp24x2f.c	aep1, a2, a5; ae_mulzaafp24s.hh.ll	aeq0, aep0, aep1 }
     be8:	e2f80f833e 	{ addi	a3, a3, -8; ae_mulaafp24s.hh.ll	aeq0, aep6, aep7 }
     bed:	e28c2fa34f 	{ ae_lp24x2f.iu	aep2, a3, 32; ae_mulaafp24s.hh.ll	aeq0, aep3, aep4 }
     bf2:	051204        	ae_mulaafp24s.hh.ll	aeq0, aep1, aep2
     bf5:	f8e7800f6f 	{ movi	a6, 15; ae_roundsp24q48sym	aep0, aeq0 }
     bfa:	520404        	ae_sp24f.l.i	aep0, a4, 0
     bfd:	2c8676        	loop	a6, c2d <SBC_synthesis8_per_channel+0x9d>
     c00:	fc24dd825f 	{ ae_lp24x2f.c	aep0, a2, a5; nop }
     c05:	5c1314        	ae_lp24x2f.i	aep1, a3, 8
     c08:	f022af032f 	{ ae_lp24x2f.i	aep0, a3, 16; ae_mulzaafp24s.hh.ll	aeq0, aep0, aep1 }
     c0d:	5c1334        	ae_lp24x2f.i	aep1, a3, 24
     c10:	5cb344        	ae_lp24x2f.iu	aep3, a3, 32
     c13:	fc24dda25f 	{ ae_lp24x2f.c	aep2, a2, a5; nop }
     c18:	e2081d825f 	{ ae_lp24x2f.c	aep0, a2, a5; ae_mulaafp24s.hh.ll	aeq0, aep2, aep0 }
     c1d:	e2201d825f 	{ ae_lp24x2f.c	aep0, a2, a5; ae_mulaafp24s.hh.ll	aeq0, aep0, aep1 }
     c22:	e26002844e 	{ addi	a4, a4, 40; ae_mulaafp24s.hh.ll	aeq0, aep0, aep3 }
     c27:	1b8c04        	ae_roundsp24q48sym	aep0, aeq0
     c2a:	520404        	ae_sp24f.l.i	aep0, a4, 0

00000c2d <SBC_synthesis8_per_channel+0x9d>:
     c2d:	1128           	l32i.n	a2, a1, 4
     c2f:	140c           	movi.n	a4, 1
     c31:	415120        	srli	a5, a2, 1
     c34:	302240        	xor	a2, a2, a4
     c37:	115580        	slli	a5, a5, 8
     c3a:	37a5c4        	ae_truncp24a32x2	aep2, a5, a12
     c3d:	ea48c01bbe 	{ addi	a11, a11, 1; ae_mulfp24s.hl	aeq1, aep2, aep2 }
     c42:	fd0f41        	l32r	a4, 80 (2150 <SBC_proto_coef_8>)
     c45:	355414        	ae_slliq56	aeq1, aeq1, 16
     c48:	369304        	ae_trunca32q48	a3, aeq1
     c4b:	042020        	extui	a2, a2, 0, 1
     c4e:	08a052        	movi	a5, 8
     c51:	a03340        	addx4	a3, a3, a4
     c54:	a02230        	addx4	a2, a2, a3
     c57:	f8c932        	addi	a3, a9, -8
     c5a:	fcc222        	addi	a2, a2, -4
     c5d:	528576        	loop	a5, cb3 <SBC_synthesis8_per_channel+0x123>
     c60:	500254        	ae_lp24f.i	aep0, a2, 20
     c63:	504264        	ae_lp24f.i	aep4, a2, 24

00000c66 <SBC_synthesis8_per_channel+0xd6>:
     c66:	de81ae021f 	{ ae_lp24f.i	aep0, a2, 4; ae_selp24.lh	aep4, aep0, aep4 }
     c6b:	501224        	ae_lp24f.i	aep1, a2, 8
     c6e:	dc21ef031f 	{ ae_lp24x2f.i	aep0, a3, 8; ae_selp24.lh	aep1, aep0, aep1 }
     c73:	f022af033f 	{ ae_lp24x2f.i	aep0, a3, 24; ae_mulzaafp24s.hh.ll	aeq0, aep0, aep1 }
     c78:	5c1324        	ae_lp24x2f.i	aep1, a3, 16
     c7b:	503244        	ae_lp24f.i	aep3, a2, 16
     c7e:	502234        	ae_lp24f.i	aep2, a2, 12
     c81:	dd69aeb27f 	{ ae_lp24f.iu	aep3, a2, 28; ae_selp24.lh	aep2, aep2, aep3 }
     c86:	e2442e121f 	{ ae_lp24f.i	aep1, a2, 4; ae_mulaafp24s.hh.ll	aeq0, aep1, aep2 }
     c8b:	dd2def134f 	{ ae_lp24x2f.i	aep1, a3, 32; ae_selp24.lh	aep3, aep3, aep1 }
     c90:	e2802e022f 	{ ae_lp24f.i	aep0, a2, 8; ae_mulaafp24s.hh.ll	aeq0, aep0, aep4 }
     c95:	e2642e423f 	{ ae_lp24f.i	aep4, a2, 12; ae_mulaafp24s.hh.ll	aeq0, aep1, aep3 }
     c9a:	de81af835f 	{ ae_lp24x2f.iu	aep0, a3, 40; ae_selp24.lh	aep4, aep0, aep4 }
     c9f:	e2802e824f 	{ ae_lp24f.iu	aep0, a2, 16; ae_mulaafp24s.hh.ll	aeq0, aep0, aep4 }
     ca4:	060834        	ae_roundsq32sym	aeq0, aeq0
     ca7:	035314        	ae_mulq32sp16s.l	aeq0, aeq0, aep5
     caa:	360094        	ae_sllisq56s	aeq0, aeq0, 2
     cad:	1b8e04        	ae_roundsp16q48sym	aep0, aeq0
     cb0:	428a14        	ae_sp16f.l.iu	aep0, a10, 2

00000cb3 <SBC_synthesis8_per_channel+0x123>:
     cb3:	1a1d62        	l16ui	a6, a13, 52
     cb6:	090c           	movi.n	a9, 0
     cb8:	f03d           	nop.n
     cba:	a08680        	addx4	a8, a6, a8
     cbd:	2f1be7        	beq	a11, a14, cf0 <SBC_synthesis8_per_channel+0x160>

00000cc0 <SBC_synthesis8_per_channel+0x130>:
     cc0:	f3f680        	wur.ae_cbegin0	a8
     cc3:	850c           	movi.n	a5, 8
     cc5:	521f42        	l16ui	a4, a15, 164
     cc8:	20c862        	addi	a6, a8, 32
     ccb:	f3f760        	wur.ae_cend0	a6
     cce:	f7c422        	addi	a2, a4, -9
     cd1:	441b           	addi.n	a4, a4, 1
     cd3:	834920        	moveqz	a4, a9, a2
     cd6:	082d           	mov.n	a2, a8
     cd8:	1149           	s32i.n	a4, a1, 4
     cda:	282f92        	l32i	a9, a15, 160
     cdd:	525f42        	s16i	a4, a15, 164
     ce0:	046040        	extui	a6, a4, 0, 1
     ce3:	a04490        	addx4	a4, a4, a9
     ce6:	ed9656        	bnez	a6, bc3 <SBC_synthesis8_per_channel+0x33>
     ce9:	fce631        	l32r	a3, 84 (1ae0 <SBC_cos_coef_8>)
     cec:	ffb6c6        	j	bcb <SBC_synthesis8_per_channel+0x3b>

00000cef <SBC_synthesis8_per_channel+0x15f>:
	...

00000cf0 <SBC_synthesis8_per_channel+0x160>:
     cf0:	f01d           	retw.n

00000cf2 <SBC_synthesis8_per_channel+0x162>:
     cf2:	00000000 61360000                                ......

00000cf8 <sbc_bit_allocation>:
     cf8:	006136        	entry	a1, 48
     cfb:	02ed           	mov.n	a14, a2
     cfd:	051252        	l16ui	a5, a2, 10
     d00:	0412c2        	l16ui	a12, a2, 8
     d03:	061292        	l16ui	a9, a2, 12
     d06:	1a12b2        	l16ui	a11, a2, 52
     d09:	04c1c0        	extui	a12, a12, 1, 1
     d0c:	09ad           	mov.n	a10, a9
     d0e:	93abc0        	movnez	a10, a11, a12
     d11:	fcddb1        	l32r	a11, 88 (acc <SBC_synthesis4_per_channel+0xe4>)
     d14:	9aad           	ae_zext16	a10, a10
     d16:	11a9           	s32i.n	a10, a1, 4
     d18:	321526        	beqi	a5, 1, d4e <sbc_bit_allocation+0x56>
     d1b:	140c           	movi.n	a4, 1
     d1d:	fcc9d2        	addi	a13, a9, -4
     d20:	ffca22        	addi	a2, a10, -1
     d23:	823d           	ae_sext16	a3, a2
     d25:	408d16        	beqz	a13, 1131 <sbc_bit_allocation+0x439>
     d28:	22e2f7        	bbsi	a2, 15, d4e <sbc_bit_allocation+0x56>
     d2b:	fcd881        	l32r	a8, 8c (1930 <SBC_OFFSET8>)
     d2e:	532a40        	max	a2, a10, a4
     d31:	19a276        	loopgtz	a2, d4e <sbc_bit_allocation+0x56>
     d34:	021e22        	l16ui	a2, a14, 4
     d37:	244030        	extui	a4, a3, 0, 3
     d3a:	9053e0        	addx2	a5, a3, a14
     d3d:	330b           	addi.n	a3, a3, -1
     d3f:	b02240        	addx8	a2, a2, a4
     d42:	45ba           	add.n	a4, a5, a11
     d44:	833d           	ae_sext16	a3, a3
     d46:	902280        	addx2	a2, a2, a8
     d49:	222d           	ae_l16si.n	a2, a2, 0
     d4b:	7f5422        	s16i	a2, a4, 254

00000d4e <sbc_bit_allocation+0x56>:
     d4e:	0f1c           	movi.n	a15, 16
     d50:	0a0c           	movi.n	a10, 0
     d52:	01a9           	s32i.n	a10, a1, 0
     d54:	001106        	j	d9c <sbc_bit_allocation+0xa4>

00000d57 <sbc_bit_allocation+0x5f>:
     d57:	000000                                        ...

00000d5a <sbc_bit_allocation+0x62>:
     d5a:	01c8           	l32i.n	a12, a1, 0
     d5c:	c1c9c0        	mul16u	a12, a9, a12
     d5f:	90cce0        	addx2	a12, a12, a14
     d62:	04dcc2        	addmi	a12, a12, 0x400
     d65:	78ccc2        	addi	a12, a12, 120

00000d68 <sbc_bit_allocation+0x70>:
     d68:	15b2d7        	bgeu	a2, a13, d81 <sbc_bit_allocation+0x89>
     d6b:	01a8           	l32i.n	a10, a1, 0
     d6d:	aa1b           	addi.n	a10, a10, 1
     d6f:	8259a0        	mull	a5, a9, a10
     d72:	9055e0        	addx2	a5, a5, a14
     d75:	04d552        	addmi	a5, a5, 0x400
     d78:	78c552        	addi	a5, a5, 120
     d7b:	02bc57        	bgeu	a12, a5, d81 <sbc_bit_allocation+0x89>
     d7e:	00abc6        	j	1031 <sbc_bit_allocation+0x339>

00000d81 <sbc_bit_allocation+0x89>:
     d81:	041e72        	l16ui	a7, a14, 8
     d84:	260c           	movi.n	a6, 2
     d86:	106760        	and	a6, a7, a6

00000d89 <sbc_bit_allocation+0x91>:
     d89:	0188           	l32i.n	a8, a1, 0
     d8b:	981b           	addi.n	a9, a8, 1
     d8d:	998d           	ae_zext16	a8, a9
     d8f:	0189           	s32i.n	a8, a1, 0
     d91:	1b4616        	beqz	a6, f49 <sbc_bit_allocation+0x251>

00000d94 <sbc_bit_allocation+0x9c>:
     d94:	01a8           	l32i.n	a10, a1, 0
     d96:	021aa6        	blti	a10, 1, d9c <sbc_bit_allocation+0xa4>
     d99:	006d46        	j	f52 <sbc_bit_allocation+0x25a>

00000d9c <sbc_bit_allocation+0xa4>:
     d9c:	0158           	l32i.n	a5, a1, 0
     d9e:	061ec2        	l16ui	a12, a14, 12
     da1:	051ed2        	l16ui	a13, a14, 10
     da4:	c1cc50        	mul16u	a12, a12, a5
     da7:	dd0b           	addi.n	a13, a13, -1
     da9:	90cce0        	addx2	a12, a12, a14
     dac:	38cc62        	addi	a6, a12, 56
     daf:	0cdc72        	addmi	a7, a12, 0xc00
     db2:	aac772        	addi	a7, a7, -86
     db5:	2add16        	beqz	a13, 1066 <sbc_bit_allocation+0x36e>
     db8:	1188           	l32i.n	a8, a1, 4
     dba:	36e816        	beqz	a8, 112c <sbc_bit_allocation+0x434>
     dbd:	090c           	movi.n	a9, 0
     dbf:	040c           	movi.n	a4, 0
     dc1:	319876        	loopnez	a8, df6 <sbc_bit_allocation+0xfe>
     dc4:	90a960        	addx2	a10, a9, a6
     dc7:	4aad           	ae_l16ui.n	a10, a10, 0
     dc9:	1129f0        	slli	a2, a9, 1
     dcc:	9039e0        	addx2	a3, a9, a14
     dcf:	043a16        	beqz	a10, e16 <sbc_bit_allocation+0x11e>
     dd2:	33ba           	add.n	a3, a3, a11
     dd4:	7f9332        	l16si	a3, a3, 254
     dd7:	080c           	movi.n	a8, 0
     dd9:	c03a30        	sub	a3, a10, a3
     ddc:	833d           	ae_sext16	a3, a3
     dde:	728830        	salt	a8, a8, a3
     de1:	215130        	srai	a5, a3, 1
     de4:	933580        	movnez	a3, a5, a8
     de7:	833d           	ae_sext16	a3, a3

00000de9 <sbc_bit_allocation+0xf1>:
     de9:	991b           	addi.n	a9, a9, 1
     deb:	534340        	max	a4, a3, a4
     dee:	527a           	add.n	a5, a2, a7
     df0:	653d           	ae_s16i.n	a3, a5, 0
     df2:	844d           	ae_sext16	a4, a4
     df4:	999d           	ae_zext16	a9, a9

00000df6 <sbc_bit_allocation+0xfe>:
     df6:	0021d2        	l32i	a13, a1, 0
     df9:	061ec2        	l16ui	a12, a14, 12
     dfc:	c1ccd0        	mul16u	a12, a12, a13
     dff:	90cce0        	addx2	a12, a12, a14
     e02:	0cdc72        	addmi	a7, a12, 0xc00
     e05:	aac772        	addi	a7, a7, -86

00000e08 <sbc_bit_allocation+0x110>:
     e08:	020c           	movi.n	a2, 0
     e0a:	081ed2        	l16ui	a13, a14, 16
     e0d:	060c           	movi.n	a6, 0
     e0f:	441b           	addi.n	a4, a4, 1
     e11:	844d           	ae_sext16	a4, a4
     e13:	000106        	j	e1b <sbc_bit_allocation+0x123>

00000e16 <sbc_bit_allocation+0x11e>:
     e16:	b37c           	movi.n	a3, -5
     e18:	fff346        	j	de9 <sbc_bit_allocation+0xf1>

00000e1b <sbc_bit_allocation+0x123>:
     e1b:	1158           	l32i.n	a5, a1, 4
     e1d:	262a           	add.n	a2, a6, a2
     e1f:	922d           	ae_zext16	a2, a2
     e21:	10b516        	beqz	a5, f30 <sbc_bit_allocation+0x238>
     e24:	a4fb           	addi.n	a10, a4, 15
     e26:	060c           	movi.n	a6, 0
     e28:	090c           	movi.n	a9, 0
     e2a:	219576        	loopnez	a5, e4f <sbc_bit_allocation+0x157>
     e2d:	903970        	addx2	a3, a9, a7
     e30:	233d           	ae_l16si.n	a3, a3, 0
     e32:	991b           	addi.n	a9, a9, 1
     e34:	999d           	ae_zext16	a9, a9
     e36:	09a437        	bge	a4, a3, e43 <sbc_bit_allocation+0x14b>
     e39:	06a3a7        	bge	a3, a10, e43 <sbc_bit_allocation+0x14b>
     e3c:	661b           	addi.n	a6, a6, 1
     e3e:	966d           	ae_zext16	a6, a6
     e40:	000246        	j	e4d <sbc_bit_allocation+0x155>

00000e43 <sbc_bit_allocation+0x14b>:
     e43:	c05430        	sub	a5, a4, a3
     e46:	862b           	addi.n	a8, a6, 2
     e48:	836850        	moveqz	a6, a8, a5
     e4b:	966d           	ae_zext16	a6, a6

00000e4d <sbc_bit_allocation+0x155>:
     e4d:	f03d           	nop.n

00000e4f <sbc_bit_allocation+0x157>:
     e4f:	809620        	add	a9, a6, a2
     e52:	ffc442        	addi	a4, a4, -1
     e55:	844d           	ae_sext16	a4, a4
     e57:	c029d7        	blt	a9, a13, e1b <sbc_bit_allocation+0x123>
     e5a:	0799d7        	bne	a9, a13, e65 <sbc_bit_allocation+0x16d>

00000e5d <sbc_bit_allocation+0x165>:
     e5d:	440b           	addi.n	a4, a4, -1
     e5f:	262a           	add.n	a2, a6, a2
     e61:	922d           	ae_zext16	a2, a2
     e63:	844d           	ae_sext16	a4, a4

00000e65 <sbc_bit_allocation+0x16d>:
     e65:	d42b           	addi.n	a13, a4, 2
     e67:	090c           	movi.n	a9, 0
     e69:	1158           	l32i.n	a5, a1, 4
     e6b:	04dcc2        	addmi	a12, a12, 0x400
     e6e:	78ccc2        	addi	a12, a12, 120
     e71:	1f9576        	loopnez	a5, e94 <sbc_bit_allocation+0x19c>
     e74:	903970        	addx2	a3, a9, a7
     e77:	233d           	ae_l16si.n	a3, a3, 0
     e79:	90a9c0        	addx2	a10, a9, a12
     e7c:	991b           	addi.n	a9, a9, 1
     e7e:	06a3d7        	bge	a3, a13, e88 <sbc_bit_allocation+0x190>
     e81:	050c           	movi.n	a5, 0
     e83:	6a5d           	ae_s16i.n	a5, a10, 0
     e85:	000246        	j	e92 <sbc_bit_allocation+0x19a>

00000e88 <sbc_bit_allocation+0x190>:
     e88:	c08340        	sub	a8, a3, a4
     e8b:	988d           	ae_zext16	a8, a8
     e8d:	638f80        	minu	a8, a15, a8
     e90:	6a8d           	ae_s16i.n	a8, a10, 0

00000e92 <sbc_bit_allocation+0x19a>:
     e92:	999d           	ae_zext16	a9, a9

00000e94 <sbc_bit_allocation+0x19c>:
     e94:	041e72        	l16ui	a7, a14, 8
     e97:	2a0c           	movi.n	a10, 2
     e99:	1067a0        	and	a6, a7, a10
     e9c:	0207a7        	bnone	a7, a10, ea2 <sbc_bit_allocation+0x1aa>
     e9f:	ffb986        	j	d89 <sbc_bit_allocation+0x91>
     ea2:	01c8           	l32i.n	a12, a1, 0
     ea4:	061e92        	l16ui	a9, a14, 12
     ea7:	c1c9c0        	mul16u	a12, a9, a12
     eaa:	081ed2        	l16ui	a13, a14, 16
     ead:	90cce0        	addx2	a12, a12, a14
     eb0:	0cdc72        	addmi	a7, a12, 0xc00
     eb3:	aac772        	addi	a7, a7, -86
     eb6:	04dcc2        	addmi	a12, a12, 0x400
     eb9:	78ccc2        	addi	a12, a12, 120
     ebc:	0232d7        	bltu	a2, a13, ec2 <sbc_bit_allocation+0x1ca>
     ebf:	ffaf86        	j	d81 <sbc_bit_allocation+0x89>
     ec2:	01a8           	l32i.n	a10, a1, 0
     ec4:	aa1b           	addi.n	a10, a10, 1
     ec6:	8259a0        	mull	a5, a9, a10
     ec9:	9055e0        	addx2	a5, a5, a14
     ecc:	04d552        	addmi	a5, a5, 0x400
     ecf:	78c552        	addi	a5, a5, 120
     ed2:	023c57        	bltu	a12, a5, ed8 <sbc_bit_allocation+0x1e0>
     ed5:	ffa3c6        	j	d68 <sbc_bit_allocation+0x70>
     ed8:	341b           	addi.n	a3, a4, 1
     eda:	338076        	loop	a0, f11 <sbc_bit_allocation+0x219>
     edd:	4c6d           	ae_l16ui.n	a6, a12, 0
     edf:	3126b6        	bltui	a6, 2, f14 <sbc_bit_allocation+0x21c>
     ee2:	2eb6f6        	bgeui	a6, 16, f14 <sbc_bit_allocation+0x21c>
     ee5:	561b           	addi.n	a5, a6, 1
     ee7:	221b           	addi.n	a2, a2, 1
     ee9:	922d           	ae_zext16	a2, a2
     eeb:	6c5d           	ae_s16i.n	a5, a12, 0
     eed:	061e92        	l16ui	a9, a14, 12
     ef0:	081ed2        	l16ui	a13, a14, 16

00000ef3 <sbc_bit_allocation+0x1fb>:
     ef3:	cc2b           	addi.n	a12, a12, 2
     ef5:	772b           	addi.n	a7, a7, 2
     ef7:	0232d7        	bltu	a2, a13, efd <sbc_bit_allocation+0x205>
     efa:	ff9706        	j	d5a <sbc_bit_allocation+0x62>
     efd:	8289a0        	mull	a8, a9, a10
     f00:	9088e0        	addx2	a8, a8, a14
     f03:	04d882        	addmi	a8, a8, 0x400
     f06:	78c882        	addi	a8, a8, 120
     f09:	023c87        	bltu	a12, a8, f0f <sbc_bit_allocation+0x217>
     f0c:	ff9286        	j	d5a <sbc_bit_allocation+0x62>
     f0f:	f03d           	nop.n

00000f11 <sbc_bit_allocation+0x219>:
     f11:	fff146        	j	eda <sbc_bit_allocation+0x1e2>

00000f14 <sbc_bit_allocation+0x21c>:
     f14:	275d           	ae_l16si.n	a5, a7, 0
     f16:	821b           	addi.n	a8, a2, 1
     f18:	d79537        	bne	a5, a3, ef3 <sbc_bit_allocation+0x1fb>
     f1b:	d4a8d7        	bge	a8, a13, ef3 <sbc_bit_allocation+0x1fb>
     f1e:	250c           	movi.n	a5, 2
     f20:	222b           	addi.n	a2, a2, 2
     f22:	922d           	ae_zext16	a2, a2
     f24:	6c5d           	ae_s16i.n	a5, a12, 0
     f26:	061e92        	l16ui	a9, a14, 12
     f29:	081ed2        	l16ui	a13, a14, 16
     f2c:	fff0c6        	j	ef3 <sbc_bit_allocation+0x1fb>

00000f2f <sbc_bit_allocation+0x237>:
	...

00000f30 <sbc_bit_allocation+0x238>:
     f30:	060c           	movi.n	a6, 0
     f32:	440b           	addi.n	a4, a4, -1
     f34:	844d           	ae_sext16	a4, a4
     f36:	02a2d7        	bge	a2, a13, f3c <sbc_bit_allocation+0x244>
     f39:	ffb786        	j	e1b <sbc_bit_allocation+0x123>
     f3c:	060c           	movi.n	a6, 0
     f3e:	c052d0        	sub	a5, a2, a13
     f41:	f18516        	beqz	a5, e5d <sbc_bit_allocation+0x165>
     f44:	ffc746        	j	e65 <sbc_bit_allocation+0x16d>

00000f47 <sbc_bit_allocation+0x24f>:
	...

00000f49 <sbc_bit_allocation+0x251>:
     f49:	e47716        	beqz	a7, d94 <sbc_bit_allocation+0x9c>
     f4c:	0228e6        	bgei	a8, 2, f52 <sbc_bit_allocation+0x25a>
     f4f:	ff9246        	j	d9c <sbc_bit_allocation+0xa4>

00000f52 <sbc_bit_allocation+0x25a>:
     f52:	1b4616        	beqz	a6, 110a <sbc_bit_allocation+0x412>
     f55:	081ed2        	l16ui	a13, a14, 16
     f58:	0232d7        	bltu	a2, a13, f5e <sbc_bit_allocation+0x266>
     f5b:	006ac6        	j	110a <sbc_bit_allocation+0x412>
     f5e:	061e92        	l16ui	a9, a14, 12
     f61:	137916        	beqz	a9, 109c <sbc_bit_allocation+0x3a4>
     f64:	04de92        	addmi	a9, a14, 0x400
     f67:	78c992        	addi	a9, a9, 120
     f6a:	496d           	ae_l16ui.n	a6, a9, 0
     f6c:	0cde72        	addmi	a7, a14, 0xc00
     f6f:	aac772        	addi	a7, a7, -86
     f72:	0226f6        	bgeui	a6, 2, f78 <sbc_bit_allocation+0x280>
     f75:	0064c6        	j	110c <sbc_bit_allocation+0x414>
     f78:	02b6b6        	bltui	a6, 16, f7e <sbc_bit_allocation+0x286>
     f7b:	006346        	j	110c <sbc_bit_allocation+0x414>
     f7e:	361b           	addi.n	a3, a6, 1
     f80:	221b           	addi.n	a2, a2, 1
     f82:	922d           	ae_zext16	a2, a2
     f84:	693d           	ae_s16i.n	a3, a9, 0
     f86:	341b           	addi.n	a3, a4, 1
     f88:	081ed2        	l16ui	a13, a14, 16

00000f8b <sbc_bit_allocation+0x293>:
     f8b:	040c           	movi.n	a4, 0

00000f8d <sbc_bit_allocation+0x295>:
     f8d:	0232d7        	bltu	a2, a13, f93 <sbc_bit_allocation+0x29b>
     f90:	005d86        	j	110a <sbc_bit_allocation+0x412>
     f93:	061e92        	l16ui	a9, a14, 12
     f96:	023497        	bltu	a4, a9, f9c <sbc_bit_allocation+0x2a4>
     f99:	003fc6        	j	109c <sbc_bit_allocation+0x3a4>
     f9c:	794a           	add.n	a7, a9, a4
     f9e:	9077e0        	addx2	a7, a7, a14
     fa1:	04d7c2        	addmi	a12, a7, 0x400
     fa4:	78ccc2        	addi	a12, a12, 120
     fa7:	4c6d           	ae_l16ui.n	a6, a12, 0
     fa9:	0cd772        	addmi	a7, a7, 0xc00
     fac:	aac772        	addi	a7, a7, -86
     faf:	4526b6        	bltui	a6, 2, ff8 <sbc_bit_allocation+0x300>
     fb2:	42b6f6        	bgeui	a6, 16, ff8 <sbc_bit_allocation+0x300>
     fb5:	561b           	addi.n	a5, a6, 1
     fb7:	221b           	addi.n	a2, a2, 1
     fb9:	922d           	ae_zext16	a2, a2
     fbb:	6c5d           	ae_s16i.n	a5, a12, 0
     fbd:	081ed2        	l16ui	a13, a14, 16

00000fc0 <sbc_bit_allocation+0x2c8>:
     fc0:	441b           	addi.n	a4, a4, 1
     fc2:	944d           	ae_zext16	a4, a4
     fc4:	0232d7        	bltu	a2, a13, fca <sbc_bit_allocation+0x2d2>
     fc7:	004fc6        	j	110a <sbc_bit_allocation+0x412>
     fca:	061e52        	l16ui	a5, a14, 12
     fcd:	023457        	bltu	a4, a5, fd3 <sbc_bit_allocation+0x2db>
     fd0:	003206        	j	109c <sbc_bit_allocation+0x3a4>
     fd3:	9074e0        	addx2	a7, a4, a14
     fd6:	04d7c2        	addmi	a12, a7, 0x400
     fd9:	78ccc2        	addi	a12, a12, 120
     fdc:	4c6d           	ae_l16ui.n	a6, a12, 0
     fde:	0cd772        	addmi	a7, a7, 0xc00
     fe1:	aac772        	addi	a7, a7, -86
     fe4:	2826b6        	bltui	a6, 2, 1010 <sbc_bit_allocation+0x318>
     fe7:	25b6f6        	bgeui	a6, 16, 1010 <sbc_bit_allocation+0x318>
     fea:	561b           	addi.n	a5, a6, 1
     fec:	221b           	addi.n	a2, a2, 1
     fee:	922d           	ae_zext16	a2, a2
     ff0:	6c5d           	ae_s16i.n	a5, a12, 0
     ff2:	081ed2        	l16ui	a13, a14, 16
     ff5:	ffe506        	j	f8d <sbc_bit_allocation+0x295>

00000ff8 <sbc_bit_allocation+0x300>:
     ff8:	278d           	ae_l16si.n	a8, a7, 0
     ffa:	c29837        	bne	a8, a3, fc0 <sbc_bit_allocation+0x2c8>
     ffd:	921b           	addi.n	a9, a2, 1
     fff:	bda9d7        	bge	a9, a13, fc0 <sbc_bit_allocation+0x2c8>
    1002:	250c           	movi.n	a5, 2
    1004:	222b           	addi.n	a2, a2, 2
    1006:	922d           	ae_zext16	a2, a2
    1008:	6c5d           	ae_s16i.n	a5, a12, 0
    100a:	081ed2        	l16ui	a13, a14, 16
    100d:	ffebc6        	j	fc0 <sbc_bit_allocation+0x2c8>

00001010 <sbc_bit_allocation+0x318>:
    1010:	278d           	ae_l16si.n	a8, a7, 0
    1012:	021837        	beq	a8, a3, 1018 <sbc_bit_allocation+0x320>
    1015:	ffdd06        	j	f8d <sbc_bit_allocation+0x295>
    1018:	921b           	addi.n	a9, a2, 1
    101a:	0229d7        	blt	a9, a13, 1020 <sbc_bit_allocation+0x328>
    101d:	ffdb06        	j	f8d <sbc_bit_allocation+0x295>
    1020:	250c           	movi.n	a5, 2
    1022:	222b           	addi.n	a2, a2, 2
    1024:	922d           	ae_zext16	a2, a2
    1026:	6c5d           	ae_s16i.n	a5, a12, 0
    1028:	081ed2        	l16ui	a13, a14, 16
    102b:	ffd786        	j	f8d <sbc_bit_allocation+0x295>

0000102e <sbc_bit_allocation+0x336>:
    102e:	000000                                        ...

00001031 <sbc_bit_allocation+0x339>:
    1031:	2e8076        	loop	a0, 1063 <sbc_bit_allocation+0x36b>
    1034:	4c6d           	ae_l16ui.n	a6, a12, 0
    1036:	0ab6f6        	bgeui	a6, 16, 1044 <sbc_bit_allocation+0x34c>
    1039:	221b           	addi.n	a2, a2, 1
    103b:	561b           	addi.n	a5, a6, 1
    103d:	6c5d           	ae_s16i.n	a5, a12, 0
    103f:	081ed2        	l16ui	a13, a14, 16
    1042:	922d           	ae_zext16	a2, a2

00001044 <sbc_bit_allocation+0x34c>:
    1044:	cc2b           	addi.n	a12, a12, 2
    1046:	0232d7        	bltu	a2, a13, 104c <sbc_bit_allocation+0x354>
    1049:	ff4d06        	j	d81 <sbc_bit_allocation+0x89>
    104c:	061e82        	l16ui	a8, a14, 12
    104f:	8288a0        	mull	a8, a8, a10
    1052:	9088e0        	addx2	a8, a8, a14
    1055:	04d882        	addmi	a8, a8, 0x400
    1058:	78c882        	addi	a8, a8, 120
    105b:	023c87        	bltu	a12, a8, 1061 <sbc_bit_allocation+0x369>
    105e:	ff47c6        	j	d81 <sbc_bit_allocation+0x89>
    1061:	f03d           	nop.n

00001063 <sbc_bit_allocation+0x36b>:
    1063:	fff286        	j	1031 <sbc_bit_allocation+0x339>

00001066 <sbc_bit_allocation+0x36e>:
    1066:	1198           	l32i.n	a9, a1, 4
    1068:	0f1916        	beqz	a9, 115d <sbc_bit_allocation+0x465>
    106b:	00a022        	movi	a2, 0
    106e:	00a042        	movi	a4, 0
    1071:	129976        	loopnez	a9, 1087 <sbc_bit_allocation+0x38f>
    1074:	903260        	addx2	a3, a2, a6
    1077:	905270        	addx2	a5, a2, a7
    107a:	233d           	ae_l16si.n	a3, a3, 0
    107c:	221b           	addi.n	a2, a2, 1
    107e:	922d           	ae_zext16	a2, a2
    1080:	534340        	max	a4, a3, a4
    1083:	653d           	ae_s16i.n	a3, a5, 0
    1085:	844d           	ae_sext16	a4, a4

00001087 <sbc_bit_allocation+0x38f>:
    1087:	01d8           	l32i.n	a13, a1, 0
    1089:	061ec2        	l16ui	a12, a14, 12
    108c:	c1ccd0        	mul16u	a12, a12, a13
    108f:	90cce0        	addx2	a12, a12, a14
    1092:	0cdc72        	addmi	a7, a12, 0xc00
    1095:	aac772        	addi	a7, a7, -86
    1098:	ff5b06        	j	e08 <sbc_bit_allocation+0x110>

0000109b <sbc_bit_allocation+0x3a3>:
	...

0000109c <sbc_bit_allocation+0x3a4>:
    109c:	6ab2d7        	bgeu	a2, a13, 110a <sbc_bit_allocation+0x412>
    109f:	061ef2        	l16ui	a15, a14, 12
    10a2:	064f16        	beqz	a15, 110a <sbc_bit_allocation+0x412>
    10a5:	04de92        	addmi	a9, a14, 0x400
    10a8:	78c992        	addi	a9, a9, 120
    10ab:	496d           	ae_l16ui.n	a6, a9, 0
    10ad:	0ab6f6        	bgeui	a6, 16, 10bb <sbc_bit_allocation+0x3c3>
    10b0:	561b           	addi.n	a5, a6, 1
    10b2:	221b           	addi.n	a2, a2, 1
    10b4:	922d           	ae_zext16	a2, a2
    10b6:	695d           	ae_s16i.n	a5, a9, 0
    10b8:	081ed2        	l16ui	a13, a14, 16

000010bb <sbc_bit_allocation+0x3c3>:
    10bb:	040c           	movi.n	a4, 0

000010bd <sbc_bit_allocation+0x3c5>:
    10bd:	49b2d7        	bgeu	a2, a13, 110a <sbc_bit_allocation+0x412>

000010c0 <sbc_bit_allocation+0x3c8>:
    10c0:	061e92        	l16ui	a9, a14, 12
    10c3:	43b497        	bgeu	a4, a9, 110a <sbc_bit_allocation+0x412>
    10c6:	c94a           	add.n	a12, a9, a4
    10c8:	90cce0        	addx2	a12, a12, a14
    10cb:	04dcc2        	addmi	a12, a12, 0x400
    10ce:	78ccc2        	addi	a12, a12, 120
    10d1:	4c6d           	ae_l16ui.n	a6, a12, 0
    10d3:	441b           	addi.n	a4, a4, 1
    10d5:	944d           	ae_zext16	a4, a4
    10d7:	0ab6f6        	bgeui	a6, 16, 10e5 <sbc_bit_allocation+0x3ed>
    10da:	561b           	addi.n	a5, a6, 1
    10dc:	221b           	addi.n	a2, a2, 1
    10de:	922d           	ae_zext16	a2, a2
    10e0:	6c5d           	ae_s16i.n	a5, a12, 0
    10e2:	081ed2        	l16ui	a13, a14, 16

000010e5 <sbc_bit_allocation+0x3ed>:
    10e5:	21b2d7        	bgeu	a2, a13, 110a <sbc_bit_allocation+0x412>
    10e8:	061e52        	l16ui	a5, a14, 12
    10eb:	90c4e0        	addx2	a12, a4, a14
    10ee:	04dcc2        	addmi	a12, a12, 0x400
    10f1:	15b457        	bgeu	a4, a5, 110a <sbc_bit_allocation+0x412>
    10f4:	78ccc2        	addi	a12, a12, 120
    10f7:	4c6d           	ae_l16ui.n	a6, a12, 0
    10f9:	c0b6f6        	bgeui	a6, 16, 10bd <sbc_bit_allocation+0x3c5>
    10fc:	561b           	addi.n	a5, a6, 1
    10fe:	6c5d           	ae_s16i.n	a5, a12, 0
    1100:	081ed2        	l16ui	a13, a14, 16
    1103:	221b           	addi.n	a2, a2, 1
    1105:	922d           	ae_zext16	a2, a2
    1107:	b532d7        	bltu	a2, a13, 10c0 <sbc_bit_allocation+0x3c8>
    110a:	f01d           	retw.n

0000110c <sbc_bit_allocation+0x414>:
    110c:	278d           	ae_l16si.n	a8, a7, 0
    110e:	341b           	addi.n	a3, a4, 1
    1110:	c08830        	sub	a8, a8, a3
    1113:	e74856        	bnez	a8, f8b <sbc_bit_allocation+0x293>
    1116:	a21b           	addi.n	a10, a2, 1
    1118:	022ad7        	blt	a10, a13, 111e <sbc_bit_allocation+0x426>
    111b:	ff9b06        	j	f8b <sbc_bit_allocation+0x293>
    111e:	250c           	movi.n	a5, 2
    1120:	222b           	addi.n	a2, a2, 2
    1122:	922d           	ae_zext16	a2, a2
    1124:	695d           	ae_s16i.n	a5, a9, 0
    1126:	081ed2        	l16ui	a13, a14, 16
    1129:	ff9786        	j	f8b <sbc_bit_allocation+0x293>

0000112c <sbc_bit_allocation+0x434>:
    112c:	040c           	movi.n	a4, 0
    112e:	ff3586        	j	e08 <sbc_bit_allocation+0x110>

00001131 <sbc_bit_allocation+0x439>:
    1131:	048f20        	extui	a8, a2, 15, 1
    1134:	c16856        	bnez	a8, d4e <sbc_bit_allocation+0x56>
    1137:	fbd681        	l32r	a8, 90 (1910 <SBC_OFFSET4>)
    113a:	532a40        	max	a2, a10, a4
    113d:	19a276        	loopgtz	a2, 115a <sbc_bit_allocation+0x462>
    1140:	021e22        	l16ui	a2, a14, 4
    1143:	144030        	extui	a4, a3, 0, 2
    1146:	9053e0        	addx2	a5, a3, a14
    1149:	330b           	addi.n	a3, a3, -1
    114b:	a02240        	addx4	a2, a2, a4
    114e:	45ba           	add.n	a4, a5, a11
    1150:	833d           	ae_sext16	a3, a3
    1152:	902280        	addx2	a2, a2, a8
    1155:	222d           	ae_l16si.n	a2, a2, 0
    1157:	7f5422        	s16i	a2, a4, 254

0000115a <sbc_bit_allocation+0x462>:
    115a:	fefc06        	j	d4e <sbc_bit_allocation+0x56>

0000115d <sbc_bit_allocation+0x465>:
    115d:	040c           	movi.n	a4, 0
    115f:	ff2946        	j	e08 <sbc_bit_allocation+0x110>

00001162 <sbc_bit_allocation+0x46a>:
    1162:	00000000 41360000                                ......

00001168 <SAT_ADD>:
    1168:	004136        	entry	a1, 32
    116b:	36c334        	ae_cvtq48a32s	aeq3, a3
    116e:	368234        	ae_cvtq48a32s	aeq2, a2
    1171:	066b04        	ae_addsq56s	aeq1, aeq2, aeq3
    1174:	061834        	ae_roundsq32sym	aeq0, aeq1
    1177:	368204        	ae_trunca32q48	a2, aeq0
    117a:	f01d           	retw.n

0000117c <SAT_SUB>:
    117c:	004136        	entry	a1, 32
    117f:	36c334        	ae_cvtq48a32s	aeq3, a3
    1182:	368234        	ae_cvtq48a32s	aeq2, a2
    1185:	066b14        	ae_subsq56s	aeq1, aeq2, aeq3
    1188:	061834        	ae_roundsq32sym	aeq0, aeq1
    118b:	368204        	ae_trunca32q48	a2, aeq0
    118e:	f01d           	retw.n

00001190 <sbc_reconstruction>:
    1190:	006136        	entry	a1, 48
    1193:	1a1282        	l16ui	a8, a2, 52
    1196:	02d2f2        	addmi	a15, a2, 0x200
    1199:	9c2f92        	l32i	a9, a15, 0x270
    119c:	9d2fa2        	l32i	a10, a15, 0x274
    119f:	0326a4        	wur.ae_bitptr	a10
    11a2:	f3f190        	wur.ae_bithead	a9
    11a5:	70cff2        	addi	a15, a15, 112
    11a8:	ffc882        	addi	a8, a8, -1
    11ab:	988d           	ae_zext16	a8, a8
    11ad:	054896        	bltz	a8, 1205 <sbc_reconstruction+0x75>
    11b0:	01c8d2        	addi	a13, a8, 1
    11b3:	90a820        	addx2	a10, a8, a2
    11b6:	0bdac2        	addmi	a12, a10, 0xb00
    11b9:	03dab2        	addmi	a11, a10, 0x300
    11bc:	7ccb82        	addi	a8, a11, 124
    11bf:	176d07        	bbci	a13, 0, 11da <sbc_reconstruction+0x4a>
    11c2:	7acb82        	addi	a8, a11, 122
    11c5:	bc1b92        	l16ui	a9, a11, 0x178
    11c8:	1c1ae2        	l16ui	a14, a10, 56
    11cb:	feccc2        	addi	a12, a12, -2
    11ce:	fecaa2        	addi	a10, a10, -2
    11d1:	c0ee90        	sub	a14, a14, a9
    11d4:	04cee2        	addi	a14, a14, 4
    11d7:	765ce2        	s16i	a14, a12, 236

000011da <sbc_reconstruction+0x4a>:
    11da:	2191d0        	srai	a9, a13, 1
    11dd:	24a976        	loopgtz	a9, 1205 <sbc_reconstruction+0x75>
    11e0:	7e1892        	l16ui	a9, a8, 252
    11e3:	1c1ab2        	l16ui	a11, a10, 56
    11e6:	fcccc2        	addi	a12, a12, -4
    11e9:	fccaa2        	addi	a10, a10, -4
    11ec:	c09b90        	sub	a9, a11, a9
    11ef:	994b           	addi.n	a9, a9, 4
    11f1:	775c92        	s16i	a9, a12, 238
    11f4:	7d1892        	l16ui	a9, a8, 250
    11f7:	1d1ab2        	l16ui	a11, a10, 58
    11fa:	fcc882        	addi	a8, a8, -4
    11fd:	c09b90        	sub	a9, a11, a9
    1200:	994b           	addi.n	a9, a9, 4
    1202:	765c92        	s16i	a9, a12, 236

00001205 <sbc_reconstruction+0x75>:
    1205:	58c2c2        	addi	a12, a2, 88
    1208:	1b12e2        	l16ui	a14, a2, 54
    120b:	1a12a2        	l16ui	a10, a2, 52
    120e:	04d282        	addmi	a8, a2, 0x400
    1211:	0ad292        	addmi	a9, a2, 0xa00
    1214:	9cc992        	addi	a9, a9, -100
    1217:	78c882        	addi	a8, a8, 120
    121a:	f3f680        	wur.ae_cbegin0	a8
    121d:	90aa20        	addx2	a10, a10, a2
    1220:	41b1e0        	srli	a11, a14, 1
    1223:	04daa2        	addmi	a10, a10, 0x400
    1226:	78caa2        	addi	a10, a10, 120
    1229:	f3f7a0        	wur.ae_cend0	a10
    122c:	4a0c           	movi.n	a10, 4
    122e:	07ab76        	loopgtz	a11, 1239 <sbc_reconstruction+0xa9>
    1231:	fc24da88af 	{ ae_lp16x2f.c	aep0, a8, a10; nop }
    1236:	468914        	ae_sp16x2f.iu	aep0, a9, 4

00001239 <sbc_reconstruction+0xa9>:
    1239:	fb9641        	l32r	a4, 94 (19d0 <S_ARRAY>)
    123c:	060c           	movi.n	a6, 0
    123e:	230c           	movi.n	a3, 2
    1240:	09d282        	addmi	a8, a2, 0x900
    1243:	0cd2a2        	addmi	a10, a2, 0xc00
    1246:	1a1292        	l16ui	a9, a2, 52
    1249:	eacaa2        	addi	a10, a10, -22
    124c:	f3f6a0        	wur.ae_cbegin0	a10
    124f:	909920        	addx2	a9, a9, a2
    1252:	0cd992        	addmi	a9, a9, 0xc00
    1255:	eac992        	addi	a9, a9, -22
    1258:	f3f790        	wur.ae_cend0	a9
    125b:	a4c882        	addi	a8, a8, -92
    125e:	389e76        	loopnez	a14, 129a <sbc_reconstruction+0x10a>
    1261:	7e98b2        	l16si	a11, a8, 252
    1264:	fc24d90a3f 	{ ae_lp16f.c	aep0, a10, a3; nop }
    1269:	420104        	ae_sp16f.l.i	aep0, a1, 0
    126c:	052b16        	beqz	a11, 12c2 <sbc_reconstruction+0x132>
    126f:	21ed           	ae_l16si.n	a14, a1, 0
    1271:	03d7b4        	ae_lb	a13, a11
    1274:	7f2f92        	l32i	a9, a15, 0x1fc
    1277:	a05b40        	addx4	a5, a11, a4
    127a:	440504        	ae_lp16x2f.i	aep0, a5, 0
    127d:	1c19b4        	ae_db	a9, a11
    1280:	7f6f92        	s32i	a9, a15, 0x1fc
    1283:	029e96        	bltz	a14, 12b0 <sbc_reconstruction+0x120>
    1286:	301dd4        	ae_cvtp24a16x2.ll	aep1, a13, a13
    1289:	071094        	ae_subp24	aep1, aep1, aep0
    128c:	020164        	ae_mulp24s.lh	aeq0, aep0, aep1
    128f:	340e94        	ae_sllaq56	aeq0, aeq0, a14

00001292 <sbc_reconstruction+0x102>:
    1292:	368904        	ae_trunca32q48	a9, aeq0
    1295:	09ec94        	ae_s32ip	a9, a12, 4

00001298 <sbc_reconstruction+0x108>:
    1298:	882b           	addi.n	a8, a8, 2

0000129a <sbc_reconstruction+0x10a>:
    129a:	e3bf10        	rur.ae_bithead	a11
    129d:	00c2c4        	rur.ae_bitptr	a12
    12a0:	1712a2        	l16ui	a10, a2, 46
    12a3:	816fc2        	s32i	a12, a15, 0x204
    12a6:	806fb2        	s32i	a11, a15, 0x200
    12a9:	cadc           	bnez.n	a10, 12c9 <sbc_reconstruction+0x139>
    12ab:	0026c6        	j	134a <sbc_reconstruction+0x1ba>

000012ae <sbc_reconstruction+0x11e>:
	...

000012b0 <sbc_reconstruction+0x120>:
    12b0:	302dd4        	ae_cvtp24a16x2.ll	aep2, a13, a13
    12b3:	0720a4        	ae_subp24	aep2, aep2, aep0
    12b6:	020264        	ae_mulp24s.lh	aeq0, aep0, aep2
    12b9:	6090e0        	neg	a9, a14
    12bc:	3409b4        	ae_sraaq56	aeq0, aeq0, a9
    12bf:	fff3c6        	j	1292 <sbc_reconstruction+0x102>

000012c2 <sbc_reconstruction+0x132>:
    12c2:	09ec64        	ae_s32ip	a6, a12, 4
    12c5:	fff3c6        	j	1298 <sbc_reconstruction+0x108>

000012c8 <sbc_reconstruction+0x138>:
	...

000012c9 <sbc_reconstruction+0x139>:
    12c9:	5ca3c2        	movi	a12, 0x35c
    12cc:	7e2fa2        	l32i	a10, a15, 0x1f8
    12cf:	5aa3b2        	movi	a11, 0x35a
    12d2:	1b1282        	l16ui	a8, a2, 54
    12d5:	c0aa20        	sub	a10, a10, a2
    12d8:	fcdaa2        	addmi	a10, a10, 0xfffffc00
    12db:	a8cad2        	addi	a13, a10, -88
    12de:	a9caa2        	addi	a10, a10, -87
    12e1:	b3add0        	movgez	a10, a13, a13
    12e4:	f4a1a0        	extui	a10, a10, 1, 16
    12e7:	61f826        	beqi	a8, 0x100, 134c <sbc_reconstruction+0x1bc>

000012ea <sbc_reconstruction+0x15a>:
    12ea:	5c18a6        	blti	a8, 1, 134a <sbc_reconstruction+0x1ba>
    12ed:	070c           	movi.n	a7, 0
    12ef:	da0b           	addi.n	a13, a10, -1
    12f1:	e2ba           	add.n	a14, a2, a11
    12f3:	31e9           	s32i.n	a14, a1, 12
    12f5:	21d9           	s32i.n	a13, a1, 8
    12f7:	90dd20        	addx2	a13, a13, a2
    12fa:	ddca           	add.n	a13, a13, a12
    12fc:	11d9           	s32i.n	a13, a1, 4

000012fe <sbc_reconstruction+0x16e>:
    12fe:	21f8           	l32i.n	a15, a1, 8
    1300:	1138           	l32i.n	a3, a1, 4
    1302:	03bf96        	bltz	a15, 1341 <sbc_reconstruction+0x1b1>

00001305 <sbc_reconstruction+0x175>:
    1305:	7e1342        	l16ui	a4, a3, 252
    1308:	061252        	l16ui	a5, a2, 12
    130b:	fb6381        	l32r	a8, 98 (1168 <SAT_ADD>)
    130e:	474a           	add.n	a4, a7, a4
    1310:	805540        	add	a5, a5, a4
    1313:	a05520        	addx4	a5, a5, a2
    1316:	a04420        	addx4	a4, a4, a2
    1319:	1624a2        	l32i	a10, a4, 88
    131c:	1625b2        	l32i	a11, a5, 88
    131f:	0008e0        	callx8	a8
    1322:	0a6d           	mov.n	a6, a10
    1324:	fb5e81        	l32r	a8, 9c (117c <SAT_SUB>)
    1327:	1625b2        	l32i	a11, a5, 88
    132a:	1624a2        	l32i	a10, a4, 88
    132d:	0008e0        	callx8	a8
    1330:	fec332        	addi	a3, a3, -2
    1333:	3198           	l32i.n	a9, a1, 12
    1335:	166462        	s32i	a6, a4, 88
    1338:	1665a2        	s32i	a10, a5, 88
    133b:	c69397        	bne	a3, a9, 1305 <sbc_reconstruction+0x175>
    133e:	1b1282        	l16ui	a8, a2, 54

00001341 <sbc_reconstruction+0x1b1>:
    1341:	0612a2        	l16ui	a10, a2, 12
    1344:	907a70        	addx2	a7, a10, a7
    1347:	b32787        	blt	a7, a8, 12fe <sbc_reconstruction+0x16e>
    134a:	f01d           	retw.n

0000134c <sbc_reconstruction+0x1bc>:
    134c:	0612d2        	l16ui	a13, a2, 12
    134f:	978d66        	bnei	a13, 8, 12ea <sbc_reconstruction+0x15a>
    1352:	070c           	movi.n	a7, 0
    1354:	03d2f2        	addmi	a15, a2, 0x300
    1357:	ea0b           	addi.n	a14, a10, -1
    1359:	21e9           	s32i.n	a14, a1, 8
    135b:	5acff2        	addi	a15, a15, 90
    135e:	31f9           	s32i.n	a15, a1, 12
    1360:	90ee20        	addx2	a14, a14, a2
    1363:	03dee2        	addmi	a14, a14, 0x300
    1366:	5ccee2        	addi	a14, a14, 92
    1369:	11e9           	s32i.n	a14, a1, 4

0000136b <sbc_reconstruction+0x1db>:
    136b:	2188           	l32i.n	a8, a1, 8
    136d:	1138           	l32i.n	a3, a1, 4
    136f:	02f896        	bltz	a8, 13a2 <sbc_reconstruction+0x212>

00001372 <sbc_reconstruction+0x1e2>:
    1372:	7e1342        	l16ui	a4, a3, 252
    1375:	fb4881        	l32r	a8, 98 (1168 <SAT_ADD>)
    1378:	474a           	add.n	a4, a7, a4
    137a:	a04420        	addx4	a4, a4, a2
    137d:	1624a2        	l32i	a10, a4, 88
    1380:	1e24b2        	l32i	a11, a4, 120
    1383:	0008e0        	callx8	a8
    1386:	0a5d           	mov.n	a5, a10
    1388:	fb4581        	l32r	a8, 9c (117c <SAT_SUB>)
    138b:	1e24b2        	l32i	a11, a4, 120
    138e:	1624a2        	l32i	a10, a4, 88
    1391:	0008e0        	callx8	a8
    1394:	166452        	s32i	a5, a4, 88
    1397:	3198           	l32i.n	a9, a1, 12
    1399:	1e64a2        	s32i	a10, a4, 120
    139c:	fec332        	addi	a3, a3, -2
    139f:	cf9397        	bne	a3, a9, 1372 <sbc_reconstruction+0x1e2>

000013a2 <sbc_reconstruction+0x212>:
    13a2:	10c772        	addi	a7, a7, 16
    13a5:	c2f7a6        	blti	a7, 0x100, 136b <sbc_reconstruction+0x1db>
    13a8:	f01d           	retw.n

000013aa <sbc_reconstruction+0x21a>:
    13aa:	00000000 41360000                                ......

000013b0 <sbc_header_parse>:
    13b0:	004136        	entry	a1, 32
    13b3:	870c           	movi.n	a7, 8
    13b5:	480c           	movi.n	a8, 4
    13b7:	04e020        	extui	a14, a2, 0, 1
    13ba:	146420        	extui	a6, a2, 4, 2
    13bd:	145620        	extui	a5, a2, 6, 2
    13c0:	025452        	s16i	a5, a4, 4
    13c3:	0b9616        	beqz	a6, 1480 <sbc_header_parse+0xd0>
    13c6:	ffc692        	addi	a9, a6, -1
    13c9:	0bb916        	beqz	a9, 1488 <sbc_header_parse+0xd8>
    13cc:	fec6a2        	addi	a10, a6, -2
    13cf:	103a16        	beqz	a10, 14d6 <sbc_header_parse+0x126>
    13d2:	fdc6b2        	addi	a11, a6, -3
    13d5:	105b16        	beqz	a11, 14de <sbc_header_parse+0x12e>

000013d8 <sbc_header_parse+0x28>:
    13d8:	9ca3a2        	movi	a10, 0x39c
    13db:	1c0c           	movi.n	a12, 1
    13dd:	8378e0        	moveqz	a7, a8, a14
    13e0:	416830        	srli	a6, a3, 8
    13e3:	04d120        	extui	a13, a2, 1, 1
    13e6:	0554d2        	s16i	a13, a4, 10
    13e9:	085462        	s16i	a6, a4, 16
    13ec:	065472        	s16i	a7, a4, 12
    13ef:	143220        	extui	a3, a2, 2, 2
    13f2:	045432        	s16i	a3, a4, 8
    13f5:	220c           	movi.n	a2, 2
    13f7:	832c30        	moveqz	a2, a12, a3
    13fa:	075422        	s16i	a2, a4, 14
    13fd:	08f316        	beqz	a3, 1490 <sbc_header_parse+0xe0>
    1400:	f30b           	addi.n	a15, a3, -1
    1402:	08af16        	beqz	a15, 1490 <sbc_header_parse+0xe0>
    1405:	031482        	l16ui	a8, a4, 6
    1408:	c1a720        	mul16u	a10, a7, a2
    140b:	979d           	ae_zext16	a9, a7
    140d:	11aae0        	slli	a10, a10, 2
    1410:	c17860        	mul16u	a7, a8, a6
    1413:	6a7b           	addi.n	a6, a10, 7
    1415:	b36aa0        	movgez	a6, a10, a10
    1418:	216360        	srai	a6, a6, 3
    141b:	312326        	beqi	a3, 2, 1450 <sbc_header_parse+0xa0>
    141e:	c79a           	add.n	a12, a7, a9
    1420:	0614b2        	l16ui	a11, a4, 12
    1423:	dc7b           	addi.n	a13, a12, 7
    1425:	cceb           	addi.n	a12, a12, 14
    1427:	b3cdd0        	movgez	a12, a13, a13
    142a:	04d4d2        	addmi	a13, a4, 0x400
    142d:	21c3c0        	srai	a12, a12, 3
    1430:	c6ca           	add.n	a12, a6, a12
    1432:	9ccdd2        	addi	a13, a13, -100
    1435:	cc4b           	addi.n	a12, a12, 4
    1437:	7e5dc2        	s16i	a12, a13, 252
    143a:	c1d2b0        	mul16u	a13, a2, a11
    143d:	1a54d2        	s16i	a13, a4, 52
    1440:	020c           	movi.n	a2, 0
    1442:	c1b8b0        	mul16u	a11, a8, a11
    1445:	1854b2        	s16i	a11, a4, 48
    1448:	c1c8d0        	mul16u	a12, a8, a13
    144b:	1b54c2        	s16i	a12, a4, 54
    144e:	f01d           	retw.n

00001450 <sbc_header_parse+0xa0>:
    1450:	f77b           	addi.n	a15, a7, 7
    1452:	e7eb           	addi.n	a14, a7, 14
    1454:	0614d2        	l16ui	a13, a4, 12
    1457:	b3eff0        	movgez	a14, a15, a15
    145a:	04d4f2        	addmi	a15, a4, 0x400
    145d:	21e3e0        	srai	a14, a14, 3
    1460:	e6ea           	add.n	a14, a6, a14
    1462:	9ccff2        	addi	a15, a15, -100
    1465:	ee4b           	addi.n	a14, a14, 4
    1467:	7e5fe2        	s16i	a14, a15, 252
    146a:	c1f2d0        	mul16u	a15, a2, a13
    146d:	1a54f2        	s16i	a15, a4, 52
    1470:	020c           	movi.n	a2, 0
    1472:	c1d8d0        	mul16u	a13, a8, a13
    1475:	1854d2        	s16i	a13, a4, 48
    1478:	c1e8f0        	mul16u	a14, a8, a15
    147b:	1b54e2        	s16i	a14, a4, 54
    147e:	f01d           	retw.n

00001480 <sbc_header_parse+0xd0>:
    1480:	035482        	s16i	a8, a4, 6
    1483:	ffd446        	j	13d8 <sbc_header_parse+0x28>

00001486 <sbc_header_parse+0xd6>:
	...

00001488 <sbc_header_parse+0xd8>:
    1488:	035472        	s16i	a7, a4, 6
    148b:	ffd246        	j	13d8 <sbc_header_parse+0x28>

0000148e <sbc_header_parse+0xde>:
	...

00001490 <sbc_header_parse+0xe0>:
    1490:	031432        	l16ui	a3, a4, 6
    1493:	c1b270        	mul16u	a11, a2, a7
    1496:	071452        	l16ui	a5, a4, 14
    1499:	11bbe0        	slli	a11, a11, 2
    149c:	c18320        	mul16u	a8, a3, a2
    149f:	9b7b           	addi.n	a9, a11, 7
    14a1:	061422        	l16ui	a2, a4, 12
    14a4:	b39bb0        	movgez	a9, a11, a11
    14a7:	219390        	srai	a9, a9, 3
    14aa:	828680        	mull	a8, a6, a8
    14ad:	c15520        	mul16u	a5, a5, a2
    14b0:	b87b           	addi.n	a11, a8, 7
    14b2:	c12320        	mul16u	a2, a3, a2
    14b5:	88eb           	addi.n	a8, a8, 14
    14b7:	b38bb0        	movgez	a8, a11, a11
    14ba:	c13350        	mul16u	a3, a3, a5
    14bd:	218380        	srai	a8, a8, 3
    14c0:	889a           	add.n	a8, a8, a9
    14c2:	94aa           	add.n	a9, a4, a10
    14c4:	884b           	addi.n	a8, a8, 4
    14c6:	7e5982        	s16i	a8, a9, 252
    14c9:	1a5452        	s16i	a5, a4, 52
    14cc:	185422        	s16i	a2, a4, 48
    14cf:	1b5432        	s16i	a3, a4, 54
    14d2:	020c           	movi.n	a2, 0
    14d4:	f01d           	retw.n

000014d6 <sbc_header_parse+0x126>:
    14d6:	cc0c           	movi.n	a12, 12
    14d8:	0354c2        	s16i	a12, a4, 6
    14db:	ffbe46        	j	13d8 <sbc_header_parse+0x28>

000014de <sbc_header_parse+0x12e>:
    14de:	0d1c           	movi.n	a13, 16
    14e0:	0354d2        	s16i	a13, a4, 6
    14e3:	ffbc46        	j	13d8 <sbc_header_parse+0x28>

000014e6 <sbc_header_parse+0x136>:
	...

000014e8 <SBC_CalculateInFrameSize>:
    14e8:	004136        	entry	a1, 32
    14eb:	9ca042        	movi	a4, 156
    14ee:	4b0c           	movi.n	a11, 4
    14f0:	860c           	movi.n	a6, 8
    14f2:	490c           	movi.n	a9, 4
    14f4:	1a0c           	movi.n	a10, 1
    14f6:	270c           	movi.n	a7, 2
    14f8:	0102c2        	l8ui	a12, a2, 1
    14fb:	020252        	l8ui	a5, a2, 2
    14fe:	000232        	l8ui	a3, a2, 0
    1501:	1484c0        	extui	a8, a12, 4, 2
    1504:	1422c0        	extui	a2, a12, 2, 2
    1507:	837a20        	moveqz	a7, a10, a2
    150a:	a08890        	addx4	a8, a8, a9
    150d:	989d           	ae_zext16	a9, a8
    150f:	04c0c0        	extui	a12, a12, 0, 1
    1512:	836bc0        	moveqz	a6, a11, a12
    1515:	031347        	beq	a3, a4, 151c <SBC_CalculateInFrameSize+0x34>
    1518:	020c           	movi.n	a2, 0
    151a:	f01d           	retw.n

0000151c <SBC_CalculateInFrameSize+0x34>:
    151c:	b2ac           	beqz.n	a2, 154b <SBC_CalculateInFrameSize+0x63>
    151e:	291226        	beqi	a2, 1, 154b <SBC_CalculateInFrameSize+0x63>
    1521:	c15580        	mul16u	a5, a5, a8
    1524:	c19670        	mul16u	a9, a6, a7
    1527:	966d           	ae_zext16	a6, a6
    1529:	1199e0        	slli	a9, a9, 2
    152c:	497b           	addi.n	a4, a9, 7
    152e:	b34990        	movgez	a4, a9, a9
    1531:	214340        	srai	a4, a4, 3
    1534:	392226        	beqi	a2, 2, 1571 <SBC_CalculateInFrameSize+0x89>
    1537:	256a           	add.n	a2, a5, a6
    1539:	327b           	addi.n	a3, a2, 7
    153b:	22eb           	addi.n	a2, a2, 14
    153d:	b32330        	movgez	a2, a3, a3
    1540:	212320        	srai	a2, a2, 3
    1543:	242a           	add.n	a2, a4, a2
    1545:	224b           	addi.n	a2, a2, 4
    1547:	922d           	ae_zext16	a2, a2
    1549:	f01d           	retw.n

0000154b <SBC_CalculateInFrameSize+0x63>:
    154b:	c12970        	mul16u	a2, a9, a7
    154e:	c14760        	mul16u	a4, a7, a6
    1551:	822520        	mull	a2, a5, a2
    1554:	1144e0        	slli	a4, a4, 2
    1557:	347b           	addi.n	a3, a4, 7
    1559:	b33440        	movgez	a3, a4, a4
    155c:	213330        	srai	a3, a3, 3
    155f:	427b           	addi.n	a4, a2, 7
    1561:	22eb           	addi.n	a2, a2, 14
    1563:	b32440        	movgez	a2, a4, a4
    1566:	212320        	srai	a2, a2, 3
    1569:	223a           	add.n	a2, a2, a3
    156b:	224b           	addi.n	a2, a2, 4
    156d:	922d           	ae_zext16	a2, a2
    156f:	f01d           	retw.n

00001571 <SBC_CalculateInFrameSize+0x89>:
    1571:	357b           	addi.n	a3, a5, 7
    1573:	25eb           	addi.n	a2, a5, 14
    1575:	b32330        	movgez	a2, a3, a3
    1578:	212320        	srai	a2, a2, 3
    157b:	242a           	add.n	a2, a4, a2
    157d:	224b           	addi.n	a2, a2, 4
    157f:	922d           	ae_zext16	a2, a2
    1581:	f01d           	retw.n

00001583 <SBC_CalculateInFrameSize+0x9b>:
	...

00001584 <SBC_CalculateOutFrameSize>:
    1584:	004136        	entry	a1, 32
    1587:	000232        	l8ui	a3, a2, 0
    158a:	9ca042        	movi	a4, 156
    158d:	020c           	movi.n	a2, 0
    158f:	011347        	beq	a3, a4, 1594 <SBC_CalculateOutFrameSize+0x10>
    1592:	f01d           	retw.n

00001594 <SBC_CalculateOutFrameSize+0x10>:
    1594:	00a122        	movi	a2, 0x100
    1597:	f01d           	retw.n

00001599 <SBC_CalculateOutFrameSize+0x15>:
    1599:	000000                                        ...

0000159c <SBC_CalculateSampleFrequency>:
    159c:	004136        	entry	a1, 32
    159f:	000232        	l8ui	a3, a2, 0
    15a2:	9ca042        	movi	a4, 156
    15a5:	010222        	l8ui	a2, a2, 1
    15a8:	041347        	beq	a3, a4, 15b0 <SBC_CalculateSampleFrequency+0x14>
    15ab:	420c           	movi.n	a2, 4
    15ad:	f01d           	retw.n

000015af <SBC_CalculateSampleFrequency+0x13>:
	...

000015b0 <SBC_CalculateSampleFrequency+0x14>:
    15b0:	412620        	srli	a2, a2, 6
    15b3:	f01d           	retw.n

000015b5 <SBC_CalculateSampleFrequency+0x19>:
    15b5:	000000                                        ...

000015b8 <_fini>:
    15b8:	008136        	entry	a1, 64

000015bb <_fini+0x3>:
    15bb:	f01d           	retw.n
