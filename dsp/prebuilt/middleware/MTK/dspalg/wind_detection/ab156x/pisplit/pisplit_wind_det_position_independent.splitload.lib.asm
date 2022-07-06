
Build/lib/pisplit_wind_det_position_independent.splitload.lib:     file format elf32-xtensa-le


Disassembly of section .text:

00000000 <_init-0x160>:
       0:	000001b4 00001f10 00009414 000093a4     ................
      10:	00000000 00009388 00000000 00009418     ................
      20:	00009430 00002580 00000898 0000259c     0....%.......%..
      30:	000008a0 000025b0 000008e0 000093b0     .....%..........
      40:	02000000 00400000 003fffff 000a3d71     ......@...?.q=..
      50:	000025d0 7fffffff ffc00000 00000200     .%..............
      60:	00200000 0000027c 00000294 000093bc     .. .|...........
      70:	000002a0 000093d0 00000314 000093e8     ................
      80:	00009400 00002488 00000898 00001f30     .....$......0...
      90:	000005e0 00001098 00001f70 000011c8     ........p.......
      a0:	00000788 00001260 00007d50 00008000     ....`...P}......
      b0:	00007c29 ffff83d7 00007f9e 00007dc0     )|...........}..
      c0:	00001510 ffff0000 00002c5d 80000000     ........],......
      d0:	00029973 00007d40 000a65b6 000058b9     s...@}...e...X..
      e0:	00000984 00008370 00007fa0 ffff8000     ....p...........
      f0:	00008424 000083f0 00008940 00007fff     $.......@.......
     100:	00002298 000009f4 00000e78 00008980     ."......x.......
     110:	00001e18 00005a82 0000917c 00001774     .....Z..|...t...
     120:	00008d80 000015e8 00001570 00001794     ........p.......
     130:	00001998 000019c4 000019f4 00001a50     ............P...
     140:	00001a24 00001a98 00001ac4 00001cd8     $...............
     150:	00001da8 00009190 0000938c 00000178     ............x...

00000160 <_init>:
     160:	008136        	entry	a1, 64
     163:	ffa781        	l32r	a8, 0 (1b4 <frame_dummy>)
     166:	f03d           	nop.n
     168:	0008e0        	callx8	a8
     16b:	ffa681        	l32r	a8, 4 (1f10 <__do_global_ctors_aux>)
     16e:	f03d           	nop.n
     170:	0008e0        	callx8	a8
     173:	f01d           	retw.n

00000175 <_init+0x15>:
     175:	000000                                        ...

00000178 <__do_global_dtors_aux>:
     178:	004136        	entry	a1, 32
     17b:	ffa331        	l32r	a3, 8 (9414 <__bss_start>)
     17e:	000382        	l8ui	a8, a3, 0
     181:	d8ec           	bnez.n	a8, 1b2 <__do_global_dtors_aux+0x3a>
     183:	ffa221        	l32r	a2, c (93a4 <p$3519_6_1>)
     186:	02b8           	l32i.n	a11, a2, 0
     188:	0ba8           	l32i.n	a10, a11, 0
     18a:	ca8c           	beqz.n	a10, 19a <__do_global_dtors_aux+0x22>

0000018c <__do_global_dtors_aux+0x14>:
     18c:	9b4b           	addi.n	a9, a11, 4
     18e:	0299           	s32i.n	a9, a2, 0
     190:	000ae0        	callx8	a10
     193:	02b8           	l32i.n	a11, a2, 0
     195:	0ba8           	l32i.n	a10, a11, 0
     197:	ff1a56        	bnez	a10, 18c <__do_global_dtors_aux+0x14>

0000019a <__do_global_dtors_aux+0x22>:
     19a:	ff9da1        	l32r	a10, 10 (0 <_text_start>)
     19d:	0b0c           	movi.n	a11, 0
     19f:	081ab7        	beq	a10, a11, 1ab <__do_global_dtors_aux+0x33>
     1a2:	ff9ca1        	l32r	a10, 14 (9388 <__EH_FRAME_BEGIN___44>)
     1a5:	ff9a81        	l32r	a8, 10 (0 <_text_start>)
     1a8:	0008e0        	callx8	a8

000001ab <__do_global_dtors_aux+0x33>:
     1ab:	1c0c           	movi.n	a12, 1
     1ad:	0043c2        	s8i	a12, a3, 0
     1b0:	f01d           	retw.n

000001b2 <__do_global_dtors_aux+0x3a>:
     1b2:	f01d           	retw.n

000001b4 <frame_dummy>:
     1b4:	004136        	entry	a1, 32
     1b7:	ff9881        	l32r	a8, 18 (0 <_text_start>)
     1ba:	090c           	movi.n	a9, 0
     1bc:	0b1897        	beq	a8, a9, 1cb <frame_dummy+0x17>
     1bf:	ff95a1        	l32r	a10, 14 (9388 <__EH_FRAME_BEGIN___44>)
     1c2:	ff96b1        	l32r	a11, 1c (9418 <object$3538_8_1>)
     1c5:	ff9481        	l32r	a8, 18 (0 <_text_start>)
     1c8:	0008e0        	callx8	a8

000001cb <frame_dummy+0x17>:
     1cb:	f01d           	retw.n

000001cd <frame_dummy+0x19>:
     1cd:	000000                                        ...

000001d0 <_start>:
     1d0:	004136        	entry	a1, 32
     1d3:	ff94a1        	l32r	a10, 24 (2580 <_DYNAMIC+0x568>)
     1d6:	ff9231        	l32r	a3, 20 (9430 <printf_ptr>)
     1d9:	0288           	l32i.n	a8, a2, 0
     1db:	ff93b1        	l32r	a11, 28 (898 <WindDet_get_memsize>)
     1de:	0389           	s32i.n	a8, a3, 0
     1e0:	0008e0        	callx8	a8
     1e3:	0388           	l32i.n	a8, a3, 0
     1e5:	ff91a1        	l32r	a10, 2c (259c <_DYNAMIC+0x584>)
     1e8:	ff92b1        	l32r	a11, 30 (8a0 <WindDet_Init>)
     1eb:	0008e0        	callx8	a8
     1ee:	0388           	l32i.n	a8, a3, 0
     1f0:	ff91a1        	l32r	a10, 34 (25b0 <_DYNAMIC+0x598>)
     1f3:	ff91b1        	l32r	a11, 38 (8e0 <WindDet_Prcs>)
     1f6:	0008e0        	callx8	a8
     1f9:	ff9021        	l32r	a2, 3c (93b0 <export_parameter_array>)
     1fc:	f01d           	retw.n

000001fe <_start+0x2e>:
	...

00000200 <fix_Tanh>:
     200:	006136        	entry	a1, 48
     203:	ff8f51        	l32r	a5, 40 (2000000 <_end+0x1ff6bcc>)
     206:	061296        	bltz	a2, 26b <fix_Tanh+0x6b>
     209:	04a527        	bge	a5, a2, 211 <fix_Tanh+0x11>
     20c:	ff8e21        	l32r	a2, 44 (400000 <_end+0x3f6bcc>)
     20f:	f01d           	retw.n

00000211 <fix_Tanh+0x11>:
     211:	040c           	movi.n	a4, 0

00000213 <fix_Tanh+0x13>:
     213:	ff8d51        	l32r	a5, 48 (3fffff <_end+0x3f6bcb>)
     216:	903220        	addx2	a3, a2, a2
     219:	ff8c81        	l32r	a8, 4c (a3d71 <_end+0x9a93d>)
     21c:	39c884        	ae_movpa24x2	aep4, a8, a8
     21f:	b03320        	addx8	a3, a3, a2
     222:	ff8c81        	l32r	a8, 54 (7fffffff <_end+0x7fff6bcb>)
     225:	105350        	and	a5, a3, a5
     228:	0159           	s32i.n	a5, a1, 0
     22a:	ff8951        	l32r	a5, 50 (25d0 <Tansig_Tab_37>)
     22d:	4c3104        	ae_lp24.i	aep3, a1, 0
     230:	013c74        	ae_mulfp24s.ll	aeq1, aep3, aep4
     233:	313630        	srai	a3, a3, 22
     236:	f8effec87f 	{ ae_cvtq48a32s	aeq1, a8; ae_roundsp24q48sym	aep1, aeq1 }
     23b:	903350        	addx2	a3, a3, a5
     23e:	fc0c6c230f 	{ ae_lp16f.i	aep2, a3, 0; ae_movq56	aeq0, aeq1 }
     243:	0122b4        	ae_mulsfp24s.ll	aeq0, aep2, aep2
     246:	011ab4        	ae_mulsfp24s.ll	aeq1, aep1, aep2
     249:	081524        	ae_mulfq32sp24s.l	aeq0, aeq0, aep1
     24c:	1b9c04        	ae_roundsp24q48sym	aep0, aeq1
     24f:	f88000415e 	{ addi	a5, a1, 4; ae_mulfq32sp24s.l	aeq0, aeq0, aep0 }
     254:	233d           	ae_l16si.n	a3, a3, 0
     256:	620504        	ae_sq32f.i	aeq0, a5, 0
     259:	1128           	l32i.n	a2, a1, 4
     25b:	113390        	slli	a3, a3, 7
     25e:	212920        	srai	a2, a2, 9
     261:	223a           	add.n	a2, a2, a3
     263:	603020        	neg	a3, a2
     266:	932340        	movnez	a2, a3, a4
     269:	f01d           	retw.n

0000026b <fix_Tanh+0x6b>:
     26b:	602020        	neg	a2, a2
     26e:	04a527        	bge	a5, a2, 276 <fix_Tanh+0x76>
     271:	ff7921        	l32r	a2, 58 (ffc00000 <_end+0xffbf6bcc>)
     274:	f01d           	retw.n

00000276 <fix_Tanh+0x76>:
     276:	140c           	movi.n	a4, 1
     278:	ffe5c6        	j	213 <fix_Tanh+0x13>

0000027b <fix_Tanh+0x7b>:
	...

0000027c <fix_Sigmoid>:
     27c:	004136        	entry	a1, 32
     27f:	ff7781        	l32r	a8, 5c (200 <fix_Tanh>)
     282:	21a120        	srai	a10, a2, 1
     285:	0008e0        	callx8	a8
     288:	ff7681        	l32r	a8, 60 (200000 <_end+0x1f6bcc>)
     28b:	2121a0        	srai	a2, a10, 1
     28e:	228a           	add.n	a2, a2, a8
     290:	f01d           	retw.n

00000292 <fix_Sigmoid+0x16>:
	...

00000294 <fix_Relu>:
     294:	004136        	entry	a1, 32
     297:	030c           	movi.n	a3, 0
     299:	532320        	max	a2, a3, a2
     29c:	f01d           	retw.n

0000029e <fix_Relu+0xa>:
	...

000002a0 <fix_compute_dense>:
     2a0:	006136        	entry	a1, 48
     2a3:	2268           	l32i.n	a6, a2, 8
     2a5:	3278           	l32i.n	a7, a2, 12
     2a7:	866d           	ae_sext16	a6, a6
     2a9:	877d           	ae_sext16	a7, a7
     2ab:	6117a6        	blti	a7, 1, 310 <fix_compute_dense+0x70>
     2ae:	050c           	movi.n	a5, 0
     2b0:	218160        	srai	a8, a6, 1
     2b3:	1189           	s32i.n	a8, a1, 4
     2b5:	000546        	j	2ce <fix_compute_dense+0x2e>

000002b8 <fix_compute_dense+0x18>:
     2b8:	00000000                                ....

000002bc <fix_compute_dense+0x1c>:
     2bc:	ff6881        	l32r	a8, 5c (200 <fix_Tanh>)
     2bf:	0008e0        	callx8	a8

000002c2 <fix_compute_dense+0x22>:
     2c2:	a08530        	addx4	a8, a5, a3
     2c5:	08a9           	s32i.n	a10, a8, 0
     2c7:	551b           	addi.n	a5, a5, 1
     2c9:	855d           	ae_sext16	a5, a5
     2cb:	411757        	beq	a7, a5, 310 <fix_compute_dense+0x70>

000002ce <fix_compute_dense+0x2e>:
     2ce:	0298           	l32i.n	a9, a2, 0
     2d0:	1288           	l32i.n	a8, a2, 4
     2d2:	d1a650        	mul16s	a10, a6, a5
     2d5:	42b8           	l32i.n	a11, a2, 16
     2d7:	88aa           	add.n	a8, a8, a10
     2d9:	959a           	add.n	a9, a5, a9
     2db:	000992        	l8ui	a9, a9, 0
     2de:	fec882        	addi	a8, a8, -2
     2e1:	11a8           	l32i.n	a10, a1, 4
     2e3:	239900        	sext	a9, a9, 7
     2e6:	119910        	slli	a9, a9, 15
     2e9:	360934        	ae_cvtq48a32s	aeq0, a9
     2ec:	049d           	mov.n	a9, a4
     2ee:	5c9904        	ae_lp24x2f.iu	aep1, a9, 0
     2f1:	07aa76        	loopgtz	a10, 2fc <fix_compute_dense+0x5c>
     2f4:	1d0814        	ae_lp8x2f.iu	aep0, a8, 2
     2f7:	e2042f991f 	{ ae_lp24x2f.iu	aep1, a9, 8; ae_mulaafp24s.hh.ll	aeq0, aep1, aep0 }

000002fc <fix_compute_dense+0x5c>:
     2fc:	060844        	ae_satq48s	aeq0, aeq0
     2ff:	620104        	ae_sq32f.i	aeq0, a1, 0
     302:	01a8           	l32i.n	a10, a1, 0
     304:	b41b66        	bnei	a11, 1, 2bc <fix_compute_dense+0x1c>
     307:	ff5781        	l32r	a8, 64 (27c <fix_Sigmoid>)
     30a:	0008e0        	callx8	a8
     30d:	ffec46        	j	2c2 <fix_compute_dense+0x22>

00000310 <fix_compute_dense+0x70>:
     310:	f01d           	retw.n

00000312 <fix_compute_dense+0x72>:
	...

00000314 <fix_compute_gru>:
     314:	06e136        	entry	a1, 0x370
     317:	4288           	l32i.n	a8, a2, 16
     319:	069272        	l16si	a7, a2, 12
     31c:	886d           	ae_sext16	a6, a8
     31e:	0216e6        	bgei	a6, 1, 324 <fix_compute_gru+0x10>
     321:	00a346        	j	5b2 <fix_compute_gru+0x29e>
     324:	d1a880        	mul16s	a10, a8, a8
     327:	d1c780        	mul16s	a12, a7, a8
     32a:	2b0c           	movi.n	a11, 2
     32c:	215260        	srai	a5, a6, 2
     32f:	21f270        	srai	a15, a7, 2
     332:	ce61f2        	s32i	a15, a1, 0x338
     335:	c86152        	s32i	a5, a1, 0x320
     338:	1096b0        	and	a9, a6, a11
     33b:	050c           	movi.n	a5, 0
     33d:	10b7b0        	and	a11, a7, a11
     340:	8ccd           	ae_sext16	a12, a12
     342:	899d           	ae_sext16	a9, a9
     344:	8aad           	ae_sext16	a10, a10
     346:	ca61a2        	s32i	a10, a1, 0x328
     349:	c46192        	s32i	a9, a1, 0x310
     34c:	c961c2        	s32i	a12, a1, 0x324
     34f:	8bbd           	ae_sext16	a11, a11
     351:	cb61b2        	s32i	a11, a1, 0x32c
     354:	c65a           	add.n	a12, a6, a5

00000356 <fix_compute_gru+0x42>:
     356:	12a8           	l32i.n	a10, a2, 4
     358:	c921b2        	l32i	a11, a1, 0x324
     35b:	0288           	l32i.n	a8, a2, 0
     35d:	d19750        	mul16s	a9, a7, a5
     360:	bbaa           	add.n	a11, a11, a10
     362:	a9aa           	add.n	a10, a9, a10
     364:	99ba           	add.n	a9, a9, a11
     366:	fecaa2        	addi	a10, a10, -2
     369:	b58a           	add.n	a11, a5, a8
     36b:	000bb2        	l8ui	a11, a11, 0
     36e:	88ca           	add.n	a8, a8, a12
     370:	000882        	l8ui	a8, a8, 0
     373:	fec992        	addi	a9, a9, -2
     376:	23bb00        	sext	a11, a11, 7
     379:	238800        	sext	a8, a8, 7
     37c:	11bb10        	slli	a11, a11, 15
     37f:	364b34        	ae_cvtq48a32s	aeq1, a11
     382:	118810        	slli	a8, a8, 15
     385:	360834        	ae_cvtq48a32s	aeq0, a8
     388:	ce21b2        	l32i	a11, a1, 0x338
     38b:	208440        	or	a8, a4, a4
     38e:	5ca804        	ae_lp24x2f.iu	aep2, a8, 0
     391:	19ab76        	loopgtz	a11, 3ae <fix_compute_gru+0x9a>
     394:	5c0814        	ae_lp24x2f.i	aep0, a8, 8
     397:	1d1914        	ae_lp8x2f.iu	aep1, a9, 2
     39a:	e2283f1a8f 	{ ae_lp8x2f.iu	aep1, a10, 2; ae_mulaafp24s.hh.ll	aeq0, aep2, aep1 }
     39f:	e2287f1a8f 	{ ae_lp8x2f.iu	aep1, a10, 2; ae_mulaafp24s.hh.ll	aeq1, aep2, aep1 }
     3a4:	e2207f198f 	{ ae_lp8x2f.iu	aep1, a9, 2; ae_mulaafp24s.hh.ll	aeq1, aep0, aep1 }
     3a9:	e2202fa82f 	{ ae_lp24x2f.iu	aep2, a8, 16; ae_mulaafp24s.hh.ll	aeq0, aep0, aep1 }

000003ae <fix_compute_gru+0x9a>:
     3ae:	ca21b2        	l32i	a11, a1, 0x328
     3b1:	038d           	mov.n	a8, a3
     3b3:	03d1d2        	addmi	a13, a1, 0x300
     3b6:	cb21c2        	l32i	a12, a1, 0x32c
     3b9:	78cdd2        	addi	a13, a13, 120
     3bc:	630d94        	ae_sq56s.i	aeq0, a13, -56
     3bf:	03d1d2        	addmi	a13, a1, 0x300
     3c2:	01bc16        	beqz	a12, 3e1 <fix_compute_gru+0xcd>
     3c5:	03d1e2        	addmi	a14, a1, 0x300
     3c8:	1d0914        	ae_lp8x2f.iu	aep0, a9, 2
     3cb:	03d1f2        	addmi	a15, a1, 0x300
     3ce:	78cff2        	addi	a15, a15, 120
     3d1:	610f94        	ae_lq56.i	aeq0, a15, -56
     3d4:	e2083f1a8f 	{ ae_lp8x2f.iu	aep1, a10, 2; ae_mulaafp24s.hh.ll	aeq0, aep2, aep0 }
     3d9:	78cee2        	addi	a14, a14, 120
     3dc:	e2287a0e9f 	{ ae_sq56s.i	aeq0, a14, -56; ae_mulaafp24s.hh.ll	aeq1, aep2, aep1 }

000003e1 <fix_compute_gru+0xcd>:
     3e1:	5ca804        	ae_lp24x2f.iu	aep2, a8, 0
     3e4:	d19650        	mul16s	a9, a6, a5
     3e7:	22a8           	l32i.n	a10, a2, 8
     3e9:	03d1c2        	addmi	a12, a1, 0x300
     3ec:	78ccc2        	addi	a12, a12, 120
     3ef:	610c94        	ae_lq56.i	aeq0, a12, -56
     3f2:	bbaa           	add.n	a11, a11, a10
     3f4:	a9aa           	add.n	a10, a9, a10
     3f6:	99ba           	add.n	a9, a9, a11
     3f8:	c821b2        	l32i	a11, a1, 0x320
     3fb:	fecaa2        	addi	a10, a10, -2
     3fe:	fec992        	addi	a9, a9, -2
     401:	19ab76        	loopgtz	a11, 41e <fix_compute_gru+0x10a>
     404:	5c0814        	ae_lp24x2f.i	aep0, a8, 8
     407:	1d1914        	ae_lp8x2f.iu	aep1, a9, 2
     40a:	e2283f1a8f 	{ ae_lp8x2f.iu	aep1, a10, 2; ae_mulaafp24s.hh.ll	aeq0, aep2, aep1 }
     40f:	e2287f1a8f 	{ ae_lp8x2f.iu	aep1, a10, 2; ae_mulaafp24s.hh.ll	aeq1, aep2, aep1 }
     414:	e2207f198f 	{ ae_lp8x2f.iu	aep1, a9, 2; ae_mulaafp24s.hh.ll	aeq1, aep0, aep1 }
     419:	e2202fa82f 	{ ae_lp24x2f.iu	aep2, a8, 16; ae_mulaafp24s.hh.ll	aeq0, aep0, aep1 }

0000041e <fix_compute_gru+0x10a>:
     41e:	c421c2        	l32i	a12, a1, 0x310
     421:	78cdd2        	addi	a13, a13, 120
     424:	630d94        	ae_sq56s.i	aeq0, a13, -56
     427:	ac9c           	beqz.n	a12, 445 <fix_compute_gru+0x131>
     429:	03d1e2        	addmi	a14, a1, 0x300
     42c:	1d3914        	ae_lp8x2f.iu	aep3, a9, 2
     42f:	03d1f2        	addmi	a15, a1, 0x300
     432:	78cff2        	addi	a15, a15, 120
     435:	618f94        	ae_lq56.i	aeq2, a15, -56
     438:	e3683f4a8f 	{ ae_lp8x2f.iu	aep4, a10, 2; ae_mulaafp24s.hh.ll	aeq2, aep2, aep3 }
     43d:	78cee2        	addi	a14, a14, 120
     440:	e2887a2e9f 	{ ae_sq56s.i	aeq2, a14, -56; ae_mulaafp24s.hh.ll	aeq1, aep2, aep4 }

00000445 <fix_compute_gru+0x131>:
     445:	ff0781        	l32r	a8, 64 (27c <fix_Sigmoid>)
     448:	02d192        	addmi	a9, a1, 0x200
     44b:	ff04c8099e 	{ addi	a9, a9, -128; ae_satq48s	aeq3, aeq1 }
     450:	623904        	ae_sq32f.i	aeq3, a9, 0
     453:	6021a2        	l32i	a10, a1, 0x180
     456:	0008e0        	callx8	a8
     459:	ff0281        	l32r	a8, 64 (27c <fix_Sigmoid>)
     45c:	90d510        	addx2	a13, a5, a1
     45f:	03d1b2        	addmi	a11, a1, 0x300
     462:	21c7a0        	srai	a12, a10, 7
     465:	1195f0        	slli	a9, a5, 1
     468:	cd6192        	s32i	a9, a1, 0x334
     46b:	02d1a2        	addmi	a10, a1, 0x200
     46e:	78cbb2        	addi	a11, a11, 120
     471:	6dcd           	ae_s16i.n	a12, a13, 0
     473:	614b94        	ae_lq56.i	aeq1, a11, -56
     476:	fc04c80aae 	{ addi	a10, a10, -128; ae_satq48s	aeq0, aeq1 }
     47b:	620a04        	ae_sq32f.i	aeq0, a10, 0
     47e:	6021a2        	l32i	a10, a1, 0x180
     481:	0008e0        	callx8	a8
     484:	2187a0        	srai	a8, a10, 7
     487:	cd2192        	l32i	a9, a1, 0x334
     48a:	551b           	addi.n	a5, a5, 1
     48c:	7fc1b2        	addi	a11, a1, 127
     48f:	41cbb2        	addi	a11, a11, 65
     492:	855d           	ae_sext16	a5, a5
     494:	c65a           	add.n	a12, a6, a5
     496:	c0e650        	sub	a14, a6, a5
     499:	99ba           	add.n	a9, a9, a11
     49b:	698d           	ae_s16i.n	a8, a9, 0
     49d:	eb5e56        	bnez	a14, 356 <fix_compute_gru+0x42>
     4a0:	048d           	mov.n	a8, a4
     4a2:	c92152        	l32i	a5, a1, 0x324
     4a5:	ca21f2        	l32i	a15, a1, 0x328
     4a8:	11c6f0        	slli	a12, a6, 1
     4ab:	21d160        	srai	a13, a6, 1
     4ae:	cc61d2        	s32i	a13, a1, 0x330
     4b1:	c561c2        	s32i	a12, a1, 0x314
     4b4:	11fff0        	slli	a15, a15, 1
     4b7:	1155f0        	slli	a5, a5, 1
     4ba:	c66152        	s32i	a5, a1, 0x318
     4bd:	c761f2        	s32i	a15, a1, 0x31c
     4c0:	050c           	movi.n	a5, 0
     4c2:	d1c750        	mul16s	a12, a7, a5
     4c5:	000906        	j	4ed <fix_compute_gru+0x1d9>

000004c8 <fix_compute_gru+0x1b4>:
	...

000004ca <fix_compute_gru+0x1b6>:
     4ca:	fee781        	l32r	a8, 68 (294 <fix_Relu>)
     4cd:	0008e0        	callx8	a8
     4d0:	6061a2        	s32i	a10, a1, 0x180

000004d3 <fix_compute_gru+0x1bf>:
     4d3:	02d182        	addmi	a8, a1, 0x200
     4d6:	90c882        	addi	a8, a8, -112
     4d9:	a08580        	addx4	a8, a5, a8
     4dc:	08a9           	s32i.n	a10, a8, 0
     4de:	551b           	addi.n	a5, a5, 1
     4e0:	048d           	mov.n	a8, a4
     4e2:	855d           	ae_sext16	a5, a5
     4e4:	d1c750        	mul16s	a12, a7, a5
     4e7:	c09650        	sub	a9, a6, a5
     4ea:	0c4916        	beqz	a9, 5b2 <fix_compute_gru+0x29e>

000004ed <fix_compute_gru+0x1d9>:
     4ed:	c521b2        	l32i	a11, a1, 0x314
     4f0:	02a8           	l32i.n	a10, a2, 0
     4f2:	5ca804        	ae_lp24x2f.iu	aep2, a8, 0
     4f5:	bb5a           	add.n	a11, a11, a5
     4f7:	aaba           	add.n	a10, a10, a11
     4f9:	c621b2        	l32i	a11, a1, 0x318
     4fc:	000aa2        	l8ui	a10, a10, 0
     4ff:	1298           	l32i.n	a9, a2, 4
     501:	bbca           	add.n	a11, a11, a12
     503:	23aa00        	sext	a10, a10, 7
     506:	11aa10        	slli	a10, a10, 15
     509:	364a34        	ae_cvtq48a32s	aeq1, a10
     50c:	ce21a2        	l32i	a10, a1, 0x338
     50f:	8099b0        	add	a9, a9, a11
     512:	fec992        	addi	a9, a9, -2
     515:	0faa76        	loopgtz	a10, 528 <fix_compute_gru+0x214>
     518:	5c0814        	ae_lp24x2f.i	aep0, a8, 8
     51b:	1d1914        	ae_lp8x2f.iu	aep1, a9, 2
     51e:	e2287f198f 	{ ae_lp8x2f.iu	aep1, a9, 2; ae_mulaafp24s.hh.ll	aeq1, aep2, aep1 }
     523:	e2206fa82f 	{ ae_lp24x2f.iu	aep2, a8, 16; ae_mulaafp24s.hh.ll	aeq1, aep0, aep1 }

00000528 <fix_compute_gru+0x214>:
     528:	c721b2        	l32i	a11, a1, 0x31c
     52b:	cc21d2        	l32i	a13, a1, 0x330
     52e:	038d           	mov.n	a8, a3
     530:	7fc1a2        	addi	a10, a1, 127
     533:	cb21c2        	l32i	a12, a1, 0x32c
     536:	41caa2        	addi	a10, a10, 65
     539:	448a04        	ae_lp16x2f.iu	aep0, a10, 0
     53c:	005c16        	beqz	a12, 545 <fix_compute_gru+0x231>
     53f:	1d5914        	ae_lp8x2f.iu	aep5, a9, 2
     542:	052d04        	ae_mulaafp24s.hh.ll	aeq1, aep2, aep5

00000545 <fix_compute_gru+0x231>:
     545:	2298           	l32i.n	a9, a2, 8
     547:	d1c650        	mul16s	a12, a6, a5
     54a:	5ca804        	ae_lp24x2f.iu	aep2, a8, 0
     54d:	bbca           	add.n	a11, a11, a12
     54f:	99ba           	add.n	a9, a9, a11
     551:	e9088fe99e 	{ addi	a9, a9, -2; ae_mulfp24s.hh	aeq2, aep2, aep0 }
     556:	3e1da6        	blti	a13, 1, 598 <fix_compute_gru+0x284>
     559:	449a14        	ae_lp16x2f.iu	aep1, a10, 4
     55c:	bd0b           	addi.n	a11, a13, -1
     55e:	1d4914        	ae_lp8x2f.iu	aep4, a9, 2
     561:	eb09ef881f 	{ ae_lp24x2f.iu	aep0, a8, 8; ae_mulfp24s.ll	aeq3, aep2, aep0 }
     566:	f977d3c99f 	{ or	a12, a9, a9; ae_roundsp24q48asym	aep3, aeq2 }
     56b:	fe0c53988f 	{ or	a9, a8, a8; ae_movq56	aeq2, aeq1 }
     570:	f97f938ccf 	{ or	a8, a12, a12; ae_roundsp24q48asym	aep2, aeq3 }
     575:	16ab76        	loopgtz	a11, 58f <fix_compute_gru+0x27b>
     578:	ea21fe329f 	{ ae_selp24.hl	aep2, aep3, aep2; ae_mulfp24s.ll	aeq1, aep0, aep1 }
     57d:	e3883f488f 	{ ae_lp8x2f.iu	aep4, a8, 2; ae_mulaafp24s.hh.ll	aeq2, aep2, aep4 }
     582:	e820af891f 	{ ae_lp24x2f.iu	aep0, a9, 8; ae_mulfp24s.hh	aeq0, aep0, aep1 }
     587:	f96fad9a1f 	{ ae_lp16x2f.iu	aep1, a10, 4; ae_roundsp24q48asym	aep2, aeq1 }
     58c:	1b8d34        	ae_roundsp24q48asym	aep3, aeq0

0000058f <fix_compute_gru+0x27b>:
     58f:	1aba64        	ae_selp24.hl	aep6, aep3, aep2
     592:	05e404        	ae_mulaafp24s.hh.ll	aeq2, aep6, aep4
     595:	066b34        	ae_movq56	aeq1, aeq2

00000598 <fix_compute_gru+0x284>:
     598:	fc04f052ce 	{ l32i	a12, a2, 20; ae_satq48s	aeq0, aeq1 }
     59d:	368a04        	ae_trunca32q48	a10, aeq0
     5a0:	6061a2        	s32i	a10, a1, 0x180
     5a3:	f23c56        	bnez	a12, 4ca <fix_compute_gru+0x1b6>
     5a6:	fead81        	l32r	a8, 5c (200 <fix_Tanh>)
     5a9:	0008e0        	callx8	a8
     5ac:	6061a2        	s32i	a10, a1, 0x180
     5af:	ffc806        	j	4d3 <fix_compute_gru+0x1bf>

000005b2 <fix_compute_gru+0x29e>:
     5b2:	02d1a2        	addmi	a10, a1, 0x200
     5b5:	019d           	mov.n	a9, a1
     5b7:	601304        	ae_lq32f.iu	aeq0, a3, 0
     5ba:	038d           	mov.n	a8, a3
     5bc:	408904        	ae_lp16f.iu	aep0, a9, 0
     5bf:	90caa2        	addi	a10, a10, -112
     5c2:	605a04        	ae_lq32f.iu	aeq1, a10, 0
     5c5:	15a676        	loopgtz	a6, 5de <fix_compute_gru+0x2ca>
     5c8:	010dc4        	ae_subq56	aeq0, aeq0, aeq1
     5cb:	d4e22c891f 	{ ae_lp16f.iu	aep0, a9, 2; ae_mulfq32sp16s.l	aeq0, aeq0, aep0 }
     5d0:	060934        	ae_roundsq32asym	aeq0, aeq0
     5d3:	f86a3d3a1f 	{ ae_lq32f.iu	aeq1, a10, 4; ae_addq56	aeq0, aeq1, aeq0 }
     5d8:	624804        	ae_sq32f.iu	aeq0, a8, 0
     5db:	601814        	ae_lq32f.iu	aeq0, a8, 4

000005de <fix_compute_gru+0x2ca>:
     5de:	f01d           	retw.n

000005e0 <RNN_Init_fx>:
     5e0:	004136        	entry	a1, 32
     5e3:	070c           	movi.n	a7, 0
     5e5:	028d           	mov.n	a8, a2
     5e7:	40c292        	addi	a9, a2, 64
     5ea:	093d           	mov.n	a3, a9
     5ec:	320c           	movi.n	a2, 3
     5ee:	6b8276        	loop	a2, 65d <RNN_Init_fx+0x7d>
     5f1:	412870        	srli	a2, a7, 8
     5f4:	904372        	s8i	a7, a3, 144
     5f7:	944372        	s8i	a7, a3, 148
     5fa:	984372        	s8i	a7, a3, 152
     5fd:	9c4372        	s8i	a7, a3, 156
     600:	a04372        	s8i	a7, a3, 160
     603:	a44372        	s8i	a7, a3, 164
     606:	a84372        	s8i	a7, a3, 168
     609:	ac4372        	s8i	a7, a3, 172
     60c:	20c332        	addi	a3, a3, 32
     60f:	714322        	s8i	a2, a3, 113
     612:	414820        	srli	a4, a2, 8
     615:	754322        	s8i	a2, a3, 117
     618:	794322        	s8i	a2, a3, 121
     61b:	7d4322        	s8i	a2, a3, 125
     61e:	814322        	s8i	a2, a3, 129
     621:	854322        	s8i	a2, a3, 133
     624:	894322        	s8i	a2, a3, 137
     627:	8d4322        	s8i	a2, a3, 141
     62a:	724342        	s8i	a4, a3, 114
     62d:	412840        	srli	a2, a4, 8
     630:	764342        	s8i	a4, a3, 118
     633:	7a4342        	s8i	a4, a3, 122
     636:	7e4342        	s8i	a4, a3, 126
     639:	824342        	s8i	a4, a3, 130
     63c:	864342        	s8i	a4, a3, 134
     63f:	8a4342        	s8i	a4, a3, 138
     642:	8e4342        	s8i	a4, a3, 142
     645:	734322        	s8i	a2, a3, 115
     648:	774322        	s8i	a2, a3, 119
     64b:	7b4322        	s8i	a2, a3, 123
     64e:	7f4322        	s8i	a2, a3, 127
     651:	834322        	s8i	a2, a3, 131
     654:	874322        	s8i	a2, a3, 135
     657:	8b4322        	s8i	a2, a3, 139
     65a:	8f4322        	s8i	a2, a3, 143

0000065d <RNN_Init_fx+0x7d>:
     65d:	093d           	mov.n	a3, a9
     65f:	06a062        	movi	a6, 6
     662:	0ca022        	movi	a2, 12
     665:	3b8276        	loop	a2, 6a4 <RNN_Init_fx+0xc4>
     668:	412870        	srli	a2, a7, 8
     66b:	f04372        	s8i	a7, a3, 240
     66e:	f44372        	s8i	a7, a3, 244
     671:	f84372        	s8i	a7, a3, 248
     674:	fc4372        	s8i	a7, a3, 252
     677:	10c332        	addi	a3, a3, 16
     67a:	e14322        	s8i	a2, a3, 225
     67d:	414820        	srli	a4, a2, 8
     680:	e54322        	s8i	a2, a3, 229
     683:	e94322        	s8i	a2, a3, 233
     686:	ed4322        	s8i	a2, a3, 237
     689:	e24342        	s8i	a4, a3, 226
     68c:	412840        	srli	a2, a4, 8
     68f:	e64342        	s8i	a4, a3, 230
     692:	ea4342        	s8i	a4, a3, 234
     695:	ee4342        	s8i	a4, a3, 238
     698:	e34322        	s8i	a2, a3, 227
     69b:	e74322        	s8i	a2, a3, 231
     69e:	eb4322        	s8i	a2, a3, 235
     6a1:	ef4322        	s8i	a2, a3, 239
     6a4:	202880        	or	a2, a8, a8
     6a7:	414870        	srli	a4, a7, 8
     6aa:	03a052        	movi	a5, 3
     6ad:	07a676        	loopgtz	a6, 6b8 <RNN_Init_fx+0xd8>
     6b0:	004272        	s8i	a7, a2, 0
     6b3:	014242        	s8i	a4, a2, 1
     6b6:	222b           	addi.n	a2, a2, 2
     6b8:	093d           	mov.n	a3, a9
     6ba:	06a0a2        	movi	a10, 6
     6bd:	328576        	loop	a5, 6f3 <RNN_Init_fx+0x113>
     6c0:	004272        	s8i	a7, a2, 0
     6c3:	014242        	s8i	a4, a2, 1
     6c6:	024272        	s8i	a7, a2, 2
     6c9:	034242        	s8i	a4, a2, 3
     6cc:	044272        	s8i	a7, a2, 4
     6cf:	054242        	s8i	a4, a2, 5
     6d2:	064272        	s8i	a7, a2, 6
     6d5:	074242        	s8i	a4, a2, 7
     6d8:	084272        	s8i	a7, a2, 8
     6db:	094242        	s8i	a4, a2, 9
     6de:	0a4272        	s8i	a7, a2, 10
     6e1:	0b4242        	s8i	a4, a2, 11
     6e4:	0c4272        	s8i	a7, a2, 12
     6e7:	0d4242        	s8i	a4, a2, 13
     6ea:	0e4272        	s8i	a7, a2, 14
     6ed:	0f4242        	s8i	a4, a2, 15
     6f0:	10c222        	addi	a2, a2, 16
     6f3:	320c           	movi.n	a2, 3
     6f5:	16aa76        	loopgtz	a10, 70f <RNN_Init_fx+0x12f>
     6f8:	41b870        	srli	a11, a7, 8
     6fb:	004372        	s8i	a7, a3, 0
     6fe:	0143b2        	s8i	a11, a3, 1
     701:	41b8b0        	srli	a11, a11, 8
     704:	0243b2        	s8i	a11, a3, 2
     707:	41b8b0        	srli	a11, a11, 8
     70a:	0343b2        	s8i	a11, a3, 3
     70d:	334b           	addi.n	a3, a3, 4
     70f:	6b8276        	loop	a2, 77e <RNN_Init_fx+0x19e>
     712:	412870        	srli	a2, a7, 8
     715:	004372        	s8i	a7, a3, 0
     718:	044372        	s8i	a7, a3, 4
     71b:	084372        	s8i	a7, a3, 8
     71e:	0c4372        	s8i	a7, a3, 12
     721:	104372        	s8i	a7, a3, 16
     724:	144372        	s8i	a7, a3, 20
     727:	184372        	s8i	a7, a3, 24
     72a:	1c4372        	s8i	a7, a3, 28
     72d:	014322        	s8i	a2, a3, 1
     730:	414820        	srli	a4, a2, 8
     733:	054322        	s8i	a2, a3, 5
     736:	094322        	s8i	a2, a3, 9
     739:	0d4322        	s8i	a2, a3, 13
     73c:	114322        	s8i	a2, a3, 17
     73f:	154322        	s8i	a2, a3, 21
     742:	194322        	s8i	a2, a3, 25
     745:	1d4322        	s8i	a2, a3, 29
     748:	024342        	s8i	a4, a3, 2
     74b:	412840        	srli	a2, a4, 8
     74e:	064342        	s8i	a4, a3, 6
     751:	0a4342        	s8i	a4, a3, 10
     754:	0e4342        	s8i	a4, a3, 14
     757:	124342        	s8i	a4, a3, 18
     75a:	164342        	s8i	a4, a3, 22
     75d:	1a4342        	s8i	a4, a3, 26
     760:	1e4342        	s8i	a4, a3, 30
     763:	034322        	s8i	a2, a3, 3
     766:	074322        	s8i	a2, a3, 7
     769:	0b4322        	s8i	a2, a3, 11
     76c:	0f4322        	s8i	a2, a3, 15
     76f:	134322        	s8i	a2, a3, 19
     772:	174322        	s8i	a2, a3, 23
     775:	1b4322        	s8i	a2, a3, 27
     778:	1f4322        	s8i	a2, a3, 31
     77b:	20c332        	addi	a3, a3, 32
     77e:	f879           	s32i.n	a7, a8, 60
     780:	2e6872        	s32i	a7, a8, 184
     783:	f01d           	retw.n

00000785 <RNN_Init_fx+0x1a5>:
     785:	000000                                        ...

00000788 <RNN_Prcs_fx>:
     788:	03a136        	entry	a1, 0x1d0
     78b:	036d           	mov.n	a6, a3
     78d:	025d           	mov.n	a5, a2
     78f:	01ad           	mov.n	a10, a1
     791:	280c           	movi.n	a8, 2
     793:	09a876        	loopgtz	a8, 7a0 <RNN_Prcs_fx+0x18>
     796:	249d           	ae_l16si.n	a9, a4, 0
     798:	442b           	addi.n	a4, a4, 2
     79a:	119950        	slli	a9, a9, 11
     79d:	09ea94        	ae_s32ip	a9, a10, 4

000007a0 <RNN_Prcs_fx+0x18>:
     7a0:	690c           	movi.n	a9, 6
     7a2:	238976        	loop	a9, 7c9 <RNN_Prcs_fx+0x41>
     7a5:	249d           	ae_l16si.n	a9, a4, 0
     7a7:	34bd           	ae_l16si.n	a11, a4, 2
     7a9:	0294c2        	l16si	a12, a4, 4
     7ac:	0394d2        	l16si	a13, a4, 6
     7af:	119950        	slli	a9, a9, 11
     7b2:	11bb50        	slli	a11, a11, 11
     7b5:	11cc50        	slli	a12, a12, 11
     7b8:	11dd50        	slli	a13, a13, 11
     7bb:	448b           	addi.n	a4, a4, 8
     7bd:	09ea94        	ae_s32ip	a9, a10, 4
     7c0:	09eab4        	ae_s32ip	a11, a10, 4
     7c3:	09eac4        	ae_s32ip	a12, a10, 4
     7c6:	09ead4        	ae_s32ip	a13, a10, 4
     7c9:	fe28a1        	l32r	a10, 6c (93bc <fix_input_dense>)
     7cc:	fe2981        	l32r	a8, 70 (2a0 <fix_compute_dense>)
     7cf:	70c1b2        	addi	a11, a1, 112
     7d2:	20c110        	or	a12, a1, a1
     7d5:	0008e0        	callx8	a8
     7d8:	fe27a1        	l32r	a10, 74 (93d0 <fix_vad_gru>)
     7db:	70c1c2        	addi	a12, a1, 112
     7de:	fe2681        	l32r	a8, 78 (314 <fix_compute_gru>)
     7e1:	01d5b2        	addmi	a11, a5, 0x100
     7e4:	d0cbb2        	addi	a11, a11, -48
     7e7:	0008e0        	callx8	a8
     7ea:	058d           	mov.n	a8, a5
     7ec:	2e0c           	movi.n	a14, 2
     7ee:	891c           	movi.n	a9, 24
     7f0:	7fc1d2        	addi	a13, a1, 127
     7f3:	51cdd2        	addi	a13, a13, 81
     7f6:	228976        	loop	a9, 81c <RNN_Prcs_fx+0x94>
     7f9:	d30892        	l8ui	a9, a8, 211
     7fc:	d208a2        	l8ui	a10, a8, 210
     7ff:	d108b2        	l8ui	a11, a8, 209
     802:	d008c2        	l8ui	a12, a8, 208
     805:	884b           	addi.n	a8, a8, 4
     807:	119980        	slli	a9, a9, 8
     80a:	2099a0        	or	a9, a9, a10
     80d:	119980        	slli	a9, a9, 8
     810:	2099b0        	or	a9, a9, a11
     813:	119980        	slli	a9, a9, 8
     816:	2099c0        	or	a9, a9, a12
     819:	09ed94        	ae_s32ip	a9, a13, 4
     81c:	01ad           	mov.n	a10, a1
     81e:	320c           	movi.n	a2, 3
     820:	7fc182        	addi	a8, a1, 127
     823:	51c882        	addi	a8, a8, 81
     826:	60c882        	addi	a8, a8, 96
     829:	06ae76        	loopgtz	a14, 833 <RNN_Prcs_fx+0xab>
     82c:	0a98           	l32i.n	a9, a10, 0
     82e:	09e894        	ae_s32ip	a9, a8, 4
     831:	aa4b           	addi.n	a10, a10, 4
     833:	08bd           	mov.n	a11, a8
     835:	0a8d           	mov.n	a8, a10
     837:	0bad           	mov.n	a10, a11
     839:	2a8276        	loop	a2, 867 <RNN_Prcs_fx+0xdf>
     83c:	0898           	l32i.n	a9, a8, 0
     83e:	18b8           	l32i.n	a11, a8, 4
     840:	28c8           	l32i.n	a12, a8, 8
     842:	38d8           	l32i.n	a13, a8, 12
     844:	48e8           	l32i.n	a14, a8, 16
     846:	58f8           	l32i.n	a15, a8, 20
     848:	6828           	l32i.n	a2, a8, 24
     84a:	7838           	l32i.n	a3, a8, 28
     84c:	09ea94        	ae_s32ip	a9, a10, 4
     84f:	20c882        	addi	a8, a8, 32
     852:	09eab4        	ae_s32ip	a11, a10, 4
     855:	09eac4        	ae_s32ip	a12, a10, 4
     858:	09ead4        	ae_s32ip	a13, a10, 4
     85b:	09eae4        	ae_s32ip	a14, a10, 4
     85e:	09eaf4        	ae_s32ip	a15, a10, 4
     861:	09ea24        	ae_s32ip	a2, a10, 4
     864:	09ea34        	ae_s32ip	a3, a10, 4
     867:	fe05a1        	l32r	a10, 7c (93e8 <fix_noise_gru>)
     86a:	70c1c2        	addi	a12, a1, 112
     86d:	01d522        	addmi	a2, a5, 0x100
     870:	fe0281        	l32r	a8, 78 (314 <fix_compute_gru>)
     873:	30c222        	addi	a2, a2, 48
     876:	02bd           	mov.n	a11, a2
     878:	0008e0        	callx8	a8
     87b:	02cd           	mov.n	a12, a2
     87d:	fe00a1        	l32r	a10, 80 (9400 <fix_wind_output>)
     880:	fdfc81        	l32r	a8, 70 (2a0 <fix_compute_dense>)
     883:	02d1b2        	addmi	a11, a1, 0x200
     886:	a0cbb2        	addi	a11, a11, -96
     889:	0008e0        	callx8	a8
     88c:	6821c2        	l32i	a12, a1, 0x1a0
     88f:	21c7c0        	srai	a12, a12, 7
     892:	66cd           	ae_s16i.n	a12, a6, 0
     894:	f01d           	retw.n

00000896 <RNN_Prcs_fx+0x10e>:
	...

00000898 <WindDet_get_memsize>:
     898:	004136        	entry	a1, 32
     89b:	fdfa21        	l32r	a2, 84 (2488 <_DYNAMIC+0x470>)
     89e:	f01d           	retw.n

000008a0 <WindDet_Init>:
     8a0:	004136        	entry	a1, 32
     8a3:	fdf981        	l32r	a8, 88 (898 <WindDet_get_memsize>)
     8a6:	0008e0        	callx8	a8
     8a9:	0acd           	mov.n	a12, a10
     8ab:	fdf881        	l32r	a8, 8c (1f30 <memset>)
     8ae:	0b0c           	movi.n	a11, 0
     8b0:	02ad           	mov.n	a10, a2
     8b2:	0008e0        	callx8	a8
     8b5:	fdf681        	l32r	a8, 90 (5e0 <RNN_Init_fx>)
     8b8:	02ad           	mov.n	a10, a2
     8ba:	0008e0        	callx8	a8
     8bd:	fdf581        	l32r	a8, 94 (1098 <ini_st_fea>)
     8c0:	02d2a2        	addmi	a10, a2, 0x200
     8c3:	f0caa2        	addi	a10, a10, -16
     8c6:	0008e0        	callx8	a8
     8c9:	03bd           	mov.n	a11, a3
     8cb:	cc1c           	movi.n	a12, 28
     8cd:	fdf281        	l32r	a8, 98 (1f70 <memcpy>)
     8d0:	24d2a2        	addmi	a10, a2, 0x2400
     8d3:	6ccaa2        	addi	a10, a10, 108
     8d6:	0008e0        	callx8	a8
     8d9:	020c           	movi.n	a2, 0
     8db:	f01d           	retw.n

000008dd <WindDet_Init+0x3d>:
     8dd:	000000                                        ...

000008e0 <WindDet_Prcs>:
     8e0:	00e136        	entry	a1, 112
     8e3:	24d282        	addmi	a8, a2, 0x2400
     8e6:	6c0882        	l8ui	a8, a8, 108
     8e9:	f97c           	movi.n	a9, -1
     8eb:	58cc           	bnez.n	a8, 8f4 <WindDet_Prcs+0x14>
     8ed:	020c           	movi.n	a2, 0
     8ef:	649d           	ae_s16i.n	a9, a4, 0
     8f1:	f01d           	retw.n

000008f3 <WindDet_Prcs+0x13>:
	...

000008f4 <WindDet_Prcs+0x14>:
     8f4:	01bd           	mov.n	a11, a1
     8f6:	03ad           	mov.n	a10, a3
     8f8:	fde981        	l32r	a8, 9c (11c8 <FEATS_Prcs>)
     8fb:	02d232        	addmi	a3, a2, 0x200
     8fe:	f0c332        	addi	a3, a3, -16
     901:	03cd           	mov.n	a12, a3
     903:	0008e0        	callx8	a8
     906:	02ad           	mov.n	a10, a2
     908:	fde681        	l32r	a8, a0 (788 <RNN_Prcs_fx>)
     90b:	40c1b2        	addi	a11, a1, 64
     90e:	01cd           	mov.n	a12, a1
     910:	0008e0        	callx8	a8
     913:	02cd           	mov.n	a12, a2
     915:	fde381        	l32r	a8, a4 (1260 <post_proc>)
     918:	03bd           	mov.n	a11, a3
     91a:	2091a2        	l16si	a10, a1, 64
     91d:	0008e0        	callx8	a8
     920:	64ad           	ae_s16i.n	a10, a4, 0
     922:	020c           	movi.n	a2, 0
     924:	f01d           	retw.n

00000926 <WindDet_Prcs+0x46>:
	...

00000928 <fxdivs>:
     928:	004136        	entry	a1, 32
     92b:	025d           	mov.n	a5, a2
     92d:	180c           	movi.n	a8, 1
     92f:	728380        	salt	a8, a3, a8
     932:	e59050        	extui	a9, a5, 16, 15
     935:	312020        	srai	a2, a2, 16
     938:	854d           	ae_sext16	a4, a5
     93a:	1144f0        	slli	a4, a4, 1
     93d:	052f20        	extui	a2, a2, 31, 1
     940:	045f50        	extui	a5, a5, 15, 1
     943:	302820        	xor	a2, a8, a2
     946:	826d           	ae_sext16	a6, a2
     948:	1ae950        	depbits	a5, a9, 1, 15
     94b:	202240        	or	a2, a2, a4
     94e:	822d           	ae_sext16	a2, a2
     950:	f40c           	movi.n	a4, 15
     952:	25a476        	loopgtz	a4, 97b <fxdivs+0x53>
     955:	170c           	movi.n	a7, 1
     957:	404f10        	ssai	31
     95a:	1f1626        	beqi	a6, 1, 97d <fxdivs+0x55>
     95d:	c06530        	sub	a6, a5, a3

00000960 <fxdivs+0x38>:
     960:	1142f0        	slli	a4, a2, 1
     963:	e45060        	extui	a5, a6, 0, 15
     966:	815520        	src	a5, a5, a2
     969:	046f60        	extui	a6, a6, 15, 1
     96c:	020c           	movi.n	a2, 0
     96e:	306860        	xor	a6, a8, a6
     971:	866d           	ae_sext16	a6, a6
     973:	832760        	moveqz	a2, a7, a6
     976:	202240        	or	a2, a2, a4
     979:	822d           	ae_sext16	a2, a2

0000097b <fxdivs+0x53>:
     97b:	f01d           	retw.n

0000097d <fxdivs+0x55>:
     97d:	635a           	add.n	a6, a3, a5
     97f:	fff746        	j	960 <fxdivs+0x38>

00000982 <fxdivs+0x5a>:
	...

00000984 <fxsqrt>:
     984:	004136        	entry	a1, 32
     987:	063216        	beqz	a2, 9ee <fxsqrt+0x6a>
     98a:	fdc831        	l32r	a3, ac (8000 <mel_win_53+0x60>)
     98d:	fdc661        	l32r	a6, a8 (7d50 <TABLE_SQRT_RNN_42>)
     990:	090c           	movi.n	a9, 0
     992:	f40c           	movi.n	a4, 15
     994:	40e280        	nsa	a8, a2
     997:	401800        	ssl	a8
     99a:	c04480        	sub	a4, a4, a8
     99d:	a17200        	sll	a7, a2
     9a0:	044040        	extui	a4, a4, 0, 1
     9a3:	400400        	ssr	a4
     9a6:	c04480        	sub	a4, a4, a8
     9a9:	b17070        	sra	a7, a7
     9ac:	417970        	srli	a7, a7, 9
     9af:	44fb           	addi.n	a4, a4, 15
     9b1:	315070        	srai	a5, a7, 16
     9b4:	f0c552        	addi	a5, a5, -16
     9b7:	535590        	max	a5, a5, a9
     9ba:	905560        	addx2	a5, a5, a6
     9bd:	256d           	ae_l16si.n	a6, a5, 0
     9bf:	355d           	ae_l16si.n	a5, a5, 2
     9c1:	214140        	srai	a4, a4, 1
     9c4:	300774        	ae_cvtp24a16x2.ll	aep0, a7, a7
     9c7:	c05560        	sub	a5, a5, a6
     9ca:	116600        	slli	a6, a6, 16
     9cd:	115500        	slli	a5, a5, 16
     9d0:	36c534        	ae_cvtq48a32s	aeq3, a5
     9d3:	368634        	ae_cvtq48a32s	aeq2, a6
     9d6:	d7e13ec37f 	{ ae_cvtq48a32s	aeq1, a3; ae_mulaq32sp16u.l	aeq2, aeq3, aep0 }
     9db:	f9c442        	addi	a4, a4, -7
     9de:	fc24fea40f 	{ ae_slaasq56s	aeq0, aeq2, a4; nop }
     9e3:	0109c4        	ae_addq56	aeq0, aeq0, aeq1
     9e6:	368204        	ae_trunca32q48	a2, aeq0
     9e9:	312020        	srai	a2, a2, 16
     9ec:	f01d           	retw.n

000009ee <fxsqrt+0x6a>:
     9ee:	020c           	movi.n	a2, 0
     9f0:	f01d           	retw.n

000009f2 <fxsqrt+0x6e>:
	...

000009f4 <mfcc_frame>:
     9f4:	006136        	entry	a1, 48
     9f7:	1139           	s32i.n	a3, a1, 4
     9f9:	0159           	s32i.n	a5, a1, 0
     9fb:	fdada1        	l32r	a10, b0 (7c29 <noise_gru_recurrent_weights_42+0x1ab9>)
     9fe:	04d292        	addmi	a9, a2, 0x400
     a01:	301aa4        	ae_cvtp24a16x2.ll	aep1, a10, a10
     a04:	bcc9a2        	addi	a10, a9, -68
     a07:	bec992        	addi	a9, a9, -66
     a0a:	40a904        	ae_lp16f.iu	aep2, a9, 0
     a0d:	fd4c2c8a0f 	{ ae_lp16f.iu	aep0, a10, 0; ae_cvtq48p24s.l	aeq1, aep2 }
     a12:	ee21c1debf 	{ movi	a11, 0x1de; ae_mulsfp24s.ll	aeq1, aep0, aep1 }
     a17:	1bd482        	addmi	a8, a4, 0x1b00
     a1a:	fc04c4c88e 	{ addi	a8, a8, 76; ae_satq48s	aeq0, aeq1 }
     a1f:	6248f4        	ae_sq32f.iu	aeq0, a8, -4
     a22:	4089f4        	ae_lp16f.iu	aep0, a9, -2
     a25:	108b76        	loop	a11, a39 <mfcc_frame+0x45>
     a28:	fc0c2c8aff 	{ ae_lp16f.iu	aep0, a10, -2; ae_cvtq48p24s.l	aeq0, aep0 }
     a2d:	0101b4        	ae_mulsfp24s.ll	aeq0, aep0, aep1
     a30:	060844        	ae_satq48s	aeq0, aeq0
     a33:	6248f4        	ae_sq32f.iu	aeq0, a8, -4
     a36:	4089f4        	ae_lp16f.iu	aep0, a9, -2

00000a39 <mfcc_frame+0x45>:
     a39:	fd9ea1        	l32r	a10, b4 (ffff83d7 <_end+0xfffeefa3>)
     a3c:	22bd           	ae_l16si.n	a11, a2, 0
     a3e:	14d482        	addmi	a8, a4, 0x1400
     a41:	18d492        	addmi	a9, a4, 0x1800
     a44:	8cc992        	addi	a9, a9, -116
     a47:	ccc882        	addi	a8, a8, -52
     a4a:	08ed           	mov.n	a14, a8
     a4c:	d1aba0        	mul16s	a10, a11, a10
     a4f:	11bb10        	slli	a11, a11, 15
     a52:	aaba           	add.n	a10, a10, a11
     a54:	11aaf0        	slli	a10, a10, 1
     a57:	0048a2        	s8i	a10, a8, 0
     a5a:	41a8a0        	srli	a10, a10, 8
     a5d:	fd96b1        	l32r	a11, b8 (7f9e <hanwindow_51+0x1de>)
     a60:	0148a2        	s8i	a10, a8, 1
     a63:	41a8a0        	srli	a10, a10, 8
     a66:	0248a2        	s8i	a10, a8, 2
     a69:	41a8a0        	srli	a10, a10, 8
     a6c:	0348a2        	s8i	a10, a8, 3
     a6f:	fd93a1        	l32r	a10, bc (7dc0 <hanwindow_51>)
     a72:	40bb04        	ae_lp16f.iu	aep3, a11, 0
     a75:	601804        	ae_lq32f.iu	aeq0, a8, 0
     a78:	40ca04        	ae_lp16f.iu	aep4, a10, 0
     a7b:	d4f23d790f 	{ ae_lq32f.iu	aeq3, a9, 0; ae_mulfq32sp16s.l	aeq0, aeq0, aep4 }
     a80:	03bad4        	ae_mulfq32sp16s.l	aeq3, aeq3, aep3
     a83:	fc00c0efcf 	{ movi	a12, 239; ae_satq48s	aeq0, aeq0 }
     a88:	624804        	ae_sq32f.iu	aeq0, a8, 0
     a8b:	fe84fd181f 	{ ae_lq32f.iu	aeq0, a8, 4; ae_satq48s	aeq2, aeq3 }
     a90:	626904        	ae_sq32f.iu	aeq2, a9, 0
     a93:	605914        	ae_lq32f.iu	aeq1, a9, 4
     a96:	408a14        	ae_lp16f.iu	aep0, a10, 2
     a99:	f03d           	nop.n
     a9b:	f03d           	nop.n
     a9d:	1b8c76        	loop	a12, abc <mfcc_frame+0xc8>

00000aa0 <mfcc_frame+0xac>:
     aa0:	d4e22c8bff 	{ ae_lp16f.iu	aep0, a11, -2; ae_mulfq32sp16s.l	aeq0, aeq0, aep0 }
     aa5:	030a54        	ae_mulfq32sp16s.l	aeq1, aeq1, aep0
     aa8:	060844        	ae_satq48s	aeq0, aeq0
     aab:	fd04f9c80f 	{ ae_sq32f.iu	aeq0, a8, 0; ae_satq48s	aeq1, aeq1 }
     ab0:	601814        	ae_lq32f.iu	aeq0, a8, 4
     ab3:	625904        	ae_sq32f.iu	aeq1, a9, 0
     ab6:	605914        	ae_lq32f.iu	aeq1, a9, 4
     ab9:	408a14        	ae_lp16f.iu	aep0, a10, 2

00000abc <mfcc_frame+0xc8>:
     abc:	0ead           	mov.n	a10, a14
     abe:	20a092        	movi	a9, 32
     ac1:	1bd482        	addmi	a8, a4, 0x1b00
     ac4:	fc2484888e 	{ addi	a8, a8, 72; ae_zeroq56	aeq0 }
     ac9:	028976        	loop	a9, acf <mfcc_frame+0xdb>
     acc:	624814        	ae_sq32f.iu	aeq0, a8, 4
     acf:	043d           	mov.n	a3, a4
     ad1:	04d422        	addmi	a2, a4, 0x400
     ad4:	fd7b81        	l32r	a8, c0 (1510 <real_512_fft>)
     ad7:	ccc222        	addi	a2, a2, -52
     ada:	20b220        	or	a11, a2, a2
     add:	0008e0        	callx8	a8
     ae0:	02bd           	mov.n	a11, a2
     ae2:	0aed           	mov.n	a14, a10
     ae4:	04d482        	addmi	a8, a4, 0x400
     ae7:	cac882        	addi	a8, a8, -54
     aea:	02a2a2        	movi	a10, 0x202
     aed:	601204        	ae_lq32f.iu	aeq0, a2, 0
     af0:	029d           	mov.n	a9, a2
     af2:	0b8a76        	loop	a10, b01 <mfcc_frame+0x10d>
     af5:	3500f4        	ae_sraiq56	aeq0, aeq0, 3
     af8:	098c04        	ae_truncp24q48x2	aep0, aeq0, aeq0
     afb:	428814        	ae_sp16f.l.iu	aep0, a8, 2
     afe:	601914        	ae_lq32f.iu	aeq0, a9, 4
     b01:	00a182        	movi	a8, 0x100
     b04:	0bad           	mov.n	a10, a11
     b06:	44db04        	ae_lp16x2f.iu	aep5, a11, 0
     b09:	f0b7ad8b1f 	{ ae_lp16x2f.iu	aep0, a11, 4; ae_mulzaap24s.hh.ll	aeq0, aep5, aep5 }
     b0e:	0a8876        	loop	a8, b1c <mfcc_frame+0x128>

00000b11 <mfcc_frame+0x11d>:
     b11:	f003bf08ef 	{ ae_trunca32q48	a8, aeq0; ae_mulzaap24s.hh.ll	aeq0, aep0, aep0 }
     b16:	09ea84        	ae_s32ip	a8, a10, 4
     b19:	448b14        	ae_lp16x2f.iu	aep0, a11, 4

00000b1c <mfcc_frame+0x128>:
     b1c:	04d382        	addmi	a8, a3, 0x400
     b1f:	368904        	ae_trunca32q48	a9, aeq0
     b22:	09ea94        	ae_s32ip	a9, a10, 4
     b25:	d0c882        	addi	a8, a8, -48
     b28:	ffa092        	movi	a9, 255
     b2b:	fc24bd380f 	{ ae_lq32f.iu	aeq1, a8, 0; ae_zeroq56	aeq0 }
     b30:	048976        	loop	a9, b38 <mfcc_frame+0x144>

00000b33 <mfcc_frame+0x13f>:
     b33:	f8663d381f 	{ ae_lq32f.iu	aeq1, a8, 4; ae_addq56	aeq0, aeq0, aeq1 }

00000b38 <mfcc_frame+0x144>:
     b38:	08d3c2        	addmi	a12, a3, 0x800
     b3b:	368804        	ae_trunca32q48	a8, aeq0
     b3e:	1389           	s32i.n	a8, a3, 4
     b40:	ccccc2        	addi	a12, a12, -52
     b43:	030ca2        	l8ui	a10, a12, 3
     b46:	020cd2        	l8ui	a13, a12, 2
     b49:	010cb2        	l8ui	a11, a12, 1
     b4c:	11aa80        	slli	a10, a10, 8
     b4f:	000cc2        	l8ui	a12, a12, 0
     b52:	20aad0        	or	a10, a10, a13
     b55:	11aa80        	slli	a10, a10, 8
     b58:	20aab0        	or	a10, a10, a11
     b5b:	04d3b2        	addmi	a11, a3, 0x400
     b5e:	11aa80        	slli	a10, a10, 8
     b61:	20aac0        	or	a10, a10, a12
     b64:	cccbb2        	addi	a11, a11, -52
     b67:	030b92        	l8ui	a9, a11, 3
     b6a:	020bf2        	l8ui	a15, a11, 2
     b6d:	010bd2        	l8ui	a13, a11, 1
     b70:	119980        	slli	a9, a9, 8
     b73:	000bb2        	l8ui	a11, a11, 0
     b76:	2099f0        	or	a9, a9, a15
     b79:	1f0c           	movi.n	a15, 1
     b7b:	119980        	slli	a9, a9, 8
     b7e:	2099d0        	or	a9, a9, a13
     b81:	119980        	slli	a9, a9, 8
     b84:	2099b0        	or	a9, a9, a11
     b87:	1b1c           	movi.n	a11, 17
     b89:	99aa           	add.n	a9, a9, a10
     b8b:	908890        	addx2	a8, a8, a9
     b8e:	40e8a0        	nsa	a10, a8
     b91:	aa9b           	addi.n	a10, a10, 9
     b93:	8aad           	ae_sext16	a10, a10
     b95:	0b2ab7        	blt	a10, a11, ba4 <mfcc_frame+0x1b0>
     b98:	efcad2        	addi	a13, a10, -17
     b9b:	401d00        	ssl	a13
     b9e:	a18800        	sll	a8, a8
     ba1:	000546        	j	bba <mfcc_frame+0x1c6>

00000ba4 <mfcc_frame+0x1b0>:
     ba4:	c09ba0        	sub	a9, a11, a10
     ba7:	0d1c           	movi.n	a13, 16
     ba9:	c0dda0        	sub	a13, a13, a10
     bac:	401d00        	ssl	a13
     baf:	a1cf00        	sll	a12, a15
     bb2:	400900        	ssr	a9
     bb5:	88ca           	add.n	a8, a8, a12
     bb7:	918080        	srl	a8, a8

00000bba <mfcc_frame+0x1c6>:
     bba:	fd4251        	l32r	a5, c4 (ffff0000 <_end+0xfffe6bcc>)
     bbd:	2a9816        	beqz	a8, e6a <mfcc_frame+0x476>
     bc0:	2ac896        	bltz	a8, e70 <mfcc_frame+0x47c>
     bc3:	40e8b0        	nsa	a11, a8
     bc6:	bb1b           	addi.n	a11, a11, 1
     bc8:	8bbd           	ae_sext16	a11, a11

00000bca <mfcc_frame+0x1d6>:
     bca:	fd41c1        	l32r	a12, d0 (29973 <_end+0x2053f>)
     bcd:	fd3ed1        	l32r	a13, c8 (2c5d <vad_gru_weights_38+0x25d>)
     bd0:	fd3f91        	l32r	a9, cc (80000000 <_end+0x7fff6bcc>)
     bd3:	401b00        	ssl	a11
     bd6:	a18800        	sll	a8, a8
     bd9:	889a           	add.n	a8, a8, a9
     bdb:	d1dbd0        	mul16s	a13, a11, a13
     bde:	590c           	movi.n	a9, 5
     be0:	f58080        	extui	a8, a8, 16, 16
     be3:	301884        	ae_cvtp24a16x2.ll	aep1, a8, a8
     be6:	c0ccd0        	sub	a12, a12, a13
     be9:	fd3a81        	l32r	a8, d4 (7d40 <ln_coef_39>)
     bec:	11cc80        	slli	a12, a12, 8
     bef:	360c34        	ae_cvtq48a32s	aeq0, a12
     bf2:	350214        	ae_slliq56	aeq0, aeq0, 8
     bf5:	f966ac880f 	{ ae_lp16f.iu	aep0, a8, 0; ae_movp48	aep2, aep1 }
     bfa:	0a8976        	loop	a9, c08 <mfcc_frame+0x214>
     bfd:	011a74        	ae_mulfp24s.ll	aeq1, aep1, aep2
     c00:	e6232c881f 	{ ae_lp16f.iu	aep0, a8, 2; ae_mulap24s.ll	aeq0, aep0, aep1 }
     c05:	1b9f14        	ae_roundsp16q48asym	aep1, aeq1

00000c08 <mfcc_frame+0x214>:
     c08:	368534        	ae_cvtq48a32s	aeq2, a5
     c0b:	3540f4        	ae_sraiq56	aeq1, aeq0, 3
     c0e:	0161e4        	ae_andq56	aeq1, aeq1, aeq2
     c11:	1b9f64        	ae_roundsp16q48asym	aep6, aeq1
     c14:	366824        	ae_trunca16p24s.l	a8, aep6

00000c17 <mfcc_frame+0x223>:
     c17:	032d           	mov.n	a2, a3
     c19:	fd2fb1        	l32r	a11, d8 (a65b6 <_end+0x9d182>)
     c1c:	fd30c1        	l32r	a12, dc (58b9 <noise_gru_weights_41+0x20e9>)
     c1f:	c09ea0        	sub	a9, a14, a10
     c22:	04d342        	addmi	a4, a3, 0x400
     c25:	7e1b           	addi.n	a7, a14, 1
     c27:	877d           	ae_sext16	a7, a7
     c29:	c8c462        	addi	a6, a4, -56
     c2c:	8299c0        	mull	a9, a9, a12
     c2f:	444b           	addi.n	a4, a4, 4
     c31:	99ba           	add.n	a9, a9, a11
     c33:	01b8           	l32i.n	a11, a1, 0
     c35:	219490        	srai	a9, a9, 4
     c38:	809890        	add	a9, a8, a9
     c3b:	01c992        	addi	a9, a9, 1
     c3e:	219190        	srai	a9, a9, 1
     c41:	6b9d           	ae_s16i.n	a9, a11, 0

00000c43 <mfcc_frame+0x24f>:
     c43:	fd2781        	l32r	a8, e0 (984 <fxsqrt>)
     c46:	04d2c2        	addmi	a12, a2, 0x400
     c49:	ccccc2        	addi	a12, a12, -52
     c4c:	000cb2        	l8ui	a11, a12, 0
     c4f:	030ca2        	l8ui	a10, a12, 3
     c52:	020cd2        	l8ui	a13, a12, 2
     c55:	010cc2        	l8ui	a12, a12, 1
     c58:	11aa80        	slli	a10, a10, 8
     c5b:	20aad0        	or	a10, a10, a13
     c5e:	11aa80        	slli	a10, a10, 8
     c61:	20aac0        	or	a10, a10, a12
     c64:	11aa80        	slli	a10, a10, 8
     c67:	20aab0        	or	a10, a10, a11
     c6a:	11aac0        	slli	a10, a10, 4
     c6d:	0008e0        	callx8	a8
     c70:	360a34        	ae_cvtq48a32s	aeq0, a10
     c73:	1e0c           	movi.n	a14, 1
     c75:	368e34        	ae_cvtq48a32s	aeq2, a14
     c78:	fc24fe87cf 	{ ae_slaasq56s	aeq3, aeq0, a7; nop }
     c7d:	09a304        	ae_ltq56s	b0, aeq3, aeq2
     c80:	224b           	addi.n	a2, a2, 4
     c82:	0304d0        	rsr.br	a13
     c85:	04d0d0        	extui	a13, a13, 0, 1
     c88:	1dcc           	bnez.n	a13, c8d <mfcc_frame+0x299>
     c8a:	06bb34        	ae_movq56	aeq2, aeq3

00000c8d <mfcc_frame+0x299>:
     c8d:	626614        	ae_sq32f.iu	aeq2, a6, 4
     c90:	af9247        	bne	a2, a4, c43 <mfcc_frame+0x24f>
     c93:	fd1581        	l32r	a8, e8 (7fa0 <mel_win_53>)
     c96:	fd13e1        	l32r	a14, e4 (8370 <mel_st_54>)
     c99:	1cd3f2        	addmi	a15, a3, 0x1c00
     c9c:	d0cff2        	addi	a15, a15, -48
     c9f:	34ce22        	addi	a2, a14, 52
     ca2:	ff24840ebe 	{ addi	a11, a14, 64; ae_zeroq56	aeq3 }
     ca7:	fc24ac880f 	{ ae_lp16f.iu	aep0, a8, 0; ae_zeroq56	aeq0 }

00000cac <mfcc_frame+0x2b8>:
     cac:	2e9d           	ae_l16si.n	a9, a14, 0
     cae:	2bad           	ae_l16si.n	a10, a11, 0
     cb0:	bb2b           	addi.n	a11, a11, 2
     cb2:	a09930        	addx4	a9, a9, a3
     cb5:	04d992        	addmi	a9, a9, 0x400
     cb8:	ccc992        	addi	a9, a9, -52
     cbb:	605904        	ae_lq32f.iu	aeq1, a9, 0
     cbe:	07aa76        	loopgtz	a10, cc9 <mfcc_frame+0x2d5>
     cc1:	f4e3ac881f 	{ ae_lp16f.iu	aep0, a8, 2; ae_mulaq32sp16s.l	aeq0, aeq1, aep0 }
     cc6:	605914        	ae_lq32f.iu	aeq1, a9, 4

00000cc9 <mfcc_frame+0x2d5>:
     cc9:	354294        	ae_slliq56	aeq1, aeq0, 10
     ccc:	069934        	ae_roundsq32asym	aeq2, aeq1
     ccf:	d6d4402eee 	{ addi	a14, a14, 2; ae_ltq56s	b1, aeq3, aeq2 }
     cd4:	fc24bf317f 	{ ae_movq56	aeq1, aeq3; ae_zeroq56	aeq0 }
     cd9:	ff1479ef1f 	{ ae_sq32f.iu	aeq2, a15, 4; ae_movq56	aeq3, aeq2 }
     cde:	0304a0        	rsr.br	a10
     ce1:	04a1a0        	extui	a10, a10, 1, 1
     ce4:	1acc           	bnez.n	a10, ce9 <mfcc_frame+0x2f5>
     ce6:	06db34        	ae_movq56	aeq3, aeq1

00000ce9 <mfcc_frame+0x2f5>:
     ce9:	bf9e27        	bne	a14, a2, cac <mfcc_frame+0x2b8>
     cec:	080c           	movi.n	a8, 0
     cee:	8f0c           	movi.n	a15, 8
     cf0:	1cd322        	addmi	a2, a3, 0x1c00
     cf3:	36be04        	ae_trunca32q48	a14, aeq3
     cf6:	d4c242        	addi	a4, a2, -44
     cf9:	3cc222        	addi	a2, a2, 60
     cfc:	04ad           	mov.n	a10, a4
     cfe:	40eee0        	nsa	a14, a14
     d01:	c0efe0        	sub	a14, a15, a14
     d04:	8eed           	ae_sext16	a14, a14
     d06:	53ee80        	max	a14, a14, a8
     d09:	fcf481        	l32r	a8, dc (58b9 <noise_gru_weights_41+0x20e9>)
     d0c:	8e9d           	ae_sext16	a9, a14
     d0e:	c0ff90        	sub	a15, a15, a9
     d11:	d1ee80        	mul16s	a14, a14, a8
     d14:	401f00        	ssl	a15
     d17:	ee8b           	addi.n	a14, a14, 8
     d19:	21e4e0        	srai	a14, a14, 4
     d1c:	001ac6        	j	d8b <mfcc_frame+0x397>

00000d1f <mfcc_frame+0x32b>:
	...

00000d21 <mfcc_frame+0x32d>:
     d21:	40e8b0        	nsa	a11, a8
     d24:	bb1b           	addi.n	a11, a11, 1
     d26:	8bbd           	ae_sext16	a11, a11

00000d28 <mfcc_frame+0x334>:
     d28:	368534        	ae_cvtq48a32s	aeq2, a5
     d2b:	d1dbd0        	mul16s	a13, a11, a13
     d2e:	401b00        	ssl	a11
     d31:	a18800        	sll	a8, a8
     d34:	889a           	add.n	a8, a8, a9
     d36:	c0ccd0        	sub	a12, a12, a13
     d39:	590c           	movi.n	a9, 5
     d3b:	f58080        	extui	a8, a8, 16, 16
     d3e:	11cc80        	slli	a12, a12, 8
     d41:	301884        	ae_cvtp24a16x2.ll	aep1, a8, a8
     d44:	fce481        	l32r	a8, d4 (7d40 <ln_coef_39>)
     d47:	360c34        	ae_cvtq48a32s	aeq0, a12
     d4a:	350214        	ae_slliq56	aeq0, aeq0, 8
     d4d:	f966ac880f 	{ ae_lp16f.iu	aep0, a8, 0; ae_movp48	aep2, aep1 }
     d52:	0a8976        	loop	a9, d60 <mfcc_frame+0x36c>
     d55:	011a74        	ae_mulfp24s.ll	aeq1, aep1, aep2
     d58:	e6232c881f 	{ ae_lp16f.iu	aep0, a8, 2; ae_mulap24s.ll	aeq0, aep0, aep1 }
     d5d:	1b9f14        	ae_roundsp16q48asym	aep1, aeq1

00000d60 <mfcc_frame+0x36c>:
     d60:	3540f4        	ae_sraiq56	aeq1, aeq0, 3
     d63:	0161e4        	ae_andq56	aeq1, aeq1, aeq2
     d66:	1b9f74        	ae_roundsp16q48asym	aep7, aeq1
     d69:	367824        	ae_trunca16p24s.l	a8, aep7

00000d6c <mfcc_frame+0x378>:
     d6c:	98ea           	add.n	a9, a8, a14
     d6e:	401f00        	ssl	a15
     d71:	004a92        	s8i	a9, a10, 0
     d74:	419890        	srli	a9, a9, 8
     d77:	014a92        	s8i	a9, a10, 1
     d7a:	419890        	srli	a9, a9, 8
     d7d:	024a92        	s8i	a9, a10, 2
     d80:	419890        	srli	a9, a9, 8
     d83:	034a92        	s8i	a9, a10, 3
     d86:	aa4b           	addi.n	a10, a10, 4
     d88:	3a1a27        	beq	a10, a2, dc6 <mfcc_frame+0x3d2>

00000d8b <mfcc_frame+0x397>:
     d8b:	000a92        	l8ui	a9, a10, 0
     d8e:	030a82        	l8ui	a8, a10, 3
     d91:	020ac2        	l8ui	a12, a10, 2
     d94:	010ab2        	l8ui	a11, a10, 1
     d97:	118880        	slli	a8, a8, 8
     d9a:	2088c0        	or	a8, a8, a12
     d9d:	118880        	slli	a8, a8, 8
     da0:	2088b0        	or	a8, a8, a11
     da3:	118880        	slli	a8, a8, 8
     da6:	208890        	or	a8, a8, a9
     da9:	a18800        	sll	a8, a8
     dac:	089c           	beqz.n	a8, dc0 <mfcc_frame+0x3cc>
     dae:	fcc8c1        	l32r	a12, d0 (29973 <_end+0x2053f>)
     db1:	fcc5d1        	l32r	a13, c8 (2c5d <vad_gru_weights_38+0x25d>)
     db4:	fcc691        	l32r	a9, cc (80000000 <_end+0x7fff6bcc>)
     db7:	f668d6        	bgez	a8, d21 <mfcc_frame+0x32d>
     dba:	0b0c           	movi.n	a11, 0
     dbc:	ffda06        	j	d28 <mfcc_frame+0x334>

00000dbf <mfcc_frame+0x3cb>:
	...

00000dc0 <mfcc_frame+0x3cc>:
     dc0:	fccb81        	l32r	a8, ec (ffff8000 <_end+0xfffeebcc>)
     dc3:	ffe946        	j	d6c <mfcc_frame+0x378>

00000dc6 <mfcc_frame+0x3d2>:
     dc6:	11b8           	l32i.n	a11, a1, 4
     dc8:	1e0c           	movi.n	a14, 1
     dca:	af1c           	movi.n	a15, 26
     dcc:	fcc991        	l32r	a9, f0 (8424 <DCT_56+0x34>)
     dcf:	aa1c           	movi.n	a10, 26
     dd1:	048d           	mov.n	a8, a4
     dd3:	fc24ac890f 	{ ae_lp16f.iu	aep0, a9, 0; ae_zeroq56	aeq0 }

00000dd8 <mfcc_frame+0x3e4>:
     dd8:	605804        	ae_lq32f.iu	aeq1, a8, 0
     ddb:	ee1b           	addi.n	a14, a14, 1
     ddd:	078a76        	loop	a10, de8 <mfcc_frame+0x3f4>
     de0:	f4e3bd381f 	{ ae_lq32f.iu	aeq1, a8, 4; ae_mulaq32sp16s.l	aeq0, aeq1, aep0 }
     de5:	408914        	ae_lp16f.iu	aep0, a9, 2

00000de8 <mfcc_frame+0x3f4>:
     de8:	aa1c           	movi.n	a10, 26
     dea:	35c414        	ae_slliq56	aeq3, aeq0, 16
     ded:	f87bd3844f 	{ or	a8, a4, a4; ae_roundsp16q48asym	aep1, aeq3 }
     df2:	fc24b29b1f 	{ ae_sp16f.l.iu	aep1, a11, 2; ae_zeroq56	aeq0 }
     df7:	dd9ef7        	bne	a14, a15, dd8 <mfcc_frame+0x3e4>
     dfa:	fcbed1        	l32r	a13, f4 (83f0 <DCT_56>)
     dfd:	020c           	movi.n	a2, 0
     dff:	0f0c           	movi.n	a15, 0
     e01:	a91c           	movi.n	a9, 26
     e03:	1ad382        	addmi	a8, a3, 0x1a00
     e06:	d8c882        	addi	a8, a8, -40
     e09:	388976        	loop	a9, e45 <mfcc_frame+0x451>
     e0c:	02d892        	addmi	a9, a8, 0x200
     e0f:	2ded           	ae_l16si.n	a14, a13, 0
     e11:	fcc992        	addi	a9, a9, -4
     e14:	884b           	addi.n	a8, a8, 4
     e16:	0309a2        	l8ui	a10, a9, 3
     e19:	0209b2        	l8ui	a11, a9, 2
     e1c:	0109c2        	l8ui	a12, a9, 1
     e1f:	000992        	l8ui	a9, a9, 0
     e22:	11aa80        	slli	a10, a10, 8
     e25:	20aab0        	or	a10, a10, a11
     e28:	11aa80        	slli	a10, a10, 8
     e2b:	20aac0        	or	a10, a10, a12
     e2e:	11aa80        	slli	a10, a10, 8
     e31:	209a90        	or	a9, a10, a9
     e34:	829e90        	mull	a9, a14, a9
     e37:	dd2b           	addi.n	a13, a13, 2
     e39:	ff9a           	add.n	a15, a15, a9
     e3b:	31ef90        	srai	a14, a9, 31
     e3e:	ee2a           	add.n	a14, a14, a2
     e40:	629f90        	saltu	a9, a15, a9
     e43:	29ea           	add.n	a2, a9, a14
     e45:	fc9981        	l32r	a8, ac (8000 <mel_win_53+0x60>)
     e48:	404200        	ssai	2
     e4b:	029d           	mov.n	a9, a2
     e4d:	219290        	srai	a9, a9, 2
     e50:	8122f0        	src	a2, a2, a15
     e53:	828a           	add.n	a8, a2, a8
     e55:	622820        	saltu	a2, a8, a2
     e58:	404010        	ssai	16
     e5b:	229a           	add.n	a2, a2, a9
     e5d:	812280        	src	a2, a2, a8
     e60:	1188           	l32i.n	a8, a1, 4
     e62:	682d           	ae_s16i.n	a2, a8, 0
     e64:	822d           	ae_sext16	a2, a2
     e66:	1329           	s32i.n	a2, a3, 4
     e68:	f01d           	retw.n

00000e6a <mfcc_frame+0x476>:
     e6a:	fca081        	l32r	a8, ec (ffff8000 <_end+0xfffeebcc>)
     e6d:	ff6986        	j	c17 <mfcc_frame+0x223>

00000e70 <mfcc_frame+0x47c>:
     e70:	0b0c           	movi.n	a11, 0
     e72:	ff5506        	j	bca <mfcc_frame+0x1d6>

00000e75 <mfcc_frame+0x481>:
     e75:	000000                                        ...

00000e78 <cmn_proc_fix>:
     e78:	006136        	entry	a1, 48
     e7b:	03bd           	mov.n	a11, a3
     e7d:	028d           	mov.n	a8, a2
     e7f:	049d           	mov.n	a9, a4
     e81:	05ad           	mov.n	a10, a5
     e83:	fc9af1        	l32r	a15, ec (ffff8000 <_end+0xfffeebcc>)
     e86:	9e1c           	movi.n	a14, 25
     e88:	fc9cd1        	l32r	a13, f8 (8940 <div_const_58>)
     e8b:	1dd462        	addmi	a6, a4, 0x1d00
     e8e:	a0c662        	addi	a6, a6, -96
     e91:	0169           	s32i.n	a6, a1, 0
     e93:	0df556        	bnez	a5, f76 <cmn_proc_fix+0xfe>
     e96:	070c           	movi.n	a7, 0
     e98:	320c           	movi.n	a2, 3
     e9a:	2c0c           	movi.n	a12, 2
     e9c:	1cd432        	addmi	a3, a4, 0x1c00
     e9f:	38c332        	addi	a3, a3, 56
     ea2:	16ac76        	loopgtz	a12, ebc <cmn_proc_fix+0x44>
     ea5:	416870        	srli	a6, a7, 8
     ea8:	044372        	s8i	a7, a3, 4
     eab:	054362        	s8i	a6, a3, 5
     eae:	416860        	srli	a6, a6, 8
     eb1:	064362        	s8i	a6, a3, 6
     eb4:	416860        	srli	a6, a6, 8
     eb7:	074362        	s8i	a6, a3, 7
     eba:	334b           	addi.n	a3, a3, 4

00000ebc <cmn_proc_fix+0x44>:
     ebc:	6c0c           	movi.n	a12, 6
     ebe:	6b8276        	loop	a2, f2d <cmn_proc_fix+0xb5>
     ec1:	412870        	srli	a2, a7, 8
     ec4:	044372        	s8i	a7, a3, 4
     ec7:	084372        	s8i	a7, a3, 8
     eca:	0c4372        	s8i	a7, a3, 12
     ecd:	104372        	s8i	a7, a3, 16
     ed0:	144372        	s8i	a7, a3, 20
     ed3:	184372        	s8i	a7, a3, 24
     ed6:	1c4372        	s8i	a7, a3, 28
     ed9:	204372        	s8i	a7, a3, 32
     edc:	054322        	s8i	a2, a3, 5
     edf:	414820        	srli	a4, a2, 8
     ee2:	094322        	s8i	a2, a3, 9
     ee5:	0d4322        	s8i	a2, a3, 13
     ee8:	114322        	s8i	a2, a3, 17
     eeb:	154322        	s8i	a2, a3, 21
     eee:	194322        	s8i	a2, a3, 25
     ef1:	1d4322        	s8i	a2, a3, 29
     ef4:	214322        	s8i	a2, a3, 33
     ef7:	064342        	s8i	a4, a3, 6
     efa:	412840        	srli	a2, a4, 8
     efd:	0a4342        	s8i	a4, a3, 10
     f00:	0e4342        	s8i	a4, a3, 14
     f03:	124342        	s8i	a4, a3, 18
     f06:	164342        	s8i	a4, a3, 22
     f09:	1a4342        	s8i	a4, a3, 26
     f0c:	1e4342        	s8i	a4, a3, 30
     f0f:	224342        	s8i	a4, a3, 34
     f12:	074322        	s8i	a2, a3, 7
     f15:	0b4322        	s8i	a2, a3, 11
     f18:	0f4322        	s8i	a2, a3, 15
     f1b:	134322        	s8i	a2, a3, 19
     f1e:	174322        	s8i	a2, a3, 23
     f21:	1b4322        	s8i	a2, a3, 27
     f24:	1f4322        	s8i	a2, a3, 31
     f27:	234322        	s8i	a2, a3, 35
     f2a:	20c332        	addi	a3, a3, 32
     f2d:	0128           	l32i.n	a2, a1, 0
     f2f:	414870        	srli	a4, a7, 8
     f32:	5da052        	movi	a5, 93
     f35:	07ac76        	loopgtz	a12, f40 <cmn_proc_fix+0xc8>
     f38:	004272        	s8i	a7, a2, 0
     f3b:	014242        	s8i	a4, a2, 1
     f3e:	222b           	addi.n	a2, a2, 2
     f40:	328576        	loop	a5, f76 <cmn_proc_fix+0xfe>
     f43:	004272        	s8i	a7, a2, 0
     f46:	014242        	s8i	a4, a2, 1
     f49:	024272        	s8i	a7, a2, 2
     f4c:	034242        	s8i	a4, a2, 3
     f4f:	044272        	s8i	a7, a2, 4
     f52:	054242        	s8i	a4, a2, 5
     f55:	064272        	s8i	a7, a2, 6
     f58:	074242        	s8i	a4, a2, 7
     f5b:	084272        	s8i	a7, a2, 8
     f5e:	094242        	s8i	a4, a2, 9
     f61:	0a4272        	s8i	a7, a2, 10
     f64:	0b4242        	s8i	a4, a2, 11
     f67:	0c4272        	s8i	a7, a2, 12
     f6a:	0d4242        	s8i	a4, a2, 13
     f6d:	0e4272        	s8i	a7, a2, 14
     f70:	0f4242        	s8i	a4, a2, 15
     f73:	10c222        	addi	a2, a2, 16

00000f76 <cmn_proc_fix+0xfe>:
     f76:	d61c           	movi.n	a6, 29
     f78:	1cd932        	addmi	a3, a9, 0x1c00
     f7b:	1dd922        	addmi	a2, a9, 0x1d00
     f7e:	285d           	ae_l16si.n	a5, a8, 0
     f80:	22d942        	addmi	a4, a9, 0x2200
     f83:	882b           	addi.n	a8, a8, 2
     f85:	4ac442        	addi	a4, a4, 74
     f88:	a2c222        	addi	a2, a2, -94
     f8b:	3cc332        	addi	a3, a3, 60
     f8e:	6b5d           	ae_s16i.n	a5, a11, 0
     f90:	72c6a0        	salt	a12, a6, a10
     f93:	93a6c0        	movnez	a10, a6, a12
     f96:	02cb52        	addi	a5, a11, 2
     f99:	fc58c1        	l32r	a12, fc (7fff <mel_win_53+0x5f>)
     f9c:	90dad0        	addx2	a13, a10, a13
     f9f:	2ddd           	ae_l16si.n	a13, a13, 0
     fa1:	93ae76        	loopgtz	a14, 1038 <cmn_proc_fix+0x1c0>
     fa4:	28ad           	ae_l16si.n	a10, a8, 0
     fa6:	0044a2        	s8i	a10, a4, 0
     fa9:	41a8a0        	srli	a10, a10, 8
     fac:	0144a2        	s8i	a10, a4, 1
     faf:	442b           	addi.n	a4, a4, 2
     fb1:	0003e2        	l8ui	a14, a3, 0
     fb4:	010362        	l8ui	a6, a3, 1
     fb7:	030392        	l8ui	a9, a3, 3
     fba:	28ad           	ae_l16si.n	a10, a8, 0
     fbc:	020372        	l8ui	a7, a3, 2
     fbf:	882b           	addi.n	a8, a8, 2
     fc1:	119980        	slli	a9, a9, 8
     fc4:	209970        	or	a9, a9, a7
     fc7:	fec272        	addi	a7, a2, -2
     fca:	119980        	slli	a9, a9, 8
     fcd:	209960        	or	a9, a9, a6
     fd0:	119980        	slli	a9, a9, 8
     fd3:	2099e0        	or	a9, a9, a14
     fd6:	9a9a           	add.n	a9, a10, a9
     fd8:	004392        	s8i	a9, a3, 0
     fdb:	82ed90        	mull	a14, a13, a9
     fde:	416890        	srli	a6, a9, 8
     fe1:	014362        	s8i	a6, a3, 1
     fe4:	40dee2        	addmi	a14, a14, 0x4000
     fe7:	416860        	srli	a6, a6, 8
     fea:	024362        	s8i	a6, a3, 2
     fed:	21efe0        	srai	a14, a14, 15
     ff0:	aeed           	ae_clamps16	a14, a14
     ff2:	416860        	srli	a6, a6, 8
     ff5:	034362        	s8i	a6, a3, 3
     ff8:	c0aae0        	sub	a10, a10, a14
     ffb:	092ca7        	blt	a12, a10, 1008 <cmn_proc_fix+0x190>
     ffe:	09aaf7        	bge	a10, a15, 100b <cmn_proc_fix+0x193>
    1001:	fc3aa1        	l32r	a10, ec (ffff8000 <_end+0xfffeebcc>)
    1004:	0000c6        	j	100b <cmn_proc_fix+0x193>

00001007 <cmn_proc_fix+0x18f>:
	...

00001008 <cmn_proc_fix+0x190>:
    1008:	fc3da1        	l32r	a10, fc (7fff <mel_win_53+0x5f>)

0000100b <cmn_proc_fix+0x193>:
    100b:	09c5a4        	ae_s16ip	a10, a5, 2
    100e:	010762        	l8ui	a6, a7, 1
    1011:	000772        	l8ui	a7, a7, 0
    1014:	222b           	addi.n	a2, a2, 2
    1016:	116680        	slli	a6, a6, 8
    1019:	206670        	or	a6, a6, a7
    101c:	866d           	ae_sext16	a6, a6
    101e:	c06960        	sub	a6, a9, a6
    1021:	004362        	s8i	a6, a3, 0
    1024:	416860        	srli	a6, a6, 8
    1027:	014362        	s8i	a6, a3, 1
    102a:	416860        	srli	a6, a6, 8
    102d:	41e860        	srli	a14, a6, 8
    1030:	024362        	s8i	a6, a3, 2
    1033:	0343e2        	s8i	a14, a3, 3
    1036:	334b           	addi.n	a3, a3, 4

00001038 <cmn_proc_fix+0x1c0>:
    1038:	0128           	l32i.n	a2, a1, 0
    103a:	330232        	l8ui	a3, a2, 51
    103d:	320262        	l8ui	a6, a2, 50
    1040:	113380        	slli	a3, a3, 8
    1043:	203360        	or	a3, a3, a6
    1046:	833d           	ae_sext16	a3, a3
    1048:	004232        	s8i	a3, a2, 0
    104b:	413830        	srli	a3, a3, 8
    104e:	014232        	s8i	a3, a2, 1
    1051:	222b           	addi.n	a2, a2, 2
    1053:	6aa132        	movi	a3, 0x16a
    1056:	2f8376        	loop	a3, 1089 <cmn_proc_fix+0x211>
    1059:	330232        	l8ui	a3, a2, 51
    105c:	320242        	l8ui	a4, a2, 50
    105f:	350252        	l8ui	a5, a2, 53
    1062:	340262        	l8ui	a6, a2, 52
    1065:	113380        	slli	a3, a3, 8
    1068:	115580        	slli	a5, a5, 8
    106b:	203340        	or	a3, a3, a4
    106e:	204560        	or	a4, a5, a6
    1071:	833d           	ae_sext16	a3, a3
    1073:	844d           	ae_sext16	a4, a4
    1075:	004232        	s8i	a3, a2, 0
    1078:	413830        	srli	a3, a3, 8
    107b:	024242        	s8i	a4, a2, 2
    107e:	414840        	srli	a4, a4, 8
    1081:	014232        	s8i	a3, a2, 1
    1084:	034242        	s8i	a4, a2, 3
    1087:	224b           	addi.n	a2, a2, 4
    1089:	a31c           	movi.n	a3, 26
    108b:	078376        	loop	a3, 1096 <cmn_proc_fix+0x21e>
    108e:	2b3d           	ae_l16si.n	a3, a11, 0
    1090:	1133f0        	slli	a3, a3, 1
    1093:	09cb34        	ae_s16ip	a3, a11, 2
    1096:	f01d           	retw.n

00001098 <ini_st_fea>:
    1098:	004136        	entry	a1, 32
    109b:	c53c           	movi.n	a5, 60
    109d:	027d           	mov.n	a7, a2
    109f:	080c           	movi.n	a8, 0
    10a1:	628d           	ae_s16i.n	a8, a2, 0
    10a3:	1cd732        	addmi	a3, a7, 0x1c00
    10a6:	045282        	s16i	a8, a2, 8
    10a9:	1dd772        	addmi	a7, a7, 0x1d00
    10ac:	22ab           	addi.n	a2, a2, 10
    10ae:	a0c772        	addi	a7, a7, -96
    10b1:	328576        	loop	a5, 10e7 <ini_st_fea+0x4f>
    10b4:	004282        	s8i	a8, a2, 0
    10b7:	014282        	s8i	a8, a2, 1
    10ba:	024282        	s8i	a8, a2, 2
    10bd:	034282        	s8i	a8, a2, 3
    10c0:	044282        	s8i	a8, a2, 4
    10c3:	054282        	s8i	a8, a2, 5
    10c6:	064282        	s8i	a8, a2, 6
    10c9:	074282        	s8i	a8, a2, 7
    10cc:	084282        	s8i	a8, a2, 8
    10cf:	094282        	s8i	a8, a2, 9
    10d2:	0a4282        	s8i	a8, a2, 10
    10d5:	0b4282        	s8i	a8, a2, 11
    10d8:	0c4282        	s8i	a8, a2, 12
    10db:	0d4282        	s8i	a8, a2, 13
    10de:	0e4282        	s8i	a8, a2, 14
    10e1:	0f4282        	s8i	a8, a2, 15
    10e4:	10c222        	addi	a2, a2, 16

000010e7 <ini_st_fea+0x4f>:
    10e7:	06a0b2        	movi	a11, 6
    10ea:	03a022        	movi	a2, 3
    10ed:	3c4382        	s8i	a8, a3, 60
    10f0:	41a880        	srli	a10, a8, 8
    10f3:	4198a0        	srli	a9, a10, 8
    10f6:	3e4392        	s8i	a9, a3, 62
    10f9:	3d43a2        	s8i	a10, a3, 61
    10fc:	419890        	srli	a9, a9, 8
    10ff:	3f4392        	s8i	a9, a3, 63
    1102:	40c332        	addi	a3, a3, 64
    1105:	6b8276        	loop	a2, 1174 <ini_st_fea+0xdc>
    1108:	412880        	srli	a2, a8, 8
    110b:	004382        	s8i	a8, a3, 0
    110e:	044382        	s8i	a8, a3, 4
    1111:	084382        	s8i	a8, a3, 8
    1114:	0c4382        	s8i	a8, a3, 12
    1117:	104382        	s8i	a8, a3, 16
    111a:	144382        	s8i	a8, a3, 20
    111d:	184382        	s8i	a8, a3, 24
    1120:	1c4382        	s8i	a8, a3, 28
    1123:	014322        	s8i	a2, a3, 1
    1126:	414820        	srli	a4, a2, 8
    1129:	054322        	s8i	a2, a3, 5
    112c:	094322        	s8i	a2, a3, 9
    112f:	0d4322        	s8i	a2, a3, 13
    1132:	114322        	s8i	a2, a3, 17
    1135:	154322        	s8i	a2, a3, 21
    1138:	194322        	s8i	a2, a3, 25
    113b:	1d4322        	s8i	a2, a3, 29
    113e:	024342        	s8i	a4, a3, 2
    1141:	412840        	srli	a2, a4, 8
    1144:	064342        	s8i	a4, a3, 6
    1147:	0a4342        	s8i	a4, a3, 10
    114a:	0e4342        	s8i	a4, a3, 14
    114d:	124342        	s8i	a4, a3, 18
    1150:	164342        	s8i	a4, a3, 22
    1153:	1a4342        	s8i	a4, a3, 26
    1156:	1e4342        	s8i	a4, a3, 30
    1159:	034322        	s8i	a2, a3, 3
    115c:	074322        	s8i	a2, a3, 7
    115f:	0b4322        	s8i	a2, a3, 11
    1162:	0f4322        	s8i	a2, a3, 15
    1165:	134322        	s8i	a2, a3, 19
    1168:	174322        	s8i	a2, a3, 23
    116b:	1b4322        	s8i	a2, a3, 27
    116e:	1f4322        	s8i	a2, a3, 31
    1171:	20c332        	addi	a3, a3, 32
    1174:	414880        	srli	a4, a8, 8
    1177:	203880        	or	a3, a8, a8
    117a:	5da052        	movi	a5, 93
    117d:	07ab76        	loopgtz	a11, 1188 <ini_st_fea+0xf0>
    1180:	004782        	s8i	a8, a7, 0
    1183:	014742        	s8i	a4, a7, 1
    1186:	772b           	addi.n	a7, a7, 2
    1188:	328576        	loop	a5, 11be <ini_st_fea+0x126>
    118b:	004732        	s8i	a3, a7, 0
    118e:	014742        	s8i	a4, a7, 1
    1191:	024732        	s8i	a3, a7, 2
    1194:	034742        	s8i	a4, a7, 3
    1197:	044732        	s8i	a3, a7, 4
    119a:	054742        	s8i	a4, a7, 5
    119d:	064732        	s8i	a3, a7, 6
    11a0:	074742        	s8i	a4, a7, 7
    11a3:	084732        	s8i	a3, a7, 8
    11a6:	094742        	s8i	a4, a7, 9
    11a9:	0a4732        	s8i	a3, a7, 10
    11ac:	0b4742        	s8i	a4, a7, 11
    11af:	0c4732        	s8i	a3, a7, 12
    11b2:	0d4742        	s8i	a4, a7, 13
    11b5:	0e4732        	s8i	a3, a7, 14
    11b8:	0f4742        	s8i	a4, a7, 15
    11bb:	10c772        	addi	a7, a7, 16
    11be:	f01d           	retw.n

000011c0 <get_fea_size>:
    11c0:	004136        	entry	a1, 32
    11c3:	fbcf21        	l32r	a2, 100 (2298 <_DYNAMIC+0x280>)
    11c6:	f01d           	retw.n

000011c8 <FEATS_Prcs>:
    11c8:	00e136        	entry	a1, 112
    11cb:	02cd           	mov.n	a12, a2
    11cd:	00a082        	movi	a8, 0
    11d0:	f0a092        	movi	a9, 240
    11d3:	22d452        	addmi	a5, a4, 0x2200
    11d6:	409552        	l16si	a5, a5, 128
    11d9:	1a8976        	loop	a9, 11f7 <FEATS_Prcs+0x2f>
    11dc:	9098c0        	addx2	a9, a8, a12
    11df:	90a840        	addx2	a10, a8, a4
    11e2:	881b           	addi.n	a8, a8, 1
    11e4:	299d           	ae_l16si.n	a9, a9, 0
    11e6:	02daa2        	addmi	a10, a10, 0x200
    11e9:	888d           	ae_sext16	a8, a8
    11eb:	eacaa2        	addi	a10, a10, -22
    11ee:	41b890        	srli	a11, a9, 8
    11f1:	004a92        	s8i	a9, a10, 0
    11f4:	014ab2        	s8i	a11, a10, 1

000011f7 <FEATS_Prcs+0x2f>:
    11f7:	a4ab           	addi.n	a10, a4, 10
    11f9:	01bd           	mov.n	a11, a1
    11fb:	fbc281        	l32r	a8, 104 (9f4 <mfcc_frame>)
    11fe:	04cd           	mov.n	a12, a4
    1200:	40c1d2        	addi	a13, a1, 64
    1203:	0008e0        	callx8	a8
    1206:	03bd           	mov.n	a11, a3
    1208:	01ad           	mov.n	a10, a1
    120a:	fbbf81        	l32r	a8, 108 (e78 <cmn_proc_fix>)
    120d:	04cd           	mov.n	a12, a4
    120f:	0494d2        	l16si	a13, a4, 8
    1212:	0008e0        	callx8	a8
    1215:	080c           	movi.n	a8, 0
    1217:	f0a092        	movi	a9, 240
    121a:	258976        	loop	a9, 1243 <FEATS_Prcs+0x7b>
    121d:	909840        	addx2	a9, a8, a4
    1220:	881b           	addi.n	a8, a8, 1
    1222:	a9ab           	addi.n	a10, a9, 10
    1224:	02daa2        	addmi	a10, a10, 0x200
    1227:	e0caa2        	addi	a10, a10, -32
    122a:	010ab2        	l8ui	a11, a10, 1
    122d:	000aa2        	l8ui	a10, a10, 0
    1230:	888d           	ae_sext16	a8, a8
    1232:	11bb80        	slli	a11, a11, 8
    1235:	20aba0        	or	a10, a11, a10
    1238:	8aad           	ae_sext16	a10, a10
    123a:	0a49a2        	s8i	a10, a9, 10
    123d:	41a8a0        	srli	a10, a10, 8
    1240:	0b49a2        	s8i	a10, a9, 11
    1243:	0494a2        	l16si	a10, a4, 8
    1246:	0b1566        	bnei	a5, 1, 1255 <FEATS_Prcs+0x8d>
    1249:	d91c           	movi.n	a9, 29
    124b:	0d29a7        	blt	a9, a10, 125c <FEATS_Prcs+0x94>
    124e:	ba1b           	addi.n	a11, a10, 1
    1250:	0454b2        	s16i	a11, a4, 8
    1253:	f01d           	retw.n

00001255 <FEATS_Prcs+0x8d>:
    1255:	ca1b           	addi.n	a12, a10, 1
    1257:	0454c2        	s16i	a12, a4, 8
    125a:	f01d           	retw.n

0000125c <FEATS_Prcs+0x94>:
    125c:	f01d           	retw.n

0000125e <FEATS_Prcs+0x96>:
	...

00001260 <post_proc>:
    1260:	00a136        	entry	a1, 80
    1263:	23bd           	ae_l16si.n	a11, a3, 0
    1265:	13c8           	l32i.n	a12, a3, 4
    1267:	02ad           	mov.n	a10, a2
    1269:	41f8a0        	srli	a15, a10, 8
    126c:	22d322        	addmi	a2, a3, 0x2200
    126f:	409272        	l16si	a7, a2, 128
    1272:	242252        	l32i	a5, a2, 144
    1275:	232262        	l32i	a6, a2, 140
    1278:	419292        	l16si	a9, a2, 130
    127b:	429282        	l16si	a8, a2, 132
    127e:	6189           	s32i.n	a8, a1, 24
    1280:	7199           	s32i.n	a9, a1, 28
    1282:	4169           	s32i.n	a6, a1, 16
    1284:	3159           	s32i.n	a5, a1, 12
    1286:	449262        	l16si	a6, a2, 136
    1289:	439252        	l16si	a5, a2, 134
    128c:	4198c0        	srli	a9, a12, 8
    128f:	459222        	l16si	a2, a2, 138
    1292:	032b57        	blt	a11, a5, 1299 <post_proc+0x39>
    1295:	b50b           	addi.n	a11, a5, -1
    1297:	8bbd           	ae_sext16	a11, a11

00001299 <post_proc+0x39>:
    1299:	908540        	addx2	a8, a5, a4
    129c:	fec882        	addi	a8, a8, -2
    129f:	0148f2        	s8i	a15, a8, 1
    12a2:	0048a2        	s8i	a10, a8, 0
    12a5:	fb94f1        	l32r	a15, f8 (8940 <div_const_58>)
    12a8:	f4e8           	l32i.n	a14, a4, 60
    12aa:	a08640        	addx4	a8, a6, a4
    12ad:	90fbf0        	addx2	a15, a11, a15
    12b0:	eaea           	add.n	a14, a10, a14
    12b2:	2ffd           	ae_l16si.n	a15, a15, 0
    12b4:	41a890        	srli	a10, a9, 8
    12b7:	f4e9           	s32i.n	a14, a4, 60
    12b9:	3c48c2        	s8i	a12, a8, 60
    12bc:	3d4892        	s8i	a9, a8, 61
    12bf:	41d8a0        	srli	a13, a10, 8
    12c2:	3e48a2        	s8i	a10, a8, 62
    12c5:	3f48d2        	s8i	a13, a8, 63
    12c8:	f4d8           	l32i.n	a13, a4, 60
    12ca:	2e24a2        	l32i	a10, a4, 184
    12cd:	404f00        	ssai	15
    12d0:	82efd0        	mull	a14, a15, a13
    12d3:	aaca           	add.n	a10, a10, a12
    12d5:	2e64a2        	s32i	a10, a4, 184
    12d8:	82baf0        	mull	a11, a10, a15
    12db:	40dee2        	addmi	a14, a14, 0x4000
    12de:	b2faf0        	mulsh	a15, a10, a15
    12e1:	21efe0        	srai	a14, a14, 15
    12e4:	40dbc2        	addmi	a12, a11, 0x4000
    12e7:	62bcb0        	saltu	a11, a12, a11
    12ea:	aeed           	ae_clamps16	a14, a14
    12ec:	81e9           	s32i.n	a14, a1, 32
    12ee:	bbfa           	add.n	a11, a11, a15
    12f0:	5e54e2        	s16i	a14, a4, 188
    12f3:	fb76f1        	l32r	a15, cc (80000000 <_end+0x7fff6bcc>)
    12f6:	81cbc0        	src	a12, a11, a12
    12f9:	fb56e1        	l32r	a14, 54 (7fffffff <_end+0x7fff6bcb>)
    12fc:	21bfb0        	srai	a11, a11, 15
    12ff:	21e9           	s32i.n	a14, a1, 8
    1301:	0e0c           	movi.n	a14, 0
    1303:	021be6        	bgei	a11, 1, 1309 <post_proc+0xa9>
    1306:	005206        	j	1452 <post_proc+0x1f2>

00001309 <post_proc+0xa9>:
    1309:	153e16        	beqz	a14, 1460 <post_proc+0x200>

0000130c <post_proc+0xac>:
    130c:	f97c           	movi.n	a9, -1
    130e:	0ced           	mov.n	a14, a12
    1310:	006bd6        	bgez	a11, 131a <post_proc+0xba>
    1313:	8b1b           	addi.n	a8, a11, 1
    1315:	18c816        	beqz	a8, 14a5 <post_proc+0x245>

00001318 <post_proc+0xb8>:
    1318:	0fed           	mov.n	a14, a15

0000131a <post_proc+0xba>:
    131a:	319fe0        	srai	a9, a14, 31

0000131d <post_proc+0xbd>:
    131d:	316492        	s32i	a9, a4, 196
    1320:	3064e2        	s32i	a14, a4, 192
    1323:	870b           	addi.n	a8, a7, -1
    1325:	13f816        	beqz	a8, 1468 <post_proc+0x208>
    1328:	239d           	ae_l16si.n	a9, a3, 0
    132a:	fb4ae1        	l32r	a14, 54 (7fffffff <_end+0x7fff6bcb>)
    132d:	3d9927        	bne	a9, a2, 136e <post_proc+0x10e>
    1330:	8128           	l32i.n	a2, a1, 32
    1332:	0a0c           	movi.n	a10, 0
    1334:	822d           	ae_sext16	a2, a2
    1336:	021be6        	bgei	a11, 1, 133c <post_proc+0xdc>
    1339:	006246        	j	14c6 <post_proc+0x266>

0000133c <post_proc+0xdc>:
    133c:	150a16        	beqz	a10, 1490 <post_proc+0x230>
    133f:	71a8           	l32i.n	a10, a1, 28
    1341:	006bd6        	bgez	a11, 134b <post_proc+0xeb>
    1344:	9b1b           	addi.n	a9, a11, 1
    1346:	193916        	beqz	a9, 14dd <post_proc+0x27d>

00001349 <post_proc+0xe9>:
    1349:	0fcd           	mov.n	a12, a15

0000134b <post_proc+0xeb>:
    134b:	02a2a7        	bge	a2, a10, 1351 <post_proc+0xf1>
    134e:	003986        	j	1438 <post_proc+0x1d8>

00001351 <post_proc+0xf1>:
    1351:	120c           	movi.n	a2, 1

00001353 <post_proc+0xf3>:
    1353:	fb4f81        	l32r	a8, 90 (5e0 <RNN_Init_fx>)
    1356:	645422        	s16i	a2, a4, 200
    1359:	04ad           	mov.n	a10, a4
    135b:	0008e0        	callx8	a8
    135e:	0a0c           	movi.n	a10, 0
    1360:	f4d8           	l32i.n	a13, a4, 60
    1362:	090c           	movi.n	a9, 0
    1364:	639d           	ae_s16i.n	a9, a3, 0
    1366:	2e6492        	s32i	a9, a4, 184
    1369:	0000c6        	j	1370 <post_proc+0x110>

0000136c <post_proc+0x10c>:
	...

0000136e <post_proc+0x10e>:
    136e:	f27c           	movi.n	a2, -1

00001370 <post_proc+0x110>:
    1370:	0104b2        	l8ui	a11, a4, 1
    1373:	0004c2        	l8ui	a12, a4, 0
    1376:	080c           	movi.n	a8, 0
    1378:	11bb80        	slli	a11, a11, 8
    137b:	20bbc0        	or	a11, a11, a12
    137e:	8bbd           	ae_sext16	a11, a11
    1380:	c0bdb0        	sub	a11, a13, a11
    1383:	f4b9           	s32i.n	a11, a4, 60
    1385:	2525a6        	blti	a5, 2, 13ae <post_proc+0x14e>
    1388:	950b           	addi.n	a9, a5, -1
    138a:	1da976        	loopgtz	a9, 13ab <post_proc+0x14b>
    138d:	909840        	addx2	a9, a8, a4
    1390:	881b           	addi.n	a8, a8, 1
    1392:	0309a2        	l8ui	a10, a9, 3
    1395:	0209b2        	l8ui	a11, a9, 2
    1398:	888d           	ae_sext16	a8, a8
    139a:	11aa80        	slli	a10, a10, 8
    139d:	20aab0        	or	a10, a10, a11
    13a0:	8aad           	ae_sext16	a10, a10
    13a2:	0049a2        	s8i	a10, a9, 0
    13a5:	41a8a0        	srli	a10, a10, 8
    13a8:	0149a2        	s8i	a10, a9, 1

000013ab <post_proc+0x14b>:
    13ab:	2e24a2        	l32i	a10, a4, 184

000013ae <post_proc+0x14e>:
    13ae:	080c           	movi.n	a8, 0
    13b0:	960b           	addi.n	a9, a6, -1
    13b2:	4304c2        	l8ui	a12, a4, 67
    13b5:	4204d2        	l8ui	a13, a4, 66
    13b8:	4104e2        	l8ui	a14, a4, 65
    13bb:	11cc80        	slli	a12, a12, 8
    13be:	20ccd0        	or	a12, a12, a13
    13c1:	11cc80        	slli	a12, a12, 8
    13c4:	4004d2        	l8ui	a13, a4, 64
    13c7:	20cce0        	or	a12, a12, a14
    13ca:	20e440        	or	a14, a4, a4
    13cd:	11cc80        	slli	a12, a12, 8
    13d0:	20ccd0        	or	a12, a12, a13
    13d3:	c0cac0        	sub	a12, a10, a12
    13d6:	2e64c2        	s32i	a12, a4, 184
    13d9:	39a976        	loopgtz	a9, 1416 <post_proc+0x1b6>
    13dc:	a098e0        	addx4	a9, a8, a14
    13df:	881b           	addi.n	a8, a8, 1
    13e1:	4709a2        	l8ui	a10, a9, 71
    13e4:	4609b2        	l8ui	a11, a9, 70
    13e7:	4509c2        	l8ui	a12, a9, 69
    13ea:	4409d2        	l8ui	a13, a9, 68
    13ed:	888d           	ae_sext16	a8, a8
    13ef:	11aa80        	slli	a10, a10, 8
    13f2:	20aab0        	or	a10, a10, a11
    13f5:	11aa80        	slli	a10, a10, 8
    13f8:	20aac0        	or	a10, a10, a12
    13fb:	11aa80        	slli	a10, a10, 8
    13fe:	20aad0        	or	a10, a10, a13
    1401:	4049a2        	s8i	a10, a9, 64
    1404:	41a8a0        	srli	a10, a10, 8
    1407:	4149a2        	s8i	a10, a9, 65
    140a:	41a8a0        	srli	a10, a10, 8
    140d:	4249a2        	s8i	a10, a9, 66
    1410:	41a8a0        	srli	a10, a10, 8
    1413:	4349a2        	s8i	a10, a9, 67

00001416 <post_proc+0x1b6>:
    1416:	23ad           	ae_l16si.n	a10, a3, 0
    1418:	0d1726        	beqi	a7, 1, 1429 <post_proc+0x1c9>
    141b:	da1b           	addi.n	a13, a10, 1
    141d:	63dd           	ae_s16i.n	a13, a3, 0
    141f:	0b0226        	beqi	a2, -1, 142e <post_proc+0x1ce>
    1422:	0e0c           	movi.n	a14, 0
    1424:	0453e2        	s16i	a14, a3, 8
    1427:	f01d           	retw.n

00001429 <post_proc+0x1c9>:
    1429:	ff4c           	movi.n	a15, 79
    142b:	01afa7        	bge	a15, a10, 1430 <post_proc+0x1d0>
    142e:	f01d           	retw.n

00001430 <post_proc+0x1d0>:
    1430:	2a1b           	addi.n	a2, a10, 1
    1432:	632d           	ae_s16i.n	a2, a3, 0
    1434:	f27c           	movi.n	a2, -1
    1436:	f01d           	retw.n

00001438 <post_proc+0x1d8>:
    1438:	6188           	l32i.n	a8, a1, 24
    143a:	622287        	blt	a2, a8, 14a0 <post_proc+0x240>
    143d:	4198           	l32i.n	a9, a1, 16
    143f:	022c97        	blt	a12, a9, 1445 <post_proc+0x1e5>
    1442:	ffc2c6        	j	1351 <post_proc+0xf1>
    1445:	3128           	l32i.n	a2, a1, 12
    1447:	180c           	movi.n	a8, 1
    1449:	722c20        	salt	a2, a12, a2
    144c:	302280        	xor	a2, a2, a8
    144f:	ffc006        	j	1353 <post_proc+0xf3>

00001452 <post_proc+0x1f2>:
    1452:	eb6b56        	bnez	a11, 130c <post_proc+0xac>
    1455:	2188           	l32i.n	a8, a1, 8
    1457:	02b8c7        	bgeu	a8, a12, 145d <post_proc+0x1fd>
    145a:	ffaac6        	j	1309 <post_proc+0xa9>
    145d:	0021c6        	j	14e8 <post_proc+0x288>

00001460 <post_proc+0x200>:
    1460:	090c           	movi.n	a9, 0
    1462:	fafce1        	l32r	a14, 54 (7fffffff <_end+0x7fff6bcb>)
    1465:	ffad06        	j	131d <post_proc+0xbd>

00001468 <post_proc+0x208>:
    1468:	0e0c           	movi.n	a14, 0
    146a:	fafa81        	l32r	a8, 54 (7fffffff <_end+0x7fff6bcb>)
    146d:	8128           	l32i.n	a2, a1, 32
    146f:	0189           	s32i.n	a8, a1, 0
    1471:	080c           	movi.n	a8, 0
    1473:	822d           	ae_sext16	a2, a2
    1475:	721ba6        	blti	a11, 1, 14eb <post_proc+0x28b>

00001478 <post_proc+0x218>:
    1478:	052e16        	beqz	a14, 14ce <post_proc+0x26e>
    147b:	004bd6        	bgez	a11, 1483 <post_proc+0x223>
    147e:	700b26        	beqi	a11, -1, 14f2 <post_proc+0x292>

00001481 <post_proc+0x221>:
    1481:	0fcd           	mov.n	a12, a15

00001483 <post_proc+0x223>:
    1483:	71b8           	l32i.n	a11, a1, 28
    1485:	2522b7        	blt	a2, a11, 14ae <post_proc+0x24e>

00001488 <post_proc+0x228>:
    1488:	120c           	movi.n	a2, 1

0000148a <post_proc+0x22a>:
    148a:	645422        	s16i	a2, a4, 200
    148d:	ffb7c6        	j	1370 <post_proc+0x110>

00001490 <post_proc+0x230>:
    1490:	71c8           	l32i.n	a12, a1, 28
    1492:	0222c7        	blt	a2, a12, 1498 <post_proc+0x238>
    1495:	ffae06        	j	1351 <post_proc+0xf1>
    1498:	61d8           	l32i.n	a13, a1, 24
    149a:	0222d7        	blt	a2, a13, 14a0 <post_proc+0x240>
    149d:	ffac06        	j	1351 <post_proc+0xf1>
    14a0:	020c           	movi.n	a2, 0
    14a2:	ffab46        	j	1353 <post_proc+0xf3>

000014a5 <post_proc+0x245>:
    14a5:	02bfc7        	bgeu	a15, a12, 14ab <post_proc+0x24b>
    14a8:	ff9b86        	j	131a <post_proc+0xba>
    14ab:	ff9a46        	j	1318 <post_proc+0xb8>

000014ae <post_proc+0x24e>:
    14ae:	61e8           	l32i.n	a14, a1, 24
    14b0:	2422e7        	blt	a2, a14, 14d8 <post_proc+0x278>
    14b3:	41f8           	l32i.n	a15, a1, 16
    14b5:	cfacf7        	bge	a12, a15, 1488 <post_proc+0x228>
    14b8:	3128           	l32i.n	a2, a1, 12
    14ba:	180c           	movi.n	a8, 1
    14bc:	722c20        	salt	a2, a12, a2
    14bf:	302280        	xor	a2, a2, a8
    14c2:	fff106        	j	148a <post_proc+0x22a>

000014c5 <post_proc+0x265>:
	...

000014c6 <post_proc+0x266>:
    14c6:	ebac           	beqz.n	a11, 14f8 <post_proc+0x298>

000014c8 <post_proc+0x268>:
    14c8:	1a0c           	movi.n	a10, 1
    14ca:	ff9b86        	j	133c <post_proc+0xdc>

000014cd <post_proc+0x26d>:
	...

000014ce <post_proc+0x26e>:
    14ce:	7188           	l32i.n	a8, a1, 28
    14d0:	b4a287        	bge	a2, a8, 1488 <post_proc+0x228>
    14d3:	6198           	l32i.n	a9, a1, 24
    14d5:	afa297        	bge	a2, a9, 1488 <post_proc+0x228>
    14d8:	020c           	movi.n	a2, 0
    14da:	ffeb06        	j	148a <post_proc+0x22a>

000014dd <post_proc+0x27d>:
    14dd:	02bfc7        	bgeu	a15, a12, 14e3 <post_proc+0x283>
    14e0:	ff99c6        	j	134b <post_proc+0xeb>
    14e3:	ff9886        	j	1349 <post_proc+0xe9>

000014e6 <post_proc+0x286>:
	...

000014e8 <post_proc+0x288>:
    14e8:	ff8806        	j	130c <post_proc+0xac>

000014eb <post_proc+0x28b>:
    14eb:	2b9c           	beqz.n	a11, 1501 <post_proc+0x2a1>

000014ed <post_proc+0x28d>:
    14ed:	1e0c           	movi.n	a14, 1
    14ef:	ffe146        	j	1478 <post_proc+0x218>

000014f2 <post_proc+0x292>:
    14f2:	8d3fc7        	bltu	a15, a12, 1483 <post_proc+0x223>
    14f5:	ffe206        	j	1481 <post_proc+0x221>

000014f8 <post_proc+0x298>:
    14f8:	02bec7        	bgeu	a14, a12, 14fe <post_proc+0x29e>
    14fb:	ff8f46        	j	133c <post_proc+0xdc>
    14fe:	fff186        	j	14c8 <post_proc+0x268>

00001501 <post_proc+0x2a1>:
    1501:	0188           	l32i.n	a8, a1, 0
    1503:	02b8c7        	bgeu	a8, a12, 1509 <post_proc+0x2a9>
    1506:	ffdb86        	j	1478 <post_proc+0x218>
    1509:	fff806        	j	14ed <post_proc+0x28d>

0000150c <post_proc+0x2ac>:
    150c:	00000000                                ....

00001510 <real_512_fft>:
    1510:	004136        	entry	a1, 32
    1513:	02ed           	mov.n	a14, a2
    1515:	fafd81        	l32r	a8, 10c (8980 <fftorder_512>)
    1518:	06d3b2        	addmi	a11, a3, 0x600
    151b:	80a092        	movi	a9, 128
    151e:	368976        	loop	a9, 1558 <real_512_fft+0x48>
    1521:	289d           	ae_l16si.n	a9, a8, 0
    1523:	38cd           	ae_l16si.n	a12, a8, 2
    1525:	10cbb2        	addi	a11, a11, 16
    1528:	a91b           	addi.n	a10, a9, 1
    152a:	dc1b           	addi.n	a13, a12, 1
    152c:	884b           	addi.n	a8, a8, 4
    152e:	b3a990        	movgez	a10, a9, a9
    1531:	b3dcc0        	movgez	a13, a12, a12
    1534:	2191a0        	srai	a9, a10, 1
    1537:	21c1d0        	srai	a12, a13, 1
    153a:	899d           	ae_sext16	a9, a9
    153c:	8ccd           	ae_sext16	a12, a12
    153e:	b099e0        	addx8	a9, a9, a14
    1541:	b0cce0        	addx8	a12, a12, a14
    1544:	09a8           	l32i.n	a10, a9, 0
    1546:	7c6ba2        	s32i	a10, a11, 0x1f0
    1549:	1998           	l32i.n	a9, a9, 4
    154b:	7d6b92        	s32i	a9, a11, 0x1f4
    154e:	0c98           	l32i.n	a9, a12, 0
    1550:	7e6b92        	s32i	a9, a11, 0x1f8
    1553:	1c98           	l32i.n	a9, a12, 4
    1555:	7f6b92        	s32i	a9, a11, 0x1fc

00001558 <real_512_fft+0x48>:
    1558:	08d3a2        	addmi	a10, a3, 0x800
    155b:	00a1b2        	movi	a11, 0x100
    155e:	8c0c           	movi.n	a12, 8
    1560:	faec81        	l32r	a8, 110 (1e18 <sw_fft_32b_2811>)
    1563:	1d0c           	movi.n	a13, 1
    1565:	0e0c           	movi.n	a14, 0
    1567:	0008e0        	callx8	a8
    156a:	0a2d           	mov.n	a2, a10
    156c:	f01d           	retw.n

0000156e <real_512_fft+0x5e>:
	...

00001570 <xtensa_fft4_32b>:
    1570:	006136        	entry	a1, 48
    1573:	60c274        	ae_lq32f.i	aeq3, a2, 28
    1576:	608234        	ae_lq32f.i	aeq2, a2, 12
    1579:	600214        	ae_lq32f.i	aeq0, a2, 4
    157c:	f8e23d225f 	{ ae_lq32f.i	aeq1, a2, 20; ae_addq56	aeq0, aeq0, aeq2 }
    1581:	f9ee3d621f 	{ ae_lq32f.i	aeq3, a2, 4; ae_addq56	aeq1, aeq1, aeq3 }
    1586:	fa663a310f 	{ ae_sq56s.i	aeq3, a1, 0; ae_addq56	aeq2, aeq0, aeq1 }
    158b:	f826fd226f 	{ ae_lq32f.i	aeq1, a2, 24; ae_subq56	aeq0, aeq0, aeq1 }
    1590:	622214        	ae_sq32f.i	aeq2, a2, 4
    1593:	60c254        	ae_lq32f.i	aeq3, a2, 20
    1596:	620254        	ae_sq32f.i	aeq0, a2, 20
    1599:	608244        	ae_lq32f.i	aeq2, a2, 16
    159c:	600224        	ae_lq32f.i	aeq0, a2, 8
    159f:	f9ea3d420f 	{ ae_lq32f.i	aeq2, a2, 0; ae_addq56	aeq1, aeq1, aeq2 }
    15a4:	01a8c4        	ae_addq56	aeq2, aeq2, aeq0
    15a7:	0129c4        	ae_addq56	aeq0, aeq2, aeq1
    15aa:	fa36fd220f 	{ ae_lq32f.i	aeq1, a2, 0; ae_subq56	aeq2, aeq2, aeq1 }
    15af:	620204        	ae_sq32f.i	aeq0, a2, 0
    15b2:	600274        	ae_lq32f.i	aeq0, a2, 28
    15b5:	fb3afd022f 	{ ae_lq32f.i	aeq0, a2, 8; ae_subq56	aeq3, aeq3, aeq0 }
    15ba:	f92afd024f 	{ ae_lq32f.i	aeq0, a2, 16; ae_subq56	aeq1, aeq1, aeq0 }
    15bf:	faaef9a24f 	{ ae_sq32f.i	aeq2, a2, 16; ae_subq56	aeq2, aeq1, aeq3 }
    15c4:	f9ee3d626f 	{ ae_lq32f.i	aeq3, a2, 24; ae_addq56	aeq1, aeq1, aeq3 }
    15c9:	fb3afd023f 	{ ae_lq32f.i	aeq0, a2, 12; ae_subq56	aeq3, aeq3, aeq0 }
    15ce:	621224        	ae_sq32f.i	aeq1, a2, 8
    15d1:	614104        	ae_lq56.i	aeq1, a1, 0
    15d4:	015cc4        	ae_subq56	aeq1, aeq1, aeq0
    15d7:	f8ee39a26f 	{ ae_sq32f.i	aeq2, a2, 24; ae_addq56	aeq0, aeq1, aeq3 }
    15dc:	f9aef9823f 	{ ae_sq32f.i	aeq0, a2, 12; ae_subq56	aeq1, aeq1, aeq3 }
    15e1:	621274        	ae_sq32f.i	aeq1, a2, 28
    15e4:	f01d           	retw.n

000015e6 <xtensa_fft4_32b+0x76>:
	...

000015e8 <xtensa_fft8_32b>:
    15e8:	010136        	entry	a1, 128
    15eb:	604244        	ae_lq32f.i	aeq1, a2, 16
    15ee:	600264        	ae_lq32f.i	aeq0, a2, 24
    15f1:	fac8f1        	l32r	a15, 114 (5a82 <noise_gru_weights_41+0x22b2>)
    15f4:	fa26c781ce 	{ addi	a12, a1, 120; ae_subq56	aeq2, aeq0, aeq1 }
    15f9:	632154        	ae_sq56s.i	aeq2, a1, 40
    15fc:	f8663d220f 	{ ae_lq32f.i	aeq1, a2, 0; ae_addq56	aeq0, aeq0, aeq1 }
    1601:	608224        	ae_lq32f.i	aeq2, a2, 8
    1604:	fbaafa0ccf 	{ ae_sq56s.i	aeq0, a12, -32; ae_subq56	aeq3, aeq1, aeq2 }
    1609:	f9ea3d425f 	{ ae_lq32f.i	aeq2, a2, 20; ae_addq56	aeq1, aeq1, aeq2 }
    160e:	633164        	ae_sq56s.i	aeq3, a1, 48
    1611:	631c94        	ae_sq56s.i	aeq1, a12, -56
    1614:	60c274        	ae_lq32f.i	aeq3, a2, 28
    1617:	f8b6fd221f 	{ ae_lq32f.i	aeq1, a2, 4; ae_subq56	aeq0, aeq2, aeq3 }
    161c:	faf63d623f 	{ ae_lq32f.i	aeq3, a2, 12; ae_addq56	aeq2, aeq2, aeq3 }
    1621:	f8aefa017f 	{ ae_sq56s.i	aeq0, a1, 56; ae_subq56	aeq0, aeq1, aeq3 }
    1626:	f9ee0202de 	{ addi	a13, a2, 32; ae_addq56	aeq1, aeq1, aeq3 }
    162b:	fbaafa0caf 	{ ae_sq56s.i	aeq0, a12, -48; ae_subq56	aeq3, aeq1, aeq2 }
    1630:	623254        	ae_sq32f.i	aeq3, a2, 20
    1633:	f9ea3dcc9f 	{ ae_lq56.i	aeq2, a12, -56; ae_addq56	aeq1, aeq1, aeq2 }
    1638:	61ccc4        	ae_lq56.i	aeq3, a12, -32
    163b:	f8f639921f 	{ ae_sq32f.i	aeq1, a2, 4; ae_addq56	aeq0, aeq2, aeq3 }
    1640:	620204        	ae_sq32f.i	aeq0, a2, 0
    1643:	fab6fde16f 	{ ae_lq56.i	aeq3, a1, 48; ae_subq56	aeq2, aeq2, aeq3 }
    1648:	610174        	ae_lq56.i	aeq0, a1, 56
    164b:	f93af9a24f 	{ ae_sq32f.i	aeq2, a2, 16; ae_subq56	aeq1, aeq3, aeq0 }
    1650:	fb7a3d8caf 	{ ae_lq56.i	aeq0, a12, -48; ae_addq56	aeq3, aeq3, aeq0 }
    1655:	621264        	ae_sq32f.i	aeq1, a2, 24
    1658:	614154        	ae_lq56.i	aeq1, a1, 40
    165b:	fa6639b22f 	{ ae_sq32f.i	aeq3, a2, 8; ae_addq56	aeq2, aeq0, aeq1 }
    1660:	f826f9a23f 	{ ae_sq32f.i	aeq2, a2, 12; ae_subq56	aeq0, aeq0, aeq1 }
    1665:	620274        	ae_sq32f.i	aeq0, a2, 28
    1668:	60cd74        	ae_lq32f.i	aeq3, a13, 28
    166b:	604d54        	ae_lq32f.i	aeq1, a13, 20
    166e:	faee3d0d3f 	{ ae_lq32f.i	aeq0, a13, 12; ae_addq56	aeq2, aeq1, aeq3 }
    1673:	622d54        	ae_sq32f.i	aeq2, a13, 20
    1676:	f9aefd4d1f 	{ ae_lq32f.i	aeq2, a13, 4; ae_subq56	aeq1, aeq1, aeq3 }
    167b:	fb72399d7f 	{ ae_sq32f.i	aeq1, a13, 28; ae_addq56	aeq3, aeq2, aeq0 }
    1680:	623d14        	ae_sq32f.i	aeq3, a13, 4
    1683:	fa32fd0d2f 	{ ae_lq32f.i	aeq0, a13, 8; ae_subq56	aeq2, aeq2, aeq0 }
    1688:	60cd04        	ae_lq32f.i	aeq3, a13, 0
    168b:	622d34        	ae_sq32f.i	aeq2, a13, 12
    168e:	f93afd4d4f 	{ ae_lq32f.i	aeq2, a13, 16; ae_subq56	aeq1, aeq3, aeq0 }
    1693:	621d24        	ae_sq32f.i	aeq1, a13, 8
    1696:	fb7a3d0d6f 	{ ae_lq32f.i	aeq0, a13, 24; ae_addq56	aeq3, aeq3, aeq0 }
    169b:	f932f9bd0f 	{ ae_sq32f.i	aeq3, a13, 0; ae_subq56	aeq1, aeq2, aeq0 }
    16a0:	621d64        	ae_sq32f.i	aeq1, a13, 24
    16a3:	fa723d2d0f 	{ ae_lq32f.i	aeq1, a13, 0; ae_addq56	aeq2, aeq2, aeq0 }
    16a8:	622d44        	ae_sq32f.i	aeq2, a13, 16
    16ab:	60cd44        	ae_lq32f.i	aeq3, a13, 16
    16ae:	011bc4        	ae_addq56	aeq0, aeq1, aeq3
    16b1:	fb3efd420f 	{ ae_lq32f.i	aeq2, a2, 0; ae_subq56	aeq3, aeq3, aeq1 }
    16b6:	633134        	ae_sq56s.i	aeq3, a1, 24
    16b9:	f932fd621f 	{ ae_lq32f.i	aeq3, a2, 4; ae_subq56	aeq1, aeq2, aeq0 }
    16be:	fa72399d0f 	{ ae_sq32f.i	aeq1, a13, 0; ae_addq56	aeq2, aeq2, aeq0 }
    16c3:	632144        	ae_sq56s.i	aeq2, a1, 32
    16c6:	604d54        	ae_lq32f.i	aeq1, a13, 20
    16c9:	608d14        	ae_lq32f.i	aeq2, a13, 4
    16cc:	0129c4        	ae_addq56	aeq0, aeq2, aeq1
    16cf:	01adc4        	ae_subq56	aeq2, aeq2, aeq1
    16d2:	017cc4        	ae_subq56	aeq1, aeq3, aeq0
    16d5:	fb7a3d814f 	{ ae_lq56.i	aeq0, a1, 32; ae_addq56	aeq3, aeq3, aeq0 }
    16da:	621d14        	ae_sq32f.i	aeq1, a13, 4
    16dd:	623214        	ae_sq32f.i	aeq3, a2, 4
    16e0:	604244        	ae_lq32f.i	aeq1, a2, 16
    16e3:	f8ea39820f 	{ ae_sq32f.i	aeq0, a2, 0; ae_addq56	aeq0, aeq1, aeq2 }
    16e8:	f9aafd625f 	{ ae_lq32f.i	aeq3, a2, 20; ae_subq56	aeq1, aeq1, aeq2 }
    16ed:	621d44        	ae_sq32f.i	aeq1, a13, 16
    16f0:	614134        	ae_lq56.i	aeq1, a1, 24
    16f3:	01bdc4        	ae_subq56	aeq2, aeq3, aeq1
    16f6:	fb7e39ad5f 	{ ae_sq32f.i	aeq2, a13, 20; ae_addq56	aeq3, aeq3, aeq1 }
    16fb:	623254        	ae_sq32f.i	aeq3, a2, 20
    16fe:	620244        	ae_sq32f.i	aeq0, a2, 16
    1701:	600d64        	ae_lq32f.i	aeq0, a13, 24
    1704:	604d74        	ae_lq32f.i	aeq1, a13, 28
    1707:	0198c4        	ae_addq56	aeq2, aeq1, aeq0
    170a:	632cb4        	ae_sq56s.i	aeq2, a12, -40
    170d:	f826fd2d2f 	{ ae_lq32f.i	aeq1, a13, 8; ae_subq56	aeq0, aeq0, aeq1 }
    1712:	608d34        	ae_lq32f.i	aeq2, a13, 12
    1715:	01edc4        	ae_subq56	aeq3, aeq2, aeq1
    1718:	f9ea18afff 	{ ae_cvtp24a16x2.ll	aep2, a15, a15; ae_addq56	aeq1, aeq1, aeq2 }
    171d:	c1a244        	ae_mulzaafq32sp16s.lh	aeq2, aeq1, aep2, aeq0, aep2
    1720:	3c48ba312f 	{ ae_sq56s.i	aeq3, a1, 16; ae_mulzasfq32sp16s.lh	aeq0, aeq0, aep2, aeq1, aep2 }
    1725:	604224        	ae_lq32f.i	aeq1, a2, 8
    1728:	fbea3a010f 	{ ae_sq56s.i	aeq0, a1, 0; ae_addq56	aeq3, aeq1, aeq2 }
    172d:	f9aafd423f 	{ ae_lq32f.i	aeq2, a2, 12; ae_subq56	aeq1, aeq1, aeq2 }
    1732:	621d24        	ae_sq32f.i	aeq1, a13, 8
    1735:	610cb4        	ae_lq56.i	aeq0, a12, -40
    1738:	614124        	ae_lq56.i	aeq1, a1, 16
    173b:	0d497a311f 	{ ae_sq56s.i	aeq3, a1, 8; ae_mulzaafq32sp16s.lh	aeq3, aeq1, aep2, aeq0, aep2 }
    1740:	c52a44        	ae_mulzasfq32sp16s.lh	aeq1, aeq1, aep2, aeq0, aep2
    1743:	012fc4        	ae_subq56	aeq0, aeq2, aeq3
    1746:	faf63de11f 	{ ae_lq56.i	aeq3, a1, 8; ae_addq56	aeq2, aeq2, aeq3 }
    174b:	620d34        	ae_sq32f.i	aeq0, a13, 12
    174e:	622234        	ae_sq32f.i	aeq2, a2, 12
    1751:	600264        	ae_lq32f.i	aeq0, a2, 24
    1754:	fb6639b22f 	{ ae_sq32f.i	aeq3, a2, 8; ae_addq56	aeq3, aeq0, aeq1 }
    1759:	f826fd427f 	{ ae_lq32f.i	aeq2, a2, 28; ae_subq56	aeq0, aeq0, aeq1 }
    175e:	620d64        	ae_sq32f.i	aeq0, a13, 24
    1761:	610104        	ae_lq56.i	aeq0, a1, 0
    1764:	016cc4        	ae_subq56	aeq1, aeq2, aeq0
    1767:	621d74        	ae_sq32f.i	aeq1, a13, 28
    176a:	fa7239b26f 	{ ae_sq32f.i	aeq3, a2, 24; ae_addq56	aeq2, aeq2, aeq0 }
    176f:	622274        	ae_sq32f.i	aeq2, a2, 28
    1772:	f01d           	retw.n

00001774 <F_get_oft_32b>:
    1774:	004136        	entry	a1, 32
    1777:	fcc242        	addi	a4, a2, -4
    177a:	7ca032        	movi	a3, 124
    177d:	0c3347        	bltu	a3, a4, 178d <F_get_oft_32b+0x19>
    1780:	fa6631        	l32r	a3, 118 (917c <fftorder_512_12b+0x2fc>)
    1783:	1122f0        	slli	a2, a2, 1
    1786:	223a           	add.n	a2, a2, a3
    1788:	7e9222        	l16si	a2, a2, 252
    178b:	f01d           	retw.n

0000178d <F_get_oft_32b+0x19>:
    178d:	220c           	movi.n	a2, 2
    178f:	f01d           	retw.n

00001791 <F_get_oft_32b+0x1d>:
    1791:	000000                                        ...

00001794 <xtensa_fft_pass_32b>:
    1794:	00a136        	entry	a1, 80
    1797:	83ad           	ae_sext16	a10, a3
    1799:	b09320        	addx8	a9, a3, a2
    179c:	600204        	ae_lq32f.i	aeq0, a2, 0
    179f:	904330        	addx2	a4, a3, a3
    17a2:	1153c0        	slli	a5, a3, 4
    17a5:	606254        	ae_lq32f.x	aeq1, a2, a5
    17a8:	b04420        	addx8	a4, a4, a2
    17ab:	608404        	ae_lq32f.i	aeq2, a4, 0
    17ae:	f9ea10825f 	{ add	a8, a2, a5; ae_addq56	aeq1, aeq1, aeq2 }
    17b3:	fa663d681f 	{ ae_lq32f.i	aeq3, a8, 4; ae_addq56	aeq2, aeq0, aeq1 }
    17b8:	f826fa211f 	{ ae_sq56s.i	aeq2, a1, 8; ae_subq56	aeq0, aeq0, aeq1 }
    17bd:	630144        	ae_sq56s.i	aeq0, a1, 32
    17c0:	600414        	ae_lq32f.i	aeq0, a4, 4
    17c3:	fb7a3d421f 	{ ae_lq32f.i	aeq2, a2, 4; ae_addq56	aeq3, aeq3, aeq0 }
    17c8:	f9f60fc86e 	{ addi	a6, a8, -4; ae_addq56	aeq1, aeq2, aeq3 }
    17cd:	fab6fd681f 	{ ae_lq32f.i	aeq3, a8, 4; ae_subq56	aeq2, aeq2, aeq3 }
    17d2:	631124        	ae_sq56s.i	aeq1, a1, 16
    17d5:	fa5181        	l32r	a8, 11c (1774 <F_get_oft_32b>)
    17d8:	fb3afb925f 	{ ae_lq32f.x	aeq0, a2, a5; ae_subq56	aeq3, aeq3, aeq0 }
    17dd:	614144        	ae_lq56.i	aeq1, a1, 32
    17e0:	fcc252        	addi	a5, a2, -4
    17e3:	633134        	ae_sq56s.i	aeq3, a1, 24
    17e6:	ff4c7d240f 	{ ae_lq32f.i	aeq1, a4, 0; ae_roundsq32sym	aeq3, aeq1 }
    17eb:	f92af9f61f 	{ ae_sq32f.iu	aeq3, a6, 4; ae_subq56	aeq1, aeq1, aeq0 }
    17f0:	fc547de11f 	{ ae_lq56.i	aeq3, a1, 8; ae_roundsq32sym	aeq0, aeq2 }
    17f5:	624614        	ae_sq32f.iu	aeq0, a6, 4
    17f8:	fe5c7d812f 	{ ae_lq56.i	aeq0, a1, 16; ae_roundsq32sym	aeq2, aeq3 }
    17fd:	ff4479e51f 	{ ae_sq32f.iu	aeq2, a5, 4; ae_roundsq32sym	aeq3, aeq0 }
    1802:	627514        	ae_sq32f.iu	aeq3, a5, 4
    1805:	600914        	ae_lq32f.i	aeq0, a9, 4
    1808:	0189c4        	ae_addq56	aeq2, aeq0, aeq1
    180b:	632104        	ae_sq56s.i	aeq2, a1, 0
    180e:	f826fda13f 	{ ae_lq56.i	aeq1, a1, 24; ae_subq56	aeq0, aeq0, aeq1 }
    1813:	608904        	ae_lq32f.i	aeq2, a9, 0
    1816:	01edc4        	ae_subq56	aeq3, aeq2, aeq1
    1819:	fa760fc92e 	{ addi	a2, a9, -4; ae_addq56	aeq2, aeq2, aeq1 }
    181e:	fd5c4fc44e 	{ addi	a4, a4, -4; ae_roundsq32sym	aeq1, aeq3 }
    1823:	ff447d810f 	{ ae_lq56.i	aeq0, a1, 0; ae_roundsq32sym	aeq3, aeq0 }
    1828:	625414        	ae_sq32f.iu	aeq1, a4, 4
    182b:	fd5479f41f 	{ ae_sq32f.iu	aeq3, a4, 4; ae_roundsq32sym	aeq1, aeq2 }
    1830:	ff4479d21f 	{ ae_sq32f.iu	aeq1, a2, 4; ae_roundsq32sym	aeq3, aeq0 }
    1835:	627214        	ae_sq32f.iu	aeq3, a2, 4
    1838:	0008e0        	callx8	a8
    183b:	21b130        	srai	a11, a3, 1
    183e:	080c           	movi.n	a8, 0
    1840:	fa38c1        	l32r	a12, 120 (8d80 <ROOTS_512_Q16>)
    1843:	feca92        	addi	a9, a10, -2
    1846:	899d           	ae_sext16	a9, a9
    1848:	9099c0        	addx2	a9, a9, a12
    184b:	458984        	ae_lp16x2f.xu	aep0, a9, a8
    184e:	8bcd           	ae_sext16	a12, a11
    1850:	021ce6        	bgei	a12, 1, 1856 <xtensa_fft_pass_32b+0xc2>
    1853:	004e46        	j	1990 <xtensa_fft_pass_32b+0x1fc>
    1856:	11aaf0        	slli	a10, a10, 1
    1859:	8aad           	ae_sext16	a10, a10
    185b:	f03d           	nop.n
    185d:	88ac76        	loopgtz	a12, 18e9 <xtensa_fft_pass_32b+0x155>
    1860:	60c424        	ae_lq32f.i	aeq3, a4, 8
    1863:	600414        	ae_lq32f.i	aeq0, a4, 4
    1866:	6002fd462f 	{ ae_lq32f.i	aeq2, a6, 8; ae_mulzasq32sp16u.lh	aeq1, aeq0, aep0, aeq3, aep0 }
    186b:	33017d061f 	{ ae_lq32f.i	aeq0, a6, 4; ae_mulzaaq32sp16u.lh	aeq3, aeq3, aep0, aeq0, aep0 }
    1870:	62007a110f 	{ ae_sq56s.i	aeq1, a1, 0; ae_mulzasq32sp16u.lh	aeq1, aeq2, aep0, aeq0, aep0 }
    1875:	c20024        	ae_mulzaaq32sp16u.lh	aeq0, aeq0, aep0, aeq2, aep0
    1878:	019bc4        	ae_addq56	aeq2, aeq1, aeq3
    187b:	015fc4        	ae_subq56	aeq1, aeq1, aeq3
    187e:	631134        	ae_sq56s.i	aeq1, a1, 24
    1881:	604524        	ae_lq32f.i	aeq1, a5, 8
    1884:	01dac4        	ae_addq56	aeq3, aeq1, aeq2
    1887:	633124        	ae_sq56s.i	aeq3, a1, 16
    188a:	f9aafde10f 	{ ae_lq56.i	aeq3, a1, 0; ae_subq56	aeq1, aeq1, aeq2 }
    188f:	018bc4        	ae_addq56	aeq2, aeq0, aeq3
    1892:	fb3afd051f 	{ ae_lq32f.i	aeq0, a5, 4; ae_subq56	aeq3, aeq3, aeq0 }
    1897:	fbe23a311f 	{ ae_sq56s.i	aeq3, a1, 8; ae_addq56	aeq3, aeq0, aeq2 }
    189c:	010ec4        	ae_subq56	aeq0, aeq0, aeq2
    189f:	fe445b89af 	{ ae_lp16x2f.xu	aep0, a9, a10; ae_roundsq32sym	aeq2, aeq0 }
    18a4:	fc4c7da12f 	{ ae_lq56.i	aeq1, a1, 16; ae_roundsq32sym	aeq0, aeq1 }
    18a9:	626614        	ae_sq32f.iu	aeq2, a6, 4
    18ac:	fe5c79c61f 	{ ae_sq32f.iu	aeq0, a6, 4; ae_roundsq32sym	aeq2, aeq3 }
    18b1:	626514        	ae_sq32f.iu	aeq2, a5, 4
    18b4:	fc4c7dc11f 	{ ae_lq56.i	aeq2, a1, 8; ae_roundsq32sym	aeq0, aeq1 }
    18b9:	624514        	ae_sq32f.iu	aeq0, a5, 4
    18bc:	604224        	ae_lq32f.i	aeq1, a2, 8
    18bf:	fbaafd021f 	{ ae_lq32f.i	aeq0, a2, 4; ae_subq56	aeq3, aeq1, aeq2 }
    18c4:	f9ea3dc13f 	{ ae_lq56.i	aeq2, a1, 24; ae_addq56	aeq1, aeq1, aeq2 }
    18c9:	fba2fa314f 	{ ae_sq56s.i	aeq3, a1, 32; ae_subq56	aeq3, aeq0, aeq2 }
    18ce:	010ac4        	ae_addq56	aeq0, aeq0, aeq2
    18d1:	068834        	ae_roundsq32sym	aeq2, aeq0
    18d4:	fc4c7da14f 	{ ae_lq56.i	aeq1, a1, 32; ae_roundsq32sym	aeq0, aeq1 }
    18d9:	626214        	ae_sq32f.iu	aeq2, a2, 4
    18dc:	fe5c79c21f 	{ ae_sq32f.iu	aeq0, a2, 4; ae_roundsq32sym	aeq2, aeq3 }
    18e1:	fc4c79e41f 	{ ae_sq32f.iu	aeq2, a4, 4; ae_roundsq32sym	aeq0, aeq1 }
    18e6:	624414        	ae_sq32f.iu	aeq0, a4, 4

000018e9 <xtensa_fft_pass_32b+0x155>:
    18e9:	11daf0        	slli	a13, a10, 1
    18ec:	fecbc2        	addi	a12, a11, -2
    18ef:	8cbd           	ae_sext16	a11, a12
    18f1:	60d0d0        	neg	a13, a13
    18f4:	4589d4        	ae_lp16x2f.xu	aep0, a9, a13
    18f7:	026cf7        	bbci	a12, 15, 18fd <xtensa_fft_pass_32b+0x169>
    18fa:	002406        	j	198e <xtensa_fft_pass_32b+0x1fa>
    18fd:	60a0a0        	neg	a10, a10
    1900:	eb1b           	addi.n	a14, a11, 1
    1902:	88ae76        	loopgtz	a14, 198e <xtensa_fft_pass_32b+0x1fa>
    1905:	608414        	ae_lq32f.i	aeq2, a4, 4
    1908:	60c424        	ae_lq32f.i	aeq3, a4, 8
    190b:	92033d261f 	{ ae_lq32f.i	aeq1, a6, 4; ae_mulzsaq32sp16u.lh	aeq0, aeq3, aep0, aeq2, aep0 }
    1910:	3302bd662f 	{ ae_lq32f.i	aeq3, a6, 8; ae_mulzaaq32sp16u.lh	aeq2, aeq2, aep0, aeq3, aep0 }
    1915:	9003ba014f 	{ ae_sq56s.i	aeq0, a1, 32; ae_mulzsaq32sp16u.lh	aeq0, aeq1, aep0, aeq3, aep0 }
    191a:	c288d4        	ae_mulzaaq32sp16u.lh	aeq3, aeq3, aep0, aeq1, aep0
    191d:	014ac4        	ae_addq56	aeq1, aeq0, aeq2
    1920:	010ec4        	ae_subq56	aeq0, aeq0, aeq2
    1923:	630114        	ae_sq56s.i	aeq0, a1, 8
    1926:	600524        	ae_lq32f.i	aeq0, a5, 8
    1929:	0189c4        	ae_addq56	aeq2, aeq0, aeq1
    192c:	632124        	ae_sq56s.i	aeq2, a1, 16
    192f:	f826fdc14f 	{ ae_lq56.i	aeq2, a1, 32; ae_subq56	aeq0, aeq0, aeq1 }
    1934:	017ac4        	ae_addq56	aeq1, aeq3, aeq2
    1937:	fab6fd651f 	{ ae_lq32f.i	aeq3, a5, 4; ae_subq56	aeq2, aeq2, aeq3 }
    193c:	fa7e3a213f 	{ ae_sq56s.i	aeq2, a1, 24; ae_addq56	aeq2, aeq3, aeq1 }
    1941:	01fdc4        	ae_subq56	aeq3, aeq3, aeq1
    1944:	fd5c5b89af 	{ ae_lp16x2f.xu	aep0, a9, a10; ae_roundsq32sym	aeq1, aeq3 }
    1949:	ff447d812f 	{ ae_lq56.i	aeq0, a1, 16; ae_roundsq32sym	aeq3, aeq0 }
    194e:	625614        	ae_sq32f.iu	aeq1, a6, 4
    1951:	fd5479f61f 	{ ae_sq32f.iu	aeq3, a6, 4; ae_roundsq32sym	aeq1, aeq2 }
    1956:	625514        	ae_sq32f.iu	aeq1, a5, 4
    1959:	ff447da13f 	{ ae_lq56.i	aeq1, a1, 24; ae_roundsq32sym	aeq3, aeq0 }
    195e:	627514        	ae_sq32f.iu	aeq3, a5, 4
    1961:	600224        	ae_lq32f.i	aeq0, a2, 8
    1964:	fa26fd621f 	{ ae_lq32f.i	aeq3, a2, 4; ae_subq56	aeq2, aeq0, aeq1 }
    1969:	f8663da11f 	{ ae_lq56.i	aeq1, a1, 8; ae_addq56	aeq0, aeq0, aeq1 }
    196e:	fa3efa210f 	{ ae_sq56s.i	aeq2, a1, 0; ae_subq56	aeq2, aeq3, aeq1 }
    1973:	01f9c4        	ae_addq56	aeq3, aeq3, aeq1
    1976:	067834        	ae_roundsq32sym	aeq1, aeq3
    1979:	ff447d810f 	{ ae_lq56.i	aeq0, a1, 0; ae_roundsq32sym	aeq3, aeq0 }
    197e:	625214        	ae_sq32f.iu	aeq1, a2, 4
    1981:	fd5479f21f 	{ ae_sq32f.iu	aeq3, a2, 4; ae_roundsq32sym	aeq1, aeq2 }
    1986:	ff4479d41f 	{ ae_sq32f.iu	aeq1, a4, 4; ae_roundsq32sym	aeq3, aeq0 }
    198b:	627414        	ae_sq32f.iu	aeq3, a4, 4

0000198e <xtensa_fft_pass_32b+0x1fa>:
    198e:	f01d           	retw.n

00001990 <xtensa_fft_pass_32b+0x1fc>:
    1990:	11aaf0        	slli	a10, a10, 1
    1993:	8aad           	ae_sext16	a10, a10
    1995:	ffd406        	j	18e9 <xtensa_fft_pass_32b+0x155>

00001998 <xtensa_fft16_32b>:
    1998:	004136        	entry	a1, 32
    199b:	f9e281        	l32r	a8, 124 (15e8 <xtensa_fft8_32b>)
    199e:	02ad           	mov.n	a10, a2
    19a0:	0008e0        	callx8	a8
    19a3:	f9e181        	l32r	a8, 128 (1570 <xtensa_fft4_32b>)
    19a6:	40c2a2        	addi	a10, a2, 64
    19a9:	0008e0        	callx8	a8
    19ac:	f9df81        	l32r	a8, 128 (1570 <xtensa_fft4_32b>)
    19af:	60c2a2        	addi	a10, a2, 96
    19b2:	0008e0        	callx8	a8
    19b5:	f9dd81        	l32r	a8, 12c (1794 <xtensa_fft_pass_32b>)
    19b8:	02ad           	mov.n	a10, a2
    19ba:	04a0b2        	movi	a11, 4
    19bd:	0008e0        	callx8	a8
    19c0:	f01d           	retw.n

000019c2 <xtensa_fft16_32b+0x2a>:
	...

000019c4 <xtensa_fft32_32b>:
    19c4:	004136        	entry	a1, 32
    19c7:	f9da81        	l32r	a8, 130 (1998 <xtensa_fft16_32b>)
    19ca:	20a220        	or	a10, a2, a2
    19cd:	0008e0        	callx8	a8
    19d0:	f9d581        	l32r	a8, 124 (15e8 <xtensa_fft8_32b>)
    19d3:	01d2a2        	addmi	a10, a2, 0x100
    19d6:	80caa2        	addi	a10, a10, -128
    19d9:	0008e0        	callx8	a8
    19dc:	f9d281        	l32r	a8, 124 (15e8 <xtensa_fft8_32b>)
    19df:	01d2a2        	addmi	a10, a2, 0x100
    19e2:	c0caa2        	addi	a10, a10, -64
    19e5:	0008e0        	callx8	a8
    19e8:	f9d181        	l32r	a8, 12c (1794 <xtensa_fft_pass_32b>)
    19eb:	02ad           	mov.n	a10, a2
    19ed:	8b0c           	movi.n	a11, 8
    19ef:	0008e0        	callx8	a8
    19f2:	f01d           	retw.n

000019f4 <xtensa_fft64_32b>:
    19f4:	004136        	entry	a1, 32
    19f7:	f9cf81        	l32r	a8, 134 (19c4 <xtensa_fft32_32b>)
    19fa:	20a220        	or	a10, a2, a2
    19fd:	0008e0        	callx8	a8
    1a00:	f9cc81        	l32r	a8, 130 (1998 <xtensa_fft16_32b>)
    1a03:	01d2a2        	addmi	a10, a2, 0x100
    1a06:	0008e0        	callx8	a8
    1a09:	f9c981        	l32r	a8, 130 (1998 <xtensa_fft16_32b>)
    1a0c:	02d2a2        	addmi	a10, a2, 0x200
    1a0f:	80caa2        	addi	a10, a10, -128
    1a12:	0008e0        	callx8	a8
    1a15:	f9c581        	l32r	a8, 12c (1794 <xtensa_fft_pass_32b>)
    1a18:	02ad           	mov.n	a10, a2
    1a1a:	10a0b2        	movi	a11, 16
    1a1d:	0008e0        	callx8	a8
    1a20:	f01d           	retw.n

00001a22 <xtensa_fft64_32b+0x2e>:
	...

00001a24 <xtensa_fft128_32b>:
    1a24:	004136        	entry	a1, 32
    1a27:	f9c481        	l32r	a8, 138 (19f4 <xtensa_fft64_32b>)
    1a2a:	20a220        	or	a10, a2, a2
    1a2d:	0008e0        	callx8	a8
    1a30:	f9c181        	l32r	a8, 134 (19c4 <xtensa_fft32_32b>)
    1a33:	02d2a2        	addmi	a10, a2, 0x200
    1a36:	0008e0        	callx8	a8
    1a39:	f9be81        	l32r	a8, 134 (19c4 <xtensa_fft32_32b>)
    1a3c:	03d2a2        	addmi	a10, a2, 0x300
    1a3f:	0008e0        	callx8	a8
    1a42:	f9ba81        	l32r	a8, 12c (1794 <xtensa_fft_pass_32b>)
    1a45:	02ad           	mov.n	a10, a2
    1a47:	0b2c           	movi.n	a11, 32
    1a49:	0008e0        	callx8	a8
    1a4c:	f01d           	retw.n

00001a4e <xtensa_fft128_32b+0x2a>:
	...

00001a50 <xtensa_fft256_32b>:
    1a50:	004136        	entry	a1, 32
    1a53:	f9b981        	l32r	a8, 138 (19f4 <xtensa_fft64_32b>)
    1a56:	02ad           	mov.n	a10, a2
    1a58:	0008e0        	callx8	a8
    1a5b:	f9b681        	l32r	a8, 134 (19c4 <xtensa_fft32_32b>)
    1a5e:	02d2a2        	addmi	a10, a2, 0x200
    1a61:	0008e0        	callx8	a8
    1a64:	f9b481        	l32r	a8, 134 (19c4 <xtensa_fft32_32b>)
    1a67:	03d2a2        	addmi	a10, a2, 0x300
    1a6a:	0008e0        	callx8	a8
    1a6d:	f9af81        	l32r	a8, 12c (1794 <xtensa_fft_pass_32b>)
    1a70:	02ad           	mov.n	a10, a2
    1a72:	20a0b2        	movi	a11, 32
    1a75:	0008e0        	callx8	a8
    1a78:	f9b081        	l32r	a8, 138 (19f4 <xtensa_fft64_32b>)
    1a7b:	04d2a2        	addmi	a10, a2, 0x400
    1a7e:	0008e0        	callx8	a8
    1a81:	f9ad81        	l32r	a8, 138 (19f4 <xtensa_fft64_32b>)
    1a84:	06d2a2        	addmi	a10, a2, 0x600
    1a87:	0008e0        	callx8	a8
    1a8a:	f9a881        	l32r	a8, 12c (1794 <xtensa_fft_pass_32b>)
    1a8d:	02ad           	mov.n	a10, a2
    1a8f:	0b4c           	movi.n	a11, 64
    1a91:	0008e0        	callx8	a8
    1a94:	f01d           	retw.n

00001a96 <xtensa_fft256_32b+0x46>:
	...

00001a98 <xtensa_fft512_32b>:
    1a98:	004136        	entry	a1, 32
    1a9b:	f9a881        	l32r	a8, 13c (1a50 <xtensa_fft256_32b>)
    1a9e:	02ad           	mov.n	a10, a2
    1aa0:	0008e0        	callx8	a8
    1aa3:	f9a781        	l32r	a8, 140 (1a24 <xtensa_fft128_32b>)
    1aa6:	08d2a2        	addmi	a10, a2, 0x800
    1aa9:	0008e0        	callx8	a8
    1aac:	f9a581        	l32r	a8, 140 (1a24 <xtensa_fft128_32b>)
    1aaf:	0cd2a2        	addmi	a10, a2, 0xc00
    1ab2:	0008e0        	callx8	a8
    1ab5:	f99d81        	l32r	a8, 12c (1794 <xtensa_fft_pass_32b>)
    1ab8:	02ad           	mov.n	a10, a2
    1aba:	80a0b2        	movi	a11, 128
    1abd:	0008e0        	callx8	a8
    1ac0:	f01d           	retw.n

00001ac2 <xtensa_fft512_32b+0x2a>:
	...

00001ac4 <real_split>:
    1ac4:	00c136        	entry	a1, 96
    1ac7:	036d           	mov.n	a6, a3
    1ac9:	f99481        	l32r	a8, 11c (1774 <F_get_oft_32b>)
    1acc:	03a8           	l32i.n	a10, a3, 0
    1ace:	13c8           	l32i.n	a12, a3, 4
    1ad0:	025d           	mov.n	a5, a2
    1ad2:	0d0c           	movi.n	a13, 0
    1ad4:	042d           	mov.n	a2, a4
    1ad6:	14d9           	s32i.n	a13, a4, 4
    1ad8:	214150        	srai	a4, a5, 1
    1adb:	847d           	ae_sext16	a7, a4
    1add:	aaca           	add.n	a10, a10, a12
    1adf:	02a9           	s32i.n	a10, a2, 0
    1ae1:	c0b570        	sub	a11, a5, a7
    1ae4:	b0bb30        	addx8	a11, a11, a3
    1ae7:	1b98           	l32i.n	a9, a11, 4
    1ae9:	b0a720        	addx8	a10, a7, a2
    1aec:	0bb8           	l32i.n	a11, a11, 0
    1aee:	0ab9           	s32i.n	a11, a10, 0
    1af0:	999a           	add.n	a9, a9, a9
    1af2:	609090        	neg	a9, a9
    1af5:	219190        	srai	a9, a9, 1
    1af8:	016a92        	s32i	a9, a10, 4
    1afb:	85ad           	ae_sext16	a10, a5
    1afd:	0008e0        	callx8	a8
    1b00:	11faf0        	slli	a15, a10, 1
    1b03:	0aed           	mov.n	a14, a10
    1b05:	4129           	s32i.n	a2, a1, 16
    1b07:	c28b           	addi.n	a12, a2, 8
    1b09:	b68b           	addi.n	a11, a6, 8
    1b0b:	5139           	s32i.n	a3, a1, 20
    1b0d:	6149           	s32i.n	a4, a1, 24
    1b0f:	b0d560        	addx8	a13, a5, a6
    1b12:	fccdd2        	addi	a13, a13, -4
    1b15:	140c           	movi.n	a4, 1
    1b17:	81a032        	movi	a3, 129
    1b1a:	060c           	movi.n	a6, 0
    1b1c:	f98121        	l32r	a2, 120 (8d80 <ROOTS_512_Q16>)
    1b1f:	723530        	salt	a3, a5, a3
    1b22:	93ef30        	movnez	a14, a15, a3
    1b25:	fecea2        	addi	a10, a14, -2
    1b28:	8aad           	ae_sext16	a10, a10
    1b2a:	21f1a0        	srai	a15, a10, 1
    1b2d:	a0ff20        	addx4	a15, a15, a2
    1b30:	441f04        	ae_lp16x2f.i	aep1, a15, 0
    1b33:	dc25807d3f 	{ movi	a3, 125; ae_selp24.lh	aep0, aep1, aep1 }
    1b38:	0aa3a7        	bge	a3, a10, 1b46 <real_split+0x82>
    1b3b:	079d           	mov.n	a9, a7
    1b3d:	60e0e0        	neg	a14, a14
    1b40:	170c           	movi.n	a7, 1
    1b42:	000106        	j	1b4a <real_split+0x86>

00001b45 <real_split+0x81>:
	...

00001b46 <real_split+0x82>:
    1b46:	079d           	mov.n	a9, a7
    1b48:	070c           	movi.n	a7, 0

00001b4a <real_split+0x86>:
    1b4a:	8159           	s32i.n	a5, a1, 32
    1b4c:	836470        	moveqz	a6, a4, a7
    1b4f:	604b04        	ae_lq32f.i	aeq1, a11, 0
    1b52:	600b14        	ae_lq32f.i	aeq0, a11, 4
    1b55:	8b4b           	addi.n	a8, a11, 4
    1b57:	0189           	s32i.n	a8, a1, 0
    1b59:	bb8b           	addi.n	a11, a11, 8
    1b5b:	16cc           	bnez.n	a6, 1b60 <real_split+0x9c>
    1b5d:	091484        	ae_movp48	aep0, aep1

00001b60 <real_split+0x9c>:
    1b60:	0229e6        	bgei	a9, 2, 1b66 <real_split+0xa2>
    1b63:	0022c6        	j	1bf2 <real_split+0x12e>
    1b66:	f8cbb2        	addi	a11, a11, -8
    1b69:	f8ccc2        	addi	a12, a12, -8
    1b6c:	890b           	addi.n	a8, a9, -1
    1b6e:	fccdf2        	addi	a15, a13, -4
    1b71:	f94e51        	l32r	a5, ac (8000 <mel_win_53+0x60>)
    1b74:	dd8b           	addi.n	a13, a13, 8
    1b76:	301554        	ae_cvtp24a16x2.ll	aep1, a5, a5
    1b79:	ff8b           	addi.n	a15, a15, 8
    1b7b:	0158           	l32i.n	a5, a1, 0
    1b7d:	66a876        	loopgtz	a8, 1be7 <real_split+0x123>
    1b80:	91013d7fef 	{ ae_lq32f.iu	aeq3, a15, -8; ae_mulzsaq32sp16u.lh	aeq2, aeq1, aep0, aeq0, aep0 }
    1b85:	1827ba016f 	{ ae_sq56s.i	aeq0, a1, 48; ae_mulzaafq32sp16u.lh	aeq0, aeq1, aep1, aeq3, aep1 }
    1b8a:	f8e23d5def 	{ ae_lq32f.iu	aeq2, a13, -8; ae_addq56	aeq0, aeq0, aeq2 }
    1b8f:	32037a115f 	{ ae_sq56s.i	aeq1, a1, 40; ae_mulzaaq32sp16u.lh	aeq1, aeq3, aep0, aeq2, aep0 }
    1b94:	f86600018f 	{ movi	a8, 1; ae_addq56	aeq0, aeq0, aeq1 }
    1b99:	630174        	ae_sq56s.i	aeq0, a1, 56
    1b9c:	9302fd816f 	{ ae_lq56.i	aeq0, a1, 48; ae_mulzsaq32sp16u.lh	aeq3, aeq2, aep0, aeq3, aep0 }
    1ba1:	49263da15f 	{ ae_lq56.i	aeq1, a1, 40; ae_mulzasfq32sp16u.lh	aeq2, aeq0, aep1, aeq2, aep1 }
    1ba6:	c00090aeaf 	{ add	a10, a14, a10; ae_mulzssq32sp16u.lh	aeq0, aeq0, aep0, aeq1, aep0 }
    1bab:	8aad           	ae_sext16	a10, a10
    1bad:	fa723d817f 	{ ae_lq56.i	aeq0, a1, 56; ae_addq56	aeq2, aeq2, aeq0 }
    1bb2:	faf63b001f 	{ ae_sraiq56	aeq0, aeq0, 1; ae_addq56	aeq2, aeq2, aeq3 }
    1bb7:	ff447b209f 	{ ae_sraiq56	aeq2, aeq2, 1; ae_roundsq32sym	aeq3, aeq0 }
    1bbc:	fd5479fc2f 	{ ae_sq32f.iu	aeq3, a12, 8; ae_roundsq32sym	aeq1, aeq2 }
    1bc1:	621c14        	ae_sq32f.i	aeq1, a12, 4
    1bc4:	0ca3a7        	bge	a3, a10, 1bd4 <real_split+0x110>
    1bc7:	60e0e0        	neg	a14, a14
    1bca:	060c           	movi.n	a6, 0
    1bcc:	307740        	xor	a7, a7, a4
    1bcf:	877d           	ae_sext16	a7, a7
    1bd1:	836870        	moveqz	a6, a8, a7

00001bd4 <real_split+0x110>:
    1bd4:	2181a0        	srai	a8, a10, 1
    1bd7:	a08820        	addx4	a8, a8, a2
    1bda:	442804        	ae_lp16x2f.i	aep2, a8, 0
    1bdd:	dc49bd3b2f 	{ ae_lq32f.iu	aeq1, a11, 8; ae_selp24.lh	aep0, aep2, aep2 }
    1be2:	668c           	beqz.n	a6, 1bec <real_split+0x128>

00001be4 <real_split+0x120>:
    1be4:	601524        	ae_lq32f.iu	aeq0, a5, 8

00001be7 <real_split+0x123>:
    1be7:	8158           	l32i.n	a5, a1, 32
    1be9:	000146        	j	1bf2 <real_split+0x12e>

00001bec <real_split+0x128>:
    1bec:	092484        	ae_movp48	aep0, aep2
    1bef:	fffc46        	j	1be4 <real_split+0x120>

00001bf2 <real_split+0x12e>:
    1bf2:	5188           	l32i.n	a8, a1, 20
    1bf4:	41c8           	l32i.n	a12, a1, 16
    1bf6:	61f8           	l32i.n	a15, a1, 24
    1bf8:	60e0e0        	neg	a14, a14
    1bfb:	fecea2        	addi	a10, a14, -2
    1bfe:	8aad           	ae_sext16	a10, a10
    1c00:	ff1b           	addi.n	a15, a15, 1
    1c02:	8ffd           	ae_sext16	a15, a15
    1c04:	b0cfc0        	addx8	a12, a15, a12
    1c07:	c0d5f0        	sub	a13, a5, a15
    1c0a:	11d9           	s32i.n	a13, a1, 4
    1c0c:	b0bf80        	addx8	a11, a15, a8
    1c0f:	b0dd80        	addx8	a13, a13, a8
    1c12:	21d9           	s32i.n	a13, a1, 8
    1c14:	dd4b           	addi.n	a13, a13, 4
    1c16:	0ea3a7        	bge	a3, a10, 1c28 <real_split+0x164>
    1c19:	60e0e0        	neg	a14, a14
    1c1c:	180c           	movi.n	a8, 1
    1c1e:	060c           	movi.n	a6, 0
    1c20:	307740        	xor	a7, a7, a4
    1c23:	877d           	ae_sext16	a7, a7
    1c25:	836870        	moveqz	a6, a8, a7

00001c28 <real_split+0x164>:
    1c28:	604b04        	ae_lq32f.i	aeq1, a11, 0
    1c2b:	9b4b           	addi.n	a9, a11, 4
    1c2d:	600b14        	ae_lq32f.i	aeq0, a11, 4
    1c30:	2181a0        	srai	a8, a10, 1
    1c33:	a08820        	addx4	a8, a8, a2
    1c36:	441804        	ae_lp16x2f.i	aep1, a8, 0
    1c39:	dc25808bbe 	{ addi	a11, a11, 8; ae_selp24.lh	aep0, aep1, aep1 }
    1c3e:	08e616        	beqz	a6, 1cd0 <real_split+0x20c>

00001c41 <real_split+0x17d>:
    1c41:	022f57        	blt	a15, a5, 1c47 <real_split+0x183>
    1c44:	002386        	j	1cd6 <real_split+0x212>
    1c47:	f8cbb2        	addi	a11, a11, -8
    1c4a:	f8ccc2        	addi	a12, a12, -8
    1c4d:	dd8b           	addi.n	a13, a13, 8
    1c4f:	1188           	l32i.n	a8, a1, 4
    1c51:	f91651        	l32r	a5, ac (8000 <mel_win_53+0x60>)
    1c54:	21f8           	l32i.n	a15, a1, 8
    1c56:	301554        	ae_cvtp24a16x2.ll	aep1, a5, a5
    1c59:	095d           	mov.n	a5, a9
    1c5b:	ff8b           	addi.n	a15, a15, 8
    1c5d:	66a876        	loopgtz	a8, 1cc7 <real_split+0x203>
    1c60:	c1013d7fef 	{ ae_lq32f.iu	aeq3, a15, -8; ae_mulzssq32sp16u.lh	aeq2, aeq1, aep0, aeq0, aep0 }
    1c65:	1827ba016f 	{ ae_sq56s.i	aeq0, a1, 48; ae_mulzaafq32sp16u.lh	aeq0, aeq1, aep1, aeq3, aep1 }
    1c6a:	f8e23d5def 	{ ae_lq32f.iu	aeq2, a13, -8; ae_addq56	aeq0, aeq0, aeq2 }
    1c6f:	62037a115f 	{ ae_sq56s.i	aeq1, a1, 40; ae_mulzasq32sp16u.lh	aeq1, aeq3, aep0, aeq2, aep0 }
    1c74:	f86600018f 	{ movi	a8, 1; ae_addq56	aeq0, aeq0, aeq1 }
    1c79:	630174        	ae_sq56s.i	aeq0, a1, 56
    1c7c:	c302fd816f 	{ ae_lq56.i	aeq0, a1, 48; ae_mulzssq32sp16u.lh	aeq3, aeq2, aep0, aeq3, aep0 }
    1c81:	49263da15f 	{ ae_lq56.i	aeq1, a1, 40; ae_mulzasfq32sp16u.lh	aeq2, aeq0, aep1, aeq2, aep1 }
    1c86:	900090aeaf 	{ add	a10, a14, a10; ae_mulzsaq32sp16u.lh	aeq0, aeq0, aep0, aeq1, aep0 }
    1c8b:	8aad           	ae_sext16	a10, a10
    1c8d:	fa723d817f 	{ ae_lq56.i	aeq0, a1, 56; ae_addq56	aeq2, aeq2, aeq0 }
    1c92:	faf63b001f 	{ ae_sraiq56	aeq0, aeq0, 1; ae_addq56	aeq2, aeq2, aeq3 }
    1c97:	ff447b209f 	{ ae_sraiq56	aeq2, aeq2, 1; ae_roundsq32sym	aeq3, aeq0 }
    1c9c:	fd5479fc2f 	{ ae_sq32f.iu	aeq3, a12, 8; ae_roundsq32sym	aeq1, aeq2 }
    1ca1:	621c14        	ae_sq32f.i	aeq1, a12, 4
    1ca4:	0ca3a7        	bge	a3, a10, 1cb4 <real_split+0x1f0>
    1ca7:	60e0e0        	neg	a14, a14
    1caa:	060c           	movi.n	a6, 0
    1cac:	307740        	xor	a7, a7, a4
    1caf:	877d           	ae_sext16	a7, a7
    1cb1:	836870        	moveqz	a6, a8, a7

00001cb4 <real_split+0x1f0>:
    1cb4:	2181a0        	srai	a8, a10, 1
    1cb7:	a08820        	addx4	a8, a8, a2
    1cba:	442804        	ae_lp16x2f.i	aep2, a8, 0
    1cbd:	dc49bd3b2f 	{ ae_lq32f.iu	aeq1, a11, 8; ae_selp24.lh	aep0, aep2, aep2 }
    1cc2:	368c           	beqz.n	a6, 1cc9 <real_split+0x205>

00001cc4 <real_split+0x200>:
    1cc4:	601524        	ae_lq32f.iu	aeq0, a5, 8

00001cc7 <real_split+0x203>:
    1cc7:	f01d           	retw.n

00001cc9 <real_split+0x205>:
    1cc9:	092484        	ae_movp48	aep0, aep2
    1ccc:	fffd06        	j	1cc4 <real_split+0x200>

00001ccf <real_split+0x20b>:
	...

00001cd0 <real_split+0x20c>:
    1cd0:	091484        	ae_movp48	aep0, aep1
    1cd3:	ffda86        	j	1c41 <real_split+0x17d>

00001cd6 <real_split+0x212>:
    1cd6:	f01d           	retw.n

00001cd8 <xtensa_real_fft_32b>:
    1cd8:	004136        	entry	a1, 32
    1cdb:	0b8266        	bnei	a2, 8, 1cea <xtensa_real_fft_32b+0x12>
    1cde:	f91181        	l32r	a8, 124 (15e8 <xtensa_fft8_32b>)
    1ce1:	20a330        	or	a10, a3, a3
    1ce4:	0020f0        	nop
    1ce7:	0008e0        	callx8	a8

00001cea <xtensa_real_fft_32b+0x12>:
    1cea:	07b266        	bnei	a2, 16, 1cf5 <xtensa_real_fft_32b+0x1d>
    1ced:	f91081        	l32r	a8, 130 (1998 <xtensa_fft16_32b>)
    1cf0:	03ad           	mov.n	a10, a3
    1cf2:	0008e0        	callx8	a8

00001cf5 <xtensa_real_fft_32b+0x1d>:
    1cf5:	09c266        	bnei	a2, 32, 1d02 <xtensa_real_fft_32b+0x2a>
    1cf8:	f90f81        	l32r	a8, 134 (19c4 <xtensa_fft32_32b>)
    1cfb:	03ad           	mov.n	a10, a3
    1cfd:	f03d           	nop.n
    1cff:	0008e0        	callx8	a8

00001d02 <xtensa_real_fft_32b+0x2a>:
    1d02:	07d266        	bnei	a2, 64, 1d0d <xtensa_real_fft_32b+0x35>
    1d05:	f90c81        	l32r	a8, 138 (19f4 <xtensa_fft64_32b>)
    1d08:	03ad           	mov.n	a10, a3
    1d0a:	0008e0        	callx8	a8

00001d0d <xtensa_real_fft_32b+0x35>:
    1d0d:	07e266        	bnei	a2, 128, 1d18 <xtensa_real_fft_32b+0x40>
    1d10:	f90c81        	l32r	a8, 140 (1a24 <xtensa_fft128_32b>)
    1d13:	03ad           	mov.n	a10, a3
    1d15:	0008e0        	callx8	a8

00001d18 <xtensa_real_fft_32b+0x40>:
    1d18:	07f266        	bnei	a2, 0x100, 1d23 <xtensa_real_fft_32b+0x4b>
    1d1b:	f90881        	l32r	a8, 13c (1a50 <xtensa_fft256_32b>)
    1d1e:	03ad           	mov.n	a10, a3
    1d20:	0008e0        	callx8	a8

00001d23 <xtensa_real_fft_32b+0x4b>:
    1d23:	00a292        	movi	a9, 0x200
    1d26:	079297        	bne	a2, a9, 1d31 <xtensa_real_fft_32b+0x59>
    1d29:	f90681        	l32r	a8, 144 (1a98 <xtensa_fft512_32b>)
    1d2c:	03ad           	mov.n	a10, a3
    1d2e:	0008e0        	callx8	a8

00001d31 <xtensa_real_fft_32b+0x59>:
    1d31:	02ad           	mov.n	a10, a2
    1d33:	03bd           	mov.n	a11, a3
    1d35:	1142d0        	slli	a4, a2, 3
    1d38:	f90481        	l32r	a8, 148 (1ac4 <real_split>)
    1d3b:	c04340        	sub	a4, a3, a4
    1d3e:	04cd           	mov.n	a12, a4
    1d40:	0008e0        	callx8	a8
    1d43:	848b           	addi.n	a8, a4, 8
    1d45:	0398           	l32i.n	a9, a3, 0
    1d47:	13a8           	l32i.n	a10, a3, 4
    1d49:	0b0c           	movi.n	a11, 0
    1d4b:	11c2c0        	slli	a12, a2, 4
    1d4e:	c4ca           	add.n	a12, a4, a12
    1d50:	13b9           	s32i.n	a11, a3, 4
    1d52:	fcccb2        	addi	a11, a12, -4
    1d55:	c099a0        	sub	a9, a9, a10
    1d58:	0399           	s32i.n	a9, a3, 0
    1d5a:	4622a6        	blti	a2, 2, 1da4 <xtensa_real_fft_32b+0xcc>
    1d5d:	a20b           	addi.n	a10, a2, -1
    1d5f:	2191a0        	srai	a9, a10, 1
    1d62:	146a07        	bbci	a10, 0, 1d7a <xtensa_real_fft_32b+0xa2>
    1d65:	10c482        	addi	a8, a4, 16
    1d68:	34f8           	l32i.n	a15, a4, 12
    1d6a:	ffdbe2        	addmi	a14, a11, 0xffffff00
    1d6d:	24d8           	l32i.n	a13, a4, 8
    1d6f:	60f0f0        	neg	a15, a15
    1d72:	0bf9           	s32i.n	a15, a11, 0
    1d74:	3f6ed2        	s32i	a13, a14, 252
    1d77:	f4ccb2        	addi	a11, a12, -12
    1d7a:	26a976        	loopgtz	a9, 1da4 <xtensa_real_fft_32b+0xcc>
    1d7d:	1898           	l32i.n	a9, a8, 4
    1d7f:	08a8           	l32i.n	a10, a8, 0
    1d81:	ffdbc2        	addmi	a12, a11, 0xffffff00
    1d84:	f8cbd2        	addi	a13, a11, -8
    1d87:	609090        	neg	a9, a9
    1d8a:	ffdde2        	addmi	a14, a13, 0xffffff00
    1d8d:	0b99           	s32i.n	a9, a11, 0
    1d8f:	3f6ca2        	s32i	a10, a12, 252
    1d92:	3898           	l32i.n	a9, a8, 12
    1d94:	f0cbb2        	addi	a11, a11, -16
    1d97:	28a8           	l32i.n	a10, a8, 8
    1d99:	609090        	neg	a9, a9
    1d9c:	10c882        	addi	a8, a8, 16
    1d9f:	0d99           	s32i.n	a9, a13, 0
    1da1:	3f6ea2        	s32i	a10, a14, 252

00001da4 <xtensa_real_fft_32b+0xcc>:
    1da4:	f01d           	retw.n

00001da6 <xtensa_real_fft_32b+0xce>:
	...

00001da8 <xtensa_comp_fft_32b>:
    1da8:	004136        	entry	a1, 32
    1dab:	168226        	beqi	a2, 8, 1dc5 <xtensa_comp_fft_32b+0x1d>
    1dae:	1eb226        	beqi	a2, 16, 1dd0 <xtensa_comp_fft_32b+0x28>

00001db1 <xtensa_comp_fft_32b+0x9>:
    1db1:	28c226        	beqi	a2, 32, 1ddd <xtensa_comp_fft_32b+0x35>

00001db4 <xtensa_comp_fft_32b+0xc>:
    1db4:	30d226        	beqi	a2, 64, 1de8 <xtensa_comp_fft_32b+0x40>

00001db7 <xtensa_comp_fft_32b+0xf>:
    1db7:	3ae226        	beqi	a2, 128, 1df5 <xtensa_comp_fft_32b+0x4d>

00001dba <xtensa_comp_fft_32b+0x12>:
    1dba:	42f226        	beqi	a2, 0x100, 1e00 <xtensa_comp_fft_32b+0x58>

00001dbd <xtensa_comp_fft_32b+0x15>:
    1dbd:	00a282        	movi	a8, 0x200
    1dc0:	471287        	beq	a2, a8, 1e0b <xtensa_comp_fft_32b+0x63>
    1dc3:	f01d           	retw.n

00001dc5 <xtensa_comp_fft_32b+0x1d>:
    1dc5:	f8d781        	l32r	a8, 124 (15e8 <xtensa_fft8_32b>)
    1dc8:	03ad           	mov.n	a10, a3
    1dca:	0008e0        	callx8	a8
    1dcd:	e0b266        	bnei	a2, 16, 1db1 <xtensa_comp_fft_32b+0x9>

00001dd0 <xtensa_comp_fft_32b+0x28>:
    1dd0:	f8d881        	l32r	a8, 130 (1998 <xtensa_fft16_32b>)
    1dd3:	03ad           	mov.n	a10, a3
    1dd5:	f03d           	nop.n
    1dd7:	0008e0        	callx8	a8
    1dda:	d6c266        	bnei	a2, 32, 1db4 <xtensa_comp_fft_32b+0xc>

00001ddd <xtensa_comp_fft_32b+0x35>:
    1ddd:	f8d581        	l32r	a8, 134 (19c4 <xtensa_fft32_32b>)
    1de0:	03ad           	mov.n	a10, a3
    1de2:	0008e0        	callx8	a8
    1de5:	ced266        	bnei	a2, 64, 1db7 <xtensa_comp_fft_32b+0xf>

00001de8 <xtensa_comp_fft_32b+0x40>:
    1de8:	f8d481        	l32r	a8, 138 (19f4 <xtensa_fft64_32b>)
    1deb:	03ad           	mov.n	a10, a3
    1ded:	f03d           	nop.n
    1def:	0008e0        	callx8	a8
    1df2:	c4e266        	bnei	a2, 128, 1dba <xtensa_comp_fft_32b+0x12>

00001df5 <xtensa_comp_fft_32b+0x4d>:
    1df5:	f8d281        	l32r	a8, 140 (1a24 <xtensa_fft128_32b>)
    1df8:	03ad           	mov.n	a10, a3
    1dfa:	0008e0        	callx8	a8
    1dfd:	bcf266        	bnei	a2, 0x100, 1dbd <xtensa_comp_fft_32b+0x15>

00001e00 <xtensa_comp_fft_32b+0x58>:
    1e00:	f8cf81        	l32r	a8, 13c (1a50 <xtensa_fft256_32b>)
    1e03:	03ad           	mov.n	a10, a3
    1e05:	0008e0        	callx8	a8
    1e08:	ffec46        	j	1dbd <xtensa_comp_fft_32b+0x15>

00001e0b <xtensa_comp_fft_32b+0x63>:
    1e0b:	f8ce81        	l32r	a8, 144 (1a98 <xtensa_fft512_32b>)
    1e0e:	03ad           	mov.n	a10, a3
    1e10:	0008e0        	callx8	a8
    1e13:	f01d           	retw.n

00001e15 <xtensa_comp_fft_32b+0x6d>:
    1e15:	000000                                        ...

00001e18 <sw_fft_32b_2811>:
    1e18:	006136        	entry	a1, 48
    1e1b:	00a282        	movi	a8, 0x200
    1e1e:	960b           	addi.n	a9, a6, -1
    1e20:	07c916        	beqz	a9, 1ea0 <sw_fft_32b_2811+0x88>

00001e23 <sw_fft_32b_2811+0xb>:
    1e23:	0a9616        	beqz	a6, 1ed0 <sw_fft_32b_2811+0xb8>

00001e26 <sw_fft_32b_2811+0xe>:
    1e26:	600204        	ae_lq32f.i	aeq0, a2, 0
    1e29:	208220        	or	a8, a2, a2
    1e2c:	1173f0        	slli	a7, a3, 1
    1e2f:	87ad           	ae_sext16	a10, a7
    1e31:	08aa76        	loopgtz	a10, 1e3d <sw_fft_32b_2811+0x25>
    1e34:	3404b4        	ae_sraaq56	aeq0, aeq0, a4
    1e37:	620804        	ae_sq32f.i	aeq0, a8, 0
    1e3a:	601814        	ae_lq32f.iu	aeq0, a8, 4

00001e3d <sw_fft_32b_2811+0x25>:
    1e3d:	a60b           	addi.n	a10, a6, -1
    1e3f:	0b7a16        	beqz	a10, 1efa <sw_fft_32b_2811+0xe2>
    1e42:	f8c281        	l32r	a8, 14c (1cd8 <xtensa_real_fft_32b>)
    1e45:	03ad           	mov.n	a10, a3
    1e47:	20b220        	or	a11, a2, a2
    1e4a:	0008e0        	callx8	a8
    1e4d:	878d           	ae_sext16	a8, a7
    1e4f:	11a3e0        	slli	a10, a3, 2
    1e52:	8aad           	ae_sext16	a10, a10

00001e54 <sw_fft_32b_2811+0x3c>:
    1e54:	421566        	bnei	a5, 1, 1e9a <sw_fft_32b_2811+0x82>
    1e57:	11b8e0        	slli	a11, a8, 2
    1e5a:	c0b2b0        	sub	a11, a2, a11
    1e5d:	600b04        	ae_lq32f.i	aeq0, a11, 0
    1e60:	fe24938bbf 	{ or	a8, a11, a11; ae_zeroq56	aeq2 }
    1e65:	07aa76        	loopgtz	a10, 1e70 <sw_fft_32b_2811+0x58>
    1e68:	fd00bd181f 	{ ae_lq32f.iu	aeq0, a8, 4; ae_absq56	aeq1, aeq0 }
    1e6d:	019ae4        	ae_orq56	aeq2, aeq2, aeq1

00001e70 <sw_fft_32b_2811+0x58>:
    1e70:	622104        	ae_sq32f.i	aeq2, a1, 0
    1e73:	01e8           	l32i.n	a14, a1, 0
    1e75:	600b04        	ae_lq32f.i	aeq0, a11, 0
    1e78:	090c           	movi.n	a9, 0
    1e7a:	40eed0        	nsa	a13, a14
    1e7d:	8ddd           	ae_sext16	a13, a13
    1e7f:	b39de0        	movgez	a9, a13, a14
    1e82:	0020f0        	nop
    1e85:	0aaa76        	loopgtz	a10, 1e93 <sw_fft_32b_2811+0x7b>
    1e88:	fc24fe890f 	{ ae_slaasq56s	aeq0, aeq0, a9; nop }
    1e8d:	620b04        	ae_sq32f.i	aeq0, a11, 0
    1e90:	601b14        	ae_lq32f.iu	aeq0, a11, 4
    1e93:	c02490        	sub	a2, a4, a9
    1e96:	822d           	ae_sext16	a2, a2
    1e98:	f01d           	retw.n

00001e9a <sw_fft_32b_2811+0x82>:
    1e9a:	042d           	mov.n	a2, a4
    1e9c:	f01d           	retw.n

00001e9e <sw_fft_32b_2811+0x86>:
	...

00001ea0 <sw_fft_32b_2811+0x88>:
    1ea0:	f8c392        	addi	a9, a3, -8
    1ea3:	f7c916        	beqz	a9, 1e23 <sw_fft_32b_2811+0xb>
    1ea6:	f0c3a2        	addi	a10, a3, -16
    1ea9:	f76a16        	beqz	a10, 1e23 <sw_fft_32b_2811+0xb>
    1eac:	e0c3b2        	addi	a11, a3, -32
    1eaf:	f70b16        	beqz	a11, 1e23 <sw_fft_32b_2811+0xb>
    1eb2:	c0c3c2        	addi	a12, a3, -64
    1eb5:	f6ac16        	beqz	a12, 1e23 <sw_fft_32b_2811+0xb>
    1eb8:	80c3d2        	addi	a13, a3, -128
    1ebb:	f64d16        	beqz	a13, 1e23 <sw_fft_32b_2811+0xb>
    1ebe:	ffd3e2        	addmi	a14, a3, 0xffffff00
    1ec1:	f5ee16        	beqz	a14, 1e23 <sw_fft_32b_2811+0xb>
    1ec4:	fed3f2        	addmi	a15, a3, 0xfffffe00
    1ec7:	f58f16        	beqz	a15, 1e23 <sw_fft_32b_2811+0xb>

00001eca <sw_fft_32b_2811+0xb2>:
    1eca:	126c           	movi.n	a2, -31
    1ecc:	f01d           	retw.n

00001ece <sw_fft_32b_2811+0xb6>:
	...

00001ed0 <sw_fft_32b_2811+0xb8>:
    1ed0:	f8c392        	addi	a9, a3, -8
    1ed3:	f4f916        	beqz	a9, 1e26 <sw_fft_32b_2811+0xe>
    1ed6:	f0c3a2        	addi	a10, a3, -16
    1ed9:	f49a16        	beqz	a10, 1e26 <sw_fft_32b_2811+0xe>
    1edc:	e0c3b2        	addi	a11, a3, -32
    1edf:	f43b16        	beqz	a11, 1e26 <sw_fft_32b_2811+0xe>
    1ee2:	c0c3c2        	addi	a12, a3, -64
    1ee5:	f3dc16        	beqz	a12, 1e26 <sw_fft_32b_2811+0xe>
    1ee8:	80c3d2        	addi	a13, a3, -128
    1eeb:	f37d16        	beqz	a13, 1e26 <sw_fft_32b_2811+0xe>
    1eee:	ffd3e2        	addmi	a14, a3, 0xffffff00
    1ef1:	f31e16        	beqz	a14, 1e26 <sw_fft_32b_2811+0xe>
    1ef4:	d29387        	bne	a3, a8, 1eca <sw_fft_32b_2811+0xb2>
    1ef7:	ffcac6        	j	1e26 <sw_fft_32b_2811+0xe>

00001efa <sw_fft_32b_2811+0xe2>:
    1efa:	f89581        	l32r	a8, 150 (1da8 <xtensa_comp_fft_32b>)
    1efd:	03ad           	mov.n	a10, a3
    1eff:	02bd           	mov.n	a11, a2
    1f01:	0008e0        	callx8	a8
    1f04:	87ad           	ae_sext16	a10, a7
    1f06:	080c           	movi.n	a8, 0
    1f08:	ffd206        	j	1e54 <sw_fft_32b_2811+0x3c>

00001f0b <sw_fft_32b_2811+0xf3>:
    1f0b:	00000000 00413600                                .....

00001f10 <__do_global_ctors_aux>:
    1f10:	004136        	entry	a1, 32
    1f13:	f89081        	l32r	a8, 154 (9190 <fftorder_512_12b+0x310>)
    1f16:	7f2882        	l32i	a8, a8, 0x1fc
    1f19:	f88f21        	l32r	a2, 158 (938c <__CTOR_LIST___47>)
    1f1c:	0c0826        	beqi	a8, -1, 1f2c <__do_global_ctors_aux+0x1c>
    1f1f:	02a8           	l32i.n	a10, a2, 0

00001f21 <__do_global_ctors_aux+0x11>:
    1f21:	000ae0        	callx8	a10
    1f24:	fcc222        	addi	a2, a2, -4
    1f27:	02a8           	l32i.n	a10, a2, 0
    1f29:	f40a66        	bnei	a10, -1, 1f21 <__do_global_ctors_aux+0x11>

00001f2c <__do_global_ctors_aux+0x1c>:
    1f2c:	f01d           	retw.n

00001f2e <__do_global_ctors_aux+0x1e>:
	...

00001f30 <memset>:
    1f30:	004136        	entry	a1, 32
    1f33:	026d           	mov.n	a6, a2
    1f35:	12bc           	beqz.n	a2, 1f6a <memset+0x3a>
    1f37:	b4ac           	beqz.n	a4, 1f66 <memset+0x36>
    1f39:	245040        	extui	a5, a4, 0, 3
    1f3c:	047d           	mov.n	a7, a4
    1f3e:	414370        	srli	a4, a7, 3
    1f41:	049576        	loopnez	a5, 1f49 <memset+0x19>
    1f44:	004232        	s8i	a3, a2, 0
    1f47:	221b           	addi.n	a2, a2, 1
    1f49:	199476        	loopnez	a4, 1f66 <memset+0x36>
    1f4c:	004232        	s8i	a3, a2, 0
    1f4f:	014232        	s8i	a3, a2, 1
    1f52:	024232        	s8i	a3, a2, 2
    1f55:	034232        	s8i	a3, a2, 3
    1f58:	044232        	s8i	a3, a2, 4
    1f5b:	054232        	s8i	a3, a2, 5
    1f5e:	064232        	s8i	a3, a2, 6
    1f61:	074232        	s8i	a3, a2, 7
    1f64:	228b           	addi.n	a2, a2, 8

00001f66 <memset+0x36>:
    1f66:	062d           	mov.n	a2, a6
    1f68:	f01d           	retw.n

00001f6a <memset+0x3a>:
    1f6a:	020c           	movi.n	a2, 0
    1f6c:	f01d           	retw.n

00001f6e <memset+0x3e>:
	...

00001f70 <memcpy>:
    1f70:	004136        	entry	a1, 32
    1f73:	027d           	mov.n	a7, a2
    1f75:	032d           	mov.n	a2, a3
    1f77:	043d           	mov.n	a3, a4
    1f79:	07d716        	beqz	a7, 1ffa <memcpy+0x8a>
    1f7c:	07a216        	beqz	a2, 1ffa <memcpy+0x8a>
    1f7f:	20b277        	bgeu	a2, a7, 1fa3 <memcpy+0x33>
    1f82:	442a           	add.n	a4, a4, a2
    1f84:	1bb747        	bgeu	a7, a4, 1fa3 <memcpy+0x33>
    1f87:	240b           	addi.n	a2, a4, -1
    1f89:	530b           	addi.n	a5, a3, -1
    1f8b:	437a           	add.n	a4, a3, a7
    1f8d:	440b           	addi.n	a4, a4, -1
    1f8f:	630526        	beqi	a5, -1, 1ff6 <memcpy+0x86>
    1f92:	099376        	loopnez	a3, 1f9f <memcpy+0x2f>
    1f95:	000232        	l8ui	a3, a2, 0
    1f98:	220b           	addi.n	a2, a2, -1
    1f9a:	440b           	addi.n	a4, a4, -1
    1f9c:	014432        	s8i	a3, a4, 1

00001f9f <memcpy+0x2f>:
    1f9f:	072d           	mov.n	a2, a7
    1fa1:	f01d           	retw.n

00001fa3 <memcpy+0x33>:
    1fa3:	074d           	mov.n	a4, a7
    1fa5:	530b           	addi.n	a5, a3, -1
    1fa7:	4b0526        	beqi	a5, -1, 1ff6 <memcpy+0x86>
    1faa:	248030        	extui	a8, a3, 0, 3
    1fad:	036d           	mov.n	a6, a3
    1faf:	413360        	srli	a3, a6, 3
    1fb2:	099876        	loopnez	a8, 1fbf <memcpy+0x4f>
    1fb5:	000292        	l8ui	a9, a2, 0
    1fb8:	004492        	s8i	a9, a4, 0
    1fbb:	221b           	addi.n	a2, a2, 1
    1fbd:	441b           	addi.n	a4, a4, 1
    1fbf:	339376        	loopnez	a3, 1ff6 <memcpy+0x86>
    1fc2:	000232        	l8ui	a3, a2, 0
    1fc5:	004432        	s8i	a3, a4, 0
    1fc8:	010232        	l8ui	a3, a2, 1
    1fcb:	014432        	s8i	a3, a4, 1
    1fce:	020232        	l8ui	a3, a2, 2
    1fd1:	024432        	s8i	a3, a4, 2
    1fd4:	030232        	l8ui	a3, a2, 3
    1fd7:	034432        	s8i	a3, a4, 3
    1fda:	040232        	l8ui	a3, a2, 4
    1fdd:	044432        	s8i	a3, a4, 4
    1fe0:	050232        	l8ui	a3, a2, 5
    1fe3:	054432        	s8i	a3, a4, 5
    1fe6:	060232        	l8ui	a3, a2, 6
    1fe9:	064432        	s8i	a3, a4, 6
    1fec:	070232        	l8ui	a3, a2, 7
    1fef:	074432        	s8i	a3, a4, 7
    1ff2:	228b           	addi.n	a2, a2, 8
    1ff4:	448b           	addi.n	a4, a4, 8
    1ff6:	072d           	mov.n	a2, a7
    1ff8:	f01d           	retw.n

00001ffa <memcpy+0x8a>:
    1ffa:	020c           	movi.n	a2, 0
    1ffc:	f01d           	retw.n

00001ffe <memcpy+0x8e>:
	...

00002000 <_fini>:
    2000:	008136        	entry	a1, 64
    2003:	f85681        	l32r	a8, 15c (178 <__do_global_dtors_aux>)
    2006:	f03d           	nop.n
    2008:	0008e0        	callx8	a8

0000200b <_fini+0xb>:
    200b:	f01d           	retw.n
