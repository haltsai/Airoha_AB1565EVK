
Build/lib/pisplit_cpd_position_independent.splitload.lib:     file format elf32-xtensa-le


Disassembly of section .text:

00000000 <_init-0x160>:
       0:	000001b4 00001c38 000026b4 00002614     ....8....&...&..
      10:	00000000 000025fc 00000000 000026b8     .....%.......&..
      20:	000026d0 000024c4 000019f4 000024dc     .&...$.......$..
      30:	00001a08 000024f4 00000448 0000250c     .....$..H....%..
      40:	00000cbc 00002524 000007c0 0000253c     ....$%......<%..
      50:	0000156c 00002554 00001ad0 00002564     l...T%......d%..
      60:	00001b90 00002574 00000c9c 00002598     ....t%.......%..
      70:	00000cac 000025bc 0000154c 000025dc     .....%..L....%..
      80:	00000268 00002620 00002650 00007fff     h... &..P&......
      90:	00004800 00002800 18000000 e8000000     .H...(..........
      a0:	000002d8 00001ef0 00001ed8 00000434     ............4...
      b0:	00000ccd 00001a50 00000324 00001a84     ....P...$.......
      c0:	20000000 00000348 00400000 7fff0000     ... H.....@.....
      d0:	000003d4 0000055c 00000c44 000008b0     ....\...D.......
      e0:	00800000 08000000 00001a10 7fffffff     ................
      f0:	00001000 40000000 00000b6c 3fffffff     .......@l......?
     100:	00040000 00000a68 00000bc8 00000b8c     ....h...........
     110:	0000038c 00000d4c 00000da8 00000e7c     ....L.......|...
     120:	00000f78 00000270 03d70a3d 0000fffd     x...p...=.......
     130:	00001284 00001408 00001d24 00001e64     ........$...d...
     140:	00004000 00001308 00001908 000008a8     .@..............
     150:	00002000 00002404 00002600 00000178     . ...$...&..x...

00000160 <_init>:
     160:	008136        	entry	a1, 64
     163:	ffa781        	l32r	a8, 0 (1b4 <frame_dummy>)
     166:	f03d           	nop.n
     168:	0008e0        	callx8	a8
     16b:	ffa681        	l32r	a8, 4 (1c38 <__do_global_ctors_aux>)
     16e:	f03d           	nop.n
     170:	0008e0        	callx8	a8
     173:	f01d           	retw.n

00000175 <_init+0x15>:
     175:	000000                                        ...

00000178 <__do_global_dtors_aux>:
     178:	004136        	entry	a1, 32
     17b:	ffa331        	l32r	a3, 8 (26b4 <completed$3521_6_3>)
     17e:	000382        	l8ui	a8, a3, 0
     181:	d8ec           	bnez.n	a8, 1b2 <__do_global_dtors_aux+0x3a>
     183:	ffa221        	l32r	a2, c (2614 <p$3519_6_1>)
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
     1a2:	ff9ca1        	l32r	a10, 14 (25fc <__EH_FRAME_BEGIN___44>)
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
     1bf:	ff95a1        	l32r	a10, 14 (25fc <__EH_FRAME_BEGIN___44>)
     1c2:	ff96b1        	l32r	a11, 1c (26b8 <object$3538_8_1>)
     1c5:	ff9481        	l32r	a8, 18 (0 <_text_start>)
     1c8:	0008e0        	callx8	a8

000001cb <frame_dummy+0x17>:
     1cb:	f01d           	retw.n

000001cd <frame_dummy+0x19>:
     1cd:	000000                                        ...

000001d0 <_start>:
     1d0:	004136        	entry	a1, 32
     1d3:	ff94a1        	l32r	a10, 24 (24c4 <_DYNAMIC+0x52c>)
     1d6:	ff9231        	l32r	a3, 20 (26d0 <printf_ptr>)
     1d9:	0288           	l32i.n	a8, a2, 0
     1db:	ff93b1        	l32r	a11, 28 (19f4 <get_CPD_memsize>)
     1de:	0389           	s32i.n	a8, a3, 0
     1e0:	0008e0        	callx8	a8
     1e3:	0388           	l32i.n	a8, a3, 0
     1e5:	ff91a1        	l32r	a10, 2c (24dc <_DYNAMIC+0x544>)
     1e8:	ff92b1        	l32r	a11, 30 (1a08 <get_INS_memsize>)
     1eb:	0008e0        	callx8	a8
     1ee:	002382        	l32i	a8, a3, 0
     1f1:	ff90a1        	l32r	a10, 34 (24f4 <_DYNAMIC+0x55c>)
     1f4:	ff91b1        	l32r	a11, 38 (448 <compander_VO_init>)
     1f7:	0008e0        	callx8	a8
     1fa:	0388           	l32i.n	a8, a3, 0
     1fc:	ff90a1        	l32r	a10, 3c (250c <_DYNAMIC+0x574>)
     1ff:	ff90b1        	l32r	a11, 40 (cbc <compander_AU_init>)
     202:	0008e0        	callx8	a8
     205:	0388           	l32i.n	a8, a3, 0
     207:	ff8fa1        	l32r	a10, 44 (2524 <_DYNAMIC+0x58c>)
     20a:	ff8fb1        	l32r	a11, 48 (7c0 <compander_VO_proc>)
     20d:	0008e0        	callx8	a8
     210:	0388           	l32i.n	a8, a3, 0
     212:	ff8ea1        	l32r	a10, 4c (253c <_DYNAMIC+0x5a4>)
     215:	ff8eb1        	l32r	a11, 50 (156c <compander_AU_proc>)
     218:	0008e0        	callx8	a8
     21b:	0388           	l32i.n	a8, a3, 0
     21d:	ff8da1        	l32r	a10, 54 (2554 <_DYNAMIC+0x5bc>)
     220:	ff8eb1        	l32r	a11, 58 (1ad0 <INS_Init>)
     223:	0008e0        	callx8	a8
     226:	002382        	l32i	a8, a3, 0
     229:	ff8ca1        	l32r	a10, 5c (2564 <_DYNAMIC+0x5cc>)
     22c:	ff8db1        	l32r	a11, 60 (1b90 <INS_Prcs>)
     22f:	0008e0        	callx8	a8
     232:	0388           	l32i.n	a8, a3, 0
     234:	ff8ca1        	l32r	a10, 64 (2574 <_DYNAMIC+0x5dc>)
     237:	ff8cb1        	l32r	a11, 68 (c9c <compander_AU_SetFrame120_init>)
     23a:	0008e0        	callx8	a8
     23d:	0388           	l32i.n	a8, a3, 0
     23f:	ff8ba1        	l32r	a10, 6c (2598 <_DYNAMIC+0x600>)
     242:	ff8bb1        	l32r	a11, 70 (cac <compander_AU_SetFrame8_init>)
     245:	0008e0        	callx8	a8
     248:	0388           	l32i.n	a8, a3, 0
     24a:	ff8aa1        	l32r	a10, 74 (25bc <_DYNAMIC+0x624>)
     24d:	ff8ab1        	l32r	a11, 78 (154c <compander_AU_SetINS_proc>)
     250:	0008e0        	callx8	a8
     253:	0388           	l32i.n	a8, a3, 0
     255:	ff89a1        	l32r	a10, 7c (25dc <_DYNAMIC+0x644>)
     258:	ff8ab1        	l32r	a11, 80 (268 <SVN_version>)
     25b:	0008e0        	callx8	a8
     25e:	ff8921        	l32r	a2, 84 (2620 <export_parameter_array>)
     261:	f01d           	retw.n

00000263 <_start+0x93>:
     263:	00000000 00413600                                .....

00000268 <SVN_version>:
     268:	004136        	entry	a1, 32
     26b:	29a422        	movi	a2, 0x429
     26e:	f01d           	retw.n

00000270 <HIFI_sqrt_cpd>:
     270:	004136        	entry	a1, 32
     273:	0358           	l32i.n	a5, a3, 0
     275:	0a0c           	movi.n	a10, 0
     277:	ff8491        	l32r	a9, 88 (2650 <TABLE_SQRT>)
     27a:	5215a6        	blti	a5, 1, 2d0 <HIFI_sqrt_cpd+0x60>
     27d:	0278           	l32i.n	a7, a2, 0
     27f:	40e580        	nsa	a8, a5
     282:	401800        	ssl	a8
     285:	c07780        	sub	a7, a7, a8
     288:	a1b500        	sll	a11, a5
     28b:	046070        	extui	a6, a7, 0, 1
     28e:	400600        	ssr	a6
     291:	b1b0b0        	sra	a11, a11
     294:	41b9b0        	srli	a11, a11, 9
     297:	3180b0        	srai	a8, a11, 16
     29a:	f0c882        	addi	a8, a8, -16
     29d:	5388a0        	max	a8, a8, a10
     2a0:	908890        	addx2	a8, a8, a9
     2a3:	289d           	ae_l16si.n	a9, a8, 0
     2a5:	388d           	ae_l16si.n	a8, a8, 2
     2a7:	301bb4        	ae_cvtp24a16x2.ll	aep1, a11, a11
     2aa:	c08890        	sub	a8, a8, a9
     2ad:	119900        	slli	a9, a9, 16
     2b0:	118800        	slli	a8, a8, 16
     2b3:	364834        	ae_cvtq48a32s	aeq1, a8
     2b6:	360934        	ae_cvtq48a32s	aeq0, a9
     2b9:	031174        	ae_mulaq32sp16u.l	aeq0, aeq1, aep1
     2bc:	f86390667f 	{ add	a6, a6, a7; ae_roundsp16q48asym	aep0, aeq0 }
     2c1:	360424        	ae_trunca16p24s.l	a4, aep0
     2c4:	216160        	srai	a6, a6, 1
     2c7:	0269           	s32i.n	a6, a2, 0
     2c9:	114400        	slli	a4, a4, 16
     2cc:	0349           	s32i.n	a4, a3, 0
     2ce:	f01d           	retw.n

000002d0 <HIFI_sqrt_cpd+0x60>:
     2d0:	02a9           	s32i.n	a10, a2, 0
     2d2:	03a9           	s32i.n	a10, a3, 0
     2d4:	f01d           	retw.n

000002d6 <HIFI_sqrt_cpd+0x66>:
	...

000002d8 <Xdiv>:
     2d8:	004136        	entry	a1, 32
     2db:	0248           	l32i.n	a4, a2, 0
     2dd:	0358           	l32i.n	a5, a3, 0
     2df:	04bc           	beqz.n	a4, 313 <Xdiv+0x3b>
     2e1:	85bc           	beqz.n	a5, 31d <Xdiv+0x45>
     2e3:	60c204        	ae_lq32f.i	aeq3, a2, 0
     2e6:	608304        	ae_lq32f.i	aeq2, a3, 0
     2e9:	366904        	ae_nsaq56s	a9, aeq2
     2ec:	367804        	ae_nsaq56s	a8, aeq3
     2ef:	438890        	min	a8, a8, a9
     2f2:	f8c892        	addi	a9, a8, -8
     2f5:	fc24feb90f 	{ ae_slaasq56s	aeq0, aeq3, a9; nop }
     2fa:	e9c882        	addi	a8, a8, -23
     2fd:	fc24fea84f 	{ ae_slaasq56s	aeq1, aeq2, a8; nop }
     302:	369204        	ae_trunca32q48	a2, aeq1
     305:	368404        	ae_trunca32q48	a4, aeq0
     308:	528c           	beqz.n	a2, 311 <Xdiv+0x39>
     30a:	d22420        	quos	a2, a4, a2
     30d:	a22d           	ae_clamps16	a2, a2
     30f:	f01d           	retw.n

00000311 <Xdiv+0x39>:
     311:	f01d           	retw.n

00000313 <Xdiv+0x3b>:
     313:	ff5e81        	l32r	a8, 8c (7fff <_end+0x592b>)
     316:	020c           	movi.n	a2, 0
     318:	832850        	moveqz	a2, a8, a5
     31b:	f01d           	retw.n

0000031d <Xdiv+0x45>:
     31d:	ff5b21        	l32r	a2, 8c (7fff <_end+0x592b>)
     320:	f01d           	retw.n

00000322 <Xdiv+0x4a>:
	...

00000324 <linear_interp>:
     324:	006136        	entry	a1, 48
     327:	c08340        	sub	a8, a3, a4
     32a:	612d           	ae_s16i.n	a2, a1, 0
     32c:	400104        	ae_lp16f.i	aep0, a1, 0
     32f:	36c834        	ae_cvtq48a32s	aeq3, a8
     332:	d7e37ec4bf 	{ ae_cvtq48a32s	aeq2, a4; ae_mulfq32sp16s.l	aeq3, aeq3, aep0 }
     337:	35f094        	ae_slliq56	aeq3, aeq3, 2
     33a:	066b04        	ae_addsq56s	aeq1, aeq2, aeq3
     33d:	061934        	ae_roundsq32asym	aeq0, aeq1
     340:	368204        	ae_trunca32q48	a2, aeq0
     343:	f01d           	retw.n

00000345 <linear_interp+0x21>:
     345:	000000                                        ...

00000348 <INS_coef_init>:
     348:	004136        	entry	a1, 32
     34b:	ff51b1        	l32r	a11, 90 (4800 <_end+0x212c>)
     34e:	ff51a1        	l32r	a10, 94 (2800 <_end+0x12c>)
     351:	ff5241        	l32r	a4, 9c (e8000000 <_end+0xe7ffd92c>)
     354:	1cc282        	addi	a8, a2, 28
     357:	01d232        	addmi	a3, a2, 0x100
     35a:	404804        	ae_lp16f.i	aep4, a8, 0
     35d:	ff4e91        	l32r	a9, 98 (18000000 <_end+0x17ffd92c>)
     360:	364934        	ae_cvtq48a32s	aeq1, a9
     363:	ff0c58babf 	{ ae_cvtp24a16x2.ll	aep3, a10, a11; ae_movq56	aeq3, aeq1 }
     368:	ed8e7ec4bf 	{ ae_cvtq48a32s	aeq2, a4; ae_mulsfs32p16s.hh	aeq3, aep3, aep4 }
     36d:	00b424        	ae_mulafs32p16s.lh	aeq2, aep3, aep4
     370:	ec8f48e33e 	{ addi	a3, a3, -114; ae_mulsfs32p16s.lh	aeq1, aep3, aep4 }
     375:	f97bbae01f 	{ ae_sllisq56s	aeq0, aeq2, 1; ae_roundsp16q48asym	aep2, aeq3 }
     37a:	f86bf2230f 	{ ae_sp16f.l.i	aep2, a3, 0; ae_roundsp16q48asym	aep1, aeq1 }
     37f:	f863b2132f 	{ ae_sp16f.l.i	aep1, a3, 4; ae_roundsp16q48asym	aep0, aeq0 }
     384:	420314        	ae_sp16f.l.i	aep0, a3, 2
     387:	f01d           	retw.n

00000389 <INS_coef_init+0x41>:
     389:	000000                                        ...

0000038c <AU_INS_coef_init>:
     38c:	004136        	entry	a1, 32
     38f:	ff40a1        	l32r	a10, 90 (4800 <_end+0x212c>)
     392:	ff4241        	l32r	a4, 9c (e8000000 <_end+0xe7ffd92c>)
     395:	360434        	ae_cvtq48a32s	aeq0, a4
     398:	01d232        	addmi	a3, a2, 0x100
     39b:	ff3f81        	l32r	a8, 98 (18000000 <_end+0x17ffd92c>)
     39e:	ff3d91        	l32r	a9, 94 (2800 <_end+0x12c>)
     3a1:	3009a4        	ae_cvtp24a16x2.ll	aep0, a9, a10
     3a4:	36c834        	ae_cvtq48a32s	aeq3, a8
     3a7:	84c352        	addi	a5, a3, -124
     3aa:	604504        	ae_lq32f.i	aeq1, a5, 0
     3ad:	355414        	ae_slliq56	aeq1, aeq1, 16
     3b0:	f4c3bf31bf 	{ ae_movq56	aeq2, aeq3; ae_mulafq32sp24s.l	aeq0, aeq1, aep0 }
     3b5:	088064        	ae_mulsfq32sp24s.h	aeq2, aeq1, aep0
     3b8:	088964        	ae_mulsfq32sp24s.l	aeq3, aeq1, aep0
     3bb:	fd3449033e 	{ addi	a3, a3, -112; ae_roundsq32asym	aeq1, aeq2 }
     3c0:	621304        	ae_sq32f.i	aeq1, a3, 0
     3c3:	fe3c7ac05f 	{ ae_sllisq56s	aeq1, aeq0, 1; ae_roundsq32asym	aeq2, aeq3 }
     3c8:	fc2c79a32f 	{ ae_sq32f.i	aeq2, a3, 8; ae_roundsq32asym	aeq0, aeq1 }
     3cd:	620314        	ae_sq32f.i	aeq0, a3, 4
     3d0:	f01d           	retw.n

000003d2 <AU_INS_coef_init+0x46>:
	...

000003d4 <INS_inter_gain>:
     3d4:	006136        	entry	a1, 48
     3d7:	01ad           	mov.n	a10, a1
     3d9:	0d9292        	l16si	a9, a2, 26
     3dc:	4cc2b2        	addi	a11, a2, 76
     3df:	ff3081        	l32r	a8, a0 (2d8 <Xdiv>)
     3e2:	119900        	slli	a9, a9, 16
     3e5:	0199           	s32i.n	a9, a1, 0
     3e7:	0008e0        	callx8	a8
     3ea:	b14b           	addi.n	a11, a1, 4
     3ec:	ff2d81        	l32r	a8, a0 (2d8 <Xdiv>)
     3ef:	11aa00        	slli	a10, a10, 16
     3f2:	479292        	l16si	a9, a2, 142
     3f5:	11a9           	s32i.n	a10, a1, 4
     3f7:	01ad           	mov.n	a10, a1
     3f9:	119900        	slli	a9, a9, 16
     3fc:	0199           	s32i.n	a9, a1, 0
     3fe:	0008e0        	callx8	a8
     401:	4992c2        	l16si	a12, a2, 146
     404:	11b8           	l32i.n	a11, a1, 4
     406:	11da00        	slli	a13, a10, 16
     409:	11cc00        	slli	a12, a12, 16
     40c:	b2bbc0        	mulsh	a11, a11, a12
     40f:	4892c2        	l16si	a12, a2, 144
     412:	360d34        	ae_cvtq48a32s	aeq0, a13
     415:	11bbf0        	slli	a11, a11, 1
     418:	11cc00        	slli	a12, a12, 16
     41b:	364c34        	ae_cvtq48a32s	aeq1, a12
     41e:	fa667ecbff 	{ ae_cvtq48a32s	aeq3, a11; ae_addsq56s	aeq2, aeq0, aeq1 }
     423:	066b04        	ae_addsq56s	aeq1, aeq2, aeq3
     426:	361094        	ae_sllisq56s	aeq0, aeq1, 2
     429:	1b8f04        	ae_roundsp16q48asym	aep0, aeq0
     42c:	0082d4        	ae_cvta32p24.h	a2, aep0
     42f:	f01d           	retw.n

00000431 <INS_inter_gain+0x5d>:
     431:	000000                                        ...

00000434 <compander_VO_PARA_init>:
     434:	004136        	entry	a1, 32
     437:	03ad           	mov.n	a10, a3
     439:	ff1a81        	l32r	a8, a4 (1ef0 <xt_memcpy>)
     43c:	b24b           	addi.n	a11, a2, 4
     43e:	0c1c           	movi.n	a12, 16
     440:	0008e0        	callx8	a8
     443:	f01d           	retw.n

00000445 <compander_VO_PARA_init+0x11>:
     445:	000000                                        ...

00000448 <compander_VO_init>:
     448:	004136        	entry	a1, 32
     44b:	22c3b2        	addi	a11, a3, 34
     44e:	4c0c           	movi.n	a12, 4
     450:	ff1581        	l32r	a8, a4 (1ef0 <xt_memcpy>)
     453:	01d2a2        	addmi	a10, a2, 0x100
     456:	38caa2        	addi	a10, a10, 56
     459:	0008e0        	callx8	a8
     45c:	02ad           	mov.n	a10, a2
     45e:	ff1281        	l32r	a8, a8 (1ed8 <xt_memset>)
     461:	0b0c           	movi.n	a11, 0
     463:	9ca0c2        	movi	a12, 156
     466:	0008e0        	callx8	a8
     469:	ff1081        	l32r	a8, ac (434 <compander_VO_PARA_init>)
     46c:	03ad           	mov.n	a10, a3
     46e:	02bd           	mov.n	a11, a2
     470:	0008e0        	callx8	a8
     473:	4a1c           	movi.n	a10, 20
     475:	ad0c           	movi.n	a13, 10
     477:	ff0e91        	l32r	a9, b0 (ccd <compander_AU_init+0x11>)
     47a:	34bc           	beqz.n	a4, 4b1 <compander_VO_init+0x69>
     47c:	8b2c           	movi.n	a11, 40
     47e:	66a6c2        	movi	a12, 0x666
     481:	e40b           	addi.n	a14, a4, -1
     483:	0a4e16        	beqz	a14, 52b <compander_VO_init+0xe3>
     486:	33a3f2        	movi	a15, 0x333
     489:	0e5c           	movi.n	a14, 80
     48b:	fec482        	addi	a8, a4, -2
     48e:	0a6816        	beqz	a8, 538 <compander_VO_init+0xf0>
     491:	1c4426        	beqi	a4, 4, 4b1 <compander_VO_init+0x69>
     494:	fbc492        	addi	a9, a4, -5
     497:	0a8916        	beqz	a9, 543 <compander_VO_init+0xfb>
     49a:	fac4d2        	addi	a13, a4, -6
     49d:	0afd16        	beqz	a13, 550 <compander_VO_init+0x108>
     4a0:	1594a7        	bne	a4, a10, 4b9 <compander_VO_init+0x71>
     4a3:	1852a2        	s16i	a10, a2, 48
     4a6:	f2b9           	s32i.n	a11, a2, 60
     4a8:	1952c2        	s16i	a12, a2, 50
     4ab:	000286        	j	4b9 <compander_VO_init+0x71>

000004ae <compander_VO_init+0x66>:
     4ae:	000000                                        ...

000004b1 <compander_VO_init+0x69>:
     4b1:	1852d2        	s16i	a13, a2, 48
     4b4:	f2a9           	s32i.n	a10, a2, 60
     4b6:	195292        	s16i	a9, a2, 50

000004b9 <compander_VO_init+0x71>:
     4b9:	12a8           	l32i.n	a10, a2, 4
     4bb:	32c8           	l32i.n	a12, a2, 12
     4bd:	22b8           	l32i.n	a11, a2, 8
     4bf:	022ca7        	blt	a12, a10, 4c5 <compander_VO_init+0x7d>
     4c2:	07aba7        	bge	a11, a10, 4cd <compander_VO_init+0x85>

000004c5 <compander_VO_init+0x7d>:
     4c5:	32a9           	s32i.n	a10, a2, 12
     4c7:	22a9           	s32i.n	a10, a2, 8
     4c9:	0acd           	mov.n	a12, a10
     4cb:	0abd           	mov.n	a11, a10

000004cd <compander_VO_init+0x85>:
     4cd:	fef981        	l32r	a8, b4 (1a50 <calc_slope_1>)
     4d0:	0008e0        	callx8	a8
     4d3:	52b8           	l32i.n	a11, a2, 20
     4d5:	4252a2        	s16i	a10, a2, 132
     4d8:	fef881        	l32r	a8, b8 (324 <linear_interp>)
     4db:	12c8           	l32i.n	a12, a2, 4
     4dd:	8aad           	ae_sext16	a10, a10
     4df:	0008e0        	callx8	a8
     4e2:	fef681        	l32r	a8, bc (1a84 <calc_slope_2>)
     4e5:	42b8           	l32i.n	a11, a2, 16
     4e7:	22c8           	l32i.n	a12, a2, 8
     4e9:	0008e0        	callx8	a8
     4ec:	105242        	s16i	a4, a2, 32
     4ef:	fef581        	l32r	a8, c4 (348 <INS_coef_init>)
     4f2:	4352a2        	s16i	a10, a2, 134
     4f5:	fef291        	l32r	a9, c0 (20000000 <_end+0x1fffd92c>)
     4f8:	0b0c           	movi.n	a11, 0
     4fa:	1752b2        	s16i	a11, a2, 46
     4fd:	1662b2        	s32i	a11, a2, 88
     500:	1362b2        	s32i	a11, a2, 76
     503:	1652b2        	s16i	a11, a2, 44
     506:	126292        	s32i	a9, a2, 72
     509:	01d2a2        	addmi	a10, a2, 0x100
     50c:	98caa2        	addi	a10, a10, -104
     50f:	d2a9           	s32i.n	a10, a2, 52
     511:	e2a9           	s32i.n	a10, a2, 56
     513:	02ad           	mov.n	a10, a2
     515:	0008e0        	callx8	a8
     518:	eca1c2        	movi	a12, 0x1ec
     51b:	feebd1        	l32r	a13, c8 (400000 <_end+0x3fd92c>)
     51e:	0003e2        	l8ui	a14, a3, 0
     521:	1252e2        	s16i	a14, a2, 36
     524:	a2d9           	s32i.n	a13, a2, 40
     526:	1352c2        	s16i	a12, a2, 38
     529:	f01d           	retw.n

0000052b <compander_VO_init+0xe3>:
     52b:	1852a2        	s16i	a10, a2, 48
     52e:	f2b9           	s32i.n	a11, a2, 60
     530:	1952c2        	s16i	a12, a2, 50
     533:	ffe086        	j	4b9 <compander_VO_init+0x71>

00000536 <compander_VO_init+0xee>:
	...

00000538 <compander_VO_init+0xf0>:
     538:	1852b2        	s16i	a11, a2, 48
     53b:	f2e9           	s32i.n	a14, a2, 60
     53d:	1952f2        	s16i	a15, a2, 50
     540:	ffdd46        	j	4b9 <compander_VO_init+0x71>

00000543 <compander_VO_init+0xfb>:
     543:	1852a2        	s16i	a10, a2, 48
     546:	f2b9           	s32i.n	a11, a2, 60
     548:	1952c2        	s16i	a12, a2, 50
     54b:	ffda86        	j	4b9 <compander_VO_init+0x71>

0000054e <compander_VO_init+0x106>:
	...

00000550 <compander_VO_init+0x108>:
     550:	1852b2        	s16i	a11, a2, 48
     553:	f2e9           	s32i.n	a14, a2, 60
     555:	1952f2        	s16i	a15, a2, 50
     558:	ffd746        	j	4b9 <compander_VO_init+0x71>

0000055b <compander_VO_init+0x113>:
	...

0000055c <cpd_block_proc>:
     55c:	006136        	entry	a1, 48
     55f:	32c242        	addi	a4, a2, 50
     562:	fec352        	addi	a5, a3, -2
     565:	1892a2        	l16si	a10, a2, 48
     568:	fc2493855f 	{ or	a8, a5, a5; ae_zeroq56	aeq0 }
     56d:	fc20ec240f 	{ ae_lp16f.i	aep2, a4, 0; ae_zerop48	aep1 }
     572:	1b1aa6        	blti	a10, 1, 591 <cpd_block_proc+0x35>
     575:	408814        	ae_lp16f.iu	aep0, a8, 2
     578:	079804        	ae_abssp24s	aep0, aep0
     57b:	e20a0ffa9e 	{ addi	a9, a10, -1; ae_mulafp24s.hh	aeq0, aep2, aep0 }
     580:	0aa976        	loopgtz	a9, 58e <cpd_block_proc+0x32>
     583:	d8066c881f 	{ ae_lp16f.iu	aep0, a8, 2; ae_maxp24s	aep1, aep1, aep0 }
     588:	079804        	ae_abssp24s	aep0, aep0
     58b:	012004        	ae_mulafp24s.hh	aeq0, aep2, aep0

0000058e <cpd_block_proc+0x32>:
     58e:	071814        	ae_maxp24s	aep1, aep1, aep0

00000591 <cpd_block_proc+0x35>:
     591:	4cc2f2        	addi	a15, a2, 76
     594:	ff247d4f0f 	{ ae_lq32f.i	aeq2, a15, 0; ae_roundsq32asym	aeq3, aeq0 }
     599:	067a14        	ae_subsq56s	aeq1, aeq3, aeq2
     59c:	3550f4        	ae_sraiq56	aeq1, aeq1, 3
     59f:	06d934        	ae_roundsq32asym	aeq3, aeq1
     5a2:	f9f64282ee 	{ addi	a14, a2, 40; ae_addsq56s	aeq1, aeq2, aeq3 }
     5a7:	621f04        	ae_sq32f.i	aeq1, a15, 0
     5aa:	60ce04        	ae_lq32f.i	aeq3, a14, 0
     5ad:	039ad4        	ae_mulfq32sp16s.l	aeq3, aeq3, aep1
     5b0:	fe3c7052de 	{ l32i	a13, a2, 20; ae_roundsq32asym	aeq2, aeq3 }
     5b5:	622104        	ae_sq32f.i	aeq2, a1, 0
     5b8:	0021c2        	l32i	a12, a1, 0
     5bb:	01adc7        	bge	a13, a12, 5c0 <cpd_block_proc+0x64>
     5be:	52c9           	s32i.n	a12, a2, 20

000005c0 <cpd_block_proc+0x64>:
     5c0:	169282        	l16si	a8, a2, 44
     5c3:	0c9292        	l16si	a9, a2, 24
     5c6:	c09980        	sub	a9, a9, a8
     5c9:	11e956        	bnez	a9, 6eb <cpd_block_proc+0x18f>
     5cc:	1722b2        	l32i	a11, a2, 92
     5cf:	1d0c           	movi.n	a13, 1
     5d1:	1652d2        	s16i	a13, a2, 44
     5d4:	53bbc0        	max	a11, a11, a12
     5d7:	11b9           	s32i.n	a11, a1, 4
     5d9:	1822d2        	l32i	a13, a2, 96
     5dc:	60c282        	addi	a8, a2, 96
     5df:	690c           	movi.n	a9, 6
     5e1:	53bbd0        	max	a11, a11, a13
     5e4:	11b9           	s32i.n	a11, a1, 4
     5e6:	5cc2b2        	addi	a11, a2, 92
     5e9:	1822a2        	l32i	a10, a2, 96
     5ec:	09eba4        	ae_s32ip	a10, a11, 4
     5ef:	0f8976        	loop	a9, 602 <cpd_block_proc+0xa6>
     5f2:	1898           	l32i.n	a9, a8, 4
     5f4:	11a8           	l32i.n	a10, a1, 4
     5f6:	884b           	addi.n	a8, a8, 4
     5f8:	539a90        	max	a9, a10, a9
     5fb:	1199           	s32i.n	a9, a1, 4
     5fd:	0898           	l32i.n	a9, a8, 0
     5ff:	09eb94        	ae_s32ip	a9, a11, 4

00000602 <cpd_block_proc+0xa6>:
     602:	0121e2        	l32i	a14, a1, 4
     605:	1662e2        	s32i	a14, a2, 88
     608:	006bc2        	s32i	a12, a11, 0
     60b:	1622b2        	l32i	a11, a2, 88

0000060e <cpd_block_proc+0xb2>:
     60e:	12c8           	l32i.n	a12, a2, 4
     610:	122262        	l32i	a6, a2, 72
     613:	0020f0        	nop
     616:	02acb7        	bge	a12, a11, 61c <cpd_block_proc+0xc0>
     619:	003a06        	j	705 <cpd_block_proc+0x1a9>
     61c:	1ac282        	addi	a8, a2, 26
     61f:	403804        	ae_lp16f.i	aep3, a8, 0
     622:	0bb0a4        	ae_sllisp24s	aep2, aep3, 1
     625:	07a314        	ae_addsp24s	aep1, aep2, aep3
     628:	001fd4        	ae_cvta32p24.l	a15, aep1
     62b:	11f9           	s32i.n	a15, a1, 4
     62d:	132282        	l32i	a8, a2, 76
     630:	02a8f7        	bge	a8, a15, 636 <cpd_block_proc+0xda>
     633:	002746        	j	6d4 <cpd_block_proc+0x178>
     636:	fea5a1        	l32r	a10, cc (7fff0000 <_end+0x7ffed92c>)
     639:	0161a2        	s32i	a10, a1, 4

0000063c <cpd_block_proc+0xe0>:
     63c:	1ec2b2        	addi	a11, a2, 30
     63f:	48c2c2        	addi	a12, a2, 72
     642:	21d2a0        	srai	a13, a10, 2
     645:	368d34        	ae_cvtq48a32s	aeq2, a13
     648:	11d9           	s32i.n	a13, a1, 4
     64a:	600c04        	ae_lq32f.i	aeq0, a12, 0
     64d:	f952ac4b0f 	{ ae_lp16f.i	aep4, a11, 0; ae_subsq56s	aeq1, aeq2, aeq0 }
     652:	034054        	ae_mulafq32sp16s.l	aeq0, aeq1, aep4
     655:	060934        	ae_roundsq32asym	aeq0, aeq0
     658:	368904        	ae_trunca32q48	a9, aeq0
     65b:	126292        	s32i	a9, a2, 72

0000065e <cpd_block_proc+0x102>:
     65e:	058d           	mov.n	a8, a5
     660:	2e0c           	movi.n	a14, 2
     662:	1892f2        	l16si	a15, a2, 48
     665:	f298           	l32i.n	a9, a2, 60
     667:	e2a8           	l32i.n	a10, a2, 56
     669:	405404        	ae_lp16f.i	aep5, a4, 0
     66c:	364634        	ae_cvtq48a32s	aeq1, a6
     66f:	d2c8           	l32i.n	a12, a2, 52
     671:	f3f6c0        	wur.ae_cbegin0	a12
     674:	fb46b0a2de 	{ l32i	a13, a2, 40; ae_subsq56s	aeq3, aeq0, aeq1 }
     679:	d7f76b9ddf 	{ ae_truncp24a32x2	aep1, a13, a13; ae_mulfq32sp16s.l	aeq3, aeq3, aep5 }
     67e:	0abd           	mov.n	a11, a10
     680:	ff3c5199cf 	{ addx2	a9, a9, a12; ae_roundsq32asym	aeq3, aeq3 }
     685:	f3f790        	wur.ae_cend0	a9
     688:	2c2fa6        	blti	a15, 2, 6b8 <cpd_block_proc+0x15c>
     68b:	1c0c           	movi.n	a12, 1
     68d:	228076        	loop	a0, 6b3 <cpd_block_proc+0x157>
     690:	f9ee597bef 	{ ae_lp16f.c	aep7, a11, a14; ae_addsq56s	aeq1, aeq1, aeq3 }
     695:	03f254        	ae_mulfq32sp16s.l	aeq2, aeq1, aep7
     698:	fb8420039e 	{ l16si	a9, a3, 0; ae_mulfq32sp24s.l	aeq2, aeq2, aep1 }
     69d:	09ca94        	ae_s16ip	a9, a10, 2
     6a0:	35a194        	ae_slliq56	aeq2, aeq2, 6
     6a3:	1baf64        	ae_roundsp16q48asym	aep6, aeq2
     6a6:	42e814        	ae_sp16f.l.iu	aep6, a8, 2
     6a9:	1892f2        	l16si	a15, a2, 48
     6ac:	cc1b           	addi.n	a12, a12, 1
     6ae:	332b           	addi.n	a3, a3, 2
     6b0:	04acf7        	bge	a12, a15, 6b8 <cpd_block_proc+0x15c>

000006b3 <cpd_block_proc+0x157>:
     6b3:	fff586        	j	68d <cpd_block_proc+0x131>

000006b6 <cpd_block_proc+0x15a>:
	...

000006b8 <cpd_block_proc+0x15c>:
     6b8:	fc24d92bef 	{ ae_lp16f.c	aep2, a11, a14; nop }
     6bd:	03aa14        	ae_mulfq32sp16s.l	aeq3, aeq0, aep2
     6c0:	fb856003ce 	{ l16si	a12, a3, 0; ae_mulfq32sp24s.l	aeq3, aeq3, aep1 }
     6c5:	6acd           	ae_s16i.n	a12, a10, 0
     6c7:	35f194        	ae_slliq56	aeq3, aeq3, 6
     6ca:	1bbf04        	ae_roundsp16q48asym	aep0, aeq3
     6cd:	428814        	ae_sp16f.l.iu	aep0, a8, 2
     6d0:	e2b9           	s32i.n	a11, a2, 56
     6d2:	f01d           	retw.n

000006d4 <cpd_block_proc+0x178>:
     6d4:	0d92d2        	l16si	a13, a2, 26
     6d7:	11dd00        	slli	a13, a13, 16
     6da:	02ad87        	bge	a13, a8, 6e0 <cpd_block_proc+0x184>
     6dd:	003446        	j	7b2 <cpd_block_proc+0x256>
     6e0:	0e92a2        	l16si	a10, a2, 28
     6e3:	11aa00        	slli	a10, a10, 16
     6e6:	11a9           	s32i.n	a10, a1, 4
     6e8:	ffd406        	j	63c <cpd_block_proc+0xe0>

000006eb <cpd_block_proc+0x18f>:
     6eb:	1e22d2        	l32i	a13, a2, 120
     6ee:	1622b2        	l32i	a11, a2, 88
     6f1:	e81b           	addi.n	a14, a8, 1
     6f3:	1652e2        	s16i	a14, a2, 44
     6f6:	53bbc0        	max	a11, a11, a12
     6f9:	53ddc0        	max	a13, a13, a12
     6fc:	1e62d2        	s32i	a13, a2, 120
     6ff:	1662b2        	s32i	a11, a2, 88
     702:	ffc206        	j	60e <cpd_block_proc+0xb2>

00000705 <cpd_block_proc+0x1a9>:
     705:	fe6c81        	l32r	a8, b8 (324 <linear_interp>)
     708:	4292a2        	l16si	a10, a2, 132
     70b:	0008e0        	callx8	a8
     70e:	21a9           	s32i.n	a10, a1, 8
     710:	2298           	l32i.n	a9, a2, 8
     712:	58c272        	addi	a7, a2, 88
     715:	17aa97        	bge	a10, a9, 730 <cpd_block_proc+0x1d4>
     718:	07bd           	mov.n	a11, a7
     71a:	2192a0        	srai	a9, a10, 2
     71d:	fe6081        	l32r	a8, a0 (2d8 <Xdiv>)
     720:	2199           	s32i.n	a9, a1, 8
     722:	08c1a2        	addi	a10, a1, 8
     725:	0008e0        	callx8	a8
     728:	11aa00        	slli	a10, a10, 16
     72b:	11a9           	s32i.n	a10, a1, 4
     72d:	0015c6        	j	788 <cpd_block_proc+0x22c>

00000730 <cpd_block_proc+0x1d4>:
     730:	4292a2        	l16si	a10, a2, 132
     733:	fe6181        	l32r	a8, b8 (324 <linear_interp>)
     736:	52b8           	l32i.n	a11, a2, 20
     738:	12c8           	l32i.n	a12, a2, 4
     73a:	0008e0        	callx8	a8
     73d:	fe5f81        	l32r	a8, bc (1a84 <calc_slope_2>)
     740:	0422b2        	l32i	a11, a2, 16
     743:	0222c2        	l32i	a12, a2, 8
     746:	0008e0        	callx8	a8
     749:	21b8           	l32i.n	a11, a1, 8
     74b:	4352a2        	s16i	a10, a2, 134
     74e:	fe5a81        	l32r	a8, b8 (324 <linear_interp>)
     751:	22c8           	l32i.n	a12, a2, 8
     753:	8aad           	ae_sext16	a10, a10
     755:	0008e0        	callx8	a8
     758:	b14b           	addi.n	a11, a1, 4
     75a:	11a9           	s32i.n	a10, a1, 4
     75c:	2298           	l32i.n	a9, a2, 8
     75e:	fe5081        	l32r	a8, a0 (2d8 <Xdiv>)
     761:	a1cb           	addi.n	a10, a1, 12
     763:	219290        	srai	a9, a9, 2
     766:	3199           	s32i.n	a9, a1, 12
     768:	0008e0        	callx8	a8
     76b:	07bd           	mov.n	a11, a7
     76d:	2198           	l32i.n	a9, a1, 8
     76f:	31a9           	s32i.n	a10, a1, 12
     771:	fe4b81        	l32r	a8, a0 (2d8 <Xdiv>)
     774:	a14b           	addi.n	a10, a1, 4
     776:	219290        	srai	a9, a9, 2
     779:	1199           	s32i.n	a9, a1, 4
     77b:	0008e0        	callx8	a8
     77e:	31b8           	l32i.n	a11, a1, 12
     780:	d1aab0        	mul16s	a10, a10, a11
     783:	11aad0        	slli	a10, a10, 3
     786:	11a9           	s32i.n	a10, a1, 4

00000788 <cpd_block_proc+0x22c>:
     788:	122282        	l32i	a8, a2, 72
     78b:	06a8a7        	bge	a8, a10, 795 <cpd_block_proc+0x239>
     78e:	400204        	ae_lp16f.i	aep0, a2, 0
     791:	0000c6        	j	798 <cpd_block_proc+0x23c>

00000794 <cpd_block_proc+0x238>:
	...

00000795 <cpd_block_proc+0x239>:
     795:	400214        	ae_lp16f.i	aep0, a2, 2

00000798 <cpd_block_proc+0x23c>:
     798:	360834        	ae_cvtq48a32s	aeq0, a8
     79b:	c0da80        	sub	a13, a10, a8
     79e:	364d34        	ae_cvtq48a32s	aeq1, a13
     7a1:	030054        	ae_mulafq32sp16s.l	aeq0, aeq1, aep0
     7a4:	fc245411df 	{ s32i.n	a13, a1, 4; ae_roundsq32asym	aeq0, aeq0 }
     7a9:	368c04        	ae_trunca32q48	a12, aeq0
     7ac:	1262c2        	s32i	a12, a2, 72
     7af:	ffaac6        	j	65e <cpd_block_proc+0x102>

000007b2 <cpd_block_proc+0x256>:
     7b2:	fe4781        	l32r	a8, d0 (3d4 <INS_inter_gain>)
     7b5:	02ad           	mov.n	a10, a2
     7b7:	0008e0        	callx8	a8
     7ba:	11a9           	s32i.n	a10, a1, 4
     7bc:	ff9f06        	j	63c <cpd_block_proc+0xe0>

000007bf <cpd_block_proc+0x263>:
	...

000007c0 <compander_VO_proc>:
     7c0:	00a136        	entry	a1, 80
     7c3:	121282        	l16ui	a8, a2, 36
     7c6:	1cc1a2        	addi	a10, a1, 28
     7c9:	0e5152        	s16i	a5, a1, 28
     7cc:	0a0816        	beqz	a8, 870 <compander_VO_proc+0xb0>
     7cf:	109292        	l16si	a9, a2, 32
     7d2:	9c92b2        	l16si	a11, a2, 0x138
     7d5:	0f52b2        	s16i	a11, a2, 30
     7d8:	69bc           	beqz.n	a9, 812 <compander_VO_proc+0x52>
     7da:	341926        	beqi	a9, 1, 812 <compander_VO_proc+0x52>
     7dd:	312926        	beqi	a9, 2, 812 <compander_VO_proc+0x52>
     7e0:	3ea1b2        	movi	a11, 0x13e
     7e3:	024966        	bnei	a9, 4, 7e9 <compander_VO_proc+0x29>
     7e6:	002206        	j	872 <compander_VO_proc+0xb2>
     7e9:	025966        	bnei	a9, 5, 7ef <compander_VO_proc+0x2f>
     7ec:	002086        	j	872 <compander_VO_proc+0xb2>
     7ef:	7f6926        	beqi	a9, 6, 872 <compander_VO_proc+0xb2>
     7f2:	401a04        	ae_lp16f.i	aep1, a10, 0
     7f5:	01d2d2        	addmi	a13, a2, 0x100
     7f8:	9f92c2        	l16si	a12, a2, 0x13e
     7fb:	3ecdd2        	addi	a13, a13, 62
     7fe:	400d04        	ae_lp16f.i	aep0, a13, 0
     801:	094c16        	beqz	a12, 899 <compander_VO_proc+0xd9>
     804:	011844        	ae_mulfp24s.hh	aeq1, aep1, aep0
     807:	061934        	ae_roundsq32asym	aeq0, aeq1
     80a:	620104        	ae_sq32f.i	aeq0, a1, 0
     80d:	000706        	j	82d <compander_VO_proc+0x6d>

00000810 <compander_VO_proc+0x50>:
	...

00000812 <compander_VO_proc+0x52>:
     812:	9d92f2        	l16si	a15, a2, 0x13a
     815:	9e92e2        	l16si	a14, a2, 0x13c
     818:	269d           	ae_l16si.n	a9, a6, 0
     81a:	080c           	movi.n	a8, 0
     81c:	36ad           	ae_l16si.n	a10, a6, 2
     81e:	119900        	slli	a9, a9, 16
     821:	006192        	s32i	a9, a1, 0
     824:	7288a0        	salt	a8, a8, a10
     827:	93ef80        	movnez	a14, a15, a8
     82a:	0f52e2        	s16i	a14, a2, 30

0000082d <compander_VO_proc+0x6d>:
     82d:	189292        	l16si	a9, a2, 48
     830:	d25490        	quos	a5, a4, a9
     833:	3915a6        	blti	a5, 1, 870 <compander_VO_proc+0xb0>
     836:	040c           	movi.n	a4, 0
     838:	26c272        	addi	a7, a2, 38
     83b:	1169f0        	slli	a6, a9, 1
     83e:	28c2b2        	addi	a11, a2, 40
     841:	0161b2        	s32i	a11, a1, 4

00000844 <compander_VO_proc+0x84>:
     844:	fe2481        	l32r	a8, d4 (55c <cpd_block_proc>)
     847:	11b8           	l32i.n	a11, a1, 4
     849:	01a8           	l32i.n	a10, a1, 0
     84b:	364a34        	ae_cvtq48a32s	aeq1, a10
     84e:	60cb04        	ae_lq32f.i	aeq3, a11, 0
     851:	f8ceac070f 	{ ae_lp16f.i	aep0, a7, 0; ae_subsq56s	aeq0, aeq1, aeq3 }
     856:	038814        	ae_mulafq32sp16s.l	aeq3, aeq0, aep0
     859:	02ad           	mov.n	a10, a2
     85b:	fe3c53b33f 	{ or	a11, a3, a3; ae_roundsq32asym	aeq2, aeq3 }
     860:	36a904        	ae_trunca32q48	a9, aeq2
     863:	0a6292        	s32i	a9, a2, 40
     866:	0008e0        	callx8	a8
     869:	336a           	add.n	a3, a3, a6
     86b:	441b           	addi.n	a4, a4, 1
     86d:	d39547        	bne	a5, a4, 844 <compander_VO_proc+0x84>

00000870 <compander_VO_proc+0xb0>:
     870:	f01d           	retw.n

00000872 <compander_VO_proc+0xb2>:
     872:	4122b4        	ae_lp16f.x	aep2, a2, a11
     875:	401624        	ae_lp16f.i	aep1, a6, 4
     878:	cd1c           	movi.n	a13, 28
     87a:	9f92c2        	l16si	a12, a2, 0x13e
     87d:	4101d4        	ae_lp16f.x	aep0, a1, a13
     880:	e804c0029f 	{ movi	a9, 2; ae_mulfp24s.hh	aeq1, aep1, aep0 }
     885:	8c9c           	beqz.n	a12, 8a1 <compander_VO_proc+0xe1>
     887:	341934        	ae_sllasq56s	aeq0, aeq1, a9
     88a:	1b8d34        	ae_roundsp24q48asym	aep3, aeq0
     88d:	01ba44        	ae_mulfp24s.hh	aeq3, aep3, aep2
     890:	06b934        	ae_roundsq32asym	aeq2, aeq3
     893:	622104        	ae_sq32f.i	aeq2, a1, 0
     896:	ffe4c6        	j	82d <compander_VO_proc+0x6d>

00000899 <compander_VO_proc+0xd9>:
     899:	11e500        	slli	a14, a5, 16
     89c:	01e9           	s32i.n	a14, a1, 0
     89e:	ffe2c6        	j	82d <compander_VO_proc+0x6d>

000008a1 <compander_VO_proc+0xe1>:
     8a1:	019844        	ae_mulfp24s.hh	aeq3, aep1, aep0
     8a4:	34b934        	ae_sllasq56s	aeq2, aeq3, a9
     8a7:	622104        	ae_sq32f.i	aeq2, a1, 0
     8aa:	ffdfc6        	j	82d <compander_VO_proc+0x6d>

000008ad <compander_VO_proc+0xed>:
     8ad:	000000                                        ...

000008b0 <compander_AU_PARA_init>:
     8b0:	004136        	entry	a1, 32
     8b3:	fe0971        	l32r	a7, d8 (c44 <AU_GBL_init+0x7c>)
     8b6:	046416        	beqz	a4, 900 <compander_AU_PARA_init+0x50>
     8b9:	840b           	addi.n	a8, a4, -1
     8bb:	0b4816        	beqz	a8, 973 <compander_AU_PARA_init+0xc3>
     8be:	fec492        	addi	a9, a4, -2
     8c1:	11c916        	beqz	a9, 9e1 <compander_AU_PARA_init+0x131>

000008c4 <compander_AU_PARA_init+0x14>:
     8c4:	5c0c           	movi.n	a12, 5
     8c6:	02d2b2        	addmi	a11, a2, 0x200
     8c9:	01d3a2        	addmi	a10, a3, 0x100
     8cc:	fdf681        	l32r	a8, a4 (1ef0 <xt_memcpy>)
     8cf:	80caa2        	addi	a10, a10, -128
     8d2:	c8cbb2        	addi	a11, a11, -56
     8d5:	0008e0        	callx8	a8
     8d8:	5c0c           	movi.n	a12, 5
     8da:	66c222        	addi	a2, a2, 102
     8dd:	07d3a2        	addmi	a10, a3, 0x700
     8e0:	fdf181        	l32r	a8, a4 (1ef0 <xt_memcpy>)
     8e3:	20caa2        	addi	a10, a10, 32
     8e6:	02bd           	mov.n	a11, a2
     8e8:	0008e0        	callx8	a8
     8eb:	02bd           	mov.n	a11, a2
     8ed:	5c0c           	movi.n	a12, 5
     8ef:	fded81        	l32r	a8, a4 (1ef0 <xt_memcpy>)
     8f2:	0ed3a2        	addmi	a10, a3, 0xe00
     8f5:	c0caa2        	addi	a10, a10, -64
     8f8:	0008e0        	callx8	a8
     8fb:	498626        	beqi	a6, 8, 948 <compander_AU_PARA_init+0x98>
     8fe:	f01d           	retw.n

00000900 <compander_AU_PARA_init+0x50>:
     900:	114516        	beqz	a5, a18 <compander_AU_PARA_init+0x168>
     903:	03ad           	mov.n	a10, a3
     905:	fde781        	l32r	a8, a4 (1ef0 <xt_memcpy>)
     908:	34c2b2        	addi	a11, a2, 52
     90b:	4c0c           	movi.n	a12, 4
     90d:	0008e0        	callx8	a8

00000910 <compander_AU_PARA_init+0x60>:
     910:	a38b           	addi.n	a10, a3, 8
     912:	fde481        	l32r	a8, a4 (1ef0 <xt_memcpy>)
     915:	44c2b2        	addi	a11, a2, 68
     918:	9c0c           	movi.n	a12, 9
     91a:	0008e0        	callx8	a8
     91d:	a38666        	bnei	a6, 8, 8c4 <compander_AU_PARA_init+0x14>
     920:	5398           	l32i.n	a9, a3, 20
     922:	23c8           	l32i.n	a12, a3, 8
     924:	33b8           	l32i.n	a11, a3, 12
     926:	43a8           	l32i.n	a10, a3, 16
     928:	cc8b           	addi.n	a12, a12, 8
     92a:	bb8b           	addi.n	a11, a11, 8
     92c:	aa8b           	addi.n	a10, a10, 8
     92e:	998b           	addi.n	a9, a9, 8
     930:	219490        	srai	a9, a9, 4
     933:	21a4a0        	srai	a10, a10, 4
     936:	21b4b0        	srai	a11, a11, 4
     939:	21c4c0        	srai	a12, a12, 4
     93c:	23c9           	s32i.n	a12, a3, 8
     93e:	33b9           	s32i.n	a11, a3, 12
     940:	43a9           	s32i.n	a10, a3, 16
     942:	5399           	s32i.n	a9, a3, 20
     944:	ffdf06        	j	8c4 <compander_AU_PARA_init+0x14>

00000947 <compander_AU_PARA_init+0x97>:
	...

00000948 <compander_AU_PARA_init+0x98>:
     948:	202392        	l32i	a9, a3, 128
     94b:	05d382        	addmi	a8, a3, 0x500
     94e:	e37a           	add.n	a14, a3, a7
     950:	5f2ed2        	l32i	a13, a14, 0x17c
     953:	e8c882        	addi	a8, a8, -24
     956:	8e28f2        	l32i	a15, a8, 0x238
     959:	998b           	addi.n	a9, a9, 8
     95b:	219490        	srai	a9, a9, 4
     95e:	dd8b           	addi.n	a13, a13, 8
     960:	21d4d0        	srai	a13, a13, 4
     963:	206392        	s32i	a9, a3, 128
     966:	ff8b           	addi.n	a15, a15, 8
     968:	21f4f0        	srai	a15, a15, 4
     96b:	8e68f2        	s32i	a15, a8, 0x238
     96e:	5f6ed2        	s32i	a13, a14, 0x17c
     971:	f01d           	retw.n

00000973 <compander_AU_PARA_init+0xc3>:
     973:	07d3a2        	addmi	a10, a3, 0x700
     976:	a0caa2        	addi	a10, a10, -96
     979:	0cb516        	beqz	a5, a48 <compander_AU_PARA_init+0x198>
     97c:	b50b           	addi.n	a11, a5, -1
     97e:	0d6b16        	beqz	a11, a58 <compander_AU_PARA_init+0x1a8>
     981:	fec5c2        	addi	a12, a5, -2
     984:	0a0c16        	beqz	a12, a28 <compander_AU_PARA_init+0x178>
     987:	4c0c           	movi.n	a12, 4
     989:	fdc681        	l32r	a8, a4 (1ef0 <xt_memcpy>)
     98c:	02d2b2        	addmi	a11, a2, 0x200
     98f:	c0cbb2        	addi	a11, a11, -64
     992:	0008e0        	callx8	a8

00000995 <compander_AU_PARA_init+0xe5>:
     995:	22c2b2        	addi	a11, a2, 34
     998:	9c0c           	movi.n	a12, 9
     99a:	fdc281        	l32r	a8, a4 (1ef0 <xt_memcpy>)
     99d:	07d3a2        	addmi	a10, a3, 0x700
     9a0:	a8caa2        	addi	a10, a10, -88
     9a3:	0008e0        	callx8	a8
     9a6:	f8c692        	addi	a9, a6, -8
     9a9:	f17956        	bnez	a9, 8c4 <compander_AU_PARA_init+0x14>
     9ac:	05d3b2        	addmi	a11, a3, 0x500
     9af:	e8cbb2        	addi	a11, a11, -24
     9b2:	732ba2        	l32i	a10, a11, 0x1cc
     9b5:	702be2        	l32i	a14, a11, 0x1c0
     9b8:	712bd2        	l32i	a13, a11, 0x1c4
     9bb:	722bc2        	l32i	a12, a11, 0x1c8
     9be:	ee8b           	addi.n	a14, a14, 8
     9c0:	dd8b           	addi.n	a13, a13, 8
     9c2:	cc8b           	addi.n	a12, a12, 8
     9c4:	aa8b           	addi.n	a10, a10, 8
     9c6:	21a4a0        	srai	a10, a10, 4
     9c9:	21c4c0        	srai	a12, a12, 4
     9cc:	21d4d0        	srai	a13, a13, 4
     9cf:	21e4e0        	srai	a14, a14, 4
     9d2:	706be2        	s32i	a14, a11, 0x1c0
     9d5:	716bd2        	s32i	a13, a11, 0x1c4
     9d8:	726bc2        	s32i	a12, a11, 0x1c8
     9db:	736ba2        	s32i	a10, a11, 0x1cc
     9de:	ffb886        	j	8c4 <compander_AU_PARA_init+0x14>

000009e1 <compander_AU_PARA_init+0x131>:
     9e1:	0cd392        	addmi	a9, a3, 0xc00
     9e4:	050516        	beqz	a5, a38 <compander_AU_PARA_init+0x188>
     9e7:	2c92c2        	l16si	a12, a2, 88
     9ea:	2b92d2        	l16si	a13, a2, 86
     9ed:	a059d2        	s16i	a13, a9, 0x140
     9f0:	a359c2        	s16i	a12, a9, 0x146

000009f3 <compander_AU_PARA_init+0x143>:
     9f3:	5ec2b2        	addi	a11, a2, 94
     9f6:	4c0c           	movi.n	a12, 4
     9f8:	fdab81        	l32r	a8, a4 (1ef0 <xt_memcpy>)
     9fb:	0dd3a2        	addmi	a10, a3, 0xd00
     9fe:	48caa2        	addi	a10, a10, 72
     a01:	0008e0        	callx8	a8
     a04:	32c2b2        	addi	a11, a2, 50
     a07:	1c0c           	movi.n	a12, 1
     a09:	fda681        	l32r	a8, a4 (1ef0 <xt_memcpy>)
     a0c:	0dd3a2        	addmi	a10, a3, 0xd00
     a0f:	58caa2        	addi	a10, a10, 88
     a12:	0008e0        	callx8	a8
     a15:	ffaac6        	j	8c4 <compander_AU_PARA_init+0x14>

00000a18 <compander_AU_PARA_init+0x168>:
     a18:	03ad           	mov.n	a10, a3
     a1a:	fda281        	l32r	a8, a4 (1ef0 <xt_memcpy>)
     a1d:	3cc2b2        	addi	a11, a2, 60
     a20:	4c0c           	movi.n	a12, 4
     a22:	0008e0        	callx8	a8
     a25:	ffb9c6        	j	910 <compander_AU_PARA_init+0x60>

00000a28 <compander_AU_PARA_init+0x178>:
     a28:	fd9f81        	l32r	a8, a4 (1ef0 <xt_memcpy>)
     a2b:	1ac2b2        	addi	a11, a2, 26
     a2e:	4c0c           	movi.n	a12, 4
     a30:	0008e0        	callx8	a8
     a33:	ffd786        	j	995 <compander_AU_PARA_init+0xe5>

00000a36 <compander_AU_PARA_init+0x186>:
	...

00000a38 <compander_AU_PARA_init+0x188>:
     a38:	2e92b2        	l16si	a11, a2, 92
     a3b:	2d92c2        	l16si	a12, a2, 90
     a3e:	a059c2        	s16i	a12, a9, 0x140
     a41:	a359b2        	s16i	a11, a9, 0x146
     a44:	ffeac6        	j	9f3 <compander_AU_PARA_init+0x143>

00000a47 <compander_AU_PARA_init+0x197>:
	...

00000a48 <compander_AU_PARA_init+0x198>:
     a48:	fd9781        	l32r	a8, a4 (1ef0 <xt_memcpy>)
     a4b:	12c2b2        	addi	a11, a2, 18
     a4e:	4c0c           	movi.n	a12, 4
     a50:	0008e0        	callx8	a8
     a53:	ffcf86        	j	995 <compander_AU_PARA_init+0xe5>

00000a56 <compander_AU_PARA_init+0x1a6>:
	...

00000a58 <compander_AU_PARA_init+0x1a8>:
     a58:	fd9381        	l32r	a8, a4 (1ef0 <xt_memcpy>)
     a5b:	b2ab           	addi.n	a11, a2, 10
     a5d:	4c0c           	movi.n	a12, 4
     a5f:	0008e0        	callx8	a8
     a62:	ffcbc6        	j	995 <compander_AU_PARA_init+0xe5>

00000a65 <compander_AU_PARA_init+0x1b5>:
     a65:	000000                                        ...

00000a68 <compander_AU_Sub_init>:
     a68:	004136        	entry	a1, 32
     a6b:	04ad           	mov.n	a10, a4
     a6d:	fd8e81        	l32r	a8, a8 (1ed8 <xt_memset>)
     a70:	0b0c           	movi.n	a11, 0
     a72:	50a3c2        	movi	a12, 0x350
     a75:	0008e0        	callx8	a8
     a78:	02ad           	mov.n	a10, a2
     a7a:	03bd           	mov.n	a11, a3
     a7c:	05cd           	mov.n	a12, a5
     a7e:	fd9781        	l32r	a8, dc (8b0 <compander_AU_PARA_init>)
     a81:	06dd           	mov.n	a13, a6
     a83:	07ed           	mov.n	a14, a7
     a85:	0008e0        	callx8	a8
     a88:	fd9721        	l32r	a2, e4 (8000000 <_end+0x7ffd92c>)
     a8b:	13d3a2        	addmi	a10, a3, 0x1300
     a8e:	741a92        	l16ui	a9, a10, 232
     a91:	fd9331        	l32r	a3, e0 (800000 <_end+0x7fd92c>)
     a94:	eccaa2        	addi	a10, a10, -20
     a97:	f8c9b2        	addi	a11, a9, -8
     a9a:	0a5b16        	beqz	a11, b43 <compander_AU_Sub_init+0xdb>
     a9d:	f6c9b2        	addi	a11, a9, -10
     aa0:	086b16        	beqz	a11, b2a <compander_AU_Sub_init+0xc2>
     aa3:	0b2c           	movi.n	a11, 32
     aa5:	0c2c           	movi.n	a12, 32
     aa7:	00a4d2        	movi	a13, 0x400
     aaa:	1154d2        	s16i	a13, a4, 34
     aad:	1054c2        	s16i	a12, a4, 32

00000ab0 <compander_AU_Sub_init+0x48>:
     ab0:	fec5e2        	addi	a14, a5, -2
     ab3:	09ce16        	beqz	a14, b53 <compander_AU_Sub_init+0xeb>
     ab6:	7e1a92        	l16ui	a9, a10, 252
     ab9:	e81c           	movi.n	a8, 30
     abb:	f8c9f2        	addi	a15, a9, -8
     abe:	078f16        	beqz	a15, b3a <compander_AU_Sub_init+0xd2>
     ac1:	f6c9a2        	addi	a10, a9, -10
     ac4:	60a092        	movi	a9, 96
     ac7:	8398a0        	moveqz	a9, a8, a10
     aca:	b499           	s32i.n	a9, a4, 44

00000acc <compander_AU_Sub_init+0x64>:
     acc:	a429           	s32i.n	a2, a4, 40
     ace:	fd85a1        	l32r	a10, e4 (8000000 <_end+0x7ffd92c>)
     ad1:	048766        	bnei	a7, 8, ad9 <compander_AU_Sub_init+0x71>
     ad4:	a439           	s32i.n	a3, a4, 40
     ad6:	fd82a1        	l32r	a10, e0 (800000 <_end+0x7fd92c>)

00000ad9 <compander_AU_Sub_init+0x71>:
     ad9:	1326a6        	blti	a6, 2, af0 <compander_AU_Sub_init+0x88>
     adc:	11dbf0        	slli	a13, a11, 1
     adf:	11e9f0        	slli	a14, a9, 1
     ae2:	1194c2        	l16si	a12, a4, 34
     ae5:	b4e9           	s32i.n	a14, a4, 44
     ae7:	1054d2        	s16i	a13, a4, 32
     aea:	21c1c0        	srai	a12, a12, 1
     aed:	1154c2        	s16i	a12, a4, 34

00000af0 <compander_AU_Sub_init+0x88>:
     af0:	fd7e81        	l32r	a8, e8 (1a10 <calc_slope>)
     af3:	54b8           	l32i.n	a11, a4, 20
     af5:	24c8           	l32i.n	a12, a4, 8
     af7:	0008e0        	callx8	a8
     afa:	0d5452        	s16i	a5, a4, 26
     afd:	f429           	s32i.n	a2, a4, 60
     aff:	01d4b2        	addmi	a11, a4, 0x100
     b02:	3c54a2        	s16i	a10, a4, 120
     b05:	190c           	movi.n	a9, 1
     b07:	0c0c           	movi.n	a12, 0
     b09:	0f54c2        	s16i	a12, a4, 30
     b0c:	1364c2        	s32i	a12, a4, 76
     b0f:	1264c2        	s32i	a12, a4, 72
     b12:	0e5492        	s16i	a9, a4, 28
     b15:	fd75a1        	l32r	a10, ec (7fffffff <_end+0x7fffd92b>)
     b18:	a0cbb2        	addi	a11, a11, -96
     b1b:	c4b9           	s32i.n	a11, a4, 48
     b1d:	d4b9           	s32i.n	a11, a4, 52
     b1f:	1064a2        	s32i	a10, a4, 64
     b22:	3d8726        	beqi	a7, 8, b63 <compander_AU_Sub_init+0xfb>
     b25:	3f5472        	s16i	a7, a4, 126
     b28:	f01d           	retw.n

00000b2a <compander_AU_Sub_init+0xc2>:
     b2a:	ab0c           	movi.n	a11, 10
     b2c:	ad0c           	movi.n	a13, 10
     b2e:	fd60e1        	l32r	a14, b0 (ccd <compander_AU_init+0x11>)
     b31:	1154e2        	s16i	a14, a4, 34
     b34:	1054d2        	s16i	a13, a4, 32
     b37:	ffdd46        	j	ab0 <compander_AU_Sub_init+0x48>

00000b3a <compander_AU_Sub_init+0xd2>:
     b3a:	891c           	movi.n	a9, 24
     b3c:	8f1c           	movi.n	a15, 24
     b3e:	b4f9           	s32i.n	a15, a4, 44
     b40:	ffe206        	j	acc <compander_AU_Sub_init+0x64>

00000b43 <compander_AU_Sub_init+0xdb>:
     b43:	8b0c           	movi.n	a11, 8
     b45:	880c           	movi.n	a8, 8
     b47:	fd6a91        	l32r	a9, f0 (1000 <F_CPD_Intp_Out+0x88>)
     b4a:	115492        	s16i	a9, a4, 34
     b4d:	105482        	s16i	a8, a4, 32
     b50:	ffd706        	j	ab0 <compander_AU_Sub_init+0x48>

00000b53 <compander_AU_Sub_init+0xeb>:
     b53:	11cbf0        	slli	a12, a11, 1
     b56:	fd67a1        	l32r	a10, f4 (40000000 <_end+0x3fffd92c>)
     b59:	a4a9           	s32i.n	a10, a4, 40
     b5b:	b4c9           	s32i.n	a12, a4, 44
     b5d:	fd65a1        	l32r	a10, f4 (40000000 <_end+0x3fffd92c>)
     b60:	ffe306        	j	af0 <compander_AU_Sub_init+0x88>

00000b63 <compander_AU_Sub_init+0xfb>:
     b63:	f439           	s32i.n	a3, a4, 60
     b65:	3f5472        	s16i	a7, a4, 126
     b68:	f01d           	retw.n

00000b6a <compander_AU_Sub_init+0x102>:
	...

00000b6c <DBB_PARA_init>:
     b6c:	004136        	entry	a1, 32
     b6f:	03ad           	mov.n	a10, a3
     b71:	fd4c81        	l32r	a8, a4 (1ef0 <xt_memcpy>)
     b74:	70c2b2        	addi	a11, a2, 112
     b77:	4c0c           	movi.n	a12, 4
     b79:	0008e0        	callx8	a8
     b7c:	a38b           	addi.n	a10, a3, 8
     b7e:	fd4981        	l32r	a8, a4 (1ef0 <xt_memcpy>)
     b81:	7cc2b2        	addi	a11, a2, 124
     b84:	2c0c           	movi.n	a12, 2
     b86:	0008e0        	callx8	a8
     b89:	f01d           	retw.n

00000b8b <DBB_PARA_init+0x1f>:
	...

00000b8c <DBB_init>:
     b8c:	004136        	entry	a1, 32
     b8f:	03ad           	mov.n	a10, a3
     b91:	fd4581        	l32r	a8, a8 (1ed8 <xt_memset>)
     b94:	0b0c           	movi.n	a11, 0
     b96:	ac0c           	movi.n	a12, 10
     b98:	0008e0        	callx8	a8
     b9b:	fd5781        	l32r	a8, f8 (b6c <DBB_PARA_init>)
     b9e:	02ad           	mov.n	a10, a2
     ba0:	03bd           	mov.n	a11, a3
     ba2:	0008e0        	callx8	a8
     ba5:	03a8           	l32i.n	a10, a3, 0
     ba7:	fd5591        	l32r	a9, fc (3fffffff <_end+0x3fffd92b>)
     baa:	fd4eb1        	l32r	a11, e4 (8000000 <_end+0x7ffd92c>)
     bad:	11caf0        	slli	a12, a10, 1
     bb0:	0529a7        	blt	a9, a10, bb9 <DBB_init+0x2d>
     bb3:	43b9           	s32i.n	a11, a3, 16
     bb5:	33c9           	s32i.n	a12, a3, 12
     bb7:	f01d           	retw.n

00000bb9 <DBB_init+0x2d>:
     bb9:	fd4ad1        	l32r	a13, e4 (8000000 <_end+0x7ffd92c>)
     bbc:	fd4ce1        	l32r	a14, ec (7fffffff <_end+0x7fffd92b>)
     bbf:	33e9           	s32i.n	a14, a3, 12
     bc1:	43d9           	s32i.n	a13, a3, 16
     bc3:	f01d           	retw.n

00000bc5 <DBB_init+0x39>:
     bc5:	000000                                        ...

00000bc8 <AU_GBL_init>:
     bc8:	004136        	entry	a1, 32
     bcb:	03ad           	mov.n	a10, a3
     bcd:	fd3581        	l32r	a8, a4 (1ef0 <xt_memcpy>)
     bd0:	02bd           	mov.n	a11, a2
     bd2:	02a0c2        	movi	a12, 2
     bd5:	0008e0        	callx8	a8
     bd8:	0b0c           	movi.n	a11, 0
     bda:	0c1c           	movi.n	a12, 16
     bdc:	02d3a2        	addmi	a10, a3, 0x200
     bdf:	000392        	l8ui	a9, a3, 0
     be2:	fd3181        	l32r	a8, a8 (1ed8 <xt_memset>)
     be5:	639d           	ae_s16i.n	a9, a3, 0
     be7:	10caa2        	addi	a10, a10, 16
     bea:	0008e0        	callx8	a8
     bed:	0b0c           	movi.n	a11, 0
     bef:	0c2c           	movi.n	a12, 32
     bf1:	fd2d81        	l32r	a8, a8 (1ed8 <xt_memset>)
     bf4:	02d3a2        	addmi	a10, a3, 0x200
     bf7:	30caa2        	addi	a10, a10, 48
     bfa:	0008e0        	callx8	a8
     bfd:	a34b           	addi.n	a10, a3, 4
     bff:	fd2981        	l32r	a8, a4 (1ef0 <xt_memcpy>)
     c02:	78c2b2        	addi	a11, a2, 120
     c05:	2c0c           	movi.n	a12, 2
     c07:	0008e0        	callx8	a8
     c0a:	1e2292        	l32i	a9, a2, 120
     c0d:	c0a252        	movi	a5, 0x2c0
     c10:	99bc           	beqz.n	a9, c4d <AU_GBL_init+0x85>
     c12:	5e1926        	beqi	a9, 1, c74 <AU_GBL_init+0xac>

00000c15 <AU_GBL_init+0x4d>:
     c15:	041392        	l16ui	a9, a3, 8
     c18:	fd2cd1        	l32r	a13, c8 (400000 <_end+0x3fd92c>)
     c1b:	ab0c           	movi.n	a11, 10
     c1d:	178926        	beqi	a9, 8, c38 <AU_GBL_init+0x70>
     c20:	f6c9c2        	addi	a12, a9, -10
     c23:	0a2c           	movi.n	a10, 32
     c25:	83abc0        	moveqz	a10, a11, a12
     c28:	0453a2        	s16i	a10, a3, 8

00000c2b <AU_GBL_init+0x63>:
     c2b:	33d9           	s32i.n	a13, a3, 12
     c2d:	0f8426        	beqi	a4, 8, c40 <AU_GBL_init+0x78>
     c30:	eca1e2        	movi	a14, 0x1ec
     c33:	0553e2        	s16i	a14, a3, 10
     c36:	f01d           	retw.n

00000c38 <AU_GBL_init+0x70>:
     c38:	8f0c           	movi.n	a15, 8
     c3a:	0453f2        	s16i	a15, a3, 8
     c3d:	fffa86        	j	c2b <AU_GBL_init+0x63>

00000c40 <AU_GBL_init+0x78>:
     c40:	eca182        	movi	a8, 0x1ec
     c43:	fd2f91        	l32r	a9, 100 (40000 <_end+0x3d92c>)
     c46:	3399           	s32i.n	a9, a3, 12
     c48:	055382        	s16i	a8, a3, 10
     c4b:	f01d           	retw.n

00000c4d <AU_GBL_init+0x85>:
     c4d:	8c2c           	movi.n	a12, 40
     c4f:	01d2b2        	addmi	a11, a2, 0x100
     c52:	02d3a2        	addmi	a10, a3, 0x200
     c55:	fd1381        	l32r	a8, a4 (1ef0 <xt_memcpy>)
     c58:	70caa2        	addi	a10, a10, 112
     c5b:	80cbb2        	addi	a11, a11, -128
     c5e:	0008e0        	callx8	a8
     c61:	a35a           	add.n	a10, a3, a5
     c63:	8c2c           	movi.n	a12, 40
     c65:	fd0f81        	l32r	a8, a4 (1ef0 <xt_memcpy>)
     c68:	01d2b2        	addmi	a11, a2, 0x100
     c6b:	d0cbb2        	addi	a11, a11, -48
     c6e:	0008e0        	callx8	a8
     c71:	ffe806        	j	c15 <AU_GBL_init+0x4d>

00000c74 <AU_GBL_init+0xac>:
     c74:	8c2c           	movi.n	a12, 40
     c76:	01d2b2        	addmi	a11, a2, 0x100
     c79:	02d3a2        	addmi	a10, a3, 0x200
     c7c:	fd0a81        	l32r	a8, a4 (1ef0 <xt_memcpy>)
     c7f:	70caa2        	addi	a10, a10, 112
     c82:	20cbb2        	addi	a11, a11, 32
     c85:	0008e0        	callx8	a8
     c88:	a35a           	add.n	a10, a3, a5
     c8a:	8c2c           	movi.n	a12, 40
     c8c:	fd0681        	l32r	a8, a4 (1ef0 <xt_memcpy>)
     c8f:	01d2b2        	addmi	a11, a2, 0x100
     c92:	70cbb2        	addi	a11, a11, 112
     c95:	0008e0        	callx8	a8
     c98:	ffde46        	j	c15 <AU_GBL_init+0x4d>

00000c9b <AU_GBL_init+0xd3>:
	...

00000c9c <compander_AU_SetFrame120_init>:
     c9c:	004136        	entry	a1, 32
     c9f:	13d242        	addmi	a4, a2, 0x1300
     ca2:	a30c           	movi.n	a3, 10
     ca4:	745432        	s16i	a3, a4, 232
     ca7:	f01d           	retw.n

00000ca9 <compander_AU_SetFrame120_init+0xd>:
     ca9:	000000                                        ...

00000cac <compander_AU_SetFrame8_init>:
     cac:	004136        	entry	a1, 32
     caf:	13d242        	addmi	a4, a2, 0x1300
     cb2:	830c           	movi.n	a3, 8
     cb4:	745432        	s16i	a3, a4, 232
     cb7:	f01d           	retw.n

00000cb9 <compander_AU_SetFrame8_init+0xd>:
     cb9:	000000                                        ...

00000cbc <compander_AU_init>:
     cbc:	004136        	entry	a1, 32
     cbf:	03ad           	mov.n	a10, a3
     cc1:	02bd           	mov.n	a11, a2
     cc3:	02cd           	mov.n	a12, a2
     cc5:	0d0c           	movi.n	a13, 0
     cc7:	fd0f81        	l32r	a8, 104 (a68 <compander_AU_Sub_init>)
     cca:	20e550        	or	a14, a5, a5
     ccd:	04fd           	mov.n	a15, a4
     ccf:	0008e0        	callx8	a8
     cd2:	03ad           	mov.n	a10, a3
     cd4:	02bd           	mov.n	a11, a2
     cd6:	1d0c           	movi.n	a13, 1
     cd8:	05ed           	mov.n	a14, a5
     cda:	20f440        	or	a15, a4, a4
     cdd:	07d262        	addmi	a6, a2, 0x700
     ce0:	fd0981        	l32r	a8, 104 (a68 <compander_AU_Sub_init>)
     ce3:	a0c662        	addi	a6, a6, -96
     ce6:	20c660        	or	a12, a6, a6
     ce9:	0008e0        	callx8	a8
     cec:	05ed           	mov.n	a14, a5
     cee:	03ad           	mov.n	a10, a3
     cf0:	02bd           	mov.n	a11, a2
     cf2:	2d0c           	movi.n	a13, 2
     cf4:	04fd           	mov.n	a15, a4
     cf6:	fd0381        	l32r	a8, 104 (a68 <compander_AU_Sub_init>)
     cf9:	0dd2c2        	addmi	a12, a2, 0xd00
     cfc:	40ccc2        	addi	a12, a12, 64
     cff:	0008e0        	callx8	a8
     d02:	04cd           	mov.n	a12, a4
     d04:	03ad           	mov.n	a10, a3
     d06:	fd0081        	l32r	a8, 108 (bc8 <AU_GBL_init>)
     d09:	14d2b2        	addmi	a11, a2, 0x1400
     d0c:	e0cbb2        	addi	a11, a11, -32
     d0f:	0008e0        	callx8	a8
     d12:	03ad           	mov.n	a10, a3
     d14:	fcfe81        	l32r	a8, 10c (b8c <DBB_init>)
     d17:	17d2b2        	addmi	a11, a2, 0x1700
     d1a:	f0cbb2        	addi	a11, a11, -16
     d1d:	0008e0        	callx8	a8
     d20:	fcfc81        	l32r	a8, 110 (38c <AU_INS_coef_init>)
     d23:	02ad           	mov.n	a10, a2
     d25:	0008e0        	callx8	a8
     d28:	fcfa81        	l32r	a8, 110 (38c <AU_INS_coef_init>)
     d2b:	06ad           	mov.n	a10, a6
     d2d:	0008e0        	callx8	a8
     d30:	0cd2a2        	addmi	a10, a2, 0xc00
     d33:	05d292        	addmi	a9, a2, 0x500
     d36:	8d29b2        	l32i	a11, a9, 0x234
     d39:	8c29c2        	l32i	a12, a9, 0x230
     d3c:	746ac2        	s32i	a12, a10, 0x1d0
     d3f:	756ab2        	s32i	a11, a10, 0x1d4
     d42:	8e2992        	l32i	a9, a9, 0x238
     d45:	766a92        	s32i	a9, a10, 0x1d8
     d48:	f01d           	retw.n

00000d4a <compander_AU_init+0x8e>:
	...

00000d4c <F_CPD_Ins_Gain>:
     d4c:	006136        	entry	a1, 48
     d4f:	01ad           	mov.n	a10, a1
     d51:	01d2b2        	addmi	a11, a2, 0x100
     d54:	202292        	l32i	a9, a2, 128
     d57:	fcd281        	l32r	a8, a0 (2d8 <Xdiv>)
     d5a:	0199           	s32i.n	a9, a1, 0
     d5c:	8ccbb2        	addi	a11, a11, -116
     d5f:	0008e0        	callx8	a8
     d62:	b14b           	addi.n	a11, a1, 4
     d64:	fccf81        	l32r	a8, a0 (2d8 <Xdiv>)
     d67:	119a00        	slli	a9, a10, 16
     d6a:	11a9           	s32i.n	a10, a1, 4
     d6c:	2422a2        	l32i	a10, a2, 144
     d6f:	01a9           	s32i.n	a10, a1, 0
     d71:	1199           	s32i.n	a9, a1, 4
     d73:	01ad           	mov.n	a10, a1
     d75:	0008e0        	callx8	a8
     d78:	11b8           	l32i.n	a11, a1, 4
     d7a:	2522e2        	l32i	a14, a2, 148
     d7d:	01e9           	s32i.n	a14, a1, 0
     d7f:	2622d2        	l32i	a13, a2, 152
     d82:	11ca00        	slli	a12, a10, 16
     d85:	368e34        	ae_cvtq48a32s	aeq2, a14
     d88:	b2bbd0        	mulsh	a11, a11, a13
     d8b:	364c34        	ae_cvtq48a32s	aeq1, a12
     d8e:	11bbf0        	slli	a11, a11, 1
     d91:	fbea7ecb3f 	{ ae_cvtq48a32s	aeq0, a11; ae_addsq56s	aeq3, aeq1, aeq2 }
     d96:	06b804        	ae_addsq56s	aeq2, aeq3, aeq0
     d99:	366094        	ae_sllisq56s	aeq1, aeq2, 2
     d9c:	061934        	ae_roundsq32asym	aeq0, aeq1
     d9f:	620104        	ae_sq32f.i	aeq0, a1, 0
     da2:	0128           	l32i.n	a2, a1, 0
     da4:	f01d           	retw.n

00000da6 <F_CPD_Ins_Gain+0x5a>:
	...

00000da8 <F_CPD_Mag_In>:
     da8:	00a136        	entry	a1, 80
     dab:	028d           	mov.n	a8, a2
     dad:	fcc372        	addi	a7, a3, -4
     db0:	fe2482229e 	{ addi	a9, a2, 34; ae_zeroq56	aeq2 }
     db5:	632144        	ae_sq56s.i	aeq2, a1, 32
     db8:	400904        	ae_lp16f.i	aep0, a9, 0
     dbb:	109292        	l16si	a9, a2, 32
     dbe:	601714        	ae_lq32f.iu	aeq0, a7, 4
     dc1:	ff1453277f 	{ or	a2, a7, a7; ae_movq56	aeq3, aeq2 }
     dc6:	0aa976        	loopgtz	a9, dd4 <F_CPD_Mag_In+0x2c>
     dc9:	fd00bd121f 	{ ae_lq32f.iu	aeq0, a2, 4; ae_absq56	aeq1, aeq0 }
     dce:	0192c4        	ae_maxq56s	aeq2, aeq2, aeq1
     dd1:	088b64        	ae_mulafq32sp24s.l	aeq3, aeq1, aep0

00000dd4 <F_CPD_Mag_In+0x2c>:
     dd4:	063b34        	ae_movq56	aeq0, aeq3
     dd7:	ff147a013f 	{ ae_sq56s.i	aeq0, a1, 24; ae_movq56	aeq3, aeq2 }
     ddc:	542526        	beqi	a5, 2, e34 <F_CPD_Mag_In+0x8c>

00000ddf <F_CPD_Mag_In+0x37>:
     ddf:	ff84fd814f 	{ ae_lq56.i	aeq0, a1, 32; ae_satq48s	aeq3, aeq3 }
     de4:	3c1666        	bnei	a6, 1, e24 <F_CPD_Mag_In+0x7c>
     de7:	fc247dc13f 	{ ae_lq56.i	aeq2, a1, 24; ae_roundsq32asym	aeq0, aeq0 }
     dec:	fe347f32ef 	{ ae_trunca32q48	a2, aeq3; ae_roundsq32asym	aeq2, aeq2 }
     df1:	d6c03a014f 	{ ae_sq56s.i	aeq0, a1, 32; ae_ltq56s	b0, aeq2, aeq0 }
     df6:	632134        	ae_sq56s.i	aeq2, a1, 24
     df9:	614144        	ae_lq56.i	aeq1, a1, 32
     dfc:	0304a0        	rsr.br	a10
     dff:	04a0a0        	extui	a10, a10, 0, 1
     e02:	052a16        	beqz	a10, e58 <F_CPD_Mag_In+0xb0>
     e05:	fcbac1        	l32r	a12, f0 (1000 <F_CPD_Intp_Out+0x88>)
     e08:	01d8d2        	addmi	a13, a8, 0x100
     e0b:	8ccdd2        	addi	a13, a13, -116
     e0e:	608d04        	ae_lq32f.i	aeq2, a13, 0
     e11:	f8ca98accf 	{ ae_cvtp24a16x2.ll	aep2, a12, a12; ae_subsq56s	aeq0, aeq1, aeq2 }
     e16:	03a014        	ae_mulafq32sp16s.l	aeq2, aeq0, aep2
     e19:	066934        	ae_roundsq32asym	aeq1, aeq2
     e1c:	369b04        	ae_trunca32q48	a11, aeq1
     e1f:	2368b2        	s32i	a11, a8, 140
     e22:	f01d           	retw.n

00000e24 <F_CPD_Mag_In+0x7c>:
     e24:	36b204        	ae_trunca32q48	a2, aeq3
     e27:	fcb1e1        	l32r	a14, ec (7fffffff <_end+0x7fffd92b>)
     e2a:	2368e2        	s32i	a14, a8, 140
     e2d:	f01d           	retw.n

00000e2f <F_CPD_Mag_In+0x87>:
     e2f:	00000000 61c14400                                .....

00000e34 <F_CPD_Mag_In+0x8c>:
     e34:	61c144        	ae_lq56.i	aeq3, a1, 32
     e37:	fcc422        	addi	a2, a4, -4
     e3a:	601214        	ae_lq32f.iu	aeq0, a2, 4
     e3d:	0aa976        	loopgtz	a9, e4b <F_CPD_Mag_In+0xa3>
     e40:	fd00bd121f 	{ ae_lq32f.iu	aeq0, a2, 4; ae_absq56	aeq1, aeq0 }
     e45:	0192c4        	ae_maxq56s	aeq2, aeq2, aeq1
     e48:	088b64        	ae_mulafq32sp24s.l	aeq3, aeq1, aep0

00000e4b <F_CPD_Mag_In+0xa3>:
     e4b:	fe1c7f213f 	{ ae_movq56	aeq0, aeq2; ae_movq56	aeq2, aeq3 }

00000e50 <F_CPD_Mag_In+0xa8>:
     e50:	ff047a214f 	{ ae_sq56s.i	aeq2, a1, 32; ae_movq56	aeq3, aeq0 }
     e55:	ffe186        	j	ddf <F_CPD_Mag_In+0x37>

00000e58 <F_CPD_Mag_In+0xb0>:
     e58:	614134        	ae_lq56.i	aeq1, a1, 24
     e5b:	fca521        	l32r	a2, f0 (1000 <F_CPD_Intp_Out+0x88>)
     e5e:	01d872        	addmi	a7, a8, 0x100
     e61:	8cc772        	addi	a7, a7, -116
     e64:	608704        	ae_lq32f.i	aeq2, a7, 0
     e67:	f8ca98b22f 	{ ae_cvtp24a16x2.ll	aep3, a2, a2; ae_subsq56s	aeq0, aeq1, aeq2 }
     e6c:	03b014        	ae_mulafq32sp16s.l	aeq2, aeq0, aep3
     e6f:	fd347f32ef 	{ ae_trunca32q48	a2, aeq3; ae_roundsq32asym	aeq1, aeq2 }
     e74:	369f04        	ae_trunca32q48	a15, aeq1
     e77:	2368f2        	s32i	a15, a8, 140
     e7a:	f01d           	retw.n

00000e7c <F_CPD_Max_In_Buf>:
     e7c:	006136        	entry	a1, 48
     e7f:	1b0c           	movi.n	a11, 1
     e81:	0e9292        	l16si	a9, a2, 28
     e84:	0c92c2        	l16si	a12, a2, 24
     e87:	a288           	l32i.n	a8, a2, 40
     e89:	1222a2        	l32i	a10, a2, 72
     e8c:	c0cc90        	sub	a12, a12, a9
     e8f:	05a837        	bge	a8, a3, e98 <F_CPD_Max_In_Buf+0x1c>
     e92:	0a6232        	s32i	a3, a2, 40
     e95:	0f52b2        	s16i	a11, a2, 30

00000e98 <F_CPD_Max_In_Buf+0x1c>:
     e98:	08cc56        	bnez	a12, f28 <F_CPD_Max_In_Buf+0xac>
     e9b:	1422e2        	l32i	a14, a2, 80
     e9e:	54c282        	addi	a8, a2, 84
     ea1:	0e52b2        	s16i	a11, a2, 28
     ea4:	53eea0        	max	a14, a14, a10
     ea7:	01e9           	s32i.n	a14, a1, 0
     ea9:	1522f2        	l32i	a15, a2, 84
     eac:	690c           	movi.n	a9, 6
     eae:	50c2b2        	addi	a11, a2, 80
     eb1:	53eef0        	max	a14, a14, a15
     eb4:	0061e2        	s32i	a14, a1, 0
     eb7:	1522d2        	l32i	a13, a2, 84
     eba:	09ebd4        	ae_s32ip	a13, a11, 4
     ebd:	0f8976        	loop	a9, ed0 <F_CPD_Max_In_Buf+0x54>
     ec0:	1898           	l32i.n	a9, a8, 4
     ec2:	01a8           	l32i.n	a10, a1, 0
     ec4:	884b           	addi.n	a8, a8, 4
     ec6:	539a90        	max	a9, a10, a9
     ec9:	0199           	s32i.n	a9, a1, 0
     ecb:	0898           	l32i.n	a9, a8, 0
     ecd:	09eb94        	ae_s32ip	a9, a11, 4

00000ed0 <F_CPD_Max_In_Buf+0x54>:
     ed0:	122282        	l32i	a8, a2, 72
     ed3:	0198           	l32i.n	a9, a1, 0
     ed5:	136292        	s32i	a9, a2, 76
     ed8:	0b89           	s32i.n	a8, a11, 0
     eda:	0f92f2        	l16si	a15, a2, 30
     edd:	126232        	s32i	a3, a2, 72
     ee0:	1f1fa6        	blti	a15, 1, f03 <F_CPD_Max_In_Buf+0x87>
     ee3:	a2a8           	l32i.n	a10, a2, 40
     ee5:	fc8081        	l32r	a8, e8 (1a10 <calc_slope>)
     ee8:	0522b2        	l32i	a11, a2, 20
     eeb:	22c8           	l32i.n	a12, a2, 8
     eed:	0008e0        	callx8	a8
     ef0:	fc67b1        	l32r	a11, 8c (7fff <_end+0x592b>)
     ef3:	090c           	movi.n	a9, 0
     ef5:	80dac2        	addmi	a12, a10, 0xffff8000
     ef8:	83abc0        	moveqz	a10, a11, a12
     efb:	01a9           	s32i.n	a10, a1, 0
     efd:	3c52a2        	s16i	a10, a2, 120
     f00:	0f5292        	s16i	a9, a2, 30

00000f03 <F_CPD_Max_In_Buf+0x87>:
     f03:	1322a2        	l32i	a10, a2, 76

00000f06 <F_CPD_Max_In_Buf+0x8a>:
     f06:	22b8           	l32i.n	a11, a2, 8
     f08:	402ba7        	blt	a11, a10, f4c <F_CPD_Max_In_Buf+0xd0>
     f0b:	2022a2        	l32i	a10, a2, 128
     f0e:	2322b2        	l32i	a11, a2, 140
     f11:	90daa0        	addx2	a13, a10, a10
     f14:	0aabd7        	bge	a11, a13, f22 <F_CPD_Max_In_Buf+0xa6>
     f17:	252ab7        	blt	a10, a11, f40 <F_CPD_Max_In_Buf+0xc4>
     f1a:	429222        	l16si	a2, a2, 132
     f1d:	112200        	slli	a2, a2, 16
     f20:	f01d           	retw.n

00000f22 <F_CPD_Max_In_Buf+0xa6>:
     f22:	fc7221        	l32r	a2, ec (7fffffff <_end+0x7fffd92b>)
     f25:	f01d           	retw.n

00000f27 <F_CPD_Max_In_Buf+0xab>:
	...

00000f28 <F_CPD_Max_In_Buf+0xac>:
     f28:	1322b2        	l32i	a11, a2, 76
     f2b:	c91b           	addi.n	a12, a9, 1
     f2d:	53aa30        	max	a10, a10, a3
     f30:	1262a2        	s32i	a10, a2, 72
     f33:	0e52c2        	s16i	a12, a2, 28
     f36:	53aab0        	max	a10, a10, a11
     f39:	1362a2        	s32i	a10, a2, 76
     f3c:	fff186        	j	f06 <F_CPD_Max_In_Buf+0x8a>

00000f3f <F_CPD_Max_In_Buf+0xc3>:
	...

00000f40 <F_CPD_Max_In_Buf+0xc4>:
     f40:	fc7581        	l32r	a8, 114 (d4c <F_CPD_Ins_Gain>)
     f43:	02ad           	mov.n	a10, a2
     f45:	0008e0        	callx8	a8
     f48:	0a2d           	mov.n	a2, a10
     f4a:	f01d           	retw.n

00000f4c <F_CPD_Max_In_Buf+0xd0>:
     f4c:	fc5581        	l32r	a8, a0 (2d8 <Xdiv>)
     f4f:	364b34        	ae_cvtq48a32s	aeq1, a11
     f52:	c0aab0        	sub	a10, a10, a11
     f55:	368a34        	ae_cvtq48a32s	aeq2, a10
     f58:	78c2b2        	addi	a11, a2, 120
     f5b:	400b04        	ae_lp16f.i	aep0, a11, 0
     f5e:	030894        	ae_mulafq32sp16s.l	aeq1, aeq2, aep0
     f61:	a28b           	addi.n	a10, a2, 8
     f63:	fc2c53b11f 	{ or	a11, a1, a1; ae_roundsq32asym	aeq0, aeq1 }
     f68:	368904        	ae_trunca32q48	a9, aeq0
     f6b:	0199           	s32i.n	a9, a1, 0
     f6d:	0008e0        	callx8	a8
     f70:	112a00        	slli	a2, a10, 16
     f73:	f01d           	retw.n

00000f75 <F_CPD_Max_In_Buf+0xf9>:
     f75:	000000                                        ...

00000f78 <F_CPD_Intp_Out>:
     f78:	008136        	entry	a1, 64
     f7b:	fec682        	addi	a8, a6, -2
     f7e:	0a5172        	s16i	a7, a1, 20
     f81:	4f1c           	movi.n	a15, 20
     f83:	02cd           	mov.n	a12, a2
     f85:	22cc92        	addi	a9, a12, 34
     f88:	032d           	mov.n	a2, a3
     f8a:	102ce2        	l32i	a14, a12, 64
     f8d:	4121f4        	ae_lp16f.x	aep2, a1, a15
     f90:	360e34        	ae_cvtq48a32s	aeq0, a14
     f93:	3ccc32        	addi	a3, a12, 60
     f96:	60c304        	ae_lq32f.i	aeq3, a3, 0
     f99:	f9de55e5ef 	{ sub	a14, a5, a14; ae_truncp24q48x2	aep3, aeq3, aeq3 }
     f9e:	f88c7ecebf 	{ ae_cvtq48a32s	aeq2, a14; ae_mulfq32sp24s.l	aeq1, aeq0, aep3 }
     fa3:	f68ab0ccae 	{ l32i	a10, a12, 48; ae_mulafq32sp16s.l	aeq0, aeq2, aep2 }
     fa8:	f3f6a0        	wur.ae_cbegin0	a10
     fab:	fe24438cbe 	{ addi	a11, a12, 56; ae_roundsq32asym	aeq2, aeq0 }
     fb0:	36ad04        	ae_trunca32q48	a13, aeq2
     fb3:	106cd2        	s32i	a13, a12, 64
     fb6:	60cb04        	ae_lq32f.i	aeq3, a11, 0
     fb9:	f8de6c090f 	{ ae_lp16f.i	aep0, a9, 0; ae_truncp24q48x2	aep1, aeq3, aeq3 }
     fbe:	fb0423fc9e 	{ l16si	a9, a12, 126; ae_mulfq32sp24s.h	aeq2, aeq2, aep1 }
     fc3:	cca8           	l32i.n	a10, a12, 48
     fc5:	fa36f0bc3e 	{ l32i	a3, a12, 44; ae_subq56	aeq2, aeq2, aeq1 }
     fca:	d7620f897e 	{ addi	a7, a9, -8; ae_mulfq32sp16s.h	aeq2, aeq2, aep0 }
     fcf:	090816        	beqz	a8, 1063 <F_CPD_Intp_Out+0xeb>
     fd2:	035d           	mov.n	a5, a3

00000fd4 <F_CPD_Intp_Out+0x5c>:
     fd4:	112182        	l32i	a8, a1, 68
     fd7:	109ce2        	l16si	a14, a12, 32
     fda:	1021b2        	l32i	a11, a1, 64
     fdd:	dc38           	l32i.n	a3, a12, 52
     fdf:	a0a5a0        	addx4	a10, a5, a10
     fe2:	f3f7a0        	wur.ae_cend0	a10
     fe5:	03fd           	mov.n	a15, a3
     fe7:	03dd           	mov.n	a13, a3
     fe9:	07c716        	beqz	a7, 1069 <F_CPD_Intp_Out+0xf1>
     fec:	4a0c           	movi.n	a10, 4
     fee:	794b           	addi.n	a7, a9, 4
     ff0:	837a80        	moveqz	a7, a10, a8

00000ff3 <F_CPD_Intp_Out+0x7b>:
     ff3:	480c           	movi.n	a8, 4
     ff5:	fc24fb8d8f 	{ ae_lq32f.c	aeq0, a13, a8; nop }
     ffa:	075b16        	beqz	a11, 1073 <F_CPD_Intp_Out+0xfb>
     ffd:	fec692        	addi	a9, a6, -2
    1000:	0c6916        	beqz	a9, 10ca <F_CPD_Intp_Out+0x152>
    1003:	511ea6        	blti	a14, 1, 1058 <F_CPD_Intp_Out+0xe0>
    1006:	fbea0fc25e 	{ addi	a5, a2, -4; ae_addq56	aeq3, aeq1, aeq2 }
    100b:	fa7f8ffe3e 	{ addi	a3, a14, -1; ae_roundsp24q48asym	aep4, aeq3 }
    1010:	f8907002ae 	{ l32i	a10, a2, 0; ae_mulfq32sp24s.l	aeq1, aeq0, aep4 }
    1015:	09efa4        	ae_s32ip	a10, a15, 4
    1018:	345794        	ae_sllaq56	aeq1, aeq1, a7
    101b:	fd148ffcef 	{ movi	a14, -4; ae_negq56	aeq1, aeq1 }
    1020:	fd2c40422e 	{ addi	a2, a2, 4; ae_roundsq32asym	aeq1, aeq1 }
    1025:	1ca376        	loopgtz	a3, 1045 <F_CPD_Intp_Out+0xcd>
    1028:	fbfa39d51f 	{ ae_sq32f.iu	aeq1, a5, 4; ae_addq56	aeq3, aeq3, aeq2 }
    102d:	f87fbbad8f 	{ ae_lq32f.c	aeq1, a13, a8; ae_roundsp24q48asym	aep0, aeq3 }
    1032:	f88170023e 	{ l32i	a3, a2, 0; ae_mulfq32sp24s.l	aeq1, aeq1, aep0 }
    1037:	09ef34        	ae_s32ip	a3, a15, 4
    103a:	345794        	ae_sllaq56	aeq1, aeq1, a7
    103d:	fd1480422e 	{ addi	a2, a2, 4; ae_negq56	aeq1, aeq1 }
    1042:	065934        	ae_roundsq32asym	aeq1, aeq1

00001045 <F_CPD_Intp_Out+0xcd>:
    1045:	05bd           	mov.n	a11, a5
    1047:	625b14        	ae_sq32f.iu	aeq1, a11, 4
    104a:	fc24fbcd8f 	{ ae_lq32f.c	aeq2, a13, a8; nop }
    104f:	fc24fbedef 	{ ae_lq32f.c	aeq3, a13, a14; nop }
    1054:	dcd9           	s32i.n	a13, a12, 52
    1056:	f01d           	retw.n

00001058 <F_CPD_Intp_Out+0xe0>:
    1058:	ce7c           	movi.n	a14, -4
    105a:	fc24fbedef 	{ ae_lq32f.c	aeq3, a13, a14; nop }
    105f:	dcd9           	s32i.n	a13, a12, 52
    1061:	f01d           	retw.n

00001063 <F_CPD_Intp_Out+0xeb>:
    1063:	1153f0        	slli	a5, a3, 1
    1066:	ffda86        	j	fd4 <F_CPD_Intp_Out+0x5c>

00001069 <F_CPD_Intp_Out+0xf1>:
    1069:	799a           	add.n	a7, a9, a9
    106b:	939780        	movnez	a9, a7, a8
    106e:	097d           	mov.n	a7, a9
    1070:	ffdfc6        	j	ff3 <F_CPD_Intp_Out+0x7b>

00001073 <F_CPD_Intp_Out+0xfb>:
    1073:	fec692        	addi	a9, a6, -2
    1076:	0e1916        	beqz	a9, 115b <F_CPD_Intp_Out+0x1e3>
    1079:	db1ea6        	blti	a14, 1, 1058 <F_CPD_Intp_Out+0xe0>
    107c:	fbea3002ae 	{ l32i	a10, a2, 0; ae_addq56	aeq3, aeq1, aeq2 }
    1081:	09efa4        	ae_s32ip	a10, a15, 4
    1084:	fa7fcfc25e 	{ addi	a5, a2, -4; ae_roundsp24q48asym	aep5, aeq3 }
    1089:	f8944ffe3e 	{ addi	a3, a14, -1; ae_mulfq32sp24s.l	aeq1, aeq0, aep5 }
    108e:	ce7c           	movi.n	a14, -4
    1090:	345794        	ae_sllaq56	aeq1, aeq1, a7
    1093:	fd2c40422e 	{ addi	a2, a2, 4; ae_roundsq32asym	aeq1, aeq1 }
    1098:	19a376        	loopgtz	a3, 10b5 <F_CPD_Intp_Out+0x13d>
    109b:	fbfa39d51f 	{ ae_sq32f.iu	aeq1, a5, 4; ae_addq56	aeq3, aeq3, aeq2 }
    10a0:	f87fbbad8f 	{ ae_lq32f.c	aeq1, a13, a8; ae_roundsp24q48asym	aep0, aeq3 }
    10a5:	f88170023e 	{ l32i	a3, a2, 0; ae_mulfq32sp24s.l	aeq1, aeq1, aep0 }
    10aa:	09ef34        	ae_s32ip	a3, a15, 4
    10ad:	345794        	ae_sllaq56	aeq1, aeq1, a7
    10b0:	fd2c40422e 	{ addi	a2, a2, 4; ae_roundsq32asym	aeq1, aeq1 }

000010b5 <F_CPD_Intp_Out+0x13d>:
    10b5:	05bd           	mov.n	a11, a5
    10b7:	625b14        	ae_sq32f.iu	aeq1, a11, 4
    10ba:	fc24fbcd8f 	{ ae_lq32f.c	aeq2, a13, a8; nop }
    10bf:	fc24fbedef 	{ ae_lq32f.c	aeq3, a13, a14; nop }
    10c4:	dcd9           	s32i.n	a13, a12, 52
    10c6:	f01d           	retw.n

000010c8 <F_CPD_Intp_Out+0x150>:
	...

000010ca <F_CPD_Intp_Out+0x152>:
    10ca:	8a1ea6        	blti	a14, 1, 1058 <F_CPD_Intp_Out+0xe0>
    10cd:	08ad           	mov.n	a10, a8
    10cf:	fbea0fc4be 	{ addi	a11, a4, -4; ae_addq56	aeq3, aeq1, aeq2 }
    10d4:	fb7f93977f 	{ or	a9, a7, a7; ae_roundsp24q48asym	aep6, aeq3 }
    10d9:	f89870025e 	{ l32i	a5, a2, 0; ae_mulfq32sp24s.l	aeq1, aeq0, aep6 }
    10de:	0f59           	s32i.n	a5, a15, 0
    10e0:	345794        	ae_sllaq56	aeq1, aeq1, a7
    10e3:	fd148fc25e 	{ addi	a5, a2, -4; ae_negq56	aeq1, aeq1 }
    10e8:	fd2c40422e 	{ addi	a2, a2, 4; ae_roundsq32asym	aeq1, aeq1 }
    10ed:	625514        	ae_sq32f.iu	aeq1, a5, 4
    10f0:	0438           	l32i.n	a3, a4, 0
    10f2:	fc24fbad8f 	{ ae_lq32f.c	aeq1, a13, a8; nop }
    10f7:	f899541f3f 	{ s32i.n	a3, a15, 4; ae_mulfq32sp24s.l	aeq1, aeq1, aep6 }
    10fc:	3e0b           	addi.n	a3, a14, -1
    10fe:	345794        	ae_sllaq56	aeq1, aeq1, a7
    1101:	ce7c           	movi.n	a14, -4
    1103:	fd1480447e 	{ addi	a7, a4, 4; ae_negq56	aeq1, aeq1 }
    1108:	fd2c408f4e 	{ addi	a4, a15, 8; ae_roundsq32asym	aeq1, aeq1 }
    110d:	37a376        	loopgtz	a3, 1148 <F_CPD_Intp_Out+0x1d0>
    1110:	fbfa39db1f 	{ ae_sq32f.iu	aeq1, a11, 4; ae_addq56	aeq3, aeq3, aeq2 }
    1115:	f87fbbadaf 	{ ae_lq32f.c	aeq1, a13, a10; ae_roundsp24q48asym	aep0, aeq3 }
    111a:	f88170023e 	{ l32i	a3, a2, 0; ae_mulfq32sp24s.l	aeq1, aeq1, aep0 }
    111f:	0439           	s32i.n	a3, a4, 0
    1121:	345994        	ae_sllaq56	aeq1, aeq1, a9
    1124:	fd1480422e 	{ addi	a2, a2, 4; ae_negq56	aeq1, aeq1 }
    1129:	065934        	ae_roundsq32asym	aeq1, aeq1
    112c:	625514        	ae_sq32f.iu	aeq1, a5, 4
    112f:	fc24fbadaf 	{ ae_lq32f.c	aeq1, a13, a10; nop }
    1134:	f88170073e 	{ l32i	a3, a7, 0; ae_mulfq32sp24s.l	aeq1, aeq1, aep0 }
    1139:	1439           	s32i.n	a3, a4, 4
    113b:	345994        	ae_sllaq56	aeq1, aeq1, a9
    113e:	fd1480844e 	{ addi	a4, a4, 8; ae_negq56	aeq1, aeq1 }
    1143:	fd2c40477e 	{ addi	a7, a7, 4; ae_roundsq32asym	aeq1, aeq1 }

00001148 <F_CPD_Intp_Out+0x1d0>:
    1148:	0b8d           	mov.n	a8, a11
    114a:	625814        	ae_sq32f.iu	aeq1, a8, 4
    114d:	fc24fbcdaf 	{ ae_lq32f.c	aeq2, a13, a10; nop }
    1152:	fc24fbedef 	{ ae_lq32f.c	aeq3, a13, a14; nop }
    1157:	dcd9           	s32i.n	a13, a12, 52
    1159:	f01d           	retw.n

0000115b <F_CPD_Intp_Out+0x1e3>:
    115b:	021ee6        	bgei	a14, 1, 1161 <F_CPD_Intp_Out+0x1e9>
    115e:	ffbd86        	j	1058 <F_CPD_Intp_Out+0xe0>
    1161:	fcc462        	addi	a6, a4, -4
    1164:	fcc252        	addi	a5, a2, -4
    1167:	356e07        	bbci	a14, 0, 11a0 <F_CPD_Intp_Out+0x228>
    116a:	015ac4        	ae_addq56	aeq1, aeq1, aeq2
    116d:	1b9d74        	ae_roundsp24q48asym	aep7, aeq1
    1170:	f89c3002ae 	{ l32i	a10, a2, 0; ae_mulfq32sp24s.l	aeq0, aeq0, aep7 }
    1175:	224b           	addi.n	a2, a2, 4
    1177:	340794        	ae_sllaq56	aeq0, aeq0, a7
    117a:	ff24540faf 	{ s32i.n	a10, a15, 0; ae_roundsq32asym	aeq3, aeq0 }
    117f:	627514        	ae_sq32f.iu	aeq3, a5, 4
    1182:	0498           	l32i.n	a9, a4, 0
    1184:	fc24fb8d8f 	{ ae_lq32f.c	aeq0, a13, a8; nop }
    1189:	f89c00444e 	{ addi	a4, a4, 4; ae_mulfq32sp24s.l	aeq0, aeq0, aep7 }
    118e:	1f99           	s32i.n	a9, a15, 4
    1190:	340794        	ae_sllaq56	aeq0, aeq0, a7
    1193:	ff24408ffe 	{ addi	a15, a15, 8; ae_roundsq32asym	aeq3, aeq0 }
    1198:	627614        	ae_sq32f.iu	aeq3, a6, 4
    119b:	fc24fb8d8f 	{ ae_lq32f.c	aeq0, a13, a8; nop }

000011a0 <F_CPD_Intp_Out+0x228>:
    11a0:	2191e0        	srai	a9, a14, 1
    11a3:	eb1916        	beqz	a9, 1058 <F_CPD_Intp_Out+0xe0>
    11a6:	01dac4        	ae_addq56	aeq3, aeq1, aeq2
    11a9:	1bbd14        	ae_roundsp24q48asym	aep1, aeq3
    11ac:	f8847002ee 	{ l32i	a14, a2, 0; ae_mulfq32sp24s.l	aeq1, aeq0, aep1 }
    11b1:	0fe9           	s32i.n	a14, a15, 0
    11b3:	345794        	ae_sllaq56	aeq1, aeq1, a7
    11b6:	fc2c4ffcef 	{ movi	a14, -4; ae_roundsq32asym	aeq0, aeq1 }
    11bb:	620514        	ae_sq32f.i	aeq0, a5, 4
    11be:	fc24fbad8f 	{ ae_lq32f.c	aeq1, a13, a8; nop }
    11c3:	f8857004be 	{ l32i	a11, a4, 0; ae_mulfq32sp24s.l	aeq1, aeq1, aep1 }
    11c8:	1fb9           	s32i.n	a11, a15, 4
    11ca:	345794        	ae_sllaq56	aeq1, aeq1, a7
    11cd:	fc2c410fbe 	{ addi	a11, a15, 16; ae_roundsq32asym	aeq0, aeq1 }
    11d2:	620614        	ae_sq32f.i	aeq0, a6, 4
    11d5:	f8fa3bad8f 	{ ae_lq32f.c	aeq1, a13, a8; ae_addq56	aeq0, aeq3, aeq2 }
    11da:	f867b012ae 	{ l32i	a10, a2, 4; ae_roundsp24q48asym	aep0, aeq0 }
    11df:	f88140822e 	{ addi	a2, a2, 8; ae_mulfq32sp24s.l	aeq1, aeq1, aep0 }
    11e4:	2fa9           	s32i.n	a10, a15, 8
    11e6:	345794        	ae_sllaq56	aeq1, aeq1, a7
    11e9:	08ad           	mov.n	a10, a8
    11eb:	ff2c53866f 	{ or	a8, a6, a6; ae_roundsq32asym	aeq3, aeq1 }
    11f0:	627524        	ae_sq32f.iu	aeq3, a5, 8
    11f3:	fc24fbadaf 	{ ae_lq32f.c	aeq1, a13, a10; nop }
    11f8:	f88170143e 	{ l32i	a3, a4, 4; ae_mulfq32sp24s.l	aeq1, aeq1, aep0 }
    11fd:	3f39           	s32i.n	a3, a15, 12
    11ff:	345794        	ae_sllaq56	aeq1, aeq1, a7
    1202:	04fd           	mov.n	a15, a4
    1204:	390b           	addi.n	a3, a9, -1
    1206:	0b4d           	mov.n	a4, a11
    1208:	fd2c53977f 	{ or	a9, a7, a7; ae_roundsq32asym	aeq1, aeq1 }
    120d:	60a376        	loopgtz	a3, 1271 <F_CPD_Intp_Out+0x2f9>
    1210:	f8e239d82f 	{ ae_sq32f.iu	aeq1, a8, 8; ae_addq56	aeq0, aeq0, aeq2 }
    1215:	f867bbadaf 	{ ae_lq32f.c	aeq1, a13, a10; ae_roundsp24q48asym	aep0, aeq0 }
    121a:	f88170023e 	{ l32i	a3, a2, 0; ae_mulfq32sp24s.l	aeq1, aeq1, aep0 }
    121f:	f8e214043f 	{ s32i.n	a3, a4, 0; ae_addq56	aeq0, aeq0, aeq2 }
    1224:	f867fed94f 	{ ae_sllaq56	aeq1, aeq1, a9; ae_roundsp24q48asym	aep1, aeq0 }
    1229:	fd2c408ffe 	{ addi	a15, a15, 8; ae_roundsq32asym	aeq1, aeq1 }
    122e:	621514        	ae_sq32f.i	aeq1, a5, 4
    1231:	fc24fbadaf 	{ ae_lq32f.c	aeq1, a13, a10; nop }
    1236:	f881700f3e 	{ l32i	a3, a15, 0; ae_mulfq32sp24s.l	aeq1, aeq1, aep0 }
    123b:	1439           	s32i.n	a3, a4, 4
    123d:	345994        	ae_sllaq56	aeq1, aeq1, a9
    1240:	065934        	ae_roundsq32asym	aeq1, aeq1
    1243:	621814        	ae_sq32f.i	aeq1, a8, 4
    1246:	fc24fbadaf 	{ ae_lq32f.c	aeq1, a13, a10; nop }
    124b:	f88570123e 	{ l32i	a3, a2, 4; ae_mulfq32sp24s.l	aeq1, aeq1, aep1 }
    1250:	2439           	s32i.n	a3, a4, 8
    1252:	345994        	ae_sllaq56	aeq1, aeq1, a9
    1255:	fd2c40822e 	{ addi	a2, a2, 8; ae_roundsq32asym	aeq1, aeq1 }
    125a:	625524        	ae_sq32f.iu	aeq1, a5, 8
    125d:	fc24fbadaf 	{ ae_lq32f.c	aeq1, a13, a10; nop }
    1262:	f885701f3e 	{ l32i	a3, a15, 4; ae_mulfq32sp24s.l	aeq1, aeq1, aep1 }
    1267:	3439           	s32i.n	a3, a4, 12
    1269:	345994        	ae_sllaq56	aeq1, aeq1, a9
    126c:	fd2c41044e 	{ addi	a4, a4, 16; ae_roundsq32asym	aeq1, aeq1 }

00001271 <F_CPD_Intp_Out+0x2f9>:
    1271:	625824        	ae_sq32f.iu	aeq1, a8, 8
    1274:	fc24fbcdaf 	{ ae_lq32f.c	aeq2, a13, a10; nop }
    1279:	fc24fbedef 	{ ae_lq32f.c	aeq3, a13, a14; nop }
    127e:	dcd9           	s32i.n	a13, a12, 52
    1280:	f01d           	retw.n

00001282 <F_CPD_Intp_Out+0x30a>:
	...

00001284 <multiband_cpd_module>:
    1284:	006136        	entry	a1, 48
    1287:	06ed           	mov.n	a14, a6
    1289:	02ad           	mov.n	a10, a2
    128b:	03bd           	mov.n	a11, a3
    128d:	fba281        	l32r	a8, 118 (da8 <F_CPD_Mag_In>)
    1290:	04cd           	mov.n	a12, a4
    1292:	20d550        	or	a13, a5, a5
    1295:	0008e0        	callx8	a8
    1298:	fba181        	l32r	a8, 11c (e7c <F_CPD_Max_In_Buf>)
    129b:	368a34        	ae_cvtq48a32s	aeq2, a10
    129e:	0e22b2        	l32i	a11, a2, 56
    12a1:	378bb4        	ae_truncp24a32x2	aep0, a11, a11
    12a4:	0885a4        	ae_mulfq32sp24s.l	aeq2, aeq2, aep0
    12a7:	3f9292        	l16si	a9, a2, 126
    12aa:	346934        	ae_sllasq56s	aeq1, aeq2, a9
    12ad:	fc2c53a22f 	{ or	a10, a2, a2; ae_roundsq32asym	aeq0, aeq1 }
    12b2:	368b04        	ae_trunca32q48	a11, aeq0
    12b5:	0008e0        	callx8	a8
    12b8:	1322b2        	l32i	a11, a2, 76
    12bb:	22c8           	l32i.n	a12, a2, 8
    12bd:	0add           	mov.n	a13, a10
    12bf:	2322e2        	l32i	a14, a2, 140
    12c2:	24acb7        	bge	a12, a11, 12ea <multiband_cpd_module+0x66>
    12c5:	0392f2        	l16si	a15, a2, 6

000012c8 <multiband_cpd_module+0x44>:
    12c8:	202282        	l32i	a8, a2, 128
    12cb:	908880        	addx2	a8, a8, a8
    12ce:	02ae87        	bge	a14, a8, 12d4 <multiband_cpd_module+0x50>
    12d1:	4492f2        	l16si	a15, a2, 136

000012d4 <multiband_cpd_module+0x50>:
    12d4:	05ed           	mov.n	a14, a5
    12d6:	04cd           	mov.n	a12, a4
    12d8:	03bd           	mov.n	a11, a3
    12da:	02ad           	mov.n	a10, a2
    12dc:	0179           	s32i.n	a7, a1, 0
    12de:	fb9081        	l32r	a8, 120 (f78 <F_CPD_Intp_Out>)
    12e1:	c198           	l32i.n	a9, a1, 48
    12e3:	1199           	s32i.n	a9, a1, 4
    12e5:	0008e0        	callx8	a8
    12e8:	f01d           	retw.n

000012ea <multiband_cpd_module+0x66>:
    12ea:	32a8           	l32i.n	a10, a2, 12
    12ec:	22fd           	ae_l16si.n	a15, a2, 0
    12ee:	328d           	ae_l16si.n	a8, a2, 2
    12f0:	02aab7        	bge	a10, a11, 12f6 <multiband_cpd_module+0x72>
    12f3:	fff446        	j	12c8 <multiband_cpd_module+0x44>

000012f6 <multiband_cpd_module+0x72>:
    12f6:	4298           	l32i.n	a9, a2, 16
    12f8:	0292f2        	l16si	a15, a2, 4
    12fb:	7299b0        	salt	a9, a9, a11
    12fe:	93f890        	movnez	a15, a8, a9
    1301:	8ffd           	ae_sext16	a15, a15
    1303:	fff046        	j	12c8 <multiband_cpd_module+0x44>

00001306 <multiband_cpd_module+0x82>:
	...

00001308 <multiband_cpd_module_2nd_stage>:
    1308:	004136        	entry	a1, 32
    130b:	06ed           	mov.n	a14, a6
    130d:	02ad           	mov.n	a10, a2
    130f:	03bd           	mov.n	a11, a3
    1311:	fb8181        	l32r	a8, 118 (da8 <F_CPD_Mag_In>)
    1314:	04cd           	mov.n	a12, a4
    1316:	05dd           	mov.n	a13, a5
    1318:	0008e0        	callx8	a8
    131b:	fb8081        	l32r	a8, 11c (e7c <F_CPD_Max_In_Buf>)
    131e:	3f9292        	l16si	a9, a2, 126
    1321:	368a34        	ae_cvtq48a32s	aeq2, a10
    1324:	346934        	ae_sllasq56s	aeq1, aeq2, a9
    1327:	fc2c53a22f 	{ or	a10, a2, a2; ae_roundsq32asym	aeq0, aeq1 }
    132c:	368b04        	ae_trunca32q48	a11, aeq0
    132f:	0008e0        	callx8	a8
    1332:	2322b2        	l32i	a11, a2, 140
    1335:	2022c2        	l32i	a12, a2, 128
    1338:	1022d2        	l32i	a13, a2, 64
    133b:	360d34        	ae_cvtq48a32s	aeq0, a13
    133e:	fd0451cccf 	{ addx2	a12, a12, a12; ae_movq56	aeq1, aeq0 }
    1343:	0eada7        	bge	a13, a10, 1355 <multiband_cpd_module_2nd_stage+0x4d>
    1346:	022bc7        	blt	a11, a12, 134c <multiband_cpd_module_2nd_stage+0x44>
    1349:	002b86        	j	13fb <multiband_cpd_module_2nd_stage+0xf3>
    134c:	4492e2        	l16si	a14, a2, 136
    134f:	300ee4        	ae_cvtp24a16x2.ll	aep0, a14, a14
    1352:	0003c6        	j	1365 <multiband_cpd_module_2nd_stage+0x5d>

00001355 <multiband_cpd_module_2nd_stage+0x4d>:
    1355:	09abc7        	bge	a11, a12, 1362 <multiband_cpd_module_2nd_stage+0x5a>
    1358:	4492f2        	l16si	a15, a2, 136
    135b:	300ff4        	ae_cvtp24a16x2.ll	aep0, a15, a15
    135e:	0000c6        	j	1365 <multiband_cpd_module_2nd_stage+0x5d>

00001361 <multiband_cpd_module_2nd_stage+0x59>:
	...

00001362 <multiband_cpd_module_2nd_stage+0x5a>:
    1362:	400234        	ae_lp16f.i	aep0, a2, 6

00001365 <multiband_cpd_module_2nd_stage+0x5d>:
    1365:	fcc4e2        	addi	a14, a4, -4
    1368:	22c282        	addi	a8, a2, 34
    136b:	c0cad0        	sub	a12, a10, a13
    136e:	368c34        	ae_cvtq48a32s	aeq2, a12
    1371:	f682f0c2be 	{ l32i	a11, a2, 48; ae_mulafq32sp16s.l	aeq1, aeq2, aep0 }
    1376:	f3f6b0        	wur.ae_cbegin0	a11
    1379:	ff2c4fc3be 	{ addi	a11, a3, -4; ae_roundsq32asym	aeq3, aeq1 }
    137e:	36b904        	ae_trunca32q48	a9, aeq3
    1381:	106292        	s32i	a9, a2, 64
    1384:	b298           	l32i.n	a9, a2, 44
    1386:	fb3aec180f 	{ ae_lp16f.i	aep1, a8, 0; ae_subq56	aeq3, aeq3, aeq0 }
    138b:	d76770c28e 	{ l32i	a8, a2, 48; ae_mulfq32sp16s.h	aeq3, aeq3, aep1 }
    1390:	6d2526        	beqi	a5, 2, 1401 <multiband_cpd_module_2nd_stage+0xf9>
    1393:	09ad           	mov.n	a10, a9

00001395 <multiband_cpd_module_2nd_stage+0x8d>:
    1395:	d298           	l32i.n	a9, a2, 52
    1397:	a08a80        	addx4	a8, a10, a8
    139a:	f3f780        	wur.ae_cend0	a8
    139d:	4d0c           	movi.n	a13, 4
    139f:	1092f2        	l16si	a15, a2, 32
    13a2:	09ad           	mov.n	a10, a9
    13a4:	fc24fba9df 	{ ae_lq32f.c	aeq1, a9, a13; nop }
    13a9:	25af76        	loopgtz	a15, 13d2 <multiband_cpd_module_2nd_stage+0xca>
    13ac:	3f92c2        	l16si	a12, a2, 126
    13af:	f8e63003fe 	{ l32i	a15, a3, 0; ae_addq56	aeq0, aeq0, aeq3 }
    13b4:	09eaf4        	ae_s32ip	a15, a10, 4
    13b7:	1b8d04        	ae_roundsp24q48asym	aep0, aeq0
    13ba:	088564        	ae_mulfq32sp24s.l	aeq2, aeq1, aep0
    13bd:	cc1b           	addi.n	a12, a12, 1
    13bf:	34ac34        	ae_sllasq56s	aeq2, aeq2, a12
    13c2:	fe3440433e 	{ addi	a3, a3, 4; ae_roundsq32asym	aeq2, aeq2 }
    13c7:	626b14        	ae_sq32f.iu	aeq2, a11, 4
    13ca:	122526        	beqi	a5, 2, 13e0 <multiband_cpd_module_2nd_stage+0xd8>

000013cd <multiband_cpd_module_2nd_stage+0xc5>:
    13cd:	fc24fba9df 	{ ae_lq32f.c	aeq1, a9, a13; nop }

000013d2 <multiband_cpd_module_2nd_stage+0xca>:
    13d2:	c87c           	movi.n	a8, -4
    13d4:	fc24fbe98f 	{ ae_lq32f.c	aeq3, a9, a8; nop }
    13d9:	d299           	s32i.n	a9, a2, 52
    13db:	f01d           	retw.n

000013dd <multiband_cpd_module_2nd_stage+0xd5>:
    13dd:	000000                                        ...

000013e0 <multiband_cpd_module_2nd_stage+0xd8>:
    13e0:	fc24fba9df 	{ ae_lq32f.c	aeq1, a9, a13; nop }
    13e5:	f8817004fe 	{ l32i	a15, a4, 0; ae_mulfq32sp24s.l	aeq1, aeq1, aep0 }
    13ea:	09eaf4        	ae_s32ip	a15, a10, 4
    13ed:	349c34        	ae_sllasq56s	aeq2, aeq1, a12
    13f0:	fd3440444e 	{ addi	a4, a4, 4; ae_roundsq32asym	aeq1, aeq2 }
    13f5:	625e14        	ae_sq32f.iu	aeq1, a14, 4
    13f8:	fff446        	j	13cd <multiband_cpd_module_2nd_stage+0xc5>

000013fb <multiband_cpd_module_2nd_stage+0xf3>:
    13fb:	400204        	ae_lp16f.i	aep0, a2, 0
    13fe:	ffd8c6        	j	1365 <multiband_cpd_module_2nd_stage+0x5d>

00001401 <multiband_cpd_module_2nd_stage+0xf9>:
    1401:	11a9f0        	slli	a10, a9, 1
    1404:	ffe346        	j	1395 <multiband_cpd_module_2nd_stage+0x8d>

00001407 <multiband_cpd_module_2nd_stage+0xff>:
	...

00001408 <DBB_module>:
    1408:	006136        	entry	a1, 48
    140b:	379344        	ae_truncp24a32x2	aep1, a3, a4
    140e:	019174        	ae_mulfp24s.ll	aeq2, aep1, aep1
    1411:	109134        	ae_mulafs56p24s.hh	aeq2, aep1, aep1
    1414:	01ad           	mov.n	a10, a1
    1416:	366c04        	ae_nsaq56s	a12, aeq2
    1419:	b14b           	addi.n	a11, a1, 4
    141b:	fb4281        	l32r	a8, 124 (270 <HIFI_sqrt_cpd>)
    141e:	fd344f8cce 	{ addi	a12, a12, -8; ae_roundsq32asym	aeq1, aeq2 }
    1423:	341c34        	ae_sllasq56s	aeq0, aeq1, a12
    1426:	60c0c0        	neg	a12, a12
    1429:	f8639401cf 	{ s32i.n	a12, a1, 0; ae_roundsp16q48asym	aep0, aeq0 }
    142e:	0009d4        	ae_cvta32p24.l	a9, aep0
    1431:	1199           	s32i.n	a9, a1, 4
    1433:	0008e0        	callx8	a8
    1436:	11a8           	l32i.n	a10, a1, 4
    1438:	0198           	l32i.n	a9, a1, 0
    143a:	22d8           	l32i.n	a13, a2, 8
    143c:	fb3bc1        	l32r	a12, 128 (3d70a3d <_end+0x3d6e369>)
    143f:	0d2996        	bltz	a9, 1515 <DBB_module+0x10d>
    1442:	401900        	ssl	a9
    1445:	a1ba00        	sll	a11, a10

00001448 <DBB_module+0x40>:
    1448:	32a8           	l32i.n	a10, a2, 12
    144a:	1badb7        	bge	a13, a11, 1469 <DBB_module+0x61>
    144d:	378cc4        	ae_truncp24a32x2	aep0, a12, a12
    1450:	360a34        	ae_cvtq48a32s	aeq0, a10
    1453:	c0eba0        	sub	a14, a11, a10
    1456:	364e34        	ae_cvtq48a32s	aeq1, a14
    1459:	080354        	ae_mularfq32sp24s.l	aeq0, aeq1, aep0
    145c:	ff245411ef 	{ s32i.n	a14, a1, 4; ae_roundsq32asym	aeq3, aeq0 }
    1461:	36ba04        	ae_trunca32q48	a10, aeq3
    1464:	32a9           	s32i.n	a10, a2, 12
    1466:	000086        	j	146c <DBB_module+0x64>

00001469 <DBB_module+0x61>:
    1469:	378cc4        	ae_truncp24a32x2	aep0, a12, a12

0000146c <DBB_module+0x64>:
    146c:	02b8           	l32i.n	a11, a2, 0
    146e:	4298           	l32i.n	a9, a2, 16
    1470:	1288           	l32i.n	a8, a2, 4
    1472:	1daab7        	bge	a10, a11, 1493 <DBB_module+0x8b>
    1475:	21b8b0        	srai	a11, a11, 8
    1478:	352ba7        	blt	a11, a10, 14b1 <DBB_module+0xa9>
    147b:	36c934        	ae_cvtq48a32s	aeq3, a9
    147e:	c08890        	sub	a8, a8, a9
    1481:	f9e2bec83f 	{ ae_cvtq48a32s	aeq0, a8; ae_roundsp16asym	aep2, aep0 }
    1486:	08ab14        	ae_mularfq32sp24s.l	aeq3, aeq0, aep2
    1489:	06b934        	ae_roundsq32asym	aeq2, aeq3
    148c:	36af04        	ae_trunca32q48	a15, aeq2
    148f:	42f9           	s32i.n	a15, a2, 16
    1491:	f01d           	retw.n

00001493 <DBB_module+0x8b>:
    1493:	fb14b1        	l32r	a11, e4 (8000000 <_end+0x7ffd92c>)
    1496:	42c8           	l32i.n	a12, a2, 16
    1498:	368c34        	ae_cvtq48a32s	aeq2, a12
    149b:	c0bbc0        	sub	a11, a11, a12
    149e:	f9e2fecbff 	{ ae_cvtq48a32s	aeq3, a11; ae_roundsp16asym	aep3, aep0 }
    14a3:	08b3d4        	ae_mularfq32sp24s.l	aeq2, aeq3, aep3
    14a6:	066934        	ae_roundsq32asym	aeq1, aeq2
    14a9:	369a04        	ae_trunca32q48	a10, aeq1
    14ac:	42a9           	s32i.n	a10, a2, 16
    14ae:	f01d           	retw.n

000014b0 <DBB_module+0xa8>:
	...

000014b1 <DBB_module+0xa9>:
    14b1:	560114        	ae_sp24x2s.i	aep0, a1, 8
    14b4:	11b9           	s32i.n	a11, a1, 4
    14b6:	fafa81        	l32r	a8, a0 (2d8 <Xdiv>)
    14b9:	a14b           	addi.n	a10, a1, 4
    14bb:	b2cb           	addi.n	a11, a2, 12
    14bd:	0008e0        	callx8	a8
    14c0:	b14b           	addi.n	a11, a1, 4
    14c2:	11da00        	slli	a13, a10, 16
    14c5:	364d34        	ae_cvtq48a32s	aeq1, a13
    14c8:	365c04        	ae_nsaq56s	a12, aeq1
    14cb:	fb1681        	l32r	a8, 124 (270 <HIFI_sqrt_cpd>)
    14ce:	01ad           	mov.n	a10, a1
    14d0:	f8ccc2        	addi	a12, a12, -8
    14d3:	341c34        	ae_sllasq56s	aeq0, aeq1, a12
    14d6:	60c0c0        	neg	a12, a12
    14d9:	fa639401cf 	{ s32i.n	a12, a1, 0; ae_roundsp16q48asym	aep4, aeq0 }
    14de:	0049d4        	ae_cvta32p24.l	a9, aep4
    14e1:	1199           	s32i.n	a9, a1, 4
    14e3:	0008e0        	callx8	a8
    14e6:	0198           	l32i.n	a9, a1, 0
    14e8:	540114        	ae_lp24x2.i	aep0, a1, 8
    14eb:	11a8           	l32i.n	a10, a1, 4
    14ed:	030996        	bltz	a9, 1521 <DBB_module+0x119>
    14f0:	12f8           	l32i.n	a15, a2, 4
    14f2:	401900        	ssl	a9
    14f5:	4288           	l32i.n	a8, a2, 16
    14f7:	36c834        	ae_cvtq48a32s	aeq3, a8
    14fa:	a1ba00        	sll	a11, a10
    14fd:	43ffb0        	min	a15, a15, a11
    1500:	c0ff80        	sub	a15, a15, a8
    1503:	fae2fecf3f 	{ ae_cvtq48a32s	aeq0, a15; ae_roundsp16asym	aep5, aep0 }
    1508:	08db14        	ae_mularfq32sp24s.l	aeq3, aeq0, aep5
    150b:	06b934        	ae_roundsq32asym	aeq2, aeq3
    150e:	36ae04        	ae_trunca32q48	a14, aeq2
    1511:	42e9           	s32i.n	a14, a2, 16
    1513:	f01d           	retw.n

00001515 <DBB_module+0x10d>:
    1515:	608090        	neg	a8, a9
    1518:	400800        	ssr	a8
    151b:	b1b0a0        	sra	a11, a10
    151e:	ffc986        	j	1448 <DBB_module+0x40>

00001521 <DBB_module+0x119>:
    1521:	12f8           	l32i.n	a15, a2, 4
    1523:	60b090        	neg	a11, a9
    1526:	4288           	l32i.n	a8, a2, 16
    1528:	36c834        	ae_cvtq48a32s	aeq3, a8
    152b:	400b00        	ssr	a11
    152e:	b1b0a0        	sra	a11, a10
    1531:	43ffb0        	min	a15, a15, a11
    1534:	c0ff80        	sub	a15, a15, a8
    1537:	fae2fecf3f 	{ ae_cvtq48a32s	aeq0, a15; ae_roundsp16asym	aep5, aep0 }
    153c:	08db14        	ae_mularfq32sp24s.l	aeq3, aeq0, aep5
    153f:	06b934        	ae_roundsq32asym	aeq2, aeq3
    1542:	36ae04        	ae_trunca32q48	a14, aeq2
    1545:	42e9           	s32i.n	a14, a2, 16
    1547:	f01d           	retw.n

00001549 <DBB_module+0x141>:
    1549:	000000                                        ...

0000154c <compander_AU_SetINS_proc>:
    154c:	004136        	entry	a1, 32
    154f:	13d222        	addmi	a2, a2, 0x1300
    1552:	711252        	l16ui	a5, a2, 226
    1555:	838c           	beqz.n	a3, 1561 <compander_AU_SetINS_proc+0x15>
    1557:	240c           	movi.n	a4, 2
    1559:	204540        	or	a4, a5, a4
    155c:	715242        	s16i	a4, a2, 226
    155f:	f01d           	retw.n

00001561 <compander_AU_SetINS_proc+0x15>:
    1561:	faf281        	l32r	a8, 12c (fffd <_end+0xd929>)
    1564:	108580        	and	a8, a5, a8
    1567:	715282        	s16i	a8, a2, 226
    156a:	f01d           	retw.n

0000156c <compander_AU_proc>:
    156c:	016136        	entry	a1, 176
    156f:	206122        	s32i	a2, a1, 128
    1572:	05cd           	mov.n	a12, a5
    1574:	06bd           	mov.n	a11, a6
    1576:	13d2d2        	addmi	a13, a2, 0x1300
    1579:	711d92        	l16ui	a9, a13, 226
    157c:	668b           	addi.n	a6, a6, 8
    157e:	06d252        	addmi	a5, a2, 0x600
    1581:	8f9582        	l16si	a8, a5, 0x11e
    1584:	216460        	srai	a6, a6, 4
    1587:	20c552        	addi	a5, a5, 32
    158a:	04a290        	extui	a10, a9, 2, 1
    158d:	04e190        	extui	a14, a9, 1, 1
    1590:	1c61e2        	s32i	a14, a1, 112
    1593:	f8c882        	addi	a8, a8, -8
    1596:	83b680        	moveqz	a11, a6, a8
    1599:	701d82        	l16ui	a8, a13, 224
    159c:	1d61a2        	s32i	a10, a1, 116
    159f:	e4cd62        	addi	a6, a13, -28
    15a2:	3e6816        	beqz	a8, 198c <compander_AU_proc+0x420>
    15a5:	741da2        	l16ui	a10, a13, 232
    15a8:	109222        	l16si	a2, a2, 32
    15ab:	392dd2        	l32i	a13, a13, 228
    15ae:	f8cae2        	addi	a14, a10, -8
    15b1:	413e16        	beqz	a14, 19c8 <compander_AU_proc+0x45c>
    15b4:	f6ca82        	addi	a8, a10, -10
    15b7:	41f816        	beqz	a8, 19da <compander_AU_proc+0x46e>
    15ba:	072da6        	blti	a13, 2, 15c5 <compander_AU_proc+0x59>
    15bd:	21c6c0        	srai	a12, a12, 6
    15c0:	000106        	j	15c8 <compander_AU_proc+0x5c>

000015c3 <compander_AU_proc+0x57>:
	...

000015c5 <compander_AU_proc+0x59>:
    15c5:	21c5c0        	srai	a12, a12, 5

000015c8 <compander_AU_proc+0x5c>:
    15c8:	1b61c2        	s32i	a12, a1, 108

000015cb <compander_AU_proc+0x5f>:
    15cb:	02e907        	bbsi	a9, 0, 15d1 <compander_AU_proc+0x65>
    15ce:	002786        	j	1670 <compander_AU_proc+0x104>
    15d1:	1b21c2        	l32i	a12, a1, 108
    15d4:	021ce6        	bgei	a12, 1, 15da <compander_AU_proc+0x6e>
    15d7:	004806        	j	16fb <compander_AU_proc+0x18f>
    15da:	202192        	l32i	a9, a1, 128
    15dd:	78c1c2        	addi	a12, a1, 120
    15e0:	360b34        	ae_cvtq48a32s	aeq0, a11
    15e3:	630c24        	ae_sq56s.i	aeq0, a12, 16
    15e6:	14d9f2        	addmi	a15, a9, 0x1400
    15e9:	15d9d2        	addmi	a13, a9, 0x1500
    15ec:	1a61d2        	s32i	a13, a1, 104
    15ef:	07d9c2        	addmi	a12, a9, 0x700
    15f2:	eccfa2        	addi	a10, a15, -20
    15f5:	eccde2        	addi	a14, a13, -20
    15f8:	eacf82        	addi	a8, a15, -22
    15fb:	176182        	s32i	a8, a1, 92
    15fe:	91e9           	s32i.n	a14, a1, 36
    1600:	1e61a2        	s32i	a10, a1, 120
    1603:	a0ccc2        	addi	a12, a12, -96
    1606:	1861c2        	s32i	a12, a1, 96
    1609:	0dd9a2        	addmi	a10, a9, 0xd00
    160c:	f0cff2        	addi	a15, a15, -16
    160f:	0e0c           	movi.n	a14, 0
    1611:	1182e0        	slli	a8, a2, 2
    1614:	f0cdd2        	addi	a13, a13, -16
    1617:	1561d2        	s32i	a13, a1, 84
    161a:	196182        	s32i	a8, a1, 100
    161d:	2161e2        	s32i	a14, a1, 132
    1620:	1f61f2        	s32i	a15, a1, 124
    1623:	40caa2        	addi	a10, a10, 64
    1626:	1161a2        	s32i	a10, a1, 68
    1629:	11f2f0        	slli	a15, a2, 1
    162c:	16d9e2        	addmi	a14, a9, 0x1600
    162f:	f1e9           	s32i.n	a14, a1, 60
    1631:	1261f2        	s32i	a15, a1, 72
    1634:	f0ce82        	addi	a8, a14, -16
    1637:	17d992        	addmi	a9, a9, 0x1700
    163a:	50cec2        	addi	a12, a14, 80
    163d:	1461c2        	s32i	a12, a1, 80
    1640:	a199           	s32i.n	a9, a1, 40
    1642:	d189           	s32i.n	a8, a1, 52
    1644:	f0c9a2        	addi	a10, a9, -16
    1647:	c1a9           	s32i.n	a10, a1, 48
    1649:	10cef2        	addi	a15, a14, 16
    164c:	20ce82        	addi	a8, a14, 32
    164f:	6189           	s32i.n	a8, a1, 24
    1651:	e1f9           	s32i.n	a15, a1, 56
    1653:	a0c992        	addi	a9, a9, -96
    1656:	30cea2        	addi	a10, a14, 48
    1659:	1061a2        	s32i	a10, a1, 64
    165c:	136192        	s32i	a9, a1, 76
    165f:	fe8b           	addi.n	a15, a14, 8
    1661:	71f9           	s32i.n	a15, a1, 28
    1663:	f8ce92        	addi	a9, a14, -8
    1666:	5199           	s32i.n	a9, a1, 20
    1668:	40cee2        	addi	a14, a14, 64
    166b:	81e9           	s32i.n	a14, a1, 32
    166d:	0031c6        	j	1738 <compander_AU_proc+0x1cc>

00001670 <compander_AU_proc+0x104>:
    1670:	1b21d2        	l32i	a13, a1, 108
    1673:	021de6        	bgei	a13, 1, 1679 <compander_AU_proc+0x10d>
    1676:	002046        	j	16fb <compander_AU_proc+0x18f>
    1679:	202182        	l32i	a8, a1, 128
    167c:	78c1a2        	addi	a10, a1, 120
    167f:	364b34        	ae_cvtq48a32s	aeq1, a11
    1682:	0f0c           	movi.n	a15, 0
    1684:	1192e0        	slli	a9, a2, 2
    1687:	196192        	s32i	a9, a1, 100
    168a:	2161f2        	s32i	a15, a1, 132
    168d:	631a24        	ae_sq56s.i	aeq1, a10, 16
    1690:	14d8e2        	addmi	a14, a8, 0x1400
    1693:	b1e9           	s32i.n	a14, a1, 44
    1695:	07d882        	addmi	a8, a8, 0x700
    1698:	eacee2        	addi	a14, a14, -22
    169b:	a0c882        	addi	a8, a8, -96
    169e:	186182        	s32i	a8, a1, 96
    16a1:	1761e2        	s32i	a14, a1, 92

000016a4 <compander_AU_proc+0x138>:
    16a4:	1821a2        	l32i	a10, a1, 96
    16a7:	03bd           	mov.n	a11, a3
    16a9:	04cd           	mov.n	a12, a4
    16ab:	07dd           	mov.n	a13, a7
    16ad:	172192        	l32i	a9, a1, 92
    16b0:	78c182        	addi	a8, a1, 120
    16b3:	0b21f2        	l32i	a15, a1, 44
    16b6:	614824        	ae_lq56.i	aeq1, a8, 16
    16b9:	fa9d81        	l32r	a8, 130 (1284 <multiband_cpd_module>)
    16bc:	60cfb4        	ae_lq32f.i	aeq3, a15, -20
    16bf:	f8ceac090f 	{ ae_lp16f.i	aep0, a9, 0; ae_subsq56s	aeq0, aeq1, aeq3 }
    16c4:	f582f1c1ee 	{ l32i	a14, a1, 112; ae_mulafq32sp16s.l	aeq3, aeq0, aep0 }
    16c9:	1d21f2        	l32i	a15, a1, 116
    16cc:	fe3c40019f 	{ movi	a9, 1; ae_roundsq32asym	aeq2, aeq3 }
    16d1:	36a204        	ae_trunca32q48	a2, aeq2
    16d4:	426622        	s32i	a2, a6, 0x108
    16d7:	2e6522        	s32i	a2, a5, 184
    16da:	006192        	s32i	a9, a1, 0
    16dd:	0008e0        	callx8	a8
    16e0:	1b2182        	l32i	a8, a1, 108
    16e3:	212192        	l32i	a9, a1, 132
    16e6:	1921b2        	l32i	a11, a1, 100
    16e9:	2e25a2        	l32i	a10, a5, 184
    16ec:	2f65a2        	s32i	a10, a5, 188
    16ef:	33ba           	add.n	a3, a3, a11
    16f1:	44ba           	add.n	a4, a4, a11
    16f3:	991b           	addi.n	a9, a9, 1
    16f5:	216192        	s32i	a9, a1, 132
    16f8:	a89897        	bne	a8, a9, 16a4 <compander_AU_proc+0x138>

000016fb <compander_AU_proc+0x18f>:
    16fb:	f01d           	retw.n

000016fd <compander_AU_proc+0x191>:
    16fd:	10c1a2        	addi	a10, a1, 16
    1700:	a1b8           	l32i.n	a11, a1, 40
    1702:	fa6781        	l32r	a8, a0 (2d8 <Xdiv>)
    1705:	2194c0        	srai	a9, a12, 4
    1708:	4199           	s32i.n	a9, a1, 16
    170a:	0008e0        	callx8	a8
    170d:	11ca00        	slli	a12, a10, 16

00001710 <compander_AU_proc+0x1a4>:
    1710:	fa8981        	l32r	a8, 134 (1408 <DBB_module>)
    1713:	c1a8           	l32i.n	a10, a1, 48
    1715:	3325b2        	l32i	a11, a5, 204
    1718:	0008e0        	callx8	a8
    171b:	1b2192        	l32i	a9, a1, 108
    171e:	1921b2        	l32i	a11, a1, 100
    1721:	2121a2        	l32i	a10, a1, 132
    1724:	fec7d2        	addi	a13, a7, -2
    1727:	33ba           	add.n	a3, a3, a11
    1729:	01caa2        	addi	a10, a10, 1
    172c:	80cb40        	add	a12, a11, a4
    172f:	834cd0        	moveqz	a4, a12, a13
    1732:	2161a2        	s32i	a10, a1, 132
    1735:	c219a7        	beq	a9, a10, 16fb <compander_AU_proc+0x18f>

00001738 <compander_AU_proc+0x1cc>:
    1738:	1f21a2        	l32i	a10, a1, 124
    173b:	fa5a81        	l32r	a8, a4 (1ef0 <xt_memcpy>)
    173e:	03bd           	mov.n	a11, a3
    1740:	1221c2        	l32i	a12, a1, 72
    1743:	0008e0        	callx8	a8
    1746:	0e2766        	bnei	a7, 2, 1758 <compander_AU_proc+0x1ec>
    1749:	1521a2        	l32i	a10, a1, 84
    174c:	fa5681        	l32r	a8, a4 (1ef0 <xt_memcpy>)
    174f:	20b440        	or	a11, a4, a4
    1752:	1221c2        	l32i	a12, a1, 72
    1755:	0008e0        	callx8	a8

00001758 <compander_AU_proc+0x1ec>:
    1758:	402692        	l32i	a9, a6, 0x100
    175b:	16a916        	beqz	a9, 18c9 <compander_AU_proc+0x35d>
    175e:	03ad           	mov.n	a10, a3
    1760:	1321b2        	l32i	a11, a1, 76
    1763:	d1c8           	l32i.n	a12, a1, 52
    1765:	fa7481        	l32r	a8, 138 (1d24 <HIFI_d1IB_v2_in32_c24_4th>)
    1768:	e1d8           	l32i.n	a13, a1, 56
    176a:	20e220        	or	a14, a2, a2
    176d:	0008e0        	callx8	a8
    1770:	1f21a2        	l32i	a10, a1, 124
    1773:	1421b2        	l32i	a11, a1, 80
    1776:	f1c8           	l32i.n	a12, a1, 60
    1778:	fa7181        	l32r	a8, 13c (1e64 <HIFI_d1IB_v2_in32_c24_3rd>)
    177b:	1021d2        	l32i	a13, a1, 64
    177e:	20e220        	or	a14, a2, a2
    1781:	0008e0        	callx8	a8
    1784:	222766        	bnei	a7, 2, 17aa <compander_AU_proc+0x23e>
    1787:	04ad           	mov.n	a10, a4
    1789:	1321b2        	l32i	a11, a1, 76
    178c:	51c8           	l32i.n	a12, a1, 20
    178e:	fa6a81        	l32r	a8, 138 (1d24 <HIFI_d1IB_v2_in32_c24_4th>)
    1791:	61d8           	l32i.n	a13, a1, 24
    1793:	02ed           	mov.n	a14, a2
    1795:	0008e0        	callx8	a8
    1798:	1521a2        	l32i	a10, a1, 84
    179b:	1421b2        	l32i	a11, a1, 80
    179e:	71c8           	l32i.n	a12, a1, 28
    17a0:	fa6781        	l32r	a8, 13c (1e64 <HIFI_d1IB_v2_in32_c24_3rd>)
    17a3:	81d8           	l32i.n	a13, a1, 32
    17a5:	02ed           	mov.n	a14, a2
    17a7:	0008e0        	callx8	a8

000017aa <compander_AU_proc+0x23e>:
    17aa:	1e2182        	l32i	a8, a1, 120
    17ad:	fa64a1        	l32r	a10, 140 (4000 <_end+0x192c>)
    17b0:	fcc392        	addi	a9, a3, -4
    17b3:	166192        	s32i	a9, a1, 88
    17b6:	300aa4        	ae_cvtp24a16x2.ll	aep0, a10, a10
    17b9:	11a276        	loopgtz	a2, 17ce <compander_AU_proc+0x262>
    17bc:	601814        	ae_lq32f.iu	aeq0, a8, 4
    17bf:	605914        	ae_lq32f.iu	aeq1, a9, 4
    17c2:	b58044        	ae_mulzasfq32sp16s.hh	aeq2, aeq1, aep0, aeq0, aep0
    17c5:	b10044        	ae_mulzaafq32sp16s.hh	aeq0, aeq1, aep0, aeq0, aep0
    17c8:	622904        	ae_sq32f.i	aeq2, a9, 0
    17cb:	620804        	ae_sq32f.i	aeq0, a8, 0

000017ce <compander_AU_proc+0x262>:
    17ce:	fec7b2        	addi	a11, a7, -2
    17d1:	146b16        	beqz	a11, 191b <compander_AU_proc+0x3af>

000017d4 <compander_AU_proc+0x268>:
    17d4:	1c21e2        	l32i	a14, a1, 112
    17d7:	1d21f2        	l32i	a15, a1, 116
    17da:	fa5581        	l32r	a8, 130 (1284 <multiband_cpd_module>)
    17dd:	090c           	movi.n	a9, 0
    17df:	1e21c2        	l32i	a12, a1, 120
    17e2:	78c1d2        	addi	a13, a1, 120
    17e5:	1721b2        	l32i	a11, a1, 92
    17e8:	614d24        	ae_lq56.i	aeq1, a13, 16
    17eb:	60cc04        	ae_lq32f.i	aeq3, a12, 0
    17ee:	f8ceac1b0f 	{ ae_lp16f.i	aep1, a11, 0; ae_subsq56s	aeq0, aeq1, aeq3 }
    17f3:	f586d3d77f 	{ or	a13, a7, a7; ae_mulafq32sp16s.l	aeq3, aeq0, aep1 }
    17f8:	03bd           	mov.n	a11, a3
    17fa:	fe3c53c44f 	{ or	a12, a4, a4; ae_roundsq32asym	aeq2, aeq3 }
    17ff:	36aa04        	ae_trunca32q48	a10, aeq2
    1802:	4266a2        	s32i	a10, a6, 0x108
    1805:	2e65a2        	s32i	a10, a5, 184
    1808:	0199           	s32i.n	a9, a1, 0
    180a:	1821a2        	l32i	a10, a1, 96
    180d:	0008e0        	callx8	a8
    1810:	1d21f2        	l32i	a15, a1, 116
    1813:	fa4781        	l32r	a8, 130 (1284 <multiband_cpd_module>)
    1816:	090c           	movi.n	a9, 0
    1818:	2021a2        	l32i	a10, a1, 128
    181b:	1a21d2        	l32i	a13, a1, 104
    181e:	2e25c2        	l32i	a12, a5, 184
    1821:	4226e2        	l32i	a14, a6, 0x108
    1824:	37aee4        	ae_truncp24a32x2	aep2, a14, a14
    1827:	2f65c2        	s32i	a12, a5, 188
    182a:	802dd2        	l32i	a13, a13, 0x200
    182d:	360d34        	ae_cvtq48a32s	aeq0, a13
    1830:	f8883151ce 	{ l32i	a12, a1, 84; ae_mulfq32sp24s.l	aeq0, aeq0, aep2 }
    1835:	1c21e2        	l32i	a14, a1, 112
    1838:	36c114        	ae_sllisq56s	aeq3, aeq0, 4
    183b:	fe3c53d77f 	{ or	a13, a7, a7; ae_roundsq32asym	aeq2, aeq3 }
    1840:	36ab04        	ae_trunca32q48	a11, aeq2
    1843:	eab9           	s32i.n	a11, a10, 56
    1845:	1f21b2        	l32i	a11, a1, 124
    1848:	0199           	s32i.n	a9, a1, 0
    184a:	0008e0        	callx8	a8
    184d:	202182        	l32i	a8, a1, 128
    1850:	162192        	l32i	a9, a1, 88
    1853:	fec7b2        	addi	a11, a7, -2
    1856:	e8f8           	l32i.n	a15, a8, 56
    1858:	f8f9           	s32i.n	a15, a8, 60
    185a:	1e21f2        	l32i	a15, a1, 120
    185d:	3c12a6        	blti	a2, 1, 189d <compander_AU_proc+0x331>
    1860:	fa38a1        	l32r	a10, 140 (4000 <_end+0x192c>)
    1863:	300aa4        	ae_cvtp24a16x2.ll	aep0, a10, a10
    1866:	21a120        	srai	a10, a2, 1
    1869:	106207        	bbci	a2, 0, 187d <compander_AU_proc+0x311>
    186c:	60df14        	ae_lq32f.iu	aeq3, a15, 4
    186f:	609914        	ae_lq32f.iu	aeq2, a9, 4
    1872:	b180b4        	ae_mulzaafq32sp16s.hh	aeq2, aeq2, aep0, aeq3, aep0
    1875:	f03d           	nop.n
    1877:	066934        	ae_roundsq32asym	aeq1, aeq2
    187a:	621904        	ae_sq32f.i	aeq1, a9, 0
    187d:	1caa76        	loopgtz	a10, 189d <compander_AU_proc+0x331>
    1880:	600f14        	ae_lq32f.i	aeq0, a15, 4
    1883:	604914        	ae_lq32f.i	aeq1, a9, 4
    1886:	08017d192f 	{ ae_lq32f.iu	aeq0, a9, 8; ae_mulzaafq32sp16s.hh	aeq1, aeq1, aep0, aeq0, aep0 }
    188b:	065934        	ae_roundsq32asym	aeq1, aeq1
    188e:	6219f4        	ae_sq32f.i	aeq1, a9, -4
    1891:	605f24        	ae_lq32f.iu	aeq1, a15, 8
    1894:	b10014        	ae_mulzaafq32sp16s.hh	aeq0, aeq0, aep0, aeq1, aep0
    1897:	060934        	ae_roundsq32asym	aeq0, aeq0
    189a:	620904        	ae_sq32f.i	aeq0, a9, 0

0000189d <compander_AU_proc+0x331>:
    189d:	0a2b16        	beqz	a11, 1943 <compander_AU_proc+0x3d7>

000018a0 <compander_AU_proc+0x334>:
    18a0:	1121a2        	l32i	a10, a1, 68
    18a3:	03bd           	mov.n	a11, a3
    18a5:	04cd           	mov.n	a12, a4
    18a7:	fa2781        	l32r	a8, 144 (1308 <multiband_cpd_module_2nd_stage>)
    18aa:	07dd           	mov.n	a13, a7
    18ac:	0e0c           	movi.n	a14, 0
    18ae:	0008e0        	callx8	a8
    18b1:	1a21a2        	l32i	a10, a1, 104
    18b4:	fa0c91        	l32r	a9, e4 (8000000 <_end+0x7ffd92c>)
    18b7:	2021c2        	l32i	a12, a1, 128
    18ba:	802aa2        	l32i	a10, a10, 0x200
    18bd:	132cc2        	l32i	a12, a12, 76
    18c0:	c0aa90        	sub	a10, a10, a9
    18c3:	e36a56        	bnez	a10, 16fd <compander_AU_proc+0x191>
    18c6:	ff9186        	j	1710 <compander_AU_proc+0x1a4>

000018c9 <compander_AU_proc+0x35d>:
    18c9:	03ad           	mov.n	a10, a3
    18cb:	1321b2        	l32i	a11, a1, 76
    18ce:	d1c8           	l32i.n	a12, a1, 52
    18d0:	fa1a81        	l32r	a8, 138 (1d24 <HIFI_d1IB_v2_in32_c24_4th>)
    18d3:	e1d8           	l32i.n	a13, a1, 56
    18d5:	02ed           	mov.n	a14, a2
    18d7:	0008e0        	callx8	a8
    18da:	1f21a2        	l32i	a10, a1, 124
    18dd:	1421b2        	l32i	a11, a1, 80
    18e0:	f1c8           	l32i.n	a12, a1, 60
    18e2:	fa1681        	l32r	a8, 13c (1e64 <HIFI_d1IB_v2_in32_c24_3rd>)
    18e5:	1021d2        	l32i	a13, a1, 64
    18e8:	02ed           	mov.n	a14, a2
    18ea:	0008e0        	callx8	a8
    18ed:	fec792        	addi	a9, a7, -2
    18f0:	eb6956        	bnez	a9, 17aa <compander_AU_proc+0x23e>
    18f3:	04ad           	mov.n	a10, a4
    18f5:	1321b2        	l32i	a11, a1, 76
    18f8:	51c8           	l32i.n	a12, a1, 20
    18fa:	fa0f81        	l32r	a8, 138 (1d24 <HIFI_d1IB_v2_in32_c24_4th>)
    18fd:	61d8           	l32i.n	a13, a1, 24
    18ff:	02ed           	mov.n	a14, a2
    1901:	0008e0        	callx8	a8
    1904:	1521a2        	l32i	a10, a1, 84
    1907:	1421b2        	l32i	a11, a1, 80
    190a:	71c8           	l32i.n	a12, a1, 28
    190c:	fa0c81        	l32r	a8, 13c (1e64 <HIFI_d1IB_v2_in32_c24_3rd>)
    190f:	0821d2        	l32i	a13, a1, 32
    1912:	20e220        	or	a14, a2, a2
    1915:	0008e0        	callx8	a8
    1918:	ffa386        	j	17aa <compander_AU_proc+0x23e>

0000191b <compander_AU_proc+0x3af>:
    191b:	fcc492        	addi	a9, a4, -4
    191e:	9188           	l32i.n	a8, a1, 36
    1920:	fa08a1        	l32r	a10, 140 (4000 <_end+0x192c>)
    1923:	300aa4        	ae_cvtp24a16x2.ll	aep0, a10, a10
    1926:	11a276        	loopgtz	a2, 193b <compander_AU_proc+0x3cf>
    1929:	601814        	ae_lq32f.iu	aeq0, a8, 4
    192c:	605914        	ae_lq32f.iu	aeq1, a9, 4
    192f:	b58044        	ae_mulzasfq32sp16s.hh	aeq2, aeq1, aep0, aeq0, aep0
    1932:	b10044        	ae_mulzaafq32sp16s.hh	aeq0, aeq1, aep0, aeq0, aep0
    1935:	622904        	ae_sq32f.i	aeq2, a9, 0
    1938:	620804        	ae_sq32f.i	aeq0, a8, 0

0000193b <compander_AU_proc+0x3cf>:
    193b:	ffa546        	j	17d4 <compander_AU_proc+0x268>

0000193e <compander_AU_proc+0x3d2>:
    193e:	00000000 fcc49200                                .....

00001943 <compander_AU_proc+0x3d7>:
    1943:	fcc492        	addi	a9, a4, -4
    1946:	91f8           	l32i.n	a15, a1, 36
    1948:	0212e6        	bgei	a2, 1, 194e <compander_AU_proc+0x3e2>
    194b:	ffd446        	j	18a0 <compander_AU_proc+0x334>
    194e:	21a120        	srai	a10, a2, 1
    1951:	f9fbb1        	l32r	a11, 140 (4000 <_end+0x192c>)
    1954:	300bb4        	ae_cvtp24a16x2.ll	aep0, a11, a11
    1957:	0e6207        	bbci	a2, 0, 1969 <compander_AU_proc+0x3fd>
    195a:	609f14        	ae_lq32f.iu	aeq2, a15, 4
    195d:	605914        	ae_lq32f.iu	aeq1, a9, 4
    1960:	b10864        	ae_mulzaafq32sp16s.hh	aeq1, aeq1, aep0, aeq2, aep0
    1963:	061934        	ae_roundsq32asym	aeq0, aeq1
    1966:	620904        	ae_sq32f.i	aeq0, a9, 0
    1969:	1caa76        	loopgtz	a10, 1989 <compander_AU_proc+0x41d>
    196c:	600f14        	ae_lq32f.i	aeq0, a15, 4
    196f:	604914        	ae_lq32f.i	aeq1, a9, 4
    1972:	08017d192f 	{ ae_lq32f.iu	aeq0, a9, 8; ae_mulzaafq32sp16s.hh	aeq1, aeq1, aep0, aeq0, aep0 }
    1977:	065934        	ae_roundsq32asym	aeq1, aeq1
    197a:	6219f4        	ae_sq32f.i	aeq1, a9, -4
    197d:	605f24        	ae_lq32f.iu	aeq1, a15, 8
    1980:	b10014        	ae_mulzaafq32sp16s.hh	aeq0, aeq0, aep0, aeq1, aep0
    1983:	060934        	ae_roundsq32asym	aeq0, aeq0
    1986:	620904        	ae_sq32f.i	aeq0, a9, 0

00001989 <compander_AU_proc+0x41d>:
    1989:	ffc4c6        	j	18a0 <compander_AU_proc+0x334>

0000198c <compander_AU_proc+0x420>:
    198c:	239c           	beqz.n	a3, 19a2 <compander_AU_proc+0x436>
    198e:	101ca6        	blti	a12, 1, 19a2 <compander_AU_proc+0x436>
    1991:	3f92d2        	l16si	a13, a2, 126
    1994:	401d00        	ssl	a13
    1997:	07ac76        	loopgtz	a12, 19a2 <compander_AU_proc+0x436>
    199a:	0398           	l32i.n	a9, a3, 0
    199c:	a19900        	sll	a9, a9
    199f:	09e394        	ae_s32ip	a9, a3, 4
    19a2:	d55416        	beqz	a4, 16fb <compander_AU_proc+0x18f>
    19a5:	fec7e2        	addi	a14, a7, -2
    19a8:	d4fe56        	bnez	a14, 16fb <compander_AU_proc+0x18f>
    19ab:	021ce6        	bgei	a12, 1, 19b1 <compander_AU_proc+0x445>
    19ae:	ff5246        	j	16fb <compander_AU_proc+0x18f>
    19b1:	2021f2        	l32i	a15, a1, 128
    19b4:	3f9ff2        	l16si	a15, a15, 126
    19b7:	401f00        	ssl	a15
    19ba:	07ac76        	loopgtz	a12, 19c5 <compander_AU_proc+0x459>
    19bd:	0498           	l32i.n	a9, a4, 0
    19bf:	a19900        	sll	a9, a9
    19c2:	09e494        	ae_s32ip	a9, a4, 4

000019c5 <compander_AU_proc+0x459>:
    19c5:	f01d           	retw.n

000019c7 <compander_AU_proc+0x45b>:
	...

000019c8 <compander_AU_proc+0x45c>:
    19c8:	052da6        	blti	a13, 2, 19d1 <compander_AU_proc+0x465>
    19cb:	21c4c0        	srai	a12, a12, 4
    19ce:	000086        	j	19d4 <compander_AU_proc+0x468>

000019d1 <compander_AU_proc+0x465>:
    19d1:	21c3c0        	srai	a12, a12, 3

000019d4 <compander_AU_proc+0x468>:
    19d4:	1b61c2        	s32i	a12, a1, 108
    19d7:	fefc06        	j	15cb <compander_AU_proc+0x5f>

000019da <compander_AU_proc+0x46e>:
    19da:	0a2da6        	blti	a13, 2, 19e8 <compander_AU_proc+0x47c>
    19dd:	4a1c           	movi.n	a10, 20
    19df:	d2aca0        	quos	a10, a12, a10
    19e2:	1b61a2        	s32i	a10, a1, 108
    19e5:	fef886        	j	15cb <compander_AU_proc+0x5f>

000019e8 <compander_AU_proc+0x47c>:
    19e8:	ad0c           	movi.n	a13, 10
    19ea:	d2dcd0        	quos	a13, a12, a13
    19ed:	1b61d2        	s32i	a13, a1, 108
    19f0:	fef5c6        	j	15cb <compander_AU_proc+0x5f>

000019f3 <compander_AU_proc+0x487>:
	...

000019f4 <get_CPD_memsize>:
    19f4:	004136        	entry	a1, 32
    19f7:	828c           	beqz.n	a2, 1a03 <get_CPD_memsize+0xf>
    19f9:	420b           	addi.n	a4, a2, -1
    19fb:	40a132        	movi	a3, 0x140
    19fe:	832340        	moveqz	a2, a3, a4
    1a01:	f01d           	retw.n

00001a03 <get_CPD_memsize+0xf>:
    1a03:	f9d121        	l32r	a2, 148 (1908 <compander_AU_proc+0x39c>)
    1a06:	f01d           	retw.n

00001a08 <get_INS_memsize>:
    1a08:	004136        	entry	a1, 32
    1a0b:	f9d021        	l32r	a2, 14c (8a8 <compander_VO_proc+0xe8>)
    1a0e:	f01d           	retw.n

00001a10 <calc_slope>:
    1a10:	006136        	entry	a1, 48
    1a13:	21b920        	srai	a11, a2, 9
    1a16:	bb2a           	add.n	a11, a11, a2
    1a18:	c0ab30        	sub	a10, a11, a3
    1a1b:	031ae6        	bgei	a10, 1, 1a22 <calc_slope+0x12>
    1a1e:	120c           	movi.n	a2, 1
    1a20:	f01d           	retw.n

00001a22 <calc_slope+0x12>:
    1a22:	f99f81        	l32r	a8, a0 (2d8 <Xdiv>)
    1a25:	378434        	ae_truncp24a32x2	aep0, a4, a3
    1a28:	c0db40        	sub	a13, a11, a4
    1a2b:	36cd34        	ae_cvtq48a32s	aeq3, a13
    1a2e:	fbe17ecabf 	{ ae_cvtq48a32s	aeq2, a10; ae_mulrfq32sp24s.l	aeq3, aeq3, aep0 }
    1a33:	fb6013a11f 	{ or	a10, a1, a1; ae_mulrfq32sp24s.h	aeq2, aeq2, aep0 }
    1a38:	fd3c4041be 	{ addi	a11, a1, 4; ae_roundsq32asym	aeq1, aeq3 }
    1a3d:	fc347f19ef 	{ ae_trunca32q48	a9, aeq1; ae_roundsq32asym	aeq0, aeq2 }
    1a42:	368c04        	ae_trunca32q48	a12, aeq0
    1a45:	01c9           	s32i.n	a12, a1, 0
    1a47:	1199           	s32i.n	a9, a1, 4
    1a49:	0008e0        	callx8	a8
    1a4c:	0a2d           	mov.n	a2, a10
    1a4e:	f01d           	retw.n

00001a50 <calc_slope_1>:
    1a50:	006136        	entry	a1, 48
    1a53:	21a940        	srai	a10, a4, 9
    1a56:	aa4a           	add.n	a10, a10, a4
    1a58:	c0aa20        	sub	a10, a10, a2
    1a5b:	1f1aa6        	blti	a10, 1, 1a7e <calc_slope_1+0x2e>
    1a5e:	c0b320        	sub	a11, a3, a2
    1a61:	041be6        	bgei	a11, 1, 1a69 <calc_slope_1+0x19>
    1a64:	f9bb21        	l32r	a2, 150 (2000 <_DYNAMIC+0x68>)
    1a67:	f01d           	retw.n

00001a69 <calc_slope_1+0x19>:
    1a69:	f98d81        	l32r	a8, a0 (2d8 <Xdiv>)
    1a6c:	01a9           	s32i.n	a10, a1, 0
    1a6e:	2192b0        	srai	a9, a11, 2
    1a71:	1199           	s32i.n	a9, a1, 4
    1a73:	a14b           	addi.n	a10, a1, 4
    1a75:	01bd           	mov.n	a11, a1
    1a77:	0008e0        	callx8	a8
    1a7a:	0a2d           	mov.n	a2, a10
    1a7c:	f01d           	retw.n

00001a7e <calc_slope_1+0x2e>:
    1a7e:	120c           	movi.n	a2, 1
    1a80:	f01d           	retw.n

00001a82 <calc_slope_1+0x32>:
	...

00001a84 <calc_slope_2>:
    1a84:	006136        	entry	a1, 48
    1a87:	f98681        	l32r	a8, a0 (2d8 <Xdiv>)
    1a8a:	21e920        	srai	a14, a2, 9
    1a8d:	378434        	ae_truncp24a32x2	aep0, a4, a3
    1a90:	ee2a           	add.n	a14, a14, a2
    1a92:	c0de40        	sub	a13, a14, a4
    1a95:	c0ee30        	sub	a14, a14, a3
    1a98:	36ce34        	ae_cvtq48a32s	aeq3, a14
    1a9b:	fb617ecdbf 	{ ae_cvtq48a32s	aeq2, a13; ae_mulrfq32sp24s.h	aeq3, aeq3, aep0 }
    1aa0:	fbe013a11f 	{ or	a10, a1, a1; ae_mulrfq32sp24s.l	aeq2, aeq2, aep0 }
    1aa5:	fd3c4041be 	{ addi	a11, a1, 4; ae_roundsq32asym	aeq1, aeq3 }
    1aaa:	fc347f19ef 	{ ae_trunca32q48	a9, aeq1; ae_roundsq32asym	aeq0, aeq2 }
    1aaf:	368c04        	ae_trunca32q48	a12, aeq0
    1ab2:	11c9           	s32i.n	a12, a1, 4
    1ab4:	219290        	srai	a9, a9, 2
    1ab7:	0199           	s32i.n	a9, a1, 0
    1ab9:	0008e0        	callx8	a8
    1abc:	0a2d           	mov.n	a2, a10
    1abe:	f01d           	retw.n

00001ac0 <INS_SetFrame120_Init>:
    1ac0:	004136        	entry	a1, 32
    1ac3:	08d242        	addmi	a4, a2, 0x800
    1ac6:	a30c           	movi.n	a3, 10
    1ac8:	535432        	s16i	a3, a4, 166
    1acb:	f01d           	retw.n

00001acd <INS_SetFrame120_Init+0xd>:
    1acd:	000000                                        ...

00001ad0 <INS_Init>:
    1ad0:	004136        	entry	a1, 32
    1ad3:	040392        	l8ui	a9, a3, 4
    1ad6:	0203b2        	l8ui	a11, a3, 2
    1ad9:	0303a2        	l8ui	a10, a3, 3
    1adc:	08d242        	addmi	a4, a2, 0x800
    1adf:	0003c2        	l8ui	a12, a3, 0
    1ae2:	050382        	l8ui	a8, a3, 5
    1ae5:	5054c2        	s16i	a12, a4, 160
    1ae8:	50a3c2        	movi	a12, 0x350
    1aeb:	118880        	slli	a8, a8, 8
    1aee:	11aa80        	slli	a10, a10, 8
    1af1:	20aab0        	or	a10, a10, a11
    1af4:	208890        	or	a8, a8, a9
    1af7:	00a0b2        	movi	a11, 0
    1afa:	888d           	ae_sext16	a8, a8
    1afc:	8aad           	ae_sext16	a10, a10
    1afe:	5154a2        	s16i	a10, a4, 162
    1b01:	525482        	s16i	a8, a4, 164
    1b04:	f96981        	l32r	a8, a8 (1ed8 <xt_memset>)
    1b07:	20a220        	or	a10, a2, a2
    1b0a:	a4c442        	addi	a4, a4, -92
    1b0d:	0008e0        	callx8	a8
    1b10:	8194d2        	l16si	a13, a4, 0x102
    1b13:	0803f2        	l8ui	a15, a3, 8
    1b16:	090382        	l8ui	a8, a3, 9
    1b19:	0a03a2        	l8ui	a10, a3, 10
    1b1c:	0b03e2        	l8ui	a14, a3, 11
    1b1f:	070392        	l8ui	a9, a3, 7
    1b22:	0603b2        	l8ui	a11, a3, 6
    1b25:	11ee80        	slli	a14, a14, 8
    1b28:	119980        	slli	a9, a9, 8
    1b2b:	2099b0        	or	a9, a9, a11
    1b2e:	20eea0        	or	a14, a14, a10
    1b31:	11ee80        	slli	a14, a14, 8
    1b34:	899d           	ae_sext16	a9, a9
    1b36:	425292        	s16i	a9, a2, 132
    1b39:	20ee80        	or	a14, a14, a8
    1b3c:	11ee80        	slli	a14, a14, 8
    1b3f:	20eef0        	or	a14, a14, a15
    1b42:	2062e2        	s32i	a14, a2, 128
    1b45:	359d26        	beqi	a13, 10, 1b7e <INS_Init+0xae>
    1b48:	60a0f2        	movi	a15, 96
    1b4b:	082c           	movi.n	a8, 32
    1b4d:	00a492        	movi	a9, 0x400
    1b50:	115292        	s16i	a9, a2, 34
    1b53:	105282        	s16i	a8, a2, 32
    1b56:	b2f9           	s32i.n	a15, a2, 44

00001b58 <INS_Init+0x88>:
    1b58:	f96e81        	l32r	a8, 110 (38c <AU_INS_coef_init>)
    1b5b:	01d2b2        	addmi	a11, a2, 0x100
    1b5e:	f963a1        	l32r	a10, ec (7fffffff <_end+0x7fffd92b>)
    1b61:	490c           	movi.n	a9, 4
    1b63:	f960c1        	l32r	a12, e4 (8000000 <_end+0x7ffd92c>)
    1b66:	f2c9           	s32i.n	a12, a2, 60
    1b68:	e2c9           	s32i.n	a12, a2, 56
    1b6a:	3f5292        	s16i	a9, a2, 126
    1b6d:	1062a2        	s32i	a10, a2, 64
    1b70:	a0cbb2        	addi	a11, a11, -96
    1b73:	c2b9           	s32i.n	a11, a2, 48
    1b75:	d2b9           	s32i.n	a11, a2, 52
    1b77:	02ad           	mov.n	a10, a2
    1b79:	0008e0        	callx8	a8
    1b7c:	f01d           	retw.n

00001b7e <INS_Init+0xae>:
    1b7e:	ed1c           	movi.n	a13, 30
    1b80:	ae0c           	movi.n	a14, 10
    1b82:	f94bf1        	l32r	a15, b0 (ccd <compander_AU_init+0x11>)
    1b85:	1152f2        	s16i	a15, a2, 34
    1b88:	1052e2        	s16i	a14, a2, 32
    1b8b:	b2d9           	s32i.n	a13, a2, 44
    1b8d:	fff1c6        	j	1b58 <INS_Init+0x88>

00001b90 <INS_Prcs>:
    1b90:	008136        	entry	a1, 64
    1b93:	08d2a2        	addmi	a10, a2, 0x800
    1b96:	509a82        	l16si	a8, a10, 160
    1b99:	a4ca92        	addi	a9, a10, -92
    1b9c:	539ab2        	l16si	a11, a10, 166
    1b9f:	02e807        	bbsi	a8, 0, 1ba5 <INS_Prcs+0x15>
    1ba2:	0021c6        	j	1c2d <INS_Prcs+0x9d>
    1ba5:	1092a2        	l16si	a10, a2, 32
    1ba8:	029b66        	bnei	a11, 10, 1bae <INS_Prcs+0x1e>
    1bab:	002046        	j	1c30 <INS_Prcs+0xa0>
    1bae:	217550        	srai	a7, a5, 5

00001bb1 <INS_Prcs+0x21>:
    1bb1:	7817a6        	blti	a7, 1, 1c2d <INS_Prcs+0x9d>
    1bb4:	050c           	movi.n	a5, 0
    1bb6:	4199           	s32i.n	a9, a1, 16
    1bb8:	11cae0        	slli	a12, a10, 2
    1bbb:	51c9           	s32i.n	a12, a1, 20
    1bbd:	000c86        	j	1bf3 <INS_Prcs+0x63>

00001bc0 <INS_Prcs+0x30>:
	...

00001bc1 <INS_Prcs+0x31>:
    1bc1:	03bd           	mov.n	a11, a3
    1bc3:	04cd           	mov.n	a12, a4
    1bc5:	06ed           	mov.n	a14, a6
    1bc7:	41f8           	l32i.n	a15, a1, 16
    1bc9:	1022a2        	l32i	a10, a2, 64
    1bcc:	080c           	movi.n	a8, 0
    1bce:	7f9f92        	l16si	a9, a15, 254
    1bd1:	72aad0        	salt	a10, a10, a13
    1bd4:	809ff2        	l16si	a15, a15, 0x100
    1bd7:	0189           	s32i.n	a8, a1, 0
    1bd9:	1189           	s32i.n	a8, a1, 4
    1bdb:	f95181        	l32r	a8, 120 (f78 <F_CPD_Intp_Out>)
    1bde:	93f9a0        	movnez	a15, a9, a10
    1be1:	8ffd           	ae_sext16	a15, a15
    1be3:	02ad           	mov.n	a10, a2
    1be5:	0008e0        	callx8	a8
    1be8:	5188           	l32i.n	a8, a1, 20
    1bea:	551b           	addi.n	a5, a5, 1
    1bec:	338a           	add.n	a3, a3, a8
    1bee:	448a           	add.n	a4, a4, a8
    1bf0:	391757        	beq	a7, a5, 1c2d <INS_Prcs+0x9d>

00001bf3 <INS_Prcs+0x63>:
    1bf3:	02ad           	mov.n	a10, a2
    1bf5:	03bd           	mov.n	a11, a3
    1bf7:	04cd           	mov.n	a12, a4
    1bf9:	f94781        	l32r	a8, 118 (da8 <F_CPD_Mag_In>)
    1bfc:	06dd           	mov.n	a13, a6
    1bfe:	1e0c           	movi.n	a14, 1
    1c00:	0008e0        	callx8	a8
    1c03:	2022a2        	l32i	a10, a2, 128
    1c06:	2322b2        	l32i	a11, a2, 140
    1c09:	f938d1        	l32r	a13, ec (7fffffff <_end+0x7fffd92b>)
    1c0c:	909aa0        	addx2	a9, a10, a10
    1c0f:	aeab97        	bge	a11, a9, 1bc1 <INS_Prcs+0x31>
    1c12:	0a2ab7        	blt	a10, a11, 1c20 <INS_Prcs+0x90>
    1c15:	4292d2        	l16si	a13, a2, 132
    1c18:	11dd00        	slli	a13, a13, 16
    1c1b:	ffe886        	j	1bc1 <INS_Prcs+0x31>

00001c1e <INS_Prcs+0x8e>:
	...

00001c20 <INS_Prcs+0x90>:
    1c20:	f93d81        	l32r	a8, 114 (d4c <F_CPD_Ins_Gain>)
    1c23:	02ad           	mov.n	a10, a2
    1c25:	0008e0        	callx8	a8
    1c28:	0add           	mov.n	a13, a10
    1c2a:	ffe4c6        	j	1bc1 <INS_Prcs+0x31>

00001c2d <INS_Prcs+0x9d>:
    1c2d:	f01d           	retw.n

00001c2f <INS_Prcs+0x9f>:
	...

00001c30 <INS_Prcs+0xa0>:
    1c30:	a70c           	movi.n	a7, 10
    1c32:	d27570        	quos	a7, a5, a7
    1c35:	ffde06        	j	1bb1 <INS_Prcs+0x21>

00001c38 <__do_global_ctors_aux>:
    1c38:	004136        	entry	a1, 32
    1c3b:	f94681        	l32r	a8, 154 (2404 <_DYNAMIC+0x46c>)
    1c3e:	7f2882        	l32i	a8, a8, 0x1fc
    1c41:	f94521        	l32r	a2, 158 (2600 <__CTOR_LIST___47>)
    1c44:	0c0826        	beqi	a8, -1, 1c54 <__do_global_ctors_aux+0x1c>
    1c47:	02a8           	l32i.n	a10, a2, 0

00001c49 <__do_global_ctors_aux+0x11>:
    1c49:	000ae0        	callx8	a10
    1c4c:	fcc222        	addi	a2, a2, -4
    1c4f:	02a8           	l32i.n	a10, a2, 0
    1c51:	f40a66        	bnei	a10, -1, 1c49 <__do_global_ctors_aux+0x11>

00001c54 <__do_global_ctors_aux+0x1c>:
    1c54:	f01d           	retw.n

00001c56 <__do_global_ctors_aux+0x1e>:
	...

00001c58 <HIFI_d1IB_v2_in32_c16_4th>:
    1c58:	004136        	entry	a1, 32
    1c5b:	444304        	ae_lp16x2f.i	aep4, a3, 0
    1c5e:	443314        	ae_lp16x2f.i	aep3, a3, 4
    1c61:	442324        	ae_lp16x2f.i	aep2, a3, 8
    1c64:	fcc222        	addi	a2, a2, -4
    1c67:	028d           	mov.n	a8, a2
    1c69:	52a676        	loopgtz	a6, 1cbf <HIFI_d1IB_v2_in32_c16_4th+0x67>
    1c6c:	60c414        	ae_lq32f.i	aeq3, a4, 4
    1c6f:	600404        	ae_lq32f.i	aeq0, a4, 0
    1c72:	609814        	ae_lq32f.iu	aeq2, a8, 4
    1c75:	0b8c39a40f 	{ ae_sq32f.i	aeq2, a4, 0; ae_mulzaafq32sp16s.hh	aeq2, aeq2, aep3, aeq0, aep4 }
    1c7a:	f512b9841f 	{ ae_sq32f.i	aeq0, a4, 4; ae_mulafq32sp16s.h	aeq2, aeq0, aep4 }
    1c7f:	f793bd250f 	{ ae_lq32f.i	aeq1, a5, 0; ae_mulafq32sp16s.l	aeq2, aeq3, aep4 }
    1c84:	03a044        	ae_mulafq32sp16s.h	aeq2, aeq1, aep2
    1c87:	f50bbd051f 	{ ae_lq32f.i	aeq0, a5, 4; ae_mulafq32sp16s.h	aeq2, aeq1, aep2 }
    1c8c:	03a014        	ae_mulafq32sp16s.l	aeq2, aeq0, aep2
    1c8f:	445334        	ae_lp16x2f.i	aep5, a3, 12
    1c92:	fe546d634f 	{ ae_lp16x2f.i	aep6, a3, 16; ae_roundsq32sym	aeq2, aeq2 }
    1c97:	b1ed94        	ae_mulzaafq32sp16s.hh	aeq3, aeq2, aep6, aeq1, aep5
    1c9a:	f517ed135f 	{ ae_lp16x2f.i	aep1, a3, 20; ae_mulafq32sp16s.h	aeq3, aeq1, aep5 }
    1c9f:	f596fd052f 	{ ae_lq32f.i	aeq0, a5, 8; ae_mulafq32sp16s.l	aeq3, aeq0, aep5 }
    1ca4:	f506f9951f 	{ ae_sq32f.i	aeq1, a5, 4; ae_mulafq32sp16s.h	aeq3, aeq0, aep1 }
    1ca9:	f506fd253f 	{ ae_lq32f.i	aeq1, a5, 12; ae_mulafq32sp16s.h	aeq3, aeq0, aep1 }
    1cae:	039854        	ae_mulafq32sp16s.l	aeq3, aeq1, aep1
    1cb1:	622504        	ae_sq32f.i	aeq2, a5, 0
    1cb4:	fe5c79853f 	{ ae_sq32f.i	aeq0, a5, 12; ae_roundsq32sym	aeq2, aeq3 }
    1cb9:	622524        	ae_sq32f.i	aeq2, a5, 8
    1cbc:	626214        	ae_sq32f.iu	aeq2, a2, 4

00001cbf <HIFI_d1IB_v2_in32_c16_4th+0x67>:
    1cbf:	f01d           	retw.n

00001cc1 <HIFI_d1IB_v2_in32_c16_4th+0x69>:
    1cc1:	000000                                        ...

00001cc4 <HIFI_d1IB_v2_in24_c24_4th>:
    1cc4:	004136        	entry	a1, 32
    1cc7:	fcc222        	addi	a2, a2, -4
    1cca:	067d           	mov.n	a7, a6
    1ccc:	049d           	mov.n	a9, a4
    1cce:	056d           	mov.n	a6, a5
    1cd0:	034d           	mov.n	a4, a3
    1cd2:	095d           	mov.n	a5, a9
    1cd4:	023d           	mov.n	a3, a2
    1cd6:	47a776        	loopgtz	a7, 1d21 <HIFI_d1IB_v2_in24_c24_4th+0x5d>
    1cd9:	540424        	ae_lp24x2.i	aep0, a4, 16
    1cdc:	5c1504        	ae_lp24x2f.i	aep1, a5, 0
    1cdf:	4ca214        	ae_lp24.iu	aep2, a2, 4
    1ce2:	dd2970240f 	{ ae_lp24x2.i	aep2, a4, 0; ae_selp24.hh	aep3, aep2, aep1 }
    1ce7:	f046b0141f 	{ ae_lp24x2.i	aep1, a4, 8; ae_mulzaafp24s.hh.ll	aeq0, aep1, aep2 }
    1cec:	e22c2f160f 	{ ae_lp24x2f.i	aep1, a6, 0; ae_mulaafp24s.hh.ll	aeq0, aep3, aep1 }
    1cf1:	e20436350f 	{ ae_sp24x2f.i	aep3, a5, 0; ae_mulaafp24s.hh.ll	aeq0, aep1, aep0 }
    1cf6:	e20630045f 	{ ae_lp24x2.i	aep0, a4, 40; ae_mulafp24s.hh	aeq0, aep1, aep0 }
    1cfb:	542434        	ae_lp24x2.i	aep2, a4, 24
    1cfe:	fae7b0344f 	{ ae_lp24x2.i	aep3, a4, 32; ae_roundsp24q48sym	aep4, aeq0 }
    1d03:	f046be414f 	{ ae_selp24.hh	aep1, aep4, aep1; ae_mulzaafp24s.hh.ll	aeq0, aep1, aep2 }
    1d08:	e2642f261f 	{ ae_lp24x2f.i	aep2, a6, 8; ae_mulaafp24s.hh.ll	aeq0, aep1, aep3 }
    1d0d:	e20836160f 	{ ae_sp24x2f.i	aep1, a6, 0; ae_mulaafp24s.hh.ll	aeq0, aep2, aep0 }
    1d12:	012004        	ae_mulafp24s.hh	aeq0, aep2, aep0
    1d15:	1b8c04        	ae_roundsp24q48sym	aep0, aeq0
    1d18:	1a8a94        	ae_selp24.hh	aep1, aep0, aep2
    1d1b:	5e1614        	ae_sp24x2f.i	aep1, a6, 8
    1d1e:	4e8314        	ae_sp24s.l.iu	aep0, a3, 4

00001d21 <HIFI_d1IB_v2_in24_c24_4th+0x5d>:
    1d21:	f01d           	retw.n

00001d23 <HIFI_d1IB_v2_in24_c24_4th+0x5f>:
	...

00001d24 <HIFI_d1IB_v2_in32_c24_4th>:
    1d24:	004136        	entry	a1, 32
    1d27:	544304        	ae_lp24x2.i	aep4, a3, 0
    1d2a:	543314        	ae_lp24x2.i	aep3, a3, 8
    1d2d:	542324        	ae_lp24x2.i	aep2, a3, 16
    1d30:	fcc222        	addi	a2, a2, -4
    1d33:	028d           	mov.n	a8, a2
    1d35:	56a676        	loopgtz	a6, 1d8f <HIFI_d1IB_v2_in32_c24_4th+0x6b>
    1d38:	600404        	ae_lq32f.i	aeq0, a4, 0
    1d3b:	609814        	ae_lq32f.iu	aeq2, a8, 4
    1d3e:	fb0c39a40f 	{ ae_sq32f.i	aeq2, a4, 0; ae_mulfq32sp24s.h	aeq2, aeq2, aep3 }
    1d43:	f552bd641f 	{ ae_lq32f.i	aeq3, a4, 4; ae_mulafq32sp24s.h	aeq2, aeq0, aep4 }
    1d48:	f552b9841f 	{ ae_sq32f.i	aeq0, a4, 4; ae_mulafq32sp24s.h	aeq2, aeq0, aep4 }
    1d4d:	f7d3bd250f 	{ ae_lq32f.i	aeq1, a5, 0; ae_mulafq32sp24s.l	aeq2, aeq3, aep4 }
    1d52:	08a264        	ae_mulafq32sp24s.h	aeq2, aeq1, aep2
    1d55:	f54bbd051f 	{ ae_lq32f.i	aeq0, a5, 4; ae_mulafq32sp24s.h	aeq2, aeq1, aep2 }
    1d5a:	08a324        	ae_mulafq32sp24s.l	aeq2, aeq0, aep2
    1d5d:	fe5470634f 	{ ae_lp24x2.i	aep6, a3, 32; ae_roundsq32sym	aeq2, aeq2 }
    1d62:	fb1870533f 	{ ae_lp24x2.i	aep5, a3, 24; ae_mulfq32sp24s.h	aeq3, aeq2, aep6 }
    1d67:	08da64        	ae_mulafq32sp24s.h	aeq3, aeq1, aep5
    1d6a:	f557f0135f 	{ ae_lp24x2.i	aep1, a3, 40; ae_mulafq32sp24s.h	aeq3, aeq1, aep5 }
    1d6f:	f5d6fd052f 	{ ae_lq32f.i	aeq0, a5, 8; ae_mulafq32sp24s.l	aeq3, aeq0, aep5 }
    1d74:	f546f9951f 	{ ae_sq32f.i	aeq1, a5, 4; ae_mulafq32sp24s.h	aeq3, aeq0, aep1 }
    1d79:	f546fd253f 	{ ae_lq32f.i	aeq1, a5, 12; ae_mulafq32sp24s.h	aeq3, aeq0, aep1 }
    1d7e:	089b64        	ae_mulafq32sp24s.l	aeq3, aeq1, aep1
    1d81:	622504        	ae_sq32f.i	aeq2, a5, 0
    1d84:	fe5c79853f 	{ ae_sq32f.i	aeq0, a5, 12; ae_roundsq32sym	aeq2, aeq3 }
    1d89:	622524        	ae_sq32f.i	aeq2, a5, 8
    1d8c:	626214        	ae_sq32f.iu	aeq2, a2, 4

00001d8f <HIFI_d1IB_v2_in32_c24_4th+0x6b>:
    1d8f:	f01d           	retw.n

00001d91 <HIFI_d1IB_v2_in32_c24_4th+0x6d>:
    1d91:	00000000 36000000                                .......

00001d98 <HIFI_d1IB_v2_in32_c16_3rd>:
    1d98:	004136        	entry	a1, 32
    1d9b:	442304        	ae_lp16x2f.i	aep2, a3, 0
    1d9e:	443314        	ae_lp16x2f.i	aep3, a3, 4
    1da1:	444324        	ae_lp16x2f.i	aep4, a3, 8
    1da4:	fcc222        	addi	a2, a2, -4
    1da7:	067d           	mov.n	a7, a6
    1da9:	049d           	mov.n	a9, a4
    1dab:	056d           	mov.n	a6, a5
    1dad:	034d           	mov.n	a4, a3
    1daf:	205990        	or	a5, a9, a9
    1db2:	203220        	or	a3, a2, a2
    1db5:	47a776        	loopgtz	a7, 1e00 <HIFI_d1IB_v2_in32_c16_3rd+0x68>
    1db8:	600504        	ae_lq32f.i	aeq0, a5, 0
    1dbb:	605214        	ae_lq32f.iu	aeq1, a2, 4
    1dbe:	094d3d651f 	{ ae_lq32f.i	aeq3, a5, 4; ae_mulzaafq32sp16s.hh	aeq2, aeq1, aep3, aeq0, aep2 }
    1dc3:	f50ab9950f 	{ ae_sq32f.i	aeq1, a5, 0; ae_mulafq32sp16s.h	aeq2, aeq0, aep2 }
    1dc8:	f78bb9851f 	{ ae_sq32f.i	aeq0, a5, 4; ae_mulafq32sp16s.l	aeq2, aeq3, aep2 }
    1dcd:	600604        	ae_lq32f.i	aeq0, a6, 0
    1dd0:	f512bd261f 	{ ae_lq32f.i	aeq1, a6, 4; ae_mulafq32sp16s.h	aeq2, aeq0, aep4 }
    1dd5:	f512ad044f 	{ ae_lp16x2f.i	aep0, a4, 16; ae_mulafq32sp16s.h	aeq2, aeq0, aep4 }
    1dda:	f593ad143f 	{ ae_lp16x2f.i	aep1, a4, 12; ae_mulafq32sp16s.l	aeq2, aeq1, aep4 }
    1ddf:	604624        	ae_lq32f.i	aeq1, a6, 8
    1de2:	ff5479963f 	{ ae_sq32f.i	aeq1, a6, 12; ae_roundsq32sym	aeq3, aeq2 }
    1de7:	0b0539861f 	{ ae_sq32f.i	aeq0, a6, 4; ae_mulzaafq32sp16s.hh	aeq2, aeq3, aep1, aeq0, aep0 }
    1dec:	f502b9b60f 	{ ae_sq32f.i	aeq3, a6, 0; ae_mulafq32sp16s.h	aeq2, aeq0, aep0 }
    1df1:	038054        	ae_mulafq32sp16s.l	aeq2, aeq1, aep0
    1df4:	038054        	ae_mulafq32sp16s.l	aeq2, aeq1, aep0
    1df7:	062834        	ae_roundsq32sym	aeq0, aeq2
    1dfa:	620624        	ae_sq32f.i	aeq0, a6, 8
    1dfd:	624314        	ae_sq32f.iu	aeq0, a3, 4

00001e00 <HIFI_d1IB_v2_in32_c16_3rd+0x68>:
    1e00:	f01d           	retw.n

00001e02 <HIFI_d1IB_v2_in32_c16_3rd+0x6a>:
	...

00001e04 <HIFI_d1IB_v2_in24_c24_3rd>:
    1e04:	004136        	entry	a1, 32
    1e07:	fcc222        	addi	a2, a2, -4
    1e0a:	067d           	mov.n	a7, a6
    1e0c:	049d           	mov.n	a9, a4
    1e0e:	056d           	mov.n	a6, a5
    1e10:	034d           	mov.n	a4, a3
    1e12:	095d           	mov.n	a5, a9
    1e14:	023d           	mov.n	a3, a2
    1e16:	46a776        	loopgtz	a7, 1e60 <HIFI_d1IB_v2_in24_c24_3rd+0x5c>
    1e19:	540424        	ae_lp24x2.i	aep0, a4, 16
    1e1c:	5c1504        	ae_lp24x2f.i	aep1, a5, 0
    1e1f:	4ca214        	ae_lp24.iu	aep2, a2, 4
    1e22:	dd2970240f 	{ ae_lp24x2.i	aep2, a4, 0; ae_selp24.hh	aep3, aep2, aep1 }
    1e27:	f046b0141f 	{ ae_lp24x2.i	aep1, a4, 8; ae_mulzaafp24s.hh.ll	aeq0, aep1, aep2 }
    1e2c:	e22c2f160f 	{ ae_lp24x2f.i	aep1, a6, 0; ae_mulaafp24s.hh.ll	aeq0, aep3, aep1 }
    1e31:	e20436350f 	{ ae_sp24x2f.i	aep3, a5, 0; ae_mulaafp24s.hh.ll	aeq0, aep1, aep0 }
    1e36:	e20630043f 	{ ae_lp24x2.i	aep0, a4, 24; ae_mulafp24s.hh	aeq0, aep1, aep0 }
    1e3b:	5c2614        	ae_lp24x2f.i	aep2, a6, 8
    1e3e:	1b8c34        	ae_roundsp24q48sym	aep3, aeq0
    1e41:	1ab994        	ae_selp24.hh	aep1, aep3, aep1
    1e44:	f006b6160f 	{ ae_sp24x2f.i	aep1, a6, 0; ae_mulzaafp24s.hh.ll	aeq0, aep1, aep0 }
    1e49:	e207b0044f 	{ ae_lp24x2.i	aep0, a4, 32; ae_mulafp24s.ll	aeq0, aep1, aep0 }
    1e4e:	012014        	ae_mulafp24s.hl	aeq0, aep2, aep0
    1e51:	012014        	ae_mulafp24s.hl	aeq0, aep2, aep0
    1e54:	1b8c04        	ae_roundsp24q48sym	aep0, aeq0
    1e57:	1a8a94        	ae_selp24.hh	aep1, aep0, aep2
    1e5a:	5e1614        	ae_sp24x2f.i	aep1, a6, 8
    1e5d:	4e8314        	ae_sp24s.l.iu	aep0, a3, 4

00001e60 <HIFI_d1IB_v2_in24_c24_3rd+0x5c>:
    1e60:	f01d           	retw.n

00001e62 <HIFI_d1IB_v2_in24_c24_3rd+0x5e>:
	...

00001e64 <HIFI_d1IB_v2_in32_c24_3rd>:
    1e64:	004136        	entry	a1, 32
    1e67:	542304        	ae_lp24x2.i	aep2, a3, 0
    1e6a:	543314        	ae_lp24x2.i	aep3, a3, 8
    1e6d:	544324        	ae_lp24x2.i	aep4, a3, 16
    1e70:	fcc222        	addi	a2, a2, -4
    1e73:	067d           	mov.n	a7, a6
    1e75:	049d           	mov.n	a9, a4
    1e77:	056d           	mov.n	a6, a5
    1e79:	034d           	mov.n	a4, a3
    1e7b:	205990        	or	a5, a9, a9
    1e7e:	203220        	or	a3, a2, a2
    1e81:	4ca776        	loopgtz	a7, 1ed1 <HIFI_d1IB_v2_in32_c24_3rd+0x6d>
    1e84:	600504        	ae_lq32f.i	aeq0, a5, 0
    1e87:	605214        	ae_lq32f.iu	aeq1, a2, 4
    1e8a:	f90d3d651f 	{ ae_lq32f.i	aeq3, a5, 4; ae_mulfq32sp24s.h	aeq2, aeq1, aep3 }
    1e8f:	f54ab9950f 	{ ae_sq32f.i	aeq1, a5, 0; ae_mulafq32sp24s.h	aeq2, aeq0, aep2 }
    1e94:	f54ab9851f 	{ ae_sq32f.i	aeq0, a5, 4; ae_mulafq32sp24s.h	aeq2, aeq0, aep2 }
    1e99:	f7cbbd060f 	{ ae_lq32f.i	aeq0, a6, 0; ae_mulafq32sp24s.l	aeq2, aeq3, aep2 }
    1e9e:	f552bd261f 	{ ae_lq32f.i	aeq1, a6, 4; ae_mulafq32sp24s.h	aeq2, aeq0, aep4 }
    1ea3:	f552b0043f 	{ ae_lp24x2.i	aep0, a4, 24; ae_mulafq32sp24s.h	aeq2, aeq0, aep4 }
    1ea8:	f5d3b0144f 	{ ae_lp24x2.i	aep1, a4, 32; ae_mulafq32sp24s.l	aeq2, aeq1, aep4 }
    1ead:	604624        	ae_lq32f.i	aeq1, a6, 8
    1eb0:	ff5479963f 	{ ae_sq32f.i	aeq1, a6, 12; ae_roundsq32sym	aeq3, aeq2 }
    1eb5:	fb0139861f 	{ ae_sq32f.i	aeq0, a6, 4; ae_mulfq32sp24s.h	aeq2, aeq3, aep0 }
    1eba:	f546b9b60f 	{ ae_sq32f.i	aeq3, a6, 0; ae_mulafq32sp24s.h	aeq2, aeq0, aep1 }
    1ebf:	089224        	ae_mulafq32sp24s.h	aeq2, aeq0, aep1
    1ec2:	089364        	ae_mulafq32sp24s.l	aeq2, aeq1, aep1
    1ec5:	089364        	ae_mulafq32sp24s.l	aeq2, aeq1, aep1
    1ec8:	062834        	ae_roundsq32sym	aeq0, aeq2
    1ecb:	620624        	ae_sq32f.i	aeq0, a6, 8
    1ece:	624314        	ae_sq32f.iu	aeq0, a3, 4

00001ed1 <HIFI_d1IB_v2_in32_c24_3rd+0x6d>:
    1ed1:	f01d           	retw.n

00001ed3 <HIFI_d1IB_v2_in32_c24_3rd+0x6f>:
    1ed3:	00000000 00413600                                .....

00001ed8 <xt_memset>:
    1ed8:	004136        	entry	a1, 32
    1edb:	026d           	mov.n	a6, a2
    1edd:	828c           	beqz.n	a2, 1ee9 <xt_memset+0x11>
    1edf:	029476        	loopnez	a4, 1ee5 <xt_memset+0xd>
    1ee2:	09c234        	ae_s16ip	a3, a2, 2

00001ee5 <xt_memset+0xd>:
    1ee5:	062d           	mov.n	a2, a6
    1ee7:	f01d           	retw.n

00001ee9 <xt_memset+0x11>:
    1ee9:	020c           	movi.n	a2, 0
    1eeb:	f01d           	retw.n

00001eed <xt_memset+0x15>:
    1eed:	000000                                        ...

00001ef0 <xt_memcpy>:
    1ef0:	004136        	entry	a1, 32
    1ef3:	027d           	mov.n	a7, a2
    1ef5:	032d           	mov.n	a2, a3
    1ef7:	043d           	mov.n	a3, a4
    1ef9:	07c716        	beqz	a7, 1f79 <xt_memcpy+0x89>
    1efc:	079216        	beqz	a2, 1f79 <xt_memcpy+0x89>
    1eff:	25b277        	bgeu	a2, a7, 1f28 <xt_memcpy+0x38>
    1f02:	904420        	addx2	a4, a4, a2
    1f05:	1fb747        	bgeu	a7, a4, 1f28 <xt_memcpy+0x38>
    1f08:	fec422        	addi	a2, a4, -2
    1f0b:	530b           	addi.n	a5, a3, -1
    1f0d:	904370        	addx2	a4, a3, a7
    1f10:	fec442        	addi	a4, a4, -2
    1f13:	5e0526        	beqi	a5, -1, 1f75 <xt_memcpy+0x85>
    1f16:	099376        	loopnez	a3, 1f23 <xt_memcpy+0x33>
    1f19:	223d           	ae_l16si.n	a3, a2, 0
    1f1b:	fec222        	addi	a2, a2, -2
    1f1e:	fec442        	addi	a4, a4, -2
    1f21:	743d           	ae_s16i.n	a3, a4, 2

00001f23 <xt_memcpy+0x33>:
    1f23:	072d           	mov.n	a2, a7
    1f25:	f01d           	retw.n

00001f27 <xt_memcpy+0x37>:
	...

00001f28 <xt_memcpy+0x38>:
    1f28:	074d           	mov.n	a4, a7
    1f2a:	530b           	addi.n	a5, a3, -1
    1f2c:	450526        	beqi	a5, -1, 1f75 <xt_memcpy+0x85>
    1f2f:	248030        	extui	a8, a3, 0, 3
    1f32:	036d           	mov.n	a6, a3
    1f34:	413360        	srli	a3, a6, 3
    1f37:	069876        	loopnez	a8, 1f41 <xt_memcpy+0x51>
    1f3a:	229d           	ae_l16si.n	a9, a2, 0
    1f3c:	09c494        	ae_s16ip	a9, a4, 2
    1f3f:	222b           	addi.n	a2, a2, 2
    1f41:	309376        	loopnez	a3, 1f75 <xt_memcpy+0x85>
    1f44:	223d           	ae_l16si.n	a3, a2, 0
    1f46:	09c434        	ae_s16ip	a3, a4, 2
    1f49:	323d           	ae_l16si.n	a3, a2, 2
    1f4b:	09c434        	ae_s16ip	a3, a4, 2
    1f4e:	029232        	l16si	a3, a2, 4
    1f51:	09c434        	ae_s16ip	a3, a4, 2
    1f54:	039232        	l16si	a3, a2, 6
    1f57:	09c434        	ae_s16ip	a3, a4, 2
    1f5a:	049232        	l16si	a3, a2, 8
    1f5d:	09c434        	ae_s16ip	a3, a4, 2
    1f60:	059232        	l16si	a3, a2, 10
    1f63:	09c434        	ae_s16ip	a3, a4, 2
    1f66:	069232        	l16si	a3, a2, 12
    1f69:	09c434        	ae_s16ip	a3, a4, 2
    1f6c:	079232        	l16si	a3, a2, 14
    1f6f:	09c434        	ae_s16ip	a3, a4, 2
    1f72:	10c222        	addi	a2, a2, 16
    1f75:	072d           	mov.n	a2, a7
    1f77:	f01d           	retw.n

00001f79 <xt_memcpy+0x89>:
    1f79:	020c           	movi.n	a2, 0
    1f7b:	f01d           	retw.n

00001f7d <xt_memcpy+0x8d>:
    1f7d:	000000                                        ...

00001f80 <_fini>:
    1f80:	008136        	entry	a1, 64
    1f83:	f87681        	l32r	a8, 15c (178 <__do_global_dtors_aux>)
    1f86:	f03d           	nop.n
    1f88:	0008e0        	callx8	a8

00001f8b <_fini+0xb>:
    1f8b:	f01d           	retw.n
