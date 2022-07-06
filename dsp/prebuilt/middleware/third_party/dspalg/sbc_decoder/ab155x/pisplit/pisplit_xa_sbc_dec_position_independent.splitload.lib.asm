
Build/lib/pisplit_xa_sbc_dec_position_independent.splitload.lib:     file format elf32-xtensa-le


Disassembly of section .text:

00000000 <_init-0xbc>:
       0:	000020f0 00001620 000020b0 00001b30     . .. .... ..0...
      10:	0000bb80 00003e80 00000d10 00000d9c     .....>..........
      20:	00001640 00001000 00007d00 0000ac44     @........}..D...
      30:	000016c8 00001758 000017e8 000019f0     ....X...........
      40:	00000604 0000055c 00000a00 00001248     ....\.......H...
      50:	0000082c 000009cc 00001234 0000123c     ,.......4...<...
      60:	000000e0 000006bc 00001b60 00000a44     ........`...D...
      70:	ffff8002 ffff8000 ffff8001 00000c40     ............@...
      80:	ffff8003 000020c0 000020d0 00001442     ..... ... ..B...
      90:	000020c8 00000cd0 00000850 ffff9440     . ......P...@...
      a0:	00001443 00001440 00001441 00000aa8     C...@...A.......
      b0:	00001bd0 00002010 00001fd0              ..... ......

000000bc <_init>:
      bc:	008136        	entry	a1, 64
      bf:	f01d           	retw.n

000000c1 <_init+0x5>:
      c1:	00000000 36000000                                .......

000000c8 <_start>:
      c8:	004136        	entry	a1, 32
      cb:	0288           	l32i.n	a8, a2, 0
      cd:	ffcc91        	l32r	a9, 0 (20f0 <printf_ptr>)
      d0:	ffcda1        	l32r	a10, 4 (1620 <_DYNAMIC+0x2cc>)
      d3:	08bd           	mov.n	a11, a8
      d5:	0989           	s32i.n	a8, a9, 0
      d7:	0008e0        	callx8	a8
      da:	ffcb21        	l32r	a2, 8 (20b0 <export_parameter_array>)
      dd:	f01d           	retw.n

000000df <_start+0x17>:
	...

000000e0 <xa_sbc_unpack_frame>:
      e0:	014136        	entry	a1, 160
      e3:	1b6122        	s32i	a2, a1, 108
      e6:	126132        	s32i	a3, a1, 72
      e9:	ffc861        	l32r	a6, c (1b30 <_DYNAMIC+0x7dc>)
      ec:	136142        	s32i	a4, a1, 76
      ef:	058d           	mov.n	a8, a5
      f1:	16a8           	l32i.n	a10, a6, 4
      f3:	2698           	l32i.n	a9, a6, 8
      f5:	06b8           	l32i.n	a11, a6, 0
      f7:	66c8           	l32i.n	a12, a6, 24
      f9:	56d8           	l32i.n	a13, a6, 20
      fb:	46e8           	l32i.n	a14, a6, 16
      fd:	36f8           	l32i.n	a15, a6, 12
      ff:	31f9           	s32i.n	a15, a1, 12
     101:	41e9           	s32i.n	a14, a1, 16
     103:	51d9           	s32i.n	a13, a1, 20
     105:	61c9           	s32i.n	a12, a1, 24
     107:	01b9           	s32i.n	a11, a1, 0
     109:	2199           	s32i.n	a9, a1, 8
     10b:	11a9           	s32i.n	a10, a1, 4
     10d:	9698           	l32i.n	a9, a6, 36
     10f:	86a8           	l32i.n	a10, a6, 32
     111:	76b8           	l32i.n	a11, a6, 28
     113:	71b9           	s32i.n	a11, a1, 28
     115:	81a9           	s32i.n	a10, a1, 32
     117:	9199           	s32i.n	a9, a1, 36
     119:	a668           	l32i.n	a6, a6, 40
     11b:	a169           	s32i.n	a6, a1, 40
     11d:	7b45a6        	blti	a5, 4, 19c <xa_sbc_unpack_frame+0xbc>
     120:	0004f2        	l8ui	a15, a4, 0
     123:	9ca062        	movi	a6, 156
     126:	761f67        	beq	a15, a6, 1a0 <xa_sbc_unpack_frame+0xc0>
     129:	e27c           	movi.n	a2, -2
     12b:	f01d           	retw.n

0000012d <xa_sbc_unpack_frame+0x4d>:
     12d:	102162        	l32i	a6, a1, 64
     130:	20c5f2        	addi	a15, a5, 32
     133:	6536f7        	bltu	a6, a15, 19c <xa_sbc_unpack_frame+0xbc>
     136:	0b0c           	movi.n	a11, 0
     138:	0a0c           	movi.n	a10, 0
     13a:	790c           	movi.n	a9, 7
     13c:	1221c2        	l32i	a12, a1, 72
     13f:	1321d2        	l32i	a13, a1, 76
     142:	080c           	movi.n	a8, 0
     144:	094c82        	s8i	a8, a12, 9
     147:	850b           	addi.n	a8, a5, -1
     149:	1b9876        	loopnez	a8, 168 <xa_sbc_unpack_frame+0x88>
     14c:	040d82        	l8ui	a8, a13, 4
     14f:	400900        	ssr	a9
     152:	990b           	addi.n	a9, a9, -1
     154:	b18080        	sra	a8, a8
     157:	048080        	extui	a8, a8, 0, 1
     15a:	401a00        	ssl	a10
     15d:	a18800        	sll	a8, a8
     160:	aa1b           	addi.n	a10, a10, 1
     162:	20bb80        	or	a11, a11, a8
     165:	094cb2        	s8i	a11, a12, 9

00000168 <xa_sbc_unpack_frame+0x88>:
     168:	122192        	l32i	a9, a1, 72
     16b:	132182        	l32i	a8, a1, 76
     16e:	0c0992        	l8ui	a9, a9, 12
     171:	040882        	l8ui	a8, a8, 4
     174:	fcc992        	addi	a9, a9, -4
     177:	3d5916        	beqz	a9, 550 <xa_sbc_unpack_frame+0x470>

0000017a <xa_sbc_unpack_frame+0x9a>:
     17a:	2189           	s32i.n	a8, a1, 8
     17c:	f1f9           	s32i.n	a15, a1, 60
     17e:	1421e2        	l32i	a14, a1, 80
     181:	3005e4        	ae_cvtp24a16x2.ll	aep0, a5, a14
     184:	ea02b101ae 	{ l32i	a10, a1, 64; ae_mulp24s.hl	aeq0, aep0, aep0 }
     189:	10c5c2        	addi	a12, a5, 16
     18c:	368b04        	ae_trunca32q48	a11, aeq0
     18f:	d1c9           	s32i.n	a12, a1, 52
     191:	e1b9           	s32i.n	a11, a1, 56
     193:	a0bbf0        	addx4	a11, a11, a15
     196:	023ab7        	bltu	a10, a11, 19c <xa_sbc_unpack_frame+0xbc>
     199:	004246        	j	2a6 <xa_sbc_unpack_frame+0x1c6>

0000019c <xa_sbc_unpack_frame+0xbc>:
     19c:	f27c           	movi.n	a2, -1
     19e:	f01d           	retw.n

000001a0 <xa_sbc_unpack_frame+0xc0>:
     1a0:	0a0c           	movi.n	a10, 0
     1a2:	73a9           	s32i.n	a10, a3, 28
     1a4:	010492        	l8ui	a9, a4, 1
     1a7:	41f690        	srli	a15, a9, 6
     1aa:	1161f2        	s32i	a15, a1, 68
     1ad:	7f9c           	beqz.n	a15, 1c8 <xa_sbc_unpack_frame+0xe8>
     1af:	cf0b           	addi.n	a12, a15, -1
     1b1:	35dc16        	beqz	a12, 512 <xa_sbc_unpack_frame+0x432>
     1b4:	fecfd2        	addi	a13, a15, -2
     1b7:	371d16        	beqz	a13, 52c <xa_sbc_unpack_frame+0x44c>
     1ba:	123f66        	bnei	a15, 3, 1d0 <xa_sbc_unpack_frame+0xf0>
     1bd:	ff94a1        	l32r	a10, 10 (bb80 <_end+0x9a8c>)
     1c0:	13a9           	s32i.n	a10, a3, 4
     1c2:	010492        	l8ui	a9, a4, 1
     1c5:	0001c6        	j	1d0 <xa_sbc_unpack_frame+0xf0>

000001c8 <xa_sbc_unpack_frame+0xe8>:
     1c8:	ff93a1        	l32r	a10, 14 (3e80 <_end+0x1d8c>)
     1cb:	13a9           	s32i.n	a10, a3, 4
     1cd:	010492        	l8ui	a9, a4, 1

000001d0 <xa_sbc_unpack_frame+0xf0>:
     1d0:	870c           	movi.n	a7, 8
     1d2:	420c           	movi.n	a2, 4
     1d4:	145490        	extui	a5, a9, 4, 2
     1d7:	d59c           	beqz.n	a5, 1f8 <xa_sbc_unpack_frame+0x118>
     1d9:	c50b           	addi.n	a12, a5, -1
     1db:	33ec16        	beqz	a12, 51d <xa_sbc_unpack_frame+0x43d>
     1de:	fec5d2        	addi	a13, a5, -2
     1e1:	353d16        	beqz	a13, 538 <xa_sbc_unpack_frame+0x458>
     1e4:	1c3566        	bnei	a5, 3, 204 <xa_sbc_unpack_frame+0x124>
     1e7:	1221b2        	l32i	a11, a1, 72
     1ea:	132192        	l32i	a9, a1, 76
     1ed:	0a1c           	movi.n	a10, 16
     1ef:	0b4ba2        	s8i	a10, a11, 11
     1f2:	010992        	l8ui	a9, a9, 1
     1f5:	0002c6        	j	204 <xa_sbc_unpack_frame+0x124>

000001f8 <xa_sbc_unpack_frame+0x118>:
     1f8:	1221a2        	l32i	a10, a1, 72
     1fb:	132192        	l32i	a9, a1, 76
     1fe:	0b4a22        	s8i	a2, a10, 11
     201:	010992        	l8ui	a9, a9, 1

00000204 <xa_sbc_unpack_frame+0x124>:
     204:	1221b2        	l32i	a11, a1, 72
     207:	140c           	movi.n	a4, 1
     209:	149290        	extui	a9, a9, 2, 2
     20c:	004b92        	s8i	a9, a11, 0
     20f:	2f9916        	beqz	a9, 50c <xa_sbc_unpack_frame+0x42c>
     212:	051926        	beqi	a9, 1, 21b <xa_sbc_unpack_frame+0x13b>
     215:	022926        	beqi	a9, 2, 21b <xa_sbc_unpack_frame+0x13b>
     218:	073966        	bnei	a9, 3, 223 <xa_sbc_unpack_frame+0x143>
     21b:	1221d2        	l32i	a13, a1, 72
     21e:	2c0c           	movi.n	a12, 2
     220:	084dc2        	s8i	a12, a13, 8

00000223 <xa_sbc_unpack_frame+0x143>:
     223:	1221e2        	l32i	a14, a1, 72
     226:	1321d2        	l32i	a13, a1, 76
     229:	080ea2        	l8ui	a10, a14, 8
     22c:	010d62        	l8ui	a6, a13, 1
     22f:	0b0e52        	l8ui	a5, a14, 11
     232:	156152        	s32i	a5, a1, 84
     235:	046160        	extui	a6, a6, 1, 1
     238:	0a4e62        	s8i	a6, a14, 10
     23b:	010df2        	l8ui	a15, a13, 1
     23e:	1461a2        	s32i	a10, a1, 80
     241:	025d           	mov.n	a5, a2
     243:	04f0f0        	extui	a15, a15, 0, 1
     246:	9357f0        	movnez	a5, a7, a15
     249:	0c4e52        	s8i	a5, a14, 12
     24c:	020dd2        	l8ui	a13, a13, 2
     24f:	4ed9           	s32i.n	a13, a14, 16
     251:	297916        	beqz	a9, 4ec <xa_sbc_unpack_frame+0x40c>
     254:	b90b           	addi.n	a11, a9, -1
     256:	292b16        	beqz	a11, 4ec <xa_sbc_unpack_frame+0x40c>
     259:	745050        	extui	a5, a5, 0, 8

0000025c <xa_sbc_unpack_frame+0x17c>:
     25c:	fec9c2        	addi	a12, a9, -2
     25f:	298c16        	beqz	a12, 4fb <xa_sbc_unpack_frame+0x41b>
     262:	fdc9e2        	addi	a14, a9, -3
     265:	292e16        	beqz	a14, 4fb <xa_sbc_unpack_frame+0x41b>

00000268 <xa_sbc_unpack_frame+0x188>:
     268:	132162        	l32i	a6, a1, 76
     26b:	1221f2        	l32i	a15, a1, 72
     26e:	010692        	l8ui	a9, a6, 1
     271:	0199           	s32i.n	a9, a1, 0
     273:	020662        	l8ui	a6, a6, 2
     276:	1169           	s32i.n	a6, a1, 4
     278:	000ff2        	l8ui	a15, a15, 0
     27b:	1168d0        	slli	a6, a8, 3
     27e:	106162        	s32i	a6, a1, 64
     281:	fdcff2        	addi	a15, a15, -3
     284:	ea5f16        	beqz	a15, 12d <xa_sbc_unpack_frame+0x4d>
     287:	1421c2        	l32i	a12, a1, 80
     28a:	3015c4        	ae_cvtp24a16x2.ll	aep1, a5, a12
     28d:	021954        	ae_mulp24s.hl	aeq1, aep1, aep1
     290:	369b04        	ae_trunca32q48	a11, aeq1
     293:	082c           	movi.n	a8, 32
     295:	e1b9           	s32i.n	a11, a1, 56
     297:	a0bb80        	addx4	a11, a11, a8
     29a:	02b6b7        	bgeu	a6, a11, 2a0 <xa_sbc_unpack_frame+0x1c0>
     29d:	ffbec6        	j	19c <xa_sbc_unpack_frame+0xbc>
     2a0:	f189           	s32i.n	a8, a1, 60
     2a2:	0d1c           	movi.n	a13, 16
     2a4:	d1d9           	s32i.n	a13, a1, 52

000002a6 <xa_sbc_unpack_frame+0x1c6>:
     2a6:	e1f8           	l32i.n	a15, a1, 56
     2a8:	29df16        	beqz	a15, 549 <xa_sbc_unpack_frame+0x469>
     2ab:	013d           	mov.n	a3, a1
     2ad:	1b21e2        	l32i	a14, a1, 108
     2b0:	d1b8           	l32i.n	a11, a1, 52
     2b2:	f188           	l32i.n	a8, a1, 60
     2b4:	0f9d           	mov.n	a9, a15
     2b6:	1321f2        	l32i	a15, a1, 76
     2b9:	369976        	loopnez	a9, 2f3 <xa_sbc_unpack_frame+0x213>
     2bc:	219380        	srai	a9, a8, 3
     2bf:	24a080        	extui	a10, a8, 0, 3
     2c2:	24c0b0        	extui	a12, a11, 0, 3
     2c5:	21d3b0        	srai	a13, a11, 3
     2c8:	9f9a           	add.n	a9, a15, a9
     2ca:	c0a2a0        	sub	a10, a2, a10
     2cd:	c0c2c0        	sub	a12, a2, a12
     2d0:	a0dd30        	addx4	a13, a13, a3
     2d3:	bb4b           	addi.n	a11, a11, 4
     2d5:	884b           	addi.n	a8, a8, 4
     2d7:	000992        	l8ui	a9, a9, 0
     2da:	400a00        	ssr	a10
     2dd:	0da8           	l32i.n	a10, a13, 0
     2df:	b19090        	sra	a9, a9
     2e2:	349090        	extui	a9, a9, 0, 4
     2e5:	401c00        	ssl	a12
     2e8:	a1c900        	sll	a12, a9
     2eb:	20aac0        	or	a10, a10, a12
     2ee:	0da9           	s32i.n	a10, a13, 0
     2f0:	09ee94        	ae_s32ip	a9, a14, 4

000002f3 <xa_sbc_unpack_frame+0x213>:
     2f3:	e198           	l32i.n	a9, a1, 56
     2f5:	00a0a2        	movi	a10, 0
     2f8:	5399a0        	max	a9, a9, a10

000002fb <xa_sbc_unpack_frame+0x21b>:
     2fb:	01ad           	mov.n	a10, a1
     2fd:	f178           	l32i.n	a7, a1, 60
     2ff:	d1b8           	l32i.n	a11, a1, 52
     301:	ff4581        	l32r	a8, 18 (d10 <xa_sbc_crc8>)
     304:	a07970        	addx4	a7, a9, a7
     307:	a0b9b0        	addx4	a11, a9, a11
     30a:	0008e0        	callx8	a8
     30d:	132182        	l32i	a8, a1, 76
     310:	030882        	l8ui	a8, a8, 3
     313:	c088a0        	sub	a8, a8, a10
     316:	1ee856        	bnez	a8, 508 <xa_sbc_unpack_frame+0x428>
     319:	122182        	l32i	a8, a1, 72
     31c:	000882        	l8ui	a8, a8, 0
     31f:	05dd           	mov.n	a13, a5
     321:	c179           	s32i.n	a7, a1, 48
     323:	188c           	beqz.n	a8, 328 <xa_sbc_unpack_frame+0x248>
     325:	021866        	bnei	a8, 1, 32b <xa_sbc_unpack_frame+0x24b>

00000328 <xa_sbc_unpack_frame+0x248>:
     328:	41d150        	srli	a13, a5, 1

0000032b <xa_sbc_unpack_frame+0x24b>:
     32b:	1121c2        	l32i	a12, a1, 68
     32e:	11edf0        	slli	a14, a13, 1
     331:	ff3a81        	l32r	a8, 1c (d9c <xa_sbc_calculate_bits>)
     334:	1b21a2        	l32i	a10, a1, 108
     337:	1221b2        	l32i	a11, a1, 72
     33a:	0008e0        	callx8	a8
     33d:	c198           	l32i.n	a9, a1, 48
     33f:	fea0a2        	movi	a10, 254
     342:	218390        	srai	a8, a9, 3
     345:	1088a0        	and	a8, a8, a10
     348:	1321a2        	l32i	a10, a1, 76
     34b:	349090        	extui	a9, a9, 0, 4
     34e:	8a8a           	add.n	a8, a10, a8
     350:	176a07        	bbci	a10, 0, 36b <xa_sbc_unpack_frame+0x28b>
     353:	fdc8c2        	addi	a12, a8, -3
     356:	0b0c           	movi.n	a11, 0
     358:	0326b4        	wur.ae_bitptr	a11
     35b:	000cf4        	ae_dbi	a12, 16
     35e:	000cf4        	ae_dbi	a12, 16
     361:	008cf4        	ae_dbi	a12, 8
     364:	1c1c94        	ae_db	a12, a9
     367:	000446        	j	37c <xa_sbc_unpack_frame+0x29c>

0000036a <xa_sbc_unpack_frame+0x28a>:
	...

0000036b <xa_sbc_unpack_frame+0x28b>:
     36b:	0c0c           	movi.n	a12, 0
     36d:	0326c4        	wur.ae_bitptr	a12
     370:	fec8c2        	addi	a12, a8, -2
     373:	000cf4        	ae_dbi	a12, 16
     376:	000cf4        	ae_dbi	a12, 16
     379:	1c1c94        	ae_db	a12, a9

0000037c <xa_sbc_unpack_frame+0x29c>:
     37c:	1d61c2        	s32i	a12, a1, 116
     37f:	1521d2        	l32i	a13, a1, 84
     382:	00e2c4        	rur.ae_bitptr	a14
     385:	1e61e2        	s32i	a14, a1, 120
     388:	0ddd16        	beqz	a13, 469 <xa_sbc_unpack_frame+0x389>
     38b:	142162        	l32i	a6, a1, 80
     38e:	0d7616        	beqz	a6, 469 <xa_sbc_unpack_frame+0x389>
     391:	032c           	movi.n	a3, 32
     393:	0b0c           	movi.n	a11, 0
     395:	11a5e0        	slli	a10, a5, 2
     398:	090c           	movi.n	a9, 0
     39a:	0d0c           	movi.n	a13, 0
     39c:	1761d2        	s32i	a13, a1, 92
     39f:	166192        	s32i	a9, a1, 88
     3a2:	1961a2        	s32i	a10, a1, 100
     3a5:	1861b2        	s32i	a11, a1, 96
     3a8:	ff1ea1        	l32r	a10, 20 (1640 <_DYNAMIC+0x2ec>)
     3ab:	ff1eb1        	l32r	a11, 24 (1000 <xa_sbc_calculate_bits+0x264>)

000003ae <xa_sbc_unpack_frame+0x2ce>:
     3ae:	020c           	movi.n	a2, 0
     3b0:	1b21f2        	l32i	a15, a1, 108
     3b3:	1821e2        	l32i	a14, a1, 96
     3b6:	142182        	l32i	a8, a1, 80
     3b9:	172162        	l32i	a6, a1, 92
     3bc:	090c           	movi.n	a9, 0
     3be:	1c6192        	s32i	a9, a1, 112
     3c1:	668a           	add.n	a6, a6, a8
     3c3:	eefa           	add.n	a14, a14, a15
     3c5:	3ccee2        	addi	a14, a14, 60
     3c8:	1166b0        	slli	a6, a6, 5
     3cb:	ff6a           	add.n	a15, a15, a6
     3cd:	fe2483cffe 	{ addi	a15, a15, 60; ae_zeroq56	aeq2 }
     3d2:	1a61f2        	s32i	a15, a1, 104

000003d5 <xa_sbc_unpack_frame+0x2f5>:
     3d5:	1b2192        	l32i	a9, a1, 108
     3d8:	20dee0        	or	a13, a14, a14
     3db:	20cee2        	addi	a14, a14, 32
     3de:	809290        	add	a9, a2, a9
     3e1:	04d982        	addmi	a8, a9, 0x400
     3e4:	44c882        	addi	a8, a8, 68
     3e7:	f03d           	nop.n
     3e9:	429576        	loopnez	a5, 42f <xa_sbc_unpack_frame+0x34f>
     3ec:	7f28f2        	l32i	a15, a8, 0x1fc
     3ef:	0377f4        	ae_lb	a7, a15
     3f2:	fc1440488e 	{ addi	a8, a8, 4; ae_movq56	aeq0, aeq2 }
     3f7:	ffac           	beqz.n	a15, 42a <xa_sbc_unpack_frame+0x34a>
     3f9:	1c1cf4        	ae_db	a12, a15
     3fc:	1177a0        	slli	a7, a7, 6
     3ff:	116fd0        	slli	a6, a15, 3
     402:	553a64        	ae_lp24x2.x	aep3, a10, a6
     405:	207730        	or	a7, a7, a3
     408:	39a774        	ae_movpa24x2	aep2, a7, a7
     40b:	ea6bbec47f 	{ ae_cvtq48a32s	aeq1, a4; ae_mulp24s.ll	aeq0, aep2, aep3 }
     410:	355434        	ae_sraiq56	aeq1, aeq1, 16
     413:	3505f4        	ae_sraiq56	aeq0, aeq0, 23
     416:	0109c4        	ae_addq56	aeq0, aeq0, aeq1
     419:	022304        	ae_mulap24s.hh	aeq0, aep2, aep3
     41c:	364b34        	ae_cvtq48a32s	aeq1, a11
     41f:	f826f0096e 	{ l32i	a6, a9, 0; ae_subq56	aeq0, aeq0, aeq1 }
     424:	340694        	ae_sllaq56	aeq0, aeq0, a6
     427:	3501f4        	ae_sraiq56	aeq0, aeq0, 7

0000042a <xa_sbc_unpack_frame+0x34a>:
     42a:	624d14        	ae_sq32f.iu	aeq0, a13, 4
     42d:	994b           	addi.n	a9, a9, 4

0000042f <xa_sbc_unpack_frame+0x34f>:
     42f:	192182        	l32i	a8, a1, 100
     432:	1c2192        	l32i	a9, a1, 112
     435:	1a2172        	l32i	a7, a1, 104
     438:	228a           	add.n	a2, a2, a8
     43a:	fe2490995f 	{ add	a9, a9, a5; ae_zeroq56	aeq2 }
     43f:	1c6192        	s32i	a9, a1, 112
     442:	8f9e77        	bne	a14, a7, 3d5 <xa_sbc_unpack_frame+0x2f5>
     445:	1521e2        	l32i	a14, a1, 84
     448:	1621d2        	l32i	a13, a1, 88
     44b:	1821f2        	l32i	a15, a1, 96
     44e:	172162        	l32i	a6, a1, 92
     451:	01cdd2        	addi	a13, a13, 1
     454:	40cff2        	addi	a15, a15, 64
     457:	02c662        	addi	a6, a6, 2
     45a:	176162        	s32i	a6, a1, 92
     45d:	1861f2        	s32i	a15, a1, 96
     460:	1661d2        	s32i	a13, a1, 88
     463:	021de7        	beq	a13, a14, 469 <xa_sbc_unpack_frame+0x389>
     466:	ffd106        	j	3ae <xa_sbc_unpack_frame+0x2ce>
     469:	1d21b2        	l32i	a11, a1, 116
     46c:	00d2c4        	rur.ae_bitptr	a13
     46f:	c0bcb0        	sub	a11, a12, a11
     472:	b0bbd0        	addx8	a11, a11, a13
     475:	1e21d2        	l32i	a13, a1, 120
     478:	c1a8           	l32i.n	a10, a1, 48
     47a:	102172        	l32i	a7, a1, 64
     47d:	c0bbd0        	sub	a11, a11, a13
     480:	aaba           	add.n	a10, a10, a11
     482:	f1a9           	s32i.n	a10, a1, 60
     484:	02a7a7        	bge	a7, a10, 48a <xa_sbc_unpack_frame+0x3aa>
     487:	ff4446        	j	19c <xa_sbc_unpack_frame+0xbc>
     48a:	1221c2        	l32i	a12, a1, 72
     48d:	000cd2        	l8ui	a13, a12, 0
     490:	090cc2        	l8ui	a12, a12, 9
     493:	3f3d66        	bnei	a13, 3, 4d6 <xa_sbc_unpack_frame+0x3f6>
     496:	1521d2        	l32i	a13, a1, 84
     499:	9dbc           	beqz.n	a13, 4d6 <xa_sbc_unpack_frame+0x3f6>
     49b:	040c           	movi.n	a4, 0
     49d:	0e0c           	movi.n	a14, 0
     49f:	1661e2        	s32i	a14, a1, 88

000004a2 <xa_sbc_unpack_frame+0x3c2>:
     4a2:	1b21b2        	l32i	a11, a1, 108
     4a5:	190c           	movi.n	a9, 1
     4a7:	b4ba           	add.n	a11, a4, a11
     4a9:	189576        	loopnez	a5, 4c5 <xa_sbc_unpack_frame+0x3e5>
     4ac:	1009c7        	bnone	a9, a12, 4c0 <xa_sbc_unpack_frame+0x3e0>
     4af:	182bf2        	l32i	a15, a11, 96
     4b2:	102be2        	l32i	a14, a11, 64
     4b5:	defa           	add.n	a13, a14, a15
     4b7:	106bd2        	s32i	a13, a11, 64
     4ba:	c0eef0        	sub	a14, a14, a15
     4bd:	186be2        	s32i	a14, a11, 96

000004c0 <xa_sbc_unpack_frame+0x3e0>:
     4c0:	1199f0        	slli	a9, a9, 1
     4c3:	bb4b           	addi.n	a11, a11, 4

000004c5 <xa_sbc_unpack_frame+0x3e5>:
     4c5:	162162        	l32i	a6, a1, 88
     4c8:	40c442        	addi	a4, a4, 64
     4cb:	152172        	l32i	a7, a1, 84
     4ce:	661b           	addi.n	a6, a6, 1
     4d0:	166162        	s32i	a6, a1, 88
     4d3:	cb9677        	bne	a6, a7, 4a2 <xa_sbc_unpack_frame+0x3c2>
     4d6:	f128           	l32i.n	a2, a1, 60
     4d8:	867c           	movi.n	a6, -8
     4da:	248020        	extui	a8, a2, 0, 3
     4dd:	106260        	and	a6, a2, a6
     4e0:	668b           	addi.n	a6, a6, 8
     4e2:	932680        	movnez	a2, a6, a8
     4e5:	212320        	srai	a2, a2, 3
     4e8:	822d           	ae_sext16	a2, a2
     4ea:	f01d           	retw.n

000004ec <xa_sbc_unpack_frame+0x40c>:
     4ec:	745050        	extui	a5, a5, 0, 8
     4ef:	1175c0        	slli	a7, a5, 4
     4f2:	0227d7        	blt	a7, a13, 4f8 <xa_sbc_unpack_frame+0x418>
     4f5:	ff58c6        	j	25c <xa_sbc_unpack_frame+0x17c>
     4f8:	000206        	j	504 <xa_sbc_unpack_frame+0x424>

000004fb <xa_sbc_unpack_frame+0x41b>:
     4fb:	1195b0        	slli	a9, a5, 5
     4fe:	0229d7        	blt	a9, a13, 504 <xa_sbc_unpack_frame+0x424>
     501:	ff58c6        	j	268 <xa_sbc_unpack_frame+0x188>

00000504 <xa_sbc_unpack_frame+0x424>:
     504:	c27c           	movi.n	a2, -4
     506:	f01d           	retw.n

00000508 <xa_sbc_unpack_frame+0x428>:
     508:	d27c           	movi.n	a2, -3
     50a:	f01d           	retw.n

0000050c <xa_sbc_unpack_frame+0x42c>:
     50c:	084b42        	s8i	a4, a11, 8
     50f:	ff4406        	j	223 <xa_sbc_unpack_frame+0x143>

00000512 <xa_sbc_unpack_frame+0x432>:
     512:	fec5a1        	l32r	a10, 28 (7d00 <_end+0x5c0c>)
     515:	13a9           	s32i.n	a10, a3, 4
     517:	010492        	l8ui	a9, a4, 1
     51a:	ff2c86        	j	1d0 <xa_sbc_unpack_frame+0xf0>

0000051d <xa_sbc_unpack_frame+0x43d>:
     51d:	1221a2        	l32i	a10, a1, 72
     520:	132192        	l32i	a9, a1, 76
     523:	0b4a72        	s8i	a7, a10, 11
     526:	010992        	l8ui	a9, a9, 1
     529:	ff35c6        	j	204 <xa_sbc_unpack_frame+0x124>

0000052c <xa_sbc_unpack_frame+0x44c>:
     52c:	fec0a1        	l32r	a10, 2c (ac44 <_end+0x8b50>)
     52f:	13a9           	s32i.n	a10, a3, 4
     531:	010492        	l8ui	a9, a4, 1
     534:	ff2606        	j	1d0 <xa_sbc_unpack_frame+0xf0>

00000537 <xa_sbc_unpack_frame+0x457>:
	...

00000538 <xa_sbc_unpack_frame+0x458>:
     538:	1221b2        	l32i	a11, a1, 72
     53b:	132192        	l32i	a9, a1, 76
     53e:	ca0c           	movi.n	a10, 12
     540:	0b4ba2        	s8i	a10, a11, 11
     543:	010992        	l8ui	a9, a9, 1
     546:	ff2e86        	j	204 <xa_sbc_unpack_frame+0x124>

00000549 <xa_sbc_unpack_frame+0x469>:
     549:	090c           	movi.n	a9, 0
     54b:	ff6b06        	j	2fb <xa_sbc_unpack_frame+0x21b>

0000054e <xa_sbc_unpack_frame+0x46e>:
	...

00000550 <xa_sbc_unpack_frame+0x470>:
     550:	f0a0c2        	movi	a12, 240
     553:	1088c0        	and	a8, a8, a12
     556:	ff0806        	j	17a <xa_sbc_unpack_frame+0x9a>

00000559 <xa_sbc_unpack_frame+0x479>:
     559:	000000                                        ...

0000055c <xa_sbc_synthesize_four>:
     55c:	004136        	entry	a1, 32
     55f:	860c           	movi.n	a6, 8
     561:	029d           	mov.n	a9, a2
     563:	208330        	or	a8, a3, a3
     566:	01d252        	addmi	a5, a2, 0x100
     569:	3cc552        	addi	a5, a5, 60
     56c:	203440        	or	a3, a4, a4
     56f:	feb021        	l32r	a2, 30 (16c8 <_DYNAMIC+0x374>)
     572:	fcc942        	addi	a4, a9, -4
     575:	168676        	loop	a6, 58f <xa_sbc_synthesize_four+0x33>
     578:	540214        	ae_lp24x2.i	aep0, a2, 8
     57b:	5413f4        	ae_lp24x2.i	aep1, a3, -8
     57e:	f023b0830f 	{ ae_lp24x2.iu	aep0, a3, 0; ae_mulzaap24s.hh.ll	aeq0, aep0, aep1 }
     583:	549224        	ae_lp24x2.iu	aep1, a2, 16
     586:	051024        	ae_mulaap24s.hh.ll	aeq0, aep1, aep0
     589:	624414        	ae_sq32f.iu	aeq0, a4, 4
     58c:	624514        	ae_sq32f.iu	aeq0, a5, 4

0000058f <xa_sbc_synthesize_four+0x33>:
     58f:	060c           	movi.n	a6, 0
     591:	092d           	mov.n	a2, a9
     593:	fea871        	l32r	a7, 34 (1758 <_DYNAMIC+0x404>)
     596:	340c           	movi.n	a4, 3
     598:	998b           	addi.n	a9, a9, 8
     59a:	f8c732        	addi	a3, a7, -8

0000059d <xa_sbc_synthesize_four+0x41>:
     59d:	54d304        	ae_lp24x2.iu	aep5, a3, 0
     5a0:	54e204        	ae_lp24x2.iu	aep6, a2, 0
     5a3:	ead7b0c26f 	{ ae_lp24x2.iu	aep4, a2, 48; ae_mulp24s.ll	aeq0, aep5, aep6 }
     5a8:	e8d6f0b32f 	{ ae_lp24x2.iu	aep3, a3, 16; ae_mulp24s.hh	aeq1, aep5, aep6 }
     5ad:	e48e70032f 	{ ae_lp24x2.i	aep0, a3, 16; ae_mulap24s.hh	aeq1, aep3, aep4 }
     5b2:	e68f30a22f 	{ ae_lp24x2.iu	aep2, a2, 16; ae_mulap24s.ll	aeq0, aep3, aep4 }
     5b7:	e64330926f 	{ ae_lp24x2.iu	aep1, a2, 48; ae_mulap24s.ll	aeq0, aep0, aep2 }
     5bc:	e44270a34f 	{ ae_lp24x2.iu	aep2, a3, 32; ae_mulap24s.hh	aeq1, aep0, aep2 }
     5c1:	f03d           	nop.n
     5c3:	f03d           	nop.n
     5c5:	138476        	loop	a4, 5dc <xa_sbc_synthesize_four+0x80>
     5c8:	e42a70822f 	{ ae_lp24x2.iu	aep0, a2, 16; ae_mulap24s.hh	aeq1, aep2, aep1 }
     5cd:	e62b30132f 	{ ae_lp24x2.i	aep1, a3, 16; ae_mulap24s.ll	aeq0, aep2, aep1 }
     5d2:	e60730a34f 	{ ae_lp24x2.iu	aep2, a3, 32; ae_mulap24s.ll	aeq0, aep1, aep0 }
     5d7:	e40670926f 	{ ae_lp24x2.iu	aep1, a2, 48; ae_mulap24s.hh	aeq1, aep1, aep0 }

000005dc <xa_sbc_synthesize_four+0x80>:
     5dc:	022904        	ae_mulap24s.hh	aeq1, aep2, aep1
     5df:	e62b13377f 	{ or	a3, a7, a7; ae_mulap24s.ll	aeq0, aep2, aep1 }
     5e4:	369214        	ae_sllisq56s	aeq2, aeq1, 8
     5e7:	36c214        	ae_sllisq56s	aeq3, aeq0, 8
     5ea:	fd84d3299f 	{ or	a2, a9, a9; ae_satq48s	aeq1, aeq3 }
     5ef:	fc80c0034f 	{ movi	a4, 3; ae_satq48s	aeq0, aeq2 }
     5f4:	fb4640166e 	{ addi	a6, a6, 1; ae_truncp24q48x2	aep7, aeq0, aeq1 }
     5f9:	46f814        	ae_sp16x2f.iu	aep7, a8, 4
     5fc:	9d2666        	bnei	a6, 2, 59d <xa_sbc_synthesize_four+0x41>
     5ff:	f01d           	retw.n

00000601 <xa_sbc_synthesize_four+0xa5>:
     601:	000000                                        ...

00000604 <xa_sbc_synthesize_eight>:
     604:	004136        	entry	a1, 32
     607:	061c           	movi.n	a6, 16
     609:	028d           	mov.n	a8, a2
     60b:	209330        	or	a9, a3, a3
     60e:	02d252        	addmi	a5, a2, 0x200
     611:	7cc552        	addi	a5, a5, 124
     614:	203440        	or	a3, a4, a4
     617:	fe8821        	l32r	a2, 38 (17e8 <_DYNAMIC+0x494>)
     61a:	fcc842        	addi	a4, a8, -4
     61d:	268676        	loop	a6, 647 <xa_sbc_synthesize_eight+0x43>
     620:	540214        	ae_lp24x2.i	aep0, a2, 8
     623:	5413d4        	ae_lp24x2.i	aep1, a3, -24
     626:	f023b003ef 	{ ae_lp24x2.i	aep0, a3, -16; ae_mulzaap24s.hh.ll	aeq0, aep0, aep1 }
     62b:	541224        	ae_lp24x2.i	aep1, a2, 16
     62e:	e204b0023f 	{ ae_lp24x2.i	aep0, a2, 24; ae_mulaap24s.hh.ll	aeq0, aep1, aep0 }
     633:	5413f4        	ae_lp24x2.i	aep1, a3, -8
     636:	e220b0830f 	{ ae_lp24x2.iu	aep0, a3, 0; ae_mulaap24s.hh.ll	aeq0, aep0, aep1 }
     63b:	549244        	ae_lp24x2.iu	aep1, a2, 32
     63e:	051024        	ae_mulaap24s.hh.ll	aeq0, aep1, aep0
     641:	624414        	ae_sq32f.iu	aeq0, a4, 4
     644:	624514        	ae_sq32f.iu	aeq0, a5, 4

00000647 <xa_sbc_synthesize_eight+0x43>:
     647:	60a042        	movi	a4, 96
     64a:	fe7c71        	l32r	a7, 3c (19f0 <_DYNAMIC+0x69c>)
     64d:	350c           	movi.n	a5, 3
     64f:	082d           	mov.n	a2, a8
     651:	20c7a2        	addi	a10, a7, 32
     654:	073d           	mov.n	a3, a7

00000656 <xa_sbc_synthesize_eight+0x52>:
     656:	778b           	addi.n	a7, a7, 8
     658:	54c304        	ae_lp24x2.iu	aep4, a3, 0
     65b:	54d204        	ae_lp24x2.iu	aep5, a2, 0
     65e:	eab3a0324f 	{ ae_lp24x2.xu	aep3, a2, a4; ae_mulp24s.ll	aeq0, aep4, aep5 }
     663:	e8b2f0a34f 	{ ae_lp24x2.iu	aep2, a3, 32; ae_mulp24s.hh	aeq1, aep4, aep5 }
     668:	e46a70834f 	{ ae_lp24x2.iu	aep0, a3, 32; ae_mulap24s.hh	aeq1, aep2, aep3 }
     66d:	e66b30924f 	{ ae_lp24x2.iu	aep1, a2, 32; ae_mulap24s.ll	aeq0, aep2, aep3 }
     672:	e62320224f 	{ ae_lp24x2.xu	aep2, a2, a4; ae_mulap24s.ll	aeq0, aep0, aep1 }
     677:	e42270934f 	{ ae_lp24x2.iu	aep1, a3, 32; ae_mulap24s.hh	aeq1, aep0, aep1 }
     67c:	f03d           	nop.n
     67e:	138576        	loop	a5, 695 <xa_sbc_synthesize_eight+0x91>
     681:	e44670824f 	{ ae_lp24x2.iu	aep0, a2, 32; ae_mulap24s.hh	aeq1, aep1, aep2 }
     686:	e64730934f 	{ ae_lp24x2.iu	aep1, a3, 32; ae_mulap24s.ll	aeq0, aep1, aep2 }
     68b:	e60720224f 	{ ae_lp24x2.xu	aep2, a2, a4; ae_mulap24s.ll	aeq0, aep1, aep0 }
     690:	e40670934f 	{ ae_lp24x2.iu	aep1, a3, 32; ae_mulap24s.hh	aeq1, aep1, aep0 }

00000695 <xa_sbc_synthesize_eight+0x91>:
     695:	e44640035f 	{ movi	a5, 3; ae_mulap24s.hh	aeq1, aep1, aep2 }
     69a:	e64713377f 	{ or	a3, a7, a7; ae_mulap24s.ll	aeq0, aep1, aep2 }
     69f:	369214        	ae_sllisq56s	aeq2, aeq1, 8
     6a2:	36c214        	ae_sllisq56s	aeq3, aeq0, 8
     6a5:	fd84d5b7af 	{ sub	a11, a7, a10; ae_satq48s	aeq1, aeq3 }
     6aa:	fc80c0888e 	{ addi	a8, a8, 8; ae_satq48s	aeq0, aeq2 }
     6af:	fb4613288f 	{ or	a2, a8, a8; ae_truncp24q48x2	aep6, aeq0, aeq1 }
     6b4:	46e914        	ae_sp16x2f.iu	aep6, a9, 4
     6b7:	f9bb56        	bnez	a11, 656 <xa_sbc_synthesize_eight+0x52>
     6ba:	f01d           	retw.n

000006bc <xa_sbc_synthesize_audio>:
     6bc:	00c136        	entry	a1, 96
     6bf:	a139           	s32i.n	a3, a1, 40
     6c1:	0803a2        	l8ui	a10, a3, 8
     6c4:	0b03b2        	l8ui	a11, a3, 11
     6c7:	029d           	mov.n	a9, a2
     6c9:	0c0382        	l8ui	a8, a3, 12
     6cc:	40a122        	movi	a2, 0x140
     6cf:	81b9           	s32i.n	a11, a1, 32
     6d1:	024866        	bnei	a8, 4, 6d7 <xa_sbc_synthesize_audio+0x1b>
     6d4:	002d86        	j	78e <xa_sbc_synthesize_audio+0xd2>
     6d7:	0d0c           	movi.n	a13, 0
     6d9:	91d9           	s32i.n	a13, a1, 36
     6db:	161a16        	beqz	a10, 840 <xa_sbc_synthesize_audio+0x184>
     6de:	b199           	s32i.n	a9, a1, 44
     6e0:	c199           	s32i.n	a9, a1, 48
     6e2:	04d982        	addmi	a8, a9, 0x400
     6e5:	020c           	movi.n	a2, 0
     6e7:	08d3e2        	addmi	a14, a3, 0x800
     6ea:	11fbc0        	slli	a15, a11, 4
     6ed:	71f9           	s32i.n	a15, a1, 28
     6ef:	24cee2        	addi	a14, a14, 36
     6f2:	e129           	s32i.n	a2, a1, 56
     6f4:	3cc882        	addi	a8, a8, 60
     6f7:	80a222        	movi	a2, 0x280
     6fa:	d189           	s32i.n	a8, a1, 52
     6fc:	02dee2        	addmi	a14, a14, 0x200
     6ff:	fccee2        	addi	a14, a14, -4
     702:	f1e9           	s32i.n	a14, a1, 60

00000704 <xa_sbc_synthesize_audio+0x48>:
     704:	f148           	l32i.n	a4, a1, 60
     706:	7138           	l32i.n	a3, a1, 28
     708:	e178           	l32i.n	a7, a1, 56
     70a:	0448           	l32i.n	a4, a4, 0
     70c:	a188           	l32i.n	a8, a1, 40
     70e:	d168           	l32i.n	a6, a1, 52
     710:	747a           	add.n	a7, a4, a7
     712:	a07780        	addx4	a7, a7, a8
     715:	8188           	l32i.n	a8, a1, 32
     717:	c158           	l32i.n	a5, a1, 48
     719:	20c772        	addi	a7, a7, 32
     71c:	d8ac           	beqz.n	a8, 74d <xa_sbc_synthesize_audio+0x91>
     71e:	b188           	l32i.n	a8, a1, 44
     720:	58c552        	addi	a5, a5, 88
     723:	338a           	add.n	a3, a3, a8
     725:	04d332        	addmi	a3, a3, 0x400
     728:	3cc332        	addi	a3, a3, 60

0000072b <xa_sbc_synthesize_audio+0x6f>:
     72b:	f0c442        	addi	a4, a4, -16
     72e:	06bd           	mov.n	a11, a6
     730:	05cd           	mov.n	a12, a5
     732:	fe4381        	l32r	a8, 40 (604 <xa_sbc_synthesize_eight>)
     735:	c0c772        	addi	a7, a7, -64
     738:	07ad           	mov.n	a10, a7
     73a:	0008e0        	callx8	a8
     73d:	40c552        	addi	a5, a5, 64
     740:	10c662        	addi	a6, a6, 16
     743:	34cc           	bnez.n	a4, 74a <xa_sbc_synthesize_audio+0x8e>
     745:	772a           	add.n	a7, a7, a2
     747:	a0a042        	movi	a4, 160

0000074a <xa_sbc_synthesize_audio+0x8e>:
     74a:	dd9637        	bne	a6, a3, 72b <xa_sbc_synthesize_audio+0x6f>

0000074d <xa_sbc_synthesize_audio+0x91>:
     74d:	a1a8           	l32i.n	a10, a1, 40
     74f:	f1e8           	l32i.n	a14, a1, 60
     751:	c1d8           	l32i.n	a13, a1, 48
     753:	9198           	l32i.n	a9, a1, 36
     755:	b1c8           	l32i.n	a12, a1, 44
     757:	d1b8           	l32i.n	a11, a1, 52
     759:	991b           	addi.n	a9, a9, 1
     75b:	01dcc2        	addmi	a12, a12, 0x100
     75e:	01dbb2        	addmi	a11, a11, 0x100
     761:	20cdd2        	addi	a13, a13, 32
     764:	09ee44        	ae_s32ip	a4, a14, 4
     767:	f1e9           	s32i.n	a14, a1, 60
     769:	c1d9           	s32i.n	a13, a1, 48
     76b:	d1b9           	s32i.n	a11, a1, 52
     76d:	b1c9           	s32i.n	a12, a1, 44
     76f:	e1b8           	l32i.n	a11, a1, 56
     771:	9199           	s32i.n	a9, a1, 36
     773:	080aa2        	l8ui	a10, a10, 8
     776:	01dbb2        	addmi	a11, a11, 0x100
     779:	40cbb2        	addi	a11, a11, 64
     77c:	e1b9           	s32i.n	a11, a1, 56
     77e:	8239a7        	bltu	a9, a10, 704 <xa_sbc_synthesize_audio+0x48>
     781:	a128           	l32i.n	a2, a1, 40
     783:	0b0222        	l8ui	a2, a2, 11
     786:	1122d0        	slli	a2, a2, 3
     789:	822d           	ae_sext16	a2, a2
     78b:	f01d           	retw.n

0000078d <xa_sbc_synthesize_audio+0xd1>:
	...

0000078e <xa_sbc_synthesize_audio+0xd2>:
     78e:	080c           	movi.n	a8, 0
     790:	1189           	s32i.n	a8, a1, 4
     792:	0b2a16        	beqz	a10, 848 <xa_sbc_synthesize_audio+0x18c>
     795:	2199           	s32i.n	a9, a1, 8
     797:	3199           	s32i.n	a9, a1, 12
     799:	08d3a2        	addmi	a10, a3, 0x800
     79c:	0c0c           	movi.n	a12, 0
     79e:	11bbd0        	slli	a11, a11, 3
     7a1:	01b9           	s32i.n	a11, a1, 0
     7a3:	51c9           	s32i.n	a12, a1, 20
     7a5:	24caa2        	addi	a10, a10, 36
     7a8:	04d9c2        	addmi	a12, a9, 0x400
     7ab:	02daa2        	addmi	a10, a10, 0x200
     7ae:	fccaa2        	addi	a10, a10, -4
     7b1:	3cccc2        	addi	a12, a12, 60
     7b4:	0461c2        	s32i	a12, a1, 16
     7b7:	61a9           	s32i.n	a10, a1, 24

000007b9 <xa_sbc_synthesize_audio+0xfd>:
     7b9:	6148           	l32i.n	a4, a1, 24
     7bb:	3158           	l32i.n	a5, a1, 12
     7bd:	5178           	l32i.n	a7, a1, 20
     7bf:	0448           	l32i.n	a4, a4, 0
     7c1:	a188           	l32i.n	a8, a1, 40
     7c3:	81d8           	l32i.n	a13, a1, 32
     7c5:	774a           	add.n	a7, a7, a4
     7c7:	a07780        	addx4	a7, a7, a8
     7ca:	20c772        	addi	a7, a7, 32
     7cd:	2dbc           	beqz.n	a13, 803 <xa_sbc_synthesize_audio+0x147>
     7cf:	2188           	l32i.n	a8, a1, 8
     7d1:	0138           	l32i.n	a3, a1, 0
     7d3:	48c552        	addi	a5, a5, 72
     7d6:	042162        	l32i	a6, a1, 16
     7d9:	338a           	add.n	a3, a3, a8
     7db:	04d332        	addmi	a3, a3, 0x400
     7de:	3cc332        	addi	a3, a3, 60

000007e1 <xa_sbc_synthesize_audio+0x125>:
     7e1:	f8c442        	addi	a4, a4, -8
     7e4:	06bd           	mov.n	a11, a6
     7e6:	20c550        	or	a12, a5, a5
     7e9:	fe1681        	l32r	a8, 44 (55c <xa_sbc_synthesize_four>)
     7ec:	e0c772        	addi	a7, a7, -32
     7ef:	20a770        	or	a10, a7, a7
     7f2:	0008e0        	callx8	a8
     7f5:	40c552        	addi	a5, a5, 64
     7f8:	668b           	addi.n	a6, a6, 8
     7fa:	24cc           	bnez.n	a4, 800 <xa_sbc_synthesize_audio+0x144>
     7fc:	772a           	add.n	a7, a7, a2
     7fe:	045c           	movi.n	a4, 80

00000800 <xa_sbc_synthesize_audio+0x144>:
     800:	dd9637        	bne	a6, a3, 7e1 <xa_sbc_synthesize_audio+0x125>

00000803 <xa_sbc_synthesize_audio+0x147>:
     803:	a1a8           	l32i.n	a10, a1, 40
     805:	61e8           	l32i.n	a14, a1, 24
     807:	31d8           	l32i.n	a13, a1, 12
     809:	1198           	l32i.n	a9, a1, 4
     80b:	21c8           	l32i.n	a12, a1, 8
     80d:	41b8           	l32i.n	a11, a1, 16
     80f:	991b           	addi.n	a9, a9, 1
     811:	01dcc2        	addmi	a12, a12, 0x100
     814:	01dbb2        	addmi	a11, a11, 0x100
     817:	20cdd2        	addi	a13, a13, 32
     81a:	09ee44        	ae_s32ip	a4, a14, 4
     81d:	61e9           	s32i.n	a14, a1, 24
     81f:	31d9           	s32i.n	a13, a1, 12
     821:	41b9           	s32i.n	a11, a1, 16
     823:	21c9           	s32i.n	a12, a1, 8
     825:	51b8           	l32i.n	a11, a1, 20
     827:	1199           	s32i.n	a9, a1, 4
     829:	080aa2        	l8ui	a10, a10, 8
     82c:	bb2a           	add.n	a11, a11, a2
     82e:	51b9           	s32i.n	a11, a1, 20
     830:	8539a7        	bltu	a9, a10, 7b9 <xa_sbc_synthesize_audio+0xfd>
     833:	a128           	l32i.n	a2, a1, 40
     835:	0b0222        	l8ui	a2, a2, 11
     838:	1122e0        	slli	a2, a2, 2
     83b:	822d           	ae_sext16	a2, a2
     83d:	f01d           	retw.n

0000083f <xa_sbc_synthesize_audio+0x183>:
	...

00000840 <xa_sbc_synthesize_audio+0x184>:
     840:	112bd0        	slli	a2, a11, 3
     843:	822d           	ae_sext16	a2, a2
     845:	f01d           	retw.n

00000847 <xa_sbc_synthesize_audio+0x18b>:
	...

00000848 <xa_sbc_synthesize_audio+0x18c>:
     848:	112be0        	slli	a2, a11, 2
     84b:	822d           	ae_sext16	a2, a2
     84d:	f01d           	retw.n

0000084f <xa_sbc_synthesize_audio+0x193>:
	...

00000850 <xa_sbc_decode_init>:
     850:	006136        	entry	a1, 48
     853:	fdfdc1        	l32r	a12, 48 (a00 <xa_sbc_decode+0x34>)
     856:	20c3a2        	addi	a10, a3, 32
     859:	0b0c           	movi.n	a11, 0
     85b:	fdfc81        	l32r	a8, 4c (1248 <__vec_memset>)
     85e:	0943b2        	s8i	a11, a3, 9
     861:	01a9           	s32i.n	a10, a1, 0
     863:	0008e0        	callx8	a8
     866:	0c0c           	movi.n	a12, 0
     868:	a80c           	movi.n	a8, 10
     86a:	fdf971        	l32r	a7, 50 (82c <xa_sbc_synthesize_audio+0x170>)
     86d:	0b0c           	movi.n	a11, 0
     86f:	a0a092        	movi	a9, 160
     872:	08d3a2        	addmi	a10, a3, 0x800
     875:	24cad2        	addi	a13, a10, 36
     878:	886a92        	s32i	a9, a10, 0x220
     87b:	896a92        	s32i	a9, a10, 0x224
     87e:	11d9           	s32i.n	a13, a1, 4
     880:	091c           	movi.n	a9, 16
     882:	ab3a           	add.n	a10, a11, a3
     884:	aa7a           	add.n	a10, a10, a7
     886:	02daa2        	addmi	a10, a10, 0x200
     889:	fccaa2        	addi	a10, a10, -4

0000088c <xa_sbc_decode_init+0x3c>:
     88c:	cc1b           	addi.n	a12, a12, 1
     88e:	40cbb2        	addi	a11, a11, 64
     891:	078976        	loop	a9, 89c <xa_sbc_decode_init+0x4c>
     894:	1198e0        	slli	a9, a8, 2
     897:	09ea94        	ae_s32ip	a9, a10, 4
     89a:	88ab           	addi.n	a8, a8, 10

0000089c <xa_sbc_decode_init+0x4c>:
     89c:	a80c           	movi.n	a8, 10
     89e:	091c           	movi.n	a9, 16
     8a0:	ab3a           	add.n	a10, a11, a3
     8a2:	aa7a           	add.n	a10, a10, a7
     8a4:	02daa2        	addmi	a10, a10, 0x200
     8a7:	fccaa2        	addi	a10, a10, -4
     8aa:	de2c66        	bnei	a12, 2, 88c <xa_sbc_decode_init+0x3c>
     8ad:	02ad           	mov.n	a10, a2
     8af:	04cd           	mov.n	a12, a4
     8b1:	05dd           	mov.n	a13, a5
     8b3:	fde881        	l32r	a8, 54 (9cc <xa_sbc_decode>)
     8b6:	06ed           	mov.n	a14, a6
     8b8:	20b330        	or	a11, a3, a3
     8bb:	0008e0        	callx8	a8
     8be:	0a2d           	mov.n	a2, a10
     8c0:	090a96        	bltz	a10, 954 <xa_sbc_decode_init+0x104>
     8c3:	01a8           	l32i.n	a10, a1, 0
     8c5:	fde181        	l32r	a8, 4c (1248 <__vec_memset>)
     8c8:	0b0c           	movi.n	a11, 0
     8ca:	fddfc1        	l32r	a12, 48 (a00 <xa_sbc_decode+0x34>)
     8cd:	0008e0        	callx8	a8
     8d0:	0c03b2        	l8ui	a11, a3, 12
     8d3:	0c0c           	movi.n	a12, 0
     8d5:	11a8           	l32i.n	a10, a1, 4
     8d7:	a0bbb0        	addx4	a11, a11, a11
     8da:	11bbe0        	slli	a11, a11, 2
     8dd:	7f6ab2        	s32i	a11, a10, 0x1fc
     8e0:	0c0392        	l8ui	a9, a3, 12
     8e3:	a80c           	movi.n	a8, 10
     8e5:	0b0c           	movi.n	a11, 0
     8e7:	a09990        	addx4	a9, a9, a9
     8ea:	1199e0        	slli	a9, a9, 2
     8ed:	806a92        	s32i	a9, a10, 0x200
     8f0:	091c           	movi.n	a9, 16
     8f2:	ab3a           	add.n	a10, a11, a3
     8f4:	aa7a           	add.n	a10, a10, a7
     8f6:	02daa2        	addmi	a10, a10, 0x200

000008f9 <xa_sbc_decode_init+0xa9>:
     8f9:	fccaa2        	addi	a10, a10, -4
     8fc:	cc1b           	addi.n	a12, a12, 1
     8fe:	40cbb2        	addi	a11, a11, 64
     901:	078976        	loop	a9, 90c <xa_sbc_decode_init+0xbc>
     904:	1198e0        	slli	a9, a8, 2
     907:	09ea94        	ae_s32ip	a9, a10, 4
     90a:	88ab           	addi.n	a8, a8, 10

0000090c <xa_sbc_decode_init+0xbc>:
     90c:	a80c           	movi.n	a8, 10
     90e:	091c           	movi.n	a9, 16
     910:	ab3a           	add.n	a10, a11, a3
     912:	aa7a           	add.n	a10, a10, a7
     914:	02daa2        	addmi	a10, a10, 0x200
     917:	de2c66        	bnei	a12, 2, 8f9 <xa_sbc_decode_init+0xa9>
     91a:	43a8           	l32i.n	a10, a3, 16
     91c:	000392        	l8ui	a9, a3, 0
     91f:	0b0352        	l8ui	a5, a3, 11
     922:	0c0342        	l8ui	a4, a3, 12
     925:	f9ac           	beqz.n	a9, 958 <xa_sbc_decode_init+0x108>
     927:	4e1926        	beqi	a9, 1, 979 <xa_sbc_decode_init+0x129>
     92a:	672926        	beqi	a9, 2, 995 <xa_sbc_decode_init+0x145>
     92d:	fdc9b2        	addi	a11, a9, -3
     930:	07cb16        	beqz	a11, 9b0 <xa_sbc_decode_init+0x160>
     933:	0b0c           	movi.n	a11, 0

00000935 <xa_sbc_decode_init+0xe5>:
     935:	fdc881        	l32r	a8, 58 (1234 <__mulsi3hifi2>)
     938:	13a8           	l32i.n	a10, a3, 4
     93a:	0008e0        	callx8	a8
     93d:	fdc781        	l32r	a8, 5c (123c <__divsi3>)
     940:	05bd           	mov.n	a11, a5
     942:	11aad0        	slli	a10, a10, 3
     945:	0008e0        	callx8	a8
     948:	fdc581        	l32r	a8, 5c (123c <__divsi3>)
     94b:	04bd           	mov.n	a11, a4
     94d:	0008e0        	callx8	a8
     950:	53a9           	s32i.n	a10, a3, 20
     952:	f01d           	retw.n

00000954 <xa_sbc_decode_init+0x104>:
     954:	f01d           	retw.n

00000956 <xa_sbc_decode_init+0x106>:
	...

00000958 <xa_sbc_decode_init+0x108>:
     958:	fdc081        	l32r	a8, 58 (1234 <__mulsi3hifi2>)
     95b:	05bd           	mov.n	a11, a5
     95d:	0008e0        	callx8	a8
     960:	da7b           	addi.n	a13, a10, 7
     962:	caeb           	addi.n	a12, a10, 14
     964:	b41b           	addi.n	a11, a4, 1
     966:	b3b440        	movgez	a11, a4, a4
     969:	21b1b0        	srai	a11, a11, 1
     96c:	b3cdd0        	movgez	a12, a13, a13
     96f:	21c3c0        	srai	a12, a12, 3
     972:	bbca           	add.n	a11, a11, a12
     974:	bb4b           	addi.n	a11, a11, 4
     976:	ffeec6        	j	935 <xa_sbc_decode_init+0xe5>

00000979 <xa_sbc_decode_init+0x129>:
     979:	fdb781        	l32r	a8, 58 (1234 <__mulsi3hifi2>)
     97c:	05bd           	mov.n	a11, a5
     97e:	0008e0        	callx8	a8
     981:	7c0c           	movi.n	a12, 7
     983:	90cac0        	addx2	a12, a10, a12
     986:	bc7b           	addi.n	a11, a12, 7
     988:	b3bcc0        	movgez	a11, a12, a12
     98b:	21b3b0        	srai	a11, a11, 3
     98e:	b4ba           	add.n	a11, a4, a11
     990:	bb4b           	addi.n	a11, a11, 4
     992:	ffe7c6        	j	935 <xa_sbc_decode_init+0xe5>

00000995 <xa_sbc_decode_init+0x145>:
     995:	fdb081        	l32r	a8, 58 (1234 <__mulsi3hifi2>)
     998:	05bd           	mov.n	a11, a5
     99a:	0008e0        	callx8	a8
     99d:	ca7b           	addi.n	a12, a10, 7
     99f:	baeb           	addi.n	a11, a10, 14
     9a1:	b3bcc0        	movgez	a11, a12, a12
     9a4:	21b3b0        	srai	a11, a11, 3
     9a7:	b4ba           	add.n	a11, a4, a11
     9a9:	bb4b           	addi.n	a11, a11, 4
     9ab:	ffe186        	j	935 <xa_sbc_decode_init+0xe5>

000009ae <xa_sbc_decode_init+0x15e>:
	...

000009b0 <xa_sbc_decode_init+0x160>:
     9b0:	fdaa81        	l32r	a8, 58 (1234 <__mulsi3hifi2>)
     9b3:	05bd           	mov.n	a11, a5
     9b5:	0008e0        	callx8	a8
     9b8:	b4aa           	add.n	a11, a4, a10
     9ba:	cb7b           	addi.n	a12, a11, 7
     9bc:	bbeb           	addi.n	a11, a11, 14
     9be:	b3bcc0        	movgez	a11, a12, a12
     9c1:	21b3b0        	srai	a11, a11, 3
     9c4:	b4ba           	add.n	a11, a4, a11
     9c6:	bb4b           	addi.n	a11, a11, 4
     9c8:	ffda46        	j	935 <xa_sbc_decode_init+0xe5>

000009cb <xa_sbc_decode_init+0x17b>:
	...

000009cc <xa_sbc_decode>:
     9cc:	006136        	entry	a1, 48
     9cf:	04cd           	mov.n	a12, a4
     9d1:	06dd           	mov.n	a13, a6
     9d3:	fda381        	l32r	a8, 60 (e0 <xa_sbc_unpack_frame>)
     9d6:	02ad           	mov.n	a10, a2
     9d8:	03bd           	mov.n	a11, a3
     9da:	0008e0        	callx8	a8
     9dd:	0a4d           	mov.n	a4, a10
     9df:	551aa6        	blti	a10, 1, a38 <xa_sbc_decode+0x6c>
     9e2:	fda081        	l32r	a8, 64 (6bc <xa_sbc_synthesize_audio>)
     9e5:	02ad           	mov.n	a10, a2
     9e7:	03bd           	mov.n	a11, a3
     9e9:	0008e0        	callx8	a8
     9ec:	080362        	l8ui	a6, a3, 8
     9ef:	0d0c           	movi.n	a13, 0
     9f1:	0a7d           	mov.n	a7, a10
     9f3:	401600        	ssl	a6
     9f6:	1196f0        	slli	a9, a6, 1
     9f9:	0199           	s32i.n	a9, a1, 0
     9fb:	a18a00        	sll	a8, a10
     9fe:	c09590        	sub	a9, a5, a9
     a01:	0c5382        	s16i	a8, a3, 24
     a04:	46bc           	beqz.n	a6, a3c <xa_sbc_decode+0x70>
     a06:	592b           	addi.n	a5, a9, 2
     a08:	04d2c2        	addmi	a12, a2, 0x400
     a0b:	3eccc2        	addi	a12, a12, 62

00000a0e <xa_sbc_decode+0x42>:
     a0e:	dd1b           	addi.n	a13, a13, 1
     a10:	e78c           	beqz.n	a7, a22 <xa_sbc_decode+0x56>
     a12:	0c8d           	mov.n	a8, a12
     a14:	01a8           	l32i.n	a10, a1, 0
     a16:	05a776        	loopgtz	a7, a1f <xa_sbc_decode+0x53>
     a19:	408814        	ae_lp16f.iu	aep0, a8, 2
     a1c:	4389a4        	ae_sp16f.l.xu	aep0, a9, a10

00000a1f <xa_sbc_decode+0x53>:
     a1f:	080362        	l8ui	a6, a3, 8

00000a22 <xa_sbc_decode+0x56>:
     a22:	01dcc2        	addmi	a12, a12, 0x100
     a25:	059d           	mov.n	a9, a5
     a27:	e33d67        	bltu	a13, a6, a0e <xa_sbc_decode+0x42>
     a2a:	042d           	mov.n	a2, a4
     a2c:	401600        	ssl	a6
     a2f:	a1d700        	sll	a13, a7
     a32:	0d53d2        	s16i	a13, a3, 26
     a35:	f01d           	retw.n

00000a37 <xa_sbc_decode+0x6b>:
	...

00000a38 <xa_sbc_decode+0x6c>:
     a38:	0a2d           	mov.n	a2, a10
     a3a:	f01d           	retw.n

00000a3c <xa_sbc_decode+0x70>:
     a3c:	042d           	mov.n	a2, a4
     a3e:	0d5382        	s16i	a8, a3, 26
     a41:	f01d           	retw.n

00000a43 <xa_sbc_decode+0x77>:
	...

00000a44 <xa_sbc_dec>:
     a44:	006136        	entry	a1, 48
     a47:	370c           	movi.n	a7, 3
     a49:	082326        	beqi	a3, 2, a55 <xa_sbc_dec+0x11>
     a4c:	051326        	beqi	a3, 1, a55 <xa_sbc_dec+0x11>
     a4f:	047216        	beqz	a2, a9a <xa_sbc_dec+0x56>
     a52:	518727        	bany	a7, a2, aa7 <xa_sbc_dec+0x63>
     a55:	53b326        	beqi	a3, 16, aac <xa_sbc_dec+0x68>
     a58:	181c           	movi.n	a8, 17
     a5a:	4e1387        	beq	a3, a8, aac <xa_sbc_dec+0x68>
     a5d:	291c           	movi.n	a9, 18
     a5f:	491397        	beq	a3, a9, aac <xa_sbc_dec+0x68>
     a62:	4a1c           	movi.n	a10, 20
     a64:	4413a7        	beq	a3, a10, aac <xa_sbc_dec+0x68>
     a67:	5b1c           	movi.n	a11, 21
     a69:	3f13b7        	beq	a3, a11, aac <xa_sbc_dec+0x68>
     a6c:	0a0c           	movi.n	a10, 0
     a6e:	160c           	movi.n	a6, 1
     a70:	00a2b2        	movi	a11, 0x200
     a73:	9d1c           	movi.n	a13, 25
     a75:	12b3d7        	bgeu	a3, a13, a8b <xa_sbc_dec+0x47>
     a78:	fd7ce1        	l32r	a14, 68 (1b60 <_DYNAMIC+0x80c>)
     a7b:	fd7cf1        	l32r	a15, 6c (a44 <xa_sbc_dec>)
     a7e:	a0e3e0        	addx4	a14, a3, a14
     a81:	0ee8           	l32i.n	a14, a14, 0
     a83:	ffd492        	addmi	a9, a4, 0xffffff00
     a86:	eefa           	add.n	a14, a14, a15
     a88:	000ea0        	jx	a14

00000a8b <xa_sbc_dec+0x47>:
     a8b:	fd7921        	l32r	a2, 70 (ffff8002 <_end+0xffff5f0e>)
     a8e:	f01d           	retw.n

00000a90 <xa_sbc_dec+0x4c>:
     a90:	22a8           	l32i.n	a10, a2, 8

00000a92 <xa_sbc_dec+0x4e>:
     a92:	05a9           	s32i.n	a10, a5, 0

00000a94 <xa_sbc_dec+0x50>:
     a94:	020c           	movi.n	a2, 0
     a96:	f01d           	retw.n

00000a98 <xa_sbc_dec+0x54>:
     a98:	35cc           	bnez.n	a5, a9f <xa_sbc_dec+0x5b>

00000a9a <xa_sbc_dec+0x56>:
     a9a:	fd7621        	l32r	a2, 74 (ffff8000 <_end+0xffff5f0c>)
     a9d:	f01d           	retw.n

00000a9f <xa_sbc_dec+0x5b>:
     a9f:	01a9           	s32i.n	a10, a1, 0
     aa1:	028757        	bany	a7, a5, aa7 <xa_sbc_dec+0x63>
     aa4:	002986        	j	b4e <xa_sbc_dec+0x10a>

00000aa7 <xa_sbc_dec+0x63>:
     aa7:	fd7421        	l32r	a2, 78 (ffff8001 <_end+0xffff5f0d>)
     aaa:	f01d           	retw.n

00000aac <xa_sbc_dec+0x68>:
     aac:	02ad           	mov.n	a10, a2
     aae:	03bd           	mov.n	a11, a3
     ab0:	fd7381        	l32r	a8, 7c (c40 <xa_sbc_dec_mem_api>)
     ab3:	04cd           	mov.n	a12, a4
     ab5:	05dd           	mov.n	a13, a5
     ab7:	0008e0        	callx8	a8
     aba:	0a2d           	mov.n	a2, a10
     abc:	f01d           	retw.n

00000abe <xa_sbc_dec+0x7a>:
     abe:	0a8916        	beqz	a9, b6a <xa_sbc_dec+0x126>
     ac1:	fed4a2        	addmi	a10, a4, 0xfffffe00
     ac4:	109a16        	beqz	a10, bd1 <xa_sbc_dec+0x18d>
     ac7:	fdd4b2        	addmi	a11, a4, 0xfffffd00
     aca:	096b16        	beqz	a11, b64 <xa_sbc_dec+0x120>

00000acd <xa_sbc_dec+0x89>:
     acd:	fd6c21        	l32r	a2, 80 (ffff8003 <_end+0xffff5f0f>)
     ad0:	f01d           	retw.n

00000ad2 <xa_sbc_dec+0x8e>:
     ad2:	0269           	s32i.n	a6, a2, 0
     ad4:	ffef06        	j	a94 <xa_sbc_dec+0x50>

00000ad7 <xa_sbc_dec+0x93>:
     ad7:	05c8           	l32i.n	a12, a5, 0
     ad9:	12c9           	s32i.n	a12, a2, 4
     adb:	ffed46        	j	a94 <xa_sbc_dec+0x50>

00000ade <xa_sbc_dec+0x9a>:
     ade:	32d8           	l32i.n	a13, a2, 12
     ae0:	05d9           	s32i.n	a13, a5, 0
     ae2:	ffeb86        	j	a94 <xa_sbc_dec+0x50>

00000ae5 <xa_sbc_dec+0xa1>:
     ae5:	0569           	s32i.n	a6, a5, 0
     ae7:	ffea46        	j	a94 <xa_sbc_dec+0x50>

00000aea <xa_sbc_dec+0xa6>:
     aea:	ffd4e2        	addmi	a14, a4, 0xffffff00
     aed:	0a9e16        	beqz	a14, b9a <xa_sbc_dec+0x156>
     af0:	d994b7        	bne	a4, a11, acd <xa_sbc_dec+0x89>
     af3:	05a9           	s32i.n	a10, a5, 0
     af5:	02f8           	l32i.n	a15, a2, 0
     af7:	f99f16        	beqz	a15, a94 <xa_sbc_dec+0x50>
     afa:	0569           	s32i.n	a6, a5, 0
     afc:	ffe506        	j	a94 <xa_sbc_dec+0x50>

00000aff <xa_sbc_dec+0xbb>:
     aff:	480c           	movi.n	a8, 4
     b01:	0589           	s32i.n	a8, a5, 0
     b03:	ffe346        	j	a94 <xa_sbc_dec+0x50>

00000b06 <xa_sbc_dec+0xc2>:
     b06:	095c           	movi.n	a9, 80
     b08:	0599           	s32i.n	a9, a5, 0
     b0a:	ffe186        	j	a94 <xa_sbc_dec+0x50>

00000b0d <xa_sbc_dec+0xc9>:
     b0d:	07b416        	beqz	a4, b8c <xa_sbc_dec+0x148>
     b10:	7d1426        	beqi	a4, 1, b91 <xa_sbc_dec+0x14d>
     b13:	fdc4a2        	addi	a10, a4, -3
     b16:	0bda16        	beqz	a10, bd7 <xa_sbc_dec+0x193>
     b19:	b02466        	bnei	a4, 2, acd <xa_sbc_dec+0x89>
     b1c:	5238           	l32i.n	a3, a2, 20
     b1e:	0338           	l32i.n	a3, a3, 0
     b20:	080332        	l8ui	a3, a3, 8

00000b23 <xa_sbc_dec+0xdf>:
     b23:	03ad           	mov.n	a10, a3
     b25:	ffda46        	j	a92 <xa_sbc_dec+0x4e>

00000b28 <xa_sbc_dec+0xe4>:
     b28:	01a9           	s32i.n	a10, a1, 0
     b2a:	ffd482        	addmi	a8, a4, 0xffffff00
     b2d:	0c4816        	beqz	a8, bf5 <xa_sbc_dec+0x1b1>
     b30:	fed492        	addmi	a9, a4, 0xfffffe00
     b33:	0b3916        	beqz	a9, bea <xa_sbc_dec+0x1a6>
     b36:	fdd4b2        	addmi	a11, a4, 0xfffffd00
     b39:	0cbb16        	beqz	a11, c08 <xa_sbc_dec+0x1c4>
     b3c:	00a4c2        	movi	a12, 0x400
     b3f:	8a94c7        	bne	a4, a12, acd <xa_sbc_dec+0x89>
     b42:	0569           	s32i.n	a6, a5, 0
     b44:	ffd306        	j	a94 <xa_sbc_dec+0x50>

00000b47 <xa_sbc_dec+0x103>:
     b47:	8d1c           	movi.n	a13, 24
     b49:	05d9           	s32i.n	a13, a5, 0
     b4b:	ffd146        	j	a94 <xa_sbc_dec+0x50>

00000b4e <xa_sbc_dec+0x10a>:
     b4e:	4259           	s32i.n	a5, a2, 16
     b50:	05ad           	mov.n	a10, a5
     b52:	01b8           	l32i.n	a11, a1, 0
     b54:	0c5c           	movi.n	a12, 80
     b56:	fd3d81        	l32r	a8, 4c (1248 <__vec_memset>)
     b59:	40c592        	addi	a9, a5, 64
     b5c:	5299           	s32i.n	a9, a2, 20
     b5e:	0008e0        	callx8	a8
     b61:	ffcbc6        	j	a94 <xa_sbc_dec+0x50>

00000b64 <xa_sbc_dec+0x120>:
     b64:	fd4891        	l32r	a9, 84 (20c0 <export_parameter_array+0x10>)
     b67:	000086        	j	b6d <xa_sbc_dec+0x129>

00000b6a <xa_sbc_dec+0x126>:
     b6a:	fd4791        	l32r	a9, 88 (20d0 <export_parameter_array+0x20>)

00000b6d <xa_sbc_dec+0x129>:
     b6d:	80c9b2        	addi	a11, a9, -128
     b70:	9dc9a2        	addi	a10, a9, -99

00000b73 <xa_sbc_dec+0x12f>:
     b73:	800bc2        	l8ui	a12, a11, 128
     b76:	0045c2        	s8i	a12, a5, 0
     b79:	bb1b           	addi.n	a11, a11, 1
     b7b:	551b           	addi.n	a5, a5, 1
     b7d:	02aab7        	bge	a10, a11, b83 <xa_sbc_dec+0x13f>
     b80:	ffc406        	j	a94 <xa_sbc_dec+0x50>
     b83:	7f0bd2        	l8ui	a13, a11, 127
     b86:	fe9d56        	bnez	a13, b73 <xa_sbc_dec+0x12f>
     b89:	ffc1c6        	j	a94 <xa_sbc_dec+0x50>

00000b8c <xa_sbc_dec+0x148>:
     b8c:	031c           	movi.n	a3, 16
     b8e:	ffe446        	j	b23 <xa_sbc_dec+0xdf>

00000b91 <xa_sbc_dec+0x14d>:
     b91:	5238           	l32i.n	a3, a2, 20
     b93:	0338           	l32i.n	a3, a3, 0
     b95:	1338           	l32i.n	a3, a3, 4
     b97:	ffe206        	j	b23 <xa_sbc_dec+0xdf>

00000b9a <xa_sbc_dec+0x156>:
     b9a:	12e8           	l32i.n	a14, a2, 4
     b9c:	52d8           	l32i.n	a13, a2, 20
     b9e:	32a9           	s32i.n	a10, a2, 12
     ba0:	22a9           	s32i.n	a10, a2, 8
     ba2:	0d38           	l32i.n	a3, a13, 0
     ba4:	2dc8           	l32i.n	a12, a13, 8
     ba6:	1da8           	l32i.n	a10, a13, 4
     ba8:	3dd8           	l32i.n	a13, a13, 12
     baa:	ee6e16        	beqz	a14, a94 <xa_sbc_dec+0x50>
     bad:	fd2981        	l32r	a8, 54 (9cc <xa_sbc_decode>)
     bb0:	03bd           	mov.n	a11, a3
     bb2:	0008e0        	callx8	a8
     bb5:	027ad6        	bgez	a10, be0 <xa_sbc_dec+0x19c>
     bb8:	6c0a26        	beqi	a10, -1, c28 <xa_sbc_dec+0x1e4>
     bbb:	e97c           	movi.n	a9, -2
     bbd:	6c1a97        	beq	a10, a9, c2d <xa_sbc_dec+0x1e9>
     bc0:	db7c           	movi.n	a11, -3
     bc2:	6e1ab7        	beq	a10, a11, c34 <xa_sbc_dec+0x1f0>
     bc5:	cc7c           	movi.n	a12, -4
     bc7:	159ac7        	bne	a10, a12, be0 <xa_sbc_dec+0x19c>
     bca:	3279           	s32i.n	a7, a2, 12
     bcc:	fd3021        	l32r	a2, 8c (1442 <_DYNAMIC+0xee>)
     bcf:	f01d           	retw.n

00000bd1 <xa_sbc_dec+0x18d>:
     bd1:	fd2f91        	l32r	a9, 90 (20c8 <export_parameter_array+0x18>)
     bd4:	ffe546        	j	b6d <xa_sbc_dec+0x129>

00000bd7 <xa_sbc_dec+0x193>:
     bd7:	5238           	l32i.n	a3, a2, 20
     bd9:	0338           	l32i.n	a3, a3, 0
     bdb:	5338           	l32i.n	a3, a3, 20
     bdd:	ffd086        	j	b23 <xa_sbc_dec+0xdf>

00000be0 <xa_sbc_dec+0x19c>:
     be0:	32a9           	s32i.n	a10, a2, 12
     be2:	0d9382        	l16si	a8, a3, 26
     be5:	2289           	s32i.n	a8, a2, 8
     be7:	ffaa46        	j	a94 <xa_sbc_dec+0x50>

00000bea <xa_sbc_dec+0x1a6>:
     bea:	fd2a81        	l32r	a8, 94 (cd0 <xa_sbc_dec_fill_mem_tables>)
     bed:	02ad           	mov.n	a10, a2
     bef:	0008e0        	callx8	a8
     bf2:	ffa786        	j	a94 <xa_sbc_dec+0x50>

00000bf5 <xa_sbc_dec+0x1b1>:
     bf5:	02ad           	mov.n	a10, a2
     bf7:	0138           	l32i.n	a3, a1, 0
     bf9:	fd1481        	l32r	a8, 4c (1248 <__vec_memset>)
     bfc:	8c1c           	movi.n	a12, 24
     bfe:	03bd           	mov.n	a11, a3
     c00:	0008e0        	callx8	a8
     c03:	0239           	s32i.n	a3, a2, 0
     c05:	ffa2c6        	j	a94 <xa_sbc_dec+0x50>

00000c08 <xa_sbc_dec+0x1c4>:
     c08:	12e8           	l32i.n	a14, a2, 4
     c0a:	fd2381        	l32r	a8, 98 (850 <xa_sbc_decode_init>)
     c0d:	52c8           	l32i.n	a12, a2, 20
     c0f:	0add           	mov.n	a13, a10
     c11:	32d9           	s32i.n	a13, a2, 12
     c13:	22d9           	s32i.n	a13, a2, 8
     c15:	0cb8           	l32i.n	a11, a12, 0
     c17:	1ca8           	l32i.n	a10, a12, 4
     c19:	3cd8           	l32i.n	a13, a12, 12
     c1b:	2cc8           	l32i.n	a12, a12, 8
     c1d:	0008e0        	callx8	a8
     c20:	e70ad6        	bgez	a10, a94 <xa_sbc_dec+0x50>
     c23:	fd1e21        	l32r	a2, 9c (ffff9440 <_end+0xffff734c>)
     c26:	f01d           	retw.n

00000c28 <xa_sbc_dec+0x1e4>:
     c28:	fd1e21        	l32r	a2, a0 (1443 <_DYNAMIC+0xef>)
     c2b:	f01d           	retw.n

00000c2d <xa_sbc_dec+0x1e9>:
     c2d:	3269           	s32i.n	a6, a2, 12
     c2f:	fd1d21        	l32r	a2, a4 (1440 <_DYNAMIC+0xec>)
     c32:	f01d           	retw.n

00000c34 <xa_sbc_dec+0x1f0>:
     c34:	5e0c           	movi.n	a14, 5
     c36:	32e9           	s32i.n	a14, a2, 12
     c38:	fd1c21        	l32r	a2, a8 (1441 <_DYNAMIC+0xed>)
     c3b:	f01d           	retw.n

00000c3d <xa_sbc_dec+0x1f9>:
     c3d:	000000                                        ...

00000c40 <xa_sbc_dec_mem_api>:
     c40:	004136        	entry	a1, 32
     c43:	020496        	bltz	a4, c67 <xa_sbc_dec_mem_api+0x27>
     c46:	1d44e6        	bgei	a4, 4, c67 <xa_sbc_dec_mem_api+0x27>
     c49:	2eb326        	beqi	a3, 16, c7b <xa_sbc_dec_mem_api+0x3b>
     c4c:	181c           	movi.n	a8, 17
     c4e:	351387        	beq	a3, a8, c87 <xa_sbc_dec_mem_api+0x47>
     c51:	291c           	movi.n	a9, 18
     c53:	4f1397        	beq	a3, a9, ca6 <xa_sbc_dec_mem_api+0x66>
     c56:	4a1c           	movi.n	a10, 20
     c58:	1013a7        	beq	a3, a10, c6c <xa_sbc_dec_mem_api+0x2c>
     c5b:	5b1c           	movi.n	a11, 21
     c5d:	1693b7        	bne	a3, a11, c77 <xa_sbc_dec_mem_api+0x37>
     c60:	f5ec           	bnez.n	a5, c93 <xa_sbc_dec_mem_api+0x53>
     c62:	fd0421        	l32r	a2, 74 (ffff8000 <_end+0xffff5f0c>)
     c65:	f01d           	retw.n

00000c67 <xa_sbc_dec_mem_api+0x27>:
     c67:	fd0621        	l32r	a2, 80 (ffff8003 <_end+0xffff5f0f>)
     c6a:	f01d           	retw.n

00000c6c <xa_sbc_dec_mem_api+0x2c>:
     c6c:	4298           	l32i.n	a9, a2, 16
     c6e:	11a4c0        	slli	a10, a4, 4
     c71:	99aa           	add.n	a9, a9, a10
     c73:	3998           	l32i.n	a9, a9, 12

00000c75 <xa_sbc_dec_mem_api+0x35>:
     c75:	0599           	s32i.n	a9, a5, 0

00000c77 <xa_sbc_dec_mem_api+0x37>:
     c77:	020c           	movi.n	a2, 0
     c79:	f01d           	retw.n

00000c7b <xa_sbc_dec_mem_api+0x3b>:
     c7b:	4298           	l32i.n	a9, a2, 16
     c7d:	11a4c0        	slli	a10, a4, 4
     c80:	99aa           	add.n	a9, a9, a10
     c82:	0998           	l32i.n	a9, a9, 0
     c84:	fffb46        	j	c75 <xa_sbc_dec_mem_api+0x35>

00000c87 <xa_sbc_dec_mem_api+0x47>:
     c87:	4298           	l32i.n	a9, a2, 16
     c89:	11a4c0        	slli	a10, a4, 4
     c8c:	99aa           	add.n	a9, a9, a10
     c8e:	1998           	l32i.n	a9, a9, 4
     c90:	fff846        	j	c75 <xa_sbc_dec_mem_api+0x35>

00000c93 <xa_sbc_dec_mem_api+0x53>:
     c93:	42b8           	l32i.n	a11, a2, 16
     c95:	11c4c0        	slli	a12, a4, 4
     c98:	bbca           	add.n	a11, a11, a12
     c9a:	1bb8           	l32i.n	a11, a11, 4
     c9c:	bb0b           	addi.n	a11, a11, -1
     c9e:	100b57        	bnone	a11, a5, cb2 <xa_sbc_dec_mem_api+0x72>
     ca1:	fcf521        	l32r	a2, 78 (ffff8001 <_end+0xffff5f0d>)
     ca4:	f01d           	retw.n

00000ca6 <xa_sbc_dec_mem_api+0x66>:
     ca6:	4298           	l32i.n	a9, a2, 16
     ca8:	11a4c0        	slli	a10, a4, 4
     cab:	99aa           	add.n	a9, a9, a10
     cad:	2998           	l32i.n	a9, a9, 8
     caf:	fff086        	j	c75 <xa_sbc_dec_mem_api+0x35>

00000cb2 <xa_sbc_dec_mem_api+0x72>:
     cb2:	52b8           	l32i.n	a11, a2, 20
     cb4:	a0b4b0        	addx4	a11, a4, a11
     cb7:	0b59           	s32i.n	a5, a11, 0
     cb9:	fba456        	bnez	a4, c77 <xa_sbc_dec_mem_api+0x37>
     cbc:	05ad           	mov.n	a10, a5
     cbe:	42d8           	l32i.n	a13, a2, 16
     cc0:	0b0c           	movi.n	a11, 0
     cc2:	fce281        	l32r	a8, 4c (1248 <__vec_memset>)
     cc5:	cdca           	add.n	a12, a13, a12
     cc7:	0cc8           	l32i.n	a12, a12, 0
     cc9:	0008e0        	callx8	a8
     ccc:	ffe9c6        	j	c77 <xa_sbc_dec_mem_api+0x37>

00000ccf <xa_sbc_dec_mem_api+0x8f>:
	...

00000cd0 <xa_sbc_dec_fill_mem_tables>:
     cd0:	004136        	entry	a1, 32
     cd3:	330c           	movi.n	a3, 3
     cd5:	c0a682        	movi	a8, 0x6c0
     cd8:	fcf591        	l32r	a9, ac (aa8 <xa_sbc_dec+0x64>)
     cdb:	2a0c           	movi.n	a10, 2
     cdd:	00a2b2        	movi	a11, 0x200
     ce0:	8d0c           	movi.n	a13, 8
     ce2:	4248           	l32i.n	a4, a2, 16
     ce4:	1c0c           	movi.n	a12, 1
     ce6:	020c           	movi.n	a2, 0
     ce8:	64c9           	s32i.n	a12, a4, 24
     cea:	94c9           	s32i.n	a12, a4, 36
     cec:	84b9           	s32i.n	a11, a4, 32
     cee:	c4b9           	s32i.n	a11, a4, 48
     cf0:	a4a9           	s32i.n	a10, a4, 40
     cf2:	d4a9           	s32i.n	a10, a4, 52
     cf4:	0499           	s32i.n	a9, a4, 0
     cf6:	4489           	s32i.n	a8, a4, 16
     cf8:	e439           	s32i.n	a3, a4, 56
     cfa:	2429           	s32i.n	a2, a4, 8
     cfc:	3429           	s32i.n	a2, a4, 12
     cfe:	7429           	s32i.n	a2, a4, 28
     d00:	b429           	s32i.n	a2, a4, 44
     d02:	f429           	s32i.n	a2, a4, 60
     d04:	14d9           	s32i.n	a13, a4, 4
     d06:	54d9           	s32i.n	a13, a4, 20
     d08:	f01d           	retw.n

00000d0a <xa_sbc_dec_fill_mem_tables+0x3a>:
     d0a:	00000000 41360000                                ......

00000d10 <xa_sbc_crc8>:
     d10:	004136        	entry	a1, 32
     d13:	02ad           	mov.n	a10, a2
     d15:	039d           	mov.n	a9, a3
     d17:	21b330        	srai	a11, a3, 3
     d1a:	781ba6        	blti	a11, 1, d96 <xa_sbc_crc8+0x86>
     d1d:	fce471        	l32r	a7, b0 (1bd0 <_DYNAMIC+0x87c>)
     d20:	f40c           	movi.n	a4, 15
     d22:	2161b0        	srai	a6, a11, 1
     d25:	0b6337        	bbci	a3, 3, d34 <xa_sbc_crc8+0x24>
     d28:	0258           	l32i.n	a5, a2, 0
     d2a:	224b           	addi.n	a2, a2, 4
     d2c:	304540        	xor	a4, a5, a4
     d2f:	a04470        	addx4	a4, a4, a7
     d32:	0448           	l32i.n	a4, a4, 0

00000d34 <xa_sbc_crc8+0x24>:
     d34:	06bc           	beqz.n	a6, d68 <xa_sbc_crc8+0x58>
     d36:	0258           	l32i.n	a5, a2, 0
     d38:	1288           	l32i.n	a8, a2, 4
     d3a:	360b           	addi.n	a3, a6, -1
     d3c:	228b           	addi.n	a2, a2, 8
     d3e:	305540        	xor	a5, a5, a4
     d41:	a05570        	addx4	a5, a5, a7
     d44:	0558           	l32i.n	a5, a5, 0
     d46:	15a376        	loopgtz	a3, d5f <xa_sbc_crc8+0x4f>
     d49:	0238           	l32i.n	a3, a2, 0
     d4b:	304850        	xor	a4, a8, a5
     d4e:	a04470        	addx4	a4, a4, a7
     d51:	0448           	l32i.n	a4, a4, 0
     d53:	1288           	l32i.n	a8, a2, 4
     d55:	228b           	addi.n	a2, a2, 8
     d57:	303340        	xor	a3, a3, a4
     d5a:	a03370        	addx4	a3, a3, a7
     d5d:	0358           	l32i.n	a5, a3, 0

00000d5f <xa_sbc_crc8+0x4f>:
     d5f:	304850        	xor	a4, a8, a5
     d62:	a04470        	addx4	a4, a4, a7
     d65:	002442        	l32i	a4, a4, 0

00000d68 <xa_sbc_crc8+0x58>:
     d68:	d71c           	movi.n	a7, 29
     d6a:	242090        	extui	a2, a9, 0, 3
     d6d:	060c           	movi.n	a6, 0
     d6f:	533b60        	max	a3, a11, a6
     d72:	a033a0        	addx4	a3, a3, a10
     d75:	0338           	l32i.n	a3, a3, 0
     d77:	16a276        	loopgtz	a2, d91 <xa_sbc_crc8+0x81>
     d7a:	062d           	mov.n	a2, a6
     d7c:	305340        	xor	a5, a3, a4
     d7f:	644040        	extui	a4, a4, 0, 7
     d82:	1144f0        	slli	a4, a4, 1
     d85:	045750        	extui	a5, a5, 7, 1
     d88:	1133f0        	slli	a3, a3, 1
     d8b:	932750        	movnez	a2, a7, a5
     d8e:	304240        	xor	a4, a2, a4

00000d91 <xa_sbc_crc8+0x81>:
     d91:	742040        	extui	a2, a4, 0, 8
     d94:	f01d           	retw.n

00000d96 <xa_sbc_crc8+0x86>:
     d96:	f40c           	movi.n	a4, 15
     d98:	fff306        	j	d68 <xa_sbc_crc8+0x58>

00000d9b <xa_sbc_crc8+0x8b>:
	...

00000d9c <xa_sbc_calculate_bits>:
     d9c:	00c136        	entry	a1, 96
     d9f:	028d           	mov.n	a8, a2
     da1:	05ad           	mov.n	a10, a5
     da3:	0c0372        	l8ui	a7, a3, 12
     da6:	039d           	mov.n	a9, a3
     da8:	d169           	s32i.n	a6, a1, 52
     daa:	fcc772        	addi	a7, a7, -4
     dad:	457716        	beqz	a7, 1208 <xa_sbc_calculate_bits+0x46c>
     db0:	fcc1b1        	l32r	a11, b4 (2010 <_DYNAMIC+0xcbc>)
     db3:	11c4b0        	slli	a12, a4, 5
     db6:	bbca           	add.n	a11, a11, a12

00000db8 <xa_sbc_calculate_bits+0x1c>:
     db8:	40a6c2        	movi	a12, 0x640
     dbb:	2e0c           	movi.n	a14, 2
     dbd:	1f0c           	movi.n	a15, 1
     dbf:	7ca632        	movi	a3, 0x67c
     dc2:	78a6d2        	movi	a13, 0x678
     dc5:	000922        	l8ui	a2, a9, 0
     dc8:	fcc872        	addi	a7, a8, -4
     dcb:	b179           	s32i.n	a7, a1, 44
     dcd:	215216        	beqz	a2, fe6 <xa_sbc_calculate_bits+0x24a>
     dd0:	720b           	addi.n	a7, a2, -1
     dd2:	210716        	beqz	a7, fe6 <xa_sbc_calculate_bits+0x24a>
     dd5:	042226        	beqi	a2, 2, ddd <xa_sbc_calculate_bits+0x41>
     dd8:	013226        	beqi	a2, 3, ddd <xa_sbc_calculate_bits+0x41>

00000ddb <xa_sbc_calculate_bits+0x3f>:
     ddb:	f01d           	retw.n

00000ddd <xa_sbc_calculate_bits+0x41>:
     ddd:	783a           	add.n	a7, a8, a3
     ddf:	a179           	s32i.n	a7, a1, 40
     de1:	0a0972        	l8ui	a7, a9, 10
     de4:	770b           	addi.n	a7, a7, -1
     de6:	42b716        	beqz	a7, 1215 <xa_sbc_calculate_bits+0x479>
     de9:	a138           	l32i.n	a3, a1, 40
     deb:	fccbb2        	addi	a11, a11, -4
     dee:	81b9           	s32i.n	a11, a1, 32
     df0:	0b0c           	movi.n	a11, 0

00000df2 <xa_sbc_calculate_bits+0x56>:
     df2:	b178           	l32i.n	a7, a1, 44
     df4:	b27c           	movi.n	a2, -5
     df6:	36c234        	ae_cvtq48a32s	aeq3, a2
     df9:	fe24b0812e 	{ l32i	a2, a1, 32; ae_zeroq56	aeq2 }
     dfe:	189a76        	loopnez	a10, e1a <xa_sbc_calculate_bits+0x7e>
     e01:	605714        	ae_lq32f.iu	aeq1, a7, 4
     e04:	d4547d121f 	{ ae_lq32f.iu	aeq0, a2, 4; ae_eqq56	b1, aeq1, aeq2 }
     e09:	015cc4        	ae_subq56	aeq1, aeq1, aeq0
     e0c:	d6c83b101f 	{ ae_sraiq56	aeq0, aeq1, 1; ae_ltq56s	b0, aeq2, aeq1 }
     e11:	344014        	ae_movtq56	aeq1, aeq0, b0
     e14:	347114        	ae_movtq56	aeq1, aeq3, b1
     e17:	625314        	ae_sq32f.iu	aeq1, a3, 4

00000e1a <xa_sbc_calculate_bits+0x7e>:
     e1a:	b179           	s32i.n	a7, a1, 44
     e1c:	bb1b           	addi.n	a11, a11, 1
     e1e:	d02b66        	bnei	a11, 2, df2 <xa_sbc_calculate_bits+0x56>
     e21:	d1b8           	l32i.n	a11, a1, 52
     e23:	404b16        	beqz	a11, 122b <xa_sbc_calculate_bits+0x48f>

00000e26 <xa_sbc_calculate_bits+0x8a>:
     e26:	a128           	l32i.n	a2, a1, 40
     e28:	d138           	l32i.n	a3, a1, 52
     e2a:	0020f0        	nop
     e2d:	601214        	ae_lq32f.iu	aeq0, a2, 4
     e30:	fd248ff33e 	{ addi	a3, a3, -1; ae_zeroq56	aeq1 }
     e35:	049376        	loopnez	a3, e3d <xa_sbc_calculate_bits+0xa1>
     e38:	f9673d121f 	{ ae_lq32f.iu	aeq0, a2, 4; ae_maxq56s	aeq1, aeq1, aeq0 }

00000e3d <xa_sbc_calculate_bits+0xa1>:
     e3d:	0101c4        	ae_maxq56s	aeq0, aeq1, aeq0

00000e40 <xa_sbc_calculate_bits+0xa4>:
     e40:	68da           	add.n	a6, a8, a13
     e42:	368b04        	ae_trunca32q48	a11, aeq0
     e45:	4948           	l32i.n	a4, a9, 16
     e47:	305ff4        	ae_cvtp24a16x2.ll	aep5, a15, a15
     e4a:	7bfb           	addi.n	a7, a11, 15
     e4c:	fd24801bbe 	{ addi	a11, a11, 1; ae_zeroq56	aeq1 }
     e51:	ce5458877f 	{ ae_cvtp24a16x2.ll	aep0, a7, a7; ae_sraip24	aep5, aep5, 8 }
     e56:	cc4018ebbf 	{ ae_cvtp24a16x2.ll	aep6, a11, a11; ae_sraip24	aep0, aep0, 8 }
     e5b:	cf583ec4bf 	{ ae_cvtq48a32s	aeq2, a4; ae_sraip24	aep6, aep6, 8 }

00000e60 <xa_sbc_calculate_bits+0xc4>:
     e60:	fe2093266f 	{ or	a2, a6, a6; ae_zerop48	aep4 }
     e65:	05aa16        	beqz	a10, ec3 <xa_sbc_calculate_bits+0x127>
     e68:	549214        	ae_lp24x2.iu	aep1, a2, 8
     e6b:	d23818feef 	{ ae_cvtp24a16x2.ll	aep7, a14, a14; ae_eqp24	b0:b1, aep6, aep1 }
     e70:	cf5c77010f 	{ ae_sp24x2s.i	aep0, a1, 0; ae_sraip24	aep7, aep7, 8 }
     e75:	d23970310f 	{ ae_lp24x2.i	aep3, a1, 0; ae_ltp24s	b4:b5, aep6, aep1 }
     e7a:	d22cff40bf 	{ ae_movp48	aep2, aep4; ae_ltp24s	b2:b3, aep3, aep1 }
     e7f:	e0a93f403f 	{ ae_movp48	aep0, aep4; ae_movtp24x2	aep2, aep5, b4:b5 }
     e84:	188704        	ae_movtp24x2	aep0, aep7, b0:b1
     e87:	e0888ffa3e 	{ addi	a3, a10, -1; ae_movtp24x2	aep2, aep4, b2:b3 }
     e8c:	d2a8bf40ff 	{ ae_movp48	aep3, aep4; ae_eqp24	b2:b3, aep2, aep5 }
     e91:	f03d           	nop.n
     e93:	f03d           	nop.n
     e95:	229376        	loopnez	a3, ebb <xa_sbc_calculate_bits+0x11f>
     e98:	e0a0b0a21f 	{ ae_lp24x2.iu	aep2, a2, 8; ae_movtp24x2	aep0, aep5, b2:b3 }
     e9d:	d90c70110f 	{ ae_lp24x2.i	aep1, a1, 0; ae_addp24	aep3, aep3, aep0 }
     ea2:	d258ff403f 	{ ae_movp48	aep0, aep4; ae_ltp24s	b2:b3, aep6, aep2 }
     ea7:	d2457f007f 	{ ae_movp48	aep1, aep0; ae_ltp24s	b4:b5, aep1, aep2 }
     eac:	19ea04        	ae_eqp24	b0:b1, aep6, aep2
     eaf:	189524        	ae_movtp24x2	aep1, aep5, b2:b3
     eb2:	189444        	ae_movtp24x2	aep1, aep4, b4:b5
     eb5:	199d24        	ae_eqp24	b2:b3, aep1, aep5
     eb8:	188704        	ae_movtp24x2	aep0, aep7, b0:b1

00000ebb <xa_sbc_calculate_bits+0x11f>:
     ebb:	188524        	ae_movtp24x2	aep0, aep5, b2:b3

00000ebe <xa_sbc_calculate_bits+0x122>:
     ebe:	da0c30010f 	{ ae_lp24x2.i	aep0, a1, 0; ae_addp24	aep4, aep3, aep0 }

00000ec3 <xa_sbc_calculate_bits+0x127>:
     ec3:	0acc84        	ae_cvtq48p24s.h	aeq3, aep4
     ec6:	0a0484        	ae_cvtq48p24s.l	aeq0, aep4
     ec9:	06cb04        	ae_addsq56s	aeq3, aeq0, aeq3
     ecc:	35f234        	ae_sraiq56	aeq3, aeq3, 8
     ecf:	06db04        	ae_addsq56s	aeq3, aeq1, aeq3
     ed2:	09a354        	ae_ltq56s	b5, aeq3, aeq2
     ed5:	0765e4        	ae_subp24	aep6, aep6, aep5
     ed8:	dca33eb57f 	{ ae_movtq56	aeq1, aeq3, b5; ae_subp24	aep0, aep0, aep5 }
     edd:	020576        	bf	b5, ee3 <xa_sbc_calculate_bits+0x147>
     ee0:	ffdf06        	j	e60 <xa_sbc_calculate_bits+0xc4>
     ee3:	09ab04        	ae_eqq56	b0, aeq3, aeq2
     ee6:	076584        	ae_subp24	aep0, aep6, aep5
     ee9:	0304f0        	rsr.br	a15
     eec:	04f0f0        	extui	a15, a15, 0, 1
     eef:	002f16        	beqz	a15, ef5 <xa_sbc_calculate_bits+0x159>
     ef2:	0904e4        	ae_movp48	aep6, aep0

00000ef5 <xa_sbc_calculate_bits+0x159>:
     ef5:	06d832        	addmi	a3, a8, 0x600
     ef8:	347014        	ae_movtq56	aeq1, aeq3, b0
     efb:	369b04        	ae_trunca32q48	a11, aeq1
     efe:	38c332        	addi	a3, a3, 56
     f01:	baac           	beqz.n	a10, f30 <xa_sbc_calculate_bits+0x194>
     f03:	041c           	movi.n	a4, 16
     f05:	fc20d8ceef 	{ ae_cvtp24a16x2.ll	aep4, a14, a14; ae_zerop48	aep1 }
     f0a:	ce5018b44f 	{ ae_cvtp24a16x2.ll	aep3, a4, a4; ae_sraip24	aep4, aep4, 8 }
     f0f:	0f3434        	ae_sraip24	aep3, aep3, 8
     f12:	076444        	ae_addp24	aep4, aep6, aep4
     f15:	149a76        	loopnez	a10, f2d <xa_sbc_calculate_bits+0x191>
     f18:	548614        	ae_lp24x2.iu	aep0, a6, 8
     f1b:	198404        	ae_ltp24s	b0:b1, aep0, aep4
     f1e:	070684        	ae_subp24	aep0, aep0, aep6
     f21:	188104        	ae_movtp24x2	aep0, aep1, b0:b1
     f24:	188014        	ae_movfp24x2	aep0, aep0, b0:b1
     f27:	0d0304        	ae_minbp24s	aep0, aep0, aep3, b0:b1
     f2a:	568314        	ae_sp24x2s.iu	aep0, a3, 8

00000f2d <xa_sbc_calculate_bits+0x191>:
     f2d:	042942        	l32i	a4, a9, 16

00000f30 <xa_sbc_calculate_bits+0x194>:
     f30:	0d0c           	movi.n	a13, 0
     f32:	006ed4        	ae_cvta32p24.l	a14, aep6
     f35:	a02a80        	addx4	a2, a10, a8
     f38:	f8ca           	add.n	a15, a8, a12
     f3a:	07d862        	addmi	a6, a8, 0x700
     f3d:	80c662        	addi	a6, a6, -128
     f40:	e1f9           	s32i.n	a15, a1, 56
     f42:	07d292        	addmi	a9, a2, 0x700
     f45:	c2ca           	add.n	a12, a2, a12
     f47:	80c992        	addi	a9, a9, -128
     f4a:	2b1b           	addi.n	a2, a11, 1
     f4c:	203cc0        	or	a3, a12, a12
     f4f:	21e8e0        	srai	a14, a14, 8
     f52:	01cee2        	addi	a14, a14, 1
     f55:	399a76        	loopnez	a10, f92 <xa_sbc_calculate_bits+0x1f6>
     f58:	02a427        	bge	a4, a2, f5e <xa_sbc_calculate_bits+0x1c2>
     f5b:	ff9f06        	j	ddb <xa_sbc_calculate_bits+0x3f>
     f5e:	e158           	l32i.n	a5, a1, 56
     f60:	0558           	l32i.n	a5, a5, 0
     f62:	3525a6        	blti	a5, 2, f9b <xa_sbc_calculate_bits+0x1ff>
     f65:	32b5e6        	bgei	a5, 16, f9b <xa_sbc_calculate_bits+0x1ff>
     f68:	551b           	addi.n	a5, a5, 1
     f6a:	bb1b           	addi.n	a11, a11, 1
     f6c:	221b           	addi.n	a2, a2, 1

00000f6e <xa_sbc_calculate_bits+0x1d2>:
     f6e:	e178           	l32i.n	a7, a1, 56
     f70:	09e754        	ae_s32ip	a5, a7, 4
     f73:	e179           	s32i.n	a7, a1, 56
     f75:	02a427        	bge	a4, a2, f7b <xa_sbc_calculate_bits+0x1df>
     f78:	ff97c6        	j	ddb <xa_sbc_calculate_bits+0x3f>
     f7b:	0358           	l32i.n	a5, a3, 0
     f7d:	2b25a6        	blti	a5, 2, fac <xa_sbc_calculate_bits+0x210>
     f80:	28b5e6        	bgei	a5, 16, fac <xa_sbc_calculate_bits+0x210>
     f83:	551b           	addi.n	a5, a5, 1
     f85:	bb1b           	addi.n	a11, a11, 1
     f87:	221b           	addi.n	a2, a2, 1

00000f89 <xa_sbc_calculate_bits+0x1ed>:
     f89:	09e354        	ae_s32ip	a5, a3, 4
     f8c:	664b           	addi.n	a6, a6, 4
     f8e:	994b           	addi.n	a9, a9, 4
     f90:	dd1b           	addi.n	a13, a13, 1

00000f92 <xa_sbc_calculate_bits+0x1f6>:
     f92:	0c3d           	mov.n	a3, a12
     f94:	e1f9           	s32i.n	a15, a1, 56
     f96:	2b1b           	addi.n	a2, a11, 1
     f98:	000846        	j	fbd <xa_sbc_calculate_bits+0x221>

00000f9b <xa_sbc_calculate_bits+0x1ff>:
     f9b:	0678           	l32i.n	a7, a6, 0
     f9d:	cd97e7        	bne	a7, a14, f6e <xa_sbc_calculate_bits+0x1d2>
     fa0:	caa247        	bge	a2, a4, f6e <xa_sbc_calculate_bits+0x1d2>
     fa3:	bb2b           	addi.n	a11, a11, 2
     fa5:	222b           	addi.n	a2, a2, 2
     fa7:	250c           	movi.n	a5, 2
     fa9:	fff046        	j	f6e <xa_sbc_calculate_bits+0x1d2>

00000fac <xa_sbc_calculate_bits+0x210>:
     fac:	0988           	l32i.n	a8, a9, 0
     fae:	d798e7        	bne	a8, a14, f89 <xa_sbc_calculate_bits+0x1ed>
     fb1:	d4a247        	bge	a2, a4, f89 <xa_sbc_calculate_bits+0x1ed>
     fb4:	bb2b           	addi.n	a11, a11, 2
     fb6:	222b           	addi.n	a2, a2, 2
     fb8:	250c           	movi.n	a5, 2
     fba:	fff2c6        	j	f89 <xa_sbc_calculate_bits+0x1ed>

00000fbd <xa_sbc_calculate_bits+0x221>:
     fbd:	239a76        	loopnez	a10, fe4 <xa_sbc_calculate_bits+0x248>
     fc0:	202427        	blt	a4, a2, fe4 <xa_sbc_calculate_bits+0x248>
     fc3:	e158           	l32i.n	a5, a1, 56
     fc5:	0558           	l32i.n	a5, a5, 0
     fc7:	03b5e6        	bgei	a5, 16, fce <xa_sbc_calculate_bits+0x232>
     fca:	551b           	addi.n	a5, a5, 1
     fcc:	221b           	addi.n	a2, a2, 1

00000fce <xa_sbc_calculate_bits+0x232>:
     fce:	e178           	l32i.n	a7, a1, 56
     fd0:	09e754        	ae_s32ip	a5, a7, 4
     fd3:	e179           	s32i.n	a7, a1, 56
     fd5:	0b2427        	blt	a4, a2, fe4 <xa_sbc_calculate_bits+0x248>
     fd8:	0358           	l32i.n	a5, a3, 0
     fda:	03b5e6        	bgei	a5, 16, fe1 <xa_sbc_calculate_bits+0x245>
     fdd:	551b           	addi.n	a5, a5, 1
     fdf:	221b           	addi.n	a2, a2, 1

00000fe1 <xa_sbc_calculate_bits+0x245>:
     fe1:	09e354        	ae_s32ip	a5, a3, 4

00000fe4 <xa_sbc_calculate_bits+0x248>:
     fe4:	f01d           	retw.n

00000fe6 <xa_sbc_calculate_bits+0x24a>:
     fe6:	78ca           	add.n	a7, a8, a12
     fe8:	e179           	s32i.n	a7, a1, 56
     fea:	070c           	movi.n	a7, 0
     fec:	c179           	s32i.n	a7, a1, 48
     fee:	080972        	l8ui	a7, a9, 8
     ff1:	de6716        	beqz	a7, ddb <xa_sbc_calculate_bits+0x3f>
     ff4:	68da           	add.n	a6, a8, a13
     ff6:	305ff4        	ae_cvtp24a16x2.ll	aep5, a15, a15
     ff9:	07d872        	addmi	a7, a8, 0x700
     ffc:	c83a           	add.n	a12, a8, a3
     ffe:	a1c9           	s32i.n	a12, a1, 40
    1000:	80c772        	addi	a7, a7, -128
    1003:	9179           	s32i.n	a7, a1, 36
    1005:	fccbc2        	addi	a12, a11, -4
    1008:	ce545481cf 	{ s32i.n	a12, a1, 32; ae_sraip24	aep5, aep5, 8 }
    100d:	0007c6        	j	1030 <xa_sbc_calculate_bits+0x294>

00001010 <xa_sbc_calculate_bits+0x274>:
    1010:	e1b8           	l32i.n	a11, a1, 56
    1012:	0f0c           	movi.n	a15, 0
    1014:	021da6        	blti	a13, 1, 101a <xa_sbc_calculate_bits+0x27e>
    1017:	006986        	j	11c1 <xa_sbc_calculate_bits+0x425>

0000101a <xa_sbc_calculate_bits+0x27e>:
    101a:	c1f8           	l32i.n	a15, a1, 48
    101c:	e188           	l32i.n	a8, a1, 56
    101e:	080972        	l8ui	a7, a9, 8
    1021:	ff1b           	addi.n	a15, a15, 1
    1023:	a08d80        	addx4	a8, a13, a8
    1026:	e189           	s32i.n	a8, a1, 56
    1028:	c1f9           	s32i.n	a15, a1, 48
    102a:	022f77        	blt	a15, a7, 1030 <xa_sbc_calculate_bits+0x294>
    102d:	ff6a86        	j	ddb <xa_sbc_calculate_bits+0x3f>

00001030 <xa_sbc_calculate_bits+0x294>:
    1030:	0a09b2        	l8ui	a11, a9, 10
    1033:	bb0b           	addi.n	a11, a11, -1
    1035:	1b0b16        	beqz	a11, 11e9 <xa_sbc_calculate_bits+0x44d>
    1038:	d1c8           	l32i.n	a12, a1, 52
    103a:	8128           	l32i.n	a2, a1, 32
    103c:	a138           	l32i.n	a3, a1, 40
    103e:	1bfc16        	beqz	a12, 1201 <xa_sbc_calculate_bits+0x465>
    1041:	b1d8           	l32i.n	a13, a1, 44
    1043:	bf7c           	movi.n	a15, -5
    1045:	fe24becfff 	{ ae_cvtq48a32s	aeq3, a15; ae_zeroq56	aeq2 }
    104a:	189c76        	loopnez	a12, 1066 <xa_sbc_calculate_bits+0x2ca>
    104d:	605d14        	ae_lq32f.iu	aeq1, a13, 4
    1050:	d4547d121f 	{ ae_lq32f.iu	aeq0, a2, 4; ae_eqq56	b1, aeq1, aeq2 }
    1055:	015cc4        	ae_subq56	aeq1, aeq1, aeq0
    1058:	d6c83b101f 	{ ae_sraiq56	aeq0, aeq1, 1; ae_ltq56s	b0, aeq2, aeq1 }
    105d:	344014        	ae_movtq56	aeq1, aeq0, b0
    1060:	347114        	ae_movtq56	aeq1, aeq3, b1
    1063:	625314        	ae_sq32f.iu	aeq1, a3, 4

00001066 <xa_sbc_calculate_bits+0x2ca>:
    1066:	d178           	l32i.n	a7, a1, 52
    1068:	b1d9           	s32i.n	a13, a1, 44
    106a:	193716        	beqz	a7, 1201 <xa_sbc_calculate_bits+0x465>

0000106d <xa_sbc_calculate_bits+0x2d1>:
    106d:	a128           	l32i.n	a2, a1, 40
    106f:	0d2132        	l32i	a3, a1, 52
    1072:	0020f0        	nop
    1075:	601214        	ae_lq32f.iu	aeq0, a2, 4
    1078:	fd248ff33e 	{ addi	a3, a3, -1; ae_zeroq56	aeq1 }
    107d:	049376        	loopnez	a3, 1085 <xa_sbc_calculate_bits+0x2e9>
    1080:	f9673d121f 	{ ae_lq32f.iu	aeq0, a2, 4; ae_maxq56s	aeq1, aeq1, aeq0 }

00001085 <xa_sbc_calculate_bits+0x2e9>:
    1085:	0101c4        	ae_maxq56s	aeq0, aeq1, aeq0

00001088 <xa_sbc_calculate_bits+0x2ec>:
    1088:	368804        	ae_trunca32q48	a8, aeq0
    108b:	604944        	ae_lq32f.i	aeq1, a9, 16
    108e:	78fb           	addi.n	a7, a8, 15
    1090:	fe2080188e 	{ addi	a8, a8, 1; ae_zerop48	aep4 }
    1095:	fc2498877f 	{ ae_cvtp24a16x2.ll	aep0, a7, a7; ae_zeroq56	aeq0 }
    109a:	cc4018f88f 	{ ae_cvtp24a16x2.ll	aep7, a8, a8; ae_sraip24	aep0, aep0, 8 }
    109f:	cf5c53266f 	{ or	a2, a6, a6; ae_sraip24	aep7, aep7, 8 }

000010a4 <xa_sbc_calculate_bits+0x308>:
    10a4:	056a16        	beqz	a10, 10fe <xa_sbc_calculate_bits+0x362>
    10a7:	549214        	ae_lp24x2.iu	aep1, a2, 8
    10aa:	d23d98eeef 	{ ae_cvtp24a16x2.ll	aep6, a14, a14; ae_eqp24	b6:b7, aep7, aep1 }
    10af:	cf5837012f 	{ ae_sp24x2s.i	aep0, a1, 16; ae_sraip24	aep6, aep6, 8 }
    10b4:	d23ef0312f 	{ ae_lp24x2.i	aep3, a1, 16; ae_ltp24s	b10:b11, aep7, aep1 }
    10b9:	d22e7f40bf 	{ ae_movp48	aep2, aep4; ae_ltp24s	b8:b9, aep3, aep1 }
    10be:	e0aabf403f 	{ ae_movp48	aep0, aep4; ae_movtp24x2	aep2, aep5, b10:b11 }
    10c3:	188664        	ae_movtp24x2	aep0, aep6, b6:b7
    10c6:	e08a0ffa3e 	{ addi	a3, a10, -1; ae_movtp24x2	aep2, aep4, b8:b9 }
    10cb:	d2a8bf40ff 	{ ae_movp48	aep3, aep4; ae_eqp24	b2:b3, aep2, aep5 }
    10d0:	229376        	loopnez	a3, 10f6 <xa_sbc_calculate_bits+0x35a>
    10d3:	e0a0b0a21f 	{ ae_lp24x2.iu	aep2, a2, 8; ae_movtp24x2	aep0, aep5, b2:b3 }
    10d8:	d90c70112f 	{ ae_lp24x2.i	aep1, a1, 16; ae_addp24	aep3, aep3, aep0 }
    10dd:	d25cff403f 	{ ae_movp48	aep0, aep4; ae_ltp24s	b2:b3, aep7, aep2 }
    10e2:	d2457f007f 	{ ae_movp48	aep1, aep0; ae_ltp24s	b4:b5, aep1, aep2 }
    10e7:	19fa04        	ae_eqp24	b0:b1, aep7, aep2
    10ea:	189524        	ae_movtp24x2	aep1, aep5, b2:b3
    10ed:	189444        	ae_movtp24x2	aep1, aep4, b4:b5
    10f0:	199d24        	ae_eqp24	b2:b3, aep1, aep5
    10f3:	188604        	ae_movtp24x2	aep0, aep6, b0:b1

000010f6 <xa_sbc_calculate_bits+0x35a>:
    10f6:	188524        	ae_movtp24x2	aep0, aep5, b2:b3

000010f9 <xa_sbc_calculate_bits+0x35d>:
    10f9:	da0c30012f 	{ ae_lp24x2.i	aep0, a1, 16; ae_addp24	aep4, aep3, aep0 }

000010fe <xa_sbc_calculate_bits+0x362>:
    10fe:	0a8c84        	ae_cvtq48p24s.h	aeq2, aep4
    1101:	0ac484        	ae_cvtq48p24s.l	aeq3, aep4
    1104:	06ba04        	ae_addsq56s	aeq2, aeq3, aeq2
    1107:	dfbf7b228f 	{ ae_sraiq56	aeq2, aeq2, 8; ae_subp24	aep7, aep7, aep5 }
    110c:	068a04        	ae_addsq56s	aeq2, aeq0, aeq2
    110f:	0992b4        	ae_ltq56s	b11, aeq2, aeq1
    1112:	dca313266f 	{ or	a2, a6, a6; ae_subp24	aep0, aep0, aep5 }
    1117:	fe20beab3f 	{ ae_movtq56	aeq0, aeq2, b11; ae_zerop48	aep4 }
    111c:	841b76        	bt	b11, 10a4 <xa_sbc_calculate_bits+0x308>
    111f:	099a04        	ae_eqq56	b0, aeq2, aeq1
    1122:	077584        	ae_subp24	aep0, aep7, aep5
    1125:	0304c0        	rsr.br	a12
    1128:	04c0c0        	extui	a12, a12, 0, 1
    112b:	1c8c           	beqz.n	a12, 1130 <xa_sbc_calculate_bits+0x394>
    112d:	0904f4        	ae_movp48	aep7, aep0

00001130 <xa_sbc_calculate_bits+0x394>:
    1130:	062d           	mov.n	a2, a6
    1132:	e138           	l32i.n	a3, a1, 56
    1134:	342014        	ae_movtq56	aeq0, aeq2, b0
    1137:	041c           	movi.n	a4, 16
    1139:	fc20d8ceef 	{ ae_cvtp24a16x2.ll	aep4, a14, a14; ae_zerop48	aep1 }
    113e:	ce5018a44f 	{ ae_cvtp24a16x2.ll	aep2, a4, a4; ae_sraip24	aep4, aep4, 8 }
    1143:	cd483f0cef 	{ ae_trunca32q48	a12, aeq0; ae_sraip24	aep2, aep2, 8 }
    1148:	da9c0f833e 	{ addi	a3, a3, -8; ae_addp24	aep4, aep7, aep4 }
    114d:	149a76        	loopnez	a10, 1165 <xa_sbc_calculate_bits+0x3c9>
    1150:	548214        	ae_lp24x2.iu	aep0, a2, 8
    1153:	198404        	ae_ltp24s	b0:b1, aep0, aep4
    1156:	070784        	ae_subp24	aep0, aep0, aep7
    1159:	188104        	ae_movtp24x2	aep0, aep1, b0:b1
    115c:	188014        	ae_movfp24x2	aep0, aep0, b0:b1
    115f:	0d0204        	ae_minbp24s	aep0, aep0, aep2, b0:b1
    1162:	568314        	ae_sp24x2s.iu	aep0, a3, 8

00001165 <xa_sbc_calculate_bits+0x3c9>:
    1165:	e1b8           	l32i.n	a11, a1, 56
    1167:	0c09d2        	l8ui	a13, a9, 12
    116a:	91f8           	l32i.n	a15, a1, 36
    116c:	030c           	movi.n	a3, 0
    116e:	021de6        	bgei	a13, 1, 1174 <xa_sbc_calculate_bits+0x3d8>
    1171:	ffa6c6        	j	1010 <xa_sbc_calculate_bits+0x274>
    1174:	0072d4        	ae_cvta32p24.l	a2, aep7
    1177:	212820        	srai	a2, a2, 8
    117a:	01c222        	addi	a2, a2, 1
    117d:	268076        	loop	a0, 11a7 <xa_sbc_calculate_bits+0x40b>
    1180:	4948           	l32i.n	a4, a9, 16
    1182:	022c47        	blt	a12, a4, 1188 <xa_sbc_calculate_bits+0x3ec>
    1185:	ffa1c6        	j	1010 <xa_sbc_calculate_bits+0x274>
    1188:	0b88           	l32i.n	a8, a11, 0
    118a:	331b           	addi.n	a3, a3, 1
    118c:	1a28a6        	blti	a8, 2, 11aa <xa_sbc_calculate_bits+0x40e>
    118f:	17b8e6        	bgei	a8, 16, 11aa <xa_sbc_calculate_bits+0x40e>
    1192:	cc1b           	addi.n	a12, a12, 1
    1194:	781b           	addi.n	a7, a8, 1
    1196:	0b79           	s32i.n	a7, a11, 0
    1198:	0c09d2        	l8ui	a13, a9, 12

0000119b <xa_sbc_calculate_bits+0x3ff>:
    119b:	ff4b           	addi.n	a15, a15, 4
    119d:	bb4b           	addi.n	a11, a11, 4
    119f:	0223d7        	blt	a3, a13, 11a5 <xa_sbc_calculate_bits+0x409>
    11a2:	ff9a86        	j	1010 <xa_sbc_calculate_bits+0x274>
    11a5:	f03d           	nop.n

000011a7 <xa_sbc_calculate_bits+0x40b>:
    11a7:	fff486        	j	117d <xa_sbc_calculate_bits+0x3e1>

000011aa <xa_sbc_calculate_bits+0x40e>:
    11aa:	0f88           	l32i.n	a8, a15, 0
    11ac:	7c1b           	addi.n	a7, a12, 1
    11ae:	e99827        	bne	a8, a2, 119b <xa_sbc_calculate_bits+0x3ff>
    11b1:	e6a747        	bge	a7, a4, 119b <xa_sbc_calculate_bits+0x3ff>
    11b4:	cc2b           	addi.n	a12, a12, 2
    11b6:	0be9           	s32i.n	a14, a11, 0
    11b8:	0c09d2        	l8ui	a13, a9, 12
    11bb:	fff706        	j	119b <xa_sbc_calculate_bits+0x3ff>

000011be <xa_sbc_calculate_bits+0x422>:
    11be:	000000                                        ...

000011c1 <xa_sbc_calculate_bits+0x425>:
    11c1:	218076        	loop	a0, 11e6 <xa_sbc_calculate_bits+0x44a>
    11c4:	4988           	l32i.n	a8, a9, 16
    11c6:	022c87        	blt	a12, a8, 11cc <xa_sbc_calculate_bits+0x430>
    11c9:	ff9346        	j	101a <xa_sbc_calculate_bits+0x27e>
    11cc:	0b88           	l32i.n	a8, a11, 0
    11ce:	ff1b           	addi.n	a15, a15, 1
    11d0:	08b8e6        	bgei	a8, 16, 11dc <xa_sbc_calculate_bits+0x440>
    11d3:	cc1b           	addi.n	a12, a12, 1
    11d5:	d81b           	addi.n	a13, a8, 1
    11d7:	0bd9           	s32i.n	a13, a11, 0
    11d9:	0c09d2        	l8ui	a13, a9, 12

000011dc <xa_sbc_calculate_bits+0x440>:
    11dc:	bb4b           	addi.n	a11, a11, 4
    11de:	022fd7        	blt	a15, a13, 11e4 <xa_sbc_calculate_bits+0x448>
    11e1:	ff8d46        	j	101a <xa_sbc_calculate_bits+0x27e>
    11e4:	f03d           	nop.n

000011e6 <xa_sbc_calculate_bits+0x44a>:
    11e6:	fff5c6        	j	11c1 <xa_sbc_calculate_bits+0x425>

000011e9 <xa_sbc_calculate_bits+0x44d>:
    11e9:	d1f8           	l32i.n	a15, a1, 52
    11eb:	a138           	l32i.n	a3, a1, 40
    11ed:	0f9c           	beqz.n	a15, 1201 <xa_sbc_calculate_bits+0x465>
    11ef:	b128           	l32i.n	a2, a1, 44
    11f1:	059f76        	loopnez	a15, 11fa <xa_sbc_calculate_bits+0x45e>
    11f4:	601214        	ae_lq32f.iu	aeq0, a2, 4
    11f7:	624314        	ae_sq32f.iu	aeq0, a3, 4

000011fa <xa_sbc_calculate_bits+0x45e>:
    11fa:	d178           	l32i.n	a7, a1, 52
    11fc:	b129           	s32i.n	a2, a1, 44
    11fe:	e6b756        	bnez	a7, 106d <xa_sbc_calculate_bits+0x2d1>
    1201:	0a1084        	ae_zeroq56	aeq0
    1204:	ffa006        	j	1088 <xa_sbc_calculate_bits+0x2ec>

00001207 <xa_sbc_calculate_bits+0x46b>:
	...

00001208 <xa_sbc_calculate_bits+0x46c>:
    1208:	fbacb1        	l32r	a11, b8 (1fd0 <_DYNAMIC+0xc7c>)
    120b:	11c4c0        	slli	a12, a4, 4
    120e:	bbca           	add.n	a11, a11, a12
    1210:	fee906        	j	db8 <xa_sbc_calculate_bits+0x1c>

00001213 <xa_sbc_calculate_bits+0x477>:
	...

00001215 <xa_sbc_calculate_bits+0x479>:
    1215:	d178           	l32i.n	a7, a1, 52
    1217:	a138           	l32i.n	a3, a1, 40
    1219:	e78c           	beqz.n	a7, 122b <xa_sbc_calculate_bits+0x48f>
    121b:	b128           	l32i.n	a2, a1, 44
    121d:	059776        	loopnez	a7, 1226 <xa_sbc_calculate_bits+0x48a>
    1220:	601214        	ae_lq32f.iu	aeq0, a2, 4
    1223:	624314        	ae_sq32f.iu	aeq0, a3, 4

00001226 <xa_sbc_calculate_bits+0x48a>:
    1226:	d1b8           	l32i.n	a11, a1, 52
    1228:	bfab56        	bnez	a11, e26 <xa_sbc_calculate_bits+0x8a>

0000122b <xa_sbc_calculate_bits+0x48f>:
    122b:	0a1084        	ae_zeroq56	aeq0
    122e:	ff0386        	j	e40 <xa_sbc_calculate_bits+0xa4>

00001231 <xa_sbc_calculate_bits+0x495>:
    1231:	000000                                        ...

00001234 <__mulsi3hifi2>:
    1234:	002136        	entry	a1, 16
    1237:	822230        	mull	a2, a2, a3
    123a:	f01d           	retw.n

0000123c <__divsi3>:
    123c:	002136        	entry	a1, 16
    123f:	d22230        	quos	a2, a2, a3
    1242:	f01d           	retw.n

00001244 <__divsi3+0x8>:
    1244:	00000000                                ....

00001248 <__vec_memset>:
    1248:	006136        	entry	a1, 48
    124b:	02ad           	mov.n	a10, a2
    124d:	038d           	mov.n	a8, a3
    124f:	207220        	or	a7, a2, a2
    1252:	078316        	beqz	a3, 12ce <__vec_memset+0x86>
    1255:	849c           	beqz.n	a4, 1271 <__vec_memset+0x29>
    1257:	03a052        	movi	a5, 3
    125a:	130527        	bnone	a5, a2, 1271 <__vec_memset+0x29>
    125d:	0d8076        	loop	a0, 126e <__vec_memset+0x26>
    1260:	004782        	s8i	a8, a7, 0
    1263:	440b           	addi.n	a4, a4, -1
    1265:	771b           	addi.n	a7, a7, 1
    1267:	648c           	beqz.n	a4, 1271 <__vec_memset+0x29>
    1269:	149070        	extui	a9, a7, 0, 2
    126c:	198c           	beqz.n	a9, 1271 <__vec_memset+0x29>

0000126e <__vec_memset+0x26>:
    126e:	fffac6        	j	125d <__vec_memset+0x15>

00001271 <__vec_memset+0x29>:
    1271:	419240        	srli	a9, a4, 2
    1274:	74b080        	extui	a11, a8, 0, 8
    1277:	11cb00        	slli	a12, a11, 16
    127a:	8b78c0        	depbits	a12, a8, 24, 8
    127d:	8a78b0        	depbits	a11, a8, 8, 8
    1280:	20bbc0        	or	a11, a11, a12
    1283:	01b9           	s32i.n	a11, a1, 0
    1285:	0af916        	beqz	a9, 1338 <__vec_memset+0xf0>
    1288:	fcc722        	addi	a2, a7, -4
    128b:	600104        	ae_lq32f.i	aeq0, a1, 0
    128e:	029976        	loopnez	a9, 1294 <__vec_memset+0x4c>
    1291:	624214        	ae_sq32f.iu	aeq0, a2, 4

00001294 <__vec_memset+0x4c>:
    1294:	1199e0        	slli	a9, a9, 2
    1297:	2fb947        	bgeu	a9, a4, 12ca <__vec_memset+0x82>
    129a:	297a           	add.n	a2, a9, a7
    129c:	c03490        	sub	a3, a4, a9
    129f:	24d030        	extui	a13, a3, 0, 3
    12a2:	049d76        	loopnez	a13, 12aa <__vec_memset+0x62>
    12a5:	004282        	s8i	a8, a2, 0
    12a8:	221b           	addi.n	a2, a2, 1

000012aa <__vec_memset+0x62>:
    12aa:	414330        	srli	a4, a3, 3
    12ad:	199476        	loopnez	a4, 12ca <__vec_memset+0x82>
    12b0:	004282        	s8i	a8, a2, 0
    12b3:	014282        	s8i	a8, a2, 1
    12b6:	024282        	s8i	a8, a2, 2
    12b9:	034282        	s8i	a8, a2, 3
    12bc:	044282        	s8i	a8, a2, 4
    12bf:	054282        	s8i	a8, a2, 5
    12c2:	064282        	s8i	a8, a2, 6
    12c5:	074282        	s8i	a8, a2, 7
    12c8:	228b           	addi.n	a2, a2, 8

000012ca <__vec_memset+0x82>:
    12ca:	0a2d           	mov.n	a2, a10
    12cc:	f01d           	retw.n

000012ce <__vec_memset+0x86>:
    12ce:	080c           	movi.n	a8, 0
    12d0:	649c           	beqz.n	a4, 12ea <__vec_memset+0xa2>
    12d2:	07a0e2        	movi	a14, 7
    12d5:	110e27        	bnone	a14, a2, 12ea <__vec_memset+0xa2>

000012d8 <__vec_memset+0x90>:
    12d8:	004782        	s8i	a8, a7, 0
    12db:	ffc442        	addi	a4, a4, -1
    12de:	01c772        	addi	a7, a7, 1
    12e1:	005416        	beqz	a4, 12ea <__vec_memset+0xa2>
    12e4:	24f070        	extui	a15, a7, 0, 3
    12e7:	fedf56        	bnez	a15, 12d8 <__vec_memset+0x90>
    12ea:	419340        	srli	a9, a4, 3
    12ed:	04c916        	beqz	a9, 133d <__vec_memset+0xf5>
    12f0:	fc208f872e 	{ addi	a2, a7, -8; ae_zerop48	aep0 }
    12f5:	029976        	loopnez	a9, 12fb <__vec_memset+0xb3>
    12f8:	568214        	ae_sp24x2s.iu	aep0, a2, 8

000012fb <__vec_memset+0xb3>:
    12fb:	1199d0        	slli	a9, a9, 3
    12fe:	c8b947        	bgeu	a9, a4, 12ca <__vec_memset+0x82>
    1301:	297a           	add.n	a2, a9, a7
    1303:	c03490        	sub	a3, a4, a9
    1306:	245030        	extui	a5, a3, 0, 3
    1309:	049576        	loopnez	a5, 1311 <__vec_memset+0xc9>
    130c:	004282        	s8i	a8, a2, 0
    130f:	221b           	addi.n	a2, a2, 1

00001311 <__vec_memset+0xc9>:
    1311:	414330        	srli	a4, a3, 3
    1314:	083d           	mov.n	a3, a8
    1316:	199476        	loopnez	a4, 1333 <__vec_memset+0xeb>
    1319:	004232        	s8i	a3, a2, 0
    131c:	014232        	s8i	a3, a2, 1
    131f:	024232        	s8i	a3, a2, 2
    1322:	034232        	s8i	a3, a2, 3
    1325:	044232        	s8i	a3, a2, 4
    1328:	054232        	s8i	a3, a2, 5
    132b:	064232        	s8i	a3, a2, 6
    132e:	074232        	s8i	a3, a2, 7
    1331:	228b           	addi.n	a2, a2, 8
    1333:	0a2d           	mov.n	a2, a10
    1335:	f01d           	retw.n

00001337 <__vec_memset+0xef>:
	...

00001338 <__vec_memset+0xf0>:
    1338:	090c           	movi.n	a9, 0
    133a:	ffd586        	j	1294 <__vec_memset+0x4c>

0000133d <__vec_memset+0xf5>:
    133d:	090c           	movi.n	a9, 0
    133f:	ffee06        	j	12fb <__vec_memset+0xb3>

00001342 <__vec_memset+0xfa>:
	...

00001344 <_fini>:
    1344:	008136        	entry	a1, 64

00001347 <_fini+0x3>:
    1347:	f01d           	retw.n
