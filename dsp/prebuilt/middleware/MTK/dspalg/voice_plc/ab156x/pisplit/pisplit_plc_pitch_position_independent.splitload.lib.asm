
Build/lib/pisplit_plc_pitch_position_independent.splitload.lib:     file format elf32-xtensa-le


Disassembly of section .text:

00000000 <_init-0xa4>:
       0:	000000fc 00001438 00001bac 00001ad4     ....8...........
      10:	00000000 00001ab8 00000000 00001bb0     ................
      20:	00001134 00000e24 00000138 00001b00     4...$...8.......
      30:	00001ae0 00008000 00007fff 00001b20     ............ ...
      40:	00000e2c 00001490 00001458 00007ffe     ,.......X.......
      50:	00800000 00001198 000013a4 7fffffff     ................
      60:	00000e00 00010000 00001128 00004000     ........(....@..
      70:	00001999 00001b30 00001bc8 00001a54     ....0.......T...
      80:	00001308 00001a70 00000118 00001a8c     ....p...........
      90:	00000360 00001ba0 000018c0 00001abc     `...............
      a0:	000000c0                                ....

000000a4 <_init>:
      a4:	008136        	entry	a1, 64
      a7:	ffd681        	l32r	a8, 0 (fc <frame_dummy>)
      aa:	f03d           	nop.n
      ac:	0008e0        	callx8	a8
      af:	ffd581        	l32r	a8, 4 (1438 <__do_global_ctors_aux>)
      b2:	f03d           	nop.n
      b4:	0008e0        	callx8	a8
      b7:	f01d           	retw.n

000000b9 <_init+0x15>:
      b9:	00000000 36000000                                .......

000000c0 <__do_global_dtors_aux>:
      c0:	004136        	entry	a1, 32
      c3:	ffd131        	l32r	a3, 8 (1bac <__bss_start>)
      c6:	000382        	l8ui	a8, a3, 0
      c9:	d8ec           	bnez.n	a8, fa <__do_global_dtors_aux+0x3a>
      cb:	ffd021        	l32r	a2, c (1ad4 <p$3519_6_1>)
      ce:	02b8           	l32i.n	a11, a2, 0
      d0:	0ba8           	l32i.n	a10, a11, 0
      d2:	ca8c           	beqz.n	a10, e2 <__do_global_dtors_aux+0x22>

000000d4 <__do_global_dtors_aux+0x14>:
      d4:	9b4b           	addi.n	a9, a11, 4
      d6:	0299           	s32i.n	a9, a2, 0
      d8:	000ae0        	callx8	a10
      db:	02b8           	l32i.n	a11, a2, 0
      dd:	0ba8           	l32i.n	a10, a11, 0
      df:	ff1a56        	bnez	a10, d4 <__do_global_dtors_aux+0x14>

000000e2 <__do_global_dtors_aux+0x22>:
      e2:	ffcba1        	l32r	a10, 10 (0 <_text_start>)
      e5:	0b0c           	movi.n	a11, 0
      e7:	081ab7        	beq	a10, a11, f3 <__do_global_dtors_aux+0x33>
      ea:	ffcaa1        	l32r	a10, 14 (1ab8 <__EH_FRAME_BEGIN___44>)
      ed:	ffc881        	l32r	a8, 10 (0 <_text_start>)
      f0:	0008e0        	callx8	a8

000000f3 <__do_global_dtors_aux+0x33>:
      f3:	1c0c           	movi.n	a12, 1
      f5:	0043c2        	s8i	a12, a3, 0
      f8:	f01d           	retw.n

000000fa <__do_global_dtors_aux+0x3a>:
      fa:	f01d           	retw.n

000000fc <frame_dummy>:
      fc:	004136        	entry	a1, 32
      ff:	ffc681        	l32r	a8, 18 (0 <_text_start>)
     102:	090c           	movi.n	a9, 0
     104:	0b1897        	beq	a8, a9, 113 <frame_dummy+0x17>
     107:	ffc3a1        	l32r	a10, 14 (1ab8 <__EH_FRAME_BEGIN___44>)
     10a:	ffc4b1        	l32r	a11, 1c (1bb0 <object$3538_8_1>)
     10d:	ffc281        	l32r	a8, 18 (0 <_text_start>)
     110:	0008e0        	callx8	a8

00000113 <frame_dummy+0x17>:
     113:	f01d           	retw.n

00000115 <frame_dummy+0x19>:
     115:	000000                                        ...

00000118 <SMART_PITCH_PLC_INI>:
     118:	004136        	entry	a1, 32
     11b:	ffc181        	l32r	a8, 20 (1134 <plc_gainctrl_ini>)
     11e:	02ad           	mov.n	a10, a2
     120:	0008e0        	callx8	a8
     123:	ffc081        	l32r	a8, 24 (e24 <find_pitch_ini>)
     126:	0008e0        	callx8	a8
     129:	ffbf81        	l32r	a8, 28 (138 <cvsd_a2plc_ini>)
     12c:	02bd           	mov.n	a11, a2
     12e:	03ad           	mov.n	a10, a3
     130:	0008e0        	callx8	a8
     133:	f01d           	retw.n

00000135 <SMART_PITCH_PLC_INI+0x1d>:
     135:	000000                                        ...

00000138 <cvsd_a2plc_ini>:
     138:	004136        	entry	a1, 32
     13b:	68a692        	movi	a9, 0x668
     13e:	872c           	movi.n	a7, 40
     140:	3a0c           	movi.n	a10, 3
     142:	1c0c           	movi.n	a12, 1
     144:	4b2342        	l32i	a4, a3, 0x12c
     147:	3a2352        	l32i	a5, a3, 232
     14a:	392362        	l32i	a6, a3, 228
     14d:	e369           	s32i.n	a6, a3, 56
     14f:	f359           	s32i.n	a5, a3, 60
     151:	060c           	movi.n	a6, 0
     153:	a0a052        	movi	a5, 160
     156:	06d416        	beqz	a4, 1c7 <cvsd_a2plc_ini+0x8f>
     159:	93a9           	s32i.n	a10, a3, 36
     15b:	3d6362        	s32i	a6, a3, 244
     15e:	43c9           	s32i.n	a12, a3, 16
     160:	8379           	s32i.n	a7, a3, 32
     162:	6359           	s32i.n	a5, a3, 24
     164:	081c           	movi.n	a8, 16
     166:	243c           	movi.n	a4, 50
     168:	e6a0d2        	movi	a13, 230
     16b:	6e4c           	movi.n	a14, 70
     16d:	08a2b2        	movi	a11, 0x208
     170:	6ea0f2        	movi	a15, 110
     173:	73f9           	s32i.n	a15, a3, 28
     175:	a3b9           	s32i.n	a11, a3, 40
     177:	b3e9           	s32i.n	a14, a3, 44
     179:	c3d9           	s32i.n	a13, a3, 48
     17b:	5349           	s32i.n	a4, a3, 20
     17d:	1389           	s32i.n	a8, a3, 4
     17f:	0389           	s32i.n	a8, a3, 0
     181:	3389           	s32i.n	a8, a3, 12
     183:	c53c           	movi.n	a5, 60
     185:	026352        	s32i	a5, a3, 8
     188:	01d382        	addmi	a8, a3, 0x100
     18b:	14c882        	addi	a8, a8, 20
     18e:	205880        	or	a5, a8, a8
     191:	0aab76        	loopgtz	a11, 19f <cvsd_a2plc_ini+0x67>
     194:	01d5a2        	addmi	a10, a5, 0x100
     197:	1c4a62        	s8i	a6, a10, 28
     19a:	1d4a62        	s8i	a6, a10, 29
     19d:	552b           	addi.n	a5, a5, 2

0000019f <cvsd_a2plc_ini+0x67>:
     19f:	83b8           	l32i.n	a11, a3, 32
     1a1:	417860        	srli	a7, a6, 8
     1a4:	539a           	add.n	a5, a3, a9
     1a6:	07ab76        	loopgtz	a11, 1b1 <cvsd_a2plc_ini+0x79>
     1a9:	004562        	s8i	a6, a5, 0
     1ac:	014572        	s8i	a7, a5, 1
     1af:	552b           	addi.n	a5, a5, 2
     1b1:	a3d8           	l32i.n	a13, a3, 40
     1b3:	805390        	add	a5, a3, a9
     1b6:	417860        	srli	a7, a6, 8
     1b9:	07ad76        	loopgtz	a13, 1c4 <cvsd_a2plc_ini+0x8c>
     1bc:	784562        	s8i	a6, a5, 120
     1bf:	794572        	s8i	a7, a5, 121
     1c2:	552b           	addi.n	a5, a5, 2
     1c4:	001986        	j	22e <cvsd_a2plc_ini+0xf6>

000001c7 <cvsd_a2plc_ini+0x8f>:
     1c7:	43c9           	s32i.n	a12, a3, 16
     1c9:	5379           	s32i.n	a7, a3, 20
     1cb:	b379           	s32i.n	a7, a3, 44
     1cd:	c359           	s32i.n	a5, a3, 48
     1cf:	78a0e2        	movi	a14, 120
     1d2:	01d382        	addmi	a8, a3, 0x100
     1d5:	eb1c           	movi.n	a11, 30
     1d7:	0d5c           	movi.n	a13, 80
     1d9:	6f1c           	movi.n	a15, 22
     1db:	840c           	movi.n	a4, 8
     1dd:	0349           	s32i.n	a4, a3, 0
     1df:	23f9           	s32i.n	a15, a3, 8
     1e1:	73d9           	s32i.n	a13, a3, 28
     1e3:	83b9           	s32i.n	a11, a3, 32
     1e5:	14c882        	addi	a8, a8, 20
     1e8:	63e9           	s32i.n	a14, a3, 24
     1ea:	e50c           	movi.n	a5, 14
     1ec:	1359           	s32i.n	a5, a3, 4
     1ee:	3359           	s32i.n	a5, a3, 12
     1f0:	86a1e2        	movi	a14, 0x186
     1f3:	0a63e2        	s32i	a14, a3, 40
     1f6:	205880        	or	a5, a8, a8
     1f9:	0aae76        	loopgtz	a14, 207 <cvsd_a2plc_ini+0xcf>
     1fc:	01d5b2        	addmi	a11, a5, 0x100
     1ff:	1c4b62        	s8i	a6, a11, 28
     202:	1d4b62        	s8i	a6, a11, 29
     205:	552b           	addi.n	a5, a5, 2

00000207 <cvsd_a2plc_ini+0xcf>:
     207:	83d8           	l32i.n	a13, a3, 32
     209:	417860        	srli	a7, a6, 8
     20c:	539a           	add.n	a5, a3, a9
     20e:	07ad76        	loopgtz	a13, 219 <cvsd_a2plc_ini+0xe1>
     211:	004562        	s8i	a6, a5, 0
     214:	014572        	s8i	a7, a5, 1
     217:	552b           	addi.n	a5, a5, 2
     219:	a3e8           	l32i.n	a14, a3, 40
     21b:	805390        	add	a5, a3, a9
     21e:	417860        	srli	a7, a6, 8
     221:	07ae76        	loopgtz	a14, 22c <cvsd_a2plc_ini+0xf4>
     224:	784562        	s8i	a6, a5, 120
     227:	794572        	s8i	a7, a5, 121
     22a:	552b           	addi.n	a5, a5, 2
     22c:	93a9           	s32i.n	a10, a3, 36

0000022e <cvsd_a2plc_ini+0xf6>:
     22e:	085d           	mov.n	a5, a8
     230:	ff7f71        	l32r	a7, 2c (1b00 <rc_win_left_16s>)
     233:	ff7f81        	l32r	a8, 30 (1ae0 <rc_win_right_16s>)
     236:	03f8           	l32i.n	a15, a3, 0
     238:	079d           	mov.n	a9, a7
     23a:	20a880        	or	a10, a8, a8
     23d:	24af76        	loopgtz	a15, 265 <cvsd_a2plc_ini+0x12d>
     240:	0348           	l32i.n	a4, a3, 0
     242:	4cb466        	bnei	a4, 16, 292 <cvsd_a2plc_ini+0x15a>
     245:	2abd           	ae_l16si.n	a11, a10, 0
     247:	fc45b2        	s8i	a11, a5, 252
     24a:	41b8b0        	srli	a11, a11, 8
     24d:	fd45b2        	s8i	a11, a5, 253
     250:	29bd           	ae_l16si.n	a11, a9, 0
     252:	dc45b2        	s8i	a11, a5, 220

00000255 <cvsd_a2plc_ini+0x11d>:
     255:	774b           	addi.n	a7, a7, 4
     257:	884b           	addi.n	a8, a8, 4
     259:	992b           	addi.n	a9, a9, 2
     25b:	41d8b0        	srli	a13, a11, 8
     25e:	aa2b           	addi.n	a10, a10, 2
     260:	dd45d2        	s8i	a13, a5, 221
     263:	552b           	addi.n	a5, a5, 2

00000265 <cvsd_a2plc_ini+0x12d>:
     265:	d369           	s32i.n	a6, a3, 52
     267:	0f1c           	movi.n	a15, 16
     269:	0b1c           	movi.n	a11, 16
     26b:	ff7491        	l32r	a9, 3c (1b20 <zc_diff_size_tab>)
     26e:	e71c           	movi.n	a7, 30
     270:	ff7181        	l32r	a8, 34 (8000 <_end+0x6434>)
     273:	0bd352        	addmi	a5, a3, 0xb00
     276:	18c552        	addi	a5, a5, 24
     279:	106382        	s32i	a8, a3, 64
     27c:	827270        	mull	a7, a2, a7
     27f:	ff6e81        	l32r	a8, 38 (7fff <_end+0x6433>)
     282:	1ec772        	addi	a7, a7, 30
     285:	a09290        	addx4	a9, a2, a9
     288:	1a6372        	s32i	a7, a3, 104
     28b:	471c           	movi.n	a7, 20
     28d:	000506        	j	2a5 <cvsd_a2plc_ini+0x16d>

00000290 <cvsd_a2plc_ini+0x158>:
	...

00000292 <cvsd_a2plc_ini+0x15a>:
     292:	28bd           	ae_l16si.n	a11, a8, 0
     294:	fc45b2        	s8i	a11, a5, 252
     297:	41b8b0        	srli	a11, a11, 8
     29a:	fd45b2        	s8i	a11, a5, 253
     29d:	27bd           	ae_l16si.n	a11, a7, 0
     29f:	dc45b2        	s8i	a11, a5, 220
     2a2:	ffebc6        	j	255 <cvsd_a2plc_ini+0x11d>

000002a5 <cvsd_a2plc_ini+0x16d>:
     2a5:	16a776        	loopgtz	a7, 2bf <cvsd_a2plc_ini+0x187>
     2a8:	41e860        	srli	a14, a6, 8
     2ab:	004562        	s8i	a6, a5, 0
     2ae:	0145e2        	s8i	a14, a5, 1
     2b1:	41e8e0        	srli	a14, a14, 8
     2b4:	0245e2        	s8i	a14, a5, 2
     2b7:	41e8e0        	srli	a14, a14, 8
     2ba:	0345e2        	s8i	a14, a5, 3
     2bd:	554b           	addi.n	a5, a5, 4

000002bf <cvsd_a2plc_ini+0x187>:
     2bf:	a348           	l32i.n	a4, a3, 40
     2c1:	01d372        	addmi	a7, a3, 0x100
     2c4:	0998           	l32i.n	a9, a9, 0
     2c6:	1b6382        	s32i	a8, a3, 108
     2c9:	1c6382        	s32i	a8, a3, 112
     2cc:	1e6362        	s32i	a6, a3, 120
     2cf:	1f6362        	s32i	a6, a3, 124
     2d2:	206362        	s32i	a6, a3, 128
     2d5:	216362        	s32i	a6, a3, 132
     2d8:	196362        	s32i	a6, a3, 100
     2db:	1d6362        	s32i	a6, a3, 116
     2de:	146392        	s32i	a9, a3, 80
     2e1:	70c772        	addi	a7, a7, 112
     2e4:	205770        	or	a5, a7, a7
     2e7:	186342        	s32i	a4, a3, 96
     2ea:	10a092        	movi	a9, 16
     2ed:	16af76        	loopgtz	a15, 307 <cvsd_a2plc_ini+0x1cf>
     2f0:	418860        	srli	a8, a6, 8
     2f3:	004562        	s8i	a6, a5, 0
     2f6:	014582        	s8i	a8, a5, 1
     2f9:	418880        	srli	a8, a8, 8
     2fc:	024582        	s8i	a8, a5, 2
     2ff:	418880        	srli	a8, a8, 8
     302:	034582        	s8i	a8, a5, 3
     305:	554b           	addi.n	a5, a5, 4
     307:	075d           	mov.n	a5, a7
     309:	16a976        	loopgtz	a9, 323 <cvsd_a2plc_ini+0x1eb>
     30c:	41a860        	srli	a10, a6, 8
     30f:	404562        	s8i	a6, a5, 64
     312:	4145a2        	s8i	a10, a5, 65
     315:	41a8a0        	srli	a10, a10, 8
     318:	4245a2        	s8i	a10, a5, 66
     31b:	41a8a0        	srli	a10, a10, 8
     31e:	4345a2        	s8i	a10, a5, 67
     321:	554b           	addi.n	a5, a5, 4
     323:	f03d           	nop.n
     325:	16ab76        	loopgtz	a11, 33f <cvsd_a2plc_ini+0x207>
     328:	41d860        	srli	a13, a6, 8
     32b:	404762        	s8i	a6, a7, 64
     32e:	4147d2        	s8i	a13, a7, 65
     331:	41d8d0        	srli	a13, a13, 8
     334:	4247d2        	s8i	a13, a7, 66
     337:	41d8d0        	srli	a13, a13, 8
     33a:	4347d2        	s8i	a13, a7, 67
     33d:	774b           	addi.n	a7, a7, 4
     33f:	1163c2        	s32i	a12, a3, 68
     342:	126362        	s32i	a6, a3, 72
     345:	136362        	s32i	a6, a3, 76
     348:	1823f2        	l32i	a15, a3, 96
     34b:	5328           	l32i.n	a2, a3, 20
     34d:	2e0c           	movi.n	a14, 2
     34f:	1563e2        	s32i	a14, a3, 84
     352:	226322        	s32i	a2, a3, 136
     355:	166322        	s32i	a2, a3, 88
     358:	c0ff20        	sub	a15, a15, a2
     35b:	1763f2        	s32i	a15, a3, 92
     35e:	f01d           	retw.n

00000360 <SMART_PITCH_PLC_PROCESS>:
     360:	012136        	entry	a1, 144
     363:	30a162        	movi	a6, 0x130
     366:	252142        	l32i	a4, a1, 148
     369:	03ad           	mov.n	a10, a3
     36b:	196132        	s32i	a3, a1, 100
     36e:	0458           	l32i.n	a5, a4, 0
     370:	2478           	l32i.n	a7, a4, 8
     372:	34b8           	l32i.n	a11, a4, 12
     374:	64f8           	l32i.n	a15, a4, 24
     376:	b488           	l32i.n	a8, a4, 44
     378:	156182        	s32i	a8, a1, 84
     37b:	1661f2        	s32i	a15, a1, 88
     37e:	a438           	l32i.n	a3, a4, 40
     380:	1924f2        	l32i	a15, a4, 100
     383:	1f9a56        	bnez	a10, 580 <SMART_PITCH_PLC_PROCESS+0x220>
     386:	30a272        	movi	a7, 0x230
     389:	e4a5d2        	movi	a13, 0x5e4
     38c:	dca6a2        	movi	a10, 0x6dc
     38f:	4c4f56        	bnez	a15, 857 <SMART_PITCH_PLC_PROCESS+0x4f7>
     392:	c0c3b0        	sub	a12, a3, a11
     395:	909b40        	addx2	a9, a11, a4
     398:	06d4f2        	addmi	a15, a4, 0x600
     39b:	68cff2        	addi	a15, a15, 104
     39e:	06d992        	addmi	a9, a9, 0x600
     3a1:	1aac76        	loopgtz	a12, 3bf <SMART_PITCH_PLC_PROCESS+0x5f>
     3a4:	e009e2        	l8ui	a14, a9, 224
     3a7:	e10982        	l8ui	a8, a9, 225
     3aa:	ff2b           	addi.n	a15, a15, 2
     3ac:	992b           	addi.n	a9, a9, 2
     3ae:	118880        	slli	a8, a8, 8
     3b1:	2088e0        	or	a8, a8, a14
     3b4:	888d           	ae_sext16	a8, a8
     3b6:	764f82        	s8i	a8, a15, 118
     3b9:	418880        	srli	a8, a8, 8
     3bc:	774f82        	s8i	a8, a15, 119

000003bf <SMART_PITCH_PLC_PROCESS+0x5f>:
     3bf:	1161b2        	s32i	a11, a1, 68
     3c2:	1461c2        	s32i	a12, a1, 80
     3c5:	02fd           	mov.n	a15, a2
     3c7:	909c40        	addx2	a9, a12, a4
     3ca:	8099d0        	add	a9, a9, a13
     3cd:	0eab76        	loopgtz	a11, 3df <SMART_PITCH_PLC_PROCESS+0x7f>
     3d0:	992b           	addi.n	a9, a9, 2
     3d2:	2fbd           	ae_l16si.n	a11, a15, 0
     3d4:	ff2b           	addi.n	a15, a15, 2
     3d6:	fa49b2        	s8i	a11, a9, 250
     3d9:	41b8b0        	srli	a11, a11, 8
     3dc:	fb49b2        	s8i	a11, a9, 251
     3df:	1521c2        	l32i	a12, a1, 84
     3e2:	1621d2        	l32i	a13, a1, 88
     3e5:	80f4a0        	add	a15, a4, a10
     3e8:	21e130        	srai	a14, a3, 1
     3eb:	02d4b2        	addmi	a11, a4, 0x200
     3ee:	2ccbb2        	addi	a11, a11, 44
     3f1:	05ae76        	loopgtz	a14, 3fa <SMART_PITCH_PLC_PROCESS+0x9a>
     3f4:	448f14        	ae_lp16x2f.iu	aep0, a15, 4
     3f7:	468b14        	ae_sp16x2f.iu	aep0, a11, 4
     3fa:	c4b8           	l32i.n	a11, a4, 48
     3fc:	74e8           	l32i.n	a14, a4, 28
     3fe:	1824f2        	l32i	a15, a4, 96
     401:	ff0f81        	l32r	a8, 40 (e2c <find_pitch>)
     404:	a47a           	add.n	a10, a4, a7
     406:	1361a2        	s32i	a10, a1, 76
     409:	0008e0        	callx8	a8
     40c:	ff0e81        	l32r	a8, 44 (1490 <__divdi3>)
     40f:	1261a2        	s32i	a10, a1, 72
     412:	242162        	l32i	a6, a1, 144
     415:	94c8           	l32i.n	a12, a4, 36
     417:	2264a2        	s32i	a10, a4, 136
     41a:	31df60        	srai	a13, a6, 31
     41d:	82cca0        	mull	a12, a12, a10
     420:	21b160        	srai	a11, a6, 1
     423:	bbca           	add.n	a11, a11, a12
     425:	0bad           	mov.n	a10, a11
     427:	06cd           	mov.n	a12, a6
     429:	31bfb0        	srai	a11, a11, 31
     42c:	0008e0        	callx8	a8
     42f:	022aa6        	blti	a10, 2, 435 <SMART_PITCH_PLC_PROCESS+0xd5>
     432:	027006        	j	df6 <SMART_PITCH_PLC_PROCESS+0xa96>
     435:	2b0c           	movi.n	a11, 2
     437:	180c           	movi.n	a8, 1
     439:	116482        	s32i	a8, a4, 68

0000043c <SMART_PITCH_PLC_PROCESS+0xdc>:
     43c:	122192        	l32i	a9, a1, 72
     43f:	00a0c2        	movi	a12, 0
     442:	68a6e2        	movi	a14, 0x668
     445:	1564b2        	s32i	a11, a4, 84
     448:	3515a6        	blti	a5, 1, 481 <SMART_PITCH_PLC_PROCESS+0x121>
     44b:	c09350        	sub	a9, a3, a5
     44e:	06d4f2        	addmi	a15, a4, 0x600
     451:	68cff2        	addi	a15, a15, 104
     454:	909940        	addx2	a9, a9, a4
     457:	01d992        	addmi	a9, a9, 0x100
     45a:	30c992        	addi	a9, a9, 48
     45d:	1da576        	loopgtz	a5, 47e <SMART_PITCH_PLC_PROCESS+0x11e>
     460:	01d9d2        	addmi	a13, a9, 0x100
     463:	010db2        	l8ui	a11, a13, 1
     466:	000dd2        	l8ui	a13, a13, 0
     469:	992b           	addi.n	a9, a9, 2
     46b:	11bb80        	slli	a11, a11, 8
     46e:	20bbd0        	or	a11, a11, a13
     471:	8bbd           	ae_sext16	a11, a11
     473:	41a8b0        	srli	a10, a11, 8
     476:	004fb2        	s8i	a11, a15, 0
     479:	014fa2        	s8i	a10, a15, 1
     47c:	ff2b           	addi.n	a15, a15, 2

0000047e <SMART_PITCH_PLC_PROCESS+0x11e>:
     47e:	222492        	l32i	a9, a4, 136

00000481 <SMART_PITCH_PLC_PROCESS+0x121>:
     481:	166492        	s32i	a9, a4, 88
     484:	182482        	l32i	a8, a4, 96
     487:	1264c2        	s32i	a12, a4, 72
     48a:	06d4b2        	addmi	a11, a4, 0x600
     48d:	02d4f2        	addmi	a15, a4, 0x200
     490:	ffcb           	addi.n	a15, a15, 12
     492:	64cbb2        	addi	a11, a11, 100
     495:	01cd           	mov.n	a12, a1
     497:	c08890        	sub	a8, a8, a9
     49a:	176482        	s32i	a8, a4, 92
     49d:	280c           	movi.n	a8, 2
     49f:	d28580        	quos	a8, a5, a8
     4a2:	17a876        	loopgtz	a8, 4bd <SMART_PITCH_PLC_PROCESS+0x15d>
     4a5:	44ab14        	ae_lp16x2f.iu	aep2, a11, 4
     4a8:	449f14        	ae_lp16x2f.iu	aep1, a15, 4
     4ab:	021274        	ae_mulp24s.ll	aeq0, aep1, aep2
     4ae:	021a44        	ae_mulp24s.hh	aeq1, aep1, aep2
     4b1:	368904        	ae_trunca32q48	a9, aeq0
     4b4:	369a04        	ae_trunca32q48	a10, aeq1
     4b7:	0ca9           	s32i.n	a10, a12, 0
     4b9:	1c99           	s32i.n	a9, a12, 4
     4bb:	cc8b           	addi.n	a12, a12, 8

000004bd <SMART_PITCH_PLC_PROCESS+0x15d>:
     4bd:	5e15a6        	blti	a5, 1, 51f <SMART_PITCH_PLC_PROCESS+0x1bf>
     4c0:	01bd           	mov.n	a11, a1
     4c2:	090c           	movi.n	a9, 0
     4c4:	02d4f2        	addmi	a15, a4, 0x200
     4c7:	f0cff2        	addi	a15, a15, -16
     4ca:	fecff2        	addi	a15, a15, -2
     4cd:	1ea576        	loopgtz	a5, 4ef <SMART_PITCH_PLC_PROCESS+0x18f>
     4d0:	1724a2        	l32i	a10, a4, 92
     4d3:	40bf14        	ae_lp16f.iu	aep3, a15, 2
     4d6:	608b04        	ae_lq32f.i	aeq2, a11, 0
     4d9:	c0aa50        	sub	a10, a10, a5
     4dc:	a9aa           	add.n	a10, a9, a10
     4de:	90aa40        	addx2	a10, a10, a4
     4e1:	414a74        	ae_lp16f.x	aep4, a10, a7
     4e4:	02b434        	ae_mulap24s.ll	aeq2, aep3, aep4
     4e7:	991b           	addi.n	a9, a9, 1
     4e9:	36a804        	ae_trunca32q48	a8, aeq2
     4ec:	09eb84        	ae_s32ip	a8, a11, 4
     4ef:	01bd           	mov.n	a11, a1
     4f1:	c0c350        	sub	a12, a3, a5
     4f4:	90cc40        	addx2	a12, a12, a4
     4f7:	01dc92        	addmi	a9, a12, 0x100
     4fa:	30c992        	addi	a9, a9, 48
     4fd:	ccea           	add.n	a12, a12, a14
     4ff:	1ca576        	loopgtz	a5, 51f <SMART_PITCH_PLC_PROCESS+0x1bf>
     502:	cc2b           	addi.n	a12, a12, 2
     504:	01d982        	addmi	a8, a9, 0x100
     507:	0bf8           	l32i.n	a15, a11, 0
     509:	992b           	addi.n	a9, a9, 2
     50b:	bb4b           	addi.n	a11, a11, 4
     50d:	21fff0        	srai	a15, a15, 15
     510:	0048f2        	s8i	a15, a8, 0
     513:	41d8f0        	srli	a13, a15, 8
     516:	0148d2        	s8i	a13, a8, 1
     519:	764cf2        	s8i	a15, a12, 118
     51c:	774cd2        	s8i	a13, a12, 119

0000051f <SMART_PITCH_PLC_PROCESS+0x1bf>:
     51f:	142192        	l32i	a9, a1, 80
     522:	02fd           	mov.n	a15, a2
     524:	112182        	l32i	a8, a1, 68
     527:	909940        	addx2	a9, a9, a4
     52a:	8099e0        	add	a9, a9, a14
     52d:	12a876        	loopgtz	a8, 543 <SMART_PITCH_PLC_PROCESS+0x1e3>
     530:	7909a2        	l8ui	a10, a9, 121
     533:	7809b2        	l8ui	a11, a9, 120
     536:	992b           	addi.n	a9, a9, 2
     538:	11aa80        	slli	a10, a10, 8
     53b:	20aab0        	or	a10, a10, a11
     53e:	8aad           	ae_sext16	a10, a10
     540:	09cfa4        	ae_s16ip	a10, a15, 2

00000543 <SMART_PITCH_PLC_PROCESS+0x1e3>:
     543:	112172        	l32i	a7, a1, 68
     546:	c07760        	sub	a7, a7, a6
     549:	737a           	add.n	a7, a3, a7
     54b:	0217e6        	bgei	a7, 1, 551 <SMART_PITCH_PLC_PROCESS+0x1f1>
     54e:	015006        	j	a92 <SMART_PITCH_PLC_PROCESS+0x732>
     551:	112152        	l32i	a5, a1, 68
     554:	f4ea           	add.n	a15, a4, a14
     556:	c05650        	sub	a5, a6, a5
     559:	909540        	addx2	a9, a5, a4
     55c:	99ea           	add.n	a9, a9, a14
     55e:	1aa776        	loopgtz	a7, 57c <SMART_PITCH_PLC_PROCESS+0x21c>
     561:	7809a2        	l8ui	a10, a9, 120
     564:	790982        	l8ui	a8, a9, 121
     567:	ff2b           	addi.n	a15, a15, 2
     569:	992b           	addi.n	a9, a9, 2
     56b:	118880        	slli	a8, a8, 8
     56e:	2088a0        	or	a8, a8, a10
     571:	888d           	ae_sext16	a8, a8
     573:	764f82        	s8i	a8, a15, 118
     576:	418880        	srli	a8, a8, 8
     579:	774f82        	s8i	a8, a15, 119
     57c:	014606        	j	a98 <SMART_PITCH_PLC_PROCESS+0x738>

0000057f <SMART_PITCH_PLC_PROCESS+0x21f>:
	...

00000580 <SMART_PITCH_PLC_PROCESS+0x220>:
     580:	25ff16        	beqz	a15, 7e3 <SMART_PITCH_PLC_PROCESS+0x483>
     583:	4b24a2        	l32i	a10, a4, 0x12c
     586:	2421b2        	l32i	a11, a1, 144
     589:	1b61b2        	s32i	a11, a1, 108
     58c:	0b5a16        	beqz	a10, 645 <SMART_PITCH_PLC_PROCESS+0x2e5>
     58f:	0b6d           	mov.n	a6, a11
     591:	1b61b2        	s32i	a11, a1, 108
     594:	88cbc2        	addi	a12, a11, -120
     597:	0aac56        	bnez	a12, 645 <SMART_PITCH_PLC_PROCESS+0x2e5>
     59a:	1624b2        	l32i	a11, a4, 88
     59d:	1724c2        	l32i	a12, a4, 92
     5a0:	06dd           	mov.n	a13, a6
     5a2:	04fd           	mov.n	a15, a4
     5a4:	fea981        	l32r	a8, 48 (1458 <getfespeech>)
     5a7:	02d452        	addmi	a5, a4, 0x200
     5aa:	30c552        	addi	a5, a5, 48
     5ad:	05ad           	mov.n	a10, a5
     5af:	05ed           	mov.n	a14, a5
     5b1:	0008e0        	callx8	a8
     5b4:	1c24b2        	l32i	a11, a4, 112
     5b7:	fea581        	l32r	a8, 4c (7ffe <_end+0x6432>)
     5ba:	1b21a2        	l32i	a10, a1, 108
     5bd:	34a2f2        	movi	a15, 0x234
     5c0:	1b28b7        	blt	a8, a11, 5df <SMART_PITCH_PLC_PROCESS+0x27f>
     5c3:	302bb4        	ae_cvtp24a16x2.ll	aep2, a11, a11
     5c6:	05ed           	mov.n	a14, a5
     5c8:	02d492        	addmi	a9, a4, 0x200
     5cb:	34c992        	addi	a9, a9, 52
     5ce:	4009e4        	ae_lp16f.i	aep0, a9, -4
     5d1:	0aa676        	loopgtz	a6, 5df <SMART_PITCH_PLC_PROCESS+0x27f>
     5d4:	018a74        	ae_mulfp24s.ll	aeq3, aep0, aep2
     5d7:	fafbec8e1f 	{ ae_lp16f.iu	aep0, a14, 2; ae_roundsp16q48sym	aep5, aeq3 }
     5dc:	425ef4        	ae_sp16f.l.i	aep5, a14, -2
     5df:	02bd           	mov.n	a11, a2
     5e1:	11d7f0        	slli	a13, a7, 1
     5e4:	01d492        	addmi	a9, a4, 0x100
     5e7:	34c992        	addi	a9, a9, 52
     5ea:	12a776        	loopgtz	a7, 600 <SMART_PITCH_PLC_PROCESS+0x2a0>
     5ed:	fd09c2        	l8ui	a12, a9, 253
     5f0:	fc09e2        	l8ui	a14, a9, 252
     5f3:	992b           	addi.n	a9, a9, 2
     5f5:	11cc80        	slli	a12, a12, 8
     5f8:	20cce0        	or	a12, a12, a14
     5fb:	8ccd           	ae_sext16	a12, a12
     5fd:	09cbc4        	ae_s16ip	a12, a11, 2

00000600 <SMART_PITCH_PLC_PROCESS+0x2a0>:
     600:	412d24        	ae_lp16f.x	aep2, a13, a2
     603:	c08670        	sub	a8, a6, a7
     606:	fe92b1        	l32r	a11, 50 (800000 <_end+0x7fe434>)
     609:	9d4a           	add.n	a9, a13, a4
     60b:	02d9c2        	addmi	a12, a9, 0x200
     60e:	30ccc2        	addi	a12, a12, 48
     611:	99fa           	add.n	a9, a9, a15
     613:	d2eb80        	quos	a14, a11, a8
     616:	4009e4        	ae_lp16f.i	aep0, a9, -4
     619:	c0bbe0        	sub	a11, a11, a14
     61c:	399be4        	ae_movpa24x2	aep1, a11, a14
     61f:	60b0e0        	neg	a11, a14
     622:	39cbe4        	ae_movpa24x2	aep4, a11, a14
     625:	80ed20        	add	a14, a13, a2
     628:	dc420feebe 	{ addi	a11, a14, -2; ae_selp24.ll	aep0, aep0, aep2 }
     62d:	11a876        	loopgtz	a8, 642 <SMART_PITCH_PLC_PROCESS+0x2e2>
     630:	041004        	ae_mulzaafp24s.hh.ll	aeq0, aep1, aep0
     633:	d8856cae1f 	{ ae_lp16f.iu	aep2, a14, 2; ae_addsp24s	aep1, aep1, aep4 }
     638:	f9e3ec8c1f 	{ ae_lp16f.iu	aep0, a12, 2; ae_roundsp16q48sym	aep3, aeq0 }
     63d:	dc4232bb1f 	{ ae_sp16f.l.iu	aep3, a11, 2; ae_selp24.ll	aep0, aep0, aep2 }
     642:	006806        	j	7e6 <SMART_PITCH_PLC_PROCESS+0x486>

00000645 <SMART_PITCH_PLC_PROCESS+0x2e5>:
     645:	30a162        	movi	a6, 0x130
     648:	1624b2        	l32i	a11, a4, 88
     64b:	1724c2        	l32i	a12, a4, 92
     64e:	d75a           	add.n	a13, a7, a5
     650:	04fd           	mov.n	a15, a4
     652:	02d4e2        	addmi	a14, a4, 0x200
     655:	fe7c81        	l32r	a8, 48 (1458 <getfespeech>)
     658:	30cee2        	addi	a14, a14, 48
     65b:	0ead           	mov.n	a10, a14
     65d:	0008e0        	callx8	a8
     660:	1b21a2        	l32i	a10, a1, 108
     663:	70a1d2        	movi	a13, 0x170
     666:	7215a6        	blti	a5, 1, 6dc <SMART_PITCH_PLC_PROCESS+0x37c>
     669:	30a2c2        	movi	a12, 0x230
     66c:	02d4f2        	addmi	a15, a4, 0x200
     66f:	01d492        	addmi	a9, a4, 0x100
     672:	90b720        	addx2	a11, a7, a2
     675:	fecbb2        	addi	a11, a11, -2
     678:	70c992        	addi	a9, a9, 112
     67b:	f0cff2        	addi	a15, a15, -16
     67e:	fecff2        	addi	a15, a15, -2
     681:	22a576        	loopgtz	a5, 6a7 <SMART_PITCH_PLC_PROCESS+0x347>
     684:	40cb14        	ae_lp16f.iu	aep4, a11, 2
     687:	40bf14        	ae_lp16f.iu	aep3, a15, 2
     68a:	023c74        	ae_mulp24s.ll	aeq1, aep3, aep4
     68d:	369804        	ae_trunca32q48	a8, aeq1
     690:	994b           	addi.n	a9, a9, 4
     692:	3c4982        	s8i	a8, a9, 60
     695:	418880        	srli	a8, a8, 8
     698:	3d4982        	s8i	a8, a9, 61
     69b:	418880        	srli	a8, a8, 8
     69e:	3e4982        	s8i	a8, a9, 62
     6a1:	418880        	srli	a8, a8, 8
     6a4:	3f4982        	s8i	a8, a9, 63

000006a7 <SMART_PITCH_PLC_PROCESS+0x347>:
     6a7:	94da           	add.n	a9, a4, a13
     6a9:	90b740        	addx2	a11, a7, a4
     6ac:	bbca           	add.n	a11, a11, a12
     6ae:	fecbb2        	addi	a11, a11, -2
     6b1:	02d4c2        	addmi	a12, a4, 0x200
     6b4:	cceb           	addi.n	a12, a12, 14
     6b6:	22a576        	loopgtz	a5, 6dc <SMART_PITCH_PLC_PROCESS+0x37c>
     6b9:	40eb14        	ae_lp16f.iu	aep6, a11, 2
     6bc:	40dc14        	ae_lp16f.iu	aep5, a12, 2
     6bf:	02d674        	ae_mulp24s.ll	aeq2, aep5, aep6
     6c2:	36ae04        	ae_trunca32q48	a14, aeq2
     6c5:	0049e2        	s8i	a14, a9, 0
     6c8:	41e8e0        	srli	a14, a14, 8
     6cb:	0149e2        	s8i	a14, a9, 1
     6ce:	41e8e0        	srli	a14, a14, 8
     6d1:	0249e2        	s8i	a14, a9, 2
     6d4:	41e8e0        	srli	a14, a14, 8
     6d7:	0349e2        	s8i	a14, a9, 3
     6da:	994b           	addi.n	a9, a9, 4

000006dc <SMART_PITCH_PLC_PROCESS+0x37c>:
     6dc:	1d24f2        	l32i	a15, a4, 116
     6df:	fcc2b2        	addi	a11, a2, -4
     6e2:	218170        	srai	a8, a7, 1
     6e5:	ff0b           	addi.n	a15, a15, -1
     6e7:	094f56        	bnez	a15, 77f <SMART_PITCH_PLC_PROCESS+0x41f>
     6ea:	2617a6        	blti	a7, 1, 714 <SMART_PITCH_PLC_PROCESS+0x3b4>
     6ed:	b46a           	add.n	a11, a4, a6
     6ef:	20f220        	or	a15, a2, a2
     6f2:	1c24c2        	l32i	a12, a4, 112
     6f5:	1ba776        	loopgtz	a7, 714 <SMART_PITCH_PLC_PROCESS+0x3b4>
     6f8:	01db92        	addmi	a9, a11, 0x100
     6fb:	010982        	l8ui	a8, a9, 1
     6fe:	000992        	l8ui	a9, a9, 0
     701:	118880        	slli	a8, a8, 8
     704:	208890        	or	a8, a8, a9
     707:	888d           	ae_sext16	a8, a8
     709:	8288c0        	mull	a8, a8, a12
     70c:	bb2b           	addi.n	a11, a11, 2
     70e:	218f80        	srai	a8, a8, 15
     711:	09cf84        	ae_s16ip	a8, a15, 2

00000714 <SMART_PITCH_PLC_PROCESS+0x3b4>:
     714:	94da           	add.n	a9, a4, a13
     716:	90b720        	addx2	a11, a7, a2
     719:	70c4c2        	addi	a12, a4, 112
     71c:	40c1e2        	addi	a14, a1, 64
     71f:	57a576        	loopgtz	a5, 77a <SMART_PITCH_PLC_PROCESS+0x41a>
     722:	402c04        	ae_lp16f.i	aep2, a12, 0
     725:	010982        	l8ui	a8, a9, 1
     728:	020972        	l8ui	a7, a9, 2
     72b:	030962        	l8ui	a6, a9, 3
     72e:	4309f2        	l8ui	a15, a9, 67
     731:	420952        	l8ui	a5, a9, 66
     734:	116680        	slli	a6, a6, 8
     737:	11ff80        	slli	a15, a15, 8
     73a:	20ff50        	or	a15, a15, a5
     73d:	206670        	or	a6, a6, a7
     740:	410952        	l8ui	a5, a9, 65
     743:	11ff80        	slli	a15, a15, 8
     746:	000972        	l8ui	a7, a9, 0
     749:	116680        	slli	a6, a6, 8
     74c:	206680        	or	a6, a6, a8
     74f:	116680        	slli	a6, a6, 8
     752:	206670        	or	a6, a6, a7
     755:	216f60        	srai	a6, a6, 15
     758:	20ff50        	or	a15, a15, a5
     75b:	11ff80        	slli	a15, a15, 8
     75e:	106162        	s32i	a6, a1, 64
     761:	401e04        	ae_lp16f.i	aep1, a14, 0
     764:	021274        	ae_mulp24s.ll	aeq0, aep1, aep2
     767:	400962        	l8ui	a6, a9, 64
     76a:	368504        	ae_trunca32q48	a5, aeq0
     76d:	994b           	addi.n	a9, a9, 4
     76f:	20ff60        	or	a15, a15, a6
     772:	ff5a           	add.n	a15, a15, a5
     774:	21fff0        	srai	a15, a15, 15
     777:	09cbf4        	ae_s16ip	a15, a11, 2

0000077a <SMART_PITCH_PLC_PROCESS+0x41a>:
     77a:	001a06        	j	7e6 <SMART_PITCH_PLC_PROCESS+0x486>

0000077d <SMART_PITCH_PLC_PROCESS+0x41d>:
	...

0000077f <SMART_PITCH_PLC_PROCESS+0x41f>:
     77f:	02d4f2        	addmi	a15, a4, 0x200
     782:	2ccff2        	addi	a15, a15, 44
     785:	05a876        	loopgtz	a8, 78e <SMART_PITCH_PLC_PROCESS+0x42e>
     788:	44bf14        	ae_lp16x2f.iu	aep3, a15, 4
     78b:	46bb14        	ae_sp16x2f.iu	aep3, a11, 4

0000078e <SMART_PITCH_PLC_PROCESS+0x42e>:
     78e:	94da           	add.n	a9, a4, a13
     790:	90b720        	addx2	a11, a7, a2
     793:	f03d           	nop.n
     795:	45a576        	loopgtz	a5, 7de <SMART_PITCH_PLC_PROCESS+0x47e>
     798:	400982        	l8ui	a8, a9, 64
     79b:	000972        	l8ui	a7, a9, 0
     79e:	0109c2        	l8ui	a12, a9, 1
     7a1:	4109d2        	l8ui	a13, a9, 65
     7a4:	0209e2        	l8ui	a14, a9, 2
     7a7:	030952        	l8ui	a5, a9, 3
     7aa:	430962        	l8ui	a6, a9, 67
     7ad:	4209f2        	l8ui	a15, a9, 66
     7b0:	115580        	slli	a5, a5, 8
     7b3:	116680        	slli	a6, a6, 8
     7b6:	2066f0        	or	a6, a6, a15
     7b9:	2055e0        	or	a5, a5, a14
     7bc:	115580        	slli	a5, a5, 8
     7bf:	116680        	slli	a6, a6, 8
     7c2:	2066d0        	or	a6, a6, a13
     7c5:	2055c0        	or	a5, a5, a12
     7c8:	115580        	slli	a5, a5, 8
     7cb:	116680        	slli	a6, a6, 8
     7ce:	994b           	addi.n	a9, a9, 4
     7d0:	206680        	or	a6, a6, a8
     7d3:	205570        	or	a5, a5, a7
     7d6:	556a           	add.n	a5, a5, a6
     7d8:	215f50        	srai	a5, a5, 15
     7db:	09cb54        	ae_s16ip	a5, a11, 2
     7de:	000106        	j	7e6 <SMART_PITCH_PLC_PROCESS+0x486>

000007e1 <SMART_PITCH_PLC_PROCESS+0x481>:
	...

000007e3 <SMART_PITCH_PLC_PROCESS+0x483>:
     7e3:	2421a2        	l32i	a10, a1, 144

000007e6 <SMART_PITCH_PLC_PROCESS+0x486>:
     7e6:	68a692        	movi	a9, 0x668
     7e9:	06d4f2        	addmi	a15, a4, 0x600
     7ec:	0a6d           	mov.n	a6, a10
     7ee:	c0d3a0        	sub	a13, a3, a10
     7f1:	90ba40        	addx2	a11, a10, a4
     7f4:	68cff2        	addi	a15, a15, 104
     7f7:	06dbb2        	addmi	a11, a11, 0x600
     7fa:	68cbb2        	addi	a11, a11, 104
     7fd:	1aad76        	loopgtz	a13, 81b <SMART_PITCH_PLC_PROCESS+0x4bb>
     800:	780be2        	l8ui	a14, a11, 120
     803:	790bc2        	l8ui	a12, a11, 121
     806:	ff2b           	addi.n	a15, a15, 2
     808:	bb2b           	addi.n	a11, a11, 2
     80a:	11cc80        	slli	a12, a12, 8
     80d:	20cce0        	or	a12, a12, a14
     810:	8ccd           	ae_sext16	a12, a12
     812:	764fc2        	s8i	a12, a15, 118
     815:	41c8c0        	srli	a12, a12, 8
     818:	774fc2        	s8i	a12, a15, 119

0000081b <SMART_PITCH_PLC_PROCESS+0x4bb>:
     81b:	0b0c           	movi.n	a11, 0
     81d:	02fd           	mov.n	a15, a2
     81f:	90ed40        	addx2	a14, a13, a4
     822:	809e90        	add	a9, a14, a9
     825:	0ea676        	loopgtz	a6, 837 <SMART_PITCH_PLC_PROCESS+0x4d7>
     828:	992b           	addi.n	a9, a9, 2
     82a:	2f8d           	ae_l16si.n	a8, a15, 0
     82c:	ff2b           	addi.n	a15, a15, 2
     82e:	764982        	s8i	a8, a9, 118
     831:	418880        	srli	a8, a8, 8
     834:	774982        	s8i	a8, a9, 119
     837:	1921a2        	l32i	a10, a1, 100
     83a:	06cd           	mov.n	a12, a6
     83c:	04dd           	mov.n	a13, a4
     83e:	fe0581        	l32r	a8, 54 (1198 <plc_gainctrl>)
     841:	1964b2        	s32i	a11, a4, 100
     844:	1d64b2        	s32i	a11, a4, 116
     847:	fdfc91        	l32r	a9, 38 (7fff <_end+0x6433>)
     84a:	1c6492        	s32i	a9, a4, 112
     84d:	1b6492        	s32i	a9, a4, 108
     850:	02bd           	mov.n	a11, a2
     852:	0008e0        	callx8	a8
     855:	f01d           	retw.n

00000857 <SMART_PITCH_PLC_PROCESS+0x4f7>:
     857:	44b8           	l32i.n	a11, a4, 16
     859:	201b66        	bnei	a11, 1, 87d <SMART_PITCH_PLC_PROCESS+0x51d>
     85c:	112492        	l32i	a9, a4, 68
     85f:	0519f7        	beq	a9, a15, 868 <SMART_PITCH_PLC_PROCESS+0x508>
     862:	1524b2        	l32i	a11, a4, 84
     865:	149bf7        	bne	a11, a15, 87d <SMART_PITCH_PLC_PROCESS+0x51d>

00000868 <SMART_PITCH_PLC_PROCESS+0x508>:
     868:	e4e8           	l32i.n	a14, a4, 56
     86a:	ee0b           	addi.n	a14, a14, -1
     86c:	39ce56        	bnez	a14, c0c <SMART_PITCH_PLC_PROCESS+0x8ac>
     86f:	1c24c2        	l32i	a12, a4, 112
     872:	f4b8           	l32i.n	a11, a4, 60
     874:	82bbc0        	mull	a11, a11, a12
     877:	21bfb0        	srai	a11, a11, 15
     87a:	00e446        	j	c0f <SMART_PITCH_PLC_PROCESS+0x8af>

0000087d <SMART_PITCH_PLC_PROCESS+0x51d>:
     87d:	1d24e2        	l32i	a14, a4, 116
     880:	ee0b           	addi.n	a14, a14, -1
     882:	1d8e16        	beqz	a14, a5e <SMART_PITCH_PLC_PROCESS+0x6fe>
     885:	e488           	l32i.n	a8, a4, 56
     887:	880b           	addi.n	a8, a8, -1
     889:	1d1816        	beqz	a8, a5e <SMART_PITCH_PLC_PROCESS+0x6fe>
     88c:	b47a           	add.n	a11, a4, a7

0000088e <SMART_PITCH_PLC_PROCESS+0x52e>:
     88e:	1724c2        	l32i	a12, a4, 92
     891:	04fd           	mov.n	a15, a4
     893:	fded81        	l32r	a8, 48 (1458 <getfespeech>)
     896:	242162        	l32i	a6, a1, 144
     899:	0bad           	mov.n	a10, a11
     89b:	07d4e2        	addmi	a14, a4, 0x700
     89e:	e0cee2        	addi	a14, a14, -32
     8a1:	1624b2        	l32i	a11, a4, 88
     8a4:	06dd           	mov.n	a13, a6
     8a6:	0008e0        	callx8	a8
     8a9:	1d24f2        	l32i	a15, a4, 116
     8ac:	ff0b           	addi.n	a15, a15, -1
     8ae:	18bf16        	beqz	a15, a3d <SMART_PITCH_PLC_PROCESS+0x6dd>
     8b1:	e488           	l32i.n	a8, a4, 56
     8b3:	880b           	addi.n	a8, a8, -1
     8b5:	184816        	beqz	a8, a3d <SMART_PITCH_PLC_PROCESS+0x6dd>

000008b8 <SMART_PITCH_PLC_PROCESS+0x558>:
     8b8:	fcc2b2        	addi	a11, a2, -4
     8bb:	219160        	srai	a9, a6, 1
     8be:	07d4f2        	addmi	a15, a4, 0x700
     8c1:	fddd81        	l32r	a8, 38 (7fff <_end+0x6433>)
     8c4:	1b6482        	s32i	a8, a4, 108
     8c7:	dccff2        	addi	a15, a15, -36
     8ca:	05a976        	loopgtz	a9, 8d3 <SMART_PITCH_PLC_PROCESS+0x573>
     8cd:	44cf14        	ae_lp16x2f.iu	aep4, a15, 4
     8d0:	46cb14        	ae_sp16x2f.iu	aep4, a11, 4

000008d3 <SMART_PITCH_PLC_PROCESS+0x573>:
     8d3:	68a6c2        	movi	a12, 0x668
     8d6:	c0d360        	sub	a13, a3, a6
     8d9:	90b640        	addx2	a11, a6, a4
     8dc:	06d4f2        	addmi	a15, a4, 0x600
     8df:	68cff2        	addi	a15, a15, 104
     8e2:	06dbb2        	addmi	a11, a11, 0x600
     8e5:	68cbb2        	addi	a11, a11, 104
     8e8:	1aad76        	loopgtz	a13, 906 <SMART_PITCH_PLC_PROCESS+0x5a6>
     8eb:	780b82        	l8ui	a8, a11, 120
     8ee:	790be2        	l8ui	a14, a11, 121
     8f1:	ff2b           	addi.n	a15, a15, 2
     8f3:	bb2b           	addi.n	a11, a11, 2
     8f5:	11ee80        	slli	a14, a14, 8
     8f8:	20ee80        	or	a14, a14, a8
     8fb:	8eed           	ae_sext16	a14, a14
     8fd:	764fe2        	s8i	a14, a15, 118
     900:	41e8e0        	srli	a14, a14, 8
     903:	774fe2        	s8i	a14, a15, 119

00000906 <SMART_PITCH_PLC_PROCESS+0x5a6>:
     906:	02fd           	mov.n	a15, a2
     908:	909d40        	addx2	a9, a13, a4
     90b:	99ca           	add.n	a9, a9, a12
     90d:	0ea676        	loopgtz	a6, 91f <SMART_PITCH_PLC_PROCESS+0x5bf>
     910:	992b           	addi.n	a9, a9, 2
     912:	2fad           	ae_l16si.n	a10, a15, 0
     914:	ff2b           	addi.n	a15, a15, 2
     916:	7649a2        	s8i	a10, a9, 118
     919:	41a8a0        	srli	a10, a10, 8
     91c:	7749a2        	s8i	a10, a9, 119
     91f:	003d86        	j	a19 <SMART_PITCH_PLC_PROCESS+0x6b9>

00000922 <SMART_PITCH_PLC_PROCESS+0x5c2>:
     922:	02fd           	mov.n	a15, a2
     924:	70c4c2        	addi	a12, a4, 112
     927:	6cc4b2        	addi	a11, a4, 108
     92a:	40c172        	addi	a7, a1, 64
     92d:	68a576        	loopgtz	a5, 999 <SMART_PITCH_PLC_PROCESS+0x639>
     930:	402b04        	ae_lp16f.i	aep2, a11, 0
     933:	404c04        	ae_lp16f.i	aep4, a12, 0
     936:	420982        	l8ui	a8, a9, 66
     939:	0309a2        	l8ui	a10, a9, 3
     93c:	0209d2        	l8ui	a13, a9, 2
     93f:	0109e2        	l8ui	a14, a9, 1
     942:	11aa80        	slli	a10, a10, 8
     945:	20aad0        	or	a10, a10, a13
     948:	11aa80        	slli	a10, a10, 8
     94b:	0009d2        	l8ui	a13, a9, 0
     94e:	20aae0        	or	a10, a10, a14
     951:	4309e2        	l8ui	a14, a9, 67
     954:	11aa80        	slli	a10, a10, 8
     957:	20aad0        	or	a10, a10, a13
     95a:	21afa0        	srai	a10, a10, 15
     95d:	11ee80        	slli	a14, a14, 8
     960:	1061a2        	s32i	a10, a1, 64
     963:	403704        	ae_lp16f.i	aep3, a7, 0
     966:	20ee80        	or	a14, a14, a8
     969:	4109a2        	l8ui	a10, a9, 65
     96c:	400982        	l8ui	a8, a9, 64
     96f:	11ee80        	slli	a14, a14, 8
     972:	20eea0        	or	a14, a14, a10
     975:	11ee80        	slli	a14, a14, 8
     978:	20ee80        	or	a14, a14, a8
     97b:	21efe0        	srai	a14, a14, 15
     97e:	1061e2        	s32i	a14, a1, 64
     981:	ea8fac170f 	{ ae_lp16f.i	aep1, a7, 0; ae_mulp24s.ll	aeq0, aep3, aep4 }
     986:	029a74        	ae_mulp24s.ll	aeq3, aep1, aep2
     989:	368a04        	ae_trunca32q48	a10, aeq0
     98c:	36bd04        	ae_trunca32q48	a13, aeq3
     98f:	994b           	addi.n	a9, a9, 4
     991:	aada           	add.n	a10, a10, a13
     993:	21afa0        	srai	a10, a10, 15
     996:	09cfa4        	ae_s16ip	a10, a15, 2

00000999 <SMART_PITCH_PLC_PROCESS+0x639>:
     999:	c0c650        	sub	a12, a6, a5
     99c:	2a1ca6        	blti	a12, 1, 9ca <SMART_PITCH_PLC_PROCESS+0x66a>
     99f:	909520        	addx2	a9, a5, a2
     9a2:	1b24d2        	l32i	a13, a4, 108
     9a5:	90b540        	addx2	a11, a5, a4
     9a8:	06dbb2        	addmi	a11, a11, 0x600
     9ab:	68cbb2        	addi	a11, a11, 104
     9ae:	18ac76        	loopgtz	a12, 9ca <SMART_PITCH_PLC_PROCESS+0x66a>
     9b1:	790be2        	l8ui	a14, a11, 121
     9b4:	780bf2        	l8ui	a15, a11, 120
     9b7:	11ee80        	slli	a14, a14, 8
     9ba:	20eef0        	or	a14, a14, a15
     9bd:	8eed           	ae_sext16	a14, a14
     9bf:	82eed0        	mull	a14, a14, a13
     9c2:	bb2b           	addi.n	a11, a11, 2
     9c4:	21efe0        	srai	a14, a14, 15
     9c7:	09c9e4        	ae_s16ip	a14, a9, 2

000009ca <SMART_PITCH_PLC_PROCESS+0x66a>:
     9ca:	c0d360        	sub	a13, a3, a6
     9cd:	90b640        	addx2	a11, a6, a4
     9d0:	06d4f2        	addmi	a15, a4, 0x600
     9d3:	68cff2        	addi	a15, a15, 104
     9d6:	06dbb2        	addmi	a11, a11, 0x600
     9d9:	68cbb2        	addi	a11, a11, 104
     9dc:	f03d           	nop.n
     9de:	1aad76        	loopgtz	a13, 9fc <SMART_PITCH_PLC_PROCESS+0x69c>
     9e1:	780be2        	l8ui	a14, a11, 120
     9e4:	790bc2        	l8ui	a12, a11, 121
     9e7:	ff2b           	addi.n	a15, a15, 2
     9e9:	bb2b           	addi.n	a11, a11, 2
     9eb:	11cc80        	slli	a12, a12, 8
     9ee:	20cce0        	or	a12, a12, a14
     9f1:	8ccd           	ae_sext16	a12, a12
     9f3:	764fc2        	s8i	a12, a15, 118
     9f6:	41c8c0        	srli	a12, a12, 8
     9f9:	774fc2        	s8i	a12, a15, 119

000009fc <SMART_PITCH_PLC_PROCESS+0x69c>:
     9fc:	02fd           	mov.n	a15, a2
     9fe:	909d40        	addx2	a9, a13, a4
     a01:	06d992        	addmi	a9, a9, 0x600
     a04:	68c992        	addi	a9, a9, 104
     a07:	0ea676        	loopgtz	a6, a19 <SMART_PITCH_PLC_PROCESS+0x6b9>
     a0a:	992b           	addi.n	a9, a9, 2
     a0c:	2fbd           	ae_l16si.n	a11, a15, 0
     a0e:	ff2b           	addi.n	a15, a15, 2
     a10:	7649b2        	s8i	a11, a9, 118
     a13:	41b8b0        	srli	a11, a11, 8
     a16:	7749b2        	s8i	a11, a9, 119

00000a19 <SMART_PITCH_PLC_PROCESS+0x6b9>:
     a19:	1b24c2        	l32i	a12, a4, 108
     a1c:	1c64c2        	s32i	a12, a4, 112

00000a1f <SMART_PITCH_PLC_PROCESS+0x6bf>:
     a1f:	06cd           	mov.n	a12, a6
     a21:	02bd           	mov.n	a11, a2
     a23:	1921a2        	l32i	a10, a1, 100
     a26:	04dd           	mov.n	a13, a4
     a28:	fd8b81        	l32r	a8, 54 (1198 <plc_gainctrl>)
     a2b:	192492        	l32i	a9, a4, 100
     a2e:	e4e8           	l32i.n	a14, a4, 56
     a30:	1d64e2        	s32i	a14, a4, 116
     a33:	991b           	addi.n	a9, a9, 1
     a35:	196492        	s32i	a9, a4, 100
     a38:	0008e0        	callx8	a8
     a3b:	f01d           	retw.n

00000a3d <SMART_PITCH_PLC_PROCESS+0x6dd>:
     a3d:	192482        	l32i	a8, a4, 100
     a40:	1124f2        	l32i	a15, a4, 68
     a43:	022f87        	blt	a15, a8, a49 <SMART_PITCH_PLC_PROCESS+0x6e9>
     a46:	ff9b86        	j	8b8 <SMART_PITCH_PLC_PROCESS+0x558>
     a49:	e498           	l32i.n	a9, a4, 56
     a4b:	990b           	addi.n	a9, a9, -1
     a4d:	0a8956        	bnez	a9, af9 <SMART_PITCH_PLC_PROCESS+0x799>
     a50:	1c24a2        	l32i	a10, a4, 112
     a53:	f498           	l32i.n	a9, a4, 60
     a55:	8299a0        	mull	a9, a9, a10
     a58:	219f90        	srai	a9, a9, 15
     a5b:	002746        	j	afc <SMART_PITCH_PLC_PROCESS+0x79c>

00000a5e <SMART_PITCH_PLC_PROCESS+0x6fe>:
     a5e:	1124e2        	l32i	a14, a4, 68
     a61:	b47a           	add.n	a11, a4, a7
     a63:	1361b2        	s32i	a11, a1, 76
     a66:	022ef7        	blt	a14, a15, a6c <SMART_PITCH_PLC_PROCESS+0x70c>
     a69:	ff8846        	j	88e <SMART_PITCH_PLC_PROCESS+0x52e>
     a6c:	1724c2        	l32i	a12, a4, 92
     a6f:	05dd           	mov.n	a13, a5
     a71:	04fd           	mov.n	a15, a4
     a73:	0bed           	mov.n	a14, a11
     a75:	0bad           	mov.n	a10, a11
     a77:	122492        	l32i	a9, a4, 72
     a7a:	fd7381        	l32r	a8, 48 (1458 <getfespeech>)
     a7d:	1a6192        	s32i	a9, a1, 104
     a80:	1624b2        	l32i	a11, a4, 88
     a83:	0008e0        	callx8	a8
     a86:	1321b2        	l32i	a11, a1, 76
     a89:	1a21a2        	l32i	a10, a1, 104
     a8c:	1264a2        	s32i	a10, a4, 72
     a8f:	ff7ec6        	j	88e <SMART_PITCH_PLC_PROCESS+0x52e>

00000a92 <SMART_PITCH_PLC_PROCESS+0x732>:
     a92:	112152        	l32i	a5, a1, 68
     a95:	c05650        	sub	a5, a6, a5

00000a98 <SMART_PITCH_PLC_PROCESS+0x738>:
     a98:	1321a2        	l32i	a10, a1, 76
     a9b:	1624b2        	l32i	a11, a4, 88
     a9e:	1724c2        	l32i	a12, a4, 92
     aa1:	05dd           	mov.n	a13, a5
     aa3:	04fd           	mov.n	a15, a4
     aa5:	fd6881        	l32r	a8, 48 (1458 <getfespeech>)
     aa8:	0bd4e2        	addmi	a14, a4, 0xb00
     aab:	68cee2        	addi	a14, a14, 104
     aae:	0008e0        	callx8	a8
     ab1:	1121b2        	l32i	a11, a1, 68
     ab4:	909740        	addx2	a9, a7, a4
     ab7:	0ad4c2        	addmi	a12, a4, 0xa00
     aba:	6cccc2        	addi	a12, a12, 108
     abd:	06d992        	addmi	a9, a9, 0x600
     ac0:	e4c992        	addi	a9, a9, -28
     ac3:	90bb20        	addx2	a11, a11, a2
     ac6:	2ba576        	loopgtz	a5, af5 <SMART_PITCH_PLC_PROCESS+0x795>
     ac9:	fd0ce2        	l8ui	a14, a12, 253
     acc:	fc0cf2        	l8ui	a15, a12, 252
     acf:	cc2b           	addi.n	a12, a12, 2
     ad1:	11ee80        	slli	a14, a14, 8
     ad4:	20eef0        	or	a14, a14, a15
     ad7:	8eed           	ae_sext16	a14, a14
     ad9:	09cbe4        	ae_s16ip	a14, a11, 2
     adc:	fb0cd2        	l8ui	a13, a12, 251
     adf:	fa0ce2        	l8ui	a14, a12, 250
     ae2:	992b           	addi.n	a9, a9, 2
     ae4:	11dd80        	slli	a13, a13, 8
     ae7:	20dde0        	or	a13, a13, a14
     aea:	8ddd           	ae_sext16	a13, a13
     aec:	fa49d2        	s8i	a13, a9, 250
     aef:	41d8d0        	srli	a13, a13, 8
     af2:	fb49d2        	s8i	a13, a9, 251

00000af5 <SMART_PITCH_PLC_PROCESS+0x795>:
     af5:	ffc986        	j	a1f <SMART_PITCH_PLC_PROCESS+0x6bf>

00000af8 <SMART_PITCH_PLC_PROCESS+0x798>:
	...

00000af9 <SMART_PITCH_PLC_PROCESS+0x799>:
     af9:	fd4f91        	l32r	a9, 38 (7fff <_end+0x6433>)

00000afc <SMART_PITCH_PLC_PROCESS+0x79c>:
     afc:	21d150        	srai	a13, a5, 1
     aff:	1b6492        	s32i	a9, a4, 108
     b02:	02d4b2        	addmi	a11, a4, 0x200
     b05:	02d4f2        	addmi	a15, a4, 0x200
     b08:	ffcb           	addi.n	a15, a15, 12
     b0a:	2ccbb2        	addi	a11, a11, 44
     b0d:	01d492        	addmi	a9, a4, 0x100
     b10:	70c992        	addi	a9, a9, 112
     b13:	09cd           	mov.n	a12, a9
     b15:	17ad76        	loopgtz	a13, b30 <SMART_PITCH_PLC_PROCESS+0x7d0>
     b18:	44eb14        	ae_lp16x2f.iu	aep6, a11, 4
     b1b:	44df14        	ae_lp16x2f.iu	aep5, a15, 4
     b1e:	025e74        	ae_mulp24s.ll	aeq1, aep5, aep6
     b21:	02d644        	ae_mulp24s.hh	aeq2, aep5, aep6
     b24:	369804        	ae_trunca32q48	a8, aeq1
     b27:	36aa04        	ae_trunca32q48	a10, aeq2
     b2a:	0ca9           	s32i.n	a10, a12, 0
     b2c:	1c89           	s32i.n	a8, a12, 4
     b2e:	cc8b           	addi.n	a12, a12, 8

00000b30 <SMART_PITCH_PLC_PROCESS+0x7d0>:
     b30:	02d4c2        	addmi	a12, a4, 0x200
     b33:	07d4b2        	addmi	a11, a4, 0x700
     b36:	02d4f2        	addmi	a15, a4, 0x200
     b39:	eccff2        	addi	a15, a15, -20
     b3c:	dccbb2        	addi	a11, a11, -36
     b3f:	b0ccc2        	addi	a12, a12, -80
     b42:	17ad76        	loopgtz	a13, b5d <SMART_PITCH_PLC_PROCESS+0x7fd>
     b45:	448b14        	ae_lp16x2f.iu	aep0, a11, 4
     b48:	44ff14        	ae_lp16x2f.iu	aep7, a15, 4
     b4b:	02f874        	ae_mulp24s.ll	aeq3, aep7, aep0
     b4e:	027044        	ae_mulp24s.hh	aeq0, aep7, aep0
     b51:	36b804        	ae_trunca32q48	a8, aeq3
     b54:	368a04        	ae_trunca32q48	a10, aeq0
     b57:	0ca9           	s32i.n	a10, a12, 0
     b59:	1c89           	s32i.n	a8, a12, 4
     b5b:	cc8b           	addi.n	a12, a12, 8
     b5d:	20f220        	or	a15, a2, a2
     b60:	70c4c2        	addi	a12, a4, 112
     b63:	6cc4b2        	addi	a11, a4, 108
     b66:	40c172        	addi	a7, a1, 64
     b69:	68a576        	loopgtz	a5, bd5 <SMART_PITCH_PLC_PROCESS+0x875>
     b6c:	406b04        	ae_lp16f.i	aep6, a11, 0
     b6f:	400c04        	ae_lp16f.i	aep0, a12, 0
     b72:	420982        	l8ui	a8, a9, 66
     b75:	0309a2        	l8ui	a10, a9, 3
     b78:	0209d2        	l8ui	a13, a9, 2
     b7b:	0109e2        	l8ui	a14, a9, 1
     b7e:	11aa80        	slli	a10, a10, 8
     b81:	20aad0        	or	a10, a10, a13
     b84:	11aa80        	slli	a10, a10, 8
     b87:	0009d2        	l8ui	a13, a9, 0
     b8a:	20aae0        	or	a10, a10, a14
     b8d:	4309e2        	l8ui	a14, a9, 67
     b90:	11aa80        	slli	a10, a10, 8
     b93:	20aad0        	or	a10, a10, a13
     b96:	21afa0        	srai	a10, a10, 15
     b99:	11ee80        	slli	a14, a14, 8
     b9c:	1061a2        	s32i	a10, a1, 64
     b9f:	407704        	ae_lp16f.i	aep7, a7, 0
     ba2:	20ee80        	or	a14, a14, a8
     ba5:	4109a2        	l8ui	a10, a9, 65
     ba8:	400982        	l8ui	a8, a9, 64
     bab:	11ee80        	slli	a14, a14, 8
     bae:	20eea0        	or	a14, a14, a10
     bb1:	11ee80        	slli	a14, a14, 8
     bb4:	20ee80        	or	a14, a14, a8
     bb7:	21efe0        	srai	a14, a14, 15
     bba:	1061e2        	s32i	a14, a1, 64
     bbd:	ea1fac570f 	{ ae_lp16f.i	aep5, a7, 0; ae_mulp24s.ll	aeq0, aep7, aep0 }
     bc2:	02de74        	ae_mulp24s.ll	aeq3, aep5, aep6
     bc5:	368a04        	ae_trunca32q48	a10, aeq0
     bc8:	36bd04        	ae_trunca32q48	a13, aeq3
     bcb:	994b           	addi.n	a9, a9, 4
     bcd:	aada           	add.n	a10, a10, a13
     bcf:	21afa0        	srai	a10, a10, 15
     bd2:	09cfa4        	ae_s16ip	a10, a15, 2
     bd5:	c0c650        	sub	a12, a6, a5
     bd8:	021ce6        	bgei	a12, 1, bde <SMART_PITCH_PLC_PROCESS+0x87e>
     bdb:	ff3d06        	j	8d3 <SMART_PITCH_PLC_PROCESS+0x573>
     bde:	909520        	addx2	a9, a5, a2
     be1:	1b24d2        	l32i	a13, a4, 108
     be4:	90b540        	addx2	a11, a5, a4
     be7:	06dbb2        	addmi	a11, a11, 0x600
     bea:	e4cbb2        	addi	a11, a11, -28
     bed:	18ac76        	loopgtz	a12, c09 <SMART_PITCH_PLC_PROCESS+0x8a9>
     bf0:	fd0be2        	l8ui	a14, a11, 253
     bf3:	fc0bf2        	l8ui	a15, a11, 252
     bf6:	11ee80        	slli	a14, a14, 8
     bf9:	20eef0        	or	a14, a14, a15
     bfc:	8eed           	ae_sext16	a14, a14
     bfe:	82eed0        	mull	a14, a14, a13
     c01:	bb2b           	addi.n	a11, a11, 2
     c03:	21efe0        	srai	a14, a14, 15
     c06:	09c9e4        	ae_s16ip	a14, a9, 2
     c09:	ff3186        	j	8d3 <SMART_PITCH_PLC_PROCESS+0x573>

00000c0c <SMART_PITCH_PLC_PROCESS+0x8ac>:
     c0c:	fd0bb1        	l32r	a11, 38 (7fff <_end+0x6433>)

00000c0f <SMART_PITCH_PLC_PROCESS+0x8af>:
     c0f:	1724c2        	l32i	a12, a4, 92
     c12:	05dd           	mov.n	a13, a5
     c14:	04fd           	mov.n	a15, a4
     c16:	fd0c81        	l32r	a8, 48 (1458 <getfespeech>)
     c19:	1b64b2        	s32i	a11, a4, 108
     c1c:	122492        	l32i	a9, a4, 72
     c1f:	e47a           	add.n	a14, a4, a7
     c21:	1361e2        	s32i	a14, a1, 76
     c24:	1a6192        	s32i	a9, a1, 104
     c27:	1624b2        	l32i	a11, a4, 88
     c2a:	0ead           	mov.n	a10, a14
     c2c:	0008e0        	callx8	a8
     c2f:	01cd           	mov.n	a12, a1
     c31:	217150        	srai	a7, a5, 1
     c34:	1824a2        	l32i	a10, a4, 96
     c37:	1a2182        	l32i	a8, a1, 104
     c3a:	2224e2        	l32i	a14, a4, 136
     c3d:	1624d2        	l32i	a13, a4, 88
     c40:	02d4f2        	addmi	a15, a4, 0x200
     c43:	06d4b2        	addmi	a11, a4, 0x600
     c46:	64cbb2        	addi	a11, a11, 100
     c49:	ffcb           	addi.n	a15, a15, 12
     c4b:	1861f2        	s32i	a15, a1, 96
     c4e:	ddea           	add.n	a13, a13, a14
     c50:	8e8a           	add.n	a8, a14, a8
     c52:	126482        	s32i	a8, a4, 72
     c55:	1664d2        	s32i	a13, a4, 88
     c58:	c0aad0        	sub	a10, a10, a13
     c5b:	1764a2        	s32i	a10, a4, 92
     c5e:	17a776        	loopgtz	a7, c79 <SMART_PITCH_PLC_PROCESS+0x919>
     c61:	44ab14        	ae_lp16x2f.iu	aep2, a11, 4
     c64:	449f14        	ae_lp16x2f.iu	aep1, a15, 4
     c67:	021a74        	ae_mulp24s.ll	aeq1, aep1, aep2
     c6a:	029244        	ae_mulp24s.hh	aeq2, aep1, aep2
     c6d:	369d04        	ae_trunca32q48	a13, aeq1
     c70:	36ae04        	ae_trunca32q48	a14, aeq2
     c73:	0ce9           	s32i.n	a14, a12, 0
     c75:	1cd9           	s32i.n	a13, a12, 4
     c77:	cc8b           	addi.n	a12, a12, 8

00000c79 <SMART_PITCH_PLC_PROCESS+0x919>:
     c79:	b46a           	add.n	a11, a4, a6
     c7b:	090c           	movi.n	a9, 0
     c7d:	02d4f2        	addmi	a15, a4, 0x200
     c80:	eccff2        	addi	a15, a15, -20
     c83:	1761f2        	s32i	a15, a1, 92
     c86:	2aa576        	loopgtz	a5, cb4 <SMART_PITCH_PLC_PROCESS+0x954>
     c89:	1724c2        	l32i	a12, a4, 92
     c8c:	c0cc50        	sub	a12, a12, a5
     c8f:	c9ca           	add.n	a12, a9, a12
     c91:	90cc40        	addx2	a12, a12, a4
     c94:	cc6a           	add.n	a12, a12, a6
     c96:	01dcc2        	addmi	a12, a12, 0x100
     c99:	010ca2        	l8ui	a10, a12, 1
     c9c:	000cc2        	l8ui	a12, a12, 0
     c9f:	991b           	addi.n	a9, a9, 1
     ca1:	11aa80        	slli	a10, a10, 8
     ca4:	20aac0        	or	a10, a10, a12
     ca7:	8aad           	ae_sext16	a10, a10
     ca9:	4188a0        	srli	a8, a10, 8
     cac:	004ba2        	s8i	a10, a11, 0
     caf:	014b82        	s8i	a8, a11, 1
     cb2:	bb2b           	addi.n	a11, a11, 2
     cb4:	fcc1c2        	addi	a12, a1, -4
     cb7:	01d4b2        	addmi	a11, a4, 0x100
     cba:	2ccbb2        	addi	a11, a11, 44
     cbd:	1ba776        	loopgtz	a7, cdc <SMART_PITCH_PLC_PROCESS+0x97c>
     cc0:	44cb14        	ae_lp16x2f.iu	aep4, a11, 4
     cc3:	44bf14        	ae_lp16x2f.iu	aep3, a15, 4
     cc6:	601c14        	ae_lq32f.iu	aeq0, a12, 4
     cc9:	023404        	ae_mulap24s.hh	aeq0, aep3, aep4
     ccc:	368e04        	ae_trunca32q48	a14, aeq0
     ccf:	0ce9           	s32i.n	a14, a12, 0
     cd1:	60dc14        	ae_lq32f.iu	aeq3, a12, 4
     cd4:	02bc34        	ae_mulap24s.ll	aeq3, aep3, aep4
     cd7:	36bd04        	ae_trunca32q48	a13, aeq3
     cda:	0cd9           	s32i.n	a13, a12, 0
     cdc:	01bd           	mov.n	a11, a1
     cde:	c09350        	sub	a9, a3, a5
     ce1:	909940        	addx2	a9, a9, a4
     ce4:	996a           	add.n	a9, a9, a6
     ce6:	14a576        	loopgtz	a5, cfe <SMART_PITCH_PLC_PROCESS+0x99e>
     ce9:	01d9c2        	addmi	a12, a9, 0x100
     cec:	0ba8           	l32i.n	a10, a11, 0
     cee:	992b           	addi.n	a9, a9, 2
     cf0:	bb4b           	addi.n	a11, a11, 4
     cf2:	21afa0        	srai	a10, a10, 15
     cf5:	004ca2        	s8i	a10, a12, 0
     cf8:	41a8a0        	srli	a10, a10, 8
     cfb:	014ca2        	s8i	a10, a12, 1
     cfe:	1321a2        	l32i	a10, a1, 76
     d01:	1624b2        	l32i	a11, a4, 88
     d04:	1724c2        	l32i	a12, a4, 92
     d07:	04fd           	mov.n	a15, a4
     d09:	242162        	l32i	a6, a1, 144
     d0c:	07d4e2        	addmi	a14, a4, 0x700
     d0f:	fcce81        	l32r	a8, 48 (1458 <getfespeech>)
     d12:	e0cee2        	addi	a14, a14, -32
     d15:	20d660        	or	a13, a6, a6
     d18:	0008e0        	callx8	a8
     d1b:	1821f2        	l32i	a15, a1, 96
     d1e:	01d492        	addmi	a9, a4, 0x100
     d21:	02d4b2        	addmi	a11, a4, 0x200
     d24:	2ccbb2        	addi	a11, a11, 44
     d27:	70c992        	addi	a9, a9, 112
     d2a:	20c990        	or	a12, a9, a9
     d2d:	17a776        	loopgtz	a7, d48 <SMART_PITCH_PLC_PROCESS+0x9e8>
     d30:	44eb14        	ae_lp16x2f.iu	aep6, a11, 4
     d33:	44df14        	ae_lp16x2f.iu	aep5, a15, 4
     d36:	025e74        	ae_mulp24s.ll	aeq1, aep5, aep6
     d39:	02d644        	ae_mulp24s.hh	aeq2, aep5, aep6
     d3c:	369d04        	ae_trunca32q48	a13, aeq1
     d3f:	36ae04        	ae_trunca32q48	a14, aeq2
     d42:	0ce9           	s32i.n	a14, a12, 0
     d44:	1cd9           	s32i.n	a13, a12, 4
     d46:	cc8b           	addi.n	a12, a12, 8
     d48:	1721f2        	l32i	a15, a1, 92
     d4b:	02d4c2        	addmi	a12, a4, 0x200
     d4e:	07d4b2        	addmi	a11, a4, 0x700
     d51:	dccbb2        	addi	a11, a11, -36
     d54:	b0ccc2        	addi	a12, a12, -80
     d57:	17a776        	loopgtz	a7, d72 <SMART_PITCH_PLC_PROCESS+0xa12>
     d5a:	448b14        	ae_lp16x2f.iu	aep0, a11, 4
     d5d:	44ff14        	ae_lp16x2f.iu	aep7, a15, 4
     d60:	02f874        	ae_mulp24s.ll	aeq3, aep7, aep0
     d63:	027044        	ae_mulp24s.hh	aeq0, aep7, aep0
     d66:	36bd04        	ae_trunca32q48	a13, aeq3
     d69:	368e04        	ae_trunca32q48	a14, aeq0
     d6c:	0ce9           	s32i.n	a14, a12, 0
     d6e:	1cd9           	s32i.n	a13, a12, 4
     d70:	cc8b           	addi.n	a12, a12, 8
     d72:	1d24f2        	l32i	a15, a4, 116
     d75:	ff0b           	addi.n	a15, a15, -1
     d77:	ba7f16        	beqz	a15, 922 <SMART_PITCH_PLC_PROCESS+0x5c2>
     d7a:	e488           	l32i.n	a8, a4, 56
     d7c:	ffc882        	addi	a8, a8, -1
     d7f:	b9f816        	beqz	a8, 922 <SMART_PITCH_PLC_PROCESS+0x5c2>
     d82:	20f220        	or	a15, a2, a2
     d85:	45a576        	loopgtz	a5, dce <SMART_PITCH_PLC_PROCESS+0xa6e>
     d88:	0209e2        	l8ui	a14, a9, 2
     d8b:	0309c2        	l8ui	a12, a9, 3
     d8e:	4309d2        	l8ui	a13, a9, 67
     d91:	420972        	l8ui	a7, a9, 66
     d94:	11cc80        	slli	a12, a12, 8
     d97:	11dd80        	slli	a13, a13, 8
     d9a:	20dd70        	or	a13, a13, a7
     d9d:	20cce0        	or	a12, a12, a14
     da0:	010972        	l8ui	a7, a9, 1
     da3:	4109e2        	l8ui	a14, a9, 65
     da6:	11cc80        	slli	a12, a12, 8
     da9:	11dd80        	slli	a13, a13, 8
     dac:	20dde0        	or	a13, a13, a14
     daf:	20cc70        	or	a12, a12, a7
     db2:	11cc80        	slli	a12, a12, 8
     db5:	11dd80        	slli	a13, a13, 8
     db8:	400972        	l8ui	a7, a9, 64
     dbb:	0009e2        	l8ui	a14, a9, 0
     dbe:	994b           	addi.n	a9, a9, 4
     dc0:	20dd70        	or	a13, a13, a7
     dc3:	20cce0        	or	a12, a12, a14
     dc6:	ccda           	add.n	a12, a12, a13
     dc8:	21cfc0        	srai	a12, a12, 15
     dcb:	09cfc4        	ae_s16ip	a12, a15, 2
     dce:	909520        	addx2	a9, a5, a2
     dd1:	c0e650        	sub	a14, a6, a5
     dd4:	90b540        	addx2	a11, a5, a4
     dd7:	06dbb2        	addmi	a11, a11, 0x600
     dda:	e4cbb2        	addi	a11, a11, -28
     ddd:	12ae76        	loopgtz	a14, df3 <SMART_PITCH_PLC_PROCESS+0xa93>
     de0:	fd0bd2        	l8ui	a13, a11, 253
     de3:	fc0be2        	l8ui	a14, a11, 252
     de6:	bb2b           	addi.n	a11, a11, 2
     de8:	11dd80        	slli	a13, a13, 8
     deb:	20dde0        	or	a13, a13, a14
     dee:	8ddd           	ae_sext16	a13, a13
     df0:	09c9d4        	ae_s16ip	a13, a9, 2
     df3:	fef4c6        	j	9ca <SMART_PITCH_PLC_PROCESS+0x66a>

00000df6 <SMART_PITCH_PLC_PROCESS+0xa96>:
     df6:	1164a2        	s32i	a10, a4, 68
     df9:	11baf0        	slli	a11, a10, 1
     dfc:	fd8f06        	j	43c <SMART_PITCH_PLC_PROCESS+0xdc>

00000dff <SMART_PITCH_PLC_PROCESS+0xa9f>:
	...

00000e00 <sqrt_q26>:
     e00:	004136        	entry	a1, 32
     e03:	fc9581        	l32r	a8, 58 (13a4 <HIFI_sqrt>)
     e06:	02bd           	mov.n	a11, a2
     e08:	5a0c           	movi.n	a10, 5
     e0a:	0008e0        	callx8	a8
     e0d:	0c6ae6        	bgei	a10, 6, e1d <sqrt_q26+0x1d>
     e10:	590c           	movi.n	a9, 5
     e12:	c099a0        	sub	a9, a9, a10
     e15:	400900        	ssr	a9
     e18:	b120b0        	sra	a2, a11
     e1b:	f01d           	retw.n

00000e1d <sqrt_q26+0x1d>:
     e1d:	fc8f21        	l32r	a2, 5c (7fffffff <_end+0x7fffe433>)
     e20:	f01d           	retw.n

00000e22 <sqrt_q26+0x22>:
	...

00000e24 <find_pitch_ini>:
     e24:	004136        	entry	a1, 32
     e27:	f01d           	retw.n

00000e29 <find_pitch_ini+0x5>:
     e29:	000000                                        ...

00000e2c <find_pitch>:
     e2c:	00e136        	entry	a1, 112
     e2f:	c129           	s32i.n	a2, a1, 48
     e31:	03bd           	mov.n	a11, a3
     e33:	8169           	s32i.n	a6, a1, 32
     e35:	4159           	s32i.n	a5, a1, 16
     e37:	2179           	s32i.n	a7, a1, 8
     e39:	045d           	mov.n	a5, a4
     e3b:	5414a6        	blti	a4, 1, e93 <find_pitch+0x67>
     e3e:	2179           	s32i.n	a7, a1, 8
     e40:	d14b           	addi.n	a13, a1, 4
     e42:	c09740        	sub	a9, a7, a4
     e45:	c0c730        	sub	a12, a7, a3
     e48:	e41b           	addi.n	a14, a4, 1
     e4a:	030c           	movi.n	a3, 0
     e4c:	21e1e0        	srai	a14, a14, 1
     e4f:	909920        	addx2	a9, a9, a2
     e52:	90cc20        	addx2	a12, a12, a2
     e55:	ffdcc2        	addmi	a12, a12, 0xffffff00
     e58:	ffd992        	addmi	a9, a9, 0xffffff00
     e5b:	020c           	movi.n	a2, 0
     e5d:	2dae76        	loopgtz	a14, e8e <find_pitch+0x62>
     e60:	7f99e2        	l16si	a14, a9, 254
     e63:	7f9ca2        	l16si	a10, a12, 254
     e66:	994b           	addi.n	a9, a9, 4
     e68:	11e9           	s32i.n	a14, a1, 4
     e6a:	01a9           	s32i.n	a10, a1, 0
     e6c:	402104        	ae_lp16f.i	aep2, a1, 0
     e6f:	403104        	ae_lp16f.i	aep3, a1, 0
     e72:	ea6bec1d0f 	{ ae_lp16f.i	aep1, a13, 0; ae_mulp24s.ll	aeq1, aep2, aep3 }
     e77:	400104        	ae_lp16f.i	aep0, a1, 0
     e7a:	ea23bf18ef 	{ ae_trunca32q48	a8, aeq1; ae_mulp24s.ll	aeq0, aep0, aep1 }
     e7f:	cc4b           	addi.n	a12, a12, 4
     e81:	368f04        	ae_trunca32q48	a15, aeq0
     e84:	218480        	srai	a8, a8, 4
     e87:	228a           	add.n	a2, a2, a8
     e89:	21f4f0        	srai	a15, a15, 4
     e8c:	33fa           	add.n	a3, a3, a15

00000e8e <find_pitch+0x62>:
     e8e:	31b9           	s32i.n	a11, a1, 12
     e90:	000146        	j	e99 <find_pitch+0x6d>

00000e93 <find_pitch+0x67>:
     e93:	030c           	movi.n	a3, 0
     e95:	020c           	movi.n	a2, 0
     e97:	31b9           	s32i.n	a11, a1, 12

00000e99 <find_pitch+0x6d>:
     e99:	90a550        	addx2	a10, a5, a5
     e9c:	fc7181        	l32r	a8, 60 (e00 <sqrt_q26>)
     e9f:	b0aa50        	addx8	a10, a10, a5
     ea2:	21a4a0        	srai	a10, a10, 4
     ea5:	d1a9           	s32i.n	a10, a1, 52
     ea7:	53aa20        	max	a10, a10, a2
     eaa:	0008e0        	callx8	a8
     ead:	21bf30        	srai	a11, a3, 15
     eb0:	fc6581        	l32r	a8, 44 (1490 <__divdi3>)
     eb3:	0add           	mov.n	a13, a10
     eb5:	0dcd           	mov.n	a12, a13
     eb7:	01a3f0        	slli	a10, a3, 17
     eba:	31dfd0        	srai	a13, a13, 31
     ebd:	0008e0        	callx8	a8
     ec0:	81e8           	l32i.n	a14, a1, 32
     ec2:	e1a9           	s32i.n	a10, a1, 56
     ec4:	022ee6        	bgei	a14, 2, eca <find_pitch+0x9e>
     ec7:	0037c6        	j	faa <find_pitch+0x17e>
     eca:	2b0c           	movi.n	a11, 2
     ecc:	3178           	l32i.n	a7, a1, 12
     ece:	2168           	l32i.n	a6, a1, 8
     ed0:	040c           	movi.n	a4, 0
     ed2:	080c           	movi.n	a8, 0
     ed4:	5189           	s32i.n	a8, a1, 20
     ed6:	442b           	addi.n	a4, a4, 2
     ed8:	c188           	l32i.n	a8, a1, 48
     eda:	c09650        	sub	a9, a6, a5
     edd:	c06670        	sub	a6, a6, a7
     ee0:	a169           	s32i.n	a6, a1, 40
     ee2:	751b           	addi.n	a7, a5, 1
     ee4:	909980        	addx2	a9, a9, a8
     ee7:	ffd992        	addmi	a9, a9, 0xffffff00
     eea:	217170        	srai	a7, a7, 1
     eed:	7179           	s32i.n	a7, a1, 28
     eef:	b199           	s32i.n	a9, a1, 44
     ef1:	765a           	add.n	a7, a6, a5
     ef3:	907780        	addx2	a7, a7, a8
     ef6:	906680        	addx2	a6, a6, a8
     ef9:	ffd662        	addmi	a6, a6, 0xffffff00
     efc:	ffd772        	addmi	a7, a7, 0xffffff00

00000eff <find_pitch+0xd3>:
     eff:	d14b           	addi.n	a13, a1, 4
     f01:	7f9792        	l16si	a9, a7, 254
     f04:	7f96a2        	l16si	a10, a6, 254
     f07:	11a9           	s32i.n	a10, a1, 4
     f09:	0199           	s32i.n	a9, a1, 0
     f0b:	404104        	ae_lp16f.i	aep4, a1, 0
     f0e:	406d04        	ae_lp16f.i	aep6, a13, 0
     f11:	407d04        	ae_lp16f.i	aep7, a13, 0
     f14:	ebfbec510f 	{ ae_lp16f.i	aep5, a1, 0; ae_mulp24s.ll	aeq3, aep6, aep7 }
     f19:	ebb380477e 	{ addi	a7, a7, 4; ae_mulp24s.ll	aeq2, aep4, aep5 }
     f1e:	36b804        	ae_trunca32q48	a8, aeq3
     f21:	36a904        	ae_trunca32q48	a9, aeq2
     f24:	664b           	addi.n	a6, a6, 4
     f26:	218480        	srai	a8, a8, 4
     f29:	219490        	srai	a9, a9, 4
     f2c:	229a           	add.n	a2, a2, a9
     f2e:	c02280        	sub	a2, a2, a8
     f31:	3915a6        	blti	a5, 1, f6e <find_pitch+0x142>
     f34:	030c           	movi.n	a3, 0
     f36:	a1c8           	l32i.n	a12, a1, 40
     f38:	c1e8           	l32i.n	a14, a1, 48
     f3a:	b198           	l32i.n	a9, a1, 44
     f3c:	cc4a           	add.n	a12, a12, a4
     f3e:	90cce0        	addx2	a12, a12, a14
     f41:	71e8           	l32i.n	a14, a1, 28
     f43:	0f61b2        	s32i	a11, a1, 60
     f46:	ffdcc2        	addmi	a12, a12, 0xffffff00
     f49:	1eae76        	loopgtz	a14, f6b <find_pitch+0x13f>
     f4c:	7f9cf2        	l16si	a15, a12, 254
     f4f:	7f9982        	l16si	a8, a9, 254
     f52:	0189           	s32i.n	a8, a1, 0
     f54:	11f9           	s32i.n	a15, a1, 4
     f56:	401d04        	ae_lp16f.i	aep1, a13, 0
     f59:	400104        	ae_lp16f.i	aep0, a1, 0
     f5c:	020174        	ae_mulp24s.ll	aeq0, aep0, aep1
     f5f:	368e04        	ae_trunca32q48	a14, aeq0
     f62:	994b           	addi.n	a9, a9, 4
     f64:	cc4b           	addi.n	a12, a12, 4
     f66:	21e4e0        	srai	a14, a14, 4
     f69:	33ea           	add.n	a3, a3, a14

00000f6b <find_pitch+0x13f>:
     f6b:	0000c6        	j	f72 <find_pitch+0x146>

00000f6e <find_pitch+0x142>:
     f6e:	030c           	movi.n	a3, 0
     f70:	f1b9           	s32i.n	a11, a1, 60

00000f72 <find_pitch+0x146>:
     f72:	d1a8           	l32i.n	a10, a1, 52
     f74:	fc3b81        	l32r	a8, 60 (e00 <sqrt_q26>)
     f77:	53aa20        	max	a10, a10, a2
     f7a:	0008e0        	callx8	a8
     f7d:	21bf30        	srai	a11, a3, 15
     f80:	fc3181        	l32r	a8, 44 (1490 <__divdi3>)
     f83:	0add           	mov.n	a13, a10
     f85:	0dcd           	mov.n	a12, a13
     f87:	01a3f0        	slli	a10, a3, 17
     f8a:	31dfd0        	srai	a13, a13, 31
     f8d:	0008e0        	callx8	a8
     f90:	f1b8           	l32i.n	a11, a1, 60
     f92:	e1e8           	l32i.n	a14, a1, 56
     f94:	8198           	l32i.n	a9, a1, 32
     f96:	442b           	addi.n	a4, a4, 2
     f98:	032ae7        	blt	a10, a14, f9f <find_pitch+0x173>
     f9b:	51b9           	s32i.n	a11, a1, 20
     f9d:	e1a9           	s32i.n	a10, a1, 56

00000f9f <find_pitch+0x173>:
     f9f:	bb2b           	addi.n	a11, a11, 2
     fa1:	0229b7        	blt	a9, a11, fa7 <find_pitch+0x17b>
     fa4:	ffd5c6        	j	eff <find_pitch+0xd3>
     fa7:	0000c6        	j	fae <find_pitch+0x182>

00000faa <find_pitch+0x17e>:
     faa:	0a0c           	movi.n	a10, 0
     fac:	51a9           	s32i.n	a10, a1, 20

00000fae <find_pitch+0x182>:
     fae:	5148           	l32i.n	a4, a1, 20
     fb0:	030c           	movi.n	a3, 0
     fb2:	440b           	addi.n	a4, a4, -1
     fb4:	534340        	max	a4, a3, a4
     fb7:	5215a6        	blti	a5, 1, 100d <find_pitch+0x1e1>
     fba:	020c           	movi.n	a2, 0
     fbc:	31c8           	l32i.n	a12, a1, 12
     fbe:	2198           	l32i.n	a9, a1, 8
     fc0:	b14b           	addi.n	a11, a1, 4
     fc2:	c1d8           	l32i.n	a13, a1, 48
     fc4:	c0c9c0        	sub	a12, a9, a12
     fc7:	c09950        	sub	a9, a9, a5
     fca:	80c4c0        	add	a12, a4, a12
     fcd:	90ccd0        	addx2	a12, a12, a13
     fd0:	9099d0        	addx2	a9, a9, a13
     fd3:	ffd992        	addmi	a9, a9, 0xffffff00
     fd6:	ffdcc2        	addmi	a12, a12, 0xffffff00
     fd9:	2da576        	loopgtz	a5, 100a <find_pitch+0x1de>
     fdc:	7f9982        	l16si	a8, a9, 254
     fdf:	7f9cf2        	l16si	a15, a12, 254
     fe2:	992b           	addi.n	a9, a9, 2
     fe4:	1189           	s32i.n	a8, a1, 4
     fe6:	01f9           	s32i.n	a15, a1, 0
     fe8:	404104        	ae_lp16f.i	aep4, a1, 0
     feb:	405104        	ae_lp16f.i	aep5, a1, 0
     fee:	ebb3ac3b0f 	{ ae_lp16f.i	aep3, a11, 0; ae_mulp24s.ll	aeq2, aep4, aep5 }
     ff3:	402104        	ae_lp16f.i	aep2, a1, 0
     ff6:	ea6bff2eef 	{ ae_trunca32q48	a14, aeq2; ae_mulp24s.ll	aeq1, aep2, aep3 }
     ffb:	cc2b           	addi.n	a12, a12, 2
     ffd:	369d04        	ae_trunca32q48	a13, aeq1
    1000:	21e4e0        	srai	a14, a14, 4
    1003:	22ea           	add.n	a2, a2, a14
    1005:	21d4d0        	srai	a13, a13, 4
    1008:	33da           	add.n	a3, a3, a13

0000100a <find_pitch+0x1de>:
    100a:	000046        	j	100f <find_pitch+0x1e3>

0000100d <find_pitch+0x1e1>:
    100d:	020c           	movi.n	a2, 0

0000100f <find_pitch+0x1e3>:
    100f:	d1a8           	l32i.n	a10, a1, 52
    1011:	fc1381        	l32r	a8, 60 (e00 <sqrt_q26>)
    1014:	53aa20        	max	a10, a10, a2
    1017:	0008e0        	callx8	a8
    101a:	21bf30        	srai	a11, a3, 15
    101d:	fc0981        	l32r	a8, 44 (1490 <__divdi3>)
    1020:	0add           	mov.n	a13, a10
    1022:	0dcd           	mov.n	a12, a13
    1024:	01a3f0        	slli	a10, a3, 17
    1027:	31dfd0        	srai	a13, a13, 31
    102a:	0008e0        	callx8	a8
    102d:	e1a9           	s32i.n	a10, a1, 56
    102f:	6149           	s32i.n	a4, a1, 24
    1031:	51d8           	l32i.n	a13, a1, 20
    1033:	81c8           	l32i.n	a12, a1, 32
    1035:	b41b           	addi.n	a11, a4, 1
    1037:	dd1b           	addi.n	a13, a13, 1
    1039:	43ccd0        	min	a12, a12, a13
    103c:	0224c7        	blt	a4, a12, 1042 <find_pitch+0x216>
    103f:	003546        	j	1118 <find_pitch+0x2ec>
    1042:	ac1b           	addi.n	a10, a12, 1
    1044:	3198           	l32i.n	a9, a1, 12
    1046:	91a9           	s32i.n	a10, a1, 36
    1048:	21a8           	l32i.n	a10, a1, 8
    104a:	c188           	l32i.n	a8, a1, 48
    104c:	c09a90        	sub	a9, a10, a9
    104f:	a199           	s32i.n	a9, a1, 40
    1051:	649a           	add.n	a6, a4, a9
    1053:	795a           	add.n	a7, a9, a5
    1055:	c0aa50        	sub	a10, a10, a5
    1058:	90aa80        	addx2	a10, a10, a8
    105b:	747a           	add.n	a7, a4, a7
    105d:	906680        	addx2	a6, a6, a8
    1060:	ffd662        	addmi	a6, a6, 0xffffff00
    1063:	441b           	addi.n	a4, a4, 1
    1065:	907780        	addx2	a7, a7, a8
    1068:	ffdaa2        	addmi	a10, a10, 0xffffff00
    106b:	b1a9           	s32i.n	a10, a1, 44
    106d:	ffd772        	addmi	a7, a7, 0xffffff00

00001070 <find_pitch+0x244>:
    1070:	7f97c2        	l16si	a12, a7, 254
    1073:	7f96a2        	l16si	a10, a6, 254
    1076:	772b           	addi.n	a7, a7, 2
    1078:	01c9           	s32i.n	a12, a1, 0
    107a:	400104        	ae_lp16f.i	aep0, a1, 0
    107d:	401104        	ae_lp16f.i	aep1, a1, 0
    1080:	01a9           	s32i.n	a10, a1, 0
    1082:	ea23ac610f 	{ ae_lp16f.i	aep6, a1, 0; ae_mulp24s.ll	aeq0, aep0, aep1 }
    1087:	407104        	ae_lp16f.i	aep7, a1, 0
    108a:	ebfbff09ef 	{ ae_trunca32q48	a9, aeq0; ae_mulp24s.ll	aeq3, aep6, aep7 }
    108f:	662b           	addi.n	a6, a6, 2
    1091:	36b804        	ae_trunca32q48	a8, aeq3
    1094:	219490        	srai	a9, a9, 4
    1097:	229a           	add.n	a2, a2, a9
    1099:	218480        	srai	a8, a8, 4
    109c:	c02280        	sub	a2, a2, a8
    109f:	3a15a6        	blti	a5, 1, 10dd <find_pitch+0x2b1>
    10a2:	030c           	movi.n	a3, 0
    10a4:	a1c8           	l32i.n	a12, a1, 40
    10a6:	c1d8           	l32i.n	a13, a1, 48
    10a8:	b198           	l32i.n	a9, a1, 44
    10aa:	cc4a           	add.n	a12, a12, a4
    10ac:	90ccd0        	addx2	a12, a12, a13
    10af:	ffdcc2        	addmi	a12, a12, 0xffffff00
    10b2:	04c1d2        	addi	a13, a1, 4
    10b5:	1ea576        	loopgtz	a5, 10d7 <find_pitch+0x2ab>
    10b8:	7f9cf2        	l16si	a15, a12, 254
    10bb:	7f9982        	l16si	a8, a9, 254
    10be:	0189           	s32i.n	a8, a1, 0
    10c0:	11f9           	s32i.n	a15, a1, 4
    10c2:	403d04        	ae_lp16f.i	aep3, a13, 0
    10c5:	402104        	ae_lp16f.i	aep2, a1, 0
    10c8:	022b74        	ae_mulp24s.ll	aeq1, aep2, aep3
    10cb:	369e04        	ae_trunca32q48	a14, aeq1
    10ce:	992b           	addi.n	a9, a9, 2
    10d0:	cc2b           	addi.n	a12, a12, 2
    10d2:	21e4e0        	srai	a14, a14, 4
    10d5:	33ea           	add.n	a3, a3, a14

000010d7 <find_pitch+0x2ab>:
    10d7:	1061b2        	s32i	a11, a1, 64
    10da:	000106        	j	10e2 <find_pitch+0x2b6>

000010dd <find_pitch+0x2b1>:
    10dd:	030c           	movi.n	a3, 0
    10df:	1061b2        	s32i	a11, a1, 64

000010e2 <find_pitch+0x2b6>:
    10e2:	d1a8           	l32i.n	a10, a1, 52
    10e4:	fbdf81        	l32r	a8, 60 (e00 <sqrt_q26>)
    10e7:	53aa20        	max	a10, a10, a2
    10ea:	0008e0        	callx8	a8
    10ed:	21bf30        	srai	a11, a3, 15
    10f0:	fbd581        	l32r	a8, 44 (1490 <__divdi3>)
    10f3:	0add           	mov.n	a13, a10
    10f5:	0dcd           	mov.n	a12, a13
    10f7:	01a3f0        	slli	a10, a3, 17
    10fa:	31dfd0        	srai	a13, a13, 31
    10fd:	0008e0        	callx8	a8
    1100:	1021b2        	l32i	a11, a1, 64
    1103:	e1e8           	l32i.n	a14, a1, 56
    1105:	9198           	l32i.n	a9, a1, 36
    1107:	441b           	addi.n	a4, a4, 1
    1109:	032ae7        	blt	a10, a14, 1110 <find_pitch+0x2e4>
    110c:	61b9           	s32i.n	a11, a1, 24
    110e:	e1a9           	s32i.n	a10, a1, 56

00001110 <find_pitch+0x2e4>:
    1110:	bb1b           	addi.n	a11, a11, 1
    1112:	c099b0        	sub	a9, a9, a11
    1115:	f57956        	bnez	a9, 1070 <find_pitch+0x244>

00001118 <find_pitch+0x2ec>:
    1118:	6188           	l32i.n	a8, a1, 24
    111a:	4128           	l32i.n	a2, a1, 16
    111c:	c02280        	sub	a2, a2, a8
    111f:	f01d           	retw.n

00001121 <find_pitch+0x2f5>:
    1121:	00000000 36000000                                .......

00001128 <Q_ubitlength>:
    1128:	004136        	entry	a1, 32
    112b:	fbce31        	l32r	a3, 64 (10000 <_end+0xe434>)
    112e:	f22230        	rems	a2, a2, a3
    1131:	f01d           	retw.n

00001133 <Q_ubitlength+0xb>:
	...

00001134 <plc_gainctrl_ini>:
    1134:	004136        	entry	a1, 32
    1137:	3222c2        	l32i	a12, a2, 200
    113a:	352232        	l32i	a3, a2, 212
    113d:	342282        	l32i	a8, a2, 208
    1140:	0b1c           	movi.n	a11, 16
    1142:	3822d2        	l32i	a13, a2, 224
    1145:	3722e2        	l32i	a14, a2, 220
    1148:	3622f2        	l32i	a15, a2, 216
    114b:	332292        	l32i	a9, a2, 204
    114e:	256292        	s32i	a9, a2, 148
    1151:	2862f2        	s32i	a15, a2, 160
    1154:	2962e2        	s32i	a14, a2, 164
    1157:	2a62d2        	s32i	a13, a2, 168
    115a:	2362b2        	s32i	a11, a2, 140
    115d:	266282        	s32i	a8, a2, 152
    1160:	276232        	s32i	a3, a2, 156
    1163:	fbc181        	l32r	a8, 68 (1128 <Q_ubitlength>)
    1166:	130c           	movi.n	a3, 1
    1168:	401c00        	ssl	a12
    116b:	a1a300        	sll	a10, a3
    116e:	aa0b           	addi.n	a10, a10, -1
    1170:	2462a2        	s32i	a10, a2, 144
    1173:	0008e0        	callx8	a8
    1176:	2522b2        	l32i	a11, a2, 148
    1179:	fbbb81        	l32r	a8, 68 (1128 <Q_ubitlength>)
    117c:	2462a2        	s32i	a10, a2, 144
    117f:	401b00        	ssl	a11
    1182:	2322b2        	l32i	a11, a2, 140
    1185:	a1a300        	sll	a10, a3
    1188:	aa0b           	addi.n	a10, a10, -1
    118a:	2562a2        	s32i	a10, a2, 148
    118d:	0008e0        	callx8	a8
    1190:	2562a2        	s32i	a10, a2, 148
    1193:	f01d           	retw.n

00001195 <plc_gainctrl_ini+0x61>:
    1195:	000000                                        ...

00001198 <plc_gainctrl>:
    1198:	004136        	entry	a1, 32
    119b:	d5a8           	l32i.n	a10, a5, 52
    119d:	2325b2        	l32i	a11, a5, 140
    11a0:	fbb281        	l32r	a8, 68 (1128 <Q_ubitlength>)
    11a3:	d0aa20        	subx2	a10, a10, a2
    11a6:	aa1b           	addi.n	a10, a10, 1
    11a8:	d5a9           	s32i.n	a10, a5, 52
    11aa:	0008e0        	callx8	a8
    11ad:	2525b2        	l32i	a11, a5, 148
    11b0:	d5a9           	s32i.n	a10, a5, 52
    11b2:	060c           	movi.n	a6, 0
    11b4:	420ab7        	bnone	a10, a11, 11fa <plc_gainctrl+0x62>
    11b7:	2325b2        	l32i	a11, a5, 140
    11ba:	070c           	movi.n	a7, 0
    11bc:	060c           	movi.n	a6, 0
    11be:	021be6        	bgei	a11, 1, 11c4 <plc_gainctrl+0x2c>
    11c1:	002506        	j	1259 <plc_gainctrl+0xc1>
    11c4:	020c           	movi.n	a2, 0

000011c6 <plc_gainctrl+0x2e>:
    11c6:	2425c2        	l32i	a12, a5, 144
    11c9:	6ccc           	bnez.n	a12, 11d3 <plc_gainctrl+0x3b>
    11cb:	176a07        	bbci	a10, 0, 11e6 <plc_gainctrl+0x4e>
    11ce:	661b           	addi.n	a6, a6, 1
    11d0:	000486        	j	11e6 <plc_gainctrl+0x4e>

000011d3 <plc_gainctrl+0x3b>:
    11d3:	10dca0        	and	a13, a12, a10
    11d6:	074ac7        	ball	a10, a12, 11e1 <plc_gainctrl+0x49>
    11d9:	096d07        	bbci	a13, 0, 11e6 <plc_gainctrl+0x4e>
    11dc:	661b           	addi.n	a6, a6, 1
    11de:	000106        	j	11e6 <plc_gainctrl+0x4e>

000011e1 <plc_gainctrl+0x49>:
    11e1:	fdc222        	addi	a2, a2, -3
    11e4:	061c           	movi.n	a6, 16
    11e6:	fba081        	l32r	a8, 68 (1128 <Q_ubitlength>)
    11e9:	21a1a0        	srai	a10, a10, 1
    11ec:	0008e0        	callx8	a8
    11ef:	2325b2        	l32i	a11, a5, 140
    11f2:	771b           	addi.n	a7, a7, 1
    11f4:	ce27b7        	blt	a7, a11, 11c6 <plc_gainctrl+0x2e>
    11f7:	000046        	j	11fc <plc_gainctrl+0x64>

000011fa <plc_gainctrl+0x62>:
    11fa:	d27c           	movi.n	a2, -3

000011fc <plc_gainctrl+0x64>:
    11fc:	2625c2        	l32i	a12, a5, 152
    11ff:	2a25e2        	l32i	a14, a5, 168
    1202:	272592        	l32i	a9, a5, 156
    1205:	72c6c0        	salt	a12, a6, a12
    1208:	11cc10        	slli	a12, a12, 15
    120b:	0ea967        	bge	a9, a6, 121d <plc_gainctrl+0x85>
    120e:	2825a2        	l32i	a10, a5, 160
    1211:	0226a7        	blt	a6, a10, 1217 <plc_gainctrl+0x7f>
    1214:	c066a0        	sub	a6, a6, a10

00001217 <plc_gainctrl+0x7f>:
    1217:	400600        	ssr	a6
    121a:	b1c0c0        	sra	a12, a12

0000121d <plc_gainctrl+0x85>:
    121d:	0226e6        	bgei	a6, 2, 1223 <plc_gainctrl+0x8b>
    1220:	fdc222        	addi	a2, a2, -3

00001223 <plc_gainctrl+0x8b>:
    1223:	3014a6        	blti	a4, 1, 1257 <plc_gainctrl+0xbf>
    1226:	1d0c           	movi.n	a13, 1
    1228:	0f0c           	movi.n	a15, 0
    122a:	29a476        	loopgtz	a4, 1257 <plc_gainctrl+0xbf>
    122d:	102582        	l32i	a8, a5, 64
    1230:	234d           	ae_l16si.n	a4, a3, 0
    1232:	2925a2        	l32i	a10, a5, 164
    1235:	c09c80        	sub	a9, a12, a8
    1238:	72b9d0        	salt	a11, a9, a13
    123b:	93aeb0        	movnez	a10, a14, a11
    123e:	aa2a           	add.n	a10, a10, a2
    1240:	53aaf0        	max	a10, a10, a15
    1243:	400a00        	ssr	a10
    1246:	b19090        	sra	a9, a9
    1249:	889a           	add.n	a8, a8, a9
    124b:	824480        	mull	a4, a4, a8
    124e:	106582        	s32i	a8, a5, 64
    1251:	214f40        	srai	a4, a4, 15
    1254:	09c344        	ae_s16ip	a4, a3, 2

00001257 <plc_gainctrl+0xbf>:
    1257:	f01d           	retw.n

00001259 <plc_gainctrl+0xc1>:
    1259:	020c           	movi.n	a2, 0
    125b:	060c           	movi.n	a6, 0
    125d:	ffe6c6        	j	11fc <plc_gainctrl+0x64>

00001260 <plcpitch_sim_init>:
    1260:	004136        	entry	a1, 32
    1263:	444c           	movi.n	a4, 68
    1265:	651c           	movi.n	a5, 22
    1267:	cca262        	movi	a6, 0x2cc
    126a:	072c           	movi.n	a7, 32
    126c:	1ea592        	movi	a9, 0x51e
    126f:	8f0c           	movi.n	a15, 8
    1271:	1a0c           	movi.n	a10, 1
    1273:	ee0c           	movi.n	a14, 14
    1275:	080c           	movi.n	a8, 0
    1277:	fd0c           	movi.n	a13, 15
    1279:	fb7cb1        	l32r	a11, 6c (4000 <_end+0x2434>)
    127c:	0ed232        	addmi	a3, a2, 0xe00
    127f:	ac0c           	movi.n	a12, 10
    1281:	dc53c2        	s16i	a12, a3, 0x1b8
    1284:	db53c2        	s16i	a12, a3, 0x1b6
    1287:	d853c2        	s16i	a12, a3, 0x1b0
    128a:	e05382        	s16i	a8, a3, 0x1c0
    128d:	da5382        	s16i	a8, a3, 0x1b4
    1290:	e653a2        	s16i	a10, a3, 0x1cc
    1293:	e553a2        	s16i	a10, a3, 0x1ca
    1296:	e453a2        	s16i	a10, a3, 0x1c8
    1299:	e353a2        	s16i	a10, a3, 0x1c6
    129c:	e253a2        	s16i	a10, a3, 0x1c4
    129f:	e153a2        	s16i	a10, a3, 0x1c2
    12a2:	df53a2        	s16i	a10, a3, 0x1be
    12a5:	dd53a2        	s16i	a10, a3, 0x1ba
    12a8:	d653f2        	s16i	a15, a3, 0x1ac
    12ab:	d753e2        	s16i	a14, a3, 0x1ae
    12ae:	d953d2        	s16i	a13, a3, 0x1b2
    12b1:	de53b2        	s16i	a11, a3, 0x1bc
    12b4:	3262f2        	s32i	a15, a2, 200
    12b7:	3362e2        	s32i	a14, a2, 204
    12ba:	3562d2        	s32i	a13, a2, 212
    12bd:	3462c2        	s32i	a12, a2, 208
    12c0:	3762c2        	s32i	a12, a2, 220
    12c3:	3862c2        	s32i	a12, a2, 224
    12c6:	3a62b2        	s32i	a11, a2, 232
    12c9:	3962a2        	s32i	a10, a2, 228
    12cc:	3b62a2        	s32i	a10, a2, 236
    12cf:	3d62a2        	s32i	a10, a2, 244
    12d2:	3e62a2        	s32i	a10, a2, 248
    12d5:	3f62a2        	s32i	a10, a2, 252
    12d8:	4062a2        	s32i	a10, a2, 0x100
    12db:	4162a2        	s32i	a10, a2, 0x104
    12de:	4262a2        	s32i	a10, a2, 0x108
    12e1:	4762a2        	s32i	a10, a2, 0x11c
    12e4:	496292        	s32i	a9, a2, 0x124
    12e7:	4a6292        	s32i	a9, a2, 0x128
    12ea:	366282        	s32i	a8, a2, 216
    12ed:	3c6282        	s32i	a8, a2, 240
    12f0:	4b6282        	s32i	a8, a2, 0x12c
    12f3:	436272        	s32i	a7, a2, 0x10c
    12f6:	446262        	s32i	a6, a2, 0x110
    12f9:	456252        	s32i	a5, a2, 0x114
    12fc:	466242        	s32i	a4, a2, 0x118
    12ff:	fb5c31        	l32r	a3, 70 (1999 <_DYNAMIC+0x29d>)
    1302:	486232        	s32i	a3, a2, 0x120
    1305:	f01d           	retw.n

00001307 <plcpitch_sim_init+0xa7>:
	...

00001308 <plcpitch_para_init>:
    1308:	004136        	entry	a1, 32
    130b:	1ea5b2        	movi	a11, 0x51e
    130e:	0fd2d2        	addmi	a13, a2, 0xf00
    1311:	589d82        	l16si	a8, a13, 176
    1314:	5a9d32        	l16si	a3, a13, 180
    1317:	579d92        	l16si	a9, a13, 174
    131a:	569da2        	l16si	a10, a13, 172
    131d:	5d9dc2        	l16si	a12, a13, 186
    1320:	5c9de2        	l16si	a14, a13, 184
    1323:	5b9df2        	l16si	a15, a13, 182
    1326:	599d42        	l16si	a4, a13, 178
    1329:	356242        	s32i	a4, a2, 212
    132c:	4962b2        	s32i	a11, a2, 0x124
    132f:	4a62b2        	s32i	a11, a2, 0x128
    1332:	3962c2        	s32i	a12, a2, 228
    1335:	3862e2        	s32i	a14, a2, 224
    1338:	3762f2        	s32i	a15, a2, 220
    133b:	346282        	s32i	a8, a2, 208
    133e:	366232        	s32i	a3, a2, 216
    1341:	336292        	s32i	a9, a2, 204
    1344:	3262a2        	s32i	a10, a2, 200
    1347:	629d42        	l16si	a4, a13, 196
    134a:	3e6242        	s32i	a4, a2, 248
    134d:	5f9da2        	l16si	a10, a13, 190
    1350:	609d92        	l16si	a9, a13, 192
    1353:	639d32        	l16si	a3, a13, 198
    1356:	619d82        	l16si	a8, a13, 194
    1359:	649df2        	l16si	a15, a13, 200
    135c:	659de2        	l16si	a14, a13, 202
    135f:	0c2c           	movi.n	a12, 32
    1361:	4362c2        	s32i	a12, a2, 0x10c
    1364:	4162e2        	s32i	a14, a2, 0x104
    1367:	4062f2        	s32i	a15, a2, 0x100
    136a:	3d6282        	s32i	a8, a2, 244
    136d:	3f6232        	s32i	a3, a2, 252
    1370:	3c6292        	s32i	a9, a2, 240
    1373:	3b62a2        	s32i	a10, a2, 236
    1376:	5e9db2        	l16si	a11, a13, 188
    1379:	fb3d41        	l32r	a4, 70 (1999 <_DYNAMIC+0x29d>)
    137c:	486242        	s32i	a4, a2, 0x120
    137f:	3a62b2        	s32i	a11, a2, 232
    1382:	6a1c           	movi.n	a10, 22
    1384:	494c           	movi.n	a9, 68
    1386:	030c           	movi.n	a3, 0
    1388:	180c           	movi.n	a8, 1
    138a:	476282        	s32i	a8, a2, 0x11c
    138d:	4b6232        	s32i	a3, a2, 0x12c
    1390:	466292        	s32i	a9, a2, 0x118
    1393:	4562a2        	s32i	a10, a2, 0x114
    1396:	cca2b2        	movi	a11, 0x2cc
    1399:	4462b2        	s32i	a11, a2, 0x110
    139c:	669dd2        	l16si	a13, a13, 204
    139f:	4262d2        	s32i	a13, a2, 0x108
    13a2:	f01d           	retw.n

000013a4 <HIFI_sqrt>:
    13a4:	006136        	entry	a1, 48
    13a7:	035d           	mov.n	a5, a3
    13a9:	030c           	movi.n	a3, 0
    13ab:	1139           	s32i.n	a3, a1, 4
    13ad:	0139           	s32i.n	a3, a1, 0
    13af:	0715e6        	bgei	a5, 1, 13ba <HIFI_sqrt+0x16>
    13b2:	0139           	s32i.n	a3, a1, 0

000013b4 <HIFI_sqrt+0x10>:
    13b4:	1139           	s32i.n	a3, a1, 4
    13b6:	0128           	l32i.n	a2, a1, 0
    13b8:	f01d           	retw.n

000013ba <HIFI_sqrt+0x16>:
    13ba:	fb2e81        	l32r	a8, 74 (1b30 <TABLE_SQRT>)
    13bd:	40e5a0        	nsa	a10, a5
    13c0:	c042a0        	sub	a4, a2, a10
    13c3:	401a00        	ssl	a10
    13c6:	a19500        	sll	a9, a5
    13c9:	046040        	extui	a6, a4, 0, 1
    13cc:	400600        	ssr	a6
    13cf:	b19090        	sra	a9, a9
    13d2:	419990        	srli	a9, a9, 9
    13d5:	317090        	srai	a7, a9, 16
    13d8:	300994        	ae_cvtp24a16x2.ll	aep0, a9, a9
    13db:	f0c772        	addi	a7, a7, -16
    13de:	537730        	max	a7, a7, a3
    13e1:	907780        	addx2	a7, a7, a8
    13e4:	278d           	ae_l16si.n	a8, a7, 0
    13e6:	377d           	ae_l16si.n	a7, a7, 2
    13e8:	346a           	add.n	a3, a4, a6
    13ea:	213130        	srai	a3, a3, 1
    13ed:	c07780        	sub	a7, a7, a8
    13f0:	118800        	slli	a8, a8, 16
    13f3:	117700        	slli	a7, a7, 16
    13f6:	364734        	ae_cvtq48a32s	aeq1, a7
    13f9:	360834        	ae_cvtq48a32s	aeq0, a8
    13fc:	030174        	ae_mulaq32sp16u.l	aeq0, aeq1, aep0
    13ff:	0139           	s32i.n	a3, a1, 0
    1401:	368304        	ae_trunca32q48	a3, aeq0
    1404:	ffeb06        	j	13b4 <HIFI_sqrt+0x10>

00001407 <HIFI_sqrt+0x63>:
	...

00001408 <_start>:
    1408:	004136        	entry	a1, 32
    140b:	fb1ca1        	l32r	a10, 7c (1a54 <_DYNAMIC+0x358>)
    140e:	fb1a31        	l32r	a3, 78 (1bc8 <printf_ptr>)
    1411:	0288           	l32i.n	a8, a2, 0
    1413:	fb1bb1        	l32r	a11, 80 (1308 <plcpitch_para_init>)
    1416:	0389           	s32i.n	a8, a3, 0
    1418:	0008e0        	callx8	a8
    141b:	0388           	l32i.n	a8, a3, 0
    141d:	fb19a1        	l32r	a10, 84 (1a70 <_DYNAMIC+0x374>)
    1420:	fb1ab1        	l32r	a11, 88 (118 <SMART_PITCH_PLC_INI>)
    1423:	0008e0        	callx8	a8
    1426:	0388           	l32i.n	a8, a3, 0
    1428:	fb19a1        	l32r	a10, 8c (1a8c <_DYNAMIC+0x390>)
    142b:	fb19b1        	l32r	a11, 90 (360 <SMART_PITCH_PLC_PROCESS>)
    142e:	0008e0        	callx8	a8
    1431:	fb1821        	l32r	a2, 94 (1ba0 <export_parameter_array>)
    1434:	f01d           	retw.n

00001436 <_start+0x2e>:
	...

00001438 <__do_global_ctors_aux>:
    1438:	004136        	entry	a1, 32
    143b:	fb1781        	l32r	a8, 98 (18c0 <_DYNAMIC+0x1c4>)
    143e:	7f2882        	l32i	a8, a8, 0x1fc
    1441:	fb1621        	l32r	a2, 9c (1abc <__CTOR_LIST___47>)
    1444:	0c0826        	beqi	a8, -1, 1454 <__do_global_ctors_aux+0x1c>
    1447:	02a8           	l32i.n	a10, a2, 0

00001449 <__do_global_ctors_aux+0x11>:
    1449:	000ae0        	callx8	a10
    144c:	fcc222        	addi	a2, a2, -4
    144f:	02a8           	l32i.n	a10, a2, 0
    1451:	f40a66        	bnei	a10, -1, 1449 <__do_global_ctors_aux+0x11>

00001454 <__do_global_ctors_aux+0x1c>:
    1454:	f01d           	retw.n

00001456 <__do_global_ctors_aux+0x1e>:
	...

00001458 <getfespeech>:
    1458:	004136        	entry	a1, 32
    145b:	1227a2        	l32i	a10, a7, 72
    145e:	052a37        	blt	a10, a3, 1467 <getfespeech+0xf>
    1461:	c0aa30        	sub	a10, a10, a3
    1464:	1267a2        	s32i	a10, a7, 72
    1467:	1ca576        	loopgtz	a5, 1487 <getfespeech+0x2f>
    146a:	ba4a           	add.n	a11, a10, a4
    146c:	1227a2        	l32i	a10, a7, 72
    146f:	90bb20        	addx2	a11, a11, a2
    1472:	2bbd           	ae_l16si.n	a11, a11, 0
    1474:	09c6b4        	ae_s16ip	a11, a6, 2
    1477:	aa1b           	addi.n	a10, a10, 1
    1479:	1267a2        	s32i	a10, a7, 72
    147c:	052a37        	blt	a10, a3, 1485 <getfespeech+0x2d>
    147f:	c0aa30        	sub	a10, a10, a3
    1482:	1267a2        	s32i	a10, a7, 72

00001485 <getfespeech+0x2d>:
    1485:	f03d           	nop.n

00001487 <getfespeech+0x2f>:
    1487:	f01d           	retw.n

00001489 <getfespeech+0x31>:
    1489:	00000000 36000000                                .......

00001490 <__divdi3>:
    1490:	00c136        	entry	a1, 96
    1493:	05ad           	mov.n	a10, a5
    1495:	039d           	mov.n	a9, a3
    1497:	047d           	mov.n	a7, a4
    1499:	2149           	s32i.n	a4, a1, 8
    149b:	02bd           	mov.n	a11, a2
    149d:	3159           	s32i.n	a5, a1, 12
    149f:	042d           	mov.n	a2, a4
    14a1:	3168           	l32i.n	a6, a1, 12
    14a3:	0b4d           	mov.n	a4, a11
    14a5:	0cf396        	bltz	a3, 1578 <__divdi3+0xe8>
    14a8:	00a082        	movi	a8, 0

000014ab <__divdi3+0x1b>:
    14ab:	0d2c           	movi.n	a13, 32
    14ad:	04cd           	mov.n	a12, a4
    14af:	00f6d6        	bgez	a6, 14c2 <__divdi3+0x32>
    14b2:	602070        	neg	a2, a7
    14b5:	f57c           	movi.n	a5, -1
    14b7:	308850        	xor	a8, a8, a5
    14ba:	305a50        	xor	a5, a10, a5
    14bd:	b51b           	addi.n	a11, a5, 1
    14bf:	835b70        	moveqz	a5, a11, a7

000014c2 <__divdi3+0x32>:
    14c2:	02ad           	mov.n	a10, a2
    14c4:	09bd           	mov.n	a11, a9
    14c6:	0bf516        	beqz	a5, 1589 <__divdi3+0xf9>
    14c9:	16b957        	bgeu	a9, a5, 14e3 <__divdi3+0x53>
    14cc:	020c           	movi.n	a2, 0

000014ce <__divdi3+0x3e>:
    14ce:	030c           	movi.n	a3, 0

000014d0 <__divdi3+0x40>:
    14d0:	1f1816        	beqz	a8, 16c5 <__divdi3+0x235>

000014d3 <__divdi3+0x43>:
    14d3:	f87c           	movi.n	a8, -1
    14d5:	303380        	xor	a3, a3, a8
    14d8:	631b           	addi.n	a6, a3, 1
    14da:	833620        	moveqz	a3, a6, a2
    14dd:	602020        	neg	a2, a2
    14e0:	f01d           	retw.n

000014e2 <__divdi3+0x52>:
	...

000014e3 <__divdi3+0x53>:
    14e3:	40f530        	nsau	a3, a5
    14e6:	1de316        	beqz	a3, 16c8 <__divdi3+0x238>
    14e9:	c07d30        	sub	a7, a13, a3
    14ec:	400700        	ssr	a7
    14ef:	916090        	srl	a6, a9
    14f2:	401300        	ssl	a3
    14f5:	81d940        	src	a13, a9, a4
    14f8:	81b520        	src	a11, a5, a2
    14fb:	9bcd           	ae_zext16	a12, a11
    14fd:	f5e0b0        	extui	a14, a11, 16, 16
    1500:	404010        	ssai	16
    1503:	c2f6e0        	quou	a15, a6, a14
    1506:	0f9d           	mov.n	a9, a15
    1508:	82acf0        	mull	a10, a12, a15
    150b:	e266e0        	remu	a6, a6, a14
    150e:	8166d0        	src	a6, a6, a13
    1511:	065d           	mov.n	a5, a6
    1513:	0eb6a7        	bgeu	a6, a10, 1525 <__divdi3+0x95>
    1516:	9f0b           	addi.n	a9, a15, -1
    1518:	5b6a           	add.n	a5, a11, a6
    151a:	0735b7        	bltu	a5, a11, 1525 <__divdi3+0x95>
    151d:	04b5a7        	bgeu	a5, a10, 1525 <__divdi3+0x95>
    1520:	5b5a           	add.n	a5, a11, a5
    1522:	fecf92        	addi	a9, a15, -2
    1525:	119900        	slli	a9, a9, 16
    1528:	401300        	ssl	a3
    152b:	c065a0        	sub	a6, a5, a10
    152e:	c2f6e0        	quou	a15, a6, a14
    1531:	0fad           	mov.n	a10, a15
    1533:	82ccf0        	mull	a12, a12, a15
    1536:	e266e0        	remu	a6, a6, a14
    1539:	0bf6d0        	depbits	a13, a6, 16, 16
    153c:	0d5d           	mov.n	a5, a13
    153e:	0ebdc7        	bgeu	a13, a12, 1550 <__divdi3+0xc0>
    1541:	af0b           	addi.n	a10, a15, -1
    1543:	5bda           	add.n	a5, a11, a13
    1545:	0735b7        	bltu	a5, a11, 1550 <__divdi3+0xc0>
    1548:	04b5c7        	bgeu	a5, a12, 1550 <__divdi3+0xc0>
    154b:	5b5a           	add.n	a5, a11, a5
    154d:	fecfa2        	addi	a10, a15, -2
    1550:	209a90        	or	a9, a10, a9
    1553:	c055c0        	sub	a5, a5, a12
    1556:	a1a200        	sll	a10, a2
    1559:	82b9a0        	mull	a11, a9, a10
    155c:	a2a9a0        	muluh	a10, a9, a10
    155f:	092d           	mov.n	a2, a9
    1561:	0e35a7        	bltu	a5, a10, 1573 <__divdi3+0xe3>
    1564:	c0c5a0        	sub	a12, a5, a10
    1567:	f63c56        	bnez	a12, 14ce <__divdi3+0x3e>
    156a:	a1d400        	sll	a13, a4
    156d:	023db7        	bltu	a13, a11, 1573 <__divdi3+0xe3>
    1570:	ffd686        	j	14ce <__divdi3+0x3e>

00001573 <__divdi3+0xe3>:
    1573:	290b           	addi.n	a2, a9, -1
    1575:	ffd546        	j	14ce <__divdi3+0x3e>

00001578 <__divdi3+0xe8>:
    1578:	6040b0        	neg	a4, a11
    157b:	f87c           	movi.n	a8, -1
    157d:	309380        	xor	a9, a3, a8
    1580:	e91b           	addi.n	a14, a9, 1
    1582:	839eb0        	moveqz	a9, a14, a11
    1585:	ffc886        	j	14ab <__divdi3+0x1b>

00001588 <__divdi3+0xf8>:
	...

00001589 <__divdi3+0xf9>:
    1589:	69b927        	bgeu	a9, a2, 15f6 <__divdi3+0x166>
    158c:	40f230        	nsau	a3, a2
    158f:	a38c           	beqz.n	a3, 159d <__divdi3+0x10d>
    1591:	401300        	ssl	a3
    1594:	a1a200        	sll	a10, a2
    1597:	81b940        	src	a11, a9, a4
    159a:	a1c400        	sll	a12, a4

0000159d <__divdi3+0x10d>:
    159d:	404010        	ssai	16
    15a0:	9a2d           	ae_zext16	a2, a10
    15a2:	f540a0        	extui	a4, a10, 16, 16
    15a5:	c2db40        	quou	a13, a11, a4
    15a8:	0d9d           	mov.n	a9, a13
    15aa:	8252d0        	mull	a5, a2, a13
    15ad:	e2bb40        	remu	a11, a11, a4
    15b0:	81bbc0        	src	a11, a11, a12
    15b3:	0b3d           	mov.n	a3, a11
    15b5:	0fbb57        	bgeu	a11, a5, 15c8 <__divdi3+0x138>
    15b8:	9d0b           	addi.n	a9, a13, -1
    15ba:	3baa           	add.n	a3, a11, a10
    15bc:	0833a7        	bltu	a3, a10, 15c8 <__divdi3+0x138>
    15bf:	05b357        	bgeu	a3, a5, 15c8 <__divdi3+0x138>
    15c2:	803a30        	add	a3, a10, a3
    15c5:	fecd92        	addi	a9, a13, -2
    15c8:	c0d350        	sub	a13, a3, a5
    15cb:	c23d40        	quou	a3, a13, a4
    15ce:	e2dd40        	remu	a13, a13, a4
    15d1:	0bfdc0        	depbits	a12, a13, 16, 16
    15d4:	824230        	mull	a4, a2, a3
    15d7:	032d           	mov.n	a2, a3
    15d9:	0cbc47        	bgeu	a12, a4, 15e9 <__divdi3+0x159>
    15dc:	230b           	addi.n	a2, a3, -1
    15de:	5caa           	add.n	a5, a12, a10
    15e0:	0535a7        	bltu	a5, a10, 15e9 <__divdi3+0x159>
    15e3:	02b547        	bgeu	a5, a4, 15e9 <__divdi3+0x159>
    15e6:	fec322        	addi	a2, a3, -2
    15e9:	030c           	movi.n	a3, 0
    15eb:	11e900        	slli	a14, a9, 16
    15ee:	2022e0        	or	a2, a2, a14
    15f1:	ffb6c6        	j	14d0 <__divdi3+0x40>

000015f4 <__divdi3+0x164>:
	...

000015f6 <__divdi3+0x166>:
    15f6:	130c           	movi.n	a3, 1
    15f8:	c2f320        	quou	a15, a3, a2
    15fb:	832f20        	moveqz	a2, a15, a2
    15fe:	02ad           	mov.n	a10, a2
    1600:	40f250        	nsau	a5, a2
    1603:	0d1516        	beqz	a5, 16d8 <__divdi3+0x248>
    1606:	c0fd50        	sub	a15, a13, a5
    1609:	401500        	ssl	a5
    160c:	a1a200        	sll	a10, a2
    160f:	9a2d           	ae_zext16	a2, a10
    1611:	400f00        	ssr	a15
    1614:	91e090        	srl	a14, a9
    1617:	401500        	ssl	a5
    161a:	81b940        	src	a11, a9, a4
    161d:	a1c400        	sll	a12, a4
    1620:	404010        	ssai	16
    1623:	f540a0        	extui	a4, a10, 16, 16
    1626:	c2de40        	quou	a13, a14, a4
    1629:	0d9d           	mov.n	a9, a13
    162b:	8252d0        	mull	a5, a2, a13
    162e:	e2ee40        	remu	a14, a14, a4
    1631:	81eeb0        	src	a14, a14, a11
    1634:	0e3d           	mov.n	a3, a14
    1636:	0ebe57        	bgeu	a14, a5, 1648 <__divdi3+0x1b8>
    1639:	9d0b           	addi.n	a9, a13, -1
    163b:	3eaa           	add.n	a3, a14, a10
    163d:	0733a7        	bltu	a3, a10, 1648 <__divdi3+0x1b8>
    1640:	04b357        	bgeu	a3, a5, 1648 <__divdi3+0x1b8>
    1643:	3a3a           	add.n	a3, a10, a3
    1645:	fecd92        	addi	a9, a13, -2
    1648:	c0e350        	sub	a14, a3, a5
    164b:	e2fe40        	remu	a15, a14, a4
    164e:	0bffb0        	depbits	a11, a15, 16, 16
    1651:	c2ee40        	quou	a14, a14, a4
    1654:	0e5d           	mov.n	a5, a14
    1656:	82d2e0        	mull	a13, a2, a14
    1659:	0b3d           	mov.n	a3, a11
    165b:	0ebbd7        	bgeu	a11, a13, 166d <__divdi3+0x1dd>
    165e:	5e0b           	addi.n	a5, a14, -1
    1660:	3baa           	add.n	a3, a11, a10
    1662:	0733a7        	bltu	a3, a10, 166d <__divdi3+0x1dd>
    1665:	04b3d7        	bgeu	a3, a13, 166d <__divdi3+0x1dd>
    1668:	3a3a           	add.n	a3, a10, a3
    166a:	fece52        	addi	a5, a14, -2
    166d:	c0b3d0        	sub	a11, a3, a13
    1670:	113900        	slli	a3, a9, 16
    1673:	203530        	or	a3, a5, a3

00001676 <__divdi3+0x1e6>:
    1676:	404010        	ssai	16
    1679:	e2eb40        	remu	a14, a11, a4
    167c:	c2db40        	quou	a13, a11, a4
    167f:	8252d0        	mull	a5, a2, a13
    1682:	0dbd           	mov.n	a11, a13
    1684:	81eec0        	src	a14, a14, a12
    1687:	0e9d           	mov.n	a9, a14
    1689:	0ebe57        	bgeu	a14, a5, 169b <__divdi3+0x20b>
    168c:	bd0b           	addi.n	a11, a13, -1
    168e:	9eaa           	add.n	a9, a14, a10
    1690:	0739a7        	bltu	a9, a10, 169b <__divdi3+0x20b>
    1693:	04b957        	bgeu	a9, a5, 169b <__divdi3+0x20b>
    1696:	9a9a           	add.n	a9, a10, a9
    1698:	fecdb2        	addi	a11, a13, -2
    169b:	c0f950        	sub	a15, a9, a5
    169e:	c25f40        	quou	a5, a15, a4
    16a1:	e2ff40        	remu	a15, a15, a4
    16a4:	0bffc0        	depbits	a12, a15, 16, 16
    16a7:	824250        	mull	a4, a2, a5
    16aa:	052d           	mov.n	a2, a5
    16ac:	0cbc47        	bgeu	a12, a4, 16bc <__divdi3+0x22c>
    16af:	250b           	addi.n	a2, a5, -1
    16b1:	9caa           	add.n	a9, a12, a10
    16b3:	0539a7        	bltu	a9, a10, 16bc <__divdi3+0x22c>
    16b6:	02b947        	bgeu	a9, a4, 16bc <__divdi3+0x22c>
    16b9:	fec522        	addi	a2, a5, -2
    16bc:	116b00        	slli	a6, a11, 16
    16bf:	202260        	or	a2, a2, a6
    16c2:	e0d856        	bnez	a8, 14d3 <__divdi3+0x43>

000016c5 <__divdi3+0x235>:
    16c5:	f01d           	retw.n

000016c7 <__divdi3+0x237>:
	...

000016c8 <__divdi3+0x238>:
    16c8:	023597        	bltu	a5, a9, 16ce <__divdi3+0x23e>
    16cb:	043427        	bltu	a4, a2, 16d3 <__divdi3+0x243>

000016ce <__divdi3+0x23e>:
    16ce:	120c           	movi.n	a2, 1
    16d0:	ff7e86        	j	14ce <__divdi3+0x3e>

000016d3 <__divdi3+0x243>:
    16d3:	020c           	movi.n	a2, 0
    16d5:	ff7d46        	j	14ce <__divdi3+0x3e>

000016d8 <__divdi3+0x248>:
    16d8:	c0b920        	sub	a11, a9, a2
    16db:	f54020        	extui	a4, a2, 16, 16
    16de:	922d           	ae_zext16	a2, a2
    16e0:	ffe486        	j	1676 <__divdi3+0x1e6>

000016e3 <__divdi3+0x253>:
	...

000016e4 <_fini>:
    16e4:	008136        	entry	a1, 64
    16e7:	fa6e81        	l32r	a8, a0 (c0 <__do_global_dtors_aux>)
    16ea:	f03d           	nop.n
    16ec:	0008e0        	callx8	a8

000016ef <_fini+0xb>:
    16ef:	f01d           	retw.n
