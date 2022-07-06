
Build/lib/pisplit_post_ec_position_independent.splitload.lib:     file format elf32-xtensa-le


Disassembly of section .text:

00000000 <_init-0x15c>:
       0:	000001b4 000019e8 00002c20 00002504     ........ ,...%..
      10:	00000000 000024ec 00000000 00002c24     .....$......$,..
      20:	00000278 89442379 0000bb75 000025b0     x...y#D.u....%..
      30:	7fffffff 00002620 80000000 00002c3c     .... &......<,..
      40:	00001f70 000016d0 00001f8c 000016d8     p...............
      50:	00001fa4 000019a0 00002510 00007fff     .........%......
      60:	000020c4 000020c8 000014c0 000003e4     . ... ..........
      70:	000028b0 000028e0 000002d0 0000259c     .(...(.......%..
      80:	00002598 00000360 000e38e4 00002890     .%..`....8...(..
      90:	000028d0 00124925 000003bc 00008000     .(..%I..........
      a0:	00000998 000020c2 00002590 00005a82     ..... ...%...Z..
      b0:	00000f2c 00001fbc 00000da0 00000d28     ,...........(...
      c0:	00000f58 0000113c 00001168 00001198     X...<...h.......
      d0:	000011f0 000011c4 00001fc0 00001264     ............d...
      e0:	00001238 0000148c 00001418 00007f80     8...............
      f0:	ffff8000 00001594 00001568 000024c0     ........h....$..
     100:	ffffb800 b8000000 7fff0000 40000000     ...............@
     110:	00001bf0 00001a08 00000df8 00001a20     ............ ...
     120:	000015a4 000015f0 007fffff 0000fffc     ................
     130:	00000590 00000694 0000080c 000001d0     ................
     140:	000017c8 000018a4 00001768 00002000     ........h.... ..
     150:	000022f4 000024f0 00000178              ."...$..x...

0000015c <_init>:
     15c:	008136        	entry	a1, 64
     15f:	ffa881        	l32r	a8, 0 (1b4 <frame_dummy>)
     162:	f03d           	nop.n
     164:	0008e0        	callx8	a8
     167:	ffa781        	l32r	a8, 4 (19e8 <__do_global_ctors_aux>)
     16a:	f03d           	nop.n
     16c:	0008e0        	callx8	a8
     16f:	f01d           	retw.n

00000171 <_init+0x15>:
     171:	00000000 36000000                                .......

00000178 <__do_global_dtors_aux>:
     178:	004136        	entry	a1, 32
     17b:	ffa331        	l32r	a3, 8 (2c20 <__bss_start>)
     17e:	000382        	l8ui	a8, a3, 0
     181:	d8ec           	bnez.n	a8, 1b2 <__do_global_dtors_aux+0x3a>
     183:	ffa221        	l32r	a2, c (2504 <p$3519_6_1>)
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
     1a2:	ff9ca1        	l32r	a10, 14 (24ec <__EH_FRAME_BEGIN___44>)
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
     1bf:	ff95a1        	l32r	a10, 14 (24ec <__EH_FRAME_BEGIN___44>)
     1c2:	ff96b1        	l32r	a11, 1c (2c24 <object$3538_8_1>)
     1c5:	ff9481        	l32r	a8, 18 (0 <_text_start>)
     1c8:	0008e0        	callx8	a8

000001cb <frame_dummy+0x17>:
     1cb:	f01d           	retw.n

000001cd <frame_dummy+0x19>:
     1cd:	000000                                        ...

000001d0 <comfort_noise_gen_wb2>:
     1d0:	006136        	entry	a1, 48
     1d3:	280c           	movi.n	a8, 2
     1d5:	02ad           	mov.n	a10, a2
     1d7:	4022b2        	l32i	a11, a2, 0x100
     1da:	11b9           	s32i.n	a11, a1, 4
     1dc:	6f9a92        	l16si	a9, a10, 222
     1df:	3a2a32        	l32i	a3, a10, 232
     1e2:	139222        	l16si	a2, a2, 38
     1e5:	ffc992        	addi	a9, a9, -1
     1e8:	fc248fc33e 	{ addi	a3, a3, -4; ae_zeroq56	aeq0 }
     1ed:	05a876        	loopgtz	a8, 1f6 <comfort_noise_gen_wb2+0x26>
     1f0:	624314        	ae_sq32f.iu	aeq0, a3, 4
     1f3:	624314        	ae_sq32f.iu	aeq0, a3, 4

000001f6 <comfort_noise_gen_wb2+0x26>:
     1f6:	2f0a72        	l8ui	a7, a10, 47
     1f9:	21a9           	s32i.n	a10, a1, 8
     1fb:	8c9a62        	l16si	a6, a10, 0x118
     1fe:	fec772        	addi	a7, a7, -2
     201:	052796        	bltz	a7, 257 <comfort_noise_gen_wb2+0x87>
     204:	1158           	l32i.n	a5, a1, 4
     206:	f47c           	movi.n	a4, -1
     208:	0199           	s32i.n	a9, a1, 0
     20a:	fcc552        	addi	a5, a5, -4

0000020d <comfort_noise_gen_wb2+0x3d>:
     20d:	0064d6        	bgez	a4, 217 <comfort_noise_gen_wb2+0x47>
     210:	11b8           	l32i.n	a11, a1, 4
     212:	0bb8           	l32i.n	a11, a11, 0
     214:	000046        	j	219 <comfort_noise_gen_wb2+0x49>

00000217 <comfort_noise_gen_wb2+0x47>:
     217:	05b8           	l32i.n	a11, a5, 0

00000219 <comfort_noise_gen_wb2+0x49>:
     219:	ff8181        	l32r	a8, 20 (278 <HIFI_sqrt>)
     21c:	01a8           	l32i.n	a10, a1, 0
     21e:	0008e0        	callx8	a8
     221:	ff81d1        	l32r	a13, 28 (bb75 <_end+0x8f35>)
     224:	ff80c1        	l32r	a12, 24 (89442379 <_end+0x8943f739>)
     227:	360b34        	ae_cvtq48a32s	aeq0, a11
     22a:	82b6d0        	mull	a11, a6, a13
     22d:	d166c0        	mul16s	a6, a6, a12
     230:	300b64        	ae_cvtp24a16x2.ll	aep0, a11, a6
     233:	d4e240455e 	{ addi	a5, a5, 4; ae_mulfq32sp16s.l	aeq1, aeq0, aep0 }
     238:	d4621092af 	{ add	a9, a2, a10; ae_mulfq32sp16s.h	aeq0, aeq0, aep0 }
     23d:	fc24fe99cf 	{ ae_slaasq56s	aeq3, aeq1, a9; nop }
     242:	fc24fe898f 	{ ae_slaasq56s	aeq2, aeq0, a9; nop }
     247:	fd5c40144e 	{ addi	a4, a4, 1; ae_roundsq32sym	aeq1, aeq3 }
     24c:	fc5479d31f 	{ ae_sq32f.iu	aeq1, a3, 4; ae_roundsq32sym	aeq0, aeq2 }
     251:	624314        	ae_sq32f.iu	aeq0, a3, 4
     254:	b59747        	bne	a7, a4, 20d <comfort_noise_gen_wb2+0x3d>

00000257 <comfort_noise_gen_wb2+0x87>:
     257:	21f8           	l32i.n	a15, a1, 8
     259:	7da0a2        	movi	a10, 125
     25c:	7ca0e2        	movi	a14, 124
     25f:	8c5f62        	s16i	a6, a15, 0x118
     262:	fc2490de7e 	{ blt	a14, a7, 273 <comfort_noise_gen_wb2+0xa3>; ae_zeroq56	aeq0 }
     267:	c08a70        	sub	a8, a10, a7
     26a:	05a876        	loopgtz	a8, 273 <comfort_noise_gen_wb2+0xa3>
     26d:	624314        	ae_sq32f.iu	aeq0, a3, 4
     270:	624314        	ae_sq32f.iu	aeq0, a3, 4

00000273 <comfort_noise_gen_wb2+0xa3>:
     273:	f01d           	retw.n

00000275 <comfort_noise_gen_wb2+0xa5>:
     275:	000000                                        ...

00000278 <HIFI_sqrt>:
     278:	004136        	entry	a1, 32
     27b:	4b13a6        	blti	a3, 1, 2ca <HIFI_sqrt+0x52>
     27e:	ff6b51        	l32r	a5, 2c (25b0 <TABLE_SQRT>)
     281:	070c           	movi.n	a7, 0
     283:	40e380        	nsa	a8, a3
     286:	401800        	ssl	a8
     289:	c02280        	sub	a2, a2, a8
     28c:	a16300        	sll	a6, a3
     28f:	043020        	extui	a3, a2, 0, 1
     292:	400300        	ssr	a3
     295:	b16060        	sra	a6, a6
     298:	416960        	srli	a6, a6, 9
     29b:	314060        	srai	a4, a6, 16
     29e:	f0c442        	addi	a4, a4, -16
     2a1:	534470        	max	a4, a4, a7
     2a4:	904450        	addx2	a4, a4, a5
     2a7:	245d           	ae_l16si.n	a5, a4, 0
     2a9:	300664        	ae_cvtp24a16x2.ll	aep0, a6, a6
     2ac:	344d           	ae_l16si.n	a4, a4, 2
     2ae:	116500        	slli	a6, a5, 16
     2b1:	360634        	ae_cvtq48a32s	aeq0, a6
     2b4:	c04450        	sub	a4, a4, a5
     2b7:	114400        	slli	a4, a4, 16
     2ba:	364434        	ae_cvtq48a32s	aeq1, a4
     2bd:	d4e110223f 	{ add	a2, a2, a3; ae_mulaq32sp16u.l	aeq0, aeq1, aep0 }
     2c2:	212120        	srai	a2, a2, 1
     2c5:	368304        	ae_trunca32q48	a3, aeq0
     2c8:	f01d           	retw.n

000002ca <HIFI_sqrt+0x52>:
     2ca:	030c           	movi.n	a3, 0
     2cc:	020c           	movi.n	a2, 0
     2ce:	f01d           	retw.n

000002d0 <Log2_norm>:
     2d0:	004136        	entry	a1, 32
     2d3:	3212a6        	blti	a2, 1, 309 <Log2_norm+0x39>
     2d6:	046c           	movi.n	a4, -32
     2d8:	2da437        	bge	a4, a3, 309 <Log2_norm+0x39>
     2db:	052c           	movi.n	a5, 32
     2dd:	04a537        	bge	a5, a3, 2e5 <Log2_norm+0x15>
     2e0:	ff5421        	l32r	a2, 30 (7fffffff <_end+0x7fffd3bf>)
     2e3:	f01d           	retw.n

000002e5 <Log2_norm+0x15>:
     2e5:	ff5341        	l32r	a4, 34 (2620 <TABLE_LOG2_1530>)
     2e8:	558820        	extui	a8, a2, 24, 6
     2eb:	1188e0        	slli	a8, a8, 2
     2ee:	603484        	ae_lq32f.xu	aeq0, a4, a8
     2f1:	604414        	ae_lq32f.i	aeq1, a4, 4
     2f4:	f92ae2022f 	{ ae_movpa24x2	aep0, a2, a2; ae_subq56	aeq1, aeq1, aeq0 }
     2f9:	030174        	ae_mulaq32sp16u.l	aeq0, aeq1, aep0
     2fc:	368204        	ae_trunca32q48	a2, aeq0
     2ff:	014360        	slli	a4, a3, 26
     302:	212520        	srai	a2, a2, 5
     305:	224a           	add.n	a2, a2, a4
     307:	f01d           	retw.n

00000309 <Log2_norm+0x39>:
     309:	ff4b21        	l32r	a2, 38 (80000000 <_end+0x7fffd3c0>)
     30c:	f01d           	retw.n

0000030e <Log2_norm+0x3e>:
	...

00000310 <siir_safr>:
     310:	004136        	entry	a1, 32
     313:	368334        	ae_cvtq48a32s	aeq2, a3
     316:	364234        	ae_cvtq48a32s	aeq1, a2
     319:	05a237        	bge	a2, a3, 322 <siir_safr+0x12>
     31c:	440404        	ae_lp16x2f.i	aep0, a4, 0
     31f:	000086        	j	325 <siir_safr+0x15>

00000322 <siir_safr+0x12>:
     322:	440414        	ae_lp16x2f.i	aep0, a4, 4

00000325 <siir_safr+0x15>:
     325:	c10864        	ae_mulzaafq32sp16s.lh	aeq1, aeq1, aep0, aeq2, aep0
     328:	061934        	ae_roundsq32asym	aeq0, aeq1
     32b:	368204        	ae_trunca32q48	a2, aeq0
     32e:	f01d           	retw.n

00000330 <_start>:
     330:	004136        	entry	a1, 32
     333:	ff43a1        	l32r	a10, 40 (1f70 <_DYNAMIC+0x4f4>)
     336:	ff4131        	l32r	a3, 3c (2c3c <printf_ptr>)
     339:	0288           	l32i.n	a8, a2, 0
     33b:	ff42b1        	l32r	a11, 44 (16d0 <get_post_ec_memsize>)
     33e:	0389           	s32i.n	a8, a3, 0
     340:	0008e0        	callx8	a8
     343:	0388           	l32i.n	a8, a3, 0
     345:	ff40a1        	l32r	a10, 48 (1f8c <_DYNAMIC+0x510>)
     348:	ff41b1        	l32r	a11, 4c (16d8 <EXT_POST_EC_Init>)
     34b:	0008e0        	callx8	a8
     34e:	0388           	l32i.n	a8, a3, 0
     350:	ff40a1        	l32r	a10, 50 (1fa4 <_DYNAMIC+0x528>)
     353:	ff40b1        	l32r	a11, 54 (19a0 <EXT_POST_EC_PRCS>)
     356:	0008e0        	callx8	a8
     359:	ff3f21        	l32r	a2, 58 (2510 <export_parameter_array>)
     35c:	f01d           	retw.n

0000035e <_start+0x2e>:
	...

00000360 <take_min_four>:
     360:	004136        	entry	a1, 32
     363:	228d           	ae_l16si.n	a8, a2, 0
     365:	3f23a6        	blti	a3, 2, 3a8 <take_min_four+0x48>
     368:	ff3d91        	l32r	a9, 5c (7fff <_end+0x53bf>)
     36b:	ff3c61        	l32r	a6, 5c (7fff <_end+0x53bf>)
     36e:	ff3b71        	l32r	a7, 5c (7fff <_end+0x53bf>)
     371:	222b           	addi.n	a2, a2, 2
     373:	430b           	addi.n	a4, a3, -1
     375:	2ca476        	loopgtz	a4, 3a5 <take_min_four+0x45>
     378:	223d           	ae_l16si.n	a3, a2, 0
     37a:	222b           	addi.n	a2, a2, 2
     37c:	0aa387        	bge	a3, a8, 38a <take_min_four+0x2a>
     37f:	069d           	mov.n	a9, a6
     381:	076d           	mov.n	a6, a7
     383:	087d           	mov.n	a7, a8
     385:	038d           	mov.n	a8, a3
     387:	000606        	j	3a3 <take_min_four+0x43>

0000038a <take_min_four+0x2a>:
     38a:	08a377        	bge	a3, a7, 396 <take_min_four+0x36>
     38d:	069d           	mov.n	a9, a6
     38f:	076d           	mov.n	a6, a7
     391:	037d           	mov.n	a7, a3
     393:	000306        	j	3a3 <take_min_four+0x43>

00000396 <take_min_four+0x36>:
     396:	06a367        	bge	a3, a6, 3a0 <take_min_four+0x40>
     399:	069d           	mov.n	a9, a6
     39b:	036d           	mov.n	a6, a3
     39d:	000086        	j	3a3 <take_min_four+0x43>

000003a0 <take_min_four+0x40>:
     3a0:	439390        	min	a9, a3, a9
     3a3:	f03d           	nop.n

000003a5 <take_min_four+0x45>:
     3a5:	000206        	j	3b1 <take_min_four+0x51>

000003a8 <take_min_four+0x48>:
     3a8:	ff2d61        	l32r	a6, 5c (7fff <_end+0x53bf>)
     3ab:	ff2c91        	l32r	a9, 5c (7fff <_end+0x53bf>)
     3ae:	ff2b71        	l32r	a7, 5c (7fff <_end+0x53bf>)

000003b1 <take_min_four+0x51>:
     3b1:	469a           	add.n	a4, a6, a9
     3b3:	287a           	add.n	a2, a8, a7
     3b5:	224a           	add.n	a2, a2, a4
     3b7:	112220        	slli	a2, a2, 14
     3ba:	f01d           	retw.n

000003bc <equal_power>:
     3bc:	004136        	entry	a1, 32
     3bf:	c08450        	sub	a8, a4, a5
     3c2:	0078d6        	bgez	a8, 3cd <equal_power+0x11>
     3c5:	023d           	mov.n	a3, a2
     3c7:	c08540        	sub	a8, a5, a4
     3ca:	000046        	j	3cf <equal_power+0x13>

000003cd <equal_power+0x11>:
     3cd:	045d           	mov.n	a5, a4

000003cf <equal_power+0x13>:
     3cf:	fcc332        	addi	a3, a3, -4
     3d2:	08a676        	loopgtz	a6, 3de <equal_power+0x22>
     3d5:	601314        	ae_lq32f.iu	aeq0, a3, 4
     3d8:	3408a4        	ae_srlaq56	aeq0, aeq0, a8
     3db:	620304        	ae_sq32f.i	aeq0, a3, 0

000003de <equal_power+0x22>:
     3de:	052d           	mov.n	a2, a5
     3e0:	f01d           	retw.n

000003e2 <equal_power+0x26>:
	...

000003e4 <gen_power>:
     3e4:	004136        	entry	a1, 32
     3e7:	3a2262        	l32i	a6, a2, 232
     3ea:	0258           	l32i.n	a5, a2, 0
     3ec:	217130        	srai	a7, a3, 1
     3ef:	f8c632        	addi	a3, a6, -8
     3f2:	0d17a6        	blti	a7, 1, 403 <gen_power+0x1f>
     3f5:	05a776        	loopgtz	a7, 3fe <gen_power+0x1a>
     3f8:	5c8514        	ae_lp24x2f.iu	aep0, a5, 8
     3fb:	5e8314        	ae_sp24x2f.iu	aep0, a3, 8

000003fe <gen_power+0x1a>:
     3fe:	0258           	l32i.n	a5, a2, 0
     400:	3a2262        	l32i	a6, a2, 232

00000403 <gen_power+0x1f>:
     403:	170c           	movi.n	a7, 1
     405:	139282        	l16si	a8, a2, 38
     408:	2f0242        	l8ui	a4, a2, 47
     40b:	fcc552        	addi	a5, a5, -4
     40e:	063d           	mov.n	a3, a6
     410:	5c0604        	ae_lp24x2f.i	aep0, a6, 0
     413:	443b           	addi.n	a4, a4, 3
     415:	1188f0        	slli	a8, a8, 1
     418:	c07780        	sub	a7, a7, a8
     41b:	6e5272        	s16i	a7, a2, 220
     41e:	0aa476        	loopgtz	a4, 42c <gen_power+0x48>
     421:	040824        	ae_mulzaap24s.hh.ll	aeq1, aep0, aep0
     424:	fc4c6f831f 	{ ae_lp24x2f.iu	aep0, a3, 8; ae_roundsq32sym	aeq0, aeq1 }
     429:	624514        	ae_sq32f.iu	aeq0, a5, 4

0000042c <gen_power+0x48>:
     42c:	f01d           	retw.n

0000042e <gen_power+0x4a>:
	...

00000430 <fft_wo>:
     430:	008136        	entry	a1, 64
     433:	2e0292        	l8ui	a9, a2, 46
     436:	ff0ab1        	l32r	a11, 60 (20c4 <fftorder_512_12b+0x4>)
     439:	260c           	movi.n	a6, 2
     43b:	0289f6        	bgeui	a9, 8, 441 <fft_wo+0x11>
     43e:	084966        	bnei	a9, 4, 44a <fft_wo+0x1a>

00000441 <fft_wo+0x11>:
     441:	f0a052        	movi	a5, 240
     444:	00a242        	movi	a4, 0x200
     447:	000286        	j	455 <fft_wo+0x25>

0000044a <fft_wo+0x1a>:
     44a:	78a052        	movi	a5, 120
     44d:	00a142        	movi	a4, 0x100
     450:	ff05b1        	l32r	a11, 64 (20c8 <fftorder_512_12b+0x8>)
     453:	460c           	movi.n	a6, 4

00000455 <fft_wo+0x25>:
     455:	02f8           	l32i.n	a15, a2, 0
     457:	470c           	movi.n	a7, 4
     459:	1a15a6        	blti	a5, 1, 477 <fft_wo+0x47>
     45c:	090c           	movi.n	a9, 0
     45e:	03ad           	mov.n	a10, a3
     460:	1185f0        	slli	a8, a5, 1
     463:	5189           	s32i.n	a8, a1, 20
     465:	09a576        	loopgtz	a5, 472 <fft_wo+0x42>
     468:	2acd           	ae_l16si.n	a12, a10, 0
     46a:	aa2b           	addi.n	a10, a10, 2
     46c:	60c1c0        	abs	a12, a12
     46f:	2099c0        	or	a9, a9, a12

00000472 <fft_wo+0x42>:
     472:	000206        	j	47e <fft_wo+0x4e>

00000475 <fft_wo+0x45>:
	...

00000477 <fft_wo+0x47>:
     477:	090c           	movi.n	a9, 0
     479:	11d5f0        	slli	a13, a5, 1
     47c:	51d9           	s32i.n	a13, a1, 20

0000047e <fft_wo+0x4e>:
     47e:	11d6f0        	slli	a13, a6, 1
     481:	d27470        	quos	a7, a4, a7
     484:	3d22a2        	l32i	a10, a2, 244
     487:	a0f4f0        	addx4	a15, a4, a15
     48a:	0c0c           	movi.n	a12, 0
     48c:	40e980        	nsa	a8, a9
     48f:	11e4e0        	slli	a14, a4, 2
     492:	129292        	l16si	a9, a2, 36
     495:	41e9           	s32i.n	a14, a1, 16
     497:	f0c882        	addi	a8, a8, -16
     49a:	5388c0        	max	a8, a8, a12
     49d:	1189           	s32i.n	a8, a1, 4
     49f:	ef4b           	addi.n	a14, a15, 4
     4a1:	438890        	min	a8, a8, a9
     4a4:	3e2292        	l32i	a9, a2, 248
     4a7:	fe24ad1a0f 	{ ae_lp16x2f.i	aep1, a10, 0; ae_zeroq56	aeq2 }
     4ac:	031684        	wur.ae_sar	a8
     4af:	f9d22d090f 	{ ae_lp16x2f.i	aep0, a9, 0; ae_truncp24q48x2	aep2, aeq2, aeq2 }
     4b4:	f82394018f 	{ s32i.n	a8, a1, 0; ae_sllssp24s	aep0, aep0 }
     4b9:	17a776        	loopgtz	a7, 4d4 <fft_wo+0xa4>
     4bc:	010944        	ae_mulfp24s.hh	aeq1, aep0, aep1
     4bf:	010174        	ae_mulfp24s.ll	aeq0, aep0, aep1
     4c2:	629fc4        	ae_sq32f.x	aeq1, a15, a12
     4c5:	628ec4        	ae_sq32f.x	aeq0, a14, a12
     4c8:	2bcd           	ae_l16si.n	a12, a11, 0
     4ca:	449914        	ae_lp16x2f.iu	aep1, a9, 4
     4cd:	bbda           	add.n	a11, a11, a13
     4cf:	f827ad9a1f 	{ ae_lp16x2f.iu	aep1, a10, 4; ae_sllssp24s	aep0, aep1 }

000004d4 <fft_wo+0xa4>:
     4d4:	2d0c           	movi.n	a13, 2
     4d6:	3e2282        	l32i	a8, a2, 248
     4d9:	d2d5d0        	quos	a13, a5, a13
     4dc:	21d9           	s32i.n	a13, a1, 8
     4de:	c0d7d0        	sub	a13, a7, a13
     4e1:	5178           	l32i.n	a7, a1, 20
     4e3:	002152        	l32i	a5, a1, 0
     4e6:	0361d2        	s32i	a13, a1, 12
     4e9:	809870        	add	a9, a8, a7
     4ec:	450874        	ae_lp16x2f.x	aep0, a8, a7
     4ef:	fcc872        	addi	a7, a8, -4
     4f2:	032182        	l32i	a8, a1, 12
     4f5:	09ad76        	loopgtz	a13, 502 <fft_wo+0xd2>
     4f8:	fd0873871f 	{ ae_sp16x2f.iu	aep0, a7, 4; ae_abssp24s	aep3, aep0 }
     4fd:	dd682d891f 	{ ae_lp16x2f.iu	aep0, a9, 4; ae_orp48	aep2, aep2, aep3 }
     502:	449af4        	ae_lp16x2f.iu	aep1, a10, -4
     505:	11d6f0        	slli	a13, a6, 1
     508:	1fa876        	loopgtz	a8, 52b <fft_wo+0xfb>
     50b:	0b00d4        	ae_sllssp24s	aep5, aep0
     50e:	015154        	ae_mulfp24s.hl	aeq0, aep5, aep1
     511:	e935f3871f 	{ ae_sp16x2f.iu	aep0, a7, 4; ae_mulfp24s.lh	aeq3, aep5, aep1 }
     516:	628fc4        	ae_sq32f.x	aeq0, a15, a12
     519:	62bec4        	ae_sq32f.x	aeq3, a14, a12
     51c:	449af4        	ae_lp16x2f.iu	aep1, a10, -4

0000051f <fft_wo+0xef>:
     51f:	fe082d891f 	{ ae_lp16x2f.iu	aep0, a9, 4; ae_abssp24s	aep4, aep0 }
     524:	2bcd           	ae_l16si.n	a12, a11, 0
     526:	dd8810bbdf 	{ add	a11, a11, a13; ae_orp48	aep2, aep2, aep4 }

0000052b <fft_wo+0xfb>:
     52b:	00a8d4        	ae_cvta32p24.h	a8, aep2
     52e:	002dd4        	ae_cvta32p24.l	a13, aep2
     531:	1198           	l32i.n	a9, a1, 4
     533:	20dd80        	or	a13, a13, a8
     536:	2188           	l32i.n	a8, a1, 8
     538:	40edd0        	nsa	a13, a13
     53b:	4399d0        	min	a9, a9, a13
     53e:	125292        	s16i	a9, a2, 36
     541:	11d6f0        	slli	a13, a6, 1
     544:	039d           	mov.n	a9, a3
     546:	440304        	ae_lp16x2f.i	aep0, a3, 0
     549:	1aa876        	loopgtz	a8, 567 <fft_wo+0x137>
     54c:	0b00e4        	ae_sllssp24s	aep6, aep0
     54f:	01e154        	ae_mulfp24s.hl	aeq2, aep6, aep1
     552:	e839f3871f 	{ ae_sp16x2f.iu	aep0, a7, 4; ae_mulfp24s.lh	aeq1, aep6, aep1 }
     557:	62afc4        	ae_sq32f.x	aeq2, a15, a12
     55a:	629ec4        	ae_sq32f.x	aeq1, a14, a12
     55d:	449af4        	ae_lp16x2f.iu	aep1, a10, -4
     560:	2bcd           	ae_l16si.n	a12, a11, 0
     562:	448914        	ae_lp16x2f.iu	aep0, a9, 4
     565:	bbda           	add.n	a11, a11, a13
     567:	8c0c           	movi.n	a12, 8
     569:	1d0c           	movi.n	a13, 1
     56b:	0e0c           	movi.n	a14, 0
     56d:	41f8           	l32i.n	a15, a1, 16
     56f:	02a8           	l32i.n	a10, a2, 0
     571:	21b140        	srai	a11, a4, 1
     574:	febd81        	l32r	a8, 68 (14c0 <sw_fft_32b_2811>)
     577:	8bbd           	ae_sext16	a11, a11
     579:	aafa           	add.n	a10, a10, a15
     57b:	0008e0        	callx8	a8
     57e:	04bd           	mov.n	a11, a4
     580:	c095a0        	sub	a9, a5, a10
     583:	feba81        	l32r	a8, 6c (3e4 <gen_power>)
     586:	02ad           	mov.n	a10, a2
     588:	135292        	s16i	a9, a2, 38
     58b:	0008e0        	callx8	a8
     58e:	f01d           	retw.n

00000590 <NR_vad1>:
     590:	004136        	entry	a1, 32
     593:	0238           	l32i.n	a3, a2, 0
     595:	2e0272        	l8ui	a7, a2, 46
     598:	33cb           	addi.n	a3, a3, 12
     59a:	0787b6        	bltui	a7, 8, 5a5 <NR_vad1+0x15>
     59d:	feb441        	l32r	a4, 70 (28b0 <wb_sb_ptn>)
     5a0:	000106        	j	5a8 <NR_vad1+0x18>

000005a3 <NR_vad1+0x13>:
	...

000005a5 <NR_vad1+0x15>:
     5a5:	feb341        	l32r	a4, 74 (28e0 <nb_sb_ptn>)

000005a8 <NR_vad1+0x18>:
     5a8:	608304        	ae_lq32f.i	aeq2, a3, 0
     5ab:	047716        	beqz	a7, 5f6 <NR_vad1+0x66>
     5ae:	906740        	addx2	a6, a7, a4
     5b1:	fd2480005f 	{ movi	a5, 0; ae_zeroq56	aeq1 }

000005b6 <NR_vad1+0x26>:
     5b6:	249d           	ae_l16si.n	a9, a4, 0
     5b8:	348d           	ae_l16si.n	a8, a4, 2
     5ba:	c08890        	sub	a8, a8, a9
     5bd:	04a876        	loopgtz	a8, 5c5 <NR_vad1+0x35>
     5c0:	f9ea3d531f 	{ ae_lq32f.iu	aeq2, a3, 4; ae_addq56	aeq1, aeq1, aeq2 }

000005c5 <NR_vad1+0x35>:
     5c5:	365d04        	ae_nsaq56s	a13, aeq1
     5c8:	6e92b2        	l16si	a11, a2, 220
     5cb:	feab81        	l32r	a8, 78 (2d0 <Log2_norm>)
     5ce:	f8cdc2        	addi	a12, a13, -8
     5d1:	c0bbd0        	sub	a11, a11, a13
     5d4:	fc24fe9c4f 	{ ae_slaasq56s	aeq1, aeq1, a12; nop }
     5d9:	369a04        	ae_trunca32q48	a10, aeq1
     5dc:	bb8b           	addi.n	a11, a11, 8
     5de:	0008e0        	callx8	a8
     5e1:	3b22e2        	l32i	a14, a2, 236
     5e4:	608304        	ae_lq32f.i	aeq2, a3, 0
     5e7:	442b           	addi.n	a4, a4, 2
     5e9:	ee5a           	add.n	a14, a14, a5
     5eb:	006ea2        	s32i	a10, a14, 0
     5ee:	fd2480455e 	{ addi	a5, a5, 4; ae_zeroq56	aeq1 }
     5f3:	bf9467        	bne	a4, a6, 5b6 <NR_vad1+0x26>

000005f6 <NR_vad1+0x66>:
     5f6:	34c2c2        	addi	a12, a2, 52
     5f9:	0392f2        	l16si	a15, a2, 6
     5fc:	3b2292        	l32i	a9, a2, 236
     5ff:	54c2b2        	addi	a11, a2, 84
     602:	608b84        	ae_lq32f.i	aeq2, a11, -32
     605:	604904        	ae_lq32f.i	aeq1, a9, 0
     608:	11ff00        	slli	a15, a15, 16
     60b:	360f34        	ae_cvtq48a32s	aeq0, a15
     60e:	049716        	beqz	a7, 65b <NR_vad1+0xcb>
     611:	01d2e2        	addmi	a14, a2, 0x100
     614:	00a0a2        	movi	a10, 0
     617:	c4cef2        	addi	a15, a14, -60
     61a:	c8cee2        	addi	a14, a14, -56
     61d:	349776        	loopnez	a7, 655 <NR_vad1+0xc5>
     620:	099224        	ae_ltq56s	b2, aeq2, aeq1
     623:	0f8d           	mov.n	a8, a15
     625:	c38e20        	movf	a8, a14, b2
     628:	441804        	ae_lp16x2f.i	aep1, a8, 0
     62b:	c19164        	ae_mulzaafq32sp16s.lh	aeq2, aeq1, aep1, aeq2, aep1
     62e:	622c04        	ae_sq32f.i	aeq2, a12, 0
     631:	60db14        	ae_lq32f.iu	aeq3, a11, 4
     634:	09b114        	ae_ltq56s	b1, aeq1, aeq3
     637:	0fdd           	mov.n	a13, a15
     639:	c3de10        	movf	a13, a14, b1
     63c:	440d04        	ae_lp16x2f.i	aep0, a13, 0
     63f:	c18874        	ae_mulzaafq32sp16s.lh	aeq3, aeq1, aep0, aeq3, aep0
     642:	fab6c06a8e 	{ addi	a8, a10, 6; ae_subq56	aeq2, aeq2, aeq3 }
     647:	d65039bb0f 	{ ae_sq32f.i	aeq3, a11, 0; ae_ltq56s	b0, aeq0, aeq2 }
     64c:	605914        	ae_lq32f.iu	aeq1, a9, 4
     64f:	609c14        	ae_lq32f.iu	aeq2, a12, 4
     652:	d3a800        	movt	a10, a8, b0

00000655 <NR_vad1+0xc5>:
     655:	3b2292        	l32i	a9, a2, 236
     658:	000046        	j	65d <NR_vad1+0xcd>

0000065b <NR_vad1+0xcb>:
     65b:	0a0c           	movi.n	a10, 0

0000065d <NR_vad1+0xcd>:
     65d:	051ae6        	bgei	a10, 1, 666 <NR_vad1+0xd6>
     660:	fe87a1        	l32r	a10, 7c (259c <NR_immediate_num+0xc>)
     663:	000086        	j	669 <NR_vad1+0xd9>

00000666 <NR_vad1+0xd6>:
     666:	fe86a1        	l32r	a10, 80 (2598 <NR_immediate_num+0x8>)

00000669 <NR_vad1+0xd9>:
     669:	604904        	ae_lq32f.i	aeq1, a9, 0
     66c:	1092b2        	l16si	a11, a2, 32
     66f:	440a04        	ae_lp16x2f.i	aep0, a10, 0
     672:	7cc2a2        	addi	a10, a2, 124
     675:	11bb00        	slli	a11, a11, 16
     678:	360b34        	ae_cvtq48a32s	aeq0, a11
     67b:	608a04        	ae_lq32f.i	aeq2, a10, 0
     67e:	0d9776        	loopnez	a7, 68f <NR_vad1+0xff>
     681:	c18864        	ae_mulzaafq32sp16s.lh	aeq3, aeq1, aep0, aeq2, aep0
     684:	605914        	ae_lq32f.iu	aeq1, a9, 4
     687:	fb7b3d5a1f 	{ ae_lq32f.iu	aeq2, a10, 4; ae_maxq56s	aeq3, aeq0, aeq3 }
     68c:	623af4        	ae_sq32f.i	aeq3, a10, -4

0000068f <NR_vad1+0xff>:
     68f:	f01d           	retw.n

00000691 <NR_vad1+0x101>:
     691:	000000                                        ...

00000694 <NR_vad2>:
     694:	006136        	entry	a1, 48
     697:	851c           	movi.n	a5, 24
     699:	a0a0f2        	movi	a15, 160
     69c:	2e02a2        	l8ui	a10, a2, 46
     69f:	7cc2b2        	addi	a11, a2, 124
     6a2:	1512e2        	l16ui	a14, a2, 42
     6a5:	021282        	l16ui	a8, a2, 4
     6a8:	01d2d2        	addmi	a13, a2, 0x100
     6ab:	80cdc2        	addi	a12, a13, -128
     6ae:	0218e7        	beq	a8, a14, 6b4 <NR_vad2+0x20>
     6b1:	002186        	j	73b <NR_vad2+0xa7>
     6b4:	01a9           	s32i.n	a10, a1, 0
     6b6:	109262        	l16si	a6, a2, 32
     6b9:	a0cd82        	addi	a8, a13, -96
     6bc:	090c           	movi.n	a9, 0
     6be:	155292        	s16i	a9, a2, 42
     6c1:	1189           	s32i.n	a8, a1, 4
     6c3:	08d662        	addmi	a6, a6, 0x800
     6c6:	116600        	slli	a6, a6, 16
     6c9:	aaac           	beqz.n	a10, 6f7 <NR_vad2+0x63>
     6cb:	084d           	mov.n	a4, a8
     6cd:	093d           	mov.n	a3, a9
     6cf:	21c9           	s32i.n	a12, a1, 8
     6d1:	31b9           	s32i.n	a11, a1, 12
     6d3:	907aa0        	addx2	a7, a10, a10
     6d6:	1177d0        	slli	a7, a7, 3

000006d9 <NR_vad2+0x45>:
     6d9:	c2a8           	l32i.n	a10, a2, 48
     6db:	fe6a81        	l32r	a8, 84 (360 <take_min_four>)
     6de:	cb0c           	movi.n	a11, 12
     6e0:	aa3a           	add.n	a10, a10, a3
     6e2:	0008e0        	callx8	a8
     6e5:	18c332        	addi	a3, a3, 24
     6e8:	53b6a0        	max	a11, a6, a10
     6eb:	09e4b4        	ae_s32ip	a11, a4, 4
     6ee:	e79377        	bne	a3, a7, 6d9 <NR_vad2+0x45>
     6f1:	01a8           	l32i.n	a10, a1, 0
     6f3:	31b8           	l32i.n	a11, a1, 12
     6f5:	21c8           	l32i.n	a12, a1, 8

000006f7 <NR_vad2+0x63>:
     6f7:	c298           	l32i.n	a9, a2, 48
     6f9:	2d02f2        	l8ui	a15, a2, 45
     6fc:	21c9           	s32i.n	a12, a1, 8
     6fe:	31b9           	s32i.n	a11, a1, 12
     700:	01a9           	s32i.n	a10, a1, 0
     702:	01a8           	l32i.n	a10, a1, 0
     704:	31b8           	l32i.n	a11, a1, 12
     706:	21c8           	l32i.n	a12, a1, 8
     708:	ff1b           	addi.n	a15, a15, 1
     70a:	0bdd           	mov.n	a13, a11
     70c:	74f0f0        	extui	a15, a15, 0, 8
     70f:	2d42f2        	s8i	a15, a2, 45
     712:	06af66        	bnei	a15, 12, 71c <NR_vad2+0x88>
     715:	0f0c           	movi.n	a15, 0
     717:	080c           	movi.n	a8, 0
     719:	2d4282        	s8i	a8, a2, 45

0000071c <NR_vad2+0x88>:
     71c:	600cf4        	ae_lq32f.i	aeq0, a12, -4
     71f:	909f90        	addx2	a9, a15, a9
     722:	e8c992        	addi	a9, a9, -24
     725:	079a76        	loopnez	a10, 730 <NR_vad2+0x9c>
     728:	f8e3bd1d1f 	{ ae_lq32f.iu	aeq0, a13, 4; ae_roundsp16q48sym	aep0, aeq0 }
     72d:	438954        	ae_sp16f.l.xu	aep0, a9, a5

00000730 <NR_vad2+0x9c>:
     730:	1198           	l32i.n	a9, a1, 4
     732:	1512e2        	l16ui	a14, a2, 42
     735:	000a46        	j	762 <NR_vad2+0xce>

00000738 <NR_vad2+0xa4>:
     738:	000000                                        ...

0000073b <NR_vad2+0xa7>:
     73b:	c288           	l32i.n	a8, a2, 48
     73d:	2d0292        	l8ui	a9, a2, 45
     740:	0bdd           	mov.n	a13, a11
     742:	600cf4        	ae_lq32f.i	aeq0, a12, -4
     745:	909980        	addx2	a9, a9, a8
     748:	e8c992        	addi	a9, a9, -24
     74b:	1a9c           	beqz.n	a10, 760 <NR_vad2+0xcc>
     74d:	0c9a76        	loopnez	a10, 75d <NR_vad2+0xc9>
     750:	f9e39a195f 	{ ae_lp16f.xu	aep1, a9, a5; ae_roundsp16q48sym	aep2, aeq0 }
     755:	d846fd1d1f 	{ ae_lq32f.iu	aeq0, a13, 4; ae_minp24s	aep1, aep1, aep2 }
     75a:	421904        	ae_sp16f.l.i	aep1, a9, 0

0000075d <NR_vad2+0xc9>:
     75d:	1512e2        	l16ui	a14, a2, 42

00000760 <NR_vad2+0xcc>:
     760:	92fa           	add.n	a9, a2, a15

00000762 <NR_vad2+0xce>:
     762:	de1b           	addi.n	a13, a14, 1
     764:	1552d2        	s16i	a13, a2, 42
     767:	0c8ab6        	bltui	a10, 8, 777 <NR_vad2+0xe3>
     76a:	fe48f1        	l32r	a15, 8c (2890 <wb_score_bands>)
     76d:	fe46e1        	l32r	a14, 88 (e38e4 <_end+0xe0ca4>)
     770:	39aee4        	ae_movpa24x2	aep2, a14, a14
     773:	000246        	j	780 <NR_vad2+0xec>

00000776 <NR_vad2+0xe2>:
	...

00000777 <NR_vad2+0xe3>:
     777:	fe46f1        	l32r	a15, 90 (28d0 <nb_score_bands>)
     77a:	fe4681        	l32r	a8, 94 (124925 <_end+0x121ce5>)
     77d:	39a884        	ae_movpa24x2	aep2, a8, a8

00000780 <NR_vad2+0xec>:
     780:	09ed           	mov.n	a14, a9
     782:	3c2292        	l32i	a9, a2, 240
     785:	fecff2        	addi	a15, a15, -2
     788:	0bdd           	mov.n	a13, a11
     78a:	fe248fc9be 	{ addi	a11, a9, -4; ae_zeroq56	aeq2 }
     78f:	fc20c0629e 	{ addi	a9, a2, 6; ae_zerop48	aep1 }
     794:	224a66        	bnei	a10, 4, 7ba <NR_vad2+0x126>
     797:	604e04        	ae_lq32f.i	aeq1, a14, 0
     79a:	600d04        	ae_lq32f.i	aeq0, a13, 0
     79d:	179a76        	loopnez	a10, 7b8 <NR_vad2+0x124>
     7a0:	fa26ecc91f 	{ ae_lp16f.iu	aep4, a9, 2; ae_subq56	aeq2, aeq0, aeq1 }
     7a5:	0ac484        	ae_cvtq48p24s.l	aeq3, aep4
     7a8:	fab6fd3e1f 	{ ae_lq32f.iu	aeq1, a14, 4; ae_subq56	aeq2, aeq2, aeq3 }
     7ad:	35a074        	ae_sraiq56	aeq2, aeq2, 1
     7b0:	f9d27d1d1f 	{ ae_lq32f.iu	aeq0, a13, 4; ae_truncp24q48x2	aep3, aeq2, aeq2 }
     7b5:	52bb14        	ae_sp24f.l.iu	aep3, a11, 4

000007b8 <NR_vad2+0x124>:
     7b8:	f01d           	retw.n

000007ba <NR_vad2+0x126>:
     7ba:	600cf4        	ae_lq32f.i	aeq0, a12, -4
     7bd:	01d282        	addmi	a8, a2, 0x100
     7c0:	c0c882        	addi	a8, a8, -64
     7c3:	604884        	ae_lq32f.i	aeq1, a8, -32
     7c6:	259a76        	loopnez	a10, 7ef <NR_vad2+0x15b>
     7c9:	f826ecd91f 	{ ae_lp16f.iu	aep5, a9, 2; ae_subq56	aeq0, aeq0, aeq1 }
     7ce:	0ac584        	ae_cvtq48p24s.l	aeq3, aep5
     7d1:	098304        	ae_ltq56s	b0, aeq3, aeq0
     7d4:	f5caac8f1f 	{ ae_lp16f.iu	aep0, a15, 2; ae_mulafq32sp24s.l	aeq2, aeq0, aep2 }
     7d9:	020076        	bf	b0, 7df <NR_vad2+0x14b>
     7dc:	079014        	ae_addsp24s	aep1, aep1, aep0

000007df <NR_vad2+0x14b>:
     7df:	f9a6fd1d1f 	{ ae_lq32f.iu	aeq0, a13, 4; ae_subq56	aeq1, aeq0, aeq3 }
     7e4:	355074        	ae_sraiq56	aeq1, aeq1, 1
     7e7:	fb4e7d3e1f 	{ ae_lq32f.iu	aeq1, a14, 4; ae_truncp24q48x2	aep7, aeq1, aeq1 }
     7ec:	52fb14        	ae_sp24f.l.iu	aep7, a11, 4

000007ef <NR_vad2+0x15b>:
     7ef:	408914        	ae_lp16f.iu	aep0, a9, 2
     7f2:	0ac084        	ae_cvtq48p24s.l	aeq3, aep0
     7f5:	09a314        	ae_ltq56s	b1, aeq3, aeq2
     7f8:	408f14        	ae_lp16f.iu	aep0, a15, 2
     7fb:	28c292        	addi	a9, a2, 40
     7fe:	020176        	bf	b1, 804 <NR_vad2+0x170>
     801:	079014        	ae_addsp24s	aep1, aep1, aep0

00000804 <NR_vad2+0x170>:
     804:	421904        	ae_sp16f.l.i	aep1, a9, 0
     807:	f01d           	retw.n

00000809 <NR_vad2+0x175>:
     809:	000000                                        ...

0000080c <NR_NoiseEstimate>:
     80c:	004136        	entry	a1, 32
     80f:	0c0c           	movi.n	a12, 0
     811:	0022f2        	l32i	a15, a2, 0
     814:	0e92b2        	l16si	a11, a2, 28
     817:	2f0262        	l8ui	a6, a2, 47
     81a:	08dfa2        	addmi	a10, a15, 0x800
     81d:	11bbc0        	slli	a11, a11, 4
     820:	0cdff2        	addmi	a15, a15, 0xc00
     823:	b8cff2        	addi	a15, a15, -72
     826:	209ff0        	or	a9, a15, a15
     829:	059676        	loopnez	a6, 832 <NR_NoiseEstimate+0x26>
     82c:	09eab4        	ae_s32ip	a11, a10, 4
     82f:	09e9c4        	ae_s32ip	a12, a9, 4

00000832 <NR_NoiseEstimate+0x26>:
     832:	1412a2        	l16ui	a10, a2, 40
     835:	4c1c           	movi.n	a12, 20
     837:	493ca7        	bltu	a12, a10, 884 <NR_NoiseEstimate+0x78>
     83a:	2e0292        	l8ui	a9, a2, 46
     83d:	027ab6        	bltui	a10, 7, 843 <NR_NoiseEstimate+0x37>
     840:	002dc6        	j	8fb <NR_NoiseEstimate+0xef>
     843:	3322a2        	l32i	a10, a2, 204
     846:	0000c6        	j	84d <NR_NoiseEstimate+0x41>

00000849 <NR_NoiseEstimate+0x3d>:
	...

0000084a <NR_NoiseEstimate+0x3e>:
     84a:	3522a2        	l32i	a10, a2, 212

0000084d <NR_NoiseEstimate+0x41>:
     84d:	39bc           	beqz.n	a9, 884 <NR_NoiseEstimate+0x78>
     84f:	11d9e0        	slli	a13, a9, 2
     852:	0b0c           	movi.n	a11, 0
     854:	fe07c1        	l32r	a12, 70 (28b0 <wb_sb_ptn>)
     857:	140c           	movi.n	a4, 1

00000859 <NR_NoiseEstimate+0x4d>:
     859:	3c22e2        	l32i	a14, a2, 240
     85c:	eeba           	add.n	a14, a14, a11
     85e:	0ee8           	l32i.n	a14, a14, 0
     860:	bb4b           	addi.n	a11, a11, 4
     862:	191ee6        	bgei	a14, 1, 87f <NR_NoiseEstimate+0x73>
     865:	2c3d           	ae_l16si.n	a3, a12, 0
     867:	3ced           	ae_l16si.n	a14, a12, 2
     869:	a093f0        	addx4	a9, a3, a15
     86c:	0fa3e7        	bge	a3, a14, 87f <NR_NoiseEstimate+0x73>
     86f:	c08e30        	sub	a8, a14, a3
     872:	538840        	max	a8, a8, a4
     875:	06a876        	loopgtz	a8, 87f <NR_NoiseEstimate+0x73>
     878:	09e8           	l32i.n	a14, a9, 0
     87a:	eeaa           	add.n	a14, a14, a10
     87c:	09e9e4        	ae_s32ip	a14, a9, 4
     87f:	cc2b           	addi.n	a12, a12, 2
     881:	d49bd7        	bne	a11, a13, 859 <NR_NoiseEstimate+0x4d>
     884:	fccf52        	addi	a5, a15, -4
     887:	6f92d2        	l16si	a13, a2, 222
     88a:	0248           	l32i.n	a4, a2, 0
     88c:	6e92c2        	l16si	a12, a2, 220
     88f:	402232        	l32i	a3, a2, 0x100
     892:	44cb           	addi.n	a4, a4, 12
     894:	111cd7        	beq	a12, a13, 8a9 <NR_NoiseEstimate+0x9d>
     897:	04ad           	mov.n	a10, a4
     899:	fdff81        	l32r	a8, 98 (3bc <equal_power>)
     89c:	03bd           	mov.n	a11, a3
     89e:	06ed           	mov.n	a14, a6
     8a0:	0008e0        	callx8	a8
     8a3:	6f52a2        	s16i	a10, a2, 222
     8a6:	6e52a2        	s16i	a10, a2, 220

000008a9 <NR_NoiseEstimate+0x9d>:
     8a9:	fcc332        	addi	a3, a3, -4
     8ac:	fc24bd440f 	{ ae_lq32f.i	aeq2, a4, 0; ae_zeroq56	aeq0 }
     8b1:	1e9676        	loopnez	a6, 8d3 <NR_NoiseEstimate+0xc7>
     8b4:	448514        	ae_lp16x2f.iu	aep0, a5, 4
     8b7:	259d           	ae_l16si.n	a9, a5, 0
     8b9:	605314        	ae_lq32f.iu	aeq1, a3, 4
     8bc:	c18864        	ae_mulzaafq32sp16s.lh	aeq3, aeq1, aep0, aeq2, aep0
     8bf:	69cc           	bnez.n	a9, 8c9 <NR_NoiseEstimate+0xbd>
     8c1:	f86b3d541f 	{ ae_lq32f.iu	aeq2, a4, 4; ae_maxq56s	aeq0, aeq0, aeq1 }
     8c6:	0001c6        	j	8d1 <NR_NoiseEstimate+0xc5>

000008c9 <NR_NoiseEstimate+0xbd>:
     8c9:	609414        	ae_lq32f.iu	aeq2, a4, 4
     8cc:	f87b39b30f 	{ ae_sq32f.i	aeq3, a3, 0; ae_maxq56s	aeq0, aeq0, aeq3 }

000008d1 <NR_NoiseEstimate+0xc5>:
     8d1:	f03d           	nop.n

000008d3 <NR_NoiseEstimate+0xc7>:
     8d3:	364a04        	ae_nsaq56s	a10, aeq0
     8d6:	8b0c           	movi.n	a11, 8
     8d8:	1daba7        	bge	a11, a10, 8f9 <NR_NoiseEstimate+0xed>
     8db:	402232        	l32i	a3, a2, 0x100
     8de:	6f9282        	l16si	a8, a2, 222
     8e1:	f8ca92        	addi	a9, a10, -8
     8e4:	fcc332        	addi	a3, a3, -4
     8e7:	c08890        	sub	a8, a8, a9
     8ea:	6f5282        	s16i	a8, a2, 222
     8ed:	089676        	loopnez	a6, 8f9 <NR_NoiseEstimate+0xed>
     8f0:	601314        	ae_lq32f.iu	aeq0, a3, 4
     8f3:	340994        	ae_sllaq56	aeq0, aeq0, a9
     8f6:	620304        	ae_sq32f.i	aeq0, a3, 0
     8f9:	f01d           	retw.n

000008fb <NR_NoiseEstimate+0xef>:
     8fb:	02bab6        	bltui	a10, 16, 901 <NR_NoiseEstimate+0xf5>
     8fe:	ffd206        	j	84a <NR_NoiseEstimate+0x3e>
     901:	3422a2        	l32i	a10, a2, 208
     904:	ffd146        	j	84d <NR_NoiseEstimate+0x41>

00000907 <NR_NoiseEstimate+0xfb>:
	...

00000908 <NR_NoiseWithGain>:
     908:	004136        	entry	a1, 32
     90b:	02a8           	l32i.n	a10, a2, 0
     90d:	141272        	l16ui	a7, a2, 40
     910:	2e0262        	l8ui	a6, a2, 46
     913:	08daa2        	addmi	a10, a10, 0x800
     916:	5077b6        	bltui	a7, 7, 96a <NR_NoiseWithGain+0x62>
     919:	0f9252        	l16si	a5, a2, 30
     91c:	4b1c           	movi.n	a11, 20
     91e:	073b77        	bltu	a11, a7, 929 <NR_NoiseWithGain+0x21>
     921:	551b           	addi.n	a5, a5, 1
     923:	215150        	srai	a5, a5, 1
     926:	40d552        	addmi	a5, a5, 0x4000

00000929 <NR_NoiseWithGain+0x21>:
     929:	d6bc           	beqz.n	a6, 96a <NR_NoiseWithGain+0x62>
     92b:	1b0c           	movi.n	a11, 1
     92d:	300554        	ae_cvtp24a16x2.ll	aep0, a5, a5
     930:	1196e0        	slli	a9, a6, 2
     933:	050c           	movi.n	a5, 0
     935:	fdce61        	l32r	a6, 70 (28b0 <wb_sb_ptn>)

00000938 <NR_NoiseWithGain+0x30>:
     938:	3c2282        	l32i	a8, a2, 240
     93b:	885a           	add.n	a8, a8, a5
     93d:	0888           	l32i.n	a8, a8, 0
     93f:	04c552        	addi	a5, a5, 4
     942:	1e18a6        	blti	a8, 1, 964 <NR_NoiseWithGain+0x5c>
     945:	268d           	ae_l16si.n	a8, a6, 0
     947:	367d           	ae_l16si.n	a7, a6, 2
     949:	a048a0        	addx4	a4, a8, a10
     94c:	14a877        	bge	a8, a7, 964 <NR_NoiseWithGain+0x5c>
     94f:	c0c780        	sub	a12, a7, a8
     952:	53ccb0        	max	a12, a12, a11
     955:	0bac76        	loopgtz	a12, 964 <NR_NoiseWithGain+0x5c>
     958:	600404        	ae_lq32f.i	aeq0, a4, 0
     95b:	030214        	ae_mulfq32sp16s.l	aeq0, aeq0, aep0
     95e:	368d04        	ae_trunca32q48	a13, aeq0
     961:	09e4d4        	ae_s32ip	a13, a4, 4
     964:	02c662        	addi	a6, a6, 2
     967:	cd9597        	bne	a5, a9, 938 <NR_NoiseWithGain+0x30>
     96a:	204aa0        	or	a4, a10, a10
     96d:	fcca62        	addi	a6, a10, -4
     970:	4c0a04        	ae_lp24.i	aep0, a10, 0
     973:	402252        	l32i	a5, a2, 0x100
     976:	6f92f2        	l16si	a15, a2, 222
     979:	2f02e2        	l8ui	a14, a2, 47
     97c:	600504        	ae_lq32f.i	aeq0, a5, 0
     97f:	06cff2        	addi	a15, a15, 6
     982:	7052f2        	s16i	a15, a2, 224
     985:	0a9e76        	loopnez	a14, 993 <NR_NoiseWithGain+0x8b>
     988:	f8e071841f 	{ ae_lp24.iu	aep0, a4, 4; ae_mulrfq32sp24s.l	aeq1, aeq0, aep0 }
     98d:	601514        	ae_lq32f.iu	aeq0, a5, 4
     990:	625614        	ae_sq32f.iu	aeq1, a6, 4

00000993 <NR_NoiseWithGain+0x8b>:
     993:	f01d           	retw.n

00000995 <NR_NoiseWithGain+0x8d>:
     995:	000000                                        ...

00000998 <common_gain_limiter>:
     998:	004136        	entry	a1, 32
     99b:	37a444        	ae_truncp24a32x2	aep2, a4, a4
     99e:	400304        	ae_lp16f.i	aep0, a3, 0
     9a1:	f03d           	nop.n
     9a3:	f03d           	nop.n
     9a5:	07a276        	loopgtz	a2, 9b0 <common_gain_limiter+0x18>
     9a8:	d80a6c832f 	{ ae_lp16f.iu	aep0, a3, 4; ae_maxp24s	aep1, aep2, aep0 }
     9ad:	5213f4        	ae_sp24f.l.i	aep1, a3, -4

000009b0 <common_gain_limiter+0x18>:
     9b0:	032d           	mov.n	a2, a3
     9b2:	f01d           	retw.n

000009b4 <NR_WienerFilter>:
     9b4:	006136        	entry	a1, 48
     9b7:	2f0232        	l8ui	a3, a2, 47
     9ba:	7092d2        	l16si	a13, a2, 224
     9bd:	0278           	l32i.n	a7, a2, 0
     9bf:	6e9242        	l16si	a4, a2, 220
     9c2:	7192c2        	l16si	a12, a2, 226
     9c5:	08d762        	addmi	a6, a7, 0x800
     9c8:	04ad           	mov.n	a10, a4
     9ca:	77cb           	addi.n	a7, a7, 12
     9cc:	075d           	mov.n	a5, a7
     9ce:	0ba4c7        	bge	a4, a12, 9dd <NR_WienerFilter+0x29>
     9d1:	0c4d           	mov.n	a4, a12
     9d3:	412252        	l32i	a5, a2, 0x104
     9d6:	180c           	movi.n	a8, 1
     9d8:	0189           	s32i.n	a8, a1, 0
     9da:	0000c6        	j	9e1 <NR_WienerFilter+0x2d>

000009dd <NR_WienerFilter+0x29>:
     9dd:	090c           	movi.n	a9, 0
     9df:	0199           	s32i.n	a9, a1, 0

000009e1 <NR_WienerFilter+0x2d>:
     9e1:	03a4d7        	bge	a4, a13, 9e8 <NR_WienerFilter+0x34>
     9e4:	0d4d           	mov.n	a4, a13
     9e6:	065d           	mov.n	a5, a6

000009e8 <NR_WienerFilter+0x34>:
     9e8:	121a47        	beq	a10, a4, 9fe <NR_WienerFilter+0x4a>
     9eb:	07bd           	mov.n	a11, a7
     9ed:	04cd           	mov.n	a12, a4
     9ef:	0add           	mov.n	a13, a10
     9f1:	fda981        	l32r	a8, 98 (3bc <equal_power>)
     9f4:	03ed           	mov.n	a14, a3
     9f6:	05ad           	mov.n	a10, a5
     9f8:	0008e0        	callx8	a8
     9fb:	7192c2        	l16si	a12, a2, 226

000009fe <NR_WienerFilter+0x4a>:
     9fe:	101c47        	beq	a12, a4, a12 <NR_WienerFilter+0x5e>
     a01:	05ad           	mov.n	a10, a5
     a03:	4122b2        	l32i	a11, a2, 0x104
     a06:	0cdd           	mov.n	a13, a12
     a08:	fda481        	l32r	a8, 98 (3bc <equal_power>)
     a0b:	03ed           	mov.n	a14, a3
     a0d:	04cd           	mov.n	a12, a4
     a0f:	0008e0        	callx8	a8

00000a12 <NR_WienerFilter+0x5e>:
     a12:	7092d2        	l16si	a13, a2, 224
     a15:	0d1d47        	beq	a13, a4, a26 <NR_WienerFilter+0x72>
     a18:	05ad           	mov.n	a10, a5
     a1a:	06bd           	mov.n	a11, a6
     a1c:	fd9f81        	l32r	a8, 98 (3bc <equal_power>)
     a1f:	04cd           	mov.n	a12, a4
     a21:	03ed           	mov.n	a14, a3
     a23:	0008e0        	callx8	a8

00000a26 <NR_WienerFilter+0x72>:
     a26:	600604        	ae_lq32f.i	aeq0, a6, 0
     a29:	22c252        	addi	a5, a2, 34
     a2c:	6e5242        	s16i	a4, a2, 220
     a2f:	705242        	s16i	a4, a2, 224
     a32:	715242        	s16i	a4, a2, 226
     a35:	4122b2        	l32i	a11, a2, 0x104
     a38:	01d292        	addmi	a9, a2, 0x100
     a3b:	fcc772        	addi	a7, a7, -4
     a3e:	07ad           	mov.n	a10, a7
     a40:	f8c992        	addi	a9, a9, -8
     a43:	604b04        	ae_lq32f.i	aeq1, a11, 0
     a46:	440984        	ae_lp16x2f.i	aep0, a9, -32
     a49:	045316        	beqz	a3, a92 <NR_WienerFilter+0xde>
     a4c:	fd84d1        	l32r	a13, 5c (7fff <_end+0x53bf>)
     a4f:	3c9376        	loopnez	a3, a8f <NR_WienerFilter+0xdb>
     a52:	fa6a3d7a1f 	{ ae_lq32f.iu	aeq3, a10, 4; ae_addq56	aeq2, aeq1, aeq0 }
     a57:	c188e4        	ae_mulzaafq32sp16s.lh	aeq3, aeq3, aep0, aeq2, aep0
     a5a:	364c04        	ae_nsaq56s	a12, aeq0
     a5d:	367e04        	ae_nsaq56s	a14, aeq3
     a60:	43cce0        	min	a12, a12, a14
     a63:	e9cce2        	addi	a14, a12, -23
     a66:	fc24febe8f 	{ ae_slaasq56s	aeq2, aeq3, a14; nop }
     a6b:	36a904        	ae_trunca32q48	a9, aeq2
     a6e:	f8ccc2        	addi	a12, a12, -8
     a71:	340c94        	ae_sllaq56	aeq0, aeq0, a12
     a74:	49cc           	bnez.n	a9, a7c <NR_WienerFilter+0xc8>
     a76:	fd8991        	l32r	a9, 9c (8000 <_end+0x53c0>)
     a79:	000146        	j	a82 <NR_WienerFilter+0xce>

00000a7c <NR_WienerFilter+0xc8>:
     a7c:	368f04        	ae_trunca32q48	a15, aeq0
     a7f:	d29f90        	quos	a9, a15, a9

00000a82 <NR_WienerFilter+0xce>:
     a82:	601614        	ae_lq32f.iu	aeq0, a6, 4
     a85:	0bcd           	mov.n	a12, a11
     a87:	c08d90        	sub	a8, a13, a9
     a8a:	605b14        	ae_lq32f.iu	aeq1, a11, 4
     a8d:	0c89           	s32i.n	a8, a12, 0

00000a8f <NR_WienerFilter+0xdb>:
     a8f:	4122b2        	l32i	a11, a2, 0x104

00000a92 <NR_WienerFilter+0xde>:
     a92:	78a0d2        	movi	a13, 120
     a95:	76bd37        	bgeu	a13, a3, b0f <NR_WienerFilter+0x15b>
     a98:	fd8281        	l32r	a8, a0 (998 <common_gain_limiter>)
     a9b:	400504        	ae_lp16f.i	aep0, a5, 0
     a9e:	01d292        	addmi	a9, a2, 0x100
     aa1:	60d944        	ae_lq32f.iu	aeq3, a9, 16
     aa4:	038ad4        	ae_mulfq32sp16s.l	aeq3, aeq3, aep0
     aa7:	8a92a2        	l16si	a10, a2, 0x114
     aaa:	36bc04        	ae_trunca32q48	a12, aeq3
     aad:	0008e0        	callx8	a8
     ab0:	8a92c2        	l16si	a12, a2, 0x114
     ab3:	0abd           	mov.n	a11, a10
     ab5:	119242        	l16si	a4, a2, 34
     ab8:	8b92a2        	l16si	a10, a2, 0x116
     abb:	fd7981        	l32r	a8, a0 (998 <common_gain_limiter>)
     abe:	114400        	slli	a4, a4, 16
     ac1:	c0aac0        	sub	a10, a10, a12
     ac4:	04cd           	mov.n	a12, a4
     ac6:	0008e0        	callx8	a8
     ac9:	379444        	ae_truncp24a32x2	aep1, a4, a4
     acc:	0abd           	mov.n	a11, a10
     ace:	01d262        	addmi	a6, a2, 0x100
     ad1:	fd67a1        	l32r	a10, 70 (28b0 <wb_sb_ptn>)
     ad4:	601634        	ae_lq32f.iu	aeq0, a6, 12
     ad7:	d4e628b2ce 	{ l16si	a12, a2, 0x116; ae_mulfq32sp16s.l	aeq0, aeq0, aep1 }
     adc:	089aa2        	l16si	a10, a10, 16
     adf:	368404        	ae_trunca32q48	a4, aeq0
     ae2:	fd6f81        	l32r	a8, a0 (998 <common_gain_limiter>)
     ae5:	c0aac0        	sub	a10, a10, a12
     ae8:	04cd           	mov.n	a12, a4
     aea:	0008e0        	callx8	a8
     aed:	fd6091        	l32r	a9, 70 (28b0 <wb_sb_ptn>)
     af0:	503604        	ae_lp24f.i	aep3, a6, 0
     af3:	37a444        	ae_truncp24a32x2	aep2, a4, a4
     af6:	ea69d3baaf 	{ or	a11, a10, a10; ae_mulfp24s.ll	aeq1, aep2, aep3 }
     afb:	089992        	l16si	a9, a9, 16
     afe:	369404        	ae_trunca32q48	a4, aeq1
     b01:	fd6781        	l32r	a8, a0 (998 <common_gain_limiter>)
     b04:	c0a390        	sub	a10, a3, a9
     b07:	04cd           	mov.n	a12, a4
     b09:	0008e0        	callx8	a8
     b0c:	0003c6        	j	b1f <NR_WienerFilter+0x16b>

00000b0f <NR_WienerFilter+0x15b>:
     b0f:	119242        	l16si	a4, a2, 34
     b12:	03ad           	mov.n	a10, a3
     b14:	fd6381        	l32r	a8, a0 (998 <common_gain_limiter>)
     b17:	114400        	slli	a4, a4, 16
     b1a:	04cd           	mov.n	a12, a4
     b1c:	0008e0        	callx8	a8

00000b1f <NR_WienerFilter+0x16b>:
     b1f:	0298           	l32i.n	a9, a2, 0
     b21:	404504        	ae_lp16f.i	aep4, a5, 0
     b24:	4422a2        	l32i	a10, a2, 0x110
     b27:	368a34        	ae_cvtq48a32s	aeq2, a10
     b2a:	d7f200038f 	{ movi	a8, 3; ae_mulfq32sp16s.l	aeq2, aeq2, aep4 }
     b2f:	07ad           	mov.n	a10, a7
     b31:	f8f78fc99e 	{ addi	a9, a9, -4; ae_roundsp24q48sym	aep0, aeq2 }
     b36:	02a876        	loopgtz	a8, b3c <NR_WienerFilter+0x188>
     b39:	528914        	ae_sp24f.l.iu	aep0, a9, 4

00000b3c <NR_WienerFilter+0x188>:
     b3c:	4122c2        	l32i	a12, a2, 0x104
     b3f:	01e8           	l32i.n	a14, a1, 0
     b41:	00a0b2        	movi	a11, 0
     b44:	500c04        	ae_lp24f.i	aep0, a12, 0
     b47:	c39c           	beqz.n	a3, b67 <NR_WienerFilter+0x1b3>
     b49:	179376        	loopnez	a3, b64 <NR_WienerFilter+0x1b0>
     b4c:	60da14        	ae_lq32f.iu	aeq3, a10, 4
     b4f:	088de4        	ae_mulfq32sp24s.l	aeq3, aeq3, aep0
     b52:	088dd4        	ae_mulrfq32sp24s.l	aeq3, aeq3, aep0
     b55:	36bd04        	ae_trunca32q48	a13, aeq3
     b58:	623c04        	ae_sq32f.i	aeq3, a12, 0
     b5b:	528914        	ae_sp24f.l.iu	aep0, a9, 4
     b5e:	508c14        	ae_lp24f.iu	aep0, a12, 4
     b61:	53bbd0        	max	a11, a11, a13
     b64:	000046        	j	b69 <NR_WienerFilter+0x1b5>

00000b67 <NR_WienerFilter+0x1b3>:
     b67:	0b0c           	movi.n	a11, 0

00000b69 <NR_WienerFilter+0x1b5>:
     b69:	231e66        	bnei	a14, 1, b90 <NR_WienerFilter+0x1dc>
     b6c:	7192f2        	l16si	a15, a2, 226
     b6f:	40eba0        	nsa	a10, a11
     b72:	c0ffa0        	sub	a15, a15, a10
     b75:	7152f2        	s16i	a15, a2, 226
     b78:	439c           	beqz.n	a3, b90 <NR_WienerFilter+0x1dc>
     b7a:	401a00        	ssl	a10
     b7d:	4122b2        	l32i	a11, a2, 0x104
     b80:	0a0c           	movi.n	a10, 0
     b82:	0a9376        	loopnez	a3, b90 <NR_WienerFilter+0x1dc>
     b85:	cbaa           	add.n	a12, a11, a10
     b87:	0c88           	l32i.n	a8, a12, 0
     b89:	aa4b           	addi.n	a10, a10, 4
     b8b:	a18800        	sll	a8, a8
     b8e:	0c89           	s32i.n	a8, a12, 0
     b90:	80a0e2        	movi	a14, 128
     b93:	fb0c           	movi.n	a11, 15
     b95:	2e02d2        	l8ui	a13, a2, 46
     b98:	5a0c           	movi.n	a10, 5
     b9a:	07a0c2        	movi	a12, 7
     b9d:	62ccd0        	saltu	a12, a12, a13
     ba0:	93abc0        	movnez	a10, a11, a12
     ba3:	101aa6        	blti	a10, 1, bb7 <NR_WienerFilter+0x203>
     ba6:	378444        	ae_truncp24a32x2	aep0, a4, a4
     ba9:	02aa76        	loopgtz	a10, baf <NR_WienerFilter+0x1fb>
     bac:	528914        	ae_sp24f.l.iu	aep0, a9, 4

00000baf <NR_WienerFilter+0x1fb>:
     baf:	2e02d2        	l8ui	a13, a2, 46
     bb2:	7c0c           	movi.n	a12, 7
     bb4:	62ccd0        	saltu	a12, a12, a13

00000bb7 <NR_WienerFilter+0x203>:
     bb7:	0298           	l32i.n	a9, a2, 0
     bb9:	3922a2        	l32i	a10, a2, 228
     bbc:	3a22b2        	l32i	a11, a2, 232
     bbf:	00a1d2        	movi	a13, 0x100
     bc2:	93edc0        	movnez	a14, a13, a12
     bc5:	5c2b04        	ae_lp24x2f.i	aep2, a11, 0
     bc8:	401a04        	ae_lp16f.i	aep1, a10, 0
     bcb:	fcc992        	addi	a9, a9, -4
     bce:	508914        	ae_lp24f.iu	aep0, a9, 4
     bd1:	17ae76        	loopgtz	a14, bec <NR_WienerFilter+0x238>
     bd4:	012954        	ae_mulfp24s.hl	aeq1, aep2, aep1
     bd7:	ea29ac9a1f 	{ ae_lp16f.iu	aep1, a10, 2; ae_mulfp24s.ll	aeq0, aep2, aep1 }
     bdc:	f8e16fab1f 	{ ae_lp24x2f.iu	aep2, a11, 8; ae_mulrfq32sp24s.l	aeq1, aeq1, aep0 }
     be1:	f8e02e891f 	{ ae_lp24f.iu	aep0, a9, 4; ae_mulrfq32sp24s.l	aeq0, aeq0, aep0 }
     be6:	621be4        	ae_sq32f.i	aeq1, a11, -8
     be9:	620bf4        	ae_sq32f.i	aeq0, a11, -4

00000bec <NR_WienerFilter+0x238>:
     bec:	f01d           	retw.n

00000bee <NR_WienerFilter+0x23a>:
	...

00000bf0 <ifft_wo_single>:
     bf0:	004136        	entry	a1, 32
     bf3:	2d0c           	movi.n	a13, 2
     bf5:	2e0282        	l8ui	a8, a2, 46
     bf8:	fd2cb1        	l32r	a11, a8 (2590 <NR_immediate_num>)
     bfb:	0248           	l32i.n	a4, a2, 0
     bfd:	3a2292        	l32i	a9, a2, 232
     c00:	0a0c           	movi.n	a10, 0
     c02:	04a9           	s32i.n	a10, a4, 0
     c04:	14a9           	s32i.n	a10, a4, 4
     c06:	5c0904        	ae_lp24x2f.i	aep0, a9, 0
     c09:	441b04        	ae_lp16x2f.i	aep1, a11, 0
     c0c:	fd20ed4b1f 	{ ae_lp16x2f.i	aep4, a11, 4; ae_zerop48	aep3 }
     c11:	1188b6        	bltui	a8, 8, c26 <ifft_wo_single+0x36>
     c14:	f0a062        	movi	a6, 240
     c17:	00a252        	movi	a5, 0x200
     c1a:	970c           	movi.n	a7, 9
     c1c:	1c0c           	movi.n	a12, 1
     c1e:	fd21a1        	l32r	a10, a4 (20c2 <fftorder_512_12b+0x2>)
     c21:	000386        	j	c33 <ifft_wo_single+0x43>

00000c24 <ifft_wo_single+0x34>:
	...

00000c26 <ifft_wo_single+0x36>:
     c26:	78a062        	movi	a6, 120
     c29:	00a152        	movi	a5, 0x100
     c2c:	870c           	movi.n	a7, 8
     c2e:	2c0c           	movi.n	a12, 2
     c30:	fd0ca1        	l32r	a10, 60 (20c4 <fftorder_512_12b+0x4>)

00000c33 <ifft_wo_single+0x43>:
     c33:	11bcf0        	slli	a11, a12, 1
     c36:	d2d5d0        	quos	a13, a5, a13
     c39:	16ad76        	loopgtz	a13, c53 <ifft_wo_single+0x63>
     c3c:	1a83e4        	ae_selp24.lh	aep6, aep0, aep3
     c3f:	f09afe0b5f 	{ ae_selp24.hl	aep5, aep0, aep3; ae_mulzaafp24s.hh.ll	aeq1, aep6, aep4 }
     c44:	f036a00aee 	{ l16si	a14, a10, 0; ae_mulzaafp24s.hh.ll	aeq0, aep5, aep1 }
     c49:	aaba           	add.n	a10, a10, a11
     c4b:	f9462f891f 	{ ae_lp24x2f.iu	aep0, a9, 8; ae_truncp24q48x2	aep2, aeq0, aeq1 }
     c50:	5f24e4        	ae_sp24x2f.x	aep2, a4, a14

00000c53 <ifft_wo_single+0x63>:
     c53:	11bcf0        	slli	a11, a12, 1
     c56:	fe982f89ef 	{ ae_lp24x2f.iu	aep0, a9, -16; ae_negsp24s	aep4, aep4 }
     c5b:	fd20cffdfe 	{ addi	a15, a13, -1; ae_zerop48	aep3 }
     c60:	16af76        	loopgtz	a15, c7a <ifft_wo_single+0x8a>
     c63:	1a83a4        	ae_selp24.lh	aep2, aep0, aep3
     c66:	f12afe031f 	{ ae_selp24.hl	aep0, aep0, aep3; ae_mulzaafp24s.hh.ll	aeq3, aep2, aep1 }
     c6b:	f182a00a8e 	{ l16si	a8, a10, 0; ae_mulzaafp24s.hh.ll	aeq2, aep0, aep4 }
     c70:	aaba           	add.n	a10, a10, a11
     c72:	fbd66f89ff 	{ ae_lp24x2f.iu	aep0, a9, -8; ae_truncp24q48x2	aep7, aeq2, aeq3 }
     c77:	5f7484        	ae_sp24x2f.x	aep7, a4, a8
     c7a:	04ad           	mov.n	a10, a4
     c7c:	85bd           	ae_sext16	a11, a5
     c7e:	8c0c           	movi.n	a12, 8
     c80:	fcfa81        	l32r	a8, 68 (14c0 <sw_fft_32b_2811>)
     c83:	0d0c           	movi.n	a13, 0
     c85:	01a0e2        	movi	a14, 1
     c88:	0008e0        	callx8	a8
     c8b:	600404        	ae_lq32f.i	aeq0, a4, 0
     c8e:	fec392        	addi	a9, a3, -2
     c91:	3f22d2        	l32i	a13, a2, 252
     c94:	139282        	l16si	a8, a2, 38
     c97:	3d22b2        	l32i	a11, a2, 244
     c9a:	c0f560        	sub	a15, a5, a6
     c9d:	11eff0        	slli	a14, a15, 1
     ca0:	fecbc2        	addi	a12, a11, -2
     ca3:	c08a80        	sub	a8, a10, a8
     ca6:	90bfb0        	addx2	a11, a15, a11
     ca9:	fecda2        	addi	a10, a13, -2
     cac:	c07870        	sub	a7, a8, a7
     caf:	04c772        	addi	a7, a7, 4
     cb2:	2216a6        	blti	a6, 1, cd8 <ifft_wo_single+0xe8>
     cb5:	1ca676        	loopgtz	a6, cd5 <ifft_wo_single+0xe5>
     cb8:	40ebf4        	ae_lp16f.iu	aep6, a11, -2
     cbb:	fc24fe878f 	{ ae_slaasq56s	aeq2, aeq0, a7; nop }
     cc0:	fd546cda1f 	{ ae_lp16f.iu	aep5, a10, 2; ae_roundsq32sym	aeq1, aeq2 }
     cc5:	ead4accc1f 	{ ae_lp16f.iu	aep4, a12, 2; ae_mulfp24s.hl	aeq0, aep5, aep6 }
     cca:	034054        	ae_mulafq32sp16s.l	aeq0, aeq1, aep4

00000ccd <ifft_wo_single+0xdd>:
     ccd:	f9e3fd142f 	{ ae_lq32f.iu	aeq0, a4, 8; ae_roundsp16q48sym	aep3, aeq0 }
     cd2:	42b914        	ae_sp16f.l.iu	aep3, a9, 2

00000cd5 <ifft_wo_single+0xe5>:
     cd5:	3f22d2        	l32i	a13, a2, 252

00000cd8 <ifft_wo_single+0xe8>:
     cd8:	039d           	mov.n	a9, a3
     cda:	400304        	ae_lp16f.i	aep0, a3, 0
     cdd:	90c6d0        	addx2	a12, a6, a13
     ce0:	adea           	add.n	a10, a13, a14
     ce2:	1136f0        	slli	a3, a6, 1
     ce5:	fecaa2        	addi	a10, a10, -2
     ce8:	feccc2        	addi	a12, a12, -2
     ceb:	c03530        	sub	a3, a5, a3
     cee:	1d13a6        	blti	a3, 1, d0f <ifft_wo_single+0x11f>
     cf1:	17a376        	loopgtz	a3, d0c <ifft_wo_single+0x11c>
     cf4:	40bbf4        	ae_lp16f.iu	aep3, a11, -2
     cf7:	ff0c2caa1f 	{ ae_lp16f.iu	aep2, a10, 2; ae_cvtq48p24s.l	aeq3, aep0 }
     cfc:	e36b6c9c1f 	{ ae_lp16f.iu	aep1, a12, 2; ae_mulafp24s.hl	aeq3, aep2, aep3 }
     d01:	408914        	ae_lp16f.iu	aep0, a9, 2
     d04:	fbfbf21a0f 	{ ae_sp16f.l.i	aep1, a10, 0; ae_roundsp16q48sym	aep7, aeq3 }
     d09:	4279f4        	ae_sp16f.l.i	aep7, a9, -2

00000d0c <ifft_wo_single+0x11c>:
     d0c:	3f22d2        	l32i	a13, a2, 252

00000d0f <ifft_wo_single+0x11f>:
     d0f:	600404        	ae_lq32f.i	aeq0, a4, 0
     d12:	fecd92        	addi	a9, a13, -2
     d15:	0caf76        	loopgtz	a15, d25 <ifft_wo_single+0x135>
     d18:	fc24fe870f 	{ ae_slaasq56s	aeq0, aeq0, a7; nop }
     d1d:	fae3bd142f 	{ ae_lq32f.iu	aeq0, a4, 8; ae_roundsp16q48sym	aep4, aeq0 }
     d22:	42c914        	ae_sp16f.l.iu	aep4, a9, 2

00000d25 <ifft_wo_single+0x135>:
     d25:	f01d           	retw.n

00000d27 <ifft_wo_single+0x137>:
	...

00000d28 <xtensa_fft4_32b>:
     d28:	006136        	entry	a1, 48
     d2b:	60c274        	ae_lq32f.i	aeq3, a2, 28
     d2e:	608234        	ae_lq32f.i	aeq2, a2, 12
     d31:	600214        	ae_lq32f.i	aeq0, a2, 4
     d34:	f8e23d225f 	{ ae_lq32f.i	aeq1, a2, 20; ae_addq56	aeq0, aeq0, aeq2 }
     d39:	f9ee3d621f 	{ ae_lq32f.i	aeq3, a2, 4; ae_addq56	aeq1, aeq1, aeq3 }
     d3e:	fa663a310f 	{ ae_sq56s.i	aeq3, a1, 0; ae_addq56	aeq2, aeq0, aeq1 }
     d43:	f826fd226f 	{ ae_lq32f.i	aeq1, a2, 24; ae_subq56	aeq0, aeq0, aeq1 }
     d48:	622214        	ae_sq32f.i	aeq2, a2, 4
     d4b:	60c254        	ae_lq32f.i	aeq3, a2, 20
     d4e:	620254        	ae_sq32f.i	aeq0, a2, 20
     d51:	608244        	ae_lq32f.i	aeq2, a2, 16
     d54:	600224        	ae_lq32f.i	aeq0, a2, 8
     d57:	f9ea3d420f 	{ ae_lq32f.i	aeq2, a2, 0; ae_addq56	aeq1, aeq1, aeq2 }
     d5c:	01a8c4        	ae_addq56	aeq2, aeq2, aeq0
     d5f:	0129c4        	ae_addq56	aeq0, aeq2, aeq1
     d62:	fa36fd220f 	{ ae_lq32f.i	aeq1, a2, 0; ae_subq56	aeq2, aeq2, aeq1 }
     d67:	620204        	ae_sq32f.i	aeq0, a2, 0
     d6a:	600274        	ae_lq32f.i	aeq0, a2, 28
     d6d:	fb3afd022f 	{ ae_lq32f.i	aeq0, a2, 8; ae_subq56	aeq3, aeq3, aeq0 }
     d72:	f92afd024f 	{ ae_lq32f.i	aeq0, a2, 16; ae_subq56	aeq1, aeq1, aeq0 }
     d77:	faaef9a24f 	{ ae_sq32f.i	aeq2, a2, 16; ae_subq56	aeq2, aeq1, aeq3 }
     d7c:	f9ee3d626f 	{ ae_lq32f.i	aeq3, a2, 24; ae_addq56	aeq1, aeq1, aeq3 }
     d81:	fb3afd023f 	{ ae_lq32f.i	aeq0, a2, 12; ae_subq56	aeq3, aeq3, aeq0 }
     d86:	621224        	ae_sq32f.i	aeq1, a2, 8
     d89:	614104        	ae_lq56.i	aeq1, a1, 0
     d8c:	015cc4        	ae_subq56	aeq1, aeq1, aeq0
     d8f:	f8ee39a26f 	{ ae_sq32f.i	aeq2, a2, 24; ae_addq56	aeq0, aeq1, aeq3 }
     d94:	f9aef9823f 	{ ae_sq32f.i	aeq0, a2, 12; ae_subq56	aeq1, aeq1, aeq3 }
     d99:	621274        	ae_sq32f.i	aeq1, a2, 28
     d9c:	f01d           	retw.n

00000d9e <xtensa_fft4_32b+0x76>:
	...

00000da0 <xtensa_fft8_32b>:
     da0:	010136        	entry	a1, 128
     da3:	604244        	ae_lq32f.i	aeq1, a2, 16
     da6:	600264        	ae_lq32f.i	aeq0, a2, 24
     da9:	fcc0f1        	l32r	a15, ac (5a82 <_end+0x2e42>)
     dac:	fa26c781ce 	{ addi	a12, a1, 120; ae_subq56	aeq2, aeq0, aeq1 }
     db1:	632154        	ae_sq56s.i	aeq2, a1, 40
     db4:	f8663d220f 	{ ae_lq32f.i	aeq1, a2, 0; ae_addq56	aeq0, aeq0, aeq1 }
     db9:	608224        	ae_lq32f.i	aeq2, a2, 8
     dbc:	fbaafa0ccf 	{ ae_sq56s.i	aeq0, a12, -32; ae_subq56	aeq3, aeq1, aeq2 }
     dc1:	f9ea3d425f 	{ ae_lq32f.i	aeq2, a2, 20; ae_addq56	aeq1, aeq1, aeq2 }
     dc6:	633164        	ae_sq56s.i	aeq3, a1, 48
     dc9:	631c94        	ae_sq56s.i	aeq1, a12, -56
     dcc:	60c274        	ae_lq32f.i	aeq3, a2, 28
     dcf:	f8b6fd221f 	{ ae_lq32f.i	aeq1, a2, 4; ae_subq56	aeq0, aeq2, aeq3 }
     dd4:	faf63d623f 	{ ae_lq32f.i	aeq3, a2, 12; ae_addq56	aeq2, aeq2, aeq3 }
     dd9:	f8aefa017f 	{ ae_sq56s.i	aeq0, a1, 56; ae_subq56	aeq0, aeq1, aeq3 }
     dde:	f9ee0202de 	{ addi	a13, a2, 32; ae_addq56	aeq1, aeq1, aeq3 }
     de3:	fbaafa0caf 	{ ae_sq56s.i	aeq0, a12, -48; ae_subq56	aeq3, aeq1, aeq2 }
     de8:	623254        	ae_sq32f.i	aeq3, a2, 20
     deb:	f9ea3dcc9f 	{ ae_lq56.i	aeq2, a12, -56; ae_addq56	aeq1, aeq1, aeq2 }
     df0:	61ccc4        	ae_lq56.i	aeq3, a12, -32
     df3:	f8f639921f 	{ ae_sq32f.i	aeq1, a2, 4; ae_addq56	aeq0, aeq2, aeq3 }
     df8:	620204        	ae_sq32f.i	aeq0, a2, 0
     dfb:	fab6fde16f 	{ ae_lq56.i	aeq3, a1, 48; ae_subq56	aeq2, aeq2, aeq3 }
     e00:	610174        	ae_lq56.i	aeq0, a1, 56
     e03:	f93af9a24f 	{ ae_sq32f.i	aeq2, a2, 16; ae_subq56	aeq1, aeq3, aeq0 }
     e08:	fb7a3d8caf 	{ ae_lq56.i	aeq0, a12, -48; ae_addq56	aeq3, aeq3, aeq0 }
     e0d:	621264        	ae_sq32f.i	aeq1, a2, 24
     e10:	614154        	ae_lq56.i	aeq1, a1, 40
     e13:	fa6639b22f 	{ ae_sq32f.i	aeq3, a2, 8; ae_addq56	aeq2, aeq0, aeq1 }
     e18:	f826f9a23f 	{ ae_sq32f.i	aeq2, a2, 12; ae_subq56	aeq0, aeq0, aeq1 }
     e1d:	620274        	ae_sq32f.i	aeq0, a2, 28
     e20:	60cd74        	ae_lq32f.i	aeq3, a13, 28
     e23:	604d54        	ae_lq32f.i	aeq1, a13, 20
     e26:	faee3d0d3f 	{ ae_lq32f.i	aeq0, a13, 12; ae_addq56	aeq2, aeq1, aeq3 }
     e2b:	622d54        	ae_sq32f.i	aeq2, a13, 20
     e2e:	f9aefd4d1f 	{ ae_lq32f.i	aeq2, a13, 4; ae_subq56	aeq1, aeq1, aeq3 }
     e33:	fb72399d7f 	{ ae_sq32f.i	aeq1, a13, 28; ae_addq56	aeq3, aeq2, aeq0 }
     e38:	623d14        	ae_sq32f.i	aeq3, a13, 4
     e3b:	fa32fd0d2f 	{ ae_lq32f.i	aeq0, a13, 8; ae_subq56	aeq2, aeq2, aeq0 }
     e40:	60cd04        	ae_lq32f.i	aeq3, a13, 0
     e43:	622d34        	ae_sq32f.i	aeq2, a13, 12
     e46:	f93afd4d4f 	{ ae_lq32f.i	aeq2, a13, 16; ae_subq56	aeq1, aeq3, aeq0 }
     e4b:	621d24        	ae_sq32f.i	aeq1, a13, 8
     e4e:	fb7a3d0d6f 	{ ae_lq32f.i	aeq0, a13, 24; ae_addq56	aeq3, aeq3, aeq0 }
     e53:	f932f9bd0f 	{ ae_sq32f.i	aeq3, a13, 0; ae_subq56	aeq1, aeq2, aeq0 }
     e58:	621d64        	ae_sq32f.i	aeq1, a13, 24
     e5b:	fa723d2d0f 	{ ae_lq32f.i	aeq1, a13, 0; ae_addq56	aeq2, aeq2, aeq0 }
     e60:	622d44        	ae_sq32f.i	aeq2, a13, 16
     e63:	60cd44        	ae_lq32f.i	aeq3, a13, 16
     e66:	011bc4        	ae_addq56	aeq0, aeq1, aeq3
     e69:	fb3efd420f 	{ ae_lq32f.i	aeq2, a2, 0; ae_subq56	aeq3, aeq3, aeq1 }
     e6e:	633134        	ae_sq56s.i	aeq3, a1, 24
     e71:	f932fd621f 	{ ae_lq32f.i	aeq3, a2, 4; ae_subq56	aeq1, aeq2, aeq0 }
     e76:	fa72399d0f 	{ ae_sq32f.i	aeq1, a13, 0; ae_addq56	aeq2, aeq2, aeq0 }
     e7b:	632144        	ae_sq56s.i	aeq2, a1, 32
     e7e:	604d54        	ae_lq32f.i	aeq1, a13, 20
     e81:	608d14        	ae_lq32f.i	aeq2, a13, 4
     e84:	0129c4        	ae_addq56	aeq0, aeq2, aeq1
     e87:	01adc4        	ae_subq56	aeq2, aeq2, aeq1
     e8a:	017cc4        	ae_subq56	aeq1, aeq3, aeq0
     e8d:	fb7a3d814f 	{ ae_lq56.i	aeq0, a1, 32; ae_addq56	aeq3, aeq3, aeq0 }
     e92:	621d14        	ae_sq32f.i	aeq1, a13, 4
     e95:	623214        	ae_sq32f.i	aeq3, a2, 4
     e98:	604244        	ae_lq32f.i	aeq1, a2, 16
     e9b:	f8ea39820f 	{ ae_sq32f.i	aeq0, a2, 0; ae_addq56	aeq0, aeq1, aeq2 }
     ea0:	f9aafd625f 	{ ae_lq32f.i	aeq3, a2, 20; ae_subq56	aeq1, aeq1, aeq2 }
     ea5:	621d44        	ae_sq32f.i	aeq1, a13, 16
     ea8:	614134        	ae_lq56.i	aeq1, a1, 24
     eab:	01bdc4        	ae_subq56	aeq2, aeq3, aeq1
     eae:	fb7e39ad5f 	{ ae_sq32f.i	aeq2, a13, 20; ae_addq56	aeq3, aeq3, aeq1 }
     eb3:	623254        	ae_sq32f.i	aeq3, a2, 20
     eb6:	620244        	ae_sq32f.i	aeq0, a2, 16
     eb9:	600d64        	ae_lq32f.i	aeq0, a13, 24
     ebc:	604d74        	ae_lq32f.i	aeq1, a13, 28
     ebf:	0198c4        	ae_addq56	aeq2, aeq1, aeq0
     ec2:	632cb4        	ae_sq56s.i	aeq2, a12, -40
     ec5:	f826fd2d2f 	{ ae_lq32f.i	aeq1, a13, 8; ae_subq56	aeq0, aeq0, aeq1 }
     eca:	608d34        	ae_lq32f.i	aeq2, a13, 12
     ecd:	01edc4        	ae_subq56	aeq3, aeq2, aeq1
     ed0:	f9ea18afff 	{ ae_cvtp24a16x2.ll	aep2, a15, a15; ae_addq56	aeq1, aeq1, aeq2 }
     ed5:	c1a244        	ae_mulzaafq32sp16s.lh	aeq2, aeq1, aep2, aeq0, aep2
     ed8:	3c48ba312f 	{ ae_sq56s.i	aeq3, a1, 16; ae_mulzasfq32sp16s.lh	aeq0, aeq0, aep2, aeq1, aep2 }
     edd:	604224        	ae_lq32f.i	aeq1, a2, 8
     ee0:	fbea3a010f 	{ ae_sq56s.i	aeq0, a1, 0; ae_addq56	aeq3, aeq1, aeq2 }
     ee5:	f9aafd423f 	{ ae_lq32f.i	aeq2, a2, 12; ae_subq56	aeq1, aeq1, aeq2 }
     eea:	621d24        	ae_sq32f.i	aeq1, a13, 8
     eed:	610cb4        	ae_lq56.i	aeq0, a12, -40
     ef0:	614124        	ae_lq56.i	aeq1, a1, 16
     ef3:	0d497a311f 	{ ae_sq56s.i	aeq3, a1, 8; ae_mulzaafq32sp16s.lh	aeq3, aeq1, aep2, aeq0, aep2 }
     ef8:	c52a44        	ae_mulzasfq32sp16s.lh	aeq1, aeq1, aep2, aeq0, aep2
     efb:	012fc4        	ae_subq56	aeq0, aeq2, aeq3
     efe:	faf63de11f 	{ ae_lq56.i	aeq3, a1, 8; ae_addq56	aeq2, aeq2, aeq3 }
     f03:	620d34        	ae_sq32f.i	aeq0, a13, 12
     f06:	622234        	ae_sq32f.i	aeq2, a2, 12
     f09:	600264        	ae_lq32f.i	aeq0, a2, 24
     f0c:	fb6639b22f 	{ ae_sq32f.i	aeq3, a2, 8; ae_addq56	aeq3, aeq0, aeq1 }
     f11:	f826fd427f 	{ ae_lq32f.i	aeq2, a2, 28; ae_subq56	aeq0, aeq0, aeq1 }
     f16:	620d64        	ae_sq32f.i	aeq0, a13, 24
     f19:	610104        	ae_lq56.i	aeq0, a1, 0
     f1c:	016cc4        	ae_subq56	aeq1, aeq2, aeq0
     f1f:	621d74        	ae_sq32f.i	aeq1, a13, 28
     f22:	fa7239b26f 	{ ae_sq32f.i	aeq3, a2, 24; ae_addq56	aeq2, aeq2, aeq0 }
     f27:	622274        	ae_sq32f.i	aeq2, a2, 28
     f2a:	f01d           	retw.n

00000f2c <F_get_oft_32b>:
     f2c:	004136        	entry	a1, 32
     f2f:	144226        	beqi	a2, 4, f47 <F_get_oft_32b+0x1b>
     f32:	158226        	beqi	a2, 8, f4b <F_get_oft_32b+0x1f>
     f35:	16b226        	beqi	a2, 16, f4f <F_get_oft_32b+0x23>
     f38:	17c226        	beqi	a2, 32, f53 <F_get_oft_32b+0x27>
     f3b:	c0c242        	addi	a4, a2, -64
     f3e:	430c           	movi.n	a3, 4
     f40:	220c           	movi.n	a2, 2
     f42:	832340        	moveqz	a2, a3, a4
     f45:	f01d           	retw.n

00000f47 <F_get_oft_32b+0x1b>:
     f47:	024c           	movi.n	a2, 64
     f49:	f01d           	retw.n

00000f4b <F_get_oft_32b+0x1f>:
     f4b:	022c           	movi.n	a2, 32
     f4d:	f01d           	retw.n

00000f4f <F_get_oft_32b+0x23>:
     f4f:	021c           	movi.n	a2, 16
     f51:	f01d           	retw.n

00000f53 <F_get_oft_32b+0x27>:
     f53:	820c           	movi.n	a2, 8
     f55:	f01d           	retw.n

00000f57 <F_get_oft_32b+0x2b>:
	...

00000f58 <xtensa_fft_pass_32b>:
     f58:	00a136        	entry	a1, 80
     f5b:	83ad           	ae_sext16	a10, a3
     f5d:	b09320        	addx8	a9, a3, a2
     f60:	600204        	ae_lq32f.i	aeq0, a2, 0
     f63:	904330        	addx2	a4, a3, a3
     f66:	1153c0        	slli	a5, a3, 4
     f69:	606254        	ae_lq32f.x	aeq1, a2, a5
     f6c:	b04420        	addx8	a4, a4, a2
     f6f:	608404        	ae_lq32f.i	aeq2, a4, 0
     f72:	f9ea10825f 	{ add	a8, a2, a5; ae_addq56	aeq1, aeq1, aeq2 }
     f77:	fa663d681f 	{ ae_lq32f.i	aeq3, a8, 4; ae_addq56	aeq2, aeq0, aeq1 }
     f7c:	f826fa211f 	{ ae_sq56s.i	aeq2, a1, 8; ae_subq56	aeq0, aeq0, aeq1 }
     f81:	630144        	ae_sq56s.i	aeq0, a1, 32
     f84:	600414        	ae_lq32f.i	aeq0, a4, 4
     f87:	fb7a3d421f 	{ ae_lq32f.i	aeq2, a2, 4; ae_addq56	aeq3, aeq3, aeq0 }
     f8c:	f9f60fc86e 	{ addi	a6, a8, -4; ae_addq56	aeq1, aeq2, aeq3 }
     f91:	fab6fd681f 	{ ae_lq32f.i	aeq3, a8, 4; ae_subq56	aeq2, aeq2, aeq3 }
     f96:	631124        	ae_sq56s.i	aeq1, a1, 16
     f99:	fc4581        	l32r	a8, b0 (f2c <F_get_oft_32b>)
     f9c:	fb3afb925f 	{ ae_lq32f.x	aeq0, a2, a5; ae_subq56	aeq3, aeq3, aeq0 }
     fa1:	614144        	ae_lq56.i	aeq1, a1, 32
     fa4:	fcc252        	addi	a5, a2, -4
     fa7:	633134        	ae_sq56s.i	aeq3, a1, 24
     faa:	ff4c7d240f 	{ ae_lq32f.i	aeq1, a4, 0; ae_roundsq32sym	aeq3, aeq1 }
     faf:	f92af9f61f 	{ ae_sq32f.iu	aeq3, a6, 4; ae_subq56	aeq1, aeq1, aeq0 }
     fb4:	fc547de11f 	{ ae_lq56.i	aeq3, a1, 8; ae_roundsq32sym	aeq0, aeq2 }
     fb9:	624614        	ae_sq32f.iu	aeq0, a6, 4
     fbc:	fe5c7d812f 	{ ae_lq56.i	aeq0, a1, 16; ae_roundsq32sym	aeq2, aeq3 }
     fc1:	ff4479e51f 	{ ae_sq32f.iu	aeq2, a5, 4; ae_roundsq32sym	aeq3, aeq0 }
     fc6:	627514        	ae_sq32f.iu	aeq3, a5, 4
     fc9:	600914        	ae_lq32f.i	aeq0, a9, 4
     fcc:	0189c4        	ae_addq56	aeq2, aeq0, aeq1
     fcf:	632104        	ae_sq56s.i	aeq2, a1, 0
     fd2:	f826fda13f 	{ ae_lq56.i	aeq1, a1, 24; ae_subq56	aeq0, aeq0, aeq1 }
     fd7:	608904        	ae_lq32f.i	aeq2, a9, 0
     fda:	01edc4        	ae_subq56	aeq3, aeq2, aeq1
     fdd:	fa760fc92e 	{ addi	a2, a9, -4; ae_addq56	aeq2, aeq2, aeq1 }
     fe2:	fd5c4fc44e 	{ addi	a4, a4, -4; ae_roundsq32sym	aeq1, aeq3 }
     fe7:	ff447d810f 	{ ae_lq56.i	aeq0, a1, 0; ae_roundsq32sym	aeq3, aeq0 }
     fec:	625414        	ae_sq32f.iu	aeq1, a4, 4
     fef:	fd5479f41f 	{ ae_sq32f.iu	aeq3, a4, 4; ae_roundsq32sym	aeq1, aeq2 }
     ff4:	ff4479d21f 	{ ae_sq32f.iu	aeq1, a2, 4; ae_roundsq32sym	aeq3, aeq0 }
     ff9:	627214        	ae_sq32f.iu	aeq3, a2, 4
     ffc:	0008e0        	callx8	a8
     fff:	21c130        	srai	a12, a3, 1
    1002:	fc2c91        	l32r	a9, b4 (1fbc <_DYNAMIC+0x540>)
    1005:	00a082        	movi	a8, 0
    1008:	11baf0        	slli	a11, a10, 1
    100b:	809b90        	add	a9, a11, a9
    100e:	458984        	ae_lp16x2f.xu	aep0, a9, a8
    1011:	88ac76        	loopgtz	a12, 109d <xtensa_fft_pass_32b+0x145>
    1014:	60c424        	ae_lq32f.i	aeq3, a4, 8
    1017:	600414        	ae_lq32f.i	aeq0, a4, 4
    101a:	6002fd462f 	{ ae_lq32f.i	aeq2, a6, 8; ae_mulzasq32sp16u.lh	aeq1, aeq0, aep0, aeq3, aep0 }
    101f:	33017d061f 	{ ae_lq32f.i	aeq0, a6, 4; ae_mulzaaq32sp16u.lh	aeq3, aeq3, aep0, aeq0, aep0 }
    1024:	62007a110f 	{ ae_sq56s.i	aeq1, a1, 0; ae_mulzasq32sp16u.lh	aeq1, aeq2, aep0, aeq0, aep0 }
    1029:	c20024        	ae_mulzaaq32sp16u.lh	aeq0, aeq0, aep0, aeq2, aep0
    102c:	019bc4        	ae_addq56	aeq2, aeq1, aeq3
    102f:	015fc4        	ae_subq56	aeq1, aeq1, aeq3
    1032:	631134        	ae_sq56s.i	aeq1, a1, 24
    1035:	604524        	ae_lq32f.i	aeq1, a5, 8
    1038:	01dac4        	ae_addq56	aeq3, aeq1, aeq2
    103b:	633124        	ae_sq56s.i	aeq3, a1, 16
    103e:	f9aafde10f 	{ ae_lq56.i	aeq3, a1, 0; ae_subq56	aeq1, aeq1, aeq2 }
    1043:	018bc4        	ae_addq56	aeq2, aeq0, aeq3
    1046:	fb3afd051f 	{ ae_lq32f.i	aeq0, a5, 4; ae_subq56	aeq3, aeq3, aeq0 }
    104b:	fbe23a311f 	{ ae_sq56s.i	aeq3, a1, 8; ae_addq56	aeq3, aeq0, aeq2 }
    1050:	010ec4        	ae_subq56	aeq0, aeq0, aeq2
    1053:	fe445b89bf 	{ ae_lp16x2f.xu	aep0, a9, a11; ae_roundsq32sym	aeq2, aeq0 }
    1058:	fc4c7da12f 	{ ae_lq56.i	aeq1, a1, 16; ae_roundsq32sym	aeq0, aeq1 }
    105d:	626614        	ae_sq32f.iu	aeq2, a6, 4
    1060:	fe5c79c61f 	{ ae_sq32f.iu	aeq0, a6, 4; ae_roundsq32sym	aeq2, aeq3 }
    1065:	626514        	ae_sq32f.iu	aeq2, a5, 4
    1068:	fc4c7dc11f 	{ ae_lq56.i	aeq2, a1, 8; ae_roundsq32sym	aeq0, aeq1 }
    106d:	624514        	ae_sq32f.iu	aeq0, a5, 4
    1070:	604224        	ae_lq32f.i	aeq1, a2, 8
    1073:	fbaafd021f 	{ ae_lq32f.i	aeq0, a2, 4; ae_subq56	aeq3, aeq1, aeq2 }
    1078:	f9ea3dc13f 	{ ae_lq56.i	aeq2, a1, 24; ae_addq56	aeq1, aeq1, aeq2 }
    107d:	fba2fa314f 	{ ae_sq56s.i	aeq3, a1, 32; ae_subq56	aeq3, aeq0, aeq2 }
    1082:	010ac4        	ae_addq56	aeq0, aeq0, aeq2
    1085:	068834        	ae_roundsq32sym	aeq2, aeq0
    1088:	fc4c7da14f 	{ ae_lq56.i	aeq1, a1, 32; ae_roundsq32sym	aeq0, aeq1 }
    108d:	626214        	ae_sq32f.iu	aeq2, a2, 4
    1090:	fe5c79c21f 	{ ae_sq32f.iu	aeq0, a2, 4; ae_roundsq32sym	aeq2, aeq3 }
    1095:	fc4c79e41f 	{ ae_sq32f.iu	aeq2, a4, 4; ae_roundsq32sym	aeq0, aeq1 }
    109a:	624414        	ae_sq32f.iu	aeq0, a4, 4

0000109d <xtensa_fft_pass_32b+0x145>:
    109d:	60b0b0        	neg	a11, a11
    10a0:	dc0b           	addi.n	a13, a12, -1
    10a2:	11eae0        	slli	a14, a10, 2
    10a5:	60e0e0        	neg	a14, a14
    10a8:	4589e4        	ae_lp16x2f.xu	aep0, a9, a14
    10ab:	f03d           	nop.n
    10ad:	88ad76        	loopgtz	a13, 1139 <xtensa_fft_pass_32b+0x1e1>
    10b0:	608414        	ae_lq32f.i	aeq2, a4, 4
    10b3:	60c424        	ae_lq32f.i	aeq3, a4, 8
    10b6:	92033d261f 	{ ae_lq32f.i	aeq1, a6, 4; ae_mulzsaq32sp16u.lh	aeq0, aeq3, aep0, aeq2, aep0 }

000010bb <xtensa_fft_pass_32b+0x163>:
    10bb:	3302bd662f 	{ ae_lq32f.i	aeq3, a6, 8; ae_mulzaaq32sp16u.lh	aeq2, aeq2, aep0, aeq3, aep0 }
    10c0:	9003ba014f 	{ ae_sq56s.i	aeq0, a1, 32; ae_mulzsaq32sp16u.lh	aeq0, aeq1, aep0, aeq3, aep0 }
    10c5:	c288d4        	ae_mulzaaq32sp16u.lh	aeq3, aeq3, aep0, aeq1, aep0
    10c8:	014ac4        	ae_addq56	aeq1, aeq0, aeq2
    10cb:	010ec4        	ae_subq56	aeq0, aeq0, aeq2
    10ce:	630114        	ae_sq56s.i	aeq0, a1, 8
    10d1:	600524        	ae_lq32f.i	aeq0, a5, 8
    10d4:	0189c4        	ae_addq56	aeq2, aeq0, aeq1
    10d7:	632124        	ae_sq56s.i	aeq2, a1, 16
    10da:	f826fdc14f 	{ ae_lq56.i	aeq2, a1, 32; ae_subq56	aeq0, aeq0, aeq1 }
    10df:	017ac4        	ae_addq56	aeq1, aeq3, aeq2
    10e2:	fab6fd651f 	{ ae_lq32f.i	aeq3, a5, 4; ae_subq56	aeq2, aeq2, aeq3 }
    10e7:	fa7e3a213f 	{ ae_sq56s.i	aeq2, a1, 24; ae_addq56	aeq2, aeq3, aeq1 }
    10ec:	01fdc4        	ae_subq56	aeq3, aeq3, aeq1
    10ef:	fd5c5b89bf 	{ ae_lp16x2f.xu	aep0, a9, a11; ae_roundsq32sym	aeq1, aeq3 }
    10f4:	ff447d812f 	{ ae_lq56.i	aeq0, a1, 16; ae_roundsq32sym	aeq3, aeq0 }
    10f9:	625614        	ae_sq32f.iu	aeq1, a6, 4
    10fc:	fd5479f61f 	{ ae_sq32f.iu	aeq3, a6, 4; ae_roundsq32sym	aeq1, aeq2 }
    1101:	625514        	ae_sq32f.iu	aeq1, a5, 4
    1104:	ff447da13f 	{ ae_lq56.i	aeq1, a1, 24; ae_roundsq32sym	aeq3, aeq0 }
    1109:	627514        	ae_sq32f.iu	aeq3, a5, 4
    110c:	600224        	ae_lq32f.i	aeq0, a2, 8
    110f:	fa26fd621f 	{ ae_lq32f.i	aeq3, a2, 4; ae_subq56	aeq2, aeq0, aeq1 }
    1114:	f8663da11f 	{ ae_lq56.i	aeq1, a1, 8; ae_addq56	aeq0, aeq0, aeq1 }
    1119:	fa3efa210f 	{ ae_sq56s.i	aeq2, a1, 0; ae_subq56	aeq2, aeq3, aeq1 }
    111e:	01f9c4        	ae_addq56	aeq3, aeq3, aeq1
    1121:	067834        	ae_roundsq32sym	aeq1, aeq3
    1124:	ff447d810f 	{ ae_lq56.i	aeq0, a1, 0; ae_roundsq32sym	aeq3, aeq0 }
    1129:	625214        	ae_sq32f.iu	aeq1, a2, 4
    112c:	fd5479f21f 	{ ae_sq32f.iu	aeq3, a2, 4; ae_roundsq32sym	aeq1, aeq2 }
    1131:	ff4479d41f 	{ ae_sq32f.iu	aeq1, a4, 4; ae_roundsq32sym	aeq3, aeq0 }
    1136:	627414        	ae_sq32f.iu	aeq3, a4, 4

00001139 <xtensa_fft_pass_32b+0x1e1>:
    1139:	f01d           	retw.n

0000113b <xtensa_fft_pass_32b+0x1e3>:
	...

0000113c <xtensa_fft16_32b>:
    113c:	004136        	entry	a1, 32
    113f:	fbde81        	l32r	a8, b8 (da0 <xtensa_fft8_32b>)
    1142:	02ad           	mov.n	a10, a2
    1144:	0008e0        	callx8	a8
    1147:	fbdd81        	l32r	a8, bc (d28 <xtensa_fft4_32b>)
    114a:	40c2a2        	addi	a10, a2, 64
    114d:	0008e0        	callx8	a8
    1150:	fbdb81        	l32r	a8, bc (d28 <xtensa_fft4_32b>)
    1153:	60c2a2        	addi	a10, a2, 96
    1156:	0008e0        	callx8	a8
    1159:	fbd981        	l32r	a8, c0 (f58 <xtensa_fft_pass_32b>)
    115c:	02ad           	mov.n	a10, a2
    115e:	4b0c           	movi.n	a11, 4
    1160:	0008e0        	callx8	a8
    1163:	f01d           	retw.n

00001165 <xtensa_fft16_32b+0x29>:
    1165:	000000                                        ...

00001168 <xtensa_fft32_32b>:
    1168:	004136        	entry	a1, 32
    116b:	fbd681        	l32r	a8, c4 (113c <xtensa_fft16_32b>)
    116e:	02ad           	mov.n	a10, a2
    1170:	0008e0        	callx8	a8
    1173:	fbd181        	l32r	a8, b8 (da0 <xtensa_fft8_32b>)
    1176:	01d2a2        	addmi	a10, a2, 0x100
    1179:	80caa2        	addi	a10, a10, -128
    117c:	0008e0        	callx8	a8
    117f:	fbce81        	l32r	a8, b8 (da0 <xtensa_fft8_32b>)
    1182:	01d2a2        	addmi	a10, a2, 0x100
    1185:	c0caa2        	addi	a10, a10, -64
    1188:	0008e0        	callx8	a8
    118b:	fbcd81        	l32r	a8, c0 (f58 <xtensa_fft_pass_32b>)
    118e:	02ad           	mov.n	a10, a2
    1190:	8b0c           	movi.n	a11, 8
    1192:	0008e0        	callx8	a8
    1195:	f01d           	retw.n

00001197 <xtensa_fft32_32b+0x2f>:
	...

00001198 <xtensa_fft64_32b>:
    1198:	004136        	entry	a1, 32
    119b:	fbcb81        	l32r	a8, c8 (1168 <xtensa_fft32_32b>)
    119e:	02ad           	mov.n	a10, a2
    11a0:	0008e0        	callx8	a8
    11a3:	fbc881        	l32r	a8, c4 (113c <xtensa_fft16_32b>)
    11a6:	01d2a2        	addmi	a10, a2, 0x100
    11a9:	0008e0        	callx8	a8
    11ac:	fbc681        	l32r	a8, c4 (113c <xtensa_fft16_32b>)
    11af:	02d2a2        	addmi	a10, a2, 0x200
    11b2:	80caa2        	addi	a10, a10, -128
    11b5:	0008e0        	callx8	a8
    11b8:	fbc281        	l32r	a8, c0 (f58 <xtensa_fft_pass_32b>)
    11bb:	02ad           	mov.n	a10, a2
    11bd:	0b1c           	movi.n	a11, 16
    11bf:	0008e0        	callx8	a8
    11c2:	f01d           	retw.n

000011c4 <xtensa_fft128_32b>:
    11c4:	004136        	entry	a1, 32
    11c7:	fbc181        	l32r	a8, cc (1198 <xtensa_fft64_32b>)
    11ca:	02ad           	mov.n	a10, a2
    11cc:	0008e0        	callx8	a8
    11cf:	fbbe81        	l32r	a8, c8 (1168 <xtensa_fft32_32b>)
    11d2:	02d2a2        	addmi	a10, a2, 0x200
    11d5:	0008e0        	callx8	a8
    11d8:	fbbc81        	l32r	a8, c8 (1168 <xtensa_fft32_32b>)
    11db:	03d2a2        	addmi	a10, a2, 0x300
    11de:	0008e0        	callx8	a8
    11e1:	fbb781        	l32r	a8, c0 (f58 <xtensa_fft_pass_32b>)
    11e4:	02ad           	mov.n	a10, a2
    11e6:	0b2c           	movi.n	a11, 32
    11e8:	0008e0        	callx8	a8
    11eb:	f01d           	retw.n

000011ed <xtensa_fft128_32b+0x29>:
    11ed:	000000                                        ...

000011f0 <xtensa_fft256_32b>:
    11f0:	004136        	entry	a1, 32
    11f3:	fbb681        	l32r	a8, cc (1198 <xtensa_fft64_32b>)
    11f6:	02ad           	mov.n	a10, a2
    11f8:	0008e0        	callx8	a8
    11fb:	fbb381        	l32r	a8, c8 (1168 <xtensa_fft32_32b>)
    11fe:	02d2a2        	addmi	a10, a2, 0x200
    1201:	0008e0        	callx8	a8
    1204:	fbb181        	l32r	a8, c8 (1168 <xtensa_fft32_32b>)
    1207:	03d2a2        	addmi	a10, a2, 0x300
    120a:	0008e0        	callx8	a8
    120d:	fbac81        	l32r	a8, c0 (f58 <xtensa_fft_pass_32b>)
    1210:	02ad           	mov.n	a10, a2
    1212:	0b2c           	movi.n	a11, 32
    1214:	0008e0        	callx8	a8
    1217:	fbad81        	l32r	a8, cc (1198 <xtensa_fft64_32b>)
    121a:	04d2a2        	addmi	a10, a2, 0x400
    121d:	0008e0        	callx8	a8
    1220:	fbab81        	l32r	a8, cc (1198 <xtensa_fft64_32b>)
    1223:	06d2a2        	addmi	a10, a2, 0x600
    1226:	0008e0        	callx8	a8
    1229:	fba581        	l32r	a8, c0 (f58 <xtensa_fft_pass_32b>)
    122c:	02ad           	mov.n	a10, a2
    122e:	0b4c           	movi.n	a11, 64
    1230:	0008e0        	callx8	a8
    1233:	f01d           	retw.n

00001235 <xtensa_fft256_32b+0x45>:
    1235:	000000                                        ...

00001238 <xtensa_fft512_32b>:
    1238:	004136        	entry	a1, 32
    123b:	fba581        	l32r	a8, d0 (11f0 <xtensa_fft256_32b>)
    123e:	02ad           	mov.n	a10, a2
    1240:	0008e0        	callx8	a8
    1243:	fba481        	l32r	a8, d4 (11c4 <xtensa_fft128_32b>)
    1246:	08d2a2        	addmi	a10, a2, 0x800
    1249:	0008e0        	callx8	a8
    124c:	fba281        	l32r	a8, d4 (11c4 <xtensa_fft128_32b>)
    124f:	0cd2a2        	addmi	a10, a2, 0xc00
    1252:	0008e0        	callx8	a8
    1255:	fb9a81        	l32r	a8, c0 (f58 <xtensa_fft_pass_32b>)
    1258:	02ad           	mov.n	a10, a2
    125a:	80a0b2        	movi	a11, 128
    125d:	0008e0        	callx8	a8
    1260:	f01d           	retw.n

00001262 <xtensa_fft512_32b+0x2a>:
	...

00001264 <real_split>:
    1264:	00a136        	entry	a1, 80
    1267:	fb9281        	l32r	a8, b0 (f2c <F_get_oft_32b>)
    126a:	03c8           	l32i.n	a12, a3, 0
    126c:	13d8           	l32i.n	a13, a3, 4
    126e:	0e0c           	movi.n	a14, 0
    1270:	219120        	srai	a9, a2, 1
    1273:	5199           	s32i.n	a9, a1, 20
    1275:	b07940        	addx8	a7, a9, a4
    1278:	c05290        	sub	a5, a2, a9
    127b:	14e9           	s32i.n	a14, a4, 4
    127d:	b06530        	addx8	a6, a5, a3
    1280:	16b8           	l32i.n	a11, a6, 4
    1282:	ccda           	add.n	a12, a12, a13
    1284:	1199d0        	slli	a9, a9, 3
    1287:	4199           	s32i.n	a9, a1, 16
    1289:	04c9           	s32i.n	a12, a4, 0
    128b:	06a8           	l32i.n	a10, a6, 0
    128d:	07a9           	s32i.n	a10, a7, 0
    128f:	bbba           	add.n	a11, a11, a11
    1291:	82ad           	ae_sext16	a10, a2
    1293:	60b0b0        	neg	a11, a11
    1296:	21b1b0        	srai	a11, a11, 1
    1299:	17b9           	s32i.n	a11, a7, 4
    129b:	0008e0        	callx8	a8
    129e:	0acd           	mov.n	a12, a10
    12a0:	2179           	s32i.n	a7, a1, 8
    12a2:	1169           	s32i.n	a6, a1, 4
    12a4:	0159           	s32i.n	a5, a1, 0
    12a6:	3139           	s32i.n	a3, a1, 12
    12a8:	d48b           	addi.n	a13, a4, 8
    12aa:	b38b           	addi.n	a11, a3, 8
    12ac:	80a0f2        	movi	a15, 128
    12af:	b0e230        	addx8	a14, a2, a3
    12b2:	fccee2        	addi	a14, a14, -4
    12b5:	022f27        	blt	a15, a2, 12bb <real_split+0x57>
    12b8:	11caf0        	slli	a12, a10, 1

000012bb <real_split+0x57>:
    12bb:	fb8761        	l32r	a6, d8 (1fc0 <ROOTS_512_Q16>)
    12be:	fecca2        	addi	a10, a12, -2
    12c1:	2181a0        	srai	a8, a10, 1
    12c4:	a08860        	addx4	a8, a8, a6
    12c7:	440804        	ae_lp16x2f.i	aep0, a8, 0
    12ca:	07eca6        	blti	a12, 128, 12d5 <real_split+0x71>
    12cd:	1f0c           	movi.n	a15, 1
    12cf:	60c0c0        	neg	a12, a12
    12d2:	000046        	j	12d7 <real_split+0x73>

000012d5 <real_split+0x71>:
    12d5:	0f0c           	movi.n	a15, 0

000012d7 <real_split+0x73>:
    12d7:	f8cdd2        	addi	a13, a13, -8
    12da:	5b4b           	addi.n	a5, a11, 4
    12dc:	604b04        	ae_lq32f.i	aeq1, a11, 0
    12df:	600b14        	ae_lq32f.i	aeq0, a11, 4
    12e2:	130c           	movi.n	a3, 1
    12e4:	7da072        	movi	a7, 125
    12e7:	5198           	l32i.n	a9, a1, 20
    12e9:	fcce42        	addi	a4, a14, -4
    12ec:	fb6c81        	l32r	a8, 9c (8000 <_end+0x53c0>)
    12ef:	ee8b           	addi.n	a14, a14, 8
    12f1:	448b           	addi.n	a4, a4, 8
    12f3:	301884        	ae_cvtp24a16x2.ll	aep1, a8, a8
    12f6:	990b           	addi.n	a9, a9, -1
    12f8:	1fcc           	bnez.n	a15, 12fd <real_split+0x99>
    12fa:	1a8084        	ae_selp24.lh	aep0, aep0, aep0
    12fd:	5da976        	loopgtz	a9, 135e <real_split+0xfa>
    1300:	630144        	ae_sq56s.i	aeq0, a1, 32
    1303:	91013d74ef 	{ ae_lq32f.iu	aeq3, a4, -8; ae_mulzsaq32sp16u.lh	aeq2, aeq1, aep0, aeq0, aep0 }
    1308:	182790aacf 	{ add	a10, a10, a12; ae_mulzaafq32sp16u.lh	aeq0, aeq1, aep1, aeq3, aep1 }
    130d:	2181a0        	srai	a8, a10, 1
    1310:	f8e23d5eef 	{ ae_lq32f.iu	aeq2, a14, -8; ae_addq56	aeq0, aeq0, aeq2 }
    1315:	32037a113f 	{ ae_sq56s.i	aeq1, a1, 24; ae_mulzaaq32sp16u.lh	aeq1, aeq3, aep0, aeq2, aep0 }
    131a:	a08860        	addx4	a8, a8, a6
    131d:	0109c4        	ae_addq56	aeq0, aeq0, aeq1
    1320:	630154        	ae_sq56s.i	aeq0, a1, 40
    1323:	9302fd814f 	{ ae_lq56.i	aeq0, a1, 32; ae_mulzsaq32sp16u.lh	aeq3, aeq2, aep0, aeq3, aep0 }
    1328:	49263da13f 	{ ae_lq56.i	aeq1, a1, 24; ae_mulzasfq32sp16u.lh	aeq2, aeq0, aep1, aeq2, aep1 }
    132d:	ce0014        	ae_mulzssq32sp16u.lh	aeq0, aeq0, aep0, aeq1, aep0
    1330:	440804        	ae_lp16x2f.i	aep0, a8, 0
    1333:	fa723d815f 	{ ae_lq56.i	aeq0, a1, 40; ae_addq56	aeq2, aeq2, aeq0 }
    1338:	faf63b001f 	{ ae_sraiq56	aeq0, aeq0, 1; ae_addq56	aeq2, aeq2, aeq3 }
    133d:	ff447b209f 	{ ae_sraiq56	aeq2, aeq2, 1; ae_roundsq32sym	aeq3, aeq0 }
    1342:	fd5479fd2f 	{ ae_sq32f.iu	aeq3, a13, 8; ae_roundsq32sym	aeq1, aeq2 }
    1347:	621d14        	ae_sq32f.i	aeq1, a13, 4
    134a:	05a7a7        	bge	a7, a10, 1353 <real_split+0xef>
    134d:	60c0c0        	neg	a12, a12
    1350:	30ff30        	xor	a15, a15, a3

00001353 <real_split+0xef>:
    1353:	605b24        	ae_lq32f.iu	aeq1, a11, 8
    1356:	1fcc           	bnez.n	a15, 135b <real_split+0xf7>
    1358:	1a8084        	ae_selp24.lh	aep0, aep0, aep0

0000135b <real_split+0xf7>:
    135b:	601524        	ae_lq32f.iu	aeq0, a5, 8

0000135e <real_split+0xfa>:
    135e:	31d8           	l32i.n	a13, a1, 12
    1360:	41b8           	l32i.n	a11, a1, 16
    1362:	11e8           	l32i.n	a14, a1, 4
    1364:	5198           	l32i.n	a9, a1, 20
    1366:	60c0c0        	neg	a12, a12
    1369:	fecca2        	addi	a10, a12, -2
    136c:	2181a0        	srai	a8, a10, 1
    136f:	991b           	addi.n	a9, a9, 1
    1371:	fccee2        	addi	a14, a14, -4
    1374:	a08860        	addx4	a8, a8, a6
    1377:	bbda           	add.n	a11, a11, a13
    1379:	440804        	ae_lp16x2f.i	aep0, a8, 0
    137c:	21d8           	l32i.n	a13, a1, 8
    137e:	bb8b           	addi.n	a11, a11, 8
    1380:	604b04        	ae_lq32f.i	aeq1, a11, 0
    1383:	5b4b           	addi.n	a5, a11, 4
    1385:	600b14        	ae_lq32f.i	aeq0, a11, 4
    1388:	dd8b           	addi.n	a13, a13, 8
    138a:	bb8b           	addi.n	a11, a11, 8
    138c:	05eca6        	blti	a12, 128, 1395 <real_split+0x131>
    138f:	60c0c0        	neg	a12, a12
    1392:	30ff30        	xor	a15, a15, a3

00001395 <real_split+0x131>:
    1395:	1fcc           	bnez.n	a15, 139a <real_split+0x136>
    1397:	1a8084        	ae_selp24.lh	aep0, aep0, aep0

0000139a <real_split+0x136>:
    139a:	78a927        	bge	a9, a2, 1416 <real_split+0x1b2>
    139d:	f8cbb2        	addi	a11, a11, -8
    13a0:	f8cdd2        	addi	a13, a13, -8
    13a3:	0188           	l32i.n	a8, a1, 0
    13a5:	fcce42        	addi	a4, a14, -4
    13a8:	fb3d91        	l32r	a9, 9c (8000 <_end+0x53c0>)
    13ab:	ee8b           	addi.n	a14, a14, 8
    13ad:	448b           	addi.n	a4, a4, 8
    13af:	301994        	ae_cvtp24a16x2.ll	aep1, a9, a9
    13b2:	ffc882        	addi	a8, a8, -1
    13b5:	5da876        	loopgtz	a8, 1416 <real_split+0x1b2>
    13b8:	630144        	ae_sq56s.i	aeq0, a1, 32
    13bb:	c1013d74ef 	{ ae_lq32f.iu	aeq3, a4, -8; ae_mulzssq32sp16u.lh	aeq2, aeq1, aep0, aeq0, aep0 }
    13c0:	182790aacf 	{ add	a10, a10, a12; ae_mulzaafq32sp16u.lh	aeq0, aeq1, aep1, aeq3, aep1 }
    13c5:	2181a0        	srai	a8, a10, 1
    13c8:	f8e23d5eef 	{ ae_lq32f.iu	aeq2, a14, -8; ae_addq56	aeq0, aeq0, aeq2 }
    13cd:	62037a113f 	{ ae_sq56s.i	aeq1, a1, 24; ae_mulzasq32sp16u.lh	aeq1, aeq3, aep0, aeq2, aep0 }
    13d2:	a08860        	addx4	a8, a8, a6
    13d5:	0109c4        	ae_addq56	aeq0, aeq0, aeq1
    13d8:	630154        	ae_sq56s.i	aeq0, a1, 40
    13db:	c302fd814f 	{ ae_lq56.i	aeq0, a1, 32; ae_mulzssq32sp16u.lh	aeq3, aeq2, aep0, aeq3, aep0 }
    13e0:	49263da13f 	{ ae_lq56.i	aeq1, a1, 24; ae_mulzasfq32sp16u.lh	aeq2, aeq0, aep1, aeq2, aep1 }
    13e5:	ca0014        	ae_mulzsaq32sp16u.lh	aeq0, aeq0, aep0, aeq1, aep0
    13e8:	440804        	ae_lp16x2f.i	aep0, a8, 0
    13eb:	fa723d815f 	{ ae_lq56.i	aeq0, a1, 40; ae_addq56	aeq2, aeq2, aeq0 }
    13f0:	faf63b001f 	{ ae_sraiq56	aeq0, aeq0, 1; ae_addq56	aeq2, aeq2, aeq3 }
    13f5:	ff447b209f 	{ ae_sraiq56	aeq2, aeq2, 1; ae_roundsq32sym	aeq3, aeq0 }
    13fa:	fd5479fd2f 	{ ae_sq32f.iu	aeq3, a13, 8; ae_roundsq32sym	aeq1, aeq2 }
    13ff:	621d14        	ae_sq32f.i	aeq1, a13, 4
    1402:	05a7a7        	bge	a7, a10, 140b <real_split+0x1a7>
    1405:	60c0c0        	neg	a12, a12
    1408:	30ff30        	xor	a15, a15, a3

0000140b <real_split+0x1a7>:
    140b:	605b24        	ae_lq32f.iu	aeq1, a11, 8
    140e:	1fcc           	bnez.n	a15, 1413 <real_split+0x1af>
    1410:	1a8084        	ae_selp24.lh	aep0, aep0, aep0

00001413 <real_split+0x1af>:
    1413:	601524        	ae_lq32f.iu	aeq0, a5, 8

00001416 <real_split+0x1b2>:
    1416:	f01d           	retw.n

00001418 <xtensa_real_fft_32b>:
    1418:	004136        	entry	a1, 32
    141b:	078266        	bnei	a2, 8, 1426 <xtensa_real_fft_32b+0xe>
    141e:	fb2681        	l32r	a8, b8 (da0 <xtensa_fft8_32b>)
    1421:	03ad           	mov.n	a10, a3
    1423:	0008e0        	callx8	a8

00001426 <xtensa_real_fft_32b+0xe>:
    1426:	07d266        	bnei	a2, 64, 1431 <xtensa_real_fft_32b+0x19>
    1429:	fb2881        	l32r	a8, cc (1198 <xtensa_fft64_32b>)
    142c:	03ad           	mov.n	a10, a3
    142e:	0008e0        	callx8	a8

00001431 <xtensa_real_fft_32b+0x19>:
    1431:	07e266        	bnei	a2, 128, 143c <xtensa_real_fft_32b+0x24>
    1434:	fb2881        	l32r	a8, d4 (11c4 <xtensa_fft128_32b>)
    1437:	03ad           	mov.n	a10, a3
    1439:	0008e0        	callx8	a8

0000143c <xtensa_real_fft_32b+0x24>:
    143c:	07f266        	bnei	a2, 0x100, 1447 <xtensa_real_fft_32b+0x2f>
    143f:	fb2481        	l32r	a8, d0 (11f0 <xtensa_fft256_32b>)
    1442:	03ad           	mov.n	a10, a3
    1444:	0008e0        	callx8	a8

00001447 <xtensa_real_fft_32b+0x2f>:
    1447:	02ad           	mov.n	a10, a2
    1449:	03bd           	mov.n	a11, a3
    144b:	1142d0        	slli	a4, a2, 3
    144e:	fb2381        	l32r	a8, dc (1264 <real_split>)
    1451:	c04340        	sub	a4, a3, a4
    1454:	04cd           	mov.n	a12, a4
    1456:	0008e0        	callx8	a8
    1459:	a48b           	addi.n	a10, a4, 8
    145b:	820b           	addi.n	a8, a2, -1
    145d:	03b8           	l32i.n	a11, a3, 0
    145f:	13c8           	l32i.n	a12, a3, 4
    1461:	0d0c           	movi.n	a13, 0
    1463:	1192c0        	slli	a9, a2, 4
    1466:	949a           	add.n	a9, a4, a9
    1468:	13d9           	s32i.n	a13, a3, 4
    146a:	fcc992        	addi	a9, a9, -4
    146d:	c0bbc0        	sub	a11, a11, a12
    1470:	03b9           	s32i.n	a11, a3, 0
    1472:	13a876        	loopgtz	a8, 1489 <xtensa_real_fft_32b+0x71>
    1475:	1ad8           	l32i.n	a13, a10, 4
    1477:	0ab8           	l32i.n	a11, a10, 0
    1479:	ffd9c2        	addmi	a12, a9, 0xffffff00
    147c:	aa8b           	addi.n	a10, a10, 8
    147e:	60d0d0        	neg	a13, a13
    1481:	09d9           	s32i.n	a13, a9, 0
    1483:	3f6cb2        	s32i	a11, a12, 252
    1486:	f8c992        	addi	a9, a9, -8

00001489 <xtensa_real_fft_32b+0x71>:
    1489:	f01d           	retw.n

0000148b <xtensa_real_fft_32b+0x73>:
	...

0000148c <xtensa_comp_fft_32b>:
    148c:	004136        	entry	a1, 32
    148f:	07b266        	bnei	a2, 16, 149a <xtensa_comp_fft_32b+0xe>
    1492:	fb0c81        	l32r	a8, c4 (113c <xtensa_fft16_32b>)
    1495:	03ad           	mov.n	a10, a3
    1497:	0008e0        	callx8	a8

0000149a <xtensa_comp_fft_32b+0xe>:
    149a:	07e266        	bnei	a2, 128, 14a5 <xtensa_comp_fft_32b+0x19>
    149d:	fb0d81        	l32r	a8, d4 (11c4 <xtensa_fft128_32b>)
    14a0:	03ad           	mov.n	a10, a3
    14a2:	0008e0        	callx8	a8

000014a5 <xtensa_comp_fft_32b+0x19>:
    14a5:	07f266        	bnei	a2, 0x100, 14b0 <xtensa_comp_fft_32b+0x24>
    14a8:	fb0a81        	l32r	a8, d0 (11f0 <xtensa_fft256_32b>)
    14ab:	03ad           	mov.n	a10, a3
    14ad:	0008e0        	callx8	a8

000014b0 <xtensa_comp_fft_32b+0x24>:
    14b0:	00a292        	movi	a9, 0x200
    14b3:	079297        	bne	a2, a9, 14be <xtensa_comp_fft_32b+0x32>
    14b6:	fb0a81        	l32r	a8, e0 (1238 <xtensa_fft512_32b>)
    14b9:	03ad           	mov.n	a10, a3
    14bb:	0008e0        	callx8	a8

000014be <xtensa_comp_fft_32b+0x32>:
    14be:	f01d           	retw.n

000014c0 <sw_fft_32b_2811>:
    14c0:	006136        	entry	a1, 48
    14c3:	121666        	bnei	a6, 1, 14d9 <sw_fft_32b_2811+0x19>
    14c6:	0fb326        	beqi	a3, 16, 14d9 <sw_fft_32b_2811+0x19>
    14c9:	0ce326        	beqi	a3, 128, 14d9 <sw_fft_32b_2811+0x19>
    14cc:	09f326        	beqi	a3, 0x100, 14d9 <sw_fft_32b_2811+0x19>
    14cf:	00a282        	movi	a8, 0x200
    14d2:	031387        	beq	a3, a8, 14d9 <sw_fft_32b_2811+0x19>

000014d5 <sw_fft_32b_2811+0x15>:
    14d5:	126c           	movi.n	a2, -31
    14d7:	f01d           	retw.n

000014d9 <sw_fft_32b_2811+0x19>:
    14d9:	a6cc           	bnez.n	a6, 14e7 <sw_fft_32b_2811+0x27>
    14db:	088326        	beqi	a3, 8, 14e7 <sw_fft_32b_2811+0x27>
    14de:	05d326        	beqi	a3, 64, 14e7 <sw_fft_32b_2811+0x27>
    14e1:	02e326        	beqi	a3, 128, 14e7 <sw_fft_32b_2811+0x27>
    14e4:	edf366        	bnei	a3, 0x100, 14d5 <sw_fft_32b_2811+0x15>
    14e7:	1173f0        	slli	a7, a3, 1
    14ea:	029d           	mov.n	a9, a2
    14ec:	600204        	ae_lq32f.i	aeq0, a2, 0
    14ef:	0b13a6        	blti	a3, 1, 14fe <sw_fft_32b_2811+0x3e>
    14f2:	08a776        	loopgtz	a7, 14fe <sw_fft_32b_2811+0x3e>
    14f5:	3404b4        	ae_sraaq56	aeq0, aeq0, a4
    14f8:	620904        	ae_sq32f.i	aeq0, a9, 0
    14fb:	601914        	ae_lq32f.iu	aeq0, a9, 4

000014fe <sw_fft_32b_2811+0x3e>:
    14fe:	101666        	bnei	a6, 1, 1512 <sw_fft_32b_2811+0x52>
    1501:	faf881        	l32r	a8, e4 (148c <xtensa_comp_fft_32b>)
    1504:	03ad           	mov.n	a10, a3
    1506:	02bd           	mov.n	a11, a2
    1508:	0008e0        	callx8	a8
    150b:	07bd           	mov.n	a11, a7
    150d:	090c           	movi.n	a9, 0
    150f:	000386        	j	1521 <sw_fft_32b_2811+0x61>

00001512 <sw_fft_32b_2811+0x52>:
    1512:	faf581        	l32r	a8, e8 (1418 <xtensa_real_fft_32b>)
    1515:	03ad           	mov.n	a10, a3
    1517:	02bd           	mov.n	a11, a2
    1519:	0008e0        	callx8	a8
    151c:	079d           	mov.n	a9, a7
    151e:	11b3e0        	slli	a11, a3, 2

00001521 <sw_fft_32b_2811+0x61>:
    1521:	3a1566        	bnei	a5, 1, 155f <sw_fft_32b_2811+0x9f>
    1524:	11a9e0        	slli	a10, a9, 2
    1527:	c0a2a0        	sub	a10, a2, a10
    152a:	0a9d           	mov.n	a9, a10
    152c:	fd24bd0a0f 	{ ae_lq32f.i	aeq0, a10, 0; ae_zeroq56	aeq1 }
    1531:	07ab76        	loopgtz	a11, 153c <sw_fft_32b_2811+0x7c>
    1534:	069864        	ae_absq56	aeq2, aeq0
    1537:	f936bd191f 	{ ae_lq32f.iu	aeq0, a9, 4; ae_orq56	aeq1, aeq1, aeq2 }

0000153c <sw_fft_32b_2811+0x7c>:
    153c:	0a9d           	mov.n	a9, a10
    153e:	621104        	ae_sq32f.i	aeq1, a1, 0
    1541:	01d8           	l32i.n	a13, a1, 0
    1543:	600a04        	ae_lq32f.i	aeq0, a10, 0
    1546:	0a0c           	movi.n	a10, 0
    1548:	40edc0        	nsa	a12, a13
    154b:	b3acd0        	movgez	a10, a12, a13
    154e:	0aab76        	loopgtz	a11, 155c <sw_fft_32b_2811+0x9c>
    1551:	fc24fe8acf 	{ ae_slaasq56s	aeq3, aeq0, a10; nop }
    1556:	623904        	ae_sq32f.i	aeq3, a9, 0
    1559:	601914        	ae_lq32f.iu	aeq0, a9, 4
    155c:	c044a0        	sub	a4, a4, a10

0000155f <sw_fft_32b_2811+0x9f>:
    155f:	842d           	ae_sext16	a2, a4
    1561:	f01d           	retw.n

00001563 <sw_fft_32b_2811+0xa3>:
    1563:	00000000 00413600                                .....

00001568 <fill_vad_thrd_Nbands>:
    1568:	004136        	entry	a1, 32
    156b:	00a062        	movi	a6, 0
    156e:	fadf71        	l32r	a7, ec (7f80 <_end+0x5340>)
    1571:	1ba476        	loopgtz	a4, 1590 <fill_vad_thrd_Nbands+0x28>
    1574:	a05620        	addx4	a5, a6, a2
    1577:	904630        	addx2	a4, a6, a3
    157a:	444d           	ae_l16ui.n	a4, a4, 0
    157c:	661b           	addi.n	a6, a6, 1
    157e:	866d           	ae_sext16	a6, a6
    1580:	218140        	srai	a8, a4, 1
    1583:	744040        	extui	a4, a4, 0, 8
    1586:	108870        	and	a8, a8, a7
    1589:	658d           	ae_s16i.n	a8, a5, 0
    158b:	114490        	slli	a4, a4, 7
    158e:	754d           	ae_s16i.n	a4, a5, 2

00001590 <fill_vad_thrd_Nbands+0x28>:
    1590:	f01d           	retw.n

00001592 <fill_vad_thrd_Nbands+0x2a>:
	...

00001594 <complement_para>:
    1594:	004136        	entry	a1, 32
    1597:	fad641        	l32r	a4, f0 (ffff8000 <_end+0xffff53c0>)
    159a:	623d           	ae_s16i.n	a3, a2, 0
    159c:	c04430        	sub	a4, a4, a3
    159f:	724d           	ae_s16i.n	a4, a2, 2
    15a1:	f01d           	retw.n

000015a3 <complement_para+0xf>:
	...

000015a4 <common_noise_gen_init>:
    15a4:	004136        	entry	a1, 32
    15a7:	fad381        	l32r	a8, f4 (1594 <complement_para>)
    15aa:	2213a2        	l16ui	a10, a3, 68
    15ad:	2d13b2        	l16ui	a11, a3, 90
    15b0:	241392        	l16ui	a9, a3, 72
    15b3:	1c0c           	movi.n	a12, 1
    15b5:	8c52c2        	s16i	a12, a2, 0x118
    15b8:	035292        	s16i	a9, a2, 6
    15bb:	1052b2        	s16i	a11, a2, 32
    15be:	0252a2        	s16i	a10, a2, 4
    15c1:	2c93b2        	l16si	a11, a3, 88
    15c4:	01d2a2        	addmi	a10, a2, 0x100
    15c7:	d8caa2        	addi	a10, a10, -40
    15ca:	0008e0        	callx8	a8
    15cd:	70c3b2        	addi	a11, a3, 112
    15d0:	faca81        	l32r	a8, f8 (1568 <fill_vad_thrd_Nbands>)
    15d3:	a28b           	addi.n	a10, a2, 8
    15d5:	4c0c           	movi.n	a12, 4
    15d7:	0008e0        	callx8	a8
    15da:	78a092        	movi	a9, 120
    15dd:	7a0c           	movi.n	a10, 7
    15df:	fac7b1        	l32r	a11, fc (24c0 <win_ns>)
    15e2:	3d62b2        	s32i	a11, a2, 244
    15e5:	2e42a2        	s8i	a10, a2, 46
    15e8:	2f4292        	s8i	a9, a2, 47
    15eb:	f01d           	retw.n

000015ed <common_noise_gen_init+0x49>:
    15ed:	000000                                        ...

000015f0 <common_NR_init>:
    15f0:	004136        	entry	a1, 32
    15f3:	2e0282        	l8ui	a8, a2, 46
    15f6:	2b18a6        	blti	a8, 1, 1625 <common_NR_init+0x35>
    15f9:	0d0c           	movi.n	a13, 0
    15fb:	0b0c           	movi.n	a11, 0
    15fd:	fac0a1        	l32r	a10, 100 (ffffb800 <_end+0xffff8bc0>)
    1600:	fac1e1        	l32r	a14, 104 (b8000000 <_end+0xb7ffd3c0>)
    1603:	01d2c2        	addmi	a12, a2, 0x100
    1606:	a0ccc2        	addi	a12, a12, -96

00001609 <common_NR_init+0x19>:
    1609:	c298           	l32i.n	a9, a2, 48
    160b:	cf0c           	movi.n	a15, 12
    160d:	0ce9           	s32i.n	a14, a12, 0
    160f:	8099d0        	add	a9, a9, a13
    1612:	18cdd2        	addi	a13, a13, 24
    1615:	02af76        	loopgtz	a15, 161b <common_NR_init+0x2b>
    1618:	09c9a4        	ae_s16ip	a10, a9, 2

0000161b <common_NR_init+0x2b>:
    161b:	2e02f2        	l8ui	a15, a2, 46
    161e:	cc4b           	addi.n	a12, a12, 4
    1620:	bb1b           	addi.n	a11, a11, 1
    1622:	e32bf7        	blt	a11, a15, 1609 <common_NR_init+0x19>

00001625 <common_NR_init+0x35>:
    1625:	2e93b2        	l16si	a11, a3, 92
    1628:	fab381        	l32r	a8, f4 (1594 <complement_para>)
    162b:	01d2a2        	addmi	a10, a2, 0x100
    162e:	c4caa2        	addi	a10, a10, -60
    1631:	0008e0        	callx8	a8
    1634:	2f93b2        	l16si	a11, a3, 94
    1637:	faaf81        	l32r	a8, f4 (1594 <complement_para>)
    163a:	01d2a2        	addmi	a10, a2, 0x100
    163d:	c8caa2        	addi	a10, a10, -56
    1640:	0008e0        	callx8	a8
    1643:	3093b2        	l16si	a11, a3, 96
    1646:	faab81        	l32r	a8, f4 (1594 <complement_para>)
    1649:	01d2a2        	addmi	a10, a2, 0x100
    164c:	cccaa2        	addi	a10, a10, -52
    164f:	0008e0        	callx8	a8
    1652:	3193b2        	l16si	a11, a3, 98
    1655:	faa781        	l32r	a8, f4 (1594 <complement_para>)
    1658:	01d2a2        	addmi	a10, a2, 0x100
    165b:	d0caa2        	addi	a10, a10, -48
    165e:	0008e0        	callx8	a8
    1661:	3293b2        	l16si	a11, a3, 100
    1664:	faa481        	l32r	a8, f4 (1594 <complement_para>)
    1667:	01d2a2        	addmi	a10, a2, 0x100
    166a:	d4caa2        	addi	a10, a10, -44
    166d:	0008e0        	callx8	a8
    1670:	7893f2        	l16si	a15, a3, 240
    1673:	03e8           	l32i.n	a14, a3, 0
    1675:	6da092        	movi	a9, 109
    1678:	9a6c           	movi.n	a10, -23
    167a:	00a1b2        	movi	a11, 0x100
    167d:	0c6c           	movi.n	a12, -32
    167f:	1d0c           	movi.n	a13, 1
    1681:	faa181        	l32r	a8, 108 (7fff0000 <_end+0x7ffed3c0>)
    1684:	436282        	s32i	a8, a2, 0x10c
    1687:	446282        	s32i	a8, a2, 0x110
    168a:	1552d2        	s16i	a13, a2, 42
    168d:	7152c2        	s16i	a12, a2, 226
    1690:	1252b2        	s16i	a11, a2, 36
    1693:	6f52a2        	s16i	a10, a2, 222
    1696:	8b5292        	s16i	a9, a2, 0x116
    1699:	02e9           	s32i.n	a14, a2, 0
    169b:	8a52f2        	s16i	a15, a2, 0x114
    169e:	10dee2        	addmi	a14, a14, 0x1000
    16a1:	b8cef2        	addi	a15, a14, -72
    16a4:	3b62f2        	s32i	a15, a2, 236
    16a7:	dccee2        	addi	a14, a14, -36
    16aa:	3c62e2        	s32i	a14, a2, 240
    16ad:	1d2426        	beqi	a4, 2, 16ce <common_NR_init+0xde>
    16b0:	2f0292        	l8ui	a9, a2, 47
    16b3:	1719a6        	blti	a9, 1, 16ce <common_NR_init+0xde>
    16b6:	0a0c           	movi.n	a10, 0
    16b8:	0b0c           	movi.n	a11, 0
    16ba:	fa94c1        	l32r	a12, 10c (40000000 <_end+0x3fffd3c0>)
    16bd:	4022d2        	l32i	a13, a2, 0x100

000016c0 <common_NR_init+0xd0>:
    16c0:	fdaa           	add.n	a15, a13, a10
    16c2:	0fc9           	s32i.n	a12, a15, 0
    16c4:	2f02e2        	l8ui	a14, a2, 47
    16c7:	bb1b           	addi.n	a11, a11, 1
    16c9:	aa4b           	addi.n	a10, a10, 4
    16cb:	f12be7        	blt	a11, a14, 16c0 <common_NR_init+0xd0>
    16ce:	f01d           	retw.n

000016d0 <get_post_ec_memsize>:
    16d0:	004136        	entry	a1, 32
    16d3:	fa8f21        	l32r	a2, 110 (1bf0 <_DYNAMIC+0x174>)
    16d6:	f01d           	retw.n

000016d8 <EXT_POST_EC_Init>:
    16d8:	004136        	entry	a1, 32
    16db:	02ad           	mov.n	a10, a2
    16dd:	fa8d81        	l32r	a8, 114 (1a08 <xt_memset>)
    16e0:	0b0c           	movi.n	a11, 0
    16e2:	fa8dc1        	l32r	a12, 118 (df8 <xtensa_fft8_32b+0x58>)
    16e5:	0008e0        	callx8	a8
    16e8:	a28b           	addi.n	a10, a2, 8
    16ea:	fa8c81        	l32r	a8, 11c (1a20 <xt_memcpy>)
    16ed:	b32b           	addi.n	a11, a3, 2
    16ef:	79a0c2        	movi	a12, 121
    16f2:	0008e0        	callx8	a8
    16f5:	5c0c           	movi.n	a12, 5
    16f7:	01d2a2        	addmi	a10, a2, 0x100
    16fa:	01d3b2        	addmi	a11, a3, 0x100
    16fd:	fa8781        	l32r	a8, 11c (1a20 <xt_memcpy>)
    1700:	f4cbb2        	addi	a11, a11, -12
    1703:	fccaa2        	addi	a10, a10, -4
    1706:	0008e0        	callx8	a8
    1709:	0229           	s32i.n	a2, a2, 0
    170b:	02bd           	mov.n	a11, a2
    170d:	fa8481        	l32r	a8, 120 (15a4 <common_noise_gen_init>)
    1710:	18d232        	addmi	a3, a2, 0x1800
    1713:	03ad           	mov.n	a10, a3
    1715:	0008e0        	callx8	a8
    1718:	fa8381        	l32r	a8, 124 (15f0 <common_NR_init>)
    171b:	03ad           	mov.n	a10, a3
    171d:	17d292        	addmi	a9, a2, 0x1700
    1720:	1bd2c2        	addmi	a12, a2, 0x1b00
    1723:	19d2b2        	addmi	a11, a2, 0x1900
    1726:	1ccbb2        	addi	a11, a11, 28
    1729:	1cccd2        	addi	a13, a12, 28
    172c:	fccce2        	addi	a14, a12, -4
    172f:	7e69e2        	s32i	a14, a9, 0x1f8
    1732:	7f69d2        	s32i	a13, a9, 0x1fc
    1735:	fcc932        	addi	a3, a9, -4
    1738:	8069b2        	s32i	a11, a9, 0x200
    173b:	3cccc2        	addi	a12, a12, 60
    173e:	02bd           	mov.n	a11, a2
    1740:	4c69c2        	s32i	a12, a9, 0x130
    1743:	0c0c           	movi.n	a12, 0
    1745:	0008e0        	callx8	a8
    1748:	8112f2        	l16ui	a15, a2, 0x102
    174b:	051292        	l16ui	a9, a2, 10
    174e:	1bd282        	addmi	a8, a2, 0x1b00
    1751:	fa42a1        	l32r	a10, 5c (7fff <_end+0x53bf>)
    1754:	10d2b2        	addmi	a11, a2, 0x1000
    1757:	7b63b2        	s32i	a11, a3, 0x1ec
    175a:	7258a2        	s16i	a10, a8, 228
    175d:	7458a2        	s16i	a10, a8, 232
    1760:	735892        	s16i	a9, a8, 230
    1763:	7558f2        	s16i	a15, a8, 234
    1766:	f01d           	retw.n

00001768 <aec_post_prcs>:
    1768:	004136        	entry	a1, 32
    176b:	fec552        	addi	a5, a5, -2
    176e:	fec442        	addi	a4, a4, -2
    1771:	406214        	ae_lp16f.i	aep6, a2, 2
    1774:	fa6d71        	l32r	a7, 128 (7fffff <_end+0x7fd3bf>)
    1777:	300664        	ae_cvtp24a16x2.ll	aep0, a6, a6
    177a:	fa3881        	l32r	a8, 5c (7fff <_end+0x53bf>)
    177d:	402204        	ae_lp16f.i	aep2, a2, 0
    1780:	626d           	ae_s16i.n	a6, a2, 0
    1782:	de42d8f88f 	{ ae_cvtp24a16x2.ll	aep7, a8, a8; ae_subsp24s	aep5, aep0, aep2 }
    1787:	eaf9a2377f 	{ ae_movpa24x2	aep3, a7, a7; ae_mulfp24s.ll	aeq0, aep6, aep7 }
    178c:	ce346c030f 	{ ae_lp16f.i	aep0, a3, 0; ae_sraip24	aep5, aep5, 4 }
    1791:	fa6780002f 	{ movi	a2, 0; ae_roundsp24q48asym	aep4, aeq0 }

00001796 <aec_post_prcs+0x2e>:
    1796:	07a524        	ae_addsp24s	aep2, aep2, aep5
    1799:	07b294        	ae_subsp24s	aep1, aep3, aep2
    179c:	011c74        	ae_mulfp24s.ll	aeq1, aep1, aep4
    179f:	221b           	addi.n	a2, a2, 1
    17a1:	fe2c400f9f 	{ movi	a9, 15; ae_roundsq32asym	aeq2, aeq1 }
    17a6:	17a976        	loopgtz	a9, 17c1 <aec_post_prcs+0x59>
    17a9:	ea09ecf51f 	{ ae_lp16f.iu	aep7, a5, 2; ae_mulfp24s.ll	aeq1, aep2, aep0 }
    17ae:	037294        	ae_mulfq32sp16s.l	aeq0, aeq2, aep7
    17b1:	ff2c7a802f 	{ ae_slliq56	aeq0, aeq0, 2; ae_roundsq32asym	aeq3, aeq1 }
    17b6:	01f8c4        	ae_addq56	aeq3, aeq3, aeq0
    17b9:	fb7bac831f 	{ ae_lp16f.iu	aep0, a3, 2; ae_roundsp16q48asym	aep6, aeq3 }
    17be:	42e414        	ae_sp16f.l.iu	aep6, a4, 2

000017c1 <aec_post_prcs+0x59>:
    17c1:	d1b266        	bnei	a2, 16, 1796 <aec_post_prcs+0x2e>
    17c4:	f01d           	retw.n

000017c6 <aec_post_prcs+0x5e>:
	...

000017c8 <ns_ifft_wo>:
    17c8:	004136        	entry	a1, 32
    17cb:	fa25a1        	l32r	a10, 60 (20c4 <fftorder_512_12b+0x4>)
    17ce:	80a082        	movi	a8, 128
    17d1:	0b0c           	movi.n	a11, 0
    17d3:	3a2292        	l32i	a9, a2, 232
    17d6:	0248           	l32i.n	a4, a2, 0
    17d8:	fa55e1        	l32r	a14, 12c (fffc <_end+0xd3bc>)
    17db:	0304d0        	rsr.br	a13
    17de:	2c0c           	movi.n	a12, 2
    17e0:	10dde0        	and	a13, a13, a14
    17e3:	20ddc0        	or	a13, a13, a12
    17e6:	1304d0        	wsr.br	a13
    17e9:	002020        	esync
    17ec:	14b9           	s32i.n	a11, a4, 4
    17ee:	04b9           	s32i.n	a11, a4, 0
    17f0:	5c0904        	ae_lp24x2f.i	aep0, a9, 0
    17f3:	0ea876        	loopgtz	a8, 1805 <ns_ifft_wo+0x3d>
    17f6:	2afd           	ae_l16si.n	a15, a10, 0
    17f8:	fc18404aae 	{ addi	a10, a10, 4; ae_negsp24s	aep1, aep0 }
    17fd:	e0042f891f 	{ ae_lp24x2f.iu	aep0, a9, 8; ae_movtp24x2	aep1, aep0, b0:b1 }
    1802:	5f14f4        	ae_sp24x2f.x	aep1, a4, a15

00001805 <ns_ifft_wo+0x3d>:
    1805:	5c89e4        	ae_lp24x2f.iu	aep0, a9, -16
    1808:	7fa082        	movi	a8, 127
    180b:	09a876        	loopgtz	a8, 1818 <ns_ifft_wo+0x50>
    180e:	2abd           	ae_l16si.n	a11, a10, 0
    1810:	aa4b           	addi.n	a10, a10, 4
    1812:	5f04b4        	ae_sp24x2f.x	aep0, a4, a11
    1815:	5c89f4        	ae_lp24x2f.iu	aep0, a9, -8
    1818:	04ad           	mov.n	a10, a4
    181a:	00a1b2        	movi	a11, 0x100
    181d:	8c0c           	movi.n	a12, 8
    181f:	fa1281        	l32r	a8, 68 (14c0 <sw_fft_32b_2811>)
    1822:	0d0c           	movi.n	a13, 0
    1824:	1e0c           	movi.n	a14, 1
    1826:	0008e0        	callx8	a8
    1829:	fec3b2        	addi	a11, a3, -2
    182c:	600404        	ae_lq32f.i	aeq0, a4, 0
    182f:	3f2292        	l32i	a9, a2, 252
    1832:	3d22c2        	l32i	a12, a2, 244
    1835:	1392e2        	l16si	a14, a2, 38
    1838:	fec992        	addi	a9, a9, -2
    183b:	feccd2        	addi	a13, a12, -2
    183e:	c0aae0        	sub	a10, a10, a14
    1841:	20ccc2        	addi	a12, a12, 32
    1844:	f8caa2        	addi	a10, a10, -8
    1847:	0e1c           	movi.n	a14, 16
    1849:	1cae76        	loopgtz	a14, 1869 <ns_ifft_wo+0xa1>
    184c:	40dcf4        	ae_lp16f.iu	aep5, a12, -2
    184f:	fc24fe8a8f 	{ ae_slaasq56s	aeq2, aeq0, a10; nop }
    1854:	fd546cc91f 	{ ae_lp16f.iu	aep4, a9, 2; ae_roundsq32sym	aeq1, aeq2 }
    1859:	eab0acbd1f 	{ ae_lp16f.iu	aep3, a13, 2; ae_mulfp24s.hl	aeq0, aep4, aep5 }
    185e:	033054        	ae_mulafq32sp16s.l	aeq0, aeq1, aep3

00001861 <ns_ifft_wo+0x99>:
    1861:	f9e3bd142f 	{ ae_lq32f.iu	aeq0, a4, 8; ae_roundsp16q48sym	aep2, aeq0 }
    1866:	42ab14        	ae_sp16f.l.iu	aep2, a11, 2

00001869 <ns_ifft_wo+0xa1>:
    1869:	e0a0d2        	movi	a13, 224
    186c:	f03d           	nop.n
    186e:	0fad76        	loopgtz	a13, 1881 <ns_ifft_wo+0xb9>
    1871:	fc24fe8a0f 	{ ae_slaasq56s	aeq0, aeq0, a10; nop }
    1876:	06c834        	ae_roundsq32sym	aeq3, aeq0
    1879:	fbfbbd142f 	{ ae_lq32f.iu	aeq0, a4, 8; ae_roundsp16q48sym	aep6, aeq3 }
    187e:	42eb14        	ae_sp16f.l.iu	aep6, a11, 2
    1881:	3f2292        	l32i	a9, a2, 252
    1884:	0e1c           	movi.n	a14, 16
    1886:	fec992        	addi	a9, a9, -2
    1889:	f03d           	nop.n
    188b:	f03d           	nop.n
    188d:	0fae76        	loopgtz	a14, 18a0 <ns_ifft_wo+0xd8>
    1890:	fc24fe8a8f 	{ ae_slaasq56s	aeq2, aeq0, a10; nop }
    1895:	066834        	ae_roundsq32sym	aeq1, aeq2
    1898:	fbebfd142f 	{ ae_lq32f.iu	aeq0, a4, 8; ae_roundsp16q48sym	aep7, aeq1 }
    189d:	42f914        	ae_sp16f.l.iu	aep7, a9, 2
    18a0:	f01d           	retw.n

000018a2 <ns_ifft_wo+0xda>:
	...

000018a4 <NOISE_GEN_Prcs>:
    18a4:	004136        	entry	a1, 32
    18a7:	039d           	mov.n	a9, a3
    18a9:	f9eea1        	l32r	a10, 64 (20c8 <fftorder_512_12b+0x8>)
    18ac:	441304        	ae_lp16x2f.i	aep1, a3, 0
    18af:	8f0c           	movi.n	a15, 8
    18b1:	3d22d2        	l32i	a13, a2, 244
    18b4:	02c8           	l32i.n	a12, a2, 0
    18b6:	3e22e2        	l32i	a14, a2, 248
    18b9:	fc2080788f 	{ movi	a8, 120; ae_zerop48	aep0 }
    18be:	04a876        	loopgtz	a8, 18c6 <NOISE_GEN_Prcs+0x22>
    18c1:	d821ad991f 	{ ae_lp16x2f.iu	aep1, a9, 4; ae_maxabssp24s	aep0, aep0, aep1 }

000018c6 <NOISE_GEN_Prcs+0x22>:
    18c6:	dc01c0009f 	{ movi	a9, 0; ae_selp24.lh	aep1, aep0, aep0 }
    18cb:	d82261228e 	{ l16si	a8, a2, 36; ae_maxp24s	aep1, aep0, aep1 }
    18d0:	0015d4        	ae_cvta32p24.l	a5, aep1
    18d3:	04dcc2        	addmi	a12, a12, 0x400
    18d6:	04ccb2        	addi	a11, a12, 4
    18d9:	40e550        	nsa	a5, a5
    18dc:	125252        	s16i	a5, a2, 36
    18df:	435580        	min	a5, a5, a8
    18e2:	031654        	wur.ae_sar	a5
    18e5:	440e04        	ae_lp16x2f.i	aep0, a14, 0
    18e8:	f823ad1d0f 	{ ae_lp16x2f.i	aep1, a13, 0; ae_sllssp24s	aep0, aep0 }
    18ed:	18af76        	loopgtz	a15, 1909 <NOISE_GEN_Prcs+0x65>
    18f0:	010944        	ae_mulfp24s.hh	aeq1, aep0, aep1
    18f3:	010174        	ae_mulfp24s.ll	aeq0, aep0, aep1
    18f6:	629c94        	ae_sq32f.x	aeq1, a12, a9
    18f9:	628b94        	ae_sq32f.x	aeq0, a11, a9
    18fc:	449d14        	ae_lp16x2f.iu	aep1, a13, 4
    18ff:	44ae14        	ae_lp16x2f.iu	aep2, a14, 4
    1902:	2a9d           	ae_l16si.n	a9, a10, 0
    1904:	f82b808aae 	{ addi	a10, a10, 8; ae_sllssp24s	aep0, aep2 }
    1909:	440304        	ae_lp16x2f.i	aep0, a3, 0
    190c:	70a0e2        	movi	a14, 112
    190f:	14ae76        	loopgtz	a14, 1927 <NOISE_GEN_Prcs+0x83>
    1912:	0b00b4        	ae_sllssp24s	aep3, aep0
    1915:	0acb84        	ae_cvtq48p24s.h	aeq3, aep3
    1918:	fe6c387c9f 	{ ae_sq32f.x	aeq3, a12, a9; ae_cvtq48p24s.l	aeq2, aep3 }
    191d:	62ab94        	ae_sq32f.x	aeq2, a11, a9
    1920:	2a9d           	ae_l16si.n	a9, a10, 0
    1922:	448314        	ae_lp16x2f.iu	aep0, a3, 4
    1925:	aa8b           	addi.n	a10, a10, 8
    1927:	3e22e2        	l32i	a14, a2, 248
    192a:	8f0c           	movi.n	a15, 8
    192c:	fccee2        	addi	a14, a14, -4
    192f:	19af76        	loopgtz	a15, 194c <NOISE_GEN_Prcs+0xa8>
    1932:	fa23adddff 	{ ae_lp16x2f.iu	aep5, a13, -4; ae_sllssp24s	aep4, aep0 }
    1937:	014d54        	ae_mulfp24s.hl	aeq1, aep4, aep5
    193a:	e8b1b38e1f 	{ ae_sp16x2f.iu	aep0, a14, 4; ae_mulfp24s.lh	aeq0, aep4, aep5 }
    193f:	629c94        	ae_sq32f.x	aeq1, a12, a9
    1942:	628b94        	ae_sq32f.x	aeq0, a11, a9
    1945:	2a9d           	ae_l16si.n	a9, a10, 0
    1947:	448314        	ae_lp16x2f.iu	aep0, a3, 4
    194a:	aa8b           	addi.n	a10, a10, 8
    194c:	80a0b2        	movi	a11, 128
    194f:	8c0c           	movi.n	a12, 8
    1951:	1d0c           	movi.n	a13, 1
    1953:	02a8           	l32i.n	a10, a2, 0
    1955:	f9c481        	l32r	a8, 68 (14c0 <sw_fft_32b_2811>)
    1958:	0e0c           	movi.n	a14, 0
    195a:	04daa2        	addmi	a10, a10, 0x400
    195d:	0008e0        	callx8	a8
    1960:	00a1b2        	movi	a11, 0x100
    1963:	c095a0        	sub	a9, a5, a10
    1966:	f9c181        	l32r	a8, 6c (3e4 <gen_power>)
    1969:	135292        	s16i	a9, a2, 38
    196c:	02ad           	mov.n	a10, a2
    196e:	0008e0        	callx8	a8
    1971:	f9ef81        	l32r	a8, 130 (590 <NR_vad1>)
    1974:	02ad           	mov.n	a10, a2
    1976:	0008e0        	callx8	a8
    1979:	f9ee81        	l32r	a8, 134 (694 <NR_vad2>)
    197c:	02ad           	mov.n	a10, a2
    197e:	0008e0        	callx8	a8
    1981:	f9ed81        	l32r	a8, 138 (80c <NR_NoiseEstimate>)
    1984:	02ad           	mov.n	a10, a2
    1986:	0008e0        	callx8	a8
    1989:	f9ec81        	l32r	a8, 13c (1d0 <comfort_noise_gen_wb2>)
    198c:	02ad           	mov.n	a10, a2
    198e:	0008e0        	callx8	a8
    1991:	f9eb81        	l32r	a8, 140 (17c8 <ns_ifft_wo>)
    1994:	04bd           	mov.n	a11, a4
    1996:	02ad           	mov.n	a10, a2
    1998:	0008e0        	callx8	a8
    199b:	f01d           	retw.n

0000199d <NOISE_GEN_Prcs+0xf9>:
    199d:	000000                                        ...

000019a0 <EXT_POST_EC_PRCS>:
    19a0:	004136        	entry	a1, 32
    19a3:	18d2a2        	addmi	a10, a2, 0x1800
    19a6:	f9e781        	l32r	a8, 144 (18a4 <NOISE_GEN_Prcs>)
    19a9:	03bd           	mov.n	a11, a3
    19ab:	02cd           	mov.n	a12, a2
    19ad:	0008e0        	callx8	a8
    19b0:	04cd           	mov.n	a12, a4
    19b2:	03bd           	mov.n	a11, a3
    19b4:	f9e581        	l32r	a8, 148 (1768 <aec_post_prcs>)
    19b7:	02dd           	mov.n	a13, a2
    19b9:	11e590        	slli	a14, a5, 7
    19bc:	1cd2a2        	addmi	a10, a2, 0x1c00
    19bf:	e4caa2        	addi	a10, a10, -28
    19c2:	7fcee2        	addi	a14, a14, 127
    19c5:	1bd222        	addmi	a2, a2, 0x1b00
    19c8:	7452e2        	s16i	a14, a2, 232
    19cb:	ecc222        	addi	a2, a2, -20
    19ce:	8eed           	ae_sext16	a14, a14
    19d0:	0008e0        	callx8	a8
    19d3:	f9def1        	l32r	a15, 14c (2000 <ROOTS_512_Q16+0x40>)
    19d6:	080c           	movi.n	a8, 0
    19d8:	7f9292        	l16si	a9, a2, 254
    19db:	669d           	ae_s16i.n	a9, a6, 0
    19dd:	768d           	ae_s16i.n	a8, a6, 2
    19df:	0256f2        	s16i	a15, a6, 4
    19e2:	f01d           	retw.n

000019e4 <EXT_POST_EC_PRCS+0x44>:
    19e4:	00000000                                ....

000019e8 <__do_global_ctors_aux>:
    19e8:	004136        	entry	a1, 32
    19eb:	f9d981        	l32r	a8, 150 (22f4 <fftorder_512_12b+0x234>)
    19ee:	7f2882        	l32i	a8, a8, 0x1fc
    19f1:	f9d821        	l32r	a2, 154 (24f0 <__CTOR_LIST___47>)
    19f4:	0c0826        	beqi	a8, -1, 1a04 <__do_global_ctors_aux+0x1c>
    19f7:	02a8           	l32i.n	a10, a2, 0

000019f9 <__do_global_ctors_aux+0x11>:
    19f9:	000ae0        	callx8	a10
    19fc:	fcc222        	addi	a2, a2, -4
    19ff:	02a8           	l32i.n	a10, a2, 0
    1a01:	f40a66        	bnei	a10, -1, 19f9 <__do_global_ctors_aux+0x11>

00001a04 <__do_global_ctors_aux+0x1c>:
    1a04:	f01d           	retw.n

00001a06 <__do_global_ctors_aux+0x1e>:
	...

00001a08 <xt_memset>:
    1a08:	004136        	entry	a1, 32
    1a0b:	026d           	mov.n	a6, a2
    1a0d:	42cc           	bnez.n	a2, 1a15 <xt_memset+0xd>
    1a0f:	020c           	movi.n	a2, 0
    1a11:	f01d           	retw.n

00001a13 <xt_memset+0xb>:
	...

00001a15 <xt_memset+0xd>:
    1a15:	029476        	loopnez	a4, 1a1b <xt_memset+0x13>
    1a18:	09c634        	ae_s16ip	a3, a6, 2

00001a1b <xt_memset+0x13>:
    1a1b:	f01d           	retw.n

00001a1d <xt_memset+0x15>:
    1a1d:	000000                                        ...

00001a20 <xt_memcpy>:
    1a20:	004136        	entry	a1, 32
    1a23:	22ac           	beqz.n	a2, 1a49 <xt_memcpy+0x29>
    1a25:	03ac           	beqz.n	a3, 1a49 <xt_memcpy+0x29>
    1a27:	0bb327        	bgeu	a3, a2, 1a36 <xt_memcpy+0x16>
    1a2a:	907430        	addx2	a7, a4, a3
    1a2d:	05b277        	bgeu	a2, a7, 1a36 <xt_memcpy+0x16>
    1a30:	903420        	addx2	a3, a4, a2
    1a33:	0007c6        	j	1a56 <xt_memcpy+0x36>

00001a36 <xt_memcpy+0x16>:
    1a36:	026d           	mov.n	a6, a2
    1a38:	000186        	j	1a42 <xt_memcpy+0x22>

00001a3b <xt_memcpy+0x1b>:
    1a3b:	235d           	ae_l16si.n	a5, a3, 0
    1a3d:	09c654        	ae_s16ip	a5, a6, 2
    1a40:	332b           	addi.n	a3, a3, 2

00001a42 <xt_memcpy+0x22>:
    1a42:	440b           	addi.n	a4, a4, -1
    1a44:	f30466        	bnei	a4, -1, 1a3b <xt_memcpy+0x1b>

00001a47 <xt_memcpy+0x27>:
    1a47:	f01d           	retw.n

00001a49 <xt_memcpy+0x29>:
    1a49:	020c           	movi.n	a2, 0
    1a4b:	f01d           	retw.n

00001a4d <xt_memcpy+0x2d>:
    1a4d:	063d           	mov.n	a3, a6
    1a4f:	fec772        	addi	a7, a7, -2
    1a52:	278d           	ae_l16si.n	a8, a7, 0
    1a54:	668d           	ae_s16i.n	a8, a6, 0

00001a56 <xt_memcpy+0x36>:
    1a56:	fec362        	addi	a6, a3, -2
    1a59:	440b           	addi.n	a4, a4, -1
    1a5b:	ee0466        	bnei	a4, -1, 1a4d <xt_memcpy+0x2d>
    1a5e:	fff946        	j	1a47 <xt_memcpy+0x27>

00001a61 <xt_memcpy+0x41>:
    1a61:	000000                                        ...

00001a64 <_fini>:
    1a64:	008136        	entry	a1, 64
    1a67:	f9bc81        	l32r	a8, 158 (178 <__do_global_dtors_aux>)
    1a6a:	f03d           	nop.n
    1a6c:	0008e0        	callx8	a8

00001a6f <_fini+0xb>:
    1a6f:	f01d           	retw.n
