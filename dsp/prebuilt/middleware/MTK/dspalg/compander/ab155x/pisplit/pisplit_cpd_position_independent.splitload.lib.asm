
Build/lib/pisplit_cpd_position_independent.splitload.lib:     file format elf32-xtensa-le


Disassembly of section .text:

00000000 <_init-0x158>:
       0:	000001ac 00001bb0 000025e4 00002544     .........%..D%..
      10:	00000000 0000252c 00000000 000025e8     ....,%.......%..
      20:	00002600 00002418 0000196c 00002430     .&...$..l...0$..
      30:	00001980 00002448 00000430 00002460     ....H$..0...`$..
      40:	00000c5c 00002478 000007a8 00002490     \...x$.......$..
      50:	000014fc 000024a8 00001a48 000024b8     .....$..H....$..
      60:	00001b08 000024c8 00000c4c 000024ec     .....$..L....$..
      70:	000014dc 0000250c 00000250 00002550     .....%..P...P%..
      80:	00002580 00007fff 00004800 00002800     .%.......H...(..
      90:	18000000 e8000000 000002c0 00001e68     ............h...
      a0:	00001e50 0000041c 00000ccd 000019c8     P...............
      b0:	0000030c 000019fc 20000000 00000330     ........... 0...
      c0:	00400000 7fff0000 000003bc 00000544     ..@.........D...
      d0:	00000c44 00000898 00800000 08000000     D...............
      e0:	00001988 7fffffff 40000000 00000b2c     ...........@,...
      f0:	3fffffff 00040000 00000a50 00000b88     ...?....P.......
     100:	00000b4c 00000374 00001000 00000cec     L...t...........
     110:	00000d48 00000e1c 00000f18 00000258     H...........X...
     120:	03d70a3d 0000fffd 00001214 00001398     =...............
     130:	00001c9c 00001ddc 00004000 00001298     .........@......
     140:	00001908 000008a8 00002000 00002334     ......... ..4#..
     150:	00002530 00000170                       0%..p...

00000158 <_init>:
     158:	008136        	entry	a1, 64
     15b:	ffa981        	l32r	a8, 0 (1ac <frame_dummy>)
     15e:	f03d           	nop.n
     160:	0008e0        	callx8	a8
     163:	ffa881        	l32r	a8, 4 (1bb0 <__do_global_ctors_aux>)
     166:	f03d           	nop.n
     168:	0008e0        	callx8	a8
     16b:	f01d           	retw.n

0000016d <_init+0x15>:
     16d:	000000                                        ...

00000170 <__do_global_dtors_aux>:
     170:	004136        	entry	a1, 32
     173:	ffa531        	l32r	a3, 8 (25e4 <completed$3521_6_3>)
     176:	000382        	l8ui	a8, a3, 0
     179:	d8ec           	bnez.n	a8, 1aa <__do_global_dtors_aux+0x3a>
     17b:	ffa421        	l32r	a2, c (2544 <p$3519_6_1>)
     17e:	02b8           	l32i.n	a11, a2, 0
     180:	0ba8           	l32i.n	a10, a11, 0
     182:	ca8c           	beqz.n	a10, 192 <__do_global_dtors_aux+0x22>

00000184 <__do_global_dtors_aux+0x14>:
     184:	9b4b           	addi.n	a9, a11, 4
     186:	0299           	s32i.n	a9, a2, 0
     188:	000ae0        	callx8	a10
     18b:	02b8           	l32i.n	a11, a2, 0
     18d:	0ba8           	l32i.n	a10, a11, 0
     18f:	ff1a56        	bnez	a10, 184 <__do_global_dtors_aux+0x14>

00000192 <__do_global_dtors_aux+0x22>:
     192:	ff9fa1        	l32r	a10, 10 (0 <_text_start>)
     195:	0b0c           	movi.n	a11, 0
     197:	081ab7        	beq	a10, a11, 1a3 <__do_global_dtors_aux+0x33>
     19a:	ff9ea1        	l32r	a10, 14 (252c <__EH_FRAME_BEGIN___44>)
     19d:	ff9c81        	l32r	a8, 10 (0 <_text_start>)
     1a0:	0008e0        	callx8	a8

000001a3 <__do_global_dtors_aux+0x33>:
     1a3:	1c0c           	movi.n	a12, 1
     1a5:	0043c2        	s8i	a12, a3, 0
     1a8:	f01d           	retw.n

000001aa <__do_global_dtors_aux+0x3a>:
     1aa:	f01d           	retw.n

000001ac <frame_dummy>:
     1ac:	004136        	entry	a1, 32
     1af:	ff9a81        	l32r	a8, 18 (0 <_text_start>)
     1b2:	090c           	movi.n	a9, 0
     1b4:	0b1897        	beq	a8, a9, 1c3 <frame_dummy+0x17>
     1b7:	ff97a1        	l32r	a10, 14 (252c <__EH_FRAME_BEGIN___44>)
     1ba:	ff98b1        	l32r	a11, 1c (25e8 <object$3538_8_1>)
     1bd:	ff9681        	l32r	a8, 18 (0 <_text_start>)
     1c0:	0008e0        	callx8	a8

000001c3 <frame_dummy+0x17>:
     1c3:	f01d           	retw.n

000001c5 <frame_dummy+0x19>:
     1c5:	000000                                        ...

000001c8 <_start>:
     1c8:	004136        	entry	a1, 32
     1cb:	ff96a1        	l32r	a10, 24 (2418 <_DYNAMIC+0x508>)
     1ce:	ff9431        	l32r	a3, 20 (2600 <printf_ptr>)
     1d1:	0288           	l32i.n	a8, a2, 0
     1d3:	ff95b1        	l32r	a11, 28 (196c <get_CPD_memsize>)
     1d6:	0389           	s32i.n	a8, a3, 0
     1d8:	0008e0        	callx8	a8
     1db:	0388           	l32i.n	a8, a3, 0
     1dd:	ff93a1        	l32r	a10, 2c (2430 <_DYNAMIC+0x520>)
     1e0:	ff94b1        	l32r	a11, 30 (1980 <get_INS_memsize>)
     1e3:	0008e0        	callx8	a8
     1e6:	002382        	l32i	a8, a3, 0
     1e9:	ff92a1        	l32r	a10, 34 (2448 <_DYNAMIC+0x538>)
     1ec:	ff93b1        	l32r	a11, 38 (430 <compander_VO_init>)
     1ef:	0008e0        	callx8	a8
     1f2:	0388           	l32i.n	a8, a3, 0
     1f4:	ff92a1        	l32r	a10, 3c (2460 <_DYNAMIC+0x550>)
     1f7:	ff92b1        	l32r	a11, 40 (c5c <compander_AU_init>)
     1fa:	0008e0        	callx8	a8
     1fd:	0388           	l32i.n	a8, a3, 0
     1ff:	ff91a1        	l32r	a10, 44 (2478 <_DYNAMIC+0x568>)
     202:	ff91b1        	l32r	a11, 48 (7a8 <compander_VO_proc>)
     205:	0008e0        	callx8	a8
     208:	0388           	l32i.n	a8, a3, 0
     20a:	ff90a1        	l32r	a10, 4c (2490 <_DYNAMIC+0x580>)
     20d:	ff90b1        	l32r	a11, 50 (14fc <compander_AU_proc>)
     210:	0008e0        	callx8	a8
     213:	0388           	l32i.n	a8, a3, 0
     215:	ff8fa1        	l32r	a10, 54 (24a8 <_DYNAMIC+0x598>)
     218:	ff90b1        	l32r	a11, 58 (1a48 <INS_Init>)
     21b:	0008e0        	callx8	a8
     21e:	002382        	l32i	a8, a3, 0
     221:	ff8ea1        	l32r	a10, 5c (24b8 <_DYNAMIC+0x5a8>)
     224:	ff8fb1        	l32r	a11, 60 (1b08 <INS_Prcs>)
     227:	0008e0        	callx8	a8
     22a:	0388           	l32i.n	a8, a3, 0
     22c:	ff8ea1        	l32r	a10, 64 (24c8 <_DYNAMIC+0x5b8>)
     22f:	ff8eb1        	l32r	a11, 68 (c4c <compander_AU_SetFrame120_init>)
     232:	0008e0        	callx8	a8
     235:	0388           	l32i.n	a8, a3, 0
     237:	ff8da1        	l32r	a10, 6c (24ec <_DYNAMIC+0x5dc>)
     23a:	ff8db1        	l32r	a11, 70 (14dc <compander_AU_SetINS_proc>)
     23d:	0008e0        	callx8	a8
     240:	0388           	l32i.n	a8, a3, 0
     242:	ff8ca1        	l32r	a10, 74 (250c <_DYNAMIC+0x5fc>)
     245:	ff8cb1        	l32r	a11, 78 (250 <SVN_version>)
     248:	0008e0        	callx8	a8
     24b:	ff8c21        	l32r	a2, 7c (2550 <export_parameter_array>)
     24e:	f01d           	retw.n

00000250 <SVN_version>:
     250:	004136        	entry	a1, 32
     253:	76a322        	movi	a2, 0x376
     256:	f01d           	retw.n

00000258 <HIFI_sqrt_cpd>:
     258:	004136        	entry	a1, 32
     25b:	0358           	l32i.n	a5, a3, 0
     25d:	0a0c           	movi.n	a10, 0
     25f:	ff8891        	l32r	a9, 80 (2580 <TABLE_SQRT>)
     262:	5215a6        	blti	a5, 1, 2b8 <HIFI_sqrt_cpd+0x60>
     265:	0278           	l32i.n	a7, a2, 0
     267:	40e580        	nsa	a8, a5
     26a:	401800        	ssl	a8
     26d:	c07780        	sub	a7, a7, a8
     270:	a1b500        	sll	a11, a5
     273:	046070        	extui	a6, a7, 0, 1
     276:	400600        	ssr	a6
     279:	b1b0b0        	sra	a11, a11
     27c:	41b9b0        	srli	a11, a11, 9
     27f:	3180b0        	srai	a8, a11, 16
     282:	f0c882        	addi	a8, a8, -16
     285:	5388a0        	max	a8, a8, a10
     288:	908890        	addx2	a8, a8, a9
     28b:	289d           	ae_l16si.n	a9, a8, 0
     28d:	388d           	ae_l16si.n	a8, a8, 2
     28f:	301bb4        	ae_cvtp24a16x2.ll	aep1, a11, a11
     292:	c08890        	sub	a8, a8, a9
     295:	119900        	slli	a9, a9, 16
     298:	118800        	slli	a8, a8, 16
     29b:	364834        	ae_cvtq48a32s	aeq1, a8
     29e:	360934        	ae_cvtq48a32s	aeq0, a9
     2a1:	031174        	ae_mulaq32sp16u.l	aeq0, aeq1, aep1
     2a4:	f86390667f 	{ add	a6, a6, a7; ae_roundsp16q48asym	aep0, aeq0 }
     2a9:	360424        	ae_trunca16p24s.l	a4, aep0
     2ac:	216160        	srai	a6, a6, 1
     2af:	0269           	s32i.n	a6, a2, 0
     2b1:	114400        	slli	a4, a4, 16
     2b4:	0349           	s32i.n	a4, a3, 0
     2b6:	f01d           	retw.n

000002b8 <HIFI_sqrt_cpd+0x60>:
     2b8:	02a9           	s32i.n	a10, a2, 0
     2ba:	03a9           	s32i.n	a10, a3, 0
     2bc:	f01d           	retw.n

000002be <HIFI_sqrt_cpd+0x66>:
	...

000002c0 <Xdiv>:
     2c0:	004136        	entry	a1, 32
     2c3:	0248           	l32i.n	a4, a2, 0
     2c5:	0358           	l32i.n	a5, a3, 0
     2c7:	04bc           	beqz.n	a4, 2fb <Xdiv+0x3b>
     2c9:	85bc           	beqz.n	a5, 305 <Xdiv+0x45>
     2cb:	60c204        	ae_lq32f.i	aeq3, a2, 0
     2ce:	608304        	ae_lq32f.i	aeq2, a3, 0
     2d1:	366904        	ae_nsaq56s	a9, aeq2
     2d4:	367804        	ae_nsaq56s	a8, aeq3
     2d7:	438890        	min	a8, a8, a9
     2da:	f8c892        	addi	a9, a8, -8
     2dd:	fc24feb90f 	{ ae_slaasq56s	aeq0, aeq3, a9; nop }
     2e2:	e9c882        	addi	a8, a8, -23
     2e5:	fc24fea84f 	{ ae_slaasq56s	aeq1, aeq2, a8; nop }
     2ea:	369204        	ae_trunca32q48	a2, aeq1
     2ed:	368404        	ae_trunca32q48	a4, aeq0
     2f0:	528c           	beqz.n	a2, 2f9 <Xdiv+0x39>
     2f2:	d22420        	quos	a2, a4, a2
     2f5:	a22d           	ae_clamps16	a2, a2
     2f7:	f01d           	retw.n

000002f9 <Xdiv+0x39>:
     2f9:	f01d           	retw.n

000002fb <Xdiv+0x3b>:
     2fb:	ff6281        	l32r	a8, 84 (7fff <_end+0x59fb>)
     2fe:	020c           	movi.n	a2, 0
     300:	832850        	moveqz	a2, a8, a5
     303:	f01d           	retw.n

00000305 <Xdiv+0x45>:
     305:	ff5f21        	l32r	a2, 84 (7fff <_end+0x59fb>)
     308:	f01d           	retw.n

0000030a <Xdiv+0x4a>:
	...

0000030c <linear_interp>:
     30c:	006136        	entry	a1, 48
     30f:	c08340        	sub	a8, a3, a4
     312:	612d           	ae_s16i.n	a2, a1, 0
     314:	400104        	ae_lp16f.i	aep0, a1, 0
     317:	36c834        	ae_cvtq48a32s	aeq3, a8
     31a:	d7e37ec4bf 	{ ae_cvtq48a32s	aeq2, a4; ae_mulfq32sp16s.l	aeq3, aeq3, aep0 }
     31f:	35f094        	ae_slliq56	aeq3, aeq3, 2
     322:	066b04        	ae_addsq56s	aeq1, aeq2, aeq3
     325:	061934        	ae_roundsq32asym	aeq0, aeq1
     328:	368204        	ae_trunca32q48	a2, aeq0
     32b:	f01d           	retw.n

0000032d <linear_interp+0x21>:
     32d:	000000                                        ...

00000330 <INS_coef_init>:
     330:	004136        	entry	a1, 32
     333:	ff55b1        	l32r	a11, 88 (4800 <_end+0x21fc>)
     336:	ff55a1        	l32r	a10, 8c (2800 <_end+0x1fc>)
     339:	ff5641        	l32r	a4, 94 (e8000000 <_end+0xe7ffd9fc>)
     33c:	1cc282        	addi	a8, a2, 28
     33f:	01d232        	addmi	a3, a2, 0x100
     342:	404804        	ae_lp16f.i	aep4, a8, 0
     345:	ff5291        	l32r	a9, 90 (18000000 <_end+0x17ffd9fc>)
     348:	364934        	ae_cvtq48a32s	aeq1, a9
     34b:	ff0c58babf 	{ ae_cvtp24a16x2.ll	aep3, a10, a11; ae_movq56	aeq3, aeq1 }
     350:	ed8e7ec4bf 	{ ae_cvtq48a32s	aeq2, a4; ae_mulsfs32p16s.hh	aeq3, aep3, aep4 }
     355:	00b424        	ae_mulafs32p16s.lh	aeq2, aep3, aep4
     358:	ec8f48e33e 	{ addi	a3, a3, -114; ae_mulsfs32p16s.lh	aeq1, aep3, aep4 }
     35d:	f97bbae01f 	{ ae_sllisq56s	aeq0, aeq2, 1; ae_roundsp16q48asym	aep2, aeq3 }
     362:	f86bf2230f 	{ ae_sp16f.l.i	aep2, a3, 0; ae_roundsp16q48asym	aep1, aeq1 }
     367:	f863b2132f 	{ ae_sp16f.l.i	aep1, a3, 4; ae_roundsp16q48asym	aep0, aeq0 }
     36c:	420314        	ae_sp16f.l.i	aep0, a3, 2
     36f:	f01d           	retw.n

00000371 <INS_coef_init+0x41>:
     371:	000000                                        ...

00000374 <AU_INS_coef_init>:
     374:	004136        	entry	a1, 32
     377:	ff44a1        	l32r	a10, 88 (4800 <_end+0x21fc>)
     37a:	ff4641        	l32r	a4, 94 (e8000000 <_end+0xe7ffd9fc>)
     37d:	360434        	ae_cvtq48a32s	aeq0, a4
     380:	01d232        	addmi	a3, a2, 0x100
     383:	ff4381        	l32r	a8, 90 (18000000 <_end+0x17ffd9fc>)
     386:	ff4191        	l32r	a9, 8c (2800 <_end+0x1fc>)
     389:	3009a4        	ae_cvtp24a16x2.ll	aep0, a9, a10
     38c:	36c834        	ae_cvtq48a32s	aeq3, a8
     38f:	84c352        	addi	a5, a3, -124
     392:	604504        	ae_lq32f.i	aeq1, a5, 0
     395:	355414        	ae_slliq56	aeq1, aeq1, 16
     398:	f4c3bf31bf 	{ ae_movq56	aeq2, aeq3; ae_mulafq32sp24s.l	aeq0, aeq1, aep0 }
     39d:	088064        	ae_mulsfq32sp24s.h	aeq2, aeq1, aep0
     3a0:	088964        	ae_mulsfq32sp24s.l	aeq3, aeq1, aep0
     3a3:	fd3449033e 	{ addi	a3, a3, -112; ae_roundsq32asym	aeq1, aeq2 }
     3a8:	621304        	ae_sq32f.i	aeq1, a3, 0
     3ab:	fe3c7ac05f 	{ ae_sllisq56s	aeq1, aeq0, 1; ae_roundsq32asym	aeq2, aeq3 }
     3b0:	fc2c79a32f 	{ ae_sq32f.i	aeq2, a3, 8; ae_roundsq32asym	aeq0, aeq1 }
     3b5:	620314        	ae_sq32f.i	aeq0, a3, 4
     3b8:	f01d           	retw.n

000003ba <AU_INS_coef_init+0x46>:
	...

000003bc <INS_inter_gain>:
     3bc:	006136        	entry	a1, 48
     3bf:	01ad           	mov.n	a10, a1
     3c1:	0d9292        	l16si	a9, a2, 26
     3c4:	4cc2b2        	addi	a11, a2, 76
     3c7:	ff3481        	l32r	a8, 98 (2c0 <Xdiv>)
     3ca:	119900        	slli	a9, a9, 16
     3cd:	0199           	s32i.n	a9, a1, 0
     3cf:	0008e0        	callx8	a8
     3d2:	b14b           	addi.n	a11, a1, 4
     3d4:	ff3181        	l32r	a8, 98 (2c0 <Xdiv>)
     3d7:	11aa00        	slli	a10, a10, 16
     3da:	479292        	l16si	a9, a2, 142
     3dd:	11a9           	s32i.n	a10, a1, 4
     3df:	01ad           	mov.n	a10, a1
     3e1:	119900        	slli	a9, a9, 16
     3e4:	0199           	s32i.n	a9, a1, 0
     3e6:	0008e0        	callx8	a8
     3e9:	4992c2        	l16si	a12, a2, 146
     3ec:	11b8           	l32i.n	a11, a1, 4
     3ee:	11da00        	slli	a13, a10, 16
     3f1:	11cc00        	slli	a12, a12, 16
     3f4:	b2bbc0        	mulsh	a11, a11, a12
     3f7:	4892c2        	l16si	a12, a2, 144
     3fa:	360d34        	ae_cvtq48a32s	aeq0, a13
     3fd:	11bbf0        	slli	a11, a11, 1
     400:	11cc00        	slli	a12, a12, 16
     403:	364c34        	ae_cvtq48a32s	aeq1, a12
     406:	fa667ecbff 	{ ae_cvtq48a32s	aeq3, a11; ae_addsq56s	aeq2, aeq0, aeq1 }
     40b:	066b04        	ae_addsq56s	aeq1, aeq2, aeq3
     40e:	361094        	ae_sllisq56s	aeq0, aeq1, 2
     411:	1b8f04        	ae_roundsp16q48asym	aep0, aeq0
     414:	0082d4        	ae_cvta32p24.h	a2, aep0
     417:	f01d           	retw.n

00000419 <INS_inter_gain+0x5d>:
     419:	000000                                        ...

0000041c <compander_VO_PARA_init>:
     41c:	004136        	entry	a1, 32
     41f:	03ad           	mov.n	a10, a3
     421:	ff1e81        	l32r	a8, 9c (1e68 <xt_memcpy>)
     424:	b24b           	addi.n	a11, a2, 4
     426:	0c1c           	movi.n	a12, 16
     428:	0008e0        	callx8	a8
     42b:	f01d           	retw.n

0000042d <compander_VO_PARA_init+0x11>:
     42d:	000000                                        ...

00000430 <compander_VO_init>:
     430:	004136        	entry	a1, 32
     433:	22c3b2        	addi	a11, a3, 34
     436:	4c0c           	movi.n	a12, 4
     438:	ff1981        	l32r	a8, 9c (1e68 <xt_memcpy>)
     43b:	01d2a2        	addmi	a10, a2, 0x100
     43e:	38caa2        	addi	a10, a10, 56
     441:	0008e0        	callx8	a8
     444:	02ad           	mov.n	a10, a2
     446:	ff1681        	l32r	a8, a0 (1e50 <xt_memset>)
     449:	0b0c           	movi.n	a11, 0
     44b:	9ca0c2        	movi	a12, 156
     44e:	0008e0        	callx8	a8
     451:	ff1481        	l32r	a8, a4 (41c <compander_VO_PARA_init>)
     454:	03ad           	mov.n	a10, a3
     456:	02bd           	mov.n	a11, a2
     458:	0008e0        	callx8	a8
     45b:	4a1c           	movi.n	a10, 20
     45d:	ad0c           	movi.n	a13, 10
     45f:	ff1291        	l32r	a9, a8 (ccd <compander_AU_init+0x71>)
     462:	34bc           	beqz.n	a4, 499 <compander_VO_init+0x69>
     464:	8b2c           	movi.n	a11, 40
     466:	66a6c2        	movi	a12, 0x666
     469:	e40b           	addi.n	a14, a4, -1
     46b:	0a4e16        	beqz	a14, 513 <compander_VO_init+0xe3>
     46e:	33a3f2        	movi	a15, 0x333
     471:	0e5c           	movi.n	a14, 80
     473:	fec482        	addi	a8, a4, -2
     476:	0a6816        	beqz	a8, 520 <compander_VO_init+0xf0>
     479:	1c4426        	beqi	a4, 4, 499 <compander_VO_init+0x69>
     47c:	fbc492        	addi	a9, a4, -5
     47f:	0a8916        	beqz	a9, 52b <compander_VO_init+0xfb>
     482:	fac4d2        	addi	a13, a4, -6
     485:	0afd16        	beqz	a13, 538 <compander_VO_init+0x108>
     488:	1594a7        	bne	a4, a10, 4a1 <compander_VO_init+0x71>
     48b:	1852a2        	s16i	a10, a2, 48
     48e:	f2b9           	s32i.n	a11, a2, 60
     490:	1952c2        	s16i	a12, a2, 50
     493:	000286        	j	4a1 <compander_VO_init+0x71>

00000496 <compander_VO_init+0x66>:
     496:	000000                                        ...

00000499 <compander_VO_init+0x69>:
     499:	1852d2        	s16i	a13, a2, 48
     49c:	f2a9           	s32i.n	a10, a2, 60
     49e:	195292        	s16i	a9, a2, 50

000004a1 <compander_VO_init+0x71>:
     4a1:	12a8           	l32i.n	a10, a2, 4
     4a3:	32c8           	l32i.n	a12, a2, 12
     4a5:	22b8           	l32i.n	a11, a2, 8
     4a7:	022ca7        	blt	a12, a10, 4ad <compander_VO_init+0x7d>
     4aa:	07aba7        	bge	a11, a10, 4b5 <compander_VO_init+0x85>

000004ad <compander_VO_init+0x7d>:
     4ad:	32a9           	s32i.n	a10, a2, 12
     4af:	22a9           	s32i.n	a10, a2, 8
     4b1:	0acd           	mov.n	a12, a10
     4b3:	0abd           	mov.n	a11, a10

000004b5 <compander_VO_init+0x85>:
     4b5:	fefd81        	l32r	a8, ac (19c8 <calc_slope_1>)
     4b8:	0008e0        	callx8	a8
     4bb:	52b8           	l32i.n	a11, a2, 20
     4bd:	4252a2        	s16i	a10, a2, 132
     4c0:	fefc81        	l32r	a8, b0 (30c <linear_interp>)
     4c3:	12c8           	l32i.n	a12, a2, 4
     4c5:	8aad           	ae_sext16	a10, a10
     4c7:	0008e0        	callx8	a8
     4ca:	fefa81        	l32r	a8, b4 (19fc <calc_slope_2>)
     4cd:	42b8           	l32i.n	a11, a2, 16
     4cf:	22c8           	l32i.n	a12, a2, 8
     4d1:	0008e0        	callx8	a8
     4d4:	105242        	s16i	a4, a2, 32
     4d7:	fef981        	l32r	a8, bc (330 <INS_coef_init>)
     4da:	4352a2        	s16i	a10, a2, 134
     4dd:	fef691        	l32r	a9, b8 (20000000 <_end+0x1fffd9fc>)
     4e0:	0b0c           	movi.n	a11, 0
     4e2:	1752b2        	s16i	a11, a2, 46
     4e5:	1662b2        	s32i	a11, a2, 88
     4e8:	1362b2        	s32i	a11, a2, 76
     4eb:	1652b2        	s16i	a11, a2, 44
     4ee:	126292        	s32i	a9, a2, 72
     4f1:	01d2a2        	addmi	a10, a2, 0x100
     4f4:	98caa2        	addi	a10, a10, -104
     4f7:	d2a9           	s32i.n	a10, a2, 52
     4f9:	e2a9           	s32i.n	a10, a2, 56
     4fb:	02ad           	mov.n	a10, a2
     4fd:	0008e0        	callx8	a8
     500:	eca1c2        	movi	a12, 0x1ec
     503:	feefd1        	l32r	a13, c0 (400000 <_end+0x3fd9fc>)
     506:	0003e2        	l8ui	a14, a3, 0
     509:	1252e2        	s16i	a14, a2, 36
     50c:	a2d9           	s32i.n	a13, a2, 40
     50e:	1352c2        	s16i	a12, a2, 38
     511:	f01d           	retw.n

00000513 <compander_VO_init+0xe3>:
     513:	1852a2        	s16i	a10, a2, 48
     516:	f2b9           	s32i.n	a11, a2, 60
     518:	1952c2        	s16i	a12, a2, 50
     51b:	ffe086        	j	4a1 <compander_VO_init+0x71>

0000051e <compander_VO_init+0xee>:
	...

00000520 <compander_VO_init+0xf0>:
     520:	1852b2        	s16i	a11, a2, 48
     523:	f2e9           	s32i.n	a14, a2, 60
     525:	1952f2        	s16i	a15, a2, 50
     528:	ffdd46        	j	4a1 <compander_VO_init+0x71>

0000052b <compander_VO_init+0xfb>:
     52b:	1852a2        	s16i	a10, a2, 48
     52e:	f2b9           	s32i.n	a11, a2, 60
     530:	1952c2        	s16i	a12, a2, 50
     533:	ffda86        	j	4a1 <compander_VO_init+0x71>

00000536 <compander_VO_init+0x106>:
	...

00000538 <compander_VO_init+0x108>:
     538:	1852b2        	s16i	a11, a2, 48
     53b:	f2e9           	s32i.n	a14, a2, 60
     53d:	1952f2        	s16i	a15, a2, 50
     540:	ffd746        	j	4a1 <compander_VO_init+0x71>

00000543 <compander_VO_init+0x113>:
	...

00000544 <cpd_block_proc>:
     544:	006136        	entry	a1, 48
     547:	32c242        	addi	a4, a2, 50
     54a:	fec352        	addi	a5, a3, -2
     54d:	1892a2        	l16si	a10, a2, 48
     550:	fc2493855f 	{ or	a8, a5, a5; ae_zeroq56	aeq0 }
     555:	fc20ec240f 	{ ae_lp16f.i	aep2, a4, 0; ae_zerop48	aep1 }
     55a:	1b1aa6        	blti	a10, 1, 579 <cpd_block_proc+0x35>
     55d:	408814        	ae_lp16f.iu	aep0, a8, 2
     560:	079804        	ae_abssp24s	aep0, aep0
     563:	e20a0ffa9e 	{ addi	a9, a10, -1; ae_mulafp24s.hh	aeq0, aep2, aep0 }
     568:	0aa976        	loopgtz	a9, 576 <cpd_block_proc+0x32>
     56b:	d8066c881f 	{ ae_lp16f.iu	aep0, a8, 2; ae_maxp24s	aep1, aep1, aep0 }
     570:	079804        	ae_abssp24s	aep0, aep0
     573:	012004        	ae_mulafp24s.hh	aeq0, aep2, aep0

00000576 <cpd_block_proc+0x32>:
     576:	071814        	ae_maxp24s	aep1, aep1, aep0

00000579 <cpd_block_proc+0x35>:
     579:	4cc2f2        	addi	a15, a2, 76
     57c:	ff247d4f0f 	{ ae_lq32f.i	aeq2, a15, 0; ae_roundsq32asym	aeq3, aeq0 }
     581:	067a14        	ae_subsq56s	aeq1, aeq3, aeq2
     584:	3550f4        	ae_sraiq56	aeq1, aeq1, 3
     587:	06d934        	ae_roundsq32asym	aeq3, aeq1
     58a:	f9f64282ee 	{ addi	a14, a2, 40; ae_addsq56s	aeq1, aeq2, aeq3 }
     58f:	621f04        	ae_sq32f.i	aeq1, a15, 0
     592:	60ce04        	ae_lq32f.i	aeq3, a14, 0
     595:	039ad4        	ae_mulfq32sp16s.l	aeq3, aeq3, aep1
     598:	fe3c7052de 	{ l32i	a13, a2, 20; ae_roundsq32asym	aeq2, aeq3 }
     59d:	622104        	ae_sq32f.i	aeq2, a1, 0
     5a0:	0021c2        	l32i	a12, a1, 0
     5a3:	01adc7        	bge	a13, a12, 5a8 <cpd_block_proc+0x64>
     5a6:	52c9           	s32i.n	a12, a2, 20

000005a8 <cpd_block_proc+0x64>:
     5a8:	169282        	l16si	a8, a2, 44
     5ab:	0c9292        	l16si	a9, a2, 24
     5ae:	c09980        	sub	a9, a9, a8
     5b1:	11e956        	bnez	a9, 6d3 <cpd_block_proc+0x18f>
     5b4:	1722b2        	l32i	a11, a2, 92
     5b7:	1d0c           	movi.n	a13, 1
     5b9:	1652d2        	s16i	a13, a2, 44
     5bc:	53bbc0        	max	a11, a11, a12
     5bf:	11b9           	s32i.n	a11, a1, 4
     5c1:	1822d2        	l32i	a13, a2, 96
     5c4:	60c282        	addi	a8, a2, 96
     5c7:	690c           	movi.n	a9, 6
     5c9:	53bbd0        	max	a11, a11, a13
     5cc:	11b9           	s32i.n	a11, a1, 4
     5ce:	5cc2b2        	addi	a11, a2, 92
     5d1:	1822a2        	l32i	a10, a2, 96
     5d4:	09eba4        	ae_s32ip	a10, a11, 4
     5d7:	0f8976        	loop	a9, 5ea <cpd_block_proc+0xa6>
     5da:	1898           	l32i.n	a9, a8, 4
     5dc:	11a8           	l32i.n	a10, a1, 4
     5de:	884b           	addi.n	a8, a8, 4
     5e0:	539a90        	max	a9, a10, a9
     5e3:	1199           	s32i.n	a9, a1, 4
     5e5:	0898           	l32i.n	a9, a8, 0
     5e7:	09eb94        	ae_s32ip	a9, a11, 4

000005ea <cpd_block_proc+0xa6>:
     5ea:	0121e2        	l32i	a14, a1, 4
     5ed:	1662e2        	s32i	a14, a2, 88
     5f0:	006bc2        	s32i	a12, a11, 0
     5f3:	1622b2        	l32i	a11, a2, 88

000005f6 <cpd_block_proc+0xb2>:
     5f6:	12c8           	l32i.n	a12, a2, 4
     5f8:	122262        	l32i	a6, a2, 72
     5fb:	0020f0        	nop
     5fe:	02acb7        	bge	a12, a11, 604 <cpd_block_proc+0xc0>
     601:	003a06        	j	6ed <cpd_block_proc+0x1a9>
     604:	1ac282        	addi	a8, a2, 26
     607:	403804        	ae_lp16f.i	aep3, a8, 0
     60a:	0bb0a4        	ae_sllisp24s	aep2, aep3, 1
     60d:	07a314        	ae_addsp24s	aep1, aep2, aep3
     610:	001fd4        	ae_cvta32p24.l	a15, aep1
     613:	11f9           	s32i.n	a15, a1, 4
     615:	132282        	l32i	a8, a2, 76
     618:	02a8f7        	bge	a8, a15, 61e <cpd_block_proc+0xda>
     61b:	002746        	j	6bc <cpd_block_proc+0x178>
     61e:	fea9a1        	l32r	a10, c4 (7fff0000 <_end+0x7ffed9fc>)
     621:	0161a2        	s32i	a10, a1, 4

00000624 <cpd_block_proc+0xe0>:
     624:	1ec2b2        	addi	a11, a2, 30
     627:	48c2c2        	addi	a12, a2, 72
     62a:	21d2a0        	srai	a13, a10, 2
     62d:	368d34        	ae_cvtq48a32s	aeq2, a13
     630:	11d9           	s32i.n	a13, a1, 4
     632:	600c04        	ae_lq32f.i	aeq0, a12, 0
     635:	f952ac4b0f 	{ ae_lp16f.i	aep4, a11, 0; ae_subsq56s	aeq1, aeq2, aeq0 }
     63a:	034054        	ae_mulafq32sp16s.l	aeq0, aeq1, aep4
     63d:	060934        	ae_roundsq32asym	aeq0, aeq0
     640:	368904        	ae_trunca32q48	a9, aeq0
     643:	126292        	s32i	a9, a2, 72

00000646 <cpd_block_proc+0x102>:
     646:	058d           	mov.n	a8, a5
     648:	2e0c           	movi.n	a14, 2
     64a:	1892f2        	l16si	a15, a2, 48
     64d:	f298           	l32i.n	a9, a2, 60
     64f:	e2a8           	l32i.n	a10, a2, 56
     651:	405404        	ae_lp16f.i	aep5, a4, 0
     654:	364634        	ae_cvtq48a32s	aeq1, a6
     657:	d2c8           	l32i.n	a12, a2, 52
     659:	f3f6c0        	wur.ae_cbegin0	a12
     65c:	fb46b0a2de 	{ l32i	a13, a2, 40; ae_subsq56s	aeq3, aeq0, aeq1 }
     661:	d7f76b9ddf 	{ ae_truncp24a32x2	aep1, a13, a13; ae_mulfq32sp16s.l	aeq3, aeq3, aep5 }
     666:	0abd           	mov.n	a11, a10
     668:	ff3c5199cf 	{ addx2	a9, a9, a12; ae_roundsq32asym	aeq3, aeq3 }
     66d:	f3f790        	wur.ae_cend0	a9
     670:	2c2fa6        	blti	a15, 2, 6a0 <cpd_block_proc+0x15c>
     673:	1c0c           	movi.n	a12, 1
     675:	228076        	loop	a0, 69b <cpd_block_proc+0x157>
     678:	f9ee597bef 	{ ae_lp16f.c	aep7, a11, a14; ae_addsq56s	aeq1, aeq1, aeq3 }
     67d:	03f254        	ae_mulfq32sp16s.l	aeq2, aeq1, aep7
     680:	fb8420039e 	{ l16si	a9, a3, 0; ae_mulfq32sp24s.l	aeq2, aeq2, aep1 }
     685:	09ca94        	ae_s16ip	a9, a10, 2
     688:	35a194        	ae_slliq56	aeq2, aeq2, 6
     68b:	1baf64        	ae_roundsp16q48asym	aep6, aeq2
     68e:	42e814        	ae_sp16f.l.iu	aep6, a8, 2
     691:	1892f2        	l16si	a15, a2, 48
     694:	cc1b           	addi.n	a12, a12, 1
     696:	332b           	addi.n	a3, a3, 2
     698:	04acf7        	bge	a12, a15, 6a0 <cpd_block_proc+0x15c>

0000069b <cpd_block_proc+0x157>:
     69b:	fff586        	j	675 <cpd_block_proc+0x131>

0000069e <cpd_block_proc+0x15a>:
	...

000006a0 <cpd_block_proc+0x15c>:
     6a0:	fc24d92bef 	{ ae_lp16f.c	aep2, a11, a14; nop }
     6a5:	03aa14        	ae_mulfq32sp16s.l	aeq3, aeq0, aep2
     6a8:	fb856003ce 	{ l16si	a12, a3, 0; ae_mulfq32sp24s.l	aeq3, aeq3, aep1 }
     6ad:	6acd           	ae_s16i.n	a12, a10, 0
     6af:	35f194        	ae_slliq56	aeq3, aeq3, 6
     6b2:	1bbf04        	ae_roundsp16q48asym	aep0, aeq3
     6b5:	428814        	ae_sp16f.l.iu	aep0, a8, 2
     6b8:	e2b9           	s32i.n	a11, a2, 56
     6ba:	f01d           	retw.n

000006bc <cpd_block_proc+0x178>:
     6bc:	0d92d2        	l16si	a13, a2, 26
     6bf:	11dd00        	slli	a13, a13, 16
     6c2:	02ad87        	bge	a13, a8, 6c8 <cpd_block_proc+0x184>
     6c5:	003446        	j	79a <cpd_block_proc+0x256>
     6c8:	0e92a2        	l16si	a10, a2, 28
     6cb:	11aa00        	slli	a10, a10, 16
     6ce:	11a9           	s32i.n	a10, a1, 4
     6d0:	ffd406        	j	624 <cpd_block_proc+0xe0>

000006d3 <cpd_block_proc+0x18f>:
     6d3:	1e22d2        	l32i	a13, a2, 120
     6d6:	1622b2        	l32i	a11, a2, 88
     6d9:	e81b           	addi.n	a14, a8, 1
     6db:	1652e2        	s16i	a14, a2, 44
     6de:	53bbc0        	max	a11, a11, a12
     6e1:	53ddc0        	max	a13, a13, a12
     6e4:	1e62d2        	s32i	a13, a2, 120
     6e7:	1662b2        	s32i	a11, a2, 88
     6ea:	ffc206        	j	5f6 <cpd_block_proc+0xb2>

000006ed <cpd_block_proc+0x1a9>:
     6ed:	fe7081        	l32r	a8, b0 (30c <linear_interp>)
     6f0:	4292a2        	l16si	a10, a2, 132
     6f3:	0008e0        	callx8	a8
     6f6:	21a9           	s32i.n	a10, a1, 8
     6f8:	2298           	l32i.n	a9, a2, 8
     6fa:	58c272        	addi	a7, a2, 88
     6fd:	17aa97        	bge	a10, a9, 718 <cpd_block_proc+0x1d4>
     700:	07bd           	mov.n	a11, a7
     702:	2192a0        	srai	a9, a10, 2
     705:	fe6481        	l32r	a8, 98 (2c0 <Xdiv>)
     708:	2199           	s32i.n	a9, a1, 8
     70a:	08c1a2        	addi	a10, a1, 8
     70d:	0008e0        	callx8	a8
     710:	11aa00        	slli	a10, a10, 16
     713:	11a9           	s32i.n	a10, a1, 4
     715:	0015c6        	j	770 <cpd_block_proc+0x22c>

00000718 <cpd_block_proc+0x1d4>:
     718:	4292a2        	l16si	a10, a2, 132
     71b:	fe6581        	l32r	a8, b0 (30c <linear_interp>)
     71e:	52b8           	l32i.n	a11, a2, 20
     720:	12c8           	l32i.n	a12, a2, 4
     722:	0008e0        	callx8	a8
     725:	fe6381        	l32r	a8, b4 (19fc <calc_slope_2>)
     728:	0422b2        	l32i	a11, a2, 16
     72b:	0222c2        	l32i	a12, a2, 8
     72e:	0008e0        	callx8	a8
     731:	21b8           	l32i.n	a11, a1, 8
     733:	4352a2        	s16i	a10, a2, 134
     736:	fe5e81        	l32r	a8, b0 (30c <linear_interp>)
     739:	22c8           	l32i.n	a12, a2, 8
     73b:	8aad           	ae_sext16	a10, a10
     73d:	0008e0        	callx8	a8
     740:	b14b           	addi.n	a11, a1, 4
     742:	11a9           	s32i.n	a10, a1, 4
     744:	2298           	l32i.n	a9, a2, 8
     746:	fe5481        	l32r	a8, 98 (2c0 <Xdiv>)
     749:	a1cb           	addi.n	a10, a1, 12
     74b:	219290        	srai	a9, a9, 2
     74e:	3199           	s32i.n	a9, a1, 12
     750:	0008e0        	callx8	a8
     753:	07bd           	mov.n	a11, a7
     755:	2198           	l32i.n	a9, a1, 8
     757:	31a9           	s32i.n	a10, a1, 12
     759:	fe4f81        	l32r	a8, 98 (2c0 <Xdiv>)
     75c:	a14b           	addi.n	a10, a1, 4
     75e:	219290        	srai	a9, a9, 2
     761:	1199           	s32i.n	a9, a1, 4
     763:	0008e0        	callx8	a8
     766:	31b8           	l32i.n	a11, a1, 12
     768:	d1aab0        	mul16s	a10, a10, a11
     76b:	11aad0        	slli	a10, a10, 3
     76e:	11a9           	s32i.n	a10, a1, 4

00000770 <cpd_block_proc+0x22c>:
     770:	122282        	l32i	a8, a2, 72
     773:	06a8a7        	bge	a8, a10, 77d <cpd_block_proc+0x239>
     776:	400204        	ae_lp16f.i	aep0, a2, 0
     779:	0000c6        	j	780 <cpd_block_proc+0x23c>

0000077c <cpd_block_proc+0x238>:
	...

0000077d <cpd_block_proc+0x239>:
     77d:	400214        	ae_lp16f.i	aep0, a2, 2

00000780 <cpd_block_proc+0x23c>:
     780:	360834        	ae_cvtq48a32s	aeq0, a8
     783:	c0da80        	sub	a13, a10, a8
     786:	364d34        	ae_cvtq48a32s	aeq1, a13
     789:	030054        	ae_mulafq32sp16s.l	aeq0, aeq1, aep0
     78c:	fc245411df 	{ s32i.n	a13, a1, 4; ae_roundsq32asym	aeq0, aeq0 }
     791:	368c04        	ae_trunca32q48	a12, aeq0
     794:	1262c2        	s32i	a12, a2, 72
     797:	ffaac6        	j	646 <cpd_block_proc+0x102>

0000079a <cpd_block_proc+0x256>:
     79a:	fe4b81        	l32r	a8, c8 (3bc <INS_inter_gain>)
     79d:	02ad           	mov.n	a10, a2
     79f:	0008e0        	callx8	a8
     7a2:	11a9           	s32i.n	a10, a1, 4
     7a4:	ff9f06        	j	624 <cpd_block_proc+0xe0>

000007a7 <cpd_block_proc+0x263>:
	...

000007a8 <compander_VO_proc>:
     7a8:	00a136        	entry	a1, 80
     7ab:	121282        	l16ui	a8, a2, 36
     7ae:	1cc1a2        	addi	a10, a1, 28
     7b1:	0e5152        	s16i	a5, a1, 28
     7b4:	0a0816        	beqz	a8, 858 <compander_VO_proc+0xb0>
     7b7:	109292        	l16si	a9, a2, 32
     7ba:	9c92b2        	l16si	a11, a2, 0x138
     7bd:	0f52b2        	s16i	a11, a2, 30
     7c0:	69bc           	beqz.n	a9, 7fa <compander_VO_proc+0x52>
     7c2:	341926        	beqi	a9, 1, 7fa <compander_VO_proc+0x52>
     7c5:	312926        	beqi	a9, 2, 7fa <compander_VO_proc+0x52>
     7c8:	3ea1b2        	movi	a11, 0x13e
     7cb:	024966        	bnei	a9, 4, 7d1 <compander_VO_proc+0x29>
     7ce:	002206        	j	85a <compander_VO_proc+0xb2>
     7d1:	025966        	bnei	a9, 5, 7d7 <compander_VO_proc+0x2f>
     7d4:	002086        	j	85a <compander_VO_proc+0xb2>
     7d7:	7f6926        	beqi	a9, 6, 85a <compander_VO_proc+0xb2>
     7da:	401a04        	ae_lp16f.i	aep1, a10, 0
     7dd:	01d2d2        	addmi	a13, a2, 0x100
     7e0:	9f92c2        	l16si	a12, a2, 0x13e
     7e3:	3ecdd2        	addi	a13, a13, 62
     7e6:	400d04        	ae_lp16f.i	aep0, a13, 0
     7e9:	094c16        	beqz	a12, 881 <compander_VO_proc+0xd9>
     7ec:	011844        	ae_mulfp24s.hh	aeq1, aep1, aep0
     7ef:	061934        	ae_roundsq32asym	aeq0, aeq1
     7f2:	620104        	ae_sq32f.i	aeq0, a1, 0
     7f5:	000706        	j	815 <compander_VO_proc+0x6d>

000007f8 <compander_VO_proc+0x50>:
	...

000007fa <compander_VO_proc+0x52>:
     7fa:	9d92f2        	l16si	a15, a2, 0x13a
     7fd:	9e92e2        	l16si	a14, a2, 0x13c
     800:	269d           	ae_l16si.n	a9, a6, 0
     802:	080c           	movi.n	a8, 0
     804:	36ad           	ae_l16si.n	a10, a6, 2
     806:	119900        	slli	a9, a9, 16
     809:	006192        	s32i	a9, a1, 0
     80c:	7288a0        	salt	a8, a8, a10
     80f:	93ef80        	movnez	a14, a15, a8
     812:	0f52e2        	s16i	a14, a2, 30

00000815 <compander_VO_proc+0x6d>:
     815:	189292        	l16si	a9, a2, 48
     818:	d25490        	quos	a5, a4, a9
     81b:	3915a6        	blti	a5, 1, 858 <compander_VO_proc+0xb0>
     81e:	040c           	movi.n	a4, 0
     820:	26c272        	addi	a7, a2, 38
     823:	1169f0        	slli	a6, a9, 1
     826:	28c2b2        	addi	a11, a2, 40
     829:	0161b2        	s32i	a11, a1, 4

0000082c <compander_VO_proc+0x84>:
     82c:	fe2881        	l32r	a8, cc (544 <cpd_block_proc>)
     82f:	11b8           	l32i.n	a11, a1, 4
     831:	01a8           	l32i.n	a10, a1, 0
     833:	364a34        	ae_cvtq48a32s	aeq1, a10
     836:	60cb04        	ae_lq32f.i	aeq3, a11, 0
     839:	f8ceac070f 	{ ae_lp16f.i	aep0, a7, 0; ae_subsq56s	aeq0, aeq1, aeq3 }
     83e:	038814        	ae_mulafq32sp16s.l	aeq3, aeq0, aep0
     841:	02ad           	mov.n	a10, a2
     843:	fe3c53b33f 	{ or	a11, a3, a3; ae_roundsq32asym	aeq2, aeq3 }
     848:	36a904        	ae_trunca32q48	a9, aeq2
     84b:	0a6292        	s32i	a9, a2, 40
     84e:	0008e0        	callx8	a8
     851:	336a           	add.n	a3, a3, a6
     853:	441b           	addi.n	a4, a4, 1
     855:	d39547        	bne	a5, a4, 82c <compander_VO_proc+0x84>

00000858 <compander_VO_proc+0xb0>:
     858:	f01d           	retw.n

0000085a <compander_VO_proc+0xb2>:
     85a:	4122b4        	ae_lp16f.x	aep2, a2, a11
     85d:	401624        	ae_lp16f.i	aep1, a6, 4
     860:	cd1c           	movi.n	a13, 28
     862:	9f92c2        	l16si	a12, a2, 0x13e
     865:	4101d4        	ae_lp16f.x	aep0, a1, a13
     868:	e804c0029f 	{ movi	a9, 2; ae_mulfp24s.hh	aeq1, aep1, aep0 }
     86d:	8c9c           	beqz.n	a12, 889 <compander_VO_proc+0xe1>
     86f:	341934        	ae_sllasq56s	aeq0, aeq1, a9
     872:	1b8d34        	ae_roundsp24q48asym	aep3, aeq0
     875:	01ba44        	ae_mulfp24s.hh	aeq3, aep3, aep2
     878:	06b934        	ae_roundsq32asym	aeq2, aeq3
     87b:	622104        	ae_sq32f.i	aeq2, a1, 0
     87e:	ffe4c6        	j	815 <compander_VO_proc+0x6d>

00000881 <compander_VO_proc+0xd9>:
     881:	11e500        	slli	a14, a5, 16
     884:	01e9           	s32i.n	a14, a1, 0
     886:	ffe2c6        	j	815 <compander_VO_proc+0x6d>

00000889 <compander_VO_proc+0xe1>:
     889:	019844        	ae_mulfp24s.hh	aeq3, aep1, aep0
     88c:	34b934        	ae_sllasq56s	aeq2, aeq3, a9
     88f:	622104        	ae_sq32f.i	aeq2, a1, 0
     892:	ffdfc6        	j	815 <compander_VO_proc+0x6d>

00000895 <compander_VO_proc+0xed>:
     895:	000000                                        ...

00000898 <compander_AU_PARA_init>:
     898:	004136        	entry	a1, 32
     89b:	fe0d71        	l32r	a7, d0 (c44 <AU_GBL_init+0xbc>)
     89e:	046416        	beqz	a4, 8e8 <compander_AU_PARA_init+0x50>
     8a1:	840b           	addi.n	a8, a4, -1
     8a3:	0b4816        	beqz	a8, 95b <compander_AU_PARA_init+0xc3>
     8a6:	fec492        	addi	a9, a4, -2
     8a9:	11c916        	beqz	a9, 9c9 <compander_AU_PARA_init+0x131>

000008ac <compander_AU_PARA_init+0x14>:
     8ac:	5c0c           	movi.n	a12, 5
     8ae:	02d2b2        	addmi	a11, a2, 0x200
     8b1:	01d3a2        	addmi	a10, a3, 0x100
     8b4:	fdfa81        	l32r	a8, 9c (1e68 <xt_memcpy>)
     8b7:	80caa2        	addi	a10, a10, -128
     8ba:	c8cbb2        	addi	a11, a11, -56
     8bd:	0008e0        	callx8	a8
     8c0:	5c0c           	movi.n	a12, 5
     8c2:	66c222        	addi	a2, a2, 102
     8c5:	07d3a2        	addmi	a10, a3, 0x700
     8c8:	fdf581        	l32r	a8, 9c (1e68 <xt_memcpy>)
     8cb:	20caa2        	addi	a10, a10, 32
     8ce:	02bd           	mov.n	a11, a2
     8d0:	0008e0        	callx8	a8
     8d3:	02bd           	mov.n	a11, a2
     8d5:	5c0c           	movi.n	a12, 5
     8d7:	fdf181        	l32r	a8, 9c (1e68 <xt_memcpy>)
     8da:	0ed3a2        	addmi	a10, a3, 0xe00
     8dd:	c0caa2        	addi	a10, a10, -64
     8e0:	0008e0        	callx8	a8
     8e3:	498626        	beqi	a6, 8, 930 <compander_AU_PARA_init+0x98>
     8e6:	f01d           	retw.n

000008e8 <compander_AU_PARA_init+0x50>:
     8e8:	114516        	beqz	a5, a00 <compander_AU_PARA_init+0x168>
     8eb:	03ad           	mov.n	a10, a3
     8ed:	fdeb81        	l32r	a8, 9c (1e68 <xt_memcpy>)
     8f0:	34c2b2        	addi	a11, a2, 52
     8f3:	4c0c           	movi.n	a12, 4
     8f5:	0008e0        	callx8	a8

000008f8 <compander_AU_PARA_init+0x60>:
     8f8:	a38b           	addi.n	a10, a3, 8
     8fa:	fde881        	l32r	a8, 9c (1e68 <xt_memcpy>)
     8fd:	44c2b2        	addi	a11, a2, 68
     900:	9c0c           	movi.n	a12, 9
     902:	0008e0        	callx8	a8
     905:	a38666        	bnei	a6, 8, 8ac <compander_AU_PARA_init+0x14>
     908:	5398           	l32i.n	a9, a3, 20
     90a:	23c8           	l32i.n	a12, a3, 8
     90c:	33b8           	l32i.n	a11, a3, 12
     90e:	43a8           	l32i.n	a10, a3, 16
     910:	cc8b           	addi.n	a12, a12, 8
     912:	bb8b           	addi.n	a11, a11, 8
     914:	aa8b           	addi.n	a10, a10, 8
     916:	998b           	addi.n	a9, a9, 8
     918:	219490        	srai	a9, a9, 4
     91b:	21a4a0        	srai	a10, a10, 4
     91e:	21b4b0        	srai	a11, a11, 4
     921:	21c4c0        	srai	a12, a12, 4
     924:	23c9           	s32i.n	a12, a3, 8
     926:	33b9           	s32i.n	a11, a3, 12
     928:	43a9           	s32i.n	a10, a3, 16
     92a:	5399           	s32i.n	a9, a3, 20
     92c:	ffdf06        	j	8ac <compander_AU_PARA_init+0x14>

0000092f <compander_AU_PARA_init+0x97>:
	...

00000930 <compander_AU_PARA_init+0x98>:
     930:	202392        	l32i	a9, a3, 128
     933:	05d382        	addmi	a8, a3, 0x500
     936:	e37a           	add.n	a14, a3, a7
     938:	5f2ed2        	l32i	a13, a14, 0x17c
     93b:	e8c882        	addi	a8, a8, -24
     93e:	8e28f2        	l32i	a15, a8, 0x238
     941:	998b           	addi.n	a9, a9, 8
     943:	219490        	srai	a9, a9, 4
     946:	dd8b           	addi.n	a13, a13, 8
     948:	21d4d0        	srai	a13, a13, 4
     94b:	206392        	s32i	a9, a3, 128
     94e:	ff8b           	addi.n	a15, a15, 8
     950:	21f4f0        	srai	a15, a15, 4
     953:	8e68f2        	s32i	a15, a8, 0x238
     956:	5f6ed2        	s32i	a13, a14, 0x17c
     959:	f01d           	retw.n

0000095b <compander_AU_PARA_init+0xc3>:
     95b:	07d3a2        	addmi	a10, a3, 0x700
     95e:	a0caa2        	addi	a10, a10, -96
     961:	0cb516        	beqz	a5, a30 <compander_AU_PARA_init+0x198>
     964:	b50b           	addi.n	a11, a5, -1
     966:	0d6b16        	beqz	a11, a40 <compander_AU_PARA_init+0x1a8>
     969:	fec5c2        	addi	a12, a5, -2
     96c:	0a0c16        	beqz	a12, a10 <compander_AU_PARA_init+0x178>
     96f:	4c0c           	movi.n	a12, 4
     971:	fdca81        	l32r	a8, 9c (1e68 <xt_memcpy>)
     974:	02d2b2        	addmi	a11, a2, 0x200
     977:	c0cbb2        	addi	a11, a11, -64
     97a:	0008e0        	callx8	a8

0000097d <compander_AU_PARA_init+0xe5>:
     97d:	22c2b2        	addi	a11, a2, 34
     980:	9c0c           	movi.n	a12, 9
     982:	fdc681        	l32r	a8, 9c (1e68 <xt_memcpy>)
     985:	07d3a2        	addmi	a10, a3, 0x700
     988:	a8caa2        	addi	a10, a10, -88
     98b:	0008e0        	callx8	a8
     98e:	f8c692        	addi	a9, a6, -8
     991:	f17956        	bnez	a9, 8ac <compander_AU_PARA_init+0x14>
     994:	05d3b2        	addmi	a11, a3, 0x500
     997:	e8cbb2        	addi	a11, a11, -24
     99a:	732ba2        	l32i	a10, a11, 0x1cc
     99d:	702be2        	l32i	a14, a11, 0x1c0
     9a0:	712bd2        	l32i	a13, a11, 0x1c4
     9a3:	722bc2        	l32i	a12, a11, 0x1c8
     9a6:	ee8b           	addi.n	a14, a14, 8
     9a8:	dd8b           	addi.n	a13, a13, 8
     9aa:	cc8b           	addi.n	a12, a12, 8
     9ac:	aa8b           	addi.n	a10, a10, 8
     9ae:	21a4a0        	srai	a10, a10, 4
     9b1:	21c4c0        	srai	a12, a12, 4
     9b4:	21d4d0        	srai	a13, a13, 4
     9b7:	21e4e0        	srai	a14, a14, 4
     9ba:	706be2        	s32i	a14, a11, 0x1c0
     9bd:	716bd2        	s32i	a13, a11, 0x1c4
     9c0:	726bc2        	s32i	a12, a11, 0x1c8
     9c3:	736ba2        	s32i	a10, a11, 0x1cc
     9c6:	ffb886        	j	8ac <compander_AU_PARA_init+0x14>

000009c9 <compander_AU_PARA_init+0x131>:
     9c9:	0cd392        	addmi	a9, a3, 0xc00
     9cc:	050516        	beqz	a5, a20 <compander_AU_PARA_init+0x188>
     9cf:	2c92c2        	l16si	a12, a2, 88
     9d2:	2b92d2        	l16si	a13, a2, 86
     9d5:	a059d2        	s16i	a13, a9, 0x140
     9d8:	a359c2        	s16i	a12, a9, 0x146

000009db <compander_AU_PARA_init+0x143>:
     9db:	5ec2b2        	addi	a11, a2, 94
     9de:	4c0c           	movi.n	a12, 4
     9e0:	fdaf81        	l32r	a8, 9c (1e68 <xt_memcpy>)
     9e3:	0dd3a2        	addmi	a10, a3, 0xd00
     9e6:	48caa2        	addi	a10, a10, 72
     9e9:	0008e0        	callx8	a8
     9ec:	32c2b2        	addi	a11, a2, 50
     9ef:	1c0c           	movi.n	a12, 1
     9f1:	fdaa81        	l32r	a8, 9c (1e68 <xt_memcpy>)
     9f4:	0dd3a2        	addmi	a10, a3, 0xd00
     9f7:	58caa2        	addi	a10, a10, 88
     9fa:	0008e0        	callx8	a8
     9fd:	ffaac6        	j	8ac <compander_AU_PARA_init+0x14>

00000a00 <compander_AU_PARA_init+0x168>:
     a00:	03ad           	mov.n	a10, a3
     a02:	fda681        	l32r	a8, 9c (1e68 <xt_memcpy>)
     a05:	3cc2b2        	addi	a11, a2, 60
     a08:	4c0c           	movi.n	a12, 4
     a0a:	0008e0        	callx8	a8
     a0d:	ffb9c6        	j	8f8 <compander_AU_PARA_init+0x60>

00000a10 <compander_AU_PARA_init+0x178>:
     a10:	fda381        	l32r	a8, 9c (1e68 <xt_memcpy>)
     a13:	1ac2b2        	addi	a11, a2, 26
     a16:	4c0c           	movi.n	a12, 4
     a18:	0008e0        	callx8	a8
     a1b:	ffd786        	j	97d <compander_AU_PARA_init+0xe5>

00000a1e <compander_AU_PARA_init+0x186>:
	...

00000a20 <compander_AU_PARA_init+0x188>:
     a20:	2e92b2        	l16si	a11, a2, 92
     a23:	2d92c2        	l16si	a12, a2, 90
     a26:	a059c2        	s16i	a12, a9, 0x140
     a29:	a359b2        	s16i	a11, a9, 0x146
     a2c:	ffeac6        	j	9db <compander_AU_PARA_init+0x143>

00000a2f <compander_AU_PARA_init+0x197>:
	...

00000a30 <compander_AU_PARA_init+0x198>:
     a30:	fd9b81        	l32r	a8, 9c (1e68 <xt_memcpy>)
     a33:	12c2b2        	addi	a11, a2, 18
     a36:	4c0c           	movi.n	a12, 4
     a38:	0008e0        	callx8	a8
     a3b:	ffcf86        	j	97d <compander_AU_PARA_init+0xe5>

00000a3e <compander_AU_PARA_init+0x1a6>:
	...

00000a40 <compander_AU_PARA_init+0x1a8>:
     a40:	fd9781        	l32r	a8, 9c (1e68 <xt_memcpy>)
     a43:	b2ab           	addi.n	a11, a2, 10
     a45:	4c0c           	movi.n	a12, 4
     a47:	0008e0        	callx8	a8
     a4a:	ffcbc6        	j	97d <compander_AU_PARA_init+0xe5>

00000a4d <compander_AU_PARA_init+0x1b5>:
     a4d:	000000                                        ...

00000a50 <compander_AU_Sub_init>:
     a50:	004136        	entry	a1, 32
     a53:	04ad           	mov.n	a10, a4
     a55:	fd9281        	l32r	a8, a0 (1e50 <xt_memset>)
     a58:	0b0c           	movi.n	a11, 0
     a5a:	50a3c2        	movi	a12, 0x350
     a5d:	0008e0        	callx8	a8
     a60:	02ad           	mov.n	a10, a2
     a62:	03bd           	mov.n	a11, a3
     a64:	05cd           	mov.n	a12, a5
     a66:	fd9b81        	l32r	a8, d4 (898 <compander_AU_PARA_init>)
     a69:	06dd           	mov.n	a13, a6
     a6b:	07ed           	mov.n	a14, a7
     a6d:	0008e0        	callx8	a8
     a70:	fd9b21        	l32r	a2, dc (8000000 <_end+0x7ffd9fc>)
     a73:	13d392        	addmi	a9, a3, 0x1300
     a76:	7419a2        	l16ui	a10, a9, 232
     a79:	fd9731        	l32r	a3, d8 (800000 <_end+0x7fd9fc>)
     a7c:	ecc992        	addi	a9, a9, -20
     a7f:	f6caa2        	addi	a10, a10, -10
     a82:	077a16        	beqz	a10, afd <compander_AU_Sub_init+0xad>
     a85:	0b2c           	movi.n	a11, 32
     a87:	0a2c           	movi.n	a10, 32
     a89:	00a4c2        	movi	a12, 0x400
     a8c:	1154c2        	s16i	a12, a4, 34
     a8f:	1054a2        	s16i	a10, a4, 32

00000a92 <compander_AU_Sub_init+0x42>:
     a92:	772526        	beqi	a5, 2, b0d <compander_AU_Sub_init+0xbd>
     a95:	02ad           	mov.n	a10, a2
     a97:	a429           	s32i.n	a2, a4, 40
     a99:	7e19e2        	l16ui	a14, a9, 252
     a9c:	ed1c           	movi.n	a13, 30
     a9e:	60a092        	movi	a9, 96
     aa1:	f6cee2        	addi	a14, a14, -10
     aa4:	839de0        	moveqz	a9, a13, a14
     aa7:	b499           	s32i.n	a9, a4, 44
     aa9:	778726        	beqi	a7, 8, b24 <compander_AU_Sub_init+0xd4>

00000aac <compander_AU_Sub_init+0x5c>:
     aac:	1326a6        	blti	a6, 2, ac3 <compander_AU_Sub_init+0x73>
     aaf:	118bf0        	slli	a8, a11, 1
     ab2:	11c9f0        	slli	a12, a9, 1
     ab5:	1194f2        	l16si	a15, a4, 34
     ab8:	b4c9           	s32i.n	a12, a4, 44
     aba:	105482        	s16i	a8, a4, 32
     abd:	21f1f0        	srai	a15, a15, 1
     ac0:	1154f2        	s16i	a15, a4, 34

00000ac3 <compander_AU_Sub_init+0x73>:
     ac3:	fd8781        	l32r	a8, e0 (1988 <calc_slope>)
     ac6:	54b8           	l32i.n	a11, a4, 20
     ac8:	24c8           	l32i.n	a12, a4, 8
     aca:	0008e0        	callx8	a8
     acd:	0d5452        	s16i	a5, a4, 26
     ad0:	f429           	s32i.n	a2, a4, 60
     ad2:	01d4b2        	addmi	a11, a4, 0x100
     ad5:	3c54a2        	s16i	a10, a4, 120
     ad8:	190c           	movi.n	a9, 1
     ada:	0c0c           	movi.n	a12, 0
     adc:	0f54c2        	s16i	a12, a4, 30
     adf:	1364c2        	s32i	a12, a4, 76
     ae2:	1264c2        	s32i	a12, a4, 72
     ae5:	0e5492        	s16i	a9, a4, 28
     ae8:	fd7fa1        	l32r	a10, e4 (7fffffff <_end+0x7fffd9fb>)
     aeb:	a0cbb2        	addi	a11, a11, -96
     aee:	c4b9           	s32i.n	a11, a4, 48
     af0:	d4b9           	s32i.n	a11, a4, 52
     af2:	1064a2        	s32i	a10, a4, 64
     af5:	248726        	beqi	a7, 8, b1d <compander_AU_Sub_init+0xcd>
     af8:	3f5472        	s16i	a7, a4, 126
     afb:	f01d           	retw.n

00000afd <compander_AU_Sub_init+0xad>:
     afd:	ab0c           	movi.n	a11, 10
     aff:	ad0c           	movi.n	a13, 10
     b01:	fd69e1        	l32r	a14, a8 (ccd <compander_AU_init+0x71>)
     b04:	1154e2        	s16i	a14, a4, 34
     b07:	1054d2        	s16i	a13, a4, 32
     b0a:	ffe106        	j	a92 <compander_AU_Sub_init+0x42>

00000b0d <compander_AU_Sub_init+0xbd>:
     b0d:	fd76a1        	l32r	a10, e8 (40000000 <_end+0x3fffd9fc>)
     b10:	fd76f1        	l32r	a15, e8 (40000000 <_end+0x3fffd9fc>)
     b13:	118bf0        	slli	a8, a11, 1
     b16:	b489           	s32i.n	a8, a4, 44
     b18:	a4f9           	s32i.n	a15, a4, 40
     b1a:	ffe946        	j	ac3 <compander_AU_Sub_init+0x73>

00000b1d <compander_AU_Sub_init+0xcd>:
     b1d:	f439           	s32i.n	a3, a4, 60
     b1f:	3f5472        	s16i	a7, a4, 126
     b22:	f01d           	retw.n

00000b24 <compander_AU_Sub_init+0xd4>:
     b24:	a439           	s32i.n	a3, a4, 40
     b26:	03ad           	mov.n	a10, a3
     b28:	ffe006        	j	aac <compander_AU_Sub_init+0x5c>

00000b2b <compander_AU_Sub_init+0xdb>:
	...

00000b2c <DBB_PARA_init>:
     b2c:	004136        	entry	a1, 32
     b2f:	03ad           	mov.n	a10, a3
     b31:	fd5a81        	l32r	a8, 9c (1e68 <xt_memcpy>)
     b34:	70c2b2        	addi	a11, a2, 112
     b37:	4c0c           	movi.n	a12, 4
     b39:	0008e0        	callx8	a8
     b3c:	a38b           	addi.n	a10, a3, 8
     b3e:	fd5781        	l32r	a8, 9c (1e68 <xt_memcpy>)
     b41:	7cc2b2        	addi	a11, a2, 124
     b44:	2c0c           	movi.n	a12, 2
     b46:	0008e0        	callx8	a8
     b49:	f01d           	retw.n

00000b4b <DBB_PARA_init+0x1f>:
	...

00000b4c <DBB_init>:
     b4c:	004136        	entry	a1, 32
     b4f:	03ad           	mov.n	a10, a3
     b51:	fd5381        	l32r	a8, a0 (1e50 <xt_memset>)
     b54:	0b0c           	movi.n	a11, 0
     b56:	ac0c           	movi.n	a12, 10
     b58:	0008e0        	callx8	a8
     b5b:	fd6481        	l32r	a8, ec (b2c <DBB_PARA_init>)
     b5e:	02ad           	mov.n	a10, a2
     b60:	03bd           	mov.n	a11, a3
     b62:	0008e0        	callx8	a8
     b65:	03a8           	l32i.n	a10, a3, 0
     b67:	fd6291        	l32r	a9, f0 (3fffffff <_end+0x3fffd9fb>)
     b6a:	fd5cb1        	l32r	a11, dc (8000000 <_end+0x7ffd9fc>)
     b6d:	11caf0        	slli	a12, a10, 1
     b70:	0529a7        	blt	a9, a10, b79 <DBB_init+0x2d>
     b73:	43b9           	s32i.n	a11, a3, 16
     b75:	33c9           	s32i.n	a12, a3, 12
     b77:	f01d           	retw.n

00000b79 <DBB_init+0x2d>:
     b79:	fd58d1        	l32r	a13, dc (8000000 <_end+0x7ffd9fc>)
     b7c:	fd5ae1        	l32r	a14, e4 (7fffffff <_end+0x7fffd9fb>)
     b7f:	33e9           	s32i.n	a14, a3, 12
     b81:	43d9           	s32i.n	a13, a3, 16
     b83:	f01d           	retw.n

00000b85 <DBB_init+0x39>:
     b85:	000000                                        ...

00000b88 <AU_GBL_init>:
     b88:	004136        	entry	a1, 32
     b8b:	03ad           	mov.n	a10, a3
     b8d:	fd4381        	l32r	a8, 9c (1e68 <xt_memcpy>)
     b90:	02bd           	mov.n	a11, a2
     b92:	02a0c2        	movi	a12, 2
     b95:	0008e0        	callx8	a8
     b98:	0b0c           	movi.n	a11, 0
     b9a:	0c1c           	movi.n	a12, 16
     b9c:	02d3a2        	addmi	a10, a3, 0x200
     b9f:	000392        	l8ui	a9, a3, 0
     ba2:	fd3f81        	l32r	a8, a0 (1e50 <xt_memset>)
     ba5:	639d           	ae_s16i.n	a9, a3, 0
     ba7:	10caa2        	addi	a10, a10, 16
     baa:	0008e0        	callx8	a8
     bad:	0b0c           	movi.n	a11, 0
     baf:	0c2c           	movi.n	a12, 32
     bb1:	fd3b81        	l32r	a8, a0 (1e50 <xt_memset>)
     bb4:	02d3a2        	addmi	a10, a3, 0x200
     bb7:	30caa2        	addi	a10, a10, 48
     bba:	0008e0        	callx8	a8
     bbd:	a34b           	addi.n	a10, a3, 4
     bbf:	fd3781        	l32r	a8, 9c (1e68 <xt_memcpy>)
     bc2:	78c2b2        	addi	a11, a2, 120
     bc5:	2c0c           	movi.n	a12, 2
     bc7:	0008e0        	callx8	a8
     bca:	1e2292        	l32i	a9, a2, 120
     bcd:	c0a252        	movi	a5, 0x2c0
     bd0:	99ac           	beqz.n	a9, bfd <AU_GBL_init+0x75>
     bd2:	4e1926        	beqi	a9, 1, c24 <AU_GBL_init+0x9c>

00000bd5 <AU_GBL_init+0x4d>:
     bd5:	041392        	l16ui	a9, a3, 8
     bd8:	fd3ab1        	l32r	a11, c0 (400000 <_end+0x3fd9fc>)
     bdb:	0a2c           	movi.n	a10, 32
     bdd:	029926        	beqi	a9, 10, be3 <AU_GBL_init+0x5b>
     be0:	0453a2        	s16i	a10, a3, 8

00000be3 <AU_GBL_init+0x5b>:
     be3:	33b9           	s32i.n	a11, a3, 12
     be5:	078426        	beqi	a4, 8, bf0 <AU_GBL_init+0x68>
     be8:	eca1c2        	movi	a12, 0x1ec
     beb:	0553c2        	s16i	a12, a3, 10
     bee:	f01d           	retw.n

00000bf0 <AU_GBL_init+0x68>:
     bf0:	eca1d2        	movi	a13, 0x1ec
     bf3:	fd40e1        	l32r	a14, f4 (40000 <_end+0x3d9fc>)
     bf6:	33e9           	s32i.n	a14, a3, 12
     bf8:	0553d2        	s16i	a13, a3, 10
     bfb:	f01d           	retw.n

00000bfd <AU_GBL_init+0x75>:
     bfd:	8c2c           	movi.n	a12, 40
     bff:	01d2b2        	addmi	a11, a2, 0x100
     c02:	02d3a2        	addmi	a10, a3, 0x200
     c05:	fd2581        	l32r	a8, 9c (1e68 <xt_memcpy>)
     c08:	70caa2        	addi	a10, a10, 112
     c0b:	80cbb2        	addi	a11, a11, -128
     c0e:	0008e0        	callx8	a8
     c11:	a35a           	add.n	a10, a3, a5
     c13:	8c2c           	movi.n	a12, 40
     c15:	fd2181        	l32r	a8, 9c (1e68 <xt_memcpy>)
     c18:	01d2b2        	addmi	a11, a2, 0x100
     c1b:	d0cbb2        	addi	a11, a11, -48
     c1e:	0008e0        	callx8	a8
     c21:	ffec06        	j	bd5 <AU_GBL_init+0x4d>

00000c24 <AU_GBL_init+0x9c>:
     c24:	8c2c           	movi.n	a12, 40
     c26:	01d2b2        	addmi	a11, a2, 0x100
     c29:	02d3a2        	addmi	a10, a3, 0x200
     c2c:	fd1c81        	l32r	a8, 9c (1e68 <xt_memcpy>)
     c2f:	70caa2        	addi	a10, a10, 112
     c32:	20cbb2        	addi	a11, a11, 32
     c35:	0008e0        	callx8	a8
     c38:	a35a           	add.n	a10, a3, a5
     c3a:	8c2c           	movi.n	a12, 40
     c3c:	fd1881        	l32r	a8, 9c (1e68 <xt_memcpy>)
     c3f:	01d2b2        	addmi	a11, a2, 0x100
     c42:	70cbb2        	addi	a11, a11, 112
     c45:	0008e0        	callx8	a8
     c48:	ffe246        	j	bd5 <AU_GBL_init+0x4d>

00000c4b <AU_GBL_init+0xc3>:
	...

00000c4c <compander_AU_SetFrame120_init>:
     c4c:	004136        	entry	a1, 32
     c4f:	13d242        	addmi	a4, a2, 0x1300
     c52:	a30c           	movi.n	a3, 10
     c54:	745432        	s16i	a3, a4, 232
     c57:	f01d           	retw.n

00000c59 <compander_AU_SetFrame120_init+0xd>:
     c59:	000000                                        ...

00000c5c <compander_AU_init>:
     c5c:	004136        	entry	a1, 32
     c5f:	03ad           	mov.n	a10, a3
     c61:	02bd           	mov.n	a11, a2
     c63:	02cd           	mov.n	a12, a2
     c65:	0d0c           	movi.n	a13, 0
     c67:	fd2481        	l32r	a8, f8 (a50 <compander_AU_Sub_init>)
     c6a:	20e550        	or	a14, a5, a5
     c6d:	04fd           	mov.n	a15, a4
     c6f:	0008e0        	callx8	a8
     c72:	03ad           	mov.n	a10, a3
     c74:	02bd           	mov.n	a11, a2
     c76:	1d0c           	movi.n	a13, 1
     c78:	05ed           	mov.n	a14, a5
     c7a:	20f440        	or	a15, a4, a4
     c7d:	07d262        	addmi	a6, a2, 0x700
     c80:	fd1e81        	l32r	a8, f8 (a50 <compander_AU_Sub_init>)
     c83:	a0c662        	addi	a6, a6, -96
     c86:	20c660        	or	a12, a6, a6
     c89:	0008e0        	callx8	a8
     c8c:	05ed           	mov.n	a14, a5
     c8e:	03ad           	mov.n	a10, a3
     c90:	02bd           	mov.n	a11, a2
     c92:	2d0c           	movi.n	a13, 2
     c94:	04fd           	mov.n	a15, a4
     c96:	fd1881        	l32r	a8, f8 (a50 <compander_AU_Sub_init>)
     c99:	0dd2c2        	addmi	a12, a2, 0xd00
     c9c:	40ccc2        	addi	a12, a12, 64
     c9f:	0008e0        	callx8	a8
     ca2:	04cd           	mov.n	a12, a4
     ca4:	03ad           	mov.n	a10, a3
     ca6:	fd1581        	l32r	a8, fc (b88 <AU_GBL_init>)
     ca9:	14d2b2        	addmi	a11, a2, 0x1400
     cac:	e0cbb2        	addi	a11, a11, -32
     caf:	0008e0        	callx8	a8
     cb2:	03ad           	mov.n	a10, a3
     cb4:	fd1381        	l32r	a8, 100 (b4c <DBB_init>)
     cb7:	17d2b2        	addmi	a11, a2, 0x1700
     cba:	f0cbb2        	addi	a11, a11, -16
     cbd:	0008e0        	callx8	a8
     cc0:	fd1181        	l32r	a8, 104 (374 <AU_INS_coef_init>)
     cc3:	02ad           	mov.n	a10, a2
     cc5:	0008e0        	callx8	a8
     cc8:	fd0f81        	l32r	a8, 104 (374 <AU_INS_coef_init>)
     ccb:	06ad           	mov.n	a10, a6
     ccd:	0008e0        	callx8	a8
     cd0:	0cd2a2        	addmi	a10, a2, 0xc00
     cd3:	05d292        	addmi	a9, a2, 0x500
     cd6:	8d29b2        	l32i	a11, a9, 0x234
     cd9:	8c29c2        	l32i	a12, a9, 0x230
     cdc:	746ac2        	s32i	a12, a10, 0x1d0
     cdf:	756ab2        	s32i	a11, a10, 0x1d4
     ce2:	8e2992        	l32i	a9, a9, 0x238
     ce5:	766a92        	s32i	a9, a10, 0x1d8
     ce8:	f01d           	retw.n

00000cea <compander_AU_init+0x8e>:
	...

00000cec <F_CPD_Ins_Gain>:
     cec:	006136        	entry	a1, 48
     cef:	01ad           	mov.n	a10, a1
     cf1:	01d2b2        	addmi	a11, a2, 0x100
     cf4:	202292        	l32i	a9, a2, 128
     cf7:	fce881        	l32r	a8, 98 (2c0 <Xdiv>)
     cfa:	0199           	s32i.n	a9, a1, 0
     cfc:	8ccbb2        	addi	a11, a11, -116
     cff:	0008e0        	callx8	a8
     d02:	b14b           	addi.n	a11, a1, 4
     d04:	fce581        	l32r	a8, 98 (2c0 <Xdiv>)
     d07:	119a00        	slli	a9, a10, 16
     d0a:	11a9           	s32i.n	a10, a1, 4
     d0c:	2422a2        	l32i	a10, a2, 144
     d0f:	01a9           	s32i.n	a10, a1, 0
     d11:	1199           	s32i.n	a9, a1, 4
     d13:	01ad           	mov.n	a10, a1
     d15:	0008e0        	callx8	a8
     d18:	11b8           	l32i.n	a11, a1, 4
     d1a:	2522e2        	l32i	a14, a2, 148
     d1d:	01e9           	s32i.n	a14, a1, 0
     d1f:	2622d2        	l32i	a13, a2, 152
     d22:	11ca00        	slli	a12, a10, 16
     d25:	368e34        	ae_cvtq48a32s	aeq2, a14
     d28:	b2bbd0        	mulsh	a11, a11, a13
     d2b:	364c34        	ae_cvtq48a32s	aeq1, a12
     d2e:	11bbf0        	slli	a11, a11, 1
     d31:	fbea7ecb3f 	{ ae_cvtq48a32s	aeq0, a11; ae_addsq56s	aeq3, aeq1, aeq2 }
     d36:	06b804        	ae_addsq56s	aeq2, aeq3, aeq0
     d39:	366094        	ae_sllisq56s	aeq1, aeq2, 2
     d3c:	061934        	ae_roundsq32asym	aeq0, aeq1
     d3f:	620104        	ae_sq32f.i	aeq0, a1, 0
     d42:	0128           	l32i.n	a2, a1, 0
     d44:	f01d           	retw.n

00000d46 <F_CPD_Ins_Gain+0x5a>:
	...

00000d48 <F_CPD_Mag_In>:
     d48:	00a136        	entry	a1, 80
     d4b:	028d           	mov.n	a8, a2
     d4d:	fcc372        	addi	a7, a3, -4
     d50:	fe2482229e 	{ addi	a9, a2, 34; ae_zeroq56	aeq2 }
     d55:	632144        	ae_sq56s.i	aeq2, a1, 32
     d58:	400904        	ae_lp16f.i	aep0, a9, 0
     d5b:	109292        	l16si	a9, a2, 32
     d5e:	601714        	ae_lq32f.iu	aeq0, a7, 4
     d61:	ff1453277f 	{ or	a2, a7, a7; ae_movq56	aeq3, aeq2 }
     d66:	0aa976        	loopgtz	a9, d74 <F_CPD_Mag_In+0x2c>
     d69:	fd00bd121f 	{ ae_lq32f.iu	aeq0, a2, 4; ae_absq56	aeq1, aeq0 }
     d6e:	0192c4        	ae_maxq56s	aeq2, aeq2, aeq1
     d71:	088b64        	ae_mulafq32sp24s.l	aeq3, aeq1, aep0

00000d74 <F_CPD_Mag_In+0x2c>:
     d74:	063b34        	ae_movq56	aeq0, aeq3
     d77:	ff147a013f 	{ ae_sq56s.i	aeq0, a1, 24; ae_movq56	aeq3, aeq2 }
     d7c:	542526        	beqi	a5, 2, dd4 <F_CPD_Mag_In+0x8c>

00000d7f <F_CPD_Mag_In+0x37>:
     d7f:	ff84fd814f 	{ ae_lq56.i	aeq0, a1, 32; ae_satq48s	aeq3, aeq3 }
     d84:	3c1666        	bnei	a6, 1, dc4 <F_CPD_Mag_In+0x7c>
     d87:	fc247dc13f 	{ ae_lq56.i	aeq2, a1, 24; ae_roundsq32asym	aeq0, aeq0 }
     d8c:	fe347f32ef 	{ ae_trunca32q48	a2, aeq3; ae_roundsq32asym	aeq2, aeq2 }
     d91:	d6c03a014f 	{ ae_sq56s.i	aeq0, a1, 32; ae_ltq56s	b0, aeq2, aeq0 }
     d96:	632134        	ae_sq56s.i	aeq2, a1, 24
     d99:	614144        	ae_lq56.i	aeq1, a1, 32
     d9c:	0304a0        	rsr.br	a10
     d9f:	04a0a0        	extui	a10, a10, 0, 1
     da2:	052a16        	beqz	a10, df8 <F_CPD_Mag_In+0xb0>
     da5:	fcd8c1        	l32r	a12, 108 (1000 <F_CPD_Intp_Out+0xe8>)
     da8:	01d8d2        	addmi	a13, a8, 0x100
     dab:	8ccdd2        	addi	a13, a13, -116
     dae:	608d04        	ae_lq32f.i	aeq2, a13, 0
     db1:	f8ca98accf 	{ ae_cvtp24a16x2.ll	aep2, a12, a12; ae_subsq56s	aeq0, aeq1, aeq2 }
     db6:	03a014        	ae_mulafq32sp16s.l	aeq2, aeq0, aep2
     db9:	066934        	ae_roundsq32asym	aeq1, aeq2
     dbc:	369b04        	ae_trunca32q48	a11, aeq1
     dbf:	2368b2        	s32i	a11, a8, 140
     dc2:	f01d           	retw.n

00000dc4 <F_CPD_Mag_In+0x7c>:
     dc4:	36b204        	ae_trunca32q48	a2, aeq3
     dc7:	fcc7e1        	l32r	a14, e4 (7fffffff <_end+0x7fffd9fb>)
     dca:	2368e2        	s32i	a14, a8, 140
     dcd:	f01d           	retw.n

00000dcf <F_CPD_Mag_In+0x87>:
     dcf:	00000000 61c14400                                .....

00000dd4 <F_CPD_Mag_In+0x8c>:
     dd4:	61c144        	ae_lq56.i	aeq3, a1, 32
     dd7:	fcc422        	addi	a2, a4, -4
     dda:	601214        	ae_lq32f.iu	aeq0, a2, 4
     ddd:	0aa976        	loopgtz	a9, deb <F_CPD_Mag_In+0xa3>
     de0:	fd00bd121f 	{ ae_lq32f.iu	aeq0, a2, 4; ae_absq56	aeq1, aeq0 }
     de5:	0192c4        	ae_maxq56s	aeq2, aeq2, aeq1
     de8:	088b64        	ae_mulafq32sp24s.l	aeq3, aeq1, aep0

00000deb <F_CPD_Mag_In+0xa3>:
     deb:	fe1c7f213f 	{ ae_movq56	aeq0, aeq2; ae_movq56	aeq2, aeq3 }

00000df0 <F_CPD_Mag_In+0xa8>:
     df0:	ff047a214f 	{ ae_sq56s.i	aeq2, a1, 32; ae_movq56	aeq3, aeq0 }
     df5:	ffe186        	j	d7f <F_CPD_Mag_In+0x37>

00000df8 <F_CPD_Mag_In+0xb0>:
     df8:	614134        	ae_lq56.i	aeq1, a1, 24
     dfb:	fcc321        	l32r	a2, 108 (1000 <F_CPD_Intp_Out+0xe8>)
     dfe:	01d872        	addmi	a7, a8, 0x100
     e01:	8cc772        	addi	a7, a7, -116
     e04:	608704        	ae_lq32f.i	aeq2, a7, 0
     e07:	f8ca98b22f 	{ ae_cvtp24a16x2.ll	aep3, a2, a2; ae_subsq56s	aeq0, aeq1, aeq2 }
     e0c:	03b014        	ae_mulafq32sp16s.l	aeq2, aeq0, aep3
     e0f:	fd347f32ef 	{ ae_trunca32q48	a2, aeq3; ae_roundsq32asym	aeq1, aeq2 }
     e14:	369f04        	ae_trunca32q48	a15, aeq1
     e17:	2368f2        	s32i	a15, a8, 140
     e1a:	f01d           	retw.n

00000e1c <F_CPD_Max_In_Buf>:
     e1c:	006136        	entry	a1, 48
     e1f:	1b0c           	movi.n	a11, 1
     e21:	0e9292        	l16si	a9, a2, 28
     e24:	0c92c2        	l16si	a12, a2, 24
     e27:	a288           	l32i.n	a8, a2, 40
     e29:	1222a2        	l32i	a10, a2, 72
     e2c:	c0cc90        	sub	a12, a12, a9
     e2f:	05a837        	bge	a8, a3, e38 <F_CPD_Max_In_Buf+0x1c>
     e32:	0a6232        	s32i	a3, a2, 40
     e35:	0f52b2        	s16i	a11, a2, 30

00000e38 <F_CPD_Max_In_Buf+0x1c>:
     e38:	08cc56        	bnez	a12, ec8 <F_CPD_Max_In_Buf+0xac>
     e3b:	1422e2        	l32i	a14, a2, 80
     e3e:	54c282        	addi	a8, a2, 84
     e41:	0e52b2        	s16i	a11, a2, 28
     e44:	53eea0        	max	a14, a14, a10
     e47:	01e9           	s32i.n	a14, a1, 0
     e49:	1522f2        	l32i	a15, a2, 84
     e4c:	690c           	movi.n	a9, 6
     e4e:	50c2b2        	addi	a11, a2, 80
     e51:	53eef0        	max	a14, a14, a15
     e54:	0061e2        	s32i	a14, a1, 0
     e57:	1522d2        	l32i	a13, a2, 84
     e5a:	09ebd4        	ae_s32ip	a13, a11, 4
     e5d:	0f8976        	loop	a9, e70 <F_CPD_Max_In_Buf+0x54>
     e60:	1898           	l32i.n	a9, a8, 4
     e62:	01a8           	l32i.n	a10, a1, 0
     e64:	884b           	addi.n	a8, a8, 4
     e66:	539a90        	max	a9, a10, a9
     e69:	0199           	s32i.n	a9, a1, 0
     e6b:	0898           	l32i.n	a9, a8, 0
     e6d:	09eb94        	ae_s32ip	a9, a11, 4

00000e70 <F_CPD_Max_In_Buf+0x54>:
     e70:	122282        	l32i	a8, a2, 72
     e73:	0198           	l32i.n	a9, a1, 0
     e75:	136292        	s32i	a9, a2, 76
     e78:	0b89           	s32i.n	a8, a11, 0
     e7a:	0f92f2        	l16si	a15, a2, 30
     e7d:	126232        	s32i	a3, a2, 72
     e80:	1f1fa6        	blti	a15, 1, ea3 <F_CPD_Max_In_Buf+0x87>
     e83:	a2a8           	l32i.n	a10, a2, 40
     e85:	fc9681        	l32r	a8, e0 (1988 <calc_slope>)
     e88:	0522b2        	l32i	a11, a2, 20
     e8b:	22c8           	l32i.n	a12, a2, 8
     e8d:	0008e0        	callx8	a8
     e90:	fc7db1        	l32r	a11, 84 (7fff <_end+0x59fb>)
     e93:	090c           	movi.n	a9, 0
     e95:	80dac2        	addmi	a12, a10, 0xffff8000
     e98:	83abc0        	moveqz	a10, a11, a12
     e9b:	01a9           	s32i.n	a10, a1, 0
     e9d:	3c52a2        	s16i	a10, a2, 120
     ea0:	0f5292        	s16i	a9, a2, 30

00000ea3 <F_CPD_Max_In_Buf+0x87>:
     ea3:	1322a2        	l32i	a10, a2, 76

00000ea6 <F_CPD_Max_In_Buf+0x8a>:
     ea6:	22b8           	l32i.n	a11, a2, 8
     ea8:	402ba7        	blt	a11, a10, eec <F_CPD_Max_In_Buf+0xd0>
     eab:	2022a2        	l32i	a10, a2, 128
     eae:	2322b2        	l32i	a11, a2, 140
     eb1:	90daa0        	addx2	a13, a10, a10
     eb4:	0aabd7        	bge	a11, a13, ec2 <F_CPD_Max_In_Buf+0xa6>
     eb7:	252ab7        	blt	a10, a11, ee0 <F_CPD_Max_In_Buf+0xc4>
     eba:	429222        	l16si	a2, a2, 132
     ebd:	112200        	slli	a2, a2, 16
     ec0:	f01d           	retw.n

00000ec2 <F_CPD_Max_In_Buf+0xa6>:
     ec2:	fc8821        	l32r	a2, e4 (7fffffff <_end+0x7fffd9fb>)
     ec5:	f01d           	retw.n

00000ec7 <F_CPD_Max_In_Buf+0xab>:
	...

00000ec8 <F_CPD_Max_In_Buf+0xac>:
     ec8:	1322b2        	l32i	a11, a2, 76
     ecb:	c91b           	addi.n	a12, a9, 1
     ecd:	53aa30        	max	a10, a10, a3
     ed0:	1262a2        	s32i	a10, a2, 72
     ed3:	0e52c2        	s16i	a12, a2, 28
     ed6:	53aab0        	max	a10, a10, a11
     ed9:	1362a2        	s32i	a10, a2, 76
     edc:	fff186        	j	ea6 <F_CPD_Max_In_Buf+0x8a>

00000edf <F_CPD_Max_In_Buf+0xc3>:
	...

00000ee0 <F_CPD_Max_In_Buf+0xc4>:
     ee0:	fc8b81        	l32r	a8, 10c (cec <F_CPD_Ins_Gain>)
     ee3:	02ad           	mov.n	a10, a2
     ee5:	0008e0        	callx8	a8
     ee8:	0a2d           	mov.n	a2, a10
     eea:	f01d           	retw.n

00000eec <F_CPD_Max_In_Buf+0xd0>:
     eec:	fc6b81        	l32r	a8, 98 (2c0 <Xdiv>)
     eef:	364b34        	ae_cvtq48a32s	aeq1, a11
     ef2:	c0aab0        	sub	a10, a10, a11
     ef5:	368a34        	ae_cvtq48a32s	aeq2, a10
     ef8:	78c2b2        	addi	a11, a2, 120
     efb:	400b04        	ae_lp16f.i	aep0, a11, 0
     efe:	030894        	ae_mulafq32sp16s.l	aeq1, aeq2, aep0
     f01:	a28b           	addi.n	a10, a2, 8
     f03:	fc2c53b11f 	{ or	a11, a1, a1; ae_roundsq32asym	aeq0, aeq1 }
     f08:	368904        	ae_trunca32q48	a9, aeq0
     f0b:	0199           	s32i.n	a9, a1, 0
     f0d:	0008e0        	callx8	a8
     f10:	112a00        	slli	a2, a10, 16
     f13:	f01d           	retw.n

00000f15 <F_CPD_Max_In_Buf+0xf9>:
     f15:	000000                                        ...

00000f18 <F_CPD_Intp_Out>:
     f18:	008136        	entry	a1, 64
     f1b:	fec682        	addi	a8, a6, -2
     f1e:	4f1c           	movi.n	a15, 20
     f20:	0a5172        	s16i	a7, a1, 20
     f23:	02cd           	mov.n	a12, a2
     f25:	22cc92        	addi	a9, a12, 34
     f28:	38ccb2        	addi	a11, a12, 56
     f2b:	032d           	mov.n	a2, a3
     f2d:	102ce2        	l32i	a14, a12, 64
     f30:	cca8           	l32i.n	a10, a12, 48
     f32:	3f9c72        	l16si	a7, a12, 126
     f35:	4121f4        	ae_lp16f.x	aep2, a1, a15
     f38:	f3f6a0        	wur.ae_cbegin0	a10
     f3b:	360e34        	ae_cvtq48a32s	aeq0, a14
     f3e:	3ccc32        	addi	a3, a12, 60
     f41:	60c304        	ae_lq32f.i	aeq3, a3, 0
     f44:	f9de55e5ef 	{ sub	a14, a5, a14; ae_truncp24q48x2	aep3, aeq3, aeq3 }
     f49:	f88c7ecebf 	{ ae_cvtq48a32s	aeq2, a14; ae_mulfq32sp24s.l	aeq1, aeq0, aep3 }
     f4e:	f68ab111ae 	{ l32i	a10, a1, 68; ae_mulafq32sp16s.l	aeq0, aeq2, aep2 }
     f53:	bc38           	l32i.n	a3, a12, 44
     f55:	fe2471015e 	{ l32i	a5, a1, 64; ae_roundsq32asym	aeq2, aeq0 }
     f5a:	36ad04        	ae_trunca32q48	a13, aeq2
     f5d:	106cd2        	s32i	a13, a12, 64
     f60:	60cb04        	ae_lq32f.i	aeq3, a11, 0
     f63:	f8de610cee 	{ l16si	a14, a12, 32; ae_truncp24q48x2	aep1, aeq3, aeq3 }
     f68:	fb0430dcde 	{ l32i	a13, a12, 52; ae_mulfq32sp24s.h	aeq2, aeq2, aep1 }
     f6d:	400904        	ae_lp16f.i	aep0, a9, 0
     f70:	fa36f0cc9e 	{ l32i	a9, a12, 48; ae_subq56	aeq2, aeq2, aeq1 }
     f75:	d76213fddf 	{ or	a15, a13, a13; ae_mulfq32sp16s.h	aeq2, aeq2, aep0 }
     f7a:	07d816        	beqz	a8, ffb <F_CPD_Intp_Out+0xe3>

00000f7d <F_CPD_Intp_Out+0x65>:
     f7d:	a09390        	addx4	a9, a3, a9
     f80:	f3f790        	wur.ae_cend0	a9
     f83:	480c           	movi.n	a8, 4
     f85:	977a           	add.n	a9, a7, a7
     f87:	9379a0        	movnez	a7, a9, a10
     f8a:	fc24fb8d8f 	{ ae_lq32f.c	aeq0, a13, a8; nop }
     f8f:	06e516        	beqz	a5, 1001 <F_CPD_Intp_Out+0xe9>
     f92:	fec692        	addi	a9, a6, -2
     f95:	0c1916        	beqz	a9, 105a <F_CPD_Intp_Out+0x142>
     f98:	541ea6        	blti	a14, 1, ff0 <F_CPD_Intp_Out+0xd8>
     f9b:	fbea0fc25e 	{ addi	a5, a2, -4; ae_addq56	aeq3, aeq1, aeq2 }
     fa0:	fa7f8ffe3e 	{ addi	a3, a14, -1; ae_roundsp24q48asym	aep4, aeq3 }
     fa5:	f8907002ae 	{ l32i	a10, a2, 0; ae_mulfq32sp24s.l	aeq1, aeq0, aep4 }
     faa:	09efa4        	ae_s32ip	a10, a15, 4
     fad:	345794        	ae_sllaq56	aeq1, aeq1, a7
     fb0:	fd148ffcef 	{ movi	a14, -4; ae_negq56	aeq1, aeq1 }
     fb5:	fd2c40422e 	{ addi	a2, a2, 4; ae_roundsq32asym	aeq1, aeq1 }
     fba:	0020f0        	nop
     fbd:	1ca376        	loopgtz	a3, fdd <F_CPD_Intp_Out+0xc5>
     fc0:	fbfa39d51f 	{ ae_sq32f.iu	aeq1, a5, 4; ae_addq56	aeq3, aeq3, aeq2 }
     fc5:	f87fbbad8f 	{ ae_lq32f.c	aeq1, a13, a8; ae_roundsp24q48asym	aep0, aeq3 }
     fca:	f88170023e 	{ l32i	a3, a2, 0; ae_mulfq32sp24s.l	aeq1, aeq1, aep0 }
     fcf:	09ef34        	ae_s32ip	a3, a15, 4
     fd2:	345794        	ae_sllaq56	aeq1, aeq1, a7
     fd5:	fd1480422e 	{ addi	a2, a2, 4; ae_negq56	aeq1, aeq1 }
     fda:	065934        	ae_roundsq32asym	aeq1, aeq1

00000fdd <F_CPD_Intp_Out+0xc5>:
     fdd:	05bd           	mov.n	a11, a5
     fdf:	625b14        	ae_sq32f.iu	aeq1, a11, 4
     fe2:	fc24fbcd8f 	{ ae_lq32f.c	aeq2, a13, a8; nop }
     fe7:	fc24fbedef 	{ ae_lq32f.c	aeq3, a13, a14; nop }
     fec:	dcd9           	s32i.n	a13, a12, 52
     fee:	f01d           	retw.n

00000ff0 <F_CPD_Intp_Out+0xd8>:
     ff0:	ce7c           	movi.n	a14, -4
     ff2:	fc24fbedef 	{ ae_lq32f.c	aeq3, a13, a14; nop }
     ff7:	dcd9           	s32i.n	a13, a12, 52
     ff9:	f01d           	retw.n

00000ffb <F_CPD_Intp_Out+0xe3>:
     ffb:	1133f0        	slli	a3, a3, 1
     ffe:	ffdec6        	j	f7d <F_CPD_Intp_Out+0x65>

00001001 <F_CPD_Intp_Out+0xe9>:
    1001:	fec692        	addi	a9, a6, -2
    1004:	0e3916        	beqz	a9, 10eb <F_CPD_Intp_Out+0x1d3>
    1007:	e51ea6        	blti	a14, 1, ff0 <F_CPD_Intp_Out+0xd8>
    100a:	fbea3002ae 	{ l32i	a10, a2, 0; ae_addq56	aeq3, aeq1, aeq2 }
    100f:	09efa4        	ae_s32ip	a10, a15, 4
    1012:	fa7fcfc25e 	{ addi	a5, a2, -4; ae_roundsp24q48asym	aep5, aeq3 }
    1017:	f8944ffe3e 	{ addi	a3, a14, -1; ae_mulfq32sp24s.l	aeq1, aeq0, aep5 }
    101c:	ce7c           	movi.n	a14, -4
    101e:	345794        	ae_sllaq56	aeq1, aeq1, a7
    1021:	fd2c40422e 	{ addi	a2, a2, 4; ae_roundsq32asym	aeq1, aeq1 }
    1026:	19a376        	loopgtz	a3, 1043 <F_CPD_Intp_Out+0x12b>
    1029:	fbfa39d51f 	{ ae_sq32f.iu	aeq1, a5, 4; ae_addq56	aeq3, aeq3, aeq2 }
    102e:	f87fbbad8f 	{ ae_lq32f.c	aeq1, a13, a8; ae_roundsp24q48asym	aep0, aeq3 }
    1033:	f88170023e 	{ l32i	a3, a2, 0; ae_mulfq32sp24s.l	aeq1, aeq1, aep0 }
    1038:	09ef34        	ae_s32ip	a3, a15, 4
    103b:	345794        	ae_sllaq56	aeq1, aeq1, a7
    103e:	fd2c40422e 	{ addi	a2, a2, 4; ae_roundsq32asym	aeq1, aeq1 }

00001043 <F_CPD_Intp_Out+0x12b>:
    1043:	05bd           	mov.n	a11, a5
    1045:	625b14        	ae_sq32f.iu	aeq1, a11, 4
    1048:	fc24fbcd8f 	{ ae_lq32f.c	aeq2, a13, a8; nop }
    104d:	fc24fbedef 	{ ae_lq32f.c	aeq3, a13, a14; nop }
    1052:	dcd9           	s32i.n	a13, a12, 52
    1054:	f01d           	retw.n

00001056 <F_CPD_Intp_Out+0x13e>:
    1056:	00000000                                ....

0000105a <F_CPD_Intp_Out+0x142>:
    105a:	921ea6        	blti	a14, 1, ff0 <F_CPD_Intp_Out+0xd8>
    105d:	08ad           	mov.n	a10, a8
    105f:	fbea0fc4be 	{ addi	a11, a4, -4; ae_addq56	aeq3, aeq1, aeq2 }
    1064:	fb7f93977f 	{ or	a9, a7, a7; ae_roundsp24q48asym	aep6, aeq3 }
    1069:	f89870025e 	{ l32i	a5, a2, 0; ae_mulfq32sp24s.l	aeq1, aeq0, aep6 }
    106e:	0f59           	s32i.n	a5, a15, 0
    1070:	345794        	ae_sllaq56	aeq1, aeq1, a7
    1073:	fd148fc25e 	{ addi	a5, a2, -4; ae_negq56	aeq1, aeq1 }
    1078:	fd2c40422e 	{ addi	a2, a2, 4; ae_roundsq32asym	aeq1, aeq1 }
    107d:	625514        	ae_sq32f.iu	aeq1, a5, 4
    1080:	0438           	l32i.n	a3, a4, 0
    1082:	fc24fbad8f 	{ ae_lq32f.c	aeq1, a13, a8; nop }
    1087:	f899541f3f 	{ s32i.n	a3, a15, 4; ae_mulfq32sp24s.l	aeq1, aeq1, aep6 }
    108c:	3e0b           	addi.n	a3, a14, -1
    108e:	345794        	ae_sllaq56	aeq1, aeq1, a7
    1091:	ce7c           	movi.n	a14, -4
    1093:	fd1480447e 	{ addi	a7, a4, 4; ae_negq56	aeq1, aeq1 }
    1098:	fd2c408f4e 	{ addi	a4, a15, 8; ae_roundsq32asym	aeq1, aeq1 }
    109d:	37a376        	loopgtz	a3, 10d8 <F_CPD_Intp_Out+0x1c0>
    10a0:	fbfa39db1f 	{ ae_sq32f.iu	aeq1, a11, 4; ae_addq56	aeq3, aeq3, aeq2 }
    10a5:	f87fbbadaf 	{ ae_lq32f.c	aeq1, a13, a10; ae_roundsp24q48asym	aep0, aeq3 }
    10aa:	f88170023e 	{ l32i	a3, a2, 0; ae_mulfq32sp24s.l	aeq1, aeq1, aep0 }
    10af:	0439           	s32i.n	a3, a4, 0
    10b1:	345994        	ae_sllaq56	aeq1, aeq1, a9
    10b4:	fd1480422e 	{ addi	a2, a2, 4; ae_negq56	aeq1, aeq1 }
    10b9:	065934        	ae_roundsq32asym	aeq1, aeq1
    10bc:	625514        	ae_sq32f.iu	aeq1, a5, 4
    10bf:	fc24fbadaf 	{ ae_lq32f.c	aeq1, a13, a10; nop }
    10c4:	f88170073e 	{ l32i	a3, a7, 0; ae_mulfq32sp24s.l	aeq1, aeq1, aep0 }
    10c9:	1439           	s32i.n	a3, a4, 4
    10cb:	345994        	ae_sllaq56	aeq1, aeq1, a9
    10ce:	fd1480844e 	{ addi	a4, a4, 8; ae_negq56	aeq1, aeq1 }
    10d3:	fd2c40477e 	{ addi	a7, a7, 4; ae_roundsq32asym	aeq1, aeq1 }

000010d8 <F_CPD_Intp_Out+0x1c0>:
    10d8:	0b8d           	mov.n	a8, a11
    10da:	625814        	ae_sq32f.iu	aeq1, a8, 4
    10dd:	fc24fbcdaf 	{ ae_lq32f.c	aeq2, a13, a10; nop }
    10e2:	fc24fbedef 	{ ae_lq32f.c	aeq3, a13, a14; nop }
    10e7:	dcd9           	s32i.n	a13, a12, 52
    10e9:	f01d           	retw.n

000010eb <F_CPD_Intp_Out+0x1d3>:
    10eb:	021ee6        	bgei	a14, 1, 10f1 <F_CPD_Intp_Out+0x1d9>
    10ee:	ffbf86        	j	ff0 <F_CPD_Intp_Out+0xd8>
    10f1:	fcc462        	addi	a6, a4, -4
    10f4:	fcc252        	addi	a5, a2, -4
    10f7:	356e07        	bbci	a14, 0, 1130 <F_CPD_Intp_Out+0x218>
    10fa:	015ac4        	ae_addq56	aeq1, aeq1, aeq2
    10fd:	1b9d74        	ae_roundsp24q48asym	aep7, aeq1
    1100:	f89c3002ae 	{ l32i	a10, a2, 0; ae_mulfq32sp24s.l	aeq0, aeq0, aep7 }
    1105:	224b           	addi.n	a2, a2, 4
    1107:	340794        	ae_sllaq56	aeq0, aeq0, a7
    110a:	ff24540faf 	{ s32i.n	a10, a15, 0; ae_roundsq32asym	aeq3, aeq0 }
    110f:	627514        	ae_sq32f.iu	aeq3, a5, 4
    1112:	0498           	l32i.n	a9, a4, 0
    1114:	fc24fb8d8f 	{ ae_lq32f.c	aeq0, a13, a8; nop }
    1119:	f89c00444e 	{ addi	a4, a4, 4; ae_mulfq32sp24s.l	aeq0, aeq0, aep7 }
    111e:	1f99           	s32i.n	a9, a15, 4
    1120:	340794        	ae_sllaq56	aeq0, aeq0, a7
    1123:	ff24408ffe 	{ addi	a15, a15, 8; ae_roundsq32asym	aeq3, aeq0 }
    1128:	627614        	ae_sq32f.iu	aeq3, a6, 4
    112b:	fc24fb8d8f 	{ ae_lq32f.c	aeq0, a13, a8; nop }

00001130 <F_CPD_Intp_Out+0x218>:
    1130:	2191e0        	srai	a9, a14, 1
    1133:	eb9916        	beqz	a9, ff0 <F_CPD_Intp_Out+0xd8>
    1136:	01dac4        	ae_addq56	aeq3, aeq1, aeq2
    1139:	1bbd14        	ae_roundsp24q48asym	aep1, aeq3
    113c:	f8847002ee 	{ l32i	a14, a2, 0; ae_mulfq32sp24s.l	aeq1, aeq0, aep1 }
    1141:	0fe9           	s32i.n	a14, a15, 0
    1143:	345794        	ae_sllaq56	aeq1, aeq1, a7
    1146:	fc2c4ffcef 	{ movi	a14, -4; ae_roundsq32asym	aeq0, aeq1 }
    114b:	620514        	ae_sq32f.i	aeq0, a5, 4
    114e:	fc24fbad8f 	{ ae_lq32f.c	aeq1, a13, a8; nop }
    1153:	f8857004be 	{ l32i	a11, a4, 0; ae_mulfq32sp24s.l	aeq1, aeq1, aep1 }
    1158:	1fb9           	s32i.n	a11, a15, 4
    115a:	345794        	ae_sllaq56	aeq1, aeq1, a7
    115d:	fc2c410fbe 	{ addi	a11, a15, 16; ae_roundsq32asym	aeq0, aeq1 }
    1162:	620614        	ae_sq32f.i	aeq0, a6, 4
    1165:	f8fa3bad8f 	{ ae_lq32f.c	aeq1, a13, a8; ae_addq56	aeq0, aeq3, aeq2 }
    116a:	f867b012ae 	{ l32i	a10, a2, 4; ae_roundsp24q48asym	aep0, aeq0 }
    116f:	f88140822e 	{ addi	a2, a2, 8; ae_mulfq32sp24s.l	aeq1, aeq1, aep0 }
    1174:	2fa9           	s32i.n	a10, a15, 8
    1176:	345794        	ae_sllaq56	aeq1, aeq1, a7
    1179:	08ad           	mov.n	a10, a8
    117b:	ff2c53866f 	{ or	a8, a6, a6; ae_roundsq32asym	aeq3, aeq1 }
    1180:	627524        	ae_sq32f.iu	aeq3, a5, 8
    1183:	fc24fbadaf 	{ ae_lq32f.c	aeq1, a13, a10; nop }
    1188:	f88170143e 	{ l32i	a3, a4, 4; ae_mulfq32sp24s.l	aeq1, aeq1, aep0 }
    118d:	3f39           	s32i.n	a3, a15, 12
    118f:	345794        	ae_sllaq56	aeq1, aeq1, a7
    1192:	04fd           	mov.n	a15, a4
    1194:	390b           	addi.n	a3, a9, -1
    1196:	0b4d           	mov.n	a4, a11
    1198:	fd2c53977f 	{ or	a9, a7, a7; ae_roundsq32asym	aeq1, aeq1 }
    119d:	60a376        	loopgtz	a3, 1201 <F_CPD_Intp_Out+0x2e9>
    11a0:	f8e239d82f 	{ ae_sq32f.iu	aeq1, a8, 8; ae_addq56	aeq0, aeq0, aeq2 }
    11a5:	f867bbadaf 	{ ae_lq32f.c	aeq1, a13, a10; ae_roundsp24q48asym	aep0, aeq0 }
    11aa:	f88170023e 	{ l32i	a3, a2, 0; ae_mulfq32sp24s.l	aeq1, aeq1, aep0 }
    11af:	f8e214043f 	{ s32i.n	a3, a4, 0; ae_addq56	aeq0, aeq0, aeq2 }
    11b4:	f867fed94f 	{ ae_sllaq56	aeq1, aeq1, a9; ae_roundsp24q48asym	aep1, aeq0 }
    11b9:	fd2c408ffe 	{ addi	a15, a15, 8; ae_roundsq32asym	aeq1, aeq1 }
    11be:	621514        	ae_sq32f.i	aeq1, a5, 4
    11c1:	fc24fbadaf 	{ ae_lq32f.c	aeq1, a13, a10; nop }
    11c6:	f881700f3e 	{ l32i	a3, a15, 0; ae_mulfq32sp24s.l	aeq1, aeq1, aep0 }
    11cb:	1439           	s32i.n	a3, a4, 4
    11cd:	345994        	ae_sllaq56	aeq1, aeq1, a9
    11d0:	065934        	ae_roundsq32asym	aeq1, aeq1
    11d3:	621814        	ae_sq32f.i	aeq1, a8, 4
    11d6:	fc24fbadaf 	{ ae_lq32f.c	aeq1, a13, a10; nop }
    11db:	f88570123e 	{ l32i	a3, a2, 4; ae_mulfq32sp24s.l	aeq1, aeq1, aep1 }
    11e0:	2439           	s32i.n	a3, a4, 8
    11e2:	345994        	ae_sllaq56	aeq1, aeq1, a9
    11e5:	fd2c40822e 	{ addi	a2, a2, 8; ae_roundsq32asym	aeq1, aeq1 }
    11ea:	625524        	ae_sq32f.iu	aeq1, a5, 8
    11ed:	fc24fbadaf 	{ ae_lq32f.c	aeq1, a13, a10; nop }
    11f2:	f885701f3e 	{ l32i	a3, a15, 4; ae_mulfq32sp24s.l	aeq1, aeq1, aep1 }
    11f7:	3439           	s32i.n	a3, a4, 12
    11f9:	345994        	ae_sllaq56	aeq1, aeq1, a9
    11fc:	fd2c41044e 	{ addi	a4, a4, 16; ae_roundsq32asym	aeq1, aeq1 }

00001201 <F_CPD_Intp_Out+0x2e9>:
    1201:	625824        	ae_sq32f.iu	aeq1, a8, 8
    1204:	fc24fbcdaf 	{ ae_lq32f.c	aeq2, a13, a10; nop }
    1209:	fc24fbedef 	{ ae_lq32f.c	aeq3, a13, a14; nop }
    120e:	dcd9           	s32i.n	a13, a12, 52
    1210:	f01d           	retw.n

00001212 <F_CPD_Intp_Out+0x2fa>:
	...

00001214 <multiband_cpd_module>:
    1214:	006136        	entry	a1, 48
    1217:	06ed           	mov.n	a14, a6
    1219:	02ad           	mov.n	a10, a2
    121b:	03bd           	mov.n	a11, a3
    121d:	fbbc81        	l32r	a8, 110 (d48 <F_CPD_Mag_In>)
    1220:	04cd           	mov.n	a12, a4
    1222:	20d550        	or	a13, a5, a5
    1225:	0008e0        	callx8	a8
    1228:	fbbb81        	l32r	a8, 114 (e1c <F_CPD_Max_In_Buf>)
    122b:	368a34        	ae_cvtq48a32s	aeq2, a10
    122e:	0e22b2        	l32i	a11, a2, 56
    1231:	378bb4        	ae_truncp24a32x2	aep0, a11, a11
    1234:	0885a4        	ae_mulfq32sp24s.l	aeq2, aeq2, aep0
    1237:	3f9292        	l16si	a9, a2, 126
    123a:	346934        	ae_sllasq56s	aeq1, aeq2, a9
    123d:	fc2c53a22f 	{ or	a10, a2, a2; ae_roundsq32asym	aeq0, aeq1 }
    1242:	368b04        	ae_trunca32q48	a11, aeq0
    1245:	0008e0        	callx8	a8
    1248:	1322b2        	l32i	a11, a2, 76
    124b:	22c8           	l32i.n	a12, a2, 8
    124d:	0add           	mov.n	a13, a10
    124f:	2322e2        	l32i	a14, a2, 140
    1252:	24acb7        	bge	a12, a11, 127a <multiband_cpd_module+0x66>
    1255:	0392f2        	l16si	a15, a2, 6

00001258 <multiband_cpd_module+0x44>:
    1258:	202282        	l32i	a8, a2, 128
    125b:	908880        	addx2	a8, a8, a8
    125e:	02ae87        	bge	a14, a8, 1264 <multiband_cpd_module+0x50>
    1261:	4492f2        	l16si	a15, a2, 136

00001264 <multiband_cpd_module+0x50>:
    1264:	05ed           	mov.n	a14, a5
    1266:	04cd           	mov.n	a12, a4
    1268:	03bd           	mov.n	a11, a3
    126a:	02ad           	mov.n	a10, a2
    126c:	0179           	s32i.n	a7, a1, 0
    126e:	fbaa81        	l32r	a8, 118 (f18 <F_CPD_Intp_Out>)
    1271:	c198           	l32i.n	a9, a1, 48
    1273:	1199           	s32i.n	a9, a1, 4
    1275:	0008e0        	callx8	a8
    1278:	f01d           	retw.n

0000127a <multiband_cpd_module+0x66>:
    127a:	32a8           	l32i.n	a10, a2, 12
    127c:	22fd           	ae_l16si.n	a15, a2, 0
    127e:	328d           	ae_l16si.n	a8, a2, 2
    1280:	02aab7        	bge	a10, a11, 1286 <multiband_cpd_module+0x72>
    1283:	fff446        	j	1258 <multiband_cpd_module+0x44>

00001286 <multiband_cpd_module+0x72>:
    1286:	4298           	l32i.n	a9, a2, 16
    1288:	0292f2        	l16si	a15, a2, 4
    128b:	7299b0        	salt	a9, a9, a11
    128e:	93f890        	movnez	a15, a8, a9
    1291:	8ffd           	ae_sext16	a15, a15
    1293:	fff046        	j	1258 <multiband_cpd_module+0x44>

00001296 <multiband_cpd_module+0x82>:
	...

00001298 <multiband_cpd_module_2nd_stage>:
    1298:	004136        	entry	a1, 32
    129b:	06ed           	mov.n	a14, a6
    129d:	02ad           	mov.n	a10, a2
    129f:	03bd           	mov.n	a11, a3
    12a1:	fb9b81        	l32r	a8, 110 (d48 <F_CPD_Mag_In>)
    12a4:	04cd           	mov.n	a12, a4
    12a6:	05dd           	mov.n	a13, a5
    12a8:	0008e0        	callx8	a8
    12ab:	fb9a81        	l32r	a8, 114 (e1c <F_CPD_Max_In_Buf>)
    12ae:	3f9292        	l16si	a9, a2, 126
    12b1:	368a34        	ae_cvtq48a32s	aeq2, a10
    12b4:	346934        	ae_sllasq56s	aeq1, aeq2, a9
    12b7:	fc2c53a22f 	{ or	a10, a2, a2; ae_roundsq32asym	aeq0, aeq1 }
    12bc:	368b04        	ae_trunca32q48	a11, aeq0
    12bf:	0008e0        	callx8	a8
    12c2:	2322b2        	l32i	a11, a2, 140
    12c5:	2022c2        	l32i	a12, a2, 128
    12c8:	1022d2        	l32i	a13, a2, 64
    12cb:	360d34        	ae_cvtq48a32s	aeq0, a13
    12ce:	fd0451cccf 	{ addx2	a12, a12, a12; ae_movq56	aeq1, aeq0 }
    12d3:	0eada7        	bge	a13, a10, 12e5 <multiband_cpd_module_2nd_stage+0x4d>
    12d6:	022bc7        	blt	a11, a12, 12dc <multiband_cpd_module_2nd_stage+0x44>
    12d9:	002b86        	j	138b <multiband_cpd_module_2nd_stage+0xf3>
    12dc:	4492e2        	l16si	a14, a2, 136
    12df:	300ee4        	ae_cvtp24a16x2.ll	aep0, a14, a14
    12e2:	0003c6        	j	12f5 <multiband_cpd_module_2nd_stage+0x5d>

000012e5 <multiband_cpd_module_2nd_stage+0x4d>:
    12e5:	09abc7        	bge	a11, a12, 12f2 <multiband_cpd_module_2nd_stage+0x5a>
    12e8:	4492f2        	l16si	a15, a2, 136
    12eb:	300ff4        	ae_cvtp24a16x2.ll	aep0, a15, a15
    12ee:	0000c6        	j	12f5 <multiband_cpd_module_2nd_stage+0x5d>

000012f1 <multiband_cpd_module_2nd_stage+0x59>:
	...

000012f2 <multiband_cpd_module_2nd_stage+0x5a>:
    12f2:	400234        	ae_lp16f.i	aep0, a2, 6

000012f5 <multiband_cpd_module_2nd_stage+0x5d>:
    12f5:	fcc4e2        	addi	a14, a4, -4
    12f8:	22c282        	addi	a8, a2, 34
    12fb:	c0cad0        	sub	a12, a10, a13
    12fe:	368c34        	ae_cvtq48a32s	aeq2, a12
    1301:	f682f0c2be 	{ l32i	a11, a2, 48; ae_mulafq32sp16s.l	aeq1, aeq2, aep0 }
    1306:	f3f6b0        	wur.ae_cbegin0	a11
    1309:	ff2c4fc3be 	{ addi	a11, a3, -4; ae_roundsq32asym	aeq3, aeq1 }
    130e:	36b904        	ae_trunca32q48	a9, aeq3
    1311:	106292        	s32i	a9, a2, 64
    1314:	b298           	l32i.n	a9, a2, 44
    1316:	fb3aec180f 	{ ae_lp16f.i	aep1, a8, 0; ae_subq56	aeq3, aeq3, aeq0 }
    131b:	d76770c28e 	{ l32i	a8, a2, 48; ae_mulfq32sp16s.h	aeq3, aeq3, aep1 }
    1320:	6d2526        	beqi	a5, 2, 1391 <multiband_cpd_module_2nd_stage+0xf9>
    1323:	09ad           	mov.n	a10, a9

00001325 <multiband_cpd_module_2nd_stage+0x8d>:
    1325:	d298           	l32i.n	a9, a2, 52
    1327:	a08a80        	addx4	a8, a10, a8
    132a:	f3f780        	wur.ae_cend0	a8
    132d:	4d0c           	movi.n	a13, 4
    132f:	1092f2        	l16si	a15, a2, 32
    1332:	09ad           	mov.n	a10, a9
    1334:	fc24fba9df 	{ ae_lq32f.c	aeq1, a9, a13; nop }
    1339:	25af76        	loopgtz	a15, 1362 <multiband_cpd_module_2nd_stage+0xca>
    133c:	3f92c2        	l16si	a12, a2, 126
    133f:	f8e63003fe 	{ l32i	a15, a3, 0; ae_addq56	aeq0, aeq0, aeq3 }
    1344:	09eaf4        	ae_s32ip	a15, a10, 4
    1347:	1b8d04        	ae_roundsp24q48asym	aep0, aeq0
    134a:	088564        	ae_mulfq32sp24s.l	aeq2, aeq1, aep0
    134d:	cc1b           	addi.n	a12, a12, 1
    134f:	34ac34        	ae_sllasq56s	aeq2, aeq2, a12
    1352:	fe3440433e 	{ addi	a3, a3, 4; ae_roundsq32asym	aeq2, aeq2 }
    1357:	626b14        	ae_sq32f.iu	aeq2, a11, 4
    135a:	122526        	beqi	a5, 2, 1370 <multiband_cpd_module_2nd_stage+0xd8>

0000135d <multiband_cpd_module_2nd_stage+0xc5>:
    135d:	fc24fba9df 	{ ae_lq32f.c	aeq1, a9, a13; nop }

00001362 <multiband_cpd_module_2nd_stage+0xca>:
    1362:	c87c           	movi.n	a8, -4
    1364:	fc24fbe98f 	{ ae_lq32f.c	aeq3, a9, a8; nop }
    1369:	d299           	s32i.n	a9, a2, 52
    136b:	f01d           	retw.n

0000136d <multiband_cpd_module_2nd_stage+0xd5>:
    136d:	000000                                        ...

00001370 <multiband_cpd_module_2nd_stage+0xd8>:
    1370:	fc24fba9df 	{ ae_lq32f.c	aeq1, a9, a13; nop }
    1375:	f8817004fe 	{ l32i	a15, a4, 0; ae_mulfq32sp24s.l	aeq1, aeq1, aep0 }
    137a:	09eaf4        	ae_s32ip	a15, a10, 4
    137d:	349c34        	ae_sllasq56s	aeq2, aeq1, a12
    1380:	fd3440444e 	{ addi	a4, a4, 4; ae_roundsq32asym	aeq1, aeq2 }
    1385:	625e14        	ae_sq32f.iu	aeq1, a14, 4
    1388:	fff446        	j	135d <multiband_cpd_module_2nd_stage+0xc5>

0000138b <multiband_cpd_module_2nd_stage+0xf3>:
    138b:	400204        	ae_lp16f.i	aep0, a2, 0
    138e:	ffd8c6        	j	12f5 <multiband_cpd_module_2nd_stage+0x5d>

00001391 <multiband_cpd_module_2nd_stage+0xf9>:
    1391:	11a9f0        	slli	a10, a9, 1
    1394:	ffe346        	j	1325 <multiband_cpd_module_2nd_stage+0x8d>

00001397 <multiband_cpd_module_2nd_stage+0xff>:
	...

00001398 <DBB_module>:
    1398:	006136        	entry	a1, 48
    139b:	379344        	ae_truncp24a32x2	aep1, a3, a4
    139e:	019174        	ae_mulfp24s.ll	aeq2, aep1, aep1
    13a1:	109134        	ae_mulafs56p24s.hh	aeq2, aep1, aep1
    13a4:	01ad           	mov.n	a10, a1
    13a6:	366c04        	ae_nsaq56s	a12, aeq2
    13a9:	b14b           	addi.n	a11, a1, 4
    13ab:	fb5c81        	l32r	a8, 11c (258 <HIFI_sqrt_cpd>)
    13ae:	fd344f8cce 	{ addi	a12, a12, -8; ae_roundsq32asym	aeq1, aeq2 }
    13b3:	341c34        	ae_sllasq56s	aeq0, aeq1, a12
    13b6:	60c0c0        	neg	a12, a12
    13b9:	f8639401cf 	{ s32i.n	a12, a1, 0; ae_roundsp16q48asym	aep0, aeq0 }
    13be:	0009d4        	ae_cvta32p24.l	a9, aep0
    13c1:	1199           	s32i.n	a9, a1, 4
    13c3:	0008e0        	callx8	a8
    13c6:	11a8           	l32i.n	a10, a1, 4
    13c8:	0198           	l32i.n	a9, a1, 0
    13ca:	22d8           	l32i.n	a13, a2, 8
    13cc:	fb55c1        	l32r	a12, 120 (3d70a3d <_end+0x3d6e439>)
    13cf:	0d2996        	bltz	a9, 14a5 <DBB_module+0x10d>
    13d2:	401900        	ssl	a9
    13d5:	a1ba00        	sll	a11, a10

000013d8 <DBB_module+0x40>:
    13d8:	32a8           	l32i.n	a10, a2, 12
    13da:	1badb7        	bge	a13, a11, 13f9 <DBB_module+0x61>
    13dd:	378cc4        	ae_truncp24a32x2	aep0, a12, a12
    13e0:	360a34        	ae_cvtq48a32s	aeq0, a10
    13e3:	c0eba0        	sub	a14, a11, a10
    13e6:	364e34        	ae_cvtq48a32s	aeq1, a14
    13e9:	080354        	ae_mularfq32sp24s.l	aeq0, aeq1, aep0
    13ec:	ff245411ef 	{ s32i.n	a14, a1, 4; ae_roundsq32asym	aeq3, aeq0 }
    13f1:	36ba04        	ae_trunca32q48	a10, aeq3
    13f4:	32a9           	s32i.n	a10, a2, 12
    13f6:	000086        	j	13fc <DBB_module+0x64>

000013f9 <DBB_module+0x61>:
    13f9:	378cc4        	ae_truncp24a32x2	aep0, a12, a12

000013fc <DBB_module+0x64>:
    13fc:	02b8           	l32i.n	a11, a2, 0
    13fe:	4298           	l32i.n	a9, a2, 16
    1400:	1288           	l32i.n	a8, a2, 4
    1402:	1daab7        	bge	a10, a11, 1423 <DBB_module+0x8b>
    1405:	21b8b0        	srai	a11, a11, 8
    1408:	352ba7        	blt	a11, a10, 1441 <DBB_module+0xa9>
    140b:	36c934        	ae_cvtq48a32s	aeq3, a9
    140e:	c08890        	sub	a8, a8, a9
    1411:	f9e2bec83f 	{ ae_cvtq48a32s	aeq0, a8; ae_roundsp16asym	aep2, aep0 }
    1416:	08ab14        	ae_mularfq32sp24s.l	aeq3, aeq0, aep2
    1419:	06b934        	ae_roundsq32asym	aeq2, aeq3
    141c:	36af04        	ae_trunca32q48	a15, aeq2
    141f:	42f9           	s32i.n	a15, a2, 16
    1421:	f01d           	retw.n

00001423 <DBB_module+0x8b>:
    1423:	fb2eb1        	l32r	a11, dc (8000000 <_end+0x7ffd9fc>)
    1426:	42c8           	l32i.n	a12, a2, 16
    1428:	368c34        	ae_cvtq48a32s	aeq2, a12
    142b:	c0bbc0        	sub	a11, a11, a12
    142e:	f9e2fecbff 	{ ae_cvtq48a32s	aeq3, a11; ae_roundsp16asym	aep3, aep0 }
    1433:	08b3d4        	ae_mularfq32sp24s.l	aeq2, aeq3, aep3
    1436:	066934        	ae_roundsq32asym	aeq1, aeq2
    1439:	369a04        	ae_trunca32q48	a10, aeq1
    143c:	42a9           	s32i.n	a10, a2, 16
    143e:	f01d           	retw.n

00001440 <DBB_module+0xa8>:
	...

00001441 <DBB_module+0xa9>:
    1441:	560114        	ae_sp24x2s.i	aep0, a1, 8
    1444:	11b9           	s32i.n	a11, a1, 4
    1446:	fb1481        	l32r	a8, 98 (2c0 <Xdiv>)
    1449:	a14b           	addi.n	a10, a1, 4
    144b:	b2cb           	addi.n	a11, a2, 12
    144d:	0008e0        	callx8	a8
    1450:	b14b           	addi.n	a11, a1, 4
    1452:	11da00        	slli	a13, a10, 16
    1455:	364d34        	ae_cvtq48a32s	aeq1, a13
    1458:	365c04        	ae_nsaq56s	a12, aeq1
    145b:	fb3081        	l32r	a8, 11c (258 <HIFI_sqrt_cpd>)
    145e:	01ad           	mov.n	a10, a1
    1460:	f8ccc2        	addi	a12, a12, -8
    1463:	341c34        	ae_sllasq56s	aeq0, aeq1, a12
    1466:	60c0c0        	neg	a12, a12
    1469:	fa639401cf 	{ s32i.n	a12, a1, 0; ae_roundsp16q48asym	aep4, aeq0 }
    146e:	0049d4        	ae_cvta32p24.l	a9, aep4
    1471:	1199           	s32i.n	a9, a1, 4
    1473:	0008e0        	callx8	a8
    1476:	0198           	l32i.n	a9, a1, 0
    1478:	540114        	ae_lp24x2.i	aep0, a1, 8
    147b:	11a8           	l32i.n	a10, a1, 4
    147d:	030996        	bltz	a9, 14b1 <DBB_module+0x119>
    1480:	12f8           	l32i.n	a15, a2, 4
    1482:	401900        	ssl	a9
    1485:	4288           	l32i.n	a8, a2, 16
    1487:	36c834        	ae_cvtq48a32s	aeq3, a8
    148a:	a1ba00        	sll	a11, a10
    148d:	43ffb0        	min	a15, a15, a11
    1490:	c0ff80        	sub	a15, a15, a8
    1493:	fae2fecf3f 	{ ae_cvtq48a32s	aeq0, a15; ae_roundsp16asym	aep5, aep0 }
    1498:	08db14        	ae_mularfq32sp24s.l	aeq3, aeq0, aep5
    149b:	06b934        	ae_roundsq32asym	aeq2, aeq3
    149e:	36ae04        	ae_trunca32q48	a14, aeq2
    14a1:	42e9           	s32i.n	a14, a2, 16
    14a3:	f01d           	retw.n

000014a5 <DBB_module+0x10d>:
    14a5:	608090        	neg	a8, a9
    14a8:	400800        	ssr	a8
    14ab:	b1b0a0        	sra	a11, a10
    14ae:	ffc986        	j	13d8 <DBB_module+0x40>

000014b1 <DBB_module+0x119>:
    14b1:	12f8           	l32i.n	a15, a2, 4
    14b3:	60b090        	neg	a11, a9
    14b6:	4288           	l32i.n	a8, a2, 16
    14b8:	36c834        	ae_cvtq48a32s	aeq3, a8
    14bb:	400b00        	ssr	a11
    14be:	b1b0a0        	sra	a11, a10
    14c1:	43ffb0        	min	a15, a15, a11
    14c4:	c0ff80        	sub	a15, a15, a8
    14c7:	fae2fecf3f 	{ ae_cvtq48a32s	aeq0, a15; ae_roundsp16asym	aep5, aep0 }
    14cc:	08db14        	ae_mularfq32sp24s.l	aeq3, aeq0, aep5
    14cf:	06b934        	ae_roundsq32asym	aeq2, aeq3
    14d2:	36ae04        	ae_trunca32q48	a14, aeq2
    14d5:	42e9           	s32i.n	a14, a2, 16
    14d7:	f01d           	retw.n

000014d9 <DBB_module+0x141>:
    14d9:	000000                                        ...

000014dc <compander_AU_SetINS_proc>:
    14dc:	004136        	entry	a1, 32
    14df:	13d222        	addmi	a2, a2, 0x1300
    14e2:	711252        	l16ui	a5, a2, 226
    14e5:	838c           	beqz.n	a3, 14f1 <compander_AU_SetINS_proc+0x15>
    14e7:	240c           	movi.n	a4, 2
    14e9:	204540        	or	a4, a5, a4
    14ec:	715242        	s16i	a4, a2, 226
    14ef:	f01d           	retw.n

000014f1 <compander_AU_SetINS_proc+0x15>:
    14f1:	fb0c81        	l32r	a8, 124 (fffd <_end+0xd9f9>)
    14f4:	108580        	and	a8, a5, a8
    14f7:	715282        	s16i	a8, a2, 226
    14fa:	f01d           	retw.n

000014fc <compander_AU_proc>:
    14fc:	016136        	entry	a1, 176
    14ff:	206122        	s32i	a2, a1, 128
    1502:	05cd           	mov.n	a12, a5
    1504:	06bd           	mov.n	a11, a6
    1506:	13d2d2        	addmi	a13, a2, 0x1300
    1509:	711d92        	l16ui	a9, a13, 226
    150c:	668b           	addi.n	a6, a6, 8
    150e:	06d252        	addmi	a5, a2, 0x600
    1511:	8f9582        	l16si	a8, a5, 0x11e
    1514:	216460        	srai	a6, a6, 4
    1517:	20c552        	addi	a5, a5, 32
    151a:	04a290        	extui	a10, a9, 2, 1
    151d:	04e190        	extui	a14, a9, 1, 1
    1520:	1c61e2        	s32i	a14, a1, 112
    1523:	f8c882        	addi	a8, a8, -8
    1526:	83b680        	moveqz	a11, a6, a8
    1529:	701d82        	l16ui	a8, a13, 224
    152c:	1d61a2        	s32i	a10, a1, 116
    152f:	e4cd62        	addi	a6, a13, -28
    1532:	3de816        	beqz	a8, 1914 <compander_AU_proc+0x418>
    1535:	741da2        	l16ui	a10, a13, 232
    1538:	109222        	l16si	a2, a2, 32
    153b:	392dd2        	l32i	a13, a13, 228
    153e:	f6caa2        	addi	a10, a10, -10
    1541:	40ba16        	beqz	a10, 1950 <compander_AU_proc+0x454>
    1544:	052da6        	blti	a13, 2, 154d <compander_AU_proc+0x51>
    1547:	21c6c0        	srai	a12, a12, 6
    154a:	000086        	j	1550 <compander_AU_proc+0x54>

0000154d <compander_AU_proc+0x51>:
    154d:	21c5c0        	srai	a12, a12, 5

00001550 <compander_AU_proc+0x54>:
    1550:	1b61c2        	s32i	a12, a1, 108

00001553 <compander_AU_proc+0x57>:
    1553:	02e907        	bbsi	a9, 0, 1559 <compander_AU_proc+0x5d>
    1556:	002786        	j	15f8 <compander_AU_proc+0xfc>
    1559:	1b2192        	l32i	a9, a1, 108
    155c:	0219e6        	bgei	a9, 1, 1562 <compander_AU_proc+0x66>
    155f:	004806        	j	1683 <compander_AU_proc+0x187>
    1562:	2021f2        	l32i	a15, a1, 128
    1565:	78c192        	addi	a9, a1, 120
    1568:	360b34        	ae_cvtq48a32s	aeq0, a11
    156b:	630924        	ae_sq56s.i	aeq0, a9, 16
    156e:	14dfd2        	addmi	a13, a15, 0x1400
    1571:	15dfa2        	addmi	a10, a15, 0x1500
    1574:	1a61a2        	s32i	a10, a1, 104
    1577:	07df92        	addmi	a9, a15, 0x700
    157a:	eccd82        	addi	a8, a13, -20
    157d:	eccac2        	addi	a12, a10, -20
    1580:	eacde2        	addi	a14, a13, -22
    1583:	1761e2        	s32i	a14, a1, 92
    1586:	91c9           	s32i.n	a12, a1, 36
    1588:	1e6182        	s32i	a8, a1, 120
    158b:	a0c992        	addi	a9, a9, -96
    158e:	186192        	s32i	a9, a1, 96
    1591:	0ddf82        	addmi	a8, a15, 0xd00
    1594:	f0cdd2        	addi	a13, a13, -16
    1597:	0c0c           	movi.n	a12, 0
    1599:	11e2e0        	slli	a14, a2, 2
    159c:	f0caa2        	addi	a10, a10, -16
    159f:	1561a2        	s32i	a10, a1, 84
    15a2:	1961e2        	s32i	a14, a1, 100
    15a5:	2161c2        	s32i	a12, a1, 132
    15a8:	1f61d2        	s32i	a13, a1, 124
    15ab:	40c882        	addi	a8, a8, 64
    15ae:	116182        	s32i	a8, a1, 68
    15b1:	11d2f0        	slli	a13, a2, 1
    15b4:	16dfc2        	addmi	a12, a15, 0x1600
    15b7:	f1c9           	s32i.n	a12, a1, 60
    15b9:	1261d2        	s32i	a13, a1, 72
    15bc:	f0cce2        	addi	a14, a12, -16
    15bf:	17dff2        	addmi	a15, a15, 0x1700
    15c2:	50cc92        	addi	a9, a12, 80
    15c5:	146192        	s32i	a9, a1, 80
    15c8:	a1f9           	s32i.n	a15, a1, 40
    15ca:	d1e9           	s32i.n	a14, a1, 52
    15cc:	f0cf82        	addi	a8, a15, -16
    15cf:	c189           	s32i.n	a8, a1, 48
    15d1:	10ccd2        	addi	a13, a12, 16
    15d4:	20cce2        	addi	a14, a12, 32
    15d7:	61e9           	s32i.n	a14, a1, 24
    15d9:	e1d9           	s32i.n	a13, a1, 56
    15db:	a0cff2        	addi	a15, a15, -96
    15de:	30cc82        	addi	a8, a12, 48
    15e1:	106182        	s32i	a8, a1, 64
    15e4:	1361f2        	s32i	a15, a1, 76
    15e7:	dc8b           	addi.n	a13, a12, 8
    15e9:	71d9           	s32i.n	a13, a1, 28
    15eb:	f8ccf2        	addi	a15, a12, -8
    15ee:	51f9           	s32i.n	a15, a1, 20
    15f0:	40ccc2        	addi	a12, a12, 64
    15f3:	81c9           	s32i.n	a12, a1, 32
    15f5:	0031c6        	j	16c0 <compander_AU_proc+0x1c4>

000015f8 <compander_AU_proc+0xfc>:
    15f8:	1b21a2        	l32i	a10, a1, 108
    15fb:	021ae6        	bgei	a10, 1, 1601 <compander_AU_proc+0x105>
    15fe:	002046        	j	1683 <compander_AU_proc+0x187>
    1601:	2021e2        	l32i	a14, a1, 128
    1604:	78c182        	addi	a8, a1, 120
    1607:	364b34        	ae_cvtq48a32s	aeq1, a11
    160a:	0d0c           	movi.n	a13, 0
    160c:	11f2e0        	slli	a15, a2, 2
    160f:	1961f2        	s32i	a15, a1, 100
    1612:	2161d2        	s32i	a13, a1, 132
    1615:	631824        	ae_sq56s.i	aeq1, a8, 16
    1618:	14dec2        	addmi	a12, a14, 0x1400
    161b:	b1c9           	s32i.n	a12, a1, 44
    161d:	07dee2        	addmi	a14, a14, 0x700
    1620:	eaccc2        	addi	a12, a12, -22
    1623:	a0cee2        	addi	a14, a14, -96
    1626:	1861e2        	s32i	a14, a1, 96
    1629:	1761c2        	s32i	a12, a1, 92

0000162c <compander_AU_proc+0x130>:
    162c:	1821a2        	l32i	a10, a1, 96
    162f:	03bd           	mov.n	a11, a3
    1631:	04cd           	mov.n	a12, a4
    1633:	07dd           	mov.n	a13, a7
    1635:	172192        	l32i	a9, a1, 92
    1638:	78c182        	addi	a8, a1, 120
    163b:	0b21f2        	l32i	a15, a1, 44
    163e:	614824        	ae_lq56.i	aeq1, a8, 16
    1641:	fab981        	l32r	a8, 128 (1214 <multiband_cpd_module>)
    1644:	60cfb4        	ae_lq32f.i	aeq3, a15, -20
    1647:	f8ceac090f 	{ ae_lp16f.i	aep0, a9, 0; ae_subsq56s	aeq0, aeq1, aeq3 }
    164c:	f582f1c1ee 	{ l32i	a14, a1, 112; ae_mulafq32sp16s.l	aeq3, aeq0, aep0 }
    1651:	1d21f2        	l32i	a15, a1, 116
    1654:	fe3c40019f 	{ movi	a9, 1; ae_roundsq32asym	aeq2, aeq3 }
    1659:	36a204        	ae_trunca32q48	a2, aeq2
    165c:	426622        	s32i	a2, a6, 0x108
    165f:	2e6522        	s32i	a2, a5, 184
    1662:	006192        	s32i	a9, a1, 0
    1665:	0008e0        	callx8	a8
    1668:	1b2182        	l32i	a8, a1, 108
    166b:	212192        	l32i	a9, a1, 132
    166e:	1921b2        	l32i	a11, a1, 100
    1671:	2e25a2        	l32i	a10, a5, 184
    1674:	2f65a2        	s32i	a10, a5, 188
    1677:	33ba           	add.n	a3, a3, a11
    1679:	44ba           	add.n	a4, a4, a11
    167b:	991b           	addi.n	a9, a9, 1
    167d:	216192        	s32i	a9, a1, 132
    1680:	a89897        	bne	a8, a9, 162c <compander_AU_proc+0x130>

00001683 <compander_AU_proc+0x187>:
    1683:	f01d           	retw.n

00001685 <compander_AU_proc+0x189>:
    1685:	10c1a2        	addi	a10, a1, 16
    1688:	a1b8           	l32i.n	a11, a1, 40
    168a:	fa8381        	l32r	a8, 98 (2c0 <Xdiv>)
    168d:	2194c0        	srai	a9, a12, 4
    1690:	4199           	s32i.n	a9, a1, 16
    1692:	0008e0        	callx8	a8
    1695:	11ca00        	slli	a12, a10, 16

00001698 <compander_AU_proc+0x19c>:
    1698:	faa581        	l32r	a8, 12c (1398 <DBB_module>)
    169b:	c1a8           	l32i.n	a10, a1, 48
    169d:	3325b2        	l32i	a11, a5, 204
    16a0:	0008e0        	callx8	a8
    16a3:	1b2192        	l32i	a9, a1, 108
    16a6:	1921b2        	l32i	a11, a1, 100
    16a9:	2121a2        	l32i	a10, a1, 132
    16ac:	fec7d2        	addi	a13, a7, -2
    16af:	33ba           	add.n	a3, a3, a11
    16b1:	01caa2        	addi	a10, a10, 1
    16b4:	80cb40        	add	a12, a11, a4
    16b7:	834cd0        	moveqz	a4, a12, a13
    16ba:	2161a2        	s32i	a10, a1, 132
    16bd:	c219a7        	beq	a9, a10, 1683 <compander_AU_proc+0x187>

000016c0 <compander_AU_proc+0x1c4>:
    16c0:	1f21a2        	l32i	a10, a1, 124
    16c3:	fa7681        	l32r	a8, 9c (1e68 <xt_memcpy>)
    16c6:	03bd           	mov.n	a11, a3
    16c8:	1221c2        	l32i	a12, a1, 72
    16cb:	0008e0        	callx8	a8
    16ce:	0e2766        	bnei	a7, 2, 16e0 <compander_AU_proc+0x1e4>
    16d1:	1521a2        	l32i	a10, a1, 84
    16d4:	fa7281        	l32r	a8, 9c (1e68 <xt_memcpy>)
    16d7:	20b440        	or	a11, a4, a4
    16da:	1221c2        	l32i	a12, a1, 72
    16dd:	0008e0        	callx8	a8

000016e0 <compander_AU_proc+0x1e4>:
    16e0:	402692        	l32i	a9, a6, 0x100
    16e3:	16a916        	beqz	a9, 1851 <compander_AU_proc+0x355>
    16e6:	03ad           	mov.n	a10, a3
    16e8:	1321b2        	l32i	a11, a1, 76
    16eb:	d1c8           	l32i.n	a12, a1, 52
    16ed:	fa9081        	l32r	a8, 130 (1c9c <HIFI_d1IB_v2_in32_c24_4th>)
    16f0:	e1d8           	l32i.n	a13, a1, 56
    16f2:	20e220        	or	a14, a2, a2
    16f5:	0008e0        	callx8	a8
    16f8:	1f21a2        	l32i	a10, a1, 124
    16fb:	1421b2        	l32i	a11, a1, 80
    16fe:	f1c8           	l32i.n	a12, a1, 60
    1700:	fa8d81        	l32r	a8, 134 (1ddc <HIFI_d1IB_v2_in32_c24_3rd>)
    1703:	1021d2        	l32i	a13, a1, 64
    1706:	20e220        	or	a14, a2, a2
    1709:	0008e0        	callx8	a8
    170c:	222766        	bnei	a7, 2, 1732 <compander_AU_proc+0x236>
    170f:	04ad           	mov.n	a10, a4
    1711:	1321b2        	l32i	a11, a1, 76
    1714:	51c8           	l32i.n	a12, a1, 20
    1716:	fa8681        	l32r	a8, 130 (1c9c <HIFI_d1IB_v2_in32_c24_4th>)
    1719:	61d8           	l32i.n	a13, a1, 24
    171b:	02ed           	mov.n	a14, a2
    171d:	0008e0        	callx8	a8
    1720:	1521a2        	l32i	a10, a1, 84
    1723:	1421b2        	l32i	a11, a1, 80
    1726:	71c8           	l32i.n	a12, a1, 28
    1728:	fa8381        	l32r	a8, 134 (1ddc <HIFI_d1IB_v2_in32_c24_3rd>)
    172b:	81d8           	l32i.n	a13, a1, 32
    172d:	02ed           	mov.n	a14, a2
    172f:	0008e0        	callx8	a8

00001732 <compander_AU_proc+0x236>:
    1732:	1e2182        	l32i	a8, a1, 120
    1735:	fa80a1        	l32r	a10, 138 (4000 <_end+0x19fc>)
    1738:	fcc392        	addi	a9, a3, -4
    173b:	166192        	s32i	a9, a1, 88
    173e:	300aa4        	ae_cvtp24a16x2.ll	aep0, a10, a10
    1741:	11a276        	loopgtz	a2, 1756 <compander_AU_proc+0x25a>
    1744:	601814        	ae_lq32f.iu	aeq0, a8, 4
    1747:	605914        	ae_lq32f.iu	aeq1, a9, 4
    174a:	b58044        	ae_mulzasfq32sp16s.hh	aeq2, aeq1, aep0, aeq0, aep0
    174d:	b10044        	ae_mulzaafq32sp16s.hh	aeq0, aeq1, aep0, aeq0, aep0
    1750:	622904        	ae_sq32f.i	aeq2, a9, 0
    1753:	620804        	ae_sq32f.i	aeq0, a8, 0

00001756 <compander_AU_proc+0x25a>:
    1756:	fec7b2        	addi	a11, a7, -2
    1759:	146b16        	beqz	a11, 18a3 <compander_AU_proc+0x3a7>

0000175c <compander_AU_proc+0x260>:
    175c:	1c21e2        	l32i	a14, a1, 112
    175f:	1d21f2        	l32i	a15, a1, 116
    1762:	fa7181        	l32r	a8, 128 (1214 <multiband_cpd_module>)
    1765:	090c           	movi.n	a9, 0
    1767:	1e21c2        	l32i	a12, a1, 120
    176a:	78c1d2        	addi	a13, a1, 120
    176d:	1721b2        	l32i	a11, a1, 92
    1770:	614d24        	ae_lq56.i	aeq1, a13, 16
    1773:	60cc04        	ae_lq32f.i	aeq3, a12, 0
    1776:	f8ceac1b0f 	{ ae_lp16f.i	aep1, a11, 0; ae_subsq56s	aeq0, aeq1, aeq3 }
    177b:	f586d3d77f 	{ or	a13, a7, a7; ae_mulafq32sp16s.l	aeq3, aeq0, aep1 }
    1780:	03bd           	mov.n	a11, a3
    1782:	fe3c53c44f 	{ or	a12, a4, a4; ae_roundsq32asym	aeq2, aeq3 }
    1787:	36aa04        	ae_trunca32q48	a10, aeq2
    178a:	4266a2        	s32i	a10, a6, 0x108
    178d:	2e65a2        	s32i	a10, a5, 184
    1790:	0199           	s32i.n	a9, a1, 0
    1792:	1821a2        	l32i	a10, a1, 96
    1795:	0008e0        	callx8	a8
    1798:	1d21f2        	l32i	a15, a1, 116
    179b:	fa6381        	l32r	a8, 128 (1214 <multiband_cpd_module>)
    179e:	090c           	movi.n	a9, 0
    17a0:	2021a2        	l32i	a10, a1, 128
    17a3:	1a21d2        	l32i	a13, a1, 104
    17a6:	2e25c2        	l32i	a12, a5, 184
    17a9:	4226e2        	l32i	a14, a6, 0x108
    17ac:	37aee4        	ae_truncp24a32x2	aep2, a14, a14
    17af:	2f65c2        	s32i	a12, a5, 188
    17b2:	802dd2        	l32i	a13, a13, 0x200
    17b5:	360d34        	ae_cvtq48a32s	aeq0, a13
    17b8:	f8883151ce 	{ l32i	a12, a1, 84; ae_mulfq32sp24s.l	aeq0, aeq0, aep2 }
    17bd:	1c21e2        	l32i	a14, a1, 112
    17c0:	36c114        	ae_sllisq56s	aeq3, aeq0, 4
    17c3:	fe3c53d77f 	{ or	a13, a7, a7; ae_roundsq32asym	aeq2, aeq3 }
    17c8:	36ab04        	ae_trunca32q48	a11, aeq2
    17cb:	eab9           	s32i.n	a11, a10, 56
    17cd:	1f21b2        	l32i	a11, a1, 124
    17d0:	0199           	s32i.n	a9, a1, 0
    17d2:	0008e0        	callx8	a8
    17d5:	202182        	l32i	a8, a1, 128
    17d8:	162192        	l32i	a9, a1, 88
    17db:	fec7b2        	addi	a11, a7, -2
    17de:	e8f8           	l32i.n	a15, a8, 56
    17e0:	f8f9           	s32i.n	a15, a8, 60
    17e2:	1e21f2        	l32i	a15, a1, 120
    17e5:	3c12a6        	blti	a2, 1, 1825 <compander_AU_proc+0x329>
    17e8:	fa54a1        	l32r	a10, 138 (4000 <_end+0x19fc>)
    17eb:	300aa4        	ae_cvtp24a16x2.ll	aep0, a10, a10
    17ee:	21a120        	srai	a10, a2, 1
    17f1:	106207        	bbci	a2, 0, 1805 <compander_AU_proc+0x309>
    17f4:	60df14        	ae_lq32f.iu	aeq3, a15, 4
    17f7:	609914        	ae_lq32f.iu	aeq2, a9, 4
    17fa:	b180b4        	ae_mulzaafq32sp16s.hh	aeq2, aeq2, aep0, aeq3, aep0
    17fd:	f03d           	nop.n
    17ff:	066934        	ae_roundsq32asym	aeq1, aeq2
    1802:	621904        	ae_sq32f.i	aeq1, a9, 0
    1805:	1caa76        	loopgtz	a10, 1825 <compander_AU_proc+0x329>
    1808:	600f14        	ae_lq32f.i	aeq0, a15, 4
    180b:	604914        	ae_lq32f.i	aeq1, a9, 4
    180e:	08017d192f 	{ ae_lq32f.iu	aeq0, a9, 8; ae_mulzaafq32sp16s.hh	aeq1, aeq1, aep0, aeq0, aep0 }
    1813:	065934        	ae_roundsq32asym	aeq1, aeq1
    1816:	6219f4        	ae_sq32f.i	aeq1, a9, -4
    1819:	605f24        	ae_lq32f.iu	aeq1, a15, 8
    181c:	b10014        	ae_mulzaafq32sp16s.hh	aeq0, aeq0, aep0, aeq1, aep0
    181f:	060934        	ae_roundsq32asym	aeq0, aeq0
    1822:	620904        	ae_sq32f.i	aeq0, a9, 0

00001825 <compander_AU_proc+0x329>:
    1825:	0a2b16        	beqz	a11, 18cb <compander_AU_proc+0x3cf>

00001828 <compander_AU_proc+0x32c>:
    1828:	1121a2        	l32i	a10, a1, 68
    182b:	03bd           	mov.n	a11, a3
    182d:	04cd           	mov.n	a12, a4
    182f:	fa4381        	l32r	a8, 13c (1298 <multiband_cpd_module_2nd_stage>)
    1832:	07dd           	mov.n	a13, a7
    1834:	0e0c           	movi.n	a14, 0
    1836:	0008e0        	callx8	a8
    1839:	1a21a2        	l32i	a10, a1, 104
    183c:	fa2891        	l32r	a9, dc (8000000 <_end+0x7ffd9fc>)
    183f:	2021c2        	l32i	a12, a1, 128
    1842:	802aa2        	l32i	a10, a10, 0x200
    1845:	132cc2        	l32i	a12, a12, 76
    1848:	c0aa90        	sub	a10, a10, a9
    184b:	e36a56        	bnez	a10, 1685 <compander_AU_proc+0x189>
    184e:	ff9186        	j	1698 <compander_AU_proc+0x19c>

00001851 <compander_AU_proc+0x355>:
    1851:	03ad           	mov.n	a10, a3
    1853:	1321b2        	l32i	a11, a1, 76
    1856:	d1c8           	l32i.n	a12, a1, 52
    1858:	fa3681        	l32r	a8, 130 (1c9c <HIFI_d1IB_v2_in32_c24_4th>)
    185b:	e1d8           	l32i.n	a13, a1, 56
    185d:	02ed           	mov.n	a14, a2
    185f:	0008e0        	callx8	a8
    1862:	1f21a2        	l32i	a10, a1, 124
    1865:	1421b2        	l32i	a11, a1, 80
    1868:	f1c8           	l32i.n	a12, a1, 60
    186a:	fa3281        	l32r	a8, 134 (1ddc <HIFI_d1IB_v2_in32_c24_3rd>)
    186d:	1021d2        	l32i	a13, a1, 64
    1870:	02ed           	mov.n	a14, a2
    1872:	0008e0        	callx8	a8
    1875:	fec792        	addi	a9, a7, -2
    1878:	eb6956        	bnez	a9, 1732 <compander_AU_proc+0x236>
    187b:	04ad           	mov.n	a10, a4
    187d:	1321b2        	l32i	a11, a1, 76
    1880:	51c8           	l32i.n	a12, a1, 20
    1882:	fa2b81        	l32r	a8, 130 (1c9c <HIFI_d1IB_v2_in32_c24_4th>)
    1885:	61d8           	l32i.n	a13, a1, 24
    1887:	02ed           	mov.n	a14, a2
    1889:	0008e0        	callx8	a8
    188c:	1521a2        	l32i	a10, a1, 84
    188f:	1421b2        	l32i	a11, a1, 80
    1892:	71c8           	l32i.n	a12, a1, 28
    1894:	fa2881        	l32r	a8, 134 (1ddc <HIFI_d1IB_v2_in32_c24_3rd>)
    1897:	0821d2        	l32i	a13, a1, 32
    189a:	20e220        	or	a14, a2, a2
    189d:	0008e0        	callx8	a8
    18a0:	ffa386        	j	1732 <compander_AU_proc+0x236>

000018a3 <compander_AU_proc+0x3a7>:
    18a3:	fcc492        	addi	a9, a4, -4
    18a6:	9188           	l32i.n	a8, a1, 36
    18a8:	fa24a1        	l32r	a10, 138 (4000 <_end+0x19fc>)
    18ab:	300aa4        	ae_cvtp24a16x2.ll	aep0, a10, a10
    18ae:	11a276        	loopgtz	a2, 18c3 <compander_AU_proc+0x3c7>
    18b1:	601814        	ae_lq32f.iu	aeq0, a8, 4
    18b4:	605914        	ae_lq32f.iu	aeq1, a9, 4
    18b7:	b58044        	ae_mulzasfq32sp16s.hh	aeq2, aeq1, aep0, aeq0, aep0
    18ba:	b10044        	ae_mulzaafq32sp16s.hh	aeq0, aeq1, aep0, aeq0, aep0
    18bd:	622904        	ae_sq32f.i	aeq2, a9, 0
    18c0:	620804        	ae_sq32f.i	aeq0, a8, 0

000018c3 <compander_AU_proc+0x3c7>:
    18c3:	ffa546        	j	175c <compander_AU_proc+0x260>

000018c6 <compander_AU_proc+0x3ca>:
    18c6:	00000000 fcc49200                                .....

000018cb <compander_AU_proc+0x3cf>:
    18cb:	fcc492        	addi	a9, a4, -4
    18ce:	91f8           	l32i.n	a15, a1, 36
    18d0:	0212e6        	bgei	a2, 1, 18d6 <compander_AU_proc+0x3da>
    18d3:	ffd446        	j	1828 <compander_AU_proc+0x32c>
    18d6:	21a120        	srai	a10, a2, 1
    18d9:	fa17b1        	l32r	a11, 138 (4000 <_end+0x19fc>)
    18dc:	300bb4        	ae_cvtp24a16x2.ll	aep0, a11, a11
    18df:	0e6207        	bbci	a2, 0, 18f1 <compander_AU_proc+0x3f5>
    18e2:	609f14        	ae_lq32f.iu	aeq2, a15, 4
    18e5:	605914        	ae_lq32f.iu	aeq1, a9, 4
    18e8:	b10864        	ae_mulzaafq32sp16s.hh	aeq1, aeq1, aep0, aeq2, aep0
    18eb:	061934        	ae_roundsq32asym	aeq0, aeq1
    18ee:	620904        	ae_sq32f.i	aeq0, a9, 0
    18f1:	1caa76        	loopgtz	a10, 1911 <compander_AU_proc+0x415>
    18f4:	600f14        	ae_lq32f.i	aeq0, a15, 4
    18f7:	604914        	ae_lq32f.i	aeq1, a9, 4
    18fa:	08017d192f 	{ ae_lq32f.iu	aeq0, a9, 8; ae_mulzaafq32sp16s.hh	aeq1, aeq1, aep0, aeq0, aep0 }
    18ff:	065934        	ae_roundsq32asym	aeq1, aeq1
    1902:	6219f4        	ae_sq32f.i	aeq1, a9, -4
    1905:	605f24        	ae_lq32f.iu	aeq1, a15, 8
    1908:	b10014        	ae_mulzaafq32sp16s.hh	aeq0, aeq0, aep0, aeq1, aep0
    190b:	060934        	ae_roundsq32asym	aeq0, aeq0
    190e:	620904        	ae_sq32f.i	aeq0, a9, 0

00001911 <compander_AU_proc+0x415>:
    1911:	ffc4c6        	j	1828 <compander_AU_proc+0x32c>

00001914 <compander_AU_proc+0x418>:
    1914:	239c           	beqz.n	a3, 192a <compander_AU_proc+0x42e>
    1916:	101ca6        	blti	a12, 1, 192a <compander_AU_proc+0x42e>
    1919:	3f92d2        	l16si	a13, a2, 126
    191c:	401d00        	ssl	a13
    191f:	07ac76        	loopgtz	a12, 192a <compander_AU_proc+0x42e>
    1922:	0398           	l32i.n	a9, a3, 0
    1924:	a19900        	sll	a9, a9
    1927:	09e394        	ae_s32ip	a9, a3, 4
    192a:	d55416        	beqz	a4, 1683 <compander_AU_proc+0x187>
    192d:	fec7e2        	addi	a14, a7, -2
    1930:	d4fe56        	bnez	a14, 1683 <compander_AU_proc+0x187>
    1933:	021ce6        	bgei	a12, 1, 1939 <compander_AU_proc+0x43d>
    1936:	ff5246        	j	1683 <compander_AU_proc+0x187>
    1939:	2021f2        	l32i	a15, a1, 128
    193c:	3f9ff2        	l16si	a15, a15, 126
    193f:	401f00        	ssl	a15
    1942:	07ac76        	loopgtz	a12, 194d <compander_AU_proc+0x451>
    1945:	0498           	l32i.n	a9, a4, 0
    1947:	a19900        	sll	a9, a9
    194a:	09e494        	ae_s32ip	a9, a4, 4

0000194d <compander_AU_proc+0x451>:
    194d:	f01d           	retw.n

0000194f <compander_AU_proc+0x453>:
	...

00001950 <compander_AU_proc+0x454>:
    1950:	0a2da6        	blti	a13, 2, 195e <compander_AU_proc+0x462>
    1953:	481c           	movi.n	a8, 20
    1955:	d28c80        	quos	a8, a12, a8
    1958:	1b6182        	s32i	a8, a1, 108
    195b:	fefd06        	j	1553 <compander_AU_proc+0x57>

0000195e <compander_AU_proc+0x462>:
    195e:	aa0c           	movi.n	a10, 10
    1960:	d2aca0        	quos	a10, a12, a10
    1963:	1b61a2        	s32i	a10, a1, 108
    1966:	fefa46        	j	1553 <compander_AU_proc+0x57>

00001969 <compander_AU_proc+0x46d>:
    1969:	000000                                        ...

0000196c <get_CPD_memsize>:
    196c:	004136        	entry	a1, 32
    196f:	828c           	beqz.n	a2, 197b <get_CPD_memsize+0xf>
    1971:	420b           	addi.n	a4, a2, -1
    1973:	40a132        	movi	a3, 0x140
    1976:	832340        	moveqz	a2, a3, a4
    1979:	f01d           	retw.n

0000197b <get_CPD_memsize+0xf>:
    197b:	f9f121        	l32r	a2, 140 (1908 <compander_AU_proc+0x40c>)
    197e:	f01d           	retw.n

00001980 <get_INS_memsize>:
    1980:	004136        	entry	a1, 32
    1983:	f9f021        	l32r	a2, 144 (8a8 <compander_AU_PARA_init+0x10>)
    1986:	f01d           	retw.n

00001988 <calc_slope>:
    1988:	006136        	entry	a1, 48
    198b:	21b920        	srai	a11, a2, 9
    198e:	bb2a           	add.n	a11, a11, a2
    1990:	c0ab30        	sub	a10, a11, a3
    1993:	031ae6        	bgei	a10, 1, 199a <calc_slope+0x12>
    1996:	120c           	movi.n	a2, 1
    1998:	f01d           	retw.n

0000199a <calc_slope+0x12>:
    199a:	f9bf81        	l32r	a8, 98 (2c0 <Xdiv>)
    199d:	378434        	ae_truncp24a32x2	aep0, a4, a3
    19a0:	c0db40        	sub	a13, a11, a4
    19a3:	36cd34        	ae_cvtq48a32s	aeq3, a13
    19a6:	fbe17ecabf 	{ ae_cvtq48a32s	aeq2, a10; ae_mulrfq32sp24s.l	aeq3, aeq3, aep0 }
    19ab:	fb6013a11f 	{ or	a10, a1, a1; ae_mulrfq32sp24s.h	aeq2, aeq2, aep0 }
    19b0:	fd3c4041be 	{ addi	a11, a1, 4; ae_roundsq32asym	aeq1, aeq3 }
    19b5:	fc347f19ef 	{ ae_trunca32q48	a9, aeq1; ae_roundsq32asym	aeq0, aeq2 }
    19ba:	368c04        	ae_trunca32q48	a12, aeq0
    19bd:	01c9           	s32i.n	a12, a1, 0
    19bf:	1199           	s32i.n	a9, a1, 4
    19c1:	0008e0        	callx8	a8
    19c4:	0a2d           	mov.n	a2, a10
    19c6:	f01d           	retw.n

000019c8 <calc_slope_1>:
    19c8:	006136        	entry	a1, 48
    19cb:	21a940        	srai	a10, a4, 9
    19ce:	aa4a           	add.n	a10, a10, a4
    19d0:	c0aa20        	sub	a10, a10, a2
    19d3:	1f1aa6        	blti	a10, 1, 19f6 <calc_slope_1+0x2e>
    19d6:	c0b320        	sub	a11, a3, a2
    19d9:	041be6        	bgei	a11, 1, 19e1 <calc_slope_1+0x19>
    19dc:	f9db21        	l32r	a2, 148 (2000 <_DYNAMIC+0xf0>)
    19df:	f01d           	retw.n

000019e1 <calc_slope_1+0x19>:
    19e1:	f9ad81        	l32r	a8, 98 (2c0 <Xdiv>)
    19e4:	01a9           	s32i.n	a10, a1, 0
    19e6:	2192b0        	srai	a9, a11, 2
    19e9:	1199           	s32i.n	a9, a1, 4
    19eb:	a14b           	addi.n	a10, a1, 4
    19ed:	01bd           	mov.n	a11, a1
    19ef:	0008e0        	callx8	a8
    19f2:	0a2d           	mov.n	a2, a10
    19f4:	f01d           	retw.n

000019f6 <calc_slope_1+0x2e>:
    19f6:	120c           	movi.n	a2, 1
    19f8:	f01d           	retw.n

000019fa <calc_slope_1+0x32>:
	...

000019fc <calc_slope_2>:
    19fc:	006136        	entry	a1, 48
    19ff:	f9a681        	l32r	a8, 98 (2c0 <Xdiv>)
    1a02:	21e920        	srai	a14, a2, 9
    1a05:	378434        	ae_truncp24a32x2	aep0, a4, a3
    1a08:	ee2a           	add.n	a14, a14, a2
    1a0a:	c0de40        	sub	a13, a14, a4
    1a0d:	c0ee30        	sub	a14, a14, a3
    1a10:	36ce34        	ae_cvtq48a32s	aeq3, a14
    1a13:	fb617ecdbf 	{ ae_cvtq48a32s	aeq2, a13; ae_mulrfq32sp24s.h	aeq3, aeq3, aep0 }
    1a18:	fbe013a11f 	{ or	a10, a1, a1; ae_mulrfq32sp24s.l	aeq2, aeq2, aep0 }
    1a1d:	fd3c4041be 	{ addi	a11, a1, 4; ae_roundsq32asym	aeq1, aeq3 }
    1a22:	fc347f19ef 	{ ae_trunca32q48	a9, aeq1; ae_roundsq32asym	aeq0, aeq2 }
    1a27:	368c04        	ae_trunca32q48	a12, aeq0
    1a2a:	11c9           	s32i.n	a12, a1, 4
    1a2c:	219290        	srai	a9, a9, 2
    1a2f:	0199           	s32i.n	a9, a1, 0
    1a31:	0008e0        	callx8	a8
    1a34:	0a2d           	mov.n	a2, a10
    1a36:	f01d           	retw.n

00001a38 <INS_SetFrame120_Init>:
    1a38:	004136        	entry	a1, 32
    1a3b:	08d242        	addmi	a4, a2, 0x800
    1a3e:	a30c           	movi.n	a3, 10
    1a40:	535432        	s16i	a3, a4, 166
    1a43:	f01d           	retw.n

00001a45 <INS_SetFrame120_Init+0xd>:
    1a45:	000000                                        ...

00001a48 <INS_Init>:
    1a48:	004136        	entry	a1, 32
    1a4b:	040392        	l8ui	a9, a3, 4
    1a4e:	0203b2        	l8ui	a11, a3, 2
    1a51:	0303a2        	l8ui	a10, a3, 3
    1a54:	08d242        	addmi	a4, a2, 0x800
    1a57:	0003c2        	l8ui	a12, a3, 0
    1a5a:	050382        	l8ui	a8, a3, 5
    1a5d:	5054c2        	s16i	a12, a4, 160
    1a60:	50a3c2        	movi	a12, 0x350
    1a63:	118880        	slli	a8, a8, 8
    1a66:	11aa80        	slli	a10, a10, 8
    1a69:	20aab0        	or	a10, a10, a11
    1a6c:	208890        	or	a8, a8, a9
    1a6f:	00a0b2        	movi	a11, 0
    1a72:	888d           	ae_sext16	a8, a8
    1a74:	8aad           	ae_sext16	a10, a10
    1a76:	5154a2        	s16i	a10, a4, 162
    1a79:	525482        	s16i	a8, a4, 164
    1a7c:	f98981        	l32r	a8, a0 (1e50 <xt_memset>)
    1a7f:	20a220        	or	a10, a2, a2
    1a82:	a4c442        	addi	a4, a4, -92
    1a85:	0008e0        	callx8	a8
    1a88:	8194d2        	l16si	a13, a4, 0x102
    1a8b:	0803f2        	l8ui	a15, a3, 8
    1a8e:	090382        	l8ui	a8, a3, 9
    1a91:	0a03a2        	l8ui	a10, a3, 10
    1a94:	0b03e2        	l8ui	a14, a3, 11
    1a97:	070392        	l8ui	a9, a3, 7
    1a9a:	0603b2        	l8ui	a11, a3, 6
    1a9d:	11ee80        	slli	a14, a14, 8
    1aa0:	119980        	slli	a9, a9, 8
    1aa3:	2099b0        	or	a9, a9, a11
    1aa6:	20eea0        	or	a14, a14, a10
    1aa9:	11ee80        	slli	a14, a14, 8
    1aac:	899d           	ae_sext16	a9, a9
    1aae:	425292        	s16i	a9, a2, 132
    1ab1:	20ee80        	or	a14, a14, a8
    1ab4:	11ee80        	slli	a14, a14, 8
    1ab7:	20eef0        	or	a14, a14, a15
    1aba:	2062e2        	s32i	a14, a2, 128
    1abd:	359d26        	beqi	a13, 10, 1af6 <INS_Init+0xae>
    1ac0:	60a0f2        	movi	a15, 96
    1ac3:	082c           	movi.n	a8, 32
    1ac5:	00a492        	movi	a9, 0x400
    1ac8:	115292        	s16i	a9, a2, 34
    1acb:	105282        	s16i	a8, a2, 32
    1ace:	b2f9           	s32i.n	a15, a2, 44

00001ad0 <INS_Init+0x88>:
    1ad0:	f98d81        	l32r	a8, 104 (374 <AU_INS_coef_init>)
    1ad3:	01d2b2        	addmi	a11, a2, 0x100
    1ad6:	f983a1        	l32r	a10, e4 (7fffffff <_end+0x7fffd9fb>)
    1ad9:	490c           	movi.n	a9, 4
    1adb:	f980c1        	l32r	a12, dc (8000000 <_end+0x7ffd9fc>)
    1ade:	f2c9           	s32i.n	a12, a2, 60
    1ae0:	e2c9           	s32i.n	a12, a2, 56
    1ae2:	3f5292        	s16i	a9, a2, 126
    1ae5:	1062a2        	s32i	a10, a2, 64
    1ae8:	a0cbb2        	addi	a11, a11, -96
    1aeb:	c2b9           	s32i.n	a11, a2, 48
    1aed:	d2b9           	s32i.n	a11, a2, 52
    1aef:	02ad           	mov.n	a10, a2
    1af1:	0008e0        	callx8	a8
    1af4:	f01d           	retw.n

00001af6 <INS_Init+0xae>:
    1af6:	ed1c           	movi.n	a13, 30
    1af8:	ae0c           	movi.n	a14, 10
    1afa:	f96bf1        	l32r	a15, a8 (ccd <compander_AU_init+0x71>)
    1afd:	1152f2        	s16i	a15, a2, 34
    1b00:	1052e2        	s16i	a14, a2, 32
    1b03:	b2d9           	s32i.n	a13, a2, 44
    1b05:	fff1c6        	j	1ad0 <INS_Init+0x88>

00001b08 <INS_Prcs>:
    1b08:	008136        	entry	a1, 64
    1b0b:	08d2a2        	addmi	a10, a2, 0x800
    1b0e:	509a82        	l16si	a8, a10, 160
    1b11:	a4ca92        	addi	a9, a10, -92
    1b14:	539ab2        	l16si	a11, a10, 166
    1b17:	02e807        	bbsi	a8, 0, 1b1d <INS_Prcs+0x15>
    1b1a:	0021c6        	j	1ba5 <INS_Prcs+0x9d>
    1b1d:	1092a2        	l16si	a10, a2, 32
    1b20:	029b66        	bnei	a11, 10, 1b26 <INS_Prcs+0x1e>
    1b23:	002046        	j	1ba8 <INS_Prcs+0xa0>
    1b26:	217550        	srai	a7, a5, 5

00001b29 <INS_Prcs+0x21>:
    1b29:	7817a6        	blti	a7, 1, 1ba5 <INS_Prcs+0x9d>
    1b2c:	050c           	movi.n	a5, 0
    1b2e:	4199           	s32i.n	a9, a1, 16
    1b30:	11cae0        	slli	a12, a10, 2
    1b33:	51c9           	s32i.n	a12, a1, 20
    1b35:	000c86        	j	1b6b <INS_Prcs+0x63>

00001b38 <INS_Prcs+0x30>:
	...

00001b39 <INS_Prcs+0x31>:
    1b39:	03bd           	mov.n	a11, a3
    1b3b:	04cd           	mov.n	a12, a4
    1b3d:	06ed           	mov.n	a14, a6
    1b3f:	41f8           	l32i.n	a15, a1, 16
    1b41:	1022a2        	l32i	a10, a2, 64
    1b44:	080c           	movi.n	a8, 0
    1b46:	7f9f92        	l16si	a9, a15, 254
    1b49:	72aad0        	salt	a10, a10, a13
    1b4c:	809ff2        	l16si	a15, a15, 0x100
    1b4f:	0189           	s32i.n	a8, a1, 0
    1b51:	1189           	s32i.n	a8, a1, 4
    1b53:	f97181        	l32r	a8, 118 (f18 <F_CPD_Intp_Out>)
    1b56:	93f9a0        	movnez	a15, a9, a10
    1b59:	8ffd           	ae_sext16	a15, a15
    1b5b:	02ad           	mov.n	a10, a2
    1b5d:	0008e0        	callx8	a8
    1b60:	5188           	l32i.n	a8, a1, 20
    1b62:	551b           	addi.n	a5, a5, 1
    1b64:	338a           	add.n	a3, a3, a8
    1b66:	448a           	add.n	a4, a4, a8
    1b68:	391757        	beq	a7, a5, 1ba5 <INS_Prcs+0x9d>

00001b6b <INS_Prcs+0x63>:
    1b6b:	02ad           	mov.n	a10, a2
    1b6d:	03bd           	mov.n	a11, a3
    1b6f:	04cd           	mov.n	a12, a4
    1b71:	f96781        	l32r	a8, 110 (d48 <F_CPD_Mag_In>)
    1b74:	06dd           	mov.n	a13, a6
    1b76:	1e0c           	movi.n	a14, 1
    1b78:	0008e0        	callx8	a8
    1b7b:	2022a2        	l32i	a10, a2, 128
    1b7e:	2322b2        	l32i	a11, a2, 140
    1b81:	f958d1        	l32r	a13, e4 (7fffffff <_end+0x7fffd9fb>)
    1b84:	909aa0        	addx2	a9, a10, a10
    1b87:	aeab97        	bge	a11, a9, 1b39 <INS_Prcs+0x31>
    1b8a:	0a2ab7        	blt	a10, a11, 1b98 <INS_Prcs+0x90>
    1b8d:	4292d2        	l16si	a13, a2, 132
    1b90:	11dd00        	slli	a13, a13, 16
    1b93:	ffe886        	j	1b39 <INS_Prcs+0x31>

00001b96 <INS_Prcs+0x8e>:
	...

00001b98 <INS_Prcs+0x90>:
    1b98:	f95d81        	l32r	a8, 10c (cec <F_CPD_Ins_Gain>)
    1b9b:	02ad           	mov.n	a10, a2
    1b9d:	0008e0        	callx8	a8
    1ba0:	0add           	mov.n	a13, a10
    1ba2:	ffe4c6        	j	1b39 <INS_Prcs+0x31>

00001ba5 <INS_Prcs+0x9d>:
    1ba5:	f01d           	retw.n

00001ba7 <INS_Prcs+0x9f>:
	...

00001ba8 <INS_Prcs+0xa0>:
    1ba8:	a70c           	movi.n	a7, 10
    1baa:	d27570        	quos	a7, a5, a7
    1bad:	ffde06        	j	1b29 <INS_Prcs+0x21>

00001bb0 <__do_global_ctors_aux>:
    1bb0:	004136        	entry	a1, 32
    1bb3:	f96681        	l32r	a8, 14c (2334 <_DYNAMIC+0x424>)
    1bb6:	7f2882        	l32i	a8, a8, 0x1fc
    1bb9:	f96521        	l32r	a2, 150 (2530 <__CTOR_LIST___47>)
    1bbc:	0c0826        	beqi	a8, -1, 1bcc <__do_global_ctors_aux+0x1c>
    1bbf:	02a8           	l32i.n	a10, a2, 0

00001bc1 <__do_global_ctors_aux+0x11>:
    1bc1:	000ae0        	callx8	a10
    1bc4:	fcc222        	addi	a2, a2, -4
    1bc7:	02a8           	l32i.n	a10, a2, 0
    1bc9:	f40a66        	bnei	a10, -1, 1bc1 <__do_global_ctors_aux+0x11>

00001bcc <__do_global_ctors_aux+0x1c>:
    1bcc:	f01d           	retw.n

00001bce <__do_global_ctors_aux+0x1e>:
	...

00001bd0 <HIFI_d1IB_v2_in32_c16_4th>:
    1bd0:	004136        	entry	a1, 32
    1bd3:	444304        	ae_lp16x2f.i	aep4, a3, 0
    1bd6:	443314        	ae_lp16x2f.i	aep3, a3, 4
    1bd9:	442324        	ae_lp16x2f.i	aep2, a3, 8
    1bdc:	fcc222        	addi	a2, a2, -4
    1bdf:	028d           	mov.n	a8, a2
    1be1:	52a676        	loopgtz	a6, 1c37 <HIFI_d1IB_v2_in32_c16_4th+0x67>
    1be4:	60c414        	ae_lq32f.i	aeq3, a4, 4
    1be7:	600404        	ae_lq32f.i	aeq0, a4, 0
    1bea:	609814        	ae_lq32f.iu	aeq2, a8, 4
    1bed:	0b8c39a40f 	{ ae_sq32f.i	aeq2, a4, 0; ae_mulzaafq32sp16s.hh	aeq2, aeq2, aep3, aeq0, aep4 }
    1bf2:	f512b9841f 	{ ae_sq32f.i	aeq0, a4, 4; ae_mulafq32sp16s.h	aeq2, aeq0, aep4 }
    1bf7:	f793bd250f 	{ ae_lq32f.i	aeq1, a5, 0; ae_mulafq32sp16s.l	aeq2, aeq3, aep4 }
    1bfc:	03a044        	ae_mulafq32sp16s.h	aeq2, aeq1, aep2
    1bff:	f50bbd051f 	{ ae_lq32f.i	aeq0, a5, 4; ae_mulafq32sp16s.h	aeq2, aeq1, aep2 }
    1c04:	03a014        	ae_mulafq32sp16s.l	aeq2, aeq0, aep2
    1c07:	445334        	ae_lp16x2f.i	aep5, a3, 12
    1c0a:	fe546d634f 	{ ae_lp16x2f.i	aep6, a3, 16; ae_roundsq32sym	aeq2, aeq2 }
    1c0f:	b1ed94        	ae_mulzaafq32sp16s.hh	aeq3, aeq2, aep6, aeq1, aep5
    1c12:	f517ed135f 	{ ae_lp16x2f.i	aep1, a3, 20; ae_mulafq32sp16s.h	aeq3, aeq1, aep5 }
    1c17:	f596fd052f 	{ ae_lq32f.i	aeq0, a5, 8; ae_mulafq32sp16s.l	aeq3, aeq0, aep5 }
    1c1c:	f506f9951f 	{ ae_sq32f.i	aeq1, a5, 4; ae_mulafq32sp16s.h	aeq3, aeq0, aep1 }
    1c21:	f506fd253f 	{ ae_lq32f.i	aeq1, a5, 12; ae_mulafq32sp16s.h	aeq3, aeq0, aep1 }
    1c26:	039854        	ae_mulafq32sp16s.l	aeq3, aeq1, aep1
    1c29:	622504        	ae_sq32f.i	aeq2, a5, 0
    1c2c:	fe5c79853f 	{ ae_sq32f.i	aeq0, a5, 12; ae_roundsq32sym	aeq2, aeq3 }
    1c31:	622524        	ae_sq32f.i	aeq2, a5, 8
    1c34:	626214        	ae_sq32f.iu	aeq2, a2, 4

00001c37 <HIFI_d1IB_v2_in32_c16_4th+0x67>:
    1c37:	f01d           	retw.n

00001c39 <HIFI_d1IB_v2_in32_c16_4th+0x69>:
    1c39:	000000                                        ...

00001c3c <HIFI_d1IB_v2_in24_c24_4th>:
    1c3c:	004136        	entry	a1, 32
    1c3f:	fcc222        	addi	a2, a2, -4
    1c42:	067d           	mov.n	a7, a6
    1c44:	049d           	mov.n	a9, a4
    1c46:	056d           	mov.n	a6, a5
    1c48:	034d           	mov.n	a4, a3
    1c4a:	095d           	mov.n	a5, a9
    1c4c:	023d           	mov.n	a3, a2
    1c4e:	47a776        	loopgtz	a7, 1c99 <HIFI_d1IB_v2_in24_c24_4th+0x5d>
    1c51:	540424        	ae_lp24x2.i	aep0, a4, 16
    1c54:	5c1504        	ae_lp24x2f.i	aep1, a5, 0
    1c57:	4ca214        	ae_lp24.iu	aep2, a2, 4
    1c5a:	dd2970240f 	{ ae_lp24x2.i	aep2, a4, 0; ae_selp24.hh	aep3, aep2, aep1 }
    1c5f:	f046b0141f 	{ ae_lp24x2.i	aep1, a4, 8; ae_mulzaafp24s.hh.ll	aeq0, aep1, aep2 }
    1c64:	e22c2f160f 	{ ae_lp24x2f.i	aep1, a6, 0; ae_mulaafp24s.hh.ll	aeq0, aep3, aep1 }
    1c69:	e20436350f 	{ ae_sp24x2f.i	aep3, a5, 0; ae_mulaafp24s.hh.ll	aeq0, aep1, aep0 }
    1c6e:	e20630045f 	{ ae_lp24x2.i	aep0, a4, 40; ae_mulafp24s.hh	aeq0, aep1, aep0 }
    1c73:	542434        	ae_lp24x2.i	aep2, a4, 24
    1c76:	fae7b0344f 	{ ae_lp24x2.i	aep3, a4, 32; ae_roundsp24q48sym	aep4, aeq0 }
    1c7b:	f046be414f 	{ ae_selp24.hh	aep1, aep4, aep1; ae_mulzaafp24s.hh.ll	aeq0, aep1, aep2 }
    1c80:	e2642f261f 	{ ae_lp24x2f.i	aep2, a6, 8; ae_mulaafp24s.hh.ll	aeq0, aep1, aep3 }
    1c85:	e20836160f 	{ ae_sp24x2f.i	aep1, a6, 0; ae_mulaafp24s.hh.ll	aeq0, aep2, aep0 }
    1c8a:	012004        	ae_mulafp24s.hh	aeq0, aep2, aep0
    1c8d:	1b8c04        	ae_roundsp24q48sym	aep0, aeq0
    1c90:	1a8a94        	ae_selp24.hh	aep1, aep0, aep2
    1c93:	5e1614        	ae_sp24x2f.i	aep1, a6, 8
    1c96:	4e8314        	ae_sp24s.l.iu	aep0, a3, 4

00001c99 <HIFI_d1IB_v2_in24_c24_4th+0x5d>:
    1c99:	f01d           	retw.n

00001c9b <HIFI_d1IB_v2_in24_c24_4th+0x5f>:
	...

00001c9c <HIFI_d1IB_v2_in32_c24_4th>:
    1c9c:	004136        	entry	a1, 32
    1c9f:	544304        	ae_lp24x2.i	aep4, a3, 0
    1ca2:	543314        	ae_lp24x2.i	aep3, a3, 8
    1ca5:	542324        	ae_lp24x2.i	aep2, a3, 16
    1ca8:	fcc222        	addi	a2, a2, -4
    1cab:	028d           	mov.n	a8, a2
    1cad:	56a676        	loopgtz	a6, 1d07 <HIFI_d1IB_v2_in32_c24_4th+0x6b>
    1cb0:	600404        	ae_lq32f.i	aeq0, a4, 0
    1cb3:	609814        	ae_lq32f.iu	aeq2, a8, 4
    1cb6:	fb0c39a40f 	{ ae_sq32f.i	aeq2, a4, 0; ae_mulfq32sp24s.h	aeq2, aeq2, aep3 }
    1cbb:	f552bd641f 	{ ae_lq32f.i	aeq3, a4, 4; ae_mulafq32sp24s.h	aeq2, aeq0, aep4 }
    1cc0:	f552b9841f 	{ ae_sq32f.i	aeq0, a4, 4; ae_mulafq32sp24s.h	aeq2, aeq0, aep4 }
    1cc5:	f7d3bd250f 	{ ae_lq32f.i	aeq1, a5, 0; ae_mulafq32sp24s.l	aeq2, aeq3, aep4 }
    1cca:	08a264        	ae_mulafq32sp24s.h	aeq2, aeq1, aep2
    1ccd:	f54bbd051f 	{ ae_lq32f.i	aeq0, a5, 4; ae_mulafq32sp24s.h	aeq2, aeq1, aep2 }
    1cd2:	08a324        	ae_mulafq32sp24s.l	aeq2, aeq0, aep2
    1cd5:	fe5470634f 	{ ae_lp24x2.i	aep6, a3, 32; ae_roundsq32sym	aeq2, aeq2 }
    1cda:	fb1870533f 	{ ae_lp24x2.i	aep5, a3, 24; ae_mulfq32sp24s.h	aeq3, aeq2, aep6 }
    1cdf:	08da64        	ae_mulafq32sp24s.h	aeq3, aeq1, aep5
    1ce2:	f557f0135f 	{ ae_lp24x2.i	aep1, a3, 40; ae_mulafq32sp24s.h	aeq3, aeq1, aep5 }
    1ce7:	f5d6fd052f 	{ ae_lq32f.i	aeq0, a5, 8; ae_mulafq32sp24s.l	aeq3, aeq0, aep5 }
    1cec:	f546f9951f 	{ ae_sq32f.i	aeq1, a5, 4; ae_mulafq32sp24s.h	aeq3, aeq0, aep1 }
    1cf1:	f546fd253f 	{ ae_lq32f.i	aeq1, a5, 12; ae_mulafq32sp24s.h	aeq3, aeq0, aep1 }
    1cf6:	089b64        	ae_mulafq32sp24s.l	aeq3, aeq1, aep1
    1cf9:	622504        	ae_sq32f.i	aeq2, a5, 0
    1cfc:	fe5c79853f 	{ ae_sq32f.i	aeq0, a5, 12; ae_roundsq32sym	aeq2, aeq3 }
    1d01:	622524        	ae_sq32f.i	aeq2, a5, 8
    1d04:	626214        	ae_sq32f.iu	aeq2, a2, 4

00001d07 <HIFI_d1IB_v2_in32_c24_4th+0x6b>:
    1d07:	f01d           	retw.n

00001d09 <HIFI_d1IB_v2_in32_c24_4th+0x6d>:
    1d09:	00000000 36000000                                .......

00001d10 <HIFI_d1IB_v2_in32_c16_3rd>:
    1d10:	004136        	entry	a1, 32
    1d13:	442304        	ae_lp16x2f.i	aep2, a3, 0
    1d16:	443314        	ae_lp16x2f.i	aep3, a3, 4
    1d19:	444324        	ae_lp16x2f.i	aep4, a3, 8
    1d1c:	fcc222        	addi	a2, a2, -4
    1d1f:	067d           	mov.n	a7, a6
    1d21:	049d           	mov.n	a9, a4
    1d23:	056d           	mov.n	a6, a5
    1d25:	034d           	mov.n	a4, a3
    1d27:	205990        	or	a5, a9, a9
    1d2a:	203220        	or	a3, a2, a2
    1d2d:	47a776        	loopgtz	a7, 1d78 <HIFI_d1IB_v2_in32_c16_3rd+0x68>
    1d30:	600504        	ae_lq32f.i	aeq0, a5, 0
    1d33:	605214        	ae_lq32f.iu	aeq1, a2, 4
    1d36:	094d3d651f 	{ ae_lq32f.i	aeq3, a5, 4; ae_mulzaafq32sp16s.hh	aeq2, aeq1, aep3, aeq0, aep2 }
    1d3b:	f50ab9950f 	{ ae_sq32f.i	aeq1, a5, 0; ae_mulafq32sp16s.h	aeq2, aeq0, aep2 }
    1d40:	f78bb9851f 	{ ae_sq32f.i	aeq0, a5, 4; ae_mulafq32sp16s.l	aeq2, aeq3, aep2 }
    1d45:	600604        	ae_lq32f.i	aeq0, a6, 0
    1d48:	f512bd261f 	{ ae_lq32f.i	aeq1, a6, 4; ae_mulafq32sp16s.h	aeq2, aeq0, aep4 }
    1d4d:	f512ad044f 	{ ae_lp16x2f.i	aep0, a4, 16; ae_mulafq32sp16s.h	aeq2, aeq0, aep4 }
    1d52:	f593ad143f 	{ ae_lp16x2f.i	aep1, a4, 12; ae_mulafq32sp16s.l	aeq2, aeq1, aep4 }
    1d57:	604624        	ae_lq32f.i	aeq1, a6, 8
    1d5a:	ff5479963f 	{ ae_sq32f.i	aeq1, a6, 12; ae_roundsq32sym	aeq3, aeq2 }
    1d5f:	0b0539861f 	{ ae_sq32f.i	aeq0, a6, 4; ae_mulzaafq32sp16s.hh	aeq2, aeq3, aep1, aeq0, aep0 }
    1d64:	f502b9b60f 	{ ae_sq32f.i	aeq3, a6, 0; ae_mulafq32sp16s.h	aeq2, aeq0, aep0 }
    1d69:	038054        	ae_mulafq32sp16s.l	aeq2, aeq1, aep0
    1d6c:	038054        	ae_mulafq32sp16s.l	aeq2, aeq1, aep0
    1d6f:	062834        	ae_roundsq32sym	aeq0, aeq2
    1d72:	620624        	ae_sq32f.i	aeq0, a6, 8
    1d75:	624314        	ae_sq32f.iu	aeq0, a3, 4

00001d78 <HIFI_d1IB_v2_in32_c16_3rd+0x68>:
    1d78:	f01d           	retw.n

00001d7a <HIFI_d1IB_v2_in32_c16_3rd+0x6a>:
	...

00001d7c <HIFI_d1IB_v2_in24_c24_3rd>:
    1d7c:	004136        	entry	a1, 32
    1d7f:	fcc222        	addi	a2, a2, -4
    1d82:	067d           	mov.n	a7, a6
    1d84:	049d           	mov.n	a9, a4
    1d86:	056d           	mov.n	a6, a5
    1d88:	034d           	mov.n	a4, a3
    1d8a:	095d           	mov.n	a5, a9
    1d8c:	023d           	mov.n	a3, a2
    1d8e:	46a776        	loopgtz	a7, 1dd8 <HIFI_d1IB_v2_in24_c24_3rd+0x5c>
    1d91:	540424        	ae_lp24x2.i	aep0, a4, 16
    1d94:	5c1504        	ae_lp24x2f.i	aep1, a5, 0
    1d97:	4ca214        	ae_lp24.iu	aep2, a2, 4
    1d9a:	dd2970240f 	{ ae_lp24x2.i	aep2, a4, 0; ae_selp24.hh	aep3, aep2, aep1 }
    1d9f:	f046b0141f 	{ ae_lp24x2.i	aep1, a4, 8; ae_mulzaafp24s.hh.ll	aeq0, aep1, aep2 }
    1da4:	e22c2f160f 	{ ae_lp24x2f.i	aep1, a6, 0; ae_mulaafp24s.hh.ll	aeq0, aep3, aep1 }
    1da9:	e20436350f 	{ ae_sp24x2f.i	aep3, a5, 0; ae_mulaafp24s.hh.ll	aeq0, aep1, aep0 }
    1dae:	e20630043f 	{ ae_lp24x2.i	aep0, a4, 24; ae_mulafp24s.hh	aeq0, aep1, aep0 }
    1db3:	5c2614        	ae_lp24x2f.i	aep2, a6, 8
    1db6:	1b8c34        	ae_roundsp24q48sym	aep3, aeq0
    1db9:	1ab994        	ae_selp24.hh	aep1, aep3, aep1
    1dbc:	f006b6160f 	{ ae_sp24x2f.i	aep1, a6, 0; ae_mulzaafp24s.hh.ll	aeq0, aep1, aep0 }
    1dc1:	e207b0044f 	{ ae_lp24x2.i	aep0, a4, 32; ae_mulafp24s.ll	aeq0, aep1, aep0 }
    1dc6:	012014        	ae_mulafp24s.hl	aeq0, aep2, aep0
    1dc9:	012014        	ae_mulafp24s.hl	aeq0, aep2, aep0
    1dcc:	1b8c04        	ae_roundsp24q48sym	aep0, aeq0
    1dcf:	1a8a94        	ae_selp24.hh	aep1, aep0, aep2
    1dd2:	5e1614        	ae_sp24x2f.i	aep1, a6, 8
    1dd5:	4e8314        	ae_sp24s.l.iu	aep0, a3, 4

00001dd8 <HIFI_d1IB_v2_in24_c24_3rd+0x5c>:
    1dd8:	f01d           	retw.n

00001dda <HIFI_d1IB_v2_in24_c24_3rd+0x5e>:
	...

00001ddc <HIFI_d1IB_v2_in32_c24_3rd>:
    1ddc:	004136        	entry	a1, 32
    1ddf:	542304        	ae_lp24x2.i	aep2, a3, 0
    1de2:	543314        	ae_lp24x2.i	aep3, a3, 8
    1de5:	544324        	ae_lp24x2.i	aep4, a3, 16
    1de8:	fcc222        	addi	a2, a2, -4
    1deb:	067d           	mov.n	a7, a6
    1ded:	049d           	mov.n	a9, a4
    1def:	056d           	mov.n	a6, a5
    1df1:	034d           	mov.n	a4, a3
    1df3:	205990        	or	a5, a9, a9
    1df6:	203220        	or	a3, a2, a2
    1df9:	4ca776        	loopgtz	a7, 1e49 <HIFI_d1IB_v2_in32_c24_3rd+0x6d>
    1dfc:	600504        	ae_lq32f.i	aeq0, a5, 0
    1dff:	605214        	ae_lq32f.iu	aeq1, a2, 4
    1e02:	f90d3d651f 	{ ae_lq32f.i	aeq3, a5, 4; ae_mulfq32sp24s.h	aeq2, aeq1, aep3 }
    1e07:	f54ab9950f 	{ ae_sq32f.i	aeq1, a5, 0; ae_mulafq32sp24s.h	aeq2, aeq0, aep2 }
    1e0c:	f54ab9851f 	{ ae_sq32f.i	aeq0, a5, 4; ae_mulafq32sp24s.h	aeq2, aeq0, aep2 }
    1e11:	f7cbbd060f 	{ ae_lq32f.i	aeq0, a6, 0; ae_mulafq32sp24s.l	aeq2, aeq3, aep2 }
    1e16:	f552bd261f 	{ ae_lq32f.i	aeq1, a6, 4; ae_mulafq32sp24s.h	aeq2, aeq0, aep4 }
    1e1b:	f552b0043f 	{ ae_lp24x2.i	aep0, a4, 24; ae_mulafq32sp24s.h	aeq2, aeq0, aep4 }
    1e20:	f5d3b0144f 	{ ae_lp24x2.i	aep1, a4, 32; ae_mulafq32sp24s.l	aeq2, aeq1, aep4 }
    1e25:	604624        	ae_lq32f.i	aeq1, a6, 8
    1e28:	ff5479963f 	{ ae_sq32f.i	aeq1, a6, 12; ae_roundsq32sym	aeq3, aeq2 }
    1e2d:	fb0139861f 	{ ae_sq32f.i	aeq0, a6, 4; ae_mulfq32sp24s.h	aeq2, aeq3, aep0 }
    1e32:	f546b9b60f 	{ ae_sq32f.i	aeq3, a6, 0; ae_mulafq32sp24s.h	aeq2, aeq0, aep1 }
    1e37:	089224        	ae_mulafq32sp24s.h	aeq2, aeq0, aep1
    1e3a:	089364        	ae_mulafq32sp24s.l	aeq2, aeq1, aep1
    1e3d:	089364        	ae_mulafq32sp24s.l	aeq2, aeq1, aep1
    1e40:	062834        	ae_roundsq32sym	aeq0, aeq2
    1e43:	620624        	ae_sq32f.i	aeq0, a6, 8
    1e46:	624314        	ae_sq32f.iu	aeq0, a3, 4

00001e49 <HIFI_d1IB_v2_in32_c24_3rd+0x6d>:
    1e49:	f01d           	retw.n

00001e4b <HIFI_d1IB_v2_in32_c24_3rd+0x6f>:
    1e4b:	00000000 00413600                                .....

00001e50 <xt_memset>:
    1e50:	004136        	entry	a1, 32
    1e53:	026d           	mov.n	a6, a2
    1e55:	828c           	beqz.n	a2, 1e61 <xt_memset+0x11>
    1e57:	029476        	loopnez	a4, 1e5d <xt_memset+0xd>
    1e5a:	09c234        	ae_s16ip	a3, a2, 2

00001e5d <xt_memset+0xd>:
    1e5d:	062d           	mov.n	a2, a6
    1e5f:	f01d           	retw.n

00001e61 <xt_memset+0x11>:
    1e61:	020c           	movi.n	a2, 0
    1e63:	f01d           	retw.n

00001e65 <xt_memset+0x15>:
    1e65:	000000                                        ...

00001e68 <xt_memcpy>:
    1e68:	004136        	entry	a1, 32
    1e6b:	027d           	mov.n	a7, a2
    1e6d:	032d           	mov.n	a2, a3
    1e6f:	043d           	mov.n	a3, a4
    1e71:	07c716        	beqz	a7, 1ef1 <xt_memcpy+0x89>
    1e74:	079216        	beqz	a2, 1ef1 <xt_memcpy+0x89>
    1e77:	25b277        	bgeu	a2, a7, 1ea0 <xt_memcpy+0x38>
    1e7a:	904420        	addx2	a4, a4, a2
    1e7d:	1fb747        	bgeu	a7, a4, 1ea0 <xt_memcpy+0x38>
    1e80:	fec422        	addi	a2, a4, -2
    1e83:	530b           	addi.n	a5, a3, -1
    1e85:	904370        	addx2	a4, a3, a7
    1e88:	fec442        	addi	a4, a4, -2
    1e8b:	5e0526        	beqi	a5, -1, 1eed <xt_memcpy+0x85>
    1e8e:	099376        	loopnez	a3, 1e9b <xt_memcpy+0x33>
    1e91:	223d           	ae_l16si.n	a3, a2, 0
    1e93:	fec222        	addi	a2, a2, -2
    1e96:	fec442        	addi	a4, a4, -2
    1e99:	743d           	ae_s16i.n	a3, a4, 2

00001e9b <xt_memcpy+0x33>:
    1e9b:	072d           	mov.n	a2, a7
    1e9d:	f01d           	retw.n

00001e9f <xt_memcpy+0x37>:
	...

00001ea0 <xt_memcpy+0x38>:
    1ea0:	074d           	mov.n	a4, a7
    1ea2:	530b           	addi.n	a5, a3, -1
    1ea4:	450526        	beqi	a5, -1, 1eed <xt_memcpy+0x85>
    1ea7:	248030        	extui	a8, a3, 0, 3
    1eaa:	036d           	mov.n	a6, a3
    1eac:	413360        	srli	a3, a6, 3
    1eaf:	069876        	loopnez	a8, 1eb9 <xt_memcpy+0x51>
    1eb2:	229d           	ae_l16si.n	a9, a2, 0
    1eb4:	09c494        	ae_s16ip	a9, a4, 2
    1eb7:	222b           	addi.n	a2, a2, 2
    1eb9:	309376        	loopnez	a3, 1eed <xt_memcpy+0x85>
    1ebc:	223d           	ae_l16si.n	a3, a2, 0
    1ebe:	09c434        	ae_s16ip	a3, a4, 2
    1ec1:	323d           	ae_l16si.n	a3, a2, 2
    1ec3:	09c434        	ae_s16ip	a3, a4, 2
    1ec6:	029232        	l16si	a3, a2, 4
    1ec9:	09c434        	ae_s16ip	a3, a4, 2
    1ecc:	039232        	l16si	a3, a2, 6
    1ecf:	09c434        	ae_s16ip	a3, a4, 2
    1ed2:	049232        	l16si	a3, a2, 8
    1ed5:	09c434        	ae_s16ip	a3, a4, 2
    1ed8:	059232        	l16si	a3, a2, 10
    1edb:	09c434        	ae_s16ip	a3, a4, 2
    1ede:	069232        	l16si	a3, a2, 12
    1ee1:	09c434        	ae_s16ip	a3, a4, 2
    1ee4:	079232        	l16si	a3, a2, 14
    1ee7:	09c434        	ae_s16ip	a3, a4, 2
    1eea:	10c222        	addi	a2, a2, 16
    1eed:	072d           	mov.n	a2, a7
    1eef:	f01d           	retw.n

00001ef1 <xt_memcpy+0x89>:
    1ef1:	020c           	movi.n	a2, 0
    1ef3:	f01d           	retw.n

00001ef5 <xt_memcpy+0x8d>:
    1ef5:	000000                                        ...

00001ef8 <_fini>:
    1ef8:	008136        	entry	a1, 64
    1efb:	f89681        	l32r	a8, 154 (170 <__do_global_dtors_aux>)
    1efe:	f03d           	nop.n
    1f00:	0008e0        	callx8	a8

00001f03 <_fini+0xb>:
    1f03:	f01d           	retw.n
