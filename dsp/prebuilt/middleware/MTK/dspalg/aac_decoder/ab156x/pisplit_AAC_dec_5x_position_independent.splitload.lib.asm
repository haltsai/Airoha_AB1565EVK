
Build/lib/pisplit_AAC_dec_5x_position_independent.splitload.lib:     file format elf32-xtensa-le


Disassembly of section .text:

00000000 <_init-0x1bc>:
       0:	00000214 00003668 00009560 000094c4     ....h6..`.......
      10:	00000000 000094ac 00000000 00009564     ............d...
      20:	0000068c 000006d8 000006ec 0000072c     ............,...
      30:	000008ac 00004df0 00004370 000027c8     .....M..pC...'..
      40:	000027a4 00003688 7fffffff 00200000     .'...6........ .
      50:	00005e20 00001f60 00002108 00002184      ^..`....!...!..
      60:	000034c0 0000ac44 00005eb0 0000078c     .4..D....^......
      70:	00002240 00001ec4 000022b8 000022f0     @"......."..."..
      80:	000022d0 00002298 00000aac 00000e90     ."..."..........
      90:	00002254 00004130 000002e0 00002284     T"..0A......."..
      a0:	00000e38 000034d0 000016a0 00002dbc     8....4.......-..
      b0:	00000e64 00001004 000034f0 000034e0     d........4...4..
      c0:	00003370 00005ea0 000015cc 000018cc     p3...^..........
      d0:	000013c0 00001ab4 0000199c 00002300     .............#..
      e0:	00002640 00002564 000024a8 000023b8     @&..d%...$...#..
      f0:	00002320 00001cb4 000094e0 01000000      #..............
     100:	00400000 00002208 000053c0 00005920     ..@.."...S.. Y..
     110:	00005a40 00004a60 00004c30 00004ab0     @Z..`J..0L...J..
     120:	00005620 00005800 00004da0 00005100      V...X...M...Q..
     130:	00004ef0 000051d0 00005b20 000027f8     .N...Q.. [...'..
     140:	001fffff 00004aa0 00004a20 00004a50     .....J.. J..PJ..
     150:	00001004 00007098 00005ed8 000066e8     .....p...^...f..
     160:	00006ce8 00006e68 00008098 00006ec8     .l..hn.......n..
     170:	00007048 00007ba9 00009550 0000957c     Hp...{..P...|...
     180:	000040e0 00000230 000040f4 00000298     .@..0....@......
     190:	0000410c 000002f4 000094d0 00004590     .A...........E..
     1a0:	000094f0 00004580 00009520 00004150     .....E.. ...PA..
     1b0:	000092b4 000094b0 000001d8              ............

000001bc <_init>:
     1bc:	008136        	entry	a1, 64
     1bf:	ff9081        	l32r	a8, 0 (214 <frame_dummy>)
     1c2:	f03d           	nop.n
     1c4:	0008e0        	callx8	a8
     1c7:	ff8f81        	l32r	a8, 4 (3668 <__do_global_ctors_aux>)
     1ca:	f03d           	nop.n
     1cc:	0008e0        	callx8	a8
     1cf:	f01d           	retw.n

000001d1 <_init+0x15>:
     1d1:	00000000 36000000                                .......

000001d8 <__do_global_dtors_aux>:
     1d8:	004136        	entry	a1, 32
     1db:	ff8b31        	l32r	a3, 8 (9560 <__bss_start>)
     1de:	000382        	l8ui	a8, a3, 0
     1e1:	d8ec           	bnez.n	a8, 212 <__do_global_dtors_aux+0x3a>
     1e3:	ff8a21        	l32r	a2, c (94c4 <p$3519_6_1>)
     1e6:	02b8           	l32i.n	a11, a2, 0
     1e8:	0ba8           	l32i.n	a10, a11, 0
     1ea:	ca8c           	beqz.n	a10, 1fa <__do_global_dtors_aux+0x22>

000001ec <__do_global_dtors_aux+0x14>:
     1ec:	9b4b           	addi.n	a9, a11, 4
     1ee:	0299           	s32i.n	a9, a2, 0
     1f0:	000ae0        	callx8	a10
     1f3:	02b8           	l32i.n	a11, a2, 0
     1f5:	0ba8           	l32i.n	a10, a11, 0
     1f7:	ff1a56        	bnez	a10, 1ec <__do_global_dtors_aux+0x14>

000001fa <__do_global_dtors_aux+0x22>:
     1fa:	ff85a1        	l32r	a10, 10 (0 <_text_start>)
     1fd:	0b0c           	movi.n	a11, 0
     1ff:	081ab7        	beq	a10, a11, 20b <__do_global_dtors_aux+0x33>
     202:	ff84a1        	l32r	a10, 14 (94ac <__EH_FRAME_BEGIN___44>)
     205:	ff8281        	l32r	a8, 10 (0 <_text_start>)
     208:	0008e0        	callx8	a8

0000020b <__do_global_dtors_aux+0x33>:
     20b:	1c0c           	movi.n	a12, 1
     20d:	0043c2        	s8i	a12, a3, 0
     210:	f01d           	retw.n

00000212 <__do_global_dtors_aux+0x3a>:
     212:	f01d           	retw.n

00000214 <frame_dummy>:
     214:	004136        	entry	a1, 32
     217:	ff8081        	l32r	a8, 18 (0 <_text_start>)
     21a:	090c           	movi.n	a9, 0
     21c:	0b1897        	beq	a8, a9, 22b <frame_dummy+0x17>
     21f:	ff7da1        	l32r	a10, 14 (94ac <__EH_FRAME_BEGIN___44>)
     222:	ff7eb1        	l32r	a11, 1c (9564 <object$3538_8_1>)
     225:	ff7c81        	l32r	a8, 18 (0 <_text_start>)
     228:	0008e0        	callx8	a8

0000022b <frame_dummy+0x17>:
     22b:	f01d           	retw.n

0000022d <frame_dummy+0x19>:
     22d:	000000                                        ...

00000230 <AIR_AAC_init>:
     230:	006136        	entry	a1, 48
     233:	ff7b81        	l32r	a8, 20 (68c <aac_open>)
     236:	02ad           	mov.n	a10, a2
     238:	2b0c           	movi.n	a11, 2
     23a:	0008e0        	callx8	a8
     23d:	ff7981        	l32r	a8, 24 (6d8 <aac_get_config>)
     240:	0a2d           	mov.n	a2, a10
     242:	0008e0        	callx8	a8
     245:	0abd           	mov.n	a11, a10
     247:	190c           	movi.n	a9, 1
     249:	ff7781        	l32r	a8, 28 (6ec <aac_set_config>)
     24c:	094a92        	s8i	a9, a10, 9
     24f:	02ad           	mov.n	a10, a2
     251:	0008e0        	callx8	a8
     254:	02ad           	mov.n	a10, a2
     256:	e14b           	addi.n	a14, a1, 4
     258:	ff7581        	l32r	a8, 2c (72c <aac_dec_init>)
     25b:	0003d2        	l8ui	a13, a3, 0
     25e:	030392        	l8ui	a9, a3, 3
     261:	0203b2        	l8ui	a11, a3, 2
     264:	0103c2        	l8ui	a12, a3, 1
     267:	8b7d90        	depbits	a9, a13, 24, 8
     26a:	01dd           	mov.n	a13, a1
     26c:	0b7c90        	depbits	a9, a12, 16, 8
     26f:	8a7b90        	depbits	a9, a11, 8, 8
     272:	04cd           	mov.n	a12, a4
     274:	03bd           	mov.n	a11, a3
     276:	f3f190        	wur.ae_bithead	a9
     279:	030c           	movi.n	a3, 0
     27b:	032634        	wur.ae_bitptr	a3
     27e:	0008e0        	callx8	a8
     281:	00da96        	bltz	a10, 292 <AIR_AAC_init+0x62>
     284:	120c           	movi.n	a2, 1
     286:	01e8           	l32i.n	a14, a1, 0
     288:	2f0c           	movi.n	a15, 2
     28a:	65fd           	ae_s16i.n	a15, a5, 0
     28c:	06e9           	s32i.n	a14, a6, 0
     28e:	673d           	ae_s16i.n	a3, a7, 0
     290:	f01d           	retw.n

00000292 <AIR_AAC_init+0x62>:
     292:	020c           	movi.n	a2, 0
     294:	f01d           	retw.n

00000296 <AIR_AAC_init+0x66>:
	...

00000298 <AIR_AAC_decoder>:
     298:	008136        	entry	a1, 64
     29b:	02ad           	mov.n	a10, a2
     29d:	020c           	movi.n	a2, 0
     29f:	0729           	s32i.n	a2, a7, 0
     2a1:	094122        	s8i	a2, a1, 9
     2a4:	54cc           	bnez.n	a4, 2ad <AIR_AAC_decoder+0x15>
     2a6:	ffa022        	movi	a2, 255
     2a9:	f01d           	retw.n

000002ab <AIR_AAC_decoder+0x13>:
	...

000002ad <AIR_AAC_decoder+0x15>:
     2ad:	03cd           	mov.n	a12, a3
     2af:	6a59           	s32i.n	a5, a10, 24
     2b1:	7a69           	s32i.n	a6, a10, 28
     2b3:	ff5f81        	l32r	a8, 30 (8ac <aac_dec_decode>)
     2b6:	84dd           	ae_sext16	a13, a4
     2b8:	01bd           	mov.n	a11, a1
     2ba:	0008e0        	callx8	a8
     2bd:	090192        	l8ui	a9, a1, 9
     2c0:	00a4b2        	movi	a11, 0x400
     2c3:	89cc           	bnez.n	a9, 2cf <AIR_AAC_decoder+0x37>
     2c5:	11a8           	l32i.n	a10, a1, 4
     2c7:	41a1a0        	srli	a10, a10, 1
     2ca:	07a9           	s32i.n	a10, a7, 0
     2cc:	000286        	j	2da <AIR_AAC_decoder+0x42>

000002cf <AIR_AAC_decoder+0x37>:
     2cf:	07b9           	s32i.n	a11, a7, 0
     2d1:	05ab76        	loopgtz	a11, 2da <AIR_AAC_decoder+0x42>
     2d4:	09e524        	ae_s32ip	a2, a5, 4
     2d7:	09e624        	ae_s32ip	a2, a6, 4

000002da <AIR_AAC_decoder+0x42>:
     2da:	090122        	l8ui	a2, a1, 9
     2dd:	f01d           	retw.n

000002df <AIR_AAC_decoder+0x47>:
	...

000002e0 <xt_memcpy1>:
     2e0:	004136        	entry	a1, 32
     2e3:	f03d           	nop.n
     2e5:	099476        	loopnez	a4, 2f2 <xt_memcpy1+0x12>
     2e8:	000352        	l8ui	a5, a3, 0
     2eb:	004252        	s8i	a5, a2, 0
     2ee:	331b           	addi.n	a3, a3, 1
     2f0:	221b           	addi.n	a2, a2, 1

000002f2 <xt_memcpy1+0x12>:
     2f2:	f01d           	retw.n

000002f4 <get_AACdec_memsize>:
     2f4:	004136        	entry	a1, 32
     2f7:	ff4f21        	l32r	a2, 34 (4df0 <vld_huffdata7+0x50>)
     2fa:	f01d           	retw.n

000002fc <get_AACdec_memsize+0x8>:
     2fc:	00000000                                ....

00000300 <chk_decode_lc>:
     300:	004136        	entry	a1, 32
     303:	030c           	movi.n	a3, 0
     305:	fec242        	addi	a4, a2, -2
     308:	f27c           	movi.n	a2, -1
     30a:	832340        	moveqz	a2, a3, a4
     30d:	f01d           	retw.n

0000030f <chk_decode_lc+0xf>:
	...

00000310 <ms_decode>:
     310:	008136        	entry	a1, 64
     313:	0139           	s32i.n	a3, a1, 0
     315:	5149           	s32i.n	a4, a1, 20
     317:	6159           	s32i.n	a5, a1, 24
     319:	7129           	s32i.n	a2, a1, 28
     31b:	04d2f2        	addmi	a15, a2, 0x400
     31e:	04d3e2        	addmi	a14, a3, 0x400
     321:	c0cee2        	addi	a14, a14, -64
     324:	41cfd2        	addi	a13, a15, 65
     327:	400f72        	l8ui	a7, a15, 64
     32a:	c0cff2        	addi	a15, a15, -64
     32d:	0f2d           	mov.n	a2, a15
     32f:	0d1716        	beqz	a7, 404 <ms_decode+0xf4>
     332:	7198           	l32i.n	a9, a1, 28
     334:	060992        	l8ui	a9, a9, 6
     337:	0c9916        	beqz	a9, 404 <ms_decode+0xf4>
     33a:	050c           	movi.n	a5, 0
     33c:	71c8           	l32i.n	a12, a1, 28
     33e:	8a0c           	movi.n	a10, 8
     340:	040c           	movi.n	a4, 0
     342:	1149           	s32i.n	a4, a1, 4
     344:	d2a6a0        	quos	a10, a6, a10
     347:	d40c           	movi.n	a4, 13
     349:	31a9           	s32i.n	a10, a1, 12
     34b:	000cc2        	l8ui	a12, a12, 0

0000034e <ms_decode+0x3e>:
     34e:	7178           	l32i.n	a7, a1, 28
     350:	1168           	l32i.n	a6, a1, 4
     352:	667a           	add.n	a6, a6, a7
     354:	2169           	s32i.n	a6, a1, 8
     356:	080662        	l8ui	a6, a6, 8
     359:	090616        	beqz	a6, 3ed <ms_decode+0xdd>
     35c:	080c           	movi.n	a8, 0
     35e:	4189           	s32i.n	a8, a1, 16

00000360 <ms_decode+0x50>:
     360:	741ca6        	blti	a12, 1, 3d8 <ms_decode+0xc8>
     363:	7198           	l32i.n	a9, a1, 28
     365:	060c           	movi.n	a6, 0

00000367 <ms_decode+0x57>:
     367:	a6da           	add.n	a10, a6, a13
     369:	000aa2        	l8ui	a10, a10, 0
     36c:	070c           	movi.n	a7, 0
     36e:	180c           	movi.n	a8, 1
     370:	4acc           	bnez.n	a10, 378 <ms_decode+0x68>
     372:	800fb2        	l8ui	a11, a15, 128
     375:	532b66        	bnei	a11, 2, 3cc <ms_decode+0xbc>

00000378 <ms_decode+0x68>:
     378:	b6ea           	add.n	a11, a6, a14
     37a:	000bb2        	l8ui	a11, a11, 0
     37d:	030c           	movi.n	a3, 0
     37f:	f2cba2        	addi	a10, a11, -14
     382:	8338a0        	moveqz	a3, a8, a10
     385:	f1cbb2        	addi	a11, a11, -15
     388:	8378b0        	moveqz	a7, a8, a11
     38b:	203370        	or	a3, a3, a7
     38e:	a3fc           	bnez.n	a3, 3cc <ms_decode+0xbc>
     390:	762a           	add.n	a7, a6, a2
     392:	000772        	l8ui	a7, a7, 0
     395:	331747        	beq	a7, a4, 3cc <ms_decode+0xbc>
     398:	8019b2        	l16ui	a11, a9, 0x100
     39b:	8119a2        	l16ui	a10, a9, 0x102
     39e:	6138           	l32i.n	a3, a1, 24
     3a0:	8b5a           	add.n	a8, a11, a5
     3a2:	26aba7        	bge	a11, a10, 3cc <ms_decode+0xbc>
     3a5:	c0cab0        	sub	a12, a10, a11
     3a8:	51a8           	l32i.n	a10, a1, 20
     3aa:	a0a8a0        	addx4	a10, a8, a10
     3ad:	a08830        	addx4	a8, a8, a3
     3b0:	130c           	movi.n	a3, 1
     3b2:	53cc30        	max	a12, a12, a3
     3b5:	0eac76        	loopgtz	a12, 3c7 <ms_decode+0xb7>
     3b8:	0878           	l32i.n	a7, a8, 0
     3ba:	0ac8           	l32i.n	a12, a10, 0
     3bc:	c0bc70        	sub	a11, a12, a7
     3bf:	cc7a           	add.n	a12, a12, a7
     3c1:	09eac4        	ae_s32ip	a12, a10, 4
     3c4:	09e8b4        	ae_s32ip	a11, a8, 4

000003c7 <ms_decode+0xb7>:
     3c7:	71c8           	l32i.n	a12, a1, 28
     3c9:	000cc2        	l8ui	a12, a12, 0
     3cc:	992b           	addi.n	a9, a9, 2
     3ce:	661b           	addi.n	a6, a6, 1
     3d0:	9326c7        	blt	a6, a12, 367 <ms_decode+0x57>
     3d3:	2168           	l32i.n	a6, a1, 8
     3d5:	080662        	l8ui	a6, a6, 8

000003d8 <ms_decode+0xc8>:
     3d8:	3188           	l32i.n	a8, a1, 12
     3da:	4178           	l32i.n	a7, a1, 16
     3dc:	558a           	add.n	a5, a5, a8
     3de:	771b           	addi.n	a7, a7, 1
     3e0:	4179           	s32i.n	a7, a1, 16
     3e2:	02a767        	bge	a7, a6, 3e8 <ms_decode+0xd8>
     3e5:	ffddc6        	j	360 <ms_decode+0x50>
     3e8:	7198           	l32i.n	a9, a1, 28
     3ea:	060992        	l8ui	a9, a9, 6

000003ed <ms_decode+0xdd>:
     3ed:	22ca           	add.n	a2, a2, a12
     3ef:	01b8           	l32i.n	a11, a1, 0
     3f1:	11a8           	l32i.n	a10, a1, 4
     3f3:	ddca           	add.n	a13, a13, a12
     3f5:	000bb2        	l8ui	a11, a11, 0
     3f8:	aa1b           	addi.n	a10, a10, 1
     3fa:	11a9           	s32i.n	a10, a1, 4
     3fc:	eeba           	add.n	a14, a14, a11
     3fe:	02aa97        	bge	a10, a9, 404 <ms_decode+0xf4>
     401:	ffd246        	j	34e <ms_decode+0x3e>
     404:	f01d           	retw.n

00000406 <ms_decode+0xf6>:
	...

00000408 <pulse_decode>:
     408:	00a136        	entry	a1, 80
     40b:	a50c           	movi.n	a5, 10
     40d:	7149           	s32i.n	a4, a1, 28
     40f:	4139           	s32i.n	a3, a1, 16
     411:	12c262        	addi	a6, a2, 18
     414:	023d           	mov.n	a3, a2
     416:	5169           	s32i.n	a6, a1, 20
     418:	04d222        	addmi	a2, a2, 0x400
     41b:	6129           	s32i.n	a2, a1, 24
     41d:	bf0222        	l8ui	a2, a2, 191
     420:	040c           	movi.n	a4, 0
     422:	ff0561        	l32r	a6, 38 (4370 <aac_pow4_3>)
     425:	902230        	addx2	a2, a2, a3
     428:	801222        	l16ui	a2, a2, 0x100

0000042b <pulse_decode+0x23>:
     42b:	834a           	add.n	a8, a3, a4
     42d:	04d882        	addmi	a8, a8, 0x400
     430:	40c882        	addi	a8, a8, 64
     433:	800882        	l8ui	a8, a8, 128
     436:	51b8           	l32i.n	a11, a1, 20
     438:	7198           	l32i.n	a9, a1, 28
     43a:	228a           	add.n	a2, a2, a8
     43c:	822d           	ae_sext16	a2, a2
     43e:	022297        	blt	a2, a9, 444 <pulse_decode+0x3c>
     441:	0042c6        	j	550 <pulse_decode+0x148>
     444:	0d0c           	movi.n	a13, 0
     446:	4178           	l32i.n	a7, a1, 16
     448:	90a430        	addx2	a10, a4, a3
     44b:	04daa2        	addmi	a10, a10, 0x400
     44e:	c8caa2        	addi	a10, a10, -56
     451:	a07270        	addx4	a7, a2, a7
     454:	7e9aa2        	l16si	a10, a10, 252
     457:	9179           	s32i.n	a7, a1, 36
     459:	0778           	l32i.n	a7, a7, 0
     45b:	0000c6        	j	462 <pulse_decode+0x5a>

0000045e <pulse_decode+0x56>:
     45e:	dd1b           	addi.n	a13, a13, 1
     460:	8ddd           	ae_sext16	a13, a13

00000462 <pulse_decode+0x5a>:
     462:	4bed           	ae_l16ui.n	a14, a11, 0
     464:	bb2b           	addi.n	a11, a11, 2
     466:	f4a2e7        	bge	a2, a14, 45e <pulse_decode+0x56>
     469:	90bd30        	addx2	a11, a13, a3
     46c:	b49bb2        	l16si	a11, a11, 0x168
     46f:	14c0b0        	extui	a12, a11, 0, 2
     472:	21b2b0        	srai	a11, a11, 2
     475:	e0cbb2        	addi	a11, a11, -32
     478:	8bbd           	ae_sext16	a11, a11
     47a:	c7dc           	bnez.n	a7, 49a <pulse_decode+0x92>
     47c:	fef081        	l32r	a8, 3c (27c8 <aac_dequant_medium>)
     47f:	0008e0        	callx8	a8

00000482 <pulse_decode+0x7a>:
     482:	6070a0        	neg	a7, a10

00000485 <pulse_decode+0x7d>:
     485:	91a8           	l32i.n	a10, a1, 36
     487:	6198           	l32i.n	a9, a1, 24
     489:	0a79           	s32i.n	a7, a10, 0
     48b:	be0992        	l8ui	a9, a9, 190
     48e:	441b           	addi.n	a4, a4, 1
     490:	744040        	extui	a4, a4, 0, 8
     493:	94b947        	bgeu	a9, a4, 42b <pulse_decode+0x23>
     496:	020c           	movi.n	a2, 0
     498:	f01d           	retw.n

0000049a <pulse_decode+0x92>:
     49a:	00f7d6        	bgez	a7, 4ad <pulse_decode+0xa5>
     49d:	607070        	neg	a7, a7
     4a0:	21a9           	s32i.n	a10, a1, 8
     4a2:	01c9           	s32i.n	a12, a1, 0
     4a4:	11b9           	s32i.n	a11, a1, 4
     4a6:	1d0c           	movi.n	a13, 1
     4a8:	81d9           	s32i.n	a13, a1, 32
     4aa:	000246        	j	4b7 <pulse_decode+0xaf>

000004ad <pulse_decode+0xa5>:
     4ad:	21a9           	s32i.n	a10, a1, 8
     4af:	01c9           	s32i.n	a12, a1, 0
     4b1:	11b9           	s32i.n	a11, a1, 4
     4b3:	0e0c           	movi.n	a14, 0
     4b5:	81e9           	s32i.n	a14, a1, 32

000004b7 <pulse_decode+0xaf>:
     4b7:	0a0c           	movi.n	a10, 0
     4b9:	fee181        	l32r	a8, 40 (27a4 <aac_dequant_small>)
     4bc:	11b8           	l32i.n	a11, a1, 4
     4be:	01c8           	l32i.n	a12, a1, 0
     4c0:	0008e0        	callx8	a8
     4c3:	1adc           	bnez.n	a10, 4d8 <pulse_decode+0xd0>
     4c5:	21a8           	l32i.n	a10, a1, 8
     4c7:	fedd81        	l32r	a8, 3c (27c8 <aac_dequant_medium>)
     4ca:	11b8           	l32i.n	a11, a1, 4
     4cc:	01c8           	l32i.n	a12, a1, 0
     4ce:	0008e0        	callx8	a8
     4d1:	0a7d           	mov.n	a7, a10
     4d3:	001bc6        	j	546 <pulse_decode+0x13e>

000004d6 <pulse_decode+0xce>:
	...

000004d8 <pulse_decode+0xd0>:
     4d8:	0acd           	mov.n	a12, a10
     4da:	31dfa0        	srai	a13, a10, 31
     4dd:	fed981        	l32r	a8, 44 (3688 <__udivdi3>)
     4e0:	21ba70        	srai	a11, a7, 10
     4e3:	01a7a0        	slli	a10, a7, 22
     4e6:	0008e0        	callx8	a8
     4e9:	fed7f1        	l32r	a15, 48 (7fffffff <_end+0x7fff6a7f>)
     4ec:	0d0c           	movi.n	a13, 0
     4ee:	5bcc           	bnez.n	a11, 4f7 <pulse_decode+0xef>
     4f0:	1bcc           	bnez.n	a11, 4f5 <pulse_decode+0xed>
     4f2:	013fa7        	bltu	a15, a10, 4f7 <pulse_decode+0xef>

000004f5 <pulse_decode+0xed>:
     4f5:	1d0c           	movi.n	a13, 1

000004f7 <pulse_decode+0xef>:
     4f7:	ddbc           	beqz.n	a13, 538 <pulse_decode+0x130>
     4f9:	fed4e1        	l32r	a14, 4c (200000 <_end+0x1f6a80>)
     4fc:	11b8           	l32i.n	a11, a1, 4
     4fe:	0178           	l32i.n	a7, a1, 0
     500:	1d0c           	movi.n	a13, 1
     502:	6da0f2        	movi	a15, 109
     505:	1eaf76        	loopgtz	a15, 527 <pulse_decode+0x11f>
     508:	a0fd60        	addx4	a15, a13, a6
     50b:	1ff8           	l32i.n	a15, a15, 4
     50d:	3480f0        	extui	a8, a15, 0, 4
     510:	c0ff80        	sub	a15, a15, a8
     513:	c08580        	sub	a8, a5, a8
     516:	400800        	ssr	a8
     519:	91f0f0        	srl	a15, a15
     51c:	8fea           	add.n	a8, a15, a14
     51e:	053a87        	bltu	a10, a8, 527 <pulse_decode+0x11f>
     521:	0fed           	mov.n	a14, a15
     523:	dd1b           	addi.n	a13, a13, 1
     525:	8ddd           	ae_sext16	a13, a13

00000527 <pulse_decode+0x11f>:
     527:	21a8           	l32i.n	a10, a1, 8
     529:	fec481        	l32r	a8, 3c (27c8 <aac_dequant_medium>)
     52c:	07cd           	mov.n	a12, a7
     52e:	aada           	add.n	a10, a10, a13
     530:	0008e0        	callx8	a8
     533:	0a7d           	mov.n	a7, a10
     535:	000346        	j	546 <pulse_decode+0x13e>

00000538 <pulse_decode+0x130>:
     538:	21a8           	l32i.n	a10, a1, 8
     53a:	fec081        	l32r	a8, 3c (27c8 <aac_dequant_medium>)
     53d:	11b8           	l32i.n	a11, a1, 4
     53f:	01c8           	l32i.n	a12, a1, 0
     541:	0008e0        	callx8	a8
     544:	77aa           	add.n	a7, a7, a10
     546:	8198           	l32i.n	a9, a1, 32
     548:	f39916        	beqz	a9, 485 <pulse_decode+0x7d>
     54b:	07ad           	mov.n	a10, a7
     54d:	ffcc46        	j	482 <pulse_decode+0x7a>

00000550 <pulse_decode+0x148>:
     550:	61a022        	movi	a2, 97
     553:	f01d           	retw.n

00000555 <pulse_decode+0x14d>:
     555:	000000                                        ...

00000558 <tns_decode>:
     558:	014136        	entry	a1, 160
     55b:	1d6122        	s32i	a2, a1, 116
     55e:	186132        	s32i	a3, a1, 96
     561:	1a6162        	s32i	a6, a1, 104
     564:	1b6172        	s32i	a7, a1, 108
     567:	858d           	ae_sext16	a8, a5
     569:	0102b2        	l8ui	a11, a2, 1
     56c:	0a0c           	movi.n	a10, 0
     56e:	190c           	movi.n	a9, 1
     570:	fecbb2        	addi	a11, a11, -2
     573:	939ab0        	movnez	a9, a10, a11
     576:	013866        	bnei	a8, 3, 57b <tns_decode+0x23>
     579:	992b           	addi.n	a9, a9, 2

0000057b <tns_decode+0x23>:
     57b:	feb5d1        	l32r	a13, 50 (5e20 <tns_sbf_max>)
     57e:	1d21b2        	l32i	a11, a1, 116
     581:	182162        	l32i	a6, a1, 96
     584:	a0c490        	addx4	a12, a4, a9
     587:	070bb2        	l8ui	a11, a11, 7
     58a:	01d662        	addmi	a6, a6, 0x100
     58d:	90c662        	addi	a6, a6, -112
     590:	90ccd0        	addx2	a12, a12, a13
     593:	2ccd           	ae_l16si.n	a12, a12, 0
     595:	1c61c2        	s32i	a12, a1, 112
     598:	0ebb16        	beqz	a11, 687 <tns_decode+0x12f>
     59b:	080c           	movi.n	a8, 0
     59d:	090c           	movi.n	a9, 0
     59f:	196192        	s32i	a9, a1, 100
     5a2:	1e6182        	s32i	a8, a1, 120

000005a5 <tns_decode+0x4d>:
     5a5:	1821c2        	l32i	a12, a1, 96
     5a8:	1e21a2        	l32i	a10, a1, 120
     5ab:	030c           	movi.n	a3, 0
     5ad:	1d2122        	l32i	a2, a1, 116
     5b0:	aaca           	add.n	a10, a10, a12
     5b2:	1f61a2        	s32i	a10, a1, 124
     5b5:	000aa2        	l8ui	a10, a10, 0
     5b8:	192142        	l32i	a4, a1, 100
     5bb:	050222        	l8ui	a2, a2, 5
     5be:	0afa16        	beqz	a10, 671 <tns_decode+0x119>
     5c1:	44ca           	add.n	a4, a4, a12
     5c3:	027d           	mov.n	a7, a2

000005c5 <tns_decode+0x6d>:
     5c5:	100492        	l8ui	a9, a4, 16
     5c8:	300452        	l8ui	a5, a4, 48
     5cb:	080c           	movi.n	a8, 0
     5cd:	c02290        	sub	a2, a2, a9
     5d0:	532280        	max	a2, a2, a8
     5d3:	481c           	movi.n	a8, 20
     5d5:	435580        	min	a5, a5, a8
     5d8:	083516        	beqz	a5, 65f <tns_decode+0x107>
     5db:	05ad           	mov.n	a10, a5
     5dd:	1f21b2        	l32i	a11, a1, 124
     5e0:	7004c2        	l8ui	a12, a4, 112
     5e3:	06dd           	mov.n	a13, a6
     5e5:	080bb2        	l8ui	a11, a11, 8
     5e8:	fe9b81        	l32r	a8, 54 (1f60 <tns_decode_coef_mod>)
     5eb:	01ed           	mov.n	a14, a1
     5ed:	bb3b           	addi.n	a11, a11, 3
     5ef:	0008e0        	callx8	a8
     5f2:	1d21b2        	l32i	a11, a1, 116
     5f5:	1c2192        	l32i	a9, a1, 112
     5f8:	000bc2        	l8ui	a12, a11, 0
     5fb:	43a790        	min	a10, a7, a9
     5fe:	439290        	min	a9, a2, a9
     601:	4399c0        	min	a9, a9, a12
     604:	43aac0        	min	a10, a10, a12
     607:	9099b0        	addx2	a9, a9, a11
     60a:	801992        	l16ui	a9, a9, 0x100
     60d:	90aab0        	addx2	a10, a10, a11
     610:	801aa2        	l16ui	a10, a10, 0x100
     613:	665a           	add.n	a6, a6, a5
     615:	c0ba90        	sub	a11, a10, a9
     618:	3d1ba6        	blti	a11, 1, 659 <tns_decode+0x101>
     61b:	5004c2        	l8ui	a12, a4, 80
     61e:	1b21d2        	l32i	a13, a1, 108
     621:	8e0c           	movi.n	a14, 8
     623:	3ccc           	bnez.n	a12, 62a <tns_decode+0xd2>
     625:	1c0c           	movi.n	a12, 1
     627:	0000c6        	j	62e <tns_decode+0xd6>

0000062a <tns_decode+0xd2>:
     62a:	9a0b           	addi.n	a9, a10, -1
     62c:	fc7c           	movi.n	a12, -1

0000062e <tns_decode+0xd6>:
     62e:	1e21a2        	l32i	a10, a1, 120
     631:	d2dde0        	quos	a13, a13, a14
     634:	82aad0        	mull	a10, a10, a13
     637:	1a21d2        	l32i	a13, a1, 104
     63a:	a9aa           	add.n	a10, a9, a10
     63c:	a0aad0        	addx4	a10, a10, a13
     63f:	0ce507        	bbsi	a5, 0, 64f <tns_decode+0xf7>
     642:	fe8581        	l32r	a8, 58 (2108 <tns_filter_m0>)
     645:	05ed           	mov.n	a14, a5
     647:	01dd           	mov.n	a13, a1
     649:	0008e0        	callx8	a8
     64c:	000246        	j	659 <tns_decode+0x101>

0000064f <tns_decode+0xf7>:
     64f:	fe8381        	l32r	a8, 5c (2184 <tns_filter_m1>)
     652:	05ed           	mov.n	a14, a5
     654:	01dd           	mov.n	a13, a1
     656:	0008e0        	callx8	a8

00000659 <tns_decode+0x101>:
     659:	1f21a2        	l32i	a10, a1, 124
     65c:	000aa2        	l8ui	a10, a10, 0

0000065f <tns_decode+0x107>:
     65f:	441b           	addi.n	a4, a4, 1
     661:	027d           	mov.n	a7, a2
     663:	331b           	addi.n	a3, a3, 1
     665:	02a3a7        	bge	a3, a10, 66b <tns_decode+0x113>
     668:	ffd646        	j	5c5 <tns_decode+0x6d>
     66b:	1d21b2        	l32i	a11, a1, 116
     66e:	070bb2        	l8ui	a11, a11, 7

00000671 <tns_decode+0x119>:
     671:	1e21c2        	l32i	a12, a1, 120
     674:	1921d2        	l32i	a13, a1, 100
     677:	cc1b           	addi.n	a12, a12, 1
     679:	dd4b           	addi.n	a13, a13, 4
     67b:	1961d2        	s32i	a13, a1, 100
     67e:	1e61c2        	s32i	a12, a1, 120
     681:	02acb7        	bge	a12, a11, 687 <tns_decode+0x12f>
     684:	ffc746        	j	5a5 <tns_decode+0x4d>

00000687 <tns_decode+0x12f>:
     687:	f01d           	retw.n

00000689 <tns_decode+0x131>:
     689:	000000                                        ...

0000068c <aac_open>:
     68c:	004136        	entry	a1, 32
     68f:	02ad           	mov.n	a10, a2
     691:	fe7381        	l32r	a8, 60 (34c0 <xt_memset1>)
     694:	0b0c           	movi.n	a11, 0
     696:	fe67c1        	l32r	a12, 34 (4df0 <vld_huffdata7+0x50>)
     699:	0008e0        	callx8	a8
     69c:	02ad           	mov.n	a10, a2
     69e:	24c292        	addi	a9, a2, 36
     6a1:	2c0c           	movi.n	a12, 2
     6a3:	00a4d2        	movi	a13, 0x400
     6a6:	0b0c           	movi.n	a11, 0
     6a8:	fe6ff1        	l32r	a15, 64 (ac44 <_end+0x16c4>)
     6ab:	074232        	s8i	a3, a2, 7
     6ae:	40d2e2        	addmi	a14, a2, 0x4000
     6b1:	01a082        	movi	a8, 1
     6b4:	504e82        	s8i	a8, a14, 80
     6b7:	584e82        	s8i	a8, a14, 88
     6ba:	156ef2        	s32i	a15, a14, 84
     6bd:	594eb2        	s8i	a11, a14, 89
     6c0:	0642b2        	s8i	a11, a2, 6
     6c3:	0252d2        	s16i	a13, a2, 4
     6c6:	0062b2        	s32i	a11, a2, 0
     6c9:	09ac76        	loopgtz	a12, 6d6 <aac_open+0x4a>
     6cc:	134ab2        	s8i	a11, a10, 19
     6cf:	aa1b           	addi.n	a10, a10, 1
     6d1:	09e9b4        	ae_s32ip	a11, a9, 4
     6d4:	19b9           	s32i.n	a11, a9, 4

000006d6 <aac_open+0x4a>:
     6d6:	f01d           	retw.n

000006d8 <aac_get_config>:
     6d8:	004136        	entry	a1, 32
     6db:	024d           	mov.n	a4, a2
     6dd:	40d232        	addmi	a3, a2, 0x4000
     6e0:	50c332        	addi	a3, a3, 80
     6e3:	020c           	movi.n	a2, 0
     6e5:	932340        	movnez	a2, a3, a4
     6e8:	f01d           	retw.n

000006ea <aac_get_config+0x12>:
	...

000006ec <aac_set_config>:
     6ec:	004136        	entry	a1, 32
     6ef:	928c           	beqz.n	a2, 6fc <aac_set_config+0x10>
     6f1:	738c           	beqz.n	a3, 6fc <aac_set_config+0x10>
     6f3:	0003a2        	l8ui	a10, a3, 0
     6f6:	ffc0a5        	call8	300 <chk_decode_lc>
     6f9:	003ad6        	bgez	a10, 700 <aac_set_config+0x14>

000006fc <aac_set_config+0x10>:
     6fc:	020c           	movi.n	a2, 0
     6fe:	f01d           	retw.n

00000700 <aac_set_config+0x14>:
     700:	40d292        	addmi	a9, a2, 0x4000
     703:	13b8           	l32i.n	a11, a3, 4
     705:	000382        	l8ui	a8, a3, 0
     708:	504982        	s8i	a8, a9, 80
     70b:	fedb16        	beqz	a11, 6fc <aac_set_config+0x10>
     70e:	0803a2        	l8ui	a10, a3, 8
     711:	1569b2        	s32i	a11, a9, 84
     714:	0903b2        	l8ui	a11, a3, 9
     717:	fe1a16        	beqz	a10, 6fc <aac_set_config+0x10>
     71a:	de5af6        	bgeui	a10, 5, 6fc <aac_set_config+0x10>
     71d:	5849a2        	s8i	a10, a9, 88
     720:	d82bf6        	bgeui	a11, 2, 6fc <aac_set_config+0x10>
     723:	120c           	movi.n	a2, 1
     725:	5949b2        	s8i	a11, a9, 89
     728:	f01d           	retw.n

0000072a <aac_set_config+0x3e>:
	...

0000072c <aac_dec_init>:
     72c:	006136        	entry	a1, 48
     72f:	080c           	movi.n	a8, 0
     731:	0189           	s32i.n	a8, a1, 0
     733:	438c           	beqz.n	a3, 73b <aac_dec_init+0xf>
     735:	24cc           	bnez.n	a4, 73b <aac_dec_init+0xf>

00000737 <aac_dec_init+0xb>:
     737:	f27c           	movi.n	a2, -1
     739:	f01d           	retw.n

0000073b <aac_dec_init+0xf>:
     73b:	ff8216        	beqz	a2, 737 <aac_dec_init+0xb>
     73e:	ff5516        	beqz	a5, 737 <aac_dec_init+0xb>
     741:	ff2616        	beqz	a6, 737 <aac_dec_init+0xb>
     744:	03bd           	mov.n	a11, a3
     746:	04cd           	mov.n	a12, a4
     748:	01fd           	mov.n	a15, a1
     74a:	fe4881        	l32r	a8, 6c (78c <aac_dec_init_buffer>)
     74d:	190c           	movi.n	a9, 1
     74f:	fe46a1        	l32r	a10, 68 (5eb0 <sample_rates>)
     752:	40d2d2        	addmi	a13, a2, 0x4000
     755:	4e0c           	movi.n	a14, 4
     757:	0842e2        	s8i	a14, a2, 8
     75a:	500dd2        	l8ui	a13, a13, 80
     75d:	0942d2        	s8i	a13, a2, 9
     760:	06ed           	mov.n	a14, a6
     762:	05dd           	mov.n	a13, a5
     764:	4aa8           	l32i.n	a10, a10, 16
     766:	05a9           	s32i.n	a10, a5, 0
     768:	004692        	s8i	a9, a6, 0
     76b:	02ad           	mov.n	a10, a2
     76d:	0008e0        	callx8	a8
     770:	030076        	bf	b0, 777 <aac_dec_init+0x4b>
     773:	0128           	l32i.n	a2, a1, 0
     775:	f01d           	retw.n

00000777 <aac_dec_init+0x4b>:
     777:	0902a2        	l8ui	a10, a2, 9
     77a:	0006f2        	l8ui	a15, a6, 0
     77d:	0a42f2        	s8i	a15, a2, 10
     780:	ffb7e5        	call8	300 <chk_decode_lc>
     783:	fb0a96        	bltz	a10, 737 <aac_dec_init+0xb>
     786:	020c           	movi.n	a2, 0
     788:	f01d           	retw.n

0000078a <aac_dec_init+0x5e>:
	...

0000078c <aac_dec_init_buffer>:
     78c:	00c136        	entry	a1, 96
     78f:	33cc           	bnez.n	a3, 796 <aac_dec_init_buffer+0xa>
     791:	120000        	andbc	b0, b0, b0
     794:	f01d           	retw.n

00000796 <aac_dec_init_buffer+0xa>:
     796:	03bd           	mov.n	a11, a3
     798:	01ad           	mov.n	a10, a1
     79a:	fe3581        	l32r	a8, 70 (2240 <aac_initbits>)
     79d:	04cd           	mov.n	a12, a4
     79f:	d159           	s32i.n	a5, a1, 52
     7a1:	0008e0        	callx8	a8
     7a4:	0b0c           	movi.n	a11, 0
     7a6:	0c5c           	movi.n	a12, 80
     7a8:	40d2a2        	addmi	a10, a2, 0x4000
     7ab:	fe2d81        	l32r	a8, 60 (34c0 <xt_memset1>)
     7ae:	5ccaa2        	addi	a10, a10, 92
     7b1:	e1a9           	s32i.n	a10, a1, 56
     7b3:	0008e0        	callx8	a8
     7b6:	845d           	ae_sext16	a5, a4
     7b8:	030c           	movi.n	a3, 0
     7ba:	000686        	j	7d8 <aac_dec_init_buffer+0x4c>

000007bd <aac_dec_init_buffer+0x31>:
     7bd:	05dd           	mov.n	a13, a5
     7bf:	e1a8           	l32i.n	a10, a1, 56
     7c1:	fe2c81        	l32r	a8, 74 (1ec4 <aac_loas_frame>)
     7c4:	01bd           	mov.n	a11, a1
     7c6:	c1cb           	addi.n	a12, a1, 12
     7c8:	0008e0        	callx8	a8
     7cb:	fe2b81        	l32r	a8, 78 (22b8 <aac_get_processed_bits>)
     7ce:	0a4d           	mov.n	a4, a10
     7d0:	01ad           	mov.n	a10, a1
     7d2:	0008e0        	callx8	a8
     7d5:	780466        	bnei	a4, -1, 851 <aac_dec_init_buffer+0xc5>

000007d8 <aac_dec_init_buffer+0x4c>:
     7d8:	fe2981        	l32r	a8, 7c (22f0 <aac_used_bytes>)
     7db:	01ad           	mov.n	a10, a1
     7dd:	0008e0        	callx8	a8
     7e0:	2198           	l32i.n	a9, a1, 8
     7e2:	040c           	movi.n	a4, 0
     7e4:	d52a97        	blt	a10, a9, 7bd <aac_dec_init_buffer+0x31>
     7e7:	01ad           	mov.n	a10, a1
     7e9:	0b0c           	movi.n	a11, 0
     7eb:	fe2581        	l32r	a8, 80 (22d0 <aac_resetbits>)
     7ee:	40d252        	addmi	a5, a2, 0x4000
     7f1:	5c4542        	s8i	a4, a5, 92
     7f4:	315542        	s16i	a4, a5, 98
     7f7:	dcc552        	addi	a5, a5, -36
     7fa:	0008e0        	callx8	a8
     7fd:	062316        	beqz	a3, 863 <aac_dec_init_buffer+0xd7>
     800:	332592        	l32i	a9, a5, 204
     803:	130c           	movi.n	a3, 1
     805:	05a916        	beqz	a9, 863 <aac_dec_init_buffer+0xd7>
     808:	064232        	s8i	a3, a2, 6
     80b:	a97b           	addi.n	a10, a9, 7
     80d:	41a3a0        	srli	a10, a10, 3
     810:	612ab6        	bltui	a10, 2, 875 <aac_dec_init_buffer+0xe9>
     813:	8acd           	ae_sext16	a12, a10
     815:	40d2b2        	addmi	a11, a2, 0x4000
     818:	fe1681        	l32r	a8, 70 (2240 <aac_initbits>)
     81b:	68cbb2        	addi	a11, a11, 104
     81e:	10c1a2        	addi	a10, a1, 16
     821:	0008e0        	callx8	a8
     824:	fe1881        	l32r	a8, 84 (2298 <aac_byte_align>)
     827:	10c1a2        	addi	a10, a1, 16
     82a:	0008e0        	callx8	a8
     82d:	10c1a2        	addi	a10, a1, 16
     830:	1cc1b2        	addi	a11, a1, 28
     833:	40d2c2        	addmi	a12, a2, 0x4000
     836:	39ccc2        	addi	a12, a12, 57
     839:	0019a5        	call8	9d4 <aac_specific_config>
     83c:	240192        	l8ui	a9, a1, 36
     83f:	040956        	bnez	a9, 883 <aac_dec_init_buffer+0xf7>
     842:	40d292        	addmi	a9, a2, 0x4000
     845:	3c09b2        	l8ui	a11, a9, 60
     848:	0046b2        	s8i	a11, a6, 0
     84b:	384932        	s8i	a3, a9, 56
     84e:	000d06        	j	886 <aac_dec_init_buffer+0xfa>

00000851 <aac_dec_init_buffer+0xc5>:
     851:	331b           	addi.n	a3, a3, 1
     853:	f81416        	beqz	a4, 7d8 <aac_dec_init_buffer+0x4c>
     856:	fe0a81        	l32r	a8, 80 (22d0 <aac_resetbits>)
     859:	b4aa           	add.n	a11, a4, a10
     85b:	01ad           	mov.n	a10, a1
     85d:	0008e0        	callx8	a8
     860:	ffdd06        	j	7d8 <aac_dec_init_buffer+0x4c>

00000863 <aac_dec_init_buffer+0xd7>:
     863:	120000        	andbc	b0, b0, b0
     866:	046407        	bbci	a4, 0, 86e <aac_dec_init_buffer+0xe2>
     869:	320000        	orbc	b0, b0, b0
     86c:	f01d           	retw.n

0000086e <aac_dec_init_buffer+0xe2>:
     86e:	f01d           	retw.n

00000870 <aac_dec_init_buffer+0xe4>:
     870:	0a0c           	movi.n	a10, 0
     872:	000046        	j	877 <aac_dec_init_buffer+0xeb>

00000875 <aac_dec_init_buffer+0xe9>:
     875:	fa7c           	movi.n	a10, -1

00000877 <aac_dec_init_buffer+0xeb>:
     877:	07a9           	s32i.n	a10, a7, 0
     879:	320000        	orbc	b0, b0, b0
     87c:	1a8c           	beqz.n	a10, 881 <aac_dec_init_buffer+0xf5>
     87e:	064242        	s8i	a4, a2, 6

00000881 <aac_dec_init_buffer+0xf5>:
     881:	f01d           	retw.n

00000883 <aac_dec_init_buffer+0xf7>:
     883:	004692        	s8i	a9, a6, 0

00000886 <aac_dec_init_buffer+0xfa>:
     886:	d198           	l32i.n	a9, a1, 52
     888:	8188           	l32i.n	a8, a1, 32
     88a:	0989           	s32i.n	a8, a9, 0
     88c:	1c01e2        	l8ui	a14, a1, 28
     88f:	1d01f2        	l8ui	a15, a1, 29
     892:	0842f2        	s8i	a15, a2, 8
     895:	0942e2        	s8i	a14, a2, 9
     898:	fdba56        	bnez	a10, 877 <aac_dec_init_buffer+0xeb>
     89b:	2501c2        	l8ui	a12, a1, 37
     89e:	2401d2        	l8ui	a13, a1, 36
     8a1:	0a42d2        	s8i	a13, a2, 10
     8a4:	fcdc56        	bnez	a12, 875 <aac_dec_init_buffer+0xe9>
     8a7:	fff146        	j	870 <aac_dec_init_buffer+0xe4>

000008aa <aac_dec_init_buffer+0x11e>:
	...

000008ac <aac_dec_decode>:
     8ac:	006136        	entry	a1, 48
     8af:	328c           	beqz.n	a2, 8b6 <aac_dec_decode+0xa>
     8b1:	138c           	beqz.n	a3, 8b6 <aac_dec_decode+0xa>
     8b3:	043456        	bnez	a4, 8fa <aac_dec_decode+0x4e>
     8b6:	fea092        	movi	a9, 254

000008b9 <aac_dec_decode+0xd>:
     8b9:	094392        	s8i	a9, a3, 9

000008bc <aac_dec_decode+0x10>:
     8bc:	190c           	movi.n	a9, 1
     8be:	000d06        	j	8f6 <aac_dec_decode+0x4a>

000008c1 <aac_dec_decode+0x15>:
     8c1:	3399           	s32i.n	a9, a3, 12
     8c3:	0843c2        	s8i	a12, a3, 8
     8c6:	060282        	l8ui	a8, a2, 6
     8c9:	82ac60        	mull	a10, a12, a6
     8cc:	13a9           	s32i.n	a10, a3, 4
     8ce:	688c           	beqz.n	a8, 8d8 <aac_dec_decode+0x2c>
     8d0:	069192        	l16si	a9, a1, 12
     8d3:	0149a6        	blti	a9, 4, 8d8 <aac_dec_decode+0x2c>
     8d6:	0399           	s32i.n	a9, a3, 0
     8d8:	1202b2        	l8ui	a11, a2, 18
     8db:	021bc7        	beq	a11, a12, 8e1 <aac_dec_decode+0x35>
     8de:	1242c2        	s8i	a12, a2, 18

000008e1 <aac_dec_decode+0x35>:
     8e1:	06dd           	mov.n	a13, a6
     8e3:	fde981        	l32r	a8, 88 (aac <output_to_PCM>)
     8e6:	02ad           	mov.n	a10, a2
     8e8:	24c2b2        	addi	a11, a2, 36
     8eb:	0008e0        	callx8	a8
     8ee:	02a8           	l32i.n	a10, a2, 0
     8f0:	090c           	movi.n	a9, 0
     8f2:	aa1b           	addi.n	a10, a10, 1
     8f4:	02a9           	s32i.n	a10, a2, 0

000008f6 <aac_dec_decode+0x4a>:
     8f6:	092d           	mov.n	a2, a9
     8f8:	f01d           	retw.n

000008fa <aac_dec_decode+0x4e>:
     8fa:	021262        	l16ui	a6, a2, 4
     8fd:	03ad           	mov.n	a10, a3
     8ff:	fdd881        	l32r	a8, 60 (34c0 <xt_memset1>)
     902:	0b0c           	movi.n	a11, 0
     904:	8c1c           	movi.n	a12, 24
     906:	0008e0        	callx8	a8
     909:	0b0c           	movi.n	a11, 0
     90b:	2c0c           	movi.n	a12, 2
     90d:	fdd481        	l32r	a8, 60 (34c0 <xt_memset1>)
     910:	40d2a2        	addmi	a10, a2, 0x4000
     913:	4dcaa2        	addi	a10, a10, 77
     916:	0008e0        	callx8	a8
     919:	35cc           	bnez.n	a5, 920 <aac_dec_decode+0x74>
     91b:	c91c           	movi.n	a9, 28
     91d:	ffe606        	j	8b9 <aac_dec_decode+0xd>

00000920 <aac_dec_decode+0x74>:
     920:	04bd           	mov.n	a11, a4
     922:	fdd381        	l32r	a8, 70 (2240 <aac_initbits>)
     925:	01ad           	mov.n	a10, a1
     927:	05cd           	mov.n	a12, a5
     929:	0008e0        	callx8	a8
     92c:	060292        	l8ui	a9, a2, 6
     92f:	79ac           	beqz.n	a9, 95a <aac_dec_decode+0xae>
     931:	01bd           	mov.n	a11, a1
     933:	c1cb           	addi.n	a12, a1, 12
     935:	05dd           	mov.n	a13, a5
     937:	fdcf81        	l32r	a8, 74 (1ec4 <aac_loas_frame>)
     93a:	40d2a2        	addmi	a10, a2, 0x4000
     93d:	5ccaa2        	addi	a10, a10, 92
     940:	0008e0        	callx8	a8
     943:	fdcd81        	l32r	a8, 78 (22b8 <aac_get_processed_bits>)
     946:	0a4d           	mov.n	a4, a10
     948:	01ad           	mov.n	a10, a1
     94a:	0008e0        	callx8	a8
     94d:	090466        	bnei	a4, -1, 95a <aac_dec_decode+0xae>
     950:	0359           	s32i.n	a5, a3, 0
     952:	d91c           	movi.n	a9, 29
     954:	094392        	s8i	a9, a3, 9
     957:	ffd846        	j	8bc <aac_dec_decode+0x10>

0000095a <aac_dec_decode+0xae>:
     95a:	02ad           	mov.n	a10, a2
     95c:	03bd           	mov.n	a11, a3
     95e:	01cd           	mov.n	a12, a1
     960:	fdcb81        	l32r	a8, 8c (e90 <raw_data_block>)
     963:	40d2d2        	addmi	a13, a2, 0x4000
     966:	39cdd2        	addi	a13, a13, 57
     969:	0008e0        	callx8	a8
     96c:	060292        	l8ui	a9, a2, 6
     96f:	899c           	beqz.n	a9, 98b <aac_dec_decode+0xdf>
     971:	fdc181        	l32r	a8, 78 (22b8 <aac_get_processed_bits>)
     974:	01ad           	mov.n	a10, a1
     976:	0008e0        	callx8	a8
     979:	40d2a2        	addmi	a10, a2, 0x4000
     97c:	192aa2        	l32i	a10, a10, 100
     97f:	8a8c           	beqz.n	a10, 98b <aac_dec_decode+0xdf>
     981:	fdc381        	l32r	a8, 90 (2254 <aac_getbits>)
     984:	8abd           	ae_sext16	a11, a10
     986:	01ad           	mov.n	a10, a1
     988:	0008e0        	callx8	a8
     98b:	0903a2        	l8ui	a10, a3, 9
     98e:	0d0292        	l8ui	a9, a2, 13
     991:	f27a56        	bnez	a10, 8bc <aac_dec_decode+0x10>
     994:	29bc           	beqz.n	a9, 9ca <aac_dec_decode+0x11e>
     996:	3039e6        	bgei	a9, 3, 9ca <aac_dec_decode+0x11e>
     999:	fdb781        	l32r	a8, 78 (22b8 <aac_get_processed_bits>)
     99c:	01ad           	mov.n	a10, a1
     99e:	0008e0        	callx8	a8
     9a1:	0902b2        	l8ui	a11, a2, 9
     9a4:	090c           	movi.n	a9, 0
     9a6:	ca7b           	addi.n	a12, a10, 7
     9a8:	41c3c0        	srli	a12, a12, 3
     9ab:	0802a2        	l8ui	a10, a2, 8
     9ae:	03c9           	s32i.n	a12, a3, 0
     9b0:	0702c2        	l8ui	a12, a2, 7
     9b3:	0b4292        	s8i	a9, a2, 11
     9b6:	1043b2        	s8i	a11, a3, 16
     9b9:	02aab6        	bltui	a10, 12, 9bf <aac_dec_decode+0x113>
     9bc:	ffc046        	j	8c1 <aac_dec_decode+0x15>
     9bf:	fdaa91        	l32r	a9, 68 (5eb0 <sample_rates>)
     9c2:	a09a90        	addx4	a9, a10, a9
     9c5:	0998           	l32i.n	a9, a9, 0
     9c7:	ffbd86        	j	8c1 <aac_dec_decode+0x15>

000009ca <aac_dec_decode+0x11e>:
     9ca:	ca0c           	movi.n	a10, 12
     9cc:	0943a2        	s8i	a10, a3, 9
     9cf:	ffba46        	j	8bc <aac_dec_decode+0x10>

000009d2 <aac_dec_decode+0x126>:
	...

000009d4 <aac_specific_config>:
     9d4:	008136        	entry	a1, 64
     9d7:	23cc           	bnez.n	a3, 9dd <aac_specific_config+0x9>
     9d9:	827c           	movi.n	a2, -8
     9db:	f01d           	retw.n

000009dd <aac_specific_config+0x9>:
     9dd:	03ad           	mov.n	a10, a3
     9df:	fda081        	l32r	a8, 60 (34c0 <xt_memset1>)
     9e2:	0b0c           	movi.n	a11, 0
     9e4:	8c1c           	movi.n	a12, 24
     9e6:	0008e0        	callx8	a8
     9e9:	fda981        	l32r	a8, 90 (2254 <aac_getbits>)
     9ec:	02ad           	mov.n	a10, a2
     9ee:	db0c           	movi.n	a11, 13
     9f0:	0008e0        	callx8	a8
     9f3:	fda8d1        	l32r	a13, 94 (4130 <object_types>)
     9f6:	34b4a0        	extui	a11, a10, 4, 4
     9f9:	3490a0        	extui	a9, a10, 0, 4
     9fc:	084392        	s8i	a9, a3, 8
     9ff:	0143b2        	s8i	a11, a3, 1
     a02:	4198a0        	srli	a9, a10, 8
     a05:	004392        	s8i	a9, a3, 0
     a08:	749090        	extui	a9, a9, 0, 8
     a0b:	d9da           	add.n	a13, a9, a13
     a0d:	0aabf6        	bgeui	a11, 12, a1b <aac_specific_config+0x47>
     a10:	fd96c1        	l32r	a12, 68 (5eb0 <sample_rates>)
     a13:	a0bbc0        	addx4	a11, a11, a12
     a16:	0bb8           	l32i.n	a11, a11, 0
     a18:	000046        	j	a1d <aac_specific_config+0x49>

00000a1b <aac_specific_config+0x47>:
     a1b:	0b0c           	movi.n	a11, 0

00000a1d <aac_specific_config+0x49>:
     a1d:	13b9           	s32i.n	a11, a3, 4
     a1f:	000dd2        	l8ui	a13, a13, 0
     a22:	031d26        	beqi	a13, 1, a29 <aac_specific_config+0x55>
     a25:	f27c           	movi.n	a2, -1
     a27:	f01d           	retw.n

00000a29 <aac_specific_config+0x55>:
     a29:	2bcc           	bnez.n	a11, a2f <aac_specific_config+0x5b>
     a2b:	e27c           	movi.n	a2, -2
     a2d:	f01d           	retw.n

00000a2f <aac_specific_config+0x5b>:
     a2f:	34e0a0        	extui	a14, a10, 0, 4
     a32:	038eb6        	bltui	a14, 8, a39 <aac_specific_config+0x65>
     a35:	d27c           	movi.n	a2, -3
     a37:	f01d           	retw.n

00000a39 <aac_specific_config+0x65>:
     a39:	241926        	beqi	a9, 1, a61 <aac_specific_config+0x8d>
     a3c:	212926        	beqi	a9, 2, a61 <aac_specific_config+0x8d>
     a3f:	1e3926        	beqi	a9, 3, a61 <aac_specific_config+0x8d>
     a42:	1b4926        	beqi	a9, 4, a61 <aac_specific_config+0x8d>
     a45:	186926        	beqi	a9, 6, a61 <aac_specific_config+0x8d>
     a48:	157926        	beqi	a9, 7, a61 <aac_specific_config+0x8d>
     a4b:	c27c           	movi.n	a2, -4
     a4d:	f01d           	retw.n

00000a4f <aac_specific_config+0x7b>:
     a4f:	a48c           	beqz.n	a4, a5d <aac_specific_config+0x89>
     a51:	04ad           	mov.n	a10, a4
     a53:	fd9181        	l32r	a8, 98 (2e0 <xt_memcpy1>)
     a56:	01bd           	mov.n	a11, a1
     a58:	2c1c           	movi.n	a12, 18
     a5a:	0008e0        	callx8	a8

00000a5d <aac_specific_config+0x89>:
     a5d:	020c           	movi.n	a2, 0
     a5f:	f01d           	retw.n

00000a61 <aac_specific_config+0x8d>:
     a61:	fd8b81        	l32r	a8, 90 (2254 <aac_getbits>)
     a64:	02ad           	mov.n	a10, a2
     a66:	2b0c           	movi.n	a11, 2
     a68:	0008e0        	callx8	a8
     a6b:	7491a0        	extui	a9, a10, 1, 8
     a6e:	41b1a0        	srli	a11, a10, 1
     a71:	0943b2        	s8i	a11, a3, 9
     a74:	301926        	beqi	a9, 1, aa8 <aac_specific_config+0xd4>
     a77:	04c0a0        	extui	a12, a10, 0, 1
     a7a:	0a43c2        	s8i	a12, a3, 10
     a7d:	0c6a07        	bbci	a10, 0, a8d <aac_specific_config+0xb9>
     a80:	fd8481        	l32r	a8, 90 (2254 <aac_getbits>)
     a83:	02ad           	mov.n	a10, a2
     a85:	eb0c           	movi.n	a11, 14
     a87:	0008e0        	callx8	a8
     a8a:	0653a2        	s16i	a10, a3, 12

00000a8d <aac_specific_config+0xb9>:
     a8d:	fd8381        	l32r	a8, 9c (2284 <aac_get1bit>)
     a90:	02ad           	mov.n	a10, a2
     a92:	0008e0        	callx8	a8
     a95:	080392        	l8ui	a9, a3, 8
     a98:	0e43a2        	s8i	a10, a3, 14
     a9b:	fbe956        	bnez	a9, a5d <aac_specific_config+0x89>
     a9e:	02bd           	mov.n	a11, a2
     aa0:	01ad           	mov.n	a10, a1
     aa2:	000865        	call8	b28 <loas_program_config_element>
     aa5:	a60076        	bf	b0, a4f <aac_specific_config+0x7b>

00000aa8 <aac_specific_config+0xd4>:
     aa8:	d27c           	movi.n	a2, -3
     aaa:	f01d           	retw.n

00000aac <output_to_PCM>:
     aac:	004136        	entry	a1, 32
     aaf:	21a150        	srai	a10, a5, 1
     ab2:	6278           	l32i.n	a7, a2, 24
     ab4:	40d282        	addmi	a8, a2, 0x4000
     ab7:	4d0892        	l8ui	a9, a8, 77
     aba:	4e0882        	l8ui	a8, a8, 78
     abd:	fcc772        	addi	a7, a7, -4
     ac0:	a09930        	addx4	a9, a9, a3
     ac3:	0998           	l32i.n	a9, a9, 0
     ac5:	a08830        	addx4	a8, a8, a3
     ac8:	0888           	l32i.n	a8, a8, 0
     aca:	fcc932        	addi	a3, a9, -4
     acd:	15aa76        	loopgtz	a10, ae6 <output_to_PCM+0x3a>
     ad0:	605314        	ae_lq32f.iu	aeq1, a3, 4
     ad3:	60d314        	ae_lq32f.iu	aeq3, a3, 4
     ad6:	361094        	ae_sllisq56s	aeq0, aeq1, 2
     ad9:	fe447af06f 	{ ae_sllisq56s	aeq1, aeq3, 2; ae_roundsq32sym	aeq2, aeq0 }
     ade:	fc4c79e71f 	{ ae_sq32f.iu	aeq2, a7, 4; ae_roundsq32sym	aeq0, aeq1 }
     ae3:	624714        	ae_sq32f.iu	aeq0, a7, 4

00000ae6 <output_to_PCM+0x3a>:
     ae6:	3c24a6        	blti	a4, 2, b26 <output_to_PCM+0x7a>
     ae9:	231987        	beq	a9, a8, b10 <output_to_PCM+0x64>
     aec:	072272        	l32i	a7, a2, 28
     aef:	fcc832        	addi	a3, a8, -4
     af2:	fcc772        	addi	a7, a7, -4
     af5:	15aa76        	loopgtz	a10, b0e <output_to_PCM+0x62>
     af8:	60d314        	ae_lq32f.iu	aeq3, a3, 4
     afb:	605314        	ae_lq32f.iu	aeq1, a3, 4
     afe:	36b094        	ae_sllisq56s	aeq2, aeq3, 2
     b01:	fc547ad0ef 	{ ae_sllisq56s	aeq3, aeq1, 2; ae_roundsq32sym	aeq0, aeq2 }
     b06:	fe5c79c71f 	{ ae_sq32f.iu	aeq0, a7, 4; ae_roundsq32sym	aeq2, aeq3 }
     b0b:	626714        	ae_sq32f.iu	aeq2, a7, 4
     b0e:	f01d           	retw.n

00000b10 <output_to_PCM+0x64>:
     b10:	1215a6        	blti	a5, 1, b26 <output_to_PCM+0x7a>
     b13:	7278           	l32i.n	a7, a2, 28
     b15:	083d           	mov.n	a3, a8
     b17:	020c           	movi.n	a2, 0
     b19:	09a576        	loopgtz	a5, b26 <output_to_PCM+0x7a>
     b1c:	972a           	add.n	a9, a7, a2
     b1e:	0388           	l32i.n	a8, a3, 0
     b20:	224b           	addi.n	a2, a2, 4
     b22:	334b           	addi.n	a3, a3, 4
     b24:	0989           	s32i.n	a8, a9, 0
     b26:	f01d           	retw.n

00000b28 <loas_program_config_element>:
     b28:	004136        	entry	a1, 32
     b2b:	02ad           	mov.n	a10, a2
     b2d:	fd4c81        	l32r	a8, 60 (34c0 <xt_memset1>)
     b30:	0b0c           	movi.n	a11, 0
     b32:	2c1c           	movi.n	a12, 18
     b34:	0008e0        	callx8	a8
     b37:	03ad           	mov.n	a10, a3
     b39:	8b1c           	movi.n	a11, 24
     b3b:	fd5581        	l32r	a8, 90 (2254 <aac_getbits>)
     b3e:	070c           	movi.n	a7, 0
     b40:	034272        	s8i	a7, a2, 3
     b43:	0008e0        	callx8	a8
     b46:	b584a0        	extui	a8, a10, 20, 12
     b49:	34b2a0        	extui	a11, a10, 2, 4
     b4c:	1490a0        	extui	a9, a10, 0, 2
     b4f:	34c6a0        	extui	a12, a10, 6, 4
     b52:	34daa0        	extui	a13, a10, 10, 4
     b55:	34eea0        	extui	a14, a10, 14, 4
     b58:	15f2a0        	extui	a15, a10, 18, 2
     b5b:	0142f2        	s8i	a15, a2, 1
     b5e:	0242e2        	s8i	a14, a2, 2
     b61:	0442d2        	s8i	a13, a2, 4
     b64:	0542c2        	s8i	a12, a2, 5
     b67:	074292        	s8i	a9, a2, 7
     b6a:	0642b2        	s8i	a11, a2, 6
     b6d:	004282        	s8i	a8, a2, 0
     b70:	fd4881        	l32r	a8, 90 (2254 <aac_getbits>)
     b73:	8b0c           	movi.n	a11, 8
     b75:	03ad           	mov.n	a10, a3
     b77:	0008e0        	callx8	a8
     b7a:	160c           	movi.n	a6, 1
     b7c:	0490a0        	extui	a9, a10, 0, 1
     b7f:	34b1a0        	extui	a11, a10, 1, 4
     b82:	41c5a0        	srli	a12, a10, 5
     b85:	0842c2        	s8i	a12, a2, 8
     b88:	0942b2        	s8i	a11, a2, 9
     b8b:	0a4292        	s8i	a9, a2, 10
     b8e:	096a07        	bbci	a10, 0, b9b <loas_program_config_element+0x73>
     b91:	fd3f81        	l32r	a8, 90 (2254 <aac_getbits>)
     b94:	03ad           	mov.n	a10, a3
     b96:	4b0c           	movi.n	a11, 4
     b98:	0008e0        	callx8	a8

00000b9b <loas_program_config_element+0x73>:
     b9b:	fd4081        	l32r	a8, 9c (2284 <aac_get1bit>)
     b9e:	03ad           	mov.n	a10, a3
     ba0:	0008e0        	callx8	a8
     ba3:	0b42a2        	s8i	a10, a2, 11
     ba6:	7490a0        	extui	a9, a10, 0, 8
     ba9:	091966        	bnei	a9, 1, bb6 <loas_program_config_element+0x8e>
     bac:	fd3981        	l32r	a8, 90 (2254 <aac_getbits>)
     baf:	03ad           	mov.n	a10, a3
     bb1:	4b0c           	movi.n	a11, 4
     bb3:	0008e0        	callx8	a8

00000bb6 <loas_program_config_element+0x8e>:
     bb6:	fd3981        	l32r	a8, 9c (2284 <aac_get1bit>)
     bb9:	03ad           	mov.n	a10, a3
     bbb:	0008e0        	callx8	a8
     bbe:	0c42a2        	s8i	a10, a2, 12
     bc1:	7490a0        	extui	a9, a10, 0, 8
     bc4:	091966        	bnei	a9, 1, bd1 <loas_program_config_element+0xa9>
     bc7:	fd3281        	l32r	a8, 90 (2254 <aac_getbits>)
     bca:	03ad           	mov.n	a10, a3
     bcc:	3b0c           	movi.n	a11, 3
     bce:	0008e0        	callx8	a8

00000bd1 <loas_program_config_element+0xa9>:
     bd1:	040292        	l8ui	a9, a2, 4
     bd4:	250c           	movi.n	a5, 2
     bd6:	040c           	movi.n	a4, 0
     bd8:	2919a6        	blti	a9, 1, c05 <loas_program_config_element+0xdd>

00000bdb <loas_program_config_element+0xb3>:
     bdb:	fd2d81        	l32r	a8, 90 (2254 <aac_getbits>)
     bde:	03ad           	mov.n	a10, a3
     be0:	5b0c           	movi.n	a11, 5
     be2:	0008e0        	callx8	a8
     be5:	040292        	l8ui	a9, a2, 4
     be8:	441b           	addi.n	a4, a4, 1
     bea:	0e02c2        	l8ui	a12, a2, 14
     bed:	04d4a0        	extui	a13, a10, 4, 1
     bf0:	06bd           	mov.n	a11, a6
     bf2:	0302a2        	l8ui	a10, a2, 3
     bf5:	93b5d0        	movnez	a11, a5, a13
     bf8:	ccba           	add.n	a12, a12, a11
     bfa:	0e42c2        	s8i	a12, a2, 14
     bfd:	aaba           	add.n	a10, a10, a11
     bff:	0342a2        	s8i	a10, a2, 3
     c02:	d52497        	blt	a4, a9, bdb <loas_program_config_element+0xb3>

00000c05 <loas_program_config_element+0xdd>:
     c05:	0502e2        	l8ui	a14, a2, 5
     c08:	040c           	movi.n	a4, 0
     c0a:	291ea6        	blti	a14, 1, c37 <loas_program_config_element+0x10f>

00000c0d <loas_program_config_element+0xe5>:
     c0d:	fd2081        	l32r	a8, 90 (2254 <aac_getbits>)
     c10:	03ad           	mov.n	a10, a3
     c12:	5b0c           	movi.n	a11, 5
     c14:	0008e0        	callx8	a8
     c17:	050292        	l8ui	a9, a2, 5
     c1a:	441b           	addi.n	a4, a4, 1
     c1c:	0f02c2        	l8ui	a12, a2, 15
     c1f:	04d4a0        	extui	a13, a10, 4, 1
     c22:	06bd           	mov.n	a11, a6
     c24:	0302a2        	l8ui	a10, a2, 3
     c27:	93b5d0        	movnez	a11, a5, a13
     c2a:	ccba           	add.n	a12, a12, a11
     c2c:	0f42c2        	s8i	a12, a2, 15
     c2f:	aaba           	add.n	a10, a10, a11
     c31:	0342a2        	s8i	a10, a2, 3
     c34:	d52497        	blt	a4, a9, c0d <loas_program_config_element+0xe5>

00000c37 <loas_program_config_element+0x10f>:
     c37:	0602e2        	l8ui	a14, a2, 6
     c3a:	040c           	movi.n	a4, 0
     c3c:	291ea6        	blti	a14, 1, c69 <loas_program_config_element+0x141>

00000c3f <loas_program_config_element+0x117>:
     c3f:	fd1481        	l32r	a8, 90 (2254 <aac_getbits>)
     c42:	03ad           	mov.n	a10, a3
     c44:	5b0c           	movi.n	a11, 5
     c46:	0008e0        	callx8	a8
     c49:	060292        	l8ui	a9, a2, 6
     c4c:	441b           	addi.n	a4, a4, 1
     c4e:	1002c2        	l8ui	a12, a2, 16
     c51:	04d4a0        	extui	a13, a10, 4, 1
     c54:	06bd           	mov.n	a11, a6
     c56:	0302a2        	l8ui	a10, a2, 3
     c59:	93b5d0        	movnez	a11, a5, a13
     c5c:	ccba           	add.n	a12, a12, a11
     c5e:	1042c2        	s8i	a12, a2, 16
     c61:	aaba           	add.n	a10, a10, a11
     c63:	0342a2        	s8i	a10, a2, 3
     c66:	d52497        	blt	a4, a9, c3f <loas_program_config_element+0x117>

00000c69 <loas_program_config_element+0x141>:
     c69:	0702e2        	l8ui	a14, a2, 7
     c6c:	040c           	movi.n	a4, 0
     c6e:	211ea6        	blti	a14, 1, c93 <loas_program_config_element+0x16b>

00000c71 <loas_program_config_element+0x149>:
     c71:	fd0781        	l32r	a8, 90 (2254 <aac_getbits>)
     c74:	03ad           	mov.n	a10, a3
     c76:	4b0c           	movi.n	a11, 4
     c78:	0008e0        	callx8	a8
     c7b:	070292        	l8ui	a9, a2, 7
     c7e:	0302a2        	l8ui	a10, a2, 3
     c81:	1102b2        	l8ui	a11, a2, 17
     c84:	441b           	addi.n	a4, a4, 1
     c86:	aa1b           	addi.n	a10, a10, 1
     c88:	bb1b           	addi.n	a11, a11, 1
     c8a:	1142b2        	s8i	a11, a2, 17
     c8d:	0342a2        	s8i	a10, a2, 3
     c90:	dd2497        	blt	a4, a9, c71 <loas_program_config_element+0x149>

00000c93 <loas_program_config_element+0x16b>:
     c93:	0802b2        	l8ui	a11, a2, 8
     c96:	040c           	movi.n	a4, 0
     c98:	111ba6        	blti	a11, 1, cad <loas_program_config_element+0x185>

00000c9b <loas_program_config_element+0x173>:
     c9b:	fcfd81        	l32r	a8, 90 (2254 <aac_getbits>)
     c9e:	03ad           	mov.n	a10, a3
     ca0:	4b0c           	movi.n	a11, 4
     ca2:	0008e0        	callx8	a8
     ca5:	080292        	l8ui	a9, a2, 8
     ca8:	441b           	addi.n	a4, a4, 1
     caa:	ed2497        	blt	a4, a9, c9b <loas_program_config_element+0x173>

00000cad <loas_program_config_element+0x185>:
     cad:	0902a2        	l8ui	a10, a2, 9
     cb0:	040c           	movi.n	a4, 0
     cb2:	111aa6        	blti	a10, 1, cc7 <loas_program_config_element+0x19f>

00000cb5 <loas_program_config_element+0x18d>:
     cb5:	fcf681        	l32r	a8, 90 (2254 <aac_getbits>)
     cb8:	03ad           	mov.n	a10, a3
     cba:	5b0c           	movi.n	a11, 5
     cbc:	0008e0        	callx8	a8
     cbf:	090292        	l8ui	a9, a2, 9
     cc2:	441b           	addi.n	a4, a4, 1
     cc4:	ed2497        	blt	a4, a9, cb5 <loas_program_config_element+0x18d>

00000cc7 <loas_program_config_element+0x19f>:
     cc7:	fcef81        	l32r	a8, 84 (2298 <aac_byte_align>)
     cca:	03ad           	mov.n	a10, a3
     ccc:	0008e0        	callx8	a8
     ccf:	fcf081        	l32r	a8, 90 (2254 <aac_getbits>)
     cd2:	03ad           	mov.n	a10, a3
     cd4:	8b0c           	movi.n	a11, 8
     cd6:	0008e0        	callx8	a8
     cd9:	0a5d           	mov.n	a5, a10
     cdb:	101aa6        	blti	a10, 1, cef <loas_program_config_element+0x1c7>
     cde:	040c           	movi.n	a4, 0

00000ce0 <loas_program_config_element+0x1b8>:
     ce0:	fcec81        	l32r	a8, 90 (2254 <aac_getbits>)
     ce3:	03ad           	mov.n	a10, a3
     ce5:	8b0c           	movi.n	a11, 8
     ce7:	0008e0        	callx8	a8
     cea:	441b           	addi.n	a4, a4, 1
     cec:	f09457        	bne	a4, a5, ce0 <loas_program_config_element+0x1b8>

00000cef <loas_program_config_element+0x1c7>:
     cef:	030292        	l8ui	a9, a2, 3
     cf2:	0439b6        	bltui	a9, 3, cfa <loas_program_config_element+0x1d2>
     cf5:	320000        	orbc	b0, b0, b0
     cf8:	f01d           	retw.n

00000cfa <loas_program_config_element+0x1d2>:
     cfa:	120000        	andbc	b0, b0, b0
     cfd:	046707        	bbci	a7, 0, d05 <loas_program_config_element+0x1dd>
     d00:	320000        	orbc	b0, b0, b0
     d03:	f01d           	retw.n

00000d05 <loas_program_config_element+0x1dd>:
     d05:	f01d           	retw.n

00000d07 <loas_program_config_element+0x1df>:
	...

00000d08 <decode_SCE>:
     d08:	006136        	entry	a1, 48
     d0b:	0d0262        	l8ui	a6, a2, 13
     d0e:	0c0272        	l8ui	a7, a2, 12
     d11:	fce381        	l32r	a8, a0 (e38 <check_decode_SCE>)
     d14:	02ad           	mov.n	a10, a2
     d16:	03bd           	mov.n	a11, a3
     d18:	0008e0        	callx8	a8
     d1b:	021076        	bt	b0, d21 <decode_SCE+0x19>
     d1e:	002046        	j	da3 <decode_SCE+0x9b>
     d21:	872a           	add.n	a8, a7, a2
     d23:	00a2b2        	movi	a11, 0x200
     d26:	0189           	s32i.n	a8, a1, 0
     d28:	40d882        	addmi	a8, a8, 0x4000
     d2b:	4b4852        	s8i	a5, a8, 75
     d2e:	20d252        	addmi	a5, a2, 0x2000
     d31:	fcdc81        	l32r	a8, a4 (34d0 <xt_memset8_0>)
     d34:	38c552        	addi	a5, a5, 56
     d37:	05ad           	mov.n	a10, a5
     d39:	0008e0        	callx8	a8
     d3c:	fcd581        	l32r	a8, 90 (2254 <aac_getbits>)
     d3f:	04ad           	mov.n	a10, a4
     d41:	4b0c           	movi.n	a11, 4
     d43:	0008e0        	callx8	a8
     d46:	04cd           	mov.n	a12, a4
     d48:	0e0c           	movi.n	a14, 0
     d4a:	05fd           	mov.n	a15, a5
     d4c:	fcd781        	l32r	a8, a8 (16a0 <one_channel_stream>)
     d4f:	f97c           	movi.n	a9, -1
     d51:	40d2b2        	addmi	a11, a2, 0x4000
     d54:	ac4b62        	s8i	a6, a11, 172
     d57:	b04ba2        	s8i	a10, a11, 176
     d5a:	575b92        	s16i	a9, a11, 174
     d5d:	02ad           	mov.n	a10, a2
     d5f:	41d2b2        	addmi	a11, a2, 0x4100
     d62:	b4cbd2        	addi	a13, a11, -76
     d65:	11d9           	s32i.n	a13, a1, 4
     d67:	accbb2        	addi	a11, a11, -84
     d6a:	0008e0        	callx8	a8
     d6d:	0148           	l32i.n	a4, a1, 0
     d6f:	1b0c           	movi.n	a11, 1
     d71:	7490a0        	extui	a9, a10, 0, 8
     d74:	09f956        	bnez	a9, e17 <decode_SCE+0x10f>
     d77:	0c0292        	l8ui	a9, a2, 12
     d7a:	992a           	add.n	a9, a9, a2
     d7c:	0e09c2        	l8ui	a12, a9, 14
     d7f:	046c56        	bnez	a12, dc9 <decode_SCE+0xc1>
     d82:	0007c6        	j	da5 <decode_SCE+0x9d>

00000d85 <decode_SCE+0x7d>:
     d85:	7d4962        	s8i	a6, a9, 125

00000d88 <decode_SCE+0x80>:
     d88:	0e04c2        	l8ui	a12, a4, 14
     d8b:	d71b           	addi.n	a13, a7, 1
     d8d:	74d0d0        	extui	a13, a13, 0, 8
     d90:	43ddb0        	min	a13, a13, a11
     d93:	0c42d2        	s8i	a13, a2, 12
     d96:	cc6a           	add.n	a12, a12, a6
     d98:	2d0c           	movi.n	a13, 2
     d9a:	74c0c0        	extui	a12, a12, 0, 8
     d9d:	43ccd0        	min	a12, a12, a13
     da0:	0d42c2        	s8i	a12, a2, 13

00000da3 <decode_SCE+0x9b>:
     da3:	f01d           	retw.n

00000da5 <decode_SCE+0x9d>:
     da5:	34c2e2        	addi	a14, a2, 52
     da8:	30d2d2        	addmi	a13, a2, 0x3000
     dab:	0e49b2        	s8i	a11, a9, 14
     dae:	0c02a2        	l8ui	a10, a2, 12
     db1:	38cdd2        	addi	a13, a13, 56
     db4:	62c8           	l32i.n	a12, a2, 24
     db6:	92c9           	s32i.n	a12, a2, 36
     db8:	a2d9           	s32i.n	a13, a2, 40
     dba:	b2e9           	s32i.n	a14, a2, 44
     dbc:	aa2a           	add.n	a10, a10, a2
     dbe:	104ab2        	s8i	a11, a10, 16
     dc1:	0c0292        	l8ui	a9, a2, 12
     dc4:	992a           	add.n	a9, a9, a2
     dc6:	0e09c2        	l8ui	a12, a9, 14

00000dc9 <decode_SCE+0xc1>:
     dc9:	021c26        	beqi	a12, 1, dcf <decode_SCE+0xc7>
     dcc:	0e49b2        	s8i	a11, a9, 14

00000dcf <decode_SCE+0xc7>:
     dcf:	45d2f2        	addmi	a15, a2, 0x4500
     dd2:	6f0ff2        	l8ui	a15, a15, 111
     dd5:	4f9c           	beqz.n	a15, ded <decode_SCE+0xe5>
     dd7:	11a8           	l32i.n	a10, a1, 4
     dd9:	0802c2        	l8ui	a12, a2, 8
     ddc:	0902d2        	l8ui	a13, a2, 9
     ddf:	05ed           	mov.n	a14, a5
     de1:	0212f2        	l16ui	a15, a2, 4
     de4:	46d2b2        	addmi	a11, a2, 0x4600
     de7:	80cbb2        	addi	a11, a11, -128
     dea:	ff76e5        	call8	558 <tns_decode>

00000ded <decode_SCE+0xe5>:
     ded:	02ad           	mov.n	a10, a2
     def:	0e0c           	movi.n	a14, 0
     df1:	fcae81        	l32r	a8, ac (2dbc <filter_bank_LC>)
     df4:	05dd           	mov.n	a13, a5
     df6:	40d2c2        	addmi	a12, a2, 0x4000
     df9:	b50cb2        	l8ui	a11, a12, 181
     dfc:	38cc52        	addi	a5, a12, 56
     dff:	b60cc2        	l8ui	a12, a12, 182
     e02:	0008e0        	callx8	a8
     e05:	1b0c           	movi.n	a11, 1
     e07:	40d2e2        	addmi	a14, a2, 0x4000
     e0a:	ac0ee2        	l8ui	a14, a14, 172
     e0d:	090c           	movi.n	a9, 0
     e0f:	7e05d2        	l8ui	a13, a5, 126
     e12:	ee2a           	add.n	a14, a14, a2
     e14:	134ed2        	s8i	a13, a14, 19

00000e17 <decode_SCE+0x10f>:
     e17:	a62a           	add.n	a10, a6, a2
     e19:	094392        	s8i	a9, a3, 9
     e1c:	0e04f2        	l8ui	a15, a4, 14
     e1f:	40daa2        	addmi	a10, a10, 0x4000
     e22:	d0ca92        	addi	a9, a10, -48
     e25:	fecff2        	addi	a15, a15, -2
     e28:	f59f56        	bnez	a15, d85 <decode_SCE+0x7d>
     e2b:	4d4a62        	s8i	a6, a10, 77
     e2e:	c61b           	addi.n	a12, a6, 1
     e30:	4e4ac2        	s8i	a12, a10, 78
     e33:	ffd446        	j	d88 <decode_SCE+0x80>

00000e36 <decode_SCE+0x12e>:
	...

00000e38 <check_decode_SCE>:
     e38:	004136        	entry	a1, 32
     e3b:	0d0242        	l8ui	a4, a2, 13
     e3e:	f92c           	movi.n	a9, 47
     e40:	0c02a2        	l8ui	a10, a2, 12
     e43:	0924b6        	bltui	a4, 2, e50 <check_decode_SCE+0x18>
     e46:	c80c           	movi.n	a8, 12
     e48:	094382        	s8i	a8, a3, 9
     e4b:	120000        	andbc	b0, b0, b0
     e4e:	f01d           	retw.n

00000e50 <check_decode_SCE+0x18>:
     e50:	0429a7        	blt	a9, a10, e58 <check_decode_SCE+0x20>
     e53:	320000        	orbc	b0, b0, b0
     e56:	f01d           	retw.n

00000e58 <check_decode_SCE+0x20>:
     e58:	db0c           	movi.n	a11, 13
     e5a:	0943b2        	s8i	a11, a3, 9
     e5d:	120000        	andbc	b0, b0, b0
     e60:	f01d           	retw.n

00000e62 <check_decode_SCE+0x2a>:
	...

00000e64 <check_decode_CPE>:
     e64:	004136        	entry	a1, 32
     e67:	0d0242        	l8ui	a4, a2, 13
     e6a:	c80c           	movi.n	a8, 12
     e6c:	120000        	andbc	b0, b0, b0
     e6f:	548c           	beqz.n	a4, e78 <check_decode_CPE+0x14>
     e71:	094382        	s8i	a8, a3, 9
     e74:	f01d           	retw.n

00000e76 <check_decode_CPE+0x12>:
     e76:	f01d           	retw.n

00000e78 <check_decode_CPE+0x14>:
     e78:	0c02a2        	l8ui	a10, a2, 12
     e7b:	f92c           	movi.n	a9, 47
     e7d:	320000        	orbc	b0, b0, b0
     e80:	f2a9a7        	bge	a9, a10, e76 <check_decode_CPE+0x12>
     e83:	db0c           	movi.n	a11, 13
     e85:	0943b2        	s8i	a11, a3, 9
     e88:	120000        	andbc	b0, b0, b0
     e8b:	f01d           	retw.n

00000e8d <check_decode_CPE+0x29>:
     e8d:	000000                                        ...

00000e90 <raw_data_block>:
     e90:	006136        	entry	a1, 48
     e93:	40d282        	addmi	a8, a2, 0x4000
     e96:	090c           	movi.n	a9, 0
     e98:	f2eb           	addi.n	a15, a2, 14
     e9a:	31f9           	s32i.n	a15, a1, 12
     e9c:	0d4292        	s8i	a9, a2, 13
     e9f:	0c4292        	s8i	a9, a2, 12
     ea2:	4bc882        	addi	a8, a8, 75
     ea5:	2189           	s32i.n	a8, a1, 8

00000ea7 <raw_data_block+0x17>:
     ea7:	fc7a81        	l32r	a8, 90 (2254 <aac_getbits>)
     eaa:	04ad           	mov.n	a10, a4
     eac:	3b0c           	movi.n	a11, 3
     eae:	0008e0        	callx8	a8
     eb1:	7460a0        	extui	a6, a10, 0, 8
     eb4:	f9c692        	addi	a9, a6, -7
     eb7:	113916        	beqz	a9, fce <raw_data_block+0x13e>
     eba:	96ac           	beqz.n	a6, ee7 <raw_data_block+0x57>
     ebc:	a60b           	addi.n	a10, a6, -1
     ebe:	0a5a16        	beqz	a10, f67 <raw_data_block+0xd7>
     ec1:	fec6b2        	addi	a11, a6, -2
     ec4:	135b16        	beqz	a11, ffd <raw_data_block+0x16d>
     ec7:	303626        	beqi	a6, 3, efb <raw_data_block+0x6b>
     eca:	414626        	beqi	a6, 4, f0f <raw_data_block+0x7f>
     ecd:	025666        	bnei	a6, 5, ed3 <raw_data_block+0x43>
     ed0:	002246        	j	f5d <raw_data_block+0xcd>
     ed3:	d06666        	bnei	a6, 6, ea7 <raw_data_block+0x17>
     ed6:	04ad           	mov.n	a10, a4
     ed8:	0053e5        	call8	1418 <fill_element>
     edb:	0943a2        	s8i	a10, a3, 9
     ede:	74c0a0        	extui	a12, a10, 0, 8
     ee1:	fc2c16        	beqz	a12, ea7 <raw_data_block+0x17>
     ee4:	003b86        	j	fd6 <raw_data_block+0x146>

00000ee7 <raw_data_block+0x57>:
     ee7:	06dd           	mov.n	a13, a6
     ee9:	02ad           	mov.n	a10, a2
     eeb:	03bd           	mov.n	a11, a3
     eed:	04cd           	mov.n	a12, a4
     eef:	ffe1a5        	call8	d08 <decode_SCE>
     ef2:	0903d2        	l8ui	a13, a3, 9
     ef5:	faed16        	beqz	a13, ea7 <raw_data_block+0x17>
     ef8:	003686        	j	fd6 <raw_data_block+0x146>

00000efb <raw_data_block+0x6b>:
     efb:	06dd           	mov.n	a13, a6
     efd:	02ad           	mov.n	a10, a2
     eff:	03bd           	mov.n	a11, a3
     f01:	04cd           	mov.n	a12, a4
     f03:	ffe065        	call8	d08 <decode_SCE>
     f06:	0903e2        	l8ui	a14, a3, 9
     f09:	f9ae16        	beqz	a14, ea7 <raw_data_block+0x17>
     f0c:	003186        	j	fd6 <raw_data_block+0x146>

00000f0f <raw_data_block+0x7f>:
     f0f:	fc6081        	l32r	a8, 90 (2254 <aac_getbits>)
     f12:	04ad           	mov.n	a10, a4
     f14:	db0c           	movi.n	a11, 13
     f16:	0008e0        	callx8	a8
     f19:	0a7d           	mov.n	a7, a10
     f1b:	7490a0        	extui	a9, a10, 0, 8
     f1e:	1199           	s32i.n	a9, a1, 4
     f20:	096d           	mov.n	a6, a9
     f22:	ffa0a2        	movi	a10, 255
     f25:	0f99a7        	bne	a9, a10, f38 <raw_data_block+0xa8>
     f28:	fc5a81        	l32r	a8, 90 (2254 <aac_getbits>)
     f2b:	04ad           	mov.n	a10, a4
     f2d:	8b0c           	movi.n	a11, 8
     f2f:	0008e0        	callx8	a8
     f32:	1168           	l32i.n	a6, a1, 4
     f34:	9a8d           	ae_zext16	a8, a10
     f36:	668a           	add.n	a6, a6, a8

00000f38 <raw_data_block+0xa8>:
     f38:	076787        	bbci	a7, 8, f43 <raw_data_block+0xb3>
     f3b:	fc5281        	l32r	a8, 84 (2298 <aac_byte_align>)
     f3e:	04ad           	mov.n	a10, a4
     f40:	0008e0        	callx8	a8

00000f43 <raw_data_block+0xb3>:
     f43:	0216e6        	bgei	a6, 1, f49 <raw_data_block+0xb9>
     f46:	ffd746        	j	ea7 <raw_data_block+0x17>
     f49:	070c           	movi.n	a7, 0

00000f4b <raw_data_block+0xbb>:
     f4b:	fc5181        	l32r	a8, 90 (2254 <aac_getbits>)
     f4e:	04ad           	mov.n	a10, a4
     f50:	8b0c           	movi.n	a11, 8
     f52:	0008e0        	callx8	a8
     f55:	771b           	addi.n	a7, a7, 1
     f57:	f09677        	bne	a6, a7, f4b <raw_data_block+0xbb>
     f5a:	ffd246        	j	ea7 <raw_data_block+0x17>

00000f5d <raw_data_block+0xcd>:
     f5d:	05ad           	mov.n	a10, a5
     f5f:	04bd           	mov.n	a11, a4
     f61:	ffbc65        	call8	b28 <loas_program_config_element>
     f64:	ffcfc6        	j	ea7 <raw_data_block+0x17>

00000f67 <raw_data_block+0xd7>:
     f67:	02ad           	mov.n	a10, a2
     f69:	03bd           	mov.n	a11, a3
     f6b:	0d0272        	l8ui	a7, a2, 13
     f6e:	fc5081        	l32r	a8, b0 (e64 <check_decode_CPE>)
     f71:	090c           	movi.n	a9, 0
     f73:	004192        	s8i	a9, a1, 0
     f76:	0008e0        	callx8	a8
     f79:	480076        	bf	b0, fc5 <raw_data_block+0x135>
     f7c:	0c02b2        	l8ui	a11, a2, 12
     f7f:	bb2a           	add.n	a11, a11, a2
     f81:	0e0b92        	l8ui	a9, a11, 14
     f84:	050956        	bnez	a9, fd8 <raw_data_block+0x148>
     f87:	2c0c           	movi.n	a12, 2
     f89:	0e4bc2        	s8i	a12, a11, 14
     f8c:	0c02b2        	l8ui	a11, a2, 12
     f8f:	bb2a           	add.n	a11, a11, a2

00000f91 <raw_data_block+0x101>:
     f91:	02ad           	mov.n	a10, a2
     f93:	07cd           	mov.n	a12, a7
     f95:	01dd           	mov.n	a13, a1
     f97:	40db92        	addmi	a9, a11, 0x4000
     f9a:	fc4681        	l32r	a8, b4 (1004 <channel_pair_element>)
     f9d:	04bd           	mov.n	a11, a4
     f9f:	ccc992        	addi	a9, a9, -52
     fa2:	7f4962        	s8i	a6, a9, 127
     fa5:	0008e0        	callx8	a8
     fa8:	2b0c           	movi.n	a11, 2
     faa:	d71b           	addi.n	a13, a7, 1
     fac:	0943a2        	s8i	a10, a3, 9
     faf:	c72a           	add.n	a12, a7, a2
     fb1:	0a0c           	movi.n	a10, 0
     fb3:	40dcc2        	addmi	a12, a12, 0x4000
     fb6:	d0ccc2        	addi	a12, a12, -48
     fb9:	7e4cd2        	s8i	a13, a12, 126
     fbc:	7d4c72        	s8i	a7, a12, 125
     fbf:	0d42b2        	s8i	a11, a2, 13
     fc2:	0c42a2        	s8i	a10, a2, 12

00000fc5 <raw_data_block+0x135>:
     fc5:	090392        	l8ui	a9, a3, 9

00000fc8 <raw_data_block+0x138>:
     fc8:	edb916        	beqz	a9, ea7 <raw_data_block+0x17>
     fcb:	0001c6        	j	fd6 <raw_data_block+0x146>

00000fce <raw_data_block+0x13e>:
     fce:	fc2d81        	l32r	a8, 84 (2298 <aac_byte_align>)
     fd1:	04ad           	mov.n	a10, a4
     fd3:	0008e0        	callx8	a8

00000fd6 <raw_data_block+0x146>:
     fd6:	f01d           	retw.n

00000fd8 <raw_data_block+0x148>:
     fd8:	b52926        	beqi	a9, 2, f91 <raw_data_block+0x101>
     fdb:	21a8           	l32i.n	a10, a1, 8
     fdd:	fc2081        	l32r	a8, 60 (34c0 <xt_memset1>)
     fe0:	0b0c           	movi.n	a11, 0
     fe2:	2c0c           	movi.n	a12, 2
     fe4:	0008e0        	callx8	a8
     fe7:	31a8           	l32i.n	a10, a1, 12
     fe9:	fc1d81        	l32r	a8, 60 (34c0 <xt_memset1>)
     fec:	0b0c           	movi.n	a11, 0
     fee:	2c0c           	movi.n	a12, 2
     ff0:	0008e0        	callx8	a8
     ff3:	591c           	movi.n	a9, 21
     ff5:	094392        	s8i	a9, a3, 9
     ff8:	591c           	movi.n	a9, 21
     ffa:	fff286        	j	fc8 <raw_data_block+0x138>

00000ffd <raw_data_block+0x16d>:
     ffd:	6a0c           	movi.n	a10, 6
     fff:	0943a2        	s8i	a10, a3, 9
    1002:	f01d           	retw.n

00001004 <channel_pair_element>:
    1004:	00c136        	entry	a1, 96
    1007:	7149           	s32i.n	a4, a1, 28
    1009:	8159           	s32i.n	a5, a1, 32
    100b:	00a2b2        	movi	a11, 0x200
    100e:	fc2581        	l32r	a8, a4 (34d0 <xt_memset8_0>)
    1011:	20d272        	addmi	a7, a2, 0x2000
    1014:	45d262        	addmi	a6, a2, 0x4500
    1017:	f5c662        	addi	a6, a6, -11
    101a:	38c772        	addi	a7, a7, 56
    101d:	07ad           	mov.n	a10, a7
    101f:	0008e0        	callx8	a8
    1022:	00a2b2        	movi	a11, 0x200
    1025:	6169           	s32i.n	a6, a1, 24
    1027:	30d242        	addmi	a4, a2, 0x3000
    102a:	fc1e81        	l32r	a8, a4 (34d0 <xt_memset8_0>)
    102d:	38c442        	addi	a4, a4, 56
    1030:	04ad           	mov.n	a10, a4
    1032:	0008e0        	callx8	a8
    1035:	03ad           	mov.n	a10, a3
    1037:	7198           	l32i.n	a9, a1, 28
    1039:	5b0c           	movi.n	a11, 5
    103b:	40d252        	addmi	a5, a2, 0x4000
    103e:	891b           	addi.n	a8, a9, 1
    1040:	575582        	s16i	a8, a5, 174
    1043:	fc1381        	l32r	a8, 90 (2254 <aac_getbits>)
    1046:	ac4592        	s8i	a9, a5, 172
    1049:	2cc552        	addi	a5, a5, 44
    104c:	0008e0        	callx8	a8
    104f:	0480a0        	extui	a8, a10, 0, 1
    1052:	81b8           	l32i.n	a11, a1, 32
    1054:	4191a0        	srli	a9, a10, 1
    1057:	41d262        	addmi	a6, a2, 0x4100
    105a:	b4c662        	addi	a6, a6, -76
    105d:	844592        	s8i	a9, a5, 132
    1060:	004b92        	s8i	a9, a11, 0
    1063:	854582        	s8i	a8, a5, 133
    1066:	116a07        	bbci	a10, 0, 107b <channel_pair_element+0x77>
    1069:	02ad           	mov.n	a10, a2
    106b:	06bd           	mov.n	a11, a6
    106d:	03cd           	mov.n	a12, a3
    106f:	002865        	call8	12f4 <ics_info>
    1072:	021aa6        	blti	a10, 1, 1078 <channel_pair_element+0x74>
    1075:	003ac6        	j	1164 <channel_pair_element+0x160>
    1078:	0045c6        	j	1193 <channel_pair_element+0x18f>

0000107b <channel_pair_element+0x77>:
    107b:	44d2e2        	addmi	a14, a2, 0x4400
    107e:	0d0c           	movi.n	a13, 0
    1080:	f44ed2        	s8i	a13, a14, 244

00001083 <channel_pair_element+0x7f>:
    1083:	02ad           	mov.n	a10, a2
    1085:	03cd           	mov.n	a12, a3
    1087:	06dd           	mov.n	a13, a6
    1089:	0e0c           	movi.n	a14, 0
    108b:	07fd           	mov.n	a15, a7
    108d:	41d2b2        	addmi	a11, a2, 0x4100
    1090:	fc0681        	l32r	a8, a8 (16a0 <one_channel_stream>)
    1093:	accbb2        	addi	a11, a11, -84
    1096:	51b9           	s32i.n	a11, a1, 20
    1098:	0008e0        	callx8	a8
    109b:	021aa6        	blti	a10, 1, 10a1 <channel_pair_element+0x9d>
    109e:	003086        	j	1164 <channel_pair_element+0x160>
    10a1:	0030c6        	j	1168 <channel_pair_element+0x164>

000010a4 <channel_pair_element+0xa0>:
    10a4:	1a0c           	movi.n	a10, 1
    10a6:	10d2b2        	addmi	a11, a2, 0x1000
    10a9:	34c2e2        	addi	a14, a2, 52
    10ac:	72d8           	l32i.n	a13, a2, 28
    10ae:	62c8           	l32i.n	a12, a2, 24
    10b0:	92c9           	s32i.n	a12, a2, 36
    10b2:	a2d9           	s32i.n	a13, a2, 40
    10b4:	b2e9           	s32i.n	a14, a2, 44
    10b6:	34cbb2        	addi	a11, a11, 52
    10b9:	c2b9           	s32i.n	a11, a2, 48
    10bb:	1049a2        	s8i	a10, a9, 16

000010be <channel_pair_element+0xba>:
    10be:	06ad           	mov.n	a10, a6
    10c0:	03bd           	mov.n	a11, a3
    10c2:	07cd           	mov.n	a12, a7
    10c4:	04dd           	mov.n	a13, a4
    10c6:	0212e2        	l16ui	a14, a2, 4
    10c9:	ff2465        	call8	310 <ms_decode>
    10cc:	06ad           	mov.n	a10, a6
    10ce:	03bd           	mov.n	a11, a3
    10d0:	07cd           	mov.n	a12, a7
    10d2:	fbf981        	l32r	a8, b8 (34f0 <is_decode>)
    10d5:	04dd           	mov.n	a13, a4
    10d7:	0212e2        	l16ui	a14, a2, 4
    10da:	0008e0        	callx8	a8
    10dd:	45d292        	addmi	a9, a2, 0x4500
    10e0:	6f0992        	l8ui	a9, a9, 111
    10e3:	499c           	beqz.n	a9, 10fb <channel_pair_element+0xf7>
    10e5:	06ad           	mov.n	a10, a6
    10e7:	0802c2        	l8ui	a12, a2, 8
    10ea:	0902d2        	l8ui	a13, a2, 9
    10ed:	07ed           	mov.n	a14, a7
    10ef:	0212f2        	l16ui	a15, a2, 4
    10f2:	46d2b2        	addmi	a11, a2, 0x4600
    10f5:	80cbb2        	addi	a11, a11, -128
    10f8:	ff45e5        	call8	558 <tns_decode>

000010fb <channel_pair_element+0xf7>:
    10fb:	4cd2a2        	addmi	a10, a2, 0x4c00
    10fe:	8ccaa2        	addi	a10, a10, -116
    1101:	7f0aa2        	l8ui	a10, a10, 127
    1104:	4a9c           	beqz.n	a10, 111c <channel_pair_element+0x118>
    1106:	03ad           	mov.n	a10, a3
    1108:	0802c2        	l8ui	a12, a2, 8
    110b:	0902d2        	l8ui	a13, a2, 9
    110e:	04ed           	mov.n	a14, a4
    1110:	0212f2        	l16ui	a15, a2, 4
    1113:	4cd2b2        	addmi	a11, a2, 0x4c00
    1116:	1ccbb2        	addi	a11, a11, 28
    1119:	ff43e5        	call8	558 <tns_decode>

0000111c <channel_pair_element+0x118>:
    111c:	07dd           	mov.n	a13, a7
    111e:	02ad           	mov.n	a10, a2
    1120:	0e0c           	movi.n	a14, 0
    1122:	fbe281        	l32r	a8, ac (2dbc <filter_bank_LC>)
    1125:	40d232        	addmi	a3, a2, 0x4000
    1128:	38c332        	addi	a3, a3, 56
    112b:	7d03b2        	l8ui	a11, a3, 125
    112e:	7e03c2        	l8ui	a12, a3, 126
    1131:	0008e0        	callx8	a8
    1134:	04dd           	mov.n	a13, a4
    1136:	02ad           	mov.n	a10, a2
    1138:	1e0c           	movi.n	a14, 1
    113a:	fbdc81        	l32r	a8, ac (2dbc <filter_bank_LC>)
    113d:	47d262        	addmi	a6, a2, 0x4700
    1140:	d4c662        	addi	a6, a6, -44
    1143:	7d06b2        	l8ui	a11, a6, 125
    1146:	7e06c2        	l8ui	a12, a6, 126
    1149:	0008e0        	callx8	a8
    114c:	8005b2        	l8ui	a11, a5, 128
    114f:	7e03a2        	l8ui	a10, a3, 126
    1152:	bb2a           	add.n	a11, a11, a2
    1154:	134ba2        	s8i	a10, a11, 19
    1157:	419592        	l16si	a9, a5, 130
    115a:	0a0c           	movi.n	a10, 0
    115c:	7e0682        	l8ui	a8, a6, 126
    115f:	992a           	add.n	a9, a9, a2
    1161:	134982        	s8i	a8, a9, 19

00001164 <channel_pair_element+0x160>:
    1164:	0a2d           	mov.n	a2, a10
    1166:	f01d           	retw.n

00001168 <channel_pair_element+0x164>:
    1168:	03cd           	mov.n	a12, a3
    116a:	51b8           	l32i.n	a11, a1, 20
    116c:	02ad           	mov.n	a10, a2
    116e:	0e0c           	movi.n	a14, 0
    1170:	04fd           	mov.n	a15, a4
    1172:	47d2d2        	addmi	a13, a2, 0x4700
    1175:	fbcc81        	l32r	a8, a8 (16a0 <one_channel_stream>)
    1178:	50cdd2        	addi	a13, a13, 80
    117b:	b1d9           	s32i.n	a13, a1, 44
    117d:	0008e0        	callx8	a8
    1180:	b138           	l32i.n	a3, a1, 44
    1182:	de1ae6        	bgei	a10, 1, 1164 <channel_pair_element+0x160>
    1185:	0c0292        	l8ui	a9, a2, 12
    1188:	992a           	add.n	a9, a9, a2
    118a:	1009c2        	l8ui	a12, a9, 16
    118d:	f2dc56        	bnez	a12, 10be <channel_pair_element+0xba>
    1190:	ffc406        	j	10a4 <channel_pair_element+0xa0>

00001193 <channel_pair_element+0x18f>:
    1193:	fbbf81        	l32r	a8, 90 (2254 <aac_getbits>)
    1196:	03ad           	mov.n	a10, a3
    1198:	2b0c           	movi.n	a11, 2
    119a:	0008e0        	callx8	a8
    119d:	c159           	s32i.n	a5, a1, 48
    119f:	d149           	s32i.n	a4, a1, 52
    11a1:	44d2b2        	addmi	a11, a2, 0x4400
    11a4:	7490a0        	extui	a9, a10, 0, 8
    11a7:	74cbb2        	addi	a11, a11, 116
    11aa:	804ba2        	s8i	a10, a11, 128
    11ad:	40d2a2        	addmi	a10, a2, 0x4000
    11b0:	38caa2        	addi	a10, a10, 56
    11b3:	91a9           	s32i.n	a10, a1, 36
    11b5:	021926        	beqi	a9, 1, 11bb <channel_pair_element+0x1b7>
    11b8:	002786        	j	125a <channel_pair_element+0x256>
    11bb:	a1b9           	s32i.n	a11, a1, 40
    11bd:	820ac2        	l8ui	a12, a10, 130
    11c0:	c159           	s32i.n	a5, a1, 48
    11c2:	d149           	s32i.n	a4, a1, 52
    11c4:	092c16        	beqz	a12, 125a <channel_pair_element+0x256>
    11c7:	7c0ab2        	l8ui	a11, a10, 124
    11ca:	0e0c           	movi.n	a14, 0
    11cc:	41e9           	s32i.n	a14, a1, 16

000011ce <channel_pair_element+0x1ca>:
    11ce:	6158           	l32i.n	a5, a1, 24
    11d0:	0b4d           	mov.n	a4, a11
    11d2:	46bba6        	blti	a11, 16, 121c <channel_pair_element+0x218>
    11d5:	11b9           	s32i.n	a11, a1, 4
    11d7:	78afd2        	movi	a13, -136
    11da:	0c1c           	movi.n	a12, 16
    11dc:	d2cbc0        	quos	a12, a11, a12
    11df:	d5da           	add.n	a13, a5, a13
    11e1:	01d9           	s32i.n	a13, a1, 0
    11e3:	31c9           	s32i.n	a12, a1, 12

000011e5 <channel_pair_element+0x1e1>:
    11e5:	f0c442        	addi	a4, a4, -16
    11e8:	fbaa81        	l32r	a8, 90 (2254 <aac_getbits>)
    11eb:	03ad           	mov.n	a10, a3
    11ed:	0b1c           	movi.n	a11, 16
    11ef:	0008e0        	callx8	a8
    11f2:	0198           	l32i.n	a9, a1, 0
    11f4:	88c5b2        	addi	a11, a5, -120
    11f7:	0c1c           	movi.n	a12, 16
    11f9:	10c552        	addi	a5, a5, 16
    11fc:	10c992        	addi	a9, a9, 16
    11ff:	0199           	s32i.n	a9, a1, 0
    1201:	0aac76        	loopgtz	a12, 120f <channel_pair_element+0x20b>
    1204:	04d0a0        	extui	a13, a10, 0, 1
    1207:	874bd2        	s8i	a13, a11, 135
    120a:	41a1a0        	srli	a10, a10, 1
    120d:	bb0b           	addi.n	a11, a11, -1

0000120f <channel_pair_element+0x20b>:
    120f:	d2b4e6        	bgei	a4, 16, 11e5 <channel_pair_element+0x1e1>
    1212:	3188           	l32i.n	a8, a1, 12
    1214:	1148           	l32i.n	a4, a1, 4
    1216:	1188c0        	slli	a8, a8, 4
    1219:	c04480        	sub	a4, a4, a8

0000121c <channel_pair_element+0x218>:
    121c:	fb9d81        	l32r	a8, 90 (2254 <aac_getbits>)
    121f:	03ad           	mov.n	a10, a3
    1221:	20b440        	or	a11, a4, a4
    1224:	0008e0        	callx8	a8
    1227:	809450        	add	a9, a4, a5
    122a:	80c992        	addi	a9, a9, -128
    122d:	0aa476        	loopgtz	a4, 123b <channel_pair_element+0x237>
    1230:	04b0a0        	extui	a11, a10, 0, 1
    1233:	7f49b2        	s8i	a11, a9, 127
    1236:	41a1a0        	srli	a10, a10, 1
    1239:	990b           	addi.n	a9, a9, -1

0000123b <channel_pair_element+0x237>:
    123b:	91d8           	l32i.n	a13, a1, 36
    123d:	41c8           	l32i.n	a12, a1, 16
    123f:	61e8           	l32i.n	a14, a1, 24
    1241:	7c0db2        	l8ui	a11, a13, 124
    1244:	fc1b           	addi.n	a15, a12, 1
    1246:	820dd2        	l8ui	a13, a13, 130
    1249:	74c0f0        	extui	a12, a15, 0, 8
    124c:	41c9           	s32i.n	a12, a1, 16
    124e:	eeba           	add.n	a14, a14, a11
    1250:	61e9           	s32i.n	a14, a1, 24
    1252:	02bcd7        	bgeu	a12, a13, 1258 <channel_pair_element+0x254>
    1255:	ffdd46        	j	11ce <channel_pair_element+0x1ca>
    1258:	a1b8           	l32i.n	a11, a1, 40
    125a:	d148           	l32i.n	a4, a1, 52
    125c:	c158           	l32i.n	a5, a1, 48
    125e:	9198           	l32i.n	a9, a1, 36
    1260:	47d2a2        	addmi	a10, a2, 0x4700
    1263:	4cd2f2        	addmi	a15, a2, 0x4c00
    1266:	800be2        	l8ui	a14, a11, 128
    1269:	8ccff2        	addi	a15, a15, -116
    126c:	d4caa2        	addi	a10, a10, -44
    126f:	21a9           	s32i.n	a10, a1, 8
    1271:	8209c2        	l8ui	a12, a9, 130
    1274:	8109d2        	l8ui	a13, a9, 129
    1277:	800982        	l8ui	a8, a9, 128
    127a:	fc0bb2        	l8ui	a11, a11, 252
    127d:	804fb2        	s8i	a11, a15, 128
    1280:	804a82        	s8i	a8, a10, 128
    1283:	7c09b2        	l8ui	a11, a9, 124
    1286:	7c4ab2        	s8i	a11, a10, 124
    1289:	fb8c81        	l32r	a8, bc (34e0 <xt_memcpy4>)
    128c:	8309b2        	l8ui	a11, a9, 131
    128f:	044fe2        	s8i	a14, a15, 4
    1292:	814ad2        	s8i	a13, a10, 129
    1295:	824ac2        	s8i	a12, a10, 130
    1298:	7f0992        	l8ui	a9, a9, 127
    129b:	cc3c           	movi.n	a12, 60
    129d:	834ab2        	s8i	a11, a10, 131
    12a0:	7f4a92        	s8i	a9, a10, 127
    12a3:	41d2b2        	addmi	a11, a2, 0x4100
    12a6:	c4cbb2        	addi	a11, a11, -60
    12a9:	47d2a2        	addmi	a10, a2, 0x4700
    12ac:	60caa2        	addi	a10, a10, 96
    12af:	0008e0        	callx8	a8
    12b2:	ac1c           	movi.n	a12, 26
    12b4:	42d2b2        	addmi	a11, a2, 0x4200
    12b7:	48d2a2        	addmi	a10, a2, 0x4800
    12ba:	fb8081        	l32r	a8, bc (34e0 <xt_memcpy4>)
    12bd:	50caa2        	addi	a10, a10, 80
    12c0:	b4cbb2        	addi	a11, a11, -76
    12c3:	0008e0        	callx8	a8
    12c6:	2198           	l32i.n	a9, a1, 8
    12c8:	8c0c           	movi.n	a12, 8
    12ca:	47d2a2        	addmi	a10, a2, 0x4700
    12cd:	40d2b2        	addmi	a11, a2, 0x4000
    12d0:	3ccbb2        	addi	a11, a11, 60
    12d3:	d8caa2        	addi	a10, a10, -40
    12d6:	09ac76        	loopgtz	a12, 12e3 <channel_pair_element+0x2df>
    12d9:	800bc2        	l8ui	a12, a11, 128
    12dc:	804ac2        	s8i	a12, a10, 128
    12df:	bb1b           	addi.n	a11, a11, 1
    12e1:	aa1b           	addi.n	a10, a10, 1
    12e3:	91d8           	l32i.n	a13, a1, 36
    12e5:	7d0de2        	l8ui	a14, a13, 125
    12e8:	7d49e2        	s8i	a14, a9, 125
    12eb:	7e0dd2        	l8ui	a13, a13, 126
    12ee:	7e49d2        	s8i	a13, a9, 126
    12f1:	ff6386        	j	1083 <channel_pair_element+0x7f>

000012f4 <ics_info>:
    12f4:	004136        	entry	a1, 32
    12f7:	fb6681        	l32r	a8, 90 (2254 <aac_getbits>)
    12fa:	04ad           	mov.n	a10, a4
    12fc:	4b0c           	movi.n	a11, 4
    12fe:	0008e0        	callx8	a8
    1301:	1491a0        	extui	a9, a10, 1, 2
    1304:	04b0a0        	extui	a11, a10, 0, 1
    1307:	0243b2        	s8i	a11, a3, 2
    130a:	014392        	s8i	a9, a3, 1
    130d:	182966        	bnei	a9, 2, 1329 <ics_info+0x35>
    1310:	fb6081        	l32r	a8, 90 (2254 <aac_getbits>)
    1313:	04ad           	mov.n	a10, a4
    1315:	bb0c           	movi.n	a11, 11
    1317:	0008e0        	callx8	a8
    131a:	6490a0        	extui	a9, a10, 0, 7
    131d:	34b7a0        	extui	a11, a10, 7, 4
    1320:	0043b2        	s8i	a11, a3, 0
    1323:	034392        	s8i	a9, a3, 3
    1326:	000306        	j	1336 <ics_info+0x42>

00001329 <ics_info+0x35>:
    1329:	fb5981        	l32r	a8, 90 (2254 <aac_getbits>)
    132c:	04ad           	mov.n	a10, a4
    132e:	6b0c           	movi.n	a11, 6
    1330:	0008e0        	callx8	a8
    1333:	0043a2        	s8i	a10, a3, 0

00001336 <ics_info+0x42>:
    1336:	fb6281        	l32r	a8, c0 (3370 <window_grouping_info>)
    1339:	02ad           	mov.n	a10, a2
    133b:	03bd           	mov.n	a11, a3
    133d:	0008e0        	callx8	a8
    1340:	0a6d           	mov.n	a6, a10
    1342:	451ae6        	bgei	a10, 1, 138b <ics_info+0x97>
    1345:	0003a2        	l8ui	a10, a3, 0
    1348:	050392        	l8ui	a9, a3, 5
    134b:	010382        	l8ui	a8, a3, 1
    134e:	3db9a7        	bgeu	a9, a10, 138f <ics_info+0x9b>
    1351:	62a062        	movi	a6, 98
    1354:	000cc6        	j	138b <ics_info+0x97>

00001357 <ics_info+0x63>:
    1357:	0b0c           	movi.n	a11, 0

00001359 <ics_info+0x65>:
    1359:	000332        	l8ui	a3, a3, 0
    135c:	04ad           	mov.n	a10, a4
    135e:	fb4f81        	l32r	a8, 9c (2284 <aac_get1bit>)
    1361:	4333b0        	min	a3, a3, a11
    1364:	743030        	extui	a3, a3, 0, 8
    1367:	0008e0        	callx8	a8
    136a:	096a07        	bbci	a10, 0, 1377 <ics_info+0x83>
    136d:	fb4881        	l32r	a8, 90 (2254 <aac_getbits>)
    1370:	04ad           	mov.n	a10, a4
    1372:	5b0c           	movi.n	a11, 5
    1374:	0008e0        	callx8	a8

00001377 <ics_info+0x83>:
    1377:	039c           	beqz.n	a3, 138b <ics_info+0x97>
    1379:	050c           	movi.n	a5, 0

0000137b <ics_info+0x87>:
    137b:	fb4881        	l32r	a8, 9c (2284 <aac_get1bit>)
    137e:	04ad           	mov.n	a10, a4
    1380:	0008e0        	callx8	a8
    1383:	551b           	addi.n	a5, a5, 1
    1385:	745050        	extui	a5, a5, 0, 8
    1388:	ef9357        	bne	a3, a5, 137b <ics_info+0x87>

0000138b <ics_info+0x97>:
    138b:	062d           	mov.n	a2, a6
    138d:	f01d           	retw.n

0000138f <ics_info+0x9b>:
    138f:	f82826        	beqi	a8, 2, 138b <ics_info+0x97>
    1392:	fb4281        	l32r	a8, 9c (2284 <aac_get1bit>)
    1395:	04ad           	mov.n	a10, a4
    1397:	0008e0        	callx8	a8
    139a:	7490a0        	extui	a9, a10, 0, 8
    139d:	04d3b2        	addmi	a11, a3, 0x400
    13a0:	bd4ba2        	s8i	a10, a11, 189
    13a3:	fe4916        	beqz	a9, 138b <ics_info+0x97>
    13a6:	0902c2        	l8ui	a12, a2, 9
    13a9:	080292        	l8ui	a9, a2, 8
    13ac:	db1c66        	bnei	a12, 1, 138b <ics_info+0x97>
    13af:	a4a9f6        	bgeui	a9, 12, 1357 <ics_info+0x63>
    13b2:	fb44b1        	l32r	a11, c4 (5ea0 <predef_sfb_max>)
    13b5:	b9ba           	add.n	a11, a9, a11
    13b7:	000bb2        	l8ui	a11, a11, 0
    13ba:	ffe6c6        	j	1359 <ics_info+0x65>

000013bd <ics_info+0xc9>:
    13bd:	000000                                        ...

000013c0 <pulse_data>:
    13c0:	004136        	entry	a1, 32
    13c3:	fb3381        	l32r	a8, 90 (2254 <aac_getbits>)
    13c6:	04ad           	mov.n	a10, a4
    13c8:	8b0c           	movi.n	a11, 8
    13ca:	0008e0        	callx8	a8
    13cd:	54b0a0        	extui	a11, a10, 0, 6
    13d0:	41c6a0        	srli	a12, a10, 6
    13d3:	050292        	l8ui	a9, a2, 5
    13d6:	0043c2        	s8i	a12, a3, 0
    13d9:	0143b2        	s8i	a11, a3, 1
    13dc:	04b9b7        	bgeu	a9, a11, 13e4 <pulse_data+0x24>
    13df:	63a022        	movi	a2, 99
    13e2:	f01d           	retw.n

000013e4 <pulse_data+0x24>:
    13e4:	74d6a0        	extui	a13, a10, 6, 8
    13e7:	027d96        	bltz	a13, 1412 <pulse_data+0x52>
    13ea:	020c           	movi.n	a2, 0

000013ec <pulse_data+0x2c>:
    13ec:	fb2981        	l32r	a8, 90 (2254 <aac_getbits>)
    13ef:	04ad           	mov.n	a10, a4
    13f1:	9b0c           	movi.n	a11, 9
    13f3:	0008e0        	callx8	a8
    13f6:	909230        	addx2	a9, a2, a3
    13f9:	3480a0        	extui	a8, a10, 0, 4
    13fc:	c32a           	add.n	a12, a3, a2
    13fe:	41b4a0        	srli	a11, a10, 4
    1401:	024cb2        	s8i	a11, a12, 2
    1404:	035982        	s16i	a8, a9, 6
    1407:	000392        	l8ui	a9, a3, 0
    140a:	221b           	addi.n	a2, a2, 1
    140c:	742020        	extui	a2, a2, 0, 8
    140f:	d9a927        	bge	a9, a2, 13ec <pulse_data+0x2c>

00001412 <pulse_data+0x52>:
    1412:	020c           	movi.n	a2, 0
    1414:	f01d           	retw.n

00001416 <pulse_data+0x56>:
	...

00001418 <fill_element>:
    1418:	004136        	entry	a1, 32
    141b:	fb1d81        	l32r	a8, 90 (2254 <aac_getbits>)
    141e:	02ad           	mov.n	a10, a2
    1420:	4b0c           	movi.n	a11, 4
    1422:	0008e0        	callx8	a8
    1425:	0a3d           	mov.n	a3, a10
    1427:	0a6d           	mov.n	a6, a10
    1429:	f90c           	movi.n	a9, 15
    142b:	0d9a97        	bne	a10, a9, 143c <fill_element+0x24>
    142e:	fb1881        	l32r	a8, 90 (2254 <aac_getbits>)
    1431:	02ad           	mov.n	a10, a2
    1433:	8b0c           	movi.n	a11, 8
    1435:	0008e0        	callx8	a8
    1438:	6a3a           	add.n	a6, a10, a3
    143a:	660b           	addi.n	a6, a6, -1

0000143c <fill_element+0x24>:
    143c:	0216e6        	bgei	a6, 1, 1442 <fill_element+0x2a>
    143f:	0033c6        	j	1512 <fill_element+0xfa>
    1442:	b70c           	movi.n	a7, 11

00001444 <fill_element+0x2c>:
    1444:	fb1381        	l32r	a8, 90 (2254 <aac_getbits>)
    1447:	02ad           	mov.n	a10, a2
    1449:	4b0c           	movi.n	a11, 4
    144b:	0008e0        	callx8	a8
    144e:	269a77        	bne	a10, a7, 1478 <fill_element+0x60>
    1451:	fb1281        	l32r	a8, 9c (2284 <aac_get1bit>)
    1454:	02ad           	mov.n	a10, a2
    1456:	0008e0        	callx8	a8
    1459:	0490a0        	extui	a9, a10, 0, 1
    145c:	0b6916        	beqz	a9, 1516 <fill_element+0xfe>
    145f:	fb0c81        	l32r	a8, 90 (2254 <aac_getbits>)
    1462:	02ad           	mov.n	a10, a2
    1464:	4b0c           	movi.n	a11, 4
    1466:	0008e0        	callx8	a8
    1469:	fb0981        	l32r	a8, 90 (2254 <aac_getbits>)
    146c:	02ad           	mov.n	a10, a2
    146e:	4b0c           	movi.n	a11, 4
    1470:	0008e0        	callx8	a8
    1473:	230c           	movi.n	a3, 2
    1475:	0027c6        	j	1518 <fill_element+0x100>

00001478 <fill_element+0x60>:
    1478:	241a66        	bnei	a10, 1, 14a0 <fill_element+0x88>
    147b:	fb0581        	l32r	a8, 90 (2254 <aac_getbits>)
    147e:	02ad           	mov.n	a10, a2
    1480:	4b0c           	movi.n	a11, 4
    1482:	0008e0        	callx8	a8
    1485:	965d           	ae_zext16	a5, a6
    1487:	450b           	addi.n	a4, a5, -1
    1489:	7c14a6        	blti	a4, 1, 1509 <fill_element+0xf1>
    148c:	030c           	movi.n	a3, 0

0000148e <fill_element+0x76>:
    148e:	fb0081        	l32r	a8, 90 (2254 <aac_getbits>)
    1491:	02ad           	mov.n	a10, a2
    1493:	8b0c           	movi.n	a11, 8
    1495:	0008e0        	callx8	a8
    1498:	331b           	addi.n	a3, a3, 1
    149a:	f09437        	bne	a4, a3, 148e <fill_element+0x76>
    149d:	001a06        	j	1509 <fill_element+0xf1>

000014a0 <fill_element+0x88>:
    14a0:	412a66        	bnei	a10, 2, 14e5 <fill_element+0xcd>
    14a3:	fafb81        	l32r	a8, 90 (2254 <aac_getbits>)
    14a6:	02ad           	mov.n	a10, a2
    14a8:	4b0c           	movi.n	a11, 4
    14aa:	0008e0        	callx8	a8
    14ad:	7490a0        	extui	a9, a10, 0, 8
    14b0:	112956        	bnez	a9, 15c6 <fill_element+0x1ae>
    14b3:	030c           	movi.n	a3, 0
    14b5:	040c           	movi.n	a4, 0

000014b7 <fill_element+0x9f>:
    14b7:	faf681        	l32r	a8, 90 (2254 <aac_getbits>)
    14ba:	02ad           	mov.n	a10, a2
    14bc:	8b0c           	movi.n	a11, 8
    14be:	0008e0        	callx8	a8
    14c1:	441b           	addi.n	a4, a4, 1
    14c3:	7490a0        	extui	a9, a10, 0, 8
    14c6:	339a           	add.n	a3, a3, a9
    14c8:	ffa0a2        	movi	a10, 255
    14cb:	e819a7        	beq	a9, a10, 14b7 <fill_element+0x9f>
    14ce:	0213e6        	bgei	a3, 1, 14d4 <fill_element+0xbc>
    14d1:	003c46        	j	15c6 <fill_element+0x1ae>
    14d4:	faef81        	l32r	a8, 90 (2254 <aac_getbits>)
    14d7:	02ad           	mov.n	a10, a2
    14d9:	8b0c           	movi.n	a11, 8
    14db:	0008e0        	callx8	a8
    14de:	534a           	add.n	a5, a3, a4
    14e0:	551b           	addi.n	a5, a5, 1
    14e2:	0008c6        	j	1509 <fill_element+0xf1>

000014e5 <fill_element+0xcd>:
    14e5:	4b0c           	movi.n	a11, 4

000014e7 <fill_element+0xcf>:
    14e7:	6b8c           	beqz.n	a11, 14f1 <fill_element+0xd9>
    14e9:	fae981        	l32r	a8, 90 (2254 <aac_getbits>)
    14ec:	02ad           	mov.n	a10, a2
    14ee:	0008e0        	callx8	a8

000014f1 <fill_element+0xd9>:
    14f1:	965d           	ae_zext16	a5, a6
    14f3:	450b           	addi.n	a4, a5, -1
    14f5:	1014a6        	blti	a4, 1, 1509 <fill_element+0xf1>
    14f8:	030c           	movi.n	a3, 0

000014fa <fill_element+0xe2>:
    14fa:	fae581        	l32r	a8, 90 (2254 <aac_getbits>)
    14fd:	02ad           	mov.n	a10, a2
    14ff:	8b0c           	movi.n	a11, 8
    1501:	0008e0        	callx8	a8
    1504:	331b           	addi.n	a3, a3, 1
    1506:	f09437        	bne	a4, a3, 14fa <fill_element+0xe2>

00001509 <fill_element+0xf1>:
    1509:	c06650        	sub	a6, a6, a5
    150c:	0216a6        	blti	a6, 1, 1512 <fill_element+0xfa>
    150f:	ffcc46        	j	1444 <fill_element+0x2c>

00001512 <fill_element+0xfa>:
    1512:	020c           	movi.n	a2, 0
    1514:	f01d           	retw.n

00001516 <fill_element+0xfe>:
    1516:	130c           	movi.n	a3, 1

00001518 <fill_element+0x100>:
    1518:	fae181        	l32r	a8, 9c (2284 <aac_get1bit>)
    151b:	02ad           	mov.n	a10, a2
    151d:	0008e0        	callx8	a8
    1520:	7490a0        	extui	a9, a10, 0, 8
    1523:	2d1966        	bnei	a9, 1, 1554 <fill_element+0x13c>
    1526:	fada81        	l32r	a8, 90 (2254 <aac_getbits>)
    1529:	02ad           	mov.n	a10, a2
    152b:	7b0c           	movi.n	a11, 7
    152d:	0008e0        	callx8	a8
    1530:	140c           	movi.n	a4, 1

00001532 <fill_element+0x11a>:
    1532:	fada81        	l32r	a8, 9c (2284 <aac_get1bit>)
    1535:	02ad           	mov.n	a10, a2
    1537:	0008e0        	callx8	a8
    153a:	7490a0        	extui	a9, a10, 0, 8
    153d:	0e1966        	bnei	a9, 1, 154f <fill_element+0x137>
    1540:	fad481        	l32r	a8, 90 (2254 <aac_getbits>)
    1543:	02ad           	mov.n	a10, a2
    1545:	7b0c           	movi.n	a11, 7
    1547:	0008e0        	callx8	a8
    154a:	441b           	addi.n	a4, a4, 1
    154c:	fff886        	j	1532 <fill_element+0x11a>

0000154f <fill_element+0x137>:
    154f:	343a           	add.n	a3, a4, a3
    1551:	743030        	extui	a3, a3, 0, 8

00001554 <fill_element+0x13c>:
    1554:	fad281        	l32r	a8, 9c (2284 <aac_get1bit>)
    1557:	02ad           	mov.n	a10, a2
    1559:	0008e0        	callx8	a8
    155c:	2c6a07        	bbci	a10, 0, 158c <fill_element+0x174>
    155f:	facc81        	l32r	a8, 90 (2254 <aac_getbits>)
    1562:	02ad           	mov.n	a10, a2
    1564:	4b0c           	movi.n	a11, 4
    1566:	0008e0        	callx8	a8
    1569:	0a4d           	mov.n	a4, a10
    156b:	fac981        	l32r	a8, 90 (2254 <aac_getbits>)
    156e:	4b0c           	movi.n	a11, 4
    1570:	02ad           	mov.n	a10, a2
    1572:	0008e0        	callx8	a8
    1575:	02ad           	mov.n	a10, a2
    1577:	8b0c           	movi.n	a11, 8
    1579:	fac581        	l32r	a8, 90 (2254 <aac_getbits>)
    157c:	745040        	extui	a5, a4, 0, 8
    157f:	551b           	addi.n	a5, a5, 1
    1581:	0008e0        	callx8	a8
    1584:	332b           	addi.n	a3, a3, 2
    1586:	743030        	extui	a3, a3, 0, 8
    1589:	000046        	j	158e <fill_element+0x176>

0000158c <fill_element+0x174>:
    158c:	150c           	movi.n	a5, 1

0000158e <fill_element+0x176>:
    158e:	fac381        	l32r	a8, 9c (2284 <aac_get1bit>)
    1591:	02ad           	mov.n	a10, a2
    1593:	0008e0        	callx8	a8
    1596:	0e6a07        	bbci	a10, 0, 15a8 <fill_element+0x190>
    1599:	fabd81        	l32r	a8, 90 (2254 <aac_getbits>)
    159c:	02ad           	mov.n	a10, a2
    159e:	8b0c           	movi.n	a11, 8
    15a0:	0008e0        	callx8	a8
    15a3:	331b           	addi.n	a3, a3, 1
    15a5:	743030        	extui	a3, a3, 0, 8

000015a8 <fill_element+0x190>:
    15a8:	1515a6        	blti	a5, 1, 15c1 <fill_element+0x1a9>
    15ab:	040c           	movi.n	a4, 0

000015ad <fill_element+0x195>:
    15ad:	fab881        	l32r	a8, 90 (2254 <aac_getbits>)
    15b0:	02ad           	mov.n	a10, a2
    15b2:	8b0c           	movi.n	a11, 8
    15b4:	0008e0        	callx8	a8
    15b7:	441b           	addi.n	a4, a4, 1
    15b9:	331b           	addi.n	a3, a3, 1
    15bb:	743030        	extui	a3, a3, 0, 8
    15be:	eb9457        	bne	a4, a5, 15ad <fill_element+0x195>

000015c1 <fill_element+0x1a9>:
    15c1:	035d           	mov.n	a5, a3
    15c3:	ffd086        	j	1509 <fill_element+0xf1>

000015c6 <fill_element+0x1ae>:
    15c6:	0b0c           	movi.n	a11, 0
    15c8:	ffc6c6        	j	14e7 <fill_element+0xcf>

000015cb <fill_element+0x1b3>:
	...

000015cc <de_interleave>:
    15cc:	008136        	entry	a1, 64
    15cf:	3129           	s32i.n	a2, a1, 12
    15d1:	058d           	mov.n	a8, a5
    15d3:	037d           	mov.n	a7, a3
    15d5:	00a462        	movi	a6, 0x400
    15d8:	050292        	l8ui	a9, a2, 5
    15db:	0602a2        	l8ui	a10, a2, 6
    15de:	21a9           	s32i.n	a10, a1, 8
    15e0:	1199           	s32i.n	a9, a1, 4
    15e2:	06a676        	loopgtz	a6, 15ec <de_interleave+0x20>
    15e5:	07c8           	l32i.n	a12, a7, 0
    15e7:	09e5c4        	ae_s32ip	a12, a5, 4
    15ea:	774b           	addi.n	a7, a7, 4

000015ec <de_interleave+0x20>:
    15ec:	21d8           	l32i.n	a13, a1, 8
    15ee:	021de6        	bgei	a13, 1, 15f4 <de_interleave+0x28>
    15f1:	002986        	j	169b <de_interleave+0xcf>
    15f4:	3168           	l32i.n	a6, a1, 12
    15f6:	1128           	l32i.n	a2, a1, 4
    15f8:	0c0c           	movi.n	a12, 0
    15fa:	8b0c           	movi.n	a11, 8
    15fc:	d2b4b0        	quos	a11, a4, a11
    15ff:	41c9           	s32i.n	a12, a1, 16
    1601:	01b9           	s32i.n	a11, a1, 0
    1603:	11bbe0        	slli	a11, a11, 2
    1606:	902260        	addx2	a2, a2, a6

00001609 <de_interleave+0x3d>:
    1609:	31e8           	l32i.n	a14, a1, 12
    160b:	41d8           	l32i.n	a13, a1, 16
    160d:	ddea           	add.n	a13, a13, a14
    160f:	080d92        	l8ui	a9, a13, 8
    1612:	1d1966        	bnei	a9, 1, 1633 <de_interleave+0x67>
    1615:	0f2c           	movi.n	a15, 32
    1617:	15af76        	loopgtz	a15, 1630 <de_interleave+0x64>
    161a:	08c8           	l32i.n	a12, a8, 0
    161c:	03c9           	s32i.n	a12, a3, 0
    161e:	18a8           	l32i.n	a10, a8, 4
    1620:	13a9           	s32i.n	a10, a3, 4
    1622:	2898           	l32i.n	a9, a8, 8
    1624:	2399           	s32i.n	a9, a3, 8
    1626:	3868           	l32i.n	a6, a8, 12
    1628:	3369           	s32i.n	a6, a3, 12
    162a:	10c882        	addi	a8, a8, 16
    162d:	10c332        	addi	a3, a3, 16

00001630 <de_interleave+0x64>:
    1630:	001646        	j	168d <de_interleave+0xc1>

00001633 <de_interleave+0x67>:
    1633:	11e8           	l32i.n	a14, a1, 4
    1635:	04ce16        	beqz	a14, 1685 <de_interleave+0xb9>
    1638:	31e8           	l32i.n	a14, a1, 12
    163a:	0f0c           	movi.n	a15, 0

0000163c <de_interleave+0x70>:
    163c:	801ec2        	l16ui	a12, a14, 0x100
    163f:	811ea2        	l16ui	a10, a14, 0x102
    1642:	c0aac0        	sub	a10, a10, a12
    1645:	9aad           	ae_zext16	a10, a10
    1647:	3319a6        	blti	a9, 1, 167e <de_interleave+0xb2>
    164a:	050c           	movi.n	a5, 0
    164c:	a04f30        	addx4	a4, a15, a3
    164f:	ca3b           	addi.n	a12, a10, 3
    1651:	21c2c0        	srai	a12, a12, 2
    1654:	047d           	mov.n	a7, a4

00001656 <de_interleave+0x8a>:
    1656:	551b           	addi.n	a5, a5, 1
    1658:	44ba           	add.n	a4, a4, a11
    165a:	1b1aa6        	blti	a10, 1, 1679 <de_interleave+0xad>
    165d:	15ac76        	loopgtz	a12, 1676 <de_interleave+0xaa>
    1660:	0898           	l32i.n	a9, a8, 0
    1662:	0799           	s32i.n	a9, a7, 0
    1664:	1868           	l32i.n	a6, a8, 4
    1666:	1769           	s32i.n	a6, a7, 4
    1668:	2898           	l32i.n	a9, a8, 8
    166a:	2799           	s32i.n	a9, a7, 8
    166c:	3868           	l32i.n	a6, a8, 12
    166e:	3769           	s32i.n	a6, a7, 12
    1670:	10c882        	addi	a8, a8, 16
    1673:	10c772        	addi	a7, a7, 16

00001676 <de_interleave+0xaa>:
    1676:	080d92        	l8ui	a9, a13, 8

00001679 <de_interleave+0xad>:
    1679:	047d           	mov.n	a7, a4
    167b:	d72597        	blt	a5, a9, 1656 <de_interleave+0x8a>

0000167e <de_interleave+0xb2>:
    167e:	ffaa           	add.n	a15, a15, a10
    1680:	ee2b           	addi.n	a14, a14, 2
    1682:	b69e27        	bne	a14, a2, 163c <de_interleave+0x70>

00001685 <de_interleave+0xb9>:
    1685:	01a8           	l32i.n	a10, a1, 0
    1687:	82aa90        	mull	a10, a10, a9
    168a:	a03a30        	addx4	a3, a10, a3

0000168d <de_interleave+0xc1>:
    168d:	41d8           	l32i.n	a13, a1, 16
    168f:	21c8           	l32i.n	a12, a1, 8
    1691:	dd1b           	addi.n	a13, a13, 1
    1693:	41d9           	s32i.n	a13, a1, 16
    1695:	c0ccd0        	sub	a12, a12, a13
    1698:	f6dc56        	bnez	a12, 1609 <de_interleave+0x3d>

0000169b <de_interleave+0xcf>:
    169b:	f01d           	retw.n

0000169d <de_interleave+0xd1>:
    169d:	000000                                        ...

000016a0 <one_channel_stream>:
    16a0:	004136        	entry	a1, 32
    16a3:	fa7b81        	l32r	a8, 90 (2254 <aac_getbits>)
    16a6:	04ad           	mov.n	a10, a4
    16a8:	8b0c           	movi.n	a11, 8
    16aa:	0008e0        	callx8	a8
    16ad:	050392        	l8ui	a9, a3, 5
    16b0:	0445a2        	s8i	a10, a5, 4
    16b3:	09dc           	bnez.n	a9, 16c7 <one_channel_stream+0x27>
    16b5:	e6cc           	bnez.n	a6, 16c7 <one_channel_stream+0x27>
    16b7:	02ad           	mov.n	a10, a2
    16b9:	05bd           	mov.n	a11, a5
    16bb:	04cd           	mov.n	a12, a4
    16bd:	ffc365        	call8	12f4 <ics_info>
    16c0:	031aa6        	blti	a10, 1, 16c7 <one_channel_stream+0x27>
    16c3:	0a2d           	mov.n	a2, a10
    16c5:	f01d           	retw.n

000016c7 <one_channel_stream+0x27>:
    16c7:	05ad           	mov.n	a10, a5
    16c9:	04bd           	mov.n	a11, a4
    16cb:	000d65        	call8	17a0 <section_data>
    16ce:	1c1aa6        	blti	a10, 1, 16ee <one_channel_stream+0x4e>
    16d1:	0a2d           	mov.n	a2, a10
    16d3:	f01d           	retw.n

000016d5 <one_channel_stream+0x35>:
    16d5:	0105a2        	l8ui	a10, a5, 1
    16d8:	0e2a66        	bnei	a10, 2, 16ea <one_channel_stream+0x4a>
    16db:	05ad           	mov.n	a10, a5
    16dd:	07bd           	mov.n	a11, a7
    16df:	fa7a81        	l32r	a8, c8 (15cc <de_interleave>)
    16e2:	0212c2        	l16ui	a12, a2, 4
    16e5:	62d8           	l32i.n	a13, a2, 24
    16e7:	0008e0        	callx8	a8

000016ea <one_channel_stream+0x4a>:
    16ea:	020c           	movi.n	a2, 0
    16ec:	f01d           	retw.n

000016ee <one_channel_stream+0x4e>:
    16ee:	fa7781        	l32r	a8, cc (18cc <decode_scale_factors>)
    16f1:	05ad           	mov.n	a10, a5
    16f3:	04bd           	mov.n	a11, a4
    16f5:	0008e0        	callx8	a8
    16f8:	031aa6        	blti	a10, 1, 16ff <one_channel_stream+0x5f>
    16fb:	0a2d           	mov.n	a2, a10
    16fd:	f01d           	retw.n

000016ff <one_channel_stream+0x5f>:
    16ff:	3ca432        	movi	a3, 0x43c
    1702:	86ec           	bnez.n	a6, 172e <one_channel_stream+0x8e>
    1704:	fa6681        	l32r	a8, 9c (2284 <aac_get1bit>)
    1707:	04ad           	mov.n	a10, a4
    1709:	0008e0        	callx8	a8
    170c:	7490a0        	extui	a9, a10, 0, 8
    170f:	653a           	add.n	a6, a5, a3
    1711:	7e46a2        	s8i	a10, a6, 126
    1714:	052916        	beqz	a9, 176a <one_channel_stream+0xca>
    1717:	05ad           	mov.n	a10, a5
    1719:	04cd           	mov.n	a12, a4
    171b:	fa6d81        	l32r	a8, d0 (13c0 <pulse_data>)
    171e:	05d5b2        	addmi	a11, a5, 0x500
    1721:	becbb2        	addi	a11, a11, -66
    1724:	0008e0        	callx8	a8
    1727:	3f1aa6        	blti	a10, 1, 176a <one_channel_stream+0xca>
    172a:	0a2d           	mov.n	a2, a10
    172c:	f01d           	retw.n

0000172e <one_channel_stream+0x8e>:
    172e:	04cd           	mov.n	a12, a4
    1730:	02ad           	mov.n	a10, a2
    1732:	fa6881        	l32r	a8, d4 (1ab4 <spectral_data>)
    1735:	05bd           	mov.n	a11, a5
    1737:	07dd           	mov.n	a13, a7
    1739:	0008e0        	callx8	a8
    173c:	031aa6        	blti	a10, 1, 1743 <one_channel_stream+0xa3>
    173f:	0a2d           	mov.n	a2, a10
    1741:	f01d           	retw.n

00001743 <one_channel_stream+0xa3>:
    1743:	953a           	add.n	a9, a5, a3
    1745:	7e0992        	l8ui	a9, a9, 126
    1748:	f89916        	beqz	a9, 16d5 <one_channel_stream+0x35>
    174b:	0105a2        	l8ui	a10, a5, 1
    174e:	132a26        	beqi	a10, 2, 1765 <one_channel_stream+0xc5>
    1751:	05ad           	mov.n	a10, a5
    1753:	07bd           	mov.n	a11, a7
    1755:	0212c2        	l16ui	a12, a2, 4
    1758:	fecae5        	call8	408 <pulse_decode>
    175b:	021ae6        	bgei	a10, 1, 1761 <one_channel_stream+0xc1>
    175e:	ffdcc6        	j	16d5 <one_channel_stream+0x35>
    1761:	0a2d           	mov.n	a2, a10
    1763:	f01d           	retw.n

00001765 <one_channel_stream+0xc5>:
    1765:	64a022        	movi	a2, 100
    1768:	f01d           	retw.n

0000176a <one_channel_stream+0xca>:
    176a:	fa4c81        	l32r	a8, 9c (2284 <aac_get1bit>)
    176d:	04ad           	mov.n	a10, a4
    176f:	0008e0        	callx8	a8
    1772:	7f46a2        	s8i	a10, a6, 127
    1775:	7490a0        	extui	a9, a10, 0, 8
    1778:	e98c           	beqz.n	a9, 178a <one_channel_stream+0xea>
    177a:	05ad           	mov.n	a10, a5
    177c:	04cd           	mov.n	a12, a4
    177e:	fa5681        	l32r	a8, d8 (199c <tns_data>)
    1781:	05d5b2        	addmi	a11, a5, 0x500
    1784:	cccbb2        	addi	a11, a11, -52
    1787:	0008e0        	callx8	a8

0000178a <one_channel_stream+0xea>:
    178a:	fa4481        	l32r	a8, 9c (2284 <aac_get1bit>)
    178d:	04ad           	mov.n	a10, a4
    178f:	0008e0        	callx8	a8
    1792:	8046a2        	s8i	a10, a6, 128
    1795:	7490a0        	extui	a9, a10, 0, 8
    1798:	f92916        	beqz	a9, 172e <one_channel_stream+0x8e>
    179b:	60a022        	movi	a2, 96
    179e:	f01d           	retw.n

000017a0 <section_data>:
    17a0:	008136        	entry	a1, 64
    17a3:	6129           	s32i.n	a2, a1, 24
    17a5:	060282        	l8ui	a8, a2, 6
    17a8:	3a0c           	movi.n	a10, 3
    17aa:	560c           	movi.n	a6, 5
    17ac:	0102b2        	l8ui	a11, a2, 1
    17af:	03d2e2        	addmi	a14, a2, 0x300
    17b2:	02d2d2        	addmi	a13, a2, 0x200
    17b5:	04d292        	addmi	a9, a2, 0x400
    17b8:	c0c9c2        	addi	a12, a9, -64
    17bb:	58cdd2        	addi	a13, a13, 88
    17be:	d0cee2        	addi	a14, a14, -48
    17c1:	11e9           	s32i.n	a14, a1, 4
    17c3:	51d9           	s32i.n	a13, a1, 20
    17c5:	41c9           	s32i.n	a12, a1, 16
    17c7:	fecbb2        	addi	a11, a11, -2
    17ca:	836ab0        	moveqz	a6, a10, a11
    17cd:	0e5816        	beqz	a8, 18b6 <section_data+0x116>
    17d0:	401600        	ssl	a6
    17d3:	170c           	movi.n	a7, 1
    17d5:	0b0c           	movi.n	a11, 0
    17d7:	0c0c           	movi.n	a12, 0
    17d9:	3cc9d2        	addi	a13, a9, 60
    17dc:	21d9           	s32i.n	a13, a1, 8
    17de:	31c9           	s32i.n	a12, a1, 12
    17e0:	71b9           	s32i.n	a11, a1, 28
    17e2:	a17700        	sll	a7, a7
    17e5:	770b           	addi.n	a7, a7, -1

000017e7 <section_data+0x47>:
    17e7:	61d8           	l32i.n	a13, a1, 24
    17e9:	000dd2        	l8ui	a13, a13, 0
    17ec:	09ed16        	beqz	a13, 188e <section_data+0xee>
    17ef:	020c           	movi.n	a2, 0
    17f1:	0e0c           	movi.n	a14, 0
    17f3:	01e9           	s32i.n	a14, a1, 0

000017f5 <section_data+0x55>:
    17f5:	fa2681        	l32r	a8, 90 (2254 <aac_getbits>)
    17f8:	03ad           	mov.n	a10, a3
    17fa:	4b0c           	movi.n	a11, 4
    17fc:	0008e0        	callx8	a8
    17ff:	7198           	l32i.n	a9, a1, 28
    1801:	0a4d           	mov.n	a4, a10
    1803:	77a0b2        	movi	a11, 119
    1806:	02ab97        	bge	a11, a9, 180c <section_data+0x6c>
    1809:	002b46        	j	18ba <section_data+0x11a>
    180c:	51c8           	l32i.n	a12, a1, 20
    180e:	004ca2        	s8i	a10, a12, 0
    1811:	da0c           	movi.n	a10, 13
    1813:	cc1b           	addi.n	a12, a12, 1
    1815:	51c9           	s32i.n	a12, a1, 20
    1817:	0694a7        	bne	a4, a10, 1821 <section_data+0x81>
    181a:	21e8           	l32i.n	a14, a1, 8
    181c:	1d0c           	movi.n	a13, 1
    181e:	7d4ed2        	s8i	a13, a14, 125

00001821 <section_data+0x81>:
    1821:	fa1b81        	l32r	a8, 90 (2254 <aac_getbits>)
    1824:	03ad           	mov.n	a10, a3
    1826:	06bd           	mov.n	a11, a6
    1828:	0008e0        	callx8	a8
    182b:	74b0a0        	extui	a11, a10, 0, 8
    182e:	1697b7        	bne	a7, a11, 1848 <section_data+0xa8>
    1831:	050c           	movi.n	a5, 0

00001833 <section_data+0x93>:
    1833:	55ba           	add.n	a5, a5, a11
    1835:	fa1681        	l32r	a8, 90 (2254 <aac_getbits>)
    1838:	03ad           	mov.n	a10, a3
    183a:	06bd           	mov.n	a11, a6
    183c:	0008e0        	callx8	a8
    183f:	74b0a0        	extui	a11, a10, 0, 8
    1842:	ed17b7        	beq	a7, a11, 1833 <section_data+0x93>
    1845:	000046        	j	184a <section_data+0xaa>

00001848 <section_data+0xa8>:
    1848:	050c           	movi.n	a5, 0

0000184a <section_data+0xaa>:
    184a:	01a8           	l32i.n	a10, a1, 0
    184c:	77a0d2        	movi	a13, 119
    184f:	11c8           	l32i.n	a12, a1, 4
    1851:	55ba           	add.n	a5, a5, a11
    1853:	b52a           	add.n	a11, a5, a2
    1855:	119b80        	slli	a9, a11, 8
    1858:	929a           	add.n	a9, a2, a9
    185a:	09cc94        	ae_s16ip	a9, a12, 2
    185d:	5e3db7        	bltu	a13, a11, 18bf <section_data+0x11f>
    1860:	603da7        	bltu	a13, a10, 18c4 <section_data+0x124>
    1863:	11c9           	s32i.n	a12, a1, 4
    1865:	71e8           	l32i.n	a14, a1, 28
    1867:	61d8           	l32i.n	a13, a1, 24
    1869:	aa1b           	addi.n	a10, a10, 1
    186b:	ee1b           	addi.n	a14, a14, 1
    186d:	71e9           	s32i.n	a14, a1, 28
    186f:	0bb2b7        	bgeu	a2, a11, 187e <section_data+0xde>
    1872:	4198           	l32i.n	a9, a1, 16
    1874:	992a           	add.n	a9, a9, a2
    1876:	049576        	loopnez	a5, 187e <section_data+0xde>
    1879:	004942        	s8i	a4, a9, 0
    187c:	991b           	addi.n	a9, a9, 1

0000187e <section_data+0xde>:
    187e:	000dd2        	l8ui	a13, a13, 0
    1881:	01a9           	s32i.n	a10, a1, 0
    1883:	0b2d           	mov.n	a2, a11
    1885:	02bbd7        	bgeu	a11, a13, 188b <section_data+0xeb>
    1888:	ffda46        	j	17f5 <section_data+0x55>
    188b:	0000c6        	j	1892 <section_data+0xf2>

0000188e <section_data+0xee>:
    188e:	0e0c           	movi.n	a14, 0
    1890:	01e9           	s32i.n	a14, a1, 0

00001892 <section_data+0xf2>:
    1892:	31f8           	l32i.n	a15, a1, 12
    1894:	6188           	l32i.n	a8, a1, 24
    1896:	0198           	l32i.n	a9, a1, 0
    1898:	b8fa           	add.n	a11, a8, a15
    189a:	04dbb2        	addmi	a11, a11, 0x400
    189d:	ff1b           	addi.n	a15, a15, 1
    189f:	b8cbb2        	addi	a11, a11, -72
    18a2:	804b92        	s8i	a9, a11, 128
    18a5:	4198           	l32i.n	a9, a1, 16
    18a7:	31f9           	s32i.n	a15, a1, 12
    18a9:	060882        	l8ui	a8, a8, 6
    18ac:	99da           	add.n	a9, a9, a13
    18ae:	4199           	s32i.n	a9, a1, 16
    18b0:	02bf87        	bgeu	a15, a8, 18b6 <section_data+0x116>
    18b3:	ffcc06        	j	17e7 <section_data+0x47>

000018b6 <section_data+0x116>:
    18b6:	020c           	movi.n	a2, 0
    18b8:	f01d           	retw.n

000018ba <section_data+0x11a>:
    18ba:	65a022        	movi	a2, 101
    18bd:	f01d           	retw.n

000018bf <section_data+0x11f>:
    18bf:	66a022        	movi	a2, 102
    18c2:	f01d           	retw.n

000018c4 <section_data+0x124>:
    18c4:	67a022        	movi	a2, 103
    18c7:	f01d           	retw.n

000018c9 <section_data+0x129>:
    18c9:	000000                                        ...

000018cc <decode_scale_factors>:
    18cc:	006136        	entry	a1, 48
    18cf:	0602b2        	l8ui	a11, a2, 6
    18d2:	040262        	l8ui	a6, a2, 4
    18d5:	04d272        	addmi	a7, a2, 0x400
    18d8:	01d242        	addmi	a4, a2, 0x100
    18db:	68c442        	addi	a4, a4, 104
    18de:	c0c772        	addi	a7, a7, -64
    18e1:	a6c682        	addi	a8, a6, -90
    18e4:	3189           	s32i.n	a8, a1, 12
    18e6:	0a6b16        	beqz	a11, 1990 <decode_scale_factors+0xc4>
    18e9:	0002a2        	l8ui	a10, a2, 0
    18ec:	1e0c           	movi.n	a14, 1
    18ee:	0f0c           	movi.n	a15, 0
    18f0:	080c           	movi.n	a8, 0
    18f2:	1189           	s32i.n	a8, a1, 4
    18f4:	01f9           	s32i.n	a15, a1, 0
    18f6:	21e9           	s32i.n	a14, a1, 8

000018f8 <decode_scale_factors+0x2c>:
    18f8:	021ae6        	bgei	a10, 1, 18fe <decode_scale_factors+0x32>
    18fb:	0020c6        	j	1982 <decode_scale_factors+0xb6>
    18fe:	050c           	movi.n	a5, 0

00001900 <decode_scale_factors+0x34>:
    1900:	a57a           	add.n	a10, a5, a7
    1902:	000aa2        	l8ui	a10, a10, 0
    1905:	fb0c           	movi.n	a11, 15
    1907:	070a16        	beqz	a10, 197b <decode_scale_factors+0xaf>
    190a:	5a1ab7        	beq	a10, a11, 1968 <decode_scale_factors+0x9c>
    190d:	ec0c           	movi.n	a12, 14
    190f:	551ac7        	beq	a10, a12, 1968 <decode_scale_factors+0x9c>
    1912:	dd0c           	movi.n	a13, 13
    1914:	251ad7        	beq	a10, a13, 193d <decode_scale_factors+0x71>
    1917:	03ad           	mov.n	a10, a3
    1919:	f9f081        	l32r	a8, dc (2300 <dehuffman_sf>)
    191c:	090c           	movi.n	a9, 0
    191e:	649d           	ae_s16i.n	a9, a4, 0
    1920:	0008e0        	callx8	a8
    1923:	66aa           	add.n	a6, a6, a10
    1925:	06b696        	bltz	a6, 1994 <decode_scale_factors+0xc8>
    1928:	68f6e6        	bgei	a6, 0x100, 1994 <decode_scale_factors+0xc8>
    192b:	646d           	ae_s16i.n	a6, a4, 0

0000192d <decode_scale_factors+0x61>:
    192d:	0002a2        	l8ui	a10, a2, 0
    1930:	442b           	addi.n	a4, a4, 2
    1932:	551b           	addi.n	a5, a5, 1
    1934:	c825a7        	blt	a5, a10, 1900 <decode_scale_factors+0x34>
    1937:	0602b2        	l8ui	a11, a2, 6
    193a:	001106        	j	1982 <decode_scale_factors+0xb6>

0000193d <decode_scale_factors+0x71>:
    193d:	21a8           	l32i.n	a10, a1, 8
    193f:	2a9c           	beqz.n	a10, 1955 <decode_scale_factors+0x89>
    1941:	f9d381        	l32r	a8, 90 (2254 <aac_getbits>)
    1944:	03ad           	mov.n	a10, a3
    1946:	9b0c           	movi.n	a11, 9
    1948:	0008e0        	callx8	a8
    194b:	ffdaa2        	addmi	a10, a10, 0xffffff00
    194e:	090c           	movi.n	a9, 0
    1950:	2199           	s32i.n	a9, a1, 8
    1952:	0001c6        	j	195d <decode_scale_factors+0x91>

00001955 <decode_scale_factors+0x89>:
    1955:	f9e181        	l32r	a8, dc (2300 <dehuffman_sf>)
    1958:	03ad           	mov.n	a10, a3
    195a:	0008e0        	callx8	a8

0000195d <decode_scale_factors+0x91>:
    195d:	3198           	l32i.n	a9, a1, 12
    195f:	99aa           	add.n	a9, a9, a10
    1961:	3199           	s32i.n	a9, a1, 12
    1963:	649d           	ae_s16i.n	a9, a4, 0
    1965:	fff106        	j	192d <decode_scale_factors+0x61>

00001968 <decode_scale_factors+0x9c>:
    1968:	f9dd81        	l32r	a8, dc (2300 <dehuffman_sf>)
    196b:	03ad           	mov.n	a10, a3
    196d:	0008e0        	callx8	a8
    1970:	0198           	l32i.n	a9, a1, 0
    1972:	99aa           	add.n	a9, a9, a10
    1974:	0199           	s32i.n	a9, a1, 0
    1976:	649d           	ae_s16i.n	a9, a4, 0
    1978:	ffec46        	j	192d <decode_scale_factors+0x61>

0000197b <decode_scale_factors+0xaf>:
    197b:	0a0c           	movi.n	a10, 0
    197d:	64ad           	ae_s16i.n	a10, a4, 0
    197f:	ffea86        	j	192d <decode_scale_factors+0x61>

00001982 <decode_scale_factors+0xb6>:
    1982:	11c8           	l32i.n	a12, a1, 4
    1984:	77aa           	add.n	a7, a7, a10
    1986:	cc1b           	addi.n	a12, a12, 1
    1988:	11c9           	s32i.n	a12, a1, 4
    198a:	02acb7        	bge	a12, a11, 1990 <decode_scale_factors+0xc4>
    198d:	ffd9c6        	j	18f8 <decode_scale_factors+0x2c>

00001990 <decode_scale_factors+0xc4>:
    1990:	020c           	movi.n	a2, 0
    1992:	f01d           	retw.n

00001994 <decode_scale_factors+0xc8>:
    1994:	68a022        	movi	a2, 104
    1997:	f01d           	retw.n

00001999 <decode_scale_factors+0xcd>:
    1999:	000000                                        ...

0000199c <tns_data>:
    199c:	00a136        	entry	a1, 80
    199f:	2129           	s32i.n	a2, a1, 8
    19a1:	7139           	s32i.n	a3, a1, 28
    19a3:	010282        	l8ui	a8, a2, 1
    19a6:	01d352        	addmi	a5, a3, 0x100
    19a9:	90c552        	addi	a5, a5, -112
    19ac:	0c2866        	bnei	a8, 2, 19bc <tns_data+0x20>
    19af:	490c           	movi.n	a9, 4
    19b1:	1b0c           	movi.n	a11, 1
    19b3:	3c0c           	movi.n	a12, 3
    19b5:	51c9           	s32i.n	a12, a1, 20
    19b7:	11b9           	s32i.n	a11, a1, 4
    19b9:	000246        	j	19c6 <tns_data+0x2a>

000019bc <tns_data+0x20>:
    19bc:	690c           	movi.n	a9, 6
    19be:	2d0c           	movi.n	a13, 2
    19c0:	5e0c           	movi.n	a14, 5
    19c2:	51e9           	s32i.n	a14, a1, 20
    19c4:	11d9           	s32i.n	a13, a1, 4

000019c6 <tns_data+0x2a>:
    19c6:	21f8           	l32i.n	a15, a1, 8
    19c8:	070ff2        	l8ui	a15, a15, 7
    19cb:	0e3f16        	beqz	a15, 1ab2 <tns_data+0x116>
    19ce:	180c           	movi.n	a8, 1
    19d0:	51a8           	l32i.n	a10, a1, 20
    19d2:	0d0c           	movi.n	a13, 0
    19d4:	01d9           	s32i.n	a13, a1, 0
    19d6:	6a9a           	add.n	a6, a10, a9
    19d8:	401a00        	ssl	a10
    19db:	a18800        	sll	a8, a8
    19de:	880b           	addi.n	a8, a8, -1
    19e0:	4189           	s32i.n	a8, a1, 16

000019e2 <tns_data+0x46>:
    19e2:	f9ab81        	l32r	a8, 90 (2254 <aac_getbits>)
    19e5:	04ad           	mov.n	a10, a4
    19e7:	11b8           	l32i.n	a11, a1, 4
    19e9:	0008e0        	callx8	a8
    19ec:	7490a0        	extui	a9, a10, 0, 8
    19ef:	0178           	l32i.n	a7, a1, 0
    19f1:	71a8           	l32i.n	a10, a1, 28
    19f3:	aa7a           	add.n	a10, a10, a7
    19f5:	81a9           	s32i.n	a10, a1, 32
    19f7:	004a92        	s8i	a9, a10, 0
    19fa:	0a2916        	beqz	a9, 1aa0 <tns_data+0x104>
    19fd:	f9a781        	l32r	a8, 9c (2284 <aac_get1bit>)
    1a00:	04ad           	mov.n	a10, a4
    1a02:	0008e0        	callx8	a8
    1a05:	8198           	l32i.n	a9, a1, 32
    1a07:	4b0c           	movi.n	a11, 4
    1a09:	74c0a0        	extui	a12, a10, 0, 8
    1a0c:	0849a2        	s8i	a10, a9, 8
    1a0f:	3a0c           	movi.n	a10, 3
    1a11:	000992        	l8ui	a9, a9, 0
    1a14:	93abc0        	movnez	a10, a11, a12
    1a17:	31a9           	s32i.n	a10, a1, 12
    1a19:	083916        	beqz	a9, 1aa0 <tns_data+0x104>
    1a1c:	030c           	movi.n	a3, 0
    1a1e:	11a7e0        	slli	a10, a7, 2
    1a21:	61a9           	s32i.n	a10, a1, 24

00001a23 <tns_data+0x87>:
    1a23:	f99b81        	l32r	a8, 90 (2254 <aac_getbits>)
    1a26:	04ad           	mov.n	a10, a4
    1a28:	06bd           	mov.n	a11, a6
    1a2a:	0008e0        	callx8	a8
    1a2d:	7178           	l32i.n	a7, a1, 28
    1a2f:	51b8           	l32i.n	a11, a1, 20
    1a31:	4198           	l32i.n	a9, a1, 16
    1a33:	6188           	l32i.n	a8, a1, 24
    1a35:	400b00        	ssr	a11
    1a38:	1099a0        	and	a9, a9, a10
    1a3b:	883a           	add.n	a8, a8, a3
    1a3d:	778a           	add.n	a7, a7, a8
    1a3f:	304792        	s8i	a9, a7, 48
    1a42:	91a0a0        	srl	a10, a10
    1a45:	1047a2        	s8i	a10, a7, 16
    1a48:	749090        	extui	a9, a9, 0, 8
    1a4b:	042916        	beqz	a9, 1a91 <tns_data+0xf5>
    1a4e:	f99081        	l32r	a8, 90 (2254 <aac_getbits>)
    1a51:	04ad           	mov.n	a10, a4
    1a53:	2b0c           	movi.n	a11, 2
    1a55:	0008e0        	callx8	a8
    1a58:	9169           	s32i.n	a6, a1, 36
    1a5a:	0490a0        	extui	a9, a10, 0, 1
    1a5d:	41c1a0        	srli	a12, a10, 1
    1a60:	3007b2        	l8ui	a11, a7, 48
    1a63:	5047c2        	s8i	a12, a7, 80
    1a66:	704792        	s8i	a9, a7, 112
    1a69:	4bac           	beqz.n	a11, 1a91 <tns_data+0xf5>
    1a6b:	3128           	l32i.n	a2, a1, 12
    1a6d:	060c           	movi.n	a6, 0
    1a6f:	c02290        	sub	a2, a2, a9
    1a72:	742020        	extui	a2, a2, 0, 8

00001a75 <tns_data+0xd9>:
    1a75:	f98681        	l32r	a8, 90 (2254 <aac_getbits>)
    1a78:	02bd           	mov.n	a11, a2
    1a7a:	04ad           	mov.n	a10, a4
    1a7c:	0008e0        	callx8	a8
    1a7f:	661b           	addi.n	a6, a6, 1
    1a81:	0045a2        	s8i	a10, a5, 0
    1a84:	300792        	l8ui	a9, a7, 48
    1a87:	551b           	addi.n	a5, a5, 1
    1a89:	746060        	extui	a6, a6, 0, 8
    1a8c:	e53697        	bltu	a6, a9, 1a75 <tns_data+0xd9>
    1a8f:	9168           	l32i.n	a6, a1, 36
    1a91:	8188           	l32i.n	a8, a1, 32
    1a93:	000882        	l8ui	a8, a8, 0
    1a96:	331b           	addi.n	a3, a3, 1
    1a98:	743030        	extui	a3, a3, 0, 8
    1a9b:	843387        	bltu	a3, a8, 1a23 <tns_data+0x87>
    1a9e:	0178           	l32i.n	a7, a1, 0

00001aa0 <tns_data+0x104>:
    1aa0:	2198           	l32i.n	a9, a1, 8
    1aa2:	771b           	addi.n	a7, a7, 1
    1aa4:	070992        	l8ui	a9, a9, 7
    1aa7:	747070        	extui	a7, a7, 0, 8
    1aaa:	0179           	s32i.n	a7, a1, 0
    1aac:	02b797        	bgeu	a7, a9, 1ab2 <tns_data+0x116>
    1aaf:	ffcbc6        	j	19e2 <tns_data+0x46>

00001ab2 <tns_data+0x116>:
    1ab2:	f01d           	retw.n

00001ab4 <spectral_data>:
    1ab4:	00e136        	entry	a1, 112
    1ab7:	05cd           	mov.n	a12, a5
    1ab9:	03ad           	mov.n	a10, a3
    1abb:	b149           	s32i.n	a4, a1, 44
    1abd:	082c           	movi.n	a8, 32
    1abf:	021292        	l16ui	a9, a2, 4
    1ac2:	03d3d2        	addmi	a13, a3, 0x300
    1ac5:	02d3b2        	addmi	a11, a3, 0x200
    1ac8:	01d3e2        	addmi	a14, a3, 0x100
    1acb:	10c3f2        	addi	a15, a3, 16
    1ace:	e1f9           	s32i.n	a15, a1, 56
    1ad0:	68cee2        	addi	a14, a14, 104
    1ad3:	58cbb2        	addi	a11, a11, 88
    1ad6:	d0cdd2        	addi	a13, a13, -48
    1ad9:	61d9           	s32i.n	a13, a1, 24
    1adb:	51b9           	s32i.n	a11, a1, 20
    1add:	a1e9           	s32i.n	a14, a1, 40
    1adf:	419390        	srli	a9, a9, 3
    1ae2:	0103e2        	l8ui	a14, a3, 1
    1ae5:	0603b2        	l8ui	a11, a3, 6
    1ae8:	dd1c           	movi.n	a13, 29
    1aea:	fecee2        	addi	a14, a14, -2
    1aed:	838de0        	moveqz	a8, a13, a14
    1af0:	c189           	s32i.n	a8, a1, 48
    1af2:	1b0b16        	beqz	a11, 1ca6 <spectral_data+0x1f2>
    1af5:	7199           	s32i.n	a9, a1, 28
    1af7:	020c           	movi.n	a2, 0
    1af9:	0f0c           	movi.n	a15, 0
    1afb:	080c           	movi.n	a8, 0
    1afd:	8189           	s32i.n	a8, a1, 32
    1aff:	91f9           	s32i.n	a15, a1, 36
    1b01:	4129           	s32i.n	a2, a1, 16
    1b03:	b8a322        	movi	a2, 0x3b8

00001b06 <spectral_data+0x52>:
    1b06:	81e8           	l32i.n	a14, a1, 32
    1b08:	e178           	l32i.n	a7, a1, 56
    1b0a:	9168           	l32i.n	a6, a1, 36
    1b0c:	eeaa           	add.n	a14, a14, a10
    1b0e:	fe2a           	add.n	a15, a14, a2
    1b10:	d1f9           	s32i.n	a15, a1, 52
    1b12:	800ff2        	l8ui	a15, a15, 128
    1b15:	a138           	l32i.n	a3, a1, 40
    1b17:	a066c0        	addx4	a6, a6, a12
    1b1a:	156f16        	beqz	a15, 1c74 <spectral_data+0x1c0>
    1b1d:	1061e2        	s32i	a14, a1, 64
    1b20:	1161c2        	s32i	a12, a1, 68
    1b23:	1261a2        	s32i	a10, a1, 72
    1b26:	080c           	movi.n	a8, 0
    1b28:	f189           	s32i.n	a8, a1, 60

00001b2a <spectral_data+0x76>:
    1b2a:	61b8           	l32i.n	a11, a1, 24
    1b2c:	41c8           	l32i.n	a12, a1, 16
    1b2e:	51a8           	l32i.n	a10, a1, 20
    1b30:	4b5d           	ae_l16ui.n	a5, a11, 0
    1b32:	e128           	l32i.n	a2, a1, 56
    1b34:	000ae2        	l8ui	a14, a10, 0
    1b37:	744850        	extui	a4, a5, 8, 8
    1b3a:	745050        	extui	a5, a5, 0, 8
    1b3d:	908520        	addx2	a8, a5, a2
    1b40:	488d           	ae_l16ui.n	a8, a8, 0
    1b42:	902420        	addx2	a2, a4, a2
    1b45:	422d           	ae_l16ui.n	a2, a2, 0
    1b47:	bb2b           	addi.n	a11, a11, 2
    1b49:	aa1b           	addi.n	a10, a10, 1
    1b4b:	c02280        	sub	a2, a2, a8
    1b4e:	158296        	bltz	a2, 1caa <spectral_data+0x1f6>
    1b51:	61b9           	s32i.n	a11, a1, 24
    1b53:	51a9           	s32i.n	a10, a1, 20
    1b55:	cc2a           	add.n	a12, a12, a2
    1b57:	00a4d2        	movi	a13, 0x400
    1b5a:	02adc7        	bge	a13, a12, 1b60 <spectral_data+0xac>
    1b5d:	005346        	j	1cae <spectral_data+0x1fa>
    1b60:	41c9           	s32i.n	a12, a1, 16
    1b62:	0f9e16        	beqz	a14, 1c5f <spectral_data+0x1ab>
    1b65:	f3ce82        	addi	a8, a14, -13
    1b68:	0f3816        	beqz	a8, 1c5f <spectral_data+0x1ab>
    1b6b:	f1ce92        	addi	a9, a14, -15
    1b6e:	0ed916        	beqz	a9, 1c5f <spectral_data+0x1ab>
    1b71:	f2cea2        	addi	a10, a14, -14
    1b74:	0e7a16        	beqz	a10, 1c5f <spectral_data+0x1ab>
    1b77:	be0b           	addi.n	a11, a14, -1
    1b79:	0c8b16        	beqz	a11, 1c45 <spectral_data+0x191>
    1b7c:	fecec2        	addi	a12, a14, -2
    1b7f:	0c2c16        	beqz	a12, 1c45 <spectral_data+0x191>
    1b82:	023e66        	bnei	a14, 3, 1b88 <spectral_data+0xd4>
    1b85:	002686        	j	1c23 <spectral_data+0x16f>
    1b88:	024e66        	bnei	a14, 4, 1b8e <spectral_data+0xda>
    1b8b:	002506        	j	1c23 <spectral_data+0x16f>
    1b8e:	775e26        	beqi	a14, 5, 1c09 <spectral_data+0x155>
    1b91:	746e26        	beqi	a14, 6, 1c09 <spectral_data+0x155>
    1b94:	2c7e26        	beqi	a14, 7, 1bc4 <spectral_data+0x110>
    1b97:	9d0c           	movi.n	a13, 9
    1b99:	271ed7        	beq	a14, a13, 1bc4 <spectral_data+0x110>
    1b9c:	248e26        	beqi	a14, 8, 1bc4 <spectral_data+0x110>
    1b9f:	219e26        	beqi	a14, 10, 1bc4 <spectral_data+0x110>
    1ba2:	c05450        	sub	a5, a4, a5
    1ba5:	b80c           	movi.n	a8, 11
    1ba7:	359e87        	bne	a14, a8, 1be0 <spectral_data+0x12c>
    1baa:	03ad           	mov.n	a10, a3
    1bac:	07bd           	mov.n	a11, a7
    1bae:	06cd           	mov.n	a12, a6
    1bb0:	c1d8           	l32i.n	a13, a1, 48
    1bb2:	f94b81        	l32r	a8, e0 (2640 <dehuffman_cb11>)
    1bb5:	05ed           	mov.n	a14, a5
    1bb7:	b1f8           	l32i.n	a15, a1, 44
    1bb9:	0008e0        	callx8	a8
    1bbc:	d1f8           	l32i.n	a15, a1, 52
    1bbe:	800ff2        	l8ui	a15, a15, 128
    1bc1:	0006c6        	j	1be0 <spectral_data+0x12c>

00001bc4 <spectral_data+0x110>:
    1bc4:	03ad           	mov.n	a10, a3
    1bc6:	07bd           	mov.n	a11, a7
    1bc8:	06cd           	mov.n	a12, a6
    1bca:	c1d8           	l32i.n	a13, a1, 48
    1bcc:	c05450        	sub	a5, a4, a5
    1bcf:	b198           	l32i.n	a9, a1, 44
    1bd1:	f94481        	l32r	a8, e4 (2564 <dehuffman_cb7_8_9_10>)
    1bd4:	0199           	s32i.n	a9, a1, 0
    1bd6:	05fd           	mov.n	a15, a5
    1bd8:	0008e0        	callx8	a8

00001bdb <spectral_data+0x127>:
    1bdb:	d1f8           	l32i.n	a15, a1, 52
    1bdd:	800ff2        	l8ui	a15, a15, 128

00001be0 <spectral_data+0x12c>:
    1be0:	1122e0        	slli	a2, a2, 2

00001be3 <spectral_data+0x12f>:
    1be3:	903530        	addx2	a3, a5, a3
    1be6:	f188           	l32i.n	a8, a1, 60
    1be8:	907570        	addx2	a7, a5, a7
    1beb:	662a           	add.n	a6, a6, a2
    1bed:	881b           	addi.n	a8, a8, 1
    1bef:	f189           	s32i.n	a8, a1, 60
    1bf1:	02a8f7        	bge	a8, a15, 1bf7 <spectral_data+0x143>
    1bf4:	ffcc86        	j	1b2a <spectral_data+0x76>
    1bf7:	1121c2        	l32i	a12, a1, 68
    1bfa:	1221a2        	l32i	a10, a1, 72
    1bfd:	1021e2        	l32i	a14, a1, 64
    1c00:	b8a322        	movi	a2, 0x3b8
    1c03:	060ab2        	l8ui	a11, a10, 6
    1c06:	001a86        	j	1c74 <spectral_data+0x1c0>

00001c09 <spectral_data+0x155>:
    1c09:	03ad           	mov.n	a10, a3
    1c0b:	07bd           	mov.n	a11, a7
    1c0d:	06cd           	mov.n	a12, a6
    1c0f:	c1d8           	l32i.n	a13, a1, 48
    1c11:	c05450        	sub	a5, a4, a5
    1c14:	b198           	l32i.n	a9, a1, 44
    1c16:	f93481        	l32r	a8, e8 (24a8 <dehuffman_cb5_cb6>)
    1c19:	0199           	s32i.n	a9, a1, 0
    1c1b:	05fd           	mov.n	a15, a5
    1c1d:	0008e0        	callx8	a8
    1c20:	ffedc6        	j	1bdb <spectral_data+0x127>

00001c23 <spectral_data+0x16f>:
    1c23:	03ad           	mov.n	a10, a3
    1c25:	07bd           	mov.n	a11, a7
    1c27:	06cd           	mov.n	a12, a6
    1c29:	c1d8           	l32i.n	a13, a1, 48
    1c2b:	c05450        	sub	a5, a4, a5
    1c2e:	b198           	l32i.n	a9, a1, 44
    1c30:	f92f81        	l32r	a8, ec (23b8 <dehuffman_cb3_cb4>)
    1c33:	0199           	s32i.n	a9, a1, 0
    1c35:	05fd           	mov.n	a15, a5
    1c37:	0008e0        	callx8	a8

00001c3a <spectral_data+0x186>:
    1c3a:	1122e0        	slli	a2, a2, 2

00001c3d <spectral_data+0x189>:
    1c3d:	d1f8           	l32i.n	a15, a1, 52
    1c3f:	800ff2        	l8ui	a15, a15, 128
    1c42:	ffe746        	j	1be3 <spectral_data+0x12f>

00001c45 <spectral_data+0x191>:
    1c45:	03ad           	mov.n	a10, a3
    1c47:	07bd           	mov.n	a11, a7
    1c49:	06cd           	mov.n	a12, a6
    1c4b:	c1d8           	l32i.n	a13, a1, 48
    1c4d:	c05450        	sub	a5, a4, a5
    1c50:	b198           	l32i.n	a9, a1, 44
    1c52:	f92781        	l32r	a8, f0 (2320 <dehuffman_cb1_cb2>)
    1c55:	0199           	s32i.n	a9, a1, 0
    1c57:	05fd           	mov.n	a15, a5
    1c59:	0008e0        	callx8	a8
    1c5c:	fff686        	j	1c3a <spectral_data+0x186>

00001c5f <spectral_data+0x1ab>:
    1c5f:	06ad           	mov.n	a10, a6
    1c61:	0b0c           	movi.n	a11, 0
    1c63:	f8ff81        	l32r	a8, 60 (34c0 <xt_memset1>)
    1c66:	1122e0        	slli	a2, a2, 2
    1c69:	02cd           	mov.n	a12, a2
    1c6b:	0008e0        	callx8	a8
    1c6e:	c05450        	sub	a5, a4, a5
    1c71:	fff206        	j	1c3d <spectral_data+0x189>

00001c74 <spectral_data+0x1c0>:
    1c74:	080e82        	l8ui	a8, a14, 8
    1c77:	a1d8           	l32i.n	a13, a1, 40
    1c79:	000a52        	l8ui	a5, a10, 0
    1c7c:	050af2        	l8ui	a15, a10, 5
    1c7f:	8198           	l32i.n	a9, a1, 32
    1c81:	230c           	movi.n	a3, 2
    1c83:	90ff30        	addx2	a15, a15, a3
    1c86:	991b           	addi.n	a9, a9, 1
    1c88:	8199           	s32i.n	a9, a1, 32
    1c8a:	90d5d0        	addx2	a13, a5, a13
    1c8d:	a1d9           	s32i.n	a13, a1, 40
    1c8f:	7158           	l32i.n	a5, a1, 28
    1c91:	e1d8           	l32i.n	a13, a1, 56
    1c93:	9138           	l32i.n	a3, a1, 36
    1c95:	825580        	mull	a5, a5, a8
    1c98:	ddfa           	add.n	a13, a13, a15
    1c9a:	e1d9           	s32i.n	a13, a1, 56
    1c9c:	335a           	add.n	a3, a3, a5
    1c9e:	9139           	s32i.n	a3, a1, 36
    1ca0:	02a9b7        	bge	a9, a11, 1ca6 <spectral_data+0x1f2>
    1ca3:	ff97c6        	j	1b06 <spectral_data+0x52>

00001ca6 <spectral_data+0x1f2>:
    1ca6:	020c           	movi.n	a2, 0
    1ca8:	f01d           	retw.n

00001caa <spectral_data+0x1f6>:
    1caa:	725c           	movi.n	a2, 87
    1cac:	f01d           	retw.n

00001cae <spectral_data+0x1fa>:
    1cae:	625c           	movi.n	a2, 86
    1cb0:	f01d           	retw.n

00001cb2 <spectral_data+0x1fe>:
	...

00001cb4 <loas_get_value>:
    1cb4:	004136        	entry	a1, 32
    1cb7:	f8f681        	l32r	a8, 90 (2254 <aac_getbits>)
    1cba:	02ad           	mov.n	a10, a2
    1cbc:	2b0c           	movi.n	a11, 2
    1cbe:	0008e0        	callx8	a8
    1cc1:	040c           	movi.n	a4, 0
    1cc3:	030c           	movi.n	a3, 0
    1cc5:	7450a0        	extui	a5, a10, 0, 8
    1cc8:	551b           	addi.n	a5, a5, 1

00001cca <loas_get_value+0x16>:
    1cca:	f8f181        	l32r	a8, 90 (2254 <aac_getbits>)
    1ccd:	02ad           	mov.n	a10, a2
    1ccf:	8b0c           	movi.n	a11, 8
    1cd1:	0008e0        	callx8	a8
    1cd4:	441b           	addi.n	a4, a4, 1
    1cd6:	113380        	slli	a3, a3, 8
    1cd9:	0a7a30        	depbits	a3, a10, 0, 8
    1cdc:	ea9547        	bne	a5, a4, 1cca <loas_get_value+0x16>
    1cdf:	032d           	mov.n	a2, a3
    1ce1:	f01d           	retw.n

00001ce3 <loas_get_value+0x2f>:
	...

00001ce4 <loas_mux_element>:
    1ce4:	00a136        	entry	a1, 80
    1ce7:	f8ea81        	l32r	a8, 90 (2254 <aac_getbits>)
    1cea:	03ad           	mov.n	a10, a3
    1cec:	1b0c           	movi.n	a11, 1
    1cee:	0008e0        	callx8	a8
    1cf1:	0442a2        	s8i	a10, a2, 4
    1cf4:	7490a0        	extui	a9, a10, 0, 8
    1cf7:	49ec           	bnez.n	a9, 1d1f <loas_mux_element+0x3b>
    1cf9:	f8e581        	l32r	a8, 90 (2254 <aac_getbits>)
    1cfc:	03ad           	mov.n	a10, a3
    1cfe:	1b0c           	movi.n	a11, 1
    1d00:	0008e0        	callx8	a8
    1d03:	0142a2        	s8i	a10, a2, 1
    1d06:	7490a0        	extui	a9, a10, 0, 8
    1d09:	041916        	beqz	a9, 1d4e <loas_mux_element+0x6a>
    1d0c:	f8e181        	l32r	a8, 90 (2254 <aac_getbits>)
    1d0f:	03ad           	mov.n	a10, a3
    1d11:	1b0c           	movi.n	a11, 1
    1d13:	0008e0        	callx8	a8
    1d16:	7490a0        	extui	a9, a10, 0, 8
    1d19:	024292        	s8i	a9, a2, 2
    1d1c:	000c46        	j	1d51 <loas_mux_element+0x6d>

00001d1f <loas_mux_element+0x3b>:
    1d1f:	000292        	l8ui	a9, a2, 0

00001d22 <loas_mux_element+0x3e>:
    1d22:	d9ac           	beqz.n	a9, 1d53 <loas_mux_element+0x6f>
    1d24:	030292        	l8ui	a9, a2, 3
    1d27:	79dc           	bnez.n	a9, 1d42 <loas_mux_element+0x5e>
    1d29:	020c           	movi.n	a2, 0
    1d2b:	ffa042        	movi	a4, 255

00001d2e <loas_mux_element+0x4a>:
    1d2e:	f8d881        	l32r	a8, 90 (2254 <aac_getbits>)
    1d31:	03ad           	mov.n	a10, a3
    1d33:	8b0c           	movi.n	a11, 8
    1d35:	0008e0        	callx8	a8
    1d38:	7490a0        	extui	a9, a10, 0, 8
    1d3b:	229a           	add.n	a2, a2, a9
    1d3d:	ed1947        	beq	a9, a4, 1d2e <loas_mux_element+0x4a>
    1d40:	f01d           	retw.n

00001d42 <loas_mux_element+0x5e>:
    1d42:	031926        	beqi	a9, 1, 1d49 <loas_mux_element+0x65>
    1d45:	020c           	movi.n	a2, 0
    1d47:	f01d           	retw.n

00001d49 <loas_mux_element+0x65>:
    1d49:	031222        	l16ui	a2, a2, 6
    1d4c:	f01d           	retw.n

00001d4e <loas_mux_element+0x6a>:
    1d4e:	020292        	l8ui	a9, a2, 2

00001d51 <loas_mux_element+0x6d>:
    1d51:	298c           	beqz.n	a9, 1d57 <loas_mux_element+0x73>

00001d53 <loas_mux_element+0x6f>:
    1d53:	020c           	movi.n	a2, 0
    1d55:	f01d           	retw.n

00001d57 <loas_mux_element+0x73>:
    1d57:	0102a2        	l8ui	a10, a2, 1
    1d5a:	6a8c           	beqz.n	a10, 1d64 <loas_mux_element+0x80>
    1d5c:	f8e681        	l32r	a8, f4 (1cb4 <loas_get_value>)
    1d5f:	03ad           	mov.n	a10, a3
    1d61:	0008e0        	callx8	a8

00001d64 <loas_mux_element+0x80>:
    1d64:	f8cb81        	l32r	a8, 90 (2254 <aac_getbits>)
    1d67:	03ad           	mov.n	a10, a3
    1d69:	1b0c           	movi.n	a11, 1
    1d6b:	0008e0        	callx8	a8
    1d6e:	0a5d           	mov.n	a5, a10
    1d70:	f8c881        	l32r	a8, 90 (2254 <aac_getbits>)
    1d73:	6b0c           	movi.n	a11, 6
    1d75:	03ad           	mov.n	a10, a3
    1d77:	0008e0        	callx8	a8
    1d7a:	0a6d           	mov.n	a6, a10
    1d7c:	f8c581        	l32r	a8, 90 (2254 <aac_getbits>)
    1d7f:	4b0c           	movi.n	a11, 4
    1d81:	03ad           	mov.n	a10, a3
    1d83:	0008e0        	callx8	a8
    1d86:	0a4d           	mov.n	a4, a10
    1d88:	f8c281        	l32r	a8, 90 (2254 <aac_getbits>)
    1d8b:	3b0c           	movi.n	a11, 3
    1d8d:	03ad           	mov.n	a10, a3
    1d8f:	0008e0        	callx8	a8
    1d92:	bd14e6        	bgei	a4, 1, 1d53 <loas_mux_element+0x6f>
    1d95:	fba516        	beqz	a5, 1d53 <loas_mux_element+0x6f>
    1d98:	b716e6        	bgei	a6, 1, 1d53 <loas_mux_element+0x6f>
    1d9b:	b41ae6        	bgei	a10, 1, 1d53 <loas_mux_element+0x6f>
    1d9e:	010292        	l8ui	a9, a2, 1
    1da1:	b98c           	beqz.n	a9, 1db0 <loas_mux_element+0xcc>
    1da3:	f8d481        	l32r	a8, f4 (1cb4 <loas_get_value>)
    1da6:	03ad           	mov.n	a10, a3
    1da8:	0008e0        	callx8	a8
    1dab:	0a5d           	mov.n	a5, a10
    1dad:	000046        	j	1db2 <loas_mux_element+0xce>

00001db0 <loas_mux_element+0xcc>:
    1db0:	050c           	movi.n	a5, 0

00001db2 <loas_mux_element+0xce>:
    1db2:	f8b181        	l32r	a8, 78 (22b8 <aac_get_processed_bits>)
    1db5:	03ad           	mov.n	a10, a3
    1db7:	0008e0        	callx8	a8
    1dba:	01bd           	mov.n	a11, a1
    1dbc:	0a4d           	mov.n	a4, a10
    1dbe:	18c1c2        	addi	a12, a1, 24
    1dc1:	03ad           	mov.n	a10, a3
    1dc3:	fec125        	call8	9d4 <aac_specific_config>
    1dc6:	f89a96        	bltz	a10, 1d53 <loas_mux_element+0x6f>
    1dc9:	f8ab81        	l32r	a8, 78 (22b8 <aac_get_processed_bits>)
    1dcc:	03ad           	mov.n	a10, a3
    1dce:	0008e0        	callx8	a8
    1dd1:	c06a40        	sub	a6, a10, a4
    1dd4:	00a292        	movi	a9, 0x200
    1dd7:	2e3967        	bltu	a9, a6, 1e09 <loas_mux_element+0x125>
    1dda:	f8a981        	l32r	a8, 80 (22d0 <aac_resetbits>)
    1ddd:	04bd           	mov.n	a11, a4
    1ddf:	03ad           	mov.n	a10, a3
    1de1:	0008e0        	callx8	a8
    1de4:	136262        	s32i	a6, a2, 76
    1de7:	064d           	mov.n	a4, a6
    1de9:	c69c           	beqz.n	a6, 1e09 <loas_mux_element+0x125>
    1deb:	027d           	mov.n	a7, a2

00001ded <loas_mux_element+0x109>:
    1ded:	03ad           	mov.n	a10, a3
    1def:	8b0c           	movi.n	a11, 8
    1df1:	f8a781        	l32r	a8, 90 (2254 <aac_getbits>)
    1df4:	43b4b0        	min	a11, a4, a11
    1df7:	b1b9           	s32i.n	a11, a1, 44
    1df9:	0008e0        	callx8	a8
    1dfc:	b1c8           	l32i.n	a12, a1, 44
    1dfe:	0c47a2        	s8i	a10, a7, 12
    1e01:	771b           	addi.n	a7, a7, 1
    1e03:	c044c0        	sub	a4, a4, a12
    1e06:	fe3456        	bnez	a4, 1ded <loas_mux_element+0x109>
    1e09:	0cb657        	bgeu	a6, a5, 1e19 <loas_mux_element+0x135>
    1e0c:	03ad           	mov.n	a10, a3
    1e0e:	f8a081        	l32r	a8, 90 (2254 <aac_getbits>)
    1e11:	c0b560        	sub	a11, a5, a6
    1e14:	8bbd           	ae_sext16	a11, a11
    1e16:	0008e0        	callx8	a8

00001e19 <loas_mux_element+0x135>:
    1e19:	f89d81        	l32r	a8, 90 (2254 <aac_getbits>)
    1e1c:	03ad           	mov.n	a10, a3
    1e1e:	3b0c           	movi.n	a11, 3
    1e20:	0008e0        	callx8	a8
    1e23:	0342a2        	s8i	a10, a2, 3
    1e26:	040c           	movi.n	a4, 0
    1e28:	7490a0        	extui	a9, a10, 0, 8
    1e2b:	e9cc           	bnez.n	a9, 1e3d <loas_mux_element+0x159>
    1e2d:	035242        	s16i	a4, a2, 6
    1e30:	f89881        	l32r	a8, 90 (2254 <aac_getbits>)
    1e33:	03ad           	mov.n	a10, a3
    1e35:	8b0c           	movi.n	a11, 8
    1e37:	0008e0        	callx8	a8
    1e3a:	000746        	j	1e5b <loas_mux_element+0x177>

00001e3d <loas_mux_element+0x159>:
    1e3d:	3a1966        	bnei	a9, 1, 1e7b <loas_mux_element+0x197>
    1e40:	f89481        	l32r	a8, 90 (2254 <aac_getbits>)
    1e43:	03ad           	mov.n	a10, a3
    1e45:	9b0c           	movi.n	a11, 9
    1e47:	0008e0        	callx8	a8
    1e4a:	0352a2        	s16i	a10, a2, 6
    1e4d:	9a9d           	ae_zext16	a9, a10
    1e4f:	f00916        	beqz	a9, 1d53 <loas_mux_element+0x6f>
    1e52:	a0a0a2        	movi	a10, 160
    1e55:	b0a9a0        	addx8	a10, a9, a10
    1e58:	0352a2        	s16i	a10, a2, 6

00001e5b <loas_mux_element+0x177>:
    1e5b:	2249           	s32i.n	a4, a2, 8
    1e5d:	f88c81        	l32r	a8, 90 (2254 <aac_getbits>)
    1e60:	03ad           	mov.n	a10, a3
    1e62:	1b0c           	movi.n	a11, 1
    1e64:	0008e0        	callx8	a8
    1e67:	6abc           	beqz.n	a10, 1ea1 <loas_mux_element+0x1bd>
    1e69:	010292        	l8ui	a9, a2, 1
    1e6c:	f98c           	beqz.n	a9, 1e7f <loas_mux_element+0x19b>
    1e6e:	f8a181        	l32r	a8, f4 (1cb4 <loas_get_value>)
    1e71:	03ad           	mov.n	a10, a3
    1e73:	0008e0        	callx8	a8
    1e76:	22a9           	s32i.n	a10, a2, 8
    1e78:	000946        	j	1ea1 <loas_mux_element+0x1bd>

00001e7b <loas_mux_element+0x197>:
    1e7b:	042d           	mov.n	a2, a4
    1e7d:	f01d           	retw.n

00001e7f <loas_mux_element+0x19b>:
    1e7f:	f88481        	l32r	a8, 90 (2254 <aac_getbits>)
    1e82:	03ad           	mov.n	a10, a3
    1e84:	1b0c           	movi.n	a11, 1
    1e86:	0008e0        	callx8	a8
    1e89:	0a4d           	mov.n	a4, a10
    1e8b:	f88181        	l32r	a8, 90 (2254 <aac_getbits>)
    1e8e:	8b0c           	movi.n	a11, 8
    1e90:	03ad           	mov.n	a10, a3
    1e92:	0008e0        	callx8	a8
    1e95:	2298           	l32i.n	a9, a2, 8
    1e97:	119980        	slli	a9, a9, 8
    1e9a:	9a9a           	add.n	a9, a10, a9
    1e9c:	2299           	s32i.n	a9, a2, 8
    1e9e:	fdd456        	bnez	a4, 1e7f <loas_mux_element+0x19b>
    1ea1:	f87b81        	l32r	a8, 90 (2254 <aac_getbits>)
    1ea4:	03ad           	mov.n	a10, a3
    1ea6:	1b0c           	movi.n	a11, 1
    1ea8:	0008e0        	callx8	a8
    1eab:	8a8c           	beqz.n	a10, 1eb7 <loas_mux_element+0x1d3>
    1ead:	f87881        	l32r	a8, 90 (2254 <aac_getbits>)
    1eb0:	03ad           	mov.n	a10, a3
    1eb2:	8b0c           	movi.n	a11, 8
    1eb4:	0008e0        	callx8	a8

00001eb7 <loas_mux_element+0x1d3>:
    1eb7:	190c           	movi.n	a9, 1
    1eb9:	004292        	s8i	a9, a2, 0
    1ebc:	190c           	movi.n	a9, 1
    1ebe:	ff9806        	j	1d22 <loas_mux_element+0x3e>

00001ec1 <loas_mux_element+0x1dd>:
    1ec1:	000000                                        ...

00001ec4 <aac_loas_frame>:
    1ec4:	008136        	entry	a1, 64
    1ec7:	5129           	s32i.n	a2, a1, 20
    1ec9:	03ad           	mov.n	a10, a3
    1ecb:	f86b81        	l32r	a8, 78 (22b8 <aac_get_processed_bits>)
    1ece:	f97c           	movi.n	a9, -1
    1ed0:	649d           	ae_s16i.n	a9, a4, 0
    1ed2:	0008e0        	callx8	a8
    1ed5:	11b5d0        	slli	a11, a5, 3
    1ed8:	61b9           	s32i.n	a11, a1, 24

00001eda <aac_loas_frame+0x16>:
    1eda:	f86881        	l32r	a8, 7c (22f0 <aac_used_bytes>)
    1edd:	03ad           	mov.n	a10, a3
    1edf:	0008e0        	callx8	a8
    1ee2:	2398           	l32i.n	a9, a3, 8
    1ee4:	5daa97        	bge	a10, a9, 1f45 <aac_loas_frame+0x81>
    1ee7:	f86781        	l32r	a8, 84 (2298 <aac_byte_align>)
    1eea:	03ad           	mov.n	a10, a3
    1eec:	0008e0        	callx8	a8
    1eef:	f86281        	l32r	a8, 78 (22b8 <aac_get_processed_bits>)
    1ef2:	03ad           	mov.n	a10, a3
    1ef4:	0008e0        	callx8	a8
    1ef7:	0a6d           	mov.n	a6, a10
    1ef9:	03bd           	mov.n	a11, a3
    1efb:	51a8           	l32i.n	a10, a1, 20
    1efd:	ffde65        	call8	1ce4 <loas_mux_element>
    1f00:	f85e81        	l32r	a8, 78 (22b8 <aac_get_processed_bits>)
    1f03:	0a2d           	mov.n	a2, a10
    1f05:	03ad           	mov.n	a10, a3
    1f07:	0008e0        	callx8	a8
    1f0a:	0072c4        	rur.ae_bitptr	a7
    1f0d:	f85c81        	l32r	a8, 80 (22d0 <aac_resetbits>)
    1f10:	61c8           	l32i.n	a12, a1, 24
    1f12:	31a9           	s32i.n	a10, a1, 12
    1f14:	86bd           	ae_sext16	a11, a6
    1f16:	23d8           	l32i.n	a13, a3, 8
    1f18:	13e8           	l32i.n	a14, a3, 4
    1f1a:	03f8           	l32i.n	a15, a3, 0
    1f1c:	01f9           	s32i.n	a15, a1, 0
    1f1e:	11e9           	s32i.n	a14, a1, 4
    1f20:	21d9           	s32i.n	a13, a1, 8
    1f22:	e36f10        	rur.ae_bithead	a6
    1f25:	41b9           	s32i.n	a11, a1, 16
    1f27:	01ad           	mov.n	a10, a1
    1f29:	bbca           	add.n	a11, a11, a12
    1f2b:	0008e0        	callx8	a8
    1f2e:	032674        	wur.ae_bitptr	a7
    1f31:	f3f160        	wur.ae_bithead	a6
    1f34:	12dc           	bnez.n	a2, 1f49 <aac_loas_frame+0x85>
    1f36:	2188           	l32i.n	a8, a1, 8
    1f38:	1198           	l32i.n	a9, a1, 4
    1f3a:	01a8           	l32i.n	a10, a1, 0
    1f3c:	03a9           	s32i.n	a10, a3, 0
    1f3e:	1399           	s32i.n	a9, a3, 4
    1f40:	2389           	s32i.n	a8, a3, 8
    1f42:	ffe506        	j	1eda <aac_loas_frame+0x16>

00001f45 <aac_loas_frame+0x81>:
    1f45:	f27c           	movi.n	a2, -1
    1f47:	f01d           	retw.n

00001f49 <aac_loas_frame+0x85>:
    1f49:	4128           	l32i.n	a2, a1, 16
    1f4b:	3188           	l32i.n	a8, a1, 12
    1f4d:	219320        	srai	a9, a2, 3
    1f50:	c02280        	sub	a2, a2, a8
    1f53:	959a           	add.n	a9, a5, a9
    1f55:	649d           	ae_s16i.n	a9, a4, 0
    1f57:	b02520        	addx8	a2, a5, a2
    1f5a:	f01d           	retw.n

00001f5c <aac_loas_frame+0x98>:
    1f5c:	00000000                                ....

00001f60 <tns_decode_coef_mod>:
    1f60:	02a136        	entry	a1, 0x150
    1f63:	fdc392        	addi	a9, a3, -3
    1f66:	080c           	movi.n	a8, 0
    1f68:	130c           	movi.n	a3, 1
    1f6a:	833890        	moveqz	a3, a8, a9
    1f6d:	732b           	addi.n	a7, a3, 2
    1f6f:	933740        	movnez	a3, a7, a4
    1f72:	1b12a6        	blti	a2, 1, 1f91 <tns_decode_coef_mod+0x31>
    1f75:	f86041        	l32r	a4, f8 (94e0 <tns_coef>)
    1f78:	a04340        	addx4	a4, a3, a4
    1f7b:	002442        	l32i	a4, a4, 0
    1f7e:	203110        	or	a3, a1, a1
    1f81:	0ca276        	loopgtz	a2, 1f91 <tns_decode_coef_mod+0x31>
    1f84:	000572        	l8ui	a7, a5, 0
    1f87:	551b           	addi.n	a5, a5, 1
    1f89:	907740        	addx2	a7, a7, a4
    1f8c:	277d           	ae_l16si.n	a7, a7, 0
    1f8e:	09e374        	ae_s32ip	a7, a3, 4

00001f91 <tns_decode_coef_mod+0x31>:
    1f91:	0138           	l32i.n	a3, a1, 0
    1f93:	f85a71        	l32r	a7, fc (1000000 <_end+0xff6a80>)
    1f96:	0679           	s32i.n	a7, a6, 0
    1f98:	113370        	slli	a3, a3, 9
    1f9b:	1639           	s32i.n	a3, a6, 4
    1f9d:	1012a6        	blti	a2, 1, 1fb1 <tns_decode_coef_mod+0x51>
    1fa0:	063d           	mov.n	a3, a6
    1fa2:	60c142        	addi	a4, a1, 96
    1fa5:	06a276        	loopgtz	a2, 1faf <tns_decode_coef_mod+0x4f>
    1fa8:	1388           	l32i.n	a8, a3, 4
    1faa:	09e484        	ae_s32ip	a8, a4, 4
    1fad:	334b           	addi.n	a3, a3, 4

00001faf <tns_decode_coef_mod+0x4f>:
    1faf:	1638           	l32i.n	a3, a6, 4

00001fb1 <tns_decode_coef_mod+0x51>:
    1fb1:	1198           	l32i.n	a9, a1, 4
    1fb3:	404010        	ssai	16
    1fb6:	11c3f0        	slli	a12, a3, 1
    1fb9:	11b970        	slli	a11, a9, 9
    1fbc:	899d           	ae_sext16	a9, a9
    1fbe:	82a9c0        	mull	a10, a9, a12
    1fc1:	b299c0        	mulsh	a9, a9, a12
    1fc4:	2d61b2        	s32i	a11, a1, 180
    1fc7:	8199a0        	src	a9, a9, a10
    1fca:	939a           	add.n	a9, a3, a9
    1fcc:	2c6192        	s32i	a9, a1, 176
    1fcf:	0232e6        	bgei	a2, 3, 1fd5 <tns_decode_coef_mod+0x75>
    1fd2:	003f86        	j	20d4 <tns_decode_coef_mod+0x174>
    1fd5:	496122        	s32i	a2, a1, 0x124
    1fd8:	4b0c           	movi.n	a11, 4
    1fda:	d20b           	addi.n	a13, a2, -1
    1fdc:	514b           	addi.n	a5, a1, 4
    1fde:	418b           	addi.n	a4, a1, 8
    1fe0:	130c           	movi.n	a3, 1
    1fe2:	486162        	s32i	a6, a1, 0x120
    1fe5:	5cc192        	addi	a9, a1, 92
    1fe8:	58c1a2        	addi	a10, a1, 88
    1feb:	7fc1e2        	addi	a14, a1, 127
    1fee:	39ce82        	addi	a8, a14, 57
    1ff1:	4561a2        	s32i	a10, a1, 0x114
    1ff4:	29cef2        	addi	a15, a14, 41
    1ff7:	466192        	s32i	a9, a1, 0x118
    1ffa:	2dce72        	addi	a7, a14, 45
    1ffd:	68c192        	addi	a9, a1, 104
    2000:	446172        	s32i	a7, a1, 0x110
    2003:	4761f2        	s32i	a15, a1, 0x11c
    2006:	f83ea1        	l32r	a10, 100 (400000 <_end+0x3f6a80>)
    2009:	360a34        	ae_cvtq48a32s	aeq0, a10
    200c:	31cee2        	addi	a14, a14, 49

0000200f <tns_decode_coef_mod+0xaf>:
    200f:	15a8           	l32i.n	a10, a5, 4
    2011:	554b           	addi.n	a5, a5, 4
    2013:	386307        	bbci	a3, 0, 204f <tns_decode_coef_mod+0xef>
    2016:	462162        	l32i	a6, a1, 0x118
    2019:	472122        	l32i	a2, a1, 0x11c
    201c:	4c2404        	ae_lp24.i	aep2, a4, 0
    201f:	2f0c           	movi.n	a15, 2
    2021:	c31b           	addi.n	a12, a3, 1
    2023:	d2ccf0        	quos	a12, a12, a15
    2026:	20f880        	or	a15, a8, a8
    2029:	1cac76        	loopgtz	a12, 2049 <tns_decode_coef_mod+0xe9>
    202c:	5c9214        	ae_lp24x2f.iu	aep1, a2, 8
    202f:	089424        	ae_mulfq32sp24s.h	aeq2, aeq0, aep1
    2032:	f8846f8fff 	{ ae_lp24x2f.iu	aep0, a15, -8; ae_mulfq32sp24s.l	aeq1, aeq0, aep1 }
    2037:	020a04        	ae_mulap24s.hh	aeq1, aep0, aep2
    203a:	028234        	ae_mulap24s.ll	aeq2, aep0, aep2
    203d:	355254        	ae_slliq56	aeq1, aeq1, 9
    2040:	35a254        	ae_slliq56	aeq2, aeq2, 9
    2043:	626614        	ae_sq32f.iu	aeq2, a6, 4
    2046:	625614        	ae_sq32f.iu	aeq1, a6, 4

00002049 <tns_decode_coef_mod+0xe9>:
    2049:	60c1c2        	addi	a12, a1, 96
    204c:	001a86        	j	20ba <tns_decode_coef_mod+0x15a>

0000204f <tns_decode_coef_mod+0xef>:
    204f:	442162        	l32i	a6, a1, 0x110
    2052:	452122        	l32i	a2, a1, 0x114
    2055:	09fd           	mov.n	a15, a9
    2057:	4c2404        	ae_lp24.i	aep2, a4, 0
    205a:	2c0c           	movi.n	a12, 2
    205c:	d2c3c0        	quos	a12, a3, a12
    205f:	241ca6        	blti	a12, 1, 2087 <tns_decode_coef_mod+0x127>
    2062:	1eac76        	loopgtz	a12, 2084 <tns_decode_coef_mod+0x124>
    2065:	5cc214        	ae_lp24x2f.iu	aep4, a2, 8
    2068:	f8106edfff 	{ ae_lp24f.iu	aep5, a15, -4; ae_mulfq32sp24s.h	aeq1, aeq0, aep4 }
    206d:	025a34        	ae_mulap24s.ll	aeq1, aep5, aep2
    2070:	355254        	ae_slliq56	aeq1, aeq1, 9
    2073:	625614        	ae_sq32f.iu	aeq1, a6, 4
    2076:	f9906ebfff 	{ ae_lp24f.iu	aep3, a15, -4; ae_mulfq32sp24s.l	aeq3, aeq0, aep4 }
    207b:	02ba34        	ae_mulap24s.ll	aeq3, aep3, aep2
    207e:	35f254        	ae_slliq56	aeq3, aeq3, 9
    2081:	627614        	ae_sq32f.iu	aeq3, a6, 4

00002084 <tns_decode_coef_mod+0x124>:
    2084:	000046        	j	2089 <tns_decode_coef_mod+0x129>

00002087 <tns_decode_coef_mod+0x127>:
    2087:	0c0c           	movi.n	a12, 0

00002089 <tns_decode_coef_mod+0x129>:
    2089:	8a2d           	ae_sext16	a2, a10
    208b:	60c1f2        	addi	a15, a1, 96
    208e:	117cf0        	slli	a7, a12, 1
    2091:	c07370        	sub	a7, a3, a7
    2094:	a077f0        	addx4	a7, a7, a15
    2097:	0778           	l32i.n	a7, a7, 0
    2099:	b0fcf0        	addx8	a15, a12, a15
    209c:	0ff8           	l32i.n	a15, a15, 0
    209e:	1177f0        	slli	a7, a7, 1
    20a1:	826270        	mull	a6, a2, a7
    20a4:	b22270        	mulsh	a2, a2, a7
    20a7:	404010        	ssai	16
    20aa:	812260        	src	a2, a2, a6
    20ad:	ff2a           	add.n	a15, a15, a2
    20af:	b02ce0        	addx8	a2, a12, a14
    20b2:	02f9           	s32i.n	a15, a2, 0
    20b4:	7fc1c2        	addi	a12, a1, 127
    20b7:	31ccc2        	addi	a12, a12, 49

000020ba <tns_decode_coef_mod+0x15a>:
    20ba:	884b           	addi.n	a8, a8, 4
    20bc:	444b           	addi.n	a4, a4, 4
    20be:	994b           	addi.n	a9, a9, 4
    20c0:	331b           	addi.n	a3, a3, 1
    20c2:	fbca           	add.n	a15, a11, a12
    20c4:	117a70        	slli	a7, a10, 9
    20c7:	bb4b           	addi.n	a11, a11, 4
    20c9:	1f79           	s32i.n	a7, a15, 4
    20cb:	c0fd30        	sub	a15, a13, a3
    20ce:	f3df56        	bnez	a15, 200f <tns_decode_coef_mod+0xaf>
    20d1:	0002c6        	j	20e0 <tns_decode_coef_mod+0x180>

000020d4 <tns_decode_coef_mod+0x174>:
    20d4:	486162        	s32i	a6, a1, 0x120
    20d7:	496122        	s32i	a2, a1, 0x124
    20da:	7fc1c2        	addi	a12, a1, 127
    20dd:	31ccc2        	addi	a12, a12, 49

000020e0 <tns_decode_coef_mod+0x180>:
    20e0:	482162        	l32i	a6, a1, 0x120
    20e3:	492122        	l32i	a2, a1, 0x124
    20e6:	60c142        	addi	a4, a1, 96
    20e9:	364b           	addi.n	a3, a6, 4
    20eb:	06a276        	loopgtz	a2, 20f5 <tns_decode_coef_mod+0x195>
    20ee:	04d8           	l32i.n	a13, a4, 0
    20f0:	09e3d4        	ae_s32ip	a13, a3, 4
    20f3:	444b           	addi.n	a4, a4, 4

000020f5 <tns_decode_coef_mod+0x195>:
    20f5:	0be207        	bbsi	a2, 0, 2104 <tns_decode_coef_mod+0x1a4>
    20f8:	364b           	addi.n	a3, a6, 4
    20fa:	06a276        	loopgtz	a2, 2104 <tns_decode_coef_mod+0x1a4>
    20fd:	0ce8           	l32i.n	a14, a12, 0
    20ff:	09e3e4        	ae_s32ip	a14, a3, 4
    2102:	cc4b           	addi.n	a12, a12, 4
    2104:	f01d           	retw.n

00002106 <tns_decode_coef_mod+0x1a6>:
	...

00002108 <tns_filter_m0>:
    2108:	022136        	entry	a1, 0x110
    210b:	1116a6        	blti	a6, 1, 2120 <tns_filter_m0+0x18>
    210e:	1196e0        	slli	a9, a6, 2
    2111:	018d           	mov.n	a8, a1
    2113:	06a676        	loopgtz	a6, 211d <tns_filter_m0+0x15>
    2116:	1578           	l32i.n	a7, a5, 4
    2118:	09e874        	ae_s32ip	a7, a8, 4
    211b:	554b           	addi.n	a5, a5, 4

0000211d <tns_filter_m0+0x15>:
    211d:	000086        	j	2123 <tns_filter_m0+0x1b>

00002120 <tns_filter_m0+0x18>:
    2120:	1196e0        	slli	a9, a6, 2

00002123 <tns_filter_m0+0x1b>:
    2123:	080c           	movi.n	a8, 0
    2125:	50c152        	addi	a5, a1, 80
    2128:	995a           	add.n	a9, a9, a5
    212a:	02a676        	loopgtz	a6, 2130 <tns_filter_m0+0x28>
    212d:	09e584        	ae_s32ip	a8, a5, 4

00002130 <tns_filter_m0+0x28>:
    2130:	f3f790        	wur.ae_cend0	a9
    2133:	11c4e0        	slli	a12, a4, 2
    2136:	c092c0        	sub	a9, a2, a12
    2139:	50c122        	addi	a2, a1, 80
    213c:	f3f620        	wur.ae_cbegin0	a2
    213f:	3e13a6        	blti	a3, 1, 2181 <tns_filter_m0+0x79>
    2142:	0a0c           	movi.n	a10, 0
    2144:	cd7c           	movi.n	a13, -4
    2146:	450c           	movi.n	a5, 4
    2148:	f8c1b2        	addi	a11, a1, -8
    214b:	280c           	movi.n	a8, 2
    214d:	d26680        	quos	a6, a6, a8
    2150:	204bb0        	or	a4, a11, a11

00002153 <tns_filter_m0+0x4b>:
    2153:	6039c4        	ae_lq32f.xu	aeq0, a9, a12
    2156:	3501f4        	ae_sraiq56	aeq0, aeq0, 7
    2159:	0fa676        	loopgtz	a6, 216c <tns_filter_m0+0x64>
    215c:	5c8414        	ae_lp24x2f.iu	aep0, a4, 8
    215f:	fc24fbc25f 	{ ae_lq32f.c	aeq2, a2, a5; nop }
    2164:	f642bba25f 	{ ae_lq32f.c	aeq1, a2, a5; ae_mulafq32sp24s.h	aeq0, aeq2, aep0 }
    2169:	080364        	ae_mulafq32sp24s.l	aeq0, aeq1, aep0

0000216c <tns_filter_m0+0x64>:
    216c:	0b4d           	mov.n	a4, a11
    216e:	aa1b           	addi.n	a10, a10, 1
    2170:	36c1d4        	ae_sllisq56s	aeq3, aeq0, 7
    2173:	623904        	ae_sq32f.i	aeq3, a9, 0
    2176:	ff94bba2df 	{ ae_lq32f.c	aeq1, a2, a13; ae_negq56	aeq3, aeq3 }
    217b:	623204        	ae_sq32f.i	aeq3, a2, 0
    217e:	d193a7        	bne	a3, a10, 2153 <tns_filter_m0+0x4b>

00002181 <tns_filter_m0+0x79>:
    2181:	f01d           	retw.n

00002183 <tns_filter_m0+0x7b>:
	...

00002184 <tns_filter_m1>:
    2184:	022136        	entry	a1, 0x110
    2187:	1116a6        	blti	a6, 1, 219c <tns_filter_m1+0x18>
    218a:	1196e0        	slli	a9, a6, 2
    218d:	018d           	mov.n	a8, a1
    218f:	06a676        	loopgtz	a6, 2199 <tns_filter_m1+0x15>
    2192:	1578           	l32i.n	a7, a5, 4
    2194:	09e874        	ae_s32ip	a7, a8, 4
    2197:	554b           	addi.n	a5, a5, 4

00002199 <tns_filter_m1+0x15>:
    2199:	000086        	j	219f <tns_filter_m1+0x1b>

0000219c <tns_filter_m1+0x18>:
    219c:	1196e0        	slli	a9, a6, 2

0000219f <tns_filter_m1+0x1b>:
    219f:	0b0c           	movi.n	a11, 0
    21a1:	50c152        	addi	a5, a1, 80
    21a4:	895a           	add.n	a8, a9, a5
    21a6:	02a676        	loopgtz	a6, 21ac <tns_filter_m1+0x28>
    21a9:	09e5b4        	ae_s32ip	a11, a5, 4

000021ac <tns_filter_m1+0x28>:
    21ac:	f3f780        	wur.ae_cend0	a8
    21af:	11d4e0        	slli	a13, a4, 2
    21b2:	c092d0        	sub	a9, a2, a13
    21b5:	50c142        	addi	a4, a1, 80
    21b8:	f3f640        	wur.ae_cbegin0	a4
    21bb:	4513a6        	blti	a3, 1, 2204 <tns_filter_m1+0x80>
    21be:	0a0c           	movi.n	a10, 0
    21c0:	450c           	movi.n	a5, 4
    21c2:	f8c1c2        	addi	a12, a1, -8
    21c5:	280c           	movi.n	a8, 2
    21c7:	d26680        	quos	a6, a6, a8
    21ca:	0c2d           	mov.n	a2, a12

000021cc <tns_filter_m1+0x48>:
    21cc:	6039d4        	ae_lq32f.xu	aeq0, a9, a13
    21cf:	3501f4        	ae_sraiq56	aeq0, aeq0, 7
    21d2:	0fa676        	loopgtz	a6, 21e5 <tns_filter_m1+0x61>
    21d5:	5c8214        	ae_lp24x2f.iu	aep0, a2, 8
    21d8:	fc24fbc45f 	{ ae_lq32f.c	aeq2, a4, a5; nop }
    21dd:	f642bba45f 	{ ae_lq32f.c	aeq1, a4, a5; ae_mulafq32sp24s.h	aeq0, aeq2, aep0 }
    21e2:	080364        	ae_mulafq32sp24s.l	aeq0, aeq1, aep0

000021e5 <tns_filter_m1+0x61>:
    21e5:	5c9214        	ae_lp24x2f.iu	aep1, a2, 8
    21e8:	fc24fba4bf 	{ ae_lq32f.c	aeq1, a4, a11; nop }
    21ed:	f447801aae 	{ addi	a10, a10, 1; ae_mulafq32sp24s.h	aeq0, aeq1, aep1 }
    21f2:	0c2d           	mov.n	a2, a12
    21f4:	36c1d4        	ae_sllisq56s	aeq3, aeq0, 7

000021f7 <tns_filter_m1+0x73>:
    21f7:	ff94b9b90f 	{ ae_sq32f.i	aeq3, a9, 0; ae_negq56	aeq3, aeq3 }
    21fc:	fc24f834bf 	{ ae_sq32f.c	aeq3, a4, a11; nop }
    2201:	c793a7        	bne	a3, a10, 21cc <tns_filter_m1+0x48>

00002204 <tns_filter_m1+0x80>:
    2204:	f01d           	retw.n

00002206 <tns_filter_m1+0x82>:
	...

00002208 <fill_DB>:
    2208:	004136        	entry	a1, 32
    220b:	d37c           	movi.n	a3, -3
    220d:	045020        	extui	a5, a2, 0, 1
    2210:	f47c           	movi.n	a4, -1
    2212:	304240        	xor	a4, a2, a4
    2215:	1155d0        	slli	a5, a5, 3
    2218:	032654        	wur.ae_bitptr	a5
    221b:	044040        	extui	a4, a4, 0, 1
    221e:	643b           	addi.n	a6, a4, 3
    2220:	12a347        	bge	a3, a4, 2236 <fill_DB+0x2e>
    2223:	040c           	movi.n	a4, 0
    2225:	0aa676        	loopgtz	a6, 2233 <fill_DB+0x2b>
    2228:	000282        	l8ui	a8, a2, 0
    222b:	114480        	slli	a4, a4, 8
    222e:	221b           	addi.n	a2, a2, 1
    2230:	0a7840        	depbits	a4, a8, 0, 8

00002233 <fill_DB+0x2b>:
    2233:	000046        	j	2238 <fill_DB+0x30>

00002236 <fill_DB+0x2e>:
    2236:	040c           	movi.n	a4, 0

00002238 <fill_DB+0x30>:
    2238:	f3f140        	wur.ae_bithead	a4
    223b:	fec222        	addi	a2, a2, -2
    223e:	f01d           	retw.n

00002240 <aac_initbits>:
    2240:	004136        	entry	a1, 32
    2243:	03ad           	mov.n	a10, a3
    2245:	f7af81        	l32r	a8, 104 (2208 <fill_DB>)
    2248:	0239           	s32i.n	a3, a2, 0
    224a:	2249           	s32i.n	a4, a2, 8
    224c:	0008e0        	callx8	a8
    224f:	12a9           	s32i.n	a10, a2, 4
    2251:	f01d           	retw.n

00002253 <aac_initbits+0x13>:
	...

00002254 <aac_getbits>:
    2254:	004136        	entry	a1, 32
    2257:	1268           	l32i.n	a6, a2, 4
    2259:	041c           	movi.n	a4, 16
    225b:	0b3437        	bltu	a4, a3, 226a <aac_getbits+0x16>
    225e:	035734        	ae_lb	a5, a3
    2261:	1c1634        	ae_db	a6, a3

00002264 <aac_getbits+0x10>:
    2264:	1269           	s32i.n	a6, a2, 4
    2266:	052d           	mov.n	a2, a5
    2268:	f01d           	retw.n

0000226a <aac_getbits+0x16>:
    226a:	f0c392        	addi	a9, a3, -16
    226d:	038794        	ae_lb	a8, a9
    2270:	1c1694        	ae_db	a6, a9
    2273:	035f04        	ae_lbi	a5, 16
    2276:	1269           	s32i.n	a6, a2, 4
    2278:	118800        	slli	a8, a8, 16
    227b:	0006f4        	ae_dbi	a6, 16
    227e:	558a           	add.n	a5, a5, a8
    2280:	fff806        	j	2264 <aac_getbits+0x10>

00002283 <aac_getbits+0x2f>:
	...

00002284 <aac_get1bit>:
    2284:	004136        	entry	a1, 32
    2287:	1238           	l32i.n	a3, a2, 4
    2289:	024d           	mov.n	a4, a2
    228b:	032f14        	ae_lbi	a2, 1
    228e:	0013f4        	ae_dbi	a3, 1
    2291:	1439           	s32i.n	a3, a4, 4
    2293:	f01d           	retw.n

00002295 <aac_get1bit+0x11>:
    2295:	000000                                        ...

00002298 <aac_byte_align>:
    2298:	004136        	entry	a1, 32
    229b:	0052c4        	rur.ae_bitptr	a5
    229e:	860c           	movi.n	a6, 8
    22a0:	f58c           	beqz.n	a5, 22b3 <aac_byte_align+0x1b>
    22a2:	1238           	l32i.n	a3, a2, 4
    22a4:	01a657        	bge	a6, a5, 22a9 <aac_byte_align+0x11>
    22a7:	061c           	movi.n	a6, 16

000022a9 <aac_byte_align+0x11>:
    22a9:	c04650        	sub	a4, a6, a5
    22ac:	1c1344        	ae_db	a3, a4
    22af:	1239           	s32i.n	a3, a2, 4
    22b1:	f01d           	retw.n

000022b3 <aac_byte_align+0x1b>:
    22b3:	f01d           	retw.n

000022b5 <aac_byte_align+0x1d>:
    22b5:	000000                                        ...

000022b8 <aac_get_processed_bits>:
    22b8:	004136        	entry	a1, 32
    22bb:	0032c4        	rur.ae_bitptr	a3
    22be:	0248           	l32i.n	a4, a2, 0
    22c0:	1228           	l32i.n	a2, a2, 4
    22c2:	c02240        	sub	a2, a2, a4
    22c5:	b02230        	addx8	a2, a2, a3
    22c8:	f0c222        	addi	a2, a2, -16
    22cb:	f01d           	retw.n

000022cd <aac_get_processed_bits+0x15>:
    22cd:	000000                                        ...

000022d0 <aac_resetbits>:
    22d0:	004136        	entry	a1, 32
    22d3:	02a8           	l32i.n	a10, a2, 0
    22d5:	f78b81        	l32r	a8, 104 (2208 <fill_DB>)
    22d8:	21b330        	srai	a11, a3, 3
    22db:	aaba           	add.n	a10, a10, a11
    22dd:	0008e0        	callx8	a8
    22e0:	12a9           	s32i.n	a10, a2, 4
    22e2:	f76b81        	l32r	a8, 90 (2254 <aac_getbits>)
    22e5:	24b030        	extui	a11, a3, 0, 3
    22e8:	02ad           	mov.n	a10, a2
    22ea:	0008e0        	callx8	a8
    22ed:	f01d           	retw.n

000022ef <aac_resetbits+0x1f>:
	...

000022f0 <aac_used_bytes>:
    22f0:	004136        	entry	a1, 32
    22f3:	f76181        	l32r	a8, 78 (22b8 <aac_get_processed_bits>)
    22f6:	02ad           	mov.n	a10, a2
    22f8:	0008e0        	callx8	a8
    22fb:	2123a0        	srai	a2, a10, 3
    22fe:	f01d           	retw.n

00002300 <dehuffman_sf>:
    2300:	004136        	entry	a1, 32
    2303:	f78151        	l32r	a5, 108 (53c0 <vld_huffdatasf>)
    2306:	630c           	movi.n	a3, 6
    2308:	038634        	ae_vldsht	a3

0000230b <dehuffman_sf+0xb>:
    230b:	1288           	l32i.n	a8, a2, 4
    230d:	130544        	ae_vldl16t	b0, a4, a5
    2310:	180814        	ae_vldl16c	a8
    2313:	1289           	s32i.n	a8, a2, 4
    2315:	f20076        	bf	b0, 230b <dehuffman_sf+0xb>
    2318:	c4c422        	addi	a2, a4, -60
    231b:	f01d           	retw.n

0000231d <dehuffman_sf+0x1d>:
    231d:	000000                                        ...

00002320 <dehuffman_cb1_cb2>:
    2320:	006136        	entry	a1, 48
    2323:	071666        	bnei	a6, 1, 232e <dehuffman_cb1_cb2+0xe>
    2326:	560c           	movi.n	a6, 5
    2328:	f77991        	l32r	a9, 10c (5920 <vld_huffdata1>)
    232b:	000106        	j	2333 <dehuffman_cb1_cb2+0x13>

0000232e <dehuffman_cb1_cb2+0xe>:
    232e:	460c           	movi.n	a6, 4
    2330:	f77891        	l32r	a9, 110 (5a40 <vld_huffdata2>)

00002333 <dehuffman_cb1_cb2+0x13>:
    2333:	038664        	ae_vldsht	a6
    2336:	c1a8           	l32i.n	a10, a1, 48
    2338:	f777e1        	l32r	a14, 114 (4a60 <acc_pow_quarter>)
    233b:	01cd           	mov.n	a12, a1
    233d:	0b0c           	movi.n	a11, 0
    233f:	11b9           	s32i.n	a11, a1, 4
    2341:	001ac6        	j	23b0 <dehuffman_cb1_cb2+0x90>

00002344 <dehuffman_cb1_cb2+0x24>:
    2344:	43fd           	ae_l16ui.n	a15, a3, 0
    2346:	228d           	ae_l16si.n	a8, a2, 0
    2348:	53bd           	ae_l16ui.n	a11, a3, 2
    234a:	222b           	addi.n	a2, a2, 2
    234c:	216280        	srai	a6, a8, 2
    234f:	148080        	extui	a8, a8, 0, 2
    2352:	c06650        	sub	a6, a6, a5
    2355:	edc662        	addi	a6, a6, -19
    2358:	a088e0        	addx4	a8, a8, a14
    235b:	604804        	ae_lq32f.i	aeq1, a8, 0
    235e:	fc24fe960f 	{ ae_slaasq56s	aeq0, aeq1, a6; nop }
    2363:	620124        	ae_sq32f.i	aeq0, a1, 8
    2366:	21d8           	l32i.n	a13, a1, 8
    2368:	332b           	addi.n	a3, a3, 2
    236a:	c0bbf0        	sub	a11, a11, a15
    236d:	60d0d0        	neg	a13, a13
    2370:	01d9           	s32i.n	a13, a1, 0

00002372 <dehuffman_cb1_cb2+0x52>:
    2372:	1ad8           	l32i.n	a13, a10, 4
    2374:	130964        	ae_vldl16t	b0, a6, a9
    2377:	180d14        	ae_vldl16c	a13
    237a:	1ad9           	s32i.n	a13, a10, 4
    237c:	f20076        	bf	b0, 2372 <dehuffman_cb1_cb2+0x52>
    237f:	fccbb2        	addi	a11, a11, -4
    2382:	148460        	extui	a8, a6, 4, 2
    2385:	14f060        	extui	a15, a6, 0, 2
    2388:	14d260        	extui	a13, a6, 2, 2
    238b:	a0ddc0        	addx4	a13, a13, a12
    238e:	a0ffc0        	addx4	a15, a15, a12
    2391:	a088c0        	addx4	a8, a8, a12
    2394:	0888           	l32i.n	a8, a8, 0
    2396:	0ff8           	l32i.n	a15, a15, 0
    2398:	0dd8           	l32i.n	a13, a13, 0
    239a:	14d9           	s32i.n	a13, a4, 4
    239c:	04f9           	s32i.n	a15, a4, 0
    239e:	2489           	s32i.n	a8, a4, 8
    23a0:	14f660        	extui	a15, a6, 6, 2
    23a3:	a0ffc0        	addx4	a15, a15, a12
    23a6:	0ff8           	l32i.n	a15, a15, 0
    23a8:	34f9           	s32i.n	a15, a4, 12
    23aa:	10c442        	addi	a4, a4, 16
    23ad:	c11be6        	bgei	a11, 1, 2372 <dehuffman_cb1_cb2+0x52>

000023b0 <dehuffman_cb1_cb2+0x90>:
    23b0:	770b           	addi.n	a7, a7, -1
    23b2:	f8e7d6        	bgez	a7, 2344 <dehuffman_cb1_cb2+0x24>
    23b5:	f01d           	retw.n

000023b7 <dehuffman_cb1_cb2+0x97>:
	...

000023b8 <dehuffman_cb3_cb4>:
    23b8:	008136        	entry	a1, 64
    23bb:	1021a2        	l32i	a10, a1, 64
    23be:	fdc6d2        	addi	a13, a6, -3
    23c1:	f755b1        	l32r	a11, 118 (4c30 <vld_huffdata4>)
    23c4:	f756c1        	l32r	a12, 11c (4ab0 <vld_huffdata3>)
    23c7:	4159           	s32i.n	a5, a1, 16
    23c9:	0e0c           	movi.n	a14, 0
    23cb:	01e9           	s32i.n	a14, a1, 0
    23cd:	550c           	movi.n	a5, 5
    23cf:	93cbd0        	movnez	a12, a11, a13
    23d2:	01dd           	mov.n	a13, a1
    23d4:	0031c6        	j	249f <dehuffman_cb3_cb4+0xe7>

000023d7 <dehuffman_cb3_cb4+0x1f>:
    23d7:	4198           	l32i.n	a9, a1, 16
    23d9:	53ed           	ae_l16ui.n	a14, a3, 2
    23db:	226d           	ae_l16si.n	a6, a2, 0
    23dd:	43fd           	ae_l16ui.n	a15, a3, 0
    23df:	222b           	addi.n	a2, a2, 2
    23e1:	332b           	addi.n	a3, a3, 2
    23e3:	148060        	extui	a8, a6, 0, 2
    23e6:	216260        	srai	a6, a6, 2
    23e9:	c0eef0        	sub	a14, a14, a15
    23ec:	c06690        	sub	a6, a6, a9
    23ef:	f74991        	l32r	a9, 114 (4a60 <acc_pow_quarter>)
    23f2:	2f0c           	movi.n	a15, 2
    23f4:	edc662        	addi	a6, a6, -19
    23f7:	a08890        	addx4	a8, a8, a9
    23fa:	608844        	ae_lq32f.i	aeq2, a8, 16
    23fd:	600804        	ae_lq32f.i	aeq0, a8, 0
    2400:	fc24fe86cf 	{ ae_slaasq56s	aeq3, aeq0, a6; nop }
    2405:	623114        	ae_sq32f.i	aeq3, a1, 4
    2408:	fc24fea64f 	{ ae_slaasq56s	aeq1, aeq2, a6; nop }
    240d:	fc24fe9f0f 	{ ae_slaasq56s	aeq0, aeq1, a15; nop }
    2412:	620124        	ae_sq32f.i	aeq0, a1, 8

00002415 <dehuffman_cb3_cb4+0x5d>:
    2415:	038654        	ae_vldsht	a5
    2418:	fccee2        	addi	a14, a14, -4

0000241b <dehuffman_cb3_cb4+0x63>:
    241b:	1ab8           	l32i.n	a11, a10, 4
    241d:	130c94        	ae_vldl16t	b0, a9, a12
    2420:	180b14        	ae_vldl16c	a11
    2423:	1ab9           	s32i.n	a11, a10, 4
    2425:	f20076        	bf	b0, 241b <dehuffman_cb3_cb4+0x63>
    2428:	148090        	extui	a8, a9, 0, 2
    242b:	a068d0        	addx4	a6, a8, a13
    242e:	0668           	l32i.n	a6, a6, 0
    2430:	c88c           	beqz.n	a8, 2440 <dehuffman_cb3_cb4+0x88>
    2432:	03ff14        	ae_lbi	a15, 1
    2435:	608060        	neg	a8, a6
    2438:	001bf4        	ae_dbi	a11, 1
    243b:	1ab9           	s32i.n	a11, a10, 4
    243d:	9368f0        	movnez	a6, a8, a15
    2440:	0469           	s32i.n	a6, a4, 0
    2442:	148290        	extui	a8, a9, 2, 2
    2445:	a068d0        	addx4	a6, a8, a13
    2448:	0668           	l32i.n	a6, a6, 0
    244a:	e88c           	beqz.n	a8, 245c <dehuffman_cb3_cb4+0xa4>
    244c:	03ff14        	ae_lbi	a15, 1
    244f:	1a88           	l32i.n	a8, a10, 4
    2451:	60b060        	neg	a11, a6
    2454:	936bf0        	movnez	a6, a11, a15
    2457:	0018f4        	ae_dbi	a8, 1
    245a:	1a89           	s32i.n	a8, a10, 4
    245c:	1469           	s32i.n	a6, a4, 4
    245e:	148490        	extui	a8, a9, 4, 2
    2461:	a068d0        	addx4	a6, a8, a13
    2464:	0668           	l32i.n	a6, a6, 0
    2466:	e88c           	beqz.n	a8, 2478 <dehuffman_cb3_cb4+0xc0>
    2468:	03ff14        	ae_lbi	a15, 1
    246b:	1a88           	l32i.n	a8, a10, 4
    246d:	60b060        	neg	a11, a6
    2470:	936bf0        	movnez	a6, a11, a15
    2473:	0018f4        	ae_dbi	a8, 1
    2476:	1a89           	s32i.n	a8, a10, 4
    2478:	2469           	s32i.n	a6, a4, 8
    247a:	148690        	extui	a8, a9, 6, 2
    247d:	a068d0        	addx4	a6, a8, a13
    2480:	0668           	l32i.n	a6, a6, 0
    2482:	e88c           	beqz.n	a8, 2494 <dehuffman_cb3_cb4+0xdc>
    2484:	03bf14        	ae_lbi	a11, 1
    2487:	1a88           	l32i.n	a8, a10, 4
    2489:	609060        	neg	a9, a6
    248c:	9369b0        	movnez	a6, a9, a11
    248f:	0018f4        	ae_dbi	a8, 1
    2492:	1a89           	s32i.n	a8, a10, 4
    2494:	3469           	s32i.n	a6, a4, 12
    2496:	10c442        	addi	a4, a4, 16
    2499:	021ea6        	blti	a14, 1, 249f <dehuffman_cb3_cb4+0xe7>
    249c:	ffdd46        	j	2415 <dehuffman_cb3_cb4+0x5d>

0000249f <dehuffman_cb3_cb4+0xe7>:
    249f:	770b           	addi.n	a7, a7, -1
    24a1:	f327d6        	bgez	a7, 23d7 <dehuffman_cb3_cb4+0x1f>
    24a4:	f01d           	retw.n

000024a6 <dehuffman_cb3_cb4+0xee>:
	...

000024a8 <dehuffman_cb5_cb6>:
    24a8:	00a136        	entry	a1, 80
    24ab:	075666        	bnei	a6, 5, 24b6 <dehuffman_cb5_cb6+0xe>
    24ae:	690c           	movi.n	a9, 6
    24b0:	f71c61        	l32r	a6, 120 (5620 <vld_huffdata5>)
    24b3:	000106        	j	24bb <dehuffman_cb5_cb6+0x13>

000024b6 <dehuffman_cb5_cb6+0xe>:
    24b6:	590c           	movi.n	a9, 5
    24b8:	f71b61        	l32r	a6, 124 (5800 <vld_huffdata6>)

000024bb <dehuffman_cb5_cb6+0x13>:
    24bb:	10c1f2        	addi	a15, a1, 16
    24be:	01bd           	mov.n	a11, a1
    24c0:	038694        	ae_vldsht	a9
    24c3:	0a0c           	movi.n	a10, 0
    24c5:	41a9           	s32i.n	a10, a1, 16
    24c7:	142192        	l32i	a9, a1, 80
    24ca:	002386        	j	255c <dehuffman_cb5_cb6+0xb4>

000024cd <dehuffman_cb5_cb6+0x25>:
    24cd:	53ad           	ae_l16ui.n	a10, a3, 2
    24cf:	43cd           	ae_l16ui.n	a12, a3, 0
    24d1:	228d           	ae_l16si.n	a8, a2, 0
    24d3:	222b           	addi.n	a2, a2, 2
    24d5:	332b           	addi.n	a3, a3, 2
    24d7:	21e280        	srai	a14, a8, 2
    24da:	c0aac0        	sub	a10, a10, a12
    24dd:	148080        	extui	a8, a8, 0, 2
    24e0:	f70dc1        	l32r	a12, 114 (4a60 <acc_pow_quarter>)
    24e3:	c0ee50        	sub	a14, a14, a5
    24e6:	efced2        	addi	a13, a14, -17
    24e9:	a088c0        	addx4	a8, a8, a12
    24ec:	60c844        	ae_lq32f.i	aeq3, a8, 16
    24ef:	f0cec2        	addi	a12, a14, -16
    24f2:	604804        	ae_lq32f.i	aeq1, a8, 0
    24f5:	edcee2        	addi	a14, a14, -19
    24f8:	fc24fe9e4f 	{ ae_slaasq56s	aeq1, aeq1, a14; nop }
    24fd:	18c882        	addi	a8, a8, 24
    2500:	608864        	ae_lq32f.i	aeq2, a8, 24
    2503:	600824        	ae_lq32f.i	aeq0, a8, 8
    2506:	fc24febdcf 	{ ae_slaasq56s	aeq3, aeq3, a13; nop }
    250b:	621f14        	ae_sq32f.i	aeq1, a15, 4
    250e:	fd14b9bf2f 	{ ae_sq32f.i	aeq3, a15, 8; ae_negq56	aeq1, aeq1 }
    2513:	ff94b99fff 	{ ae_sq32f.i	aeq1, a15, -4; ae_negq56	aeq3, aeq3 }
    2518:	623fe4        	ae_sq32f.i	aeq3, a15, -8
    251b:	fc24fe8d4f 	{ ae_slaasq56s	aeq1, aeq0, a13; nop }
    2520:	621f34        	ae_sq32f.i	aeq1, a15, 12
    2523:	fd14beac0f 	{ ae_slaasq56s	aeq0, aeq2, a12; ae_negq56	aeq1, aeq1 }
    2528:	621fd4        	ae_sq32f.i	aeq1, a15, -12
    252b:	fc10b98f4f 	{ ae_sq32f.i	aeq0, a15, 16; ae_negq56	aeq0, aeq0 }
    2530:	620fc4        	ae_sq32f.i	aeq0, a15, -16

00002533 <dehuffman_cb5_cb6+0x8b>:
    2533:	19d8           	l32i.n	a13, a9, 4
    2535:	1306c4        	ae_vldl16t	b0, a12, a6
    2538:	180d14        	ae_vldl16c	a13
    253b:	19d9           	s32i.n	a13, a9, 4
    253d:	f20076        	bf	b0, 2533 <dehuffman_cb5_cb6+0x8b>
    2540:	fecaa2        	addi	a10, a10, -2
    2543:	34e4c0        	extui	a14, a12, 4, 4
    2546:	3480c0        	extui	a8, a12, 0, 4
    2549:	a088b0        	addx4	a8, a8, a11
    254c:	a0eeb0        	addx4	a14, a14, a11
    254f:	0ee8           	l32i.n	a14, a14, 0
    2551:	0888           	l32i.n	a8, a8, 0
    2553:	0489           	s32i.n	a8, a4, 0
    2555:	14e9           	s32i.n	a14, a4, 4
    2557:	448b           	addi.n	a4, a4, 8
    2559:	d61ae6        	bgei	a10, 1, 2533 <dehuffman_cb5_cb6+0x8b>

0000255c <dehuffman_cb5_cb6+0xb4>:
    255c:	770b           	addi.n	a7, a7, -1
    255e:	f6b7d6        	bgez	a7, 24cd <dehuffman_cb5_cb6+0x25>
    2561:	f01d           	retw.n

00002563 <dehuffman_cb5_cb6+0xbb>:
	...

00002564 <dehuffman_cb7_8_9_10>:
    2564:	008136        	entry	a1, 64
    2567:	2129           	s32i.n	a2, a1, 8
    2569:	3139           	s32i.n	a3, a1, 12
    256b:	1159           	s32i.n	a5, a1, 4
    256d:	0179           	s32i.n	a7, a1, 0
    256f:	097666        	bnei	a6, 7, 257c <dehuffman_cb7_8_9_10+0x18>
    2572:	f6ed21        	l32r	a2, 128 (4da0 <vld_huffdata7>)
    2575:	5c0c           	movi.n	a12, 5
    2577:	41c9           	s32i.n	a12, a1, 16
    2579:	000886        	j	259f <dehuffman_cb7_8_9_10+0x3b>

0000257c <dehuffman_cb7_8_9_10+0x18>:
    257c:	098666        	bnei	a6, 8, 2589 <dehuffman_cb7_8_9_10+0x25>
    257f:	f6eb21        	l32r	a2, 12c (5100 <vld_huffdata8>)
    2582:	4d0c           	movi.n	a13, 4
    2584:	41d9           	s32i.n	a13, a1, 16
    2586:	000546        	j	259f <dehuffman_cb7_8_9_10+0x3b>

00002589 <dehuffman_cb7_8_9_10+0x25>:
    2589:	9e0c           	movi.n	a14, 9
    258b:	0996e7        	bne	a6, a14, 2598 <dehuffman_cb7_8_9_10+0x34>
    258e:	f6e821        	l32r	a2, 130 (4ef0 <vld_huffdata9>)
    2591:	4f0c           	movi.n	a15, 4
    2593:	41f9           	s32i.n	a15, a1, 16
    2595:	000186        	j	259f <dehuffman_cb7_8_9_10+0x3b>

00002598 <dehuffman_cb7_8_9_10+0x34>:
    2598:	520c           	movi.n	a2, 5
    259a:	4129           	s32i.n	a2, a1, 16
    259c:	f6e621        	l32r	a2, 134 (51d0 <vld_huffdata10>)
    259f:	102162        	l32i	a6, a1, 64

000025a2 <dehuffman_cb7_8_9_10+0x3e>:
    25a2:	0198           	l32i.n	a9, a1, 0
    25a4:	990b           	addi.n	a9, a9, -1
    25a6:	092996        	bltz	a9, 263c <dehuffman_cb7_8_9_10+0xd8>
    25a9:	21a8           	l32i.n	a10, a1, 8
    25ab:	3138           	l32i.n	a3, a1, 12
    25ad:	0199           	s32i.n	a9, a1, 0
    25af:	2a5d           	ae_l16si.n	a5, a10, 0
    25b1:	438d           	ae_l16ui.n	a8, a3, 0
    25b3:	aa2b           	addi.n	a10, a10, 2
    25b5:	332b           	addi.n	a3, a3, 2
    25b7:	3139           	s32i.n	a3, a1, 12
    25b9:	21a9           	s32i.n	a10, a1, 8
    25bb:	433d           	ae_l16ui.n	a3, a3, 0
    25bd:	14a050        	extui	a10, a5, 0, 2
    25c0:	51a9           	s32i.n	a10, a1, 20
    25c2:	11a8           	l32i.n	a10, a1, 4
    25c4:	215250        	srai	a5, a5, 2
    25c7:	c03380        	sub	a3, a3, a8
    25ca:	c055a0        	sub	a5, a5, a10

000025cd <dehuffman_cb7_8_9_10+0x69>:
    25cd:	41b8           	l32i.n	a11, a1, 16
    25cf:	0386b4        	ae_vldsht	a11

000025d2 <dehuffman_cb7_8_9_10+0x6e>:
    25d2:	16c8           	l32i.n	a12, a6, 4
    25d4:	130274        	ae_vldl16t	b0, a7, a2
    25d7:	180c14        	ae_vldl16c	a12
    25da:	16c9           	s32i.n	a12, a6, 4
    25dc:	f20076        	bf	b0, 25d2 <dehuffman_cb7_8_9_10+0x6e>
    25df:	34a070        	extui	a10, a7, 0, 4
    25e2:	7afc           	bnez.n	a10, 261d <dehuffman_cb7_8_9_10+0xb9>
    25e4:	0d0c           	movi.n	a13, 0
    25e6:	04d9           	s32i.n	a13, a4, 0

000025e8 <dehuffman_cb7_8_9_10+0x84>:
    25e8:	34a470        	extui	a10, a7, 4, 4
    25eb:	744b           	addi.n	a7, a4, 4
    25ed:	da9c           	beqz.n	a10, 260e <dehuffman_cb7_8_9_10+0xaa>
    25ef:	f69381        	l32r	a8, 3c (27c8 <aac_dequant_medium>)
    25f2:	05bd           	mov.n	a11, a5
    25f4:	51c8           	l32i.n	a12, a1, 20
    25f6:	0008e0        	callx8	a8
    25f9:	1698           	l32i.n	a9, a6, 4
    25fb:	60b0a0        	neg	a11, a10
    25fe:	03cf14        	ae_lbi	a12, 1
    2601:	0019f4        	ae_dbi	a9, 1
    2604:	1699           	s32i.n	a9, a6, 4
    2606:	93abc0        	movnez	a10, a11, a12
    2609:	07a9           	s32i.n	a10, a7, 0
    260b:	0000c6        	j	2612 <dehuffman_cb7_8_9_10+0xae>

0000260e <dehuffman_cb7_8_9_10+0xaa>:
    260e:	0d0c           	movi.n	a13, 0
    2610:	14d9           	s32i.n	a13, a4, 4

00002612 <dehuffman_cb7_8_9_10+0xae>:
    2612:	474b           	addi.n	a4, a7, 4
    2614:	fec332        	addi	a3, a3, -2
    2617:	b213e6        	bgei	a3, 1, 25cd <dehuffman_cb7_8_9_10+0x69>
    261a:	ffe106        	j	25a2 <dehuffman_cb7_8_9_10+0x3e>

0000261d <dehuffman_cb7_8_9_10+0xb9>:
    261d:	f68781        	l32r	a8, 3c (27c8 <aac_dequant_medium>)
    2620:	05bd           	mov.n	a11, a5
    2622:	51c8           	l32i.n	a12, a1, 20
    2624:	0008e0        	callx8	a8
    2627:	1698           	l32i.n	a9, a6, 4
    2629:	60b0a0        	neg	a11, a10
    262c:	03cf14        	ae_lbi	a12, 1
    262f:	0019f4        	ae_dbi	a9, 1
    2632:	1699           	s32i.n	a9, a6, 4
    2634:	93abc0        	movnez	a10, a11, a12
    2637:	04a9           	s32i.n	a10, a4, 0
    2639:	ffeac6        	j	25e8 <dehuffman_cb7_8_9_10+0x84>

0000263c <dehuffman_cb7_8_9_10+0xd8>:
    263c:	f01d           	retw.n

0000263e <dehuffman_cb7_8_9_10+0xda>:
	...

00002640 <dehuffman_cb11>:
    2640:	008136        	entry	a1, 64
    2643:	2129           	s32i.n	a2, a1, 8
    2645:	3139           	s32i.n	a3, a1, 12
    2647:	1159           	s32i.n	a5, a1, 4
    2649:	0169           	s32i.n	a6, a1, 0
    264b:	f6bb61        	l32r	a6, 138 (5b20 <vld_huffdata11>)
    264e:	41a9           	s32i.n	a10, a1, 16
    2650:	5199           	s32i.n	a9, a1, 20

00002652 <dehuffman_cb11+0x12>:
    2652:	0198           	l32i.n	a9, a1, 0
    2654:	990b           	addi.n	a9, a9, -1
    2656:	148996        	bltz	a9, 27a2 <dehuffman_cb11+0x162>
    2659:	21b8           	l32i.n	a11, a1, 8
    265b:	0199           	s32i.n	a9, a1, 0
    265d:	3158           	l32i.n	a5, a1, 12
    265f:	590c           	movi.n	a9, 5
    2661:	2bad           	ae_l16si.n	a10, a11, 0
    2663:	458d           	ae_l16ui.n	a8, a5, 0
    2665:	bb2b           	addi.n	a11, a11, 2
    2667:	552b           	addi.n	a5, a5, 2
    2669:	3159           	s32i.n	a5, a1, 12
    266b:	21b9           	s32i.n	a11, a1, 8
    266d:	455d           	ae_l16ui.n	a5, a5, 0
    266f:	14b0a0        	extui	a11, a10, 0, 2
    2672:	61b9           	s32i.n	a11, a1, 24
    2674:	11b8           	l32i.n	a11, a1, 4
    2676:	21a2a0        	srai	a10, a10, 2
    2679:	c05580        	sub	a5, a5, a8
    267c:	c0aab0        	sub	a10, a10, a11
    267f:	71a9           	s32i.n	a10, a1, 28

00002681 <dehuffman_cb11+0x41>:
    2681:	038694        	ae_vldsht	a9

00002684 <dehuffman_cb11+0x44>:
    2684:	17d8           	l32i.n	a13, a7, 4
    2686:	1306e4        	ae_vldl16t	b0, a14, a6
    2689:	180d14        	ae_vldl16c	a13
    268c:	17d9           	s32i.n	a13, a7, 4
    268e:	f20076        	bf	b0, 2684 <dehuffman_cb11+0x44>
    2691:	54a5e0        	extui	a10, a14, 5, 6
    2694:	8a8c           	beqz.n	a10, 26a0 <dehuffman_cb11+0x60>
    2696:	03cf14        	ae_lbi	a12, 1
    2699:	51c9           	s32i.n	a12, a1, 20
    269b:	001df4        	ae_dbi	a13, 1
    269e:	17d9           	s32i.n	a13, a7, 4

000026a0 <dehuffman_cb11+0x60>:
    26a0:	4420e0        	extui	a2, a14, 0, 5
    26a3:	828c           	beqz.n	a2, 26af <dehuffman_cb11+0x6f>
    26a5:	03ef14        	ae_lbi	a14, 1
    26a8:	41e9           	s32i.n	a14, a1, 16
    26aa:	001df4        	ae_dbi	a13, 1
    26ad:	17d9           	s32i.n	a13, a7, 4

000026af <dehuffman_cb11+0x6f>:
    26af:	05ca56        	bnez	a10, 270f <dehuffman_cb11+0xcf>
    26b2:	0f0c           	movi.n	a15, 0
    26b4:	04f9           	s32i.n	a15, a4, 0

000026b6 <dehuffman_cb11+0x76>:
    26b6:	344b           	addi.n	a3, a4, 4
    26b8:	0d2216        	beqz	a2, 278e <dehuffman_cb11+0x14e>
    26bb:	f0c282        	addi	a8, a2, -16
    26be:	0b3856        	bnez	a8, 2775 <dehuffman_cb11+0x135>
    26c1:	71b8           	l32i.n	a11, a1, 28
    26c3:	61c8           	l32i.n	a12, a1, 24
    26c5:	f69d81        	l32r	a8, 13c (27f8 <aac_dequant_large>)
    26c8:	6f2c           	movi.n	a15, 38
    26ca:	17d8           	l32i.n	a13, a7, 4
    26cc:	03ef54        	ae_lbi	a14, 5
    26cf:	f69ca1        	l32r	a10, 140 (1fffff <_end+0x1f6a7f>)
    26d2:	005df4        	ae_dbi	a13, 5
    26d5:	15ee04        	ae_lbki	a14, a14, 16
    26d8:	a46c           	movi.n	a4, -22
    26da:	17d9           	s32i.n	a13, a7, 4
    26dc:	30aea0        	xor	a10, a14, a10
    26df:	40faa0        	nsau	a10, a10
    26e2:	904a40        	addx2	a4, a10, a4
    26e5:	f9ca92        	addi	a9, a10, -7
    26e8:	401900        	ssl	a9
    26eb:	1c1d44        	ae_db	a13, a4
    26ee:	11aaf0        	slli	a10, a10, 1
    26f1:	17d9           	s32i.n	a13, a7, 4
    26f3:	c0ffa0        	sub	a15, a15, a10
    26f6:	1a0c           	movi.n	a10, 1
    26f8:	a1aa00        	sll	a10, a10
    26fb:	da0b           	addi.n	a13, a10, -1
    26fd:	400f00        	ssr	a15
    2700:	b1e0e0        	sra	a14, a14
    2703:	10dde0        	and	a13, a13, a14
    2706:	20aad0        	or	a10, a10, a13
    2709:	0008e0        	callx8	a8
    270c:	001c46        	j	2781 <dehuffman_cb11+0x141>

0000270f <dehuffman_cb11+0xcf>:
    270f:	4bba66        	bnei	a10, 16, 275e <dehuffman_cb11+0x11e>
    2712:	71b8           	l32i.n	a11, a1, 28
    2714:	61c8           	l32i.n	a12, a1, 24
    2716:	f68981        	l32r	a8, 13c (27f8 <aac_dequant_large>)
    2719:	632c           	movi.n	a3, 38
    271b:	03ff54        	ae_lbi	a15, 5
    271e:	f688a1        	l32r	a10, 140 (1fffff <_end+0x1f6a7f>)
    2721:	005df4        	ae_dbi	a13, 5
    2724:	15ff04        	ae_lbki	a15, a15, 16
    2727:	ae6c           	movi.n	a14, -22
    2729:	17d9           	s32i.n	a13, a7, 4
    272b:	30afa0        	xor	a10, a15, a10
    272e:	40faa0        	nsau	a10, a10
    2731:	90eae0        	addx2	a14, a10, a14
    2734:	f9ca92        	addi	a9, a10, -7
    2737:	401900        	ssl	a9
    273a:	1c1de4        	ae_db	a13, a14
    273d:	11aaf0        	slli	a10, a10, 1
    2740:	17d9           	s32i.n	a13, a7, 4
    2742:	c033a0        	sub	a3, a3, a10
    2745:	1a0c           	movi.n	a10, 1
    2747:	a1aa00        	sll	a10, a10
    274a:	ea0b           	addi.n	a14, a10, -1
    274c:	400300        	ssr	a3
    274f:	b1f0f0        	sra	a15, a15
    2752:	10eef0        	and	a14, a14, a15
    2755:	20aae0        	or	a10, a10, a14
    2758:	0008e0        	callx8	a8
    275b:	000246        	j	2768 <dehuffman_cb11+0x128>

0000275e <dehuffman_cb11+0x11e>:
    275e:	f63781        	l32r	a8, 3c (27c8 <aac_dequant_medium>)
    2761:	71b8           	l32i.n	a11, a1, 28
    2763:	61c8           	l32i.n	a12, a1, 24
    2765:	0008e0        	callx8	a8

00002768 <dehuffman_cb11+0x128>:
    2768:	51b8           	l32i.n	a11, a1, 20
    276a:	6090a0        	neg	a9, a10
    276d:	93a9b0        	movnez	a10, a9, a11
    2770:	04a9           	s32i.n	a10, a4, 0
    2772:	ffd006        	j	26b6 <dehuffman_cb11+0x76>

00002775 <dehuffman_cb11+0x135>:
    2775:	02ad           	mov.n	a10, a2
    2777:	f63181        	l32r	a8, 3c (27c8 <aac_dequant_medium>)
    277a:	71b8           	l32i.n	a11, a1, 28
    277c:	61c8           	l32i.n	a12, a1, 24
    277e:	0008e0        	callx8	a8

00002781 <dehuffman_cb11+0x141>:
    2781:	41b8           	l32i.n	a11, a1, 16
    2783:	6090a0        	neg	a9, a10
    2786:	93a9b0        	movnez	a10, a9, a11
    2789:	03a9           	s32i.n	a10, a3, 0
    278b:	0000c6        	j	2792 <dehuffman_cb11+0x152>

0000278e <dehuffman_cb11+0x14e>:
    278e:	0c0c           	movi.n	a12, 0
    2790:	14c9           	s32i.n	a12, a4, 4

00002792 <dehuffman_cb11+0x152>:
    2792:	434b           	addi.n	a4, a3, 4
    2794:	590c           	movi.n	a9, 5
    2796:	fec552        	addi	a5, a5, -2
    2799:	0215a6        	blti	a5, 1, 279f <dehuffman_cb11+0x15f>
    279c:	ffb846        	j	2681 <dehuffman_cb11+0x41>
    279f:	ffabc6        	j	2652 <dehuffman_cb11+0x12>

000027a2 <dehuffman_cb11+0x162>:
    27a2:	f01d           	retw.n

000027a4 <aac_dequant_small>:
    27a4:	004136        	entry	a1, 32
    27a7:	f66781        	l32r	a8, 144 (4aa0 <Q_quarter_exp>)
    27aa:	a05240        	addx4	a5, a2, a4
    27ad:	f65961        	l32r	a6, 114 (4a60 <acc_pow_quarter>)
    27b0:	908280        	addx2	a8, a2, a8
    27b3:	288d           	ae_l16si.n	a8, a8, 0
    27b5:	a05560        	addx4	a5, a5, a6
    27b8:	604504        	ae_lq32f.i	aeq1, a5, 0
    27bb:	283a           	add.n	a2, a8, a3
    27bd:	fc24fe920f 	{ ae_slaasq56s	aeq0, aeq1, a2; nop }
    27c2:	368204        	ae_trunca32q48	a2, aeq0
    27c5:	f01d           	retw.n

000027c7 <aac_dequant_small+0x23>:
	...

000027c8 <aac_dequant_medium>:
    27c8:	004136        	entry	a1, 32
    27cb:	f65251        	l32r	a5, 114 (4a60 <acc_pow_quarter>)
    27ce:	f61a61        	l32r	a6, 38 (4370 <aac_pow4_3>)
    27d1:	a06260        	addx4	a6, a2, a6
    27d4:	0668           	l32i.n	a6, a6, 0
    27d6:	a05450        	addx4	a5, a4, a5
    27d9:	0558           	l32i.n	a5, a5, 0
    27db:	347060        	extui	a7, a6, 0, 4
    27de:	c06670        	sub	a6, a6, a7
    27e1:	273a           	add.n	a2, a7, a3
    27e3:	eec222        	addi	a2, a2, -18
    27e6:	b25560        	mulsh	a5, a5, a6
    27e9:	364534        	ae_cvtq48a32s	aeq1, a5
    27ec:	fc24fe920f 	{ ae_slaasq56s	aeq0, aeq1, a2; nop }
    27f1:	368204        	ae_trunca32q48	a2, aeq0
    27f4:	f01d           	retw.n

000027f6 <aac_dequant_medium+0x2e>:
	...

000027f8 <aac_dequant_large>:
    27f8:	004136        	entry	a1, 32
    27fb:	f60f71        	l32r	a7, 38 (4370 <aac_pow4_3>)
    27fe:	80a052        	movi	a5, 128
    2801:	6da527        	bge	a5, a2, 2872 <aac_dequant_large+0x7a>
    2804:	1a0c           	movi.n	a10, 1
    2806:	f650b1        	l32r	a11, 148 (4a20 <crs_mant>)
    2809:	f65091        	l32r	a9, 14c (4a50 <crs_exp>)
    280c:	360234        	ae_cvtq48a32s	aeq0, a2
    280f:	364c04        	ae_nsaq56s	a12, aeq0
    2812:	082c           	movi.n	a8, 32
    2814:	f61c           	movi.n	a6, 31
    2816:	c066c0        	sub	a6, a6, a12
    2819:	c088c0        	sub	a8, a8, a12
    281c:	400800        	ssr	a8
    281f:	906690        	addx2	a6, a6, a9
    2822:	266d           	ae_l16si.n	a6, a6, 0
    2824:	b15020        	sra	a5, a2
    2827:	a05570        	addx4	a5, a5, a7
    282a:	1598           	l32i.n	a9, a5, 4
    282c:	34d060        	extui	a13, a6, 0, 4
    282f:	0558           	l32i.n	a5, a5, 0
    2831:	416460        	srli	a6, a6, 4
    2834:	a06640        	addx4	a6, a6, a4
    2837:	34c050        	extui	a12, a5, 0, 4
    283a:	a066b0        	addx4	a6, a6, a11
    283d:	0668           	l32i.n	a6, a6, 0
    283f:	c055c0        	sub	a5, a5, a12
    2842:	34b090        	extui	a11, a9, 0, 4
    2845:	4bda           	add.n	a4, a11, a13
    2847:	c099b0        	sub	a9, a9, a11
    284a:	c0bbc0        	sub	a11, a11, a12
    284d:	400b00        	ssr	a11
    2850:	b15050        	sra	a5, a5
    2853:	c09950        	sub	a9, a9, a5
    2856:	401800        	ssl	a8
    2859:	a1aa00        	sll	a10, a10
    285c:	aa0b           	addi.n	a10, a10, -1
    285e:	1022a0        	and	a2, a2, a10
    2861:	822290        	mull	a2, a2, a9
    2864:	400800        	ssr	a8
    2867:	b12020        	sra	a2, a2
    286a:	252a           	add.n	a2, a5, a2
    286c:	b22620        	mulsh	a2, a6, a2
    286f:	000546        	j	2888 <aac_dequant_large+0x90>

00002872 <aac_dequant_large+0x7a>:
    2872:	a05270        	addx4	a5, a2, a7
    2875:	f62721        	l32r	a2, 114 (4a60 <acc_pow_quarter>)
    2878:	0558           	l32i.n	a5, a5, 0
    287a:	a02420        	addx4	a2, a4, a2
    287d:	0228           	l32i.n	a2, a2, 0
    287f:	344050        	extui	a4, a5, 0, 4
    2882:	c05540        	sub	a5, a5, a4
    2885:	b22250        	mulsh	a2, a2, a5

00002888 <aac_dequant_large+0x90>:
    2888:	368234        	ae_cvtq48a32s	aeq2, a2
    288b:	834a           	add.n	a8, a3, a4
    288d:	eec882        	addi	a8, a8, -18
    2890:	fc24fea84f 	{ ae_slaasq56s	aeq1, aeq2, a8; nop }
    2895:	369204        	ae_trunca32q48	a2, aeq1
    2898:	f01d           	retw.n

0000289a <aac_dequant_large+0xa2>:
    289a:	00000000 41360000                                ......

000028a0 <aac_passf4pos_ido1>:
    28a0:	004136        	entry	a1, 32
    28a3:	b08240        	addx8	a8, a2, a4
    28a6:	906220        	addx2	a6, a2, a2
    28a9:	1172e0        	slli	a7, a2, 2
    28ac:	a07740        	addx4	a7, a7, a4
    28af:	1166f0        	slli	a6, a6, 1
    28b2:	a06640        	addx4	a6, a6, a4
    28b5:	4ea276        	loopgtz	a2, 2907 <aac_passf4pos_ido1+0x67>
    28b8:	73a8           	l32i.n	a10, a3, 28
    28ba:	53b8           	l32i.n	a11, a3, 20
    28bc:	63e8           	l32i.n	a14, a3, 24
    28be:	23f8           	l32i.n	a15, a3, 8
    28c0:	1328           	l32i.n	a2, a3, 4
    28c2:	33d8           	l32i.n	a13, a3, 12
    28c4:	cfea           	add.n	a12, a15, a14
    28c6:	52ba           	add.n	a5, a2, a11
    28c8:	c0ffe0        	sub	a15, a15, a14
    28cb:	c022b0        	sub	a2, a2, a11
    28ce:	9daa           	add.n	a9, a13, a10
    28d0:	c0e2f0        	sub	a14, a2, a15
    28d3:	ff2a           	add.n	a15, a15, a2
    28d5:	c02590        	sub	a2, a5, a9
    28d8:	559a           	add.n	a5, a5, a9
    28da:	1459           	s32i.n	a5, a4, 4
    28dc:	1729           	s32i.n	a2, a7, 4
    28de:	18f9           	s32i.n	a15, a8, 4
    28e0:	16e9           	s32i.n	a14, a6, 4
    28e2:	43e8           	l32i.n	a14, a3, 16
    28e4:	03b8           	l32i.n	a11, a3, 0
    28e6:	c0aad0        	sub	a10, a10, a13
    28e9:	dbea           	add.n	a13, a11, a14
    28eb:	20c332        	addi	a3, a3, 32
    28ee:	c0bbe0        	sub	a11, a11, a14
    28f1:	c09ba0        	sub	a9, a11, a10
    28f4:	aaba           	add.n	a10, a10, a11
    28f6:	c0bdc0        	sub	a11, a13, a12
    28f9:	ccda           	add.n	a12, a12, a13
    28fb:	09f4c4        	ae_s32ip	a12, a4, 8
    28fe:	09f7b4        	ae_s32ip	a11, a7, 8
    2901:	09f8a4        	ae_s32ip	a10, a8, 8
    2904:	09f694        	ae_s32ip	a9, a6, 8

00002907 <aac_passf4pos_ido1+0x67>:
    2907:	f01d           	retw.n

00002909 <aac_passf4pos_ido1+0x69>:
    2909:	000000                                        ...

0000290c <aac_passf4pos>:
    290c:	018136        	entry	a1, 192
    290f:	d139           	s32i.n	a3, a1, 52
    2911:	4149           	s32i.n	a4, a1, 16
    2913:	5159           	s32i.n	a5, a1, 20
    2915:	0213e6        	bgei	a3, 1, 291b <aac_passf4pos+0xf>
    2918:	009f86        	j	2b9a <aac_passf4pos+0x28e>
    291b:	0129           	s32i.n	a2, a1, 0
    291d:	b08260        	addx8	a8, a2, a6
    2920:	2c0c           	movi.n	a12, 2
    2922:	090c           	movi.n	a9, 0
    2924:	11e2b0        	slli	a14, a2, 5
    2927:	f20b           	addi.n	a15, a2, -1
    2929:	2b0c           	movi.n	a11, 2
    292b:	f8c6a2        	addi	a10, a6, -8
    292e:	05dd           	mov.n	a13, a5
    2930:	1261d2        	s32i	a13, a1, 72
    2933:	61a9           	s32i.n	a10, a1, 24
    2935:	1361b2        	s32i	a11, a1, 76
    2938:	31f9           	s32i.n	a15, a1, 12
    293a:	81e9           	s32i.n	a14, a1, 32
    293c:	f199           	s32i.n	a9, a1, 60
    293e:	1061c2        	s32i	a12, a1, 64
    2941:	f8c882        	addi	a8, a8, -8
    2944:	9189           	s32i.n	a8, a1, 36
    2946:	21c170        	srai	a12, a7, 1
    2949:	909220        	addx2	a9, a2, a2
    294c:	11e2e0        	slli	a14, a2, 2
    294f:	11f2d0        	slli	a15, a2, 3
    2952:	11b2c0        	slli	a11, a2, 4
    2955:	04ad           	mov.n	a10, a4
    2957:	1161a2        	s32i	a10, a1, 68
    295a:	b6ba           	add.n	a11, a6, a11
    295c:	71f9           	s32i.n	a15, a1, 28
    295e:	11e9           	s32i.n	a14, a1, 4
    2960:	1199f0        	slli	a9, a9, 1
    2963:	1172f0        	slli	a7, a2, 1
    2966:	e179           	s32i.n	a7, a1, 56
    2968:	2199           	s32i.n	a9, a1, 8
    296a:	1187e0        	slli	a8, a7, 2
    296d:	a0ee40        	addx4	a14, a14, a4
    2970:	11ffe0        	slli	a15, a15, 2
    2973:	f8cbb2        	addi	a11, a11, -8
    2976:	a1b9           	s32i.n	a11, a1, 40
    2978:	b1f9           	s32i.n	a15, a1, 44
    297a:	1461e2        	s32i	a14, a1, 80
    297d:	c189           	s32i.n	a8, a1, 48
    297f:	a09940        	addx4	a9, a9, a4
    2982:	166192        	s32i	a9, a1, 88
    2985:	8cca           	add.n	a8, a12, a12
    2987:	a07740        	addx4	a7, a7, a4
    298a:	90ecc0        	addx2	a14, a12, a12
    298d:	11fcf0        	slli	a15, a12, 1
    2990:	a0ff50        	addx4	a15, a15, a5
    2993:	a0ee50        	addx4	a14, a14, a5
    2996:	156172        	s32i	a7, a1, 84
    2999:	a0b850        	addx4	a11, a8, a5
    299c:	1761b2        	s32i	a11, a1, 92
    299f:	a07c50        	addx4	a7, a12, a5
    29a2:	ee4b           	addi.n	a14, a14, 4
    29a4:	ff4b           	addi.n	a15, a15, 4
    29a6:	1961f2        	s32i	a15, a1, 100
    29a9:	1a61e2        	s32i	a14, a1, 104
    29ac:	cc8a           	add.n	a12, a12, a8
    29ae:	b74b           	addi.n	a11, a7, 4
    29b0:	1861b2        	s32i	a11, a1, 96
    29b3:	a0cc50        	addx4	a12, a12, a5
    29b6:	1b61c2        	s32i	a12, a1, 108

000029b9 <aac_passf4pos+0xad>:
    29b9:	172162        	l32i	a6, a1, 92
    29bc:	112192        	l32i	a9, a1, 68
    29bf:	152142        	l32i	a4, a1, 84
    29c2:	162122        	l32i	a2, a1, 88
    29c5:	142132        	l32i	a3, a1, 80
    29c8:	04a8           	l32i.n	a10, a4, 0
    29ca:	02b8           	l32i.n	a11, a2, 0
    29cc:	1358           	l32i.n	a5, a3, 4
    29ce:	19f8           	l32i.n	a15, a9, 4
    29d0:	0338           	l32i.n	a3, a3, 0
    29d2:	0998           	l32i.n	a9, a9, 0
    29d4:	1228           	l32i.n	a2, a2, 4
    29d6:	1448           	l32i.n	a4, a4, 4
    29d8:	df5a           	add.n	a13, a15, a5
    29da:	caba           	add.n	a12, a10, a11
    29dc:	c0ff50        	sub	a15, a15, a5
    29df:	e42a           	add.n	a14, a4, a2
    29e1:	c0aab0        	sub	a10, a10, a11
    29e4:	c02240        	sub	a2, a2, a4
    29e7:	c08de0        	sub	a8, a13, a14
    29ea:	b93a           	add.n	a11, a9, a3
    29ec:	ddea           	add.n	a13, a13, a14
    29ee:	c09930        	sub	a9, a9, a3
    29f1:	1321e2        	l32i	a14, a1, 76
    29f4:	1b2132        	l32i	a3, a1, 108
    29f7:	c04920        	sub	a4, a9, a2
    29fa:	c05bc0        	sub	a5, a11, a12
    29fd:	992a           	add.n	a9, a9, a2
    29ff:	bbca           	add.n	a11, a11, a12
    2a01:	c02fa0        	sub	a2, a15, a10
    2a04:	1221c2        	l32i	a12, a1, 72
    2a07:	aafa           	add.n	a10, a10, a15
    2a09:	01f8           	l32i.n	a15, a1, 0
    2a0b:	1cd9           	s32i.n	a13, a12, 4
    2a0d:	0cb9           	s32i.n	a11, a12, 0
    2a0f:	1021d2        	l32i	a13, a1, 64
    2a12:	91b8           	l32i.n	a11, a1, 36
    2a14:	61c8           	l32i.n	a12, a1, 24
    2a16:	17a9           	s32i.n	a10, a7, 4
    2a18:	0799           	s32i.n	a9, a7, 0
    2a1a:	a1a8           	l32i.n	a10, a1, 40
    2a1c:	182192        	l32i	a9, a1, 96
    2a1f:	0659           	s32i.n	a5, a6, 0
    2a21:	1689           	s32i.n	a8, a6, 4
    2a23:	075d           	mov.n	a5, a7
    2a25:	0349           	s32i.n	a4, a3, 0
    2a27:	192172        	l32i	a7, a1, 100
    2a2a:	1329           	s32i.n	a2, a3, 4
    2a2c:	1a2162        	l32i	a6, a1, 104
    2a2f:	022fe6        	bgei	a15, 2, 2a35 <aac_passf4pos+0x129>
    2a32:	003a86        	j	2b20 <aac_passf4pos+0x214>
    2a35:	11f8           	l32i.n	a15, a1, 4
    2a37:	2188           	l32i.n	a8, a1, 8
    2a39:	1c6152        	s32i	a5, a1, 112
    2a3c:	5128           	l32i.n	a2, a1, 20
    2a3e:	4158           	l32i.n	a5, a1, 16
    2a40:	88da           	add.n	a8, a8, a13
    2a42:	a02e20        	addx4	a2, a14, a2
    2a45:	ffda           	add.n	a15, a15, a13
    2a47:	e1e8           	l32i.n	a14, a1, 56
    2a49:	a0ff50        	addx4	a15, a15, a5
    2a4c:	a04d50        	addx4	a4, a13, a5
    2a4f:	edea           	add.n	a14, a13, a14
    2a51:	a0ee50        	addx4	a14, a14, a5
    2a54:	a0d850        	addx4	a13, a8, a5
    2a57:	3158           	l32i.n	a5, a1, 12
    2a59:	c0a576        	loopgtz	a5, 2b1d <aac_passf4pos+0x211>
    2a5c:	2561d2        	s32i	a13, a1, 148
    2a5f:	1d58           	l32i.n	a5, a13, 4
    2a61:	1e38           	l32i.n	a3, a14, 4
    2a63:	2261a2        	s32i	a10, a1, 136
    2a66:	2461b2        	s32i	a11, a1, 144
    2a69:	0ea8           	l32i.n	a10, a14, 0
    2a6b:	0fb8           	l32i.n	a11, a15, 0
    2a6d:	ee8b           	addi.n	a14, a14, 8
    2a6f:	0dd8           	l32i.n	a13, a13, 0
    2a71:	c08530        	sub	a8, a5, a3
    2a74:	236182        	s32i	a8, a1, 140
    2a77:	335a           	add.n	a3, a3, a5
    2a79:	266132        	s32i	a3, a1, 152
    2a7c:	0488           	l32i.n	a8, a4, 0
    2a7e:	3ada           	add.n	a3, a10, a13
    2a80:	c0aad0        	sub	a10, a10, a13
    2a83:	d8ba           	add.n	a13, a8, a11
    2a85:	c088b0        	sub	a8, a8, a11
    2a88:	c05d30        	sub	a5, a13, a3
    2a8b:	360534        	ae_cvtq48a32s	aeq0, a5
    2a8e:	1fb8           	l32i.n	a11, a15, 4
    2a90:	1458           	l32i.n	a5, a4, 4
    2a92:	dd3a           	add.n	a13, a13, a3
    2a94:	ff8b           	addi.n	a15, a15, 8
    2a96:	35ba           	add.n	a3, a5, a11
    2a98:	c055b0        	sub	a5, a5, a11
    2a9b:	2621b2        	l32i	a11, a1, 152
    2a9e:	448b           	addi.n	a4, a4, 8
    2aa0:	02d9           	s32i.n	a13, a2, 0
    2aa2:	c0d3b0        	sub	a13, a3, a11
    2aa5:	368d34        	ae_cvtq48a32s	aeq2, a13
    2aa8:	33ba           	add.n	a3, a3, a11
    2aaa:	78c1d2        	addi	a13, a1, 120
    2aad:	1239           	s32i.n	a3, a2, 4
    2aaf:	2421b2        	l32i	a11, a1, 144
    2ab2:	228b           	addi.n	a2, a2, 8
    2ab4:	5cbc14        	ae_lp24x2f.iu	aep3, a12, 8
    2ab7:	5ccb14        	ae_lp24x2f.iu	aep4, a11, 8
    2aba:	fa90503a5f 	{ add	a3, a10, a5; ae_mulfq32sp24s.l	aeq1, aeq2, aep4 }
    2abf:	f452d555af 	{ sub	a5, a5, a10; ae_mulafq32sp24s.h	aeq1, aeq0, aep4 }
    2ac4:	2321a2        	l32i	a10, a1, 140
    2ac7:	631d04        	ae_sq56s.i	aeq1, a13, 0
    2aca:	364334        	ae_cvtq48a32s	aeq1, a3
    2acd:	631d14        	ae_sq56s.i	aeq1, a13, 8
    2ad0:	f89010d8af 	{ add	a13, a8, a10; ae_mulfq32sp24s.l	aeq0, aeq0, aep4 }
    2ad5:	fa509588af 	{ sub	a8, a8, a10; ae_mulsfq32sp24s.h	aeq0, aeq2, aep4 }
    2ada:	f88d7ecdff 	{ ae_cvtq48a32s	aeq3, a13; ae_mulfq32sp24s.l	aeq1, aeq1, aep3 }
    2adf:	fb8d0781ae 	{ addi	a10, a1, 120; ae_mulfq32sp24s.l	aeq2, aeq3, aep3 }
    2ae4:	f64ffdea1f 	{ ae_lq56.i	aeq3, a10, 8; ae_mulafq32sp24s.h	aeq1, aeq3, aep3 }
    2ae9:	fb4db251de 	{ l32i	a13, a1, 148; ae_mulsfq32sp24s.h	aeq2, aeq3, aep3 }
    2aee:	2221a2        	l32i	a10, a1, 136
    2af1:	626914        	ae_sq32f.iu	aeq2, a9, 4
    2af4:	625914        	ae_sq32f.iu	aeq1, a9, 4
    2af7:	364534        	ae_cvtq48a32s	aeq1, a5
    2afa:	624714        	ae_sq32f.iu	aeq0, a7, 4
    2afd:	5caa14        	ae_lp24x2f.iu	aep2, a10, 8
    2b00:	f9893ec8ff 	{ ae_cvtq48a32s	aeq3, a8; ae_mulfq32sp24s.l	aeq2, aeq1, aep2 }
    2b05:	f74b87813e 	{ addi	a3, a1, 120; ae_mulafq32sp24s.h	aeq2, aeq3, aep2 }
    2b0a:	fb89408dde 	{ addi	a13, a13, 8; ae_mulfq32sp24s.l	aeq3, aeq3, aep2 }
    2b0f:	f949fd830f 	{ ae_lq56.i	aeq0, a3, 0; ae_mulsfq32sp24s.h	aeq3, aeq1, aep2 }
    2b14:	624714        	ae_sq32f.iu	aeq0, a7, 4
    2b17:	627614        	ae_sq32f.iu	aeq3, a6, 4
    2b1a:	626614        	ae_sq32f.iu	aeq2, a6, 4

00002b1d <aac_passf4pos+0x211>:
    2b1d:	1c2152        	l32i	a5, a1, 112

00002b20 <aac_passf4pos+0x214>:
    2b20:	1021f2        	l32i	a15, a1, 64
    2b23:	1a21e2        	l32i	a14, a1, 104
    2b26:	e1c8           	l32i.n	a12, a1, 56
    2b28:	1321b2        	l32i	a11, a1, 76
    2b2b:	1921d2        	l32i	a13, a1, 100
    2b2e:	c178           	l32i.n	a7, a1, 48
    2b30:	1821a2        	l32i	a10, a1, 96
    2b33:	1b2192        	l32i	a9, a1, 108
    2b36:	172182        	l32i	a8, a1, 92
    2b39:	aa7a           	add.n	a10, a10, a7
    2b3b:	997a           	add.n	a9, a9, a7
    2b3d:	887a           	add.n	a8, a8, a7
    2b3f:	dd7a           	add.n	a13, a13, a7
    2b41:	bbca           	add.n	a11, a11, a12
    2b43:	ee7a           	add.n	a14, a14, a7
    2b45:	b1c8           	l32i.n	a12, a1, 44
    2b47:	1a61e2        	s32i	a14, a1, 104
    2b4a:	1961d2        	s32i	a13, a1, 100
    2b4d:	176182        	s32i	a8, a1, 92
    2b50:	1b6192        	s32i	a9, a1, 108
    2b53:	1361b2        	s32i	a11, a1, 76
    2b56:	1861a2        	s32i	a10, a1, 96
    2b59:	1221b2        	l32i	a11, a1, 72
    2b5c:	81a8           	l32i.n	a10, a1, 32
    2b5e:	112192        	l32i	a9, a1, 68
    2b61:	7188           	l32i.n	a8, a1, 28
    2b63:	1521d2        	l32i	a13, a1, 84
    2b66:	1621e2        	l32i	a14, a1, 88
    2b69:	757a           	add.n	a7, a5, a7
    2b6b:	ddca           	add.n	a13, a13, a12
    2b6d:	eeca           	add.n	a14, a14, a12
    2b6f:	ff8a           	add.n	a15, a15, a8
    2b71:	1061f2        	s32i	a15, a1, 64
    2b74:	1661e2        	s32i	a14, a1, 88
    2b77:	1561d2        	s32i	a13, a1, 84
    2b7a:	99aa           	add.n	a9, a9, a10
    2b7c:	bb8a           	add.n	a11, a11, a8
    2b7e:	f1a8           	l32i.n	a10, a1, 60
    2b80:	1261b2        	s32i	a11, a1, 72
    2b83:	116192        	s32i	a9, a1, 68
    2b86:	1421b2        	l32i	a11, a1, 80
    2b89:	d198           	l32i.n	a9, a1, 52
    2b8b:	aa1b           	addi.n	a10, a10, 1
    2b8d:	f1a9           	s32i.n	a10, a1, 60
    2b8f:	c099a0        	sub	a9, a9, a10
    2b92:	bbca           	add.n	a11, a11, a12
    2b94:	1461b2        	s32i	a11, a1, 80
    2b97:	e1e956        	bnez	a9, 29b9 <aac_passf4pos+0xad>

00002b9a <aac_passf4pos+0x28e>:
    2b9a:	f01d           	retw.n

00002b9c <imdct_long>:
    2b9c:	006136        	entry	a1, 48
    2b9f:	f56c71        	l32r	a7, 150 (1004 <channel_pair_element>)
    2ba2:	f8c2b2        	addi	a11, a2, -8
    2ba5:	00a1e2        	movi	a14, 0x100
    2ba8:	fcc252        	addi	a5, a2, -4
    2bab:	00a282        	movi	a8, 0x200
    2bae:	f56961        	l32r	a6, 154 (7098 <fft_twi64_L+0x1d8>)
    2bb1:	fcc3d2        	addi	a13, a3, -4
    2bb4:	10d2a2        	addmi	a10, a2, 0x1000
    2bb7:	aa4b           	addi.n	a10, a10, 4
    2bb9:	0d9d           	mov.n	a9, a13
    2bbb:	06cd           	mov.n	a12, a6
    2bbd:	19a876        	loopgtz	a8, 2bda <imdct_long+0x3e>
    2bc0:	5c8c14        	ae_lp24x2f.iu	aep0, a12, 8
    2bc3:	60db24        	ae_lq32f.iu	aeq3, a11, 8
    2bc6:	fa813d5aef 	{ ae_lq32f.iu	aeq2, a10, -8; ae_mulfq32sp24s.l	aeq0, aeq3, aep0 }
    2bcb:	080da4        	ae_mulfq32sp24s.l	aeq1, aeq2, aep0
    2bce:	0808e4        	ae_mulsfq32sp24s.h	aeq1, aeq3, aep0
    2bd1:	0802a4        	ae_mulafq32sp24s.h	aeq0, aeq2, aep0
    2bd4:	625914        	ae_sq32f.iu	aeq1, a9, 4
    2bd7:	624914        	ae_sq32f.iu	aeq0, a9, 4

00002bda <imdct_long+0x3e>:
    2bda:	f55fc1        	l32r	a12, 158 (5ed8 <sample_rates+0x28>)
    2bdd:	05ad           	mov.n	a10, a5
    2bdf:	08d292        	addmi	a9, a2, 0x800
    2be2:	08d3b2        	addmi	a11, a3, 0x800
    2be5:	fccbb2        	addi	a11, a11, -4
    2be8:	fcc992        	addi	a9, a9, -4
    2beb:	f03d           	nop.n
    2bed:	38ae76        	loopgtz	a14, 2c29 <imdct_long+0x8d>
    2bf0:	5ccc14        	ae_lp24x2f.iu	aep4, a12, 8
    2bf3:	609b14        	ae_lq32f.iu	aeq2, a11, 4
    2bf6:	601d14        	ae_lq32f.iu	aeq0, a13, 4
    2bf9:	fbe27d3d1f 	{ ae_lq32f.iu	aeq1, a13, 4; ae_addsq56s	aeq3, aeq0, aeq2 }
    2bfe:	fac2b9fa1f 	{ ae_sq32f.iu	aeq3, a10, 4; ae_subsq56s	aeq2, aeq0, aeq2 }
    2c03:	632104        	ae_sq56s.i	aeq2, a1, 0
    2c06:	fb903d7b1f 	{ ae_lq32f.iu	aeq3, a11, 4; ae_mulfq32sp24s.l	aeq2, aeq2, aep4 }
    2c0b:	061b14        	ae_subsq56s	aeq0, aeq1, aeq3
    2c0e:	08c024        	ae_mulsfq32sp24s.h	aeq2, aeq0, aep4
    2c11:	06db04        	ae_addsq56s	aeq3, aeq1, aeq3
    2c14:	f8903da10f 	{ ae_lq56.i	aeq1, a1, 0; ae_mulfq32sp24s.l	aeq0, aeq0, aep4 }
    2c19:	084264        	ae_mulafq32sp24s.h	aeq0, aeq1, aep4
    2c1c:	fd5479fa1f 	{ ae_sq32f.iu	aeq3, a10, 4; ae_roundsq32sym	aeq1, aeq2 }
    2c21:	ff4479d91f 	{ ae_sq32f.iu	aeq1, a9, 4; ae_roundsq32sym	aeq3, aeq0 }
    2c26:	627914        	ae_sq32f.iu	aeq3, a9, 4
    2c29:	0a4c           	movi.n	a10, 64
    2c2b:	2b0c           	movi.n	a11, 2
    2c2d:	02cd           	mov.n	a12, a2
    2c2f:	03dd           	mov.n	a13, a3
    2c31:	f54ae1        	l32r	a14, 15c (66e8 <fft_twi512_L+0x808>)
    2c34:	00a2f2        	movi	a15, 0x200
    2c37:	ffcd65        	call8	290c <aac_passf4pos>
    2c3a:	0a1c           	movi.n	a10, 16
    2c3c:	8b0c           	movi.n	a11, 8
    2c3e:	03cd           	mov.n	a12, a3
    2c40:	02dd           	mov.n	a13, a2
    2c42:	f547e1        	l32r	a14, 160 (6ce8 <fft_twi512_L+0xe08>)
    2c45:	00a2f2        	movi	a15, 0x200
    2c48:	ffcc25        	call8	290c <aac_passf4pos>
    2c4b:	4a0c           	movi.n	a10, 4
    2c4d:	0b2c           	movi.n	a11, 32
    2c4f:	02cd           	mov.n	a12, a2
    2c51:	03dd           	mov.n	a13, a3
    2c53:	f544e1        	l32r	a14, 164 (6e68 <fft_twi512_L+0xf88>)
    2c56:	00a2f2        	movi	a15, 0x200
    2c59:	ffcb25        	call8	290c <aac_passf4pos>
    2c5c:	02cd           	mov.n	a12, a2
    2c5e:	80a0a2        	movi	a10, 128
    2c61:	03bd           	mov.n	a11, a3
    2c63:	ffc3e5        	call8	28a0 <aac_passf4pos_ido1>
    2c66:	00a1d2        	movi	a13, 0x100
    2c69:	08d4b2        	addmi	a11, a4, 0x800
    2c6c:	08d392        	addmi	a9, a3, 0x800
    2c6f:	f8c9a2        	addi	a10, a9, -8
    2c72:	f8cbc2        	addi	a12, a11, -8
    2c75:	994b           	addi.n	a9, a9, 4
    2c77:	bb4b           	addi.n	a11, a11, 4
    2c79:	23ad76        	loopgtz	a13, 2ca0 <imdct_long+0x104>
    2c7c:	5cd614        	ae_lp24x2f.iu	aep5, a6, 8
    2c7f:	60d514        	ae_lq32f.iu	aeq3, a5, 4
    2c82:	fa153d351f 	{ ae_lq32f.iu	aeq1, a5, 4; ae_mulfq32sp24s.h	aeq0, aeq3, aep5 }
    2c87:	085364        	ae_mulafq32sp24s.l	aeq0, aeq1, aep5
    2c8a:	08dde4        	ae_mulfq32sp24s.l	aeq3, aeq3, aep5
    2c8d:	060824        	ae_negq56	aeq0, aeq0
    2c90:	f955f9cc2f 	{ ae_sq32f.iu	aeq0, a12, 8; ae_mulsfq32sp24s.h	aeq3, aeq1, aep5 }
    2c95:	624be4        	ae_sq32f.iu	aeq0, a11, -8

00002c98 <imdct_long+0xfc>:
    2c98:	ff94b9fa2f 	{ ae_sq32f.iu	aeq3, a10, 8; ae_negq56	aeq3, aeq3 }
    2c9d:	6279e4        	ae_sq32f.iu	aeq3, a9, -8

00002ca0 <imdct_long+0x104>:
    2ca0:	f8c4a2        	addi	a10, a4, -8
    2ca3:	947a           	add.n	a9, a4, a7
    2ca5:	b37a           	add.n	a11, a3, a7
    2ca7:	f8c3c2        	addi	a12, a3, -8
    2caa:	00a1d2        	movi	a13, 0x100
    2cad:	21ad76        	loopgtz	a13, 2cd2 <imdct_long+0x136>
    2cb0:	5ce614        	ae_lp24x2f.iu	aep6, a6, 8
    2cb3:	605514        	ae_lq32f.iu	aeq1, a5, 4
    2cb6:	086564        	ae_mulfq32sp24s.l	aeq0, aeq1, aep6
    2cb9:	f8197d551f 	{ ae_lq32f.iu	aeq2, a5, 4; ae_mulfq32sp24s.h	aeq1, aeq1, aep6 }
    2cbe:	086ba4        	ae_mulafq32sp24s.l	aeq1, aeq2, aep6
    2cc1:	0860a4        	ae_mulsfq32sp24s.h	aeq0, aeq2, aep6

00002cc4 <imdct_long+0x128>:
    2cc4:	fd14b9dc2f 	{ ae_sq32f.iu	aeq1, a12, 8; ae_negq56	aeq1, aeq1 }
    2cc9:	625be4        	ae_sq32f.iu	aeq1, a11, -8
    2ccc:	624a24        	ae_sq32f.iu	aeq0, a10, 8
    2ccf:	6249e4        	ae_sq32f.iu	aeq0, a9, -8

00002cd2 <imdct_long+0x136>:
    2cd2:	f01d           	retw.n

00002cd4 <imdct_short>:
    2cd4:	004136        	entry	a1, 32
    2cd7:	04a252        	movi	a5, 0x204
    2cda:	f8c2b2        	addi	a11, a2, -8
    2cdd:	fcc392        	addi	a9, a3, -4
    2ce0:	084c           	movi.n	a8, 64
    2ce2:	f52141        	l32r	a4, 168 (8098 <mdct_twi2048_L+0xff8>)
    2ce5:	02d2a2        	addmi	a10, a2, 0x200
    2ce8:	aa4b           	addi.n	a10, a10, 4
    2cea:	20c440        	or	a12, a4, a4
    2ced:	1ea876        	loopgtz	a8, 2d0f <imdct_short+0x3b>
    2cf0:	5c8c14        	ae_lp24x2f.iu	aep0, a12, 8
    2cf3:	609b24        	ae_lq32f.iu	aeq2, a11, 8
    2cf6:	fa807d1aef 	{ ae_lq32f.iu	aeq0, a10, -8; ae_mulfq32sp24s.l	aeq1, aeq2, aep0 }
    2cfb:	088d24        	ae_mulfq32sp24s.l	aeq3, aeq0, aep0
    2cfe:	0888a4        	ae_mulsfq32sp24s.h	aeq3, aeq2, aep0
    2d01:	080a24        	ae_mulafq32sp24s.h	aeq1, aeq0, aep0
    2d04:	06b834        	ae_roundsq32sym	aeq2, aeq3
    2d07:	fc4c79e91f 	{ ae_sq32f.iu	aeq2, a9, 4; ae_roundsq32sym	aeq0, aeq1 }
    2d0c:	624914        	ae_sq32f.iu	aeq0, a9, 4

00002d0f <imdct_short+0x3b>:
    2d0f:	0a1c           	movi.n	a10, 16
    2d11:	1b0c           	movi.n	a11, 1
    2d13:	03cd           	mov.n	a12, a3
    2d15:	02dd           	mov.n	a13, a2
    2d17:	f515e1        	l32r	a14, 16c (6ec8 <fft_twi64_L+0x8>)
    2d1a:	0f4c           	movi.n	a15, 64
    2d1c:	ffbee5        	call8	290c <aac_passf4pos>
    2d1f:	4a0c           	movi.n	a10, 4
    2d21:	4b0c           	movi.n	a11, 4
    2d23:	02cd           	mov.n	a12, a2
    2d25:	03dd           	mov.n	a13, a3
    2d27:	f512e1        	l32r	a14, 170 (7048 <fft_twi64_L+0x188>)
    2d2a:	0f4c           	movi.n	a15, 64
    2d2c:	ffbde5        	call8	290c <aac_passf4pos>
    2d2f:	0a1c           	movi.n	a10, 16
    2d31:	03bd           	mov.n	a11, a3
    2d33:	02cd           	mov.n	a12, a2
    2d35:	ffb6a5        	call8	28a0 <aac_passf4pos_ido1>
    2d38:	fcc2b2        	addi	a11, a2, -4
    2d3b:	0e2c           	movi.n	a14, 32
    2d3d:	03d3c2        	addmi	a12, a3, 0x300
    2d40:	01d392        	addmi	a9, a3, 0x100
    2d43:	f8c9a2        	addi	a10, a9, -8
    2d46:	f8ccd2        	addi	a13, a12, -8
    2d49:	994b           	addi.n	a9, a9, 4
    2d4b:	cc4b           	addi.n	a12, a12, 4
    2d4d:	2bae76        	loopgtz	a14, 2d7c <imdct_short+0xa8>
    2d50:	5c9414        	ae_lp24x2f.iu	aep1, a4, 8
    2d53:	60db14        	ae_lq32f.iu	aeq3, a11, 4
    2d56:	0895e4        	ae_mulfq32sp24s.l	aeq2, aeq3, aep1
    2d59:	fb057d1b1f 	{ ae_lq32f.iu	aeq0, a11, 4; ae_mulfq32sp24s.h	aeq3, aeq3, aep1 }
    2d5e:	089b24        	ae_mulafq32sp24s.l	aeq3, aeq0, aep1
    2d61:	089024        	ae_mulsfq32sp24s.h	aeq2, aeq0, aep1
    2d64:	067834        	ae_roundsq32sym	aeq1, aeq3
    2d67:	064924        	ae_negq56	aeq1, aeq1
    2d6a:	fc5479dd2f 	{ ae_sq32f.iu	aeq1, a13, 8; ae_roundsq32sym	aeq0, aeq2 }
    2d6f:	ff4479dcef 	{ ae_sq32f.iu	aeq1, a12, -8; ae_roundsq32sym	aeq3, aeq0 }
    2d74:	ff94b9ca2f 	{ ae_sq32f.iu	aeq0, a10, 8; ae_negq56	aeq3, aeq3 }
    2d79:	6279e4        	ae_sq32f.iu	aeq3, a9, -8
    2d7c:	c35a           	add.n	a12, a3, a5
    2d7e:	f8c3d2        	addi	a13, a3, -8
    2d81:	0e2c           	movi.n	a14, 32
    2d83:	04d392        	addmi	a9, a3, 0x400
    2d86:	02d3a2        	addmi	a10, a3, 0x200
    2d89:	f8caa2        	addi	a10, a10, -8
    2d8c:	994b           	addi.n	a9, a9, 4
    2d8e:	26ae76        	loopgtz	a14, 2db8 <imdct_short+0xe4>
    2d91:	5ca414        	ae_lp24x2f.iu	aep2, a4, 8
    2d94:	60db14        	ae_lq32f.iu	aeq3, a11, 4
    2d97:	082de4        	ae_mulfq32sp24s.l	aeq1, aeq3, aep2
    2d9a:	fb097d1b1f 	{ ae_lq32f.iu	aeq0, a11, 4; ae_mulfq32sp24s.h	aeq3, aeq3, aep2 }
    2d9f:	08ab24        	ae_mulafq32sp24s.l	aeq3, aeq0, aep2
    2da2:	082824        	ae_mulsfq32sp24s.h	aeq1, aeq0, aep2
    2da5:	06b834        	ae_roundsq32sym	aeq2, aeq3

00002da8 <imdct_short+0xd4>:
    2da8:	fe90b9ed2f 	{ ae_sq32f.iu	aeq2, a13, 8; ae_negq56	aeq2, aeq2 }
    2dad:	fc4c79ecef 	{ ae_sq32f.iu	aeq2, a12, -8; ae_roundsq32sym	aeq0, aeq1 }
    2db2:	624a24        	ae_sq32f.iu	aeq0, a10, 8
    2db5:	6249e4        	ae_sq32f.iu	aeq0, a9, -8

00002db8 <imdct_short+0xe4>:
    2db8:	f01d           	retw.n

00002dba <imdct_short+0xe6>:
	...

00002dbc <filter_bank_LC>:
    2dbc:	00a136        	entry	a1, 80
    2dbf:	03bd           	mov.n	a11, a3
    2dc1:	00a282        	movi	a8, 0x200
    2dc4:	05ad           	mov.n	a10, a5
    2dc6:	a09620        	addx4	a9, a6, a2
    2dc9:	d62a           	add.n	a13, a6, a2
    2dcb:	f4eae1        	l32r	a14, 174 (7ba9 <mdct_twi2048_L+0xb09>)
    2dce:	300ee4        	ae_cvtp24a16x2.ll	aep0, a14, a14
    2dd1:	130dd2        	l8ui	a13, a13, 19
    2dd4:	9958           	l32i.n	a5, a9, 36
    2dd6:	fccac2        	addi	a12, a10, -4
    2dd9:	b998           	l32i.n	a9, a9, 44
    2ddb:	006192        	s32i	a9, a1, 0
    2dde:	f4e691        	l32r	a9, 178 (9550 <win_mode>)
    2de1:	10a876        	loopgtz	a8, 2df5 <filter_bank_LC+0x39>
    2de4:	605c14        	ae_lq32f.iu	aeq1, a12, 4
    2de7:	d4e37d1c1f 	{ ae_lq32f.iu	aeq0, a12, 4; ae_mulfq32sp16s.l	aeq1, aeq1, aep0 }
    2dec:	030214        	ae_mulfq32sp16s.l	aeq0, aeq0, aep0
    2def:	621cf4        	ae_sq32f.i	aeq1, a12, -4
    2df2:	620c04        	ae_sq32f.i	aeq0, a12, 0

00002df5 <filter_bank_LC+0x39>:
    2df5:	a2c8           	l32i.n	a12, a2, 40
    2df7:	56cc           	bnez.n	a6, 2e00 <filter_bank_LC+0x44>
    2df9:	9268           	l32i.n	a6, a2, 36
    2dfb:	0c7d           	mov.n	a7, a12
    2dfd:	0001c6        	j	2e08 <filter_bank_LC+0x4c>

00002e00 <filter_bank_LC+0x44>:
    2e00:	0c6d           	mov.n	a6, a12
    2e02:	20d272        	addmi	a7, a2, 0x2000
    2e05:	38c772        	addi	a7, a7, 56

00002e08 <filter_bank_LC+0x4c>:
    2e08:	a03d90        	addx4	a3, a13, a9
    2e0b:	a04490        	addx4	a4, a4, a9
    2e0e:	0488           	l32i.n	a8, a4, 0
    2e10:	1189           	s32i.n	a8, a1, 4
    2e12:	2448           	l32i.n	a4, a4, 8
    2e14:	0388           	l32i.n	a8, a3, 0
    2e16:	2189           	s32i.n	a8, a1, 8
    2e18:	2338           	l32i.n	a3, a3, 8
    2e1a:	2f9b16        	beqz	a11, 3117 <filter_bank_LC+0x35b>
    2e1d:	020c           	movi.n	a2, 0
    2e1f:	eb0b           	addi.n	a14, a11, -1
    2e21:	376e16        	beqz	a14, 319b <filter_bank_LC+0x3df>
    2e24:	fdcbf2        	addi	a15, a11, -3
    2e27:	450f16        	beqz	a15, 327b <filter_bank_LC+0x4bf>
    2e2a:	31a9           	s32i.n	a10, a1, 12
    2e2c:	fecb82        	addi	a8, a11, -2
    2e2f:	365856        	bnez	a8, 3198 <filter_bank_LC+0x3dc>
    2e32:	06bd           	mov.n	a11, a6
    2e34:	ffe9e5        	call8	2cd4 <imdct_short>
    2e37:	31a8           	l32i.n	a10, a1, 12
    2e39:	04d6b2        	addmi	a11, a6, 0x400
    2e3c:	02daa2        	addmi	a10, a10, 0x200
    2e3f:	ffe965        	call8	2cd4 <imdct_short>
    2e42:	31a8           	l32i.n	a10, a1, 12
    2e44:	08d6b2        	addmi	a11, a6, 0x800
    2e47:	04daa2        	addmi	a10, a10, 0x400
    2e4a:	ffe8a5        	call8	2cd4 <imdct_short>
    2e4d:	31a8           	l32i.n	a10, a1, 12
    2e4f:	0cd6b2        	addmi	a11, a6, 0xc00
    2e52:	06daa2        	addmi	a10, a10, 0x600
    2e55:	ffe7e5        	call8	2cd4 <imdct_short>
    2e58:	31a8           	l32i.n	a10, a1, 12
    2e5a:	07bd           	mov.n	a11, a7
    2e5c:	08daa2        	addmi	a10, a10, 0x800
    2e5f:	ffe765        	call8	2cd4 <imdct_short>
    2e62:	31a8           	l32i.n	a10, a1, 12
    2e64:	04d7b2        	addmi	a11, a7, 0x400
    2e67:	0adaa2        	addmi	a10, a10, 0xa00
    2e6a:	ffe6a5        	call8	2cd4 <imdct_short>
    2e6d:	31a8           	l32i.n	a10, a1, 12
    2e6f:	08d7b2        	addmi	a11, a7, 0x800
    2e72:	0cdaa2        	addmi	a10, a10, 0xc00
    2e75:	ffe5e5        	call8	2cd4 <imdct_short>
    2e78:	31a8           	l32i.n	a10, a1, 12
    2e7a:	0cd7b2        	addmi	a11, a7, 0xc00
    2e7d:	0edaa2        	addmi	a10, a10, 0xe00
    2e80:	ffe525        	call8	2cd4 <imdct_short>
    2e83:	032192        	l32i	a9, a1, 12
    2e86:	00a4b2        	movi	a11, 0x400
    2e89:	06ab76        	loopgtz	a11, 2e93 <filter_bank_LC+0xd7>
    2e8c:	06c8           	l32i.n	a12, a6, 0
    2e8e:	09e9c4        	ae_s32ip	a12, a9, 4
    2e91:	664b           	addi.n	a6, a6, 4

00002e93 <filter_bank_LC+0xd7>:
    2e93:	209550        	or	a9, a5, a5
    2e96:	404010        	ssai	16
    2e99:	0021b2        	l32i	a11, a1, 0
    2e9c:	80a082        	movi	a8, 128
    2e9f:	c0a1d2        	movi	a13, 0x1c0
    2ea2:	ffdbb2        	addmi	a11, a11, 0xffffff00
    2ea5:	07ad76        	loopgtz	a13, 2eb0 <filter_bank_LC+0xf4>
    2ea8:	402bc2        	l32i	a12, a11, 0x100
    2eab:	09e9c4        	ae_s32ip	a12, a9, 4
    2eae:	bb4b           	addi.n	a11, a11, 4
    2eb0:	3198           	l32i.n	a9, a1, 12
    2eb2:	01c8           	l32i.n	a12, a1, 0
    2eb4:	0ad5d2        	addmi	a13, a5, 0xa00
    2eb7:	dd4b           	addi.n	a13, a13, 4
    2eb9:	01ddd2        	addmi	a13, a13, 0x100
    2ebc:	fccdd2        	addi	a13, a13, -4
    2ebf:	0adcf2        	addmi	a15, a12, 0xa00
    2ec2:	01d9e2        	addmi	a14, a9, 0x100
    2ec5:	09d962        	addmi	a6, a9, 0x900
    2ec8:	05d9a2        	addmi	a10, a9, 0x500
    2ecb:	aa4b           	addi.n	a10, a10, 4
    2ecd:	84c662        	addi	a6, a6, -124
    2ed0:	ee4b           	addi.n	a14, a14, 4
    2ed2:	ff4b           	addi.n	a15, a15, 4
    2ed4:	51d9           	s32i.n	a13, a1, 20
    2ed6:	09dcc2        	addmi	a12, a12, 0x900
    2ed9:	04dd           	mov.n	a13, a4
    2edb:	0cbd           	mov.n	a11, a12
    2edd:	41f9           	s32i.n	a15, a1, 16
    2edf:	61e9           	s32i.n	a14, a1, 24
    2ee1:	07d5f2        	addmi	a15, a5, 0x700
    2ee4:	04ed           	mov.n	a14, a4
    2ee6:	c1a876        	loopgtz	a8, 2fab <filter_bank_LC+0x1ef>
    2ee9:	b1c9           	s32i.n	a12, a1, 44
    2eeb:	7129           	s32i.n	a2, a1, 28
    2eed:	8149           	s32i.n	a4, a1, 32
    2eef:	9159           	s32i.n	a5, a1, 36
    2ef1:	a179           	s32i.n	a7, a1, 40
    2ef3:	6158           	l32i.n	a5, a1, 24
    2ef5:	7f9d72        	l16si	a7, a13, 254
    2ef8:	2e4d           	ae_l16si.n	a4, a14, 0
    2efa:	3f2522        	l32i	a2, a5, 252
    2efd:	bf25c2        	l32i	a12, a5, 0x2fc
    2f00:	844d           	ae_sext16	a4, a4
    2f02:	554b           	addi.n	a5, a5, 4
    2f04:	6159           	s32i.n	a5, a1, 24
    2f06:	825c40        	mull	a5, a12, a4
    2f09:	b2cc40        	mulsh	a12, a12, a4
    2f0c:	877d           	ae_sext16	a7, a7
    2f0e:	81cc50        	src	a12, a12, a5
    2f11:	825270        	mull	a5, a2, a7
    2f14:	b22270        	mulsh	a2, a2, a7
    2f17:	812250        	src	a2, a2, a5
    2f1a:	0b58           	l32i.n	a5, a11, 0
    2f1c:	902250        	addx2	a2, a2, a5
    2f1f:	90cc20        	addx2	a12, a12, a2
    2f22:	806fc2        	s32i	a12, a15, 0x200
    2f25:	bf2a82        	l32i	a8, a10, 0x2fc
    2f28:	3f2a22        	l32i	a2, a10, 252
    2f2b:	82c840        	mull	a12, a8, a4
    2f2e:	825270        	mull	a5, a2, a7
    2f31:	b28840        	mulsh	a8, a8, a4
    2f34:	b22270        	mulsh	a2, a2, a7
    2f37:	8188c0        	src	a8, a8, a12
    2f3a:	41c8           	l32i.n	a12, a1, 16
    2f3c:	fecdd2        	addi	a13, a13, -2
    2f3f:	812250        	src	a2, a2, a5
    2f42:	cc4b           	addi.n	a12, a12, 4
    2f44:	3e2c52        	l32i	a5, a12, 248
    2f47:	ee2b           	addi.n	a14, a14, 2
    2f49:	aa4b           	addi.n	a10, a10, 4
    2f4b:	902250        	addx2	a2, a2, a5
    2f4e:	5158           	l32i.n	a5, a1, 20
    2f50:	41c9           	s32i.n	a12, a1, 16
    2f52:	908820        	addx2	a8, a8, a2
    2f55:	09e584        	ae_s32ip	a8, a5, 4
    2f58:	5f2682        	l32i	a8, a6, 0x17c
    2f5b:	5159           	s32i.n	a5, a1, 20
    2f5d:	be2cc2        	l32i	a12, a12, 0x2f8
    2f60:	822870        	mull	a2, a8, a7
    2f63:	b28870        	mulsh	a8, a8, a7
    2f66:	df2672        	l32i	a7, a6, 0x37c
    2f69:	818820        	src	a8, a8, a2
    2f6c:	664b           	addi.n	a6, a6, 4
    2f6e:	822740        	mull	a2, a7, a4
    2f71:	9088c0        	addx2	a8, a8, a12
    2f74:	b27740        	mulsh	a7, a7, a4
    2f77:	234d           	ae_l16si.n	a4, a3, 0
    2f79:	332b           	addi.n	a3, a3, 2
    2f7b:	817720        	src	a7, a7, a2
    2f7e:	907780        	addx2	a7, a7, a8
    2f81:	844d           	ae_sext16	a4, a4
    2f83:	7f6572        	s32i	a7, a5, 0x1fc
    2f86:	09c8           	l32i.n	a12, a9, 0
    2f88:	9158           	l32i.n	a5, a1, 36
    2f8a:	a178           	l32i.n	a7, a1, 40
    2f8c:	822c40        	mull	a2, a12, a4
    2f8f:	b2cc40        	mulsh	a12, a12, a4
    2f92:	994b           	addi.n	a9, a9, 4
    2f94:	81cc20        	src	a12, a12, a2
    2f97:	fddb22        	addmi	a2, a11, 0xfffffd00
    2f9a:	402222        	l32i	a2, a2, 0x100
    2f9d:	8148           	l32i.n	a4, a1, 32
    2f9f:	bb4b           	addi.n	a11, a11, 4
    2fa1:	90cc20        	addx2	a12, a12, a2
    2fa4:	09efc4        	ae_s32ip	a12, a15, 4
    2fa7:	7128           	l32i.n	a2, a1, 28
    2fa9:	b1c8           	l32i.n	a12, a1, 44
    2fab:	0fd5b2        	addmi	a11, a5, 0xf00
    2fae:	079d           	mov.n	a9, a7
    2fb0:	404010        	ssai	16
    2fb3:	04dd           	mov.n	a13, a4
    2fb5:	04ed           	mov.n	a14, a4
    2fb7:	31f8           	l32i.n	a15, a1, 12
    2fb9:	0138           	l32i.n	a3, a1, 0
    2fbb:	084c           	movi.n	a8, 64
    2fbd:	0cdff2        	addmi	a15, a15, 0xc00
    2fc0:	0dd332        	addmi	a3, a3, 0xd00
    2fc3:	334b           	addi.n	a3, a3, 4
    2fc5:	ff4b           	addi.n	a15, a15, 4
    2fc7:	36a876        	loopgtz	a8, 3001 <filter_bank_LC+0x245>
    2fca:	0968           	l32i.n	a6, a9, 0
    2fcc:	7f9d52        	l16si	a5, a13, 254
    2fcf:	7f2f82        	l32i	a8, a15, 0x1fc
    2fd2:	fecdd2        	addi	a13, a13, -2
    2fd5:	855d           	ae_sext16	a5, a5
    2fd7:	82a850        	mull	a10, a8, a5
    2fda:	b28850        	mulsh	a8, a8, a5
    2fdd:	2e5d           	ae_l16si.n	a5, a14, 0
    2fdf:	ff4b           	addi.n	a15, a15, 4
    2fe1:	8188a0        	src	a8, a8, a10
    2fe4:	855d           	ae_sext16	a5, a5
    2fe6:	82a650        	mull	a10, a6, a5
    2fe9:	b26650        	mulsh	a6, a6, a5
    2fec:	994b           	addi.n	a9, a9, 4
    2fee:	8166a0        	src	a6, a6, a10
    2ff1:	7f23a2        	l32i	a10, a3, 0x1fc
    2ff4:	ee2b           	addi.n	a14, a14, 2
    2ff6:	334b           	addi.n	a3, a3, 4
    2ff8:	9088a0        	addx2	a8, a8, a10
    2ffb:	906680        	addx2	a6, a6, a8
    2ffe:	09eb64        	ae_s32ip	a6, a11, 4
    3001:	01b8           	l32i.n	a11, a1, 0
    3003:	01d792        	addmi	a9, a7, 0x100
    3006:	80c4d2        	addi	a13, a4, -128
    3009:	404010        	ssai	16
    300c:	084c           	movi.n	a8, 64
    300e:	0321f2        	l32i	a15, a1, 12
    3011:	01d4e2        	addmi	a14, a4, 0x100
    3014:	80cee2        	addi	a14, a14, -128
    3017:	0ddff2        	addmi	a15, a15, 0xd00
    301a:	04cff2        	addi	a15, a15, 4
    301d:	31a876        	loopgtz	a8, 3052 <filter_bank_LC+0x296>
    3020:	7f9da2        	l16si	a10, a13, 254
    3023:	0938           	l32i.n	a3, a9, 0
    3025:	2e6d           	ae_l16si.n	a6, a14, 0
    3027:	7f2f52        	l32i	a5, a15, 0x1fc
    302a:	fecdd2        	addi	a13, a13, -2
    302d:	ff4b           	addi.n	a15, a15, 4
    302f:	ee2b           	addi.n	a14, a14, 2
    3031:	994b           	addi.n	a9, a9, 4
    3033:	866d           	ae_sext16	a6, a6
    3035:	828360        	mull	a8, a3, a6
    3038:	8aad           	ae_sext16	a10, a10
    303a:	b23360        	mulsh	a3, a3, a6
    303d:	8265a0        	mull	a6, a5, a10
    3040:	b255a0        	mulsh	a5, a5, a10
    3043:	813380        	src	a3, a3, a8
    3046:	815560        	src	a5, a5, a6
    3049:	1155f0        	slli	a5, a5, 1
    304c:	903350        	addx2	a3, a3, a5
    304f:	09eb34        	ae_s32ip	a3, a11, 4
    3052:	04dd           	mov.n	a13, a4
    3054:	404010        	ssai	16
    3057:	80a082        	movi	a8, 128
    305a:	01f8           	l32i.n	a15, a1, 0
    305c:	0bd7b2        	addmi	a11, a7, 0xb00
    305f:	07d7e2        	addmi	a14, a7, 0x700
    3062:	04d7a2        	addmi	a10, a7, 0x400
    3065:	84caa2        	addi	a10, a10, -124
    3068:	ee4b           	addi.n	a14, a14, 4
    306a:	bb4b           	addi.n	a11, a11, 4
    306c:	04df92        	addmi	a9, a15, 0x400
    306f:	01dff2        	addmi	a15, a15, 0x100
    3072:	994b           	addi.n	a9, a9, 4
    3074:	01d992        	addmi	a9, a9, 0x100
    3077:	fcc992        	addi	a9, a9, -4
    307a:	8ea876        	loopgtz	a8, 310c <filter_bank_LC+0x350>
    307d:	7129           	s32i.n	a2, a1, 28
    307f:	242d           	ae_l16si.n	a2, a4, 0
    3081:	1f2a62        	l32i	a6, a10, 124
    3084:	822d           	ae_sext16	a2, a2
    3086:	828620        	mull	a8, a6, a2
    3089:	b26620        	mulsh	a6, a6, a2
    308c:	7f9d52        	l16si	a5, a13, 254
    308f:	816680        	src	a6, a6, a8
    3092:	802782        	l32i	a8, a7, 0x200
    3095:	855d           	ae_sext16	a5, a5
    3097:	823850        	mull	a3, a8, a5
    309a:	b28850        	mulsh	a8, a8, a5
    309d:	818830        	src	a8, a8, a3
    30a0:	1188f0        	slli	a8, a8, 1
    30a3:	906680        	addx2	a6, a6, a8
    30a6:	09ef64        	ae_s32ip	a6, a15, 4
    30a9:	3f2e32        	l32i	a3, a14, 252
    30ac:	826320        	mull	a6, a3, a2
    30af:	b23320        	mulsh	a3, a3, a2
    30b2:	813360        	src	a3, a3, a6
    30b5:	9f2a62        	l32i	a6, a10, 0x27c
    30b8:	828650        	mull	a8, a6, a5
    30bb:	b26650        	mulsh	a6, a6, a5
    30be:	816680        	src	a6, a6, a8
    30c1:	1166f0        	slli	a6, a6, 1
    30c4:	903360        	addx2	a3, a3, a6
    30c7:	7f6f32        	s32i	a3, a15, 0x1fc
    30ca:	3f2b62        	l32i	a6, a11, 252
    30cd:	828620        	mull	a8, a6, a2
    30d0:	b26620        	mulsh	a6, a6, a2
    30d3:	816680        	src	a6, a6, a8
    30d6:	bf2e82        	l32i	a8, a14, 0x2fc
    30d9:	fecdd2        	addi	a13, a13, -2
    30dc:	442b           	addi.n	a4, a4, 2
    30de:	822850        	mull	a2, a8, a5
    30e1:	b28850        	mulsh	a8, a8, a5
    30e4:	774b           	addi.n	a7, a7, 4
    30e6:	818820        	src	a8, a8, a2
    30e9:	1188f0        	slli	a8, a8, 1
    30ec:	906680        	addx2	a6, a6, a8
    30ef:	09e964        	ae_s32ip	a6, a9, 4
    30f2:	bf2b22        	l32i	a2, a11, 0x2fc
    30f5:	aa4b           	addi.n	a10, a10, 4
    30f7:	ee4b           	addi.n	a14, a14, 4
    30f9:	823250        	mull	a3, a2, a5
    30fc:	b22250        	mulsh	a2, a2, a5
    30ff:	bb4b           	addi.n	a11, a11, 4
    3101:	812230        	src	a2, a2, a3
    3104:	1122f0        	slli	a2, a2, 1
    3107:	7f6922        	s32i	a2, a9, 0x1fc
    310a:	7128           	l32i.n	a2, a1, 28
    310c:	c0a182        	movi	a8, 0x1c0
    310f:	02a876        	loopgtz	a8, 3115 <filter_bank_LC+0x359>
    3112:	09ec24        	ae_s32ip	a2, a12, 4
    3115:	f01d           	retw.n

00003117 <filter_bank_LC+0x35b>:
    3117:	06bd           	mov.n	a11, a6
    3119:	07cd           	mov.n	a12, a7
    311b:	ffa825        	call8	2b9c <imdct_long>
    311e:	fcc6c2        	addi	a12, a6, -4
    3121:	fcc5a2        	addi	a10, a5, -4
    3124:	fcc792        	addi	a9, a7, -4
    3127:	00a1f2        	movi	a15, 0x100
    312a:	21e8           	l32i.n	a14, a1, 8
    312c:	11d8           	l32i.n	a13, a1, 4
    312e:	01b8           	l32i.n	a11, a1, 0
    3130:	fccee2        	addi	a14, a14, -4
    3133:	08ddd2        	addmi	a13, a13, 0x800
    3136:	fccbb2        	addi	a11, a11, -4
    3139:	5baf76        	loopgtz	a15, 3198 <filter_bank_LC+0x3dc>
    313c:	44adf4        	ae_lp16x2f.iu	aep2, a13, -4
    313f:	44be14        	ae_lp16x2f.iu	aep3, a14, 4
    3142:	605c14        	ae_lq32f.iu	aeq1, a12, 4
    3145:	601b14        	ae_lq32f.iu	aeq0, a11, 4
    3148:	f40fbd791f 	{ ae_lq32f.iu	aeq3, a9, 4; ae_mulafq32sp16s.h	aeq0, aeq1, aep3 }
    314d:	03aad4        	ae_mulfq32sp16s.l	aeq3, aeq3, aep2
    3150:	624a14        	ae_sq32f.iu	aeq0, a10, 4
    3153:	623b04        	ae_sq32f.i	aeq3, a11, 0
    3156:	609c14        	ae_lq32f.iu	aeq2, a12, 4
    3159:	605b14        	ae_lq32f.iu	aeq1, a11, 4
    315c:	f68efd191f 	{ ae_lq32f.iu	aeq0, a9, 4; ae_mulafq32sp16s.l	aeq1, aeq2, aep3 }
    3161:	032204        	ae_mulfq32sp16s.h	aeq0, aeq0, aep2
    3164:	625a14        	ae_sq32f.iu	aeq1, a10, 4
    3167:	620b04        	ae_sq32f.i	aeq0, a11, 0
    316a:	448df4        	ae_lp16x2f.iu	aep0, a13, -4
    316d:	449e14        	ae_lp16x2f.iu	aep1, a14, 4
    3170:	60dc14        	ae_lq32f.iu	aeq3, a12, 4
    3173:	609b14        	ae_lq32f.iu	aeq2, a11, 4
    3176:	f747bd391f 	{ ae_lq32f.iu	aeq1, a9, 4; ae_mulafq32sp24s.h	aeq2, aeq3, aep1 }
    317b:	030a54        	ae_mulfq32sp16s.l	aeq1, aeq1, aep0
    317e:	626a14        	ae_sq32f.iu	aeq2, a10, 4
    3181:	621b04        	ae_sq32f.i	aeq1, a11, 0
    3184:	601c14        	ae_lq32f.iu	aeq0, a12, 4
    3187:	60db14        	ae_lq32f.iu	aeq3, a11, 4
    318a:	f5c6fd591f 	{ ae_lq32f.iu	aeq2, a9, 4; ae_mulafq32sp24s.l	aeq3, aeq0, aep1 }
    318f:	038284        	ae_mulfq32sp16s.h	aeq2, aeq2, aep0
    3192:	627a14        	ae_sq32f.iu	aeq3, a10, 4
    3195:	622b04        	ae_sq32f.i	aeq2, a11, 0

00003198 <filter_bank_LC+0x3dc>:
    3198:	f01d           	retw.n

0000319a <filter_bank_LC+0x3de>:
	...

0000319b <filter_bank_LC+0x3df>:
    319b:	06bd           	mov.n	a11, a6
    319d:	07cd           	mov.n	a12, a7
    319f:	ff9fe5        	call8	2b9c <imdct_long>
    31a2:	2198           	l32i.n	a9, a1, 8
    31a4:	0138           	l32i.n	a3, a1, 0
    31a6:	404010        	ssai	16
    31a9:	00a1f2        	movi	a15, 0x100
    31ac:	ffd332        	addmi	a3, a3, 0xffffff00
    31af:	03bd           	mov.n	a11, a3
    31b1:	68af76        	loopgtz	a15, 321d <filter_bank_LC+0x461>
    31b4:	0399d2        	l16si	a13, a9, 6
    31b7:	0299a2        	l16si	a10, a9, 4
    31ba:	298d           	ae_l16si.n	a8, a9, 0
    31bc:	06c8           	l32i.n	a12, a6, 0
    31be:	402be2        	l32i	a14, a11, 0x100
    31c1:	888d           	ae_sext16	a8, a8
    31c3:	82fc80        	mull	a15, a12, a8
    31c6:	b2cc80        	mulsh	a12, a12, a8
    31c9:	398d           	ae_l16si.n	a8, a9, 2
    31cb:	81ccf0        	src	a12, a12, a15
    31ce:	90cce0        	addx2	a12, a12, a14
    31d1:	05c9           	s32i.n	a12, a5, 0
    31d3:	16c8           	l32i.n	a12, a6, 4
    31d5:	888d           	ae_sext16	a8, a8
    31d7:	412be2        	l32i	a14, a11, 0x104
    31da:	82fc80        	mull	a15, a12, a8
    31dd:	b2cc80        	mulsh	a12, a12, a8
    31e0:	10cbb2        	addi	a11, a11, 16
    31e3:	81ccf0        	src	a12, a12, a15
    31e6:	90cce0        	addx2	a12, a12, a14
    31e9:	15c9           	s32i.n	a12, a5, 4
    31eb:	26e8           	l32i.n	a14, a6, 8
    31ed:	8aad           	ae_sext16	a10, a10
    31ef:	3e2bf2        	l32i	a15, a11, 248
    31f2:	828ea0        	mull	a8, a14, a10
    31f5:	b2eea0        	mulsh	a14, a14, a10
    31f8:	8ddd           	ae_sext16	a13, a13
    31fa:	81ee80        	src	a14, a14, a8
    31fd:	90eef0        	addx2	a14, a14, a15
    3200:	25e9           	s32i.n	a14, a5, 8
    3202:	3688           	l32i.n	a8, a6, 12
    3204:	998b           	addi.n	a9, a9, 8
    3206:	3f2ba2        	l32i	a10, a11, 252
    3209:	82c8d0        	mull	a12, a8, a13
    320c:	b288d0        	mulsh	a8, a8, a13
    320f:	10c662        	addi	a6, a6, 16
    3212:	8188c0        	src	a8, a8, a12
    3215:	9088a0        	addx2	a8, a8, a10
    3218:	3589           	s32i.n	a8, a5, 12
    321a:	10c552        	addi	a5, a5, 16

0000321d <filter_bank_LC+0x461>:
    321d:	01d3b2        	addmi	a11, a3, 0x100
    3220:	079d           	mov.n	a9, a7
    3222:	c0a1a2        	movi	a10, 0x1c0
    3225:	06aa76        	loopgtz	a10, 322f <filter_bank_LC+0x473>
    3228:	09c8           	l32i.n	a12, a9, 0
    322a:	09ebc4        	ae_s32ip	a12, a11, 4
    322d:	994b           	addi.n	a9, a9, 4
    322f:	404010        	ssai	16
    3232:	80a0d2        	movi	a13, 128
    3235:	0198           	l32i.n	a9, a1, 0
    3237:	04d7a2        	addmi	a10, a7, 0x400
    323a:	84caa2        	addi	a10, a10, -124
    323d:	04d992        	addmi	a9, a9, 0x400
    3240:	04c992        	addi	a9, a9, 4
    3243:	03d992        	addmi	a9, a9, 0x300
    3246:	fcc992        	addi	a9, a9, -4
    3249:	1bad76        	loopgtz	a13, 3268 <filter_bank_LC+0x4ac>
    324c:	7f94d2        	l16si	a13, a4, 254
    324f:	df2ab2        	l32i	a11, a10, 0x37c
    3252:	fec442        	addi	a4, a4, -2
    3255:	8ddd           	ae_sext16	a13, a13
    3257:	82cbd0        	mull	a12, a11, a13
    325a:	b2bbd0        	mulsh	a11, a11, a13
    325d:	aa4b           	addi.n	a10, a10, 4
    325f:	81bbc0        	src	a11, a11, a12
    3262:	11bbf0        	slli	a11, a11, 1
    3265:	09e9b4        	ae_s32ip	a11, a9, 4
    3268:	0021b2        	l32i	a11, a1, 0
    326b:	c0a1e2        	movi	a14, 0x1c0
    326e:	09dbb2        	addmi	a11, a11, 0x900
    3271:	02ae76        	loopgtz	a14, 3277 <filter_bank_LC+0x4bb>
    3274:	09eb24        	ae_s32ip	a2, a11, 4
    3277:	f01d           	retw.n

00003279 <filter_bank_LC+0x4bd>:
	...

0000327b <filter_bank_LC+0x4bf>:
    327b:	04a542        	movi	a4, 0x504
    327e:	06bd           	mov.n	a11, a6
    3280:	07cd           	mov.n	a12, a7
    3282:	ff91a5        	call8	2b9c <imdct_long>
    3285:	0128           	l32i.n	a2, a1, 0
    3287:	059d           	mov.n	a9, a5
    3289:	c0a1c2        	movi	a12, 0x1c0
    328c:	ffd222        	addmi	a2, a2, 0xffffff00
    328f:	02bd           	mov.n	a11, a2
    3291:	07ac76        	loopgtz	a12, 329c <filter_bank_LC+0x4e0>
    3294:	402b82        	l32i	a8, a11, 0x100
    3297:	09e984        	ae_s32ip	a8, a9, 4
    329a:	bb4b           	addi.n	a11, a11, 4

0000329c <filter_bank_LC+0x4e0>:
    329c:	b64a           	add.n	a11, a6, a4
    329e:	404010        	ssai	16
    32a1:	80a0c2        	movi	a12, 128
    32a4:	0198           	l32i.n	a9, a1, 0
    32a6:	07d5a2        	addmi	a10, a5, 0x700
    32a9:	0afd           	mov.n	a15, a10
    32ab:	04d992        	addmi	a9, a9, 0x400
    32ae:	04c992        	addi	a9, a9, 4
    32b1:	1eac76        	loopgtz	a12, 32d3 <filter_bank_LC+0x517>
    32b4:	238d           	ae_l16si.n	a8, a3, 0
    32b6:	7f2bc2        	l32i	a12, a11, 0x1fc
    32b9:	bf29d2        	l32i	a13, a9, 0x2fc
    32bc:	bb4b           	addi.n	a11, a11, 4
    32be:	994b           	addi.n	a9, a9, 4
    32c0:	888d           	ae_sext16	a8, a8
    32c2:	82ec80        	mull	a14, a12, a8
    32c5:	b2cc80        	mulsh	a12, a12, a8
    32c8:	332b           	addi.n	a3, a3, 2
    32ca:	81cce0        	src	a12, a12, a14
    32cd:	90ccd0        	addx2	a12, a12, a13
    32d0:	09efc4        	ae_s32ip	a12, a15, 4
    32d3:	02daa2        	addmi	a10, a10, 0x200
    32d6:	c0a1c2        	movi	a12, 0x1c0
    32d9:	0021b2        	l32i	a11, a1, 0
    32dc:	09d692        	addmi	a9, a6, 0x900
    32df:	84c992        	addi	a9, a9, -124
    32e2:	09dbb2        	addmi	a11, a11, 0x900
    32e5:	0dac76        	loopgtz	a12, 32f6 <filter_bank_LC+0x53a>
    32e8:	0bc8           	l32i.n	a12, a11, 0
    32ea:	1f29d2        	l32i	a13, a9, 124
    32ed:	994b           	addi.n	a9, a9, 4
    32ef:	bb4b           	addi.n	a11, a11, 4
    32f1:	ccda           	add.n	a12, a12, a13
    32f3:	09eac4        	ae_s32ip	a12, a10, 4
    32f6:	01d2b2        	addmi	a11, a2, 0x100
    32f9:	012192        	l32i	a9, a1, 4
    32fc:	404010        	ssai	16
    32ff:	00a4e2        	movi	a14, 0x400
    3302:	07d992        	addmi	a9, a9, 0x700
    3305:	1aae76        	loopgtz	a14, 3323 <filter_bank_LC+0x567>
    3308:	7f99d2        	l16si	a13, a9, 254
    330b:	07a8           	l32i.n	a10, a7, 0
    330d:	fec992        	addi	a9, a9, -2
    3310:	8ddd           	ae_sext16	a13, a13
    3312:	82cad0        	mull	a12, a10, a13
    3315:	b2aad0        	mulsh	a10, a10, a13
    3318:	774b           	addi.n	a7, a7, 4
    331a:	81aac0        	src	a10, a10, a12
    331d:	11aaf0        	slli	a10, a10, 1
    3320:	09eba4        	ae_s32ip	a10, a11, 4
    3323:	f01d           	retw.n

00003325 <filter_bank_LC+0x569>:
    3325:	000000                                        ...

00003328 <_start>:
    3328:	004136        	entry	a1, 32
    332b:	f395a1        	l32r	a10, 180 (40e0 <_DYNAMIC+0x820>)
    332e:	f39331        	l32r	a3, 17c (957c <printf_ptr>)
    3331:	0288           	l32i.n	a8, a2, 0
    3333:	f394b1        	l32r	a11, 184 (230 <AIR_AAC_init>)
    3336:	0389           	s32i.n	a8, a3, 0
    3338:	0008e0        	callx8	a8
    333b:	0388           	l32i.n	a8, a3, 0
    333d:	f392a1        	l32r	a10, 188 (40f4 <_DYNAMIC+0x834>)
    3340:	f393b1        	l32r	a11, 18c (298 <AIR_AAC_decoder>)
    3343:	0008e0        	callx8	a8
    3346:	0388           	l32i.n	a8, a3, 0
    3348:	f392a1        	l32r	a10, 190 (410c <_DYNAMIC+0x84c>)
    334b:	f392b1        	l32r	a11, 194 (2f4 <get_AACdec_memsize>)
    334e:	0008e0        	callx8	a8
    3351:	f39121        	l32r	a2, 198 (94d0 <export_parameter_array>)
    3354:	f01d           	retw.n

00003356 <_start+0x2e>:
	...

00003358 <is_intensity>:
    3358:	004136        	entry	a1, 32
    335b:	f30c           	movi.n	a3, 15
    335d:	0b1237        	beq	a2, a3, 336c <is_intensity+0x14>
    3360:	f2c282        	addi	a8, a2, -14
    3363:	f47c           	movi.n	a4, -1
    3365:	020c           	movi.n	a2, 0
    3367:	832480        	moveqz	a2, a4, a8
    336a:	f01d           	retw.n

0000336c <is_intensity+0x14>:
    336c:	120c           	movi.n	a2, 1
    336e:	f01d           	retw.n

00003370 <window_grouping_info>:
    3370:	004136        	entry	a1, 32
    3373:	1b0c           	movi.n	a11, 1
    3375:	010352        	l8ui	a5, a3, 1
    3378:	080262        	l8ui	a6, a2, 8
    337b:	10c382        	addi	a8, a3, 16
    337e:	0f2516        	beqz	a5, 3474 <window_grouping_info+0x104>
    3381:	450b           	addi.n	a4, a5, -1
    3383:	0ed416        	beqz	a4, 3474 <window_grouping_info+0x104>
    3386:	fdc592        	addi	a9, a5, -3
    3389:	0e7916        	beqz	a9, 3474 <window_grouping_info+0x104>
    338c:	fec5a2        	addi	a10, a5, -2
    338f:	086a56        	bnez	a10, 3419 <window_grouping_info+0xa9>
    3392:	f382a1        	l32r	a10, 19c (4590 <num_swb_128_window>)
    3395:	f382e1        	l32r	a14, 1a0 (94f0 <swb_offset_128_window>)
    3398:	8c0c           	movi.n	a12, 8
    339a:	a6aa           	add.n	a10, a6, a10
    339c:	a0e6e0        	addx4	a14, a6, a14
    339f:	0ee8           	l32i.n	a14, a14, 0
    33a1:	0643b2        	s8i	a11, a3, 6
    33a4:	000aa2        	l8ui	a10, a10, 0
    33a7:	0543a2        	s8i	a10, a3, 5
    33aa:	0743c2        	s8i	a12, a3, 7
    33ad:	0843b2        	s8i	a11, a3, 8
    33b0:	4a9c           	beqz.n	a10, 33c8 <window_grouping_info+0x58>
    33b2:	0e6d           	mov.n	a6, a14
    33b4:	01d392        	addmi	a9, a3, 0x100
    33b7:	070c           	movi.n	a7, 0

000033b9 <window_grouping_info+0x49>:
    33b9:	46cd           	ae_l16ui.n	a12, a6, 0
    33bb:	09c9c4        	ae_s16ip	a12, a9, 2
    33be:	0503a2        	l8ui	a10, a3, 5
    33c1:	771b           	addi.n	a7, a7, 1
    33c3:	662b           	addi.n	a6, a6, 2
    33c5:	f027a7        	blt	a7, a10, 33b9 <window_grouping_info+0x49>

000033c8 <window_grouping_info+0x58>:
    33c8:	0212f2        	l16ui	a15, a2, 4
    33cb:	904a30        	addx2	a4, a10, a3
    33ce:	41f3f0        	srli	a15, a15, 3
    33d1:	8054f2        	s16i	a15, a4, 0x100
    33d4:	0703d2        	l8ui	a13, a3, 7
    33d7:	dd0b           	addi.n	a13, a13, -1
    33d9:	521da6        	blti	a13, 1, 342f <window_grouping_info+0xbf>
    33dc:	070c           	movi.n	a7, 0
    33de:	690c           	movi.n	a9, 6

000033e0 <window_grouping_info+0x70>:
    33e0:	c05970        	sub	a5, a9, a7
    33e3:	030342        	l8ui	a4, a3, 3
    33e6:	400500        	ssr	a5
    33e9:	060352        	l8ui	a5, a3, 6
    33ec:	b14040        	sra	a4, a4
    33ef:	0fe407        	bbsi	a4, 0, 3402 <window_grouping_info+0x92>
    33f2:	a51b           	addi.n	a10, a5, 1
    33f4:	74a0a0        	extui	a10, a10, 0, 8
    33f7:	0643a2        	s8i	a10, a3, 6
    33fa:	aa3a           	add.n	a10, a10, a3
    33fc:	074ab2        	s8i	a11, a10, 7
    33ff:	000246        	j	340c <window_grouping_info+0x9c>

00003402 <window_grouping_info+0x92>:
    3402:	d53a           	add.n	a13, a5, a3
    3404:	070dc2        	l8ui	a12, a13, 7
    3407:	cc1b           	addi.n	a12, a12, 1
    3409:	074dc2        	s8i	a12, a13, 7

0000340c <window_grouping_info+0x9c>:
    340c:	0703d2        	l8ui	a13, a3, 7
    340f:	771b           	addi.n	a7, a7, 1
    3411:	dd0b           	addi.n	a13, a13, -1
    3413:	c927d7        	blt	a7, a13, 33e0 <window_grouping_info+0x70>
    3416:	000546        	j	342f <window_grouping_info+0xbf>

00003419 <window_grouping_info+0xa9>:
    3419:	0b2d           	mov.n	a2, a11
    341b:	f01d           	retw.n

0000341d <window_grouping_info+0xad>:
    341d:	090c           	movi.n	a9, 0

0000341f <window_grouping_info+0xaf>:
    341f:	689d           	ae_s16i.n	a9, a8, 0
    3421:	0603f2        	l8ui	a15, a3, 6
    3424:	cc1b           	addi.n	a12, a12, 1
    3426:	882b           	addi.n	a8, a8, 2
    3428:	0d2df7        	blt	a13, a15, 3439 <window_grouping_info+0xc9>

0000342b <window_grouping_info+0xbb>:
    342b:	020c           	movi.n	a2, 0
    342d:	f01d           	retw.n

0000342f <window_grouping_info+0xbf>:
    342f:	060342        	l8ui	a4, a3, 6
    3432:	f514a6        	blti	a4, 1, 342b <window_grouping_info+0xbb>
    3435:	03cd           	mov.n	a12, a3
    3437:	0d0c           	movi.n	a13, 0

00003439 <window_grouping_info+0xc9>:
    3439:	0503a2        	l8ui	a10, a3, 5
    343c:	dd1b           	addi.n	a13, a13, 1
    343e:	0e6d           	mov.n	a6, a14
    3440:	d91aa6        	blti	a10, 1, 341d <window_grouping_info+0xad>
    3443:	070c           	movi.n	a7, 0
    3445:	090c           	movi.n	a9, 0

00003447 <window_grouping_info+0xd7>:
    3447:	46fd           	ae_l16ui.n	a15, a6, 0
    3449:	080c52        	l8ui	a5, a12, 8
    344c:	b71b           	addi.n	a11, a7, 1
    344e:	1d9ba7        	bne	a11, a10, 346f <window_grouping_info+0xff>
    3451:	021272        	l16ui	a7, a2, 4
    3454:	417370        	srli	a7, a7, 3

00003457 <window_grouping_info+0xe7>:
    3457:	662b           	addi.n	a6, a6, 2
    3459:	09c894        	ae_s16ip	a9, a8, 2
    345c:	c047f0        	sub	a4, a7, a15
    345f:	824450        	mull	a4, a4, a5
    3462:	0503a2        	l8ui	a10, a3, 5
    3465:	0b7d           	mov.n	a7, a11
    3467:	994a           	add.n	a9, a9, a4
    3469:	da2ba7        	blt	a11, a10, 3447 <window_grouping_info+0xd7>
    346c:	ffebc6        	j	341f <window_grouping_info+0xaf>

0000346f <window_grouping_info+0xff>:
    346f:	567d           	ae_l16ui.n	a7, a6, 2
    3471:	fff886        	j	3457 <window_grouping_info+0xe7>

00003474 <window_grouping_info+0x104>:
    3474:	f34c91        	l32r	a9, 1a4 (4580 <num_swb_1024_window>)
    3477:	f34ca1        	l32r	a10, 1a8 (9520 <swb_offset_1024_window>)
    347a:	969a           	add.n	a9, a6, a9
    347c:	a066a0        	addx4	a6, a6, a10
    347f:	0668           	l32i.n	a6, a6, 0
    3481:	0643b2        	s8i	a11, a3, 6
    3484:	000992        	l8ui	a9, a9, 0
    3487:	0743b2        	s8i	a11, a3, 7
    348a:	0843b2        	s8i	a11, a3, 8
    348d:	054392        	s8i	a9, a3, 5
    3490:	1819a6        	blti	a9, 1, 34ac <window_grouping_info+0x13c>
    3493:	01d392        	addmi	a9, a3, 0x100
    3496:	070c           	movi.n	a7, 0

00003498 <window_grouping_info+0x128>:
    3498:	46cd           	ae_l16ui.n	a12, a6, 0
    349a:	09c8c4        	ae_s16ip	a12, a8, 2
    349d:	46bd           	ae_l16ui.n	a11, a6, 0
    349f:	09c9b4        	ae_s16ip	a11, a9, 2
    34a2:	0503a2        	l8ui	a10, a3, 5
    34a5:	771b           	addi.n	a7, a7, 1
    34a7:	662b           	addi.n	a6, a6, 2
    34a9:	eb27a7        	blt	a7, a10, 3498 <window_grouping_info+0x128>

000034ac <window_grouping_info+0x13c>:
    34ac:	0212f2        	l16ui	a15, a2, 4
    34af:	68fd           	ae_s16i.n	a15, a8, 0
    34b1:	0503e2        	l8ui	a14, a3, 5
    34b4:	0212d2        	l16ui	a13, a2, 4
    34b7:	90ee30        	addx2	a14, a14, a3
    34ba:	805ed2        	s16i	a13, a14, 0x100
    34bd:	ffda86        	j	342b <window_grouping_info+0xbb>

000034c0 <xt_memset1>:
    34c0:	004136        	entry	a1, 32
    34c3:	f03d           	nop.n
    34c5:	049476        	loopnez	a4, 34cd <xt_memset1+0xd>
    34c8:	004232        	s8i	a3, a2, 0
    34cb:	221b           	addi.n	a2, a2, 1

000034cd <xt_memset1+0xd>:
    34cd:	f01d           	retw.n

000034cf <xt_memset1+0xf>:
	...

000034d0 <xt_memset8_0>:
    34d0:	004136        	entry	a1, 32
    34d3:	fc208f822e 	{ addi	a2, a2, -8; ae_zerop48	aep0 }
    34d8:	029376        	loopnez	a3, 34de <xt_memset8_0+0xe>
    34db:	5e8214        	ae_sp24x2f.iu	aep0, a2, 8

000034de <xt_memset8_0+0xe>:
    34de:	f01d           	retw.n

000034e0 <xt_memcpy4>:
    34e0:	004136        	entry	a1, 32
    34e3:	069476        	loopnez	a4, 34ed <xt_memcpy4+0xd>
    34e6:	0358           	l32i.n	a5, a3, 0
    34e8:	09e254        	ae_s32ip	a5, a2, 4
    34eb:	334b           	addi.n	a3, a3, 4

000034ed <xt_memcpy4+0xd>:
    34ed:	f01d           	retw.n

000034ef <xt_memcpy4+0xf>:
	...

000034f0 <is_decode>:
    34f0:	00c136        	entry	a1, 96
    34f3:	0129           	s32i.n	a2, a1, 0
    34f5:	a139           	s32i.n	a3, a1, 40
    34f7:	0603b2        	l8ui	a11, a3, 6
    34fa:	04d3c2        	addmi	a12, a3, 0x400
    34fd:	01d3d2        	addmi	a13, a3, 0x100
    3500:	68cdd2        	addi	a13, a13, 104
    3503:	c0ccc2        	addi	a12, a12, -64
    3506:	c1c9           	s32i.n	a12, a1, 48
    3508:	31d9           	s32i.n	a13, a1, 12
    350a:	04d2c2        	addmi	a12, a2, 0x400
    350d:	41cc82        	addi	a8, a12, 65
    3510:	4189           	s32i.n	a8, a1, 16
    3512:	14fb16        	beqz	a11, 3665 <is_decode+0x175>
    3515:	8149           	s32i.n	a4, a1, 32
    3517:	7159           	s32i.n	a5, a1, 28
    3519:	880c           	movi.n	a8, 8
    351b:	0e0c           	movi.n	a14, 0
    351d:	0f0c           	movi.n	a15, 0
    351f:	c0ccd2        	addi	a13, a12, -64
    3522:	91d9           	s32i.n	a13, a1, 36
    3524:	0003c2        	l8ui	a12, a3, 0
    3527:	11f9           	s32i.n	a15, a1, 4
    3529:	b1e9           	s32i.n	a14, a1, 44
    352b:	d28680        	quos	a8, a6, a8
    352e:	5189           	s32i.n	a8, a1, 20

00003530 <is_decode+0x40>:
    3530:	a1d8           	l32i.n	a13, a1, 40
    3532:	11a8           	l32i.n	a10, a1, 4
    3534:	aada           	add.n	a10, a10, a13
    3536:	21a9           	s32i.n	a10, a1, 8
    3538:	080aa2        	l8ui	a10, a10, 8
    353b:	102a16        	beqz	a10, 3641 <is_decode+0x151>
    353e:	0e0c           	movi.n	a14, 0
    3540:	61e9           	s32i.n	a14, a1, 24

00003542 <is_decode+0x52>:
    3542:	021ce6        	bgei	a12, 1, 3548 <is_decode+0x58>
    3545:	0037c6        	j	3628 <is_decode+0x138>
    3548:	a158           	l32i.n	a5, a1, 40
    354a:	3128           	l32i.n	a2, a1, 12
    354c:	41b8           	l32i.n	a11, a1, 16
    354e:	040c           	movi.n	a4, 0

00003550 <is_decode+0x60>:
    3550:	c1a8           	l32i.n	a10, a1, 48
    3552:	f1b9           	s32i.n	a11, a1, 60
    3554:	0f0c           	movi.n	a15, 0
    3556:	a4aa           	add.n	a10, a4, a10
    3558:	000aa2        	l8ui	a10, a10, 0
    355b:	190c           	movi.n	a9, 1
    355d:	080c           	movi.n	a8, 0
    355f:	f2cad2        	addi	a13, a10, -14
    3562:	f1cae2        	addi	a14, a10, -15
    3565:	8389e0        	moveqz	a8, a9, a14
    3568:	83f9d0        	moveqz	a15, a9, a13
    356b:	20ff80        	or	a15, a15, a8
    356e:	0a3f16        	beqz	a15, 3615 <is_decode+0x125>
    3571:	7198           	l32i.n	a9, a1, 28
    3573:	8138           	l32i.n	a3, a1, 32
    3575:	801582        	l16ui	a8, a5, 0x100
    3578:	22bd           	ae_l16si.n	a11, a2, 0
    357a:	811562        	l16ui	a6, a5, 0x102
    357d:	b178           	l32i.n	a7, a1, 44
    357f:	21c2b0        	srai	a12, a11, 2
    3582:	c06680        	sub	a6, a6, a8
    3585:	14b0b0        	extui	a11, a11, 0, 2
    3588:	d1c9           	s32i.n	a12, a1, 52
    358a:	787a           	add.n	a7, a8, a7
    358c:	f308c1        	l32r	a12, 1ac (4150 <pow05_table_m>)
    358f:	a03730        	addx4	a3, a7, a3
    3592:	a07790        	addx4	a7, a7, a9
    3595:	a0bbc0        	addx4	a11, a11, a12
    3598:	0bb8           	l32i.n	a11, a11, 0
    359a:	e1b9           	s32i.n	a11, a1, 56
    359c:	ffdba5        	call8	3358 <is_intensity>
    359f:	91e8           	l32i.n	a14, a1, 36
    35a1:	e1c8           	l32i.n	a12, a1, 56
    35a3:	800ee2        	l8ui	a14, a14, 128
    35a6:	f1b8           	l32i.n	a11, a1, 60
    35a8:	d1d8           	l32i.n	a13, a1, 52
    35aa:	0d1e66        	bnei	a14, 1, 35bb <is_decode+0xcb>
    35ad:	000bf2        	l8ui	a15, a11, 0
    35b0:	1e0c           	movi.n	a14, 1
    35b2:	11fff0        	slli	a15, a15, 1
    35b5:	c0eef0        	sub	a14, a14, a15
    35b8:	000046        	j	35bd <is_decode+0xcd>

000035bb <is_decode+0xcb>:
    35bb:	1e0c           	movi.n	a14, 1

000035bd <is_decode+0xcd>:
    35bd:	021ea7        	beq	a14, a10, 35c3 <is_decode+0xd3>
    35c0:	60c0c0        	neg	a12, a12

000035c3 <is_decode+0xd3>:
    35c3:	271de6        	bgei	a13, 1, 35ee <is_decode+0xfe>
    35c6:	8c9d           	ae_sext16	a9, a12
    35c8:	1c0c           	movi.n	a12, 1
    35ca:	c0ccd0        	sub	a12, a12, a13

000035cd <is_decode+0xdd>:
    35cd:	660b           	addi.n	a6, a6, -1
    35cf:	03d8           	l32i.n	a13, a3, 0
    35d1:	401c00        	ssl	a12
    35d4:	334b           	addi.n	a3, a3, 4
    35d6:	a1dd00        	sll	a13, a13
    35d9:	82e9d0        	mull	a14, a9, a13
    35dc:	b2d9d0        	mulsh	a13, a9, a13
    35df:	404010        	ssai	16
    35e2:	81dde0        	src	a13, a13, a14
    35e5:	09e7d4        	ae_s32ip	a13, a7, 4
    35e8:	e116e6        	bgei	a6, 1, 35cd <is_decode+0xdd>
    35eb:	000846        	j	3610 <is_decode+0x120>

000035ee <is_decode+0xfe>:
    35ee:	8c9d           	ae_sext16	a9, a12
    35f0:	ad0b           	addi.n	a10, a13, -1

000035f2 <is_decode+0x102>:
    35f2:	660b           	addi.n	a6, a6, -1
    35f4:	03e8           	l32i.n	a14, a3, 0
    35f6:	400a00        	ssr	a10
    35f9:	334b           	addi.n	a3, a3, 4
    35fb:	b1e0e0        	sra	a14, a14
    35fe:	82fe90        	mull	a15, a14, a9
    3601:	b2ee90        	mulsh	a14, a14, a9
    3604:	404010        	ssai	16
    3607:	81eef0        	src	a14, a14, a15
    360a:	09e7e4        	ae_s32ip	a14, a7, 4
    360d:	e116e6        	bgei	a6, 1, 35f2 <is_decode+0x102>

00003610 <is_decode+0x120>:
    3610:	a1c8           	l32i.n	a12, a1, 40
    3612:	000cc2        	l8ui	a12, a12, 0

00003615 <is_decode+0x125>:
    3615:	bb1b           	addi.n	a11, a11, 1
    3617:	552b           	addi.n	a5, a5, 2
    3619:	222b           	addi.n	a2, a2, 2
    361b:	441b           	addi.n	a4, a4, 1
    361d:	02a4c7        	bge	a4, a12, 3623 <is_decode+0x133>
    3620:	ffcb06        	j	3550 <is_decode+0x60>
    3623:	21a8           	l32i.n	a10, a1, 8
    3625:	080aa2        	l8ui	a10, a10, 8

00003628 <is_decode+0x138>:
    3628:	61b8           	l32i.n	a11, a1, 24
    362a:	51e8           	l32i.n	a14, a1, 20
    362c:	b1d8           	l32i.n	a13, a1, 44
    362e:	bb1b           	addi.n	a11, a11, 1
    3630:	61b9           	s32i.n	a11, a1, 24
    3632:	ddea           	add.n	a13, a13, a14
    3634:	b1d9           	s32i.n	a13, a1, 44
    3636:	02aba7        	bge	a11, a10, 363c <is_decode+0x14c>
    3639:	ffc146        	j	3542 <is_decode+0x52>
    363c:	a1b8           	l32i.n	a11, a1, 40
    363e:	060bb2        	l8ui	a11, a11, 6

00003641 <is_decode+0x151>:
    3641:	41e8           	l32i.n	a14, a1, 16
    3643:	01f8           	l32i.n	a15, a1, 0
    3645:	11d8           	l32i.n	a13, a1, 4
    3647:	3188           	l32i.n	a8, a1, 12
    3649:	000ff2        	l8ui	a15, a15, 0
    364c:	dd1b           	addi.n	a13, a13, 1
    364e:	908c80        	addx2	a8, a12, a8
    3651:	3189           	s32i.n	a8, a1, 12
    3653:	11d9           	s32i.n	a13, a1, 4
    3655:	c188           	l32i.n	a8, a1, 48
    3657:	eefa           	add.n	a14, a14, a15
    3659:	41e9           	s32i.n	a14, a1, 16
    365b:	88ca           	add.n	a8, a8, a12
    365d:	c189           	s32i.n	a8, a1, 48
    365f:	02adb7        	bge	a13, a11, 3665 <is_decode+0x175>
    3662:	ffb286        	j	3530 <is_decode+0x40>

00003665 <is_decode+0x175>:
    3665:	f01d           	retw.n

00003667 <is_decode+0x177>:
	...

00003668 <__do_global_ctors_aux>:
    3668:	004136        	entry	a1, 32
    366b:	f2d181        	l32r	a8, 1b0 (92b4 <kbd_win_long+0x714>)
    366e:	7f2882        	l32i	a8, a8, 0x1fc
    3671:	f2d021        	l32r	a2, 1b4 (94b0 <__CTOR_LIST___47>)
    3674:	0c0826        	beqi	a8, -1, 3684 <__do_global_ctors_aux+0x1c>
    3677:	02a8           	l32i.n	a10, a2, 0

00003679 <__do_global_ctors_aux+0x11>:
    3679:	000ae0        	callx8	a10
    367c:	fcc222        	addi	a2, a2, -4
    367f:	02a8           	l32i.n	a10, a2, 0
    3681:	f40a66        	bnei	a10, -1, 3679 <__do_global_ctors_aux+0x11>

00003684 <__do_global_ctors_aux+0x1c>:
    3684:	f01d           	retw.n

00003686 <__do_global_ctors_aux+0x1e>:
	...

00003688 <__udivdi3>:
    3688:	008136        	entry	a1, 64
    368b:	049d           	mov.n	a9, a4
    368d:	02ad           	mov.n	a10, a2
    368f:	038d           	mov.n	a8, a3
    3691:	0b2c           	movi.n	a11, 32
    3693:	09b516        	beqz	a5, 3732 <__udivdi3+0xaa>
    3696:	06b357        	bgeu	a3, a5, 36a0 <__udivdi3+0x18>
    3699:	020c           	movi.n	a2, 0
    369b:	030c           	movi.n	a3, 0
    369d:	f01d           	retw.n

0000369f <__udivdi3+0x17>:
	...

000036a0 <__udivdi3+0x18>:
    36a0:	40f540        	nsau	a4, a5
    36a3:	1d4416        	beqz	a4, 387b <__udivdi3+0x1f3>
    36a6:	c02b40        	sub	a2, a11, a4
    36a9:	400200        	ssr	a2
    36ac:	91f030        	srl	a15, a3
    36af:	401400        	ssl	a4
    36b2:	81c3a0        	src	a12, a3, a10
    36b5:	81b590        	src	a11, a5, a9
    36b8:	9b8d           	ae_zext16	a8, a11
    36ba:	f5d0b0        	extui	a13, a11, 16, 16
    36bd:	404010        	ssai	16
    36c0:	c2efd0        	quou	a14, a15, a13
    36c3:	0e5d           	mov.n	a5, a14
    36c5:	8238e0        	mull	a3, a8, a14
    36c8:	e2ffd0        	remu	a15, a15, a13
    36cb:	81ffc0        	src	a15, a15, a12
    36ce:	0f2d           	mov.n	a2, a15
    36d0:	0ebf37        	bgeu	a15, a3, 36e2 <__udivdi3+0x5a>
    36d3:	5e0b           	addi.n	a5, a14, -1
    36d5:	2bfa           	add.n	a2, a11, a15
    36d7:	0732b7        	bltu	a2, a11, 36e2 <__udivdi3+0x5a>
    36da:	04b237        	bgeu	a2, a3, 36e2 <__udivdi3+0x5a>
    36dd:	2b2a           	add.n	a2, a11, a2
    36df:	fece52        	addi	a5, a14, -2
    36e2:	116500        	slli	a6, a5, 16
    36e5:	401400        	ssl	a4
    36e8:	c03230        	sub	a3, a2, a3
    36eb:	c2e3d0        	quou	a14, a3, a13
    36ee:	0e2d           	mov.n	a2, a14
    36f0:	a15900        	sll	a5, a9
    36f3:	8288e0        	mull	a8, a8, a14
    36f6:	e233d0        	remu	a3, a3, a13
    36f9:	0bf3c0        	depbits	a12, a3, 16, 16
    36fc:	0c3d           	mov.n	a3, a12
    36fe:	0ebc87        	bgeu	a12, a8, 3710 <__udivdi3+0x88>
    3701:	2e0b           	addi.n	a2, a14, -1
    3703:	3bca           	add.n	a3, a11, a12
    3705:	0733b7        	bltu	a3, a11, 3710 <__udivdi3+0x88>
    3708:	04b387        	bgeu	a3, a8, 3710 <__udivdi3+0x88>
    370b:	3b3a           	add.n	a3, a11, a3
    370d:	fece22        	addi	a2, a14, -2
    3710:	c03380        	sub	a3, a3, a8
    3713:	202260        	or	a2, a2, a6
    3716:	828250        	mull	a8, a2, a5
    3719:	a25250        	muluh	a5, a2, a5
    371c:	0b3357        	bltu	a3, a5, 372b <__udivdi3+0xa3>
    371f:	c09350        	sub	a9, a3, a5
    3722:	07f956        	bnez	a9, 37a5 <__udivdi3+0x11d>
    3725:	a1ba00        	sll	a11, a10
    3728:	79bb87        	bgeu	a11, a8, 37a5 <__udivdi3+0x11d>

0000372b <__udivdi3+0xa3>:
    372b:	220b           	addi.n	a2, a2, -1
    372d:	030c           	movi.n	a3, 0
    372f:	f01d           	retw.n

00003731 <__udivdi3+0xa9>:
	...

00003732 <__udivdi3+0xaa>:
    3732:	75b347        	bgeu	a3, a4, 37ab <__udivdi3+0x123>
    3735:	40f450        	nsau	a5, a4
    3738:	a58c           	beqz.n	a5, 3746 <__udivdi3+0xbe>
    373a:	401500        	ssl	a5
    373d:	a14400        	sll	a4, a4
    3740:	813320        	src	a3, a3, a2
    3743:	a12200        	sll	a2, a2

00003746 <__udivdi3+0xbe>:
    3746:	945d           	ae_zext16	a5, a4
    3748:	404010        	ssai	16
    374b:	f58040        	extui	a8, a4, 16, 16
    374e:	e2c380        	remu	a12, a3, a8
    3751:	c2b380        	quou	a11, a3, a8
    3754:	0bad           	mov.n	a10, a11
    3756:	81cc20        	src	a12, a12, a2
    3759:	8295b0        	mull	a9, a5, a11
    375c:	0c3d           	mov.n	a3, a12
    375e:	0ebc97        	bgeu	a12, a9, 3770 <__udivdi3+0xe8>
    3761:	ab0b           	addi.n	a10, a11, -1
    3763:	3c4a           	add.n	a3, a12, a4
    3765:	073347        	bltu	a3, a4, 3770 <__udivdi3+0xe8>
    3768:	04b397        	bgeu	a3, a9, 3770 <__udivdi3+0xe8>
    376b:	343a           	add.n	a3, a4, a3
    376d:	fecba2        	addi	a10, a11, -2
    3770:	c0d390        	sub	a13, a3, a9
    3773:	c29d80        	quou	a9, a13, a8
    3776:	093d           	mov.n	a3, a9
    3778:	825590        	mull	a5, a5, a9
    377b:	e2dd80        	remu	a13, a13, a8
    377e:	0bfd20        	depbits	a2, a13, 16, 16
    3781:	16b257        	bgeu	a2, a5, 379b <__udivdi3+0x113>
    3784:	390b           	addi.n	a3, a9, -1
    3786:	224a           	add.n	a2, a2, a4
    3788:	0f3247        	bltu	a2, a4, 379b <__udivdi3+0x113>
    378b:	0cb257        	bgeu	a2, a5, 379b <__udivdi3+0x113>
    378e:	112a00        	slli	a2, a10, 16
    3791:	fec932        	addi	a3, a9, -2
    3794:	202320        	or	a2, a3, a2
    3797:	030c           	movi.n	a3, 0
    3799:	f01d           	retw.n

0000379b <__udivdi3+0x113>:
    379b:	112a00        	slli	a2, a10, 16
    379e:	202320        	or	a2, a3, a2
    37a1:	030c           	movi.n	a3, 0
    37a3:	f01d           	retw.n

000037a5 <__udivdi3+0x11d>:
    37a5:	030c           	movi.n	a3, 0
    37a7:	f01d           	retw.n

000037a9 <__udivdi3+0x121>:
	...

000037ab <__udivdi3+0x123>:
    37ab:	130c           	movi.n	a3, 1
    37ad:	c24340        	quou	a4, a3, a4
    37b0:	839490        	moveqz	a9, a4, a9
    37b3:	094d           	mov.n	a4, a9
    37b5:	40f950        	nsau	a5, a9
    37b8:	0dc516        	beqz	a5, 3898 <__udivdi3+0x210>
    37bb:	401500        	ssl	a5
    37be:	c0eb50        	sub	a14, a11, a5
    37c1:	a14900        	sll	a4, a9
    37c4:	400e00        	ssr	a14
    37c7:	91d080        	srl	a13, a8
    37ca:	401500        	ssl	a5
    37cd:	813820        	src	a3, a8, a2
    37d0:	945d           	ae_zext16	a5, a4
    37d2:	a12200        	sll	a2, a2
    37d5:	f58040        	extui	a8, a4, 16, 16
    37d8:	404010        	ssai	16
    37db:	c2cd80        	quou	a12, a13, a8
    37de:	0cbd           	mov.n	a11, a12
    37e0:	82a5c0        	mull	a10, a5, a12
    37e3:	e2dd80        	remu	a13, a13, a8
    37e6:	81dd30        	src	a13, a13, a3
    37e9:	0d9d           	mov.n	a9, a13
    37eb:	0ebda7        	bgeu	a13, a10, 37fd <__udivdi3+0x175>
    37ee:	bc0b           	addi.n	a11, a12, -1
    37f0:	9d4a           	add.n	a9, a13, a4
    37f2:	073947        	bltu	a9, a4, 37fd <__udivdi3+0x175>
    37f5:	04b9a7        	bgeu	a9, a10, 37fd <__udivdi3+0x175>
    37f8:	949a           	add.n	a9, a4, a9
    37fa:	feccb2        	addi	a11, a12, -2
    37fd:	c0d9a0        	sub	a13, a9, a10
    3800:	e2ed80        	remu	a14, a13, a8
    3803:	0bfe30        	depbits	a3, a14, 16, 16
    3806:	c2dd80        	quou	a13, a13, a8
    3809:	0dad           	mov.n	a10, a13
    380b:	82c5d0        	mull	a12, a5, a13
    380e:	039d           	mov.n	a9, a3
    3810:	0eb3c7        	bgeu	a3, a12, 3822 <__udivdi3+0x19a>
    3813:	ad0b           	addi.n	a10, a13, -1
    3815:	934a           	add.n	a9, a3, a4
    3817:	073947        	bltu	a9, a4, 3822 <__udivdi3+0x19a>
    381a:	04b9c7        	bgeu	a9, a12, 3822 <__udivdi3+0x19a>
    381d:	949a           	add.n	a9, a4, a9
    381f:	fecda2        	addi	a10, a13, -2
    3822:	c039c0        	sub	a3, a9, a12
    3825:	11fb00        	slli	a15, a11, 16
    3828:	20aaf0        	or	a10, a10, a15

0000382b <__udivdi3+0x1a3>:
    382b:	404010        	ssai	16
    382e:	e2d380        	remu	a13, a3, a8
    3831:	c2c380        	quou	a12, a3, a8
    3834:	0cbd           	mov.n	a11, a12
    3836:	8295c0        	mull	a9, a5, a12
    3839:	81dd20        	src	a13, a13, a2
    383c:	0d3d           	mov.n	a3, a13
    383e:	0ebd97        	bgeu	a13, a9, 3850 <__udivdi3+0x1c8>
    3841:	bc0b           	addi.n	a11, a12, -1
    3843:	3d4a           	add.n	a3, a13, a4
    3845:	073347        	bltu	a3, a4, 3850 <__udivdi3+0x1c8>
    3848:	04b397        	bgeu	a3, a9, 3850 <__udivdi3+0x1c8>
    384b:	343a           	add.n	a3, a4, a3
    384d:	feccb2        	addi	a11, a12, -2
    3850:	c0e390        	sub	a14, a3, a9
    3853:	c29e80        	quou	a9, a14, a8
    3856:	093d           	mov.n	a3, a9
    3858:	825590        	mull	a5, a5, a9
    385b:	e2ee80        	remu	a14, a14, a8
    385e:	0bfe20        	depbits	a2, a14, 16, 16
    3861:	23b257        	bgeu	a2, a5, 3888 <__udivdi3+0x200>
    3864:	390b           	addi.n	a3, a9, -1
    3866:	224a           	add.n	a2, a2, a4
    3868:	1c3247        	bltu	a2, a4, 3888 <__udivdi3+0x200>
    386b:	19b257        	bgeu	a2, a5, 3888 <__udivdi3+0x200>
    386e:	113b00        	slli	a3, a11, 16
    3871:	fec922        	addi	a2, a9, -2
    3874:	202230        	or	a2, a2, a3
    3877:	0a3d           	mov.n	a3, a10
    3879:	f01d           	retw.n

0000387b <__udivdi3+0x1f3>:
    387b:	023537        	bltu	a5, a3, 3881 <__udivdi3+0x1f9>
    387e:	103297        	bltu	a2, a9, 3892 <__udivdi3+0x20a>

00003881 <__udivdi3+0x1f9>:
    3881:	120c           	movi.n	a2, 1
    3883:	030c           	movi.n	a3, 0
    3885:	f01d           	retw.n

00003887 <__udivdi3+0x1ff>:
	...

00003888 <__udivdi3+0x200>:
    3888:	112b00        	slli	a2, a11, 16
    388b:	202320        	or	a2, a3, a2
    388e:	0a3d           	mov.n	a3, a10
    3890:	f01d           	retw.n

00003892 <__udivdi3+0x20a>:
    3892:	020c           	movi.n	a2, 0
    3894:	030c           	movi.n	a3, 0
    3896:	f01d           	retw.n

00003898 <__udivdi3+0x210>:
    3898:	03ad           	mov.n	a10, a3
    389a:	995d           	ae_zext16	a5, a9
    389c:	c03890        	sub	a3, a8, a9
    389f:	f58090        	extui	a8, a9, 16, 16
    38a2:	ffe146        	j	382b <__udivdi3+0x1a3>

000038a5 <__udivdi3+0x21d>:
    38a5:	000000                                        ...

000038a8 <_fini>:
    38a8:	008136        	entry	a1, 64
    38ab:	f24381        	l32r	a8, 1b8 (1d8 <__do_global_dtors_aux>)
    38ae:	f03d           	nop.n
    38b0:	0008e0        	callx8	a8

000038b3 <_fini+0xb>:
    38b3:	f01d           	retw.n
