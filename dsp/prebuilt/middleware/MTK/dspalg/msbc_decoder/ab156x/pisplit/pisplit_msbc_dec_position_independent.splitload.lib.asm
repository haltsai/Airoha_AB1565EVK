
Build/lib/pisplit_msbc_dec_position_independent.splitload.lib:     file format elf32-xtensa-le


Disassembly of section .text:

00000000 <_init-0xb8>:
   0:	0000010c 00000fb0 0000245c 00002414     ........\$...$..
  10:	00000000 000023f8 00000000 00002460     .....#......`$..
  20:	00000128 0000080c 000022c0 00000358     (........"..X...
  30:	000001c8 00000214 00000164 000005b0     ........d.......
  40:	000005d8 000008a8 00000ba0 00000cc8     ................
  50:	000013c0 000013a0 00000baa 00000bea     ................
  60:	00001460 00000880 00000894 00005a00     `............Z..
  70:	00001600 000014f0 000014b0 00001870     ............p...
  80:	00001670 00001570 00001be0 00002478     p...p.......x$..
  90:	000023c0 00000f88 000023d4 00000e10     .#.......#......
  a0:	00002418 00000f98 00000f60 00002200     .$......`...."..
  b0:	000023fc 000000d0                       .#......

000000b8 <_init>:
  b8:	008136        	entry	a1, 64
  bb:	ffd181        	l32r	a8, 0 (10c <frame_dummy>)
  be:	f03d           	nop.n
  c0:	0008e0        	callx8	a8
  c3:	ffd081        	l32r	a8, 4 (fb0 <__do_global_ctors_aux>)
  c6:	f03d           	nop.n
  c8:	0008e0        	callx8	a8
  cb:	f01d           	retw.n

000000cd <_init+0x15>:
  cd:	000000                                        ...

000000d0 <__do_global_dtors_aux>:
  d0:	004136        	entry	a1, 32
  d3:	ffcd31        	l32r	a3, 8 (245c <__bss_start>)
  d6:	000382        	l8ui	a8, a3, 0
  d9:	d8ec           	bnez.n	a8, 10a <__do_global_dtors_aux+0x3a>
  db:	ffcc21        	l32r	a2, c (2414 <p$3519_6_1>)
  de:	02b8           	l32i.n	a11, a2, 0
  e0:	0ba8           	l32i.n	a10, a11, 0
  e2:	ca8c           	beqz.n	a10, f2 <__do_global_dtors_aux+0x22>

000000e4 <__do_global_dtors_aux+0x14>:
  e4:	9b4b           	addi.n	a9, a11, 4
  e6:	0299           	s32i.n	a9, a2, 0
  e8:	000ae0        	callx8	a10
  eb:	02b8           	l32i.n	a11, a2, 0
  ed:	0ba8           	l32i.n	a10, a11, 0
  ef:	ff1a56        	bnez	a10, e4 <__do_global_dtors_aux+0x14>

000000f2 <__do_global_dtors_aux+0x22>:
  f2:	ffc7a1        	l32r	a10, 10 (0 <_text_start>)
  f5:	0b0c           	movi.n	a11, 0
  f7:	081ab7        	beq	a10, a11, 103 <__do_global_dtors_aux+0x33>
  fa:	ffc6a1        	l32r	a10, 14 (23f8 <__EH_FRAME_BEGIN___44>)
  fd:	ffc481        	l32r	a8, 10 (0 <_text_start>)
 100:	0008e0        	callx8	a8

00000103 <__do_global_dtors_aux+0x33>:
 103:	1c0c           	movi.n	a12, 1
 105:	0043c2        	s8i	a12, a3, 0
 108:	f01d           	retw.n

0000010a <__do_global_dtors_aux+0x3a>:
 10a:	f01d           	retw.n

0000010c <frame_dummy>:
 10c:	004136        	entry	a1, 32
 10f:	ffc281        	l32r	a8, 18 (0 <_text_start>)
 112:	090c           	movi.n	a9, 0
 114:	0b1897        	beq	a8, a9, 123 <frame_dummy+0x17>
 117:	ffbfa1        	l32r	a10, 14 (23f8 <__EH_FRAME_BEGIN___44>)
 11a:	ffc0b1        	l32r	a11, 1c (2460 <object$3538_8_1>)
 11d:	ffbe81        	l32r	a8, 18 (0 <_text_start>)
 120:	0008e0        	callx8	a8

00000123 <frame_dummy+0x17>:
 123:	f01d           	retw.n

00000125 <frame_dummy+0x19>:
 125:	000000                                        ...

00000128 <xt_memset>:
 128:	004136        	entry	a1, 32
 12b:	026d           	mov.n	a6, a2
 12d:	42cc           	bnez.n	a2, 135 <xt_memset+0xd>
 12f:	020c           	movi.n	a2, 0
 131:	f01d           	retw.n

00000133 <xt_memset+0xb>:
	...

00000135 <xt_memset+0xd>:
 135:	029476        	loopnez	a4, 13b <xt_memset+0x13>
 138:	09c634        	ae_s16ip	a3, a6, 2

0000013b <xt_memset+0x13>:
 13b:	f01d           	retw.n

0000013d <xt_memset+0x15>:
 13d:	000000                                        ...

00000140 <sbc_init>:
 140:	004136        	entry	a1, 32
 143:	02ad           	mov.n	a10, a2
 145:	ffb681        	l32r	a8, 20 (128 <xt_memset>)
 148:	0b0c           	movi.n	a11, 0
 14a:	ffb6c1        	l32r	a12, 24 (80c <sbc_bit_allocation+0x234>)
 14d:	0008e0        	callx8	a8
 150:	10d2a2        	addmi	a10, a2, 0x1000
 153:	090c           	movi.n	a9, 0
 155:	04d2c2        	addmi	a12, a2, 0x400
 158:	00a2b2        	movi	a11, 0x200
 15b:	4c5cb2        	s16i	a11, a12, 152
 15e:	085a92        	s16i	a9, a10, 16
 161:	f01d           	retw.n

00000163 <sbc_init+0x23>:
	...

00000164 <sbc_crc8>:
 164:	004136        	entry	a1, 32
 167:	027d           	mov.n	a7, a2
 169:	249030        	extui	a9, a3, 0, 3
 16c:	218330        	srai	a8, a3, 3
 16f:	2a18a6        	blti	a8, 1, 19d <sbc_crc8+0x39>
 172:	025d           	mov.n	a5, a2
 174:	ffad61        	l32r	a6, 28 (22c0 <SBC_crc_table>)
 177:	f20c           	movi.n	a2, 15
 179:	1da876        	loopgtz	a8, 19a <sbc_crc8+0x36>
 17c:	454d           	ae_l16ui.n	a4, a5, 0
 17e:	552b           	addi.n	a5, a5, 2
 180:	302420        	xor	a2, a4, a2
 183:	044020        	extui	a4, a2, 0, 1
 186:	412120        	srli	a2, a2, 1
 189:	902260        	addx2	a2, a2, a6
 18c:	422d           	ae_l16ui.n	a2, a2, 0
 18e:	1144d0        	slli	a4, a4, 3
 191:	400400        	ssr	a4
 194:	b12020        	sra	a2, a2
 197:	742020        	extui	a2, a2, 0, 8

0000019a <sbc_crc8+0x36>:
 19a:	0000c6        	j	1a1 <sbc_crc8+0x3d>

0000019d <sbc_crc8+0x39>:
 19d:	080c           	movi.n	a8, 0
 19f:	f20c           	movi.n	a2, 15

000001a1 <sbc_crc8+0x3d>:
 1a1:	90a870        	addx2	a10, a8, a7
 1a4:	d61c           	movi.n	a6, 29
 1a6:	070c           	movi.n	a7, 0
 1a8:	4a3d           	ae_l16ui.n	a3, a10, 0
 1aa:	16a976        	loopgtz	a9, 1c4 <sbc_crc8+0x60>
 1ad:	644020        	extui	a4, a2, 0, 7
 1b0:	305320        	xor	a5, a3, a2
 1b3:	1144f0        	slli	a4, a4, 1
 1b6:	1133f0        	slli	a3, a3, 1
 1b9:	045750        	extui	a5, a5, 7, 1
 1bc:	072d           	mov.n	a2, a7
 1be:	932650        	movnez	a2, a6, a5
 1c1:	302240        	xor	a2, a2, a4

000001c4 <sbc_crc8+0x60>:
 1c4:	f01d           	retw.n

000001c6 <sbc_crc8+0x62>:
	...

000001c8 <sbc_extract_scale_factors_joint_ch4>:
 1c8:	004136        	entry	a1, 32
 1cb:	38c262        	addi	a6, a2, 56
 1ce:	16c252        	addi	a5, a2, 22
 1d1:	890c           	movi.n	a9, 8
 1d3:	02d242        	addmi	a4, a2, 0x200
 1d6:	1a12b2        	l16ui	a11, a2, 52
 1d9:	70c4a2        	addi	a10, a4, 112
 1dc:	9b2442        	l32i	a4, a4, 0x26c
 1df:	41b1b0        	srli	a11, a11, 1
 1e2:	1eab76        	loopgtz	a11, 204 <sbc_extract_scale_factors_joint_ch4+0x3c>
 1e5:	03c794        	ae_lb	a12, a9
 1e8:	1c1494        	ae_db	a4, a9
 1eb:	3470c0        	extui	a7, a12, 0, 4
 1ee:	767d           	ae_s16i.n	a7, a6, 2
 1f0:	34c4c0        	extui	a12, a12, 4, 4
 1f3:	66cd           	ae_s16i.n	a12, a6, 0
 1f5:	458d           	ae_l16ui.n	a8, a5, 0
 1f7:	1177c0        	slli	a7, a7, 4
 1fa:	664b           	addi.n	a6, a6, 4
 1fc:	2088c0        	or	a8, a8, a12
 1ff:	09c584        	ae_s16ip	a8, a5, 2
 202:	657d           	ae_s16i.n	a7, a5, 0

00000204 <sbc_extract_scale_factors_joint_ch4+0x3c>:
 204:	7f6a42        	s32i	a4, a10, 0x1fc
 207:	4e0c           	movi.n	a14, 4
 209:	db2b           	addi.n	a13, a11, 2
 20b:	b0dde0        	addx8	a13, a13, a14
 20e:	1552d2        	s16i	a13, a2, 42
 211:	f01d           	retw.n

00000213 <sbc_extract_scale_factors_joint_ch4+0x4b>:
	...

00000214 <sbc_extract_scale_factors_joint_ch8_non>:
 214:	004136        	entry	a1, 32
 217:	38c272        	addi	a7, a2, 56
 21a:	890c           	movi.n	a9, 8
 21c:	1a12b2        	l16ui	a11, a2, 52
 21f:	906320        	addx2	a6, a3, a2
 222:	02d252        	addmi	a5, a2, 0x200
 225:	70c5a2        	addi	a10, a5, 112
 228:	12c662        	addi	a6, a6, 18
 22b:	9b2552        	l32i	a5, a5, 0x26c
 22e:	41b1b0        	srli	a11, a11, 1
 231:	14ab76        	loopgtz	a11, 249 <sbc_extract_scale_factors_joint_ch8_non+0x35>
 234:	038794        	ae_lb	a8, a9
 237:	1c1594        	ae_db	a5, a9
 23a:	34d080        	extui	a13, a8, 0, 4
 23d:	34c480        	extui	a12, a8, 4, 4
 240:	67cd           	ae_s16i.n	a12, a7, 0
 242:	77dd           	ae_s16i.n	a13, a7, 2
 244:	09c684        	ae_s16ip	a8, a6, 2
 247:	774b           	addi.n	a7, a7, 4

00000249 <sbc_extract_scale_factors_joint_ch8_non+0x35>:
 249:	7f6a52        	s32i	a5, a10, 0x1fc
 24c:	e3ba           	add.n	a14, a3, a11
 24e:	11eed0        	slli	a14, a14, 3
 251:	1552e2        	s16i	a14, a2, 42
 254:	f01d           	retw.n

00000256 <sbc_extract_scale_factors_joint_ch8_non+0x42>:
	...

00000258 <sbc_get_frame_length>:
 258:	004136        	entry	a1, 32
 25b:	04d222        	addmi	a2, a2, 0x400
 25e:	4c1222        	l16ui	a2, a2, 152
 261:	f01d           	retw.n

00000263 <sbc_get_frame_length+0xb>:
	...

00000264 <sbc_get_sample_freq>:
 264:	004136        	entry	a1, 32
 267:	021222        	l16ui	a2, a2, 4
 26a:	f01d           	retw.n

0000026c <sbc_frame_header_decode>:
 26c:	004136        	entry	a1, 32
 26f:	9ca072        	movi	a7, 156
 272:	060c           	movi.n	a6, 0
 274:	0ad342        	addmi	a4, a3, 0xa00
 277:	626d           	ae_s16i.n	a6, a2, 0
 279:	832422        	l32i	a2, a4, 0x20c
 27c:	10c462        	addi	a6, a4, 16

0000027f <sbc_frame_header_decode+0x13>:
 27f:	000282        	l8ui	a8, a2, 0
 282:	221b           	addi.n	a2, a2, 1
 284:	7f6622        	s32i	a2, a6, 0x1fc
 287:	f49877        	bne	a8, a7, 27f <sbc_frame_header_decode+0x13>
 28a:	140c           	movi.n	a4, 1
 28c:	480c           	movi.n	a8, 4
 28e:	0002b2        	l8ui	a11, a2, 0
 291:	d21b           	addi.n	a13, a2, 1
 293:	7f66d2        	s32i	a13, a6, 0x1fc
 296:	1494b0        	extui	a9, a11, 4, 2
 299:	14a6b0        	extui	a10, a11, 6, 2
 29c:	0470b0        	extui	a7, a11, 0, 1
 29f:	0953b2        	s16i	a11, a3, 18
 2a2:	04c1b0        	extui	a12, a11, 1, 1
 2a5:	0553c2        	s16i	a12, a3, 10
 2a8:	7f2622        	l32i	a2, a6, 0x1fc
 2ab:	a07780        	addx4	a7, a7, a8
 2ae:	0253a2        	s16i	a10, a3, 4
 2b1:	a09980        	addx4	a9, a9, a8
 2b4:	9ca3a2        	movi	a10, 0x39c
 2b7:	035392        	s16i	a9, a3, 6
 2ba:	065372        	s16i	a7, a3, 12
 2bd:	280c           	movi.n	a8, 2
 2bf:	c21b           	addi.n	a12, a2, 1
 2c1:	14b2b0        	extui	a11, a11, 2, 2
 2c4:	0453b2        	s16i	a11, a3, 8
 2c7:	8384b0        	moveqz	a8, a4, a11
 2ca:	075382        	s16i	a8, a3, 14
 2cd:	071352        	l16ui	a5, a3, 14
 2d0:	000222        	l8ui	a2, a2, 0
 2d3:	7f66c2        	s32i	a12, a6, 0x1fc
 2d6:	085322        	s16i	a2, a3, 16
 2d9:	0a5322        	s16i	a2, a3, 20
 2dc:	04ab16        	beqz	a11, 32a <sbc_frame_header_decode+0xbe>
 2df:	471b26        	beqi	a11, 1, 32a <sbc_frame_header_decode+0xbe>
 2e2:	c12290        	mul16u	a2, a2, a9
 2e5:	43aa           	add.n	a4, a3, a10
 2e7:	c1e780        	mul16u	a14, a7, a8
 2ea:	978d           	ae_zext16	a8, a7
 2ec:	2171e0        	srai	a7, a14, 1
 2ef:	012b26        	beqi	a11, 2, 2f4 <sbc_frame_header_decode+0x88>
 2f2:	228a           	add.n	a2, a2, a8

000002f4 <sbc_frame_header_decode+0x88>:
 2f4:	f27b           	addi.n	a15, a2, 7
 2f6:	21f3f0        	srai	a15, a15, 3
 2f9:	f7fa           	add.n	a15, a7, a15
 2fb:	ff4b           	addi.n	a15, a15, 4
 2fd:	7e54f2        	s16i	a15, a4, 252

00000300 <sbc_frame_header_decode+0x94>:
 300:	061382        	l16ui	a8, a3, 12
 303:	031342        	l16ui	a4, a3, 6
 306:	7f2622        	l32i	a2, a6, 0x1fc
 309:	c15580        	mul16u	a5, a5, a8
 30c:	c18480        	mul16u	a8, a4, a8
 30f:	1a5352        	s16i	a5, a3, 52
 312:	c14450        	mul16u	a4, a4, a5
 315:	1b5342        	s16i	a4, a3, 54
 318:	185382        	s16i	a8, a3, 48
 31b:	421b           	addi.n	a4, a2, 1
 31d:	000222        	l8ui	a2, a2, 0
 320:	7f6642        	s32i	a4, a6, 0x1fc
 323:	165322        	s16i	a2, a3, 44
 326:	020c           	movi.n	a2, 0
 328:	f01d           	retw.n

0000032a <sbc_frame_header_decode+0xbe>:
 32a:	c1b890        	mul16u	a11, a8, a9
 32d:	c1c870        	mul16u	a12, a8, a7
 330:	82b2b0        	mull	a11, a2, a11
 333:	21c1c0        	srai	a12, a12, 1
 336:	bb7b           	addi.n	a11, a11, 7
 338:	21b3b0        	srai	a11, a11, 3
 33b:	bbca           	add.n	a11, a11, a12
 33d:	c3aa           	add.n	a12, a3, a10
 33f:	bb4b           	addi.n	a11, a11, 4
 341:	7e5cb2        	s16i	a11, a12, 252
 344:	ffee06        	j	300 <sbc_frame_header_decode+0x94>

00000347 <sbc_frame_header_decode+0xdb>:
	...

00000348 <change_endian16x>:
 348:	004136        	entry	a1, 32
 34b:	743820        	extui	a3, a2, 8, 8
 34e:	8a7230        	depbits	a3, a2, 8, 8
 351:	832d           	ae_sext16	a2, a3
 353:	f01d           	retw.n

00000355 <change_endian16x+0xd>:
 355:	000000                                        ...

00000358 <SBC_decode_preload>:
 358:	004136        	entry	a1, 32
 35b:	04d2a2        	addmi	a10, a2, 0x400
 35e:	0cd252        	addmi	a5, a2, 0xc00
 361:	3558           	l32i.n	a5, a5, 12
 363:	d37c           	movi.n	a3, -3
 365:	f47c           	movi.n	a4, -1
 367:	304540        	xor	a4, a5, a4
 36a:	046050        	extui	a6, a5, 0, 1
 36d:	1166d0        	slli	a6, a6, 3
 370:	044040        	extui	a4, a4, 0, 1
 373:	743b           	addi.n	a7, a4, 3
 375:	032664        	wur.ae_bitptr	a6
 378:	12a347        	bge	a3, a4, 38e <SBC_decode_preload+0x36>
 37b:	040c           	movi.n	a4, 0
 37d:	0aa776        	loopgtz	a7, 38b <SBC_decode_preload+0x33>
 380:	000582        	l8ui	a8, a5, 0
 383:	114480        	slli	a4, a4, 8
 386:	551b           	addi.n	a5, a5, 1
 388:	0a7840        	depbits	a4, a8, 0, 8

0000038b <SBC_decode_preload+0x33>:
 38b:	000046        	j	390 <SBC_decode_preload+0x38>

0000038e <SBC_decode_preload+0x36>:
 38e:	040c           	movi.n	a4, 0

00000390 <SBC_decode_preload+0x38>:
 390:	f3f140        	wur.ae_bithead	a4
 393:	fec592        	addi	a9, a5, -2
 396:	1b6a92        	s32i	a9, a10, 108
 399:	f01d           	retw.n

0000039b <SBC_decode_preload+0x43>:
	...

0000039c <sbc_frame_decode>:
 39c:	004136        	entry	a1, 32
 39f:	ff2381        	l32r	a8, 2c (358 <SBC_decode_preload>)
 3a2:	02ad           	mov.n	a10, a2
 3a4:	0008e0        	callx8	a8
 3a7:	041292        	l16ui	a9, a2, 8
 3aa:	6ca2d2        	movi	a13, 0x26c
 3ad:	0b0c           	movi.n	a11, 0
 3af:	283966        	bnei	a9, 3, 3db <sbc_frame_decode+0x3f>
 3b2:	02d2e2        	addmi	a14, a2, 0x200
 3b5:	0612b2        	l16ui	a11, a2, 12
 3b8:	03a7b4        	ae_lb	a10, a11
 3bb:	9b2ec2        	l32i	a12, a14, 0x26c
 3be:	1752a2        	s16i	a10, a2, 46
 3c1:	9a9d           	ae_zext16	a9, a10
 3c3:	1c1cb4        	ae_db	a12, a11
 3c6:	9b6ec2        	s32i	a12, a14, 0x26c
 3c9:	fccbb2        	addi	a11, a11, -4
 3cc:	1a4b56        	bnez	a11, 574 <sbc_frame_decode+0x1d8>
 3cf:	34a0a0        	extui	a10, a10, 0, 4
 3d2:	11aac0        	slli	a10, a10, 4

000003d5 <sbc_frame_decode+0x39>:
 3d5:	0b52a2        	s16i	a10, a2, 22
 3d8:	000106        	j	3e0 <sbc_frame_decode+0x44>

000003db <sbc_frame_decode+0x3f>:
 3db:	090c           	movi.n	a9, 0
 3dd:	1752b2        	s16i	a11, a2, 46

000003e0 <sbc_frame_decode+0x44>:
 3e0:	59bc           	beqz.n	a9, 419 <sbc_frame_decode+0x7d>
 3e2:	1a0c           	movi.n	a10, 1
 3e4:	e2da           	add.n	a14, a2, a13
 3e6:	0612c2        	l16ui	a12, a2, 12
 3e9:	04d2d2        	addmi	a13, a2, 0x400
 3ec:	58cdd2        	addi	a13, a13, 88
 3ef:	7f6ed2        	s32i	a13, a14, 0x1fc
 3f2:	bc0b           	addi.n	a11, a12, -1
 3f4:	401b00        	ssl	a11
 3f7:	a1aa00        	sll	a10, a10
 3fa:	bc9c           	beqz.n	a12, 419 <sbc_frame_decode+0x7d>
 3fc:	0b0c           	movi.n	a11, 0

000003fe <sbc_frame_decode+0x62>:
 3fe:	1712f2        	l16ui	a15, a2, 46
 401:	0c0af7        	bnone	a10, a15, 411 <sbc_frame_decode+0x75>
 404:	7f2e82        	l32i	a8, a14, 0x1fc
 407:	6dbd           	ae_s16i.n	a11, a13, 0
 409:	0612c2        	l16ui	a12, a2, 12
 40c:	d82b           	addi.n	a13, a8, 2
 40e:	7f6ed2        	s32i	a13, a14, 0x1fc

00000411 <sbc_frame_decode+0x75>:
 411:	41a1a0        	srli	a10, a10, 1
 414:	bb1b           	addi.n	a11, a11, 1
 416:	e42bc7        	blt	a11, a12, 3fe <sbc_frame_decode+0x62>
 419:	041292        	l16ui	a9, a2, 8
 41c:	0612a2        	l16ui	a10, a2, 12
 41f:	103966        	bnei	a9, 3, 433 <sbc_frame_decode+0x97>
 422:	fccaa2        	addi	a10, a10, -4
 425:	106a56        	bnez	a10, 52f <sbc_frame_decode+0x193>
 428:	ff0281        	l32r	a8, 30 (1c8 <sbc_extract_scale_factors_joint_ch4>)
 42b:	02ad           	mov.n	a10, a2
 42d:	0008e0        	callx8	a8
 430:	000246        	j	43d <sbc_frame_decode+0xa1>

00000433 <sbc_frame_decode+0x97>:
 433:	ff0081        	l32r	a8, 34 (214 <sbc_extract_scale_factors_joint_ch8_non>)
 436:	02ad           	mov.n	a10, a2
 438:	2b0c           	movi.n	a11, 2
 43a:	0008e0        	callx8	a8

0000043d <sbc_frame_decode+0xa1>:
 43d:	fefe81        	l32r	a8, 38 (164 <sbc_crc8>)
 440:	12c2a2        	addi	a10, a2, 18
 443:	1512b2        	l16ui	a11, a2, 42
 446:	0008e0        	callx8	a8
 449:	161292        	l16ui	a9, a2, 44
 44c:	0a19a7        	beq	a9, a10, 45a <sbc_frame_decode+0xbe>
 44f:	fefb81        	l32r	a8, 3c (5b0 <sbc_zero_sb_samples>)
 452:	02ad           	mov.n	a10, a2
 454:	0008e0        	callx8	a8
 457:	0003c6        	j	46a <sbc_frame_decode+0xce>

0000045a <sbc_frame_decode+0xbe>:
 45a:	fef981        	l32r	a8, 40 (5d8 <sbc_bit_allocation>)
 45d:	02ad           	mov.n	a10, a2
 45f:	0008e0        	callx8	a8
 462:	fef881        	l32r	a8, 44 (8a8 <sbc_reconstruction>)
 465:	02ad           	mov.n	a10, a2
 467:	0008e0        	callx8	a8

0000046a <sbc_frame_decode+0xce>:
 46a:	0712a2        	l16ui	a10, a2, 14
 46d:	1b1292        	l16ui	a9, a2, 54
 470:	041a66        	bnei	a10, 1, 478 <sbc_frame_decode+0xdc>
 473:	096d           	mov.n	a6, a9
 475:	000086        	j	47b <sbc_frame_decode+0xdf>

00000478 <sbc_frame_decode+0xdc>:
 478:	416190        	srli	a6, a9, 1

0000047b <sbc_frame_decode+0xdf>:
 47b:	0fd292        	addmi	a9, a2, 0xf00
 47e:	889992        	l16si	a9, a9, 0x110
 481:	20a772        	movi	a7, 0x720
 484:	291966        	bnei	a9, 1, 4b1 <sbc_frame_decode+0x115>
 487:	0612a2        	l16ui	a10, a2, 12
 48a:	05d292        	addmi	a9, a2, 0x500
 48d:	0bd252        	addmi	a5, a2, 0xb00
 490:	531582        	l16ui	a8, a5, 166
 493:	a0c992        	addi	a9, a9, -96
 496:	286592        	s32i	a9, a5, 160
 499:	525582        	s16i	a8, a5, 164
 49c:	a8c552        	addi	a5, a5, -88
 49f:	6c4a66        	bnei	a10, 4, 50f <sbc_frame_decode+0x173>
 4a2:	02cd           	mov.n	a12, a2
 4a4:	fee981        	l32r	a8, 48 (ba0 <SBC_synthesis4_per_channel>)
 4a7:	0a0c           	movi.n	a10, 0
 4a9:	03bd           	mov.n	a11, a3
 4ab:	0008e0        	callx8	a8
 4ae:	001a46        	j	51b <sbc_frame_decode+0x17f>

000004b1 <sbc_frame_decode+0x115>:
 4b1:	2d2966        	bnei	a9, 2, 4e2 <sbc_frame_decode+0x146>
 4b4:	07d292        	addmi	a9, a2, 0x700
 4b7:	0bd252        	addmi	a5, a2, 0xb00
 4ba:	541582        	l16ui	a8, a5, 168
 4bd:	20c992        	addi	a9, a9, 32
 4c0:	286592        	s32i	a9, a5, 160
 4c3:	061292        	l16ui	a9, a2, 12
 4c6:	525582        	s16i	a8, a5, 164
 4c9:	a8c552        	addi	a5, a5, -88
 4cc:	fcc992        	addi	a9, a9, -4
 4cf:	0a6956        	bnez	a9, 579 <sbc_frame_decode+0x1dd>
 4d2:	02cd           	mov.n	a12, a2
 4d4:	fedd81        	l32r	a8, 48 (ba0 <SBC_synthesis4_per_channel>)
 4d7:	1a0c           	movi.n	a10, 1
 4d9:	04bd           	mov.n	a11, a4
 4db:	0008e0        	callx8	a8
 4de:	0028c6        	j	585 <sbc_frame_decode+0x1e9>

000004e1 <sbc_frame_decode+0x145>:
	...

000004e2 <sbc_frame_decode+0x146>:
 4e2:	045956        	bnez	a9, 52b <sbc_frame_decode+0x18f>
 4e5:	05d292        	addmi	a9, a2, 0x500
 4e8:	0bd252        	addmi	a5, a2, 0xb00
 4eb:	531582        	l16ui	a8, a5, 166
 4ee:	a0c992        	addi	a9, a9, -96
 4f1:	286592        	s32i	a9, a5, 160
 4f4:	061292        	l16ui	a9, a2, 12
 4f7:	525582        	s16i	a8, a5, 164
 4fa:	a8c552        	addi	a5, a5, -88
 4fd:	3b4966        	bnei	a9, 4, 53c <sbc_frame_decode+0x1a0>
 500:	03bd           	mov.n	a11, a3
 502:	fed181        	l32r	a8, 48 (ba0 <SBC_synthesis4_per_channel>)
 505:	0a0c           	movi.n	a10, 0
 507:	02cd           	mov.n	a12, a2
 509:	0008e0        	callx8	a8
 50c:	000e06        	j	548 <sbc_frame_decode+0x1ac>

0000050f <sbc_frame_decode+0x173>:
 50f:	02cd           	mov.n	a12, a2
 511:	fece81        	l32r	a8, 4c (cc8 <SBC_synthesis8_per_channel>)
 514:	0a0c           	movi.n	a10, 0
 516:	03bd           	mov.n	a11, a3
 518:	0008e0        	callx8	a8

0000051b <sbc_frame_decode+0x17f>:
 51b:	7e1592        	l16ui	a9, a5, 252
 51e:	7f5592        	s16i	a9, a5, 254
 521:	06a676        	loopgtz	a6, 52b <sbc_frame_decode+0x18f>
 524:	23ad           	ae_l16si.n	a10, a3, 0
 526:	09c4a4        	ae_s16ip	a10, a4, 2
 529:	332b           	addi.n	a3, a3, 2

0000052b <sbc_frame_decode+0x18f>:
 52b:	020c           	movi.n	a2, 0
 52d:	f01d           	retw.n

0000052f <sbc_frame_decode+0x193>:
 52f:	fec181        	l32r	a8, 34 (214 <sbc_extract_scale_factors_joint_ch8_non>)
 532:	02ad           	mov.n	a10, a2
 534:	3b0c           	movi.n	a11, 3
 536:	0008e0        	callx8	a8
 539:	ffc006        	j	43d <sbc_frame_decode+0xa1>

0000053c <sbc_frame_decode+0x1a0>:
 53c:	03bd           	mov.n	a11, a3
 53e:	fec381        	l32r	a8, 4c (cc8 <SBC_synthesis8_per_channel>)
 541:	0a0c           	movi.n	a10, 0
 543:	02cd           	mov.n	a12, a2
 545:	0008e0        	callx8	a8

00000548 <sbc_frame_decode+0x1ac>:
 548:	071292        	l16ui	a9, a2, 14
 54b:	7e15a2        	l16ui	a10, a5, 252
 54e:	7f55a2        	s16i	a10, a5, 254
 551:	d62966        	bnei	a9, 2, 52b <sbc_frame_decode+0x18f>
 554:	8015c2        	l16ui	a12, a5, 0x100
 557:	d27a           	add.n	a13, a2, a7
 559:	0612b2        	l16ui	a11, a2, 12
 55c:	3e65d2        	s32i	a13, a5, 248
 55f:	7e55c2        	s16i	a12, a5, 252
 562:	324b66        	bnei	a11, 4, 598 <sbc_frame_decode+0x1fc>
 565:	02cd           	mov.n	a12, a2
 567:	feb881        	l32r	a8, 48 (ba0 <SBC_synthesis4_per_channel>)
 56a:	04bd           	mov.n	a11, a4
 56c:	1a0c           	movi.n	a10, 1
 56e:	0008e0        	callx8	a8
 571:	000bc6        	j	5a4 <sbc_frame_decode+0x208>

00000574 <sbc_frame_decode+0x1d8>:
 574:	09ad           	mov.n	a10, a9
 576:	ff96c6        	j	3d5 <sbc_frame_decode+0x39>

00000579 <sbc_frame_decode+0x1dd>:
 579:	02cd           	mov.n	a12, a2
 57b:	feb481        	l32r	a8, 4c (cc8 <SBC_synthesis8_per_channel>)
 57e:	1a0c           	movi.n	a10, 1
 580:	04bd           	mov.n	a11, a4
 582:	0008e0        	callx8	a8

00000585 <sbc_frame_decode+0x1e9>:
 585:	7e1592        	l16ui	a9, a5, 252
 588:	805592        	s16i	a9, a5, 0x100
 58b:	06a676        	loopgtz	a6, 595 <sbc_frame_decode+0x1f9>
 58e:	24ad           	ae_l16si.n	a10, a4, 0
 590:	09c3a4        	ae_s16ip	a10, a3, 2
 593:	442b           	addi.n	a4, a4, 2

00000595 <sbc_frame_decode+0x1f9>:
 595:	ffe486        	j	52b <sbc_frame_decode+0x18f>

00000598 <sbc_frame_decode+0x1fc>:
 598:	02cd           	mov.n	a12, a2
 59a:	feac81        	l32r	a8, 4c (cc8 <SBC_synthesis8_per_channel>)
 59d:	04bd           	mov.n	a11, a4
 59f:	1a0c           	movi.n	a10, 1
 5a1:	0008e0        	callx8	a8

000005a4 <sbc_frame_decode+0x208>:
 5a4:	7e1592        	l16ui	a9, a5, 252
 5a7:	805592        	s16i	a9, a5, 0x100
 5aa:	ffdf46        	j	52b <sbc_frame_decode+0x18f>

000005ad <sbc_frame_decode+0x211>:
 5ad:	000000                                        ...

000005b0 <sbc_zero_sb_samples>:
 5b0:	004136        	entry	a1, 32
 5b3:	58c222        	addi	a2, a2, 88
 5b6:	040c           	movi.n	a4, 0
 5b8:	00a132        	movi	a3, 0x100
 5bb:	f03d           	nop.n
 5bd:	02a376        	loopgtz	a3, 5c3 <sbc_zero_sb_samples+0x13>
 5c0:	09e244        	ae_s32ip	a4, a2, 4

000005c3 <sbc_zero_sb_samples+0x13>:
 5c3:	f01d           	retw.n

000005c5 <sbc_zero_sb_samples+0x15>:
 5c5:	000000                                        ...

000005c8 <pseudo_stereo>:
 5c8:	004136        	entry	a1, 32
 5cb:	f01d           	retw.n

000005cd <pseudo_stereo+0x5>:
 5cd:	000000                                        ...

000005d0 <pseudo_stereo_setup>:
 5d0:	004136        	entry	a1, 32
 5d3:	f01d           	retw.n

000005d5 <pseudo_stereo_setup+0x5>:
 5d5:	000000                                        ...

000005d8 <sbc_bit_allocation>:
 5d8:	008136        	entry	a1, 64
 5db:	051232        	l16ui	a3, a2, 10
 5de:	0612e2        	l16ui	a14, a2, 12
 5e1:	041252        	l16ui	a5, a2, 8
 5e4:	1a1282        	l16ui	a8, a2, 52
 5e7:	0e4d           	mov.n	a4, a14
 5e9:	049150        	extui	a9, a5, 1, 1
 5ec:	93e890        	movnez	a14, a8, a9
 5ef:	351326        	beqi	a3, 1, 628 <sbc_bit_allocation+0x50>
 5f2:	fe9791        	l32r	a9, 50 (13c0 <SBC_OFFSET8>)
 5f5:	021272        	l16ui	a7, a2, 4
 5f8:	fe9781        	l32r	a8, 54 (13a0 <SBC_OFFSET4>)
 5fb:	fcc4a2        	addi	a10, a4, -4
 5fe:	c17740        	mul16u	a7, a7, a4
 601:	9389a0        	movnez	a8, a9, a10
 604:	907780        	addx2	a7, a7, a8
 607:	1d1ea6        	blti	a14, 1, 628 <sbc_bit_allocation+0x50>
 60a:	540b           	addi.n	a5, a4, -1
 60c:	00a062        	movi	a6, 0
 60f:	04d242        	addmi	a4, a2, 0x400
 612:	78c442        	addi	a4, a4, 120
 615:	0c9e76        	loopnez	a14, 625 <sbc_bit_allocation+0x4d>
 618:	108560        	and	a8, a5, a6
 61b:	661b           	addi.n	a6, a6, 1
 61d:	908870        	addx2	a8, a8, a7
 620:	288d           	ae_l16si.n	a8, a8, 0
 622:	09c484        	ae_s16ip	a8, a4, 2

00000625 <sbc_bit_allocation+0x4d>:
 625:	041252        	l16ui	a5, a2, 8
 628:	0f0c           	movi.n	a15, 0
 62a:	0b0c           	movi.n	a11, 0
 62c:	0a1c           	movi.n	a10, 16
 62e:	03d292        	addmi	a9, a2, 0x300
 631:	1d0c           	movi.n	a13, 1
 633:	31d9           	s32i.n	a13, a1, 12
 635:	7cc992        	addi	a9, a9, 124
 638:	1199           	s32i.n	a9, a1, 4

0000063a <sbc_bit_allocation+0x62>:
 63a:	450b           	addi.n	a4, a5, -1
 63c:	260c           	movi.n	a6, 2
 63e:	130c           	movi.n	a3, 1
 640:	833640        	moveqz	a3, a6, a4
 643:	022f37        	blt	a15, a3, 649 <sbc_bit_allocation+0x71>
 646:	006386        	j	7d8 <sbc_bit_allocation+0x200>
 649:	061282        	l16ui	a8, a2, 12
 64c:	fe8341        	l32r	a4, 58 (baa <SBC_synthesis4_per_channel+0xa>)
 64f:	828f80        	mull	a8, a15, a8
 652:	0512c2        	l16ui	a12, a2, 10
 655:	908820        	addx2	a8, a8, a2
 658:	38c892        	addi	a9, a8, 56
 65b:	0cd852        	addmi	a5, a8, 0xc00
 65e:	aac552        	addi	a5, a5, -86
 661:	231c66        	bnei	a12, 1, 688 <sbc_bit_allocation+0xb0>
 664:	021ee6        	bgei	a14, 1, 66a <sbc_bit_allocation+0x92>
 667:	008286        	j	875 <sbc_bit_allocation+0x29d>
 66a:	00a072        	movi	a7, 0
 66d:	099e76        	loopnez	a14, 67a <sbc_bit_allocation+0xa2>
 670:	29cd           	ae_l16si.n	a12, a9, 0
 672:	09c5c4        	ae_s16ip	a12, a5, 2
 675:	992b           	addi.n	a9, a9, 2
 677:	537c70        	max	a7, a12, a7

0000067a <sbc_bit_allocation+0xa2>:
 67a:	061282        	l16ui	a8, a2, 12
 67d:	828f80        	mull	a8, a15, a8
 680:	908820        	addx2	a8, a8, a2
 683:	584a           	add.n	a5, a8, a4
 685:	000e86        	j	6c3 <sbc_bit_allocation+0xeb>

00000688 <sbc_bit_allocation+0xb0>:
 688:	351ea6        	blti	a14, 1, 6c1 <sbc_bit_allocation+0xe9>
 68b:	098d           	mov.n	a8, a9
 68d:	070c           	movi.n	a7, 0
 68f:	1198           	l32i.n	a9, a1, 4
 691:	1e9e76        	loopnez	a14, 6b3 <sbc_bit_allocation+0xdb>
 694:	486d           	ae_l16ui.n	a6, a8, 0
 696:	882b           	addi.n	a8, a8, 2
 698:	36cc           	bnez.n	a6, 69f <sbc_bit_allocation+0xc7>
 69a:	b67c           	movi.n	a6, -5
 69c:	0002c6        	j	6ab <sbc_bit_allocation+0xd3>

0000069f <sbc_bit_allocation+0xc7>:
 69f:	7e99c2        	l16si	a12, a9, 252
 6a2:	c066c0        	sub	a6, a6, a12
 6a5:	0216a6        	blti	a6, 1, 6ab <sbc_bit_allocation+0xd3>
 6a8:	216160        	srai	a6, a6, 1
 6ab:	09c564        	ae_s16ip	a6, a5, 2
 6ae:	537670        	max	a7, a6, a7
 6b1:	992b           	addi.n	a9, a9, 2

000006b3 <sbc_bit_allocation+0xdb>:
 6b3:	061282        	l16ui	a8, a2, 12
 6b6:	828f80        	mull	a8, a15, a8
 6b9:	908820        	addx2	a8, a8, a2
 6bc:	584a           	add.n	a5, a8, a4
 6be:	000046        	j	6c3 <sbc_bit_allocation+0xeb>

000006c1 <sbc_bit_allocation+0xe9>:
 6c1:	070c           	movi.n	a7, 0

000006c3 <sbc_bit_allocation+0xeb>:
 6c3:	4159           	s32i.n	a5, a1, 16
 6c5:	771b           	addi.n	a7, a7, 1
 6c7:	0812d2        	l16ui	a13, a2, 16
 6ca:	060c           	movi.n	a6, 0
 6cc:	0189           	s32i.n	a8, a1, 0

000006ce <sbc_bit_allocation+0xf6>:
 6ce:	06cd           	mov.n	a12, a6
 6d0:	221ea6        	blti	a14, 1, 6f6 <sbc_bit_allocation+0x11e>
 6d3:	080c           	movi.n	a8, 0
 6d5:	97fb           	addi.n	a9, a7, 15
 6d7:	4158           	l32i.n	a5, a1, 16
 6d9:	159e76        	loopnez	a14, 6f2 <sbc_bit_allocation+0x11a>
 6dc:	256d           	ae_l16si.n	a6, a5, 0
 6de:	552b           	addi.n	a5, a5, 2
 6e0:	07a767        	bge	a7, a6, 6eb <sbc_bit_allocation+0x113>
 6e3:	04a697        	bge	a6, a9, 6eb <sbc_bit_allocation+0x113>
 6e6:	881b           	addi.n	a8, a8, 1
 6e8:	000106        	j	6f0 <sbc_bit_allocation+0x118>

000006eb <sbc_bit_allocation+0x113>:
 6eb:	019677        	bne	a6, a7, 6f0 <sbc_bit_allocation+0x118>
 6ee:	882b           	addi.n	a8, a8, 2
 6f0:	f03d           	nop.n

000006f2 <sbc_bit_allocation+0x11a>:
 6f2:	000086        	j	6f8 <sbc_bit_allocation+0x120>

000006f5 <sbc_bit_allocation+0x11d>:
	...

000006f6 <sbc_bit_allocation+0x11e>:
 6f6:	080c           	movi.n	a8, 0

000006f8 <sbc_bit_allocation+0x120>:
 6f8:	770b           	addi.n	a7, a7, -1
 6fa:	68ca           	add.n	a6, a8, a12
 6fc:	ce26d7        	blt	a6, a13, 6ce <sbc_bit_allocation+0xf6>
 6ff:	0396d7        	bne	a6, a13, 706 <sbc_bit_allocation+0x12e>
 702:	06cd           	mov.n	a12, a6
 704:	770b           	addi.n	a7, a7, -1

00000706 <sbc_bit_allocation+0x12e>:
 706:	4158           	l32i.n	a5, a1, 16
 708:	0198           	l32i.n	a9, a1, 0
 70a:	fe5481        	l32r	a8, 5c (bea <SBC_synthesis4_per_channel+0x4a>)
 70d:	d72b           	addi.n	a13, a7, 2
 70f:	0cd992        	addmi	a9, a9, 0xc00
 712:	eac992        	addi	a9, a9, -22
 715:	15ae76        	loopgtz	a14, 72e <sbc_bit_allocation+0x156>
 718:	256d           	ae_l16si.n	a6, a5, 0
 71a:	552b           	addi.n	a5, a5, 2
 71c:	04a6d7        	bge	a6, a13, 724 <sbc_bit_allocation+0x14c>
 71f:	69bd           	ae_s16i.n	a11, a9, 0
 721:	0001c6        	j	72c <sbc_bit_allocation+0x154>

00000724 <sbc_bit_allocation+0x14c>:
 724:	c03670        	sub	a3, a6, a7
 727:	6333a0        	minu	a3, a3, a10
 72a:	693d           	ae_s16i.n	a3, a9, 0

0000072c <sbc_bit_allocation+0x154>:
 72c:	992b           	addi.n	a9, a9, 2

0000072e <sbc_bit_allocation+0x156>:
 72e:	041252        	l16ui	a5, a2, 8
 731:	049150        	extui	a9, a5, 1, 1
 734:	095956        	bnez	a9, 7cd <sbc_bit_allocation+0x1f5>
 737:	061262        	l16ui	a6, a2, 12
 73a:	0812d2        	l16ui	a13, a2, 16
 73d:	371b           	addi.n	a3, a7, 1
 73f:	829f60        	mull	a9, a15, a6
 742:	2139           	s32i.n	a3, a1, 8
 744:	909920        	addx2	a9, a9, a2
 747:	598a           	add.n	a5, a9, a8
 749:	0cd992        	addmi	a9, a9, 0xc00
 74c:	aac992        	addi	a9, a9, -86

0000074f <sbc_bit_allocation+0x177>:
 74f:	42acd7        	bge	a12, a13, 795 <sbc_bit_allocation+0x1bd>
 752:	3148           	l32i.n	a4, a1, 12
 754:	824460        	mull	a4, a4, a6
 757:	904420        	addx2	a4, a4, a2
 75a:	448a           	add.n	a4, a4, a8
 75c:	35b547        	bgeu	a5, a4, 795 <sbc_bit_allocation+0x1bd>
 75f:	454d           	ae_l16ui.n	a4, a5, 0
 761:	1124b6        	bltui	a4, 2, 776 <sbc_bit_allocation+0x19e>
 764:	0eb4f6        	bgeui	a4, 16, 776 <sbc_bit_allocation+0x19e>
 767:	cc1b           	addi.n	a12, a12, 1
 769:	841b           	addi.n	a8, a4, 1
 76b:	658d           	ae_s16i.n	a8, a5, 0

0000076d <sbc_bit_allocation+0x195>:
 76d:	061262        	l16ui	a6, a2, 12
 770:	0812d2        	l16ui	a13, a2, 16
 773:	0002c6        	j	782 <sbc_bit_allocation+0x1aa>

00000776 <sbc_bit_allocation+0x19e>:
 776:	2148           	l32i.n	a4, a1, 8
 778:	293d           	ae_l16si.n	a3, a9, 0
 77a:	8c1b           	addi.n	a8, a12, 1
 77c:	029347        	bne	a3, a4, 782 <sbc_bit_allocation+0x1aa>
 77f:	0928d7        	blt	a8, a13, 78c <sbc_bit_allocation+0x1b4>
 782:	fe3681        	l32r	a8, 5c (bea <SBC_synthesis4_per_channel+0x4a>)
 785:	552b           	addi.n	a5, a5, 2
 787:	992b           	addi.n	a9, a9, 2
 789:	fff086        	j	74f <sbc_bit_allocation+0x177>

0000078c <sbc_bit_allocation+0x1b4>:
 78c:	cc2b           	addi.n	a12, a12, 2
 78e:	2d0c           	movi.n	a13, 2
 790:	65dd           	ae_s16i.n	a13, a5, 0
 792:	fff5c6        	j	76d <sbc_bit_allocation+0x195>

00000795 <sbc_bit_allocation+0x1bd>:
 795:	825f60        	mull	a5, a15, a6
 798:	fe3181        	l32r	a8, 5c (bea <SBC_synthesis4_per_channel+0x4a>)
 79b:	905520        	addx2	a5, a5, a2
 79e:	0cd552        	addmi	a5, a5, 0xc00
 7a1:	eac552        	addi	a5, a5, -22

000007a4 <sbc_bit_allocation+0x1cc>:
 7a4:	22acd7        	bge	a12, a13, 7ca <sbc_bit_allocation+0x1f2>
 7a7:	061232        	l16ui	a3, a2, 12
 7aa:	3198           	l32i.n	a9, a1, 12
 7ac:	829930        	mull	a9, a9, a3
 7af:	909920        	addx2	a9, a9, a2
 7b2:	998a           	add.n	a9, a9, a8
 7b4:	12b597        	bgeu	a5, a9, 7ca <sbc_bit_allocation+0x1f2>
 7b7:	454d           	ae_l16ui.n	a4, a5, 0
 7b9:	08b4f6        	bgeui	a4, 16, 7c5 <sbc_bit_allocation+0x1ed>
 7bc:	cc1b           	addi.n	a12, a12, 1
 7be:	941b           	addi.n	a9, a4, 1
 7c0:	659d           	ae_s16i.n	a9, a5, 0
 7c2:	0812d2        	l16ui	a13, a2, 16

000007c5 <sbc_bit_allocation+0x1ed>:
 7c5:	552b           	addi.n	a5, a5, 2
 7c7:	fff646        	j	7a4 <sbc_bit_allocation+0x1cc>

000007ca <sbc_bit_allocation+0x1f2>:
 7ca:	041252        	l16ui	a5, a2, 8

000007cd <sbc_bit_allocation+0x1f5>:
 7cd:	31d8           	l32i.n	a13, a1, 12
 7cf:	ff1b           	addi.n	a15, a15, 1
 7d1:	dd1b           	addi.n	a13, a13, 1
 7d3:	31d9           	s32i.n	a13, a1, 12
 7d5:	ff9846        	j	63a <sbc_bit_allocation+0x62>

000007d8 <sbc_bit_allocation+0x200>:
 7d8:	04e150        	extui	a14, a5, 1, 1
 7db:	094e16        	beqz	a14, 873 <sbc_bit_allocation+0x29b>
 7de:	f71b           	addi.n	a15, a7, 1
 7e0:	0812d2        	l16ui	a13, a2, 16
 7e3:	070c           	movi.n	a7, 0
 7e5:	21f9           	s32i.n	a15, a1, 8
 7e7:	0f0c           	movi.n	a15, 0

000007e9 <sbc_bit_allocation+0x211>:
 7e9:	4eacd7        	bge	a12, a13, 83b <sbc_bit_allocation+0x263>
 7ec:	061262        	l16ui	a6, a2, 12
 7ef:	48a767        	bge	a7, a6, 83b <sbc_bit_allocation+0x263>
 7f2:	829f60        	mull	a9, a15, a6
 7f5:	979a           	add.n	a9, a7, a9
 7f7:	909920        	addx2	a9, a9, a2
 7fa:	0cd952        	addmi	a5, a9, 0xc00
 7fd:	eac552        	addi	a5, a5, -22
 800:	454d           	ae_l16ui.n	a4, a5, 0
 802:	0cd992        	addmi	a9, a9, 0xc00
 805:	aac992        	addi	a9, a9, -86
 808:	1524b6        	bltui	a4, 2, 821 <sbc_bit_allocation+0x249>
 80b:	12b4f6        	bgeui	a4, 16, 821 <sbc_bit_allocation+0x249>
 80e:	cc1b           	addi.n	a12, a12, 1
 810:	a41b           	addi.n	a10, a4, 1
 812:	65ad           	ae_s16i.n	a10, a5, 0

00000814 <sbc_bit_allocation+0x23c>:
 814:	0812d2        	l16ui	a13, a2, 16

00000817 <sbc_bit_allocation+0x23f>:
 817:	1b1f66        	bnei	a15, 1, 836 <sbc_bit_allocation+0x25e>
 81a:	771b           	addi.n	a7, a7, 1
 81c:	0f0c           	movi.n	a15, 0
 81e:	fff1c6        	j	7e9 <sbc_bit_allocation+0x211>

00000821 <sbc_bit_allocation+0x249>:
 821:	21e8           	l32i.n	a14, a1, 8
 823:	29bd           	ae_l16si.n	a11, a9, 0
 825:	ee9be7        	bne	a11, a14, 817 <sbc_bit_allocation+0x23f>
 828:	3c1b           	addi.n	a3, a12, 1
 82a:	e9a3d7        	bge	a3, a13, 817 <sbc_bit_allocation+0x23f>
 82d:	cc2b           	addi.n	a12, a12, 2
 82f:	280c           	movi.n	a8, 2
 831:	658d           	ae_s16i.n	a8, a5, 0
 833:	fff746        	j	814 <sbc_bit_allocation+0x23c>

00000836 <sbc_bit_allocation+0x25e>:
 836:	1f0c           	movi.n	a15, 1
 838:	ffeb46        	j	7e9 <sbc_bit_allocation+0x211>

0000083b <sbc_bit_allocation+0x263>:
 83b:	070c           	movi.n	a7, 0
 83d:	0f0c           	movi.n	a15, 0
 83f:	0000c6        	j	846 <sbc_bit_allocation+0x26e>

00000842 <sbc_bit_allocation+0x26a>:
 842:	771b           	addi.n	a7, a7, 1
 844:	0f0c           	movi.n	a15, 0

00000846 <sbc_bit_allocation+0x26e>:
 846:	29acd7        	bge	a12, a13, 873 <sbc_bit_allocation+0x29b>
 849:	061262        	l16ui	a6, a2, 12
 84c:	23a767        	bge	a7, a6, 873 <sbc_bit_allocation+0x29b>
 84f:	825f60        	mull	a5, a15, a6
 852:	575a           	add.n	a5, a7, a5
 854:	905520        	addx2	a5, a5, a2
 857:	0cd552        	addmi	a5, a5, 0xc00
 85a:	eac552        	addi	a5, a5, -22
 85d:	454d           	ae_l16ui.n	a4, a5, 0
 85f:	08b4f6        	bgeui	a4, 16, 86b <sbc_bit_allocation+0x293>
 862:	cc1b           	addi.n	a12, a12, 1
 864:	841b           	addi.n	a8, a4, 1
 866:	658d           	ae_s16i.n	a8, a5, 0
 868:	0812d2        	l16ui	a13, a2, 16

0000086b <sbc_bit_allocation+0x293>:
 86b:	d31f26        	beqi	a15, 1, 842 <sbc_bit_allocation+0x26a>
 86e:	1f0c           	movi.n	a15, 1
 870:	fff486        	j	846 <sbc_bit_allocation+0x26e>

00000873 <sbc_bit_allocation+0x29b>:
 873:	f01d           	retw.n

00000875 <sbc_bit_allocation+0x29d>:
 875:	070c           	movi.n	a7, 0
 877:	ff9206        	j	6c3 <sbc_bit_allocation+0xeb>

0000087a <sbc_bit_allocation+0x2a2>:
 87a:	00000000 41360000                                ......

00000880 <SAT_ADD>:
 880:	004136        	entry	a1, 32
 883:	36c334        	ae_cvtq48a32s	aeq3, a3
 886:	368234        	ae_cvtq48a32s	aeq2, a2
 889:	066b04        	ae_addsq56s	aeq1, aeq2, aeq3
 88c:	061834        	ae_roundsq32sym	aeq0, aeq1
 88f:	368204        	ae_trunca32q48	a2, aeq0
 892:	f01d           	retw.n

00000894 <SAT_SUB>:
 894:	004136        	entry	a1, 32
 897:	36c334        	ae_cvtq48a32s	aeq3, a3
 89a:	368234        	ae_cvtq48a32s	aeq2, a2
 89d:	066b14        	ae_subsq56s	aeq1, aeq2, aeq3
 8a0:	061834        	ae_roundsq32sym	aeq0, aeq1
 8a3:	368204        	ae_trunca32q48	a2, aeq0
 8a6:	f01d           	retw.n

000008a8 <sbc_reconstruction>:
 8a8:	006136        	entry	a1, 48
 8ab:	1a1282        	l16ui	a8, a2, 52
 8ae:	48c292        	addi	a9, a2, 72
 8b1:	0cd2c2        	addmi	a12, a2, 0xc00
 8b4:	4d0c           	movi.n	a13, 4
 8b6:	303dd4        	ae_cvtp24a16x2.ll	aep3, a13, a13
 8b9:	c8ccb2        	addi	a11, a12, -56
 8bc:	400984        	ae_lp16f.i	aep0, a9, -16
 8bf:	eacca2        	addi	a10, a12, -22
 8c2:	38c292        	addi	a9, a2, 56
 8c5:	faccc2        	addi	a12, a12, -6
 8c8:	401c84        	ae_lp16f.i	aep1, a12, -16
 8cb:	f03d           	nop.n
 8cd:	0c9876        	loopnez	a8, 8dd <sbc_reconstruction+0x35>
 8d0:	de22ac891f 	{ ae_lp16f.iu	aep0, a9, 2; ae_subsp24s	aep4, aep0, aep1 }
 8d5:	d9712c9a1f 	{ ae_lp16f.iu	aep1, a10, 2; ae_addsp24s	aep2, aep4, aep3 }
 8da:	42ab14        	ae_sp16f.l.iu	aep2, a11, 2

000008dd <sbc_reconstruction+0x35>:
 8dd:	fde0f1        	l32r	a15, 60 (1460 <S_ARRAY>)
 8e0:	2d0c           	movi.n	a13, 2
 8e2:	1b12e2        	l16ui	a14, a2, 54
 8e5:	1a12a2        	l16ui	a10, a2, 52
 8e8:	0cd292        	addmi	a9, a2, 0xc00
 8eb:	02d2c2        	addmi	a12, a2, 0x200
 8ee:	6ccc32        	addi	a3, a12, 108
 8f1:	cac992        	addi	a9, a9, -54
 8f4:	9b2cc2        	l32i	a12, a12, 0x26c
 8f7:	f3f690        	wur.ae_cbegin0	a9
 8fa:	90aa20        	addx2	a10, a10, a2
 8fd:	0cdaa2        	addmi	a10, a10, 0xc00
 900:	cacaa2        	addi	a10, a10, -54
 903:	f3f7a0        	wur.ae_cend0	a10
 906:	fd248542ae 	{ addi	a10, a2, 84; ae_zeroq56	aeq1 }
 90b:	f03d           	nop.n
 90d:	2cae76        	loopgtz	a14, 93d <sbc_reconstruction+0x95>
 910:	1019b2        	l16ui	a11, a9, 32
 913:	fc0c5909df 	{ ae_lp16f.c	aep0, a9, a13; ae_movq56	aeq0, aeq1 }
 918:	0387b4        	ae_lb	a8, a11
 91b:	bb9c           	beqz.n	a11, 93a <sbc_reconstruction+0x92>
 91d:	1c1cb4        	ae_db	a12, a11
 920:	306884        	ae_cvtp24a16x2.ll	aep6, a8, a8
 923:	11ebe0        	slli	a14, a11, 2
 926:	455fe4        	ae_lp16x2f.x	aep5, a15, a14
 929:	0765e4        	ae_subp24	aep6, aep6, aep5
 92c:	02d664        	ae_mulp24s.lh	aeq2, aep5, aep6
 92f:	360e24        	ae_trunca16p24s.l	a14, aep0
 932:	fc24feae0f 	{ ae_slaasq56s	aeq0, aeq2, a14; nop }
 937:	060834        	ae_roundsq32sym	aeq0, aeq0

0000093a <sbc_reconstruction+0x92>:
 93a:	624a14        	ae_sq32f.iu	aeq0, a10, 4

0000093d <sbc_reconstruction+0x95>:
 93d:	8063c2        	s32i	a12, a3, 0x200
 940:	0fd292        	addmi	a9, a2, 0xf00
 943:	889992        	l16si	a9, a9, 0x110
 946:	5aa3b2        	movi	a11, 0x35a
 949:	5ca3c2        	movi	a12, 0x35c
 94c:	6b1966        	bnei	a9, 1, 9bb <sbc_reconstruction+0x113>
 94f:	1712a2        	l16ui	a10, a2, 46
 952:	1aba16        	beqz	a10, b01 <sbc_reconstruction+0x259>
 955:	1b12a2        	l16ui	a10, a2, 54
 958:	7f23e2        	l32i	a14, a3, 0x1fc
 95b:	2f0c           	movi.n	a15, 2
 95d:	ffdad2        	addmi	a13, a10, 0xffffff00
 960:	c0ee20        	sub	a14, a14, a2
 963:	fcdee2        	addmi	a14, a14, 0xfffffc00
 966:	a8cee2        	addi	a14, a14, -88
 969:	d2eef0        	quos	a14, a14, a15
 96c:	11e9           	s32i.n	a14, a1, 4
 96e:	191d56        	bnez	a13, b03 <sbc_reconstruction+0x25b>
 971:	0612f2        	l16ui	a15, a2, 12
 974:	f8cff2        	addi	a15, a15, -8
 977:	188f56        	bnez	a15, b03 <sbc_reconstruction+0x25b>
 97a:	060c           	movi.n	a6, 0
 97c:	8e0b           	addi.n	a8, a14, -1
 97e:	03d272        	addmi	a7, a2, 0x300
 981:	5ac772        	addi	a7, a7, 90
 984:	908820        	addx2	a8, a8, a2
 987:	03d882        	addmi	a8, a8, 0x300
 98a:	5cc882        	addi	a8, a8, 92
 98d:	0189           	s32i.n	a8, a1, 0

0000098f <sbc_reconstruction+0xe7>:
 98f:	1198           	l32i.n	a9, a1, 4
 991:	0138           	l32i.n	a3, a1, 0
 993:	1c19a6        	blti	a9, 1, 9b3 <sbc_reconstruction+0x10b>

00000996 <sbc_reconstruction+0xee>:
 996:	7e1342        	l16ui	a4, a3, 252
 999:	fdb281        	l32r	a8, 64 (880 <SAT_ADD>)
 99c:	464a           	add.n	a4, a6, a4
 99e:	a04420        	addx4	a4, a4, a2
 9a1:	1624a2        	l32i	a10, a4, 88
 9a4:	1e24b2        	l32i	a11, a4, 120
 9a7:	0008e0        	callx8	a8
 9aa:	1664a2        	s32i	a10, a4, 88
 9ad:	fec332        	addi	a3, a3, -2
 9b0:	e29377        	bne	a3, a7, 996 <sbc_reconstruction+0xee>

000009b3 <sbc_reconstruction+0x10b>:
 9b3:	10c662        	addi	a6, a6, 16
 9b6:	d5f6a6        	blti	a6, 0x100, 98f <sbc_reconstruction+0xe7>
 9b9:	f01d           	retw.n

000009bb <sbc_reconstruction+0x113>:
 9bb:	6b2966        	bnei	a9, 2, a2a <sbc_reconstruction+0x182>
 9be:	171282        	l16ui	a8, a2, 46
 9c1:	13c816        	beqz	a8, b01 <sbc_reconstruction+0x259>
 9c4:	1b12a2        	l16ui	a10, a2, 54
 9c7:	7f23d2        	l32i	a13, a3, 0x1fc
 9ca:	2e0c           	movi.n	a14, 2
 9cc:	ffda92        	addmi	a9, a10, 0xffffff00
 9cf:	c0dd20        	sub	a13, a13, a2
 9d2:	fcddd2        	addmi	a13, a13, 0xfffffc00
 9d5:	a8cdd2        	addi	a13, a13, -88
 9d8:	d2dde0        	quos	a13, a13, a14
 9db:	11d9           	s32i.n	a13, a1, 4
 9dd:	16e956        	bnez	a9, b4f <sbc_reconstruction+0x2a7>
 9e0:	0612e2        	l16ui	a14, a2, 12
 9e3:	f8cee2        	addi	a14, a14, -8
 9e6:	165e56        	bnez	a14, b4f <sbc_reconstruction+0x2a7>
 9e9:	060c           	movi.n	a6, 0
 9eb:	fd0b           	addi.n	a15, a13, -1
 9ed:	03d272        	addmi	a7, a2, 0x300
 9f0:	5ac772        	addi	a7, a7, 90
 9f3:	90ff20        	addx2	a15, a15, a2
 9f6:	03dff2        	addmi	a15, a15, 0x300
 9f9:	5ccff2        	addi	a15, a15, 92
 9fc:	01f9           	s32i.n	a15, a1, 0

000009fe <sbc_reconstruction+0x156>:
 9fe:	1188           	l32i.n	a8, a1, 4
 a00:	0138           	l32i.n	a3, a1, 0
 a02:	1c18a6        	blti	a8, 1, a22 <sbc_reconstruction+0x17a>

00000a05 <sbc_reconstruction+0x15d>:
 a05:	7e1342        	l16ui	a4, a3, 252
 a08:	fd9881        	l32r	a8, 68 (894 <SAT_SUB>)
 a0b:	464a           	add.n	a4, a6, a4
 a0d:	a04420        	addx4	a4, a4, a2
 a10:	1624a2        	l32i	a10, a4, 88
 a13:	1e24b2        	l32i	a11, a4, 120
 a16:	0008e0        	callx8	a8
 a19:	1e64a2        	s32i	a10, a4, 120
 a1c:	fec332        	addi	a3, a3, -2
 a1f:	e29377        	bne	a3, a7, a05 <sbc_reconstruction+0x15d>

00000a22 <sbc_reconstruction+0x17a>:
 a22:	10c662        	addi	a6, a6, 16
 a25:	d5f6a6        	blti	a6, 0x100, 9fe <sbc_reconstruction+0x156>
 a28:	f01d           	retw.n

00000a2a <sbc_reconstruction+0x182>:
 a2a:	0d3956        	bnez	a9, b01 <sbc_reconstruction+0x259>
 a2d:	171282        	l16ui	a8, a2, 46
 a30:	0cd816        	beqz	a8, b01 <sbc_reconstruction+0x259>
 a33:	7f2392        	l32i	a9, a3, 0x1fc
 a36:	1b12a2        	l16ui	a10, a2, 54
 a39:	2d0c           	movi.n	a13, 2
 a3b:	c09920        	sub	a9, a9, a2
 a3e:	fcd992        	addmi	a9, a9, 0xfffffc00
 a41:	a8c992        	addi	a9, a9, -88
 a44:	d299d0        	quos	a9, a9, a13
 a47:	1199           	s32i.n	a9, a1, 4
 a49:	57fa66        	bnei	a10, 0x100, aa4 <sbc_reconstruction+0x1fc>
 a4c:	0612d2        	l16ui	a13, a2, 12
 a4f:	518d66        	bnei	a13, 8, aa4 <sbc_reconstruction+0x1fc>
 a52:	060c           	movi.n	a6, 0
 a54:	e90b           	addi.n	a14, a9, -1
 a56:	03d272        	addmi	a7, a2, 0x300
 a59:	5ac772        	addi	a7, a7, 90
 a5c:	90ee20        	addx2	a14, a14, a2
 a5f:	03dee2        	addmi	a14, a14, 0x300
 a62:	5ccee2        	addi	a14, a14, 92
 a65:	01e9           	s32i.n	a14, a1, 0

00000a67 <sbc_reconstruction+0x1bf>:
 a67:	11f8           	l32i.n	a15, a1, 4
 a69:	0138           	l32i.n	a3, a1, 0
 a6b:	2d1fa6        	blti	a15, 1, a9c <sbc_reconstruction+0x1f4>

00000a6e <sbc_reconstruction+0x1c6>:
 a6e:	7e1342        	l16ui	a4, a3, 252
 a71:	fd7c81        	l32r	a8, 64 (880 <SAT_ADD>)
 a74:	464a           	add.n	a4, a6, a4
 a76:	a04420        	addx4	a4, a4, a2
 a79:	1624a2        	l32i	a10, a4, 88
 a7c:	1e24b2        	l32i	a11, a4, 120
 a7f:	0008e0        	callx8	a8
 a82:	0a5d           	mov.n	a5, a10
 a84:	fd7981        	l32r	a8, 68 (894 <SAT_SUB>)
 a87:	1e24b2        	l32i	a11, a4, 120
 a8a:	1624a2        	l32i	a10, a4, 88
 a8d:	0008e0        	callx8	a8
 a90:	166452        	s32i	a5, a4, 88
 a93:	1e64a2        	s32i	a10, a4, 120
 a96:	fec332        	addi	a3, a3, -2
 a99:	d19377        	bne	a3, a7, a6e <sbc_reconstruction+0x1c6>

00000a9c <sbc_reconstruction+0x1f4>:
 a9c:	10c662        	addi	a6, a6, 16
 a9f:	c4f6a6        	blti	a6, 0x100, a67 <sbc_reconstruction+0x1bf>
 aa2:	f01d           	retw.n

00000aa4 <sbc_reconstruction+0x1fc>:
 aa4:	059a16        	beqz	a10, b01 <sbc_reconstruction+0x259>
 aa7:	1198           	l32i.n	a9, a1, 4
 aa9:	72ba           	add.n	a7, a2, a11
 aab:	060c           	movi.n	a6, 0
 aad:	990b           	addi.n	a9, a9, -1
 aaf:	909920        	addx2	a9, a9, a2
 ab2:	99ca           	add.n	a9, a9, a12
 ab4:	0199           	s32i.n	a9, a1, 0

00000ab6 <sbc_reconstruction+0x20e>:
 ab6:	11b8           	l32i.n	a11, a1, 4
 ab8:	0138           	l32i.n	a3, a1, 0
 aba:	3a1ba6        	blti	a11, 1, af8 <sbc_reconstruction+0x250>

00000abd <sbc_reconstruction+0x215>:
 abd:	7e1342        	l16ui	a4, a3, 252
 ac0:	061252        	l16ui	a5, a2, 12
 ac3:	fd6881        	l32r	a8, 64 (880 <SAT_ADD>)
 ac6:	464a           	add.n	a4, a6, a4
 ac8:	554a           	add.n	a5, a5, a4
 aca:	a05520        	addx4	a5, a5, a2
 acd:	a04420        	addx4	a4, a4, a2
 ad0:	1624a2        	l32i	a10, a4, 88
 ad3:	1625b2        	l32i	a11, a5, 88
 ad6:	0008e0        	callx8	a8
 ad9:	21a9           	s32i.n	a10, a1, 8
 adb:	fd6381        	l32r	a8, 68 (894 <SAT_SUB>)
 ade:	1625b2        	l32i	a11, a5, 88
 ae1:	1624a2        	l32i	a10, a4, 88
 ae4:	0008e0        	callx8	a8
 ae7:	fec332        	addi	a3, a3, -2
 aea:	2198           	l32i.n	a9, a1, 8
 aec:	166492        	s32i	a9, a4, 88
 aef:	1665a2        	s32i	a10, a5, 88
 af2:	c79377        	bne	a3, a7, abd <sbc_reconstruction+0x215>
 af5:	1b12a2        	l16ui	a10, a2, 54

00000af8 <sbc_reconstruction+0x250>:
 af8:	0612b2        	l16ui	a11, a2, 12
 afb:	906b60        	addx2	a6, a11, a6
 afe:	b426a7        	blt	a6, a10, ab6 <sbc_reconstruction+0x20e>

00000b01 <sbc_reconstruction+0x259>:
 b01:	f01d           	retw.n

00000b03 <sbc_reconstruction+0x25b>:
 b03:	fa1aa6        	blti	a10, 1, b01 <sbc_reconstruction+0x259>
 b06:	11d8           	l32i.n	a13, a1, 4
 b08:	72ba           	add.n	a7, a2, a11
 b0a:	060c           	movi.n	a6, 0
 b0c:	dd0b           	addi.n	a13, a13, -1
 b0e:	90dd20        	addx2	a13, a13, a2
 b11:	ddca           	add.n	a13, a13, a12
 b13:	01d9           	s32i.n	a13, a1, 0

00000b15 <sbc_reconstruction+0x26d>:
 b15:	11e8           	l32i.n	a14, a1, 4
 b17:	0138           	l32i.n	a3, a1, 0
 b19:	271ea6        	blti	a14, 1, b44 <sbc_reconstruction+0x29c>

00000b1c <sbc_reconstruction+0x274>:
 b1c:	7e1342        	l16ui	a4, a3, 252
 b1f:	0612b2        	l16ui	a11, a2, 12
 b22:	fd5081        	l32r	a8, 64 (880 <SAT_ADD>)
 b25:	464a           	add.n	a4, a6, a4
 b27:	bb4a           	add.n	a11, a11, a4
 b29:	a0bb20        	addx4	a11, a11, a2
 b2c:	a04420        	addx4	a4, a4, a2
 b2f:	1624a2        	l32i	a10, a4, 88
 b32:	162bb2        	l32i	a11, a11, 88
 b35:	0008e0        	callx8	a8
 b38:	1664a2        	s32i	a10, a4, 88
 b3b:	fec332        	addi	a3, a3, -2
 b3e:	da9377        	bne	a3, a7, b1c <sbc_reconstruction+0x274>
 b41:	1b12a2        	l16ui	a10, a2, 54

00000b44 <sbc_reconstruction+0x29c>:
 b44:	061282        	l16ui	a8, a2, 12
 b47:	906860        	addx2	a6, a8, a6
 b4a:	c726a7        	blt	a6, a10, b15 <sbc_reconstruction+0x26d>
 b4d:	f01d           	retw.n

00000b4f <sbc_reconstruction+0x2a7>:
 b4f:	faea16        	beqz	a10, b01 <sbc_reconstruction+0x259>
 b52:	1198           	l32i.n	a9, a1, 4
 b54:	72ba           	add.n	a7, a2, a11
 b56:	060c           	movi.n	a6, 0
 b58:	990b           	addi.n	a9, a9, -1
 b5a:	909920        	addx2	a9, a9, a2
 b5d:	99ca           	add.n	a9, a9, a12
 b5f:	0199           	s32i.n	a9, a1, 0

00000b61 <sbc_reconstruction+0x2b9>:
 b61:	11b8           	l32i.n	a11, a1, 4
 b63:	0138           	l32i.n	a3, a1, 0
 b65:	271ba6        	blti	a11, 1, b90 <sbc_reconstruction+0x2e8>

00000b68 <sbc_reconstruction+0x2c0>:
 b68:	7e13a2        	l16ui	a10, a3, 252
 b6b:	061252        	l16ui	a5, a2, 12
 b6e:	fd3e81        	l32r	a8, 68 (894 <SAT_SUB>)
 b71:	a6aa           	add.n	a10, a6, a10
 b73:	55aa           	add.n	a5, a5, a10
 b75:	a05520        	addx4	a5, a5, a2
 b78:	a0aa20        	addx4	a10, a10, a2
 b7b:	162aa2        	l32i	a10, a10, 88
 b7e:	1625b2        	l32i	a11, a5, 88
 b81:	0008e0        	callx8	a8
 b84:	1665a2        	s32i	a10, a5, 88
 b87:	fec332        	addi	a3, a3, -2
 b8a:	da9377        	bne	a3, a7, b68 <sbc_reconstruction+0x2c0>
 b8d:	1b12a2        	l16ui	a10, a2, 54

00000b90 <sbc_reconstruction+0x2e8>:
 b90:	0612b2        	l16ui	a11, a2, 12
 b93:	906b60        	addx2	a6, a11, a6
 b96:	c726a7        	blt	a6, a10, b61 <sbc_reconstruction+0x2b9>
 b99:	f01d           	retw.n

00000b9b <sbc_reconstruction+0x2f3>:
 b9b:	00000000 00613600                                .....

00000ba0 <SBC_synthesis4_per_channel>:
 ba0:	006136        	entry	a1, 48
 ba3:	fec372        	addi	a7, a3, -2
 ba6:	031452        	l16ui	a5, a4, 6
 ba9:	4a0c           	movi.n	a10, 4
 bab:	932a20        	movnez	a2, a10, a2
 bae:	a09240        	addx4	a9, a2, a4
 bb1:	58c992        	addi	a9, a9, 88
 bb4:	0215e6        	bgei	a5, 1, bba <SBC_synthesis4_per_channel+0x1a>
 bb7:	004206        	j	cc3 <SBC_synthesis4_per_channel+0x123>
 bba:	0b0c           	movi.n	a11, 0
 bbc:	fd2dc1        	l32r	a12, 70 (1600 <SBC_cos_coef_8+0x90>)
 bbf:	030c           	movi.n	a3, 0
 bc1:	880c           	movi.n	a8, 8
 bc3:	fd2ae1        	l32r	a14, 6c (5a00 <_end+0x3584>)
 bc6:	fd2bf1        	l32r	a15, 74 (14f0 <SBC_cos_coef_4+0x40>)
 bc9:	0bd4d2        	addmi	a13, a4, 0xb00
 bcc:	a4cdd2        	addi	a13, a13, -92
 bcf:	01f9           	s32i.n	a15, a1, 0
 bd1:	303ee4        	ae_cvtp24a16x2.ll	aep3, a14, a14
 bd4:	fd248001ef 	{ movi	a14, 1; ae_zeroq56	aeq1 }

00000bd9 <SBC_synthesis4_per_channel+0x39>:
 bd9:	f3f690        	wur.ae_cbegin0	a9
 bdc:	3f2da2        	l32i	a10, a13, 252
 bdf:	10c962        	addi	a6, a9, 16
 be2:	801df2        	l16ui	a15, a13, 0x100
 be5:	f3f760        	wur.ae_cend0	a6
 be8:	096d           	mov.n	a6, a9
 bea:	f7cf52        	addi	a5, a15, -9
 bed:	2f1b           	addi.n	a2, a15, 1
 bef:	832350        	moveqz	a2, a3, a5
 bf2:	805d22        	s16i	a2, a13, 0x100
 bf5:	850c           	movi.n	a5, 8
 bf7:	a02fa0        	addx4	a2, a15, a10
 bfa:	0a6f07        	bbci	a15, 0, c08 <SBC_synthesis4_per_channel+0x68>
 bfd:	0138           	l32i.n	a3, a1, 0
 bff:	01daa2        	addmi	a10, a10, 0x100
 c02:	a0caa2        	addi	a10, a10, -96
 c05:	000086        	j	c0b <SBC_synthesis4_per_channel+0x6b>

00000c08 <SBC_synthesis4_per_channel+0x68>:
 c08:	fd1c31        	l32r	a3, 78 (14b0 <SBC_cos_coef_4>)

00000c0b <SBC_synthesis4_per_channel+0x6b>:
 c0b:	f8c332        	addi	a3, a3, -8
 c0e:	1da576        	loopgtz	a5, c2f <SBC_synthesis4_per_channel+0x8f>
 c11:	fc24ddc68f 	{ ae_lp24x2f.c	aep4, a6, a8; nop }
 c16:	5cd314        	ae_lp24x2f.iu	aep5, a3, 8
 c19:	fc0c6fa31f 	{ ae_lp24x2f.iu	aep2, a3, 8; ae_movq56	aeq0, aeq1 }
 c1e:	e2b01d968f 	{ ae_lp24x2f.c	aep1, a6, a8; ae_mulaafp24s.hh.ll	aeq0, aep4, aep5 }
 c23:	051204        	ae_mulaafp24s.hh.ll	aeq0, aep1, aep2
 c26:	1b8c04        	ae_roundsp24q48sym	aep0, aeq0
 c29:	520204        	ae_sp24f.l.i	aep0, a2, 0
 c2c:	28c222        	addi	a2, a2, 40

00000c2f <SBC_synthesis4_per_channel+0x8f>:
 c2f:	4151f0        	srli	a5, a15, 1
 c32:	115580        	slli	a5, a5, 8
 c35:	37e5c4        	ae_truncp24a32x2	aep6, a5, a12
 c38:	ebd88f8a2e 	{ addi	a2, a10, -8; ae_mulfp24s.hl	aeq2, aep6, aep6 }
 c3d:	fd0f61        	l32r	a6, 7c (1870 <SBC_proto_coef_4>)
 c40:	35a414        	ae_slliq56	aeq2, aeq2, 16
 c43:	36a504        	ae_trunca32q48	a5, aeq2
 c46:	303fe0        	xor	a3, a15, a14
 c49:	043030        	extui	a3, a3, 0, 1
 c4c:	a05560        	addx4	a5, a5, a6
 c4f:	a03350        	addx4	a3, a3, a5
 c52:	460c           	movi.n	a6, 4
 c54:	fcc3a2        	addi	a10, a3, -4
 c57:	030c           	movi.n	a3, 0
 c59:	55a676        	loopgtz	a6, cb2 <SBC_synthesis4_per_channel+0x112>
 c5c:	50fa14        	ae_lp24f.iu	aep7, a10, 4
 c5f:	5ce214        	ae_lp24x2f.iu	aep6, a2, 8
 c62:	5cc214        	ae_lp24x2f.iu	aep4, a2, 8
 c65:	509a14        	ae_lp24f.iu	aep1, a10, 4
 c68:	fe0c6eda1f 	{ ae_lp24f.iu	aep5, a10, 4; ae_movq56	aeq2, aeq1 }
 c6d:	df3dee8a1f 	{ ae_lp24f.iu	aep0, a10, 4; ae_selp24.lh	aep7, aep7, aep1 }
 c72:	e3f82e9a1f 	{ ae_lp24f.iu	aep1, a10, 4; ae_mulaafp24s.hh.ll	aeq2, aep6, aep7 }

00000c77 <SBC_synthesis4_per_channel+0xd7>:
 c77:	de15ef821f 	{ ae_lp24x2f.iu	aep0, a2, 8; ae_selp24.lh	aep5, aep5, aep0 }
 c7c:	e3b02fd21f 	{ ae_lp24x2f.iu	aep5, a2, 8; ae_mulaafp24s.hh.ll	aeq2, aep4, aep5 }
 c81:	50aa14        	ae_lp24f.iu	aep2, a10, 4
 c84:	dc45eeea1f 	{ ae_lp24f.iu	aep6, a10, 4; ae_selp24.lh	aep1, aep1, aep2 }
 c89:	e3202f921f 	{ ae_lp24x2f.iu	aep1, a2, 8; ae_mulaafp24s.hh.ll	aeq2, aep0, aep1 }
 c8e:	50fa14        	ae_lp24f.iu	aep7, a10, 4
 c91:	50aa14        	ae_lp24f.iu	aep2, a10, 4
 c94:	dff9aeca1f 	{ ae_lp24f.iu	aep4, a10, 4; ae_selp24.lh	aep6, aep6, aep7 }
 c99:	e3d43e24af 	{ ae_selp24.lh	aep2, aep2, aep4; ae_mulaafp24s.hh.ll	aeq2, aep5, aep6 }
 c9e:	059204        	ae_mulaafp24s.hh.ll	aeq2, aep1, aep2
 ca1:	062834        	ae_roundsq32sym	aeq0, aeq2
 ca4:	033314        	ae_mulq32sp16s.l	aeq0, aeq0, aep3
 ca7:	36c094        	ae_sllisq56s	aeq3, aeq0, 2
 caa:	fbfbee8a1f 	{ ae_lp24f.iu	aep0, a10, 4; ae_roundsp16q48sym	aep7, aeq3 }
 caf:	42f714        	ae_sp16f.l.iu	aep7, a7, 2

00000cb2 <SBC_synthesis4_per_channel+0x112>:
 cb2:	1a14a2        	l16ui	a10, a4, 52
 cb5:	031462        	l16ui	a6, a4, 6
 cb8:	bb1b           	addi.n	a11, a11, 1
 cba:	a09a90        	addx4	a9, a10, a9
 cbd:	02ab67        	bge	a11, a6, cc3 <SBC_synthesis4_per_channel+0x123>
 cc0:	ffc546        	j	bd9 <SBC_synthesis4_per_channel+0x39>

00000cc3 <SBC_synthesis4_per_channel+0x123>:
 cc3:	f01d           	retw.n

00000cc5 <SBC_synthesis4_per_channel+0x125>:
 cc5:	000000                                        ...

00000cc8 <SBC_synthesis8_per_channel>:
 cc8:	006136        	entry	a1, 48
 ccb:	fec372        	addi	a7, a3, -2
 cce:	0bd4e2        	addmi	a14, a4, 0xb00
 cd1:	0f0c           	movi.n	a15, 0
 cd3:	890c           	movi.n	a9, 8
 cd5:	882c           	movi.n	a8, 40
 cd7:	58c4c2        	addi	a12, a4, 88
 cda:	78c4a2        	addi	a10, a4, 120
 cdd:	fce851        	l32r	a5, 80 (1670 <SBC_cos_coef_8+0x100>)
 ce0:	fce3d1        	l32r	a13, 6c (5a00 <_end+0x3584>)
 ce3:	305dd4        	ae_cvtp24a16x2.ll	aep5, a13, a13
 ce6:	0159           	s32i.n	a5, a1, 0
 ce8:	83ac20        	moveqz	a10, a12, a2
 ceb:	0314c2        	l16ui	a12, a4, 6
 cee:	0a6d           	mov.n	a6, a10
 cf0:	20ca22        	addi	a2, a10, 32

00000cf3 <SBC_synthesis8_per_channel+0x2b>:
 cf3:	f3f6a0        	wur.ae_cbegin0	a10
 cf6:	521ed2        	l16ui	a13, a14, 164
 cf9:	f3f720        	wur.ae_cend0	a2
 cfc:	282eb2        	l32i	a11, a14, 160
 cff:	f7cd52        	addi	a5, a13, -9
 d02:	dd1b           	addi.n	a13, a13, 1
 d04:	83df50        	moveqz	a13, a15, a5
 d07:	525ed2        	s16i	a13, a14, 164
 d0a:	051c           	movi.n	a5, 16
 d0c:	a02db0        	addx4	a2, a13, a11
 d0f:	d8c222        	addi	a2, a2, -40
 d12:	0a6d07        	bbci	a13, 0, d20 <SBC_synthesis8_per_channel+0x58>
 d15:	0138           	l32i.n	a3, a1, 0
 d17:	01dbb2        	addmi	a11, a11, 0x100
 d1a:	40cbb2        	addi	a11, a11, 64
 d1d:	000086        	j	d23 <SBC_synthesis8_per_channel+0x5b>

00000d20 <SBC_synthesis8_per_channel+0x58>:
 d20:	fcd931        	l32r	a3, 84 (1570 <SBC_cos_coef_8>)

00000d23 <SBC_synthesis8_per_channel+0x5b>:
 d23:	f8c332        	addi	a3, a3, -8
 d26:	2ca576        	loopgtz	a5, d56 <SBC_synthesis8_per_channel+0x8e>
 d29:	fc24dd869f 	{ ae_lp24x2f.c	aep0, a6, a9; nop }
 d2e:	5c9314        	ae_lp24x2f.iu	aep1, a3, 8
 d31:	5cf314        	ae_lp24x2f.iu	aep7, a3, 8
 d34:	fc24dde69f 	{ ae_lp24x2f.c	aep6, a6, a9; nop }
 d39:	fc24ddb69f 	{ ae_lp24x2f.c	aep3, a6, a9; nop }
 d3e:	f022afc31f 	{ ae_lp24x2f.iu	aep4, a3, 8; ae_mulzaafp24s.hh.ll	aeq0, aep0, aep1 }
 d43:	e2f82fa31f 	{ ae_lp24x2f.iu	aep2, a3, 8; ae_mulaafp24s.hh.ll	aeq0, aep6, aep7 }
 d48:	e28c1d969f 	{ ae_lp24x2f.c	aep1, a6, a9; ae_mulaafp24s.hh.ll	aeq0, aep3, aep4 }
 d4d:	051204        	ae_mulaafp24s.hh.ll	aeq0, aep1, aep2
 d50:	1b8c04        	ae_roundsp24q48sym	aep0, aeq0
 d53:	538284        	ae_sp24f.l.xu	aep0, a2, a8

00000d56 <SBC_synthesis8_per_channel+0x8e>:
 d56:	60a132        	movi	a3, 0x160
 d59:	2121d0        	srai	a2, a13, 1
 d5c:	160c           	movi.n	a6, 1
 d5e:	822230        	mull	a2, a2, a3
 d61:	fcc931        	l32r	a3, 88 (1be0 <SBC_proto_coef_8>)
 d64:	305d60        	xor	a5, a13, a6
 d67:	045050        	extui	a5, a5, 0, 1
 d6a:	802320        	add	a2, a3, a2
 d6d:	a05520        	addx4	a5, a5, a2
 d70:	f8cb22        	addi	a2, a11, -8
 d73:	fcc5b2        	addi	a11, a5, -4
 d76:	08a052        	movi	a5, 8
 d79:	53a576        	loopgtz	a5, dd0 <SBC_synthesis8_per_channel+0x108>
 d7c:	5ca214        	ae_lp24x2f.iu	aep2, a2, 8
 d7f:	50cb14        	ae_lp24f.iu	aep4, a11, 4
 d82:	50eb14        	ae_lp24f.iu	aep6, a11, 4
 d85:	5cf214        	ae_lp24x2f.iu	aep7, a2, 8
 d88:	5cb214        	ae_lp24x2f.iu	aep3, a2, 8
 d8b:	ded1ae8b1f 	{ ae_lp24f.iu	aep0, a11, 4; ae_selp24.lh	aep4, aep4, aep6 }
 d90:	f18aee9b1f 	{ ae_lp24f.iu	aep1, a11, 4; ae_mulzaafp24s.hh.ll	aeq3, aep2, aep4 }
 d95:	dc21aecb1f 	{ ae_lp24f.iu	aep4, a11, 4; ae_selp24.lh	aep0, aep0, aep1 }

00000d9a <SBC_synthesis8_per_channel+0xd2>:
 d9a:	e31c6f821f 	{ ae_lp24x2f.iu	aep0, a2, 8; ae_mulaafp24s.hh.ll	aeq3, aep7, aep0 }
 d9f:	50eb14        	ae_lp24f.iu	aep6, a11, 4
 da2:	ded1ae9b1f 	{ ae_lp24f.iu	aep1, a11, 4; ae_selp24.lh	aep4, aep4, aep6 }
 da7:	e38c6fc21f 	{ ae_lp24x2f.iu	aep4, a2, 8; ae_mulaafp24s.hh.ll	aeq3, aep3, aep4 }
 dac:	50ab14        	ae_lp24f.iu	aep2, a11, 4
 daf:	50eb14        	ae_lp24f.iu	aep6, a11, 4
 db2:	dc45eefb1f 	{ ae_lp24f.iu	aep7, a11, 4; ae_selp24.lh	aep1, aep1, aep2 }
 db7:	e3207e6faf 	{ ae_selp24.lh	aep6, aep6, aep7; ae_mulaafp24s.hh.ll	aeq3, aep0, aep1 }
 dbc:	05ce04        	ae_mulaafp24s.hh.ll	aeq3, aep4, aep6
 dbf:	06b834        	ae_roundsq32sym	aeq2, aeq3
 dc2:	03d394        	ae_mulq32sp16s.l	aeq2, aeq2, aep5
 dc5:	366094        	ae_sllisq56s	aeq1, aeq2, 2
 dc8:	f9ebaebb1f 	{ ae_lp24f.iu	aep3, a11, 4; ae_roundsp16q48sym	aep2, aeq1 }
 dcd:	42a714        	ae_sp16f.l.iu	aep2, a7, 2

00000dd0 <SBC_synthesis8_per_channel+0x108>:
 dd0:	1a1452        	l16ui	a5, a4, 52
 dd3:	cc0b           	addi.n	a12, a12, -1
 dd5:	a0a5a0        	addx4	a10, a5, a10
 dd8:	0a6d           	mov.n	a6, a10
 dda:	20ca22        	addi	a2, a10, 32
 ddd:	021ca6        	blti	a12, 1, de3 <SBC_synthesis8_per_channel+0x11b>
 de0:	ffc3c6        	j	cf3 <SBC_synthesis8_per_channel+0x2b>
 de3:	f01d           	retw.n

00000de5 <SBC_synthesis8_per_channel+0x11d>:
 de5:	000000                                        ...

00000de8 <_start>:
 de8:	004136        	entry	a1, 32
 deb:	fca9a1        	l32r	a10, 90 (23c0 <SBC_crc_table+0x100>)
 dee:	fca731        	l32r	a3, 8c (2478 <printf_ptr>)
 df1:	0288           	l32i.n	a8, a2, 0
 df3:	fca8b1        	l32r	a11, 94 (f88 <msbc_dec_init>)
 df6:	0389           	s32i.n	a8, a3, 0
 df8:	0008e0        	callx8	a8
 dfb:	0388           	l32i.n	a8, a3, 0
 dfd:	fca6a1        	l32r	a10, 98 (23d4 <SBC_crc_table+0x114>)
 e00:	fca7b1        	l32r	a11, 9c (e10 <msbc_dec_process>)
 e03:	0008e0        	callx8	a8
 e06:	fca621        	l32r	a2, a0 (2418 <export_parameter_array>)
 e09:	f01d           	retw.n

00000e0b <_start+0x23>:
 e0b:	00000000 00613600                                .....

00000e10 <msbc_dec_process>:
 e10:	006136        	entry	a1, 48
 e13:	0149           	s32i.n	a4, a1, 0
 e15:	02ad           	mov.n	a10, a2
 e17:	0bd272        	addmi	a7, a2, 0xb00
 e1a:	fc8481        	l32r	a8, 2c (358 <SBC_decode_preload>)
 e1d:	436732        	s32i	a3, a7, 0x10c
 e20:	a4c772        	addi	a7, a7, -92
 e23:	0008e0        	callx8	a8
 e26:	0a0c           	movi.n	a10, 0
 e28:	263c           	movi.n	a6, 50
 e2a:	38c392        	addi	a9, a3, 56
 e2d:	04d2c2        	addmi	a12, a2, 0x400
 e30:	9ccc42        	addi	a4, a12, -100
 e33:	933950        	movnez	a3, a9, a5
 e36:	276c32        	s32i	a3, a12, 156
 e39:	890c           	movi.n	a9, 8
 e3b:	35cc           	bnez.n	a5, e42 <msbc_dec_process+0x32>
 e3d:	72ad           	ae_s16i.n	a10, a2, 2
 e3f:	000346        	j	e50 <msbc_dec_process+0x40>

00000e42 <msbc_dec_process+0x32>:
 e42:	d32b           	addi.n	a13, a3, 2
 e44:	43bd           	ae_l16ui.n	a11, a3, 0
 e46:	276cd2        	s32i	a13, a12, 156
 e49:	729d           	ae_s16i.n	a9, a2, 2
 e4b:	11bb80        	slli	a11, a11, 8
 e4e:	62bd           	ae_s16i.n	a11, a2, 0

00000e50 <msbc_dec_process+0x40>:
 e50:	030c           	movi.n	a3, 0
 e52:	ada052        	movi	a5, 173

00000e55 <msbc_dec_process+0x45>:
 e55:	fc9381        	l32r	a8, a4 (f98 <msbc_get_byte>)
 e58:	02ad           	mov.n	a10, a2
 e5a:	0008e0        	callx8	a8
 e5d:	041a57        	beq	a10, a5, e65 <msbc_dec_process+0x55>
 e60:	331b           	addi.n	a3, a3, 1
 e62:	ef9367        	bne	a3, a6, e55 <msbc_dec_process+0x45>

00000e65 <msbc_dec_process+0x55>:
 e65:	130c           	movi.n	a3, 1
 e67:	78a062        	movi	a6, 120
 e6a:	719a57        	bne	a10, a5, edf <msbc_dec_process+0xcf>
 e6d:	fc8d81        	l32r	a8, a4 (f98 <msbc_get_byte>)
 e70:	02ad           	mov.n	a10, a2
 e72:	0008e0        	callx8	a8
 e75:	fc8b81        	l32r	a8, a4 (f98 <msbc_get_byte>)
 e78:	0952a2        	s16i	a10, a2, 18
 e7b:	02ad           	mov.n	a10, a2
 e7d:	0008e0        	callx8	a8
 e80:	fc8981        	l32r	a8, a4 (f98 <msbc_get_byte>)
 e83:	0a52a2        	s16i	a10, a2, 20
 e86:	02ad           	mov.n	a10, a2
 e88:	0008e0        	callx8	a8
 e8b:	fc6a81        	l32r	a8, 34 (214 <sbc_extract_scale_factors_joint_ch8_non>)
 e8e:	075232        	s16i	a3, a2, 14
 e91:	1652a2        	s16i	a10, a2, 44
 e94:	0b0c           	movi.n	a11, 0
 e96:	890c           	movi.n	a9, 8
 e98:	fc0c           	movi.n	a12, 15
 e9a:	ad1c           	movi.n	a13, 26
 e9c:	0852d2        	s16i	a13, a2, 16
 e9f:	0352c2        	s16i	a12, a2, 6
 ea2:	065292        	s16i	a9, a2, 12
 ea5:	0552b2        	s16i	a11, a2, 10
 ea8:	0452b2        	s16i	a11, a2, 8
 eab:	0252b2        	s16i	a11, a2, 4
 eae:	9a3c           	movi.n	a10, 57
 eb0:	2b0c           	movi.n	a11, 2
 eb2:	7e54a2        	s16i	a10, a4, 252
 eb5:	1a5292        	s16i	a9, a2, 52
 eb8:	1b5262        	s16i	a6, a2, 54
 ebb:	02ad           	mov.n	a10, a2
 ebd:	0008e0        	callx8	a8
 ec0:	fc5e81        	l32r	a8, 38 (164 <sbc_crc8>)
 ec3:	12c2a2        	addi	a10, a2, 18
 ec6:	1512b2        	l16ui	a11, a2, 42
 ec9:	0008e0        	callx8	a8
 ecc:	161292        	l16ui	a9, a2, 44
 ecf:	6519a7        	beq	a9, a10, f38 <msbc_dec_process+0x128>
 ed2:	fc5a81        	l32r	a8, 3c (5b0 <sbc_zero_sb_samples>)
 ed5:	02ad           	mov.n	a10, a2
 ed7:	0008e0        	callx8	a8
 eda:	130c           	movi.n	a3, 1
 edc:	000cc6        	j	f13 <msbc_dec_process+0x103>

00000edf <msbc_dec_process+0xcf>:
 edf:	fc5781        	l32r	a8, 3c (5b0 <sbc_zero_sb_samples>)
 ee2:	02ad           	mov.n	a10, a2
 ee4:	0008e0        	callx8	a8
 ee7:	130c           	movi.n	a3, 1
 ee9:	9b3c           	movi.n	a11, 57
 eeb:	ac1c           	movi.n	a12, 26
 eed:	1d0c           	movi.n	a13, 1
 eef:	fe0c           	movi.n	a14, 15
 ef1:	0f0c           	movi.n	a15, 0
 ef3:	8a0c           	movi.n	a10, 8
 ef5:	0652a2        	s16i	a10, a2, 12
 ef8:	0252f2        	s16i	a15, a2, 4
 efb:	0452f2        	s16i	a15, a2, 8
 efe:	0552f2        	s16i	a15, a2, 10
 f01:	0352e2        	s16i	a14, a2, 6
 f04:	0752d2        	s16i	a13, a2, 14
 f07:	0852c2        	s16i	a12, a2, 16
 f0a:	7e54b2        	s16i	a11, a4, 252
 f0d:	1a52a2        	s16i	a10, a2, 52
 f10:	1b5262        	s16i	a6, a2, 54

00000f13 <msbc_dec_process+0x103>:
 f13:	02cd           	mov.n	a12, a2
 f15:	01b8           	l32i.n	a11, a1, 0
 f17:	fc4d81        	l32r	a8, 4c (cc8 <SBC_synthesis8_per_channel>)
 f1a:	811792        	l16ui	a9, a7, 0x102
 f1d:	05d2a2        	addmi	a10, a2, 0x500
 f20:	a0caa2        	addi	a10, a10, -96
 f23:	805792        	s16i	a9, a7, 0x100
 f26:	3f67a2        	s32i	a10, a7, 252
 f29:	0a0c           	movi.n	a10, 0
 f2b:	0008e0        	callx8	a8
 f2e:	032d           	mov.n	a2, a3
 f30:	8017b2        	l16ui	a11, a7, 0x100
 f33:	8157b2        	s16i	a11, a7, 0x102
 f36:	f01d           	retw.n

00000f38 <msbc_dec_process+0x128>:
 f38:	fc4281        	l32r	a8, 40 (5d8 <sbc_bit_allocation>)
 f3b:	02ad           	mov.n	a10, a2
 f3d:	0008e0        	callx8	a8
 f40:	fc4181        	l32r	a8, 44 (8a8 <sbc_reconstruction>)
 f43:	02ad           	mov.n	a10, a2
 f45:	0008e0        	callx8	a8
 f48:	030c           	movi.n	a3, 0
 f4a:	fff146        	j	f13 <msbc_dec_process+0x103>

00000f4d <msbc_dec_process+0x13d>:
 f4d:	000000                                        ...

00000f50 <msbc_change_endian16>:
 f50:	004136        	entry	a1, 32
 f53:	743820        	extui	a3, a2, 8, 8
 f56:	8a7230        	depbits	a3, a2, 8, 8
 f59:	832d           	ae_sext16	a2, a3
 f5b:	f01d           	retw.n

00000f5d <msbc_change_endian16+0xd>:
 f5d:	000000                                        ...

00000f60 <mSBC_memory_clear>:
 f60:	004136        	entry	a1, 32
 f63:	58c242        	addi	a4, a2, 88
 f66:	050c           	movi.n	a5, 0
 f68:	a0a082        	movi	a8, 160
 f6b:	00a132        	movi	a3, 0x100
 f6e:	02a376        	loopgtz	a3, f74 <mSBC_memory_clear+0x14>
 f71:	09e454        	ae_s32ip	a5, a4, 4

00000f74 <mSBC_memory_clear+0x14>:
 f74:	05d242        	addmi	a4, a2, 0x500
 f77:	a0c442        	addi	a4, a4, -96
 f7a:	02a876        	loopgtz	a8, f80 <mSBC_memory_clear+0x20>
 f7d:	09e454        	ae_s32ip	a5, a4, 4
 f80:	0bd282        	addmi	a8, a2, 0xb00
 f83:	535852        	s16i	a5, a8, 166
 f86:	f01d           	retw.n

00000f88 <msbc_dec_init>:
 f88:	004136        	entry	a1, 32
 f8b:	fc4781        	l32r	a8, a8 (f60 <mSBC_memory_clear>)
 f8e:	02ad           	mov.n	a10, a2
 f90:	0008e0        	callx8	a8
 f93:	f01d           	retw.n

00000f95 <msbc_dec_init+0xd>:
 f95:	000000                                        ...

00000f98 <msbc_get_byte>:
 f98:	004136        	entry	a1, 32
 f9b:	850c           	movi.n	a5, 8
 f9d:	02d242        	addmi	a4, a2, 0x200
 fa0:	9b2432        	l32i	a3, a4, 0x26c
 fa3:	032754        	ae_lb	a2, a5
 fa6:	1c1354        	ae_db	a3, a5
 fa9:	922d           	ae_zext16	a2, a2
 fab:	9b6432        	s32i	a3, a4, 0x26c
 fae:	f01d           	retw.n

00000fb0 <__do_global_ctors_aux>:
 fb0:	004136        	entry	a1, 32
 fb3:	fc3e81        	l32r	a8, ac (2200 <SBC_proto_coef_8+0x620>)
 fb6:	7f2882        	l32i	a8, a8, 0x1fc
 fb9:	fc3d21        	l32r	a2, b0 (23fc <__CTOR_LIST___47>)
 fbc:	0c0826        	beqi	a8, -1, fcc <__do_global_ctors_aux+0x1c>
 fbf:	02a8           	l32i.n	a10, a2, 0

00000fc1 <__do_global_ctors_aux+0x11>:
 fc1:	000ae0        	callx8	a10
 fc4:	fcc222        	addi	a2, a2, -4
 fc7:	02a8           	l32i.n	a10, a2, 0
 fc9:	f40a66        	bnei	a10, -1, fc1 <__do_global_ctors_aux+0x11>

00000fcc <__do_global_ctors_aux+0x1c>:
 fcc:	f01d           	retw.n

00000fce <__do_global_ctors_aux+0x1e>:
	...

00000fd0 <_fini>:
 fd0:	008136        	entry	a1, 64
 fd3:	fc3881        	l32r	a8, b4 (d0 <__do_global_dtors_aux>)
 fd6:	f03d           	nop.n
 fd8:	0008e0        	callx8	a8

00000fdb <_fini+0xb>:
 fdb:	f01d           	retw.n
