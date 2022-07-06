
Build/lib/pisplit_ec120_position_independent.splitload.lib:     file format elf32-xtensa-le


Disassembly of section .text:

00000000 <_init-0x37c>:
       0:	000003d4 000055e8 00006cf0 00006954     .....U...l..Ti..
      10:	00000000 0000693c 00000000 00006cf4     ....<i.......l..
      20:	00005620 00006d14 00006d0c 000003f0      V...m...m......
      30:	00001000 000005b4 89442379 0000bb75     ........y#D.u...
      40:	00006a30 7fffffff 00006aa0 80000000     0j.......j......
      50:	00006d10 00006070 000012c8 00006088     .m..p`.......`..
      60:	00001358 000060a0 0000296c 000060b8     X....`..l)...`..
      70:	00002808 000060d4 000029b0 000060f0     .(...`...)...`..
      80:	000012c0 00006104 000012d0 00006124     .....a......$a..
      90:	00001548 00006144 00002984 00006164     H...Da...)..da..
      a0:	00002a1c 00006188 00002a54 000061a8     .*...a..T*...a..
      b0:	00002aa8 000061cc 00002ab8 00006960     .*...a...*..`i..
      c0:	00005608 0000fff7 00155555 00000ab0     .V......UU......
      d0:	00000a98 00007fff 000014fc 00200000     .............. .
      e0:	002aaa00 000069a0 000069a4 00010000     ..*..i...i......
      f0:	00080000 00004d88 00004c80 00000b28     .....M...L..(...
     100:	00000b84 000069b8 00000c08 00000cbc     .....i..........
     110:	00003630 00003b48 00000448 00001300     06..H;..H.......
     120:	00001504 000012d8 000012ec 00000708     ................
     130:	0000432c 0d190210 00008000 00006d18     ,C...........m..
     140:	00002000 00002bfc 00004530 00001334     . ...+..0E..4...
     150:	00001358 00800000 000009ac 00006bbc     X............k..
     160:	00006bb0 000069d0 00006bcc 00006c0c     .k...i...k...l..
     170:	00006c50 00006bd0 000015f6 000015f8     Pl...k..........
     180:	000015fa 000015fc 007fffff 0000060c     ................
     190:	00004d10 0000064c 7fff0000 00001bac     .M..L...........
     1a0:	000015fc 0000190c 00001050 00001758     ........P...X...
     1b0:	00001678 000017d8 00001970 00000d3c     x.......p...<...
     1c0:	00000fb0 0000198c 00002e64 00003114     ........d....1..
     1d0:	00001adc 000033d8 00003288 000037ac     .....3...2...7..
     1e0:	00001b80 00003924 00003c54 00003db0     ....$9..T<...=..
     1f0:	00001884 000016e8 0000179c 00001af0     ................
     200:	00003658 0000454c 0000122c 00001270     X6..LE..,...p...
     210:	00005277 00004424 00001234 00001b10     wR..$D..4.......
     220:	10000000 00002b98 000069bc 40000000     .....+...i.....@
     230:	0e390000 3eb80000 18000000 0d330000     ..9....>......3.
     240:	00006000 00001770 00004000 00002ba8     .`..p....@...+..
     250:	00002bec 00002bc4 ffff8000 000ccccc     .+...+..........
     260:	00393e4b 00175e97 00002b10 00003254     K>9..^...+..T2..
     270:	00002e08 000046d8 3fff0000 c0010000     .....F.....?....
     280:	000069d8 000069c8 00006ccd 000069cc     .i...i...l...i..
     290:	00007efa 00000fff 000069c0 00007e00     .~.......i...~..
     2a0:	00004666 00003b00 00004a3d 000043d7     fF...;..=J...C..
     2b0:	00007fae 00000800 00007f80 00004104     .............A..
     2c0:	000040c8 000061f0 00006210 ffffb800     .@...a...b......
     2d0:	b8000000 00004114 0000fffc 00006534     .....A......4e..
     2e0:	00005540 00006538 0000475c 00004908     @U..8e..\G...I..
     2f0:	00004a0c 00004b84 00000500 0000434c     .J...K......LC..
     300:	000047a8 00004734 00035e51 00006cb0     .G..4G..Q^...l..
     310:	00006ce0 00006a1c 00006a18 000e38e4     .l...j...j...8..
     320:	00006c90 00006cd0 00124925 80000001     .l...l..%I......
     330:	00005a82 00004fac 0000642c 00004e20     .Z...O..,d.. N..
     340:	00004da8 00004fd8 000051bc 000051e8     .M...O...Q...Q..
     350:	00005218 00005270 00005244 00006430     .R..pR..DR..0d..
     360:	000052e4 000052b8 0000550c 00005498     .R...R...U...T..
     370:	00006744 00006940 00000398              Dg..@i......

0000037c <_init>:
     37c:	008136        	entry	a1, 64
     37f:	ff2081        	l32r	a8, 0 (3d4 <frame_dummy>)
     382:	f03d           	nop.n
     384:	0008e0        	callx8	a8
     387:	ff1f81        	l32r	a8, 4 (55e8 <__do_global_ctors_aux>)
     38a:	f03d           	nop.n
     38c:	0008e0        	callx8	a8
     38f:	f01d           	retw.n

00000391 <_init+0x15>:
     391:	00000000 36000000                                .......

00000398 <__do_global_dtors_aux>:
     398:	004136        	entry	a1, 32
     39b:	ff1b31        	l32r	a3, 8 (6cf0 <__bss_start>)
     39e:	000382        	l8ui	a8, a3, 0
     3a1:	d8ec           	bnez.n	a8, 3d2 <__do_global_dtors_aux+0x3a>
     3a3:	ff1a21        	l32r	a2, c (6954 <p$3519_6_1>)
     3a6:	02b8           	l32i.n	a11, a2, 0
     3a8:	0ba8           	l32i.n	a10, a11, 0
     3aa:	ca8c           	beqz.n	a10, 3ba <__do_global_dtors_aux+0x22>

000003ac <__do_global_dtors_aux+0x14>:
     3ac:	9b4b           	addi.n	a9, a11, 4
     3ae:	0299           	s32i.n	a9, a2, 0
     3b0:	000ae0        	callx8	a10
     3b3:	02b8           	l32i.n	a11, a2, 0
     3b5:	0ba8           	l32i.n	a10, a11, 0
     3b7:	ff1a56        	bnez	a10, 3ac <__do_global_dtors_aux+0x14>

000003ba <__do_global_dtors_aux+0x22>:
     3ba:	ff15a1        	l32r	a10, 10 (0 <_text_start>)
     3bd:	0b0c           	movi.n	a11, 0
     3bf:	081ab7        	beq	a10, a11, 3cb <__do_global_dtors_aux+0x33>
     3c2:	ff14a1        	l32r	a10, 14 (693c <__EH_FRAME_BEGIN___44>)
     3c5:	ff1281        	l32r	a8, 10 (0 <_text_start>)
     3c8:	0008e0        	callx8	a8

000003cb <__do_global_dtors_aux+0x33>:
     3cb:	1c0c           	movi.n	a12, 1
     3cd:	0043c2        	s8i	a12, a3, 0
     3d0:	f01d           	retw.n

000003d2 <__do_global_dtors_aux+0x3a>:
     3d2:	f01d           	retw.n

000003d4 <frame_dummy>:
     3d4:	004136        	entry	a1, 32
     3d7:	ff1081        	l32r	a8, 18 (0 <_text_start>)
     3da:	090c           	movi.n	a9, 0
     3dc:	0b1897        	beq	a8, a9, 3eb <frame_dummy+0x17>
     3df:	ff0da1        	l32r	a10, 14 (693c <__EH_FRAME_BEGIN___44>)
     3e2:	ff0eb1        	l32r	a11, 1c (6cf4 <object$3538_8_1>)
     3e5:	ff0c81        	l32r	a8, 18 (0 <_text_start>)
     3e8:	0008e0        	callx8	a8

000003eb <frame_dummy+0x17>:
     3eb:	f01d           	retw.n

000003ed <frame_dummy+0x19>:
     3ed:	000000                                        ...

000003f0 <common_ECNR_PARA_init_v2>:
     3f0:	004136        	entry	a1, 32
     3f3:	a28b           	addi.n	a10, a2, 8
     3f5:	ff0a81        	l32r	a8, 20 (5620 <xt_memcpy>)
     3f8:	b32b           	addi.n	a11, a3, 2
     3fa:	79a0c2        	movi	a12, 121
     3fd:	0008e0        	callx8	a8
     400:	5c0c           	movi.n	a12, 5
     402:	01d3b2        	addmi	a11, a3, 0x100
     405:	01d2a2        	addmi	a10, a2, 0x100
     408:	ff0681        	l32r	a8, 20 (5620 <xt_memcpy>)
     40b:	fccaa2        	addi	a10, a10, -4
     40e:	f4cbb2        	addi	a11, a11, -12
     411:	0008e0        	callx8	a8
     414:	d7a0c2        	movi	a12, 215
     417:	01d3b2        	addmi	a11, a3, 0x100
     41a:	01d2a2        	addmi	a10, a2, 0x100
     41d:	ff0081        	l32r	a8, 20 (5620 <xt_memcpy>)
     420:	aa8b           	addi.n	a10, a10, 8
     422:	fecbb2        	addi	a11, a11, -2
     425:	0008e0        	callx8	a8
     428:	6c3c           	movi.n	a12, 54
     42a:	03d2a2        	addmi	a10, a2, 0x300
     42d:	03d3b2        	addmi	a11, a3, 0x300
     430:	fefc81        	l32r	a8, 20 (5620 <xt_memcpy>)
     433:	accbb2        	addi	a11, a11, -84
     436:	b8caa2        	addi	a10, a10, -72
     439:	0008e0        	callx8	a8
     43c:	fefa91        	l32r	a9, 24 (6d14 <p_ecnr_mem>)
     43f:	0998           	l32i.n	a9, a9, 0
     441:	0299           	s32i.n	a9, a2, 0
     443:	f01d           	retw.n

00000445 <common_ECNR_PARA_init_v2+0x55>:
     445:	000000                                        ...

00000448 <TX_ECNR_PARA_init>:
     448:	004136        	entry	a1, 32
     44b:	03bd           	mov.n	a11, a3
     44d:	fef551        	l32r	a5, 24 (6d14 <p_ecnr_mem>)
     450:	fef641        	l32r	a4, 28 (6d0c <aec_nr_para>)
     453:	fef681        	l32r	a8, 2c (3f0 <common_ECNR_PARA_init_v2>)
     456:	05a8           	l32i.n	a10, a5, 0
     458:	04a9           	s32i.n	a10, a4, 0
     45a:	0008e0        	callx8	a8
     45d:	4c0c           	movi.n	a12, 4
     45f:	04d8           	l32i.n	a13, a4, 0
     461:	feef81        	l32r	a8, 20 (5620 <xt_memcpy>)
     464:	0568           	l32i.n	a6, a5, 0
     466:	821de2        	l16ui	a14, a13, 0x104
     469:	811d92        	l16ui	a9, a13, 0x102
     46c:	041df2        	l16ui	a15, a13, 8
     46f:	02ddb2        	addmi	a11, a13, 0x200
     472:	16d662        	addmi	a6, a6, 0x1600
     475:	f6c6a2        	addi	a10, a6, -10
     478:	bacbb2        	addi	a11, a11, -70
     47b:	0256f2        	s16i	a15, a6, 4
     47e:	939e20        	movnez	a9, a14, a2
     481:	025d92        	s16i	a9, a13, 4
     484:	0008e0        	callx8	a8
     487:	3c0c           	movi.n	a12, 3
     489:	fee581        	l32r	a8, 20 (5620 <xt_memcpy>)
     48c:	78a0e2        	movi	a14, 120
     48f:	04a8           	l32i.n	a10, a4, 0
     491:	05d8           	l32i.n	a13, a5, 0
     493:	03d3b2        	addmi	a11, a3, 0x300
     496:	4ecbb2        	addi	a11, a11, 78
     499:	35ddd2        	addmi	a13, a13, 0x3500
     49c:	db1a92        	l16ui	a9, a10, 0x1b6
     49f:	03daa2        	addmi	a10, a10, 0x300
     4a2:	24caa2        	addi	a10, a10, 36
     4a5:	4399e0        	min	a9, a9, a14
     4a8:	725d92        	s16i	a9, a13, 228
     4ab:	0008e0        	callx8	a8
     4ae:	f01d           	retw.n

000004b0 <RX_ECNR_PARA_init>:
     4b0:	004136        	entry	a1, 32
     4b3:	03bd           	mov.n	a11, a3
     4b5:	fedba1        	l32r	a10, 24 (6d14 <p_ecnr_mem>)
     4b8:	fedc31        	l32r	a3, 28 (6d0c <aec_nr_para>)
     4bb:	fedc81        	l32r	a8, 2c (3f0 <common_ECNR_PARA_init_v2>)
     4be:	0aa8           	l32i.n	a10, a10, 0
     4c0:	03a9           	s32i.n	a10, a3, 0
     4c2:	0008e0        	callx8	a8
     4c5:	03a8           	l32i.n	a10, a3, 0
     4c7:	b42b           	addi.n	a11, a4, 2
     4c9:	03daa2        	addmi	a10, a10, 0x300
     4cc:	2acaa2        	addi	a10, a10, 42
     4cf:	e29c           	beqz.n	a2, 4f1 <RX_ECNR_PARA_init+0x41>
     4d1:	fed381        	l32r	a8, 20 (5620 <xt_memcpy>)
     4d4:	80a0c2        	movi	a12, 128
     4d7:	0008e0        	callx8	a8
     4da:	002392        	l32i	a9, a3, 0
     4dd:	fed4a1        	l32r	a10, 30 (1000 <subband_aec_mid_channel+0x50>)
     4e0:	80a0b2        	movi	a11, 128
     4e3:	04d992        	addmi	a9, a9, 0x400
     4e6:	2ac992        	addi	a9, a9, 42
     4e9:	02ab76        	loopgtz	a11, 4ef <RX_ECNR_PARA_init+0x3f>
     4ec:	09c9a4        	ae_s16ip	a10, a9, 2

000004ef <RX_ECNR_PARA_init+0x3f>:
     4ef:	f01d           	retw.n

000004f1 <RX_ECNR_PARA_init+0x41>:
     4f1:	fecb81        	l32r	a8, 20 (5620 <xt_memcpy>)
     4f4:	00a1c2        	movi	a12, 0x100
     4f7:	0008e0        	callx8	a8
     4fa:	f01d           	retw.n

000004fc <RX_ECNR_PARA_init+0x4c>:
     4fc:	00000000                                ....

00000500 <comfort_noise_gen_wb2>:
     500:	006136        	entry	a1, 48
     503:	139272        	l16si	a7, a2, 38
     506:	fec7b1        	l32r	a11, 24 (6d14 <p_ecnr_mem>)
     509:	280c           	movi.n	a8, 2
     50b:	3a2232        	l32i	a3, a2, 232
     50e:	6f9292        	l16si	a9, a2, 222
     511:	4022a2        	l32i	a10, a2, 0x100
     514:	11a9           	s32i.n	a10, a1, 4
     516:	990b           	addi.n	a9, a9, -1
     518:	fc248fc33e 	{ addi	a3, a3, -4; ae_zeroq56	aeq0 }
     51d:	05a876        	loopgtz	a8, 526 <comfort_noise_gen_wb2+0x26>
     520:	624314        	ae_sq32f.iu	aeq0, a3, 4
     523:	624314        	ae_sq32f.iu	aeq0, a3, 4

00000526 <comfort_noise_gen_wb2+0x26>:
     526:	0b58           	l32i.n	a5, a11, 0
     528:	2f0262        	l8ui	a6, a2, 47
     52b:	1148           	l32i.n	a4, a1, 4
     52d:	15d552        	addmi	a5, a5, 0x1500
     530:	fec662        	addi	a6, a6, -2
     533:	c54b           	addi.n	a12, a5, 4
     535:	809552        	l16si	a5, a5, 0x100
     538:	05c696        	bltz	a6, 598 <comfort_noise_gen_wb2+0x98>
     53b:	fcc442        	addi	a4, a4, -4
     53e:	f27c           	movi.n	a2, -1
     540:	0199           	s32i.n	a9, a1, 0

00000542 <comfort_noise_gen_wb2+0x42>:
     542:	0082d6        	bgez	a2, 54e <comfort_noise_gen_wb2+0x4e>
     545:	11b8           	l32i.n	a11, a1, 4
     547:	0bb8           	l32i.n	a11, a11, 0
     549:	0000c6        	j	550 <comfort_noise_gen_wb2+0x50>

0000054c <comfort_noise_gen_wb2+0x4c>:
	...

0000054e <comfort_noise_gen_wb2+0x4e>:
     54e:	04b8           	l32i.n	a11, a4, 0

00000550 <comfort_noise_gen_wb2+0x50>:
     550:	feb981        	l32r	a8, 34 (5b4 <HIFI_sqrt>)
     553:	01a8           	l32i.n	a10, a1, 0
     555:	0008e0        	callx8	a8
     558:	feb9d1        	l32r	a13, 3c (bb75 <_end+0x4e59>)
     55b:	feb7c1        	l32r	a12, 38 (89442379 <_end+0x8943b65d>)
     55e:	360b34        	ae_cvtq48a32s	aeq0, a11
     561:	82b5d0        	mull	a11, a5, a13
     564:	d155c0        	mul16s	a5, a5, a12
     567:	300b54        	ae_cvtp24a16x2.ll	aep0, a11, a5
     56a:	d4e240444e 	{ addi	a4, a4, 4; ae_mulfq32sp16s.l	aeq1, aeq0, aep0 }
     56f:	d4621097af 	{ add	a9, a7, a10; ae_mulfq32sp16s.h	aeq0, aeq0, aep0 }
     574:	fc24fe99cf 	{ ae_slaasq56s	aeq3, aeq1, a9; nop }
     579:	fc24fe898f 	{ ae_slaasq56s	aeq2, aeq0, a9; nop }
     57e:	fd5c40122e 	{ addi	a2, a2, 1; ae_roundsq32sym	aeq1, aeq3 }
     583:	fc5479d31f 	{ ae_sq32f.iu	aeq1, a3, 4; ae_roundsq32sym	aeq0, aeq2 }
     588:	624314        	ae_sq32f.iu	aeq0, a3, 4
     58b:	b39627        	bne	a6, a2, 542 <comfort_noise_gen_wb2+0x42>
     58e:	fea5c1        	l32r	a12, 24 (6d14 <p_ecnr_mem>)
     591:	0cc8           	l32i.n	a12, a12, 0
     593:	15dcc2        	addmi	a12, a12, 0x1500
     596:	cc4b           	addi.n	a12, a12, 4

00000598 <comfort_noise_gen_wb2+0x98>:
     598:	7e5c52        	s16i	a5, a12, 252
     59b:	7da0a2        	movi	a10, 125
     59e:	7ca0d2        	movi	a13, 124
     5a1:	fc2490dd6e 	{ blt	a13, a6, 5b2 <comfort_noise_gen_wb2+0xb2>; ae_zeroq56	aeq0 }
     5a6:	c0ea60        	sub	a14, a10, a6
     5a9:	05ae76        	loopgtz	a14, 5b2 <comfort_noise_gen_wb2+0xb2>
     5ac:	624314        	ae_sq32f.iu	aeq0, a3, 4
     5af:	624314        	ae_sq32f.iu	aeq0, a3, 4

000005b2 <comfort_noise_gen_wb2+0xb2>:
     5b2:	f01d           	retw.n

000005b4 <HIFI_sqrt>:
     5b4:	004136        	entry	a1, 32
     5b7:	4b13a6        	blti	a3, 1, 606 <HIFI_sqrt+0x52>
     5ba:	fea151        	l32r	a5, 40 (6a30 <TABLE_SQRT>)
     5bd:	070c           	movi.n	a7, 0
     5bf:	40e380        	nsa	a8, a3
     5c2:	401800        	ssl	a8
     5c5:	c02280        	sub	a2, a2, a8
     5c8:	a16300        	sll	a6, a3
     5cb:	043020        	extui	a3, a2, 0, 1
     5ce:	400300        	ssr	a3
     5d1:	b16060        	sra	a6, a6
     5d4:	416960        	srli	a6, a6, 9
     5d7:	314060        	srai	a4, a6, 16
     5da:	f0c442        	addi	a4, a4, -16
     5dd:	534470        	max	a4, a4, a7
     5e0:	904450        	addx2	a4, a4, a5
     5e3:	245d           	ae_l16si.n	a5, a4, 0
     5e5:	300664        	ae_cvtp24a16x2.ll	aep0, a6, a6
     5e8:	344d           	ae_l16si.n	a4, a4, 2
     5ea:	116500        	slli	a6, a5, 16
     5ed:	360634        	ae_cvtq48a32s	aeq0, a6
     5f0:	c04450        	sub	a4, a4, a5
     5f3:	114400        	slli	a4, a4, 16
     5f6:	364434        	ae_cvtq48a32s	aeq1, a4
     5f9:	d4e110223f 	{ add	a2, a2, a3; ae_mulaq32sp16u.l	aeq0, aeq1, aep0 }
     5fe:	212120        	srai	a2, a2, 1
     601:	368304        	ae_trunca32q48	a3, aeq0
     604:	f01d           	retw.n

00000606 <HIFI_sqrt+0x52>:
     606:	030c           	movi.n	a3, 0
     608:	020c           	movi.n	a2, 0
     60a:	f01d           	retw.n

0000060c <Log2_norm>:
     60c:	004136        	entry	a1, 32
     60f:	3212a6        	blti	a2, 1, 645 <Log2_norm+0x39>
     612:	046c           	movi.n	a4, -32
     614:	2da437        	bge	a4, a3, 645 <Log2_norm+0x39>
     617:	052c           	movi.n	a5, 32
     619:	04a537        	bge	a5, a3, 621 <Log2_norm+0x15>
     61c:	fe8a21        	l32r	a2, 44 (7fffffff <_end+0x7fff92e3>)
     61f:	f01d           	retw.n

00000621 <Log2_norm+0x15>:
     621:	fe8941        	l32r	a4, 48 (6aa0 <TABLE_LOG2_1530>)
     624:	558820        	extui	a8, a2, 24, 6
     627:	1188e0        	slli	a8, a8, 2
     62a:	603484        	ae_lq32f.xu	aeq0, a4, a8
     62d:	604414        	ae_lq32f.i	aeq1, a4, 4
     630:	f92ae2022f 	{ ae_movpa24x2	aep0, a2, a2; ae_subq56	aeq1, aeq1, aeq0 }
     635:	030174        	ae_mulaq32sp16u.l	aeq0, aeq1, aep0
     638:	368204        	ae_trunca32q48	a2, aeq0
     63b:	014360        	slli	a4, a3, 26
     63e:	212520        	srai	a2, a2, 5
     641:	224a           	add.n	a2, a2, a4
     643:	f01d           	retw.n

00000645 <Log2_norm+0x39>:
     645:	fe8121        	l32r	a2, 4c (80000000 <_end+0x7fff92e4>)
     648:	f01d           	retw.n

0000064a <Log2_norm+0x3e>:
	...

0000064c <siir_safr>:
     64c:	004136        	entry	a1, 32
     64f:	368334        	ae_cvtq48a32s	aeq2, a3
     652:	364234        	ae_cvtq48a32s	aeq1, a2
     655:	05a237        	bge	a2, a3, 65e <siir_safr+0x12>
     658:	440404        	ae_lp16x2f.i	aep0, a4, 0
     65b:	000086        	j	661 <siir_safr+0x15>

0000065e <siir_safr+0x12>:
     65e:	440414        	ae_lp16x2f.i	aep0, a4, 4

00000661 <siir_safr+0x15>:
     661:	c10864        	ae_mulzaafq32sp16s.lh	aeq1, aeq1, aep0, aeq2, aep0
     664:	061934        	ae_roundsq32asym	aeq0, aeq1
     667:	368204        	ae_trunca32q48	a2, aeq0
     66a:	f01d           	retw.n

0000066c <_start>:
     66c:	004136        	entry	a1, 32
     66f:	fe79a1        	l32r	a10, 54 (6070 <_DYNAMIC+0x9f4>)
     672:	fe7731        	l32r	a3, 50 (6d10 <printf_ptr>)
     675:	0288           	l32i.n	a8, a2, 0
     677:	fe78b1        	l32r	a11, 58 (12c8 <get_ec120_memsize>)
     67a:	0389           	s32i.n	a8, a3, 0
     67c:	0008e0        	callx8	a8
     67f:	0388           	l32i.n	a8, a3, 0
     681:	fe76a1        	l32r	a10, 5c (6088 <_DYNAMIC+0xa0c>)
     684:	fe77b1        	l32r	a11, 60 (1358 <Voice_EC120_Init>)
     687:	0008e0        	callx8	a8
     68a:	0388           	l32i.n	a8, a3, 0
     68c:	fe76a1        	l32r	a10, 64 (60a0 <_DYNAMIC+0xa24>)
     68f:	fe76b1        	l32r	a11, 68 (296c <Voice_EC120_Prcs>)
     692:	0008e0        	callx8	a8
     695:	0388           	l32i.n	a8, a3, 0
     697:	fe75a1        	l32r	a10, 6c (60b8 <_DYNAMIC+0xa3c>)
     69a:	fe75b1        	l32r	a11, 70 (2808 <Voice_PostEC120_Prcs>)
     69d:	0008e0        	callx8	a8
     6a0:	0388           	l32i.n	a8, a3, 0
     6a2:	fe74a1        	l32r	a10, 74 (60d4 <_DYNAMIC+0xa58>)
     6a5:	fe74b1        	l32r	a11, 78 (29b0 <EC_REF_GAIN_READBACK>)
     6a8:	0008e0        	callx8	a8
     6ab:	0388           	l32i.n	a8, a3, 0
     6ad:	fe73a1        	l32r	a10, 7c (60f0 <_DYNAMIC+0xa74>)
     6b0:	fe74b1        	l32r	a11, 80 (12c0 <get_ECNR_SVN>)
     6b3:	0008e0        	callx8	a8
     6b6:	0388           	l32i.n	a8, a3, 0
     6b8:	fe73a1        	l32r	a10, 84 (6104 <_DYNAMIC+0xa88>)
     6bb:	fe73b1        	l32r	a11, 88 (12d0 <get_ec120_inear_memsize>)
     6be:	0008e0        	callx8	a8
     6c1:	0388           	l32i.n	a8, a3, 0
     6c3:	fe72a1        	l32r	a10, 8c (6124 <_DYNAMIC+0xaa8>)
     6c6:	fe72b1        	l32r	a11, 90 (1548 <Voice_EC120_Inear_Init>)
     6c9:	0008e0        	callx8	a8
     6cc:	0388           	l32i.n	a8, a3, 0
     6ce:	fe71a1        	l32r	a10, 94 (6144 <_DYNAMIC+0xac8>)
     6d1:	fe71b1        	l32r	a11, 98 (2984 <Voice_EC120_Inear_Prcs>)
     6d4:	0008e0        	callx8	a8
     6d7:	0388           	l32i.n	a8, a3, 0
     6d9:	fe70a1        	l32r	a10, 9c (6164 <_DYNAMIC+0xae8>)
     6dc:	fe71b1        	l32r	a11, a0 (2a1c <EC_Inear_REF_GAIN_READBACK>)
     6df:	0008e0        	callx8	a8
     6e2:	0388           	l32i.n	a8, a3, 0
     6e4:	fe70a1        	l32r	a10, a4 (6188 <_DYNAMIC+0xb0c>)
     6e7:	fe70b1        	l32r	a11, a8 (2a54 <EC_PreLim_Coef_READBACK>)
     6ea:	0008e0        	callx8	a8
     6ed:	0388           	l32i.n	a8, a3, 0
     6ef:	fe6fa1        	l32r	a10, ac (61a8 <_DYNAMIC+0xb2c>)
     6f2:	fe6fb1        	l32r	a11, b0 (2aa8 <DaulMIC_power_Coef_READBACK>)
     6f5:	0008e0        	callx8	a8
     6f8:	0388           	l32i.n	a8, a3, 0
     6fa:	fe6ea1        	l32r	a10, b4 (61cc <_DYNAMIC+0xb50>)
     6fd:	fe6eb1        	l32r	a11, b8 (2ab8 <PostEC120_Info>)
     700:	0008e0        	callx8	a8
     703:	fe6e21        	l32r	a2, bc (6960 <export_parameter_array>)
     706:	f01d           	retw.n

00000708 <SBAEC_INIT>:
     708:	00a136        	entry	a1, 80
     70b:	fe4661        	l32r	a6, 24 (6d14 <p_ecnr_mem>)
     70e:	0b0c           	movi.n	a11, 0
     710:	0658           	l32i.n	a5, a6, 0
     712:	30a1c2        	movi	a12, 0x130
     715:	fe6a81        	l32r	a8, c0 (5608 <xt_memset>)
     718:	28d542        	addmi	a4, a5, 0x2800
     71b:	29d522        	addmi	a2, a5, 0x2900
     71e:	c0c232        	addi	a3, a2, -64
     721:	40c272        	addi	a7, a2, 64
     724:	78c442        	addi	a4, a4, 120
     727:	04ad           	mov.n	a10, a4
     729:	2ad552        	addmi	a5, a5, 0x2a00
     72c:	80c552        	addi	a5, a5, -128
     72f:	0008e0        	callx8	a8
     732:	0b0c           	movi.n	a11, 0
     734:	a0a2c2        	movi	a12, 0x2a0
     737:	a139           	s32i.n	a3, a1, 40
     739:	06a8           	l32i.n	a10, a6, 0
     73b:	9129           	s32i.n	a2, a1, 36
     73d:	fe6081        	l32r	a8, c0 (5608 <xt_memset>)
     740:	16daa2        	addmi	a10, a10, 0x1600
     743:	38caa2        	addi	a10, a10, 56
     746:	0008e0        	callx8	a8
     749:	fe3731        	l32r	a3, 28 (6d0c <aec_nr_para>)
     74c:	8e1c           	movi.n	a14, 24
     74e:	03c8           	l32i.n	a12, a3, 0
     750:	02d422        	addmi	a2, a4, 0x200
     753:	fe5ca1        	l32r	a10, c4 (fff7 <_end+0x92db>)
     756:	041cd2        	l16ui	a13, a12, 8
     759:	781c           	movi.n	a8, 23
     75b:	cf0c           	movi.n	a15, 12
     75d:	10fdf0        	and	a15, a13, a15
     760:	02af26        	beqi	a15, 12, 766 <SBAEC_INIT+0x5e>
     763:	0e6d47        	bbci	a13, 4, 775 <SBAEC_INIT+0x6d>

00000766 <SBAEC_INIT+0x5e>:
     766:	131c92        	l16ui	a9, a12, 38
     769:	10ada0        	and	a10, a13, a10
     76c:	045ca2        	s16i	a10, a12, 8
     76f:	02b897        	bgeu	a8, a9, 775 <SBAEC_INIT+0x6d>
     772:	135ce2        	s16i	a14, a12, 38
     775:	e39c92        	l16si	a9, a12, 0x1c6
     778:	39dc           	bnez.n	a9, 78f <SBAEC_INIT+0x87>
     77a:	2c52e2        	s16i	a14, a2, 88
     77d:	0b2c           	movi.n	a11, 32
     77f:	0d3c           	movi.n	a13, 48
     781:	0f4c           	movi.n	a15, 64
     783:	2a52f2        	s16i	a15, a2, 84
     786:	2b52d2        	s16i	a13, a2, 86
     789:	2d52b2        	s16i	a11, a2, 90
     78c:	001306        	j	7dc <SBAEC_INIT+0xd4>

0000078f <SBAEC_INIT+0x87>:
     78f:	131c82        	l16ui	a8, a12, 38
     792:	34a490        	extui	a10, a9, 4, 4
     795:	34b090        	extui	a11, a9, 0, 4
     798:	bb1b           	addi.n	a11, a11, 1
     79a:	aa1b           	addi.n	a10, a10, 1
     79c:	11aad0        	slli	a10, a10, 3
     79f:	11bbe0        	slli	a11, a11, 2
     7a2:	2a52b2        	s16i	a11, a2, 84
     7a5:	2b52a2        	s16i	a10, a2, 86
     7a8:	8bbd           	ae_sext16	a11, a11
     7aa:	4388b0        	min	a8, a8, a11
     7ad:	8aad           	ae_sext16	a10, a10
     7af:	2b0c           	movi.n	a11, 2
     7b1:	d2aab0        	quos	a10, a10, a11
     7b4:	8aad           	ae_sext16	a10, a10
     7b6:	4388a0        	min	a8, a8, a10
     7b9:	34a890        	extui	a10, a9, 8, 4
     7bc:	aa1b           	addi.n	a10, a10, 1
     7be:	11aae0        	slli	a10, a10, 2
     7c1:	2c52a2        	s16i	a10, a2, 88
     7c4:	8aad           	ae_sext16	a10, a10
     7c6:	4388a0        	min	a8, a8, a10
     7c9:	34ac90        	extui	a10, a9, 12, 4
     7cc:	aa1b           	addi.n	a10, a10, 1
     7ce:	11aae0        	slli	a10, a10, 2
     7d1:	2d52a2        	s16i	a10, a2, 90
     7d4:	8aad           	ae_sext16	a10, a10
     7d6:	4388a0        	min	a8, a8, a10
     7d9:	135c82        	s16i	a8, a12, 38

000007dc <SBAEC_INIT+0xd4>:
     7dc:	bceb           	addi.n	a11, a12, 14
     7de:	fe1081        	l32r	a8, 20 (5620 <xt_memcpy>)
     7e1:	a42b           	addi.n	a10, a4, 2
     7e3:	ec0c           	movi.n	a12, 14
     7e5:	0008e0        	callx8	a8
     7e8:	fe3981        	l32r	a8, cc (ab0 <Fill_CH_MEM_WB>)
     7eb:	0398           	l32i.n	a9, a3, 0
     7ed:	06f8           	l32i.n	a15, a6, 0
     7ef:	34ed           	ae_l16si.n	a14, a4, 2
     7f1:	0619a2        	l16ui	a10, a9, 12
     7f4:	d819c2        	l16ui	a12, a9, 0x1b0
     7f7:	c619b2        	l16ui	a11, a9, 0x18c
     7fa:	d999d2        	l16si	a13, a9, 0x1b2
     7fd:	eeea           	add.n	a14, a14, a14
     7ff:	16dff2        	addmi	a15, a15, 0x1600
     802:	38cff2        	addi	a15, a15, 56
     805:	74ed           	ae_s16i.n	a14, a4, 2
     807:	1154d2        	s16i	a13, a4, 34
     80a:	0f54b2        	s16i	a11, a4, 30
     80d:	1054c2        	s16i	a12, a4, 32
     810:	91b8           	l32i.n	a11, a1, 36
     812:	07cd           	mov.n	a12, a7
     814:	05dd           	mov.n	a13, a5
     816:	04ed           	mov.n	a14, a4
     818:	11aac0        	slli	a10, a10, 4
     81b:	94a9           	s32i.n	a10, a4, 36
     81d:	0998           	l32i.n	a9, a9, 0
     81f:	fe2aa1        	l32r	a10, c8 (155555 <_end+0x14e839>)
     822:	44a9           	s32i.n	a10, a4, 16
     824:	a1a8           	l32i.n	a10, a1, 40
     826:	04d992        	addmi	a9, a9, 0x400
     829:	8199           	s32i.n	a9, a1, 32
     82b:	0199           	s32i.n	a9, a1, 0
     82d:	0008e0        	callx8	a8
     830:	a1a8           	l32i.n	a10, a1, 40
     832:	2a92b2        	l16si	a11, a2, 84
     835:	1d0c           	movi.n	a13, 1
     837:	03c8           	l32i.n	a12, a3, 0
     839:	06e8           	l32i.n	a14, a6, 0
     83b:	fe2581        	l32r	a8, d0 (a98 <Fill_sbco>)
     83e:	169cc2        	l16si	a12, a12, 44
     841:	15dee2        	addmi	a14, a14, 0x1500
     844:	482e92        	l32i	a9, a14, 0x120
     847:	e599           	s32i.n	a9, a5, 56
     849:	442ee2        	l32i	a14, a14, 0x110
     84c:	d5e9           	s32i.n	a14, a5, 52
     84e:	0008e0        	callx8	a8
     851:	91a8           	l32i.n	a10, a1, 36
     853:	fe1f81        	l32r	a8, d0 (a98 <Fill_sbco>)
     856:	03c8           	l32i.n	a12, a3, 0
     858:	2b92b2        	l16si	a11, a2, 86
     85b:	2d0c           	movi.n	a13, 2
     85d:	179cc2        	l16si	a12, a12, 46
     860:	d2bbd0        	quos	a11, a11, a13
     863:	8bbd           	ae_sext16	a11, a11
     865:	1d0c           	movi.n	a13, 1
     867:	0008e0        	callx8	a8
     86a:	07ad           	mov.n	a10, a7
     86c:	2c92b2        	l16si	a11, a2, 88
     86f:	03c8           	l32i.n	a12, a3, 0
     871:	fe1781        	l32r	a8, d0 (a98 <Fill_sbco>)
     874:	1d0c           	movi.n	a13, 1
     876:	189cc2        	l16si	a12, a12, 48
     879:	0008e0        	callx8	a8
     87c:	05ad           	mov.n	a10, a5
     87e:	2d92b2        	l16si	a11, a2, 90
     881:	03c8           	l32i.n	a12, a3, 0
     883:	fe1381        	l32r	a8, d0 (a98 <Fill_sbco>)
     886:	1d0c           	movi.n	a13, 1
     888:	199cc2        	l16si	a12, a12, 50
     88b:	0008e0        	callx8	a8
     88e:	03d8           	l32i.n	a13, a3, 0
     890:	fe11b1        	l32r	a11, d4 (7fff <_end+0x12e3>)
     893:	06e8           	l32i.n	a14, a6, 0
     895:	dc1d82        	l16ui	a8, a13, 0x1b8
     898:	021d92        	l16ui	a9, a13, 4
     89b:	051da2        	l16ui	a10, a13, 10
     89e:	041df2        	l16ui	a15, a13, 8
     8a1:	15dee2        	addmi	a14, a14, 0x1500
     8a4:	fccee2        	addi	a14, a14, -4
     8a7:	14c1f0        	extui	a12, a15, 1, 2
     8aa:	151dd2        	l16ui	a13, a13, 42
     8ad:	04f0f0        	extui	a15, a15, 0, 1
     8b0:	2452f2        	s16i	a15, a2, 72
     8b3:	7c5ed2        	s16i	a13, a14, 248
     8b6:	815ec2        	s16i	a12, a14, 0x102
     8b9:	1554b2        	s16i	a11, a4, 42
     8bc:	2552a2        	s16i	a10, a2, 74
     8bf:	649d           	ae_s16i.n	a9, a4, 0
     8c1:	4e4282        	s8i	a8, a2, 78
     8c4:	03f8           	l32i.n	a15, a3, 0
     8c6:	dc1ff2        	l16ui	a15, a15, 0x1b8
     8c9:	41f8f0        	srli	a15, a15, 8
     8cc:	4f42f2        	s8i	a15, a2, 79
     8cf:	06c8           	l32i.n	a12, a6, 0
     8d1:	fe0151        	l32r	a5, d8 (14fc <Voice_EC120_Init+0x1a4>)
     8d4:	15dcd2        	addmi	a13, a12, 0x1500
     8d7:	fccdd2        	addi	a13, a13, -4
     8da:	819dd2        	l16si	a13, a13, 0x102
     8dd:	432ee2        	l32i	a14, a14, 0x10c
     8e0:	1462e2        	s32i	a14, a2, 80
     8e3:	fecdd2        	addi	a13, a13, -2
     8e6:	0bdd56        	bnez	a13, 9a7 <SBAEC_INIT+0x29f>
     8e9:	04ed           	mov.n	a14, a4
     8eb:	02d4d2        	addmi	a13, a4, 0x200
     8ee:	2b92a2        	l16si	a10, a2, 86
     8f1:	2d92b2        	l16si	a11, a2, 90
     8f4:	8198           	l32i.n	a9, a1, 32
     8f6:	2c9282        	l16si	a8, a2, 88
     8f9:	2a92f2        	l16si	a15, a2, 84
     8fc:	01d992        	addmi	a9, a9, 0x100
     8ff:	88ba           	add.n	a8, a8, a11
     901:	ffaa           	add.n	a15, a15, a10
     903:	18c992        	addi	a9, a9, 24
     906:	0199           	s32i.n	a9, a1, 0
     908:	01d4a2        	addmi	a10, a4, 0x100
     90b:	88cdb2        	addi	a11, a13, -120
     90e:	61b9           	s32i.n	a11, a1, 24
     910:	ff8a           	add.n	a15, a15, a8
     912:	48caa2        	addi	a10, a10, 72
     915:	41a9           	s32i.n	a10, a1, 16
     917:	a0ffc0        	addx4	a15, a15, a12
     91a:	fdec81        	l32r	a8, cc (ab0 <Fill_CH_MEM_WB>)
     91d:	0dcd           	mov.n	a12, a13
     91f:	16dff2        	addmi	a15, a15, 0x1600
     922:	38cff2        	addi	a15, a15, 56
     925:	c8ccc2        	addi	a12, a12, -56
     928:	dd8b           	addi.n	a13, a13, 8
     92a:	71d9           	s32i.n	a13, a1, 28
     92c:	51c9           	s32i.n	a12, a1, 20
     92e:	0008e0        	callx8	a8
     931:	41a8           	l32i.n	a10, a1, 16
     933:	2a92b2        	l16si	a11, a2, 84
     936:	a1e8           	l32i.n	a14, a1, 40
     938:	03c8           	l32i.n	a12, a3, 0
     93a:	9188           	l32i.n	a8, a1, 36
     93c:	06f8           	l32i.n	a15, a6, 0
     93e:	1a9cc2        	l16si	a12, a12, 52
     941:	e888           	l32i.n	a8, a8, 56
     943:	ff5a           	add.n	a15, a15, a5
     945:	706482        	s32i	a8, a4, 0x1c0
     948:	472f82        	l32i	a8, a15, 0x11c
     94b:	8f6482        	s32i	a8, a4, 0x23c
     94e:	492ff2        	l32i	a15, a15, 0x124
     951:	9064f2        	s32i	a15, a4, 0x240
     954:	fddf81        	l32r	a8, d0 (a98 <Fill_sbco>)
     957:	eee8           	l32i.n	a14, a14, 56
     959:	6064e2        	s32i	a14, a4, 0x180
     95c:	e7d8           	l32i.n	a13, a7, 56
     95e:	8064d2        	s32i	a13, a4, 0x200
     961:	0d0c           	movi.n	a13, 0
     963:	0008e0        	callx8	a8
     966:	5158           	l32i.n	a5, a1, 20
     968:	61a8           	l32i.n	a10, a1, 24
     96a:	fdd981        	l32r	a8, d0 (a98 <Fill_sbco>)
     96d:	03c8           	l32i.n	a12, a3, 0
     96f:	2b92b2        	l16si	a11, a2, 86
     972:	2d0c           	movi.n	a13, 2
     974:	1b9cc2        	l16si	a12, a12, 54
     977:	d2bbd0        	quos	a11, a11, a13
     97a:	8bbd           	ae_sext16	a11, a11
     97c:	0d0c           	movi.n	a13, 0
     97e:	0008e0        	callx8	a8
     981:	05ad           	mov.n	a10, a5
     983:	2c92b2        	l16si	a11, a2, 88
     986:	0d0c           	movi.n	a13, 0
     988:	03c8           	l32i.n	a12, a3, 0
     98a:	fdd181        	l32r	a8, d0 (a98 <Fill_sbco>)
     98d:	7148           	l32i.n	a4, a1, 28
     98f:	1c9cc2        	l16si	a12, a12, 56
     992:	0008e0        	callx8	a8
     995:	04ad           	mov.n	a10, a4
     997:	2d92b2        	l16si	a11, a2, 90
     99a:	03c8           	l32i.n	a12, a3, 0
     99c:	fdcd81        	l32r	a8, d0 (a98 <Fill_sbco>)
     99f:	0d0c           	movi.n	a13, 0
     9a1:	1d9cc2        	l16si	a12, a12, 58
     9a4:	0008e0        	callx8	a8

000009a7 <SBAEC_INIT+0x29f>:
     9a7:	f01d           	retw.n

000009a9 <SBAEC_INIT+0x2a1>:
     9a9:	000000                                        ...

000009ac <SBAEC_INIT_inear>:
     9ac:	008136        	entry	a1, 64
     9af:	fdc781        	l32r	a8, cc (ab0 <Fill_CH_MEM_WB>)
     9b2:	fd9d41        	l32r	a4, 28 (6d0c <aec_nr_para>)
     9b5:	28d2e2        	addmi	a14, a2, 0x2800
     9b8:	34d232        	addmi	a3, a2, 0x3400
     9bb:	8a2332        	l32i	a3, a3, 0x228
     9be:	78cee2        	addi	a14, a14, 120
     9c1:	0498           	l32i.n	a9, a4, 0
     9c3:	03d352        	addmi	a5, a3, 0x300
     9c6:	04d372        	addmi	a7, a3, 0x400
     9c9:	01d3f2        	addmi	a15, a3, 0x100
     9cc:	90cff2        	addi	a15, a15, -112
     9cf:	f8c762        	addi	a6, a7, -8
     9d2:	78c5b2        	addi	a11, a5, 120
     9d5:	41b9           	s32i.n	a11, a1, 16
     9d7:	06dd           	mov.n	a13, a6
     9d9:	0998           	l32i.n	a9, a9, 0
     9db:	b8c772        	addi	a7, a7, -72
     9de:	38c552        	addi	a5, a5, 56
     9e1:	05ad           	mov.n	a10, a5
     9e3:	07cd           	mov.n	a12, a7
     9e5:	06d992        	addmi	a9, a9, 0x600
     9e8:	e0c992        	addi	a9, a9, -32
     9eb:	0199           	s32i.n	a9, a1, 0
     9ed:	0008e0        	callx8	a8
     9f0:	2ad2b2        	addmi	a11, a2, 0x2a00
     9f3:	04d382        	addmi	a8, a3, 0x400
     9f6:	03d3f2        	addmi	a15, a3, 0x300
     9f9:	14d292        	addmi	a9, a2, 0x1400
     9fc:	05ad           	mov.n	a10, a5
     9fe:	27d2c2        	addmi	a12, a2, 0x2700
     a01:	b4a152        	movi	a5, 0x1b4
     a04:	8e2ce2        	l32i	a14, a12, 0x238
     a07:	882992        	l32i	a9, a9, 0x220
     a0a:	822ff2        	l32i	a15, a15, 0x208
     a0d:	b8f9           	s32i.n	a15, a8, 44
     a0f:	c899           	s32i.n	a9, a8, 48
     a11:	7e2cd2        	l32i	a13, a12, 0x1f8
     a14:	d0cb22        	addi	a2, a11, -48
     a17:	ec63e2        	s32i	a14, a3, 0x3b0
     a1a:	669bb2        	l16si	a11, a11, 204
     a1d:	dc63d2        	s32i	a13, a3, 0x370
     a20:	9e2cc2        	l32i	a12, a12, 0x278
     a23:	fc63c2        	s32i	a12, a3, 0x3f0
     a26:	04c8           	l32i.n	a12, a4, 0
     a28:	fdaa81        	l32r	a8, d0 (a98 <Fill_sbco>)
     a2b:	4d0c           	movi.n	a13, 4
     a2d:	02dcc2        	addmi	a12, a12, 0x200
     a30:	579cc2        	l16si	a12, a12, 174
     a33:	0008e0        	callx8	a8
     a36:	41a8           	l32i.n	a10, a1, 16
     a38:	fda681        	l32r	a8, d0 (a98 <Fill_sbco>)
     a3b:	04c8           	l32i.n	a12, a4, 0
     a3d:	7f92b2        	l16si	a11, a2, 254
     a40:	2d0c           	movi.n	a13, 2
     a42:	cc5a           	add.n	a12, a12, a5
     a44:	d2bbd0        	quos	a11, a11, a13
     a47:	8bbd           	ae_sext16	a11, a11
     a49:	7e9cc2        	l16si	a12, a12, 252
     a4c:	4d0c           	movi.n	a13, 4
     a4e:	0008e0        	callx8	a8
     a51:	07ad           	mov.n	a10, a7
     a53:	8092b2        	l16si	a11, a2, 0x100
     a56:	04c8           	l32i.n	a12, a4, 0
     a58:	4d0c           	movi.n	a13, 4
     a5a:	fd9d81        	l32r	a8, d0 (a98 <Fill_sbco>)
     a5d:	cc5a           	add.n	a12, a12, a5
     a5f:	7f9cc2        	l16si	a12, a12, 254
     a62:	0008e0        	callx8	a8
     a65:	06ad           	mov.n	a10, a6
     a67:	8192b2        	l16si	a11, a2, 0x102
     a6a:	04c8           	l32i.n	a12, a4, 0
     a6c:	4d0c           	movi.n	a13, 4
     a6e:	fd9881        	l32r	a8, d0 (a98 <Fill_sbco>)
     a71:	cc5a           	add.n	a12, a12, a5
     a73:	809cc2        	l16si	a12, a12, 0x100
     a76:	0008e0        	callx8	a8
     a79:	3c0c           	movi.n	a12, 3
     a7b:	fd6981        	l32r	a8, 20 (5620 <xt_memcpy>)
     a7e:	04b8           	l32i.n	a11, a4, 0
     a80:	05d3a2        	addmi	a10, a3, 0x500
     a83:	f6caa2        	addi	a10, a10, -10
     a86:	03dbb2        	addmi	a11, a11, 0x300
     a89:	88cbb2        	addi	a11, a11, -120
     a8c:	0008e0        	callx8	a8
     a8f:	416232        	s32i	a3, a2, 0x104
     a92:	f01d           	retw.n

00000a94 <SBAEC_INIT_inear+0xe8>:
     a94:	00000000                                ....

00000a98 <Fill_sbco>:
     a98:	004136        	entry	a1, 32
     a9b:	115232        	s16i	a3, a2, 34
     a9e:	125252        	s16i	a5, a2, 36
     aa1:	118400        	slli	a8, a4, 16
     aa4:	fd8c61        	l32r	a6, d4 (7fff <_end+0x12e3>)
     aa7:	105262        	s16i	a6, a2, 32
     aaa:	5289           	s32i.n	a8, a2, 20
     aac:	4289           	s32i.n	a8, a2, 16
     aae:	f01d           	retw.n

00000ab0 <Fill_CH_MEM_WB>:
     ab0:	004136        	entry	a1, 32
     ab3:	8188           	l32i.n	a8, a1, 32
     ab5:	a279           	s32i.n	a7, a2, 40
     ab7:	01d6b2        	addmi	a11, a6, 0x100
     aba:	ad9be2        	l16si	a14, a11, 0x15a
     abd:	aa9bc2        	l16si	a12, a11, 0x154
     ac0:	ac9ba2        	l16si	a10, a11, 0x158
     ac3:	ab9bb2        	l16si	a11, a11, 0x156
     ac6:	909c70        	addx2	a9, a12, a7
     ac9:	a399           	s32i.n	a9, a3, 40
     acb:	90db90        	addx2	a13, a11, a9
     ace:	a4d9           	s32i.n	a13, a4, 40
     ad0:	90dad0        	addx2	a13, a10, a13
     ad3:	a5d9           	s32i.n	a13, a5, 40
     ad5:	90eed0        	addx2	a14, a14, a13
     ad8:	90cce0        	addx2	a12, a12, a14
     adb:	90bbc0        	addx2	a11, a11, a12
     ade:	90aab0        	addx2	a10, a10, a11
     ae1:	b2e9           	s32i.n	a14, a2, 44
     ae3:	a2f8           	l32i.n	a15, a2, 40
     ae5:	a4e8           	l32i.n	a14, a4, 40
     ae7:	b3c9           	s32i.n	a12, a3, 44
     ae9:	b4b9           	s32i.n	a11, a4, 44
     aeb:	28c8c2        	addi	a12, a8, 40
     aee:	50c8b2        	addi	a11, a8, 80
     af1:	b5a9           	s32i.n	a10, a5, 44
     af3:	c399           	s32i.n	a9, a3, 48
     af5:	64c8a2        	addi	a10, a8, 100
     af8:	78c892        	addi	a9, a8, 120
     afb:	c2f9           	s32i.n	a15, a2, 48
     afd:	c4e9           	s32i.n	a14, a4, 48
     aff:	c5d9           	s32i.n	a13, a5, 48
     b01:	d389           	s32i.n	a8, a3, 52
     b03:	f3c9           	s32i.n	a12, a3, 60
     b05:	01d882        	addmi	a8, a8, 0x100
     b08:	d2b9           	s32i.n	a11, a2, 52
     b0a:	8cc8c2        	addi	a12, a8, -116
     b0d:	a0c8b2        	addi	a11, a8, -96
     b10:	d4a9           	s32i.n	a10, a4, 52
     b12:	f299           	s32i.n	a9, a2, 60
     b14:	c8c8a2        	addi	a10, a8, -56
     b17:	f0c892        	addi	a9, a8, -16
     b1a:	f4c9           	s32i.n	a12, a4, 60
     b1c:	884b           	addi.n	a8, a8, 4
     b1e:	f5b9           	s32i.n	a11, a5, 60
     b20:	e3a9           	s32i.n	a10, a3, 56
     b22:	e299           	s32i.n	a9, a2, 56
     b24:	e489           	s32i.n	a8, a4, 56
     b26:	f01d           	retw.n

00000b28 <common_x_pow>:
     b28:	006136        	entry	a1, 48
     b2b:	fd6e91        	l32r	a9, e4 (69a0 <immediate_num>)
     b2e:	36c434        	ae_cvtq48a32s	aeq3, a4
     b31:	fe84bec33f 	{ ae_cvtq48a32s	aeq0, a3; ae_absq56	aeq2, aeq3 }
     b36:	065864        	ae_absq56	aeq1, aeq0
     b39:	d65413622f 	{ or	a6, a2, a2; ae_ltq56s	b0, aeq1, aeq2 }
     b3e:	fd6751        	l32r	a5, dc (200000 <_end+0x1f92e4>)
     b41:	441904        	ae_lp16x2f.i	aep1, a9, 0
     b44:	200076        	bf	b0, b68 <common_x_pow+0x40>
     b47:	fb36fec57f 	{ ae_cvtq48a32s	aeq1, a5; ae_subq56	aeq3, aeq2, aeq1 }
     b4c:	d65d80012f 	{ movi	a2, 1; ae_ltq56s	b6, aeq1, aeq3 }
     b51:	fd6381        	l32r	a8, e0 (2aaa00 <_end+0x2a3ce4>)
     b54:	ff0462088f 	{ ae_movpa24x2	aep0, a8, a8; ae_movq56	aeq3, aeq0 }
     b59:	080676        	bf	b6, b65 <common_x_pow+0x3d>
     b5c:	030a14        	ae_mulfq32sp16s.l	aeq1, aeq0, aep0
     b5f:	06d934        	ae_roundsq32asym	aeq3, aeq1
     b62:	ffffc6        	j	b65 <common_x_pow+0x3d>

00000b65 <common_x_pow+0x3d>:
     b65:	000046        	j	b6a <common_x_pow+0x42>

00000b68 <common_x_pow+0x40>:
     b68:	020c           	movi.n	a2, 0

00000b6a <common_x_pow+0x42>:
     b6a:	608624        	ae_lq32f.i	aeq2, a6, 8
     b6d:	0f243d263f 	{ ae_lq32f.i	aeq1, a6, 12; ae_mulzaafq32sp16s.lh	aeq2, aeq2, aep1, aeq0, aep1 }
     b72:	c11974        	ae_mulzaafq32sp16s.lh	aeq1, aeq1, aep1, aeq3, aep1
     b75:	06a934        	ae_roundsq32asym	aeq2, aeq2
     b78:	fe2c79a62f 	{ ae_sq32f.i	aeq2, a6, 8; ae_roundsq32asym	aeq2, aeq1 }
     b7d:	622634        	ae_sq32f.i	aeq2, a6, 12
     b80:	f01d           	retw.n

00000b82 <common_x_pow+0x5a>:
	...

00000b84 <ref_gain_adapt>:
     b84:	004136        	entry	a1, 32
     b87:	fd5591        	l32r	a9, dc (200000 <_end+0x1f92e4>)
     b8a:	5378           	l32i.n	a7, a3, 20
     b8c:	fd5781        	l32r	a8, e8 (69a4 <immediate_num+0x4>)
     b8f:	360734        	ae_cvtq48a32s	aeq0, a7
     b92:	0205d6        	bgez	a5, bb6 <ref_gain_adapt+0x32>
     b95:	440804        	ae_lp16x2f.i	aep0, a8, 0
     b98:	059262        	l16si	a6, a2, 10
     b9b:	368434        	ae_cvtq48a32s	aeq2, a4
     b9e:	038a84        	ae_mulfq32sp16s.h	aeq3, aeq2, aep0
     ba1:	116600        	slli	a6, a6, 16
     ba4:	364634        	ae_cvtq48a32s	aeq1, a6
     ba7:	09b104        	ae_ltq56s	b0, aeq1, aeq3
     baa:	1e0076        	bf	b0, bcc <ref_gain_adapt+0x48>
     bad:	a10004        	ae_mulzaafq32sp16s.ll	aeq0, aeq0, aep0, aeq0, aep0

00000bb0 <ref_gain_adapt+0x2c>:
     bb0:	060934        	ae_roundsq32asym	aeq0, aeq0
     bb3:	368704        	ae_trunca32q48	a7, aeq0

00000bb6 <ref_gain_adapt+0x32>:
     bb6:	608344        	ae_lq32f.i	aeq2, a3, 16
     bb9:	539790        	max	a9, a7, a9
     bbc:	5399           	s32i.n	a9, a3, 20
     bbe:	440844        	ae_lp16x2f.i	aep0, a8, 16
     bc1:	c18084        	ae_mulzaafq32sp16s.lh	aeq2, aeq2, aep0, aeq0, aep0
     bc4:	066934        	ae_roundsq32asym	aeq1, aeq2
     bc7:	621344        	ae_sq32f.i	aeq1, a3, 16
     bca:	f01d           	retw.n

00000bcc <ref_gain_adapt+0x48>:
     bcc:	440814        	ae_lp16x2f.i	aep0, a8, 4
     bcf:	038a84        	ae_mulfq32sp16s.h	aeq3, aeq2, aep0
     bd2:	09b114        	ae_ltq56s	b1, aeq1, aeq3
     bd5:	050176        	bf	b1, bde <ref_gain_adapt+0x5a>
     bd8:	a10004        	ae_mulzaafq32sp16s.ll	aeq0, aeq0, aep0, aeq0, aep0
     bdb:	fff446        	j	bb0 <ref_gain_adapt+0x2c>

00000bde <ref_gain_adapt+0x5a>:
     bde:	440824        	ae_lp16x2f.i	aep0, a8, 8
     be1:	038a44        	ae_mulfq32sp16s.h	aeq3, aeq1, aep0
     be4:	09b224        	ae_ltq56s	b2, aeq2, aeq3
     be7:	d5e36d183f 	{ ae_lp16x2f.i	aep1, a8, 12; ae_mulfq32sp16s.l	aeq3, aeq1, aep0 }
     bec:	c00276        	bf	b2, bb0 <ref_gain_adapt+0x2c>
     bef:	09a334        	ae_ltq56s	b3, aeq3, aeq2
     bf2:	02d2b2        	addmi	a11, a2, 0x200
     bf5:	3a0c           	movi.n	a10, 3
     bf7:	051376        	bt	b3, c00 <ref_gain_adapt+0x7c>
     bfa:	4e0bb2        	l8ui	a11, a11, 78
     bfd:	af0ab7        	bnone	a10, a11, bb0 <ref_gain_adapt+0x2c>

00000c00 <ref_gain_adapt+0x7c>:
     c00:	031214        	ae_mulfq32sp16s.l	aeq0, aeq0, aep1
     c03:	ffea46        	j	bb0 <ref_gain_adapt+0x2c>

00000c06 <ref_gain_adapt+0x82>:
	...

00000c08 <aec_post_calc>:
     c08:	006136        	entry	a1, 48
     c0b:	fd3991        	l32r	a9, f0 (80000 <_end+0x792e4>)
     c0e:	fd37a1        	l32r	a10, ec (10000 <_end+0x92e4>)
     c11:	5d4466        	bnei	a4, 4, c72 <aec_post_calc+0x6a>
     c14:	364934        	ae_cvtq48a32s	aeq1, a9
     c17:	20c2d2        	addi	a13, a2, 32
     c1a:	400d04        	ae_lp16f.i	aep0, a13, 0
     c1d:	d4e3f023be 	{ l32i	a11, a3, 8; ae_mulfq32sp16u.l	aeq1, aeq1, aep0 }
     c22:	360a34        	ae_cvtq48a32s	aeq0, a10
     c25:	621104        	ae_sq32f.i	aeq1, a1, 0
     c28:	d4e2b001ce 	{ l32i	a12, a1, 0; ae_mulfq32sp16u.l	aeq0, aeq0, aep0 }
     c2d:	620104        	ae_sq32f.i	aeq0, a1, 0
     c30:	53bbc0        	max	a11, a11, a12

00000c33 <aec_post_calc+0x2b>:
     c33:	01c8           	l32i.n	a12, a1, 0
     c35:	33a8           	l32i.n	a10, a3, 12
     c37:	fd2f81        	l32r	a8, f4 (4d88 <airoha_divide_limit_1>)
     c3a:	53aac0        	max	a10, a10, a12
     c3d:	43aba0        	min	a10, a11, a10
     c40:	0008e0        	callx8	a8
     c43:	4c2244        	ae_lp24.i	aep2, a2, 16
     c46:	379aa4        	ae_truncp24a32x2	aep1, a10, a10
     c49:	60c364        	ae_lq32f.i	aeq3, a3, 24
     c4c:	019a34        	ae_mulafp24s.ll	aeq3, aep1, aep2
     c4f:	fe3c7d037f 	{ ae_lq32f.i	aeq0, a3, 28; ae_roundsq32asym	aeq2, aeq3 }
     c54:	622364        	ae_sq32f.i	aeq2, a3, 24
     c57:	1792e2        	l16si	a14, a2, 46
     c5a:	0292d2        	l16si	a13, a2, 4
     c5d:	403274        	ae_lp16f.i	aep3, a2, 14
     c60:	f866a162fe 	{ l16si	a15, a2, 44; ae_movp48	aep0, aep1 }
     c65:	2bade7        	bge	a13, a14, c94 <aec_post_calc+0x8c>
     c68:	0a9292        	l16si	a9, a2, 20
     c6b:	411fe6        	bgei	a15, 1, cb0 <aec_post_calc+0xa8>
     c6e:	001006        	j	cb2 <aec_post_calc+0xaa>

00000c71 <aec_post_calc+0x69>:
	...

00000c72 <aec_post_calc+0x6a>:
     c72:	364934        	ae_cvtq48a32s	aeq1, a9
     c75:	1ec2d2        	addi	a13, a2, 30
     c78:	401d04        	ae_lp16f.i	aep1, a13, 0
     c7b:	d4e7f023be 	{ l32i	a11, a3, 8; ae_mulfq32sp16u.l	aeq1, aeq1, aep1 }
     c80:	360a34        	ae_cvtq48a32s	aeq0, a10
     c83:	621104        	ae_sq32f.i	aeq1, a1, 0
     c86:	d4e6b001ce 	{ l32i	a12, a1, 0; ae_mulfq32sp16u.l	aeq0, aeq0, aep1 }
     c8b:	620104        	ae_sq32f.i	aeq0, a1, 0
     c8e:	53bbc0        	max	a11, a11, a12
     c91:	ffe786        	j	c33 <aec_post_calc+0x2b>

00000c94 <aec_post_calc+0x8c>:
     c94:	0b9292        	l16si	a9, a2, 22
     c97:	e90b           	addi.n	a14, a9, -1
     c99:	05ae76        	loopgtz	a14, ca2 <aec_post_calc+0x9a>
     c9c:	018174        	ae_mulfp24s.ll	aeq2, aep0, aep1
     c9f:	1bad04        	ae_roundsp24q48asym	aep0, aeq2

00000ca2 <aec_post_calc+0x9a>:
     ca2:	070b34        	ae_maxp24s	aep3, aep0, aep3
     ca5:	013234        	ae_mulafp24s.ll	aeq0, aep3, aep2
     ca8:	06c934        	ae_roundsq32asym	aeq3, aeq0
     cab:	623374        	ae_sq32f.i	aeq3, a3, 28
     cae:	f01d           	retw.n

00000cb0 <aec_post_calc+0xa8>:
     cb0:	992b           	addi.n	a9, a9, 2

00000cb2 <aec_post_calc+0xaa>:
     cb2:	031694        	wur.ae_sar	a9
     cb5:	0b1084        	ae_sllssp24s	aep0, aep1
     cb8:	fff986        	j	ca2 <aec_post_calc+0x9a>

00000cbb <aec_post_calc+0xb3>:
	...

00000cbc <aec_post_calc_FB>:
     cbc:	004136        	entry	a1, 32
     cbf:	972242        	l32i	a4, a2, 0x25c
     cc2:	fd0c81        	l32r	a8, f4 (4d88 <airoha_divide_limit_1>)
     cc5:	fcdab1        	l32r	a11, 30 (1000 <subband_aec_mid_channel+0x50>)
     cc8:	33c8           	l32i.n	a12, a3, 12
     cca:	23a8           	l32i.n	a10, a3, 8
     ccc:	00a2d2        	movi	a13, 0x200
     ccf:	53ccd0        	max	a12, a12, a13
     cd2:	53aab0        	max	a10, a10, a11
     cd5:	0abd           	mov.n	a11, a10
     cd7:	43aac0        	min	a10, a10, a12
     cda:	0008e0        	callx8	a8
     cdd:	4c2244        	ae_lp24.i	aep2, a2, 16
     ce0:	379aa4        	ae_truncp24a32x2	aep1, a10, a10
     ce3:	604364        	ae_lq32f.i	aeq1, a3, 24
     ce6:	011a34        	ae_mulafp24s.ll	aeq1, aep1, aep2
     ce9:	04d4a2        	addmi	a10, a4, 0x400
     cec:	061934        	ae_roundsq32asym	aeq0, aeq1
     cef:	620364        	ae_sq32f.i	aeq0, a3, 24
     cf2:	7e9af2        	l16si	a15, a10, 252
     cf5:	0292e2        	l16si	a14, a2, 4
     cf8:	05d4b2        	addmi	a11, a4, 0x500
     cfb:	f866bd037f 	{ ae_lq32f.i	aeq0, a3, 28; ae_movp48	aep0, aep1 }
     d00:	0caef7        	bge	a14, a15, d10 <aec_post_calc_FB+0x54>
     d03:	7f9a82        	l16si	a8, a10, 254
     d06:	0a9292        	l16si	a9, a2, 20
     d09:	2218e6        	bgei	a8, 1, d2f <aec_post_calc_FB+0x73>
     d0c:	000846        	j	d31 <aec_post_calc_FB+0x75>

00000d0f <aec_post_calc_FB+0x53>:
	...

00000d10 <aec_post_calc_FB+0x54>:
     d10:	0b9292        	l16si	a9, a2, 22
     d13:	a90b           	addi.n	a10, a9, -1
     d15:	05aa76        	loopgtz	a10, d1e <aec_post_calc_FB+0x62>
     d18:	018174        	ae_mulfp24s.ll	aeq2, aep0, aep1
     d1b:	1bad04        	ae_roundsp24q48asym	aep0, aeq2

00000d1e <aec_post_calc_FB+0x62>:
     d1e:	409bb4        	ae_lp16f.iu	aep1, a11, -10
     d21:	070914        	ae_maxp24s	aep1, aep0, aep1
     d24:	011234        	ae_mulafp24s.ll	aeq0, aep1, aep2
     d27:	06c934        	ae_roundsq32asym	aeq3, aeq0
     d2a:	623374        	ae_sq32f.i	aeq3, a3, 28
     d2d:	f01d           	retw.n

00000d2f <aec_post_calc_FB+0x73>:
     d2f:	992b           	addi.n	a9, a9, 2

00000d31 <aec_post_calc_FB+0x75>:
     d31:	031694        	wur.ae_sar	a9
     d34:	0b1084        	ae_sllssp24s	aep0, aep1
     d37:	fff8c6        	j	d1e <aec_post_calc_FB+0x62>

00000d3a <aec_post_calc_FB+0x7e>:
	...

00000d3c <subband_aec_block>:
     d3c:	008136        	entry	a1, 64
     d3f:	069d           	mov.n	a9, a6
     d41:	c3d8           	l32i.n	a13, a3, 48
     d43:	4e0c           	movi.n	a14, 4
     d45:	60c344        	ae_lq32f.i	aeq3, a3, 16
     d48:	119382        	l16si	a8, a3, 34
     d4b:	e3c8           	l32i.n	a12, a3, 56
     d4d:	9422b2        	l32i	a11, a2, 0x250
     d50:	2a0c           	movi.n	a10, 2
     d52:	0a23f2        	l32i	a15, a3, 40
     d55:	f3f6f0        	wur.ae_cbegin0	a15
     d58:	d2a4a0        	quos	a10, a4, a10
     d5b:	fccbb2        	addi	a11, a11, -4
     d5e:	440c04        	ae_lp16x2f.i	aep0, a12, 0
     d61:	fc249188ff 	{ addx2	a8, a8, a15; ae_zeroq56	aeq0 }
     d66:	f3f780        	wur.ae_cend0	a8
     d69:	19aa76        	loopgtz	a10, d86 <subband_aec_block+0x4a>
     d6c:	a180f4        	ae_mulzaafq32sp16s.ll	aeq2, aeq3, aep0, aeq3, aep0
     d6f:	b108f4        	ae_mulzaafq32sp16s.hh	aeq1, aeq3, aep0, aeq3, aep0
     d72:	f9739abdef 	{ ae_lp16x2f.c	aep3, a13, a14; ae_roundsp16q48asym	aep2, aeq2 }
     d77:	f86bed8c1f 	{ ae_lp16x2f.iu	aep0, a12, 4; ae_roundsp16q48asym	aep1, aeq1 }
     d7c:	f06e3e125f 	{ ae_selp24.hl	aep1, aep1, aep2; ae_mulssfp24s.hh.ll	aeq0, aep3, aep3 }
     d81:	e224339b1f 	{ ae_sp16x2f.iu	aep1, a11, 4; ae_mulaafp24s.hh.ll	aeq0, aep1, aep1 }

00000d86 <subband_aec_block+0x4a>:
     d86:	31a9           	s32i.n	a10, a1, 12
     d88:	35c234        	ae_sraiq56	aeq3, aeq0, 8
     d8b:	618304        	ae_lq56.i	aeq2, a3, 0
     d8e:	faf63092be 	{ l32i	a11, a2, 36; ae_addq56	aeq2, aeq2, aeq3 }
     d93:	fc80fecb7f 	{ ae_cvtq48a32s	aeq1, a11; ae_satq48s	aeq0, aeq2 }
     d98:	f86b14219f 	{ s32i.n	a9, a1, 8; ae_maxq56s	aeq0, aeq0, aeq1 }
     d9d:	630304        	ae_sq56s.i	aeq0, a3, 0
     da0:	0125d6        	bgez	a5, db6 <subband_aec_block+0x7a>
     da3:	1093a2        	l16si	a10, a3, 32
     da6:	fcd481        	l32r	a8, f8 (4c80 <airoha_divide>)
     da9:	368b04        	ae_trunca32q48	a11, aeq0
     dac:	11aa70        	slli	a10, a10, 9
     daf:	0008e0        	callx8	a8
     db2:	01b9           	s32i.n	a11, a1, 0
     db4:	11a9           	s32i.n	a10, a1, 4

00000db6 <subband_aec_block+0x7a>:
     db6:	270c           	movi.n	a7, 2
     db8:	c3a8           	l32i.n	a10, a3, 48
     dba:	119362        	l16si	a6, a3, 34
     dbd:	0f2392        	l32i	a9, a3, 60
     dc0:	9422d2        	l32i	a13, a2, 0x250
     dc3:	0b23c2        	l32i	a12, a3, 44
     dc6:	0d23e2        	l32i	a14, a3, 52
     dc9:	fecdd2        	addi	a13, a13, -2
     dcc:	feccc2        	addi	a12, a12, -2
     dcf:	feceb2        	addi	a11, a14, -2
     dd2:	fec9f2        	addi	a15, a9, -2
     dd5:	5e14a6        	blti	a4, 1, e37 <subband_aec_block+0xfb>
     dd8:	00a0e2        	movi	a14, 0

00000ddb <subband_aec_block+0x9f>:
     ddb:	0c9d           	mov.n	a9, a12
     ddd:	40dd14        	ae_lp16f.iu	aep5, a13, 2
     de0:	fc24e2da7f 	{ ae_sp16f.l.c	aep5, a10, a7; nop }
     de5:	40cb14        	ae_lp16f.iu	aep4, a11, 2
     de8:	fc8c2c891f 	{ ae_lp16f.iu	aep0, a9, 2; ae_cvtq48p24s.l	aeq0, aep4 }
     ded:	09a676        	loopgtz	a6, dfa <subband_aec_block+0xbe>
     df0:	fc24d96a7f 	{ ae_lp16f.c	aep6, a10, a7; nop }
     df5:	eec1ac891f 	{ ae_lp16f.iu	aep0, a9, 2; ae_mulsfp24s.ll	aeq0, aep0, aep6 }

00000dfa <subband_aec_block+0xbe>:
     dfa:	f863801eee 	{ addi	a14, a14, 1; ae_roundsp16q48asym	aep0, aeq0 }
     dff:	428f14        	ae_sp16f.l.iu	aep0, a15, 2
     e02:	02a5d6        	bgez	a5, e30 <subband_aec_block+0xf4>
     e05:	002192        	l32i	a9, a1, 0
     e08:	364934        	ae_cvtq48a32s	aeq1, a9
     e0b:	d4e370118e 	{ l32i	a8, a1, 4; ae_mulfq32sp16s.l	aeq1, aeq1, aep0 }
     e10:	209cc0        	or	a9, a12, a12
     e13:	fc24fe980f 	{ ae_slaasq56s	aeq0, aeq1, a8; nop }
     e18:	f967ac891f 	{ ae_lp16f.iu	aep0, a9, 2; ae_roundsp24q48asym	aep2, aeq0 }
     e1d:	0fa676        	loopgtz	a6, e30 <subband_aec_block+0xf4>

00000e20 <subband_aec_block+0xe4>:
     e20:	fe0c190a7f 	{ ae_lp16f.c	aep0, a10, a7; ae_cvtq48p24s.l	aeq2, aep0 }
     e25:	01a034        	ae_mulafp24s.ll	aeq2, aep2, aep0
     e28:	fb73ec891f 	{ ae_lp16f.iu	aep0, a9, 2; ae_roundsp16q48asym	aep7, aeq2 }
     e2d:	4279f4        	ae_sp16f.l.i	aep7, a9, -2
     e30:	a794e7        	bne	a4, a14, ddb <subband_aec_block+0x9f>
     e33:	d3e8           	l32i.n	a14, a3, 52
     e35:	f398           	l32i.n	a9, a3, 60

00000e37 <subband_aec_block+0xfb>:
     e37:	31b8           	l32i.n	a11, a1, 12
     e39:	0c63a2        	s32i	a10, a3, 48
     e3c:	fcc992        	addi	a9, a9, -4
     e3f:	fd248fceae 	{ addi	a10, a14, -4; ae_zeroq56	aeq1 }
     e44:	fe0c7f113f 	{ ae_movq56	aeq0, aeq1; ae_movq56	aeq2, aeq1 }
     e49:	0dab76        	loopgtz	a11, e5a <subband_aec_block+0x11e>
     e4c:	449a14        	ae_lp16x2f.iu	aep1, a10, 4
     e4f:	e2242da91f 	{ ae_lp16x2f.iu	aep2, a9, 4; ae_mulaafp24s.hh.ll	aeq0, aep1, aep1 }
     e54:	05a204        	ae_mulaafp24s.hh.ll	aeq2, aep2, aep2
     e57:	051a04        	ae_mulaafp24s.hh.ll	aeq1, aep1, aep2

00000e5a <subband_aec_block+0x11e>:
     e5a:	21c8           	l32i.n	a12, a1, 8
     e5c:	134ca6        	blti	a12, 4, e73 <subband_aec_block+0x137>
     e5f:	35e134        	ae_sraiq56	aeq3, aeq2, 4
     e62:	01efc4        	ae_subq56	aeq3, aeq2, aeq3
     e65:	09b004        	ae_ltq56s	b0, aeq0, aeq3
     e68:	030460        	rsr.br	a6
     e6b:	046060        	extui	a6, a6, 0, 1
     e6e:	0000c6        	j	e75 <subband_aec_block+0x139>

00000e71 <subband_aec_block+0x135>:
	...

00000e73 <subband_aec_block+0x137>:
     e73:	060c           	movi.n	a6, 0

00000e75 <subband_aec_block+0x139>:
     e75:	03ad           	mov.n	a10, a3
     e77:	fca181        	l32r	a8, fc (b28 <common_x_pow>)
     e7a:	35c134        	ae_sraiq56	aeq3, aeq0, 4
     e7d:	fd3c7b118f 	{ ae_sraiq56	aeq2, aeq1, 4; ae_roundsq32asym	aeq1, aeq3 }
     e82:	fc347f1bef 	{ ae_trunca32q48	a11, aeq1; ae_roundsq32asym	aeq0, aeq2 }
     e87:	368c04        	ae_trunca32q48	a12, aeq0
     e8a:	0008e0        	callx8	a8
     e8d:	9a6a           	add.n	a9, a10, a6
     e8f:	a98c           	beqz.n	a9, e9d <subband_aec_block+0x161>
     e91:	04cd           	mov.n	a12, a4
     e93:	fc6381        	l32r	a8, 20 (5620 <xt_memcpy>)
     e96:	f3a8           	l32i.n	a10, a3, 60
     e98:	d3b8           	l32i.n	a11, a3, 52
     e9a:	0008e0        	callx8	a8

00000e9d <subband_aec_block+0x161>:
     e9d:	2d0c           	movi.n	a13, 2
     e9f:	b3c8           	l32i.n	a12, a3, 44
     ea1:	0d92a2        	l16si	a10, a2, 26
     ea4:	fc24a1139e 	{ l16si	a9, a3, 34; ae_zeroq56	aeq0 }
     ea9:	630124        	ae_sq56s.i	aeq0, a1, 16
     eac:	11baf0        	slli	a11, a10, 1
     eaf:	9099c0        	addx2	a9, a9, a12
     eb2:	d2aad0        	quos	a10, a10, a13
     eb5:	8aad           	ae_sext16	a10, a10
     eb7:	c099b0        	sub	a9, a9, a11
     eba:	fcc992        	addi	a9, a9, -4
     ebd:	08aa76        	loopgtz	a10, ec9 <subband_aec_block+0x18d>
     ec0:	44b914        	ae_lp16x2f.iu	aep3, a9, 4
     ec3:	053304        	ae_mulaafp24s.hh.ll	aeq0, aep3, aep3
     ec6:	630124        	ae_sq56s.i	aeq0, a1, 16

00000ec9 <subband_aec_block+0x18d>:
     ec9:	fc8a81        	l32r	a8, f4 (4d88 <airoha_divide_limit_1>)
     ecc:	33a8           	l32i.n	a10, a3, 12
     ece:	610124        	ae_lq56.i	aeq0, a1, 16
     ed1:	3540b4        	ae_sraiq56	aeq1, aeq0, 2
     ed4:	fc2c4000bf 	{ movi	a11, 0; ae_roundsq32asym	aeq0, aeq1 }
     ed9:	53aab0        	max	a10, a10, a11
     edc:	630124        	ae_sq56s.i	aeq0, a1, 16
     edf:	23b8           	l32i.n	a11, a3, 8
     ee1:	0008e0        	callx8	a8
     ee4:	610124        	ae_lq56.i	aeq0, a1, 16
     ee7:	0492f2        	l16si	a15, a2, 8
     eea:	fc7ae1        	l32r	a14, d4 (7fff <_end+0x12e3>)
     eed:	3180a0        	srai	a8, a10, 16
     ef0:	1293b2        	l16si	a11, a3, 36
     ef3:	c0ee80        	sub	a14, a14, a8
     ef6:	53eef0        	max	a14, a14, a15
     ef9:	256b17        	bbci	a11, 1, f22 <subband_aec_block+0x1e6>
     efc:	05dd           	mov.n	a13, a5
     efe:	02ad           	mov.n	a10, a2
     f00:	03bd           	mov.n	a11, a3
     f02:	039292        	l16si	a9, a2, 6
     f05:	368404        	ae_trunca32q48	a4, aeq0
     f08:	fc7e81        	l32r	a8, 100 (b84 <ref_gain_adapt>)
     f0b:	8299e0        	mull	a9, a9, a14
     f0e:	04cd           	mov.n	a12, a4
     f10:	40d992        	addmi	a9, a9, 0x4000
     f13:	219f90        	srai	a9, a9, 15
     f16:	105392        	s16i	a9, a3, 32
     f19:	0008e0        	callx8	a8
     f1c:	1293b2        	l16si	a11, a3, 36
     f1f:	000146        	j	f28 <subband_aec_block+0x1ec>

00000f22 <subband_aec_block+0x1e6>:
     f22:	1053e2        	s16i	a14, a3, 32
     f25:	368404        	ae_trunca32q48	a4, aeq0

00000f28 <subband_aec_block+0x1ec>:
     f28:	0592a2        	l16si	a10, a2, 10
     f2b:	11aa00        	slli	a10, a10, 16
     f2e:	54aa47        	bge	a10, a4, f86 <subband_aec_block+0x24a>
     f31:	fc74a1        	l32r	a10, 104 (69b8 <immediate_num+0x18>)
     f34:	0692c2        	l16si	a12, a2, 12
     f37:	20bb70        	or	a11, a11, a7
     f3a:	1253b2        	s16i	a11, a3, 36
     f3d:	11cc00        	slli	a12, a12, 16
     f40:	05ac47        	bge	a12, a4, f49 <subband_aec_block+0x20d>
     f43:	403a04        	ae_lp16f.i	aep3, a10, 0
     f46:	000086        	j	f4c <subband_aec_block+0x210>

00000f49 <subband_aec_block+0x20d>:
     f49:	403a14        	ae_lp16f.i	aep3, a10, 2

00000f4c <subband_aec_block+0x210>:
     f4c:	b3e8           	l32i.n	a14, a3, 44
     f4e:	0d92d2        	l16si	a13, a2, 26
     f51:	1193a2        	l16si	a10, a3, 34
     f54:	2f0c           	movi.n	a15, 2
     f56:	11cdf0        	slli	a12, a13, 1
     f59:	90aae0        	addx2	a10, a10, a14
     f5c:	d2ddf0        	quos	a13, a13, a15
     f5f:	8ddd           	ae_sext16	a13, a13
     f61:	c0aac0        	sub	a10, a10, a12
     f64:	feca92        	addi	a9, a10, -2
     f67:	440a04        	ae_lp16x2f.i	aep0, a10, 0
     f6a:	181da6        	blti	a13, 1, f86 <subband_aec_block+0x24a>
     f6d:	12ad76        	loopgtz	a13, f83 <subband_aec_block+0x247>
     f70:	018b54        	ae_mulfp24s.hl	aeq3, aep0, aep3
     f73:	018374        	ae_mulfp24s.ll	aeq2, aep0, aep3
     f76:	fa7bed8a1f 	{ ae_lp16x2f.iu	aep0, a10, 4; ae_roundsp16q48asym	aep5, aeq3 }
     f7b:	fa73b2d91f 	{ ae_sp16f.l.iu	aep5, a9, 2; ae_roundsp16q48asym	aep4, aeq2 }
     f80:	42c914        	ae_sp16f.l.iu	aep4, a9, 2

00000f83 <subband_aec_block+0x247>:
     f83:	1293b2        	l16si	a11, a3, 36
     f86:	0d6b07        	bbci	a11, 0, f97 <subband_aec_block+0x25b>
     f89:	02ad           	mov.n	a10, a2
     f8b:	fc5f81        	l32r	a8, 108 (c08 <aec_post_calc>)
     f8e:	03bd           	mov.n	a11, a3
     f90:	21c8           	l32i.n	a12, a1, 8
     f92:	0008e0        	callx8	a8
     f95:	f01d           	retw.n

00000f97 <subband_aec_block+0x25b>:
     f97:	106b27        	bbci	a11, 2, fab <subband_aec_block+0x26f>
     f9a:	972292        	l32i	a9, a2, 0x25c
     f9d:	a98c           	beqz.n	a9, fab <subband_aec_block+0x26f>
     f9f:	03bd           	mov.n	a11, a3
     fa1:	fc5a81        	l32r	a8, 10c (cbc <aec_post_calc_FB>)
     fa4:	02ad           	mov.n	a10, a2
     fa6:	21c8           	l32i.n	a12, a1, 8
     fa8:	0008e0        	callx8	a8
     fab:	f01d           	retw.n

00000fad <subband_aec_block+0x271>:
     fad:	000000                                        ...

00000fb0 <subband_aec_mid_channel>:
     fb0:	008136        	entry	a1, 64
     fb3:	c398           	l32i.n	a9, a3, 48
     fb5:	60c344        	ae_lq32f.i	aeq3, a3, 16
     fb8:	460c           	movi.n	a6, 4
     fba:	2159           	s32i.n	a5, a1, 8
     fbc:	a80c           	movi.n	a8, 10
     fbe:	1193c2        	l16si	a12, a3, 34
     fc1:	a3d8           	l32i.n	a13, a3, 40
     fc3:	9422b2        	l32i	a11, a2, 0x250
     fc6:	f3f6d0        	wur.ae_cbegin0	a13
     fc9:	a0ccd0        	addx4	a12, a12, a13
     fcc:	e3d8           	l32i.n	a13, a3, 56
     fce:	fccbb2        	addi	a11, a11, -4
     fd1:	f3f7c0        	wur.ae_cend0	a12
     fd4:	fc24ad0d0f 	{ ae_lp16x2f.i	aep0, a13, 0; ae_zeroq56	aeq0 }
     fd9:	19a876        	loopgtz	a8, ff6 <subband_aec_mid_channel+0x46>
     fdc:	a180f4        	ae_mulzaafq32sp16s.ll	aeq2, aeq3, aep0, aeq3, aep0
     fdf:	b108f4        	ae_mulzaafq32sp16s.hh	aeq1, aeq3, aep0, aeq3, aep0
     fe2:	f9739ab96f 	{ ae_lp16x2f.c	aep3, a9, a6; ae_roundsp16q48asym	aep2, aeq2 }
     fe7:	f86bed8d1f 	{ ae_lp16x2f.iu	aep0, a13, 4; ae_roundsp16q48asym	aep1, aeq1 }
     fec:	f06e3e125f 	{ ae_selp24.hl	aep1, aep1, aep2; ae_mulssfp24s.hh.ll	aeq0, aep3, aep3 }
     ff1:	e224339b1f 	{ ae_sp16x2f.iu	aep1, a11, 4; ae_mulaafp24s.hh.ll	aeq0, aep1, aep1 }

00000ff6 <subband_aec_mid_channel+0x46>:
     ff6:	92e8           	l32i.n	a14, a2, 36
     ff8:	35c1b4        	ae_sraiq56	aeq3, aeq0, 6
     ffb:	618304        	ae_lq56.i	aeq2, a3, 0
     ffe:	01abc4        	ae_addq56	aeq2, aeq2, aeq3
    1001:	11eee0        	slli	a14, a14, 2
    1004:	fc80fece7f 	{ ae_cvtq48a32s	aeq1, a14; ae_satq48s	aeq0, aeq2 }
    1009:	0110c4        	ae_maxq56s	aeq0, aeq0, aeq1
    100c:	630304        	ae_sq56s.i	aeq0, a3, 0
    100f:	0154d6        	bgez	a4, 1028 <subband_aec_mid_channel+0x78>
    1012:	1093a2        	l16si	a10, a3, 32
    1015:	fc3881        	l32r	a8, f8 (4c80 <airoha_divide>)
    1018:	368b04        	ae_trunca32q48	a11, aeq0
    101b:	11aa50        	slli	a10, a10, 11
    101e:	0008e0        	callx8	a8
    1021:	0bed           	mov.n	a14, a11
    1023:	0add           	mov.n	a13, a10
    1025:	ffffc6        	j	1028 <subband_aec_mid_channel+0x78>

00001028 <subband_aec_mid_channel+0x78>:
    1028:	c398           	l32i.n	a9, a3, 48
    102a:	119352        	l16si	a5, a3, 34
    102d:	a70c           	movi.n	a7, 10
    102f:	11e9           	s32i.n	a14, a1, 4
    1031:	01d9           	s32i.n	a13, a1, 0
    1033:	b3c8           	l32i.n	a12, a3, 44
    1035:	d3f8           	l32i.n	a15, a3, 52
    1037:	f3a8           	l32i.n	a10, a3, 60
    1039:	9422b2        	l32i	a11, a2, 0x250
    103c:	fccff2        	addi	a15, a15, -4
    103f:	fecaa2        	addi	a10, a10, -2
    1042:	fccbb2        	addi	a11, a11, -4
    1045:	fecc82        	addi	a8, a12, -2
    1048:	3189           	s32i.n	a8, a1, 12

0000104a <subband_aec_mid_channel+0x9a>:
    104a:	44db14        	ae_lp16x2f.iu	aep5, a11, 4
    104d:	fc24e4596f 	{ ae_sp16x2f.c	aep5, a9, a6; nop }
    1052:	44cf14        	ae_lp16x2f.iu	aep4, a15, 4
    1055:	fd8413dccf 	{ or	a13, a12, a12; ae_cvtq48p24s.h	aeq1, aep4 }
    105a:	fc8c2d0c0f 	{ ae_lp16x2f.i	aep0, a12, 0; ae_cvtq48p24s.l	aeq0, aep4 }
    105f:	0ca576        	loopgtz	a5, 106f <subband_aec_mid_channel+0xbf>
    1062:	fc24dae96f 	{ ae_lp16x2f.c	aep6, a9, a6; nop }
    1067:	050ec4        	ae_mulssfp24s.hh.ll	aeq1, aep0, aep6
    106a:	eec02d8d1f 	{ ae_lp16x2f.iu	aep0, a13, 4; ae_mulsafp24s.hl.lh	aeq0, aep0, aep6 }

0000106f <subband_aec_mid_channel+0xbf>:
    106f:	f86b8ff77e 	{ addi	a7, a7, -1; ae_roundsp16q48asym	aep0, aeq1 }
    1074:	f963f28a1f 	{ ae_sp16f.l.iu	aep0, a10, 2; ae_roundsp16q48asym	aep3, aeq0 }
    1079:	42ba14        	ae_sp16f.l.iu	aep3, a10, 2
    107c:	0434d6        	bgez	a4, 10c3 <subband_aec_mid_channel+0x113>
    107f:	11d8           	l32i.n	a13, a1, 4
    1081:	368d34        	ae_cvtq48a32s	aeq2, a13
    1084:	038a94        	ae_mulfq32sp16s.l	aeq3, aeq2, aep0
    1087:	d7ee30018e 	{ l32i	a8, a1, 0; ae_mulfq32sp16s.l	aeq2, aeq2, aep3 }
    108c:	fc24feb80f 	{ ae_slaasq56s	aeq0, aeq3, a8; nop }
    1091:	fc24fea84f 	{ ae_slaasq56s	aeq1, aeq2, a8; nop }
    1096:	fa6fb031ee 	{ l32i	a14, a1, 12; ae_roundsp24q48asym	aep4, aeq1 }
    109b:	f967d3dccf 	{ or	a13, a12, a12; ae_roundsp24q48asym	aep3, aeq0 }
    10a0:	dd8ced0c0f 	{ ae_lp16x2f.i	aep0, a12, 0; ae_selp24.hl	aep3, aep3, aep4 }
    10a5:	1aa576        	loopgtz	a5, 10c3 <subband_aec_mid_channel+0x113>
    10a8:	0ac084        	ae_cvtq48p24s.l	aeq3, aep0
    10ab:	fc041af96f 	{ ae_lp16x2f.c	aep7, a9, a6; ae_cvtq48p24s.h	aeq0, aep0 }
    10b0:	053704        	ae_mulaafp24s.hh.ll	aeq0, aep3, aep7
    10b3:	05bf54        	ae_mulasfp24s.hl.lh	aeq3, aep3, aep7
    10b6:	fb63ad8d1f 	{ ae_lp16x2f.iu	aep0, a13, 4; ae_roundsp16q48asym	aep6, aeq0 }
    10bb:	fa7bf2ee1f 	{ ae_sp16f.l.iu	aep6, a14, 2; ae_roundsp16q48asym	aep5, aeq3 }
    10c0:	42de14        	ae_sp16f.l.iu	aep5, a14, 2
    10c3:	f83756        	bnez	a7, 104a <subband_aec_mid_channel+0x9a>
    10c6:	ae0c           	movi.n	a14, 10
    10c8:	d3f8           	l32i.n	a15, a3, 52
    10ca:	c399           	s32i.n	a9, a3, 48
    10cc:	fe24b0f39e 	{ l32i	a9, a3, 60; ae_zeroq56	aeq2 }
    10d1:	632124        	ae_sq56s.i	aeq2, a1, 16
    10d4:	fd146d0f0f 	{ ae_lp16x2f.i	aep0, a15, 0; ae_movq56	aeq1, aeq2 }
    10d9:	fc146d390f 	{ ae_lp16x2f.i	aep3, a9, 0; ae_movq56	aeq0, aeq2 }
    10de:	10ae76        	loopgtz	a14, 10f2 <subband_aec_mid_channel+0x142>
    10e1:	050004        	ae_mulaafp24s.hh.ll	aeq0, aep0, aep0
    10e4:	050b04        	ae_mulaafp24s.hh.ll	aeq1, aep0, aep3

000010e7 <subband_aec_mid_channel+0x137>:
    10e7:	eb602db91f 	{ ae_lp16x2f.iu	aep3, a9, 4; ae_mulasfp24s.hl.lh	aeq2, aep0, aep3 }
    10ec:	630124        	ae_sq56s.i	aeq0, a1, 16
    10ef:	448f14        	ae_lp16x2f.iu	aep0, a15, 4

000010f2 <subband_aec_mid_channel+0x142>:
    10f2:	fbd081        	l32r	a8, 34 (5b4 <HIFI_sqrt>)
    10f5:	35a134        	ae_sraiq56	aeq2, aeq2, 4
    10f8:	f877fb114f 	{ ae_sraiq56	aeq1, aeq1, 4; ae_roundsp24q48asym	aep1, aeq2 }
    10fd:	1b9d04        	ae_roundsp24q48asym	aep0, aeq1
    1100:	dc20bda12f 	{ ae_lq56.i	aeq1, a1, 16; ae_selp24.hl	aep0, aep0, aep1 }
    1105:	f002bb118f 	{ ae_sraiq56	aeq2, aeq1, 4; ae_mulzaafp24s.hh.ll	aeq0, aep0, aep0 }
    110a:	fd344000af 	{ movi	a10, 0; ae_roundsq32asym	aeq1, aeq2 }
    110f:	ff247a112f 	{ ae_sq56s.i	aeq1, a1, 16; ae_roundsq32asym	aeq3, aeq0 }
    1114:	36bb04        	ae_trunca32q48	a11, aeq3
    1117:	0008e0        	callx8	a8
    111a:	fbf881        	l32r	a8, fc (b28 <common_x_pow>)
    111d:	614124        	ae_lq56.i	aeq1, a1, 16
    1120:	368b34        	ae_cvtq48a32s	aeq2, a11
    1123:	fc24feaa0f 	{ ae_slaasq56s	aeq0, aeq2, a10; nop }
    1128:	369b04        	ae_trunca32q48	a11, aeq1
    112b:	ff2453a33f 	{ or	a10, a3, a3; ae_roundsq32asym	aeq3, aeq0 }
    1130:	36bc04        	ae_trunca32q48	a12, aeq3
    1133:	0008e0        	callx8	a8
    1136:	ca8c           	beqz.n	a10, 1146 <subband_aec_mid_channel+0x196>
    1138:	f3a8           	l32i.n	a10, a3, 60
    113a:	fbb981        	l32r	a8, 20 (5620 <xt_memcpy>)
    113d:	0d23b2        	l32i	a11, a3, 52
    1140:	14a0c2        	movi	a12, 20
    1143:	0008e0        	callx8	a8

00001146 <subband_aec_mid_channel+0x196>:
    1146:	0d92a2        	l16si	a10, a2, 26
    1149:	119392        	l16si	a9, a3, 34
    114c:	fc24b0b3be 	{ l32i	a11, a3, 44; ae_zeroq56	aeq0 }
    1151:	630134        	ae_sq56s.i	aeq0, a1, 24
    1154:	c099a0        	sub	a9, a9, a10
    1157:	a099b0        	addx4	a9, a9, a11
    115a:	fcc992        	addi	a9, a9, -4
    115d:	08aa76        	loopgtz	a10, 1169 <subband_aec_mid_channel+0x1b9>
    1160:	44a914        	ae_lp16x2f.iu	aep2, a9, 4
    1163:	052204        	ae_mulaafp24s.hh.ll	aeq0, aep2, aep2
    1166:	630134        	ae_sq56s.i	aeq0, a1, 24

00001169 <subband_aec_mid_channel+0x1b9>:
    1169:	33a8           	l32i.n	a10, a3, 12
    116b:	fbe281        	l32r	a8, f4 (4d88 <airoha_divide_limit_1>)
    116e:	61c134        	ae_lq56.i	aeq3, a1, 24
    1171:	3530b4        	ae_sraiq56	aeq0, aeq3, 2
    1174:	ff247023be 	{ l32i	a11, a3, 8; ae_roundsq32asym	aeq3, aeq0 }
    1179:	633134        	ae_sq56s.i	aeq3, a1, 24
    117c:	0008e0        	callx8	a8
    117f:	610134        	ae_lq56.i	aeq0, a1, 24
    1182:	1293b2        	l16si	a11, a3, 36
    1185:	fbd3e1        	l32r	a14, d4 (7fff <_end+0x12e3>)
    1188:	0492f2        	l16si	a15, a2, 8
    118b:	3160a0        	srai	a6, a10, 16
    118e:	c0ee60        	sub	a14, a14, a6
    1191:	53eef0        	max	a14, a14, a15
    1194:	260c           	movi.n	a6, 2
    1196:	266b17        	bbci	a11, 1, 11c0 <subband_aec_mid_channel+0x210>
    1199:	02ad           	mov.n	a10, a2
    119b:	03bd           	mov.n	a11, a3
    119d:	04dd           	mov.n	a13, a4
    119f:	039292        	l16si	a9, a2, 6
    11a2:	368404        	ae_trunca32q48	a4, aeq0
    11a5:	fbd681        	l32r	a8, 100 (b84 <ref_gain_adapt>)
    11a8:	8299e0        	mull	a9, a9, a14
    11ab:	04cd           	mov.n	a12, a4
    11ad:	40d992        	addmi	a9, a9, 0x4000
    11b0:	219f90        	srai	a9, a9, 15
    11b3:	105392        	s16i	a9, a3, 32
    11b6:	0008e0        	callx8	a8
    11b9:	1293b2        	l16si	a11, a3, 36
    11bc:	000186        	j	11c6 <subband_aec_mid_channel+0x216>

000011bf <subband_aec_mid_channel+0x20f>:
	...

000011c0 <subband_aec_mid_channel+0x210>:
    11c0:	1053e2        	s16i	a14, a3, 32
    11c3:	368404        	ae_trunca32q48	a4, aeq0

000011c6 <subband_aec_mid_channel+0x216>:
    11c6:	0592a2        	l16si	a10, a2, 10
    11c9:	11aa00        	slli	a10, a10, 16
    11cc:	4aaa47        	bge	a10, a4, 121a <subband_aec_mid_channel+0x26a>
    11cf:	fbcda1        	l32r	a10, 104 (69b8 <immediate_num+0x18>)
    11d2:	0692c2        	l16si	a12, a2, 12
    11d5:	20bb60        	or	a11, a11, a6
    11d8:	1253b2        	s16i	a11, a3, 36
    11db:	11cc00        	slli	a12, a12, 16
    11de:	05ac47        	bge	a12, a4, 11e7 <subband_aec_mid_channel+0x237>
    11e1:	403a04        	ae_lp16f.i	aep3, a10, 0
    11e4:	000086        	j	11ea <subband_aec_mid_channel+0x23a>

000011e7 <subband_aec_mid_channel+0x237>:
    11e7:	403a14        	ae_lp16f.i	aep3, a10, 2

000011ea <subband_aec_mid_channel+0x23a>:
    11ea:	0d92d2        	l16si	a13, a2, 26
    11ed:	1193a2        	l16si	a10, a3, 34
    11f0:	b3c8           	l32i.n	a12, a3, 44
    11f2:	c0aad0        	sub	a10, a10, a13
    11f5:	a0aac0        	addx4	a10, a10, a12
    11f8:	feca92        	addi	a9, a10, -2
    11fb:	440a04        	ae_lp16x2f.i	aep0, a10, 0
    11fe:	181da6        	blti	a13, 1, 121a <subband_aec_mid_channel+0x26a>
    1201:	12ad76        	loopgtz	a13, 1217 <subband_aec_mid_channel+0x267>
    1204:	018354        	ae_mulfp24s.hl	aeq2, aep0, aep3
    1207:	010b74        	ae_mulfp24s.ll	aeq1, aep0, aep3
    120a:	fa73ed8a1f 	{ ae_lp16x2f.iu	aep0, a10, 4; ae_roundsp16q48asym	aep5, aeq2 }
    120f:	fa6bb2d91f 	{ ae_sp16f.l.iu	aep5, a9, 2; ae_roundsp16q48asym	aep4, aeq1 }
    1214:	42c914        	ae_sp16f.l.iu	aep4, a9, 2

00001217 <subband_aec_mid_channel+0x267>:
    1217:	1293b2        	l16si	a11, a3, 36
    121a:	0b6b07        	bbci	a11, 0, 1229 <subband_aec_mid_channel+0x279>
    121d:	03bd           	mov.n	a11, a3
    121f:	fbba81        	l32r	a8, 108 (c08 <aec_post_calc>)
    1222:	02ad           	mov.n	a10, a2
    1224:	21c8           	l32i.n	a12, a1, 8
    1226:	0008e0        	callx8	a8

00001229 <subband_aec_mid_channel+0x279>:
    1229:	f01d           	retw.n

0000122b <subband_aec_mid_channel+0x27b>:
	...

0000122c <dt_detect>:
    122c:	004136        	entry	a1, 32
    122f:	0c9242        	l16si	a4, a2, 24
    1232:	179262        	l16si	a6, a2, 46
    1235:	189282        	l16si	a8, a2, 48
    1238:	114400        	slli	a4, a4, 16
    123b:	1aa437        	bge	a4, a3, 1259 <dt_detect+0x2d>
    123e:	80a052        	movi	a5, 128
    1241:	14a587        	bge	a5, a8, 1259 <dt_detect+0x2d>
    1244:	169232        	l16si	a3, a2, 44
    1247:	0e9252        	l16si	a5, a2, 28
    124a:	662b           	addi.n	a6, a6, 2
    124c:	334b           	addi.n	a3, a3, 4
    124e:	433350        	min	a3, a3, a5
    1251:	833d           	ae_sext16	a3, a3
    1253:	165232        	s16i	a3, a2, 44
    1256:	0002c6        	j	1265 <dt_detect+0x39>

00001259 <dt_detect+0x2d>:
    1259:	660b           	addi.n	a6, a6, -1
    125b:	030c           	movi.n	a3, 0
    125d:	0e9252        	l16si	a5, a2, 28
    1260:	040c           	movi.n	a4, 0
    1262:	165242        	s16i	a4, a2, 44

00001265 <dt_detect+0x39>:
    1265:	538630        	max	a8, a6, a3
    1268:	438850        	min	a8, a8, a5
    126b:	175282        	s16i	a8, a2, 46
    126e:	f01d           	retw.n

00001270 <dt_detect_FB>:
    1270:	004136        	entry	a1, 32
    1273:	04d272        	addmi	a7, a2, 0x400
    1276:	7c9742        	l16si	a4, a7, 248
    1279:	fcc762        	addi	a6, a7, -4
    127c:	7e9722        	l16si	a2, a7, 252
    127f:	114400        	slli	a4, a4, 16
    1282:	1da437        	bge	a4, a3, 12a3 <dt_detect_FB+0x33>
    1285:	7a9782        	l16si	a8, a7, 244
    1288:	7f9732        	l16si	a3, a7, 254
    128b:	80a052        	movi	a5, 128
    128e:	11a587        	bge	a5, a8, 12a3 <dt_detect_FB+0x33>
    1291:	7d9752        	l16si	a5, a7, 250
    1294:	222b           	addi.n	a2, a2, 2
    1296:	334b           	addi.n	a3, a3, 4
    1298:	433350        	min	a3, a3, a5
    129b:	833d           	ae_sext16	a3, a3
    129d:	7f5732        	s16i	a3, a7, 254
    12a0:	0002c6        	j	12af <dt_detect_FB+0x3f>

000012a3 <dt_detect_FB+0x33>:
    12a3:	220b           	addi.n	a2, a2, -1
    12a5:	030c           	movi.n	a3, 0
    12a7:	7f9652        	l16si	a5, a6, 254
    12aa:	040c           	movi.n	a4, 0
    12ac:	815642        	s16i	a4, a6, 0x102

000012af <dt_detect_FB+0x3f>:
    12af:	538230        	max	a8, a2, a3
    12b2:	438850        	min	a8, a8, a5
    12b5:	805682        	s16i	a8, a6, 0x100
    12b8:	f01d           	retw.n

000012ba <dt_detect_FB+0x4a>:
    12ba:	00000000 41360000                                ......

000012c0 <get_ECNR_SVN>:
    12c0:	004136        	entry	a1, 32
    12c3:	4aa422        	movi	a2, 0x44a
    12c6:	f01d           	retw.n

000012c8 <get_ec120_memsize>:
    12c8:	004136        	entry	a1, 32
    12cb:	fb9121        	l32r	a2, 110 (3630 <cali_power+0x258>)
    12ce:	f01d           	retw.n

000012d0 <get_ec120_inear_memsize>:
    12d0:	004136        	entry	a1, 32
    12d3:	fb9021        	l32r	a2, 114 (3b48 <beam2_proc+0x224>)
    12d6:	f01d           	retw.n

000012d8 <sb_ana_init>:
    12d8:	004136        	entry	a1, 32
    12db:	0b0c           	movi.n	a11, 0
    12dd:	ec1c           	movi.n	a12, 30
    12df:	fb7881        	l32r	a8, c0 (5608 <xt_memset>)
    12e2:	a24b           	addi.n	a10, a2, 4
    12e4:	02a9           	s32i.n	a10, a2, 0
    12e6:	0008e0        	callx8	a8
    12e9:	f01d           	retw.n

000012eb <sb_ana_init+0x13>:
	...

000012ec <sb_syn_init>:
    12ec:	004136        	entry	a1, 32
    12ef:	0b0c           	movi.n	a11, 0
    12f1:	cc3c           	movi.n	a12, 60
    12f3:	fb7381        	l32r	a8, c0 (5608 <xt_memset>)
    12f6:	a24b           	addi.n	a10, a2, 4
    12f8:	02a9           	s32i.n	a10, a2, 0
    12fa:	0008e0        	callx8	a8
    12fd:	f01d           	retw.n

000012ff <sb_syn_init+0x13>:
	...

00001300 <HP_coef_setup>:
    1300:	004136        	entry	a1, 32
    1303:	02ad           	mov.n	a10, a2
    1305:	fb4681        	l32r	a8, 20 (5620 <xt_memcpy>)
    1308:	03bd           	mov.n	a11, a3
    130a:	4c0c           	movi.n	a12, 4
    130c:	0008e0        	callx8	a8
    130f:	a2cb           	addi.n	a10, a2, 12
    1311:	fb4381        	l32r	a8, 20 (5620 <xt_memcpy>)
    1314:	b38b           	addi.n	a11, a3, 8
    1316:	ac0c           	movi.n	a12, 10
    1318:	0008e0        	callx8	a8
    131b:	1cc3b2        	addi	a11, a3, 28
    131e:	fb4081        	l32r	a8, 20 (5620 <xt_memcpy>)
    1321:	24c2a2        	addi	a10, a2, 36
    1324:	8c0c           	movi.n	a12, 8
    1326:	0008e0        	callx8	a8
    1329:	7298           	l32i.n	a9, a2, 28
    132b:	12a8           	l32i.n	a10, a2, 4
    132d:	22a9           	s32i.n	a10, a2, 8
    132f:	8299           	s32i.n	a9, a2, 32
    1331:	f01d           	retw.n

00001333 <HP_coef_setup+0x33>:
	...

00001334 <tx_ref_vad_init>:
    1334:	004136        	entry	a1, 32
    1337:	02ad           	mov.n	a10, a2
    1339:	fb6181        	l32r	a8, c0 (5608 <xt_memset>)
    133c:	0b0c           	movi.n	a11, 0
    133e:	ec0c           	movi.n	a12, 14
    1340:	0008e0        	callx8	a8
    1343:	03d3b2        	addmi	a11, a3, 0x300
    1346:	fb3681        	l32r	a8, 20 (5620 <xt_memcpy>)
    1349:	02ad           	mov.n	a10, a2
    134b:	7c0c           	movi.n	a12, 7
    134d:	0008e0        	callx8	a8
    1350:	329d           	ae_l16si.n	a9, a2, 2
    1352:	999a           	add.n	a9, a9, a9
    1354:	729d           	ae_s16i.n	a9, a2, 2
    1356:	f01d           	retw.n

00001358 <Voice_EC120_Init>:
    1358:	006136        	entry	a1, 48
    135b:	02ad           	mov.n	a10, a2
    135d:	04bd           	mov.n	a11, a4
    135f:	fb3121        	l32r	a2, 24 (6d14 <p_ecnr_mem>)
    1362:	fb6d81        	l32r	a8, 118 (448 <TX_ECNR_PARA_init>)
    1365:	0c0c           	movi.n	a12, 0
    1367:	0239           	s32i.n	a3, a2, 0
    1369:	0008e0        	callx8	a8
    136c:	fb2f41        	l32r	a4, 28 (6d0c <aec_nr_para>)
    136f:	02a8           	l32i.n	a10, a2, 0
    1371:	0498           	l32i.n	a9, a4, 0
    1373:	fb6a81        	l32r	a8, 11c (1300 <HP_coef_setup>)
    1376:	10daa2        	addmi	a10, a10, 0x1000
    1379:	01d9b2        	addmi	a11, a9, 0x100
    137c:	bb8b           	addi.n	a11, a11, 8
    137e:	231992        	l16ui	a9, a9, 70
    1381:	659d           	ae_s16i.n	a9, a5, 0
    1383:	0008e0        	callx8	a8
    1386:	0b0c           	movi.n	a11, 0
    1388:	02a8           	l32i.n	a10, a2, 0
    138a:	a4a2c2        	movi	a12, 0x2a4
    138d:	fb4c81        	l32r	a8, c0 (5608 <xt_memset>)
    1390:	31daa2        	addmi	a10, a10, 0x3100
    1393:	9ccaa2        	addi	a10, a10, -100
    1396:	0008e0        	callx8	a8
    1399:	0b0c           	movi.n	a11, 0
    139b:	02a8           	l32i.n	a10, a2, 0
    139d:	ec0c           	movi.n	a12, 14
    139f:	fb4881        	l32r	a8, c0 (5608 <xt_memset>)
    13a2:	10daa2        	addmi	a10, a10, 0x1000
    13a5:	34caa2        	addi	a10, a10, 52
    13a8:	0008e0        	callx8	a8
    13ab:	fb5d31        	l32r	a3, 120 (1504 <Voice_EC120_Init+0x1ac>)
    13ae:	0b0c           	movi.n	a11, 0
    13b0:	02a8           	l32i.n	a10, a2, 0
    13b2:	78a0c2        	movi	a12, 120
    13b5:	fb4281        	l32r	a8, c0 (5608 <xt_memset>)
    13b8:	15daa2        	addmi	a10, a10, 0x1500
    13bb:	aa4b           	addi.n	a10, a10, 4
    13bd:	0008e0        	callx8	a8
    13c0:	02a8           	l32i.n	a10, a2, 0
    13c2:	fb5881        	l32r	a8, 124 (12d8 <sb_ana_init>)
    13c5:	2bdaa2        	addmi	a10, a10, 0x2b00
    13c8:	d8caa2        	addi	a10, a10, -40
    13cb:	0008e0        	callx8	a8
    13ce:	02a8           	l32i.n	a10, a2, 0
    13d0:	fb5581        	l32r	a8, 124 (12d8 <sb_ana_init>)
    13d3:	2bdaa2        	addmi	a10, a10, 0x2b00
    13d6:	18caa2        	addi	a10, a10, 24
    13d9:	0008e0        	callx8	a8
    13dc:	02a8           	l32i.n	a10, a2, 0
    13de:	fb5281        	l32r	a8, 128 (12ec <sb_syn_init>)
    13e1:	2bdaa2        	addmi	a10, a10, 0x2b00
    13e4:	58caa2        	addi	a10, a10, 88
    13e7:	0008e0        	callx8	a8
    13ea:	04c8           	l32i.n	a12, a4, 0
    13ec:	02d8           	l32i.n	a13, a2, 0
    13ee:	fb4f81        	l32r	a8, 12c (708 <SBAEC_INIT>)
    13f1:	0cb8           	l32i.n	a11, a12, 0
    13f3:	ad3a           	add.n	a10, a13, a3
    13f5:	331ce2        	l16ui	a14, a12, 102
    13f8:	0fddd2        	addmi	a13, a13, 0xf00
    13fb:	a85de2        	s16i	a14, a13, 0x150
    13fe:	341cc2        	l16ui	a12, a12, 104
    1401:	78cbb2        	addi	a11, a11, 120
    1404:	50cb92        	addi	a9, a11, 80
    1407:	78cbf2        	addi	a15, a11, 120
    140a:	a95dc2        	s16i	a12, a13, 0x152
    140d:	416ab2        	s32i	a11, a10, 0x104
    1410:	28cfe2        	addi	a14, a15, 40
    1413:	436af2        	s32i	a15, a10, 0x10c
    1416:	446ae2        	s32i	a14, a10, 0x110
    1419:	426a92        	s32i	a9, a10, 0x108
    141c:	78cfc2        	addi	a12, a15, 120
    141f:	50cfd2        	addi	a13, a15, 80
    1422:	456ad2        	s32i	a13, a10, 0x114
    1425:	466ac2        	s32i	a12, a10, 0x118
    1428:	78cc92        	addi	a9, a12, 120
    142b:	50ccb2        	addi	a11, a12, 80
    142e:	486ab2        	s32i	a11, a10, 0x120
    1431:	4a6a92        	s32i	a9, a10, 0x128
    1434:	28ccc2        	addi	a12, a12, 40
    1437:	476ac2        	s32i	a12, a10, 0x11c
    143a:	0008e0        	callx8	a8
    143d:	fb3c81        	l32r	a8, 130 (432c <NOISE_GEN_init>)
    1440:	0008e0        	callx8	a8
    1443:	0b0c           	movi.n	a11, 0
    1445:	2c1c           	movi.n	a12, 18
    1447:	0268           	l32i.n	a6, a2, 0
    1449:	0458           	l32i.n	a5, a4, 0
    144b:	fb1d81        	l32r	a8, c0 (5608 <xt_memset>)
    144e:	36d662        	addmi	a6, a6, 0x3600
    1451:	7558           	l32i.n	a5, a5, 28
    1453:	0159           	s32i.n	a5, a1, 0
    1455:	664b           	addi.n	a6, a6, 4
    1457:	06ad           	mov.n	a10, a6
    1459:	0008e0        	callx8	a8
    145c:	fb37e1        	l32r	a14, 138 (8000 <_end+0x12e4>)
    145f:	04c8           	l32i.n	a12, a4, 0
    1461:	fb34f1        	l32r	a15, 134 (d190210 <_end+0xd1894f4>)
    1464:	fb1981        	l32r	a8, c8 (155555 <_end+0x14e839>)
    1467:	301cb2        	l16ui	a11, a12, 96
    146a:	1e1cd2        	l16ui	a13, a12, 60
    146d:	c08580        	sub	a8, a5, a8
    1470:	835f80        	moveqz	a5, a15, a8
    1473:	0159           	s32i.n	a5, a1, 0
    1475:	74a850        	extui	a10, a5, 8, 8
    1478:	749050        	extui	a9, a5, 0, 8
    147b:	758850        	extui	a8, a5, 24, 8
    147e:	201cf2        	l16ui	a15, a12, 64
    1481:	0356f2        	s16i	a15, a6, 6
    1484:	c0eed0        	sub	a14, a14, a13
    1487:	204682        	s8i	a8, a6, 32
    148a:	c199b0        	mul16u	a9, a9, a11
    148d:	fb0c81        	l32r	a8, c0 (5608 <xt_memset>)
    1490:	1b0c           	movi.n	a11, 1
    1492:	11ee00        	slli	a14, a14, 16
    1495:	75f050        	extui	a15, a5, 16, 8
    1498:	2146f2        	s8i	a15, a6, 33
    149b:	ddea           	add.n	a13, a13, a14
    149d:	219590        	srai	a9, a9, 5
    14a0:	1f1ce2        	l16ui	a14, a12, 62
    14a3:	0e5692        	s16i	a9, a6, 28
    14a6:	321cc2        	l16ui	a12, a12, 100
    14a9:	06d9           	s32i.n	a13, a6, 0
    14ab:	0256e2        	s16i	a14, a6, 4
    14ae:	c1aac0        	mul16u	a10, a10, a12
    14b1:	fb22e1        	l32r	a14, 13c (6d18 <avc_vol>)
    14b4:	21a5a0        	srai	a10, a10, 5
    14b7:	0f56a2        	s16i	a10, a6, 30
    14ba:	02a8           	l32i.n	a10, a2, 0
    14bc:	fb21d1        	l32r	a13, 140 (2000 <Voice_EC_Prcs_common+0x314>)
    14bf:	6edd           	ae_s16i.n	a13, a14, 0
    14c1:	ca3a           	add.n	a12, a10, a3
    14c3:	7f5cb2        	s16i	a11, a12, 254
    14c6:	23daa2        	addmi	a10, a10, 0x2300
    14c9:	78caa2        	addi	a10, a10, 120
    14cc:	0b0c           	movi.n	a11, 0
    14ce:	0c5c           	movi.n	a12, 80
    14d0:	0008e0        	callx8	a8
    14d3:	0b0c           	movi.n	a11, 0
    14d5:	02a8           	l32i.n	a10, a2, 0
    14d7:	4c2c           	movi.n	a12, 36
    14d9:	faf981        	l32r	a8, c0 (5608 <xt_memset>)
    14dc:	24daa2        	addmi	a10, a10, 0x2400
    14df:	18caa2        	addi	a10, a10, 24
    14e2:	0008e0        	callx8	a8
    14e5:	02a8           	l32i.n	a10, a2, 0
    14e7:	ba3a           	add.n	a11, a10, a3
    14e9:	7d9bb2        	l16si	a11, a11, 250
    14ec:	2d2b66        	bnei	a11, 2, 151d <Voice_EC120_Init+0x1c5>
    14ef:	fb1581        	l32r	a8, 144 (2bfc <TWO_MIC_WB_init>)
    14f2:	04a8           	l32i.n	a10, a4, 0
    14f4:	0008e0        	callx8	a8
    14f7:	02a8           	l32i.n	a10, a2, 0
    14f9:	fb0a81        	l32r	a8, 124 (12d8 <sb_ana_init>)
    14fc:	2cdaa2        	addmi	a10, a10, 0x2c00
    14ff:	d4caa2        	addi	a10, a10, -44
    1502:	0008e0        	callx8	a8
    1505:	02a8           	l32i.n	a10, a2, 0
    1507:	fb0881        	l32r	a8, 128 (12ec <sb_syn_init>)
    150a:	2cdaa2        	addmi	a10, a10, 0x2c00
    150d:	14caa2        	addi	a10, a10, 20
    1510:	0008e0        	callx8	a8
    1513:	fb0d81        	l32r	a8, 148 (4530 <NC_NOISE_EST_init>)
    1516:	04a8           	l32i.n	a10, a4, 0
    1518:	0008e0        	callx8	a8
    151b:	02a8           	l32i.n	a10, a2, 0

0000151d <Voice_EC120_Init+0x1c5>:
    151d:	04b8           	l32i.n	a11, a4, 0
    151f:	fb0b81        	l32r	a8, 14c (1334 <tx_ref_vad_init>)
    1522:	36daa2        	addmi	a10, a10, 0x3600
    1525:	e8caa2        	addi	a10, a10, -24
    1528:	0008e0        	callx8	a8
    152b:	e0a1c2        	movi	a12, 0x1e0
    152e:	fae481        	l32r	a8, c0 (5608 <xt_memset>)
    1531:	02d8           	l32i.n	a13, a2, 0
    1533:	090c           	movi.n	a9, 0
    1535:	09bd           	mov.n	a11, a9
    1537:	10dda2        	addmi	a10, a13, 0x1000
    153a:	54caa2        	addi	a10, a10, 84
    153d:	35ddd2        	addmi	a13, a13, 0x3500
    1540:	735d92        	s16i	a9, a13, 230
    1543:	0008e0        	callx8	a8
    1546:	f01d           	retw.n

00001548 <Voice_EC120_Inear_Init>:
    1548:	004136        	entry	a1, 32
    154b:	05dd           	mov.n	a13, a5
    154d:	04cd           	mov.n	a12, a4
    154f:	fb0081        	l32r	a8, 150 (1358 <Voice_EC120_Init>)
    1552:	02ad           	mov.n	a10, a2
    1554:	03bd           	mov.n	a11, a3
    1556:	0008e0        	callx8	a8
    1559:	0b0c           	movi.n	a11, 0
    155b:	8ca2c2        	movi	a12, 0x28c
    155e:	36d3a2        	addmi	a10, a3, 0x3600
    1561:	fad781        	l32r	a8, c0 (5608 <xt_memset>)
    1564:	0a9d           	mov.n	a9, a10
    1566:	30caa2        	addi	a10, a10, 48
    1569:	a9a9           	s32i.n	a10, a9, 40
    156b:	0008e0        	callx8	a8
    156e:	54a142        	movi	a4, 0x154
    1571:	faad51        	l32r	a5, 28 (6d0c <aec_nr_para>)
    1574:	faec81        	l32r	a8, 124 (12d8 <sb_ana_init>)
    1577:	3ad3b2        	addmi	a11, a3, 0x3a00
    157a:	05c8           	l32i.n	a12, a5, 0
    157c:	44cb22        	addi	a2, a11, 68
    157f:	68cba2        	addi	a10, a11, 104
    1582:	01dcc2        	addmi	a12, a12, 0x100
    1585:	879c92        	l16si	a9, a12, 0x10e
    1588:	a15b92        	s16i	a9, a11, 0x142
    158b:	cb9cc2        	l16si	a12, a12, 0x196
    158e:	a05bc2        	s16i	a12, a11, 0x140
    1591:	0008e0        	callx8	a8
    1594:	fae581        	l32r	a8, 128 (12ec <sb_syn_init>)
    1597:	3bd3a2        	addmi	a10, a3, 0x3b00
    159a:	a8caa2        	addi	a10, a10, -88
    159d:	0008e0        	callx8	a8
    15a0:	fadf81        	l32r	a8, 11c (1300 <HP_coef_setup>)
    15a3:	faec91        	l32r	a9, 154 (800000 <_end+0x7f92e4>)
    15a6:	00a1d2        	movi	a13, 0x100
    15a9:	05b8           	l32i.n	a11, a5, 0
    15ab:	16d3c2        	addmi	a12, a3, 0x1600
    15ae:	37d3a2        	addmi	a10, a3, 0x3700
    15b1:	88caa2        	addi	a10, a10, -120
    15b4:	bcc8           	l32i.n	a12, a12, 44
    15b6:	fb4a           	add.n	a15, a11, a4
    15b8:	9d9fe2        	l16si	a14, a15, 0x13a
    15bb:	78cc62        	addi	a6, a12, 120
    15be:	50cc72        	addi	a7, a12, 80
    15c1:	03dbb2        	addmi	a11, a11, 0x300
    15c4:	b8cbb2        	addi	a11, a11, -72
    15c7:	3d6272        	s32i	a7, a2, 244
    15ca:	3e6262        	s32i	a6, a2, 248
    15cd:	9e9ff2        	l16si	a15, a15, 0x13c
    15d0:	7752f2        	s16i	a15, a2, 238
    15d3:	28ccc2        	addi	a12, a12, 40
    15d6:	3c62c2        	s32i	a12, a2, 240
    15d9:	8eed           	ae_sext16	a14, a14
    15db:	7712c2        	l16ui	a12, a2, 238
    15de:	53eed0        	max	a14, a14, a13
    15e1:	7652e2        	s16i	a14, a2, 236
    15e4:	53ccd0        	max	a12, a12, a13
    15e7:	9ccd           	ae_zext16	a12, a12
    15e9:	d299c0        	quos	a9, a9, a12
    15ec:	775292        	s16i	a9, a2, 238
    15ef:	0008e0        	callx8	a8
    15f2:	fad981        	l32r	a8, 158 (9ac <SBAEC_INIT_inear>)
    15f5:	03ad           	mov.n	a10, a3
    15f7:	0008e0        	callx8	a8
    15fa:	f01d           	retw.n

000015fc <mic120_delay>:
    15fc:	004136        	entry	a1, 32
    15ff:	02ad           	mov.n	a10, a2
    1601:	fa8781        	l32r	a8, 20 (5620 <xt_memcpy>)
    1604:	04bd           	mov.n	a11, a4
    1606:	05cd           	mov.n	a12, a5
    1608:	0008e0        	callx8	a8
    160b:	03bd           	mov.n	a11, a3
    160d:	90a520        	addx2	a10, a5, a2
    1610:	78a0c2        	movi	a12, 120
    1613:	fa8381        	l32r	a8, 20 (5620 <xt_memcpy>)
    1616:	c0cc50        	sub	a12, a12, a5
    1619:	1125f0        	slli	a2, a5, 1
    161c:	0008e0        	callx8	a8
    161f:	05cd           	mov.n	a12, a5
    1621:	04ad           	mov.n	a10, a4
    1623:	c0b320        	sub	a11, a3, a2
    1626:	fa7e81        	l32r	a8, 20 (5620 <xt_memcpy>)
    1629:	01dbb2        	addmi	a11, a11, 0x100
    162c:	f0cbb2        	addi	a11, a11, -16
    162f:	0008e0        	callx8	a8
    1632:	f01d           	retw.n

00001634 <sbiir>:
    1634:	004136        	entry	a1, 32
    1637:	4c1404        	ae_lp24.i	aep1, a4, 0
    163a:	5c7304        	ae_lp24x2f.i	aep7, a3, 0
    163d:	fdec2c020f 	{ ae_lp16f.i	aep0, a2, 0; ae_cvtq48p24s.l	aeq1, aep7 }
    1642:	fee43e079f 	{ ae_selp24.hl	aep2, aep0, aep7; ae_cvtq48p24s.h	aeq2, aep7 }
    1647:	059244        	ae_mulasfp24s.hh.ll	aeq2, aep1, aep2
    164a:	5c3314        	ae_lp24x2f.i	aep3, a3, 8
    164d:	fa77b4030f 	{ ae_sp24f.l.i	aep0, a3, 0; ae_roundsp24q48asym	aep4, aeq2 }
    1652:	df71b1541f 	{ ae_lp24.i	aep5, a4, 4; ae_selp24.lh	aep6, aep4, aep3 }
    1657:	055e44        	ae_mulasfp24s.hh.ll	aeq1, aep5, aep6
    165a:	0a0b84        	ae_cvtq48p24s.h	aeq0, aep3
    165d:	f86ff4431f 	{ ae_sp24f.l.i	aep4, a3, 4; ae_roundsp24q48asym	aep1, aeq1 }
    1662:	dd6671242f 	{ ae_lp24.i	aep2, a4, 8; ae_selp24.ll	aep3, aep1, aep3 }
    1667:	052344        	ae_mulasfp24s.hh.ll	aeq0, aep2, aep3
    166a:	f867b4132f 	{ ae_sp24f.l.i	aep1, a3, 8; ae_roundsp24q48asym	aep0, aeq0 }
    166f:	520334        	ae_sp24f.l.i	aep0, a3, 12
    1672:	348204        	ae_movap24s.l	a2, aep0
    1675:	f01d           	retw.n

00001677 <sbiir+0x43>:
	...

00001678 <tx_downsample_by2>:
    1678:	008136        	entry	a1, 64
    167b:	fec332        	addi	a3, a3, -2
    167e:	440404        	ae_lp16x2f.i	aep0, a4, 0
    1681:	0169           	s32i.n	a6, a1, 0
    1683:	fab681        	l32r	a8, 15c (6bbc <wb_r01+0xc>)
    1686:	fec272        	addi	a7, a2, -2
    1689:	10c5a2        	addi	a10, a5, 16
    168c:	600604        	ae_lq32f.i	aeq0, a6, 0
    168f:	630124        	ae_sq56s.i	aeq0, a1, 16
    1692:	21a9           	s32i.n	a10, a1, 8
    1694:	020c           	movi.n	a2, 0
    1696:	1189           	s32i.n	a8, a1, 4
    1698:	056d           	mov.n	a6, a5

0000169a <tx_downsample_by2+0x22>:
    169a:	06bd           	mov.n	a11, a6
    169c:	614124        	ae_lq56.i	aeq1, a1, 16
    169f:	e20053a44f 	{ or	a10, a4, a4; ae_mulaafp24s.hh.ll	aeq1, aep0, aep0 }
    16a4:	11c8           	l32i.n	a12, a1, 4
    16a6:	631124        	ae_sq56s.i	aeq1, a1, 16
    16a9:	fff8a5        	call8	1634 <sbiir>
    16ac:	21b8           	l32i.n	a11, a1, 8
    16ae:	0a5d           	mov.n	a5, a10
    16b0:	faacc1        	l32r	a12, 160 (6bb0 <wb_r01>)
    16b3:	a42b           	addi.n	a10, a4, 2
    16b5:	fff7e5        	call8	1634 <sbiir>
    16b8:	4e1c           	movi.n	a14, 20
    16ba:	221b           	addi.n	a2, a2, 1
    16bc:	f5aa           	add.n	a15, a5, a10
    16be:	c08a50        	sub	a8, a10, a5
    16c1:	218180        	srai	a8, a8, 1
    16c4:	21f1f0        	srai	a15, a15, 1
    16c7:	39bf84        	ae_movpa24x2	aep3, a15, a8
    16ca:	f9eead841f 	{ ae_lp16x2f.iu	aep0, a4, 4; ae_roundsp16asym	aep2, aep3 }
    16cf:	dc4972a31f 	{ ae_sp16f.l.iu	aep2, a3, 2; ae_selp24.hh	aep1, aep2, aep2 }
    16d4:	429714        	ae_sp16f.l.iu	aep1, a7, 2
    16d7:	bf92e7        	bne	a2, a14, 169a <tx_downsample_by2+0x22>
    16da:	0198           	l32i.n	a9, a1, 0
    16dc:	61c124        	ae_lq56.i	aeq3, a1, 16
    16df:	06b934        	ae_roundsq32asym	aeq2, aeq3
    16e2:	622904        	ae_sq32f.i	aeq2, a9, 0
    16e5:	f01d           	retw.n

000016e7 <tx_downsample_by2+0x6f>:
	...

000016e8 <tx_upsample_by2>:
    16e8:	00a136        	entry	a1, 80
    16eb:	401204        	ae_lp16f.i	aep1, a2, 0
    16ee:	400304        	ae_lp16f.i	aep0, a3, 0
    16f1:	1169           	s32i.n	a6, a1, 4
    16f3:	fa9a81        	l32r	a8, 15c (6bbc <wb_r01+0xc>)
    16f6:	fcc472        	addi	a7, a4, -4
    16f9:	10c5a2        	addi	a10, a5, 16
    16fc:	c12b           	addi.n	a12, a1, 2
    16fe:	600604        	ae_lq32f.i	aeq0, a6, 0
    1701:	630134        	ae_sq56s.i	aeq0, a1, 24
    1704:	21c9           	s32i.n	a12, a1, 8
    1706:	31a9           	s32i.n	a10, a1, 12
    1708:	040c           	movi.n	a4, 0
    170a:	4189           	s32i.n	a8, a1, 16
    170c:	056d           	mov.n	a6, a5

0000170e <tx_upsample_by2+0x26>:
    170e:	06bd           	mov.n	a11, a6
    1710:	fa94c1        	l32r	a12, 160 (6bb0 <wb_r01>)
    1713:	d9053da13f 	{ ae_lq56.i	aeq1, a1, 24; ae_addsp24s	aep2, aep1, aep0 }
    1718:	e24bf021ae 	{ l32i	a10, a1, 8; ae_mulafp24s.ll	aeq1, aep2, aep2 }
    171d:	dc06b2210f 	{ ae_sp16f.l.i	aep2, a1, 0; ae_subsp24s	aep0, aep1, aep0 }
    1722:	420114        	ae_sp16f.l.i	aep0, a1, 2
    1725:	631134        	ae_sq56s.i	aeq1, a1, 24
    1728:	fff0a5        	call8	1634 <sbiir>
    172b:	31b8           	l32i.n	a11, a1, 12
    172d:	0a5d           	mov.n	a5, a10
    172f:	41c8           	l32i.n	a12, a1, 16
    1731:	01ad           	mov.n	a10, a1
    1733:	fff025        	call8	1634 <sbiir>
    1736:	442b           	addi.n	a4, a4, 2
    1738:	782c           	movi.n	a8, 39
    173a:	408314        	ae_lp16f.iu	aep0, a3, 2
    173d:	39c5a4        	ae_movpa24x2	aep4, a5, a10
    1740:	f9f2ec921f 	{ ae_lp16f.iu	aep1, a2, 2; ae_roundsp16asym	aep3, aep4 }
    1745:	46b714        	ae_sp16x2f.iu	aep3, a7, 4
    1748:	c2a847        	bge	a8, a4, 170e <tx_upsample_by2+0x26>
    174b:	1198           	l32i.n	a9, a1, 4
    174d:	61c134        	ae_lq56.i	aeq3, a1, 24
    1750:	06b934        	ae_roundsq32asym	aeq2, aeq3
    1753:	622904        	ae_sq32f.i	aeq2, a9, 0
    1756:	f01d           	retw.n

00001758 <emph_filter_N>:
    1758:	004136        	entry	a1, 32
    175b:	442304        	ae_lp16x2f.i	aep2, a3, 0
    175e:	02a062        	movi	a6, 2
    1761:	440404        	ae_lp16x2f.i	aep0, a4, 0
    1764:	d26560        	quos	a6, a5, a6
    1767:	fcc252        	addi	a5, a2, -4
    176a:	fec222        	addi	a2, a2, -2
    176d:	23a676        	loopgtz	a6, 1794 <emph_filter_N+0x3c>
    1770:	44b514        	ae_lp16x2f.iu	aep3, a5, 4
    1773:	0a4b84        	ae_cvtq48p24s.h	aeq1, aep3
    1776:	0a0384        	ae_cvtq48p24s.l	aeq0, aep3
    1779:	012304        	ae_mulafp24s.hh	aeq0, aep2, aep3
    177c:	012804        	ae_mulafp24s.hh	aeq1, aep2, aep0
    177f:	e2087e384f 	{ ae_selp24.hh	aep5, aep3, aep0; ae_mulaafp24s.hh.ll	aeq1, aep2, aep0 }
    1784:	052504        	ae_mulaafp24s.hh.ll	aeq0, aep2, aep5
    1787:	1b9f44        	ae_roundsp16q48asym	aep4, aeq1
    178a:	f863f2c21f 	{ ae_sp16f.l.iu	aep4, a2, 2; ae_roundsp16q48asym	aep1, aeq0 }
    178f:	dc6db2921f 	{ ae_sp16f.l.iu	aep1, a2, 2; ae_selp24.lh	aep0, aep3, aep3 }

00001794 <emph_filter_N+0x3c>:
    1794:	460404        	ae_sp16x2f.i	aep0, a4, 0
    1797:	f01d           	retw.n

00001799 <emph_filter_N+0x41>:
    1799:	000000                                        ...

0000179c <deemph_filter_N>:
    179c:	004136        	entry	a1, 32
    179f:	600414        	ae_lq32f.i	aeq0, a4, 4
    17a2:	400204        	ae_lp16f.i	aep0, a2, 0
    17a5:	fa6f61        	l32r	a6, 164 (69d0 <immediate_num+0x30>)
    17a8:	442304        	ae_lp16x2f.i	aep2, a3, 0
    17ab:	604404        	ae_lq32f.i	aeq1, a4, 0
    17ae:	9949ad360f 	{ ae_lp16x2f.i	aep3, a6, 0; ae_mulzssfq32sp16s.hh	aeq2, aeq1, aep2, aeq1, aep2 }
    17b3:	f03d           	nop.n
    17b5:	14a576        	loopgtz	a5, 17cd <deemph_filter_N+0x31>
    17b8:	03a414        	ae_mulsfq32sp16s.l	aeq2, aeq0, aep2
    17bb:	0583c4        	ae_mulssfp24s.hh.ll	aeq2, aep0, aep3
    17be:	fd347f113f 	{ ae_movq56	aeq0, aeq1; ae_roundsq32asym	aeq1, aeq2 }
    17c3:	f86bec821f 	{ ae_lp16f.iu	aep0, a2, 2; ae_roundsp16q48asym	aep1, aeq1 }
    17c8:	9949b212ff 	{ ae_sp16f.l.i	aep1, a2, -2; ae_mulzssfq32sp16s.hh	aeq2, aeq1, aep2, aeq1, aep2 }

000017cd <deemph_filter_N+0x31>:
    17cd:	621404        	ae_sq32f.i	aeq1, a4, 0
    17d0:	620414        	ae_sq32f.i	aeq0, a4, 4
    17d3:	f01d           	retw.n

000017d5 <deemph_filter_N+0x39>:
    17d5:	000000                                        ...

000017d8 <analy_3band>:
    17d8:	006136        	entry	a1, 48
    17db:	7f0c           	movi.n	a15, 7
    17dd:	fa62e1        	l32r	a14, 168 (6bcc <wb_r01+0x1c>)
    17e0:	fcc4b2        	addi	a11, a4, -4
    17e3:	fa62d1        	l32r	a13, 16c (6c0c <sb_g10+0x3c>)
    17e6:	24c2c2        	addi	a12, a2, 36
    17e9:	834b           	addi.n	a8, a3, 4
    17eb:	fec692        	addi	a9, a6, -2
    17ee:	fec5a2        	addi	a10, a5, -2
    17f1:	40c372        	addi	a7, a3, 64
    17f4:	450c           	movi.n	a5, 4
    17f6:	f3f770        	wur.ae_cend0	a7
    17f9:	fcc262        	addi	a6, a2, -4
    17fc:	f3f680        	wur.ae_cbegin0	a8
    17ff:	0328           	l32i.n	a2, a3, 0
    1801:	ff24934ddf 	{ or	a4, a13, a13; ae_zeroq56	aeq3 }
    1806:	fd1c538eef 	{ or	a8, a14, a14; ae_movq56	aeq1, aeq3 }

0000180b <analy_3band+0x33>:
    180b:	443614        	ae_lp16x2f.i	aep3, a6, 4
    180e:	fc24e4325f 	{ ae_sp16x2f.c	aep3, a2, a5; nop }
    1813:	fc24da925f 	{ ae_lp16x2f.c	aep1, a2, a5; nop }
    1818:	44a814        	ae_lp16x2f.iu	aep2, a8, 4
    181b:	eb29ad841f 	{ ae_lp16x2f.iu	aep0, a4, 4; ae_mulfp24s.ll	aeq2, aep2, aep1 }
    1820:	ea2180466e 	{ addi	a6, a6, 4; ae_mulfp24s.ll	aeq0, aep0, aep1 }
    1825:	27af76        	loopgtz	a15, 1850 <analy_3band+0x78>
    1828:	fc24da825f 	{ ae_lp16x2f.c	aep0, a2, a5; nop }
    182d:	449414        	ae_lp16x2f.iu	aep1, a4, 4
    1830:	011034        	ae_mulafp24s.ll	aeq0, aep1, aep0
    1833:	e2066df81f 	{ ae_lp16x2f.iu	aep7, a8, 4; ae_mulafp24s.hh	aeq1, aep1, aep0 }
    1838:	e31f9ad25f 	{ ae_lp16x2f.c	aep5, a2, a5; ae_mulafp24s.ll	aeq2, aep7, aep0 }
    183d:	e31e6de81f 	{ ae_lp16x2f.iu	aep6, a8, 4; ae_mulafp24s.hh	aeq3, aep7, aep0 }
    1842:	01ed04        	ae_mulafp24s.hh	aeq3, aep6, aep5
    1845:	e3bbadc41f 	{ ae_lp16x2f.iu	aep4, a4, 4; ae_mulafp24s.ll	aeq2, aep6, aep5 }
    184a:	014d04        	ae_mulafp24s.hh	aeq1, aep4, aep5
    184d:	014534        	ae_mulafp24s.ll	aeq0, aep4, aep5

00001850 <analy_3band+0x78>:
    1850:	7f0c           	movi.n	a15, 7
    1852:	f963d34ddf 	{ or	a4, a13, a13; ae_roundsp16q48asym	aep3, aeq0 }
    1857:	f8f6138eef 	{ or	a8, a14, a14; ae_addq56	aeq0, aeq2, aeq3 }
    185c:	f96bbb001f 	{ ae_sraiq56	aeq0, aeq0, 1; ae_roundsp16q48asym	aep2, aeq1 }
    1861:	f867f2b91f 	{ ae_sp16f.l.iu	aep3, a9, 2; ae_roundsp24q48asym	aep1, aeq0 }
    1866:	f8b6f2a91f 	{ ae_sp16f.l.iu	aep2, a9, 2; ae_subq56	aeq0, aeq2, aeq3 }
    186b:	ff24bb001f 	{ ae_sraiq56	aeq0, aeq0, 1; ae_zeroq56	aeq3 }
    1870:	f863b49b1f 	{ ae_sp24f.l.iu	aep1, a11, 4; ae_roundsp16q48asym	aep0, aeq0 }
    1875:	fd1c728a1f 	{ ae_sp16f.l.iu	aep0, a10, 2; ae_movq56	aeq1, aeq3 }
    187a:	8d96c7        	bne	a6, a12, 180b <analy_3band+0x33>
    187d:	0329           	s32i.n	a2, a3, 0
    187f:	f01d           	retw.n

00001881 <analy_3band+0xa9>:
    1881:	000000                                        ...

00001884 <synth_3band>:
    1884:	004136        	entry	a1, 32
    1887:	080c           	movi.n	a8, 0
    1889:	7cc572        	addi	a7, a5, 124
    188c:	b54b           	addi.n	a11, a5, 4
    188e:	fec692        	addi	a9, a6, -2
    1891:	fec2c2        	addi	a12, a2, -2
    1894:	fcc4a2        	addi	a10, a4, -4
    1897:	fec3d2        	addi	a13, a3, -2
    189a:	0548           	l32i.n	a4, a5, 0
    189c:	fa3531        	l32r	a3, 170 (6c50 <sb_h2ir_v2>)
    189f:	fa3521        	l32r	a2, 174 (6bd0 <sb_g10>)
    18a2:	fe2480046f 	{ movi	a6, 4; ae_zeroq56	aeq2 }
    18a7:	f3f6b0        	wur.ae_cbegin0	a11
    18aa:	f3f770        	wur.ae_cend0	a7
    18ad:	fd147f213f 	{ ae_movq56	aeq0, aeq2; ae_movq56	aeq1, aeq2 }

000018b2 <synth_3band+0x2e>:
    18b2:	40cd14        	ae_lp16f.iu	aep4, a13, 2
    18b5:	40bc14        	ae_lp16f.iu	aep3, a12, 2
    18b8:	dd8e80188e 	{ addi	a8, a8, 1; ae_subsp24s	aep2, aep3, aep4 }
    18bd:	d88d400fef 	{ movi	a14, 15; ae_addsp24s	aep1, aep3, aep4 }
    18c2:	dc466d8a1f 	{ ae_lp16x2f.iu	aep0, a10, 4; ae_selp24.ll	aep1, aep1, aep2 }
    18c7:	fc24e4146f 	{ ae_sp16x2f.c	aep1, a4, a6; nop }
    18cc:	fc24e4046f 	{ ae_sp16x2f.c	aep0, a4, a6; nop }
    18d1:	441204        	ae_lp16x2f.i	aep1, a2, 0
    18d4:	440304        	ae_lp16x2f.i	aep0, a3, 0
    18d7:	16ae76        	loopgtz	a14, 18f1 <synth_3band+0x6d>
    18da:	fc24dae46f 	{ ae_lp16x2f.c	aep6, a4, a6; nop }
    18df:	011e04        	ae_mulafp24s.hh	aeq1, aep1, aep6
    18e2:	e2c79ad46f 	{ ae_lp16x2f.c	aep5, a4, a6; ae_mulafp24s.ll	aeq0, aep1, aep6 }
    18e7:	eea0ad921f 	{ ae_lp16x2f.iu	aep1, a2, 4; ae_mulsfp24s.hl	aeq0, aep0, aep5 }
    18ec:	e2a2ed831f 	{ ae_lp16x2f.iu	aep0, a3, 4; ae_mulafp24s.lh	aeq1, aep0, aep5 }

000018f1 <synth_3band+0x6d>:
    18f1:	fb6bcc422e 	{ addi	a2, a2, -60; ae_roundsp16q48asym	aep7, aeq1 }
    18f6:	f8638c433e 	{ addi	a3, a3, -60; ae_roundsp16q48asym	aep0, aeq0 }
    18fb:	fc1472891f 	{ ae_sp16f.l.iu	aep0, a9, 2; ae_movq56	aeq0, aeq2 }
    1900:	fd1472f91f 	{ ae_sp16f.l.iu	aep7, a9, 2; ae_movq56	aeq1, aeq2 }
    1905:	a99866        	bnei	a8, 10, 18b2 <synth_3band+0x2e>
    1908:	0549           	s32i.n	a4, a5, 0
    190a:	f01d           	retw.n

0000190c <biquad_2cascade_24bitTx>:
    190c:	004136        	entry	a1, 32
    190f:	542304        	ae_lp24x2.i	aep2, a3, 0
    1912:	541314        	ae_lp24x2.i	aep1, a3, 8
    1915:	18c482        	addi	a8, a4, 24
    1918:	540324        	ae_lp24x2.i	aep0, a3, 16
    191b:	fcc242        	addi	a4, a2, -4
    191e:	fec522        	addi	a2, a5, -2
    1921:	40a676        	loopgtz	a6, 1965 <biquad_2cascade_24bitTx+0x59>
    1924:	5468d4        	ae_lp24x2.i	aep6, a8, -24
    1927:	50f414        	ae_lp24f.iu	aep7, a4, 4
    192a:	5438e4        	ae_lp24x2.i	aep3, a8, -16
    192d:	f14ebe72af 	{ ae_selp24.lh	aep2, aep7, aep2; ae_mulzaafp24s.hh.ll	aeq2, aep3, aep2 }
    1932:	e3583058ff 	{ ae_lp24x2.i	aep5, a8, -8; ae_mulaafp24s.hh.ll	aeq2, aep6, aep2 }
    1937:	01d184        	ae_mulsfp24s.hh	aeq2, aep5, aep1
    193a:	f13630481f 	{ ae_lp24x2.i	aep4, a8, 8; ae_mulssfp24s.hh.ll	aeq2, aep5, aep1 }
    193f:	044904        	ae_mulzaafp24s.hh.ll	aeq1, aep4, aep1
    1942:	1bad34        	ae_roundsp24q48asym	aep3, aeq2
    1945:	dc2df0780f 	{ ae_lp24x2.i	aep7, a8, 0; ae_selp24.lh	aep1, aep3, aep1 }
    194a:	e23c70682f 	{ ae_lp24x2.i	aep6, a8, 16; ae_mulaafp24s.hh.ll	aeq1, aep7, aep1 }
    194f:	016884        	ae_mulsfp24s.hh	aeq1, aep6, aep0
    1952:	0568c4        	ae_mulssfp24s.hh.ll	aeq1, aep6, aep0
    1955:	fa6fb1586f 	{ ae_lp24.i	aep5, a8, 24; ae_roundsp24q48asym	aep4, aeq1 }
    195a:	045404        	ae_mulzaafp24s.hh.ll	aeq0, aep5, aep4
    195d:	1b8f34        	ae_roundsp16q48asym	aep3, aeq0
    1960:	dc11b2b21f 	{ ae_sp16f.l.iu	aep3, a2, 2; ae_selp24.lh	aep0, aep4, aep0 }

00001965 <biquad_2cascade_24bitTx+0x59>:
    1965:	562304        	ae_sp24x2s.i	aep2, a3, 0
    1968:	561314        	ae_sp24x2s.i	aep1, a3, 8
    196b:	560324        	ae_sp24x2s.i	aep0, a3, 16
    196e:	f01d           	retw.n

00001970 <conv_24_to_16>:
    1970:	004136        	entry	a1, 32
    1973:	f8c252        	addi	a5, a2, -8
    1976:	540c           	movi.n	a4, 5
    1978:	fcc322        	addi	a2, a3, -4
    197b:	f03d           	nop.n
    197d:	08a476        	loopgtz	a4, 1989 <conv_24_to_16+0x19>
    1980:	5c9514        	ae_lp24x2f.iu	aep1, a5, 8
    1983:	091184        	ae_roundsp16asym	aep0, aep1
    1986:	468214        	ae_sp16x2f.iu	aep0, a2, 4

00001989 <conv_24_to_16+0x19>:
    1989:	f01d           	retw.n

0000198b <conv_24_to_16+0x1b>:
	...

0000198c <gain_after_ec>:
    198c:	00c136        	entry	a1, 96
    198f:	4159           	s32i.n	a5, a1, 16
    1991:	f9a481        	l32r	a8, 24 (6d14 <p_ecnr_mem>)
    1994:	07bd           	mov.n	a11, a7
    1996:	3149           	s32i.n	a4, a1, 12
    1998:	04fd           	mov.n	a15, a4
    199a:	05ed           	mov.n	a14, a5
    199c:	069d           	mov.n	a9, a6
    199e:	fec2d2        	addi	a13, a2, -2
    19a1:	fec3c2        	addi	a12, a3, -2
    19a4:	61c9           	s32i.n	a12, a1, 24
    19a6:	51d9           	s32i.n	a13, a1, 20
    19a8:	2199           	s32i.n	a9, a1, 8
    19aa:	440e04        	ae_lp16x2f.i	aep0, a14, 0
    19ad:	441f04        	ae_lp16x2f.i	aep1, a15, 0
    19b0:	561154        	ae_sp24x2s.i	aep1, a1, 40
    19b3:	560164        	ae_sp24x2s.i	aep0, a1, 48
    19b6:	0d3d           	mov.n	a3, a13
    19b8:	0c2d           	mov.n	a2, a12
    19ba:	054d           	mov.n	a4, a5
    19bc:	fecf62        	addi	a6, a15, -2
    19bf:	fece72        	addi	a7, a14, -2
    19c2:	1179           	s32i.n	a7, a1, 4
    19c4:	0888           	l32i.n	a8, a8, 0
    19c6:	0169           	s32i.n	a6, a1, 0
    19c8:	0f5d           	mov.n	a5, a15
    19ca:	16d882        	addmi	a8, a8, 0x1600
    19cd:	40a8a4        	ae_lp16f.iu	aep2, a8, -12
    19d0:	562144        	ae_sp24x2s.i	aep2, a1, 32
    19d3:	6bac           	beqz.n	a11, 19fd <gain_after_ec+0x71>
    19d5:	f9c881        	l32r	a8, f8 (4c80 <airoha_divide>)
    19d8:	f9d8a1        	l32r	a10, 138 (8000 <_end+0x12e4>)
    19db:	0008e0        	callx8	a8
    19de:	540164        	ae_lp24x2.i	aep0, a1, 48
    19e1:	541154        	ae_lp24x2.i	aep1, a1, 40
    19e4:	61c8           	l32i.n	a12, a1, 24
    19e6:	41e8           	l32i.n	a14, a1, 16
    19e8:	542144        	ae_lp24x2.i	aep2, a1, 32
    19eb:	360b34        	ae_cvtq48a32s	aeq0, a11
    19ee:	d4ea3031fe 	{ l32i	a15, a1, 12; ae_mulfq32sp16s.l	aeq0, aeq0, aep2 }
    19f3:	51d8           	l32i.n	a13, a1, 20
    19f5:	340a94        	ae_sllaq56	aeq0, aeq0, a10
    19f8:	f967b0219e 	{ l32i	a9, a1, 8; ae_roundsp24q48asym	aep2, aeq0 }

000019fd <gain_after_ec+0x71>:
    19fd:	0dbd           	mov.n	a11, a13
    19ff:	20acc0        	or	a10, a12, a12
    1a02:	0aa082        	movi	a8, 10
    1a05:	47a876        	loopgtz	a8, 1a50 <gain_after_ec+0xc4>
    1a08:	ea09acfb1f 	{ ae_lp16f.iu	aep7, a11, 2; ae_mulfp24s.ll	aeq0, aep2, aep0 }
    1a0d:	e828ecda1f 	{ ae_lp16f.iu	aep5, a10, 2; ae_mulfp24s.hh	aeq1, aep2, aep1 }
    1a12:	eb29ad951f 	{ ae_lp16x2f.iu	aep1, a5, 4; ae_mulfp24s.ll	aeq2, aep2, aep1 }
    1a17:	350114        	ae_slliq56	aeq0, aeq0, 4
    1a1a:	e908faa18f 	{ ae_slliq56	aeq2, aeq2, 4; ae_mulfp24s.hh	aeq3, aep2, aep0 }
    1a1f:	faf3ba914f 	{ ae_slliq56	aeq1, aeq1, 4; ae_roundsp16q48sym	aep4, aeq2 }
    1a24:	fbebbab1cf 	{ ae_slliq56	aeq3, aeq3, 4; ae_roundsp16q48sym	aep6, aeq1 }
    1a29:	eaa9f2e61f 	{ ae_sp16f.l.iu	aep6, a6, 2; ae_mulfp24s.ll	aeq1, aep2, aep5 }
    1a2e:	ebe9b2c61f 	{ ae_sp16f.l.iu	aep4, a6, 2; ae_mulfp24s.ll	aeq2, aep2, aep7 }
    1a33:	355114        	ae_slliq56	aeq1, aeq1, 4
    1a36:	35a114        	ae_slliq56	aeq2, aeq2, 4
    1a39:	f9fbed841f 	{ ae_lp16x2f.iu	aep0, a4, 4; ae_roundsp16q48sym	aep3, aeq3 }
    1a3e:	fbe3b2b71f 	{ ae_sp16f.l.iu	aep3, a7, 2; ae_roundsp16q48sym	aep6, aeq0 }
    1a43:	faf3b2e71f 	{ ae_sp16f.l.iu	aep6, a7, 2; ae_roundsp16q48sym	aep4, aeq2 }
    1a48:	f9ebf2c31f 	{ ae_sp16f.l.iu	aep4, a3, 2; ae_roundsp16q48sym	aep3, aeq1 }
    1a4d:	42b214        	ae_sp16f.l.iu	aep3, a2, 2

00001a50 <gain_after_ec+0xc4>:
    1a50:	a90b           	addi.n	a10, a9, -1
    1a52:	082a56        	bnez	a10, 1ad8 <gain_after_ec+0x14c>
    1a55:	1178           	l32i.n	a7, a1, 4
    1a57:	0168           	l32i.n	a6, a1, 0
    1a59:	f97291        	l32r	a9, 24 (6d14 <p_ecnr_mem>)
    1a5c:	a80c           	movi.n	a8, 10
    1a5e:	0e4d           	mov.n	a4, a14
    1a60:	441e04        	ae_lp16x2f.i	aep1, a14, 0
    1a63:	0f5d           	mov.n	a5, a15
    1a65:	440f04        	ae_lp16x2f.i	aep0, a15, 0
    1a68:	0c2d           	mov.n	a2, a12
    1a6a:	0d3d           	mov.n	a3, a13
    1a6c:	f9c6c1        	l32r	a12, 184 (15fc <mic120_delay>)
    1a6f:	02ad           	mov.n	a10, a2
    1a71:	f9c3d1        	l32r	a13, 180 (15fa <Voice_EC120_Inear_Init+0xb2>)
    1a74:	20b330        	or	a11, a3, a3
    1a77:	f9c0f1        	l32r	a15, 178 (15f6 <Voice_EC120_Inear_Init+0xae>)
    1a7a:	f9c0e1        	l32r	a14, 17c (15f8 <Voice_EC120_Inear_Init+0xb0>)
    1a7d:	57a876        	loopgtz	a8, 1ad8 <gain_after_ec+0x14c>
    1a80:	0988           	l32i.n	a8, a9, 0
    1a82:	40da14        	ae_lp16f.iu	aep5, a10, 2
    1a85:	4148e4        	ae_lp16f.x	aep4, a8, a14
    1a88:	ea31d9f8cf 	{ ae_lp16f.x	aep7, a8, a12; ae_mulfp24s.ll	aeq1, aep4, aep1 }
    1a8d:	eb1dacab1f 	{ ae_lp16f.iu	aep2, a11, 2; ae_mulfp24s.ll	aeq2, aep7, aep0 }

00001a92 <gain_after_ec+0x106>:
    1a92:	e91ced851f 	{ ae_lp16x2f.iu	aep0, a5, 4; ae_mulfp24s.hh	aeq3, aep7, aep0 }
    1a97:	355094        	ae_slliq56	aeq1, aeq1, 2
    1a9a:	35f094        	ae_slliq56	aeq3, aeq3, 2
    1a9d:	fbfbbaa0af 	{ ae_slliq56	aeq2, aeq2, 2; ae_roundsp16q48sym	aep6, aeq3 }
    1aa2:	f9f3f2e61f 	{ ae_sp16f.l.iu	aep6, a6, 2; ae_roundsp16q48sym	aep3, aeq2 }
    1aa7:	e830b2b61f 	{ ae_sp16f.l.iu	aep3, a6, 2; ae_mulfp24s.hh	aeq0, aep4, aep1 }
    1aac:	0988           	l32i.n	a8, a9, 0
    1aae:	350094        	ae_slliq56	aeq0, aeq0, 2
    1ab1:	449414        	ae_lp16x2f.iu	aep1, a4, 4
    1ab4:	fbe399c8df 	{ ae_lp16f.x	aep4, a8, a13; ae_roundsp16q48sym	aep6, aeq0 }
    1ab9:	f9ebd9f8ff 	{ ae_lp16f.x	aep7, a8, a15; ae_roundsp16q48sym	aep3, aeq1 }
    1abe:	eb5df2e71f 	{ ae_sp16f.l.iu	aep6, a7, 2; ae_mulfp24s.ll	aeq3, aep7, aep2 }
    1ac3:	ebb1b2b71f 	{ ae_sp16f.l.iu	aep3, a7, 2; ae_mulfp24s.ll	aeq2, aep4, aep5 }
    1ac8:	35f094        	ae_slliq56	aeq3, aeq3, 2
    1acb:	f9fbbaa0af 	{ ae_slliq56	aeq2, aeq2, 2; ae_roundsp16q48sym	aep2, aeq3 }
    1ad0:	fbf3f2a31f 	{ ae_sp16f.l.iu	aep2, a3, 2; ae_roundsp16q48sym	aep7, aeq2 }
    1ad5:	42f214        	ae_sp16f.l.iu	aep7, a2, 2

00001ad8 <gain_after_ec+0x14c>:
    1ad8:	f01d           	retw.n

00001ada <gain_after_ec+0x14e>:
	...

00001adc <adjust_phase>:
    1adc:	004136        	entry	a1, 32
    1adf:	301334        	ae_cvtp24a16x2.ll	aep1, a3, a3
    1ae2:	360234        	ae_cvtq48a32s	aeq0, a2
    1ae5:	a11104        	ae_mulzaafq32sp16s.ll	aeq0, aeq0, aep1, aeq0, aep1
    1ae8:	1b8f04        	ae_roundsp16q48asym	aep0, aeq0
    1aeb:	0002d4        	ae_cvta32p24.l	a2, aep0
    1aee:	f01d           	retw.n

00001af0 <adjust_bib_state>:
    1af0:	004136        	entry	a1, 32
    1af3:	fcc242        	addi	a4, a2, -4
    1af6:	600204        	ae_lq32f.i	aeq0, a2, 0
    1af9:	830c           	movi.n	a3, 8
    1afb:	f03d           	nop.n
    1afd:	0aa376        	loopgtz	a3, 1b0b <adjust_bib_state+0x1b>
    1b00:	350074        	ae_sraiq56	aeq0, aeq0, 1
    1b03:	f867bd121f 	{ ae_lq32f.iu	aeq0, a2, 4; ae_roundsp24q48asym	aep0, aeq0 }
    1b08:	528414        	ae_sp24f.l.iu	aep0, a4, 4

00001b0b <adjust_bib_state+0x1b>:
    1b0b:	f01d           	retw.n

00001b0d <adjust_bib_state+0x1d>:
    1b0d:	000000                                        ...

00001b10 <aec_post_prcs>:
    1b10:	004136        	entry	a1, 32
    1b13:	fec552        	addi	a5, a5, -2
    1b16:	28c282        	addi	a8, a2, 40
    1b19:	159292        	l16si	a9, a2, 42
    1b1c:	02d272        	addmi	a7, a2, 0x200
    1b1f:	f96da1        	l32r	a10, d4 (7fff <_end+0x12e3>)
    1b22:	f999b1        	l32r	a11, 188 (7fffff <_end+0x7f92e3>)
    1b25:	39bbb4        	ae_movpa24x2	aep3, a11, a11
    1b28:	307aa4        	ae_cvtp24a16x2.ll	aep7, a10, a10
    1b2b:	4ac772        	addi	a7, a7, 74
    1b2e:	302994        	ae_cvtp24a16x2.ll	aep2, a9, a9
    1b31:	400804        	ae_lp16f.i	aep0, a8, 0
    1b34:	155262        	s16i	a6, a2, 42
    1b37:	de42ec670f 	{ ae_lp16f.i	aep6, a7, 0; ae_subsp24s	aep5, aep0, aep2 }
    1b3c:	eaf9ac030f 	{ ae_lp16f.i	aep0, a3, 0; ae_mulfp24s.ll	aeq0, aep6, aep7 }
    1b41:	ce15cfe42e 	{ addi	a2, a4, -2; ae_sraip24	aep5, aep5, 3 }
    1b46:	fa6780004f 	{ movi	a4, 0; ae_roundsp24q48asym	aep4, aeq0 }

00001b4b <aec_post_prcs+0x3b>:
    1b4b:	07a524        	ae_addsp24s	aep2, aep2, aep5
    1b4e:	07b294        	ae_subsp24s	aep1, aep3, aep2
    1b51:	011c74        	ae_mulfp24s.ll	aeq1, aep1, aep4
    1b54:	441b           	addi.n	a4, a4, 1
    1b56:	fe2c400fcf 	{ movi	a12, 15; ae_roundsq32asym	aeq2, aeq1 }
    1b5b:	f03d           	nop.n
    1b5d:	17ac76        	loopgtz	a12, 1b78 <aec_post_prcs+0x68>
    1b60:	ea09ecf51f 	{ ae_lp16f.iu	aep7, a5, 2; ae_mulfp24s.ll	aeq1, aep2, aep0 }
    1b65:	037294        	ae_mulfq32sp16s.l	aeq0, aeq2, aep7
    1b68:	ff2c7a802f 	{ ae_slliq56	aeq0, aeq0, 2; ae_roundsq32asym	aeq3, aeq1 }
    1b6d:	01f8c4        	ae_addq56	aeq3, aeq3, aeq0
    1b70:	fb7bac831f 	{ ae_lp16f.iu	aep0, a3, 2; ae_roundsp16q48asym	aep6, aeq3 }
    1b75:	42e214        	ae_sp16f.l.iu	aep6, a2, 2

00001b78 <aec_post_prcs+0x68>:
    1b78:	cf8466        	bnei	a4, 8, 1b4b <aec_post_prcs+0x3b>
    1b7b:	f01d           	retw.n

00001b7d <aec_post_prcs+0x6d>:
    1b7d:	000000                                        ...

00001b80 <gain_ref>:
    1b80:	004136        	entry	a1, 32
    1b83:	303334        	ae_cvtp24a16x2.ll	aep3, a3, a3
    1b86:	fcc222        	addi	a2, a2, -4
    1b89:	448214        	ae_lp16x2f.iu	aep0, a2, 4
    1b8c:	f03d           	nop.n
    1b8e:	17a476        	loopgtz	a4, 1ba9 <gain_ref+0x29>
    1b91:	010b54        	ae_mulfp24s.hl	aeq1, aep0, aep3
    1b94:	ea61ad821f 	{ ae_lp16x2f.iu	aep0, a2, 4; ae_mulfp24s.ll	aeq0, aep0, aep3 }
    1b99:	355094        	ae_slliq56	aeq1, aeq1, 2
    1b9c:	f96bba802f 	{ ae_slliq56	aeq0, aeq0, 2; ae_roundsp16q48asym	aep2, aeq1 }
    1ba1:	f863f222ef 	{ ae_sp16f.l.i	aep2, a2, -4; ae_roundsp16q48asym	aep1, aeq0 }
    1ba6:	4212f4        	ae_sp16f.l.i	aep1, a2, -2

00001ba9 <gain_ref+0x29>:
    1ba9:	f01d           	retw.n

00001bab <gain_ref+0x2b>:
	...

00001bac <TxRef_VAD>:
    1bac:	006136        	entry	a1, 48
    1baf:	00a092        	movi	a9, 0
    1bb2:	78a082        	movi	a8, 120
    1bb5:	08a876        	loopgtz	a8, 1bc1 <TxRef_VAD+0x15>
    1bb8:	23ad           	ae_l16si.n	a10, a3, 0
    1bba:	332b           	addi.n	a3, a3, 2
    1bbc:	60a1a0        	abs	a10, a10
    1bbf:	99aa           	add.n	a9, a9, a10

00001bc1 <TxRef_VAD+0x15>:
    1bc1:	a99a           	add.n	a10, a9, a9
    1bc3:	eaac           	beqz.n	a10, 1bf5 <TxRef_VAD+0x49>
    1bc5:	f97181        	l32r	a8, 18c (60c <Log2_norm>)
    1bc8:	7b1c           	movi.n	a11, 23
    1bca:	40eac0        	nsa	a12, a10
    1bcd:	401c00        	ssl	a12
    1bd0:	a1aa00        	sll	a10, a10
    1bd3:	c0bbc0        	sub	a11, a11, a12
    1bd6:	0008e0        	callx8	a8
    1bd9:	f96d81        	l32r	a8, 190 (4d10 <airoha_divide+0x90>)
    1bdc:	f91c31        	l32r	a3, 4c (80000000 <_end+0x7fff92e4>)
    1bdf:	829a80        	mull	a9, a10, a8
    1be2:	b28a80        	mulsh	a8, a10, a8
    1be5:	393a           	add.n	a3, a9, a3
    1be7:	623390        	saltu	a3, a3, a9
    1bea:	338a           	add.n	a3, a3, a8
    1bec:	0ad332        	addmi	a3, a3, 0xa00
    1bef:	a2c332        	addi	a3, a3, -94
    1bf2:	000046        	j	1bf7 <TxRef_VAD+0x4b>

00001bf5 <TxRef_VAD+0x49>:
    1bf5:	030c           	movi.n	a3, 0

00001bf7 <TxRef_VAD+0x4b>:
    1bf7:	f96781        	l32r	a8, 194 (64c <siir_safr>)
    1bfa:	0392a2        	l16si	a10, a2, 6
    1bfd:	029292        	l16si	a9, a2, 4
    1c00:	f94e41        	l32r	a4, 138 (8000 <_end+0x12e4>)
    1c03:	0492b2        	l16si	a11, a2, 8
    1c06:	533930        	max	a3, a9, a3
    1c09:	c0c4a0        	sub	a12, a4, a10
    1c0c:	11cc00        	slli	a12, a12, 16
    1c0f:	aaca           	add.n	a10, a10, a12
    1c11:	01a9           	s32i.n	a10, a1, 0
    1c13:	c0c4b0        	sub	a12, a4, a11
    1c16:	11cc00        	slli	a12, a12, 16
    1c19:	bbca           	add.n	a11, a11, a12
    1c1b:	11b9           	s32i.n	a11, a1, 4
    1c1d:	01cd           	mov.n	a12, a1
    1c1f:	03bd           	mov.n	a11, a3
    1c21:	42a8           	l32i.n	a10, a2, 16
    1c23:	0008e0        	callx8	a8
    1c26:	01cd           	mov.n	a12, a1
    1c28:	059292        	l16si	a9, a2, 10
    1c2b:	42a9           	s32i.n	a10, a2, 16
    1c2d:	f95981        	l32r	a8, 194 (64c <siir_safr>)
    1c30:	0692a2        	l16si	a10, a2, 12
    1c33:	c0b490        	sub	a11, a4, a9
    1c36:	11bb00        	slli	a11, a11, 16
    1c39:	99ba           	add.n	a9, a9, a11
    1c3b:	0199           	s32i.n	a9, a1, 0
    1c3d:	c0b4a0        	sub	a11, a4, a10
    1c40:	11bb00        	slli	a11, a11, 16
    1c43:	aaba           	add.n	a10, a10, a11
    1c45:	03bd           	mov.n	a11, a3
    1c47:	11a9           	s32i.n	a10, a1, 4
    1c49:	52a8           	l32i.n	a10, a2, 20
    1c4b:	0008e0        	callx8	a8
    1c4e:	42e8           	l32i.n	a14, a2, 16
    1c50:	52a9           	s32i.n	a10, a2, 20
    1c52:	22cd           	ae_l16si.n	a12, a2, 0
    1c54:	0add           	mov.n	a13, a10
    1c56:	0a0c           	movi.n	a10, 0
    1c58:	ccea           	add.n	a12, a12, a14
    1c5a:	0cacd7        	bge	a12, a13, 1c6a <TxRef_VAD+0xbe>
    1c5d:	190c           	movi.n	a9, 1

00001c5f <TxRef_VAD+0xb3>:
    1c5f:	0c5292        	s16i	a9, a2, 24

00001c62 <TxRef_VAD+0xb6>:
    1c62:	0d52a2        	s16i	a10, a2, 26
    1c65:	0c9222        	l16si	a2, a2, 24
    1c68:	f01d           	retw.n

00001c6a <TxRef_VAD+0xbe>:
    1c6a:	0c92f2        	l16si	a15, a2, 24
    1c6d:	0d92b2        	l16si	a11, a2, 26
    1c70:	091f66        	bnei	a15, 1, 1c7d <TxRef_VAD+0xd1>
    1c73:	328d           	ae_l16si.n	a8, a2, 2
    1c75:	0a9d           	mov.n	a9, a10
    1c77:	02a8b7        	bge	a8, a11, 1c7d <TxRef_VAD+0xd1>
    1c7a:	fff846        	j	1c5f <TxRef_VAD+0xb3>

00001c7d <TxRef_VAD+0xd1>:
    1c7d:	ab1b           	addi.n	a10, a11, 1
    1c7f:	fff7c6        	j	1c62 <TxRef_VAD+0xb6>

00001c82 <TxRef_VAD+0xd6>:
	...

00001c84 <vector_gain_Q9_15>:
    1c84:	004136        	entry	a1, 32
    1c87:	399334        	ae_movpa24x2	aep1, a3, a3
    1c8a:	440204        	ae_lp16x2f.i	aep0, a2, 0
    1c8d:	215140        	srai	a5, a4, 1
    1c90:	18a576        	loopgtz	a5, 1cac <vector_gain_Q9_15+0x28>
    1c93:	010954        	ae_mulfp24s.hl	aeq1, aep0, aep1
    1c96:	010174        	ae_mulfp24s.ll	aeq0, aep0, aep1
    1c99:	355214        	ae_slliq56	aeq1, aeq1, 8
    1c9c:	f96bba820f 	{ ae_slliq56	aeq0, aeq0, 8; ae_roundsp16q48asym	aep2, aeq1 }
    1ca1:	f863b2220f 	{ ae_sp16f.l.i	aep2, a2, 0; ae_roundsp16q48asym	aep0, aeq0 }
    1ca6:	420214        	ae_sp16f.l.i	aep0, a2, 2
    1ca9:	448214        	ae_lp16x2f.iu	aep0, a2, 4

00001cac <vector_gain_Q9_15+0x28>:
    1cac:	f01d           	retw.n

00001cae <vector_gain_Q9_15+0x2a>:
	...

00001cb0 <main_err_alpha>:
    1cb0:	004136        	entry	a1, 32
    1cb3:	a40c           	movi.n	a4, 10
    1cb5:	f93871        	l32r	a7, 198 (7fff0000 <_end+0x7ffe92e4>)
    1cb8:	682252        	l32i	a5, a2, 0x1a0
    1cbb:	f91f61        	l32r	a6, 138 (8000 <_end+0x12e4>)
    1cbe:	1e2222        	l32i	a2, a2, 120
    1cc1:	fec552        	addi	a5, a5, -2
    1cc4:	c06630        	sub	a6, a6, a3
    1cc7:	408204        	ae_lp16f.iu	aep0, a2, 0
    1cca:	116600        	slli	a6, a6, 16
    1ccd:	636a           	add.n	a6, a3, a6
    1ccf:	836730        	moveqz	a6, a7, a3
    1cd2:	309664        	ae_cvtp24a16x2.lh	aep1, a6, a6
    1cd5:	10a476        	loopgtz	a4, 1ce9 <main_err_alpha+0x39>
    1cd8:	e804aca51f 	{ ae_lp16f.iu	aep2, a5, 2; ae_mulfp24s.hh	aeq0, aep1, aep0 }
    1cdd:	011234        	ae_mulafp24s.ll	aeq0, aep1, aep2
    1ce0:	1b8f04        	ae_roundsp16q48asym	aep0, aeq0
    1ce3:	420504        	ae_sp16f.l.i	aep0, a5, 0
    1ce6:	408214        	ae_lp16f.iu	aep0, a2, 2

00001ce9 <main_err_alpha+0x39>:
    1ce9:	f01d           	retw.n

00001ceb <main_err_alpha+0x3b>:
	...

00001cec <Voice_EC_Prcs_common>:
    1cec:	02a136        	entry	a1, 0x150
    1cef:	456142        	s32i	a4, a1, 0x114
    1cf2:	059d           	mov.n	a9, a5
    1cf4:	02bd           	mov.n	a11, a2
    1cf6:	f8cb51        	l32r	a5, 24 (6d14 <p_ecnr_mem>)
    1cf9:	03cd           	mov.n	a12, a3
    1cfb:	072d           	mov.n	a2, a7
    1cfd:	05e8           	l32i.n	a14, a5, 0
    1cff:	296192        	s32i	a9, a1, 164
    1d02:	2861c2        	s32i	a12, a1, 160
    1d05:	2adea2        	addmi	a10, a14, 0x2a00
    1d08:	28de42        	addmi	a4, a14, 0x2800
    1d0b:	15de82        	addmi	a8, a14, 0x1500
    1d0e:	1bdef2        	addmi	a15, a14, 0x1b00
    1d11:	78cff2        	addi	a15, a15, 120
    1d14:	7f9882        	l16si	a8, a8, 254
    1d17:	78c442        	addi	a4, a4, 120
    1d1a:	601aa2        	l16ui	a10, a10, 192
    1d1d:	012866        	bnei	a8, 2, 1d22 <Voice_EC_Prcs_common+0x36>
    1d20:	63cc           	bnez.n	a3, 1d2a <Voice_EC_Prcs_common+0x3e>

00001d22 <Voice_EC_Prcs_common+0x36>:
    1d22:	180c           	movi.n	a8, 1
    1d24:	416182        	s32i	a8, a1, 0x104
    1d27:	000106        	j	1d2f <Voice_EC_Prcs_common+0x43>

00001d2a <Voice_EC_Prcs_common+0x3e>:
    1d2a:	290c           	movi.n	a9, 2
    1d2c:	416192        	s32i	a9, a1, 0x104

00001d2f <Voice_EC_Prcs_common+0x43>:
    1d2f:	24de32        	addmi	a3, a14, 0x2400
    1d32:	60c332        	addi	a3, a3, 96
    1d35:	1acc           	bnez.n	a10, 1d3a <Voice_EC_Prcs_common+0x4e>
    1d37:	02ab06        	j	27e7 <Voice_EC_Prcs_common+0xafb>
    1d3a:	3e6122        	s32i	a2, a1, 248
    1d3d:	486162        	s32i	a6, a1, 0x120
    1d40:	3761f2        	s32i	a15, a1, 220
    1d43:	35de82        	addmi	a8, a14, 0x3500
    1d46:	4661b2        	s32i	a11, a1, 0x118
    1d49:	4521d2        	l32i	a13, a1, 0x114
    1d4c:	36dea2        	addmi	a10, a14, 0x3600
    1d4f:	0c9d           	mov.n	a9, a12
    1d51:	3d6192        	s32i	a9, a1, 244
    1d54:	e8caa2        	addi	a10, a10, -24
    1d57:	4361d2        	s32i	a13, a1, 0x10c
    1d5a:	2921b2        	l32i	a11, a1, 164
    1d5d:	4a2882        	l32i	a8, a8, 0x128
    1d60:	406182        	s32i	a8, a1, 0x100
    1d63:	f90e81        	l32r	a8, 19c (1bac <TxRef_VAD>)
    1d66:	5421e2        	l32i	a14, a1, 0x150
    1d69:	4461e2        	s32i	a14, a1, 0x110
    1d6c:	0008e0        	callx8	a8
    1d6f:	05e8           	l32i.n	a14, a5, 0
    1d71:	15def2        	addmi	a15, a14, 0x1500
    1d74:	819f72        	l16si	a7, a15, 0x102
    1d77:	4aac           	beqz.n	a10, 1d9f <Voice_EC_Prcs_common+0xb3>
    1d79:	1894d2        	l16si	a13, a4, 48
    1d7c:	ffa192        	movi	a9, 0x1ff
    1d7f:	c0afa2        	movi	a10, -64
    1d82:	0baa77        	bge	a10, a7, 1d91 <Voice_EC_Prcs_common+0xa5>
    1d85:	f87c           	movi.n	a8, -1
    1d87:	770b           	addi.n	a7, a7, -1
    1d89:	437780        	min	a7, a7, a8
    1d8c:	877d           	ae_sext16	a7, a7
    1d8e:	815f72        	s16i	a7, a15, 0x102

00001d91 <Voice_EC_Prcs_common+0xa5>:
    1d91:	1929d7        	blt	a9, a13, 1dae <Voice_EC_Prcs_common+0xc2>
    1d94:	819f72        	l16si	a7, a15, 0x102
    1d97:	ad1b           	addi.n	a10, a13, 1
    1d99:	1854a2        	s16i	a10, a4, 48
    1d9c:	000386        	j	1dae <Voice_EC_Prcs_common+0xc2>

00001d9f <Voice_EC_Prcs_common+0xb3>:
    1d9f:	0bd7e6        	bgei	a7, 64, 1dae <Voice_EC_Prcs_common+0xc2>
    1da2:	180c           	movi.n	a8, 1
    1da4:	771b           	addi.n	a7, a7, 1
    1da6:	537780        	max	a7, a7, a8
    1da9:	877d           	ae_sext16	a7, a7
    1dab:	815f72        	s16i	a7, a15, 0x102
    1dae:	35def2        	addmi	a15, a14, 0x3500
    1db1:	e8cff2        	addi	a15, a15, -24
    1db4:	7f1f92        	l16ui	a9, a15, 254
    1db7:	89dc           	bnez.n	a9, 1dd3 <Voice_EC_Prcs_common+0xe7>
    1db9:	080c           	movi.n	a8, 0
    1dbb:	186482        	s32i	a8, a4, 96
    1dbe:	286482        	s32i	a8, a4, 160
    1dc1:	386482        	s32i	a8, a4, 224
    1dc4:	486482        	s32i	a8, a4, 0x120
    1dc7:	196482        	s32i	a8, a4, 100
    1dca:	296482        	s32i	a8, a4, 164
    1dcd:	396482        	s32i	a8, a4, 228
    1dd0:	496482        	s32i	a8, a4, 0x124

00001dd3 <Voice_EC_Prcs_common+0xe7>:
    1dd3:	2921b2        	l32i	a11, a1, 164
    1dd6:	7e1fd2        	l16ui	a13, a15, 252
    1dd9:	3721a2        	l32i	a10, a1, 220
    1ddc:	f8f181        	l32r	a8, 1a0 (15fc <mic120_delay>)
    1ddf:	15dec2        	addmi	a12, a14, 0x1500
    1de2:	cc4b           	addi.n	a12, a12, 4
    1de4:	0008e0        	callx8	a8
    1de7:	10c1a2        	addi	a10, a1, 16
    1dea:	0b0c           	movi.n	a11, 0
    1dec:	ec0c           	movi.n	a12, 14
    1dee:	f8b481        	l32r	a8, c0 (5608 <xt_memset>)
    1df1:	372192        	l32i	a9, a1, 220
    1df4:	476192        	s32i	a9, a1, 0x11c
    1df7:	0008e0        	callx8	a8
    1dfa:	4a6122        	s32i	a2, a1, 0x128
    1dfd:	4521a2        	l32i	a10, a1, 0x114
    1e00:	4021e2        	l32i	a14, a1, 0x100
    1e03:	0f0c           	movi.n	a15, 0
    1e05:	c453f2        	s16i	a15, a3, 0x188
    1e08:	03dee2        	addmi	a14, a14, 0x300
    1e0b:	34ceb2        	addi	a11, a14, 52
    1e0e:	4961b2        	s32i	a11, a1, 0x124
    1e11:	04ca16        	beqz	a10, 1e61 <Voice_EC_Prcs_common+0x175>
    1e14:	4021d2        	l32i	a13, a1, 0x100
    1e17:	e69bb2        	l16si	a11, a11, 0x1cc
    1e1a:	0c0c           	movi.n	a12, 0
    1e1c:	d46dc2        	s32i	a12, a13, 0x350
    1e1f:	e46dc2        	s32i	a12, a13, 0x390
    1e22:	f46dc2        	s32i	a12, a13, 0x3d0
    1e25:	446ec2        	s32i	a12, a14, 0x110
    1e28:	d56dc2        	s32i	a12, a13, 0x354
    1e2b:	e56dc2        	s32i	a12, a13, 0x394
    1e2e:	f56dc2        	s32i	a12, a13, 0x3d4
    1e31:	456ec2        	s32i	a12, a14, 0x114
    1e34:	78a0c2        	movi	a12, 120
    1e37:	ffe4e5        	call8	1c84 <vector_gain_Q9_15>
    1e3a:	4521a2        	l32i	a10, a1, 0x114
    1e3d:	cc3c           	movi.n	a12, 60
    1e3f:	05b8           	l32i.n	a11, a5, 0
    1e41:	1961b2        	s32i	a11, a1, 100
    1e44:	00b565        	call8	299c <conv_16_to_24_x2>
    1e47:	1921a2        	l32i	a10, a1, 100
    1e4a:	4521d2        	l32i	a13, a1, 0x114
    1e4d:	4021c2        	l32i	a12, a1, 0x100
    1e50:	78a0e2        	movi	a14, 120
    1e53:	f8d481        	l32r	a8, 1a4 (190c <biquad_2cascade_24bitTx>)
    1e56:	0cbd           	mov.n	a11, a12
    1e58:	58ccc2        	addi	a12, a12, 88
    1e5b:	0008e0        	callx8	a8
    1e5e:	4a6122        	s32i	a2, a1, 0x128

00001e61 <Voice_EC_Prcs_common+0x175>:
    1e61:	02d3b2        	addmi	a11, a3, 0x200
    1e64:	02d4d2        	addmi	a13, a4, 0x200
    1e67:	01d422        	addmi	a2, a4, 0x100
    1e6a:	928b           	addi.n	a9, a2, 8
    1e6c:	c8c2a2        	addi	a10, a2, -56
    1e6f:	ed8b           	addi.n	a14, a13, 8
    1e71:	a0cbc2        	addi	a12, a11, -96
    1e74:	88c282        	addi	a8, a2, -120
    1e77:	c8cdf2        	addi	a15, a13, -56
    1e7a:	2161f2        	s32i	a15, a1, 132
    1e7d:	3c6182        	s32i	a8, a1, 240
    1e80:	2761c2        	s32i	a12, a1, 156
    1e83:	2261e2        	s32i	a14, a1, 136
    1e86:	3a61a2        	s32i	a10, a1, 232
    1e89:	3b6192        	s32i	a9, a1, 236
    1e8c:	2ccbb2        	addi	a11, a11, 44
    1e8f:	2661b2        	s32i	a11, a1, 152
    1e92:	01d392        	addmi	a9, a3, 0x100
    1e95:	88cdd2        	addi	a13, a13, -120
    1e98:	2361d2        	s32i	a13, a1, 140
    1e9b:	94c9a2        	addi	a10, a9, -108
    1e9e:	48c222        	addi	a2, a2, 72
    1ea1:	206122        	s32i	a2, a1, 128
    1ea4:	1c61a2        	s32i	a10, a1, 112
    1ea7:	acc992        	addi	a9, a9, -84
    1eaa:	186192        	s32i	a9, a1, 96
    1ead:	4021a2        	l32i	a10, a1, 0x100
    1eb0:	18c192        	addi	a9, a1, 24
    1eb3:	386192        	s32i	a9, a1, 224
    1eb6:	01dab2        	addmi	a11, a10, 0x100
    1eb9:	04dae2        	addmi	a14, a10, 0x400
    1ebc:	38cad2        	addi	a13, a10, 56
    1ebf:	18cac2        	addi	a12, a10, 24
    1ec2:	40c192        	addi	a9, a1, 64
    1ec5:	1d6192        	s32i	a9, a1, 116
    1ec8:	3561c2        	s32i	a12, a1, 212
    1ecb:	3461d2        	s32i	a13, a1, 208
    1ece:	b8ce22        	addi	a2, a14, -72
    1ed1:	38ce82        	addi	a8, a14, 56
    1ed4:	f8cef2        	addi	a15, a14, -8
    1ed7:	8ccbb2        	addi	a11, a11, -116
    1eda:	2a61b2        	s32i	a11, a1, 168
    1edd:	2f61f2        	s32i	a15, a1, 188
    1ee0:	2c6182        	s32i	a8, a1, 176
    1ee3:	2e6122        	s32i	a2, a1, 184
    1ee6:	14c1d2        	addi	a13, a1, 20
    1ee9:	1f61d2        	s32i	a13, a1, 124
    1eec:	1cc122        	addi	a2, a1, 28
    1eef:	48c482        	addi	a8, a4, 72
    1ef2:	24c1f2        	addi	a15, a1, 36
    1ef5:	2b61f2        	s32i	a15, a1, 172
    1ef8:	396182        	s32i	a8, a1, 228
    1efb:	03daa2        	addmi	a10, a10, 0x300
    1efe:	3f6122        	s32i	a2, a1, 252
    1f01:	78cee2        	addi	a14, a14, 120
    1f04:	3161e2        	s32i	a14, a1, 196
    1f07:	03d322        	addmi	a2, a3, 0x300
    1f0a:	78cab2        	addi	a11, a10, 120
    1f0d:	38cac2        	addi	a12, a10, 56
    1f10:	380c           	movi.n	a8, 3
    1f12:	426182        	s32i	a8, a1, 0x108
    1f15:	2d61c2        	s32i	a12, a1, 180
    1f18:	3061b2        	s32i	a11, a1, 192
    1f1b:	b8c222        	addi	a2, a2, -72
    1f1e:	28c1e2        	addi	a14, a1, 40
    1f21:	3261e2        	s32i	a14, a1, 200
    1f24:	30caa2        	addi	a10, a10, 48
    1f27:	366122        	s32i	a2, a1, 216
    1f2a:	7cc3b2        	addi	a11, a3, 124
    1f2d:	20c1c2        	addi	a12, a1, 32
    1f30:	2561c2        	s32i	a12, a1, 148
    1f33:	1a61b2        	s32i	a11, a1, 104
    1f36:	f89c21        	l32r	a2, 1a8 (1050 <subband_aec_mid_channel+0xa0>)
    1f39:	3361a2        	s32i	a10, a1, 204
    1f3c:	38c1a2        	addi	a10, a1, 56
    1f3f:	1b61a2        	s32i	a10, a1, 108

00001f42 <Voice_EC_Prcs_common+0x256>:
    1f42:	4621a2        	l32i	a10, a1, 0x118
    1f45:	05c8           	l32i.n	a12, a5, 0
    1f47:	8d2c           	movi.n	a13, 40
    1f49:	f89881        	l32r	a8, 1ac (1758 <emph_filter_N>)
    1f4c:	bc2a           	add.n	a11, a12, a2
    1f4e:	10dcc2        	addmi	a12, a12, 0x1000
    1f51:	34ccc2        	addi	a12, a12, 52
    1f54:	0008e0        	callx8	a8
    1f57:	4721a2        	l32i	a10, a1, 0x11c
    1f5a:	05c8           	l32i.n	a12, a5, 0
    1f5c:	8d2c           	movi.n	a13, 40
    1f5e:	f89381        	l32r	a8, 1ac (1758 <emph_filter_N>)
    1f61:	bc2a           	add.n	a11, a12, a2
    1f63:	10dcc2        	addmi	a12, a12, 0x1000
    1f66:	38ccc2        	addi	a12, a12, 56
    1f69:	0008e0        	callx8	a8
    1f6c:	4621c2        	l32i	a12, a1, 0x118
    1f6f:	05b8           	l32i.n	a11, a5, 0
    1f71:	10c1e2        	addi	a14, a1, 16
    1f74:	f88f81        	l32r	a8, 1b0 (1678 <tx_downsample_by2>)
    1f77:	23dbd2        	addmi	a13, a11, 0x2300
    1f7a:	15dbb2        	addmi	a11, a11, 0x1500
    1f7d:	78cdd2        	addi	a13, a13, 120
    1f80:	432ba2        	l32i	a10, a11, 0x10c
    1f83:	442bb2        	l32i	a11, a11, 0x110
    1f86:	0008e0        	callx8	a8
    1f89:	4721c2        	l32i	a12, a1, 0x11c
    1f8c:	05b8           	l32i.n	a11, a5, 0
    1f8e:	3821e2        	l32i	a14, a1, 224
    1f91:	f88781        	l32r	a8, 1b0 (1678 <tx_downsample_by2>)
    1f94:	24dbd2        	addmi	a13, a11, 0x2400
    1f97:	15dbb2        	addmi	a11, a11, 0x1500
    1f9a:	98cdd2        	addi	a13, a13, -104
    1f9d:	472ba2        	l32i	a10, a11, 0x11c
    1fa0:	482bb2        	l32i	a11, a11, 0x120
    1fa3:	0008e0        	callx8	a8
    1fa6:	3f24d2        	l32i	a13, a4, 252
    1fa9:	05c8           	l32i.n	a12, a5, 0
    1fab:	2f24e2        	l32i	a14, a4, 188
    1fae:	f88181        	l32r	a8, 1b4 (17d8 <analy_3band>)
    1fb1:	2bdcb2        	addmi	a11, a12, 0x2b00
    1fb4:	15dcc2        	addmi	a12, a12, 0x1500
    1fb7:	d8cbb2        	addi	a11, a11, -40
    1fba:	432ca2        	l32i	a10, a12, 0x10c
    1fbd:	422cc2        	l32i	a12, a12, 0x108
    1fc0:	0008e0        	callx8	a8
    1fc3:	1f24d2        	l32i	a13, a4, 124
    1fc6:	05a8           	l32i.n	a10, a5, 0
    1fc8:	ae0c           	movi.n	a14, 10
    1fca:	f87681        	l32r	a8, 1a4 (190c <biquad_2cascade_24bitTx>)
    1fcd:	10dac2        	addmi	a12, a10, 0x1000
    1fd0:	24dab2        	addmi	a11, a10, 0x2400
    1fd3:	18cbb2        	addi	a11, a11, 24
    1fd6:	16daa2        	addmi	a10, a10, 0x1600
    1fd9:	2aa8           	l32i.n	a10, a10, 8
    1fdb:	0008e0        	callx8	a8
    1fde:	4024d2        	l32i	a13, a4, 0x100
    1fe1:	05c8           	l32i.n	a12, a5, 0
    1fe3:	3024e2        	l32i	a14, a4, 192
    1fe6:	f87381        	l32r	a8, 1b4 (17d8 <analy_3band>)
    1fe9:	2bdcb2        	addmi	a11, a12, 0x2b00
    1fec:	15dcc2        	addmi	a12, a12, 0x1500
    1fef:	18cbb2        	addi	a11, a11, 24
    1ff2:	472ca2        	l32i	a10, a12, 0x11c
    1ff5:	422cc2        	l32i	a12, a12, 0x108
    1ff8:	0008e0        	callx8	a8
    1ffb:	05a8           	l32i.n	a10, a5, 0
    1ffd:	2024b2        	l32i	a11, a4, 128
    2000:	f86e81        	l32r	a8, 1b8 (1970 <conv_24_to_16>)
    2003:	16daa2        	addmi	a10, a10, 0x1600
    2006:	2aa8           	l32i.n	a10, a10, 8
    2008:	0008e0        	callx8	a8
    200b:	04ad           	mov.n	a10, a4
    200d:	3921b2        	l32i	a11, a1, 228
    2010:	ac0c           	movi.n	a12, 10
    2012:	f86a81        	l32r	a8, 1bc (d3c <subband_aec_block>)
    2015:	07dd           	mov.n	a13, a7
    2017:	1e0c           	movi.n	a14, 1
    2019:	0008e0        	callx8	a8
    201c:	04ad           	mov.n	a10, a4
    201e:	3a21b2        	l32i	a11, a1, 232
    2021:	ac0c           	movi.n	a12, 10
    2023:	f86681        	l32r	a8, 1bc (d3c <subband_aec_block>)
    2026:	07dd           	mov.n	a13, a7
    2028:	3e0c           	movi.n	a14, 3
    202a:	0008e0        	callx8	a8
    202d:	04ad           	mov.n	a10, a4
    202f:	3b21b2        	l32i	a11, a1, 236
    2032:	4c1c           	movi.n	a12, 20
    2034:	f86281        	l32r	a8, 1bc (d3c <subband_aec_block>)
    2037:	07dd           	mov.n	a13, a7
    2039:	4e0c           	movi.n	a14, 4
    203b:	0008e0        	callx8	a8
    203e:	04ad           	mov.n	a10, a4
    2040:	3c21b2        	l32i	a11, a1, 240
    2043:	f85f81        	l32r	a8, 1c0 (fb0 <subband_aec_mid_channel>)
    2046:	07cd           	mov.n	a12, a7
    2048:	2d0c           	movi.n	a13, 2
    204a:	0008e0        	callx8	a8
    204d:	2124a2        	l32i	a10, a4, 132
    2050:	4124b2        	l32i	a11, a4, 0x104
    2053:	5124c2        	l32i	a12, a4, 0x144
    2056:	3124d2        	l32i	a13, a4, 196
    2059:	f85a81        	l32r	a8, 1c4 (198c <gain_after_ec>)
    205c:	0e0c           	movi.n	a14, 0
    205e:	0f0c           	movi.n	a15, 0
    2060:	0008e0        	callx8	a8
    2063:	412192        	l32i	a9, a1, 0x104
    2066:	05e8           	l32i.n	a14, a5, 0
    2068:	fec992        	addi	a9, a9, -2
    206b:	3f0956        	bnez	a9, 245f <Voice_EC_Prcs_common+0x773>
    206e:	be2a           	add.n	a11, a14, a2
    2070:	3d21a2        	l32i	a10, a1, 244
    2073:	8d2c           	movi.n	a13, 40
    2075:	f84d81        	l32r	a8, 1ac (1758 <emph_filter_N>)
    2078:	10dec2        	addmi	a12, a14, 0x1000
    207b:	3cccc2        	addi	a12, a12, 60
    207e:	0008e0        	callx8	a8
    2081:	3d21c2        	l32i	a12, a1, 244
    2084:	05b8           	l32i.n	a11, a5, 0
    2086:	1f21e2        	l32i	a14, a1, 124
    2089:	f84981        	l32r	a8, 1b0 (1678 <tx_downsample_by2>)
    208c:	24dbd2        	addmi	a13, a11, 0x2400
    208f:	15dbb2        	addmi	a11, a11, 0x1500
    2092:	b8cdd2        	addi	a13, a13, -72
    2095:	452ba2        	l32i	a10, a11, 0x114
    2098:	462bb2        	l32i	a11, a11, 0x118
    209b:	0008e0        	callx8	a8
    209e:	7f24d2        	l32i	a13, a4, 0x1fc
    20a1:	05c8           	l32i.n	a12, a5, 0
    20a3:	6f24e2        	l32i	a14, a4, 0x1bc
    20a6:	f84381        	l32r	a8, 1b4 (17d8 <analy_3band>)
    20a9:	2cdcb2        	addmi	a11, a12, 0x2c00
    20ac:	15dcc2        	addmi	a12, a12, 0x1500
    20af:	d4cbb2        	addi	a11, a11, -44
    20b2:	452ca2        	l32i	a10, a12, 0x114
    20b5:	422cc2        	l32i	a12, a12, 0x108
    20b8:	0008e0        	callx8	a8
    20bb:	5f24d2        	l32i	a13, a4, 0x17c
    20be:	05a8           	l32i.n	a10, a5, 0
    20c0:	ae0c           	movi.n	a14, 10
    20c2:	f83881        	l32r	a8, 1a4 (190c <biquad_2cascade_24bitTx>)
    20c5:	10dac2        	addmi	a12, a10, 0x1000
    20c8:	24dab2        	addmi	a11, a10, 0x2400
    20cb:	30cbb2        	addi	a11, a11, 48
    20ce:	16daa2        	addmi	a10, a10, 0x1600
    20d1:	2aa8           	l32i.n	a10, a10, 8
    20d3:	0008e0        	callx8	a8
    20d6:	04ad           	mov.n	a10, a4
    20d8:	2021b2        	l32i	a11, a1, 128
    20db:	ac0c           	movi.n	a12, 10
    20dd:	f83781        	l32r	a8, 1bc (d3c <subband_aec_block>)
    20e0:	07dd           	mov.n	a13, a7
    20e2:	1e0c           	movi.n	a14, 1
    20e4:	0008e0        	callx8	a8
    20e7:	04ad           	mov.n	a10, a4
    20e9:	2121b2        	l32i	a11, a1, 132
    20ec:	ac0c           	movi.n	a12, 10
    20ee:	f83381        	l32r	a8, 1bc (d3c <subband_aec_block>)
    20f1:	07dd           	mov.n	a13, a7
    20f3:	3e0c           	movi.n	a14, 3
    20f5:	0008e0        	callx8	a8
    20f8:	04ad           	mov.n	a10, a4
    20fa:	2221b2        	l32i	a11, a1, 136
    20fd:	4c1c           	movi.n	a12, 20
    20ff:	f82f81        	l32r	a8, 1bc (d3c <subband_aec_block>)
    2102:	07dd           	mov.n	a13, a7
    2104:	4e0c           	movi.n	a14, 4
    2106:	0008e0        	callx8	a8
    2109:	04ad           	mov.n	a10, a4
    210b:	2321b2        	l32i	a11, a1, 140
    210e:	f82c81        	l32r	a8, 1c0 (fb0 <subband_aec_mid_channel>)
    2111:	07cd           	mov.n	a12, a7
    2113:	2d0c           	movi.n	a13, 2
    2115:	0008e0        	callx8	a8
    2118:	6124a2        	l32i	a10, a4, 0x184
    211b:	8124b2        	l32i	a11, a4, 0x204
    211e:	9124c2        	l32i	a12, a4, 0x244
    2121:	7124d2        	l32i	a13, a4, 0x1c4
    2124:	f82881        	l32r	a8, 1c4 (198c <gain_after_ec>)
    2127:	1e0c           	movi.n	a14, 1
    2129:	0f0c           	movi.n	a15, 0
    212b:	0008e0        	callx8	a8
    212e:	05e8           	l32i.n	a14, a5, 0
    2130:	15def2        	addmi	a15, a14, 0x1500
    2133:	829f92        	l16si	a9, a15, 0x104
    2136:	049890        	extui	a9, a9, 8, 1
    2139:	28c916        	beqz	a9, 23c9 <Voice_EC_Prcs_common+0x6dd>
    213c:	03ad           	mov.n	a10, a3
    213e:	f82281        	l32r	a8, 1c8 (2e64 <prelim_filter>)
    2141:	30c1b2        	addi	a11, a1, 48
    2144:	50c1c2        	addi	a12, a1, 80
    2147:	0008e0        	callx8	a8
    214a:	03ad           	mov.n	a10, a3
    214c:	1a21b2        	l32i	a11, a1, 104
    214f:	f81f81        	l32r	a8, 1cc (3114 <prelim_filter3>)
    2152:	3c0c           	movi.n	a12, 3
    2154:	1b21d2        	l32i	a13, a1, 108
    2157:	0008e0        	callx8	a8
    215a:	03ad           	mov.n	a10, a3
    215c:	1c21b2        	l32i	a11, a1, 112
    215f:	f81b81        	l32r	a8, 1cc (3114 <prelim_filter3>)
    2162:	4c0c           	movi.n	a12, 4
    2164:	1d21d2        	l32i	a13, a1, 116
    2167:	0008e0        	callx8	a8
    216a:	c1a8           	l32i.n	a10, a1, 48
    216c:	f81981        	l32r	a8, 1d0 (1adc <adjust_phase>)
    216f:	2a93b2        	l16si	a11, a3, 84
    2172:	1e61b2        	s32i	a11, a1, 120
    2175:	0008e0        	callx8	a8
    2178:	c1a9           	s32i.n	a10, a1, 48
    217a:	f81581        	l32r	a8, 1d0 (1adc <adjust_phase>)
    217d:	1e21b2        	l32i	a11, a1, 120
    2180:	e1a8           	l32i.n	a10, a1, 56
    2182:	0008e0        	callx8	a8
    2185:	e1a9           	s32i.n	a10, a1, 56
    2187:	f81281        	l32r	a8, 1d0 (1adc <adjust_phase>)
    218a:	1e21b2        	l32i	a11, a1, 120
    218d:	1021a2        	l32i	a10, a1, 64
    2190:	0008e0        	callx8	a8
    2193:	0a93b2        	l16si	a11, a3, 20
    2196:	109392        	l16si	a9, a3, 32
    2199:	1061a2        	s32i	a10, a1, 64
    219c:	dd7c           	movi.n	a13, -3
    219e:	24a9b7        	bge	a9, a11, 21c6 <Voice_EC_Prcs_common+0x4da>
    21a1:	03ad           	mov.n	a10, a3
    21a3:	f80c81        	l32r	a8, 1d4 (33d8 <cali_power>)
    21a6:	1821b2        	l32i	a11, a1, 96
    21a9:	30c1c2        	addi	a12, a1, 48
    21ac:	0008e0        	callx8	a8
    21af:	f80a81        	l32r	a8, 1d8 (3288 <cali_module_short>)
    21b2:	03ad           	mov.n	a10, a3
    21b4:	30c1b2        	addi	a11, a1, 48
    21b7:	0008e0        	callx8	a8
    21ba:	6793d2        	l16si	a13, a3, 206
    21bd:	6a9392        	l16si	a9, a3, 212
    21c0:	176192        	s32i	a9, a1, 92
    21c3:	000506        	j	21db <Voice_EC_Prcs_common+0x4ef>

000021c6 <Voice_EC_Prcs_common+0x4da>:
    21c6:	0b0c           	movi.n	a11, 0
    21c8:	0a0c           	movi.n	a10, 0
    21ca:	dc7c           	movi.n	a12, -3
    21cc:	6753c2        	s16i	a12, a3, 206
    21cf:	1761a2        	s32i	a10, a1, 92
    21d2:	6b53b2        	s16i	a11, a3, 214
    21d5:	6953b2        	s16i	a11, a3, 210
    21d8:	6a53b2        	s16i	a11, a3, 212

000021db <Voice_EC_Prcs_common+0x4ef>:
    21db:	0e0c           	movi.n	a14, 0
    21dd:	1f0c           	movi.n	a15, 1
    21df:	f7ff81        	l32r	a8, 1dc (37ac <beam1_proc>)
    21e2:	1721a2        	l32i	a10, a1, 92
    21e5:	ce13c2        	l16ui	a12, a3, 0x19c
    21e8:	699392        	l16si	a9, a3, 210
    21eb:	0b2c           	movi.n	a11, 32
    21ed:	63ccb0        	minu	a12, a12, a11
    21f0:	c0bbc0        	sub	a11, a11, a12
    21f3:	8299c0        	mull	a9, a9, a12
    21f6:	82aab0        	mull	a10, a10, a11
    21f9:	2721b2        	l32i	a11, a1, 156
    21fc:	6823c2        	l32i	a12, a3, 0x1a0
    21ff:	99aa           	add.n	a9, a9, a10
    2201:	03ad           	mov.n	a10, a3
    2203:	10c992        	addi	a9, a9, 16
    2206:	219590        	srai	a9, a9, 5
    2209:	6a5392        	s16i	a9, a3, 212
    220c:	0008e0        	callx8	a8
    220f:	03ad           	mov.n	a10, a3
    2211:	2621b2        	l32i	a11, a1, 152
    2214:	8d23c2        	l32i	a12, a3, 0x234
    2217:	6993d2        	l16si	a13, a3, 210
    221a:	f7f081        	l32r	a8, 1dc (37ac <beam1_proc>)
    221d:	0e0c           	movi.n	a14, 0
    221f:	0f0c           	movi.n	a15, 0
    2221:	0008e0        	callx8	a8
    2224:	03ad           	mov.n	a10, a3
    2226:	3621b2        	l32i	a11, a1, 216
    2229:	b023c2        	l32i	a12, a3, 0x2c0
    222c:	6a93d2        	l16si	a13, a3, 212
    222f:	f7eb81        	l32r	a8, 1dc (37ac <beam1_proc>)
    2232:	0e0c           	movi.n	a14, 0
    2234:	0f0c           	movi.n	a15, 0
    2236:	0008e0        	callx8	a8
    2239:	03ad           	mov.n	a10, a3
    223b:	3621b2        	l32i	a11, a1, 216
    223e:	b023c2        	l32i	a12, a3, 0x2c0
    2241:	6a93d2        	l16si	a13, a3, 212
    2244:	f7e681        	l32r	a8, 1dc (37ac <beam1_proc>)
    2247:	ae0c           	movi.n	a14, 10
    2249:	0f0c           	movi.n	a15, 0
    224b:	0008e0        	callx8	a8
    224e:	8c23a2        	l32i	a10, a3, 0x230
    2251:	f7e381        	l32r	a8, 1e0 (1b80 <gain_ref>)
    2254:	0493b2        	l16si	a11, a3, 8
    2257:	5c0c           	movi.n	a12, 5
    2259:	0008e0        	callx8	a8
    225c:	af23a2        	l32i	a10, a3, 0x2bc
    225f:	f7e081        	l32r	a8, 1e0 (1b80 <gain_ref>)
    2262:	0593b2        	l16si	a11, a3, 10
    2265:	ac0c           	movi.n	a12, 10
    2267:	0008e0        	callx8	a8
    226a:	05e8           	l32i.n	a14, a5, 0
    226c:	6823b2        	l32i	a11, a3, 0x1a0
    226f:	2fdea2        	addmi	a10, a14, 0x2f00
    2272:	2ede92        	addmi	a9, a14, 0x2e00
    2275:	c4c992        	addi	a9, a9, -60
    2278:	7f9992        	l16si	a9, a9, 254
    227b:	c0caa2        	addi	a10, a10, -64
    227e:	1661a2        	s32i	a10, a1, 88
    2281:	99bc           	beqz.n	a9, 22be <Voice_EC_Prcs_common+0x5d2>
    2283:	f76781        	l32r	a8, 20 (5620 <xt_memcpy>)
    2286:	1e23a2        	l32i	a10, a3, 120
    2289:	ac0c           	movi.n	a12, 10
    228b:	0008e0        	callx8	a8
    228e:	2721b2        	l32i	a11, a1, 156
    2291:	8e1c           	movi.n	a14, 24
    2293:	f7d481        	l32r	a8, 1e4 (3924 <beam2_proc>)
    2296:	090c           	movi.n	a9, 0
    2298:	2891f2        	l16si	a15, a1, 80
    229b:	05a8           	l32i.n	a10, a5, 0
    229d:	6793d2        	l16si	a13, a3, 206
    22a0:	6823c2        	l32i	a12, a3, 0x1a0
    22a3:	16daa2        	addmi	a10, a10, 0x1600
    22a6:	3aad           	ae_l16si.n	a10, a10, 2
    22a8:	01a9           	s32i.n	a10, a1, 0
    22aa:	1199           	s32i.n	a9, a1, 4
    22ac:	03ad           	mov.n	a10, a3
    22ae:	0008e0        	callx8	a8
    22b1:	1621b2        	l32i	a11, a1, 88
    22b4:	03ad           	mov.n	a10, a3
    22b6:	2bbd           	ae_l16si.n	a11, a11, 0
    22b8:	ff9f65        	call8	1cb0 <main_err_alpha>
    22bb:	0007c6        	j	22de <Voice_EC_Prcs_common+0x5f2>

000022be <Voice_EC_Prcs_common+0x5d2>:
    22be:	f7c981        	l32r	a8, 1e4 (3924 <beam2_proc>)
    22c1:	090c           	movi.n	a9, 0
    22c3:	2891f2        	l16si	a15, a1, 80
    22c6:	6793d2        	l16si	a13, a3, 206
    22c9:	0bcd           	mov.n	a12, a11
    22cb:	16dea2        	addmi	a10, a14, 0x1600
    22ce:	2721b2        	l32i	a11, a1, 156
    22d1:	8e1c           	movi.n	a14, 24
    22d3:	3aad           	ae_l16si.n	a10, a10, 2
    22d5:	01a9           	s32i.n	a10, a1, 0
    22d7:	1199           	s32i.n	a9, a1, 4
    22d9:	03ad           	mov.n	a10, a3
    22db:	0008e0        	callx8	a8

000022de <Voice_EC_Prcs_common+0x5f2>:
    22de:	2621b2        	l32i	a11, a1, 152
    22e1:	8e1c           	movi.n	a14, 24
    22e3:	f77cf1        	l32r	a15, d4 (7fff <_end+0x12e3>)
    22e6:	f7bf81        	l32r	a8, 1e4 (3924 <beam2_proc>)
    22e9:	090c           	movi.n	a9, 0
    22eb:	6993d2        	l16si	a13, a3, 210
    22ee:	8d23c2        	l32i	a12, a3, 0x234
    22f1:	0a1c           	movi.n	a10, 16
    22f3:	01a9           	s32i.n	a10, a1, 0
    22f5:	1199           	s32i.n	a9, a1, 4
    22f7:	03ad           	mov.n	a10, a3
    22f9:	0008e0        	callx8	a8
    22fc:	3621b2        	l32i	a11, a1, 216
    22ff:	9e1c           	movi.n	a14, 25
    2301:	f774f1        	l32r	a15, d4 (7fff <_end+0x12e3>)
    2304:	f7b881        	l32r	a8, 1e4 (3924 <beam2_proc>)
    2307:	090c           	movi.n	a9, 0
    2309:	6a93d2        	l16si	a13, a3, 212
    230c:	b023c2        	l32i	a12, a3, 0x2c0
    230f:	0a1c           	movi.n	a10, 16
    2311:	01a9           	s32i.n	a10, a1, 0
    2313:	1199           	s32i.n	a9, a1, 4
    2315:	03ad           	mov.n	a10, a3
    2317:	0008e0        	callx8	a8
    231a:	3621b2        	l32i	a11, a1, 216
    231d:	9e1c           	movi.n	a14, 25
    231f:	f76df1        	l32r	a15, d4 (7fff <_end+0x12e3>)
    2322:	f7b081        	l32r	a8, 1e4 (3924 <beam2_proc>)
    2325:	a90c           	movi.n	a9, 10
    2327:	6a93d2        	l16si	a13, a3, 212
    232a:	b023c2        	l32i	a12, a3, 0x2c0
    232d:	0a1c           	movi.n	a10, 16
    232f:	01a9           	s32i.n	a10, a1, 0
    2331:	1199           	s32i.n	a9, a1, 4
    2333:	03ad           	mov.n	a10, a3
    2335:	0008e0        	callx8	a8
    2338:	f7ac81        	l32r	a8, 1e8 (3c54 <beam1_mid_band>)
    233b:	03ad           	mov.n	a10, a3
    233d:	0008e0        	callx8	a8
    2340:	d223a2        	l32i	a10, a3, 0x348
    2343:	f7a781        	l32r	a8, 1e0 (1b80 <gain_ref>)
    2346:	0393b2        	l16si	a11, a3, 6
    2349:	ac0c           	movi.n	a12, 10
    234b:	0008e0        	callx8	a8
    234e:	f7a781        	l32r	a8, 1ec (3db0 <beam2_mid_band>)
    2351:	03ad           	mov.n	a10, a3
    2353:	0008e0        	callx8	a8
    2356:	6823a2        	l32i	a10, a3, 0x1a0
    2359:	8b23b2        	l32i	a11, a3, 0x22c
    235c:	05e8           	l32i.n	a14, a5, 0
    235e:	d123c2        	l32i	a12, a3, 0x344
    2361:	f7a381        	l32r	a8, 1f0 (1884 <synth_3band>)
    2364:	2bded2        	addmi	a13, a14, 0x2b00
    2367:	58cdd2        	addi	a13, a13, 88
    236a:	16dee2        	addmi	a14, a14, 0x1600
    236d:	9ee8           	l32i.n	a14, a14, 36
    236f:	0008e0        	callx8	a8
    2372:	ae23b2        	l32i	a11, a3, 0x2b8
    2375:	4821c2        	l32i	a12, a1, 0x120
    2378:	05a8           	l32i.n	a10, a5, 0
    237a:	3f21e2        	l32i	a14, a1, 252
    237d:	f79d81        	l32r	a8, 1f4 (16e8 <tx_upsample_by2>)
    2380:	24dad2        	addmi	a13, a10, 0x2400
    2383:	d8cdd2        	addi	a13, a13, -40
    2386:	16daa2        	addmi	a10, a10, 0x1600
    2389:	9aa8           	l32i.n	a10, a10, 36
    238b:	0008e0        	callx8	a8
    238e:	6a23a2        	l32i	a10, a3, 0x1a8
    2391:	8d23b2        	l32i	a11, a3, 0x234
    2394:	05e8           	l32i.n	a14, a5, 0
    2396:	d323c2        	l32i	a12, a3, 0x34c
    2399:	f79581        	l32r	a8, 1f0 (1884 <synth_3band>)
    239c:	2cded2        	addmi	a13, a14, 0x2c00
    239f:	14cdd2        	addi	a13, a13, 20
    23a2:	16dee2        	addmi	a14, a14, 0x1600
    23a5:	bee8           	l32i.n	a14, a14, 44
    23a7:	0008e0        	callx8	a8
    23aa:	b023b2        	l32i	a11, a3, 0x2c0
    23ad:	3e21c2        	l32i	a12, a1, 248
    23b0:	05a8           	l32i.n	a10, a5, 0
    23b2:	2521e2        	l32i	a14, a1, 148
    23b5:	f78f81        	l32r	a8, 1f4 (16e8 <tx_upsample_by2>)
    23b8:	24dad2        	addmi	a13, a10, 0x2400
    23bb:	f8cdd2        	addi	a13, a13, -8
    23be:	16daa2        	addmi	a10, a10, 0x1600
    23c1:	baa8           	l32i.n	a10, a10, 44
    23c3:	0008e0        	callx8	a8
    23c6:	002006        	j	244a <Voice_EC_Prcs_common+0x75e>

000023c9 <Voice_EC_Prcs_common+0x6dd>:
    23c9:	6124a2        	l32i	a10, a4, 0x184
    23cc:	8124b2        	l32i	a11, a4, 0x204
    23cf:	7124c2        	l32i	a12, a4, 0x1c4
    23d2:	2cded2        	addmi	a13, a14, 0x2c00
    23d5:	f78681        	l32r	a8, 1f0 (1884 <synth_3band>)
    23d8:	14cdd2        	addi	a13, a13, 20
    23db:	4b2fe2        	l32i	a14, a15, 0x12c
    23de:	0008e0        	callx8	a8
    23e1:	9124b2        	l32i	a11, a4, 0x244
    23e4:	3e21c2        	l32i	a12, a1, 248
    23e7:	05a8           	l32i.n	a10, a5, 0
    23e9:	2521e2        	l32i	a14, a1, 148
    23ec:	f78281        	l32r	a8, 1f4 (16e8 <tx_upsample_by2>)
    23ef:	24dad2        	addmi	a13, a10, 0x2400
    23f2:	f8cdd2        	addi	a13, a13, -8
    23f5:	16daa2        	addmi	a10, a10, 0x1600
    23f8:	baa8           	l32i.n	a10, a10, 44
    23fa:	0008e0        	callx8	a8
    23fd:	3e21a2        	l32i	a10, a1, 248
    2400:	05c8           	l32i.n	a12, a5, 0
    2402:	8d2c           	movi.n	a13, 40
    2404:	f77d81        	l32r	a8, 1f8 (179c <deemph_filter_N>)
    2407:	bc2a           	add.n	a11, a12, a2
    2409:	10dcc2        	addmi	a12, a12, 0x1000
    240c:	48ccc2        	addi	a12, a12, 72
    240f:	0008e0        	callx8	a8
    2412:	2124a2        	l32i	a10, a4, 132
    2415:	4124b2        	l32i	a11, a4, 0x104
    2418:	05e8           	l32i.n	a14, a5, 0
    241a:	3124c2        	l32i	a12, a4, 196
    241d:	f77481        	l32r	a8, 1f0 (1884 <synth_3band>)
    2420:	2bded2        	addmi	a13, a14, 0x2b00
    2423:	58cdd2        	addi	a13, a13, 88
    2426:	16dee2        	addmi	a14, a14, 0x1600
    2429:	9ee8           	l32i.n	a14, a14, 36
    242b:	0008e0        	callx8	a8
    242e:	5124b2        	l32i	a11, a4, 0x144
    2431:	4821c2        	l32i	a12, a1, 0x120
    2434:	05a8           	l32i.n	a10, a5, 0
    2436:	3f21e2        	l32i	a14, a1, 252
    2439:	f76e81        	l32r	a8, 1f4 (16e8 <tx_upsample_by2>)
    243c:	24dad2        	addmi	a13, a10, 0x2400
    243f:	d8cdd2        	addi	a13, a13, -40
    2442:	16daa2        	addmi	a10, a10, 0x1600
    2445:	9aa8           	l32i.n	a10, a10, 36
    2447:	0008e0        	callx8	a8

0000244a <Voice_EC_Prcs_common+0x75e>:
    244a:	3e21e2        	l32i	a14, a1, 248
    244d:	3d21f2        	l32i	a15, a1, 244
    2450:	50cee2        	addi	a14, a14, 80
    2453:	50cff2        	addi	a15, a15, 80
    2456:	3d61f2        	s32i	a15, a1, 244
    2459:	3e61e2        	s32i	a14, a1, 248
    245c:	000d46        	j	2495 <Voice_EC_Prcs_common+0x7a9>

0000245f <Voice_EC_Prcs_common+0x773>:
    245f:	2124a2        	l32i	a10, a4, 132
    2462:	4124b2        	l32i	a11, a4, 0x104
    2465:	3124c2        	l32i	a12, a4, 196
    2468:	f76281        	l32r	a8, 1f0 (1884 <synth_3band>)
    246b:	2bded2        	addmi	a13, a14, 0x2b00
    246e:	58cdd2        	addi	a13, a13, 88
    2471:	16dee2        	addmi	a14, a14, 0x1600
    2474:	9ee8           	l32i.n	a14, a14, 36
    2476:	0008e0        	callx8	a8
    2479:	5124b2        	l32i	a11, a4, 0x144
    247c:	4821c2        	l32i	a12, a1, 0x120
    247f:	05a8           	l32i.n	a10, a5, 0
    2481:	3f21e2        	l32i	a14, a1, 252
    2484:	f75c81        	l32r	a8, 1f4 (16e8 <tx_upsample_by2>)
    2487:	24dad2        	addmi	a13, a10, 0x2400
    248a:	d8cdd2        	addi	a13, a13, -40
    248d:	16daa2        	addmi	a10, a10, 0x1600
    2490:	9aa8           	l32i.n	a10, a10, 36
    2492:	0008e0        	callx8	a8

00002495 <Voice_EC_Prcs_common+0x7a9>:
    2495:	4821a2        	l32i	a10, a1, 0x120
    2498:	05c8           	l32i.n	a12, a5, 0
    249a:	8d2c           	movi.n	a13, 40
    249c:	f75781        	l32r	a8, 1f8 (179c <deemph_filter_N>)
    249f:	bc2a           	add.n	a11, a12, a2
    24a1:	10dcc2        	addmi	a12, a12, 0x1000
    24a4:	40ccc2        	addi	a12, a12, 64
    24a7:	0008e0        	callx8	a8
    24aa:	4521f2        	l32i	a15, a1, 0x114
    24ad:	482182        	l32i	a8, a1, 0x120
    24b0:	472192        	l32i	a9, a1, 0x11c
    24b3:	4621a2        	l32i	a10, a1, 0x118
    24b6:	50c882        	addi	a8, a8, 80
    24b9:	50c992        	addi	a9, a9, 80
    24bc:	50caa2        	addi	a10, a10, 80
    24bf:	4661a2        	s32i	a10, a1, 0x118
    24c2:	476192        	s32i	a9, a1, 0x11c
    24c5:	486182        	s32i	a8, a1, 0x120
    24c8:	121f16        	beqz	a15, 25ed <Voice_EC_Prcs_common+0x901>
    24cb:	4321a2        	l32i	a10, a1, 0x10c
    24ce:	2a21c2        	l32i	a12, a1, 168
    24d1:	05b8           	l32i.n	a11, a5, 0
    24d3:	f73681        	l32r	a8, 1ac (1758 <emph_filter_N>)
    24d6:	8d2c           	movi.n	a13, 40
    24d8:	bb2a           	add.n	a11, a11, a2
    24da:	0008e0        	callx8	a8
    24dd:	4321c2        	l32i	a12, a1, 0x10c
    24e0:	3521d2        	l32i	a13, a1, 212
    24e3:	4921b2        	l32i	a11, a1, 0x124
    24e6:	2b21e2        	l32i	a14, a1, 172
    24e9:	f73181        	l32r	a8, 1b0 (1678 <tx_downsample_by2>)
    24ec:	742ba2        	l32i	a10, a11, 0x1d0
    24ef:	752bb2        	l32i	a11, a11, 0x1d4
    24f2:	0008e0        	callx8	a8
    24f5:	2c21b2        	l32i	a11, a1, 176
    24f8:	4921e2        	l32i	a14, a1, 0x124
    24fb:	f72e81        	l32r	a8, 1b4 (17d8 <analy_3band>)
    24fe:	05c8           	l32i.n	a12, a5, 0
    2500:	742ea2        	l32i	a10, a14, 0x1d0
    2503:	2e2ed2        	l32i	a13, a14, 184
    2506:	16dcc2        	addmi	a12, a12, 0x1600
    2509:	2cc8           	l32i.n	a12, a12, 8
    250b:	1e2ee2        	l32i	a14, a14, 120
    250e:	0008e0        	callx8	a8
    2511:	4921b2        	l32i	a11, a1, 0x124
    2514:	05a8           	l32i.n	a10, a5, 0
    2516:	f72881        	l32r	a8, 1b8 (1970 <conv_24_to_16>)
    2519:	ebb8           	l32i.n	a11, a11, 56
    251b:	16daa2        	addmi	a10, a10, 0x1600
    251e:	2aa8           	l32i.n	a10, a10, 8
    2520:	0008e0        	callx8	a8
    2523:	04ad           	mov.n	a10, a4
    2525:	2d21b2        	l32i	a11, a1, 180
    2528:	ac0c           	movi.n	a12, 10
    252a:	f72481        	l32r	a8, 1bc (d3c <subband_aec_block>)
    252d:	07dd           	mov.n	a13, a7
    252f:	1e0c           	movi.n	a14, 1
    2531:	0008e0        	callx8	a8
    2534:	04ad           	mov.n	a10, a4
    2536:	2e21b2        	l32i	a11, a1, 184
    2539:	ac0c           	movi.n	a12, 10
    253b:	f72081        	l32r	a8, 1bc (d3c <subband_aec_block>)
    253e:	07dd           	mov.n	a13, a7
    2540:	3e0c           	movi.n	a14, 3
    2542:	0008e0        	callx8	a8
    2545:	04ad           	mov.n	a10, a4
    2547:	2f21b2        	l32i	a11, a1, 188
    254a:	4c1c           	movi.n	a12, 20
    254c:	f71c81        	l32r	a8, 1bc (d3c <subband_aec_block>)
    254f:	07dd           	mov.n	a13, a7
    2551:	4e0c           	movi.n	a14, 4
    2553:	0008e0        	callx8	a8
    2556:	04ad           	mov.n	a10, a4
    2558:	3021b2        	l32i	a11, a1, 192
    255b:	f71981        	l32r	a8, 1c0 (fb0 <subband_aec_mid_channel>)
    255e:	07cd           	mov.n	a12, a7
    2560:	2d0c           	movi.n	a13, 2
    2562:	0008e0        	callx8	a8
    2565:	4921f2        	l32i	a15, a1, 0x124
    2568:	0e0c           	movi.n	a14, 0
    256a:	f71681        	l32r	a8, 1c4 (198c <gain_after_ec>)
    256d:	102fa2        	l32i	a10, a15, 64
    2570:	302fb2        	l32i	a11, a15, 192
    2573:	402fc2        	l32i	a12, a15, 0x100
    2576:	202fd2        	l32i	a13, a15, 128
    2579:	e69ff2        	l16si	a15, a15, 0x1cc
    257c:	0008e0        	callx8	a8
    257f:	4921e2        	l32i	a14, a1, 0x124
    2582:	3121d2        	l32i	a13, a1, 196
    2585:	f71a81        	l32r	a8, 1f0 (1884 <synth_3band>)
    2588:	102ea2        	l32i	a10, a14, 64
    258b:	302eb2        	l32i	a11, a14, 192
    258e:	202ec2        	l32i	a12, a14, 128
    2591:	762ee2        	l32i	a14, a14, 0x1d8
    2594:	0008e0        	callx8	a8
    2597:	4421c2        	l32i	a12, a1, 0x110
    259a:	3421d2        	l32i	a13, a1, 208
    259d:	4921b2        	l32i	a11, a1, 0x124
    25a0:	3221e2        	l32i	a14, a1, 200
    25a3:	f71481        	l32r	a8, 1f4 (16e8 <tx_upsample_by2>)
    25a6:	762ba2        	l32i	a10, a11, 0x1d8
    25a9:	402bb2        	l32i	a11, a11, 0x100
    25ac:	0008e0        	callx8	a8
    25af:	4921f2        	l32i	a15, a1, 0x124
    25b2:	ef9fc2        	l16si	a12, a15, 0x1de
    25b5:	ee9ff2        	l16si	a15, a15, 0x1dc
    25b8:	026c37        	bbci	a12, 3, 25be <Voice_EC_Prcs_common+0x8d2>
    25bb:	60f0f0        	neg	a15, a15

000025be <Voice_EC_Prcs_common+0x8d2>:
    25be:	11bfc0        	slli	a11, a15, 4
    25c1:	4421a2        	l32i	a10, a1, 0x110
    25c4:	8c2c           	movi.n	a12, 40
    25c6:	ff6be5        	call8	1c84 <vector_gain_Q9_15>
    25c9:	4421a2        	l32i	a10, a1, 0x110
    25cc:	3321c2        	l32i	a12, a1, 204
    25cf:	05b8           	l32i.n	a11, a5, 0
    25d1:	f70981        	l32r	a8, 1f8 (179c <deemph_filter_N>)
    25d4:	8d2c           	movi.n	a13, 40
    25d6:	bb2a           	add.n	a11, a11, a2
    25d8:	0008e0        	callx8	a8
    25db:	4421c2        	l32i	a12, a1, 0x110
    25de:	4321d2        	l32i	a13, a1, 0x10c
    25e1:	50ccc2        	addi	a12, a12, 80
    25e4:	50cdd2        	addi	a13, a13, 80
    25e7:	4361d2        	s32i	a13, a1, 0x10c
    25ea:	4461c2        	s32i	a12, a1, 0x110

000025ed <Voice_EC_Prcs_common+0x901>:
    25ed:	4221e2        	l32i	a14, a1, 0x108
    25f0:	ee0b           	addi.n	a14, a14, -1
    25f2:	4261e2        	s32i	a14, a1, 0x108
    25f5:	949e56        	bnez	a14, 1f42 <Voice_EC_Prcs_common+0x256>
    25f8:	41f8           	l32i.n	a15, a1, 16
    25fa:	f6d171        	l32r	a7, 140 (2000 <Voice_EC_Prcs_common+0x314>)
    25fd:	0d27f7        	blt	a7, a15, 260e <Voice_EC_Prcs_common+0x922>
    2600:	05a8           	l32i.n	a10, a5, 0
    2602:	f6fe81        	l32r	a8, 1fc (1af0 <adjust_bib_state>)
    2605:	23daa2        	addmi	a10, a10, 0x2300
    2608:	78caa2        	addi	a10, a10, 120
    260b:	0008e0        	callx8	a8

0000260e <Voice_EC_Prcs_common+0x922>:
    260e:	71c8           	l32i.n	a12, a1, 28
    2610:	0d27c7        	blt	a7, a12, 2621 <Voice_EC_Prcs_common+0x935>
    2613:	05a8           	l32i.n	a10, a5, 0
    2615:	f6f981        	l32r	a8, 1fc (1af0 <adjust_bib_state>)
    2618:	24daa2        	addmi	a10, a10, 0x2400
    261b:	d8caa2        	addi	a10, a10, -40
    261e:	0008e0        	callx8	a8

00002621 <Voice_EC_Prcs_common+0x935>:
    2621:	61c8           	l32i.n	a12, a1, 24
    2623:	0d27c7        	blt	a7, a12, 2634 <Voice_EC_Prcs_common+0x948>
    2626:	05a8           	l32i.n	a10, a5, 0
    2628:	f6f581        	l32r	a8, 1fc (1af0 <adjust_bib_state>)
    262b:	24daa2        	addmi	a10, a10, 0x2400
    262e:	98caa2        	addi	a10, a10, -104
    2631:	0008e0        	callx8	a8

00002634 <Voice_EC_Prcs_common+0x948>:
    2634:	4121c2        	l32i	a12, a1, 0x104
    2637:	422c66        	bnei	a12, 2, 267d <Voice_EC_Prcs_common+0x991>
    263a:	51d8           	l32i.n	a13, a1, 20
    263c:	0d27d7        	blt	a7, a13, 264d <Voice_EC_Prcs_common+0x961>
    263f:	05a8           	l32i.n	a10, a5, 0
    2641:	f6ee81        	l32r	a8, 1fc (1af0 <adjust_bib_state>)
    2644:	24daa2        	addmi	a10, a10, 0x2400
    2647:	b8caa2        	addi	a10, a10, -72
    264a:	0008e0        	callx8	a8

0000264d <Voice_EC_Prcs_common+0x961>:
    264d:	81c8           	l32i.n	a12, a1, 32
    264f:	0d27c7        	blt	a7, a12, 2660 <Voice_EC_Prcs_common+0x974>
    2652:	05a8           	l32i.n	a10, a5, 0
    2654:	f6ea81        	l32r	a8, 1fc (1af0 <adjust_bib_state>)
    2657:	24daa2        	addmi	a10, a10, 0x2400
    265a:	f8caa2        	addi	a10, a10, -8
    265d:	0008e0        	callx8	a8

00002660 <Voice_EC_Prcs_common+0x974>:
    2660:	05c8           	l32i.n	a12, a5, 0
    2662:	35dcc2        	addmi	a12, a12, 0x3500
    2665:	e8ccc2        	addi	a12, a12, -24
    2668:	7f1cc2        	l16ui	a12, a12, 254
    266b:	ec8c           	beqz.n	a12, 267d <Voice_EC_Prcs_common+0x991>
    266d:	f6e481        	l32r	a8, 200 (3658 <mag_calibration>)
    2670:	03ad           	mov.n	a10, a3
    2672:	0008e0        	callx8	a8
    2675:	090c           	movi.n	a9, 0
    2677:	136392        	s32i	a9, a3, 76
    267a:	146392        	s32i	a9, a3, 80
    267d:	05e8           	l32i.n	a14, a5, 0
    267f:	16dea2        	addmi	a10, a14, 0x1600
    2682:	029aa2        	l16si	a10, a10, 4
    2685:	4a2132        	l32i	a3, a1, 0x128
    2688:	666a87        	bbci	a10, 8, 26f2 <Voice_EC_Prcs_common+0xa06>
    268b:	35dec2        	addmi	a12, a14, 0x3500
    268e:	e8ccc2        	addi	a12, a12, -24
    2691:	7f1cc2        	l16ui	a12, a12, 254
    2694:	14deb2        	addmi	a11, a14, 0x1400
    2697:	14cbb2        	addi	a11, a11, 20
    269a:	eccc           	bnez.n	a12, 26ac <Voice_EC_Prcs_common+0x9c0>
    269c:	0bad           	mov.n	a10, a11
    269e:	f66081        	l32r	a8, 20 (5620 <xt_memcpy>)
    26a1:	78a0c2        	movi	a12, 120
    26a4:	03bd           	mov.n	a11, a3
    26a6:	0008e0        	callx8	a8
    26a9:	000c06        	j	26dd <Voice_EC_Prcs_common+0x9f1>

000026ac <Voice_EC_Prcs_common+0x9c0>:
    26ac:	78a0c2        	movi	a12, 120
    26af:	1ddea2        	addmi	a10, a14, 0x1d00
    26b2:	f65b81        	l32r	a8, 20 (5620 <xt_memcpy>)
    26b5:	58caa2        	addi	a10, a10, 88
    26b8:	2461a2        	s32i	a10, a1, 144
    26bb:	0008e0        	callx8	a8
    26be:	03bd           	mov.n	a11, a3
    26c0:	2421a2        	l32i	a10, a1, 144
    26c3:	78a0c2        	movi	a12, 120
    26c6:	f65681        	l32r	a8, 20 (5620 <xt_memcpy>)
    26c9:	01daa2        	addmi	a10, a10, 0x100
    26cc:	f0caa2        	addi	a10, a10, -16
    26cf:	0008e0        	callx8	a8
    26d2:	f6cc81        	l32r	a8, 204 (454c <NC_NOISE_EST_Prcs>)
    26d5:	2421b2        	l32i	a11, a1, 144
    26d8:	05a8           	l32i.n	a10, a5, 0
    26da:	0008e0        	callx8	a8

000026dd <Voice_EC_Prcs_common+0x9f1>:
    26dd:	03ad           	mov.n	a10, a3
    26df:	05c8           	l32i.n	a12, a5, 0
    26e1:	78a0d2        	movi	a13, 120
    26e4:	f6c581        	l32r	a8, 1f8 (179c <deemph_filter_N>)
    26e7:	bc2a           	add.n	a11, a12, a2
    26e9:	10dcc2        	addmi	a12, a12, 0x1000
    26ec:	48ccc2        	addi	a12, a12, 72
    26ef:	0008e0        	callx8	a8

000026f2 <Voice_EC_Prcs_common+0xa06>:
    26f2:	4521f2        	l32i	a15, a1, 0x114
    26f5:	9188           	l32i.n	a8, a1, 36
    26f7:	8f9c           	beqz.n	a15, 2713 <Voice_EC_Prcs_common+0xa27>
    26f9:	082787        	blt	a7, a8, 2705 <Voice_EC_Prcs_common+0xa19>
    26fc:	f6c081        	l32r	a8, 1fc (1af0 <adjust_bib_state>)
    26ff:	3521a2        	l32i	a10, a1, 212
    2702:	0008e0        	callx8	a8

00002705 <Voice_EC_Prcs_common+0xa19>:
    2705:	a198           	l32i.n	a9, a1, 40
    2707:	082797        	blt	a7, a9, 2713 <Voice_EC_Prcs_common+0xa27>
    270a:	f6bc81        	l32r	a8, 1fc (1af0 <adjust_bib_state>)
    270d:	3421a2        	l32i	a10, a1, 208
    2710:	0008e0        	callx8	a8
    2713:	05e8           	l32i.n	a14, a5, 0
    2715:	35de92        	addmi	a9, a14, 0x3500
    2718:	e8c992        	addi	a9, a9, -24
    271b:	7f1992        	l16ui	a9, a9, 254
    271e:	071916        	beqz	a9, 2793 <Voice_EC_Prcs_common+0xaa7>
    2721:	01d4e2        	addmi	a14, a4, 0x100
    2724:	22c422        	addi	a2, a4, 34
    2727:	400204        	ae_lp16f.i	aep0, a2, 0
    272a:	20cee2        	addi	a14, a14, 32
    272d:	600e04        	ae_lq32f.i	aeq0, a14, 0
    2730:	d4e23184be 	{ l32i	a11, a4, 96; ae_mulfq32sp16s.l	aeq0, aeq0, aep0 }
    2735:	54c1d2        	addi	a13, a1, 84
    2738:	620d04        	ae_sq32f.i	aeq0, a13, 0
    273b:	1521c2        	l32i	a12, a1, 84
    273e:	f6b281        	l32r	a8, 208 (122c <dt_detect>)
    2741:	04ad           	mov.n	a10, a4
    2743:	53bbc0        	max	a11, a11, a12
    2746:	0008e0        	callx8	a8
    2749:	4521f2        	l32i	a15, a1, 0x114
    274c:	cf8c           	beqz.n	a15, 275c <Voice_EC_Prcs_common+0xa70>
    274e:	4921b2        	l32i	a11, a1, 0x124
    2751:	f6ae81        	l32r	a8, 20c (1270 <dt_detect_FB>)
    2754:	4021a2        	l32i	a10, a1, 0x100
    2757:	7bb8           	l32i.n	a11, a11, 28
    2759:	0008e0        	callx8	a8

0000275c <Voice_EC_Prcs_common+0xa70>:
    275c:	05d8           	l32i.n	a13, a5, 0
    275e:	34cd           	ae_l16si.n	a12, a4, 2
    2760:	16ddd2        	addmi	a13, a13, 0x1600
    2763:	3ddd           	ae_l16si.n	a13, a13, 2
    2765:	09acd7        	bge	a12, a13, 2772 <Voice_EC_Prcs_common+0xa86>
    2768:	f68ce1        	l32r	a14, 198 (7fff0000 <_end+0x7ffe92e4>)
    276b:	360e34        	ae_cvtq48a32s	aeq0, a14
    276e:	0005c6        	j	2789 <Voice_EC_Prcs_common+0xa9d>

00002771 <Voice_EC_Prcs_common+0xa85>:
	...

00002772 <Voice_EC_Prcs_common+0xa86>:
    2772:	401204        	ae_lp16f.i	aep1, a2, 0
    2775:	01d482        	addmi	a8, a4, 0x100
    2778:	24c882        	addi	a8, a8, 36
    277b:	600804        	ae_lq32f.i	aeq0, a8, 0
    277e:	d4e60644fe 	{ addi	a15, a4, 100; ae_mulfq32sp16s.l	aeq0, aeq0, aep1 }
    2783:	604f04        	ae_lq32f.i	aeq1, a15, 0
    2786:	0110c4        	ae_maxq56s	aeq0, aeq0, aeq1

00002789 <Voice_EC_Prcs_common+0xa9d>:
    2789:	f96382849e 	{ addi	a9, a4, 40; ae_roundsp16q48asym	aep2, aeq0 }
    278e:	422904        	ae_sp16f.l.i	aep2, a9, 0
    2791:	05e8           	l32i.n	a14, a5, 0

00002793 <Voice_EC_Prcs_common+0xaa7>:
    2793:	440604        	ae_lp16x2f.i	aep0, a6, 0
    2796:	2a0c           	movi.n	a10, 2
    2798:	34de92        	addmi	a9, a14, 0x3400
    279b:	36de22        	addmi	a2, a14, 0x3600
    279e:	224b           	addi.n	a2, a2, 4
    27a0:	fc2481c99e 	{ addi	a9, a9, 28; ae_zeroq56	aeq0 }
    27a5:	08aa76        	loopgtz	a10, 27b1 <Voice_EC_Prcs_common+0xac5>
    27a8:	7f2982        	l32i	a8, a9, 0x1fc
    27ab:	fcc992        	addi	a9, a9, -4
    27ae:	816982        	s32i	a8, a9, 0x204

000027b1 <Voice_EC_Prcs_common+0xac5>:
    27b1:	c93c           	movi.n	a9, 60
    27b3:	f03d           	nop.n
    27b5:	04a976        	loopgtz	a9, 27bd <Voice_EC_Prcs_common+0xad1>

000027b8 <Voice_EC_Prcs_common+0xacc>:
    27b8:	e2002d861f 	{ ae_lp16x2f.iu	aep0, a6, 4; ae_mulaafp24s.hh.ll	aeq0, aep0, aep0 }

000027bd <Voice_EC_Prcs_common+0xad1>:
    27bd:	3581f4        	ae_sraiq56	aeq2, aeq0, 7
    27c0:	f69491        	l32r	a9, 210 (5277 <xtensa_fft256_32b+0x7>)
    27c3:	303994        	ae_cvtp24a16x2.ll	aep3, a9, a9
    27c6:	03b294        	ae_mulfq32sp16s.l	aeq2, aeq2, aep3
    27c9:	f61a81        	l32r	a8, 34 (5b4 <HIFI_sqrt>)
    27cc:	fd344000af 	{ movi	a10, 0; ae_roundsq32asym	aeq1, aeq2 }
    27d1:	369b04        	ae_trunca32q48	a11, aeq1
    27d4:	0008e0        	callx8	a8
    27d7:	364b34        	ae_cvtq48a32s	aeq1, a11
    27da:	fc24fe9a0f 	{ ae_slaasq56s	aeq0, aeq1, a10; nop }
    27df:	06c934        	ae_roundsq32asym	aeq3, aeq0
    27e2:	623244        	ae_sq32f.i	aeq3, a2, 16
    27e5:	f01d           	retw.n

000027e7 <Voice_EC_Prcs_common+0xafb>:
    27e7:	f60e81        	l32r	a8, 20 (5620 <xt_memcpy>)
    27ea:	06ad           	mov.n	a10, a6
    27ec:	78a0c2        	movi	a12, 120
    27ef:	0008e0        	callx8	a8
    27f2:	412192        	l32i	a9, a1, 0x104
    27f5:	0d2966        	bnei	a9, 2, 2806 <Voice_EC_Prcs_common+0xb1a>
    27f8:	02ad           	mov.n	a10, a2
    27fa:	f60981        	l32r	a8, 20 (5620 <xt_memcpy>)
    27fd:	2821b2        	l32i	a11, a1, 160
    2800:	78a0c2        	movi	a12, 120
    2803:	0008e0        	callx8	a8
    2806:	f01d           	retw.n

00002808 <Voice_PostEC120_Prcs>:
    2808:	004136        	entry	a1, 32
    280b:	f60651        	l32r	a5, 24 (6d14 <p_ecnr_mem>)
    280e:	002572        	l32i	a7, a5, 0
    2811:	fcc2c2        	addi	a12, a2, -4
    2814:	3ca082        	movi	a8, 60
    2817:	10d792        	addmi	a9, a7, 0x1000
    281a:	11d7b2        	addmi	a11, a7, 0x1100
    281d:	40cbb2        	addi	a11, a11, 64
    2820:	50c992        	addi	a9, a9, 80
    2823:	28d772        	addmi	a7, a7, 0x2800
    2826:	78c772        	addi	a7, a7, 120
    2829:	0ba876        	loopgtz	a8, 2838 <Voice_PostEC120_Prcs+0x30>
    282c:	448b14        	ae_lp16x2f.iu	aep0, a11, 4
    282f:	449c14        	ae_lp16x2f.iu	aep1, a12, 4
    2832:	461b04        	ae_sp16x2f.i	aep1, a11, 0
    2835:	468914        	ae_sp16x2f.iu	aep0, a9, 4

00002838 <Voice_PostEC120_Prcs+0x30>:
    2838:	05a8           	l32i.n	a10, a5, 0
    283a:	35da92        	addmi	a9, a10, 0x3500
    283d:	7319b2        	l16ui	a11, a9, 230
    2840:	13dad2        	addmi	a13, a10, 0x1300
    2843:	e8c992        	addi	a9, a9, -24
    2846:	3bec           	bnez.n	a11, 286d <Voice_PostEC120_Prcs+0x65>
    2848:	f67461        	l32r	a6, 218 (1234 <dt_detect+0x8>)
    284b:	f67281        	l32r	a8, 214 (4424 <NOISE_GEN_Prcs>)
    284e:	12dab2        	addmi	a11, a10, 0x1200
    2851:	34cbb2        	addi	a11, a11, 52
    2854:	10daa2        	addmi	a10, a10, 0x1000
    2857:	54caa2        	addi	a10, a10, 84
    285a:	0008e0        	callx8	a8
    285d:	0598           	l32i.n	a9, a5, 0
    285f:	d96a           	add.n	a13, a9, a6
    2861:	35d992        	addmi	a9, a9, 0x3500
    2864:	e8c992        	addi	a9, a9, -24
    2867:	7f19b2        	l16ui	a11, a9, 254
    286a:	000086        	j	2870 <Voice_PostEC120_Prcs+0x68>

0000286d <Voice_PostEC120_Prcs+0x65>:
    286d:	24cdd2        	addi	a13, a13, 36

00002870 <Voice_PostEC120_Prcs+0x68>:
    2870:	060c           	movi.n	a6, 0
    2872:	eb1b           	addi.n	a14, a11, 1
    2874:	01d7f2        	addmi	a15, a7, 0x100
    2877:	a41ff2        	l16ui	a15, a15, 0x148
    287a:	9eed           	ae_zext16	a14, a14
    287c:	7f59e2        	s16i	a14, a9, 254
    287f:	022eb6        	bltui	a14, 2, 2885 <Voice_PostEC120_Prcs+0x7d>
    2882:	7f5962        	s16i	a6, a9, 254

00002885 <Voice_PostEC120_Prcs+0x7d>:
    2885:	0f9c           	beqz.n	a15, 2899 <Voice_PostEC120_Prcs+0x91>
    2887:	03cd           	mov.n	a12, a3
    2889:	02bd           	mov.n	a11, a2
    288b:	f66481        	l32r	a8, 21c (1b10 <aec_post_prcs>)
    288e:	07ad           	mov.n	a10, a7
    2890:	1497e2        	l16si	a14, a7, 40
    2893:	0008e0        	callx8	a8
    2896:	000306        	j	28a6 <Voice_PostEC120_Prcs+0x9e>

00002899 <Voice_PostEC120_Prcs+0x91>:
    2899:	02bd           	mov.n	a11, a2
    289b:	f5e181        	l32r	a8, 20 (5620 <xt_memcpy>)
    289e:	03ad           	mov.n	a10, a3
    28a0:	78a0c2        	movi	a12, 120
    28a3:	0008e0        	callx8	a8

000028a6 <Voice_PostEC120_Prcs+0x9e>:
    28a6:	05a8           	l32i.n	a10, a5, 0
    28a8:	dd0c           	movi.n	a13, 13
    28aa:	36dab2        	addmi	a11, a10, 0x3600
    28ad:	2cdaf2        	addmi	a15, a10, 0x2c00
    28b0:	5c1ff2        	l16ui	a15, a15, 184
    28b3:	2b4b           	addi.n	a2, a11, 4
    28b5:	250be2        	l8ui	a14, a11, 37
    28b8:	11cfc0        	slli	a12, a15, 4
    28bb:	c0ccf0        	sub	a12, a12, a15
    28be:	cc6b           	addi.n	a12, a12, 6
    28c0:	d2ccd0        	quos	a12, a12, a13
    28c3:	8ccd           	ae_sext16	a12, a12
    28c5:	052ce7        	blt	a12, a14, 28ce <Voice_PostEC120_Prcs+0xc6>
    28c8:	119bb2        	l16si	a11, a11, 34
    28cb:	000b46        	j	28fc <Voice_PostEC120_Prcs+0xf4>

000028ce <Voice_PostEC120_Prcs+0xc6>:
    28ce:	240bf2        	l8ui	a15, a11, 36
    28d1:	109bd2        	l16si	a13, a11, 32
    28d4:	119bb2        	l16si	a11, a11, 34
    28d7:	042fc7        	blt	a15, a12, 28df <Voice_PostEC120_Prcs+0xd7>
    28da:	0dbd           	mov.n	a11, a13
    28dc:	000706        	j	28fc <Voice_PostEC120_Prcs+0xf4>

000028df <Voice_PostEC120_Prcs+0xd7>:
    28df:	c08cf0        	sub	a8, a12, a15
    28e2:	c0bbd0        	sub	a11, a11, a13
    28e5:	82bb80        	mull	a11, a11, a8
    28e8:	c08ef0        	sub	a8, a14, a15
    28eb:	11bb80        	slli	a11, a11, 8
    28ee:	d2bb80        	quos	a11, a11, a8
    28f1:	01dbb2        	addmi	a11, a11, 0x100
    28f4:	80cbb2        	addi	a11, a11, -128
    28f7:	21b8b0        	srai	a11, a11, 8
    28fa:	bdba           	add.n	a11, a13, a11
    28fc:	f60fd1        	l32r	a13, 138 (8000 <_end+0x12e4>)
    28ff:	36dac2        	addmi	a12, a10, 0x3600
    2902:	0ced           	mov.n	a14, a12
    2904:	601e74        	ae_lq32f.iu	aeq0, a14, 28
    2907:	60dc44        	ae_lq32f.iu	aeq3, a12, 16
    290a:	c0ddb0        	sub	a13, a13, a11
    290d:	303bd4        	ae_cvtp24a16x2.ll	aep3, a11, a13
    2910:	c1bbc4        	ae_mulzaafq32sp16s.lh	aeq3, aeq3, aep3, aeq0, aep3
    2913:	f5f981        	l32r	a8, f8 (4c80 <airoha_divide>)
    2916:	067834        	ae_roundsq32sym	aeq1, aeq3
    2919:	621c04        	ae_sq32f.i	aeq1, a12, 0
    291c:	0292b2        	l16si	a11, a2, 4
    291f:	608224        	ae_lq32f.i	aeq2, a2, 8
    2922:	442204        	ae_lp16x2f.i	aep2, a2, 0
    2925:	c12a64        	ae_mulzaafq32sp16s.lh	aeq1, aeq1, aep2, aeq2, aep2
    2928:	11bb00        	slli	a11, a11, 16
    292b:	061934        	ae_roundsq32asym	aeq0, aeq1
    292e:	368a04        	ae_trunca32q48	a10, aeq0
    2931:	22a9           	s32i.n	a10, a2, 8
    2933:	0008e0        	callx8	a8
    2936:	f601f1        	l32r	a15, 13c (6d18 <avc_vol>)
    2939:	368b34        	ae_cvtq48a32s	aeq2, a11
    293c:	fecac2        	addi	a12, a10, -2
    293f:	f600b1        	l32r	a11, 140 (2000 <Voice_EC_Prcs_common+0x314>)
    2942:	fc24feac4f 	{ ae_slaasq56s	aeq1, aeq2, a12; nop }
    2947:	fa6bb0058e 	{ l32i	a8, a5, 0; ae_roundsp16q48asym	aep4, aeq1 }
    294c:	364924        	ae_trunca16p24s.l	a9, aep4
    294f:	0392a2        	l16si	a10, a2, 6
    2952:	27d882        	addmi	a8, a8, 0x2700
    2955:	5399b0        	max	a9, a9, a11
    2958:	4399a0        	min	a9, a9, a10
    295b:	6f9d           	ae_s16i.n	a9, a15, 0
    295d:	746d           	ae_s16i.n	a6, a4, 2
    295f:	bc9882        	l16si	a8, a8, 0x178
    2962:	648d           	ae_s16i.n	a8, a4, 0
    2964:	2ffd           	ae_l16si.n	a15, a15, 0
    2966:	0254f2        	s16i	a15, a4, 4
    2969:	f01d           	retw.n

0000296b <Voice_PostEC120_Prcs+0x163>:
	...

0000296c <Voice_EC120_Prcs>:
    296c:	006136        	entry	a1, 48
    296f:	06fd           	mov.n	a15, a6
    2971:	05ed           	mov.n	a14, a5
    2973:	04dd           	mov.n	a13, a4
    2975:	03bd           	mov.n	a11, a3
    2977:	02ad           	mov.n	a10, a2
    2979:	0c0c           	movi.n	a12, 0
    297b:	080c           	movi.n	a8, 0
    297d:	0189           	s32i.n	a8, a1, 0
    297f:	ff36e5        	call8	1cec <Voice_EC_Prcs_common>
    2982:	f01d           	retw.n

00002984 <Voice_EC120_Inear_Prcs>:
    2984:	006136        	entry	a1, 48
    2987:	07fd           	mov.n	a15, a7
    2989:	06ed           	mov.n	a14, a6
    298b:	05dd           	mov.n	a13, a5
    298d:	04cd           	mov.n	a12, a4
    298f:	03bd           	mov.n	a11, a3
    2991:	02ad           	mov.n	a10, a2
    2993:	c188           	l32i.n	a8, a1, 48
    2995:	0189           	s32i.n	a8, a1, 0
    2997:	ff3565        	call8	1cec <Voice_EC_Prcs_common>
    299a:	f01d           	retw.n

0000299c <conv_16_to_24_x2>:
    299c:	004136        	entry	a1, 32
    299f:	f8c332        	addi	a3, a3, -8
    29a2:	fcc222        	addi	a2, a2, -4
    29a5:	05a476        	loopgtz	a4, 29ae <conv_16_to_24_x2+0x12>
    29a8:	448214        	ae_lp16x2f.iu	aep0, a2, 4
    29ab:	5e8314        	ae_sp24x2f.iu	aep0, a3, 8

000029ae <conv_16_to_24_x2+0x12>:
    29ae:	f01d           	retw.n

000029b0 <EC_REF_GAIN_READBACK>:
    29b0:	004136        	entry	a1, 32
    29b3:	f59c31        	l32r	a3, 24 (6d14 <p_ecnr_mem>)
    29b6:	0338           	l32i.n	a3, a3, 0
    29b8:	28d362        	addmi	a6, a3, 0x2800
    29bb:	15d332        	addmi	a3, a3, 0x1500
    29be:	342642        	l32i	a4, a6, 208
    29c1:	642692        	l32i	a9, a6, 0x190
    29c4:	542682        	l32i	a8, a6, 0x150
    29c7:	442652        	l32i	a5, a6, 0x110
    29ca:	319090        	srai	a9, a9, 16
    29cd:	318080        	srai	a8, a8, 16
    29d0:	315050        	srai	a5, a5, 16
    29d3:	314040        	srai	a4, a4, 16
    29d6:	624d           	ae_s16i.n	a4, a2, 0
    29d8:	725d           	ae_s16i.n	a5, a2, 2
    29da:	025282        	s16i	a8, a2, 4
    29dd:	035292        	s16i	a9, a2, 6
    29e0:	7f9332        	l16si	a3, a3, 254
    29e3:	040c           	movi.n	a4, 0
    29e5:	252366        	bnei	a3, 2, 2a0e <EC_REF_GAIN_READBACK+0x5e>
    29e8:	742682        	l32i	a8, a6, 0x1d0
    29eb:	a42642        	l32i	a4, a6, 0x290
    29ee:	942652        	l32i	a5, a6, 0x250
    29f1:	842672        	l32i	a7, a6, 0x210
    29f4:	314040        	srai	a4, a4, 16
    29f7:	315050        	srai	a5, a5, 16
    29fa:	317070        	srai	a7, a7, 16
    29fd:	318080        	srai	a8, a8, 16
    2a00:	045282        	s16i	a8, a2, 8
    2a03:	055272        	s16i	a7, a2, 10
    2a06:	065252        	s16i	a5, a2, 12

00002a09 <EC_REF_GAIN_READBACK+0x59>:
    2a09:	075242        	s16i	a4, a2, 14
    2a0c:	f01d           	retw.n

00002a0e <EC_REF_GAIN_READBACK+0x5e>:
    2a0e:	045242        	s16i	a4, a2, 8
    2a11:	055242        	s16i	a4, a2, 10
    2a14:	065242        	s16i	a4, a2, 12
    2a17:	fffb86        	j	2a09 <EC_REF_GAIN_READBACK+0x59>

00002a1a <EC_REF_GAIN_READBACK+0x6a>:
	...

00002a1c <EC_Inear_REF_GAIN_READBACK>:
    2a1c:	004136        	entry	a1, 32
    2a1f:	f58131        	l32r	a3, 24 (6d14 <p_ecnr_mem>)
    2a22:	0338           	l32i.n	a3, a3, 0
    2a24:	34d332        	addmi	a3, a3, 0x3400
    2a27:	8a2332        	l32i	a3, a3, 0x228
    2a2a:	d22352        	l32i	a5, a3, 0x348
    2a2d:	e22342        	l32i	a4, a3, 0x388
    2a30:	315050        	srai	a5, a5, 16
    2a33:	314040        	srai	a4, a4, 16
    2a36:	724d           	ae_s16i.n	a4, a2, 2
    2a38:	625d           	ae_s16i.n	a5, a2, 0
    2a3a:	f22342        	l32i	a4, a3, 0x3c8
    2a3d:	02d332        	addmi	a3, a3, 0x200
    2a40:	822332        	l32i	a3, a3, 0x208
    2a43:	314040        	srai	a4, a4, 16
    2a46:	025242        	s16i	a4, a2, 4
    2a49:	313030        	srai	a3, a3, 16
    2a4c:	035232        	s16i	a3, a2, 6
    2a4f:	f01d           	retw.n

00002a51 <EC_Inear_REF_GAIN_READBACK+0x35>:
    2a51:	000000                                        ...

00002a54 <EC_PreLim_Coef_READBACK>:
    2a54:	004136        	entry	a1, 32
    2a57:	f57361        	l32r	a6, 24 (6d14 <p_ecnr_mem>)
    2a5a:	024d           	mov.n	a4, a2
    2a5c:	0668           	l32i.n	a6, a6, 0
    2a5e:	390c           	movi.n	a9, 3
    2a60:	03a032        	movi	a3, 3
    2a63:	23d652        	addmi	a5, a6, 0x2300
    2a66:	88c552        	addi	a5, a5, -120
    2a69:	0aa376        	loopgtz	a3, 2a77 <EC_PreLim_Coef_READBACK+0x23>
    2a6c:	7f2582        	l32i	a8, a5, 0x1fc
    2a6f:	554b           	addi.n	a5, a5, 4
    2a71:	318080        	srai	a8, a8, 16
    2a74:	09c484        	ae_s16ip	a8, a4, 2

00002a77 <EC_PreLim_Coef_READBACK+0x23>:
    2a77:	426b           	addi.n	a4, a2, 6
    2a79:	23d652        	addmi	a5, a6, 0x2300
    2a7c:	e4c552        	addi	a5, a5, -28
    2a7f:	22cb           	addi.n	a2, a2, 12
    2a81:	0aa976        	loopgtz	a9, 2a8f <EC_PreLim_Coef_READBACK+0x3b>
    2a84:	7f2582        	l32i	a8, a5, 0x1fc
    2a87:	554b           	addi.n	a5, a5, 4
    2a89:	318080        	srai	a8, a8, 16
    2a8c:	09c484        	ae_s16ip	a8, a4, 2
    2a8f:	390c           	movi.n	a9, 3
    2a91:	23d642        	addmi	a4, a6, 0x2300
    2a94:	fcc442        	addi	a4, a4, -4
    2a97:	0aa976        	loopgtz	a9, 2aa5 <EC_PreLim_Coef_READBACK+0x51>
    2a9a:	7f2482        	l32i	a8, a4, 0x1fc
    2a9d:	444b           	addi.n	a4, a4, 4
    2a9f:	318080        	srai	a8, a8, 16
    2aa2:	09c284        	ae_s16ip	a8, a2, 2
    2aa5:	f01d           	retw.n

00002aa7 <EC_PreLim_Coef_READBACK+0x53>:
	...

00002aa8 <DaulMIC_power_Coef_READBACK>:
    2aa8:	004136        	entry	a1, 32
    2aab:	f55e21        	l32r	a2, 24 (6d14 <p_ecnr_mem>)
    2aae:	0228           	l32i.n	a2, a2, 0
    2ab0:	25d222        	addmi	a2, a2, 0x2500
    2ab3:	769222        	l16si	a2, a2, 236
    2ab6:	f01d           	retw.n

00002ab8 <PostEC120_Info>:
    2ab8:	004136        	entry	a1, 32
    2abb:	f55ab1        	l32r	a11, 24 (6d14 <p_ecnr_mem>)
    2abe:	f5a061        	l32r	a6, 140 (2000 <Voice_EC_Prcs_common+0x314>)
    2ac1:	0a0c           	movi.n	a10, 0
    2ac3:	0b88           	l32i.n	a8, a11, 0
    2ac5:	72ad           	ae_s16i.n	a10, a2, 2
    2ac7:	025262        	s16i	a6, a2, 4
    2aca:	27d832        	addmi	a3, a8, 0x2700
    2acd:	35d852        	addmi	a5, a8, 0x3500
    2ad0:	28d872        	addmi	a7, a8, 0x2800
    2ad3:	78c762        	addi	a6, a7, 120
    2ad6:	e8c542        	addi	a4, a5, -24
    2ad9:	731552        	l16ui	a5, a5, 230
    2adc:	bc9332        	l16si	a3, a3, 0x178
    2adf:	623d           	ae_s16i.n	a3, a2, 0
    2ae1:	161566        	bnei	a5, 1, 2afb <PostEC120_Info+0x43>
    2ae4:	509792        	l16si	a9, a7, 160
    2ae7:	2ad8c2        	addmi	a12, a8, 0x2a00
    2aea:	219790        	srai	a9, a9, 7
    2aed:	c54c92        	s8i	a9, a12, 197
    2af0:	0b58           	l32i.n	a5, a11, 0
    2af2:	35d552        	addmi	a5, a5, 0x3500
    2af5:	e8c542        	addi	a4, a5, -24
    2af8:	731552        	l16ui	a5, a5, 230

00002afb <PostEC120_Info+0x43>:
    2afb:	02d622        	addmi	a2, a6, 0x200
    2afe:	b51b           	addi.n	a11, a5, 1
    2b00:	4d0222        	l8ui	a2, a2, 77
    2b03:	9bbd           	ae_zext16	a11, a11
    2b05:	7f54b2        	s16i	a11, a4, 254
    2b08:	022bb6        	bltui	a11, 2, 2b0e <PostEC120_Info+0x56>
    2b0b:	7f54a2        	s16i	a10, a4, 254

00002b0e <PostEC120_Info+0x56>:
    2b0e:	f01d           	retw.n

00002b10 <find_phase>:
    2b10:	004136        	entry	a1, 32
    2b13:	345030        	extui	a5, a3, 0, 4
    2b16:	75cc           	bnez.n	a5, 2b21 <find_phase+0x11>
    2b18:	600204        	ae_lq32f.i	aeq0, a2, 0
    2b1b:	064b34        	ae_movq56	aeq1, aeq0
    2b1e:	000ac6        	j	2b4d <find_phase+0x3d>

00002b21 <find_phase+0x11>:
    2b21:	081566        	bnei	a5, 1, 2b2d <find_phase+0x1d>
    2b24:	600214        	ae_lq32f.i	aeq0, a2, 4
    2b27:	064b34        	ae_movq56	aeq1, aeq0
    2b2a:	0007c6        	j	2b4d <find_phase+0x3d>

00002b2d <find_phase+0x1d>:
    2b2d:	082566        	bnei	a5, 2, 2b39 <find_phase+0x29>
    2b30:	600224        	ae_lq32f.i	aeq0, a2, 8
    2b33:	064b34        	ae_movq56	aeq1, aeq0
    2b36:	0004c6        	j	2b4d <find_phase+0x3d>

00002b39 <find_phase+0x29>:
    2b39:	243566        	bnei	a5, 3, 2b61 <find_phase+0x51>
    2b3c:	604214        	ae_lq32f.i	aeq1, a2, 4
    2b3f:	600204        	ae_lq32f.i	aeq0, a2, 0
    2b42:	0001c6        	j	2b4d <find_phase+0x3d>

00002b45 <find_phase+0x35>:
    2b45:	604224        	ae_lq32f.i	aeq1, a2, 8
    2b48:	fd14bd021f 	{ ae_lq32f.i	aeq0, a2, 4; ae_negq56	aeq1, aeq1 }

00002b4d <find_phase+0x3d>:
    2b4d:	0109c4        	ae_addq56	aeq0, aeq0, aeq1
    2b50:	350074        	ae_sraiq56	aeq0, aeq0, 1
    2b53:	026347        	bbci	a3, 4, 2b59 <find_phase+0x49>
    2b56:	060824        	ae_negq56	aeq0, aeq0

00002b59 <find_phase+0x49>:
    2b59:	064934        	ae_roundsq32asym	aeq1, aeq0
    2b5c:	369204        	ae_trunca32q48	a2, aeq1
    2b5f:	f01d           	retw.n

00002b61 <find_phase+0x51>:
    2b61:	0a4566        	bnei	a5, 4, 2b6f <find_phase+0x5f>
    2b64:	604214        	ae_lq32f.i	aeq1, a2, 4
    2b67:	fd14bd020f 	{ ae_lq32f.i	aeq0, a2, 0; ae_negq56	aeq1, aeq1 }
    2b6c:	fff746        	j	2b4d <find_phase+0x3d>

00002b6f <find_phase+0x5f>:
    2b6f:	085566        	bnei	a5, 5, 2b7b <find_phase+0x6b>
    2b72:	604224        	ae_lq32f.i	aeq1, a2, 8
    2b75:	600204        	ae_lq32f.i	aeq0, a2, 0
    2b78:	fff446        	j	2b4d <find_phase+0x3d>

00002b7b <find_phase+0x6b>:
    2b7b:	0a6566        	bnei	a5, 6, 2b89 <find_phase+0x79>
    2b7e:	604224        	ae_lq32f.i	aeq1, a2, 8
    2b81:	fd14bd020f 	{ ae_lq32f.i	aeq0, a2, 0; ae_negq56	aeq1, aeq1 }
    2b86:	fff0c6        	j	2b4d <find_phase+0x3d>

00002b89 <find_phase+0x79>:
    2b89:	b87566        	bnei	a5, 7, 2b45 <find_phase+0x35>
    2b8c:	604224        	ae_lq32f.i	aeq1, a2, 8
    2b8f:	600214        	ae_lq32f.i	aeq0, a2, 4
    2b92:	ffedc6        	j	2b4d <find_phase+0x3d>

00002b95 <find_phase+0x85>:
    2b95:	000000                                        ...

00002b98 <memset_S16>:
    2b98:	004136        	entry	a1, 32
    2b9b:	f03d           	nop.n
    2b9d:	02a476        	loopgtz	a4, 2ba3 <memset_S16+0xb>
    2ba0:	09c234        	ae_s16ip	a3, a2, 2

00002ba3 <memset_S16+0xb>:
    2ba3:	f01d           	retw.n

00002ba5 <memset_S16+0xd>:
    2ba5:	000000                                        ...

00002ba8 <beam2_setup>:
    2ba8:	004136        	entry	a1, 32
    2bab:	0c5232        	s16i	a3, a2, 24
    2bae:	20c2a2        	addi	a10, a2, 32
    2bb1:	f51fb1        	l32r	a11, 30 (1000 <subband_aec_mid_channel+0x50>)
    2bb4:	cc0c           	movi.n	a12, 12
    2bb6:	f59b81        	l32r	a8, 224 (2b98 <memset_S16>)
    2bb9:	f59991        	l32r	a9, 220 (10000000 <_end+0xfff92e4>)
    2bbc:	2299           	s32i.n	a9, a2, 8
    2bbe:	0008e0        	callx8	a8
    2bc1:	f01d           	retw.n

00002bc3 <beam2_setup+0x1b>:
	...

00002bc4 <cal_vad_thrd_x1>:
    2bc4:	004136        	entry	a1, 32
    2bc7:	f59851        	l32r	a5, 228 (69bc <immediate_num+0x1c>)
    2bca:	445304        	ae_lp16x2f.i	aep5, a3, 0
    2bcd:	443204        	ae_lp16x2f.i	aep3, a2, 0
    2bd0:	dead2d250f 	{ ae_lp16x2f.i	aep2, a5, 0; ae_selp24.hh	aep4, aep3, aep5 }
    2bd5:	f08afe35ff 	{ ae_selp24.ll	aep3, aep3, aep5; ae_mulzaafp24s.hh.ll	aeq1, aep2, aep4 }
    2bda:	042304        	ae_mulzaafp24s.hh.ll	aeq0, aep2, aep3
    2bdd:	1b9f14        	ae_roundsp16q48asym	aep1, aeq1
    2be0:	f863b2140f 	{ ae_sp16f.l.i	aep1, a4, 0; ae_roundsp16q48asym	aep0, aeq0 }
    2be5:	420414        	ae_sp16f.l.i	aep0, a4, 2
    2be8:	f01d           	retw.n

00002bea <cal_vad_thrd_x1+0x26>:
	...

00002bec <CH13_cb_setup>:
    2bec:	004136        	entry	a1, 32
    2bef:	14c282        	addi	a8, a2, 20
    2bf2:	6cc232        	addi	a3, a2, 108
    2bf5:	d239           	s32i.n	a3, a2, 52
    2bf7:	3289           	s32i.n	a8, a2, 12
    2bf9:	f01d           	retw.n

00002bfb <CH13_cb_setup+0xf>:
	...

00002bfc <TWO_MIC_WB_init>:
    2bfc:	004136        	entry	a1, 32
    2bff:	f50941        	l32r	a4, 24 (6d14 <p_ecnr_mem>)
    2c02:	0b0c           	movi.n	a11, 0
    2c04:	0468           	l32i.n	a6, a4, 0
    2c06:	0ca2c2        	movi	a12, 0x20c
    2c09:	f52d81        	l32r	a8, c0 (5608 <xt_memset>)
    2c0c:	24d632        	addmi	a3, a6, 0x2400
    2c0f:	60c332        	addi	a3, a3, 96
    2c12:	28d662        	addmi	a6, a6, 0x2800
    2c15:	78c662        	addi	a6, a6, 120
    2c18:	03ad           	mov.n	a10, a3
    2c1a:	0008e0        	callx8	a8
    2c1d:	a3eb           	addi.n	a10, a3, 14
    2c1f:	5c0c           	movi.n	a12, 5
    2c21:	f4ff81        	l32r	a8, 20 (5620 <xt_memcpy>)
    2c24:	01d2b2        	addmi	a11, a2, 0x100
    2c27:	b4cbb2        	addi	a11, a11, -76
    2c2a:	0008e0        	callx8	a8
    2c2d:	4c0c           	movi.n	a12, 4
    2c2f:	f4fc81        	l32r	a8, 20 (5620 <xt_memcpy>)
    2c32:	01d352        	addmi	a5, a3, 0x100
    2c35:	5f1272        	l16ui	a7, a2, 190
    2c38:	01d2b2        	addmi	a11, a2, 0x100
    2c3b:	c0cbb2        	addi	a11, a11, -64
    2c3e:	725372        	s16i	a7, a3, 228
    2c41:	acc552        	addi	a5, a5, -84
    2c44:	c0a072        	movi	a7, 192
    2c47:	05ad           	mov.n	a10, a5
    2c49:	0008e0        	callx8	a8
    2c4c:	8c0c           	movi.n	a12, 8
    2c4e:	f4f481        	l32r	a8, 20 (5620 <xt_memcpy>)
    2c51:	01d342        	addmi	a4, a3, 0x100
    2c54:	01d2b2        	addmi	a11, a2, 0x100
    2c57:	c8cbb2        	addi	a11, a11, -56
    2c5a:	bcc442        	addi	a4, a4, -68
    2c5d:	04ad           	mov.n	a10, a4
    2c5f:	0008e0        	callx8	a8
    2c62:	f4ef81        	l32r	a8, 20 (5620 <xt_memcpy>)
    2c65:	6f12a2        	l16ui	a10, a2, 222
    2c68:	701292        	l16ui	a9, a2, 224
    2c6b:	04d3c2        	addmi	a12, a3, 0x400
    2c6e:	6c12b2        	l16ui	a11, a2, 216
    2c71:	6d12d2        	l16ui	a13, a2, 218
    2c74:	2b53d2        	s16i	a13, a3, 86
    2c77:	0a5cb2        	s16i	a11, a12, 20
    2c7a:	065392        	s16i	a9, a3, 12
    2c7d:	6c0c           	movi.n	a12, 6
    2c7f:	6653a2        	s16i	a10, a3, 204
    2c82:	02d2b2        	addmi	a11, a2, 0x200
    2c85:	a4cbb2        	addi	a11, a11, -92
    2c88:	03ad           	mov.n	a10, a3
    2c8a:	0008e0        	callx8	a8
    2c8d:	f568a1        	l32r	a10, 230 (e390000 <_end+0xe3892e4>)
    2c90:	f56981        	l32r	a8, 234 (3eb80000 <_end+0x3eb792e4>)
    2c93:	f512b1        	l32r	a11, dc (200000 <_end+0x1f92e4>)
    2c96:	40c392        	addi	a9, a3, 64
    2c99:	f564c1        	l32r	a12, 22c (40000000 <_end+0x3fff92e4>)
    2c9c:	f569d1        	l32r	a13, 240 (6000 <_DYNAMIC+0x984>)
    2c9f:	f567e1        	l32r	a14, 23c (d330000 <_end+0xd3292e4>)
    2ca2:	f565f1        	l32r	a15, 238 (18000000 <_end+0x17ff92e4>)
    2ca5:	5f63f2        	s32i	a15, a3, 0x17c
    2ca8:	5e63e2        	s32i	a14, a3, 0x178
    2cab:	2a53d2        	s16i	a13, a3, 84
    2cae:	4463c2        	s32i	a12, a3, 0x110
    2cb1:	4b63c2        	s32i	a12, a3, 0x12c
    2cb4:	5463c2        	s32i	a12, a3, 0x150
    2cb7:	5b63c2        	s32i	a12, a3, 0x16c
    2cba:	196392        	s32i	a9, a3, 100
    2cbd:	1663b2        	s32i	a11, a3, 88
    2cc0:	386382        	s32i	a8, a3, 224
    2cc3:	1863a2        	s32i	a10, a3, 96
    2cc6:	f55781        	l32r	a8, 224 (2b98 <memset_S16>)
    2cc9:	01d3a2        	addmi	a10, a3, 0x100
    2ccc:	8ccab2        	addi	a11, a10, -116
    2ccf:	a4ca92        	addi	a9, a10, -92
    2cd2:	256392        	s32i	a9, a3, 148
    2cd5:	1f63b2        	s32i	a11, a3, 124
    2cd8:	14caa2        	addi	a10, a10, 20
    2cdb:	f55ac1        	l32r	a12, 244 (1770 <emph_filter_N+0x18>)
    2cde:	f55ab1        	l32r	a11, 248 (4000 <beam2_mid_band+0x250>)
    2ce1:	c753c2        	s16i	a12, a3, 0x18e
    2ce4:	cc0c           	movi.n	a12, 12
    2ce6:	0008e0        	callx8	a8
    2ce9:	7126a2        	l32i	a10, a6, 0x1c4
    2cec:	812682        	l32i	a8, a6, 0x204
    2cef:	412692        	l32i	a9, a6, 0x104
    2cf2:	3126b2        	l32i	a11, a6, 196
    2cf5:	6126c2        	l32i	a12, a6, 0x184
    2cf8:	2126d2        	l32i	a13, a6, 132
    2cfb:	9126e2        	l32i	a14, a6, 0x244
    2cfe:	5126f2        	l32i	a15, a6, 0x144
    2d01:	b063f2        	s32i	a15, a3, 0x2c0
    2d04:	af63e2        	s32i	a14, a3, 0x2bc
    2d07:	6a63d2        	s32i	a13, a3, 0x1a8
    2d0a:	6963c2        	s32i	a12, a3, 0x1a4
    2d0d:	d363b2        	s32i	a11, a3, 0x34c
    2d10:	8d6392        	s32i	a9, a3, 0x234
    2d13:	8c6382        	s32i	a8, a3, 0x230
    2d16:	d263a2        	s32i	a10, a3, 0x348
    2d19:	f54c81        	l32r	a8, 24c (2ba8 <beam2_setup>)
    2d1c:	2a0c           	movi.n	a10, 2
    2d1e:	6f9292        	l16si	a9, a2, 222
    2d21:	02b8           	l32i.n	a11, a2, 0
    2d23:	d163b2        	s32i	a11, a3, 0x344
    2d26:	d299a0        	quos	a9, a9, a10
    2d29:	3ccbc2        	addi	a12, a11, 60
    2d2c:	28cbd2        	addi	a13, a11, 40
    2d2f:	6863d2        	s32i	a13, a3, 0x1a0
    2d32:	8b63c2        	s32i	a12, a3, 0x22c
    2d35:	685392        	s16i	a9, a3, 208
    2d38:	02d3a2        	addmi	a10, a3, 0x200
    2d3b:	50cbb2        	addi	a11, a11, 80
    2d3e:	d4caa2        	addi	a10, a10, -44
    2d41:	ae63b2        	s32i	a11, a3, 0x2b8
    2d44:	7292b2        	l16si	a11, a2, 228
    2d47:	0008e0        	callx8	a8
    2d4a:	7492b2        	l16si	a11, a2, 232
    2d4d:	f53f81        	l32r	a8, 24c (2ba8 <beam2_setup>)
    2d50:	02d3a2        	addmi	a10, a3, 0x200
    2d53:	60caa2        	addi	a10, a10, 96
    2d56:	0008e0        	callx8	a8
    2d59:	7592b2        	l16si	a11, a2, 234
    2d5c:	f53c81        	l32r	a8, 24c (2ba8 <beam2_setup>)
    2d5f:	03d3a2        	addmi	a10, a3, 0x300
    2d62:	eccaa2        	addi	a10, a10, -20
    2d65:	0008e0        	callx8	a8
    2d68:	cc0c           	movi.n	a12, 12
    2d6a:	f52e81        	l32r	a8, 224 (2b98 <memset_S16>)
    2d6d:	f52c91        	l32r	a9, 220 (10000000 <_end+0xfff92e4>)
    2d70:	03d3b2        	addmi	a11, a3, 0x300
    2d73:	7392a2        	l16si	a10, a2, 230
    2d76:	535ba2        	s16i	a10, a11, 166
    2d79:	ea6392        	s32i	a9, a3, 0x3a8
    2d7c:	f4adb1        	l32r	a11, 30 (1000 <subband_aec_mid_channel+0x50>)
    2d7f:	04d3a2        	addmi	a10, a3, 0x400
    2d82:	bccaa2        	addi	a10, a10, -68
    2d85:	0008e0        	callx8	a8
    2d88:	f53281        	l32r	a8, 250 (2bec <CH13_cb_setup>)
    2d8b:	02d3a2        	addmi	a10, a3, 0x200
    2d8e:	a0caa2        	addi	a10, a10, -96
    2d91:	0008e0        	callx8	a8
    2d94:	f52f81        	l32r	a8, 250 (2bec <CH13_cb_setup>)
    2d97:	02d3a2        	addmi	a10, a3, 0x200
    2d9a:	2ccaa2        	addi	a10, a10, 44
    2d9d:	0008e0        	callx8	a8
    2da0:	b4a062        	movi	a6, 180
    2da3:	f52b81        	l32r	a8, 250 (2bec <CH13_cb_setup>)
    2da6:	03d3a2        	addmi	a10, a3, 0x300
    2da9:	b8caa2        	addi	a10, a10, -72
    2dac:	0008e0        	callx8	a8
    2daf:	04bd           	mov.n	a11, a4
    2db1:	05ad           	mov.n	a10, a5
    2db3:	c36a           	add.n	a12, a3, a6
    2db5:	f52781        	l32r	a8, 254 (2bc4 <cal_vad_thrd_x1>)
    2db8:	03d3d2        	addmi	a13, a3, 0x300
    2dbb:	04d392        	addmi	a9, a3, 0x400
    2dbe:	d4c992        	addi	a9, a9, -44
    2dc1:	5ccdd2        	addi	a13, a13, 92
    2dc4:	d463d2        	s32i	a13, a3, 0x350
    2dc7:	e76392        	s32i	a9, a3, 0x39c
    2dca:	0008e0        	callx8	a8
    2dcd:	b37a           	add.n	a11, a3, a7
    2dcf:	01d3c2        	addmi	a12, a3, 0x100
    2dd2:	01d3a2        	addmi	a10, a3, 0x100
    2dd5:	f51f81        	l32r	a8, 254 (2bc4 <cal_vad_thrd_x1>)
    2dd8:	b0caa2        	addi	a10, a10, -80
    2ddb:	b8ccc2        	addi	a12, a12, -72
    2dde:	0008e0        	callx8	a8
    2de1:	f49041        	l32r	a4, 24 (6d14 <p_ecnr_mem>)
    2de4:	8c0c           	movi.n	a12, 8
    2de6:	02d3a2        	addmi	a10, a3, 0x200
    2de9:	02d2b2        	addmi	a11, a2, 0x200
    2dec:	f48d81        	l32r	a8, 20 (5620 <xt_memcpy>)
    2def:	94cbb2        	addi	a11, a11, -108
    2df2:	90caa2        	addi	a10, a10, -112
    2df5:	0008e0        	callx8	a8
    2df8:	0498           	l32i.n	a9, a4, 0
    2dfa:	14d992        	addmi	a9, a9, 0x1400
    2dfd:	822992        	l32i	a9, a9, 0x208
    2e00:	1e6392        	s32i	a9, a3, 120
    2e03:	f01d           	retw.n

00002e05 <TWO_MIC_WB_init+0x209>:
    2e05:	000000                                        ...

00002e08 <take_max_four>:
    2e08:	004136        	entry	a1, 32
    2e0b:	228d           	ae_l16si.n	a8, a2, 0
    2e0d:	3f23a6        	blti	a3, 2, 2e50 <take_max_four+0x48>
    2e10:	f51291        	l32r	a9, 258 (ffff8000 <_end+0xffff12e4>)
    2e13:	f51161        	l32r	a6, 258 (ffff8000 <_end+0xffff12e4>)
    2e16:	f51071        	l32r	a7, 258 (ffff8000 <_end+0xffff12e4>)
    2e19:	222b           	addi.n	a2, a2, 2
    2e1b:	430b           	addi.n	a4, a3, -1
    2e1d:	2ca476        	loopgtz	a4, 2e4d <take_max_four+0x45>
    2e20:	223d           	ae_l16si.n	a3, a2, 0
    2e22:	222b           	addi.n	a2, a2, 2
    2e24:	0aa837        	bge	a8, a3, 2e32 <take_max_four+0x2a>
    2e27:	069d           	mov.n	a9, a6
    2e29:	076d           	mov.n	a6, a7
    2e2b:	087d           	mov.n	a7, a8
    2e2d:	038d           	mov.n	a8, a3
    2e2f:	000606        	j	2e4b <take_max_four+0x43>

00002e32 <take_max_four+0x2a>:
    2e32:	08a737        	bge	a7, a3, 2e3e <take_max_four+0x36>
    2e35:	069d           	mov.n	a9, a6
    2e37:	076d           	mov.n	a6, a7
    2e39:	037d           	mov.n	a7, a3
    2e3b:	000306        	j	2e4b <take_max_four+0x43>

00002e3e <take_max_four+0x36>:
    2e3e:	06a637        	bge	a6, a3, 2e48 <take_max_four+0x40>
    2e41:	069d           	mov.n	a9, a6
    2e43:	036d           	mov.n	a6, a3
    2e45:	000086        	j	2e4b <take_max_four+0x43>

00002e48 <take_max_four+0x40>:
    2e48:	539390        	max	a9, a3, a9
    2e4b:	f03d           	nop.n

00002e4d <take_max_four+0x45>:
    2e4d:	000206        	j	2e59 <take_max_four+0x51>

00002e50 <take_max_four+0x48>:
    2e50:	f50261        	l32r	a6, 258 (ffff8000 <_end+0xffff12e4>)
    2e53:	f50191        	l32r	a9, 258 (ffff8000 <_end+0xffff12e4>)
    2e56:	f50071        	l32r	a7, 258 (ffff8000 <_end+0xffff12e4>)

00002e59 <take_max_four+0x51>:
    2e59:	469a           	add.n	a4, a6, a9
    2e5b:	287a           	add.n	a2, a8, a7
    2e5d:	224a           	add.n	a2, a2, a4
    2e5f:	112220        	slli	a2, a2, 14
    2e62:	f01d           	retw.n

00002e64 <prelim_filter>:
    2e64:	012136        	entry	a1, 144
    2e67:	01ad           	mov.n	a10, a1
    2e69:	0b0c           	movi.n	a11, 0
    2e6b:	6c0c           	movi.n	a12, 6
    2e6d:	f139           	s32i.n	a3, a1, 60
    2e6f:	f49481        	l32r	a8, c0 (5608 <xt_memset>)
    2e72:	78c192        	addi	a9, a1, 120
    2e75:	126122        	s32i	a2, a1, 72
    2e78:	02fd           	mov.n	a15, a2
    2e7a:	04dd           	mov.n	a13, a4
    2e7c:	e1d9           	s32i.n	a13, a1, 56
    2e7e:	692f72        	l32i	a7, a15, 0x1a4
    2e81:	192f62        	l32i	a6, a15, 100
    2e84:	24cf52        	addi	a5, a15, 36
    2e87:	40c242        	addi	a4, a2, 64
    2e8a:	f3f640        	wur.ae_cbegin0	a4
    2e8d:	46c222        	addi	a2, a2, 70
    2e90:	f3f720        	wur.ae_cend0	a2
    2e93:	682f42        	l32i	a4, a15, 0x1a0
    2e96:	6a2f22        	l32i	a2, a15, 0x1a8
    2e99:	22cff2        	addi	a15, a15, 34
    2e9c:	1061f2        	s32i	a15, a1, 64
    2e9f:	400f04        	ae_lp16f.i	aep0, a15, 0
    2ea2:	5609b4        	ae_sp24x2s.i	aep0, a9, -40
    2ea5:	0008e0        	callx8	a8
    2ea8:	3e0c           	movi.n	a14, 3
    2eaa:	0aa092        	movi	a9, 10
    2ead:	fec732        	addi	a3, a7, -2
    2eb0:	fcc1b2        	addi	a11, a1, -4
    2eb3:	fec2c2        	addi	a12, a2, -2
    2eb6:	78c1d2        	addi	a13, a1, 120
    2eb9:	541db4        	ae_lp24x2.i	aep1, a13, -40
    2ebc:	0d61c2        	s32i	a12, a1, 52
    2ebf:	1161b2        	s32i	a11, a1, 68
    2ec2:	0ec122        	addi	a2, a1, 14
    2ec5:	02a072        	movi	a7, 2
    2ec8:	fe2494c19f 	{ s32i.n	a9, a1, 48; ae_zeroq56	aeq2 }
    2ecd:	fc147f217f 	{ ae_movq56	aeq1, aeq2; ae_movq56	aeq0, aeq2 }

00002ed2 <prelim_filter+0x6e>:
    2ed2:	402314        	ae_lp16f.i	aep2, a3, 2
    2ed5:	fc24e2a67f 	{ ae_sp16f.l.c	aep2, a6, a7; nop }
    2eda:	60c504        	ae_lq32f.i	aeq3, a5, 0
    2edd:	0cae76        	loopgtz	a14, 2eed <prelim_filter+0x89>
    2ee0:	fc24d9367f 	{ ae_lp16f.c	aep3, a6, a7; nop }
    2ee5:	0330d4        	ae_mulafq32sp16s.l	aeq0, aeq3, aep3
    2ee8:	e26ffd751f 	{ ae_lq32f.iu	aeq3, a5, 4; ae_mulafp24s.ll	aeq1, aep3, aep3 }

00002eed <prelim_filter+0x89>:
    2eed:	f48281        	l32r	a8, f8 (4c80 <airoha_divide>)
    2ef0:	f4d6c1        	l32r	a12, 248 (4000 <beam2_mid_band+0x250>)
    2ef3:	fe2c3b106f 	{ ae_sraiq56	aeq1, aeq1, 2; ae_cvtq48p24s.l	aeq2, aep1 }
    2ef8:	fd2c7a80df 	{ ae_slliq56	aeq3, aeq0, 1; ae_roundsq32asym	aeq1, aeq1 }
    2efd:	0138c4        	ae_addq56	aeq0, aeq3, aeq0
    2f00:	f832f2140f 	{ ae_sp16f.l.i	aep1, a4, 0; ae_subq56	aeq0, aeq2, aeq0 }
    2f05:	fa63bf1bef 	{ ae_trunca32q48	a11, aeq1; ae_roundsp16q48asym	aep4, aeq0 }
    2f0a:	004ad4        	ae_cvta32p24.l	a10, aep4
    2f0d:	424214        	ae_sp16f.l.i	aep4, a2, 2
    2f10:	53bbc0        	max	a11, a11, a12
    2f13:	21a6a0        	srai	a10, a10, 6
    2f16:	0008e0        	callx8	a8
    2f19:	d1c8           	l32i.n	a12, a1, 52
    2f1b:	f0c552        	addi	a5, a5, -16
    2f1e:	442b           	addi.n	a4, a4, 2
    2f20:	222b           	addi.n	a2, a2, 2
    2f22:	332b           	addi.n	a3, a3, 2
    2f24:	3d0c           	movi.n	a13, 3
    2f26:	c198           	l32i.n	a9, a1, 48
    2f28:	f4cde1        	l32r	a14, 25c (ccccc <_end+0xc5fb0>)
    2f2b:	360b34        	ae_cvtq48a32s	aeq0, a11
    2f2e:	fc24fe8acf 	{ ae_slaasq56s	aeq3, aeq0, a10; nop }
    2f33:	398ee4        	ae_movpa24x2	aep0, a14, a14
    2f36:	1121a2        	l32i	a10, a1, 68
    2f39:	3e0c           	movi.n	a14, 3
    2f3b:	fe248ff99e 	{ addi	a9, a9, -1; ae_zeroq56	aeq2 }
    2f40:	f87fd4c19f 	{ s32i.n	a9, a1, 48; ae_roundsp24q48asym	aep1, aeq3 }
    2f45:	1aad76        	loopgtz	a13, 2f63 <prelim_filter+0xff>
    2f48:	fc24d9567f 	{ ae_lp16f.c	aep5, a6, a7; nop }
    2f4d:	605514        	ae_lq32f.iu	aeq1, a5, 4
    2f50:	011d34        	ae_mulafp24s.ll	aeq1, aep1, aep5
    2f53:	fc2c7d7a1f 	{ ae_lq32f.iu	aeq3, a10, 4; ae_roundsq32asym	aeq0, aeq1 }
    2f58:	088b24        	ae_mulafq32sp24s.l	aeq3, aeq0, aep0
    2f5b:	fd3c79850f 	{ ae_sq32f.i	aeq0, a5, 0; ae_roundsq32asym	aeq1, aeq3 }
    2f60:	621a04        	ae_sq32f.i	aeq1, a10, 0
    2f63:	f8c552        	addi	a5, a5, -8
    2f66:	fd146c9c1f 	{ ae_lp16f.iu	aep1, a12, 2; ae_movq56	aeq1, aeq2 }
    2f6b:	fc1454d1cf 	{ s32i.n	a12, a1, 52; ae_movq56	aeq0, aeq2 }
    2f70:	f5e956        	bnez	a9, 2ed2 <prelim_filter+0x6e>
    2f73:	af0c           	movi.n	a15, 10
    2f75:	1221a2        	l32i	a10, a1, 72
    2f78:	1021b2        	l32i	a11, a1, 64
    2f7b:	10c192        	addi	a9, a1, 16
    2f7e:	196a62        	s32i	a6, a10, 100
    2f81:	421b04        	ae_sp16f.l.i	aep1, a11, 0
    2f84:	401904        	ae_lp16f.i	aep1, a9, 0
    2f87:	682aa2        	l32i	a10, a10, 0x1a0
    2f8a:	f4b5b1        	l32r	a11, 260 (393e4b <_end+0x38d12f>)
    2f8d:	fd24a20bbf 	{ ae_movpa24x2	aep0, a11, a11; ae_zeroq56	aeq1 }
    2f92:	fc0c4feaae 	{ addi	a10, a10, -2; ae_movq56	aeq0, aeq1 }
    2f97:	15af76        	loopgtz	a15, 2fb0 <prelim_filter+0x14c>

00002f9a <prelim_filter+0x136>:
    2f9a:	eb21ec9a1f 	{ ae_lp16f.iu	aep1, a10, 2; ae_mulfp24s.ll	aeq3, aep0, aep1 }
    2f9f:	018174        	ae_mulfp24s.ll	aeq2, aep0, aep1
    2fa2:	1bbd74        	ae_roundsp24q48asym	aep7, aeq3
    2fa5:	017f34        	ae_mulafp24s.ll	aeq1, aep7, aep7
    2fa8:	1bad64        	ae_roundsp24q48asym	aep6, aeq2
    2fab:	e2dbac991f 	{ ae_lp16f.iu	aep1, a9, 2; ae_mulafp24s.ll	aeq0, aep6, aep6 }

00002fb0 <prelim_filter+0x14c>:
    2fb0:	f44f91        	l32r	a9, ec (10000 <_end+0x92e4>)
    2fb3:	1221f2        	l32i	a15, a1, 72
    2fb6:	f49ca1        	l32r	a10, 228 (69bc <immediate_num+0x1c>)
    2fb9:	fc244781be 	{ addi	a11, a1, 120; ae_roundsq32asym	aeq0, aeq0 }
    2fbe:	630bd4        	ae_sq56s.i	aeq0, a11, -24
    2fc1:	443a04        	ae_lp16x2f.i	aep3, a10, 0
    2fc4:	5ccf82        	addi	a8, a15, 92
    2fc7:	fe2c7d680f 	{ ae_lq32f.i	aeq3, a8, 0; ae_roundsq32asym	aeq2, aeq1 }
    2fcc:	0f6ec58ffe 	{ addi	a15, a15, 88; ae_mulzaafq32sp16s.lh	aeq3, aeq2, aep3, aeq3, aep3 }
    2fd1:	fa32fd2f0f 	{ ae_lq32f.i	aeq1, a15, 0; ae_subq56	aeq2, aeq2, aeq0 }

00002fd6 <prelim_filter+0x172>:
    2fd6:	0c6cfec93f 	{ ae_cvtq48a32s	aeq0, a9; ae_mulzaafq32sp16s.lh	aeq1, aeq0, aep3, aeq1, aep3 }
    2fdb:	09a054        	ae_ltq56s	b5, aeq0, aeq2
    2fde:	06b934        	ae_roundsq32asym	aeq2, aeq3
    2fe1:	fc2c79a80f 	{ ae_sq32f.i	aeq2, a8, 0; ae_roundsq32asym	aeq0, aeq1 }
    2fe6:	620f04        	ae_sq32f.i	aeq0, a15, 0
    2fe9:	610bd4        	ae_lq56.i	aeq0, a11, -24
    2fec:	2c0576        	bf	b5, 301c <prelim_filter+0x1b8>
    2fef:	580c           	movi.n	a8, 5
    2ff1:	1221a2        	l32i	a10, a1, 72
    2ff4:	10c192        	addi	a9, a1, 16
    2ff7:	80a0b2        	movi	a11, 128
    2ffa:	682aa2        	l32i	a10, a10, 0x1a0
    2ffd:	399bb4        	ae_movpa24x2	aep1, a11, a11
    3000:	440904        	ae_lp16x2f.i	aep0, a9, 0
    3003:	fccaa2        	addi	a10, a10, -4
    3006:	12a876        	loopgtz	a8, 301c <prelim_filter+0x1b8>
    3009:	cf00edea1f 	{ ae_lp16x2f.iu	aep6, a10, 4; ae_sraip24	aep7, aep0, 1 }
    300e:	0f60e4        	ae_sraip24	aep6, aep6, 1
    3011:	07e754        	ae_addsp24s	aep5, aep6, aep7
    3014:	da352d891f 	{ ae_lp16x2f.iu	aep0, a9, 4; ae_addsp24s	aep4, aep5, aep1 }
    3019:	4649f4        	ae_sp16x2f.i	aep4, a9, -4

0000301c <prelim_filter+0x1b8>:
    301c:	1221a2        	l32i	a10, a1, 72
    301f:	f42ac1        	l32r	a12, c8 (155555 <_end+0x14e839>)
    3022:	39fcc4        	ae_movpa24x2	aep7, a12, a12
    3025:	4ccab2        	addi	a11, a10, 76
    3028:	60cb04        	ae_lq32f.i	aeq3, a11, 0
    302b:	08fb24        	ae_mulafq32sp24s.l	aeq3, aeq0, aep7
    302e:	f43181        	l32r	a8, f4 (4d88 <airoha_divide_limit_1>)
    3031:	06b934        	ae_roundsq32asym	aeq2, aeq3
    3034:	622b04        	ae_sq32f.i	aeq2, a11, 0
    3037:	0b9ac2        	l16si	a12, a10, 22
    303a:	162ab2        	l32i	a11, a10, 88
    303d:	172aa2        	l32i	a10, a10, 92
    3040:	11cc20        	slli	a12, a12, 14
    3043:	53bbc0        	max	a11, a11, a12
    3046:	0008e0        	callx8	a8
    3049:	1221b2        	l32i	a11, a1, 72
    304c:	f486c1        	l32r	a12, 264 (175e97 <_end+0x16f17b>)
    304f:	379aa4        	ae_truncp24a32x2	aep1, a10, a10
    3052:	f8e6820bde 	{ addi	a13, a11, 32; ae_roundsp16asym	aep0, aep1 }
    3057:	420d04        	ae_sp16f.l.i	aep0, a13, 0
    305a:	398cc4        	ae_movpa24x2	aep0, a12, a12
    305d:	ad0c           	movi.n	a13, 10
    305f:	692b72        	l32i	a7, a11, 0x1a4
    3062:	50cbb2        	addi	a11, a11, 80
    3065:	60cb04        	ae_lq32f.i	aeq3, a11, 0
    3068:	401704        	ae_lp16f.i	aep1, a7, 0
    306b:	f03d           	nop.n
    306d:	0aad76        	loopgtz	a13, 307b <prelim_filter+0x217>
    3070:	010174        	ae_mulfp24s.ll	aeq0, aep0, aep1
    3073:	1b8d24        	ae_roundsp24q48asym	aep2, aeq0
    3076:	e34bec971f 	{ ae_lp16f.iu	aep1, a7, 2; ae_mulafp24s.ll	aeq3, aep2, aep2 }

0000307b <prelim_filter+0x217>:
    307b:	067934        	ae_roundsq32asym	aeq1, aeq3
    307e:	621b04        	ae_sq32f.i	aeq1, a11, 0
    3081:	1221b2        	l32i	a11, a1, 72
    3084:	f47981        	l32r	a8, 268 (2b10 <find_phase>)
    3087:	01ad           	mov.n	a10, a1
    3089:	2b9bb2        	l16si	a11, a11, 86
    308c:	0008e0        	callx8	a8
    308f:	1221d2        	l32i	a13, a1, 72
    3092:	162dd2        	l32i	a13, a13, 88
    3095:	0a5d           	mov.n	a5, a10
    3097:	ffa3c2        	movi	a12, 0x3ff
    309a:	64acd7        	bge	a12, a13, 3102 <prelim_filter+0x29e>
    309d:	1221a2        	l32i	a10, a1, 72
    30a0:	089ab2        	l16si	a11, a10, 16
    30a3:	109aa2        	l16si	a10, a10, 32
    30a6:	c0aab0        	sub	a10, a10, a11
    30a9:	061ae6        	bgei	a10, 1, 30b3 <prelim_filter+0x24f>
    30ac:	f40a91        	l32r	a9, d4 (7fff <_end+0x12e3>)
    30af:	001286        	j	30fd <prelim_filter+0x299>

000030b2 <prelim_filter+0x24e>:
	...

000030b3 <prelim_filter+0x24f>:
    30b3:	1221b2        	l32i	a11, a1, 72
    30b6:	099bb2        	l16si	a11, a11, 18
    30b9:	1eaab7        	bge	a10, a11, 30db <prelim_filter+0x277>
    30bc:	f40e81        	l32r	a8, f4 (4d88 <airoha_divide_limit_1>)
    30bf:	0008e0        	callx8	a8
    30c2:	37aaa4        	ae_truncp24a32x2	aep2, a10, a10
    30c5:	10c192        	addi	a9, a1, 16
    30c8:	401904        	ae_lp16f.i	aep1, a9, 0
    30cb:	aa0c           	movi.n	a10, 10
    30cd:	0aaa76        	loopgtz	a10, 30db <prelim_filter+0x277>
    30d0:	019274        	ae_mulfp24s.ll	aeq2, aep1, aep2
    30d3:	f973ec991f 	{ ae_lp16f.iu	aep1, a9, 2; ae_roundsp16q48asym	aep3, aeq2 }
    30d8:	4239f4        	ae_sp16f.l.i	aep3, a9, -2

000030db <prelim_filter+0x277>:
    30db:	1221a2        	l32i	a10, a1, 72
    30de:	05a0b2        	movi	a11, 5
    30e1:	682aa2        	l32i	a10, a10, 0x1a0
    30e4:	10c192        	addi	a9, a1, 16
    30e7:	440904        	ae_lp16x2f.i	aep0, a9, 0
    30ea:	fccaa2        	addi	a10, a10, -4
    30ed:	0aab76        	loopgtz	a11, 30fb <prelim_filter+0x297>
    30f0:	44da14        	ae_lp16x2f.iu	aep5, a10, 4
    30f3:	de16ad891f 	{ ae_lp16x2f.iu	aep0, a9, 4; ae_subsp24s	aep4, aep5, aep0 }
    30f8:	464a04        	ae_sp16x2f.i	aep4, a10, 0

000030fb <prelim_filter+0x297>:
    30fb:	090c           	movi.n	a9, 0

000030fd <prelim_filter+0x299>:
    30fd:	1a0c           	movi.n	a10, 1
    30ff:	000106        	j	3107 <prelim_filter+0x2a3>

00003102 <prelim_filter+0x29e>:
    3102:	0a0c           	movi.n	a10, 0
    3104:	f3f491        	l32r	a9, d4 (7fff <_end+0x12e3>)

00003107 <prelim_filter+0x2a3>:
    3107:	f1c8           	l32i.n	a12, a1, 60
    3109:	e1b8           	l32i.n	a11, a1, 56
    310b:	0c59           	s32i.n	a5, a12, 0
    310d:	1ca9           	s32i.n	a10, a12, 4
    310f:	6b9d           	ae_s16i.n	a9, a11, 0
    3111:	f01d           	retw.n

00003113 <prelim_filter+0x2af>:
	...

00003114 <prelim_filter3>:
    3114:	00a136        	entry	a1, 80
    3117:	6139           	s32i.n	a3, a1, 24
    3119:	1159           	s32i.n	a5, a1, 4
    311b:	2b9292        	l16si	a9, a2, 86
    311e:	183466        	bnei	a4, 3, 313a <prelim_filter3+0x26>
    3121:	8d22a2        	l32i	a10, a2, 0x234
    3124:	250c           	movi.n	a5, 2
    3126:	ab0c           	movi.n	a11, 10
    3128:	f3c2c1        	l32r	a12, 30 (1000 <subband_aec_mid_channel+0x50>)
    312b:	21d590        	srai	a13, a9, 5
    312e:	01c9           	s32i.n	a12, a1, 0
    3130:	71b9           	s32i.n	a11, a1, 28
    3132:	41d9           	s32i.n	a13, a1, 16
    3134:	8c2292        	l32i	a9, a2, 0x230
    3137:	000546        	j	3150 <prelim_filter3+0x3c>

0000313a <prelim_filter3+0x26>:
    313a:	b022a2        	l32i	a10, a2, 0x2c0
    313d:	4e1c           	movi.n	a14, 20
    313f:	215a90        	srai	a5, a9, 10
    3142:	f441f1        	l32r	a15, 248 (4000 <beam2_mid_band+0x250>)
    3145:	af2292        	l32i	a9, a2, 0x2bc
    3148:	01f9           	s32i.n	a15, a1, 0
    314a:	71e9           	s32i.n	a14, a1, 28
    314c:	4159           	s32i.n	a5, a1, 16
    314e:	550c           	movi.n	a5, 5

00003150 <prelim_filter3+0x3c>:
    3150:	6128           	l32i.n	a2, a1, 24
    3152:	51a9           	s32i.n	a10, a1, 20
    3154:	7188           	l32i.n	a8, a1, 28
    3156:	10c242        	addi	a4, a2, 16
    3159:	16c2b2        	addi	a11, a2, 22
    315c:	31b9           	s32i.n	a11, a1, 12
    315e:	f3f7b0        	wur.ae_cend0	a11
    3161:	f3f640        	wur.ae_cbegin0	a4
    3164:	401b04        	ae_lp16f.i	aep1, a11, 0
    3167:	0248           	l32i.n	a4, a2, 0
    3169:	224b           	addi.n	a2, a2, 4
    316b:	2129           	s32i.n	a2, a1, 8
    316d:	0218e6        	bgei	a8, 1, 3173 <prelim_filter3+0x5f>
    3170:	002406        	j	3204 <prelim_filter3+0xf0>
    3173:	fec972        	addi	a7, a9, -2
    3176:	feca32        	addi	a3, a10, -2
    3179:	260c           	movi.n	a6, 2
    317b:	90a890        	addx2	a10, a8, a9
    317e:	fe2480038f 	{ movi	a8, 3; ae_zeroq56	aeq2 }
    3183:	fd144feaae 	{ addi	a10, a10, -2; ae_movq56	aeq1, aeq2 }
    3188:	fc145481af 	{ s32i.n	a10, a1, 32; ae_movq56	aeq0, aeq2 }

0000318d <prelim_filter3+0x79>:
    318d:	400714        	ae_lp16f.i	aep0, a7, 2
    3190:	fc24e2846f 	{ ae_sp16f.l.c	aep0, a4, a6; nop }
    3195:	60c204        	ae_lq32f.i	aeq3, a2, 0
    3198:	0ca876        	loopgtz	a8, 31a8 <prelim_filter3+0x94>
    319b:	fc24d9246f 	{ ae_lp16f.c	aep2, a4, a6; nop }
    31a0:	0320d4        	ae_mulafq32sp16s.l	aeq0, aeq3, aep2
    31a3:	e24bfd721f 	{ ae_lq32f.iu	aeq3, a2, 4; ae_mulafp24s.ll	aeq1, aep2, aep2 }

000031a8 <prelim_filter3+0x94>:
    31a8:	f3d381        	l32r	a8, f4 (4d88 <airoha_divide_limit_1>)
    31ab:	400500        	ssr	a5
    31ae:	fe2c3a80df 	{ ae_slliq56	aeq3, aeq0, 1; ae_cvtq48p24s.l	aeq2, aep1 }
    31b3:	0138c4        	ae_addq56	aeq0, aeq3, aeq0
    31b6:	012cc4        	ae_subq56	aeq0, aeq2, aeq0
    31b9:	f963fb106f 	{ ae_sraiq56	aeq1, aeq1, 2; ae_roundsp16q48asym	aep3, aeq0 }
    31be:	003ad4        	ae_cvta32p24.l	a10, aep3
    31c1:	065934        	ae_roundsq32asym	aeq1, aeq1
    31c4:	369b04        	ae_trunca32q48	a11, aeq1
    31c7:	b1a0a0        	sra	a10, a10
    31ca:	0008e0        	callx8	a8
    31cd:	379aa4        	ae_truncp24a32x2	aep1, a10, a10
    31d0:	60d2d4        	ae_lq32f.iu	aeq3, a2, -12
    31d3:	81c8           	l32i.n	a12, a1, 32
    31d5:	380c           	movi.n	a8, 3
    31d7:	fe2480277e 	{ addi	a7, a7, 2; ae_zeroq56	aeq2 }
    31dc:	fd144003bf 	{ movi	a11, 3; ae_movq56	aeq1, aeq2 }
    31e1:	f03d           	nop.n
    31e3:	f03d           	nop.n
    31e5:	10ab76        	loopgtz	a11, 31f9 <prelim_filter3+0xe5>
    31e8:	fc24d9446f 	{ ae_lp16f.c	aep4, a4, a6; nop }
    31ed:	019c34        	ae_mulafp24s.ll	aeq3, aep1, aep4
    31f0:	063934        	ae_roundsq32asym	aeq0, aeq3
    31f3:	620204        	ae_sq32f.i	aeq0, a2, 0
    31f6:	60d214        	ae_lq32f.iu	aeq3, a2, 4
    31f9:	f4c222        	addi	a2, a2, -12
    31fc:	fc146c931f 	{ ae_lp16f.iu	aep1, a3, 2; ae_movq56	aeq0, aeq2 }
    3201:	8897c7        	bne	a7, a12, 318d <prelim_filter3+0x79>

00003204 <prelim_filter3+0xf0>:
    3204:	61f8           	l32i.n	a15, a1, 24
    3206:	31e8           	l32i.n	a14, a1, 12
    3208:	71d8           	l32i.n	a13, a1, 28
    320a:	006f42        	s32i	a4, a15, 0
    320d:	421e04        	ae_sp16f.l.i	aep1, a14, 0
    3210:	fc24b051ee 	{ l32i	a14, a1, 20; ae_zeroq56	aeq0 }
    3215:	630154        	ae_sq56s.i	aeq0, a1, 40
    3218:	21d1d0        	srai	a13, a13, 1
    321b:	209ee0        	or	a9, a14, a14
    321e:	440e04        	ae_lp16x2f.i	aep0, a14, 0
    3221:	08ad76        	loopgtz	a13, 322d <prelim_filter3+0x119>
    3224:	050004        	ae_mulaafp24s.hh.ll	aeq0, aep0, aep0
    3227:	448914        	ae_lp16x2f.iu	aep0, a9, 4
    322a:	630154        	ae_sq56s.i	aeq0, a1, 40

0000322d <prelim_filter3+0x119>:
    322d:	0128           	l32i.n	a2, a1, 0
    322f:	1138           	l32i.n	a3, a1, 4
    3231:	21a8           	l32i.n	a10, a1, 8
    3233:	618154        	ae_lq56.i	aeq2, a1, 40
    3236:	f40c81        	l32r	a8, 268 (2b10 <find_phase>)
    3239:	fe347041be 	{ l32i	a11, a1, 16; ae_roundsq32asym	aeq2, aeq2 }
    323e:	632154        	ae_sq56s.i	aeq2, a1, 40
    3241:	0008e0        	callx8	a8
    3244:	61c154        	ae_lq56.i	aeq3, a1, 40
    3247:	36b904        	ae_trunca32q48	a9, aeq3
    324a:	03a9           	s32i.n	a10, a3, 0
    324c:	729290        	salt	a9, a2, a9
    324f:	1399           	s32i.n	a9, a3, 4
    3251:	f01d           	retw.n

00003253 <prelim_filter3+0x13f>:
	...

00003254 <cm_short_vad>:
    3254:	004136        	entry	a1, 32
    3257:	234d           	ae_l16si.n	a4, a3, 0
    3259:	335d           	ae_l16si.n	a5, a3, 2
    325b:	029382        	l16si	a8, a3, 4
    325e:	04a427        	bge	a4, a2, 3266 <cm_short_vad+0x12>
    3261:	430c           	movi.n	a3, 4
    3263:	000706        	j	3283 <cm_short_vad+0x2f>

00003266 <cm_short_vad+0x12>:
    3266:	04a527        	bge	a5, a2, 326e <cm_short_vad+0x1a>
    3269:	230c           	movi.n	a3, 2
    326b:	000506        	j	3283 <cm_short_vad+0x2f>

0000326e <cm_short_vad+0x1a>:
    326e:	04a827        	bge	a8, a2, 3276 <cm_short_vad+0x22>
    3271:	030c           	movi.n	a3, 0
    3273:	000306        	j	3283 <cm_short_vad+0x2f>

00003276 <cm_short_vad+0x22>:
    3276:	0393a2        	l16si	a10, a3, 6
    3279:	e97c           	movi.n	a9, -2
    327b:	c37c           	movi.n	a3, -4
    327d:	72aa20        	salt	a10, a10, a2
    3280:	9339a0        	movnez	a3, a9, a10
    3283:	032d           	mov.n	a2, a3
    3285:	f01d           	retw.n

00003287 <cm_short_vad+0x33>:
	...

00003288 <cali_module_short>:
    3288:	004136        	entry	a1, 32
    328b:	f36681        	l32r	a8, 24 (6d14 <p_ecnr_mem>)
    328e:	0888           	l32i.n	a8, a8, 0
    3290:	15d882        	addmi	a8, a8, 0x1500
    3293:	819882        	l16si	a8, a8, 0x102
    3296:	040c           	movi.n	a4, 0
    3298:	0228e6        	bgei	a8, 2, 329e <cali_module_short+0x16>
    329b:	003586        	j	3375 <cali_module_short+0xed>
    329e:	1398           	l32i.n	a9, a3, 4
    32a0:	03a8           	l32i.n	a10, a3, 0
    32a2:	0219e6        	bgei	a9, 1, 32a8 <cali_module_short+0x20>
    32a5:	003a06        	j	3391 <cali_module_short+0x109>
    32a8:	5792c2        	l16si	a12, a2, 174
    32ab:	6892b2        	l16si	a11, a2, 208
    32ae:	31a0a0        	srai	a10, a10, 16
    32b1:	3caca7        	bge	a12, a10, 32f1 <cali_module_short+0x69>
    32b4:	5692c2        	l16si	a12, a2, 172
    32b7:	6692e2        	l16si	a14, a2, 204
    32ba:	6e92d2        	l16si	a13, a2, 220
    32bd:	06aca7        	bge	a12, a10, 32c7 <cali_module_short+0x3f>
    32c0:	bb4b           	addi.n	a11, a11, 4
    32c2:	4a0c           	movi.n	a10, 4
    32c4:	0000c6        	j	32cb <cali_module_short+0x43>

000032c7 <cali_module_short+0x3f>:
    32c7:	bb3b           	addi.n	a11, a11, 3
    32c9:	2a0c           	movi.n	a10, 2

000032cb <cali_module_short+0x43>:
    32cb:	43bbe0        	min	a11, a11, a14
    32ce:	382de6        	bgei	a13, 2, 330a <cali_module_short+0x82>
    32d1:	6d9292        	l16si	a9, a2, 218
    32d4:	991b           	addi.n	a9, a9, 1
    32d6:	899d           	ae_sext16	a9, a9
    32d8:	6d5292        	s16i	a9, a2, 218
    32db:	02c9e6        	bgei	a9, 32, 32e1 <cali_module_short+0x59>
    32de:	002d06        	j	3396 <cali_module_short+0x10e>
    32e1:	fed9e2        	addmi	a14, a9, 0xfffffe00
    32e4:	2d0c           	movi.n	a13, 2
    32e6:	1c0c           	movi.n	a12, 1
    32e8:	83cde0        	moveqz	a12, a13, a14
    32eb:	6e52c2        	s16i	a12, a2, 220
    32ee:	000606        	j	330a <cali_module_short+0x82>

000032f1 <cali_module_short+0x69>:
    32f1:	5892f2        	l16si	a15, a2, 176
    32f4:	5992c2        	l16si	a12, a2, 178
    32f7:	022fa7        	blt	a15, a10, 32fd <cali_module_short+0x75>
    32fa:	0020c6        	j	3381 <cali_module_short+0xf9>
    32fd:	04ad           	mov.n	a10, a4
    32ff:	0001c6        	j	330a <cali_module_short+0x82>

00003302 <cali_module_short+0x7a>:
    3302:	ca7c           	movi.n	a10, -4
    3304:	f9cbb2        	addi	a11, a11, -7
    3307:	53bb40        	max	a11, a11, a4

0000330a <cali_module_short+0x82>:
    330a:	6852b2        	s16i	a11, a2, 208

0000330d <cali_module_short+0x85>:
    330d:	6752a2        	s16i	a10, a2, 206
    3310:	33c8           	l32i.n	a12, a3, 12
    3312:	591ca6        	blti	a12, 1, 336f <cali_module_short+0xe7>
    3315:	23a8           	l32i.n	a10, a3, 8
    3317:	01d2b2        	addmi	a11, a2, 0x100
    331a:	f3d481        	l32r	a8, 26c (3254 <cm_short_vad>)
    331d:	bccbb2        	addi	a11, a11, -68
    3320:	31a0a0        	srai	a10, a10, 16
    3323:	0008e0        	callx8	a8
    3326:	13b8           	l32i.n	a11, a3, 4
    3328:	6952a2        	s16i	a10, a2, 210
    332b:	201ba6        	blti	a11, 1, 334f <cali_module_short+0xc7>
    332e:	f3cf81        	l32r	a8, 26c (3254 <cm_short_vad>)
    3331:	f3bd91        	l32r	a9, 228 (69bc <immediate_num+0x1c>)
    3334:	23b8           	l32i.n	a11, a3, 8
    3336:	03a8           	l32i.n	a10, a3, 0
    3338:	37aab4        	ae_truncp24a32x2	aep2, a10, a11
    333b:	441904        	ae_lp16x2f.i	aep1, a9, 0
    333e:	041204        	ae_mulzaafp24s.hh.ll	aeq0, aep1, aep2
    3341:	01d2b2        	addmi	a11, a2, 0x100
    3344:	f8638b4bbe 	{ addi	a11, a11, -76; ae_roundsp16q48asym	aep0, aeq0 }
    3349:	360a24        	ae_trunca16p24s.l	a10, aep0
    334c:	0008e0        	callx8	a8

0000334f <cali_module_short+0xc7>:
    334f:	6b52a2        	s16i	a10, a2, 214
    3352:	53c8           	l32i.n	a12, a3, 20
    3354:	121ca6        	blti	a12, 1, 336a <cali_module_short+0xe2>
    3357:	43a8           	l32i.n	a10, a3, 16
    3359:	01d2b2        	addmi	a11, a2, 0x100
    335c:	f3c481        	l32r	a8, 26c (3254 <cm_short_vad>)
    335f:	c4cbb2        	addi	a11, a11, -60
    3362:	31a0a0        	srai	a10, a10, 16
    3365:	0008e0        	callx8	a8
    3368:	0a4d           	mov.n	a4, a10

0000336a <cali_module_short+0xe2>:
    336a:	6a5242        	s16i	a4, a2, 212
    336d:	f01d           	retw.n

0000336f <cali_module_short+0xe7>:
    336f:	695242        	s16i	a4, a2, 210
    3372:	fff646        	j	334f <cali_module_short+0xc7>

00003375 <cali_module_short+0xed>:
    3375:	675242        	s16i	a4, a2, 206
    3378:	6b5242        	s16i	a4, a2, 214
    337b:	695242        	s16i	a4, a2, 210
    337e:	fffa06        	j	336a <cali_module_short+0xe2>

00003381 <cali_module_short+0xf9>:
    3381:	022ca7        	blt	a12, a10, 3387 <cali_module_short+0xff>
    3384:	ffde86        	j	3302 <cali_module_short+0x7a>
    3387:	ea7c           	movi.n	a10, -2
    3389:	bb0b           	addi.n	a11, a11, -1
    338b:	53bb40        	max	a11, a11, a4
    338e:	ffde06        	j	330a <cali_module_short+0x82>

00003391 <cali_module_short+0x109>:
    3391:	04ad           	mov.n	a10, a4
    3393:	ffdd86        	j	330d <cali_module_short+0x85>

00003396 <cali_module_short+0x10e>:
    3396:	04d2d2        	addmi	a13, a2, 0x400
    3399:	0a9dd2        	l16si	a13, a13, 20
    339c:	1622c2        	l32i	a12, a2, 88
    339f:	11dd60        	slli	a13, a13, 10
    33a2:	022cd7        	blt	a12, a13, 33a8 <cali_module_short+0x120>
    33a5:	ffd846        	j	330a <cali_module_short+0x82>
    33a8:	6d5242        	s16i	a4, a2, 218
    33ab:	ffd6c6        	j	330a <cali_module_short+0x82>

000033ae <cali_module_short+0x126>:
	...

000033b0 <LH_34_1_setup>:
    33b0:	004136        	entry	a1, 32
    33b3:	1348           	l32i.n	a4, a3, 4
    33b5:	0338           	l32i.n	a3, a3, 0
    33b7:	0258           	l32i.n	a5, a2, 0
    33b9:	0a2426        	beqi	a4, 2, 33c7 <LH_34_1_setup+0x17>
    33bc:	313030        	srai	a3, a3, 16
    33bf:	f8d332        	addmi	a3, a3, 0xfffff800
    33c2:	a33d           	ae_clamps16	a3, a3
    33c4:	113300        	slli	a3, a3, 16

000033c7 <LH_34_1_setup+0x17>:
    33c7:	8248           	l32i.n	a4, a2, 32
    33c9:	535530        	max	a5, a5, a3
    33cc:	0259           	s32i.n	a5, a2, 0
    33ce:	434430        	min	a4, a4, a3
    33d1:	8249           	s32i.n	a4, a2, 32
    33d3:	f01d           	retw.n

000033d5 <LH_34_1_setup+0x25>:
    33d5:	000000                                        ...

000033d8 <cali_power>:
    33d8:	006136        	entry	a1, 48
    33db:	f31281        	l32r	a8, 24 (6d14 <p_ecnr_mem>)
    33de:	0888           	l32i.n	a8, a8, 0
    33e0:	15d882        	addmi	a8, a8, 0x1500
    33e3:	819882        	l16si	a8, a8, 0x102
    33e6:	0228e6        	bgei	a8, 2, 33ec <cali_power+0x14>
    33e9:	009106        	j	3631 <cali_power+0x259>
    33ec:	7293b2        	l16si	a11, a3, 228
    33ef:	1c93a2        	l16si	a10, a3, 56
    33f2:	300ab4        	ae_cvtp24a16x2.ll	aep0, a10, a11
    33f5:	010044        	ae_mulfp24s.hh	aeq0, aep0, aep0
    33f8:	6c93d2        	l16si	a13, a3, 216
    33fb:	d4e230149e 	{ l32i	a9, a4, 4; ae_mulfq32sp16s.l	aeq0, aeq0, aep0 }
    3400:	5c0c           	movi.n	a12, 5
    3402:	f863801dde 	{ addi	a13, a13, 1; ae_roundsp16q48asym	aep0, aeq0 }
    3407:	0219e6        	bgei	a9, 1, 340d <cali_power+0x35>
    340a:	009086        	j	3650 <cali_power+0x278>
    340d:	560104        	ae_sp24x2s.i	aep0, a1, 0
    3410:	6a22a2        	l32i	a10, a2, 0x1a8
    3413:	692292        	l32i	a9, a2, 0x1a4
    3416:	64d0d0        	extui	a13, a13, 0, 7
    3419:	6c53d2        	s16i	a13, a3, 216
    341c:	fc248fc99e 	{ addi	a9, a9, -4; ae_zeroq56	aeq0 }
    3421:	fd046d1a0f 	{ ae_lp16x2f.i	aep1, a10, 0; ae_movq56	aeq1, aeq0 }
    3426:	09ac76        	loopgtz	a12, 3433 <cali_power+0x5b>
    3429:	e2246d991f 	{ ae_lp16x2f.iu	aep1, a9, 4; ae_mulaafp24s.hh.ll	aeq1, aep1, aep1 }
    342e:	e2242d9a1f 	{ ae_lp16x2f.iu	aep1, a10, 4; ae_mulaafp24s.hh.ll	aeq0, aep1, aep1 }

00003433 <cali_power+0x5b>:
    3433:	f2ff41        	l32r	a4, 30 (1000 <subband_aec_mid_channel+0x50>)
    3436:	7393d2        	l16si	a13, a3, 230
    3439:	f33fe1        	l32r	a14, 138 (8000 <_end+0x12e4>)
    343c:	35d0b4        	ae_sraiq56	aeq3, aeq1, 2
    343f:	01d3c2        	addmi	a12, a3, 0x100
    3442:	3540b4        	ae_sraiq56	aeq1, aeq0, 2
    3445:	0cbd           	mov.n	a11, a12
    3447:	c8cbb2        	addi	a11, a11, -56
    344a:	c4ccc2        	addi	a12, a12, -60
    344d:	600c04        	ae_lq32f.i	aeq0, a12, 0
    3450:	fe3c55eedf 	{ sub	a14, a14, a13; ae_roundsq32asym	aeq2, aeq3 }
    3455:	fd2c58adef 	{ ae_cvtp24a16x2.ll	aep2, a13, a14; ae_roundsq32asym	aeq1, aeq1 }
    345a:	0c4a3d6b0f 	{ ae_lq32f.i	aeq3, a11, 0; ae_mulzaafq32sp16s.lh	aeq0, aeq0, aep2, aeq2, aep2 }
    345f:	c1aad4        	ae_mulzaafq32sp16s.lh	aeq3, aeq3, aep2, aeq1, aep2
    3462:	068934        	ae_roundsq32asym	aeq2, aeq0
    3465:	fd3c79ac0f 	{ ae_sq32f.i	aeq2, a12, 0; ae_roundsq32asym	aeq1, aeq3 }
    346a:	621b04        	ae_sq32f.i	aeq1, a11, 0
    346d:	3123a2        	l32i	a10, a3, 196
    3470:	3223b2        	l32i	a11, a3, 200
    3473:	f32081        	l32r	a8, f4 (4d88 <airoha_divide_limit_1>)
    3476:	21a3a0        	srai	a10, a10, 3
    3479:	53b4b0        	max	a11, a4, a11
    347c:	0008e0        	callx8	a8
    347f:	020c           	movi.n	a2, 0
    3481:	2123b2        	l32i	a11, a3, 132
    3484:	0a5d           	mov.n	a5, a10
    3486:	6c93d2        	l16si	a13, a3, 216
    3489:	2923a2        	l32i	a10, a3, 164
    348c:	53bb50        	max	a11, a11, a5
    348f:	2163b2        	s32i	a11, a3, 132
    3492:	43aa50        	min	a10, a10, a5
    3495:	2963a2        	s32i	a10, a3, 164
    3498:	04fd56        	bnez	a13, 34eb <cali_power+0x113>
    349b:	f37581        	l32r	a8, 270 (2e08 <take_max_four>)
    349e:	6d9372        	l16si	a7, a3, 218
    34a1:	37daa4        	ae_truncp24a32x2	aep5, a10, a10
    34a4:	37ebb4        	ae_truncp24a32x2	aep6, a11, a11
    34a7:	01d362        	addmi	a6, a3, 0x100
    34aa:	cb0c           	movi.n	a11, 12
    34ac:	88c6a2        	addi	a10, a6, -120
    34af:	fafa8a866e 	{ addi	a6, a6, -88; ae_roundsp16asym	aep4, aep6 }
    34b4:	1197f0        	slli	a9, a7, 1
    34b7:	f9f6e34a9f 	{ ae_sp16f.l.x	aep4, a10, a9; ae_roundsp16asym	aep3, aep5 }
    34bc:	433694        	ae_sp16f.l.x	aep3, a6, a9
    34bf:	0008e0        	callx8	a8
    34c2:	2863a2        	s32i	a10, a3, 160
    34c5:	f36b81        	l32r	a8, 274 (46d8 <take_min_four>)
    34c8:	cb0c           	movi.n	a11, 12
    34ca:	06ad           	mov.n	a10, a6
    34cc:	0008e0        	callx8	a8
    34cf:	971b           	addi.n	a9, a7, 1
    34d1:	3063a2        	s32i	a10, a3, 192
    34d4:	ab0c           	movi.n	a11, 10
    34d6:	f368c1        	l32r	a12, 278 (3fff0000 <_end+0x3ffe92e4>)
    34d9:	f368d1        	l32r	a13, 27c (c0010000 <_end+0xc00092e4>)
    34dc:	2163d2        	s32i	a13, a3, 132
    34df:	2963c2        	s32i	a12, a3, 164
    34e2:	72ab70        	salt	a10, a11, a7
    34e5:	9392a0        	movnez	a9, a2, a10
    34e8:	6d5392        	s16i	a9, a3, 218

000034eb <cali_power+0x113>:
    34eb:	3223e2        	l32i	a14, a3, 200
    34ee:	547104        	ae_lp24x2.i	aep7, a1, 0
    34f1:	01d392        	addmi	a9, a3, 0x100
    34f4:	0224e7        	blt	a4, a14, 34fa <cali_power+0x122>
    34f7:	0022c6        	j	3586 <cali_power+0x1ae>
    34fa:	360534        	ae_cvtq48a32s	aeq0, a5
    34fd:	d4fe4c09fe 	{ addi	a15, a9, -64; ae_mulfq32sp16s.l	aeq1, aeq0, aep7 }
    3502:	608f04        	ae_lq32f.i	aeq2, a15, 0
    3505:	d6c800cc6f 	{ movi	a6, 204; ae_ltq56s	b0, aeq2, aeq1 }
    350a:	d0a072        	movi	a7, 208
    350d:	f35c41        	l32r	a4, 280 (69d8 <immediate_num+0x38>)
    3510:	260076        	bf	b0, 353a <cali_power+0x162>
    3513:	ccc9c2        	addi	a12, a9, -52
    3516:	d0c9d2        	addi	a13, a9, -48
    3519:	445424        	ae_lp16x2f.i	aep5, a4, 8
    351c:	446404        	ae_lp16x2f.i	aep6, a4, 0
    351f:	fe2c7d6d0f 	{ ae_lq32f.i	aeq3, a13, 0; ae_roundsq32asym	aeq2, aeq1 }
    3524:	0fd97d0c0f 	{ ae_lq32f.i	aeq0, a12, 0; ae_mulzaafq32sp16s.lh	aeq3, aeq3, aep6, aeq0, aep6 }
    3529:	c15524        	ae_mulzaafq32sp16s.lh	aeq0, aeq0, aep5, aeq2, aep5
    352c:	06b934        	ae_roundsq32asym	aeq2, aeq3
    352f:	ff2479ad0f 	{ ae_sq32f.i	aeq2, a13, 0; ae_roundsq32asym	aeq3, aeq0 }
    3534:	623c04        	ae_sq32f.i	aeq3, a12, 0
    3537:	0012c6        	j	3586 <cali_power+0x1ae>

0000353a <cali_power+0x162>:
    353a:	547104        	ae_lp24x2.i	aep7, a1, 0
    353d:	a0c9e2        	addi	a14, a9, -96
    3540:	604e04        	ae_lq32f.i	aeq1, a14, 0
    3543:	037a54        	ae_mulfq32sp16s.l	aeq1, aeq1, aep7
    3546:	099014        	ae_ltq56s	b1, aeq0, aeq1
    3549:	630114        	ae_sq56s.i	aeq0, a1, 8
    354c:	360176        	bf	b1, 3586 <cali_power+0x1ae>
    354f:	05ad           	mov.n	a10, a5
    3551:	f2e981        	l32r	a8, f8 (4c80 <airoha_divide>)
    3554:	540104        	ae_lp24x2.i	aep0, a1, 0
    3557:	000bd4        	ae_cvta32p24.l	a11, aep0
    355a:	0008e0        	callx8	a8
    355d:	441404        	ae_lp16x2f.i	aep1, a4, 0
    3560:	442414        	ae_lp16x2f.i	aep2, a4, 4
    3563:	606374        	ae_lq32f.x	aeq1, a3, a7
    3566:	360b34        	ae_cvtq48a32s	aeq0, a11
    3569:	fc24fe8acf 	{ ae_slaasq56s	aeq3, aeq0, a10; nop }
    356e:	fe3c7d811f 	{ ae_lq56.i	aeq0, a1, 8; ae_roundsq32asym	aeq2, aeq3 }
    3573:	0c4b7bf36f 	{ ae_lq32f.x	aeq3, a3, a6; ae_mulzaafq32sp16s.lh	aeq1, aeq1, aep2, aeq2, aep2 }
    3578:	c199c4        	ae_mulzaafq32sp16s.lh	aeq3, aeq3, aep1, aeq0, aep1
    357b:	061934        	ae_roundsq32asym	aeq0, aeq1
    357e:	fe3c78437f 	{ ae_sq32f.x	aeq0, a3, a7; ae_roundsq32asym	aeq2, aeq3 }
    3583:	62a364        	ae_sq32f.x	aeq2, a3, a6
    3586:	3323c2        	l32i	a12, a3, 204
    3589:	f2da81        	l32r	a8, f4 (4d88 <airoha_divide_limit_1>)
    358c:	3423b2        	l32i	a11, a3, 208
    358f:	c0a5c0        	sub	a10, a5, a12
    3592:	21a1a0        	srai	a10, a10, 1
    3595:	c0bbc0        	sub	a11, a11, a12
    3598:	0008e0        	callx8	a8
    359b:	31b0a0        	srai	a11, a10, 16
    359e:	7693e2        	l16si	a14, a3, 236
    35a1:	01d3d2        	addmi	a13, a3, 0x100
    35a4:	d4cdd2        	addi	a13, a13, -44
    35a7:	11ce00        	slli	a12, a14, 16
    35aa:	312ac7        	blt	a10, a12, 35df <cali_power+0x207>
    35ad:	749382        	l16si	a8, a3, 232
    35b0:	c0fbe0        	sub	a15, a11, a14
    35b3:	303f84        	ae_cvtp24a16x2.ll	aep3, a15, a8
    35b6:	013b44        	ae_mulfp24s.hh	aeq1, aep3, aep3
    35b9:	033a54        	ae_mulfq32sp16s.l	aeq1, aeq1, aep3
    35bc:	3613d4        	ae_sllisq56s	aeq0, aeq1, 15
    35bf:	ff247d4d0f 	{ ae_lq32f.i	aeq2, a13, 0; ae_roundsq32asym	aeq3, aeq0 }
    35c4:	35f174        	ae_sraiq56	aeq3, aeq3, 5
    35c7:	01abc4        	ae_addq56	aeq2, aeq2, aeq3
    35ca:	066934        	ae_roundsq32asym	aeq1, aeq2
    35cd:	621d04        	ae_sq32f.i	aeq1, a13, 0
    35d0:	7693c2        	l16si	a12, a3, 236
    35d3:	6f5322        	s16i	a2, a3, 222
    35d6:	352392        	l32i	a9, a3, 212
    35d9:	11cc00        	slli	a12, a12, 16
    35dc:	000b06        	j	360c <cali_power+0x234>

000035df <cali_power+0x207>:
    35df:	6f93e2        	l16si	a14, a3, 222
    35e2:	7593f2        	l16si	a15, a3, 234
    35e5:	882c           	movi.n	a8, 40
    35e7:	ee1b           	addi.n	a14, a14, 1
    35e9:	8eed           	ae_sext16	a14, a14
    35eb:	43ee80        	min	a14, a14, a8
    35ee:	6f53e2        	s16i	a14, a3, 222
    35f1:	304ef4        	ae_cvtp24a16x2.ll	aep4, a14, a15
    35f4:	014c54        	ae_mulfp24s.hl	aeq1, aep4, aep4
    35f7:	60cd04        	ae_lq32f.i	aeq3, a13, 0
    35fa:	361294        	ae_sllisq56s	aeq0, aeq1, 10
    35fd:	01fcc4        	ae_subq56	aeq3, aeq3, aeq0
    3600:	06b934        	ae_roundsq32asym	aeq2, aeq3
    3603:	36a904        	ae_trunca32q48	a9, aeq2
    3606:	539920        	max	a9, a9, a2
    3609:	356392        	s32i	a9, a3, 212

0000360c <cali_power+0x234>:
    360c:	f30881        	l32r	a8, 22c (40000000 <_end+0x3fff92e4>)
    360f:	21f190        	srai	a15, a9, 1
    3612:	ff8a           	add.n	a15, a15, a8
    3614:	37daf4        	ae_truncp24a32x2	aep5, a10, a15
    3617:	015564        	ae_mulfp24s.lh	aeq0, aep5, aep5
    361a:	368c34        	ae_cvtq48a32s	aeq2, a12
    361d:	350054        	ae_slliq56	aeq0, aeq0, 1
    3620:	098624        	ae_leq56s	b2, aeq2, aeq0
    3623:	110276        	bf	b2, 3638 <cali_power+0x260>
    3626:	6e9392        	l16si	a9, a3, 220
    3629:	994b           	addi.n	a9, a9, 4

0000362b <cali_power+0x253>:
    362b:	6e5392        	s16i	a9, a3, 220
    362e:	000046        	j	3633 <cali_power+0x25b>

00003631 <cali_power+0x259>:
    3631:	0b0c           	movi.n	a11, 0

00003633 <cali_power+0x25b>:
    3633:	7053b2        	s16i	a11, a3, 224
    3636:	f01d           	retw.n

00003638 <cali_power+0x260>:
    3638:	7793a2        	l16si	a10, a3, 238
    363b:	11aa00        	slli	a10, a10, 16
    363e:	364a34        	ae_cvtq48a32s	aeq1, a10
    3641:	099034        	ae_ltq56s	b3, aeq0, aeq1
    3644:	eb0376        	bf	b3, 3633 <cali_power+0x25b>
    3647:	6e9392        	l16si	a9, a3, 220
    364a:	fcc992        	addi	a9, a9, -4
    364d:	fff686        	j	362b <cali_power+0x253>

00003650 <cali_power+0x278>:
    3650:	0b0c           	movi.n	a11, 0
    3652:	fff746        	j	3633 <cali_power+0x25b>

00003655 <cali_power+0x27d>:
    3655:	000000                                        ...

00003658 <mag_calibration>:
    3658:	004136        	entry	a1, 32
    365b:	f30ac1        	l32r	a12, 284 (69c8 <immediate_num+0x28>)
    365e:	01d2b2        	addmi	a11, a2, 0x100
    3661:	4cc2d2        	addi	a13, a2, 76
    3664:	604d04        	ae_lq32f.i	aeq1, a13, 0
    3667:	601ba4        	ae_lq32f.iu	aeq0, a11, -24
    366a:	440c04        	ae_lp16x2f.i	aep0, a12, 0
    366d:	c10014        	ae_mulzaafq32sp16s.lh	aeq0, aeq0, aep0, aeq1, aep0
    3670:	01d292        	addmi	a9, a2, 0x100
    3673:	ff244502ae 	{ addi	a10, a2, 80; ae_roundsq32asym	aeq3, aeq0 }
    3678:	623b04        	ae_sq32f.i	aeq3, a11, 0
    367b:	608a04        	ae_lq32f.i	aeq2, a10, 0
    367e:	6059b4        	ae_lq32f.iu	aeq1, a9, -20
    3681:	c10864        	ae_mulzaafq32sp16s.lh	aeq1, aeq1, aep0, aeq2, aep0
    3684:	f26b81        	l32r	a8, 30 (1000 <subband_aec_mid_channel+0x50>)
    3687:	061934        	ae_roundsq32asym	aeq0, aeq1
    368a:	620904        	ae_sq32f.i	aeq0, a9, 0
    368d:	3a22b2        	l32i	a11, a2, 232
    3690:	0228b7        	blt	a8, a11, 3696 <mag_calibration+0x3e>
    3693:	004406        	j	37a7 <mag_calibration+0x14f>
    3696:	3b22a2        	l32i	a10, a2, 236
    3699:	f29681        	l32r	a8, f4 (4d88 <airoha_divide_limit_1>)
    369c:	21a2a0        	srai	a10, a10, 2
    369f:	0008e0        	callx8	a8
    36a2:	37aaa4        	ae_truncp24a32x2	aep2, a10, a10
    36a5:	02d2c2        	addmi	a12, a2, 0x200
    36a8:	f8eac8ecce 	{ addi	a12, a12, -114; ae_roundsp16asym	aep1, aep2 }
    36ad:	421c04        	ae_sp16f.l.i	aep1, a12, 0
    36b0:	6c92b2        	l16si	a11, a2, 216
    36b3:	bb1b           	addi.n	a11, a11, 1
    36b5:	8bbd           	ae_sext16	a11, a11
    36b7:	6c52b2        	s16i	a11, a2, 216
    36ba:	e0cbb2        	addi	a11, a11, -32
    36bd:	0ceb56        	bnez	a11, 378f <mag_calibration+0x137>
    36c0:	cb0c           	movi.n	a11, 12
    36c2:	f2eb81        	l32r	a8, 270 (2e08 <take_max_four>)
    36c5:	3c2292        	l32i	a9, a2, 240
    36c8:	7392a2        	l16si	a10, a2, 230
    36cb:	040c           	movi.n	a4, 0
    36cd:	6c5242        	s16i	a4, a2, 216
    36d0:	90aa20        	addx2	a10, a10, a2
    36d3:	319090        	srai	a9, a9, 16
    36d6:	7a5a92        	s16i	a9, a10, 244
    36d9:	01d2a2        	addmi	a10, a2, 0x100
    36dc:	f4caa2        	addi	a10, a10, -12
    36df:	0008e0        	callx8	a8
    36e2:	f2e481        	l32r	a8, 274 (46d8 <take_min_four>)
    36e5:	442292        	l32i	a9, a2, 0x110
    36e8:	0abd           	mov.n	a11, a10
    36ea:	4362b2        	s32i	a11, a2, 0x10c
    36ed:	7392a2        	l16si	a10, a2, 230
    36f0:	cb0c           	movi.n	a11, 12
    36f2:	319090        	srai	a9, a9, 16
    36f5:	90aa20        	addx2	a10, a10, a2
    36f8:	8a5a92        	s16i	a9, a10, 0x114
    36fb:	01d2a2        	addmi	a10, a2, 0x100
    36fe:	14caa2        	addi	a10, a10, 20
    3701:	0008e0        	callx8	a8
    3704:	4b62a2        	s32i	a10, a2, 0x12c
    3707:	01d292        	addmi	a9, a2, 0x100
    370a:	e4c932        	addi	a3, a9, -28
    370d:	404304        	ae_lp16f.i	aep4, a3, 0
    3710:	01c474        	ae_mulfp24s.ll	aeq2, aep4, aep4
    3713:	09dd           	mov.n	a13, a9
    3715:	f973fd5d3f 	{ ae_lq32f.iu	aeq2, a13, 12; ae_roundsp16q48asym	aep3, aeq2 }
    371a:	d6ee02c9ce 	{ addi	a12, a9, 44; ae_mulfq32sp16s.l	aeq0, aeq2, aep3 }
    371f:	604c04        	ae_lq32f.i	aeq1, a12, 0
    3722:	098104        	ae_ltq56s	b0, aeq1, aeq0
    3725:	f2d8e1        	l32r	a14, 288 (6ccd <wb_sb_ptn+0x1d>)
    3728:	305ee4        	ae_cvtp24a16x2.ll	aep5, a14, a14
    372b:	1f0076        	bf	b0, 374e <mag_calibration+0xf6>
    372e:	06c934        	ae_roundsq32asym	aeq3, aeq0
    3731:	03dad4        	ae_mulfq32sp16s.l	aeq3, aeq3, aep5
    3734:	099314        	ae_ltq56s	b1, aeq3, aeq1
    3737:	130176        	bf	b1, 374e <mag_calibration+0xf6>
    373a:	f2d481        	l32r	a8, 28c (69cc <immediate_num+0x2c>)
    373d:	09fd           	mov.n	a15, a9
    373f:	605f84        	ae_lq32f.iu	aeq1, a15, -32
    3742:	446804        	ae_lp16x2f.i	aep6, a8, 0
    3745:	c16e64        	ae_mulzaafq32sp16s.lh	aeq1, aeq1, aep6, aeq2, aep6
    3748:	061934        	ae_roundsq32asym	aeq0, aeq1
    374b:	620f04        	ae_sq32f.i	aeq0, a15, 0
    374e:	f23981        	l32r	a8, 34 (5b4 <HIFI_sqrt>)
    3751:	0a0c           	movi.n	a10, 0
    3753:	3822b2        	l32i	a11, a2, 224
    3756:	0008e0        	callx8	a8
    3759:	360b34        	ae_cvtq48a32s	aeq0, a11
    375c:	fc24fe8acf 	{ ae_slaasq56s	aeq3, aeq0, a10; nop }
    3761:	fe3c6c030f 	{ ae_lp16f.i	aep0, a3, 0; ae_roundsq32asym	aeq2, aeq3 }
    3766:	038294        	ae_mulfq32sp16s.l	aeq2, aeq2, aep0
    3769:	fb73c542ae 	{ addi	a10, a2, 84; ae_roundsp16q48asym	aep7, aeq2 }
    376e:	427a04        	ae_sp16f.l.i	aep7, a10, 0
    3771:	739292        	l16si	a9, a2, 230
    3774:	c792a2        	l16si	a10, a2, 0x18e
    3777:	991b           	addi.n	a9, a9, 1
    3779:	11aa00        	slli	a10, a10, 16
    377c:	3c62a2        	s32i	a10, a2, 240
    377f:	4462a2        	s32i	a10, a2, 0x110
    3782:	899d           	ae_sext16	a9, a9
    3784:	735292        	s16i	a9, a2, 230
    3787:	1ca9a6        	blti	a9, 12, 37a7 <mag_calibration+0x14f>
    378a:	735242        	s16i	a4, a2, 230
    378d:	f01d           	retw.n

0000378f <mag_calibration+0x137>:
    378f:	c792d2        	l16si	a13, a2, 0x18e
    3792:	4422b2        	l32i	a11, a2, 0x110
    3795:	3c22c2        	l32i	a12, a2, 240
    3798:	11dd00        	slli	a13, a13, 16
    379b:	43bbd0        	min	a11, a11, a13
    379e:	53ccd0        	max	a12, a12, a13
    37a1:	3c62c2        	s32i	a12, a2, 240
    37a4:	4462b2        	s32i	a11, a2, 0x110
    37a7:	f01d           	retw.n

000037a9 <mag_calibration+0x151>:
    37a9:	000000                                        ...

000037ac <beam1_proc>:
    37ac:	008136        	entry	a1, 64
    37af:	3139           	s32i.n	a3, a1, 12
    37b1:	1c0c           	movi.n	a12, 1
    37b3:	fc2491464f 	{ addx2	a4, a6, a4; ae_zeroq56	aeq0 }
    37b8:	1166f0        	slli	a6, a6, 1
    37bb:	3b15a6        	blti	a5, 1, 37fa <beam1_proc+0x4e>
    37be:	049d           	mov.n	a9, a4
    37c0:	440404        	ae_lp16x2f.i	aep0, a4, 0
    37c3:	5b0c           	movi.n	a11, 5
    37c5:	9d1c           	movi.n	a13, 25
    37c7:	15a0a2        	movi	a10, 21
    37ca:	fd044002ef 	{ movi	a14, 2; ae_movq56	aeq1, aeq0 }
    37cf:	72ee50        	salt	a14, a14, a5
    37d2:	93ade0        	movnez	a10, a13, a14
    37d5:	04ab76        	loopgtz	a11, 37dd <beam1_proc+0x31>
    37d8:	e2006d891f 	{ ae_lp16x2f.iu	aep0, a9, 4; ae_mulaafp24s.hh.ll	aeq1, aep0, aep0 }

000037dd <beam1_proc+0x31>:
    37dd:	f24681        	l32r	a8, f8 (4c80 <airoha_divide>)
    37e0:	3550b4        	ae_sraiq56	aeq1, aeq1, 2
    37e3:	401a00        	ssl	a10
    37e6:	a1ac00        	sll	a10, a12
    37e9:	061934        	ae_roundsq32asym	aeq0, aeq1
    37ec:	368b04        	ae_trunca32q48	a11, aeq0
    37ef:	0008e0        	callx8	a8
    37f2:	fc249441bf 	{ s32i.n	a11, a1, 16; ae_zeroq56	aeq0 }
    37f7:	000046        	j	37fc <beam1_proc+0x50>

000037fa <beam1_proc+0x4e>:
    37fa:	4199           	s32i.n	a9, a1, 16

000037fc <beam1_proc+0x50>:
    37fc:	0d0c           	movi.n	a13, 0
    37fe:	31f8           	l32i.n	a15, a1, 12
    3800:	6e92b2        	l16si	a11, a2, 220
    3803:	fec432        	addi	a3, a4, -2
    3806:	3fc8           	l32i.n	a12, a15, 12
    3808:	1f88           	l32i.n	a8, a15, 4
    380a:	24cf92        	addi	a9, a15, 36
    380d:	10cfe2        	addi	a14, a15, 16
    3810:	21e9           	s32i.n	a14, a1, 8
    3812:	0199           	s32i.n	a9, a1, 0
    3814:	f3f790        	wur.ae_cend0	a9
    3817:	403e04        	ae_lp16f.i	aep3, a14, 0
    381a:	686a           	add.n	a6, a8, a6
    381c:	fec662        	addi	a6, a6, -2
    381f:	14cff2        	addi	a15, a15, 20
    3822:	11f9           	s32i.n	a15, a1, 4
    3824:	f3f6f0        	wur.ae_cbegin0	a15
    3827:	064d           	mov.n	a4, a6
    3829:	041ba6        	blti	a11, 1, 3831 <beam1_proc+0x85>
    382c:	180c           	movi.n	a8, 1
    382e:	93d870        	movnez	a13, a8, a7
    3831:	18c2b2        	addi	a11, a2, 24
    3834:	0e0c           	movi.n	a14, 0
    3836:	af0c           	movi.n	a15, 10
    3838:	880c           	movi.n	a8, 8
    383a:	0470d0        	extui	a7, a13, 0, 1
    383d:	fd6c0002df 	{ movi	a13, 2; ae_cvtq48p24s.l	aeq1, aep3 }

00003842 <beam1_proc+0x96>:
    3842:	408314        	ae_lp16f.iu	aep0, a3, 2
    3845:	fc24e28cdf 	{ ae_sp16f.l.c	aep0, a12, a13; nop }
    384a:	400904        	ae_lp16f.i	aep0, a9, 0
    384d:	40b614        	ae_lp16f.iu	aep3, a6, 2
    3850:	09a876        	loopgtz	a8, 385d <beam1_proc+0xb1>
    3853:	fc24d91cdf 	{ ae_lp16f.c	aep1, a12, a13; nop }
    3858:	ee21ec891f 	{ ae_lp16f.iu	aep0, a9, 2; ae_mulsfp24s.ll	aeq1, aep0, aep1 }

0000385d <beam1_proc+0xb1>:
    385d:	f86bcffffe 	{ addi	a15, a15, -1; ae_roundsp16q48asym	aep1, aeq1 }
    3862:	f8668f099e 	{ addi	a9, a9, -16; ae_movp48	aep0, aep1 }
    3867:	17ac           	beqz.n	a7, 388c <beam1_proc+0xe0>
    3869:	402214        	ae_lp16f.i	aep2, a2, 2
    386c:	405204        	ae_lp16f.i	aep5, a2, 0
    386f:	eb35ec422f 	{ ae_lp16f.i	aep4, a2, 4; ae_mulfp24s.ll	aeq3, aep5, aep1 }
    3874:	608274        	ae_lq32f.i	aeq2, a2, 28
    3877:	35f094        	ae_slliq56	aeq3, aeq3, 2
    387a:	f792fd226f 	{ ae_lq32f.i	aeq1, a2, 24; ae_mulafq32sp16s.l	aeq3, aeq2, aep4 }
    387f:	03a854        	ae_mulafq32sp16s.l	aeq3, aeq1, aep2
    3882:	fe3c799b1f 	{ ae_sq32f.i	aeq1, a11, 4; ae_roundsq32asym	aeq2, aeq3 }
    3887:	f873b9ab0f 	{ ae_sq32f.i	aeq2, a11, 0; ae_roundsp16q48asym	aep0, aeq2 }

0000388c <beam1_proc+0xe0>:
    388c:	428414        	ae_sp16f.l.iu	aep0, a4, 2
    388f:	3015a6        	blti	a5, 1, 38c3 <beam1_proc+0x117>
    3892:	2dee07        	bbsi	a14, 0, 38c3 <beam1_proc+0x117>
    3895:	4188           	l32i.n	a8, a1, 16
    3897:	368834        	ae_cvtq48a32s	aeq2, a8
    389a:	039294        	ae_mulfq32sp16s.l	aeq2, aeq2, aep1
    389d:	400904        	ae_lp16f.i	aep0, a9, 0
    38a0:	fc24feaa4f 	{ ae_slaasq56s	aeq1, aeq2, a10; nop }
    38a5:	f96f80088f 	{ movi	a8, 8; ae_roundsp24q48asym	aep2, aeq1 }
    38aa:	0020f0        	nop
    38ad:	0fa876        	loopgtz	a8, 38c0 <beam1_proc+0x114>
    38b0:	ff0c197cdf 	{ ae_lp16f.c	aep7, a12, a13; ae_cvtq48p24s.l	aeq3, aep0 }
    38b5:	01af34        	ae_mulafp24s.ll	aeq3, aep2, aep7
    38b8:	fb7bac891f 	{ ae_lp16f.iu	aep0, a9, 2; ae_roundsp16q48asym	aep6, aeq3 }
    38bd:	4269f4        	ae_sp16f.l.i	aep6, a9, -2

000038c0 <beam1_proc+0x114>:
    38c0:	f0c992        	addi	a9, a9, -16
    38c3:	ee1b           	addi.n	a14, a14, 1
    38c5:	fd6c00088f 	{ movi	a8, 8; ae_cvtq48p24s.l	aeq1, aep3 }
    38ca:	f74f56        	bnez	a15, 3842 <beam1_proc+0x96>
    38cd:	4b0c           	movi.n	a11, 4
    38cf:	11d8           	l32i.n	a13, a1, 4
    38d1:	01a8           	l32i.n	a10, a1, 0
    38d3:	31f8           	l32i.n	a15, a1, 12
    38d5:	21e8           	l32i.n	a14, a1, 8
    38d7:	209aa0        	or	a9, a10, a10
    38da:	036fc2        	s32i	a12, a15, 12
    38dd:	423e04        	ae_sp16f.l.i	aep3, a14, 0
    38e0:	fd046d0d4f 	{ ae_lp16x2f.i	aep0, a13, 16; ae_movq56	aeq1, aeq0 }
    38e5:	04ab76        	loopgtz	a11, 38ed <beam1_proc+0x141>

000038e8 <beam1_proc+0x13c>:
    38e8:	e2006d891f 	{ ae_lp16x2f.iu	aep0, a9, 4; ae_mulaafp24s.hh.ll	aeq1, aep0, aep0 }

000038ed <beam1_proc+0x141>:
    38ed:	f86bb031be 	{ l32i	a11, a1, 12; ae_roundsp16q48asym	aep0, aeq1 }
    38f2:	360924        	ae_trunca16p24s.l	a9, aep0
    38f5:	069282        	l16si	a8, a2, 12
    38f8:	095b92        	s16i	a9, a11, 18
    38fb:	899d           	ae_sext16	a9, a9
    38fd:	1ea897        	bge	a8, a9, 391f <beam1_proc+0x173>
    3900:	0a9d           	mov.n	a9, a10
    3902:	08a0c2        	movi	a12, 8
    3905:	f262d1        	l32r	a13, 290 (7efa <_end+0x11de>)
    3908:	34cbe2        	addi	a14, a11, 52
    390b:	400e84        	ae_lp16f.i	aep0, a14, -16
    390e:	302dd4        	ae_cvtp24a16x2.ll	aep2, a13, a13
    3911:	0aac76        	loopgtz	a12, 391f <beam1_proc+0x173>
    3914:	012074        	ae_mulfp24s.ll	aeq0, aep2, aep0
    3917:	f863ec891f 	{ ae_lp16f.iu	aep0, a9, 2; ae_roundsp16q48asym	aep1, aeq0 }
    391c:	4219f4        	ae_sp16f.l.i	aep1, a9, -2

0000391f <beam1_proc+0x173>:
    391f:	f01d           	retw.n

00003921 <beam1_proc+0x175>:
    3921:	000000                                        ...

00003924 <beam2_proc>:
    3924:	00e136        	entry	a1, 112
    3927:	02fd           	mov.n	a15, a2
    3929:	03ed           	mov.n	a14, a3
    392b:	07ad           	mov.n	a10, a7
    392d:	b159           	s32i.n	a5, a1, 44
    392f:	1d9392        	l16si	a9, a3, 58
    3932:	f388           	l32i.n	a8, a3, 60
    3934:	06bd           	mov.n	a11, a6
    3936:	119910        	slli	a9, a9, 15
    3939:	02a897        	bge	a8, a9, 393f <beam2_proc+0x1b>
    393c:	fec6b2        	addi	a11, a6, -2

0000393f <beam2_proc+0x1b>:
    393f:	51b9           	s32i.n	a11, a1, 20
    3941:	b1c8           	l32i.n	a12, a1, 44
    3943:	1e68           	l32i.n	a6, a14, 4
    3945:	0e78           	l32i.n	a7, a14, 0
    3947:	1d21d2        	l32i	a13, a1, 116
    394a:	6cce22        	addi	a2, a14, 108
    394d:	38ce82        	addi	a8, a14, 56
    3950:	6189           	s32i.n	a8, a1, 24
    3952:	400804        	ae_lp16f.i	aep0, a8, 0
    3955:	f3f620        	wur.ae_cbegin0	a2
    3958:	907d70        	addx2	a7, a13, a7
    395b:	fec772        	addi	a7, a7, -2
    395e:	902d40        	addx2	a2, a13, a4
    3961:	fec222        	addi	a2, a2, -2
    3964:	de48           	l32i.n	a4, a14, 52
    3966:	90dd60        	addx2	a13, a13, a6
    3969:	fecdd2        	addi	a13, a13, -2
    396c:	7cce62        	addi	a6, a14, 124
    396f:	7169           	s32i.n	a6, a1, 28
    3971:	f3f760        	wur.ae_cend0	a6
    3974:	0e1ce6        	bgei	a12, 1, 3986 <beam2_proc+0x62>
    3977:	f1d781        	l32r	a8, d4 (7fff <_end+0x12e3>)
    397a:	089a87        	bne	a10, a8, 3986 <beam2_proc+0x62>
    397d:	a1a9           	s32i.n	a10, a1, 40
    397f:	190c           	movi.n	a9, 1
    3981:	c199           	s32i.n	a9, a1, 48
    3983:	000146        	j	398c <beam2_proc+0x68>

00003986 <beam2_proc+0x62>:
    3986:	a1a9           	s32i.n	a10, a1, 40
    3988:	0b0c           	movi.n	a11, 0
    398a:	c1b9           	s32i.n	a11, a1, 48

0000398c <beam2_proc+0x68>:
    398c:	51a8           	l32i.n	a10, a1, 20
    398e:	40ce82        	addi	a8, a14, 64
    3991:	9189           	s32i.n	a8, a1, 36
    3993:	feca32        	addi	a3, a10, -2
    3996:	fcca52        	addi	a5, a10, -4
    3999:	f8cac2        	addi	a12, a10, -8
    399c:	facab2        	addi	a11, a10, -6
    399f:	21b9           	s32i.n	a11, a1, 8
    39a1:	31c9           	s32i.n	a12, a1, 12
    39a3:	8159           	s32i.n	a5, a1, 32
    39a5:	1139           	s32i.n	a3, a1, 4
    39a7:	050c           	movi.n	a5, 0
    39a9:	230c           	movi.n	a3, 2
    39ab:	ac0c           	movi.n	a12, 10
    39ad:	fbcaa2        	addi	a10, a10, -5
    39b0:	fc249441af 	{ s32i.n	a10, a1, 16; ae_zeroq56	aeq0 }

000039b5 <beam2_proc+0x91>:
    39b5:	409d14        	ae_lp16f.iu	aep1, a13, 2
    39b8:	fc24e2943f 	{ ae_sp16f.l.c	aep1, a4, a3; nop }
    39bd:	6e9f92        	l16si	a9, a15, 220
    39c0:	fd0440088f 	{ movi	a8, 8; ae_movq56	aeq1, aeq0 }
    39c5:	0e7916        	beqz	a9, 3ab0 <beam2_proc+0x18c>
    39c8:	fe0c2c160f 	{ ae_lp16f.i	aep1, a6, 0; ae_cvtq48p24s.l	aeq2, aep0 }
    39cd:	09a876        	loopgtz	a8, 39da <beam2_proc+0xb6>
    39d0:	fc24d9243f 	{ ae_lp16f.c	aep2, a4, a3; nop }
    39d5:	e247ec961f 	{ ae_lp16f.iu	aep1, a6, 2; ae_mulafp24s.ll	aeq1, aep1, aep2 }

000039da <beam2_proc+0xb6>:
    39da:	a188           	l32i.n	a8, a1, 40
    39dc:	f96fd8c88f 	{ ae_cvtp24a16x2.ll	aep4, a8, a8; ae_roundsp24q48asym	aep3, aeq1 }
    39e1:	05b4c4        	ae_mulssfp24s.hh.ll	aeq2, aep3, aep4
    39e4:	f0c662        	addi	a6, a6, -16
    39e7:	f973ff096f 	{ ae_trunca16p24s.l	a9, aep0; ae_roundsp16q48asym	aep3, aeq2 }
    39ec:	363824        	ae_trunca16p24s.l	a8, aep3

000039ef <beam2_proc+0xcb>:
    39ef:	f962ac821f 	{ ae_lp16f.iu	aep0, a2, 2; ae_movp48	aep2, aep0 }
    39f4:	609190        	abs	a9, a9
    39f7:	608180        	abs	a8, a8
    39fa:	729980        	salt	a9, a9, a8
    39fd:	19cc           	bnez.n	a9, 3a02 <beam2_proc+0xde>
    39ff:	0934a4        	ae_movp48	aep2, aep3

00003a02 <beam2_proc+0xde>:
    3a02:	c198           	l32i.n	a9, a1, 48
    3a04:	42a714        	ae_sp16f.l.iu	aep2, a7, 2
    3a07:	0ad916        	beqz	a9, 3ab8 <beam2_proc+0x194>
    3a0a:	158c           	beqz.n	a5, 3a0f <beam2_proc+0xeb>
    3a0c:	715566        	bnei	a5, 5, 3a81 <beam2_proc+0x15d>

00003a0f <beam2_proc+0xeb>:
    3a0f:	4b0c           	movi.n	a11, 4
    3a11:	fd044004af 	{ movi	a10, 4; ae_movq56	aeq1, aeq0 }
    3a16:	04aa76        	loopgtz	a10, 3a1e <beam2_proc+0xfa>
    3a19:	fc24d9243f 	{ ae_lp16f.c	aep2, a4, a3; nop }
    3a1e:	04ab76        	loopgtz	a11, 3a26 <beam2_proc+0x102>
    3a21:	e24bd9243f 	{ ae_lp16f.c	aep2, a4, a3; ae_mulafp24s.ll	aeq1, aep2, aep2 }

00003a26 <beam2_proc+0x102>:
    3a26:	e24bf0919e 	{ l32i	a9, a1, 36; ae_mulafp24s.ll	aeq1, aep2, aep2 }
    3a2b:	f21aa1        	l32r	a10, 294 (fff <subband_aec_mid_channel+0x4f>)
    3a2e:	3590b4        	ae_sraiq56	aeq2, aeq1, 2
    3a31:	ff347d290f 	{ ae_lq32f.i	aeq1, a9, 0; ae_roundsq32asym	aeq3, aeq2 }
    3a36:	f9ea39b90f 	{ ae_sq32f.i	aeq3, a9, 0; ae_addq56	aeq1, aeq1, aeq2 }
    3a3b:	ff2c668f8e 	{ l16si	a8, a15, 208; ae_roundsq32asym	aeq3, aeq1 }
    3a40:	36bb04        	ae_trunca32q48	a11, aeq3
    3a43:	116eb2        	s32i	a11, a14, 68
    3a46:	0218e6        	bgei	a8, 1, 3a4c <beam2_proc+0x128>
    3a49:	007146        	j	3c12 <beam2_proc+0x2ee>
    3a4c:	02aab7        	bge	a10, a11, 3a52 <beam2_proc+0x12e>
    3a4f:	0040c6        	j	3b56 <beam2_proc+0x232>
    3a52:	81a8           	l32i.n	a10, a1, 32
    3a54:	d1f9           	s32i.n	a15, a1, 52
    3a56:	01e9           	s32i.n	a14, a1, 0
    3a58:	e1d9           	s32i.n	a13, a1, 56
    3a5a:	f1c9           	s32i.n	a12, a1, 60
    3a5c:	78c182        	addi	a8, a1, 120
    3a5f:	560894        	ae_sp24x2s.i	aep0, a8, -56

00003a62 <beam2_proc+0x13e>:
    3a62:	401a00        	ssl	a10
    3a65:	f1a481        	l32r	a8, f8 (4c80 <airoha_divide>)
    3a68:	1a0c           	movi.n	a10, 1
    3a6a:	a1aa00        	sll	a10, a10
    3a6d:	0008e0        	callx8	a8
    3a70:	e1d8           	l32i.n	a13, a1, 56
    3a72:	01e8           	l32i.n	a14, a1, 0
    3a74:	d1f8           	l32i.n	a15, a1, 52
    3a76:	78c1c2        	addi	a12, a1, 120
    3a79:	540c94        	ae_lp24x2.i	aep0, a12, -56
    3a7c:	fc24b0f1ce 	{ l32i	a12, a1, 60; ae_zeroq56	aeq0 }

00003a81 <beam2_proc+0x15d>:
    3a81:	405704        	ae_lp16f.i	aep5, a7, 0
    3a84:	368b34        	ae_cvtq48a32s	aeq2, a11
    3a87:	03d294        	ae_mulfq32sp16s.l	aeq2, aeq2, aep5
    3a8a:	401604        	ae_lp16f.i	aep1, a6, 0
    3a8d:	fc24feaa4f 	{ ae_slaasq56s	aeq1, aeq2, a10; nop }
    3a92:	f96fc0088f 	{ movi	a8, 8; ae_roundsp24q48asym	aep3, aeq1 }
    3a97:	0fa876        	loopgtz	a8, 3aaa <beam2_proc+0x186>
    3a9a:	ff2c19743f 	{ ae_lp16f.c	aep7, a4, a3; ae_cvtq48p24s.l	aeq3, aep1 }
    3a9f:	01bf34        	ae_mulafp24s.ll	aeq3, aep3, aep7
    3aa2:	fb7bac961f 	{ ae_lp16f.iu	aep1, a6, 2; ae_roundsp16q48asym	aep6, aeq3 }
    3aa7:	4266f4        	ae_sp16f.l.i	aep6, a6, -2

00003aaa <beam2_proc+0x186>:
    3aaa:	f0c662        	addi	a6, a6, -16
    3aad:	0001c6        	j	3ab8 <beam2_proc+0x194>

00003ab0 <beam2_proc+0x18c>:
    3ab0:	f862ec821f 	{ ae_lp16f.iu	aep0, a2, 2; ae_movp48	aep1, aep0 }
    3ab5:	429714        	ae_sp16f.l.iu	aep1, a7, 2

00003ab8 <beam2_proc+0x194>:
    3ab8:	551b           	addi.n	a5, a5, 1
    3aba:	cc0b           	addi.n	a12, a12, -1
    3abc:	ef5c56        	bnez	a12, 39b5 <beam2_proc+0x91>
    3abf:	7198           	l32i.n	a9, a1, 28
    3ac1:	4a0c           	movi.n	a10, 4
    3ac3:	de49           	s32i.n	a4, a14, 52
    3ac5:	6188           	l32i.n	a8, a1, 24
    3ac7:	01de22        	addmi	a2, a14, 0x100
    3aca:	80c222        	addi	a2, a2, -128
    3acd:	420804        	ae_sp16f.l.i	aep0, a8, 0
    3ad0:	fd046d22ff 	{ ae_lp16x2f.i	aep2, a2, -4; ae_movq56	aeq1, aeq0 }
    3ad5:	04aa76        	loopgtz	a10, 3add <beam2_proc+0x1b9>
    3ad8:	e2486da91f 	{ ae_lp16x2f.iu	aep2, a9, 4; ae_mulaafp24s.hh.ll	aeq1, aep2, aep2 }

00003add <beam2_proc+0x1b9>:
    3add:	d97c           	movi.n	a9, -3
    3adf:	b1a8           	l32i.n	a10, a1, 44
    3ae1:	f96b83aebe 	{ addi	a11, a14, 58; ae_roundsp16q48asym	aep2, aeq1 }
    3ae6:	422b04        	ae_sp16f.l.i	aep2, a11, 0
    3ae9:	02a9a7        	bge	a9, a10, 3aef <beam2_proc+0x1cb>
    3aec:	002206        	j	3b78 <beam2_proc+0x254>
    3aef:	689fc2        	l16si	a12, a15, 208
    3af2:	082c56        	bnez	a12, 3b78 <beam2_proc+0x254>
    3af5:	279ed2        	l16si	a13, a14, 78
    3af8:	1d9ea2        	l16si	a10, a14, 58
    3afb:	122e92        	l32i	a9, a14, 72
    3afe:	dd1b           	addi.n	a13, a13, 1
    3b00:	8ddd           	ae_sext16	a13, a13
    3b02:	275ed2        	s16i	a13, a14, 78
    3b05:	6acda6        	blti	a13, 32, 3b73 <beam2_proc+0x24f>
    3b08:	126ea2        	s32i	a10, a14, 72
    3b0b:	289eb2        	l16si	a11, a14, 80
    3b0e:	10c982        	addi	a8, a9, 16
    3b11:	218580        	srai	a8, a8, 5
    3b14:	090c           	movi.n	a9, 0
    3b16:	275e92        	s16i	a9, a14, 78
    3b19:	90bbe0        	addx2	a11, a11, a14
    3b1c:	2a5b82        	s16i	a8, a11, 84
    3b1f:	289ef2        	l16si	a15, a14, 80
    3b22:	01e9           	s32i.n	a14, a1, 0
    3b24:	ff1b           	addi.n	a15, a15, 1
    3b26:	8ffd           	ae_sext16	a15, a15
    3b28:	285ef2        	s16i	a15, a14, 80
    3b2b:	04afa6        	blti	a15, 12, 3b33 <beam2_proc+0x20f>
    3b2e:	01e9           	s32i.n	a14, a1, 0
    3b30:	285e92        	s16i	a9, a14, 80

00003b33 <beam2_proc+0x20f>:
    3b33:	01a8           	l32i.n	a10, a1, 0
    3b35:	f1cf81        	l32r	a8, 274 (46d8 <take_min_four>)
    3b38:	cb0c           	movi.n	a11, 12
    3b3a:	54caa2        	addi	a10, a10, 84
    3b3d:	0008e0        	callx8	a8
    3b40:	0abd           	mov.n	a11, a10
    3b42:	01a8           	l32i.n	a10, a1, 0
    3b44:	f19481        	l32r	a8, 194 (64c <siir_safr>)
    3b47:	f1d4c1        	l32r	a12, 298 (69c0 <immediate_num+0x20>)
    3b4a:	faa8           	l32i.n	a10, a10, 60
    3b4c:	0008e0        	callx8	a8
    3b4f:	01e8           	l32i.n	a14, a1, 0
    3b51:	fea9           	s32i.n	a10, a14, 60
    3b53:	000846        	j	3b78 <beam2_proc+0x254>

00003b56 <beam2_proc+0x232>:
    3b56:	b198           	l32i.n	a9, a1, 44
    3b58:	41a8           	l32i.n	a10, a1, 16
    3b5a:	d87c           	movi.n	a8, -3
    3b5c:	02a897        	bge	a8, a9, 3b62 <beam2_proc+0x23e>
    3b5f:	002086        	j	3be5 <beam2_proc+0x2c1>
    3b62:	d1f9           	s32i.n	a15, a1, 52
    3b64:	01e9           	s32i.n	a14, a1, 0
    3b66:	e1d9           	s32i.n	a13, a1, 56
    3b68:	f1c9           	s32i.n	a12, a1, 60
    3b6a:	78c182        	addi	a8, a1, 120
    3b6d:	560894        	ae_sp24x2s.i	aep0, a8, -56
    3b70:	ffbb86        	j	3a62 <beam2_proc+0x13e>

00003b73 <beam2_proc+0x24f>:
    3b73:	ba9a           	add.n	a11, a10, a9
    3b75:	126eb2        	s32i	a11, a14, 72
    3b78:	112ed2        	l32i	a13, a14, 68
    3b7b:	f12dc1        	l32r	a12, 30 (1000 <subband_aec_mid_channel+0x50>)
    3b7e:	61acd7        	bge	a12, a13, 3be3 <beam2_proc+0x2bf>
    3b81:	1c21f2        	l32i	a15, a1, 112
    3b84:	112fe6        	bgei	a15, 2, 3b99 <beam2_proc+0x275>
    3b87:	f1c691        	l32r	a9, 2a0 (4666 <NC_NOISE_EST_Prcs+0x11a>)
    3b8a:	f1c681        	l32r	a8, 2a4 (3b00 <beam2_proc+0x1dc>)
    3b8d:	f1c3a1        	l32r	a10, 29c (7e00 <_end+0x10e4>)
    3b90:	301aa4        	ae_cvtp24a16x2.ll	aep1, a10, a10
    3b93:	302894        	ae_cvtp24a16x2.ll	aep2, a8, a9
    3b96:	000386        	j	3ba8 <beam2_proc+0x284>

00003b99 <beam2_proc+0x275>:
    3b99:	f1c3c1        	l32r	a12, 2a8 (4a3d <NR_vad2+0x31>)
    3b9c:	f1c4b1        	l32r	a11, 2ac (43d7 <ns_ifft_wo+0x8b>)
    3b9f:	269ed2        	l16si	a13, a14, 76
    3ba2:	301dd4        	ae_cvtp24a16x2.ll	aep1, a13, a13
    3ba5:	302bc4        	ae_cvtp24a16x2.ll	aep2, a11, a12

00003ba8 <beam2_proc+0x284>:
    3ba8:	fe88           	l32i.n	a8, a14, 60
    3baa:	1d9ef2        	l16si	a15, a14, 58
    3bad:	360834        	ae_cvtq48a32s	aeq0, a8
    3bb0:	03a204        	ae_mulfq32sp16s.h	aeq2, aeq0, aep2
    3bb3:	11ff10        	slli	a15, a15, 15
    3bb6:	364f34        	ae_cvtq48a32s	aeq1, a15
    3bb9:	099204        	ae_ltq56s	b0, aeq2, aeq1
    3bbc:	230076        	bf	b0, 3be3 <beam2_proc+0x2bf>
    3bbf:	03aa14        	ae_mulfq32sp16s.l	aeq3, aeq0, aep2
    3bc2:	09b514        	ae_leq56s	b1, aeq1, aeq3
    3bc5:	050176        	bf	b1, 3bce <beam2_proc+0x2aa>
    3bc8:	f1ba91        	l32r	a9, 2b0 (7fae <_end+0x1292>)
    3bcb:	301994        	ae_cvtp24a16x2.ll	aep1, a9, a9

00003bce <beam2_proc+0x2aa>:
    3bce:	7168           	l32i.n	a6, a1, 28
    3bd0:	4022e4        	ae_lp16f.i	aep2, a2, -4
    3bd3:	8a0c           	movi.n	a10, 8
    3bd5:	0aaa76        	loopgtz	a10, 3be3 <beam2_proc+0x2bf>
    3bd8:	011274        	ae_mulfp24s.ll	aeq0, aep1, aep2
    3bdb:	f963eca61f 	{ ae_lp16f.iu	aep2, a6, 2; ae_roundsp16q48asym	aep3, aeq0 }
    3be0:	4236f4        	ae_sp16f.l.i	aep3, a6, -2
    3be3:	f01d           	retw.n

00003be5 <beam2_proc+0x2c1>:
    3be5:	b188           	l32i.n	a8, a1, 44
    3be7:	f1c9           	s32i.n	a12, a1, 60
    3be9:	e97c           	movi.n	a9, -2
    3beb:	109897        	bne	a8, a9, 3bff <beam2_proc+0x2db>
    3bee:	21a8           	l32i.n	a10, a1, 8
    3bf0:	d1f9           	s32i.n	a15, a1, 52
    3bf2:	01e9           	s32i.n	a14, a1, 0
    3bf4:	e1d9           	s32i.n	a13, a1, 56
    3bf6:	78c1c2        	addi	a12, a1, 120
    3bf9:	560c94        	ae_sp24x2s.i	aep0, a12, -56
    3bfc:	ff9886        	j	3a62 <beam2_proc+0x13e>

00003bff <beam2_proc+0x2db>:
    3bff:	31a8           	l32i.n	a10, a1, 12
    3c01:	d1f9           	s32i.n	a15, a1, 52
    3c03:	01e9           	s32i.n	a14, a1, 0
    3c05:	e1d9           	s32i.n	a13, a1, 56
    3c07:	f1c9           	s32i.n	a12, a1, 60
    3c09:	78c182        	addi	a8, a1, 120
    3c0c:	560894        	ae_sp24x2s.i	aep0, a8, -56
    3c0f:	ff93c6        	j	3a62 <beam2_proc+0x13e>

00003c12 <beam2_proc+0x2ee>:
    3c12:	b1a8           	l32i.n	a10, a1, 44
    3c14:	f1c9           	s32i.n	a12, a1, 60
    3c16:	d97c           	movi.n	a9, -3
    3c18:	1029a7        	blt	a9, a10, 3c2c <beam2_proc+0x308>
    3c1b:	51a8           	l32i.n	a10, a1, 20
    3c1d:	d1f9           	s32i.n	a15, a1, 52
    3c1f:	01e9           	s32i.n	a14, a1, 0
    3c21:	e1d9           	s32i.n	a13, a1, 56
    3c23:	78c1c2        	addi	a12, a1, 120
    3c26:	560c94        	ae_sp24x2s.i	aep0, a12, -56
    3c29:	ff8d46        	j	3a62 <beam2_proc+0x13e>

00003c2c <beam2_proc+0x308>:
    3c2c:	e97c           	movi.n	a9, -2
    3c2e:	109a97        	bne	a10, a9, 3c42 <beam2_proc+0x31e>
    3c31:	11a8           	l32i.n	a10, a1, 4
    3c33:	d1f9           	s32i.n	a15, a1, 52
    3c35:	01e9           	s32i.n	a14, a1, 0
    3c37:	e1d9           	s32i.n	a13, a1, 56
    3c39:	78c1c2        	addi	a12, a1, 120
    3c3c:	560c94        	ae_sp24x2s.i	aep0, a12, -56
    3c3f:	ff87c6        	j	3a62 <beam2_proc+0x13e>

00003c42 <beam2_proc+0x31e>:
    3c42:	81a8           	l32i.n	a10, a1, 32
    3c44:	d1f9           	s32i.n	a15, a1, 52
    3c46:	01e9           	s32i.n	a14, a1, 0
    3c48:	e1d9           	s32i.n	a13, a1, 56
    3c4a:	78c182        	addi	a8, a1, 120
    3c4d:	560894        	ae_sp24x2s.i	aep0, a8, -56
    3c50:	ff8386        	j	3a62 <beam2_proc+0x13e>

00003c53 <beam2_proc+0x32f>:
	...

00003c54 <beam1_mid_band>:
    3c54:	006136        	entry	a1, 48
    3c57:	6b9232        	l16si	a3, a2, 214
    3c5a:	1b0c           	movi.n	a11, 1
    3c5c:	fc24bd32fe 	{ l32i	a15, a2, 0x34c; ae_zeroq56	aeq0 }
    3c61:	3c13a6        	blti	a3, 1, 3ca1 <beam1_mid_band+0x4d>
    3c64:	a80c           	movi.n	a8, 10
    3c66:	991c           	movi.n	a9, 25
    3c68:	5a1c           	movi.n	a10, 21
    3c6a:	2c0c           	movi.n	a12, 2
    3c6c:	fd046d0f0f 	{ ae_lp16x2f.i	aep0, a15, 0; ae_movq56	aeq1, aeq0 }
    3c71:	72cc30        	salt	a12, a12, a3
    3c74:	93a9c0        	movnez	a10, a9, a12
    3c77:	04a876        	loopgtz	a8, 3c7f <beam1_mid_band+0x2b>
    3c7a:	e2006d8f1f 	{ ae_lp16x2f.iu	aep0, a15, 4; ae_mulaafp24s.hh.ll	aeq1, aep0, aep0 }

00003c7f <beam1_mid_band+0x2b>:
    3c7f:	f18dc1        	l32r	a12, 2b4 (800 <SBAEC_INIT+0xf8>)
    3c82:	401a00        	ssl	a10
    3c85:	3550b4        	ae_sraiq56	aeq1, aeq1, 2
    3c88:	061934        	ae_roundsq32asym	aeq0, aeq1
    3c8b:	368904        	ae_trunca32q48	a9, aeq0
    3c8e:	f11a81        	l32r	a8, f8 (4c80 <airoha_divide>)
    3c91:	a1ab00        	sll	a10, a11
    3c94:	53b9c0        	max	a11, a9, a12
    3c97:	0008e0        	callx8	a8
    3c9a:	0a1084        	ae_zeroq56	aeq0
    3c9d:	000006        	j	3ca1 <beam1_mid_band+0x4d>

00003ca0 <beam1_mid_band+0x4c>:
	...

00003ca1 <beam1_mid_band+0x4d>:
    3ca1:	d422c2        	l32i	a12, a2, 0x350
    3ca4:	040c           	movi.n	a4, 0
    3ca6:	4d0c           	movi.n	a13, 4
    3ca8:	a50c           	movi.n	a5, 10
    3caa:	880c           	movi.n	a8, 8
    3cac:	d22262        	l32i	a6, a2, 0x348
    3caf:	03d2e2        	addmi	a14, a2, 0x300
    3cb2:	0e9d           	mov.n	a9, a14
    3cb4:	5ccef2        	addi	a15, a14, 92
    3cb7:	f3f6f0        	wur.ae_cbegin0	a15
    3cba:	7cc992        	addi	a9, a9, 124
    3cbd:	0199           	s32i.n	a9, a1, 0
    3cbf:	f3f790        	wur.ae_cend0	a9
    3cc2:	d322f2        	l32i	a15, a2, 0x34c
    3cc5:	fcc672        	addi	a7, a6, -4
    3cc8:	54cee2        	addi	a14, a14, 84
    3ccb:	11e9           	s32i.n	a14, a1, 4
    3ccd:	444e04        	ae_lp16x2f.i	aep4, a14, 0
    3cd0:	fd840fe66e 	{ addi	a6, a6, -2; ae_cvtq48p24s.h	aeq1, aep4 }
    3cd5:	fe8c0fcffe 	{ addi	a15, a15, -4; ae_cvtq48p24s.l	aeq2, aep4 }

00003cda <beam1_mid_band+0x86>:
    3cda:	448f14        	ae_lp16x2f.iu	aep0, a15, 4
    3cdd:	fc24e40cdf 	{ ae_sp16x2f.c	aep0, a12, a13; nop }
    3ce2:	440904        	ae_lp16x2f.i	aep0, a9, 0
    3ce5:	0ca876        	loopgtz	a8, 3cf5 <beam1_mid_band+0xa1>
    3ce8:	fc24da9cdf 	{ ae_lp16x2f.c	aep1, a12, a13; nop }
    3ced:	0509c4        	ae_mulssfp24s.hh.ll	aeq1, aep0, aep1
    3cf0:	ef202d891f 	{ ae_lp16x2f.iu	aep0, a9, 4; ae_mulsafp24s.hl.lh	aeq2, aep0, aep1 }

00003cf5 <beam1_mid_band+0xa1>:
    3cf5:	550b           	addi.n	a5, a5, -1
    3cf7:	e0c992        	addi	a9, a9, -32
    3cfa:	f86bedc71f 	{ ae_lp16x2f.iu	aep4, a7, 4; ae_roundsp16q48asym	aep1, aeq1 }
    3cff:	f873b2961f 	{ ae_sp16f.l.iu	aep1, a6, 2; ae_roundsp16q48asym	aep0, aeq2 }
    3d04:	428614        	ae_sp16f.l.iu	aep0, a6, 2
    3d07:	4513a6        	blti	a3, 1, 3d50 <beam1_mid_band+0xfc>
    3d0a:	42e407        	bbsi	a4, 0, 3d50 <beam1_mid_band+0xfc>
    3d0d:	364b34        	ae_cvtq48a32s	aeq1, a11
    3d10:	039254        	ae_mulfq32sp16s.l	aeq2, aeq1, aep1
    3d13:	030a54        	ae_mulfq32sp16s.l	aeq1, aeq1, aep0
    3d16:	fc24feaa8f 	{ ae_slaasq56s	aeq2, aeq2, a10; nop }
    3d1b:	fc24fe9acf 	{ ae_slaasq56s	aeq3, aeq1, a10; nop }
    3d20:	fa7fcfe9ee 	{ addi	a14, a9, -2; ae_roundsp24q48asym	aep5, aeq3 }
    3d25:	f977c0088f 	{ movi	a8, 8; ae_roundsp24q48asym	aep3, aeq2 }
    3d2a:	ddaced090f 	{ ae_lp16x2f.i	aep0, a9, 0; ae_selp24.hl	aep3, aep3, aep5 }
    3d2f:	1aa876        	loopgtz	a8, 3d4d <beam1_mid_band+0xf9>
    3d32:	0a8084        	ae_cvtq48p24s.l	aeq2, aep0

00003d35 <beam1_mid_band+0xe1>:
    3d35:	ff041a8cdf 	{ ae_lp16x2f.c	aep0, a12, a13; ae_cvtq48p24s.h	aeq3, aep0 }
    3d3a:	05b804        	ae_mulaafp24s.hh.ll	aeq3, aep3, aep0
    3d3d:	05b054        	ae_mulasfp24s.hl.lh	aeq2, aep3, aep0
    3d40:	fb7bed891f 	{ ae_lp16x2f.iu	aep0, a9, 4; ae_roundsp16q48asym	aep7, aeq3 }
    3d45:	fb73b2fe1f 	{ ae_sp16f.l.iu	aep7, a14, 2; ae_roundsp16q48asym	aep6, aeq2 }
    3d4a:	42ee14        	ae_sp16f.l.iu	aep6, a14, 2

00003d4d <beam1_mid_band+0xf9>:
    3d4d:	e0c992        	addi	a9, a9, -32
    3d50:	fd8400144e 	{ addi	a4, a4, 1; ae_cvtq48p24s.h	aeq1, aep4 }
    3d55:	fe8c00088f 	{ movi	a8, 8; ae_cvtq48p24s.l	aeq2, aep4 }
    3d5a:	f7c556        	bnez	a5, 3cda <beam1_mid_band+0x86>
    3d5d:	11b8           	l32i.n	a11, a1, 4
    3d5f:	08a0a2        	movi	a10, 8
    3d62:	d462c2        	s32i	a12, a2, 0x350
    3d65:	464b04        	ae_sp16x2f.i	aep4, a11, 0
    3d68:	fd046d090f 	{ ae_lp16x2f.i	aep0, a9, 0; ae_movq56	aeq1, aeq0 }
    3d6d:	04aa76        	loopgtz	a10, 3d75 <beam1_mid_band+0x121>

00003d70 <beam1_mid_band+0x11c>:
    3d70:	e2006d891f 	{ ae_lp16x2f.iu	aep0, a9, 4; ae_mulaafp24s.hh.ll	aeq1, aep0, aep0 }

00003d75 <beam1_mid_band+0x121>:
    3d75:	03d2a2        	addmi	a10, a2, 0x300
    3d78:	3510b4        	ae_sraiq56	aeq0, aeq1, 2
    3d7b:	1b8f14        	ae_roundsp16q48asym	aep1, aeq0
    3d7e:	361d24        	ae_trunca16p24s.l	a13, aep1
    3d81:	0692c2        	l16si	a12, a2, 12
    3d84:	2c5ad2        	s16i	a13, a10, 88
    3d87:	8ddd           	ae_sext16	a13, a13
    3d89:	1eacd7        	bge	a12, a13, 3dab <beam1_mid_band+0x157>
    3d8c:	0198           	l32i.n	a9, a1, 0
    3d8e:	10a0e2        	movi	a14, 16
    3d91:	f13ff1        	l32r	a15, 290 (7efa <_end+0x11de>)
    3d94:	7eca82        	addi	a8, a10, 126
    3d97:	4008f4        	ae_lp16f.i	aep0, a8, -2
    3d9a:	302ff4        	ae_cvtp24a16x2.ll	aep2, a15, a15
    3d9d:	0aae76        	loopgtz	a14, 3dab <beam1_mid_band+0x157>
    3da0:	012874        	ae_mulfp24s.ll	aeq1, aep2, aep0
    3da3:	f96bec891f 	{ ae_lp16f.iu	aep0, a9, 2; ae_roundsp16q48asym	aep3, aeq1 }
    3da8:	4239f4        	ae_sp16f.l.i	aep3, a9, -2

00003dab <beam1_mid_band+0x157>:
    3dab:	f01d           	retw.n

00003dad <beam1_mid_band+0x159>:
    3dad:	000000                                        ...

00003db0 <beam2_mid_band>:
    3db0:	00e136        	entry	a1, 112
    3db3:	02ed           	mov.n	a14, a2
    3db5:	ea2282        	l32i	a8, a2, 0x3a8
    3db8:	03d2b2        	addmi	a11, a2, 0x300
    3dbb:	6b92c2        	l16si	a12, a2, 214
    3dbe:	91c9           	s32i.n	a12, a1, 36
    3dc0:	529b92        	l16si	a9, a11, 164
    3dc3:	a8cbb2        	addi	a11, a11, -88
    3dc6:	b1b9           	s32i.n	a11, a1, 44
    3dc8:	119910        	slli	a9, a9, 15
    3dcb:	04a897        	bge	a8, a9, 3dd3 <beam2_mid_band+0x23>
    3dce:	691c           	movi.n	a9, 22
    3dd0:	000046        	j	3dd5 <beam2_mid_band+0x25>

00003dd3 <beam2_mid_band+0x23>:
    3dd3:	891c           	movi.n	a9, 24

00003dd5 <beam2_mid_band+0x25>:
    3dd5:	91f8           	l32i.n	a15, a1, 36
    3dd7:	b138           	l32i.n	a3, a1, 44
    3dd9:	d12e72        	l32i	a7, a14, 0x344
    3ddc:	d32ec2        	l32i	a12, a14, 0x34c
    3ddf:	d22ed2        	l32i	a13, a14, 0x348
    3de2:	04de82        	addmi	a8, a14, 0x400
    3de5:	084d           	mov.n	a4, a8
    3de7:	d4c8a2        	addi	a10, a8, -44
    3dea:	f3f6a0        	wur.ae_cbegin0	a10
    3ded:	f4c442        	addi	a4, a4, -12
    3df0:	6149           	s32i.n	a4, a1, 24
    3df2:	f3f740        	wur.ae_cend0	a4
    3df5:	fccdd2        	addi	a13, a13, -4
    3df8:	fcccc2        	addi	a12, a12, -4
    3dfb:	fcc772        	addi	a7, a7, -4
    3dfe:	3d2332        	l32i	a3, a3, 244
    3e01:	a0c882        	addi	a8, a8, -96
    3e04:	4189           	s32i.n	a8, a1, 16
    3e06:	440804        	ae_lp16x2f.i	aep0, a8, 0
    3e09:	041fe6        	bgei	a15, 1, 3e11 <beam2_mid_band+0x61>
    3e0c:	6e9eb2        	l16si	a11, a14, 220
    3e0f:	5bcc           	bnez.n	a11, 3e18 <beam2_mid_band+0x68>

00003e11 <beam2_mid_band+0x61>:
    3e11:	0f0c           	movi.n	a15, 0
    3e13:	a1f9           	s32i.n	a15, a1, 40
    3e15:	0000c6        	j	3e1c <beam2_mid_band+0x6c>

00003e18 <beam2_mid_band+0x68>:
    3e18:	180c           	movi.n	a8, 1
    3e1a:	a189           	s32i.n	a8, a1, 40

00003e1c <beam2_mid_band+0x6c>:
    3e1c:	0199           	s32i.n	a9, a1, 0
    3e1e:	880c           	movi.n	a8, 8
    3e20:	fec952        	addi	a5, a9, -2
    3e23:	fcc962        	addi	a6, a9, -4
    3e26:	f8c922        	addi	a2, a9, -8
    3e29:	fac9f2        	addi	a15, a9, -6
    3e2c:	fbc9b2        	addi	a11, a9, -5
    3e2f:	51b9           	s32i.n	a11, a1, 20
    3e31:	21f9           	s32i.n	a15, a1, 8
    3e33:	3129           	s32i.n	a2, a1, 12
    3e35:	7169           	s32i.n	a6, a1, 28
    3e37:	016152        	s32i	a5, a1, 4
    3e3a:	00a062        	movi	a6, 0
    3e3d:	04a052        	movi	a5, 4
    3e40:	0aa022        	movi	a2, 10
    3e43:	04def2        	addmi	a15, a14, 0x400
    3e46:	fd248acffe 	{ addi	a15, a15, -84; ae_zeroq56	aeq1 }
    3e4b:	fc0c5481ff 	{ s32i.n	a15, a1, 32; ae_movq56	aeq0, aeq1 }

00003e50 <beam2_mid_band+0xa0>:
    3e50:	44ad14        	ae_lp16x2f.iu	aep2, a13, 4
    3e53:	fc24e4235f 	{ ae_sp16x2f.c	aep2, a3, a5; nop }
    3e58:	f102ad140f 	{ ae_lp16x2f.i	aep1, a4, 0; ae_mulzaafp24s.hh.ll	aeq2, aep0, aep0 }
    3e5d:	0ca876        	loopgtz	a8, 3e6d <beam2_mid_band+0xbd>
    3e60:	fc24daa35f 	{ ae_lp16x2f.c	aep2, a3, a5; nop }
    3e65:	051204        	ae_mulaafp24s.hh.ll	aeq0, aep1, aep2
    3e68:	ea446d941f 	{ ae_lp16x2f.iu	aep1, a4, 4; ae_mulasfp24s.hl.lh	aeq1, aep1, aep2 }

00003e6d <beam2_mid_band+0xbd>:
    3e6d:	1b9d44        	ae_roundsp24q48asym	aep4, aeq1
    3e70:	1b8d14        	ae_roundsp24q48asym	aep1, aeq0
    3e73:	1a9414        	ae_selp24.ll	aep1, aep1, aep4
    3e76:	0781c4        	ae_subsp24s	aep4, aep0, aep1
    3e79:	07c184        	ae_subsp24s	aep0, aep4, aep1
    3e7c:	090194        	ae_roundsp16asym	aep1, aep0
    3e7f:	049904        	ae_mulzaafp24s.hh.ll	aeq3, aep1, aep1
    3e82:	a198           	l32i.n	a9, a1, 40
    3e84:	d6d80e044e 	{ addi	a4, a4, -32; ae_ltq56s	b0, aeq2, aeq3 }
    3e89:	1f0c           	movi.n	a15, 1
    3e8b:	448c14        	ae_lp16x2f.iu	aep0, a12, 4
    3e8e:	050076        	bf	b0, 3e97 <beam2_mid_band+0xe7>
    3e91:	094494        	ae_movp48	aep1, aep4
    3e94:	000046        	j	3e99 <beam2_mid_band+0xe9>

00003e97 <beam2_mid_band+0xe7>:
    3e97:	0f0c           	movi.n	a15, 0

00003e99 <beam2_mid_band+0xe9>:
    3e99:	0911d4        	ae_roundsp16asym	aep5, aep1
    3e9c:	46d714        	ae_sp16x2f.iu	aep5, a7, 4
    3e9f:	0c7916        	beqz	a9, 3f6a <beam2_mid_band+0x1ba>
    3ea2:	168c           	beqz.n	a6, 3ea7 <beam2_mid_band+0xf7>
    3ea4:	7c5666        	bnei	a6, 5, 3f24 <beam2_mid_band+0x174>

00003ea7 <beam2_mid_band+0xf7>:
    3ea7:	4b0c           	movi.n	a11, 4
    3ea9:	fc248004af 	{ movi	a10, 4; ae_zeroq56	aeq0 }
    3eae:	04aa76        	loopgtz	a10, 3eb6 <beam2_mid_band+0x106>
    3eb1:	fc24daa35f 	{ ae_lp16x2f.c	aep2, a3, a5; nop }

00003eb6 <beam2_mid_band+0x106>:
    3eb6:	c1e9           	s32i.n	a14, a1, 48
    3eb8:	d1d9           	s32i.n	a13, a1, 52
    3eba:	0e61c2        	s32i	a12, a1, 56
    3ebd:	04ab76        	loopgtz	a11, 3ec5 <beam2_mid_band+0x115>

00003ec0 <beam2_mid_band+0x110>:
    3ec0:	e2481aa35f 	{ ae_lp16x2f.c	aep2, a3, a5; ae_mulaafp24s.hh.ll	aeq0, aep2, aep2 }

00003ec5 <beam2_mid_band+0x115>:
    3ec5:	e2483081ae 	{ l32i	a10, a1, 32; ae_mulaafp24s.hh.ll	aeq0, aep2, aep2 }
    3eca:	b198           	l32i.n	a9, a1, 44
    3ecc:	3540b4        	ae_sraiq56	aeq1, aeq0, 2
    3ecf:	ff2c7d4a0f 	{ ae_lq32f.i	aeq2, a10, 0; ae_roundsq32asym	aeq3, aeq1 }
    3ed4:	fa7639ba0f 	{ ae_sq32f.i	aeq3, a10, 0; ae_addq56	aeq2, aeq2, aeq1 }
    3ed9:	fd34668e8e 	{ l16si	a8, a14, 208; ae_roundsq32asym	aeq1, aeq2 }
    3ede:	369b04        	ae_trunca32q48	a11, aeq1
    3ee1:	4269b2        	s32i	a11, a9, 0x108
    3ee4:	0218e6        	bgei	a8, 1, 3eea <beam2_mid_band+0x13a>
    3ee7:	006ec6        	j	40a6 <beam2_mid_band+0x2f6>
    3eea:	ffa782        	movi	a8, 0x7ff
    3eed:	02a8b7        	bge	a8, a11, 3ef3 <beam2_mid_band+0x143>
    3ef0:	004cc6        	j	4027 <beam2_mid_band+0x277>
    3ef3:	71a8           	l32i.n	a10, a1, 28

00003ef5 <beam2_mid_band+0x145>:
    3ef5:	78c192        	addi	a9, a1, 120
    3ef8:	560994        	ae_sp24x2s.i	aep0, a9, -56
    3efb:	0b1fa6        	blti	a15, 1, 3f0a <beam2_mid_band+0x15a>
    3efe:	fecaa2        	addi	a10, a10, -2
    3f01:	c1e9           	s32i.n	a14, a1, 48
    3f03:	d1d9           	s32i.n	a13, a1, 52
    3f05:	e1c9           	s32i.n	a12, a1, 56
    3f07:	560994        	ae_sp24x2s.i	aep0, a9, -56

00003f0a <beam2_mid_band+0x15a>:
    3f0a:	401a00        	ssl	a10
    3f0d:	f07a81        	l32r	a8, f8 (4c80 <airoha_divide>)
    3f10:	1a0c           	movi.n	a10, 1
    3f12:	a1aa00        	sll	a10, a10
    3f15:	0008e0        	callx8	a8
    3f18:	d1d8           	l32i.n	a13, a1, 52
    3f1a:	c1e8           	l32i.n	a14, a1, 48
    3f1c:	78c1c2        	addi	a12, a1, 120
    3f1f:	540c94        	ae_lp24x2.i	aep0, a12, -56
    3f22:	e1c8           	l32i.n	a12, a1, 56

00003f24 <beam2_mid_band+0x174>:
    3f24:	446704        	ae_lp16x2f.i	aep6, a7, 0
    3f27:	360b34        	ae_cvtq48a32s	aeq0, a11
    3f2a:	036a04        	ae_mulfq32sp16s.h	aeq1, aeq0, aep6
    3f2d:	036214        	ae_mulfq32sp16s.l	aeq0, aeq0, aep6
    3f30:	fc24fe9a8f 	{ ae_slaasq56s	aeq2, aeq1, a10; nop }
    3f35:	fc24fe8acf 	{ ae_slaasq56s	aeq3, aeq0, a10; nop }
    3f3a:	fa7fed140f 	{ ae_lp16x2f.i	aep1, a4, 0; ae_roundsp24q48asym	aep5, aeq3 }
    3f3f:	fa778fe49e 	{ addi	a9, a4, -2; ae_roundsp24q48asym	aep4, aeq2 }
    3f44:	deb08008ff 	{ movi	a15, 8; ae_selp24.hl	aep4, aep4, aep5 }
    3f49:	1aaf76        	loopgtz	a15, 3f67 <beam2_mid_band+0x1b7>
    3f4c:	0a4184        	ae_cvtq48p24s.l	aeq1, aep1
    3f4f:	fe241ab35f 	{ ae_lp16x2f.c	aep3, a3, a5; ae_cvtq48p24s.h	aeq2, aep1 }
    3f54:	05c304        	ae_mulaafp24s.hh.ll	aeq2, aep4, aep3
    3f57:	054b54        	ae_mulasfp24s.hl.lh	aeq1, aep4, aep3
    3f5a:	f973ad941f 	{ ae_lp16x2f.iu	aep1, a4, 4; ae_roundsp16q48asym	aep2, aeq2 }
    3f5f:	fb6bf2a91f 	{ ae_sp16f.l.iu	aep2, a9, 2; ae_roundsp16q48asym	aep7, aeq1 }
    3f64:	42f914        	ae_sp16f.l.iu	aep7, a9, 2

00003f67 <beam2_mid_band+0x1b7>:
    3f67:	e0c442        	addi	a4, a4, -32

00003f6a <beam2_mid_band+0x1ba>:
    3f6a:	661b           	addi.n	a6, a6, 1
    3f6c:	fd2480088f 	{ movi	a8, 8; ae_zeroq56	aeq1 }
    3f71:	fc0c4ff22e 	{ addi	a2, a2, -1; ae_movq56	aeq0, aeq1 }
    3f76:	ed6256        	bnez	a2, 3e50 <beam2_mid_band+0xa0>
    3f79:	6148           	l32i.n	a4, a1, 24
    3f7b:	08a082        	movi	a8, 8
    3f7e:	0421a2        	l32i	a10, a1, 16
    3f81:	0b21b2        	l32i	a11, a1, 44
    3f84:	04de92        	addmi	a9, a14, 0x400
    3f87:	f8c922        	addi	a2, a9, -8
    3f8a:	3d6b32        	s32i	a3, a11, 244
    3f8d:	460a04        	ae_sp16x2f.i	aep0, a10, 0
    3f90:	fc24ad29df 	{ ae_lp16x2f.i	aep2, a9, -12; ae_zeroq56	aeq0 }
    3f95:	04a876        	loopgtz	a8, 3f9d <beam2_mid_band+0x1ed>
    3f98:	e2482da41f 	{ ae_lp16x2f.iu	aep2, a4, 4; ae_mulaafp24s.hh.ll	aeq0, aep2, aep2 }

00003f9d <beam2_mid_band+0x1ed>:
    3f9d:	91d8           	l32i.n	a13, a1, 36
    3f9f:	dc7c           	movi.n	a12, -3
    3fa1:	35c0b4        	ae_sraiq56	aeq3, aeq0, 2
    3fa4:	04def2        	addmi	a15, a14, 0x400
    3fa7:	fa7b8a4ffe 	{ addi	a15, a15, -92; ae_roundsp16q48asym	aep4, aeq3 }
    3fac:	424f04        	ae_sp16f.l.i	aep4, a15, 0
    3faf:	02acd7        	bge	a12, a13, 3fb5 <beam2_mid_band+0x205>
    3fb2:	002306        	j	4042 <beam2_mid_band+0x292>
    3fb5:	689e82        	l16si	a8, a14, 208
    3fb8:	086856        	bnez	a8, 4042 <beam2_mid_band+0x292>
    3fbb:	b1c8           	l32i.n	a12, a1, 44
    3fbd:	889c92        	l16si	a9, a12, 0x110
    3fc0:	7e9cb2        	l16si	a11, a12, 252
    3fc3:	432ca2        	l32i	a10, a12, 0x10c
    3fc6:	991b           	addi.n	a9, a9, 1
    3fc8:	899d           	ae_sext16	a9, a9
    3fca:	885c92        	s16i	a9, a12, 0x110
    3fcd:	6cc9a6        	blti	a9, 32, 403d <beam2_mid_band+0x28d>
    3fd0:	0cdd           	mov.n	a13, a12
    3fd2:	436cb2        	s32i	a11, a12, 0x10c
    3fd5:	899c82        	l16si	a8, a12, 0x112
    3fd8:	10caf2        	addi	a15, a10, 16
    3fdb:	21f5f0        	srai	a15, a15, 5
    3fde:	0a0c           	movi.n	a10, 0
    3fe0:	885ca2        	s16i	a10, a12, 0x110
    3fe3:	9088e0        	addx2	a8, a8, a14
    3fe6:	03d882        	addmi	a8, a8, 0x300
    3fe9:	a8c882        	addi	a8, a8, -88
    3fec:	8a58f2        	s16i	a15, a8, 0x114
    3fef:	899cc2        	l16si	a12, a12, 0x112
    3ff2:	cc1b           	addi.n	a12, a12, 1
    3ff4:	8ccd           	ae_sext16	a12, a12
    3ff6:	895dc2        	s16i	a12, a13, 0x112
    3ff9:	04aca6        	blti	a12, 12, 4001 <beam2_mid_band+0x251>
    3ffc:	b188           	l32i.n	a8, a1, 44
    3ffe:	8958a2        	s16i	a10, a8, 0x112

00004001 <beam2_mid_band+0x251>:
    4001:	cb0c           	movi.n	a11, 12
    4003:	f09c81        	l32r	a8, 274 (46d8 <take_min_four>)
    4006:	04dea2        	addmi	a10, a14, 0x400
    4009:	bccaa2        	addi	a10, a10, -68
    400c:	0008e0        	callx8	a8
    400f:	0abd           	mov.n	a11, a10
    4011:	b1a8           	l32i.n	a10, a1, 44
    4013:	f06081        	l32r	a8, 194 (64c <siir_safr>)
    4016:	f0a0c1        	l32r	a12, 298 (69c0 <immediate_num+0x20>)
    4019:	402aa2        	l32i	a10, a10, 0x100
    401c:	0008e0        	callx8	a8
    401f:	b1b8           	l32i.n	a11, a1, 44
    4021:	406ba2        	s32i	a10, a11, 0x100
    4024:	000686        	j	4042 <beam2_mid_band+0x292>

00004027 <beam2_mid_band+0x277>:
    4027:	9198           	l32i.n	a9, a1, 36
    4029:	51a8           	l32i.n	a10, a1, 20
    402b:	d87c           	movi.n	a8, -3
    402d:	022897        	blt	a8, a9, 4033 <beam2_mid_band+0x283>
    4030:	ffb046        	j	3ef5 <beam2_mid_band+0x145>

00004033 <beam2_mid_band+0x283>:
    4033:	e87c           	movi.n	a8, -2
    4035:	689987        	bne	a9, a8, 40a1 <beam2_mid_band+0x2f1>
    4038:	21a8           	l32i.n	a10, a1, 8
    403a:	ffadc6        	j	3ef5 <beam2_mid_band+0x145>

0000403d <beam2_mid_band+0x28d>:
    403d:	9baa           	add.n	a9, a11, a10
    403f:	436c92        	s32i	a9, a12, 0x10c
    4042:	b1e8           	l32i.n	a14, a1, 44
    4044:	effbd1        	l32r	a13, 30 (1000 <subband_aec_mid_channel+0x50>)
    4047:	422ee2        	l32i	a14, a14, 0x108
    404a:	51ade7        	bge	a13, a14, 409f <beam2_mid_band+0x2ef>
    404d:	b188           	l32i.n	a8, a1, 44
    404f:	f096a1        	l32r	a10, 2a8 (4a3d <NR_vad2+0x31>)
    4052:	f07d91        	l32r	a9, 248 (4000 <beam2_mid_band+0x250>)
    4055:	3009a4        	ae_cvtp24a16x2.ll	aep0, a9, a10
    4058:	7e98f2        	l16si	a15, a8, 252
    405b:	402882        	l32i	a8, a8, 0x100
    405e:	364834        	ae_cvtq48a32s	aeq1, a8
    4061:	038244        	ae_mulfq32sp16s.h	aeq2, aeq1, aep0
    4064:	11ff10        	slli	a15, a15, 15
    4067:	360f34        	ae_cvtq48a32s	aeq0, a15
    406a:	098214        	ae_ltq56s	b1, aeq2, aeq0
    406d:	2e0176        	bf	b1, 409f <beam2_mid_band+0x2ef>
    4070:	038a54        	ae_mulfq32sp16s.l	aeq3, aeq1, aep0
    4073:	098324        	ae_ltq56s	b2, aeq3, aeq0
    4076:	0a0276        	bf	b2, 4084 <beam2_mid_band+0x2d4>
    4079:	b1b8           	l32i.n	a11, a1, 44
    407b:	7f9bb2        	l16si	a11, a11, 254
    407e:	302bb4        	ae_cvtp24a16x2.ll	aep2, a11, a11
    4081:	000146        	j	408a <beam2_mid_band+0x2da>

00004084 <beam2_mid_band+0x2d4>:
    4084:	f08bc1        	l32r	a12, 2b0 (7fae <_end+0x1292>)
    4087:	302cc4        	ae_cvtp24a16x2.ll	aep2, a12, a12

0000408a <beam2_mid_band+0x2da>:
    408a:	6198           	l32i.n	a9, a1, 24
    408c:	4012e4        	ae_lp16f.i	aep1, a2, -4
    408f:	0d1c           	movi.n	a13, 16
    4091:	0aad76        	loopgtz	a13, 409f <beam2_mid_band+0x2ef>
    4094:	012174        	ae_mulfp24s.ll	aeq0, aep2, aep1
    4097:	fa63ec991f 	{ ae_lp16f.iu	aep1, a9, 2; ae_roundsp16q48asym	aep5, aeq0 }
    409c:	4259f4        	ae_sp16f.l.i	aep5, a9, -2
    409f:	f01d           	retw.n

000040a1 <beam2_mid_band+0x2f1>:
    40a1:	31a8           	l32i.n	a10, a1, 12
    40a3:	ff9386        	j	3ef5 <beam2_mid_band+0x145>

000040a6 <beam2_mid_band+0x2f6>:
    40a6:	9198           	l32i.n	a9, a1, 36
    40a8:	01a8           	l32i.n	a10, a1, 0
    40aa:	d87c           	movi.n	a8, -3
    40ac:	022897        	blt	a8, a9, 40b2 <beam2_mid_band+0x302>
    40af:	ff9086        	j	3ef5 <beam2_mid_band+0x145>

000040b2 <beam2_mid_band+0x302>:
    40b2:	e87c           	movi.n	a8, -2
    40b4:	049987        	bne	a9, a8, 40bc <beam2_mid_band+0x30c>
    40b7:	11a8           	l32i.n	a10, a1, 4
    40b9:	ff8e06        	j	3ef5 <beam2_mid_band+0x145>

000040bc <beam2_mid_band+0x30c>:
    40bc:	71a8           	l32i.n	a10, a1, 28
    40be:	ff8cc6        	j	3ef5 <beam2_mid_band+0x145>

000040c1 <beam2_mid_band+0x311>:
    40c1:	00000000 36000000                                .......

000040c8 <fill_vad_thrd_Nbands>:
    40c8:	004136        	entry	a1, 32
    40cb:	00a062        	movi	a6, 0
    40ce:	f07a71        	l32r	a7, 2b8 (7f80 <_end+0x1264>)
    40d1:	1ba476        	loopgtz	a4, 40f0 <fill_vad_thrd_Nbands+0x28>
    40d4:	a05620        	addx4	a5, a6, a2
    40d7:	904630        	addx2	a4, a6, a3
    40da:	444d           	ae_l16ui.n	a4, a4, 0
    40dc:	661b           	addi.n	a6, a6, 1
    40de:	866d           	ae_sext16	a6, a6
    40e0:	218140        	srai	a8, a4, 1
    40e3:	744040        	extui	a4, a4, 0, 8
    40e6:	108870        	and	a8, a8, a7
    40e9:	658d           	ae_s16i.n	a8, a5, 0
    40eb:	114490        	slli	a4, a4, 7
    40ee:	754d           	ae_s16i.n	a4, a5, 2

000040f0 <fill_vad_thrd_Nbands+0x28>:
    40f0:	f01d           	retw.n

000040f2 <fill_vad_thrd_Nbands+0x2a>:
	...

000040f4 <fill_EQ_gain>:
    40f4:	004136        	entry	a1, 32
    40f7:	06a476        	loopgtz	a4, 4101 <fill_EQ_gain+0xd>
    40fa:	235d           	ae_l16si.n	a5, a3, 0
    40fc:	09c254        	ae_s16ip	a5, a2, 2
    40ff:	332b           	addi.n	a3, a3, 2

00004101 <fill_EQ_gain+0xd>:
    4101:	f01d           	retw.n

00004103 <fill_EQ_gain+0xf>:
	...

00004104 <complement_para>:
    4104:	004136        	entry	a1, 32
    4107:	f05441        	l32r	a4, 258 (ffff8000 <_end+0xffff12e4>)
    410a:	623d           	ae_s16i.n	a3, a2, 0
    410c:	c04430        	sub	a4, a4, a3
    410f:	724d           	ae_s16i.n	a4, a2, 2
    4111:	f01d           	retw.n

00004113 <complement_para+0xf>:
	...

00004114 <WB_NR_init>:
    4114:	006136        	entry	a1, 48
    4117:	02ad           	mov.n	a10, a2
    4119:	efe981        	l32r	a8, c0 (5608 <xt_memset>)
    411c:	0b0c           	movi.n	a11, 0
    411e:	8ca0c2        	movi	a12, 140
    4121:	0008e0        	callx8	a8
    4124:	efc061        	l32r	a6, 24 (6d14 <p_ecnr_mem>)
    4127:	976c           	movi.n	a7, -23
    4129:	f04051        	l32r	a5, 22c (40000000 <_end+0x3fff92e4>)
    412c:	071356        	bnez	a3, 41a1 <WB_NR_init+0x8d>
    412f:	2c94b2        	l16si	a11, a4, 88
    4132:	f06281        	l32r	a8, 2bc (4104 <complement_para>)
    4135:	241492        	l16ui	a9, a4, 72
    4138:	2214c2        	l16ui	a12, a4, 68
    413b:	2d14d2        	l16ui	a13, a4, 90
    413e:	1e0c           	movi.n	a14, 1
    4140:	06f8           	l32i.n	a15, a6, 0
    4142:	01d2a2        	addmi	a10, a2, 0x100
    4145:	d8caa2        	addi	a10, a10, -40
    4148:	16dff2        	addmi	a15, a15, 0x1600
    414b:	6fed           	ae_s16i.n	a14, a15, 0
    414d:	1052d2        	s16i	a13, a2, 32
    4150:	0252c2        	s16i	a12, a2, 4
    4153:	035292        	s16i	a9, a2, 6
    4156:	0008e0        	callx8	a8
    4159:	a28b           	addi.n	a10, a2, 8
    415b:	f05981        	l32r	a8, 2c0 (40c8 <fill_vad_thrd_Nbands>)
    415e:	70c4b2        	addi	a11, a4, 112
    4161:	4c0c           	movi.n	a12, 4
    4163:	0008e0        	callx8	a8
    4166:	7c0c           	movi.n	a12, 7
    4168:	06a8           	l32i.n	a10, a6, 0
    416a:	78a0b2        	movi	a11, 120
    416d:	f055d1        	l32r	a13, 2c4 (61f0 <win_ns>)
    4170:	3d62d2        	s32i	a13, a2, 244
    4173:	2f42b2        	s8i	a11, a2, 47
    4176:	2e42c2        	s8i	a12, a2, 46
    4179:	0698           	l32i.n	a9, a6, 0
    417b:	32dac2        	addmi	a12, a10, 0x3200
    417e:	33dab2        	addmi	a11, a10, 0x3300
    4181:	9ccbb2        	addi	a11, a11, -100
    4184:	7cccc2        	addi	a12, a12, 124
    4187:	3e62c2        	s32i	a12, a2, 248
    418a:	3f62b2        	s32i	a11, a2, 252
    418d:	31daa2        	addmi	a10, a10, 0x3100
    4190:	35d992        	addmi	a9, a9, 0x3500
    4193:	9ccaa2        	addi	a10, a10, -100
    4196:	4062a2        	s32i	a10, a2, 0x100
    4199:	dcc992        	addi	a9, a9, -36
    419c:	c299           	s32i.n	a9, a2, 48
    419e:	002946        	j	4247 <WB_NR_init+0x133>

000041a1 <WB_NR_init+0x8d>:
    41a1:	fec3e2        	addi	a14, a3, -2
    41a4:	09fe56        	bnez	a14, 4247 <WB_NR_init+0x133>
    41a7:	2c94b2        	l16si	a11, a4, 88
    41aa:	f04481        	l32r	a8, 2bc (4104 <complement_para>)
    41ad:	5014c2        	l16ui	a12, a4, 160
    41b0:	b90c           	movi.n	a9, 11
    41b2:	01d2a2        	addmi	a10, a2, 0x100
    41b5:	2414d2        	l16ui	a13, a4, 72
    41b8:	2d14e2        	l16ui	a14, a4, 90
    41bb:	1052e2        	s16i	a14, a2, 32
    41be:	0352d2        	s16i	a13, a2, 6
    41c1:	d8caa2        	addi	a10, a10, -40
    41c4:	939cc0        	movnez	a9, a12, a12
    41c7:	025292        	s16i	a9, a2, 4
    41ca:	0008e0        	callx8	a8
    41cd:	a28b           	addi.n	a10, a2, 8
    41cf:	f03c81        	l32r	a8, 2c0 (40c8 <fill_vad_thrd_Nbands>)
    41d2:	70c4b2        	addi	a11, a4, 112
    41d5:	4c0c           	movi.n	a12, 4
    41d7:	0008e0        	callx8	a8
    41da:	efb981        	l32r	a8, c0 (5608 <xt_memset>)
    41dd:	f03ac1        	l32r	a12, 2c8 (6210 <win_512_v3_nc>)
    41e0:	06a8           	l32i.n	a10, a6, 0
    41e2:	3d62c2        	s32i	a12, a2, 244
    41e5:	eea0c2        	movi	a12, 238
    41e8:	33dab2        	addmi	a11, a10, 0x3300
    41eb:	2fdaa2        	addmi	a10, a10, 0x2f00
    41ee:	bccbb2        	addi	a11, a11, -68
    41f1:	3e62b2        	s32i	a11, a2, 248
    41f4:	c0caa2        	addi	a10, a10, -64
    41f7:	4262a2        	s32i	a10, a2, 0x108
    41fa:	0b0c           	movi.n	a11, 0
    41fc:	0008e0        	callx8	a8
    41ff:	3c0c           	movi.n	a12, 3
    4201:	ef8781        	l32r	a8, 20 (5620 <xt_memcpy>)
    4204:	090c           	movi.n	a9, 0
    4206:	03d4b2        	addmi	a11, a4, 0x300
    4209:	4a0c           	movi.n	a10, 4
    420b:	0f3c           	movi.n	a15, 48
    420d:	4222d2        	l32i	a13, a2, 0x108
    4210:	01d9           	s32i.n	a13, a1, 0
    4212:	2f42f2        	s8i	a15, a2, 47
    4215:	2e42a2        	s8i	a10, a2, 46
    4218:	06e8           	l32i.n	a14, a6, 0
    421a:	24cbb2        	addi	a11, a11, 36
    421d:	ad2b           	addi.n	a10, a13, 2
    421f:	36dee2        	addmi	a14, a14, 0x3600
    4222:	84cee2        	addi	a14, a14, -124
    4225:	c2e9           	s32i.n	a14, a2, 48
    4227:	6d9d           	ae_s16i.n	a9, a13, 0
    4229:	0008e0        	callx8	a8
    422c:	002192        	l32i	a9, a1, 0
    422f:	26a0b2        	movi	a11, 38
    4232:	065972        	s16i	a7, a9, 12
    4235:	075972        	s16i	a7, a9, 14
    4238:	085972        	s16i	a7, a9, 16
    423b:	01d992        	addmi	a9, a9, 0x100
    423e:	44c992        	addi	a9, a9, 68
    4241:	02ab76        	loopgtz	a11, 4247 <WB_NR_init+0x133>
    4244:	09e954        	ae_s32ip	a5, a9, 4
    4247:	2e02a2        	l8ui	a10, a2, 46
    424a:	2b1aa6        	blti	a10, 1, 4279 <WB_NR_init+0x165>
    424d:	0d0c           	movi.n	a13, 0
    424f:	0b0c           	movi.n	a11, 0
    4251:	f01ea1        	l32r	a10, 2cc (ffffb800 <_end+0xffff4ae4>)
    4254:	f01fe1        	l32r	a14, 2d0 (b8000000 <_end+0xb7ff92e4>)
    4257:	01d2c2        	addmi	a12, a2, 0x100
    425a:	a0ccc2        	addi	a12, a12, -96

0000425d <WB_NR_init+0x149>:
    425d:	c298           	l32i.n	a9, a2, 48
    425f:	cf0c           	movi.n	a15, 12
    4261:	0ce9           	s32i.n	a14, a12, 0
    4263:	8099d0        	add	a9, a9, a13
    4266:	18cdd2        	addi	a13, a13, 24
    4269:	02af76        	loopgtz	a15, 426f <WB_NR_init+0x15b>
    426c:	09c9a4        	ae_s16ip	a10, a9, 2

0000426f <WB_NR_init+0x15b>:
    426f:	2e02f2        	l8ui	a15, a2, 46
    4272:	cc4b           	addi.n	a12, a12, 4
    4274:	bb1b           	addi.n	a11, a11, 1
    4276:	e32bf7        	blt	a11, a15, 425d <WB_NR_init+0x149>

00004279 <WB_NR_init+0x165>:
    4279:	2e94b2        	l16si	a11, a4, 92
    427c:	f01081        	l32r	a8, 2bc (4104 <complement_para>)
    427f:	01d2a2        	addmi	a10, a2, 0x100
    4282:	c4caa2        	addi	a10, a10, -60
    4285:	0008e0        	callx8	a8
    4288:	2f94b2        	l16si	a11, a4, 94
    428b:	f00c81        	l32r	a8, 2bc (4104 <complement_para>)
    428e:	01d2a2        	addmi	a10, a2, 0x100
    4291:	c8caa2        	addi	a10, a10, -56
    4294:	0008e0        	callx8	a8
    4297:	3094b2        	l16si	a11, a4, 96
    429a:	f00881        	l32r	a8, 2bc (4104 <complement_para>)
    429d:	01d2a2        	addmi	a10, a2, 0x100
    42a0:	cccaa2        	addi	a10, a10, -52
    42a3:	0008e0        	callx8	a8
    42a6:	3194b2        	l16si	a11, a4, 98
    42a9:	f00481        	l32r	a8, 2bc (4104 <complement_para>)
    42ac:	01d2a2        	addmi	a10, a2, 0x100
    42af:	d0caa2        	addi	a10, a10, -48
    42b2:	0008e0        	callx8	a8
    42b5:	3294b2        	l16si	a11, a4, 100
    42b8:	f00181        	l32r	a8, 2bc (4104 <complement_para>)
    42bb:	01d2a2        	addmi	a10, a2, 0x100
    42be:	d4caa2        	addi	a10, a10, -44
    42c1:	0008e0        	callx8	a8
    42c4:	6f5272        	s16i	a7, a2, 222
    42c7:	06c8           	l32i.n	a12, a6, 0
    42c9:	04b8           	l32i.n	a11, a4, 0
    42cb:	00a192        	movi	a9, 0x100
    42ce:	0a6c           	movi.n	a10, -32
    42d0:	7894d2        	l16si	a13, a4, 240
    42d3:	8094e2        	l16si	a14, a4, 0x100
    42d6:	efb0f1        	l32r	a15, 198 (7fff0000 <_end+0x7ffe92e4>)
    42d9:	180c           	movi.n	a8, 1
    42db:	155282        	s16i	a8, a2, 42
    42de:	4362f2        	s32i	a15, a2, 0x10c
    42e1:	4462f2        	s32i	a15, a2, 0x110
    42e4:	8b52e2        	s16i	a14, a2, 0x116
    42e7:	8a52d2        	s16i	a13, a2, 0x114
    42ea:	7152a2        	s16i	a10, a2, 226
    42ed:	125292        	s16i	a9, a2, 36
    42f0:	02b9           	s32i.n	a11, a2, 0
    42f2:	1bdcc2        	addmi	a12, a12, 0x1b00
    42f5:	10dbb2        	addmi	a11, a11, 0x1000
    42f8:	78ccc2        	addi	a12, a12, 120
    42fb:	3a62c2        	s32i	a12, a2, 232
    42fe:	b8cbc2        	addi	a12, a11, -72
    4301:	3b62c2        	s32i	a12, a2, 236
    4304:	dccbb2        	addi	a11, a11, -36
    4307:	3c62b2        	s32i	a11, a2, 240
    430a:	1a2326        	beqi	a3, 2, 4328 <WB_NR_init+0x214>
    430d:	2f0292        	l8ui	a9, a2, 47
    4310:	1419a6        	blti	a9, 1, 4328 <WB_NR_init+0x214>
    4313:	0a0c           	movi.n	a10, 0
    4315:	0b0c           	movi.n	a11, 0
    4317:	4022c2        	l32i	a12, a2, 0x100

0000431a <WB_NR_init+0x206>:
    431a:	ecaa           	add.n	a14, a12, a10
    431c:	0e59           	s32i.n	a5, a14, 0
    431e:	2f02d2        	l8ui	a13, a2, 47
    4321:	bb1b           	addi.n	a11, a11, 1
    4323:	aa4b           	addi.n	a10, a10, 4
    4325:	f12bd7        	blt	a11, a13, 431a <WB_NR_init+0x206>
    4328:	f01d           	retw.n

0000432a <WB_NR_init+0x216>:
	...

0000432c <NOISE_GEN_init>:
    432c:	004136        	entry	a1, 32
    432f:	ef3da1        	l32r	a10, 24 (6d14 <p_ecnr_mem>)
    4332:	0b0c           	movi.n	a11, 0
    4334:	ef3dc1        	l32r	a12, 28 (6d0c <aec_nr_para>)
    4337:	0aa8           	l32i.n	a10, a10, 0
    4339:	efe681        	l32r	a8, 2d4 (4114 <WB_NR_init>)
    433c:	0cc8           	l32i.n	a12, a12, 0
    433e:	2ddaa2        	addmi	a10, a10, 0x2d00
    4341:	90caa2        	addi	a10, a10, -112
    4344:	0008e0        	callx8	a8
    4347:	f01d           	retw.n

00004349 <NOISE_GEN_init+0x1d>:
    4349:	000000                                        ...

0000434c <ns_ifft_wo>:
    434c:	004136        	entry	a1, 32
    434f:	efe3a1        	l32r	a10, 2dc (6534 <fftorder_512_12b+0x4>)
    4352:	80a082        	movi	a8, 128
    4355:	0b0c           	movi.n	a11, 0
    4357:	3a2292        	l32i	a9, a2, 232
    435a:	0248           	l32i.n	a4, a2, 0
    435c:	efdfe1        	l32r	a14, 2d8 (fffc <_end+0x92e0>)
    435f:	0304d0        	rsr.br	a13
    4362:	2c0c           	movi.n	a12, 2
    4364:	10dde0        	and	a13, a13, a14
    4367:	20ddc0        	or	a13, a13, a12
    436a:	1304d0        	wsr.br	a13
    436d:	002020        	esync
    4370:	14b9           	s32i.n	a11, a4, 4
    4372:	04b9           	s32i.n	a11, a4, 0
    4374:	5c0904        	ae_lp24x2f.i	aep0, a9, 0
    4377:	0ea876        	loopgtz	a8, 4389 <ns_ifft_wo+0x3d>
    437a:	2afd           	ae_l16si.n	a15, a10, 0
    437c:	fc18404aae 	{ addi	a10, a10, 4; ae_negsp24s	aep1, aep0 }
    4381:	e0042f891f 	{ ae_lp24x2f.iu	aep0, a9, 8; ae_movtp24x2	aep1, aep0, b0:b1 }
    4386:	5f14f4        	ae_sp24x2f.x	aep1, a4, a15

00004389 <ns_ifft_wo+0x3d>:
    4389:	5c89e4        	ae_lp24x2f.iu	aep0, a9, -16
    438c:	7fa082        	movi	a8, 127
    438f:	09a876        	loopgtz	a8, 439c <ns_ifft_wo+0x50>
    4392:	2abd           	ae_l16si.n	a11, a10, 0
    4394:	aa4b           	addi.n	a10, a10, 4
    4396:	5f04b4        	ae_sp24x2f.x	aep0, a4, a11
    4399:	5c89f4        	ae_lp24x2f.iu	aep0, a9, -8
    439c:	04ad           	mov.n	a10, a4
    439e:	00a1b2        	movi	a11, 0x100
    43a1:	8c0c           	movi.n	a12, 8
    43a3:	efcf81        	l32r	a8, 2e0 (5540 <sw_fft_32b_2811>)
    43a6:	0d0c           	movi.n	a13, 0
    43a8:	1e0c           	movi.n	a14, 1
    43aa:	0008e0        	callx8	a8
    43ad:	fec3b2        	addi	a11, a3, -2
    43b0:	600404        	ae_lq32f.i	aeq0, a4, 0
    43b3:	3f2292        	l32i	a9, a2, 252
    43b6:	3d22c2        	l32i	a12, a2, 244
    43b9:	1392e2        	l16si	a14, a2, 38
    43bc:	fec992        	addi	a9, a9, -2
    43bf:	feccd2        	addi	a13, a12, -2
    43c2:	c0aae0        	sub	a10, a10, a14
    43c5:	20ccc2        	addi	a12, a12, 32
    43c8:	f8caa2        	addi	a10, a10, -8
    43cb:	0e1c           	movi.n	a14, 16
    43cd:	1cae76        	loopgtz	a14, 43ed <ns_ifft_wo+0xa1>
    43d0:	40dcf4        	ae_lp16f.iu	aep5, a12, -2
    43d3:	fc24fe8a8f 	{ ae_slaasq56s	aeq2, aeq0, a10; nop }
    43d8:	fd546cc91f 	{ ae_lp16f.iu	aep4, a9, 2; ae_roundsq32sym	aeq1, aeq2 }
    43dd:	eab0acbd1f 	{ ae_lp16f.iu	aep3, a13, 2; ae_mulfp24s.hl	aeq0, aep4, aep5 }
    43e2:	033054        	ae_mulafq32sp16s.l	aeq0, aeq1, aep3

000043e5 <ns_ifft_wo+0x99>:
    43e5:	f9e3bd142f 	{ ae_lq32f.iu	aeq0, a4, 8; ae_roundsp16q48sym	aep2, aeq0 }
    43ea:	42ab14        	ae_sp16f.l.iu	aep2, a11, 2

000043ed <ns_ifft_wo+0xa1>:
    43ed:	e0a0d2        	movi	a13, 224
    43f0:	0fad76        	loopgtz	a13, 4403 <ns_ifft_wo+0xb7>
    43f3:	fc24fe8a0f 	{ ae_slaasq56s	aeq0, aeq0, a10; nop }
    43f8:	06c834        	ae_roundsq32sym	aeq3, aeq0
    43fb:	fbfbbd142f 	{ ae_lq32f.iu	aeq0, a4, 8; ae_roundsp16q48sym	aep6, aeq3 }
    4400:	42eb14        	ae_sp16f.l.iu	aep6, a11, 2
    4403:	3f2292        	l32i	a9, a2, 252
    4406:	0e1c           	movi.n	a14, 16
    4408:	fec992        	addi	a9, a9, -2
    440b:	f03d           	nop.n
    440d:	0fae76        	loopgtz	a14, 4420 <ns_ifft_wo+0xd4>
    4410:	fc24fe8a8f 	{ ae_slaasq56s	aeq2, aeq0, a10; nop }
    4415:	066834        	ae_roundsq32sym	aeq1, aeq2
    4418:	fbebfd142f 	{ ae_lq32f.iu	aeq0, a4, 8; ae_roundsp16q48sym	aep7, aeq1 }
    441d:	42f914        	ae_sp16f.l.iu	aep7, a9, 2
    4420:	f01d           	retw.n

00004422 <ns_ifft_wo+0xd6>:
	...

00004424 <NOISE_GEN_Prcs>:
    4424:	004136        	entry	a1, 32
    4427:	eeff41        	l32r	a4, 24 (6d14 <p_ecnr_mem>)
    442a:	209220        	or	a9, a2, a2
    442d:	efada1        	l32r	a10, 2e4 (6538 <fftorder_512_12b+0x8>)
    4430:	002442        	l32i	a4, a4, 0
    4433:	441204        	ae_lp16x2f.i	aep1, a2, 0
    4436:	78a082        	movi	a8, 120
    4439:	2dd442        	addmi	a4, a4, 0x2d00
    443c:	90c442        	addi	a4, a4, -112
    443f:	3d24d2        	l32i	a13, a4, 244
    4442:	0024c2        	l32i	a12, a4, 0
    4445:	3e24e2        	l32i	a14, a4, 248
    4448:	fc20a124fe 	{ l16si	a15, a4, 36; ae_zerop48	aep0 }
    444d:	04a876        	loopgtz	a8, 4455 <NOISE_GEN_Prcs+0x31>
    4450:	d821ad991f 	{ ae_lp16x2f.iu	aep1, a9, 4; ae_maxabssp24s	aep0, aep0, aep1 }

00004455 <NOISE_GEN_Prcs+0x31>:
    4455:	dc01c0009f 	{ movi	a9, 0; ae_selp24.lh	aep1, aep0, aep0 }
    445a:	d82240088f 	{ movi	a8, 8; ae_maxp24s	aep1, aep0, aep1 }
    445f:	0015d4        	ae_cvta32p24.l	a5, aep1
    4462:	04dcc2        	addmi	a12, a12, 0x400
    4465:	bc4b           	addi.n	a11, a12, 4
    4467:	40e550        	nsa	a5, a5
    446a:	125452        	s16i	a5, a4, 36
    446d:	4355f0        	min	a5, a5, a15
    4470:	031654        	wur.ae_sar	a5
    4473:	440e04        	ae_lp16x2f.i	aep0, a14, 0
    4476:	f823ad1d0f 	{ ae_lp16x2f.i	aep1, a13, 0; ae_sllssp24s	aep0, aep0 }
    447b:	f03d           	nop.n
    447d:	18a876        	loopgtz	a8, 4499 <NOISE_GEN_Prcs+0x75>
    4480:	010944        	ae_mulfp24s.hh	aeq1, aep0, aep1
    4483:	010174        	ae_mulfp24s.ll	aeq0, aep0, aep1
    4486:	629c94        	ae_sq32f.x	aeq1, a12, a9
    4489:	628b94        	ae_sq32f.x	aeq0, a11, a9
    448c:	449d14        	ae_lp16x2f.iu	aep1, a13, 4
    448f:	44ae14        	ae_lp16x2f.iu	aep2, a14, 4
    4492:	2a9d           	ae_l16si.n	a9, a10, 0
    4494:	f82b808aae 	{ addi	a10, a10, 8; ae_sllssp24s	aep0, aep2 }
    4499:	440204        	ae_lp16x2f.i	aep0, a2, 0
    449c:	70a0e2        	movi	a14, 112
    449f:	14ae76        	loopgtz	a14, 44b7 <NOISE_GEN_Prcs+0x93>
    44a2:	0b00b4        	ae_sllssp24s	aep3, aep0
    44a5:	0acb84        	ae_cvtq48p24s.h	aeq3, aep3
    44a8:	fe6c387c9f 	{ ae_sq32f.x	aeq3, a12, a9; ae_cvtq48p24s.l	aeq2, aep3 }
    44ad:	62ab94        	ae_sq32f.x	aeq2, a11, a9
    44b0:	2a9d           	ae_l16si.n	a9, a10, 0
    44b2:	448214        	ae_lp16x2f.iu	aep0, a2, 4
    44b5:	aa8b           	addi.n	a10, a10, 8
    44b7:	3e24e2        	l32i	a14, a4, 248
    44ba:	8f0c           	movi.n	a15, 8
    44bc:	fccee2        	addi	a14, a14, -4
    44bf:	19af76        	loopgtz	a15, 44dc <NOISE_GEN_Prcs+0xb8>
    44c2:	fa23adddff 	{ ae_lp16x2f.iu	aep5, a13, -4; ae_sllssp24s	aep4, aep0 }
    44c7:	014d54        	ae_mulfp24s.hl	aeq1, aep4, aep5
    44ca:	e8b1b38e1f 	{ ae_sp16x2f.iu	aep0, a14, 4; ae_mulfp24s.lh	aeq0, aep4, aep5 }
    44cf:	629c94        	ae_sq32f.x	aeq1, a12, a9
    44d2:	628b94        	ae_sq32f.x	aeq0, a11, a9
    44d5:	2a9d           	ae_l16si.n	a9, a10, 0
    44d7:	448214        	ae_lp16x2f.iu	aep0, a2, 4
    44da:	aa8b           	addi.n	a10, a10, 8
    44dc:	80a0b2        	movi	a11, 128
    44df:	8c0c           	movi.n	a12, 8
    44e1:	1d0c           	movi.n	a13, 1
    44e3:	04a8           	l32i.n	a10, a4, 0
    44e5:	ef7e81        	l32r	a8, 2e0 (5540 <sw_fft_32b_2811>)
    44e8:	0e0c           	movi.n	a14, 0
    44ea:	04daa2        	addmi	a10, a10, 0x400
    44ed:	0008e0        	callx8	a8
    44f0:	00a1b2        	movi	a11, 0x100
    44f3:	c095a0        	sub	a9, a5, a10
    44f6:	ef7c81        	l32r	a8, 2e8 (475c <gen_power>)
    44f9:	135492        	s16i	a9, a4, 38
    44fc:	04ad           	mov.n	a10, a4
    44fe:	0008e0        	callx8	a8
    4501:	ef7a81        	l32r	a8, 2ec (4908 <NR_vad1>)
    4504:	04ad           	mov.n	a10, a4
    4506:	0008e0        	callx8	a8
    4509:	ef7981        	l32r	a8, 2f0 (4a0c <NR_vad2>)
    450c:	04ad           	mov.n	a10, a4
    450e:	0008e0        	callx8	a8
    4511:	ef7881        	l32r	a8, 2f4 (4b84 <NR_NoiseEstimate>)
    4514:	04ad           	mov.n	a10, a4
    4516:	0008e0        	callx8	a8
    4519:	ef7781        	l32r	a8, 2f8 (500 <comfort_noise_gen_wb2>)
    451c:	04ad           	mov.n	a10, a4
    451e:	0008e0        	callx8	a8
    4521:	ef7681        	l32r	a8, 2fc (434c <ns_ifft_wo>)
    4524:	03bd           	mov.n	a11, a3
    4526:	04ad           	mov.n	a10, a4
    4528:	0008e0        	callx8	a8
    452b:	f01d           	retw.n

0000452d <NOISE_GEN_Prcs+0x109>:
    452d:	000000                                        ...

00004530 <NC_NOISE_EST_init>:
    4530:	004136        	entry	a1, 32
    4533:	eebca1        	l32r	a10, 24 (6d14 <p_ecnr_mem>)
    4536:	02cd           	mov.n	a12, a2
    4538:	0aa8           	l32i.n	a10, a10, 0
    453a:	2b0c           	movi.n	a11, 2
    453c:	ef6681        	l32r	a8, 2d4 (4114 <WB_NR_init>)
    453f:	2edaa2        	addmi	a10, a10, 0x2e00
    4542:	a8caa2        	addi	a10, a10, -88
    4545:	0008e0        	callx8	a8
    4548:	f01d           	retw.n

0000454a <NC_NOISE_EST_init+0x1a>:
	...

0000454c <NC_NOISE_EST_Prcs>:
    454c:	006136        	entry	a1, 48
    454f:	03bd           	mov.n	a11, a3
    4551:	ef6b81        	l32r	a8, 300 (47a8 <fft_wo>)
    4554:	2ed232        	addmi	a3, a2, 0x2e00
    4557:	a8c332        	addi	a3, a3, -88
    455a:	03ad           	mov.n	a10, a3
    455c:	0008e0        	callx8	a8
    455f:	ef6381        	l32r	a8, 2ec (4908 <NR_vad1>)
    4562:	03ad           	mov.n	a10, a3
    4564:	0008e0        	callx8	a8
    4567:	ef6281        	l32r	a8, 2f0 (4a0c <NR_vad2>)
    456a:	03ad           	mov.n	a10, a3
    456c:	0008e0        	callx8	a8
    456f:	2dd242        	addmi	a4, a2, 0x2d00
    4572:	662492        	l32i	a9, a4, 0x198
    4575:	b4c452        	addi	a5, a4, -76
    4578:	6c2442        	l32i	a4, a4, 0x1b0
    457b:	09a8           	l32i.n	a10, a9, 0
    457d:	160c           	movi.n	a6, 1
    457f:	0904c2        	l8ui	a12, a4, 9
    4582:	141ae6        	bgei	a10, 1, 459a <NC_NOISE_EST_Prcs+0x4e>
    4585:	1988           	l32i.n	a8, a9, 4
    4587:	0f18e6        	bgei	a8, 1, 459a <NC_NOISE_EST_Prcs+0x4e>
    458a:	29a8           	l32i.n	a10, a9, 8
    458c:	0a1ae6        	bgei	a10, 1, 459a <NC_NOISE_EST_Prcs+0x4e>
    458f:	39b8           	l32i.n	a11, a9, 12
    4591:	051be6        	bgei	a11, 1, 459a <NC_NOISE_EST_Prcs+0x4e>
    4594:	060c           	movi.n	a6, 0
    4596:	000006        	j	459a <NC_NOISE_EST_Prcs+0x4e>

00004599 <NC_NOISE_EST_Prcs+0x4d>:
	...

0000459a <NC_NOISE_EST_Prcs+0x4e>:
    459a:	44a172        	movi	a7, 0x144
    459d:	bccc           	bnez.n	a12, 45ac <NC_NOISE_EST_Prcs+0x60>
    459f:	0804d2        	l8ui	a13, a4, 8
    45a2:	6dcc           	bnez.n	a13, 45ac <NC_NOISE_EST_Prcs+0x60>
    45a4:	46cc           	bnez.n	a6, 45ac <NC_NOISE_EST_Prcs+0x60>
    45a6:	66a632        	movi	a3, 0x666
    45a9:	000046        	j	45ae <NC_NOISE_EST_Prcs+0x62>

000045ac <NC_NOISE_EST_Prcs+0x60>:
    45ac:	030c           	movi.n	a3, 0

000045ae <NC_NOISE_EST_Prcs+0x62>:
    45ae:	0894c2        	l16si	a12, a4, 16
    45b1:	0694d2        	l16si	a13, a4, 12
    45b4:	01d4a2        	addmi	a10, a4, 0x100
    45b7:	acca22        	addi	a2, a10, -84
    45ba:	121dc7        	beq	a13, a12, 45d0 <NC_NOISE_EST_Prcs+0x84>
    45bd:	44cab2        	addi	a11, a10, 68
    45c0:	ef5181        	l32r	a8, 304 (4734 <equal_power>)
    45c3:	6e2c           	movi.n	a14, 38
    45c5:	02ad           	mov.n	a10, a2
    45c7:	0008e0        	callx8	a8
    45ca:	0654a2        	s16i	a10, a4, 12
    45cd:	0854a2        	s16i	a10, a4, 16

000045d0 <NC_NOISE_EST_Prcs+0x84>:
    45d0:	6a2c           	movi.n	a10, 38
    45d2:	01d4c2        	addmi	a12, a4, 0x100
    45d5:	eed8b1        	l32r	a11, 138 (8000 <_end+0x12e4>)
    45d8:	40cc92        	addi	a9, a12, 64
    45db:	ccccc2        	addi	a12, a12, -52
    45de:	608c84        	ae_lq32f.i	aeq2, a12, -32
    45e1:	c0bb30        	sub	a11, a11, a3
    45e4:	fc249883bf 	{ ae_cvtp24a16x2.ll	aep0, a3, a11; ae_zeroq56	aeq0 }
    45e9:	19aa76        	loopgtz	a10, 4606 <NC_NOISE_EST_Prcs+0xba>
    45ec:	605914        	ae_lq32f.iu	aeq1, a9, 4
    45ef:	63cc           	bnez.n	a3, 45f9 <NC_NOISE_EST_Prcs+0xad>
    45f1:	f86b3d521f 	{ ae_lq32f.iu	aeq2, a2, 4; ae_maxq56s	aeq0, aeq0, aeq1 }
    45f6:	000286        	j	4604 <NC_NOISE_EST_Prcs+0xb8>

000045f9 <NC_NOISE_EST_Prcs+0xad>:
    45f9:	c18864        	ae_mulzaafq32sp16s.lh	aeq3, aeq1, aep0, aeq2, aep0
    45fc:	609214        	ae_lq32f.iu	aeq2, a2, 4
    45ff:	f87b39b90f 	{ ae_sq32f.i	aeq3, a9, 0; ae_maxq56s	aeq0, aeq0, aeq3 }

00004604 <NC_NOISE_EST_Prcs+0xb8>:
    4604:	f03d           	nop.n

00004606 <NC_NOISE_EST_Prcs+0xba>:
    4606:	364b04        	ae_nsaq56s	a11, aeq0
    4609:	14c4a2        	addi	a10, a4, 20
    460c:	8c0c           	movi.n	a12, 8
    460e:	21acb7        	bge	a12, a11, 4633 <NC_NOISE_EST_Prcs+0xe7>
    4611:	0a9d           	mov.n	a9, a10
    4613:	26a0d2        	movi	a13, 38
    4616:	0694e2        	l16si	a14, a4, 12
    4619:	f8cbf2        	addi	a15, a11, -8
    461c:	401f00        	ssl	a15
    461f:	c0eef0        	sub	a14, a14, a15
    4622:	0654e2        	s16i	a14, a4, 12
    4625:	0aad76        	loopgtz	a13, 4633 <NC_NOISE_EST_Prcs+0xe7>
    4628:	4c2982        	l32i	a8, a9, 0x130
    462b:	994b           	addi.n	a9, a9, 4
    462d:	a18800        	sll	a8, a8
    4630:	4b6982        	s32i	a8, a9, 0x12c

00004633 <NC_NOISE_EST_Prcs+0xe7>:
    4633:	3d2592        	l32i	a9, a5, 244
    4636:	e89582        	l16si	a8, a5, 0x1d0
    4639:	6b2c           	movi.n	a11, 38
    463b:	0bab76        	loopgtz	a11, 464a <NC_NOISE_EST_Prcs+0xfe>
    463e:	994b           	addi.n	a9, a9, 4
    4640:	0ae8           	l32i.n	a14, a10, 0
    4642:	266ae2        	s32i	a14, a10, 152
    4645:	29d8           	l32i.n	a13, a9, 8
    4647:	09ead4        	ae_s32ip	a13, a10, 4

0000464a <NC_NOISE_EST_Prcs+0xfe>:
    464a:	34fd           	ae_l16si.n	a15, a4, 2
    464c:	fc24a0749e 	{ l16si	a9, a4, 14; ae_zeroq56	aeq0 }
    4651:	0804a2        	l8ui	a10, a4, 8
    4654:	084462        	s8i	a6, a4, 8
    4657:	085492        	s16i	a9, a4, 16
    465a:	0944a2        	s8i	a10, a4, 9
    465d:	075482        	s16i	a8, a4, 14
    4660:	072f16        	beqz	a15, 46d6 <NC_NOISE_EST_Prcs+0x18a>
    4663:	947a           	add.n	a9, a4, a7
    4665:	fb1c           	movi.n	a11, 31
    4667:	01d4d2        	addmi	a13, a4, 0x100
    466a:	0694a2        	l16si	a10, a4, 12
    466d:	64cdd2        	addi	a13, a13, 100
    4670:	604d84        	ae_lq32f.i	aeq1, a13, -32
    4673:	c0aca0        	sub	a10, a12, a10
    4676:	43aab0        	min	a10, a10, a11
    4679:	01aad6        	bgez	a10, 4697 <NC_NOISE_EST_Prcs+0x14b>
    467c:	60a0a0        	neg	a10, a10
    467f:	6e2c           	movi.n	a14, 38
    4681:	f03d           	nop.n
    4683:	f03d           	nop.n
    4685:	09ae76        	loopgtz	a14, 4692 <NC_NOISE_EST_Prcs+0x146>
    4688:	fc24fe9a4f 	{ ae_slaasq56s	aeq1, aeq1, a10; nop }

0000468d <NC_NOISE_EST_Prcs+0x141>:
    468d:	f8667d391f 	{ ae_lq32f.iu	aeq1, a9, 4; ae_addsq56s	aeq0, aeq0, aeq1 }

00004692 <NC_NOISE_EST_Prcs+0x146>:
    4692:	000386        	j	46a4 <NC_NOISE_EST_Prcs+0x158>

00004695 <NC_NOISE_EST_Prcs+0x149>:
	...

00004697 <NC_NOISE_EST_Prcs+0x14b>:
    4697:	6f2c           	movi.n	a15, 38
    4699:	07af76        	loopgtz	a15, 46a4 <NC_NOISE_EST_Prcs+0x158>
    469c:	349ab4        	ae_sraaq56	aeq2, aeq1, a10
    469f:	f8e27d391f 	{ ae_lq32f.iu	aeq1, a9, 4; ae_addsq56s	aeq0, aeq0, aeq2 }

000046a4 <NC_NOISE_EST_Prcs+0x158>:
    46a4:	ef1991        	l32r	a9, 308 (35e51 <_end+0x2f135>)
    46a7:	398994        	ae_movpa24x2	aep0, a9, a9
    46aa:	080d24        	ae_mulfq32sp24s.l	aeq1, aeq0, aep0
    46ad:	06d834        	ae_roundsq32sym	aeq3, aeq1
    46b0:	623104        	ae_sq32f.i	aeq3, a1, 0
    46b3:	0188           	l32i.n	a8, a1, 0
    46b5:	349d           	ae_l16si.n	a9, a4, 2
    46b7:	24ad           	ae_l16si.n	a10, a4, 0
    46b9:	218880        	srai	a8, a8, 8
    46bc:	07a897        	bge	a8, a9, 46c7 <NC_NOISE_EST_Prcs+0x17b>
    46bf:	029492        	l16si	a9, a4, 4
    46c2:	99aa           	add.n	a9, a9, a10
    46c4:	000146        	j	46cd <NC_NOISE_EST_Prcs+0x181>

000046c7 <NC_NOISE_EST_Prcs+0x17b>:
    46c7:	039492        	l16si	a9, a4, 6
    46ca:	c09a90        	sub	a9, a10, a9

000046cd <NC_NOISE_EST_Prcs+0x181>:
    46cd:	a9bd           	ae_clamps16	a11, a9
    46cf:	0a0c           	movi.n	a10, 0
    46d1:	53aab0        	max	a10, a10, a11
    46d4:	64ad           	ae_s16i.n	a10, a4, 0

000046d6 <NC_NOISE_EST_Prcs+0x18a>:
    46d6:	f01d           	retw.n

000046d8 <take_min_four>:
    46d8:	004136        	entry	a1, 32
    46db:	228d           	ae_l16si.n	a8, a2, 0
    46dd:	3f23a6        	blti	a3, 2, 4720 <take_min_four+0x48>
    46e0:	ee7d91        	l32r	a9, d4 (7fff <_end+0x12e3>)
    46e3:	ee7c61        	l32r	a6, d4 (7fff <_end+0x12e3>)
    46e6:	ee7b71        	l32r	a7, d4 (7fff <_end+0x12e3>)
    46e9:	222b           	addi.n	a2, a2, 2
    46eb:	430b           	addi.n	a4, a3, -1
    46ed:	2ca476        	loopgtz	a4, 471d <take_min_four+0x45>
    46f0:	223d           	ae_l16si.n	a3, a2, 0
    46f2:	222b           	addi.n	a2, a2, 2
    46f4:	0aa387        	bge	a3, a8, 4702 <take_min_four+0x2a>
    46f7:	069d           	mov.n	a9, a6
    46f9:	076d           	mov.n	a6, a7
    46fb:	087d           	mov.n	a7, a8
    46fd:	038d           	mov.n	a8, a3
    46ff:	000606        	j	471b <take_min_four+0x43>

00004702 <take_min_four+0x2a>:
    4702:	08a377        	bge	a3, a7, 470e <take_min_four+0x36>
    4705:	069d           	mov.n	a9, a6
    4707:	076d           	mov.n	a6, a7
    4709:	037d           	mov.n	a7, a3
    470b:	000306        	j	471b <take_min_four+0x43>

0000470e <take_min_four+0x36>:
    470e:	06a367        	bge	a3, a6, 4718 <take_min_four+0x40>
    4711:	069d           	mov.n	a9, a6
    4713:	036d           	mov.n	a6, a3
    4715:	000086        	j	471b <take_min_four+0x43>

00004718 <take_min_four+0x40>:
    4718:	439390        	min	a9, a3, a9
    471b:	f03d           	nop.n

0000471d <take_min_four+0x45>:
    471d:	000206        	j	4729 <take_min_four+0x51>

00004720 <take_min_four+0x48>:
    4720:	ee6d61        	l32r	a6, d4 (7fff <_end+0x12e3>)
    4723:	ee6c91        	l32r	a9, d4 (7fff <_end+0x12e3>)
    4726:	ee6b71        	l32r	a7, d4 (7fff <_end+0x12e3>)

00004729 <take_min_four+0x51>:
    4729:	469a           	add.n	a4, a6, a9
    472b:	287a           	add.n	a2, a8, a7
    472d:	224a           	add.n	a2, a2, a4
    472f:	112220        	slli	a2, a2, 14
    4732:	f01d           	retw.n

00004734 <equal_power>:
    4734:	004136        	entry	a1, 32
    4737:	c08450        	sub	a8, a4, a5
    473a:	0078d6        	bgez	a8, 4745 <equal_power+0x11>
    473d:	023d           	mov.n	a3, a2
    473f:	c08540        	sub	a8, a5, a4
    4742:	000046        	j	4747 <equal_power+0x13>

00004745 <equal_power+0x11>:
    4745:	045d           	mov.n	a5, a4

00004747 <equal_power+0x13>:
    4747:	fcc332        	addi	a3, a3, -4
    474a:	08a676        	loopgtz	a6, 4756 <equal_power+0x22>
    474d:	601314        	ae_lq32f.iu	aeq0, a3, 4
    4750:	3408a4        	ae_srlaq56	aeq0, aeq0, a8
    4753:	620304        	ae_sq32f.i	aeq0, a3, 0

00004756 <equal_power+0x22>:
    4756:	052d           	mov.n	a2, a5
    4758:	f01d           	retw.n

0000475a <equal_power+0x26>:
	...

0000475c <gen_power>:
    475c:	004136        	entry	a1, 32
    475f:	3a2262        	l32i	a6, a2, 232
    4762:	0258           	l32i.n	a5, a2, 0
    4764:	217130        	srai	a7, a3, 1
    4767:	f8c632        	addi	a3, a6, -8
    476a:	0d17a6        	blti	a7, 1, 477b <gen_power+0x1f>
    476d:	05a776        	loopgtz	a7, 4776 <gen_power+0x1a>
    4770:	5c8514        	ae_lp24x2f.iu	aep0, a5, 8
    4773:	5e8314        	ae_sp24x2f.iu	aep0, a3, 8

00004776 <gen_power+0x1a>:
    4776:	0258           	l32i.n	a5, a2, 0
    4778:	3a2262        	l32i	a6, a2, 232

0000477b <gen_power+0x1f>:
    477b:	170c           	movi.n	a7, 1
    477d:	139282        	l16si	a8, a2, 38
    4780:	2f0242        	l8ui	a4, a2, 47
    4783:	fcc552        	addi	a5, a5, -4
    4786:	063d           	mov.n	a3, a6
    4788:	5c0604        	ae_lp24x2f.i	aep0, a6, 0
    478b:	443b           	addi.n	a4, a4, 3
    478d:	1188f0        	slli	a8, a8, 1
    4790:	c07780        	sub	a7, a7, a8
    4793:	6e5272        	s16i	a7, a2, 220
    4796:	0aa476        	loopgtz	a4, 47a4 <gen_power+0x48>
    4799:	040824        	ae_mulzaap24s.hh.ll	aeq1, aep0, aep0
    479c:	fc4c6f831f 	{ ae_lp24x2f.iu	aep0, a3, 8; ae_roundsq32sym	aeq0, aeq1 }
    47a1:	624514        	ae_sq32f.iu	aeq0, a5, 4

000047a4 <gen_power+0x48>:
    47a4:	f01d           	retw.n

000047a6 <gen_power+0x4a>:
	...

000047a8 <fft_wo>:
    47a8:	008136        	entry	a1, 64
    47ab:	2e0292        	l8ui	a9, a2, 46
    47ae:	eecbb1        	l32r	a11, 2dc (6534 <fftorder_512_12b+0x4>)
    47b1:	260c           	movi.n	a6, 2
    47b3:	0289f6        	bgeui	a9, 8, 47b9 <fft_wo+0x11>
    47b6:	084966        	bnei	a9, 4, 47c2 <fft_wo+0x1a>

000047b9 <fft_wo+0x11>:
    47b9:	f0a052        	movi	a5, 240
    47bc:	00a242        	movi	a4, 0x200
    47bf:	000286        	j	47cd <fft_wo+0x25>

000047c2 <fft_wo+0x1a>:
    47c2:	78a052        	movi	a5, 120
    47c5:	00a142        	movi	a4, 0x100
    47c8:	eec7b1        	l32r	a11, 2e4 (6538 <fftorder_512_12b+0x8>)
    47cb:	460c           	movi.n	a6, 4

000047cd <fft_wo+0x25>:
    47cd:	02f8           	l32i.n	a15, a2, 0
    47cf:	470c           	movi.n	a7, 4
    47d1:	1a15a6        	blti	a5, 1, 47ef <fft_wo+0x47>
    47d4:	090c           	movi.n	a9, 0
    47d6:	03ad           	mov.n	a10, a3
    47d8:	1185f0        	slli	a8, a5, 1
    47db:	5189           	s32i.n	a8, a1, 20
    47dd:	09a576        	loopgtz	a5, 47ea <fft_wo+0x42>
    47e0:	2acd           	ae_l16si.n	a12, a10, 0
    47e2:	aa2b           	addi.n	a10, a10, 2
    47e4:	60c1c0        	abs	a12, a12
    47e7:	2099c0        	or	a9, a9, a12

000047ea <fft_wo+0x42>:
    47ea:	000206        	j	47f6 <fft_wo+0x4e>

000047ed <fft_wo+0x45>:
	...

000047ef <fft_wo+0x47>:
    47ef:	090c           	movi.n	a9, 0
    47f1:	11d5f0        	slli	a13, a5, 1
    47f4:	51d9           	s32i.n	a13, a1, 20

000047f6 <fft_wo+0x4e>:
    47f6:	11d6f0        	slli	a13, a6, 1
    47f9:	d27470        	quos	a7, a4, a7
    47fc:	3d22a2        	l32i	a10, a2, 244
    47ff:	a0f4f0        	addx4	a15, a4, a15
    4802:	0c0c           	movi.n	a12, 0
    4804:	40e980        	nsa	a8, a9
    4807:	11e4e0        	slli	a14, a4, 2
    480a:	129292        	l16si	a9, a2, 36
    480d:	41e9           	s32i.n	a14, a1, 16
    480f:	f0c882        	addi	a8, a8, -16
    4812:	5388c0        	max	a8, a8, a12
    4815:	1189           	s32i.n	a8, a1, 4
    4817:	ef4b           	addi.n	a14, a15, 4
    4819:	438890        	min	a8, a8, a9
    481c:	3e2292        	l32i	a9, a2, 248
    481f:	fe24ad1a0f 	{ ae_lp16x2f.i	aep1, a10, 0; ae_zeroq56	aeq2 }
    4824:	031684        	wur.ae_sar	a8
    4827:	f9d22d090f 	{ ae_lp16x2f.i	aep0, a9, 0; ae_truncp24q48x2	aep2, aeq2, aeq2 }
    482c:	f82394018f 	{ s32i.n	a8, a1, 0; ae_sllssp24s	aep0, aep0 }
    4831:	17a776        	loopgtz	a7, 484c <fft_wo+0xa4>
    4834:	010944        	ae_mulfp24s.hh	aeq1, aep0, aep1
    4837:	010174        	ae_mulfp24s.ll	aeq0, aep0, aep1
    483a:	629fc4        	ae_sq32f.x	aeq1, a15, a12
    483d:	628ec4        	ae_sq32f.x	aeq0, a14, a12
    4840:	2bcd           	ae_l16si.n	a12, a11, 0
    4842:	449914        	ae_lp16x2f.iu	aep1, a9, 4
    4845:	bbda           	add.n	a11, a11, a13
    4847:	f827ad9a1f 	{ ae_lp16x2f.iu	aep1, a10, 4; ae_sllssp24s	aep0, aep1 }

0000484c <fft_wo+0xa4>:
    484c:	2d0c           	movi.n	a13, 2
    484e:	3e2282        	l32i	a8, a2, 248
    4851:	d2d5d0        	quos	a13, a5, a13
    4854:	21d9           	s32i.n	a13, a1, 8
    4856:	c0d7d0        	sub	a13, a7, a13
    4859:	5178           	l32i.n	a7, a1, 20
    485b:	002152        	l32i	a5, a1, 0
    485e:	0361d2        	s32i	a13, a1, 12
    4861:	809870        	add	a9, a8, a7
    4864:	450874        	ae_lp16x2f.x	aep0, a8, a7
    4867:	fcc872        	addi	a7, a8, -4
    486a:	032182        	l32i	a8, a1, 12
    486d:	09ad76        	loopgtz	a13, 487a <fft_wo+0xd2>
    4870:	fd0873871f 	{ ae_sp16x2f.iu	aep0, a7, 4; ae_abssp24s	aep3, aep0 }
    4875:	dd682d891f 	{ ae_lp16x2f.iu	aep0, a9, 4; ae_orp48	aep2, aep2, aep3 }
    487a:	449af4        	ae_lp16x2f.iu	aep1, a10, -4
    487d:	11d6f0        	slli	a13, a6, 1
    4880:	1fa876        	loopgtz	a8, 48a3 <fft_wo+0xfb>
    4883:	0b00d4        	ae_sllssp24s	aep5, aep0
    4886:	015154        	ae_mulfp24s.hl	aeq0, aep5, aep1
    4889:	e935f3871f 	{ ae_sp16x2f.iu	aep0, a7, 4; ae_mulfp24s.lh	aeq3, aep5, aep1 }
    488e:	628fc4        	ae_sq32f.x	aeq0, a15, a12
    4891:	62bec4        	ae_sq32f.x	aeq3, a14, a12
    4894:	449af4        	ae_lp16x2f.iu	aep1, a10, -4

00004897 <fft_wo+0xef>:
    4897:	fe082d891f 	{ ae_lp16x2f.iu	aep0, a9, 4; ae_abssp24s	aep4, aep0 }
    489c:	2bcd           	ae_l16si.n	a12, a11, 0
    489e:	dd8810bbdf 	{ add	a11, a11, a13; ae_orp48	aep2, aep2, aep4 }

000048a3 <fft_wo+0xfb>:
    48a3:	00a8d4        	ae_cvta32p24.h	a8, aep2
    48a6:	002dd4        	ae_cvta32p24.l	a13, aep2
    48a9:	1198           	l32i.n	a9, a1, 4
    48ab:	20dd80        	or	a13, a13, a8
    48ae:	2188           	l32i.n	a8, a1, 8
    48b0:	40edd0        	nsa	a13, a13
    48b3:	4399d0        	min	a9, a9, a13
    48b6:	125292        	s16i	a9, a2, 36
    48b9:	11d6f0        	slli	a13, a6, 1
    48bc:	039d           	mov.n	a9, a3
    48be:	440304        	ae_lp16x2f.i	aep0, a3, 0
    48c1:	1aa876        	loopgtz	a8, 48df <fft_wo+0x137>
    48c4:	0b00e4        	ae_sllssp24s	aep6, aep0
    48c7:	01e154        	ae_mulfp24s.hl	aeq2, aep6, aep1
    48ca:	e839f3871f 	{ ae_sp16x2f.iu	aep0, a7, 4; ae_mulfp24s.lh	aeq1, aep6, aep1 }
    48cf:	62afc4        	ae_sq32f.x	aeq2, a15, a12
    48d2:	629ec4        	ae_sq32f.x	aeq1, a14, a12
    48d5:	449af4        	ae_lp16x2f.iu	aep1, a10, -4
    48d8:	2bcd           	ae_l16si.n	a12, a11, 0
    48da:	448914        	ae_lp16x2f.iu	aep0, a9, 4
    48dd:	bbda           	add.n	a11, a11, a13
    48df:	8c0c           	movi.n	a12, 8
    48e1:	1d0c           	movi.n	a13, 1
    48e3:	0e0c           	movi.n	a14, 0
    48e5:	41f8           	l32i.n	a15, a1, 16
    48e7:	02a8           	l32i.n	a10, a2, 0
    48e9:	21b140        	srai	a11, a4, 1
    48ec:	ee7d81        	l32r	a8, 2e0 (5540 <sw_fft_32b_2811>)
    48ef:	8bbd           	ae_sext16	a11, a11
    48f1:	aafa           	add.n	a10, a10, a15
    48f3:	0008e0        	callx8	a8
    48f6:	04bd           	mov.n	a11, a4
    48f8:	c095a0        	sub	a9, a5, a10
    48fb:	ee7b81        	l32r	a8, 2e8 (475c <gen_power>)
    48fe:	02ad           	mov.n	a10, a2
    4900:	135292        	s16i	a9, a2, 38
    4903:	0008e0        	callx8	a8
    4906:	f01d           	retw.n

00004908 <NR_vad1>:
    4908:	004136        	entry	a1, 32
    490b:	0238           	l32i.n	a3, a2, 0
    490d:	2e0272        	l8ui	a7, a2, 46
    4910:	33cb           	addi.n	a3, a3, 12
    4912:	0787b6        	bltui	a7, 8, 491d <NR_vad1+0x15>
    4915:	ee7d41        	l32r	a4, 30c (6cb0 <wb_sb_ptn>)
    4918:	000106        	j	4920 <NR_vad1+0x18>

0000491b <NR_vad1+0x13>:
	...

0000491d <NR_vad1+0x15>:
    491d:	ee7c41        	l32r	a4, 310 (6ce0 <nb_sb_ptn>)

00004920 <NR_vad1+0x18>:
    4920:	608304        	ae_lq32f.i	aeq2, a3, 0
    4923:	047716        	beqz	a7, 496e <NR_vad1+0x66>
    4926:	906740        	addx2	a6, a7, a4
    4929:	fd2480005f 	{ movi	a5, 0; ae_zeroq56	aeq1 }

0000492e <NR_vad1+0x26>:
    492e:	249d           	ae_l16si.n	a9, a4, 0
    4930:	348d           	ae_l16si.n	a8, a4, 2
    4932:	c08890        	sub	a8, a8, a9
    4935:	04a876        	loopgtz	a8, 493d <NR_vad1+0x35>
    4938:	f9ea3d531f 	{ ae_lq32f.iu	aeq2, a3, 4; ae_addq56	aeq1, aeq1, aeq2 }

0000493d <NR_vad1+0x35>:
    493d:	365d04        	ae_nsaq56s	a13, aeq1
    4940:	6e92b2        	l16si	a11, a2, 220
    4943:	ee1281        	l32r	a8, 18c (60c <Log2_norm>)
    4946:	f8cdc2        	addi	a12, a13, -8
    4949:	c0bbd0        	sub	a11, a11, a13
    494c:	fc24fe9c4f 	{ ae_slaasq56s	aeq1, aeq1, a12; nop }
    4951:	369a04        	ae_trunca32q48	a10, aeq1
    4954:	bb8b           	addi.n	a11, a11, 8
    4956:	0008e0        	callx8	a8
    4959:	3b22e2        	l32i	a14, a2, 236
    495c:	608304        	ae_lq32f.i	aeq2, a3, 0
    495f:	442b           	addi.n	a4, a4, 2
    4961:	ee5a           	add.n	a14, a14, a5
    4963:	006ea2        	s32i	a10, a14, 0
    4966:	fd2480455e 	{ addi	a5, a5, 4; ae_zeroq56	aeq1 }
    496b:	bf9467        	bne	a4, a6, 492e <NR_vad1+0x26>

0000496e <NR_vad1+0x66>:
    496e:	34c2c2        	addi	a12, a2, 52
    4971:	0392f2        	l16si	a15, a2, 6
    4974:	3b2292        	l32i	a9, a2, 236
    4977:	54c2b2        	addi	a11, a2, 84
    497a:	608b84        	ae_lq32f.i	aeq2, a11, -32
    497d:	604904        	ae_lq32f.i	aeq1, a9, 0
    4980:	11ff00        	slli	a15, a15, 16
    4983:	360f34        	ae_cvtq48a32s	aeq0, a15
    4986:	049716        	beqz	a7, 49d3 <NR_vad1+0xcb>
    4989:	01d2e2        	addmi	a14, a2, 0x100
    498c:	00a0a2        	movi	a10, 0
    498f:	c4cef2        	addi	a15, a14, -60
    4992:	c8cee2        	addi	a14, a14, -56
    4995:	349776        	loopnez	a7, 49cd <NR_vad1+0xc5>
    4998:	099224        	ae_ltq56s	b2, aeq2, aeq1
    499b:	0f8d           	mov.n	a8, a15
    499d:	c38e20        	movf	a8, a14, b2
    49a0:	441804        	ae_lp16x2f.i	aep1, a8, 0
    49a3:	c19164        	ae_mulzaafq32sp16s.lh	aeq2, aeq1, aep1, aeq2, aep1
    49a6:	622c04        	ae_sq32f.i	aeq2, a12, 0
    49a9:	60db14        	ae_lq32f.iu	aeq3, a11, 4
    49ac:	09b114        	ae_ltq56s	b1, aeq1, aeq3
    49af:	0fdd           	mov.n	a13, a15
    49b1:	c3de10        	movf	a13, a14, b1
    49b4:	440d04        	ae_lp16x2f.i	aep0, a13, 0
    49b7:	c18874        	ae_mulzaafq32sp16s.lh	aeq3, aeq1, aep0, aeq3, aep0
    49ba:	fab6c06a8e 	{ addi	a8, a10, 6; ae_subq56	aeq2, aeq2, aeq3 }
    49bf:	d65039bb0f 	{ ae_sq32f.i	aeq3, a11, 0; ae_ltq56s	b0, aeq0, aeq2 }
    49c4:	605914        	ae_lq32f.iu	aeq1, a9, 4
    49c7:	609c14        	ae_lq32f.iu	aeq2, a12, 4
    49ca:	d3a800        	movt	a10, a8, b0

000049cd <NR_vad1+0xc5>:
    49cd:	3b2292        	l32i	a9, a2, 236
    49d0:	000046        	j	49d5 <NR_vad1+0xcd>

000049d3 <NR_vad1+0xcb>:
    49d3:	0a0c           	movi.n	a10, 0

000049d5 <NR_vad1+0xcd>:
    49d5:	051ae6        	bgei	a10, 1, 49de <NR_vad1+0xd6>
    49d8:	ee4fa1        	l32r	a10, 314 (6a1c <NR_immediate_num+0xc>)
    49db:	000086        	j	49e1 <NR_vad1+0xd9>

000049de <NR_vad1+0xd6>:
    49de:	ee4ea1        	l32r	a10, 318 (6a18 <NR_immediate_num+0x8>)

000049e1 <NR_vad1+0xd9>:
    49e1:	604904        	ae_lq32f.i	aeq1, a9, 0
    49e4:	1092b2        	l16si	a11, a2, 32
    49e7:	440a04        	ae_lp16x2f.i	aep0, a10, 0
    49ea:	7cc2a2        	addi	a10, a2, 124
    49ed:	11bb00        	slli	a11, a11, 16
    49f0:	360b34        	ae_cvtq48a32s	aeq0, a11
    49f3:	608a04        	ae_lq32f.i	aeq2, a10, 0
    49f6:	0d9776        	loopnez	a7, 4a07 <NR_vad1+0xff>
    49f9:	c18864        	ae_mulzaafq32sp16s.lh	aeq3, aeq1, aep0, aeq2, aep0
    49fc:	605914        	ae_lq32f.iu	aeq1, a9, 4
    49ff:	fb7b3d5a1f 	{ ae_lq32f.iu	aeq2, a10, 4; ae_maxq56s	aeq3, aeq0, aeq3 }
    4a04:	623af4        	ae_sq32f.i	aeq3, a10, -4

00004a07 <NR_vad1+0xff>:
    4a07:	f01d           	retw.n

00004a09 <NR_vad1+0x101>:
    4a09:	000000                                        ...

00004a0c <NR_vad2>:
    4a0c:	006136        	entry	a1, 48
    4a0f:	851c           	movi.n	a5, 24
    4a11:	a0a0f2        	movi	a15, 160
    4a14:	2e02a2        	l8ui	a10, a2, 46
    4a17:	7cc2b2        	addi	a11, a2, 124
    4a1a:	1512e2        	l16ui	a14, a2, 42
    4a1d:	021282        	l16ui	a8, a2, 4
    4a20:	01d2d2        	addmi	a13, a2, 0x100
    4a23:	80cdc2        	addi	a12, a13, -128
    4a26:	0218e7        	beq	a8, a14, 4a2c <NR_vad2+0x20>
    4a29:	002186        	j	4ab3 <NR_vad2+0xa7>
    4a2c:	01a9           	s32i.n	a10, a1, 0
    4a2e:	109262        	l16si	a6, a2, 32
    4a31:	a0cd82        	addi	a8, a13, -96
    4a34:	090c           	movi.n	a9, 0
    4a36:	155292        	s16i	a9, a2, 42
    4a39:	1189           	s32i.n	a8, a1, 4
    4a3b:	08d662        	addmi	a6, a6, 0x800
    4a3e:	116600        	slli	a6, a6, 16
    4a41:	aaac           	beqz.n	a10, 4a6f <NR_vad2+0x63>
    4a43:	084d           	mov.n	a4, a8
    4a45:	093d           	mov.n	a3, a9
    4a47:	21c9           	s32i.n	a12, a1, 8
    4a49:	31b9           	s32i.n	a11, a1, 12
    4a4b:	907aa0        	addx2	a7, a10, a10
    4a4e:	1177d0        	slli	a7, a7, 3

00004a51 <NR_vad2+0x45>:
    4a51:	c2a8           	l32i.n	a10, a2, 48
    4a53:	ee0881        	l32r	a8, 274 (46d8 <take_min_four>)
    4a56:	cb0c           	movi.n	a11, 12
    4a58:	aa3a           	add.n	a10, a10, a3
    4a5a:	0008e0        	callx8	a8
    4a5d:	18c332        	addi	a3, a3, 24
    4a60:	53b6a0        	max	a11, a6, a10
    4a63:	09e4b4        	ae_s32ip	a11, a4, 4
    4a66:	e79377        	bne	a3, a7, 4a51 <NR_vad2+0x45>
    4a69:	01a8           	l32i.n	a10, a1, 0
    4a6b:	31b8           	l32i.n	a11, a1, 12
    4a6d:	21c8           	l32i.n	a12, a1, 8

00004a6f <NR_vad2+0x63>:
    4a6f:	c298           	l32i.n	a9, a2, 48
    4a71:	2d02f2        	l8ui	a15, a2, 45
    4a74:	21c9           	s32i.n	a12, a1, 8
    4a76:	31b9           	s32i.n	a11, a1, 12
    4a78:	01a9           	s32i.n	a10, a1, 0
    4a7a:	01a8           	l32i.n	a10, a1, 0
    4a7c:	31b8           	l32i.n	a11, a1, 12
    4a7e:	21c8           	l32i.n	a12, a1, 8
    4a80:	ff1b           	addi.n	a15, a15, 1
    4a82:	0bdd           	mov.n	a13, a11
    4a84:	74f0f0        	extui	a15, a15, 0, 8
    4a87:	2d42f2        	s8i	a15, a2, 45
    4a8a:	06af66        	bnei	a15, 12, 4a94 <NR_vad2+0x88>
    4a8d:	0f0c           	movi.n	a15, 0
    4a8f:	080c           	movi.n	a8, 0
    4a91:	2d4282        	s8i	a8, a2, 45

00004a94 <NR_vad2+0x88>:
    4a94:	600cf4        	ae_lq32f.i	aeq0, a12, -4
    4a97:	909f90        	addx2	a9, a15, a9
    4a9a:	e8c992        	addi	a9, a9, -24
    4a9d:	079a76        	loopnez	a10, 4aa8 <NR_vad2+0x9c>
    4aa0:	f8e3bd1d1f 	{ ae_lq32f.iu	aeq0, a13, 4; ae_roundsp16q48sym	aep0, aeq0 }
    4aa5:	438954        	ae_sp16f.l.xu	aep0, a9, a5

00004aa8 <NR_vad2+0x9c>:
    4aa8:	1198           	l32i.n	a9, a1, 4
    4aaa:	1512e2        	l16ui	a14, a2, 42
    4aad:	000a46        	j	4ada <NR_vad2+0xce>

00004ab0 <NR_vad2+0xa4>:
    4ab0:	000000                                        ...

00004ab3 <NR_vad2+0xa7>:
    4ab3:	c288           	l32i.n	a8, a2, 48
    4ab5:	2d0292        	l8ui	a9, a2, 45
    4ab8:	0bdd           	mov.n	a13, a11
    4aba:	600cf4        	ae_lq32f.i	aeq0, a12, -4
    4abd:	909980        	addx2	a9, a9, a8
    4ac0:	e8c992        	addi	a9, a9, -24
    4ac3:	1a9c           	beqz.n	a10, 4ad8 <NR_vad2+0xcc>
    4ac5:	0c9a76        	loopnez	a10, 4ad5 <NR_vad2+0xc9>
    4ac8:	f9e39a195f 	{ ae_lp16f.xu	aep1, a9, a5; ae_roundsp16q48sym	aep2, aeq0 }
    4acd:	d846fd1d1f 	{ ae_lq32f.iu	aeq0, a13, 4; ae_minp24s	aep1, aep1, aep2 }
    4ad2:	421904        	ae_sp16f.l.i	aep1, a9, 0

00004ad5 <NR_vad2+0xc9>:
    4ad5:	1512e2        	l16ui	a14, a2, 42

00004ad8 <NR_vad2+0xcc>:
    4ad8:	92fa           	add.n	a9, a2, a15

00004ada <NR_vad2+0xce>:
    4ada:	de1b           	addi.n	a13, a14, 1
    4adc:	1552d2        	s16i	a13, a2, 42
    4adf:	0c8ab6        	bltui	a10, 8, 4aef <NR_vad2+0xe3>
    4ae2:	ee0ff1        	l32r	a15, 320 (6c90 <wb_score_bands>)
    4ae5:	ee0de1        	l32r	a14, 31c (e38e4 <_end+0xdcbc8>)
    4ae8:	39aee4        	ae_movpa24x2	aep2, a14, a14
    4aeb:	000246        	j	4af8 <NR_vad2+0xec>

00004aee <NR_vad2+0xe2>:
	...

00004aef <NR_vad2+0xe3>:
    4aef:	ee0df1        	l32r	a15, 324 (6cd0 <nb_score_bands>)
    4af2:	ee0d81        	l32r	a8, 328 (124925 <_end+0x11dc09>)
    4af5:	39a884        	ae_movpa24x2	aep2, a8, a8

00004af8 <NR_vad2+0xec>:
    4af8:	09ed           	mov.n	a14, a9
    4afa:	3c2292        	l32i	a9, a2, 240
    4afd:	fecff2        	addi	a15, a15, -2
    4b00:	0bdd           	mov.n	a13, a11
    4b02:	fe248fc9be 	{ addi	a11, a9, -4; ae_zeroq56	aeq2 }
    4b07:	fc20c0629e 	{ addi	a9, a2, 6; ae_zerop48	aep1 }
    4b0c:	224a66        	bnei	a10, 4, 4b32 <NR_vad2+0x126>
    4b0f:	604e04        	ae_lq32f.i	aeq1, a14, 0
    4b12:	600d04        	ae_lq32f.i	aeq0, a13, 0
    4b15:	179a76        	loopnez	a10, 4b30 <NR_vad2+0x124>
    4b18:	fa26ecc91f 	{ ae_lp16f.iu	aep4, a9, 2; ae_subq56	aeq2, aeq0, aeq1 }
    4b1d:	0ac484        	ae_cvtq48p24s.l	aeq3, aep4
    4b20:	fab6fd3e1f 	{ ae_lq32f.iu	aeq1, a14, 4; ae_subq56	aeq2, aeq2, aeq3 }
    4b25:	35a074        	ae_sraiq56	aeq2, aeq2, 1
    4b28:	f9d27d1d1f 	{ ae_lq32f.iu	aeq0, a13, 4; ae_truncp24q48x2	aep3, aeq2, aeq2 }
    4b2d:	52bb14        	ae_sp24f.l.iu	aep3, a11, 4

00004b30 <NR_vad2+0x124>:
    4b30:	f01d           	retw.n

00004b32 <NR_vad2+0x126>:
    4b32:	600cf4        	ae_lq32f.i	aeq0, a12, -4
    4b35:	01d282        	addmi	a8, a2, 0x100
    4b38:	c0c882        	addi	a8, a8, -64
    4b3b:	604884        	ae_lq32f.i	aeq1, a8, -32
    4b3e:	259a76        	loopnez	a10, 4b67 <NR_vad2+0x15b>
    4b41:	f826ecd91f 	{ ae_lp16f.iu	aep5, a9, 2; ae_subq56	aeq0, aeq0, aeq1 }
    4b46:	0ac584        	ae_cvtq48p24s.l	aeq3, aep5
    4b49:	098304        	ae_ltq56s	b0, aeq3, aeq0
    4b4c:	f5caac8f1f 	{ ae_lp16f.iu	aep0, a15, 2; ae_mulafq32sp24s.l	aeq2, aeq0, aep2 }
    4b51:	020076        	bf	b0, 4b57 <NR_vad2+0x14b>
    4b54:	079014        	ae_addsp24s	aep1, aep1, aep0

00004b57 <NR_vad2+0x14b>:
    4b57:	f9a6fd1d1f 	{ ae_lq32f.iu	aeq0, a13, 4; ae_subq56	aeq1, aeq0, aeq3 }
    4b5c:	355074        	ae_sraiq56	aeq1, aeq1, 1
    4b5f:	fb4e7d3e1f 	{ ae_lq32f.iu	aeq1, a14, 4; ae_truncp24q48x2	aep7, aeq1, aeq1 }
    4b64:	52fb14        	ae_sp24f.l.iu	aep7, a11, 4

00004b67 <NR_vad2+0x15b>:
    4b67:	408914        	ae_lp16f.iu	aep0, a9, 2
    4b6a:	0ac084        	ae_cvtq48p24s.l	aeq3, aep0
    4b6d:	09a314        	ae_ltq56s	b1, aeq3, aeq2
    4b70:	408f14        	ae_lp16f.iu	aep0, a15, 2
    4b73:	28c292        	addi	a9, a2, 40
    4b76:	020176        	bf	b1, 4b7c <NR_vad2+0x170>
    4b79:	079014        	ae_addsp24s	aep1, aep1, aep0

00004b7c <NR_vad2+0x170>:
    4b7c:	421904        	ae_sp16f.l.i	aep1, a9, 0
    4b7f:	f01d           	retw.n

00004b81 <NR_vad2+0x175>:
    4b81:	000000                                        ...

00004b84 <NR_NoiseEstimate>:
    4b84:	004136        	entry	a1, 32
    4b87:	0c0c           	movi.n	a12, 0
    4b89:	0022f2        	l32i	a15, a2, 0
    4b8c:	0e92b2        	l16si	a11, a2, 28
    4b8f:	2f0262        	l8ui	a6, a2, 47
    4b92:	04dfa2        	addmi	a10, a15, 0x400
    4b95:	11bbc0        	slli	a11, a11, 4
    4b98:	06dff2        	addmi	a15, a15, 0x600
    4b9b:	e0cff2        	addi	a15, a15, -32
    4b9e:	209ff0        	or	a9, a15, a15
    4ba1:	059676        	loopnez	a6, 4baa <NR_NoiseEstimate+0x26>
    4ba4:	09eab4        	ae_s32ip	a11, a10, 4
    4ba7:	09e9c4        	ae_s32ip	a12, a9, 4

00004baa <NR_NoiseEstimate+0x26>:
    4baa:	1412a2        	l16ui	a10, a2, 40
    4bad:	4c1c           	movi.n	a12, 20
    4baf:	493ca7        	bltu	a12, a10, 4bfc <NR_NoiseEstimate+0x78>
    4bb2:	2e0292        	l8ui	a9, a2, 46
    4bb5:	027ab6        	bltui	a10, 7, 4bbb <NR_NoiseEstimate+0x37>
    4bb8:	002dc6        	j	4c73 <NR_NoiseEstimate+0xef>
    4bbb:	3322a2        	l32i	a10, a2, 204
    4bbe:	0000c6        	j	4bc5 <NR_NoiseEstimate+0x41>

00004bc1 <NR_NoiseEstimate+0x3d>:
	...

00004bc2 <NR_NoiseEstimate+0x3e>:
    4bc2:	3522a2        	l32i	a10, a2, 212

00004bc5 <NR_NoiseEstimate+0x41>:
    4bc5:	39bc           	beqz.n	a9, 4bfc <NR_NoiseEstimate+0x78>
    4bc7:	11d9e0        	slli	a13, a9, 2
    4bca:	0b0c           	movi.n	a11, 0
    4bcc:	edd0c1        	l32r	a12, 30c (6cb0 <wb_sb_ptn>)
    4bcf:	140c           	movi.n	a4, 1

00004bd1 <NR_NoiseEstimate+0x4d>:
    4bd1:	3c22e2        	l32i	a14, a2, 240
    4bd4:	eeba           	add.n	a14, a14, a11
    4bd6:	0ee8           	l32i.n	a14, a14, 0
    4bd8:	bb4b           	addi.n	a11, a11, 4
    4bda:	191ee6        	bgei	a14, 1, 4bf7 <NR_NoiseEstimate+0x73>
    4bdd:	2c3d           	ae_l16si.n	a3, a12, 0
    4bdf:	3ced           	ae_l16si.n	a14, a12, 2
    4be1:	a093f0        	addx4	a9, a3, a15
    4be4:	0fa3e7        	bge	a3, a14, 4bf7 <NR_NoiseEstimate+0x73>
    4be7:	c08e30        	sub	a8, a14, a3
    4bea:	538840        	max	a8, a8, a4
    4bed:	06a876        	loopgtz	a8, 4bf7 <NR_NoiseEstimate+0x73>
    4bf0:	09e8           	l32i.n	a14, a9, 0
    4bf2:	eeaa           	add.n	a14, a14, a10
    4bf4:	09e9e4        	ae_s32ip	a14, a9, 4
    4bf7:	cc2b           	addi.n	a12, a12, 2
    4bf9:	d49bd7        	bne	a11, a13, 4bd1 <NR_NoiseEstimate+0x4d>
    4bfc:	fccf52        	addi	a5, a15, -4
    4bff:	6f92d2        	l16si	a13, a2, 222
    4c02:	0248           	l32i.n	a4, a2, 0
    4c04:	6e92c2        	l16si	a12, a2, 220
    4c07:	402232        	l32i	a3, a2, 0x100
    4c0a:	44cb           	addi.n	a4, a4, 12
    4c0c:	111cd7        	beq	a12, a13, 4c21 <NR_NoiseEstimate+0x9d>
    4c0f:	04ad           	mov.n	a10, a4
    4c11:	edbc81        	l32r	a8, 304 (4734 <equal_power>)
    4c14:	03bd           	mov.n	a11, a3
    4c16:	06ed           	mov.n	a14, a6
    4c18:	0008e0        	callx8	a8
    4c1b:	6f52a2        	s16i	a10, a2, 222
    4c1e:	6e52a2        	s16i	a10, a2, 220

00004c21 <NR_NoiseEstimate+0x9d>:
    4c21:	fcc332        	addi	a3, a3, -4
    4c24:	fc24bd440f 	{ ae_lq32f.i	aeq2, a4, 0; ae_zeroq56	aeq0 }
    4c29:	1e9676        	loopnez	a6, 4c4b <NR_NoiseEstimate+0xc7>
    4c2c:	448514        	ae_lp16x2f.iu	aep0, a5, 4
    4c2f:	259d           	ae_l16si.n	a9, a5, 0
    4c31:	605314        	ae_lq32f.iu	aeq1, a3, 4
    4c34:	c18864        	ae_mulzaafq32sp16s.lh	aeq3, aeq1, aep0, aeq2, aep0
    4c37:	69cc           	bnez.n	a9, 4c41 <NR_NoiseEstimate+0xbd>
    4c39:	f86b3d541f 	{ ae_lq32f.iu	aeq2, a4, 4; ae_maxq56s	aeq0, aeq0, aeq1 }
    4c3e:	0001c6        	j	4c49 <NR_NoiseEstimate+0xc5>

00004c41 <NR_NoiseEstimate+0xbd>:
    4c41:	609414        	ae_lq32f.iu	aeq2, a4, 4
    4c44:	f87b39b30f 	{ ae_sq32f.i	aeq3, a3, 0; ae_maxq56s	aeq0, aeq0, aeq3 }

00004c49 <NR_NoiseEstimate+0xc5>:
    4c49:	f03d           	nop.n

00004c4b <NR_NoiseEstimate+0xc7>:
    4c4b:	364a04        	ae_nsaq56s	a10, aeq0
    4c4e:	8b0c           	movi.n	a11, 8
    4c50:	1daba7        	bge	a11, a10, 4c71 <NR_NoiseEstimate+0xed>
    4c53:	402232        	l32i	a3, a2, 0x100
    4c56:	6f9282        	l16si	a8, a2, 222
    4c59:	f8ca92        	addi	a9, a10, -8
    4c5c:	fcc332        	addi	a3, a3, -4
    4c5f:	c08890        	sub	a8, a8, a9
    4c62:	6f5282        	s16i	a8, a2, 222
    4c65:	089676        	loopnez	a6, 4c71 <NR_NoiseEstimate+0xed>
    4c68:	601314        	ae_lq32f.iu	aeq0, a3, 4
    4c6b:	340994        	ae_sllaq56	aeq0, aeq0, a9
    4c6e:	620304        	ae_sq32f.i	aeq0, a3, 0
    4c71:	f01d           	retw.n

00004c73 <NR_NoiseEstimate+0xef>:
    4c73:	02bab6        	bltui	a10, 16, 4c79 <NR_NoiseEstimate+0xf5>
    4c76:	ffd206        	j	4bc2 <NR_NoiseEstimate+0x3e>
    4c79:	3422a2        	l32i	a10, a2, 208
    4c7c:	ffd146        	j	4bc5 <NR_NoiseEstimate+0x41>

00004c7f <NR_NoiseEstimate+0xfb>:
	...

00004c80 <airoha_divide>:
    4c80:	006136        	entry	a1, 48
    4c83:	028d           	mov.n	a8, a2
    4c85:	82cc           	bnez.n	a2, 4c91 <airoha_divide+0x11>
    4c87:	0f0356        	bnez	a3, 4d7b <airoha_divide+0xfb>
    4c8a:	ecee31        	l32r	a3, 44 (7fffffff <_end+0x7fff92e3>)
    4c8d:	020c           	movi.n	a2, 0
    4c8f:	f01d           	retw.n

00004c91 <airoha_divide+0x11>:
    4c91:	eda6e1        	l32r	a14, 32c (80000001 <_end+0x7fff92e5>)
    4c94:	ececd1        	l32r	a13, 44 (7fffffff <_end+0x7fff92e3>)
    4c97:	c3cc           	bnez.n	a3, 4ca7 <airoha_divide+0x27>
    4c99:	0e3d           	mov.n	a3, a14
    4c9b:	020c           	movi.n	a2, 0
    4c9d:	722280        	salt	a2, a2, a8

00004ca0 <airoha_divide+0x20>:
    4ca0:	933d20        	movnez	a3, a13, a2
    4ca3:	f23c           	movi.n	a2, 63

00004ca5 <airoha_divide+0x25>:
    4ca5:	f01d           	retw.n

00004ca7 <airoha_divide+0x27>:
    4ca7:	605130        	abs	a5, a3
    4caa:	602180        	abs	a2, a8
    4cad:	02ad           	mov.n	a10, a2
    4caf:	057d           	mov.n	a7, a5
    4cb1:	0072d6        	bgez	a2, 4cbc <airoha_divide+0x3c>
    4cb4:	41a120        	srli	a10, a2, 1
    4cb7:	120c           	movi.n	a2, 1
    4cb9:	000046        	j	4cbe <airoha_divide+0x3e>

00004cbc <airoha_divide+0x3c>:
    4cbc:	020c           	movi.n	a2, 0

00004cbe <airoha_divide+0x3e>:
    4cbe:	40ea40        	nsa	a4, a10
    4cc1:	401400        	ssl	a4
    4cc4:	a1aa00        	sll	a10, a10
    4cc7:	0045d6        	bgez	a5, 4ccf <airoha_divide+0x4f>
    4cca:	417150        	srli	a7, a5, 1
    4ccd:	220b           	addi.n	a2, a2, -1

00004ccf <airoha_divide+0x4f>:
    4ccf:	40e7f0        	nsa	a15, a7
    4cd2:	ff1b           	addi.n	a15, a15, 1
    4cd4:	401f00        	ssl	a15
    4cd7:	c04f40        	sub	a4, a15, a4
    4cda:	224a           	add.n	a2, a2, a4
    4cdc:	a17700        	sll	a7, a7
    4cdf:	97cd           	ae_zext16	a12, a7
    4ce1:	f5f070        	extui	a15, a7, 16, 16
    4ce4:	c2baf0        	quou	a11, a10, a15
    4ce7:	0b9d           	mov.n	a9, a11
    4ce9:	824fb0        	mull	a4, a15, a11
    4cec:	82cbc0        	mull	a12, a11, a12
    4cef:	c04a40        	sub	a4, a10, a4
    4cf2:	114400        	slli	a4, a4, 16
    4cf5:	046d           	mov.n	a6, a4
    4cf7:	10b4c7        	bgeu	a4, a12, 4d0b <airoha_divide+0x8b>
    4cfa:	647a           	add.n	a6, a4, a7

00004cfc <airoha_divide+0x7c>:
    4cfc:	9b0b           	addi.n	a9, a11, -1
    4cfe:	093677        	bltu	a6, a7, 4d0b <airoha_divide+0x8b>
    4d01:	06b6c7        	bgeu	a6, a12, 4d0b <airoha_divide+0x8b>
    4d04:	09bd           	mov.n	a11, a9
    4d06:	667a           	add.n	a6, a6, a7
    4d08:	fffc06        	j	4cfc <airoha_divide+0x7c>

00004d0b <airoha_divide+0x8b>:
    4d0b:	f5b090        	extui	a11, a9, 16, 16
    4d0e:	c066c0        	sub	a6, a6, a12
    4d11:	c266f0        	quou	a6, a6, a15
    4d14:	119900        	slli	a9, a9, 16
    4d17:	696a           	add.n	a6, a9, a6
    4d19:	629690        	saltu	a9, a6, a9
    4d1c:	99ba           	add.n	a9, a9, a11
    4d1e:	0b0c           	movi.n	a11, 0
    4d20:	143a77        	bltu	a10, a7, 4d38 <airoha_divide+0xb8>
    4d23:	221b           	addi.n	a2, a2, 1
    4d25:	a62b           	addi.n	a10, a6, 2
    4d27:	404200        	ssai	2
    4d2a:	627a60        	saltu	a7, a10, a6
    4d2d:	779a           	add.n	a7, a7, a9
    4d2f:	8167a0        	src	a6, a7, a10
    4d32:	417270        	srli	a7, a7, 2
    4d35:	0003c6        	j	4d48 <airoha_divide+0xc8>

00004d38 <airoha_divide+0xb8>:
    4d38:	404100        	ssai	1
    4d3b:	a61b           	addi.n	a10, a6, 1
    4d3d:	627a60        	saltu	a7, a10, a6
    4d40:	779a           	add.n	a7, a7, a9
    4d42:	8167a0        	src	a6, a7, a10
    4d45:	417170        	srli	a7, a7, 1

00004d48 <airoha_divide+0xc8>:
    4d48:	ecbfa1        	l32r	a10, 44 (7fffffff <_end+0x7fff92e3>)
    4d4b:	090c           	movi.n	a9, 0
    4d4d:	0637b7        	bltu	a7, a11, 4d57 <airoha_divide+0xd7>
    4d50:	17cc           	bnez.n	a7, 4d55 <airoha_divide+0xd5>
    4d52:	01ba67        	bgeu	a10, a6, 4d57 <airoha_divide+0xd7>

00004d55 <airoha_divide+0xd5>:
    4d55:	190c           	movi.n	a9, 1

00004d57 <airoha_divide+0xd7>:
    4d57:	305830        	xor	a5, a8, a3
    4d5a:	315f50        	srai	a5, a5, 31
    4d5d:	398c           	beqz.n	a9, 4d64 <airoha_divide+0xe4>
    4d5f:	221b           	addi.n	a2, a2, 1
    4d61:	ed3261        	l32r	a6, 22c (40000000 <_end+0x3fff92e4>)

00004d64 <airoha_divide+0xe4>:
    4d64:	c0c2c2        	addi	a12, a2, -64
    4d67:	60f060        	neg	a15, a6
    4d6a:	063d           	mov.n	a3, a6
    4d6c:	933f50        	movnez	a3, a15, a5
    4d6f:	f32c56        	bnez	a12, 4ca5 <airoha_divide+0x25>
    4d72:	0d3d           	mov.n	a3, a13
    4d74:	052d           	mov.n	a2, a5
    4d76:	0edd           	mov.n	a13, a14
    4d78:	ffc906        	j	4ca0 <airoha_divide+0x20>

00004d7b <airoha_divide+0xfb>:
    4d7b:	f12256        	bnez	a2, 4c91 <airoha_divide+0x11>
    4d7e:	030c           	movi.n	a3, 0
    4d80:	c1af22        	movi	a2, -63
    4d83:	f01d           	retw.n

00004d85 <airoha_divide+0x105>:
    4d85:	000000                                        ...

00004d88 <airoha_divide_limit_1>:
    4d88:	006136        	entry	a1, 48
    4d8b:	ecdb81        	l32r	a8, f8 (4c80 <airoha_divide>)
    4d8e:	03bd           	mov.n	a11, a3
    4d90:	02ad           	mov.n	a10, a2
    4d92:	0008e0        	callx8	a8
    4d95:	368b34        	ae_cvtq48a32s	aeq2, a11
    4d98:	fc24feaa4f 	{ ae_slaasq56s	aeq1, aeq2, a10; nop }
    4d9d:	061934        	ae_roundsq32asym	aeq0, aeq1
    4da0:	620104        	ae_sq32f.i	aeq0, a1, 0
    4da3:	0128           	l32i.n	a2, a1, 0
    4da5:	f01d           	retw.n

00004da7 <airoha_divide_limit_1+0x1f>:
	...

00004da8 <xtensa_fft4_32b>:
    4da8:	006136        	entry	a1, 48
    4dab:	60c274        	ae_lq32f.i	aeq3, a2, 28
    4dae:	608234        	ae_lq32f.i	aeq2, a2, 12
    4db1:	600214        	ae_lq32f.i	aeq0, a2, 4
    4db4:	f8e23d225f 	{ ae_lq32f.i	aeq1, a2, 20; ae_addq56	aeq0, aeq0, aeq2 }
    4db9:	f9ee3d621f 	{ ae_lq32f.i	aeq3, a2, 4; ae_addq56	aeq1, aeq1, aeq3 }
    4dbe:	fa663a310f 	{ ae_sq56s.i	aeq3, a1, 0; ae_addq56	aeq2, aeq0, aeq1 }
    4dc3:	f826fd226f 	{ ae_lq32f.i	aeq1, a2, 24; ae_subq56	aeq0, aeq0, aeq1 }
    4dc8:	622214        	ae_sq32f.i	aeq2, a2, 4
    4dcb:	60c254        	ae_lq32f.i	aeq3, a2, 20
    4dce:	620254        	ae_sq32f.i	aeq0, a2, 20
    4dd1:	608244        	ae_lq32f.i	aeq2, a2, 16
    4dd4:	600224        	ae_lq32f.i	aeq0, a2, 8
    4dd7:	f9ea3d420f 	{ ae_lq32f.i	aeq2, a2, 0; ae_addq56	aeq1, aeq1, aeq2 }
    4ddc:	01a8c4        	ae_addq56	aeq2, aeq2, aeq0
    4ddf:	0129c4        	ae_addq56	aeq0, aeq2, aeq1
    4de2:	fa36fd220f 	{ ae_lq32f.i	aeq1, a2, 0; ae_subq56	aeq2, aeq2, aeq1 }
    4de7:	620204        	ae_sq32f.i	aeq0, a2, 0
    4dea:	600274        	ae_lq32f.i	aeq0, a2, 28
    4ded:	fb3afd022f 	{ ae_lq32f.i	aeq0, a2, 8; ae_subq56	aeq3, aeq3, aeq0 }
    4df2:	f92afd024f 	{ ae_lq32f.i	aeq0, a2, 16; ae_subq56	aeq1, aeq1, aeq0 }
    4df7:	faaef9a24f 	{ ae_sq32f.i	aeq2, a2, 16; ae_subq56	aeq2, aeq1, aeq3 }
    4dfc:	f9ee3d626f 	{ ae_lq32f.i	aeq3, a2, 24; ae_addq56	aeq1, aeq1, aeq3 }
    4e01:	fb3afd023f 	{ ae_lq32f.i	aeq0, a2, 12; ae_subq56	aeq3, aeq3, aeq0 }
    4e06:	621224        	ae_sq32f.i	aeq1, a2, 8
    4e09:	614104        	ae_lq56.i	aeq1, a1, 0
    4e0c:	015cc4        	ae_subq56	aeq1, aeq1, aeq0
    4e0f:	f8ee39a26f 	{ ae_sq32f.i	aeq2, a2, 24; ae_addq56	aeq0, aeq1, aeq3 }
    4e14:	f9aef9823f 	{ ae_sq32f.i	aeq0, a2, 12; ae_subq56	aeq1, aeq1, aeq3 }
    4e19:	621274        	ae_sq32f.i	aeq1, a2, 28
    4e1c:	f01d           	retw.n

00004e1e <xtensa_fft4_32b+0x76>:
	...

00004e20 <xtensa_fft8_32b>:
    4e20:	010136        	entry	a1, 128
    4e23:	604244        	ae_lq32f.i	aeq1, a2, 16
    4e26:	600264        	ae_lq32f.i	aeq0, a2, 24
    4e29:	ed41f1        	l32r	a15, 330 (5a82 <_DYNAMIC+0x406>)
    4e2c:	fa26c781ce 	{ addi	a12, a1, 120; ae_subq56	aeq2, aeq0, aeq1 }
    4e31:	632154        	ae_sq56s.i	aeq2, a1, 40
    4e34:	f8663d220f 	{ ae_lq32f.i	aeq1, a2, 0; ae_addq56	aeq0, aeq0, aeq1 }
    4e39:	608224        	ae_lq32f.i	aeq2, a2, 8
    4e3c:	fbaafa0ccf 	{ ae_sq56s.i	aeq0, a12, -32; ae_subq56	aeq3, aeq1, aeq2 }
    4e41:	f9ea3d425f 	{ ae_lq32f.i	aeq2, a2, 20; ae_addq56	aeq1, aeq1, aeq2 }
    4e46:	633164        	ae_sq56s.i	aeq3, a1, 48
    4e49:	631c94        	ae_sq56s.i	aeq1, a12, -56
    4e4c:	60c274        	ae_lq32f.i	aeq3, a2, 28
    4e4f:	f8b6fd221f 	{ ae_lq32f.i	aeq1, a2, 4; ae_subq56	aeq0, aeq2, aeq3 }
    4e54:	faf63d623f 	{ ae_lq32f.i	aeq3, a2, 12; ae_addq56	aeq2, aeq2, aeq3 }
    4e59:	f8aefa017f 	{ ae_sq56s.i	aeq0, a1, 56; ae_subq56	aeq0, aeq1, aeq3 }
    4e5e:	f9ee0202de 	{ addi	a13, a2, 32; ae_addq56	aeq1, aeq1, aeq3 }
    4e63:	fbaafa0caf 	{ ae_sq56s.i	aeq0, a12, -48; ae_subq56	aeq3, aeq1, aeq2 }
    4e68:	623254        	ae_sq32f.i	aeq3, a2, 20
    4e6b:	f9ea3dcc9f 	{ ae_lq56.i	aeq2, a12, -56; ae_addq56	aeq1, aeq1, aeq2 }
    4e70:	61ccc4        	ae_lq56.i	aeq3, a12, -32
    4e73:	f8f639921f 	{ ae_sq32f.i	aeq1, a2, 4; ae_addq56	aeq0, aeq2, aeq3 }
    4e78:	620204        	ae_sq32f.i	aeq0, a2, 0
    4e7b:	fab6fde16f 	{ ae_lq56.i	aeq3, a1, 48; ae_subq56	aeq2, aeq2, aeq3 }
    4e80:	610174        	ae_lq56.i	aeq0, a1, 56
    4e83:	f93af9a24f 	{ ae_sq32f.i	aeq2, a2, 16; ae_subq56	aeq1, aeq3, aeq0 }
    4e88:	fb7a3d8caf 	{ ae_lq56.i	aeq0, a12, -48; ae_addq56	aeq3, aeq3, aeq0 }
    4e8d:	621264        	ae_sq32f.i	aeq1, a2, 24
    4e90:	614154        	ae_lq56.i	aeq1, a1, 40
    4e93:	fa6639b22f 	{ ae_sq32f.i	aeq3, a2, 8; ae_addq56	aeq2, aeq0, aeq1 }
    4e98:	f826f9a23f 	{ ae_sq32f.i	aeq2, a2, 12; ae_subq56	aeq0, aeq0, aeq1 }
    4e9d:	620274        	ae_sq32f.i	aeq0, a2, 28
    4ea0:	60cd74        	ae_lq32f.i	aeq3, a13, 28
    4ea3:	604d54        	ae_lq32f.i	aeq1, a13, 20
    4ea6:	faee3d0d3f 	{ ae_lq32f.i	aeq0, a13, 12; ae_addq56	aeq2, aeq1, aeq3 }
    4eab:	622d54        	ae_sq32f.i	aeq2, a13, 20
    4eae:	f9aefd4d1f 	{ ae_lq32f.i	aeq2, a13, 4; ae_subq56	aeq1, aeq1, aeq3 }
    4eb3:	fb72399d7f 	{ ae_sq32f.i	aeq1, a13, 28; ae_addq56	aeq3, aeq2, aeq0 }
    4eb8:	623d14        	ae_sq32f.i	aeq3, a13, 4
    4ebb:	fa32fd0d2f 	{ ae_lq32f.i	aeq0, a13, 8; ae_subq56	aeq2, aeq2, aeq0 }
    4ec0:	60cd04        	ae_lq32f.i	aeq3, a13, 0
    4ec3:	622d34        	ae_sq32f.i	aeq2, a13, 12
    4ec6:	f93afd4d4f 	{ ae_lq32f.i	aeq2, a13, 16; ae_subq56	aeq1, aeq3, aeq0 }
    4ecb:	621d24        	ae_sq32f.i	aeq1, a13, 8
    4ece:	fb7a3d0d6f 	{ ae_lq32f.i	aeq0, a13, 24; ae_addq56	aeq3, aeq3, aeq0 }
    4ed3:	f932f9bd0f 	{ ae_sq32f.i	aeq3, a13, 0; ae_subq56	aeq1, aeq2, aeq0 }
    4ed8:	621d64        	ae_sq32f.i	aeq1, a13, 24
    4edb:	fa723d2d0f 	{ ae_lq32f.i	aeq1, a13, 0; ae_addq56	aeq2, aeq2, aeq0 }
    4ee0:	622d44        	ae_sq32f.i	aeq2, a13, 16
    4ee3:	60cd44        	ae_lq32f.i	aeq3, a13, 16
    4ee6:	011bc4        	ae_addq56	aeq0, aeq1, aeq3
    4ee9:	fb3efd420f 	{ ae_lq32f.i	aeq2, a2, 0; ae_subq56	aeq3, aeq3, aeq1 }
    4eee:	633134        	ae_sq56s.i	aeq3, a1, 24
    4ef1:	f932fd621f 	{ ae_lq32f.i	aeq3, a2, 4; ae_subq56	aeq1, aeq2, aeq0 }
    4ef6:	fa72399d0f 	{ ae_sq32f.i	aeq1, a13, 0; ae_addq56	aeq2, aeq2, aeq0 }
    4efb:	632144        	ae_sq56s.i	aeq2, a1, 32
    4efe:	604d54        	ae_lq32f.i	aeq1, a13, 20
    4f01:	608d14        	ae_lq32f.i	aeq2, a13, 4
    4f04:	0129c4        	ae_addq56	aeq0, aeq2, aeq1
    4f07:	01adc4        	ae_subq56	aeq2, aeq2, aeq1
    4f0a:	017cc4        	ae_subq56	aeq1, aeq3, aeq0
    4f0d:	fb7a3d814f 	{ ae_lq56.i	aeq0, a1, 32; ae_addq56	aeq3, aeq3, aeq0 }
    4f12:	621d14        	ae_sq32f.i	aeq1, a13, 4
    4f15:	623214        	ae_sq32f.i	aeq3, a2, 4
    4f18:	604244        	ae_lq32f.i	aeq1, a2, 16
    4f1b:	f8ea39820f 	{ ae_sq32f.i	aeq0, a2, 0; ae_addq56	aeq0, aeq1, aeq2 }
    4f20:	f9aafd625f 	{ ae_lq32f.i	aeq3, a2, 20; ae_subq56	aeq1, aeq1, aeq2 }
    4f25:	621d44        	ae_sq32f.i	aeq1, a13, 16
    4f28:	614134        	ae_lq56.i	aeq1, a1, 24
    4f2b:	01bdc4        	ae_subq56	aeq2, aeq3, aeq1
    4f2e:	fb7e39ad5f 	{ ae_sq32f.i	aeq2, a13, 20; ae_addq56	aeq3, aeq3, aeq1 }
    4f33:	623254        	ae_sq32f.i	aeq3, a2, 20
    4f36:	620244        	ae_sq32f.i	aeq0, a2, 16
    4f39:	600d64        	ae_lq32f.i	aeq0, a13, 24
    4f3c:	604d74        	ae_lq32f.i	aeq1, a13, 28
    4f3f:	0198c4        	ae_addq56	aeq2, aeq1, aeq0
    4f42:	632cb4        	ae_sq56s.i	aeq2, a12, -40
    4f45:	f826fd2d2f 	{ ae_lq32f.i	aeq1, a13, 8; ae_subq56	aeq0, aeq0, aeq1 }
    4f4a:	608d34        	ae_lq32f.i	aeq2, a13, 12
    4f4d:	01edc4        	ae_subq56	aeq3, aeq2, aeq1
    4f50:	f9ea18afff 	{ ae_cvtp24a16x2.ll	aep2, a15, a15; ae_addq56	aeq1, aeq1, aeq2 }
    4f55:	c1a244        	ae_mulzaafq32sp16s.lh	aeq2, aeq1, aep2, aeq0, aep2
    4f58:	3c48ba312f 	{ ae_sq56s.i	aeq3, a1, 16; ae_mulzasfq32sp16s.lh	aeq0, aeq0, aep2, aeq1, aep2 }
    4f5d:	604224        	ae_lq32f.i	aeq1, a2, 8
    4f60:	fbea3a010f 	{ ae_sq56s.i	aeq0, a1, 0; ae_addq56	aeq3, aeq1, aeq2 }
    4f65:	f9aafd423f 	{ ae_lq32f.i	aeq2, a2, 12; ae_subq56	aeq1, aeq1, aeq2 }
    4f6a:	621d24        	ae_sq32f.i	aeq1, a13, 8
    4f6d:	610cb4        	ae_lq56.i	aeq0, a12, -40
    4f70:	614124        	ae_lq56.i	aeq1, a1, 16
    4f73:	0d497a311f 	{ ae_sq56s.i	aeq3, a1, 8; ae_mulzaafq32sp16s.lh	aeq3, aeq1, aep2, aeq0, aep2 }
    4f78:	c52a44        	ae_mulzasfq32sp16s.lh	aeq1, aeq1, aep2, aeq0, aep2
    4f7b:	012fc4        	ae_subq56	aeq0, aeq2, aeq3
    4f7e:	faf63de11f 	{ ae_lq56.i	aeq3, a1, 8; ae_addq56	aeq2, aeq2, aeq3 }
    4f83:	620d34        	ae_sq32f.i	aeq0, a13, 12
    4f86:	622234        	ae_sq32f.i	aeq2, a2, 12
    4f89:	600264        	ae_lq32f.i	aeq0, a2, 24
    4f8c:	fb6639b22f 	{ ae_sq32f.i	aeq3, a2, 8; ae_addq56	aeq3, aeq0, aeq1 }
    4f91:	f826fd427f 	{ ae_lq32f.i	aeq2, a2, 28; ae_subq56	aeq0, aeq0, aeq1 }
    4f96:	620d64        	ae_sq32f.i	aeq0, a13, 24
    4f99:	610104        	ae_lq56.i	aeq0, a1, 0
    4f9c:	016cc4        	ae_subq56	aeq1, aeq2, aeq0
    4f9f:	621d74        	ae_sq32f.i	aeq1, a13, 28
    4fa2:	fa7239b26f 	{ ae_sq32f.i	aeq3, a2, 24; ae_addq56	aeq2, aeq2, aeq0 }
    4fa7:	622274        	ae_sq32f.i	aeq2, a2, 28
    4faa:	f01d           	retw.n

00004fac <F_get_oft_32b>:
    4fac:	004136        	entry	a1, 32
    4faf:	144226        	beqi	a2, 4, 4fc7 <F_get_oft_32b+0x1b>
    4fb2:	158226        	beqi	a2, 8, 4fcb <F_get_oft_32b+0x1f>
    4fb5:	16b226        	beqi	a2, 16, 4fcf <F_get_oft_32b+0x23>
    4fb8:	17c226        	beqi	a2, 32, 4fd3 <F_get_oft_32b+0x27>
    4fbb:	c0c242        	addi	a4, a2, -64
    4fbe:	430c           	movi.n	a3, 4
    4fc0:	220c           	movi.n	a2, 2
    4fc2:	832340        	moveqz	a2, a3, a4
    4fc5:	f01d           	retw.n

00004fc7 <F_get_oft_32b+0x1b>:
    4fc7:	024c           	movi.n	a2, 64
    4fc9:	f01d           	retw.n

00004fcb <F_get_oft_32b+0x1f>:
    4fcb:	022c           	movi.n	a2, 32
    4fcd:	f01d           	retw.n

00004fcf <F_get_oft_32b+0x23>:
    4fcf:	021c           	movi.n	a2, 16
    4fd1:	f01d           	retw.n

00004fd3 <F_get_oft_32b+0x27>:
    4fd3:	820c           	movi.n	a2, 8
    4fd5:	f01d           	retw.n

00004fd7 <F_get_oft_32b+0x2b>:
	...

00004fd8 <xtensa_fft_pass_32b>:
    4fd8:	00a136        	entry	a1, 80
    4fdb:	83ad           	ae_sext16	a10, a3
    4fdd:	b09320        	addx8	a9, a3, a2
    4fe0:	600204        	ae_lq32f.i	aeq0, a2, 0
    4fe3:	904330        	addx2	a4, a3, a3
    4fe6:	1153c0        	slli	a5, a3, 4
    4fe9:	606254        	ae_lq32f.x	aeq1, a2, a5
    4fec:	b04420        	addx8	a4, a4, a2
    4fef:	608404        	ae_lq32f.i	aeq2, a4, 0
    4ff2:	f9ea10825f 	{ add	a8, a2, a5; ae_addq56	aeq1, aeq1, aeq2 }
    4ff7:	fa663d681f 	{ ae_lq32f.i	aeq3, a8, 4; ae_addq56	aeq2, aeq0, aeq1 }
    4ffc:	f826fa211f 	{ ae_sq56s.i	aeq2, a1, 8; ae_subq56	aeq0, aeq0, aeq1 }
    5001:	630144        	ae_sq56s.i	aeq0, a1, 32
    5004:	600414        	ae_lq32f.i	aeq0, a4, 4
    5007:	fb7a3d421f 	{ ae_lq32f.i	aeq2, a2, 4; ae_addq56	aeq3, aeq3, aeq0 }
    500c:	f9f60fc86e 	{ addi	a6, a8, -4; ae_addq56	aeq1, aeq2, aeq3 }
    5011:	fab6fd681f 	{ ae_lq32f.i	aeq3, a8, 4; ae_subq56	aeq2, aeq2, aeq3 }
    5016:	631124        	ae_sq56s.i	aeq1, a1, 16
    5019:	ecc681        	l32r	a8, 334 (4fac <F_get_oft_32b>)
    501c:	fb3afb925f 	{ ae_lq32f.x	aeq0, a2, a5; ae_subq56	aeq3, aeq3, aeq0 }
    5021:	614144        	ae_lq56.i	aeq1, a1, 32
    5024:	fcc252        	addi	a5, a2, -4
    5027:	633134        	ae_sq56s.i	aeq3, a1, 24
    502a:	ff4c7d240f 	{ ae_lq32f.i	aeq1, a4, 0; ae_roundsq32sym	aeq3, aeq1 }
    502f:	f92af9f61f 	{ ae_sq32f.iu	aeq3, a6, 4; ae_subq56	aeq1, aeq1, aeq0 }
    5034:	fc547de11f 	{ ae_lq56.i	aeq3, a1, 8; ae_roundsq32sym	aeq0, aeq2 }
    5039:	624614        	ae_sq32f.iu	aeq0, a6, 4
    503c:	fe5c7d812f 	{ ae_lq56.i	aeq0, a1, 16; ae_roundsq32sym	aeq2, aeq3 }
    5041:	ff4479e51f 	{ ae_sq32f.iu	aeq2, a5, 4; ae_roundsq32sym	aeq3, aeq0 }
    5046:	627514        	ae_sq32f.iu	aeq3, a5, 4
    5049:	600914        	ae_lq32f.i	aeq0, a9, 4
    504c:	0189c4        	ae_addq56	aeq2, aeq0, aeq1
    504f:	632104        	ae_sq56s.i	aeq2, a1, 0
    5052:	f826fda13f 	{ ae_lq56.i	aeq1, a1, 24; ae_subq56	aeq0, aeq0, aeq1 }
    5057:	608904        	ae_lq32f.i	aeq2, a9, 0
    505a:	01edc4        	ae_subq56	aeq3, aeq2, aeq1
    505d:	fa760fc92e 	{ addi	a2, a9, -4; ae_addq56	aeq2, aeq2, aeq1 }
    5062:	fd5c4fc44e 	{ addi	a4, a4, -4; ae_roundsq32sym	aeq1, aeq3 }
    5067:	ff447d810f 	{ ae_lq56.i	aeq0, a1, 0; ae_roundsq32sym	aeq3, aeq0 }
    506c:	625414        	ae_sq32f.iu	aeq1, a4, 4
    506f:	fd5479f41f 	{ ae_sq32f.iu	aeq3, a4, 4; ae_roundsq32sym	aeq1, aeq2 }
    5074:	ff4479d21f 	{ ae_sq32f.iu	aeq1, a2, 4; ae_roundsq32sym	aeq3, aeq0 }
    5079:	627214        	ae_sq32f.iu	aeq3, a2, 4
    507c:	0008e0        	callx8	a8
    507f:	21c130        	srai	a12, a3, 1
    5082:	ecad91        	l32r	a9, 338 (642c <win_512_v3_nc+0x21c>)
    5085:	00a082        	movi	a8, 0
    5088:	11baf0        	slli	a11, a10, 1
    508b:	809b90        	add	a9, a11, a9
    508e:	458984        	ae_lp16x2f.xu	aep0, a9, a8
    5091:	88ac76        	loopgtz	a12, 511d <xtensa_fft_pass_32b+0x145>
    5094:	60c424        	ae_lq32f.i	aeq3, a4, 8
    5097:	600414        	ae_lq32f.i	aeq0, a4, 4
    509a:	6002fd462f 	{ ae_lq32f.i	aeq2, a6, 8; ae_mulzasq32sp16u.lh	aeq1, aeq0, aep0, aeq3, aep0 }
    509f:	33017d061f 	{ ae_lq32f.i	aeq0, a6, 4; ae_mulzaaq32sp16u.lh	aeq3, aeq3, aep0, aeq0, aep0 }
    50a4:	62007a110f 	{ ae_sq56s.i	aeq1, a1, 0; ae_mulzasq32sp16u.lh	aeq1, aeq2, aep0, aeq0, aep0 }
    50a9:	c20024        	ae_mulzaaq32sp16u.lh	aeq0, aeq0, aep0, aeq2, aep0
    50ac:	019bc4        	ae_addq56	aeq2, aeq1, aeq3
    50af:	015fc4        	ae_subq56	aeq1, aeq1, aeq3
    50b2:	631134        	ae_sq56s.i	aeq1, a1, 24
    50b5:	604524        	ae_lq32f.i	aeq1, a5, 8
    50b8:	01dac4        	ae_addq56	aeq3, aeq1, aeq2
    50bb:	633124        	ae_sq56s.i	aeq3, a1, 16
    50be:	f9aafde10f 	{ ae_lq56.i	aeq3, a1, 0; ae_subq56	aeq1, aeq1, aeq2 }
    50c3:	018bc4        	ae_addq56	aeq2, aeq0, aeq3
    50c6:	fb3afd051f 	{ ae_lq32f.i	aeq0, a5, 4; ae_subq56	aeq3, aeq3, aeq0 }
    50cb:	fbe23a311f 	{ ae_sq56s.i	aeq3, a1, 8; ae_addq56	aeq3, aeq0, aeq2 }
    50d0:	010ec4        	ae_subq56	aeq0, aeq0, aeq2
    50d3:	fe445b89bf 	{ ae_lp16x2f.xu	aep0, a9, a11; ae_roundsq32sym	aeq2, aeq0 }
    50d8:	fc4c7da12f 	{ ae_lq56.i	aeq1, a1, 16; ae_roundsq32sym	aeq0, aeq1 }
    50dd:	626614        	ae_sq32f.iu	aeq2, a6, 4
    50e0:	fe5c79c61f 	{ ae_sq32f.iu	aeq0, a6, 4; ae_roundsq32sym	aeq2, aeq3 }
    50e5:	626514        	ae_sq32f.iu	aeq2, a5, 4
    50e8:	fc4c7dc11f 	{ ae_lq56.i	aeq2, a1, 8; ae_roundsq32sym	aeq0, aeq1 }
    50ed:	624514        	ae_sq32f.iu	aeq0, a5, 4
    50f0:	604224        	ae_lq32f.i	aeq1, a2, 8
    50f3:	fbaafd021f 	{ ae_lq32f.i	aeq0, a2, 4; ae_subq56	aeq3, aeq1, aeq2 }
    50f8:	f9ea3dc13f 	{ ae_lq56.i	aeq2, a1, 24; ae_addq56	aeq1, aeq1, aeq2 }
    50fd:	fba2fa314f 	{ ae_sq56s.i	aeq3, a1, 32; ae_subq56	aeq3, aeq0, aeq2 }
    5102:	010ac4        	ae_addq56	aeq0, aeq0, aeq2
    5105:	068834        	ae_roundsq32sym	aeq2, aeq0
    5108:	fc4c7da14f 	{ ae_lq56.i	aeq1, a1, 32; ae_roundsq32sym	aeq0, aeq1 }
    510d:	626214        	ae_sq32f.iu	aeq2, a2, 4
    5110:	fe5c79c21f 	{ ae_sq32f.iu	aeq0, a2, 4; ae_roundsq32sym	aeq2, aeq3 }
    5115:	fc4c79e41f 	{ ae_sq32f.iu	aeq2, a4, 4; ae_roundsq32sym	aeq0, aeq1 }
    511a:	624414        	ae_sq32f.iu	aeq0, a4, 4

0000511d <xtensa_fft_pass_32b+0x145>:
    511d:	60b0b0        	neg	a11, a11
    5120:	dc0b           	addi.n	a13, a12, -1
    5122:	11eae0        	slli	a14, a10, 2
    5125:	60e0e0        	neg	a14, a14
    5128:	4589e4        	ae_lp16x2f.xu	aep0, a9, a14
    512b:	f03d           	nop.n
    512d:	88ad76        	loopgtz	a13, 51b9 <xtensa_fft_pass_32b+0x1e1>
    5130:	608414        	ae_lq32f.i	aeq2, a4, 4
    5133:	60c424        	ae_lq32f.i	aeq3, a4, 8
    5136:	92033d261f 	{ ae_lq32f.i	aeq1, a6, 4; ae_mulzsaq32sp16u.lh	aeq0, aeq3, aep0, aeq2, aep0 }

0000513b <xtensa_fft_pass_32b+0x163>:
    513b:	3302bd662f 	{ ae_lq32f.i	aeq3, a6, 8; ae_mulzaaq32sp16u.lh	aeq2, aeq2, aep0, aeq3, aep0 }
    5140:	9003ba014f 	{ ae_sq56s.i	aeq0, a1, 32; ae_mulzsaq32sp16u.lh	aeq0, aeq1, aep0, aeq3, aep0 }
    5145:	c288d4        	ae_mulzaaq32sp16u.lh	aeq3, aeq3, aep0, aeq1, aep0
    5148:	014ac4        	ae_addq56	aeq1, aeq0, aeq2
    514b:	010ec4        	ae_subq56	aeq0, aeq0, aeq2
    514e:	630114        	ae_sq56s.i	aeq0, a1, 8
    5151:	600524        	ae_lq32f.i	aeq0, a5, 8
    5154:	0189c4        	ae_addq56	aeq2, aeq0, aeq1
    5157:	632124        	ae_sq56s.i	aeq2, a1, 16
    515a:	f826fdc14f 	{ ae_lq56.i	aeq2, a1, 32; ae_subq56	aeq0, aeq0, aeq1 }
    515f:	017ac4        	ae_addq56	aeq1, aeq3, aeq2
    5162:	fab6fd651f 	{ ae_lq32f.i	aeq3, a5, 4; ae_subq56	aeq2, aeq2, aeq3 }
    5167:	fa7e3a213f 	{ ae_sq56s.i	aeq2, a1, 24; ae_addq56	aeq2, aeq3, aeq1 }
    516c:	01fdc4        	ae_subq56	aeq3, aeq3, aeq1
    516f:	fd5c5b89bf 	{ ae_lp16x2f.xu	aep0, a9, a11; ae_roundsq32sym	aeq1, aeq3 }
    5174:	ff447d812f 	{ ae_lq56.i	aeq0, a1, 16; ae_roundsq32sym	aeq3, aeq0 }
    5179:	625614        	ae_sq32f.iu	aeq1, a6, 4
    517c:	fd5479f61f 	{ ae_sq32f.iu	aeq3, a6, 4; ae_roundsq32sym	aeq1, aeq2 }
    5181:	625514        	ae_sq32f.iu	aeq1, a5, 4
    5184:	ff447da13f 	{ ae_lq56.i	aeq1, a1, 24; ae_roundsq32sym	aeq3, aeq0 }
    5189:	627514        	ae_sq32f.iu	aeq3, a5, 4
    518c:	600224        	ae_lq32f.i	aeq0, a2, 8
    518f:	fa26fd621f 	{ ae_lq32f.i	aeq3, a2, 4; ae_subq56	aeq2, aeq0, aeq1 }
    5194:	f8663da11f 	{ ae_lq56.i	aeq1, a1, 8; ae_addq56	aeq0, aeq0, aeq1 }
    5199:	fa3efa210f 	{ ae_sq56s.i	aeq2, a1, 0; ae_subq56	aeq2, aeq3, aeq1 }
    519e:	01f9c4        	ae_addq56	aeq3, aeq3, aeq1
    51a1:	067834        	ae_roundsq32sym	aeq1, aeq3
    51a4:	ff447d810f 	{ ae_lq56.i	aeq0, a1, 0; ae_roundsq32sym	aeq3, aeq0 }
    51a9:	625214        	ae_sq32f.iu	aeq1, a2, 4
    51ac:	fd5479f21f 	{ ae_sq32f.iu	aeq3, a2, 4; ae_roundsq32sym	aeq1, aeq2 }
    51b1:	ff4479d41f 	{ ae_sq32f.iu	aeq1, a4, 4; ae_roundsq32sym	aeq3, aeq0 }
    51b6:	627414        	ae_sq32f.iu	aeq3, a4, 4

000051b9 <xtensa_fft_pass_32b+0x1e1>:
    51b9:	f01d           	retw.n

000051bb <xtensa_fft_pass_32b+0x1e3>:
	...

000051bc <xtensa_fft16_32b>:
    51bc:	004136        	entry	a1, 32
    51bf:	ec5f81        	l32r	a8, 33c (4e20 <xtensa_fft8_32b>)
    51c2:	02ad           	mov.n	a10, a2
    51c4:	0008e0        	callx8	a8
    51c7:	ec5e81        	l32r	a8, 340 (4da8 <xtensa_fft4_32b>)
    51ca:	40c2a2        	addi	a10, a2, 64
    51cd:	0008e0        	callx8	a8
    51d0:	ec5c81        	l32r	a8, 340 (4da8 <xtensa_fft4_32b>)
    51d3:	60c2a2        	addi	a10, a2, 96
    51d6:	0008e0        	callx8	a8
    51d9:	ec5a81        	l32r	a8, 344 (4fd8 <xtensa_fft_pass_32b>)
    51dc:	02ad           	mov.n	a10, a2
    51de:	4b0c           	movi.n	a11, 4
    51e0:	0008e0        	callx8	a8
    51e3:	f01d           	retw.n

000051e5 <xtensa_fft16_32b+0x29>:
    51e5:	000000                                        ...

000051e8 <xtensa_fft32_32b>:
    51e8:	004136        	entry	a1, 32
    51eb:	ec5781        	l32r	a8, 348 (51bc <xtensa_fft16_32b>)
    51ee:	02ad           	mov.n	a10, a2
    51f0:	0008e0        	callx8	a8
    51f3:	ec5281        	l32r	a8, 33c (4e20 <xtensa_fft8_32b>)
    51f6:	01d2a2        	addmi	a10, a2, 0x100
    51f9:	80caa2        	addi	a10, a10, -128
    51fc:	0008e0        	callx8	a8
    51ff:	ec4f81        	l32r	a8, 33c (4e20 <xtensa_fft8_32b>)
    5202:	01d2a2        	addmi	a10, a2, 0x100
    5205:	c0caa2        	addi	a10, a10, -64
    5208:	0008e0        	callx8	a8
    520b:	ec4e81        	l32r	a8, 344 (4fd8 <xtensa_fft_pass_32b>)
    520e:	02ad           	mov.n	a10, a2
    5210:	8b0c           	movi.n	a11, 8
    5212:	0008e0        	callx8	a8
    5215:	f01d           	retw.n

00005217 <xtensa_fft32_32b+0x2f>:
	...

00005218 <xtensa_fft64_32b>:
    5218:	004136        	entry	a1, 32
    521b:	ec4c81        	l32r	a8, 34c (51e8 <xtensa_fft32_32b>)
    521e:	02ad           	mov.n	a10, a2
    5220:	0008e0        	callx8	a8
    5223:	ec4981        	l32r	a8, 348 (51bc <xtensa_fft16_32b>)
    5226:	01d2a2        	addmi	a10, a2, 0x100
    5229:	0008e0        	callx8	a8
    522c:	ec4781        	l32r	a8, 348 (51bc <xtensa_fft16_32b>)
    522f:	02d2a2        	addmi	a10, a2, 0x200
    5232:	80caa2        	addi	a10, a10, -128
    5235:	0008e0        	callx8	a8
    5238:	ec4381        	l32r	a8, 344 (4fd8 <xtensa_fft_pass_32b>)
    523b:	02ad           	mov.n	a10, a2
    523d:	0b1c           	movi.n	a11, 16
    523f:	0008e0        	callx8	a8
    5242:	f01d           	retw.n

00005244 <xtensa_fft128_32b>:
    5244:	004136        	entry	a1, 32
    5247:	ec4281        	l32r	a8, 350 (5218 <xtensa_fft64_32b>)
    524a:	02ad           	mov.n	a10, a2
    524c:	0008e0        	callx8	a8
    524f:	ec3f81        	l32r	a8, 34c (51e8 <xtensa_fft32_32b>)
    5252:	02d2a2        	addmi	a10, a2, 0x200
    5255:	0008e0        	callx8	a8
    5258:	ec3d81        	l32r	a8, 34c (51e8 <xtensa_fft32_32b>)
    525b:	03d2a2        	addmi	a10, a2, 0x300
    525e:	0008e0        	callx8	a8
    5261:	ec3881        	l32r	a8, 344 (4fd8 <xtensa_fft_pass_32b>)
    5264:	02ad           	mov.n	a10, a2
    5266:	0b2c           	movi.n	a11, 32
    5268:	0008e0        	callx8	a8
    526b:	f01d           	retw.n

0000526d <xtensa_fft128_32b+0x29>:
    526d:	000000                                        ...

00005270 <xtensa_fft256_32b>:
    5270:	004136        	entry	a1, 32
    5273:	ec3781        	l32r	a8, 350 (5218 <xtensa_fft64_32b>)
    5276:	02ad           	mov.n	a10, a2
    5278:	0008e0        	callx8	a8
    527b:	ec3481        	l32r	a8, 34c (51e8 <xtensa_fft32_32b>)
    527e:	02d2a2        	addmi	a10, a2, 0x200
    5281:	0008e0        	callx8	a8
    5284:	ec3281        	l32r	a8, 34c (51e8 <xtensa_fft32_32b>)
    5287:	03d2a2        	addmi	a10, a2, 0x300
    528a:	0008e0        	callx8	a8
    528d:	ec2d81        	l32r	a8, 344 (4fd8 <xtensa_fft_pass_32b>)
    5290:	02ad           	mov.n	a10, a2
    5292:	0b2c           	movi.n	a11, 32
    5294:	0008e0        	callx8	a8
    5297:	ec2e81        	l32r	a8, 350 (5218 <xtensa_fft64_32b>)
    529a:	04d2a2        	addmi	a10, a2, 0x400
    529d:	0008e0        	callx8	a8
    52a0:	ec2c81        	l32r	a8, 350 (5218 <xtensa_fft64_32b>)
    52a3:	06d2a2        	addmi	a10, a2, 0x600
    52a6:	0008e0        	callx8	a8
    52a9:	ec2681        	l32r	a8, 344 (4fd8 <xtensa_fft_pass_32b>)
    52ac:	02ad           	mov.n	a10, a2
    52ae:	0b4c           	movi.n	a11, 64
    52b0:	0008e0        	callx8	a8
    52b3:	f01d           	retw.n

000052b5 <xtensa_fft256_32b+0x45>:
    52b5:	000000                                        ...

000052b8 <xtensa_fft512_32b>:
    52b8:	004136        	entry	a1, 32
    52bb:	ec2681        	l32r	a8, 354 (5270 <xtensa_fft256_32b>)
    52be:	02ad           	mov.n	a10, a2
    52c0:	0008e0        	callx8	a8
    52c3:	ec2581        	l32r	a8, 358 (5244 <xtensa_fft128_32b>)
    52c6:	08d2a2        	addmi	a10, a2, 0x800
    52c9:	0008e0        	callx8	a8
    52cc:	ec2381        	l32r	a8, 358 (5244 <xtensa_fft128_32b>)
    52cf:	0cd2a2        	addmi	a10, a2, 0xc00
    52d2:	0008e0        	callx8	a8
    52d5:	ec1b81        	l32r	a8, 344 (4fd8 <xtensa_fft_pass_32b>)
    52d8:	02ad           	mov.n	a10, a2
    52da:	80a0b2        	movi	a11, 128
    52dd:	0008e0        	callx8	a8
    52e0:	f01d           	retw.n

000052e2 <xtensa_fft512_32b+0x2a>:
	...

000052e4 <real_split>:
    52e4:	00a136        	entry	a1, 80
    52e7:	ec1381        	l32r	a8, 334 (4fac <F_get_oft_32b>)
    52ea:	03c8           	l32i.n	a12, a3, 0
    52ec:	13d8           	l32i.n	a13, a3, 4
    52ee:	0e0c           	movi.n	a14, 0
    52f0:	219120        	srai	a9, a2, 1
    52f3:	5199           	s32i.n	a9, a1, 20
    52f5:	b07940        	addx8	a7, a9, a4
    52f8:	c05290        	sub	a5, a2, a9
    52fb:	14e9           	s32i.n	a14, a4, 4
    52fd:	b06530        	addx8	a6, a5, a3
    5300:	16b8           	l32i.n	a11, a6, 4
    5302:	ccda           	add.n	a12, a12, a13
    5304:	1199d0        	slli	a9, a9, 3
    5307:	4199           	s32i.n	a9, a1, 16
    5309:	04c9           	s32i.n	a12, a4, 0
    530b:	06a8           	l32i.n	a10, a6, 0
    530d:	07a9           	s32i.n	a10, a7, 0
    530f:	bbba           	add.n	a11, a11, a11
    5311:	82ad           	ae_sext16	a10, a2
    5313:	60b0b0        	neg	a11, a11
    5316:	21b1b0        	srai	a11, a11, 1
    5319:	17b9           	s32i.n	a11, a7, 4
    531b:	0008e0        	callx8	a8
    531e:	0acd           	mov.n	a12, a10
    5320:	2179           	s32i.n	a7, a1, 8
    5322:	1169           	s32i.n	a6, a1, 4
    5324:	0159           	s32i.n	a5, a1, 0
    5326:	3139           	s32i.n	a3, a1, 12
    5328:	d48b           	addi.n	a13, a4, 8
    532a:	b38b           	addi.n	a11, a3, 8
    532c:	80a0f2        	movi	a15, 128
    532f:	b0e230        	addx8	a14, a2, a3
    5332:	fccee2        	addi	a14, a14, -4
    5335:	022f27        	blt	a15, a2, 533b <real_split+0x57>
    5338:	11caf0        	slli	a12, a10, 1

0000533b <real_split+0x57>:
    533b:	ec0861        	l32r	a6, 35c (6430 <ROOTS_512_Q16>)
    533e:	fecca2        	addi	a10, a12, -2
    5341:	2181a0        	srai	a8, a10, 1
    5344:	a08860        	addx4	a8, a8, a6
    5347:	440804        	ae_lp16x2f.i	aep0, a8, 0
    534a:	07eca6        	blti	a12, 128, 5355 <real_split+0x71>
    534d:	1f0c           	movi.n	a15, 1
    534f:	60c0c0        	neg	a12, a12
    5352:	000046        	j	5357 <real_split+0x73>

00005355 <real_split+0x71>:
    5355:	0f0c           	movi.n	a15, 0

00005357 <real_split+0x73>:
    5357:	f8cdd2        	addi	a13, a13, -8
    535a:	5b4b           	addi.n	a5, a11, 4
    535c:	604b04        	ae_lq32f.i	aeq1, a11, 0
    535f:	600b14        	ae_lq32f.i	aeq0, a11, 4
    5362:	130c           	movi.n	a3, 1
    5364:	7da072        	movi	a7, 125
    5367:	5198           	l32i.n	a9, a1, 20
    5369:	fcce42        	addi	a4, a14, -4
    536c:	eb7381        	l32r	a8, 138 (8000 <_end+0x12e4>)
    536f:	ee8b           	addi.n	a14, a14, 8
    5371:	448b           	addi.n	a4, a4, 8
    5373:	301884        	ae_cvtp24a16x2.ll	aep1, a8, a8
    5376:	990b           	addi.n	a9, a9, -1
    5378:	1fcc           	bnez.n	a15, 537d <real_split+0x99>
    537a:	1a8084        	ae_selp24.lh	aep0, aep0, aep0
    537d:	5da976        	loopgtz	a9, 53de <real_split+0xfa>
    5380:	630144        	ae_sq56s.i	aeq0, a1, 32
    5383:	91013d74ef 	{ ae_lq32f.iu	aeq3, a4, -8; ae_mulzsaq32sp16u.lh	aeq2, aeq1, aep0, aeq0, aep0 }
    5388:	182790aacf 	{ add	a10, a10, a12; ae_mulzaafq32sp16u.lh	aeq0, aeq1, aep1, aeq3, aep1 }
    538d:	2181a0        	srai	a8, a10, 1
    5390:	f8e23d5eef 	{ ae_lq32f.iu	aeq2, a14, -8; ae_addq56	aeq0, aeq0, aeq2 }
    5395:	32037a113f 	{ ae_sq56s.i	aeq1, a1, 24; ae_mulzaaq32sp16u.lh	aeq1, aeq3, aep0, aeq2, aep0 }
    539a:	a08860        	addx4	a8, a8, a6
    539d:	0109c4        	ae_addq56	aeq0, aeq0, aeq1
    53a0:	630154        	ae_sq56s.i	aeq0, a1, 40
    53a3:	9302fd814f 	{ ae_lq56.i	aeq0, a1, 32; ae_mulzsaq32sp16u.lh	aeq3, aeq2, aep0, aeq3, aep0 }
    53a8:	49263da13f 	{ ae_lq56.i	aeq1, a1, 24; ae_mulzasfq32sp16u.lh	aeq2, aeq0, aep1, aeq2, aep1 }
    53ad:	ce0014        	ae_mulzssq32sp16u.lh	aeq0, aeq0, aep0, aeq1, aep0
    53b0:	440804        	ae_lp16x2f.i	aep0, a8, 0
    53b3:	fa723d815f 	{ ae_lq56.i	aeq0, a1, 40; ae_addq56	aeq2, aeq2, aeq0 }
    53b8:	faf63b001f 	{ ae_sraiq56	aeq0, aeq0, 1; ae_addq56	aeq2, aeq2, aeq3 }
    53bd:	ff447b209f 	{ ae_sraiq56	aeq2, aeq2, 1; ae_roundsq32sym	aeq3, aeq0 }
    53c2:	fd5479fd2f 	{ ae_sq32f.iu	aeq3, a13, 8; ae_roundsq32sym	aeq1, aeq2 }
    53c7:	621d14        	ae_sq32f.i	aeq1, a13, 4
    53ca:	05a7a7        	bge	a7, a10, 53d3 <real_split+0xef>
    53cd:	60c0c0        	neg	a12, a12
    53d0:	30ff30        	xor	a15, a15, a3

000053d3 <real_split+0xef>:
    53d3:	605b24        	ae_lq32f.iu	aeq1, a11, 8
    53d6:	1fcc           	bnez.n	a15, 53db <real_split+0xf7>
    53d8:	1a8084        	ae_selp24.lh	aep0, aep0, aep0

000053db <real_split+0xf7>:
    53db:	601524        	ae_lq32f.iu	aeq0, a5, 8

000053de <real_split+0xfa>:
    53de:	31d8           	l32i.n	a13, a1, 12
    53e0:	41b8           	l32i.n	a11, a1, 16
    53e2:	11e8           	l32i.n	a14, a1, 4
    53e4:	5198           	l32i.n	a9, a1, 20
    53e6:	60c0c0        	neg	a12, a12
    53e9:	fecca2        	addi	a10, a12, -2
    53ec:	2181a0        	srai	a8, a10, 1
    53ef:	991b           	addi.n	a9, a9, 1
    53f1:	fccee2        	addi	a14, a14, -4
    53f4:	a08860        	addx4	a8, a8, a6
    53f7:	bbda           	add.n	a11, a11, a13
    53f9:	440804        	ae_lp16x2f.i	aep0, a8, 0
    53fc:	21d8           	l32i.n	a13, a1, 8
    53fe:	bb8b           	addi.n	a11, a11, 8
    5400:	604b04        	ae_lq32f.i	aeq1, a11, 0
    5403:	5b4b           	addi.n	a5, a11, 4
    5405:	600b14        	ae_lq32f.i	aeq0, a11, 4
    5408:	dd8b           	addi.n	a13, a13, 8
    540a:	bb8b           	addi.n	a11, a11, 8
    540c:	05eca6        	blti	a12, 128, 5415 <real_split+0x131>
    540f:	60c0c0        	neg	a12, a12
    5412:	30ff30        	xor	a15, a15, a3

00005415 <real_split+0x131>:
    5415:	1fcc           	bnez.n	a15, 541a <real_split+0x136>
    5417:	1a8084        	ae_selp24.lh	aep0, aep0, aep0

0000541a <real_split+0x136>:
    541a:	78a927        	bge	a9, a2, 5496 <real_split+0x1b2>
    541d:	f8cbb2        	addi	a11, a11, -8
    5420:	f8cdd2        	addi	a13, a13, -8
    5423:	0188           	l32i.n	a8, a1, 0
    5425:	fcce42        	addi	a4, a14, -4
    5428:	eb4491        	l32r	a9, 138 (8000 <_end+0x12e4>)
    542b:	ee8b           	addi.n	a14, a14, 8
    542d:	448b           	addi.n	a4, a4, 8
    542f:	301994        	ae_cvtp24a16x2.ll	aep1, a9, a9
    5432:	ffc882        	addi	a8, a8, -1
    5435:	5da876        	loopgtz	a8, 5496 <real_split+0x1b2>
    5438:	630144        	ae_sq56s.i	aeq0, a1, 32
    543b:	c1013d74ef 	{ ae_lq32f.iu	aeq3, a4, -8; ae_mulzssq32sp16u.lh	aeq2, aeq1, aep0, aeq0, aep0 }
    5440:	182790aacf 	{ add	a10, a10, a12; ae_mulzaafq32sp16u.lh	aeq0, aeq1, aep1, aeq3, aep1 }
    5445:	2181a0        	srai	a8, a10, 1
    5448:	f8e23d5eef 	{ ae_lq32f.iu	aeq2, a14, -8; ae_addq56	aeq0, aeq0, aeq2 }
    544d:	62037a113f 	{ ae_sq56s.i	aeq1, a1, 24; ae_mulzasq32sp16u.lh	aeq1, aeq3, aep0, aeq2, aep0 }
    5452:	a08860        	addx4	a8, a8, a6
    5455:	0109c4        	ae_addq56	aeq0, aeq0, aeq1
    5458:	630154        	ae_sq56s.i	aeq0, a1, 40
    545b:	c302fd814f 	{ ae_lq56.i	aeq0, a1, 32; ae_mulzssq32sp16u.lh	aeq3, aeq2, aep0, aeq3, aep0 }
    5460:	49263da13f 	{ ae_lq56.i	aeq1, a1, 24; ae_mulzasfq32sp16u.lh	aeq2, aeq0, aep1, aeq2, aep1 }
    5465:	ca0014        	ae_mulzsaq32sp16u.lh	aeq0, aeq0, aep0, aeq1, aep0
    5468:	440804        	ae_lp16x2f.i	aep0, a8, 0
    546b:	fa723d815f 	{ ae_lq56.i	aeq0, a1, 40; ae_addq56	aeq2, aeq2, aeq0 }
    5470:	faf63b001f 	{ ae_sraiq56	aeq0, aeq0, 1; ae_addq56	aeq2, aeq2, aeq3 }
    5475:	ff447b209f 	{ ae_sraiq56	aeq2, aeq2, 1; ae_roundsq32sym	aeq3, aeq0 }
    547a:	fd5479fd2f 	{ ae_sq32f.iu	aeq3, a13, 8; ae_roundsq32sym	aeq1, aeq2 }
    547f:	621d14        	ae_sq32f.i	aeq1, a13, 4
    5482:	05a7a7        	bge	a7, a10, 548b <real_split+0x1a7>
    5485:	60c0c0        	neg	a12, a12
    5488:	30ff30        	xor	a15, a15, a3

0000548b <real_split+0x1a7>:
    548b:	605b24        	ae_lq32f.iu	aeq1, a11, 8
    548e:	1fcc           	bnez.n	a15, 5493 <real_split+0x1af>
    5490:	1a8084        	ae_selp24.lh	aep0, aep0, aep0

00005493 <real_split+0x1af>:
    5493:	601524        	ae_lq32f.iu	aeq0, a5, 8

00005496 <real_split+0x1b2>:
    5496:	f01d           	retw.n

00005498 <xtensa_real_fft_32b>:
    5498:	004136        	entry	a1, 32
    549b:	078266        	bnei	a2, 8, 54a6 <xtensa_real_fft_32b+0xe>
    549e:	eba781        	l32r	a8, 33c (4e20 <xtensa_fft8_32b>)
    54a1:	03ad           	mov.n	a10, a3
    54a3:	0008e0        	callx8	a8

000054a6 <xtensa_real_fft_32b+0xe>:
    54a6:	07d266        	bnei	a2, 64, 54b1 <xtensa_real_fft_32b+0x19>
    54a9:	eba981        	l32r	a8, 350 (5218 <xtensa_fft64_32b>)
    54ac:	03ad           	mov.n	a10, a3
    54ae:	0008e0        	callx8	a8

000054b1 <xtensa_real_fft_32b+0x19>:
    54b1:	07e266        	bnei	a2, 128, 54bc <xtensa_real_fft_32b+0x24>
    54b4:	eba981        	l32r	a8, 358 (5244 <xtensa_fft128_32b>)
    54b7:	03ad           	mov.n	a10, a3
    54b9:	0008e0        	callx8	a8

000054bc <xtensa_real_fft_32b+0x24>:
    54bc:	07f266        	bnei	a2, 0x100, 54c7 <xtensa_real_fft_32b+0x2f>
    54bf:	eba581        	l32r	a8, 354 (5270 <xtensa_fft256_32b>)
    54c2:	03ad           	mov.n	a10, a3
    54c4:	0008e0        	callx8	a8

000054c7 <xtensa_real_fft_32b+0x2f>:
    54c7:	02ad           	mov.n	a10, a2
    54c9:	03bd           	mov.n	a11, a3
    54cb:	1142d0        	slli	a4, a2, 3
    54ce:	eba481        	l32r	a8, 360 (52e4 <real_split>)
    54d1:	c04340        	sub	a4, a3, a4
    54d4:	04cd           	mov.n	a12, a4
    54d6:	0008e0        	callx8	a8
    54d9:	a48b           	addi.n	a10, a4, 8
    54db:	820b           	addi.n	a8, a2, -1
    54dd:	03b8           	l32i.n	a11, a3, 0
    54df:	13c8           	l32i.n	a12, a3, 4
    54e1:	0d0c           	movi.n	a13, 0
    54e3:	1192c0        	slli	a9, a2, 4
    54e6:	949a           	add.n	a9, a4, a9
    54e8:	13d9           	s32i.n	a13, a3, 4
    54ea:	fcc992        	addi	a9, a9, -4
    54ed:	c0bbc0        	sub	a11, a11, a12
    54f0:	03b9           	s32i.n	a11, a3, 0
    54f2:	13a876        	loopgtz	a8, 5509 <xtensa_real_fft_32b+0x71>
    54f5:	1ad8           	l32i.n	a13, a10, 4
    54f7:	0ab8           	l32i.n	a11, a10, 0
    54f9:	ffd9c2        	addmi	a12, a9, 0xffffff00
    54fc:	aa8b           	addi.n	a10, a10, 8
    54fe:	60d0d0        	neg	a13, a13
    5501:	09d9           	s32i.n	a13, a9, 0
    5503:	3f6cb2        	s32i	a11, a12, 252
    5506:	f8c992        	addi	a9, a9, -8

00005509 <xtensa_real_fft_32b+0x71>:
    5509:	f01d           	retw.n

0000550b <xtensa_real_fft_32b+0x73>:
	...

0000550c <xtensa_comp_fft_32b>:
    550c:	004136        	entry	a1, 32
    550f:	07b266        	bnei	a2, 16, 551a <xtensa_comp_fft_32b+0xe>
    5512:	eb8d81        	l32r	a8, 348 (51bc <xtensa_fft16_32b>)
    5515:	03ad           	mov.n	a10, a3
    5517:	0008e0        	callx8	a8

0000551a <xtensa_comp_fft_32b+0xe>:
    551a:	07e266        	bnei	a2, 128, 5525 <xtensa_comp_fft_32b+0x19>
    551d:	eb8e81        	l32r	a8, 358 (5244 <xtensa_fft128_32b>)
    5520:	03ad           	mov.n	a10, a3
    5522:	0008e0        	callx8	a8

00005525 <xtensa_comp_fft_32b+0x19>:
    5525:	07f266        	bnei	a2, 0x100, 5530 <xtensa_comp_fft_32b+0x24>
    5528:	eb8b81        	l32r	a8, 354 (5270 <xtensa_fft256_32b>)
    552b:	03ad           	mov.n	a10, a3
    552d:	0008e0        	callx8	a8

00005530 <xtensa_comp_fft_32b+0x24>:
    5530:	00a292        	movi	a9, 0x200
    5533:	079297        	bne	a2, a9, 553e <xtensa_comp_fft_32b+0x32>
    5536:	eb8b81        	l32r	a8, 364 (52b8 <xtensa_fft512_32b>)
    5539:	03ad           	mov.n	a10, a3
    553b:	0008e0        	callx8	a8

0000553e <xtensa_comp_fft_32b+0x32>:
    553e:	f01d           	retw.n

00005540 <sw_fft_32b_2811>:
    5540:	006136        	entry	a1, 48
    5543:	121666        	bnei	a6, 1, 5559 <sw_fft_32b_2811+0x19>
    5546:	0fb326        	beqi	a3, 16, 5559 <sw_fft_32b_2811+0x19>
    5549:	0ce326        	beqi	a3, 128, 5559 <sw_fft_32b_2811+0x19>
    554c:	09f326        	beqi	a3, 0x100, 5559 <sw_fft_32b_2811+0x19>
    554f:	00a282        	movi	a8, 0x200
    5552:	031387        	beq	a3, a8, 5559 <sw_fft_32b_2811+0x19>

00005555 <sw_fft_32b_2811+0x15>:
    5555:	126c           	movi.n	a2, -31
    5557:	f01d           	retw.n

00005559 <sw_fft_32b_2811+0x19>:
    5559:	a6cc           	bnez.n	a6, 5567 <sw_fft_32b_2811+0x27>
    555b:	088326        	beqi	a3, 8, 5567 <sw_fft_32b_2811+0x27>
    555e:	05d326        	beqi	a3, 64, 5567 <sw_fft_32b_2811+0x27>
    5561:	02e326        	beqi	a3, 128, 5567 <sw_fft_32b_2811+0x27>
    5564:	edf366        	bnei	a3, 0x100, 5555 <sw_fft_32b_2811+0x15>
    5567:	1173f0        	slli	a7, a3, 1
    556a:	029d           	mov.n	a9, a2
    556c:	600204        	ae_lq32f.i	aeq0, a2, 0
    556f:	0b13a6        	blti	a3, 1, 557e <sw_fft_32b_2811+0x3e>
    5572:	08a776        	loopgtz	a7, 557e <sw_fft_32b_2811+0x3e>
    5575:	3404b4        	ae_sraaq56	aeq0, aeq0, a4
    5578:	620904        	ae_sq32f.i	aeq0, a9, 0
    557b:	601914        	ae_lq32f.iu	aeq0, a9, 4

0000557e <sw_fft_32b_2811+0x3e>:
    557e:	101666        	bnei	a6, 1, 5592 <sw_fft_32b_2811+0x52>
    5581:	eb7981        	l32r	a8, 368 (550c <xtensa_comp_fft_32b>)
    5584:	03ad           	mov.n	a10, a3
    5586:	02bd           	mov.n	a11, a2
    5588:	0008e0        	callx8	a8
    558b:	07bd           	mov.n	a11, a7
    558d:	090c           	movi.n	a9, 0
    558f:	000386        	j	55a1 <sw_fft_32b_2811+0x61>

00005592 <sw_fft_32b_2811+0x52>:
    5592:	eb7681        	l32r	a8, 36c (5498 <xtensa_real_fft_32b>)
    5595:	03ad           	mov.n	a10, a3
    5597:	02bd           	mov.n	a11, a2
    5599:	0008e0        	callx8	a8
    559c:	079d           	mov.n	a9, a7
    559e:	11b3e0        	slli	a11, a3, 2

000055a1 <sw_fft_32b_2811+0x61>:
    55a1:	3a1566        	bnei	a5, 1, 55df <sw_fft_32b_2811+0x9f>
    55a4:	11a9e0        	slli	a10, a9, 2
    55a7:	c0a2a0        	sub	a10, a2, a10
    55aa:	0a9d           	mov.n	a9, a10
    55ac:	fd24bd0a0f 	{ ae_lq32f.i	aeq0, a10, 0; ae_zeroq56	aeq1 }
    55b1:	07ab76        	loopgtz	a11, 55bc <sw_fft_32b_2811+0x7c>
    55b4:	069864        	ae_absq56	aeq2, aeq0
    55b7:	f936bd191f 	{ ae_lq32f.iu	aeq0, a9, 4; ae_orq56	aeq1, aeq1, aeq2 }

000055bc <sw_fft_32b_2811+0x7c>:
    55bc:	0a9d           	mov.n	a9, a10
    55be:	621104        	ae_sq32f.i	aeq1, a1, 0
    55c1:	01d8           	l32i.n	a13, a1, 0
    55c3:	600a04        	ae_lq32f.i	aeq0, a10, 0
    55c6:	0a0c           	movi.n	a10, 0
    55c8:	40edc0        	nsa	a12, a13
    55cb:	b3acd0        	movgez	a10, a12, a13
    55ce:	0aab76        	loopgtz	a11, 55dc <sw_fft_32b_2811+0x9c>
    55d1:	fc24fe8acf 	{ ae_slaasq56s	aeq3, aeq0, a10; nop }
    55d6:	623904        	ae_sq32f.i	aeq3, a9, 0
    55d9:	601914        	ae_lq32f.iu	aeq0, a9, 4
    55dc:	c044a0        	sub	a4, a4, a10

000055df <sw_fft_32b_2811+0x9f>:
    55df:	842d           	ae_sext16	a2, a4
    55e1:	f01d           	retw.n

000055e3 <sw_fft_32b_2811+0xa3>:
    55e3:	00000000 00413600                                .....

000055e8 <__do_global_ctors_aux>:
    55e8:	004136        	entry	a1, 32
    55eb:	eb6181        	l32r	a8, 370 (6744 <fftorder_512_12b+0x214>)
    55ee:	7f2882        	l32i	a8, a8, 0x1fc
    55f1:	eb6021        	l32r	a2, 374 (6940 <__CTOR_LIST___47>)
    55f4:	0c0826        	beqi	a8, -1, 5604 <__do_global_ctors_aux+0x1c>
    55f7:	02a8           	l32i.n	a10, a2, 0

000055f9 <__do_global_ctors_aux+0x11>:
    55f9:	000ae0        	callx8	a10
    55fc:	fcc222        	addi	a2, a2, -4
    55ff:	02a8           	l32i.n	a10, a2, 0
    5601:	f40a66        	bnei	a10, -1, 55f9 <__do_global_ctors_aux+0x11>

00005604 <__do_global_ctors_aux+0x1c>:
    5604:	f01d           	retw.n

00005606 <__do_global_ctors_aux+0x1e>:
	...

00005608 <xt_memset>:
    5608:	004136        	entry	a1, 32
    560b:	026d           	mov.n	a6, a2
    560d:	42cc           	bnez.n	a2, 5615 <xt_memset+0xd>
    560f:	020c           	movi.n	a2, 0
    5611:	f01d           	retw.n

00005613 <xt_memset+0xb>:
	...

00005615 <xt_memset+0xd>:
    5615:	029476        	loopnez	a4, 561b <xt_memset+0x13>
    5618:	09c634        	ae_s16ip	a3, a6, 2

0000561b <xt_memset+0x13>:
    561b:	f01d           	retw.n

0000561d <xt_memset+0x15>:
    561d:	000000                                        ...

00005620 <xt_memcpy>:
    5620:	004136        	entry	a1, 32
    5623:	22ac           	beqz.n	a2, 5649 <xt_memcpy+0x29>
    5625:	03ac           	beqz.n	a3, 5649 <xt_memcpy+0x29>
    5627:	0bb327        	bgeu	a3, a2, 5636 <xt_memcpy+0x16>
    562a:	907430        	addx2	a7, a4, a3
    562d:	05b277        	bgeu	a2, a7, 5636 <xt_memcpy+0x16>
    5630:	903420        	addx2	a3, a4, a2
    5633:	0007c6        	j	5656 <xt_memcpy+0x36>

00005636 <xt_memcpy+0x16>:
    5636:	026d           	mov.n	a6, a2
    5638:	000186        	j	5642 <xt_memcpy+0x22>

0000563b <xt_memcpy+0x1b>:
    563b:	235d           	ae_l16si.n	a5, a3, 0
    563d:	09c654        	ae_s16ip	a5, a6, 2
    5640:	332b           	addi.n	a3, a3, 2

00005642 <xt_memcpy+0x22>:
    5642:	440b           	addi.n	a4, a4, -1
    5644:	f30466        	bnei	a4, -1, 563b <xt_memcpy+0x1b>

00005647 <xt_memcpy+0x27>:
    5647:	f01d           	retw.n

00005649 <xt_memcpy+0x29>:
    5649:	020c           	movi.n	a2, 0
    564b:	f01d           	retw.n

0000564d <xt_memcpy+0x2d>:
    564d:	063d           	mov.n	a3, a6
    564f:	fec772        	addi	a7, a7, -2
    5652:	278d           	ae_l16si.n	a8, a7, 0
    5654:	668d           	ae_s16i.n	a8, a6, 0

00005656 <xt_memcpy+0x36>:
    5656:	fec362        	addi	a6, a3, -2
    5659:	440b           	addi.n	a4, a4, -1
    565b:	ee0466        	bnei	a4, -1, 564d <xt_memcpy+0x2d>
    565e:	fff946        	j	5647 <xt_memcpy+0x27>

00005661 <xt_memcpy+0x41>:
    5661:	000000                                        ...

00005664 <_fini>:
    5664:	008136        	entry	a1, 64
    5667:	eb4481        	l32r	a8, 378 (398 <__do_global_dtors_aux>)
    566a:	f03d           	nop.n
    566c:	0008e0        	callx8	a8

0000566f <_fini+0xb>:
    566f:	f01d           	retw.n
