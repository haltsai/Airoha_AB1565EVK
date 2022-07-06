
Build/lib/pisplit_plc_pitch_position_independent.splitload.lib:     file format elf32-xtensa-le


Disassembly of section .text:

00000000 <_init-0x48>:
       0:	00001934 0000180c 00001250 00000070     4.......P...p...
      10:	000002b0 00001870 000018a0 00001880     ....p...........
      20:	00008000 00007fff 000018c0 00007ffe     ................
      30:	00800000 7fffffff 00010000 00004000     .............@..
      40:	00001999 000018d0                       ........

00000048 <_init>:
      48:	008136        	entry	a1, 64
      4b:	f01d           	retw.n

0000004d <_init+0x5>:
      4d:	000000                                        ...

00000050 <_start>:
      50:	004136        	entry	a1, 32
      53:	ffeca1        	l32r	a10, 4 (180c <_DYNAMIC+0x218>)
      56:	ffecb1        	l32r	a11, 8 (1250 <plcpitch_para_init>)
      59:	ffecc1        	l32r	a12, c (70 <SMART_PITCH_PLC_INI>)
      5c:	ffe991        	l32r	a9, 0 (1934 <printf_ptr>)
      5f:	0288           	l32i.n	a8, a2, 0
      61:	ffebd1        	l32r	a13, 10 (2b0 <SMART_PITCH_PLC_PROCESS>)
      64:	0989           	s32i.n	a8, a9, 0
      66:	0008e0        	callx8	a8
      69:	ffea21        	l32r	a2, 14 (1870 <export_parameter_array>)
      6c:	f01d           	retw.n

0000006e <_start+0x1e>:
	...

00000070 <SMART_PITCH_PLC_INI>:
      70:	004136        	entry	a1, 32
      73:	20a220        	or	a10, a2, a2
      76:	010065        	call8	107c <plc_gainctrl_ini>
      79:	012d65        	call8	1350 <find_pitch_ini>
      7c:	02bd           	mov.n	a11, a2
      7e:	03ad           	mov.n	a10, a3
      80:	000065        	call8	88 <cvsd_a2plc_ini>
      83:	f01d           	retw.n

00000085 <SMART_PITCH_PLC_INI+0x15>:
      85:	000000                                        ...

00000088 <cvsd_a2plc_ini>:
      88:	004136        	entry	a1, 32
      8b:	68a692        	movi	a9, 0x668
      8e:	872c           	movi.n	a7, 40
      90:	3a0c           	movi.n	a10, 3
      92:	1c0c           	movi.n	a12, 1
      94:	4b2342        	l32i	a4, a3, 0x12c
      97:	3a2352        	l32i	a5, a3, 232
      9a:	392362        	l32i	a6, a3, 228
      9d:	e369           	s32i.n	a6, a3, 56
      9f:	f359           	s32i.n	a5, a3, 60
      a1:	060c           	movi.n	a6, 0
      a3:	a0a052        	movi	a5, 160
      a6:	06d416        	beqz	a4, 117 <cvsd_a2plc_ini+0x8f>
      a9:	93a9           	s32i.n	a10, a3, 36
      ab:	3d6362        	s32i	a6, a3, 244
      ae:	43c9           	s32i.n	a12, a3, 16
      b0:	8379           	s32i.n	a7, a3, 32
      b2:	6359           	s32i.n	a5, a3, 24
      b4:	081c           	movi.n	a8, 16
      b6:	243c           	movi.n	a4, 50
      b8:	e6a0d2        	movi	a13, 230
      bb:	6e4c           	movi.n	a14, 70
      bd:	08a2b2        	movi	a11, 0x208
      c0:	6ea0f2        	movi	a15, 110
      c3:	73f9           	s32i.n	a15, a3, 28
      c5:	a3b9           	s32i.n	a11, a3, 40
      c7:	b3e9           	s32i.n	a14, a3, 44
      c9:	c3d9           	s32i.n	a13, a3, 48
      cb:	5349           	s32i.n	a4, a3, 20
      cd:	1389           	s32i.n	a8, a3, 4
      cf:	0389           	s32i.n	a8, a3, 0
      d1:	3389           	s32i.n	a8, a3, 12
      d3:	c53c           	movi.n	a5, 60
      d5:	026352        	s32i	a5, a3, 8
      d8:	01d382        	addmi	a8, a3, 0x100
      db:	14c882        	addi	a8, a8, 20
      de:	205880        	or	a5, a8, a8
      e1:	0aab76        	loopgtz	a11, ef <cvsd_a2plc_ini+0x67>
      e4:	01d5a2        	addmi	a10, a5, 0x100
      e7:	1c4a62        	s8i	a6, a10, 28
      ea:	1d4a62        	s8i	a6, a10, 29
      ed:	552b           	addi.n	a5, a5, 2

000000ef <cvsd_a2plc_ini+0x67>:
      ef:	83b8           	l32i.n	a11, a3, 32
      f1:	417860        	srli	a7, a6, 8
      f4:	539a           	add.n	a5, a3, a9
      f6:	07ab76        	loopgtz	a11, 101 <cvsd_a2plc_ini+0x79>
      f9:	004562        	s8i	a6, a5, 0
      fc:	014572        	s8i	a7, a5, 1
      ff:	552b           	addi.n	a5, a5, 2
     101:	a3d8           	l32i.n	a13, a3, 40
     103:	805390        	add	a5, a3, a9
     106:	417860        	srli	a7, a6, 8
     109:	07ad76        	loopgtz	a13, 114 <cvsd_a2plc_ini+0x8c>
     10c:	784562        	s8i	a6, a5, 120
     10f:	794572        	s8i	a7, a5, 121
     112:	552b           	addi.n	a5, a5, 2
     114:	001986        	j	17e <cvsd_a2plc_ini+0xf6>

00000117 <cvsd_a2plc_ini+0x8f>:
     117:	43c9           	s32i.n	a12, a3, 16
     119:	5379           	s32i.n	a7, a3, 20
     11b:	b379           	s32i.n	a7, a3, 44
     11d:	c359           	s32i.n	a5, a3, 48
     11f:	78a0e2        	movi	a14, 120
     122:	01d382        	addmi	a8, a3, 0x100
     125:	eb1c           	movi.n	a11, 30
     127:	0d5c           	movi.n	a13, 80
     129:	6f1c           	movi.n	a15, 22
     12b:	840c           	movi.n	a4, 8
     12d:	0349           	s32i.n	a4, a3, 0
     12f:	23f9           	s32i.n	a15, a3, 8
     131:	73d9           	s32i.n	a13, a3, 28
     133:	83b9           	s32i.n	a11, a3, 32
     135:	14c882        	addi	a8, a8, 20
     138:	63e9           	s32i.n	a14, a3, 24
     13a:	e50c           	movi.n	a5, 14
     13c:	1359           	s32i.n	a5, a3, 4
     13e:	3359           	s32i.n	a5, a3, 12
     140:	86a1e2        	movi	a14, 0x186
     143:	0a63e2        	s32i	a14, a3, 40
     146:	205880        	or	a5, a8, a8
     149:	0aae76        	loopgtz	a14, 157 <cvsd_a2plc_ini+0xcf>
     14c:	01d5b2        	addmi	a11, a5, 0x100
     14f:	1c4b62        	s8i	a6, a11, 28
     152:	1d4b62        	s8i	a6, a11, 29
     155:	552b           	addi.n	a5, a5, 2

00000157 <cvsd_a2plc_ini+0xcf>:
     157:	83d8           	l32i.n	a13, a3, 32
     159:	417860        	srli	a7, a6, 8
     15c:	539a           	add.n	a5, a3, a9
     15e:	07ad76        	loopgtz	a13, 169 <cvsd_a2plc_ini+0xe1>
     161:	004562        	s8i	a6, a5, 0
     164:	014572        	s8i	a7, a5, 1
     167:	552b           	addi.n	a5, a5, 2
     169:	a3e8           	l32i.n	a14, a3, 40
     16b:	805390        	add	a5, a3, a9
     16e:	417860        	srli	a7, a6, 8
     171:	07ae76        	loopgtz	a14, 17c <cvsd_a2plc_ini+0xf4>
     174:	784562        	s8i	a6, a5, 120
     177:	794572        	s8i	a7, a5, 121
     17a:	552b           	addi.n	a5, a5, 2
     17c:	93a9           	s32i.n	a10, a3, 36

0000017e <cvsd_a2plc_ini+0xf6>:
     17e:	085d           	mov.n	a5, a8
     180:	ffa671        	l32r	a7, 18 (18a0 <rc_win_left_16s>)
     183:	ffa681        	l32r	a8, 1c (1880 <rc_win_right_16s>)
     186:	03f8           	l32i.n	a15, a3, 0
     188:	079d           	mov.n	a9, a7
     18a:	20a880        	or	a10, a8, a8
     18d:	24af76        	loopgtz	a15, 1b5 <cvsd_a2plc_ini+0x12d>
     190:	0348           	l32i.n	a4, a3, 0
     192:	4cb466        	bnei	a4, 16, 1e2 <cvsd_a2plc_ini+0x15a>
     195:	2abd           	ae_l16si.n	a11, a10, 0
     197:	fc45b2        	s8i	a11, a5, 252
     19a:	41b8b0        	srli	a11, a11, 8
     19d:	fd45b2        	s8i	a11, a5, 253
     1a0:	29bd           	ae_l16si.n	a11, a9, 0
     1a2:	dc45b2        	s8i	a11, a5, 220

000001a5 <cvsd_a2plc_ini+0x11d>:
     1a5:	774b           	addi.n	a7, a7, 4
     1a7:	884b           	addi.n	a8, a8, 4
     1a9:	992b           	addi.n	a9, a9, 2
     1ab:	41d8b0        	srli	a13, a11, 8
     1ae:	aa2b           	addi.n	a10, a10, 2
     1b0:	dd45d2        	s8i	a13, a5, 221
     1b3:	552b           	addi.n	a5, a5, 2

000001b5 <cvsd_a2plc_ini+0x12d>:
     1b5:	d369           	s32i.n	a6, a3, 52
     1b7:	0f1c           	movi.n	a15, 16
     1b9:	0b1c           	movi.n	a11, 16
     1bb:	ff9b91        	l32r	a9, 28 (18c0 <zc_diff_size_tab>)
     1be:	e71c           	movi.n	a7, 30
     1c0:	ff9881        	l32r	a8, 20 (8000 <_end+0x66c8>)
     1c3:	0bd352        	addmi	a5, a3, 0xb00
     1c6:	18c552        	addi	a5, a5, 24
     1c9:	106382        	s32i	a8, a3, 64
     1cc:	827270        	mull	a7, a2, a7
     1cf:	ff9581        	l32r	a8, 24 (7fff <_end+0x66c7>)
     1d2:	1ec772        	addi	a7, a7, 30
     1d5:	a09290        	addx4	a9, a2, a9
     1d8:	1a6372        	s32i	a7, a3, 104
     1db:	471c           	movi.n	a7, 20
     1dd:	000506        	j	1f5 <cvsd_a2plc_ini+0x16d>

000001e0 <cvsd_a2plc_ini+0x158>:
	...

000001e2 <cvsd_a2plc_ini+0x15a>:
     1e2:	28bd           	ae_l16si.n	a11, a8, 0
     1e4:	fc45b2        	s8i	a11, a5, 252
     1e7:	41b8b0        	srli	a11, a11, 8
     1ea:	fd45b2        	s8i	a11, a5, 253
     1ed:	27bd           	ae_l16si.n	a11, a7, 0
     1ef:	dc45b2        	s8i	a11, a5, 220
     1f2:	ffebc6        	j	1a5 <cvsd_a2plc_ini+0x11d>

000001f5 <cvsd_a2plc_ini+0x16d>:
     1f5:	16a776        	loopgtz	a7, 20f <cvsd_a2plc_ini+0x187>
     1f8:	41e860        	srli	a14, a6, 8
     1fb:	004562        	s8i	a6, a5, 0
     1fe:	0145e2        	s8i	a14, a5, 1
     201:	41e8e0        	srli	a14, a14, 8
     204:	0245e2        	s8i	a14, a5, 2
     207:	41e8e0        	srli	a14, a14, 8
     20a:	0345e2        	s8i	a14, a5, 3
     20d:	554b           	addi.n	a5, a5, 4

0000020f <cvsd_a2plc_ini+0x187>:
     20f:	a348           	l32i.n	a4, a3, 40
     211:	01d372        	addmi	a7, a3, 0x100
     214:	0998           	l32i.n	a9, a9, 0
     216:	1b6382        	s32i	a8, a3, 108
     219:	1c6382        	s32i	a8, a3, 112
     21c:	1e6362        	s32i	a6, a3, 120
     21f:	1f6362        	s32i	a6, a3, 124
     222:	206362        	s32i	a6, a3, 128
     225:	216362        	s32i	a6, a3, 132
     228:	196362        	s32i	a6, a3, 100
     22b:	1d6362        	s32i	a6, a3, 116
     22e:	146392        	s32i	a9, a3, 80
     231:	70c772        	addi	a7, a7, 112
     234:	205770        	or	a5, a7, a7
     237:	186342        	s32i	a4, a3, 96
     23a:	10a092        	movi	a9, 16
     23d:	16af76        	loopgtz	a15, 257 <cvsd_a2plc_ini+0x1cf>
     240:	418860        	srli	a8, a6, 8
     243:	004562        	s8i	a6, a5, 0
     246:	014582        	s8i	a8, a5, 1
     249:	418880        	srli	a8, a8, 8
     24c:	024582        	s8i	a8, a5, 2
     24f:	418880        	srli	a8, a8, 8
     252:	034582        	s8i	a8, a5, 3
     255:	554b           	addi.n	a5, a5, 4
     257:	075d           	mov.n	a5, a7
     259:	16a976        	loopgtz	a9, 273 <cvsd_a2plc_ini+0x1eb>
     25c:	41a860        	srli	a10, a6, 8
     25f:	404562        	s8i	a6, a5, 64
     262:	4145a2        	s8i	a10, a5, 65
     265:	41a8a0        	srli	a10, a10, 8
     268:	4245a2        	s8i	a10, a5, 66
     26b:	41a8a0        	srli	a10, a10, 8
     26e:	4345a2        	s8i	a10, a5, 67
     271:	554b           	addi.n	a5, a5, 4
     273:	f03d           	nop.n
     275:	16ab76        	loopgtz	a11, 28f <cvsd_a2plc_ini+0x207>
     278:	41d860        	srli	a13, a6, 8
     27b:	404762        	s8i	a6, a7, 64
     27e:	4147d2        	s8i	a13, a7, 65
     281:	41d8d0        	srli	a13, a13, 8
     284:	4247d2        	s8i	a13, a7, 66
     287:	41d8d0        	srli	a13, a13, 8
     28a:	4347d2        	s8i	a13, a7, 67
     28d:	774b           	addi.n	a7, a7, 4
     28f:	1163c2        	s32i	a12, a3, 68
     292:	126362        	s32i	a6, a3, 72
     295:	136362        	s32i	a6, a3, 76
     298:	1823f2        	l32i	a15, a3, 96
     29b:	5328           	l32i.n	a2, a3, 20
     29d:	2e0c           	movi.n	a14, 2
     29f:	1563e2        	s32i	a14, a3, 84
     2a2:	226322        	s32i	a2, a3, 136
     2a5:	166322        	s32i	a2, a3, 88
     2a8:	c0ff20        	sub	a15, a15, a2
     2ab:	1763f2        	s32i	a15, a3, 92
     2ae:	f01d           	retw.n

000002b0 <SMART_PITCH_PLC_PROCESS>:
     2b0:	012136        	entry	a1, 144
     2b3:	30a162        	movi	a6, 0x130
     2b6:	252142        	l32i	a4, a1, 148
     2b9:	03ad           	mov.n	a10, a3
     2bb:	196132        	s32i	a3, a1, 100
     2be:	0458           	l32i.n	a5, a4, 0
     2c0:	2478           	l32i.n	a7, a4, 8
     2c2:	34b8           	l32i.n	a11, a4, 12
     2c4:	64f8           	l32i.n	a15, a4, 24
     2c6:	b488           	l32i.n	a8, a4, 44
     2c8:	156182        	s32i	a8, a1, 84
     2cb:	1661f2        	s32i	a15, a1, 88
     2ce:	a438           	l32i.n	a3, a4, 40
     2d0:	1924f2        	l32i	a15, a4, 100
     2d3:	1f9a56        	bnez	a10, 4d0 <SMART_PITCH_PLC_PROCESS+0x220>
     2d6:	30a272        	movi	a7, 0x230
     2d9:	e4a5d2        	movi	a13, 0x5e4
     2dc:	dca6a2        	movi	a10, 0x6dc
     2df:	4c4f56        	bnez	a15, 7a7 <SMART_PITCH_PLC_PROCESS+0x4f7>
     2e2:	c0c3b0        	sub	a12, a3, a11
     2e5:	909b40        	addx2	a9, a11, a4
     2e8:	06d4f2        	addmi	a15, a4, 0x600
     2eb:	68cff2        	addi	a15, a15, 104
     2ee:	06d992        	addmi	a9, a9, 0x600
     2f1:	1aac76        	loopgtz	a12, 30f <SMART_PITCH_PLC_PROCESS+0x5f>
     2f4:	e009e2        	l8ui	a14, a9, 224
     2f7:	e10982        	l8ui	a8, a9, 225
     2fa:	ff2b           	addi.n	a15, a15, 2
     2fc:	992b           	addi.n	a9, a9, 2
     2fe:	118880        	slli	a8, a8, 8
     301:	2088e0        	or	a8, a8, a14
     304:	888d           	ae_sext16	a8, a8
     306:	764f82        	s8i	a8, a15, 118
     309:	418880        	srli	a8, a8, 8
     30c:	774f82        	s8i	a8, a15, 119

0000030f <SMART_PITCH_PLC_PROCESS+0x5f>:
     30f:	1161b2        	s32i	a11, a1, 68
     312:	1461c2        	s32i	a12, a1, 80
     315:	02fd           	mov.n	a15, a2
     317:	909c40        	addx2	a9, a12, a4
     31a:	8099d0        	add	a9, a9, a13
     31d:	0eab76        	loopgtz	a11, 32f <SMART_PITCH_PLC_PROCESS+0x7f>
     320:	992b           	addi.n	a9, a9, 2
     322:	2fbd           	ae_l16si.n	a11, a15, 0
     324:	ff2b           	addi.n	a15, a15, 2
     326:	fa49b2        	s8i	a11, a9, 250
     329:	41b8b0        	srli	a11, a11, 8
     32c:	fb49b2        	s8i	a11, a9, 251
     32f:	1521c2        	l32i	a12, a1, 84
     332:	1621d2        	l32i	a13, a1, 88
     335:	80f4a0        	add	a15, a4, a10
     338:	21e130        	srai	a14, a3, 1
     33b:	02d4b2        	addmi	a11, a4, 0x200
     33e:	2ccbb2        	addi	a11, a11, 44
     341:	05ae76        	loopgtz	a14, 34a <SMART_PITCH_PLC_PROCESS+0x9a>
     344:	448f14        	ae_lp16x2f.iu	aep0, a15, 4
     347:	468b14        	ae_sp16x2f.iu	aep0, a11, 4
     34a:	0c24b2        	l32i	a11, a4, 48
     34d:	0724e2        	l32i	a14, a4, 28
     350:	1824f2        	l32i	a15, a4, 96
     353:	80a470        	add	a10, a4, a7
     356:	1361a2        	s32i	a10, a1, 76
     359:	00a1e5        	call8	d78 <find_pitch>
     35c:	1261a2        	s32i	a10, a1, 72
     35f:	242162        	l32i	a6, a1, 144
     362:	2264a2        	s32i	a10, a4, 136
     365:	0924b2        	l32i	a11, a4, 36
     368:	31df60        	srai	a13, a6, 31
     36b:	20c660        	or	a12, a6, a6
     36e:	82bba0        	mull	a11, a11, a10
     371:	21a160        	srai	a10, a6, 1
     374:	80aab0        	add	a10, a10, a11
     377:	31bfa0        	srai	a11, a10, 31
     37a:	010165        	call8	1390 <__divdi3>
     37d:	022aa6        	blti	a10, 2, 383 <SMART_PITCH_PLC_PROCESS+0xd3>
     380:	027086        	j	d46 <SMART_PITCH_PLC_PROCESS+0xa96>
     383:	2b0c           	movi.n	a11, 2
     385:	180c           	movi.n	a8, 1
     387:	116482        	s32i	a8, a4, 68

0000038a <SMART_PITCH_PLC_PROCESS+0xda>:
     38a:	122192        	l32i	a9, a1, 72
     38d:	0c0c           	movi.n	a12, 0
     38f:	68a6e2        	movi	a14, 0x668
     392:	1564b2        	s32i	a11, a4, 84
     395:	3515a6        	blti	a5, 1, 3ce <SMART_PITCH_PLC_PROCESS+0x11e>
     398:	c09350        	sub	a9, a3, a5
     39b:	06d4f2        	addmi	a15, a4, 0x600
     39e:	68cff2        	addi	a15, a15, 104
     3a1:	909940        	addx2	a9, a9, a4
     3a4:	01d992        	addmi	a9, a9, 0x100
     3a7:	30c992        	addi	a9, a9, 48
     3aa:	1da576        	loopgtz	a5, 3cb <SMART_PITCH_PLC_PROCESS+0x11b>
     3ad:	01d9d2        	addmi	a13, a9, 0x100
     3b0:	010db2        	l8ui	a11, a13, 1
     3b3:	000dd2        	l8ui	a13, a13, 0
     3b6:	992b           	addi.n	a9, a9, 2
     3b8:	11bb80        	slli	a11, a11, 8
     3bb:	20bbd0        	or	a11, a11, a13
     3be:	8bbd           	ae_sext16	a11, a11
     3c0:	41a8b0        	srli	a10, a11, 8
     3c3:	004fb2        	s8i	a11, a15, 0
     3c6:	014fa2        	s8i	a10, a15, 1
     3c9:	ff2b           	addi.n	a15, a15, 2

000003cb <SMART_PITCH_PLC_PROCESS+0x11b>:
     3cb:	222492        	l32i	a9, a4, 136

000003ce <SMART_PITCH_PLC_PROCESS+0x11e>:
     3ce:	166492        	s32i	a9, a4, 88
     3d1:	182482        	l32i	a8, a4, 96
     3d4:	1264c2        	s32i	a12, a4, 72
     3d7:	06d4b2        	addmi	a11, a4, 0x600
     3da:	02d4f2        	addmi	a15, a4, 0x200
     3dd:	ffcb           	addi.n	a15, a15, 12
     3df:	64cbb2        	addi	a11, a11, 100
     3e2:	20c110        	or	a12, a1, a1
     3e5:	c08890        	sub	a8, a8, a9
     3e8:	176482        	s32i	a8, a4, 92
     3eb:	02a082        	movi	a8, 2
     3ee:	d28580        	quos	a8, a5, a8
     3f1:	17a876        	loopgtz	a8, 40c <SMART_PITCH_PLC_PROCESS+0x15c>
     3f4:	44ab14        	ae_lp16x2f.iu	aep2, a11, 4
     3f7:	449f14        	ae_lp16x2f.iu	aep1, a15, 4
     3fa:	021274        	ae_mulp24s.ll	aeq0, aep1, aep2
     3fd:	021a44        	ae_mulp24s.hh	aeq1, aep1, aep2
     400:	368904        	ae_trunca32q48	a9, aeq0
     403:	369a04        	ae_trunca32q48	a10, aeq1
     406:	0ca9           	s32i.n	a10, a12, 0
     408:	1c99           	s32i.n	a9, a12, 4
     40a:	cc8b           	addi.n	a12, a12, 8

0000040c <SMART_PITCH_PLC_PROCESS+0x15c>:
     40c:	5f15a6        	blti	a5, 1, 46f <SMART_PITCH_PLC_PROCESS+0x1bf>
     40f:	01bd           	mov.n	a11, a1
     411:	00a092        	movi	a9, 0
     414:	02d4f2        	addmi	a15, a4, 0x200
     417:	f0cff2        	addi	a15, a15, -16
     41a:	fecff2        	addi	a15, a15, -2
     41d:	1ea576        	loopgtz	a5, 43f <SMART_PITCH_PLC_PROCESS+0x18f>
     420:	1724a2        	l32i	a10, a4, 92
     423:	40bf14        	ae_lp16f.iu	aep3, a15, 2
     426:	608b04        	ae_lq32f.i	aeq2, a11, 0
     429:	c0aa50        	sub	a10, a10, a5
     42c:	a9aa           	add.n	a10, a9, a10
     42e:	90aa40        	addx2	a10, a10, a4
     431:	414a74        	ae_lp16f.x	aep4, a10, a7
     434:	02b434        	ae_mulap24s.ll	aeq2, aep3, aep4
     437:	991b           	addi.n	a9, a9, 1
     439:	36a804        	ae_trunca32q48	a8, aeq2
     43c:	09eb84        	ae_s32ip	a8, a11, 4
     43f:	01bd           	mov.n	a11, a1
     441:	c0c350        	sub	a12, a3, a5
     444:	90cc40        	addx2	a12, a12, a4
     447:	01dc92        	addmi	a9, a12, 0x100
     44a:	30c992        	addi	a9, a9, 48
     44d:	ccea           	add.n	a12, a12, a14
     44f:	1ca576        	loopgtz	a5, 46f <SMART_PITCH_PLC_PROCESS+0x1bf>
     452:	cc2b           	addi.n	a12, a12, 2
     454:	01d982        	addmi	a8, a9, 0x100
     457:	0bf8           	l32i.n	a15, a11, 0
     459:	992b           	addi.n	a9, a9, 2
     45b:	bb4b           	addi.n	a11, a11, 4
     45d:	21fff0        	srai	a15, a15, 15
     460:	0048f2        	s8i	a15, a8, 0
     463:	41d8f0        	srli	a13, a15, 8
     466:	0148d2        	s8i	a13, a8, 1
     469:	764cf2        	s8i	a15, a12, 118
     46c:	774cd2        	s8i	a13, a12, 119

0000046f <SMART_PITCH_PLC_PROCESS+0x1bf>:
     46f:	142192        	l32i	a9, a1, 80
     472:	02fd           	mov.n	a15, a2
     474:	112182        	l32i	a8, a1, 68
     477:	909940        	addx2	a9, a9, a4
     47a:	8099e0        	add	a9, a9, a14
     47d:	12a876        	loopgtz	a8, 493 <SMART_PITCH_PLC_PROCESS+0x1e3>
     480:	7909a2        	l8ui	a10, a9, 121
     483:	7809b2        	l8ui	a11, a9, 120
     486:	992b           	addi.n	a9, a9, 2
     488:	11aa80        	slli	a10, a10, 8
     48b:	20aab0        	or	a10, a10, a11
     48e:	8aad           	ae_sext16	a10, a10
     490:	09cfa4        	ae_s16ip	a10, a15, 2

00000493 <SMART_PITCH_PLC_PROCESS+0x1e3>:
     493:	112172        	l32i	a7, a1, 68
     496:	c07760        	sub	a7, a7, a6
     499:	737a           	add.n	a7, a3, a7
     49b:	0217e6        	bgei	a7, 1, 4a1 <SMART_PITCH_PLC_PROCESS+0x1f1>
     49e:	015006        	j	9e2 <SMART_PITCH_PLC_PROCESS+0x732>
     4a1:	112152        	l32i	a5, a1, 68
     4a4:	f4ea           	add.n	a15, a4, a14
     4a6:	c05650        	sub	a5, a6, a5
     4a9:	909540        	addx2	a9, a5, a4
     4ac:	99ea           	add.n	a9, a9, a14
     4ae:	1aa776        	loopgtz	a7, 4cc <SMART_PITCH_PLC_PROCESS+0x21c>
     4b1:	7809a2        	l8ui	a10, a9, 120
     4b4:	790982        	l8ui	a8, a9, 121
     4b7:	ff2b           	addi.n	a15, a15, 2
     4b9:	992b           	addi.n	a9, a9, 2
     4bb:	118880        	slli	a8, a8, 8
     4be:	2088a0        	or	a8, a8, a10
     4c1:	888d           	ae_sext16	a8, a8
     4c3:	764f82        	s8i	a8, a15, 118
     4c6:	418880        	srli	a8, a8, 8
     4c9:	774f82        	s8i	a8, a15, 119
     4cc:	014606        	j	9e8 <SMART_PITCH_PLC_PROCESS+0x738>

000004cf <SMART_PITCH_PLC_PROCESS+0x21f>:
	...

000004d0 <SMART_PITCH_PLC_PROCESS+0x220>:
     4d0:	25ff16        	beqz	a15, 733 <SMART_PITCH_PLC_PROCESS+0x483>
     4d3:	4b24a2        	l32i	a10, a4, 0x12c
     4d6:	2421b2        	l32i	a11, a1, 144
     4d9:	1b61b2        	s32i	a11, a1, 108
     4dc:	0b5a16        	beqz	a10, 595 <SMART_PITCH_PLC_PROCESS+0x2e5>
     4df:	206bb0        	or	a6, a11, a11
     4e2:	1b61b2        	s32i	a11, a1, 108
     4e5:	88cbc2        	addi	a12, a11, -120
     4e8:	0a9c56        	bnez	a12, 595 <SMART_PITCH_PLC_PROCESS+0x2e5>
     4eb:	1624b2        	l32i	a11, a4, 88
     4ee:	1724c2        	l32i	a12, a4, 92
     4f1:	20d660        	or	a13, a6, a6
     4f4:	20f440        	or	a15, a4, a4
     4f7:	02d452        	addmi	a5, a4, 0x200
     4fa:	30c552        	addi	a5, a5, 48
     4fd:	05ad           	mov.n	a10, a5
     4ff:	05ed           	mov.n	a14, a5
     501:	00e565        	call8	1358 <getfespeech>
     504:	1c24b2        	l32i	a11, a4, 112
     507:	fec981        	l32r	a8, 2c (7ffe <_end+0x66c6>)
     50a:	1b21a2        	l32i	a10, a1, 108
     50d:	34a2f2        	movi	a15, 0x234
     510:	1b28b7        	blt	a8, a11, 52f <SMART_PITCH_PLC_PROCESS+0x27f>
     513:	302bb4        	ae_cvtp24a16x2.ll	aep2, a11, a11
     516:	05ed           	mov.n	a14, a5
     518:	02d492        	addmi	a9, a4, 0x200
     51b:	34c992        	addi	a9, a9, 52
     51e:	4009e4        	ae_lp16f.i	aep0, a9, -4
     521:	0aa676        	loopgtz	a6, 52f <SMART_PITCH_PLC_PROCESS+0x27f>
     524:	018a74        	ae_mulfp24s.ll	aeq3, aep0, aep2
     527:	fafbec8e1f 	{ ae_lp16f.iu	aep0, a14, 2; ae_roundsp16q48sym	aep5, aeq3 }
     52c:	425ef4        	ae_sp16f.l.i	aep5, a14, -2
     52f:	02bd           	mov.n	a11, a2
     531:	11d7f0        	slli	a13, a7, 1
     534:	01d492        	addmi	a9, a4, 0x100
     537:	34c992        	addi	a9, a9, 52
     53a:	12a776        	loopgtz	a7, 550 <SMART_PITCH_PLC_PROCESS+0x2a0>
     53d:	fd09c2        	l8ui	a12, a9, 253
     540:	fc09e2        	l8ui	a14, a9, 252
     543:	992b           	addi.n	a9, a9, 2
     545:	11cc80        	slli	a12, a12, 8
     548:	20cce0        	or	a12, a12, a14
     54b:	8ccd           	ae_sext16	a12, a12
     54d:	09cbc4        	ae_s16ip	a12, a11, 2

00000550 <SMART_PITCH_PLC_PROCESS+0x2a0>:
     550:	412d24        	ae_lp16f.x	aep2, a13, a2
     553:	c08670        	sub	a8, a6, a7
     556:	feb6b1        	l32r	a11, 30 (800000 <_end+0x7fe6c8>)
     559:	9d4a           	add.n	a9, a13, a4
     55b:	02d9c2        	addmi	a12, a9, 0x200
     55e:	30ccc2        	addi	a12, a12, 48
     561:	99fa           	add.n	a9, a9, a15
     563:	d2eb80        	quos	a14, a11, a8
     566:	4009e4        	ae_lp16f.i	aep0, a9, -4
     569:	c0bbe0        	sub	a11, a11, a14
     56c:	399be4        	ae_movpa24x2	aep1, a11, a14
     56f:	60b0e0        	neg	a11, a14
     572:	39cbe4        	ae_movpa24x2	aep4, a11, a14
     575:	80ed20        	add	a14, a13, a2
     578:	dc420feebe 	{ addi	a11, a14, -2; ae_selp24.ll	aep0, aep0, aep2 }
     57d:	11a876        	loopgtz	a8, 592 <SMART_PITCH_PLC_PROCESS+0x2e2>
     580:	041004        	ae_mulzaafp24s.hh.ll	aeq0, aep1, aep0
     583:	d8856cae1f 	{ ae_lp16f.iu	aep2, a14, 2; ae_addsp24s	aep1, aep1, aep4 }
     588:	f9e3ec8c1f 	{ ae_lp16f.iu	aep0, a12, 2; ae_roundsp16q48sym	aep3, aeq0 }
     58d:	dc4232bb1f 	{ ae_sp16f.l.iu	aep3, a11, 2; ae_selp24.ll	aep0, aep0, aep2 }
     592:	006806        	j	736 <SMART_PITCH_PLC_PROCESS+0x486>

00000595 <SMART_PITCH_PLC_PROCESS+0x2e5>:
     595:	30a162        	movi	a6, 0x130
     598:	1624b2        	l32i	a11, a4, 88
     59b:	1724c2        	l32i	a12, a4, 92
     59e:	80d750        	add	a13, a7, a5
     5a1:	20f440        	or	a15, a4, a4
     5a4:	02d4e2        	addmi	a14, a4, 0x200
     5a7:	30cee2        	addi	a14, a14, 48
     5aa:	20aee0        	or	a10, a14, a14
     5ad:	00daa5        	call8	1358 <getfespeech>
     5b0:	1b21a2        	l32i	a10, a1, 108
     5b3:	70a1d2        	movi	a13, 0x170
     5b6:	7215a6        	blti	a5, 1, 62c <SMART_PITCH_PLC_PROCESS+0x37c>
     5b9:	30a2c2        	movi	a12, 0x230
     5bc:	02d4f2        	addmi	a15, a4, 0x200
     5bf:	01d492        	addmi	a9, a4, 0x100
     5c2:	90b720        	addx2	a11, a7, a2
     5c5:	fecbb2        	addi	a11, a11, -2
     5c8:	70c992        	addi	a9, a9, 112
     5cb:	f0cff2        	addi	a15, a15, -16
     5ce:	fecff2        	addi	a15, a15, -2
     5d1:	22a576        	loopgtz	a5, 5f7 <SMART_PITCH_PLC_PROCESS+0x347>
     5d4:	40cb14        	ae_lp16f.iu	aep4, a11, 2
     5d7:	40bf14        	ae_lp16f.iu	aep3, a15, 2
     5da:	023c74        	ae_mulp24s.ll	aeq1, aep3, aep4
     5dd:	369804        	ae_trunca32q48	a8, aeq1
     5e0:	994b           	addi.n	a9, a9, 4
     5e2:	3c4982        	s8i	a8, a9, 60
     5e5:	418880        	srli	a8, a8, 8
     5e8:	3d4982        	s8i	a8, a9, 61
     5eb:	418880        	srli	a8, a8, 8
     5ee:	3e4982        	s8i	a8, a9, 62
     5f1:	418880        	srli	a8, a8, 8
     5f4:	3f4982        	s8i	a8, a9, 63

000005f7 <SMART_PITCH_PLC_PROCESS+0x347>:
     5f7:	94da           	add.n	a9, a4, a13
     5f9:	90b740        	addx2	a11, a7, a4
     5fc:	bbca           	add.n	a11, a11, a12
     5fe:	fecbb2        	addi	a11, a11, -2
     601:	02d4c2        	addmi	a12, a4, 0x200
     604:	cceb           	addi.n	a12, a12, 14
     606:	22a576        	loopgtz	a5, 62c <SMART_PITCH_PLC_PROCESS+0x37c>
     609:	40eb14        	ae_lp16f.iu	aep6, a11, 2
     60c:	40dc14        	ae_lp16f.iu	aep5, a12, 2
     60f:	02d674        	ae_mulp24s.ll	aeq2, aep5, aep6
     612:	36ae04        	ae_trunca32q48	a14, aeq2
     615:	0049e2        	s8i	a14, a9, 0
     618:	41e8e0        	srli	a14, a14, 8
     61b:	0149e2        	s8i	a14, a9, 1
     61e:	41e8e0        	srli	a14, a14, 8
     621:	0249e2        	s8i	a14, a9, 2
     624:	41e8e0        	srli	a14, a14, 8
     627:	0349e2        	s8i	a14, a9, 3
     62a:	994b           	addi.n	a9, a9, 4

0000062c <SMART_PITCH_PLC_PROCESS+0x37c>:
     62c:	1d24f2        	l32i	a15, a4, 116
     62f:	fcc2b2        	addi	a11, a2, -4
     632:	218170        	srai	a8, a7, 1
     635:	ff0b           	addi.n	a15, a15, -1
     637:	094f56        	bnez	a15, 6cf <SMART_PITCH_PLC_PROCESS+0x41f>
     63a:	2617a6        	blti	a7, 1, 664 <SMART_PITCH_PLC_PROCESS+0x3b4>
     63d:	b46a           	add.n	a11, a4, a6
     63f:	20f220        	or	a15, a2, a2
     642:	1c24c2        	l32i	a12, a4, 112
     645:	1ba776        	loopgtz	a7, 664 <SMART_PITCH_PLC_PROCESS+0x3b4>
     648:	01db92        	addmi	a9, a11, 0x100
     64b:	010982        	l8ui	a8, a9, 1
     64e:	000992        	l8ui	a9, a9, 0
     651:	118880        	slli	a8, a8, 8
     654:	208890        	or	a8, a8, a9
     657:	888d           	ae_sext16	a8, a8
     659:	8288c0        	mull	a8, a8, a12
     65c:	bb2b           	addi.n	a11, a11, 2
     65e:	218f80        	srai	a8, a8, 15
     661:	09cf84        	ae_s16ip	a8, a15, 2

00000664 <SMART_PITCH_PLC_PROCESS+0x3b4>:
     664:	94da           	add.n	a9, a4, a13
     666:	90b720        	addx2	a11, a7, a2
     669:	70c4c2        	addi	a12, a4, 112
     66c:	40c1e2        	addi	a14, a1, 64
     66f:	57a576        	loopgtz	a5, 6ca <SMART_PITCH_PLC_PROCESS+0x41a>
     672:	402c04        	ae_lp16f.i	aep2, a12, 0
     675:	010982        	l8ui	a8, a9, 1
     678:	020972        	l8ui	a7, a9, 2
     67b:	030962        	l8ui	a6, a9, 3
     67e:	4309f2        	l8ui	a15, a9, 67
     681:	420952        	l8ui	a5, a9, 66
     684:	116680        	slli	a6, a6, 8
     687:	11ff80        	slli	a15, a15, 8
     68a:	20ff50        	or	a15, a15, a5
     68d:	206670        	or	a6, a6, a7
     690:	410952        	l8ui	a5, a9, 65
     693:	11ff80        	slli	a15, a15, 8
     696:	000972        	l8ui	a7, a9, 0
     699:	116680        	slli	a6, a6, 8
     69c:	206680        	or	a6, a6, a8
     69f:	116680        	slli	a6, a6, 8
     6a2:	206670        	or	a6, a6, a7
     6a5:	216f60        	srai	a6, a6, 15
     6a8:	20ff50        	or	a15, a15, a5
     6ab:	11ff80        	slli	a15, a15, 8
     6ae:	106162        	s32i	a6, a1, 64
     6b1:	401e04        	ae_lp16f.i	aep1, a14, 0
     6b4:	021274        	ae_mulp24s.ll	aeq0, aep1, aep2
     6b7:	400962        	l8ui	a6, a9, 64
     6ba:	368504        	ae_trunca32q48	a5, aeq0
     6bd:	994b           	addi.n	a9, a9, 4
     6bf:	20ff60        	or	a15, a15, a6
     6c2:	ff5a           	add.n	a15, a15, a5
     6c4:	21fff0        	srai	a15, a15, 15
     6c7:	09cbf4        	ae_s16ip	a15, a11, 2

000006ca <SMART_PITCH_PLC_PROCESS+0x41a>:
     6ca:	001a06        	j	736 <SMART_PITCH_PLC_PROCESS+0x486>

000006cd <SMART_PITCH_PLC_PROCESS+0x41d>:
	...

000006cf <SMART_PITCH_PLC_PROCESS+0x41f>:
     6cf:	02d4f2        	addmi	a15, a4, 0x200
     6d2:	2ccff2        	addi	a15, a15, 44
     6d5:	05a876        	loopgtz	a8, 6de <SMART_PITCH_PLC_PROCESS+0x42e>
     6d8:	44bf14        	ae_lp16x2f.iu	aep3, a15, 4
     6db:	46bb14        	ae_sp16x2f.iu	aep3, a11, 4

000006de <SMART_PITCH_PLC_PROCESS+0x42e>:
     6de:	94da           	add.n	a9, a4, a13
     6e0:	90b720        	addx2	a11, a7, a2
     6e3:	f03d           	nop.n
     6e5:	45a576        	loopgtz	a5, 72e <SMART_PITCH_PLC_PROCESS+0x47e>
     6e8:	400982        	l8ui	a8, a9, 64
     6eb:	000972        	l8ui	a7, a9, 0
     6ee:	0109c2        	l8ui	a12, a9, 1
     6f1:	4109d2        	l8ui	a13, a9, 65
     6f4:	0209e2        	l8ui	a14, a9, 2
     6f7:	030952        	l8ui	a5, a9, 3
     6fa:	430962        	l8ui	a6, a9, 67
     6fd:	4209f2        	l8ui	a15, a9, 66
     700:	115580        	slli	a5, a5, 8
     703:	116680        	slli	a6, a6, 8
     706:	2066f0        	or	a6, a6, a15
     709:	2055e0        	or	a5, a5, a14
     70c:	115580        	slli	a5, a5, 8
     70f:	116680        	slli	a6, a6, 8
     712:	2066d0        	or	a6, a6, a13
     715:	2055c0        	or	a5, a5, a12
     718:	115580        	slli	a5, a5, 8
     71b:	116680        	slli	a6, a6, 8
     71e:	994b           	addi.n	a9, a9, 4
     720:	206680        	or	a6, a6, a8
     723:	205570        	or	a5, a5, a7
     726:	556a           	add.n	a5, a5, a6
     728:	215f50        	srai	a5, a5, 15
     72b:	09cb54        	ae_s16ip	a5, a11, 2
     72e:	000106        	j	736 <SMART_PITCH_PLC_PROCESS+0x486>

00000731 <SMART_PITCH_PLC_PROCESS+0x481>:
	...

00000733 <SMART_PITCH_PLC_PROCESS+0x483>:
     733:	2421a2        	l32i	a10, a1, 144

00000736 <SMART_PITCH_PLC_PROCESS+0x486>:
     736:	68a692        	movi	a9, 0x668
     739:	06d4f2        	addmi	a15, a4, 0x600
     73c:	0a6d           	mov.n	a6, a10
     73e:	c0d3a0        	sub	a13, a3, a10
     741:	90ba40        	addx2	a11, a10, a4
     744:	68cff2        	addi	a15, a15, 104
     747:	06dbb2        	addmi	a11, a11, 0x600
     74a:	68cbb2        	addi	a11, a11, 104
     74d:	1aad76        	loopgtz	a13, 76b <SMART_PITCH_PLC_PROCESS+0x4bb>
     750:	780be2        	l8ui	a14, a11, 120
     753:	790bc2        	l8ui	a12, a11, 121
     756:	ff2b           	addi.n	a15, a15, 2
     758:	bb2b           	addi.n	a11, a11, 2
     75a:	11cc80        	slli	a12, a12, 8
     75d:	20cce0        	or	a12, a12, a14
     760:	8ccd           	ae_sext16	a12, a12
     762:	764fc2        	s8i	a12, a15, 118
     765:	41c8c0        	srli	a12, a12, 8
     768:	774fc2        	s8i	a12, a15, 119

0000076b <SMART_PITCH_PLC_PROCESS+0x4bb>:
     76b:	0b0c           	movi.n	a11, 0
     76d:	02fd           	mov.n	a15, a2
     76f:	90ed40        	addx2	a14, a13, a4
     772:	809e90        	add	a9, a14, a9
     775:	0fa676        	loopgtz	a6, 788 <SMART_PITCH_PLC_PROCESS+0x4d8>
     778:	992b           	addi.n	a9, a9, 2
     77a:	2f8d           	ae_l16si.n	a8, a15, 0
     77c:	02cff2        	addi	a15, a15, 2
     77f:	764982        	s8i	a8, a9, 118
     782:	418880        	srli	a8, a8, 8
     785:	774982        	s8i	a8, a9, 119
     788:	1921a2        	l32i	a10, a1, 100
     78b:	20c660        	or	a12, a6, a6
     78e:	20d440        	or	a13, a4, a4
     791:	1964b2        	s32i	a11, a4, 100
     794:	1d64b2        	s32i	a11, a4, 116
     797:	fe2391        	l32r	a9, 24 (7fff <_end+0x66c7>)
     79a:	1c6492        	s32i	a9, a4, 112
     79d:	1b6492        	s32i	a9, a4, 108
     7a0:	02bd           	mov.n	a11, a2
     7a2:	0093e5        	call8	10e0 <plc_gainctrl>
     7a5:	f01d           	retw.n

000007a7 <SMART_PITCH_PLC_PROCESS+0x4f7>:
     7a7:	44b8           	l32i.n	a11, a4, 16
     7a9:	201b66        	bnei	a11, 1, 7cd <SMART_PITCH_PLC_PROCESS+0x51d>
     7ac:	112492        	l32i	a9, a4, 68
     7af:	0519f7        	beq	a9, a15, 7b8 <SMART_PITCH_PLC_PROCESS+0x508>
     7b2:	1524b2        	l32i	a11, a4, 84
     7b5:	149bf7        	bne	a11, a15, 7cd <SMART_PITCH_PLC_PROCESS+0x51d>

000007b8 <SMART_PITCH_PLC_PROCESS+0x508>:
     7b8:	e4e8           	l32i.n	a14, a4, 56
     7ba:	ee0b           	addi.n	a14, a14, -1
     7bc:	39ce56        	bnez	a14, b5c <SMART_PITCH_PLC_PROCESS+0x8ac>
     7bf:	1c24c2        	l32i	a12, a4, 112
     7c2:	f4b8           	l32i.n	a11, a4, 60
     7c4:	82bbc0        	mull	a11, a11, a12
     7c7:	21bfb0        	srai	a11, a11, 15
     7ca:	00e446        	j	b5f <SMART_PITCH_PLC_PROCESS+0x8af>

000007cd <SMART_PITCH_PLC_PROCESS+0x51d>:
     7cd:	1d24e2        	l32i	a14, a4, 116
     7d0:	ffcee2        	addi	a14, a14, -1
     7d3:	1d7e16        	beqz	a14, 9ae <SMART_PITCH_PLC_PROCESS+0x6fe>
     7d6:	0e2482        	l32i	a8, a4, 56
     7d9:	ffc882        	addi	a8, a8, -1
     7dc:	1ce816        	beqz	a8, 9ae <SMART_PITCH_PLC_PROCESS+0x6fe>
     7df:	b47a           	add.n	a11, a4, a7

000007e1 <SMART_PITCH_PLC_PROCESS+0x531>:
     7e1:	1724c2        	l32i	a12, a4, 92
     7e4:	04fd           	mov.n	a15, a4
     7e6:	242162        	l32i	a6, a1, 144
     7e9:	0bad           	mov.n	a10, a11
     7eb:	07d4e2        	addmi	a14, a4, 0x700
     7ee:	e0cee2        	addi	a14, a14, -32
     7f1:	1624b2        	l32i	a11, a4, 88
     7f4:	06dd           	mov.n	a13, a6
     7f6:	00b625        	call8	1358 <getfespeech>
     7f9:	1d24f2        	l32i	a15, a4, 116
     7fc:	ff0b           	addi.n	a15, a15, -1
     7fe:	18bf16        	beqz	a15, 98d <SMART_PITCH_PLC_PROCESS+0x6dd>
     801:	e488           	l32i.n	a8, a4, 56
     803:	880b           	addi.n	a8, a8, -1
     805:	184816        	beqz	a8, 98d <SMART_PITCH_PLC_PROCESS+0x6dd>

00000808 <SMART_PITCH_PLC_PROCESS+0x558>:
     808:	fcc2b2        	addi	a11, a2, -4
     80b:	219160        	srai	a9, a6, 1
     80e:	07d4f2        	addmi	a15, a4, 0x700
     811:	fe0481        	l32r	a8, 24 (7fff <_end+0x66c7>)
     814:	1b6482        	s32i	a8, a4, 108
     817:	dccff2        	addi	a15, a15, -36
     81a:	05a976        	loopgtz	a9, 823 <SMART_PITCH_PLC_PROCESS+0x573>
     81d:	44cf14        	ae_lp16x2f.iu	aep4, a15, 4
     820:	46cb14        	ae_sp16x2f.iu	aep4, a11, 4

00000823 <SMART_PITCH_PLC_PROCESS+0x573>:
     823:	68a6c2        	movi	a12, 0x668
     826:	c0d360        	sub	a13, a3, a6
     829:	90b640        	addx2	a11, a6, a4
     82c:	06d4f2        	addmi	a15, a4, 0x600
     82f:	68cff2        	addi	a15, a15, 104
     832:	06dbb2        	addmi	a11, a11, 0x600
     835:	68cbb2        	addi	a11, a11, 104
     838:	1aad76        	loopgtz	a13, 856 <SMART_PITCH_PLC_PROCESS+0x5a6>
     83b:	780b82        	l8ui	a8, a11, 120
     83e:	790be2        	l8ui	a14, a11, 121
     841:	ff2b           	addi.n	a15, a15, 2
     843:	bb2b           	addi.n	a11, a11, 2
     845:	11ee80        	slli	a14, a14, 8
     848:	20ee80        	or	a14, a14, a8
     84b:	8eed           	ae_sext16	a14, a14
     84d:	764fe2        	s8i	a14, a15, 118
     850:	41e8e0        	srli	a14, a14, 8
     853:	774fe2        	s8i	a14, a15, 119

00000856 <SMART_PITCH_PLC_PROCESS+0x5a6>:
     856:	02fd           	mov.n	a15, a2
     858:	909d40        	addx2	a9, a13, a4
     85b:	99ca           	add.n	a9, a9, a12
     85d:	0ea676        	loopgtz	a6, 86f <SMART_PITCH_PLC_PROCESS+0x5bf>
     860:	992b           	addi.n	a9, a9, 2
     862:	2fad           	ae_l16si.n	a10, a15, 0
     864:	ff2b           	addi.n	a15, a15, 2
     866:	7649a2        	s8i	a10, a9, 118
     869:	41a8a0        	srli	a10, a10, 8
     86c:	7749a2        	s8i	a10, a9, 119
     86f:	003d86        	j	969 <SMART_PITCH_PLC_PROCESS+0x6b9>

00000872 <SMART_PITCH_PLC_PROCESS+0x5c2>:
     872:	02fd           	mov.n	a15, a2
     874:	70c4c2        	addi	a12, a4, 112
     877:	6cc4b2        	addi	a11, a4, 108
     87a:	40c172        	addi	a7, a1, 64
     87d:	68a576        	loopgtz	a5, 8e9 <SMART_PITCH_PLC_PROCESS+0x639>
     880:	402b04        	ae_lp16f.i	aep2, a11, 0
     883:	404c04        	ae_lp16f.i	aep4, a12, 0
     886:	420982        	l8ui	a8, a9, 66
     889:	0309a2        	l8ui	a10, a9, 3
     88c:	0209d2        	l8ui	a13, a9, 2
     88f:	0109e2        	l8ui	a14, a9, 1
     892:	11aa80        	slli	a10, a10, 8
     895:	20aad0        	or	a10, a10, a13
     898:	11aa80        	slli	a10, a10, 8
     89b:	0009d2        	l8ui	a13, a9, 0
     89e:	20aae0        	or	a10, a10, a14
     8a1:	4309e2        	l8ui	a14, a9, 67
     8a4:	11aa80        	slli	a10, a10, 8
     8a7:	20aad0        	or	a10, a10, a13
     8aa:	21afa0        	srai	a10, a10, 15
     8ad:	11ee80        	slli	a14, a14, 8
     8b0:	1061a2        	s32i	a10, a1, 64
     8b3:	403704        	ae_lp16f.i	aep3, a7, 0
     8b6:	20ee80        	or	a14, a14, a8
     8b9:	4109a2        	l8ui	a10, a9, 65
     8bc:	400982        	l8ui	a8, a9, 64
     8bf:	11ee80        	slli	a14, a14, 8
     8c2:	20eea0        	or	a14, a14, a10
     8c5:	11ee80        	slli	a14, a14, 8
     8c8:	20ee80        	or	a14, a14, a8
     8cb:	21efe0        	srai	a14, a14, 15
     8ce:	1061e2        	s32i	a14, a1, 64
     8d1:	ea8fac170f 	{ ae_lp16f.i	aep1, a7, 0; ae_mulp24s.ll	aeq0, aep3, aep4 }
     8d6:	029a74        	ae_mulp24s.ll	aeq3, aep1, aep2
     8d9:	368a04        	ae_trunca32q48	a10, aeq0
     8dc:	36bd04        	ae_trunca32q48	a13, aeq3
     8df:	994b           	addi.n	a9, a9, 4
     8e1:	aada           	add.n	a10, a10, a13
     8e3:	21afa0        	srai	a10, a10, 15
     8e6:	09cfa4        	ae_s16ip	a10, a15, 2

000008e9 <SMART_PITCH_PLC_PROCESS+0x639>:
     8e9:	c0c650        	sub	a12, a6, a5
     8ec:	2a1ca6        	blti	a12, 1, 91a <SMART_PITCH_PLC_PROCESS+0x66a>
     8ef:	909520        	addx2	a9, a5, a2
     8f2:	1b24d2        	l32i	a13, a4, 108
     8f5:	90b540        	addx2	a11, a5, a4
     8f8:	06dbb2        	addmi	a11, a11, 0x600
     8fb:	68cbb2        	addi	a11, a11, 104
     8fe:	18ac76        	loopgtz	a12, 91a <SMART_PITCH_PLC_PROCESS+0x66a>
     901:	790be2        	l8ui	a14, a11, 121
     904:	780bf2        	l8ui	a15, a11, 120
     907:	11ee80        	slli	a14, a14, 8
     90a:	20eef0        	or	a14, a14, a15
     90d:	8eed           	ae_sext16	a14, a14
     90f:	82eed0        	mull	a14, a14, a13
     912:	bb2b           	addi.n	a11, a11, 2
     914:	21efe0        	srai	a14, a14, 15
     917:	09c9e4        	ae_s16ip	a14, a9, 2

0000091a <SMART_PITCH_PLC_PROCESS+0x66a>:
     91a:	c0d360        	sub	a13, a3, a6
     91d:	90b640        	addx2	a11, a6, a4
     920:	06d4f2        	addmi	a15, a4, 0x600
     923:	68cff2        	addi	a15, a15, 104
     926:	06dbb2        	addmi	a11, a11, 0x600
     929:	68cbb2        	addi	a11, a11, 104
     92c:	f03d           	nop.n
     92e:	1aad76        	loopgtz	a13, 94c <SMART_PITCH_PLC_PROCESS+0x69c>
     931:	780be2        	l8ui	a14, a11, 120
     934:	790bc2        	l8ui	a12, a11, 121
     937:	ff2b           	addi.n	a15, a15, 2
     939:	bb2b           	addi.n	a11, a11, 2
     93b:	11cc80        	slli	a12, a12, 8
     93e:	20cce0        	or	a12, a12, a14
     941:	8ccd           	ae_sext16	a12, a12
     943:	764fc2        	s8i	a12, a15, 118
     946:	41c8c0        	srli	a12, a12, 8
     949:	774fc2        	s8i	a12, a15, 119

0000094c <SMART_PITCH_PLC_PROCESS+0x69c>:
     94c:	02fd           	mov.n	a15, a2
     94e:	909d40        	addx2	a9, a13, a4
     951:	06d992        	addmi	a9, a9, 0x600
     954:	68c992        	addi	a9, a9, 104
     957:	0ea676        	loopgtz	a6, 969 <SMART_PITCH_PLC_PROCESS+0x6b9>
     95a:	992b           	addi.n	a9, a9, 2
     95c:	2fbd           	ae_l16si.n	a11, a15, 0
     95e:	ff2b           	addi.n	a15, a15, 2
     960:	7649b2        	s8i	a11, a9, 118
     963:	41b8b0        	srli	a11, a11, 8
     966:	7749b2        	s8i	a11, a9, 119

00000969 <SMART_PITCH_PLC_PROCESS+0x6b9>:
     969:	1b24c2        	l32i	a12, a4, 108
     96c:	1c64c2        	s32i	a12, a4, 112

0000096f <SMART_PITCH_PLC_PROCESS+0x6bf>:
     96f:	20c660        	or	a12, a6, a6
     972:	20b220        	or	a11, a2, a2
     975:	1921a2        	l32i	a10, a1, 100
     978:	1924d2        	l32i	a13, a4, 100
     97b:	0e24e2        	l32i	a14, a4, 56
     97e:	1d64e2        	s32i	a14, a4, 116
     981:	dd1b           	addi.n	a13, a13, 1
     983:	1964d2        	s32i	a13, a4, 100
     986:	04dd           	mov.n	a13, a4
     988:	007565        	call8	10e0 <plc_gainctrl>
     98b:	f01d           	retw.n

0000098d <SMART_PITCH_PLC_PROCESS+0x6dd>:
     98d:	192482        	l32i	a8, a4, 100
     990:	1124f2        	l32i	a15, a4, 68
     993:	022f87        	blt	a15, a8, 999 <SMART_PITCH_PLC_PROCESS+0x6e9>
     996:	ff9b86        	j	808 <SMART_PITCH_PLC_PROCESS+0x558>
     999:	e498           	l32i.n	a9, a4, 56
     99b:	990b           	addi.n	a9, a9, -1
     99d:	0a8956        	bnez	a9, a49 <SMART_PITCH_PLC_PROCESS+0x799>
     9a0:	1c24a2        	l32i	a10, a4, 112
     9a3:	f498           	l32i.n	a9, a4, 60
     9a5:	8299a0        	mull	a9, a9, a10
     9a8:	219f90        	srai	a9, a9, 15
     9ab:	002746        	j	a4c <SMART_PITCH_PLC_PROCESS+0x79c>

000009ae <SMART_PITCH_PLC_PROCESS+0x6fe>:
     9ae:	1124e2        	l32i	a14, a4, 68
     9b1:	80b470        	add	a11, a4, a7
     9b4:	1361b2        	s32i	a11, a1, 76
     9b7:	022ef7        	blt	a14, a15, 9bd <SMART_PITCH_PLC_PROCESS+0x70d>
     9ba:	ff88c6        	j	7e1 <SMART_PITCH_PLC_PROCESS+0x531>
     9bd:	1724c2        	l32i	a12, a4, 92
     9c0:	20d550        	or	a13, a5, a5
     9c3:	20ebb0        	or	a14, a11, a11
     9c6:	0bad           	mov.n	a10, a11
     9c8:	1224f2        	l32i	a15, a4, 72
     9cb:	1a61f2        	s32i	a15, a1, 104
     9ce:	1624b2        	l32i	a11, a4, 88
     9d1:	04fd           	mov.n	a15, a4
     9d3:	009865        	call8	1358 <getfespeech>
     9d6:	1321b2        	l32i	a11, a1, 76
     9d9:	1a2182        	l32i	a8, a1, 104
     9dc:	126482        	s32i	a8, a4, 72
     9df:	ff7f86        	j	7e1 <SMART_PITCH_PLC_PROCESS+0x531>

000009e2 <SMART_PITCH_PLC_PROCESS+0x732>:
     9e2:	112152        	l32i	a5, a1, 68
     9e5:	c05650        	sub	a5, a6, a5

000009e8 <SMART_PITCH_PLC_PROCESS+0x738>:
     9e8:	1321a2        	l32i	a10, a1, 76
     9eb:	1624b2        	l32i	a11, a4, 88
     9ee:	1724c2        	l32i	a12, a4, 92
     9f1:	05dd           	mov.n	a13, a5
     9f3:	04fd           	mov.n	a15, a4
     9f5:	0bd4e2        	addmi	a14, a4, 0xb00
     9f8:	68cee2        	addi	a14, a14, 104
     9fb:	0020f0        	nop
     9fe:	0095a5        	call8	1358 <getfespeech>
     a01:	1121b2        	l32i	a11, a1, 68
     a04:	909740        	addx2	a9, a7, a4
     a07:	0ad4c2        	addmi	a12, a4, 0xa00
     a0a:	6cccc2        	addi	a12, a12, 108
     a0d:	06d992        	addmi	a9, a9, 0x600
     a10:	e4c992        	addi	a9, a9, -28
     a13:	90bb20        	addx2	a11, a11, a2
     a16:	2ba576        	loopgtz	a5, a45 <SMART_PITCH_PLC_PROCESS+0x795>
     a19:	fd0ce2        	l8ui	a14, a12, 253
     a1c:	fc0cf2        	l8ui	a15, a12, 252
     a1f:	cc2b           	addi.n	a12, a12, 2
     a21:	11ee80        	slli	a14, a14, 8
     a24:	20eef0        	or	a14, a14, a15
     a27:	8eed           	ae_sext16	a14, a14
     a29:	09cbe4        	ae_s16ip	a14, a11, 2
     a2c:	fb0cd2        	l8ui	a13, a12, 251
     a2f:	fa0ce2        	l8ui	a14, a12, 250
     a32:	992b           	addi.n	a9, a9, 2
     a34:	11dd80        	slli	a13, a13, 8
     a37:	20dde0        	or	a13, a13, a14
     a3a:	8ddd           	ae_sext16	a13, a13
     a3c:	fa49d2        	s8i	a13, a9, 250
     a3f:	41d8d0        	srli	a13, a13, 8
     a42:	fb49d2        	s8i	a13, a9, 251

00000a45 <SMART_PITCH_PLC_PROCESS+0x795>:
     a45:	ffc986        	j	96f <SMART_PITCH_PLC_PROCESS+0x6bf>

00000a48 <SMART_PITCH_PLC_PROCESS+0x798>:
	...

00000a49 <SMART_PITCH_PLC_PROCESS+0x799>:
     a49:	fd7691        	l32r	a9, 24 (7fff <_end+0x66c7>)

00000a4c <SMART_PITCH_PLC_PROCESS+0x79c>:
     a4c:	21d150        	srai	a13, a5, 1
     a4f:	1b6492        	s32i	a9, a4, 108
     a52:	02d4b2        	addmi	a11, a4, 0x200
     a55:	02d4f2        	addmi	a15, a4, 0x200
     a58:	ffcb           	addi.n	a15, a15, 12
     a5a:	2ccbb2        	addi	a11, a11, 44
     a5d:	01d492        	addmi	a9, a4, 0x100
     a60:	70c992        	addi	a9, a9, 112
     a63:	09cd           	mov.n	a12, a9
     a65:	17ad76        	loopgtz	a13, a80 <SMART_PITCH_PLC_PROCESS+0x7d0>
     a68:	44eb14        	ae_lp16x2f.iu	aep6, a11, 4
     a6b:	44df14        	ae_lp16x2f.iu	aep5, a15, 4
     a6e:	025e74        	ae_mulp24s.ll	aeq1, aep5, aep6
     a71:	02d644        	ae_mulp24s.hh	aeq2, aep5, aep6
     a74:	369804        	ae_trunca32q48	a8, aeq1
     a77:	36aa04        	ae_trunca32q48	a10, aeq2
     a7a:	0ca9           	s32i.n	a10, a12, 0
     a7c:	1c89           	s32i.n	a8, a12, 4
     a7e:	cc8b           	addi.n	a12, a12, 8

00000a80 <SMART_PITCH_PLC_PROCESS+0x7d0>:
     a80:	02d4c2        	addmi	a12, a4, 0x200
     a83:	07d4b2        	addmi	a11, a4, 0x700
     a86:	02d4f2        	addmi	a15, a4, 0x200
     a89:	eccff2        	addi	a15, a15, -20
     a8c:	dccbb2        	addi	a11, a11, -36
     a8f:	b0ccc2        	addi	a12, a12, -80
     a92:	17ad76        	loopgtz	a13, aad <SMART_PITCH_PLC_PROCESS+0x7fd>
     a95:	448b14        	ae_lp16x2f.iu	aep0, a11, 4
     a98:	44ff14        	ae_lp16x2f.iu	aep7, a15, 4
     a9b:	02f874        	ae_mulp24s.ll	aeq3, aep7, aep0
     a9e:	027044        	ae_mulp24s.hh	aeq0, aep7, aep0
     aa1:	36b804        	ae_trunca32q48	a8, aeq3
     aa4:	368a04        	ae_trunca32q48	a10, aeq0
     aa7:	0ca9           	s32i.n	a10, a12, 0
     aa9:	1c89           	s32i.n	a8, a12, 4
     aab:	cc8b           	addi.n	a12, a12, 8
     aad:	20f220        	or	a15, a2, a2
     ab0:	70c4c2        	addi	a12, a4, 112
     ab3:	6cc4b2        	addi	a11, a4, 108
     ab6:	40c172        	addi	a7, a1, 64
     ab9:	68a576        	loopgtz	a5, b25 <SMART_PITCH_PLC_PROCESS+0x875>
     abc:	406b04        	ae_lp16f.i	aep6, a11, 0
     abf:	400c04        	ae_lp16f.i	aep0, a12, 0
     ac2:	420982        	l8ui	a8, a9, 66
     ac5:	0309a2        	l8ui	a10, a9, 3
     ac8:	0209d2        	l8ui	a13, a9, 2
     acb:	0109e2        	l8ui	a14, a9, 1
     ace:	11aa80        	slli	a10, a10, 8
     ad1:	20aad0        	or	a10, a10, a13
     ad4:	11aa80        	slli	a10, a10, 8
     ad7:	0009d2        	l8ui	a13, a9, 0
     ada:	20aae0        	or	a10, a10, a14
     add:	4309e2        	l8ui	a14, a9, 67
     ae0:	11aa80        	slli	a10, a10, 8
     ae3:	20aad0        	or	a10, a10, a13
     ae6:	21afa0        	srai	a10, a10, 15
     ae9:	11ee80        	slli	a14, a14, 8
     aec:	1061a2        	s32i	a10, a1, 64
     aef:	407704        	ae_lp16f.i	aep7, a7, 0
     af2:	20ee80        	or	a14, a14, a8
     af5:	4109a2        	l8ui	a10, a9, 65
     af8:	400982        	l8ui	a8, a9, 64
     afb:	11ee80        	slli	a14, a14, 8
     afe:	20eea0        	or	a14, a14, a10
     b01:	11ee80        	slli	a14, a14, 8
     b04:	20ee80        	or	a14, a14, a8
     b07:	21efe0        	srai	a14, a14, 15
     b0a:	1061e2        	s32i	a14, a1, 64
     b0d:	ea1fac570f 	{ ae_lp16f.i	aep5, a7, 0; ae_mulp24s.ll	aeq0, aep7, aep0 }
     b12:	02de74        	ae_mulp24s.ll	aeq3, aep5, aep6
     b15:	368a04        	ae_trunca32q48	a10, aeq0
     b18:	36bd04        	ae_trunca32q48	a13, aeq3
     b1b:	994b           	addi.n	a9, a9, 4
     b1d:	aada           	add.n	a10, a10, a13
     b1f:	21afa0        	srai	a10, a10, 15
     b22:	09cfa4        	ae_s16ip	a10, a15, 2
     b25:	c0c650        	sub	a12, a6, a5
     b28:	021ce6        	bgei	a12, 1, b2e <SMART_PITCH_PLC_PROCESS+0x87e>
     b2b:	ff3d06        	j	823 <SMART_PITCH_PLC_PROCESS+0x573>
     b2e:	909520        	addx2	a9, a5, a2
     b31:	1b24d2        	l32i	a13, a4, 108
     b34:	90b540        	addx2	a11, a5, a4
     b37:	06dbb2        	addmi	a11, a11, 0x600
     b3a:	e4cbb2        	addi	a11, a11, -28
     b3d:	18ac76        	loopgtz	a12, b59 <SMART_PITCH_PLC_PROCESS+0x8a9>
     b40:	fd0be2        	l8ui	a14, a11, 253
     b43:	fc0bf2        	l8ui	a15, a11, 252
     b46:	11ee80        	slli	a14, a14, 8
     b49:	20eef0        	or	a14, a14, a15
     b4c:	8eed           	ae_sext16	a14, a14
     b4e:	82eed0        	mull	a14, a14, a13
     b51:	bb2b           	addi.n	a11, a11, 2
     b53:	21efe0        	srai	a14, a14, 15
     b56:	09c9e4        	ae_s16ip	a14, a9, 2
     b59:	ff3186        	j	823 <SMART_PITCH_PLC_PROCESS+0x573>

00000b5c <SMART_PITCH_PLC_PROCESS+0x8ac>:
     b5c:	fd32b1        	l32r	a11, 24 (7fff <_end+0x66c7>)

00000b5f <SMART_PITCH_PLC_PROCESS+0x8af>:
     b5f:	1724c2        	l32i	a12, a4, 92
     b62:	20d550        	or	a13, a5, a5
     b65:	1224f2        	l32i	a15, a4, 72
     b68:	1b64b2        	s32i	a11, a4, 108
     b6b:	80e470        	add	a14, a4, a7
     b6e:	1361e2        	s32i	a14, a1, 76
     b71:	1624b2        	l32i	a11, a4, 88
     b74:	1a61f2        	s32i	a15, a1, 104
     b77:	20aee0        	or	a10, a14, a14
     b7a:	04fd           	mov.n	a15, a4
     b7c:	007da5        	call8	1358 <getfespeech>
     b7f:	01cd           	mov.n	a12, a1
     b81:	217150        	srai	a7, a5, 1
     b84:	182482        	l32i	a8, a4, 96
     b87:	1a21d2        	l32i	a13, a1, 104
     b8a:	2224a2        	l32i	a10, a4, 136
     b8d:	162492        	l32i	a9, a4, 88
     b90:	02d4f2        	addmi	a15, a4, 0x200
     b93:	06d4b2        	addmi	a11, a4, 0x600
     b96:	64cbb2        	addi	a11, a11, 100
     b99:	ffcb           	addi.n	a15, a15, 12
     b9b:	1861f2        	s32i	a15, a1, 96
     b9e:	99aa           	add.n	a9, a9, a10
     ba0:	dada           	add.n	a13, a10, a13
     ba2:	1264d2        	s32i	a13, a4, 72
     ba5:	166492        	s32i	a9, a4, 88
     ba8:	c08890        	sub	a8, a8, a9
     bab:	176482        	s32i	a8, a4, 92
     bae:	17a776        	loopgtz	a7, bc9 <SMART_PITCH_PLC_PROCESS+0x919>
     bb1:	44ab14        	ae_lp16x2f.iu	aep2, a11, 4
     bb4:	449f14        	ae_lp16x2f.iu	aep1, a15, 4
     bb7:	021a74        	ae_mulp24s.ll	aeq1, aep1, aep2
     bba:	029244        	ae_mulp24s.hh	aeq2, aep1, aep2
     bbd:	369d04        	ae_trunca32q48	a13, aeq1
     bc0:	36ae04        	ae_trunca32q48	a14, aeq2
     bc3:	0ce9           	s32i.n	a14, a12, 0
     bc5:	1cd9           	s32i.n	a13, a12, 4
     bc7:	cc8b           	addi.n	a12, a12, 8

00000bc9 <SMART_PITCH_PLC_PROCESS+0x919>:
     bc9:	b46a           	add.n	a11, a4, a6
     bcb:	090c           	movi.n	a9, 0
     bcd:	02d4f2        	addmi	a15, a4, 0x200
     bd0:	eccff2        	addi	a15, a15, -20
     bd3:	1761f2        	s32i	a15, a1, 92
     bd6:	2aa576        	loopgtz	a5, c04 <SMART_PITCH_PLC_PROCESS+0x954>
     bd9:	1724c2        	l32i	a12, a4, 92
     bdc:	c0cc50        	sub	a12, a12, a5
     bdf:	c9ca           	add.n	a12, a9, a12
     be1:	90cc40        	addx2	a12, a12, a4
     be4:	cc6a           	add.n	a12, a12, a6
     be6:	01dcc2        	addmi	a12, a12, 0x100
     be9:	010ca2        	l8ui	a10, a12, 1
     bec:	000cc2        	l8ui	a12, a12, 0
     bef:	991b           	addi.n	a9, a9, 1
     bf1:	11aa80        	slli	a10, a10, 8
     bf4:	20aac0        	or	a10, a10, a12
     bf7:	8aad           	ae_sext16	a10, a10
     bf9:	4188a0        	srli	a8, a10, 8
     bfc:	004ba2        	s8i	a10, a11, 0
     bff:	014b82        	s8i	a8, a11, 1
     c02:	bb2b           	addi.n	a11, a11, 2
     c04:	fcc1c2        	addi	a12, a1, -4
     c07:	01d4b2        	addmi	a11, a4, 0x100
     c0a:	2ccbb2        	addi	a11, a11, 44
     c0d:	1ba776        	loopgtz	a7, c2c <SMART_PITCH_PLC_PROCESS+0x97c>
     c10:	44cb14        	ae_lp16x2f.iu	aep4, a11, 4
     c13:	44bf14        	ae_lp16x2f.iu	aep3, a15, 4
     c16:	601c14        	ae_lq32f.iu	aeq0, a12, 4
     c19:	023404        	ae_mulap24s.hh	aeq0, aep3, aep4
     c1c:	368e04        	ae_trunca32q48	a14, aeq0
     c1f:	0ce9           	s32i.n	a14, a12, 0
     c21:	60dc14        	ae_lq32f.iu	aeq3, a12, 4
     c24:	02bc34        	ae_mulap24s.ll	aeq3, aep3, aep4
     c27:	36bd04        	ae_trunca32q48	a13, aeq3
     c2a:	0cd9           	s32i.n	a13, a12, 0
     c2c:	01bd           	mov.n	a11, a1
     c2e:	c09350        	sub	a9, a3, a5
     c31:	909940        	addx2	a9, a9, a4
     c34:	996a           	add.n	a9, a9, a6
     c36:	17a576        	loopgtz	a5, c51 <SMART_PITCH_PLC_PROCESS+0x9a1>
     c39:	01d9c2        	addmi	a12, a9, 0x100
     c3c:	002ba2        	l32i	a10, a11, 0
     c3f:	02c992        	addi	a9, a9, 2
     c42:	04cbb2        	addi	a11, a11, 4
     c45:	21afa0        	srai	a10, a10, 15
     c48:	004ca2        	s8i	a10, a12, 0
     c4b:	41a8a0        	srli	a10, a10, 8
     c4e:	014ca2        	s8i	a10, a12, 1
     c51:	1321a2        	l32i	a10, a1, 76
     c54:	1624b2        	l32i	a11, a4, 88
     c57:	1724c2        	l32i	a12, a4, 92
     c5a:	04fd           	mov.n	a15, a4
     c5c:	242162        	l32i	a6, a1, 144
     c5f:	07d4e2        	addmi	a14, a4, 0x700
     c62:	e0cee2        	addi	a14, a14, -32
     c65:	20d660        	or	a13, a6, a6
     c68:	006ee5        	call8	1358 <getfespeech>
     c6b:	1821f2        	l32i	a15, a1, 96
     c6e:	01d492        	addmi	a9, a4, 0x100
     c71:	02d4b2        	addmi	a11, a4, 0x200
     c74:	2ccbb2        	addi	a11, a11, 44
     c77:	70c992        	addi	a9, a9, 112
     c7a:	20c990        	or	a12, a9, a9
     c7d:	17a776        	loopgtz	a7, c98 <SMART_PITCH_PLC_PROCESS+0x9e8>
     c80:	44eb14        	ae_lp16x2f.iu	aep6, a11, 4
     c83:	44df14        	ae_lp16x2f.iu	aep5, a15, 4
     c86:	025e74        	ae_mulp24s.ll	aeq1, aep5, aep6
     c89:	02d644        	ae_mulp24s.hh	aeq2, aep5, aep6
     c8c:	369d04        	ae_trunca32q48	a13, aeq1
     c8f:	36ae04        	ae_trunca32q48	a14, aeq2
     c92:	0ce9           	s32i.n	a14, a12, 0
     c94:	1cd9           	s32i.n	a13, a12, 4
     c96:	cc8b           	addi.n	a12, a12, 8
     c98:	1721f2        	l32i	a15, a1, 92
     c9b:	02d4c2        	addmi	a12, a4, 0x200
     c9e:	07d4b2        	addmi	a11, a4, 0x700
     ca1:	dccbb2        	addi	a11, a11, -36
     ca4:	b0ccc2        	addi	a12, a12, -80
     ca7:	17a776        	loopgtz	a7, cc2 <SMART_PITCH_PLC_PROCESS+0xa12>
     caa:	448b14        	ae_lp16x2f.iu	aep0, a11, 4
     cad:	44ff14        	ae_lp16x2f.iu	aep7, a15, 4
     cb0:	02f874        	ae_mulp24s.ll	aeq3, aep7, aep0
     cb3:	027044        	ae_mulp24s.hh	aeq0, aep7, aep0
     cb6:	36bd04        	ae_trunca32q48	a13, aeq3
     cb9:	368e04        	ae_trunca32q48	a14, aeq0
     cbc:	0ce9           	s32i.n	a14, a12, 0
     cbe:	1cd9           	s32i.n	a13, a12, 4
     cc0:	cc8b           	addi.n	a12, a12, 8
     cc2:	1d24f2        	l32i	a15, a4, 116
     cc5:	ff0b           	addi.n	a15, a15, -1
     cc7:	ba7f16        	beqz	a15, 872 <SMART_PITCH_PLC_PROCESS+0x5c2>
     cca:	e488           	l32i.n	a8, a4, 56
     ccc:	ffc882        	addi	a8, a8, -1
     ccf:	b9f816        	beqz	a8, 872 <SMART_PITCH_PLC_PROCESS+0x5c2>
     cd2:	20f220        	or	a15, a2, a2
     cd5:	45a576        	loopgtz	a5, d1e <SMART_PITCH_PLC_PROCESS+0xa6e>
     cd8:	0209e2        	l8ui	a14, a9, 2
     cdb:	0309c2        	l8ui	a12, a9, 3
     cde:	4309d2        	l8ui	a13, a9, 67
     ce1:	420972        	l8ui	a7, a9, 66
     ce4:	11cc80        	slli	a12, a12, 8
     ce7:	11dd80        	slli	a13, a13, 8
     cea:	20dd70        	or	a13, a13, a7
     ced:	20cce0        	or	a12, a12, a14
     cf0:	010972        	l8ui	a7, a9, 1
     cf3:	4109e2        	l8ui	a14, a9, 65
     cf6:	11cc80        	slli	a12, a12, 8
     cf9:	11dd80        	slli	a13, a13, 8
     cfc:	20dde0        	or	a13, a13, a14
     cff:	20cc70        	or	a12, a12, a7
     d02:	11cc80        	slli	a12, a12, 8
     d05:	11dd80        	slli	a13, a13, 8
     d08:	400972        	l8ui	a7, a9, 64
     d0b:	0009e2        	l8ui	a14, a9, 0
     d0e:	994b           	addi.n	a9, a9, 4
     d10:	20dd70        	or	a13, a13, a7
     d13:	20cce0        	or	a12, a12, a14
     d16:	ccda           	add.n	a12, a12, a13
     d18:	21cfc0        	srai	a12, a12, 15
     d1b:	09cfc4        	ae_s16ip	a12, a15, 2
     d1e:	909520        	addx2	a9, a5, a2
     d21:	c0e650        	sub	a14, a6, a5
     d24:	90b540        	addx2	a11, a5, a4
     d27:	06dbb2        	addmi	a11, a11, 0x600
     d2a:	e4cbb2        	addi	a11, a11, -28
     d2d:	12ae76        	loopgtz	a14, d43 <SMART_PITCH_PLC_PROCESS+0xa93>
     d30:	fd0bd2        	l8ui	a13, a11, 253
     d33:	fc0be2        	l8ui	a14, a11, 252
     d36:	bb2b           	addi.n	a11, a11, 2
     d38:	11dd80        	slli	a13, a13, 8
     d3b:	20dde0        	or	a13, a13, a14
     d3e:	8ddd           	ae_sext16	a13, a13
     d40:	09c9d4        	ae_s16ip	a13, a9, 2
     d43:	fef4c6        	j	91a <SMART_PITCH_PLC_PROCESS+0x66a>

00000d46 <SMART_PITCH_PLC_PROCESS+0xa96>:
     d46:	1164a2        	s32i	a10, a4, 68
     d49:	11baf0        	slli	a11, a10, 1
     d4c:	fd8e86        	j	38a <SMART_PITCH_PLC_PROCESS+0xda>

00000d4f <SMART_PITCH_PLC_PROCESS+0xa9f>:
	...

00000d50 <sqrt_q26>:
     d50:	004136        	entry	a1, 32
     d53:	20b220        	or	a11, a2, a2
     d56:	05a0a2        	movi	a10, 5
     d59:	005925        	call8	12ec <HIFI_sqrt>
     d5c:	0d6ae6        	bgei	a10, 6, d6d <sqrt_q26+0x1d>
     d5f:	05a022        	movi	a2, 5
     d62:	c022a0        	sub	a2, a2, a10
     d65:	400200        	ssr	a2
     d68:	b120b0        	sra	a2, a11
     d6b:	f01d           	retw.n

00000d6d <sqrt_q26+0x1d>:
     d6d:	fcb121        	l32r	a2, 34 (7fffffff <_end+0x7fffe6c7>)
     d70:	f01d           	retw.n

00000d72 <sqrt_q26+0x22>:
     d72:	00000000 e1360000                                ......

00000d78 <find_pitch>:
     d78:	00e136        	entry	a1, 112
     d7b:	c129           	s32i.n	a2, a1, 48
     d7d:	03bd           	mov.n	a11, a3
     d7f:	8169           	s32i.n	a6, a1, 32
     d81:	4159           	s32i.n	a5, a1, 16
     d83:	2179           	s32i.n	a7, a1, 8
     d85:	045d           	mov.n	a5, a4
     d87:	5414a6        	blti	a4, 1, ddf <find_pitch+0x67>
     d8a:	2179           	s32i.n	a7, a1, 8
     d8c:	d14b           	addi.n	a13, a1, 4
     d8e:	c09740        	sub	a9, a7, a4
     d91:	c0c730        	sub	a12, a7, a3
     d94:	e41b           	addi.n	a14, a4, 1
     d96:	030c           	movi.n	a3, 0
     d98:	21e1e0        	srai	a14, a14, 1
     d9b:	909920        	addx2	a9, a9, a2
     d9e:	90cc20        	addx2	a12, a12, a2
     da1:	ffdcc2        	addmi	a12, a12, 0xffffff00
     da4:	ffd992        	addmi	a9, a9, 0xffffff00
     da7:	020c           	movi.n	a2, 0
     da9:	2dae76        	loopgtz	a14, dda <find_pitch+0x62>
     dac:	7f99e2        	l16si	a14, a9, 254
     daf:	7f9ca2        	l16si	a10, a12, 254
     db2:	994b           	addi.n	a9, a9, 4
     db4:	11e9           	s32i.n	a14, a1, 4
     db6:	01a9           	s32i.n	a10, a1, 0
     db8:	402104        	ae_lp16f.i	aep2, a1, 0
     dbb:	403104        	ae_lp16f.i	aep3, a1, 0
     dbe:	ea6bec1d0f 	{ ae_lp16f.i	aep1, a13, 0; ae_mulp24s.ll	aeq1, aep2, aep3 }
     dc3:	400104        	ae_lp16f.i	aep0, a1, 0
     dc6:	ea23bf18ef 	{ ae_trunca32q48	a8, aeq1; ae_mulp24s.ll	aeq0, aep0, aep1 }
     dcb:	cc4b           	addi.n	a12, a12, 4
     dcd:	368f04        	ae_trunca32q48	a15, aeq0
     dd0:	218480        	srai	a8, a8, 4
     dd3:	228a           	add.n	a2, a2, a8
     dd5:	21f4f0        	srai	a15, a15, 4
     dd8:	33fa           	add.n	a3, a3, a15

00000dda <find_pitch+0x62>:
     dda:	31b9           	s32i.n	a11, a1, 12
     ddc:	000146        	j	de5 <find_pitch+0x6d>

00000ddf <find_pitch+0x67>:
     ddf:	030c           	movi.n	a3, 0
     de1:	020c           	movi.n	a2, 0
     de3:	31b9           	s32i.n	a11, a1, 12

00000de5 <find_pitch+0x6d>:
     de5:	90a550        	addx2	a10, a5, a5
     de8:	b0aa50        	addx8	a10, a10, a5
     deb:	21a4a0        	srai	a10, a10, 4
     dee:	0d61a2        	s32i	a10, a1, 52
     df1:	53aa20        	max	a10, a10, a2
     df4:	fff5a5        	call8	d50 <sqrt_q26>
     df7:	21bf30        	srai	a11, a3, 15
     dfa:	20caa0        	or	a12, a10, a10
     dfd:	31dfa0        	srai	a13, a10, 31
     e00:	01e3f0        	slli	a14, a3, 17
     e03:	20aee0        	or	a10, a14, a14
     e06:	0058a5        	call8	1390 <__divdi3>
     e09:	0821f2        	l32i	a15, a1, 32
     e0c:	0e61a2        	s32i	a10, a1, 56
     e0f:	022fe6        	bgei	a15, 2, e15 <find_pitch+0x9d>
     e12:	003806        	j	ef6 <find_pitch+0x17e>
     e15:	02a0b2        	movi	a11, 2
     e18:	3178           	l32i.n	a7, a1, 12
     e1a:	2168           	l32i.n	a6, a1, 8
     e1c:	040c           	movi.n	a4, 0
     e1e:	080c           	movi.n	a8, 0
     e20:	5189           	s32i.n	a8, a1, 20
     e22:	442b           	addi.n	a4, a4, 2
     e24:	c188           	l32i.n	a8, a1, 48
     e26:	c09650        	sub	a9, a6, a5
     e29:	c06670        	sub	a6, a6, a7
     e2c:	a169           	s32i.n	a6, a1, 40
     e2e:	751b           	addi.n	a7, a5, 1
     e30:	909980        	addx2	a9, a9, a8
     e33:	ffd992        	addmi	a9, a9, 0xffffff00
     e36:	217170        	srai	a7, a7, 1
     e39:	7179           	s32i.n	a7, a1, 28
     e3b:	b199           	s32i.n	a9, a1, 44
     e3d:	765a           	add.n	a7, a6, a5
     e3f:	907780        	addx2	a7, a7, a8
     e42:	906680        	addx2	a6, a6, a8
     e45:	ffd662        	addmi	a6, a6, 0xffffff00
     e48:	ffd772        	addmi	a7, a7, 0xffffff00

00000e4b <find_pitch+0xd3>:
     e4b:	d14b           	addi.n	a13, a1, 4
     e4d:	7f9792        	l16si	a9, a7, 254
     e50:	7f96a2        	l16si	a10, a6, 254
     e53:	11a9           	s32i.n	a10, a1, 4
     e55:	0199           	s32i.n	a9, a1, 0
     e57:	404104        	ae_lp16f.i	aep4, a1, 0
     e5a:	406d04        	ae_lp16f.i	aep6, a13, 0
     e5d:	407d04        	ae_lp16f.i	aep7, a13, 0
     e60:	ebfbec510f 	{ ae_lp16f.i	aep5, a1, 0; ae_mulp24s.ll	aeq3, aep6, aep7 }
     e65:	ebb380477e 	{ addi	a7, a7, 4; ae_mulp24s.ll	aeq2, aep4, aep5 }
     e6a:	36b804        	ae_trunca32q48	a8, aeq3
     e6d:	36a904        	ae_trunca32q48	a9, aeq2
     e70:	664b           	addi.n	a6, a6, 4
     e72:	218480        	srai	a8, a8, 4
     e75:	219490        	srai	a9, a9, 4
     e78:	229a           	add.n	a2, a2, a9
     e7a:	c02280        	sub	a2, a2, a8
     e7d:	3915a6        	blti	a5, 1, eba <find_pitch+0x142>
     e80:	030c           	movi.n	a3, 0
     e82:	a1c8           	l32i.n	a12, a1, 40
     e84:	c1e8           	l32i.n	a14, a1, 48
     e86:	b198           	l32i.n	a9, a1, 44
     e88:	cc4a           	add.n	a12, a12, a4
     e8a:	90cce0        	addx2	a12, a12, a14
     e8d:	71e8           	l32i.n	a14, a1, 28
     e8f:	0f61b2        	s32i	a11, a1, 60
     e92:	ffdcc2        	addmi	a12, a12, 0xffffff00
     e95:	1eae76        	loopgtz	a14, eb7 <find_pitch+0x13f>
     e98:	7f9cf2        	l16si	a15, a12, 254
     e9b:	7f9982        	l16si	a8, a9, 254
     e9e:	0189           	s32i.n	a8, a1, 0
     ea0:	11f9           	s32i.n	a15, a1, 4
     ea2:	401d04        	ae_lp16f.i	aep1, a13, 0
     ea5:	400104        	ae_lp16f.i	aep0, a1, 0
     ea8:	020174        	ae_mulp24s.ll	aeq0, aep0, aep1
     eab:	368e04        	ae_trunca32q48	a14, aeq0
     eae:	994b           	addi.n	a9, a9, 4
     eb0:	cc4b           	addi.n	a12, a12, 4
     eb2:	21e4e0        	srai	a14, a14, 4
     eb5:	33ea           	add.n	a3, a3, a14

00000eb7 <find_pitch+0x13f>:
     eb7:	0000c6        	j	ebe <find_pitch+0x146>

00000eba <find_pitch+0x142>:
     eba:	030c           	movi.n	a3, 0
     ebc:	f1b9           	s32i.n	a11, a1, 60

00000ebe <find_pitch+0x146>:
     ebe:	0d21a2        	l32i	a10, a1, 52
     ec1:	53aa20        	max	a10, a10, a2
     ec4:	ffe8a5        	call8	d50 <sqrt_q26>
     ec7:	21bf30        	srai	a11, a3, 15
     eca:	20caa0        	or	a12, a10, a10
     ecd:	31dfa0        	srai	a13, a10, 31
     ed0:	01e3f0        	slli	a14, a3, 17
     ed3:	20aee0        	or	a10, a14, a14
     ed6:	004ba5        	call8	1390 <__divdi3>
     ed9:	0e21f2        	l32i	a15, a1, 56
     edc:	0f21b2        	l32i	a11, a1, 60
     edf:	02c442        	addi	a4, a4, 2
     ee2:	032af7        	blt	a10, a15, ee9 <find_pitch+0x171>
     ee5:	51b9           	s32i.n	a11, a1, 20
     ee7:	e1a9           	s32i.n	a10, a1, 56

00000ee9 <find_pitch+0x171>:
     ee9:	81a8           	l32i.n	a10, a1, 32
     eeb:	bb2b           	addi.n	a11, a11, 2
     eed:	022ab7        	blt	a10, a11, ef3 <find_pitch+0x17b>
     ef0:	ffd5c6        	j	e4b <find_pitch+0xd3>
     ef3:	0000c6        	j	efa <find_pitch+0x182>

00000ef6 <find_pitch+0x17e>:
     ef6:	0b0c           	movi.n	a11, 0
     ef8:	51b9           	s32i.n	a11, a1, 20

00000efa <find_pitch+0x182>:
     efa:	5148           	l32i.n	a4, a1, 20
     efc:	030c           	movi.n	a3, 0
     efe:	440b           	addi.n	a4, a4, -1
     f00:	534340        	max	a4, a3, a4
     f03:	5215a6        	blti	a5, 1, f59 <find_pitch+0x1e1>
     f06:	020c           	movi.n	a2, 0
     f08:	31c8           	l32i.n	a12, a1, 12
     f0a:	2198           	l32i.n	a9, a1, 8
     f0c:	b14b           	addi.n	a11, a1, 4
     f0e:	c1d8           	l32i.n	a13, a1, 48
     f10:	c0c9c0        	sub	a12, a9, a12
     f13:	c09950        	sub	a9, a9, a5
     f16:	80c4c0        	add	a12, a4, a12
     f19:	90ccd0        	addx2	a12, a12, a13
     f1c:	9099d0        	addx2	a9, a9, a13
     f1f:	ffd992        	addmi	a9, a9, 0xffffff00
     f22:	ffdcc2        	addmi	a12, a12, 0xffffff00
     f25:	2da576        	loopgtz	a5, f56 <find_pitch+0x1de>
     f28:	7f9982        	l16si	a8, a9, 254
     f2b:	7f9cf2        	l16si	a15, a12, 254
     f2e:	992b           	addi.n	a9, a9, 2
     f30:	1189           	s32i.n	a8, a1, 4
     f32:	01f9           	s32i.n	a15, a1, 0
     f34:	404104        	ae_lp16f.i	aep4, a1, 0
     f37:	405104        	ae_lp16f.i	aep5, a1, 0
     f3a:	ebb3ac3b0f 	{ ae_lp16f.i	aep3, a11, 0; ae_mulp24s.ll	aeq2, aep4, aep5 }
     f3f:	402104        	ae_lp16f.i	aep2, a1, 0
     f42:	ea6bff2eef 	{ ae_trunca32q48	a14, aeq2; ae_mulp24s.ll	aeq1, aep2, aep3 }
     f47:	cc2b           	addi.n	a12, a12, 2
     f49:	369d04        	ae_trunca32q48	a13, aeq1
     f4c:	21e4e0        	srai	a14, a14, 4
     f4f:	22ea           	add.n	a2, a2, a14
     f51:	21d4d0        	srai	a13, a13, 4
     f54:	33da           	add.n	a3, a3, a13

00000f56 <find_pitch+0x1de>:
     f56:	000086        	j	f5c <find_pitch+0x1e4>

00000f59 <find_pitch+0x1e1>:
     f59:	00a022        	movi	a2, 0

00000f5c <find_pitch+0x1e4>:
     f5c:	0d21a2        	l32i	a10, a1, 52
     f5f:	53aa20        	max	a10, a10, a2
     f62:	ffdee5        	call8	d50 <sqrt_q26>
     f65:	21bf30        	srai	a11, a3, 15
     f68:	20caa0        	or	a12, a10, a10
     f6b:	31dfa0        	srai	a13, a10, 31
     f6e:	01e3f0        	slli	a14, a3, 17
     f71:	20aee0        	or	a10, a14, a14
     f74:	0041a5        	call8	1390 <__divdi3>
     f77:	0e61a2        	s32i	a10, a1, 56
     f7a:	066142        	s32i	a4, a1, 24
     f7d:	51d8           	l32i.n	a13, a1, 20
     f7f:	81c8           	l32i.n	a12, a1, 32
     f81:	b41b           	addi.n	a11, a4, 1
     f83:	dd1b           	addi.n	a13, a13, 1
     f85:	43ccd0        	min	a12, a12, a13
     f88:	0224c7        	blt	a4, a12, f8e <find_pitch+0x216>
     f8b:	003546        	j	1064 <find_pitch+0x2ec>
     f8e:	ac1b           	addi.n	a10, a12, 1
     f90:	3198           	l32i.n	a9, a1, 12
     f92:	91a9           	s32i.n	a10, a1, 36
     f94:	21a8           	l32i.n	a10, a1, 8
     f96:	c188           	l32i.n	a8, a1, 48
     f98:	c09a90        	sub	a9, a10, a9
     f9b:	a199           	s32i.n	a9, a1, 40
     f9d:	649a           	add.n	a6, a4, a9
     f9f:	795a           	add.n	a7, a9, a5
     fa1:	c0aa50        	sub	a10, a10, a5
     fa4:	90aa80        	addx2	a10, a10, a8
     fa7:	747a           	add.n	a7, a4, a7
     fa9:	906680        	addx2	a6, a6, a8
     fac:	ffd662        	addmi	a6, a6, 0xffffff00
     faf:	441b           	addi.n	a4, a4, 1
     fb1:	907780        	addx2	a7, a7, a8
     fb4:	ffdaa2        	addmi	a10, a10, 0xffffff00
     fb7:	b1a9           	s32i.n	a10, a1, 44
     fb9:	ffd772        	addmi	a7, a7, 0xffffff00

00000fbc <find_pitch+0x244>:
     fbc:	7f97c2        	l16si	a12, a7, 254
     fbf:	7f96a2        	l16si	a10, a6, 254
     fc2:	772b           	addi.n	a7, a7, 2
     fc4:	01c9           	s32i.n	a12, a1, 0
     fc6:	400104        	ae_lp16f.i	aep0, a1, 0
     fc9:	401104        	ae_lp16f.i	aep1, a1, 0
     fcc:	01a9           	s32i.n	a10, a1, 0
     fce:	ea23ac610f 	{ ae_lp16f.i	aep6, a1, 0; ae_mulp24s.ll	aeq0, aep0, aep1 }
     fd3:	407104        	ae_lp16f.i	aep7, a1, 0
     fd6:	ebfbff09ef 	{ ae_trunca32q48	a9, aeq0; ae_mulp24s.ll	aeq3, aep6, aep7 }
     fdb:	662b           	addi.n	a6, a6, 2
     fdd:	36b804        	ae_trunca32q48	a8, aeq3
     fe0:	219490        	srai	a9, a9, 4
     fe3:	229a           	add.n	a2, a2, a9
     fe5:	218480        	srai	a8, a8, 4
     fe8:	c02280        	sub	a2, a2, a8
     feb:	3a15a6        	blti	a5, 1, 1029 <find_pitch+0x2b1>
     fee:	030c           	movi.n	a3, 0
     ff0:	a1c8           	l32i.n	a12, a1, 40
     ff2:	c1d8           	l32i.n	a13, a1, 48
     ff4:	b198           	l32i.n	a9, a1, 44
     ff6:	cc4a           	add.n	a12, a12, a4
     ff8:	90ccd0        	addx2	a12, a12, a13
     ffb:	ffdcc2        	addmi	a12, a12, 0xffffff00
     ffe:	04c1d2        	addi	a13, a1, 4
    1001:	1ea576        	loopgtz	a5, 1023 <find_pitch+0x2ab>
    1004:	7f9cf2        	l16si	a15, a12, 254
    1007:	7f9982        	l16si	a8, a9, 254
    100a:	0189           	s32i.n	a8, a1, 0
    100c:	11f9           	s32i.n	a15, a1, 4
    100e:	403d04        	ae_lp16f.i	aep3, a13, 0
    1011:	402104        	ae_lp16f.i	aep2, a1, 0
    1014:	022b74        	ae_mulp24s.ll	aeq1, aep2, aep3
    1017:	369e04        	ae_trunca32q48	a14, aeq1
    101a:	992b           	addi.n	a9, a9, 2
    101c:	cc2b           	addi.n	a12, a12, 2
    101e:	21e4e0        	srai	a14, a14, 4
    1021:	33ea           	add.n	a3, a3, a14

00001023 <find_pitch+0x2ab>:
    1023:	1061b2        	s32i	a11, a1, 64
    1026:	000106        	j	102e <find_pitch+0x2b6>

00001029 <find_pitch+0x2b1>:
    1029:	030c           	movi.n	a3, 0
    102b:	1061b2        	s32i	a11, a1, 64

0000102e <find_pitch+0x2b6>:
    102e:	0d21a2        	l32i	a10, a1, 52
    1031:	53aa20        	max	a10, a10, a2
    1034:	ffd1a5        	call8	d50 <sqrt_q26>
    1037:	21bf30        	srai	a11, a3, 15
    103a:	20caa0        	or	a12, a10, a10
    103d:	31dfa0        	srai	a13, a10, 31
    1040:	01e3f0        	slli	a14, a3, 17
    1043:	20aee0        	or	a10, a14, a14
    1046:	0034a5        	call8	1390 <__divdi3>
    1049:	0e21f2        	l32i	a15, a1, 56
    104c:	1021b2        	l32i	a11, a1, 64
    104f:	01c442        	addi	a4, a4, 1
    1052:	042af7        	blt	a10, a15, 105a <find_pitch+0x2e2>
    1055:	0661b2        	s32i	a11, a1, 24
    1058:	e1a9           	s32i.n	a10, a1, 56

0000105a <find_pitch+0x2e2>:
    105a:	91a8           	l32i.n	a10, a1, 36
    105c:	bb1b           	addi.n	a11, a11, 1
    105e:	c0aab0        	sub	a10, a10, a11
    1061:	f57a56        	bnez	a10, fbc <find_pitch+0x244>

00001064 <find_pitch+0x2ec>:
    1064:	6188           	l32i.n	a8, a1, 24
    1066:	4128           	l32i.n	a2, a1, 16
    1068:	c02280        	sub	a2, a2, a8
    106b:	f01d           	retw.n

0000106d <find_pitch+0x2f5>:
    106d:	000000                                        ...

00001070 <Q_ubitlength>:
    1070:	004136        	entry	a1, 32
    1073:	fbf131        	l32r	a3, 38 (10000 <_end+0xe6c8>)
    1076:	f22230        	rems	a2, a2, a3
    1079:	f01d           	retw.n

0000107b <Q_ubitlength+0xb>:
	...

0000107c <plc_gainctrl_ini>:
    107c:	004136        	entry	a1, 32
    107f:	3222c2        	l32i	a12, a2, 200
    1082:	342282        	l32i	a8, a2, 208
    1085:	352232        	l32i	a3, a2, 212
    1088:	3822d2        	l32i	a13, a2, 224
    108b:	3722e2        	l32i	a14, a2, 220
    108e:	3622f2        	l32i	a15, a2, 216
    1091:	332292        	l32i	a9, a2, 204
    1094:	10a0b2        	movi	a11, 16
    1097:	2362b2        	s32i	a11, a2, 140
    109a:	256292        	s32i	a9, a2, 148
    109d:	2862f2        	s32i	a15, a2, 160
    10a0:	2962e2        	s32i	a14, a2, 164
    10a3:	2a62d2        	s32i	a13, a2, 168
    10a6:	276232        	s32i	a3, a2, 156
    10a9:	266282        	s32i	a8, a2, 152
    10ac:	01a032        	movi	a3, 1
    10af:	401c00        	ssl	a12
    10b2:	a1a300        	sll	a10, a3
    10b5:	aa0b           	addi.n	a10, a10, -1
    10b7:	2462a2        	s32i	a10, a2, 144
    10ba:	fffb65        	call8	1070 <Q_ubitlength>
    10bd:	2522b2        	l32i	a11, a2, 148
    10c0:	2462a2        	s32i	a10, a2, 144
    10c3:	401b00        	ssl	a11
    10c6:	2322b2        	l32i	a11, a2, 140
    10c9:	a1a300        	sll	a10, a3
    10cc:	aa0b           	addi.n	a10, a10, -1
    10ce:	2562a2        	s32i	a10, a2, 148
    10d1:	fff9e5        	call8	1070 <Q_ubitlength>
    10d4:	2562a2        	s32i	a10, a2, 148
    10d7:	f01d           	retw.n

000010d9 <plc_gainctrl_ini+0x5d>:
    10d9:	00000000 36000000                                .......

000010e0 <plc_gainctrl>:
    10e0:	004136        	entry	a1, 32
    10e3:	0d25a2        	l32i	a10, a5, 52
    10e6:	2325b2        	l32i	a11, a5, 140
    10e9:	d0aa20        	subx2	a10, a10, a2
    10ec:	01caa2        	addi	a10, a10, 1
    10ef:	0d65a2        	s32i	a10, a5, 52
    10f2:	fff7e5        	call8	1070 <Q_ubitlength>
    10f5:	2525b2        	l32i	a11, a5, 148
    10f8:	d5a9           	s32i.n	a10, a5, 52
    10fa:	060c           	movi.n	a6, 0
    10fc:	420ab7        	bnone	a10, a11, 1142 <plc_gainctrl+0x62>
    10ff:	2325b2        	l32i	a11, a5, 140
    1102:	070c           	movi.n	a7, 0
    1104:	060c           	movi.n	a6, 0
    1106:	021be6        	bgei	a11, 1, 110c <plc_gainctrl+0x2c>
    1109:	002506        	j	11a1 <plc_gainctrl+0xc1>
    110c:	020c           	movi.n	a2, 0

0000110e <plc_gainctrl+0x2e>:
    110e:	2425c2        	l32i	a12, a5, 144
    1111:	6ccc           	bnez.n	a12, 111b <plc_gainctrl+0x3b>
    1113:	176a07        	bbci	a10, 0, 112e <plc_gainctrl+0x4e>
    1116:	661b           	addi.n	a6, a6, 1
    1118:	000486        	j	112e <plc_gainctrl+0x4e>

0000111b <plc_gainctrl+0x3b>:
    111b:	10dca0        	and	a13, a12, a10
    111e:	074ac7        	ball	a10, a12, 1129 <plc_gainctrl+0x49>
    1121:	096d07        	bbci	a13, 0, 112e <plc_gainctrl+0x4e>
    1124:	661b           	addi.n	a6, a6, 1
    1126:	000106        	j	112e <plc_gainctrl+0x4e>

00001129 <plc_gainctrl+0x49>:
    1129:	fdc222        	addi	a2, a2, -3
    112c:	061c           	movi.n	a6, 16
    112e:	21a1a0        	srai	a10, a10, 1
    1131:	fff3e5        	call8	1070 <Q_ubitlength>
    1134:	2325b2        	l32i	a11, a5, 140
    1137:	771b           	addi.n	a7, a7, 1
    1139:	d127b7        	blt	a7, a11, 110e <plc_gainctrl+0x2e>
    113c:	000106        	j	1144 <plc_gainctrl+0x64>

0000113f <plc_gainctrl+0x5f>:
    113f:	000000                                        ...

00001142 <plc_gainctrl+0x62>:
    1142:	d27c           	movi.n	a2, -3

00001144 <plc_gainctrl+0x64>:
    1144:	2625c2        	l32i	a12, a5, 152
    1147:	2a25e2        	l32i	a14, a5, 168
    114a:	2725d2        	l32i	a13, a5, 156
    114d:	72c6c0        	salt	a12, a6, a12
    1150:	11cc10        	slli	a12, a12, 15
    1153:	0ead67        	bge	a13, a6, 1165 <plc_gainctrl+0x85>
    1156:	2825a2        	l32i	a10, a5, 160
    1159:	0226a7        	blt	a6, a10, 115f <plc_gainctrl+0x7f>
    115c:	c066a0        	sub	a6, a6, a10

0000115f <plc_gainctrl+0x7f>:
    115f:	400600        	ssr	a6
    1162:	b1c0c0        	sra	a12, a12

00001165 <plc_gainctrl+0x85>:
    1165:	0226e6        	bgei	a6, 2, 116b <plc_gainctrl+0x8b>
    1168:	fdc222        	addi	a2, a2, -3

0000116b <plc_gainctrl+0x8b>:
    116b:	3014a6        	blti	a4, 1, 119f <plc_gainctrl+0xbf>
    116e:	1d0c           	movi.n	a13, 1
    1170:	0f0c           	movi.n	a15, 0
    1172:	29a476        	loopgtz	a4, 119f <plc_gainctrl+0xbf>
    1175:	102582        	l32i	a8, a5, 64
    1178:	234d           	ae_l16si.n	a4, a3, 0
    117a:	2925a2        	l32i	a10, a5, 164
    117d:	c09c80        	sub	a9, a12, a8
    1180:	72b9d0        	salt	a11, a9, a13
    1183:	93aeb0        	movnez	a10, a14, a11
    1186:	aa2a           	add.n	a10, a10, a2
    1188:	53aaf0        	max	a10, a10, a15
    118b:	400a00        	ssr	a10
    118e:	b19090        	sra	a9, a9
    1191:	889a           	add.n	a8, a8, a9
    1193:	824480        	mull	a4, a4, a8
    1196:	106582        	s32i	a8, a5, 64
    1199:	214f40        	srai	a4, a4, 15
    119c:	09c344        	ae_s16ip	a4, a3, 2

0000119f <plc_gainctrl+0xbf>:
    119f:	f01d           	retw.n

000011a1 <plc_gainctrl+0xc1>:
    11a1:	020c           	movi.n	a2, 0
    11a3:	060c           	movi.n	a6, 0
    11a5:	ffe6c6        	j	1144 <plc_gainctrl+0x64>

000011a8 <plcpitch_sim_init>:
    11a8:	004136        	entry	a1, 32
    11ab:	444c           	movi.n	a4, 68
    11ad:	651c           	movi.n	a5, 22
    11af:	cca262        	movi	a6, 0x2cc
    11b2:	072c           	movi.n	a7, 32
    11b4:	1ea592        	movi	a9, 0x51e
    11b7:	8f0c           	movi.n	a15, 8
    11b9:	1a0c           	movi.n	a10, 1
    11bb:	ee0c           	movi.n	a14, 14
    11bd:	080c           	movi.n	a8, 0
    11bf:	fd0c           	movi.n	a13, 15
    11c1:	fb9eb1        	l32r	a11, 3c (4000 <_end+0x26c8>)
    11c4:	0ed232        	addmi	a3, a2, 0xe00
    11c7:	ac0c           	movi.n	a12, 10
    11c9:	dc53c2        	s16i	a12, a3, 0x1b8
    11cc:	db53c2        	s16i	a12, a3, 0x1b6
    11cf:	d853c2        	s16i	a12, a3, 0x1b0
    11d2:	e05382        	s16i	a8, a3, 0x1c0
    11d5:	da5382        	s16i	a8, a3, 0x1b4
    11d8:	e653a2        	s16i	a10, a3, 0x1cc
    11db:	e553a2        	s16i	a10, a3, 0x1ca
    11de:	e453a2        	s16i	a10, a3, 0x1c8
    11e1:	e353a2        	s16i	a10, a3, 0x1c6
    11e4:	e253a2        	s16i	a10, a3, 0x1c4
    11e7:	e153a2        	s16i	a10, a3, 0x1c2
    11ea:	df53a2        	s16i	a10, a3, 0x1be
    11ed:	dd53a2        	s16i	a10, a3, 0x1ba
    11f0:	d653f2        	s16i	a15, a3, 0x1ac
    11f3:	d753e2        	s16i	a14, a3, 0x1ae
    11f6:	d953d2        	s16i	a13, a3, 0x1b2
    11f9:	de53b2        	s16i	a11, a3, 0x1bc
    11fc:	3262f2        	s32i	a15, a2, 200
    11ff:	3362e2        	s32i	a14, a2, 204
    1202:	3562d2        	s32i	a13, a2, 212
    1205:	3462c2        	s32i	a12, a2, 208
    1208:	3762c2        	s32i	a12, a2, 220
    120b:	3862c2        	s32i	a12, a2, 224
    120e:	3a62b2        	s32i	a11, a2, 232
    1211:	3962a2        	s32i	a10, a2, 228
    1214:	3b62a2        	s32i	a10, a2, 236
    1217:	3d62a2        	s32i	a10, a2, 244
    121a:	3e62a2        	s32i	a10, a2, 248
    121d:	3f62a2        	s32i	a10, a2, 252
    1220:	4062a2        	s32i	a10, a2, 0x100
    1223:	4162a2        	s32i	a10, a2, 0x104
    1226:	4262a2        	s32i	a10, a2, 0x108
    1229:	4762a2        	s32i	a10, a2, 0x11c
    122c:	496292        	s32i	a9, a2, 0x124
    122f:	4a6292        	s32i	a9, a2, 0x128
    1232:	366282        	s32i	a8, a2, 216
    1235:	3c6282        	s32i	a8, a2, 240
    1238:	4b6282        	s32i	a8, a2, 0x12c
    123b:	436272        	s32i	a7, a2, 0x10c
    123e:	446262        	s32i	a6, a2, 0x110
    1241:	456252        	s32i	a5, a2, 0x114
    1244:	466242        	s32i	a4, a2, 0x118
    1247:	fb7e31        	l32r	a3, 40 (1999 <_end+0x61>)
    124a:	486232        	s32i	a3, a2, 0x120
    124d:	f01d           	retw.n

0000124f <plcpitch_sim_init+0xa7>:
	...

00001250 <plcpitch_para_init>:
    1250:	004136        	entry	a1, 32
    1253:	1ea5b2        	movi	a11, 0x51e
    1256:	0fd2d2        	addmi	a13, a2, 0xf00
    1259:	589d82        	l16si	a8, a13, 176
    125c:	5a9d32        	l16si	a3, a13, 180
    125f:	579d92        	l16si	a9, a13, 174
    1262:	569da2        	l16si	a10, a13, 172
    1265:	5d9dc2        	l16si	a12, a13, 186
    1268:	5c9de2        	l16si	a14, a13, 184
    126b:	5b9df2        	l16si	a15, a13, 182
    126e:	599d42        	l16si	a4, a13, 178
    1271:	356242        	s32i	a4, a2, 212
    1274:	4962b2        	s32i	a11, a2, 0x124
    1277:	4a62b2        	s32i	a11, a2, 0x128
    127a:	3962c2        	s32i	a12, a2, 228
    127d:	3862e2        	s32i	a14, a2, 224
    1280:	3762f2        	s32i	a15, a2, 220
    1283:	346282        	s32i	a8, a2, 208
    1286:	366232        	s32i	a3, a2, 216
    1289:	336292        	s32i	a9, a2, 204
    128c:	3262a2        	s32i	a10, a2, 200
    128f:	629d42        	l16si	a4, a13, 196
    1292:	3e6242        	s32i	a4, a2, 248
    1295:	5f9da2        	l16si	a10, a13, 190
    1298:	609d92        	l16si	a9, a13, 192
    129b:	639d32        	l16si	a3, a13, 198
    129e:	619d82        	l16si	a8, a13, 194
    12a1:	649df2        	l16si	a15, a13, 200
    12a4:	659de2        	l16si	a14, a13, 202
    12a7:	0c2c           	movi.n	a12, 32
    12a9:	4362c2        	s32i	a12, a2, 0x10c
    12ac:	4162e2        	s32i	a14, a2, 0x104
    12af:	4062f2        	s32i	a15, a2, 0x100
    12b2:	3d6282        	s32i	a8, a2, 244
    12b5:	3f6232        	s32i	a3, a2, 252
    12b8:	3c6292        	s32i	a9, a2, 240
    12bb:	3b62a2        	s32i	a10, a2, 236
    12be:	5e9db2        	l16si	a11, a13, 188
    12c1:	fb5f41        	l32r	a4, 40 (1999 <_end+0x61>)
    12c4:	486242        	s32i	a4, a2, 0x120
    12c7:	3a62b2        	s32i	a11, a2, 232
    12ca:	6a1c           	movi.n	a10, 22
    12cc:	494c           	movi.n	a9, 68
    12ce:	030c           	movi.n	a3, 0
    12d0:	180c           	movi.n	a8, 1
    12d2:	476282        	s32i	a8, a2, 0x11c
    12d5:	4b6232        	s32i	a3, a2, 0x12c
    12d8:	466292        	s32i	a9, a2, 0x118
    12db:	4562a2        	s32i	a10, a2, 0x114
    12de:	cca2b2        	movi	a11, 0x2cc
    12e1:	4462b2        	s32i	a11, a2, 0x110
    12e4:	669dd2        	l16si	a13, a13, 204
    12e7:	4262d2        	s32i	a13, a2, 0x108
    12ea:	f01d           	retw.n

000012ec <HIFI_sqrt>:
    12ec:	006136        	entry	a1, 48
    12ef:	035d           	mov.n	a5, a3
    12f1:	030c           	movi.n	a3, 0
    12f3:	1139           	s32i.n	a3, a1, 4
    12f5:	0139           	s32i.n	a3, a1, 0
    12f7:	0715e6        	bgei	a5, 1, 1302 <HIFI_sqrt+0x16>
    12fa:	0139           	s32i.n	a3, a1, 0

000012fc <HIFI_sqrt+0x10>:
    12fc:	1139           	s32i.n	a3, a1, 4
    12fe:	0128           	l32i.n	a2, a1, 0
    1300:	f01d           	retw.n

00001302 <HIFI_sqrt+0x16>:
    1302:	fb5081        	l32r	a8, 44 (18d0 <TABLE_SQRT>)
    1305:	40e5a0        	nsa	a10, a5
    1308:	c042a0        	sub	a4, a2, a10
    130b:	401a00        	ssl	a10
    130e:	a19500        	sll	a9, a5
    1311:	046040        	extui	a6, a4, 0, 1
    1314:	400600        	ssr	a6
    1317:	b19090        	sra	a9, a9
    131a:	419990        	srli	a9, a9, 9
    131d:	317090        	srai	a7, a9, 16
    1320:	300994        	ae_cvtp24a16x2.ll	aep0, a9, a9
    1323:	f0c772        	addi	a7, a7, -16
    1326:	537730        	max	a7, a7, a3
    1329:	907780        	addx2	a7, a7, a8
    132c:	278d           	ae_l16si.n	a8, a7, 0
    132e:	377d           	ae_l16si.n	a7, a7, 2
    1330:	346a           	add.n	a3, a4, a6
    1332:	213130        	srai	a3, a3, 1
    1335:	c07780        	sub	a7, a7, a8
    1338:	118800        	slli	a8, a8, 16
    133b:	117700        	slli	a7, a7, 16
    133e:	364734        	ae_cvtq48a32s	aeq1, a7
    1341:	360834        	ae_cvtq48a32s	aeq0, a8
    1344:	030174        	ae_mulaq32sp16u.l	aeq0, aeq1, aep0
    1347:	0139           	s32i.n	a3, a1, 0
    1349:	368304        	ae_trunca32q48	a3, aeq0
    134c:	ffeb06        	j	12fc <HIFI_sqrt+0x10>

0000134f <HIFI_sqrt+0x63>:
	...

00001350 <find_pitch_ini>:
    1350:	004136        	entry	a1, 32
    1353:	f01d           	retw.n

00001355 <find_pitch_ini+0x5>:
    1355:	000000                                        ...

00001358 <getfespeech>:
    1358:	004136        	entry	a1, 32
    135b:	1227a2        	l32i	a10, a7, 72
    135e:	052a37        	blt	a10, a3, 1367 <getfespeech+0xf>
    1361:	c0aa30        	sub	a10, a10, a3
    1364:	1267a2        	s32i	a10, a7, 72
    1367:	1ca576        	loopgtz	a5, 1387 <getfespeech+0x2f>
    136a:	ba4a           	add.n	a11, a10, a4
    136c:	1227a2        	l32i	a10, a7, 72
    136f:	90bb20        	addx2	a11, a11, a2
    1372:	2bbd           	ae_l16si.n	a11, a11, 0
    1374:	09c6b4        	ae_s16ip	a11, a6, 2
    1377:	aa1b           	addi.n	a10, a10, 1
    1379:	1267a2        	s32i	a10, a7, 72
    137c:	052a37        	blt	a10, a3, 1385 <getfespeech+0x2d>
    137f:	c0aa30        	sub	a10, a10, a3
    1382:	1267a2        	s32i	a10, a7, 72

00001385 <getfespeech+0x2d>:
    1385:	f03d           	nop.n

00001387 <getfespeech+0x2f>:
    1387:	f01d           	retw.n

00001389 <getfespeech+0x31>:
    1389:	00000000 36000000                                .......

00001390 <__divdi3>:
    1390:	00c136        	entry	a1, 96
    1393:	05ad           	mov.n	a10, a5
    1395:	039d           	mov.n	a9, a3
    1397:	047d           	mov.n	a7, a4
    1399:	2149           	s32i.n	a4, a1, 8
    139b:	02bd           	mov.n	a11, a2
    139d:	3159           	s32i.n	a5, a1, 12
    139f:	042d           	mov.n	a2, a4
    13a1:	3168           	l32i.n	a6, a1, 12
    13a3:	0b4d           	mov.n	a4, a11
    13a5:	0cf396        	bltz	a3, 1478 <__divdi3+0xe8>
    13a8:	00a082        	movi	a8, 0

000013ab <__divdi3+0x1b>:
    13ab:	0d2c           	movi.n	a13, 32
    13ad:	04cd           	mov.n	a12, a4
    13af:	00f6d6        	bgez	a6, 13c2 <__divdi3+0x32>
    13b2:	602070        	neg	a2, a7
    13b5:	f57c           	movi.n	a5, -1
    13b7:	308850        	xor	a8, a8, a5
    13ba:	305a50        	xor	a5, a10, a5
    13bd:	b51b           	addi.n	a11, a5, 1
    13bf:	835b70        	moveqz	a5, a11, a7

000013c2 <__divdi3+0x32>:
    13c2:	02ad           	mov.n	a10, a2
    13c4:	09bd           	mov.n	a11, a9
    13c6:	0bf516        	beqz	a5, 1489 <__divdi3+0xf9>
    13c9:	16b957        	bgeu	a9, a5, 13e3 <__divdi3+0x53>
    13cc:	020c           	movi.n	a2, 0

000013ce <__divdi3+0x3e>:
    13ce:	030c           	movi.n	a3, 0

000013d0 <__divdi3+0x40>:
    13d0:	1f1816        	beqz	a8, 15c5 <__divdi3+0x235>

000013d3 <__divdi3+0x43>:
    13d3:	f87c           	movi.n	a8, -1
    13d5:	303380        	xor	a3, a3, a8
    13d8:	631b           	addi.n	a6, a3, 1
    13da:	833620        	moveqz	a3, a6, a2
    13dd:	602020        	neg	a2, a2
    13e0:	f01d           	retw.n

000013e2 <__divdi3+0x52>:
	...

000013e3 <__divdi3+0x53>:
    13e3:	40f530        	nsau	a3, a5
    13e6:	1de316        	beqz	a3, 15c8 <__divdi3+0x238>
    13e9:	c07d30        	sub	a7, a13, a3
    13ec:	400700        	ssr	a7
    13ef:	916090        	srl	a6, a9
    13f2:	401300        	ssl	a3
    13f5:	81d940        	src	a13, a9, a4
    13f8:	81b520        	src	a11, a5, a2
    13fb:	9bcd           	ae_zext16	a12, a11
    13fd:	f5e0b0        	extui	a14, a11, 16, 16
    1400:	404010        	ssai	16
    1403:	c2f6e0        	quou	a15, a6, a14
    1406:	0f9d           	mov.n	a9, a15
    1408:	82acf0        	mull	a10, a12, a15
    140b:	e266e0        	remu	a6, a6, a14
    140e:	8166d0        	src	a6, a6, a13
    1411:	065d           	mov.n	a5, a6
    1413:	0eb6a7        	bgeu	a6, a10, 1425 <__divdi3+0x95>
    1416:	9f0b           	addi.n	a9, a15, -1
    1418:	5b6a           	add.n	a5, a11, a6
    141a:	0735b7        	bltu	a5, a11, 1425 <__divdi3+0x95>
    141d:	04b5a7        	bgeu	a5, a10, 1425 <__divdi3+0x95>
    1420:	5b5a           	add.n	a5, a11, a5
    1422:	fecf92        	addi	a9, a15, -2
    1425:	119900        	slli	a9, a9, 16
    1428:	401300        	ssl	a3
    142b:	c065a0        	sub	a6, a5, a10
    142e:	c2f6e0        	quou	a15, a6, a14
    1431:	0fad           	mov.n	a10, a15
    1433:	82ccf0        	mull	a12, a12, a15
    1436:	e266e0        	remu	a6, a6, a14
    1439:	0bf6d0        	depbits	a13, a6, 16, 16
    143c:	0d5d           	mov.n	a5, a13
    143e:	0ebdc7        	bgeu	a13, a12, 1450 <__divdi3+0xc0>
    1441:	af0b           	addi.n	a10, a15, -1
    1443:	5bda           	add.n	a5, a11, a13
    1445:	0735b7        	bltu	a5, a11, 1450 <__divdi3+0xc0>
    1448:	04b5c7        	bgeu	a5, a12, 1450 <__divdi3+0xc0>
    144b:	5b5a           	add.n	a5, a11, a5
    144d:	fecfa2        	addi	a10, a15, -2
    1450:	209a90        	or	a9, a10, a9
    1453:	c055c0        	sub	a5, a5, a12
    1456:	a1a200        	sll	a10, a2
    1459:	82b9a0        	mull	a11, a9, a10
    145c:	a2a9a0        	muluh	a10, a9, a10
    145f:	092d           	mov.n	a2, a9
    1461:	0e35a7        	bltu	a5, a10, 1473 <__divdi3+0xe3>
    1464:	c0c5a0        	sub	a12, a5, a10
    1467:	f63c56        	bnez	a12, 13ce <__divdi3+0x3e>
    146a:	a1d400        	sll	a13, a4
    146d:	023db7        	bltu	a13, a11, 1473 <__divdi3+0xe3>
    1470:	ffd686        	j	13ce <__divdi3+0x3e>

00001473 <__divdi3+0xe3>:
    1473:	290b           	addi.n	a2, a9, -1
    1475:	ffd546        	j	13ce <__divdi3+0x3e>

00001478 <__divdi3+0xe8>:
    1478:	6040b0        	neg	a4, a11
    147b:	f87c           	movi.n	a8, -1
    147d:	309380        	xor	a9, a3, a8
    1480:	e91b           	addi.n	a14, a9, 1
    1482:	839eb0        	moveqz	a9, a14, a11
    1485:	ffc886        	j	13ab <__divdi3+0x1b>

00001488 <__divdi3+0xf8>:
	...

00001489 <__divdi3+0xf9>:
    1489:	69b927        	bgeu	a9, a2, 14f6 <__divdi3+0x166>
    148c:	40f230        	nsau	a3, a2
    148f:	a38c           	beqz.n	a3, 149d <__divdi3+0x10d>
    1491:	401300        	ssl	a3
    1494:	a1a200        	sll	a10, a2
    1497:	81b940        	src	a11, a9, a4
    149a:	a1c400        	sll	a12, a4

0000149d <__divdi3+0x10d>:
    149d:	404010        	ssai	16
    14a0:	9a2d           	ae_zext16	a2, a10
    14a2:	f540a0        	extui	a4, a10, 16, 16
    14a5:	c2db40        	quou	a13, a11, a4
    14a8:	0d9d           	mov.n	a9, a13
    14aa:	8252d0        	mull	a5, a2, a13
    14ad:	e2bb40        	remu	a11, a11, a4
    14b0:	81bbc0        	src	a11, a11, a12
    14b3:	0b3d           	mov.n	a3, a11
    14b5:	0fbb57        	bgeu	a11, a5, 14c8 <__divdi3+0x138>
    14b8:	9d0b           	addi.n	a9, a13, -1
    14ba:	3baa           	add.n	a3, a11, a10
    14bc:	0833a7        	bltu	a3, a10, 14c8 <__divdi3+0x138>
    14bf:	05b357        	bgeu	a3, a5, 14c8 <__divdi3+0x138>
    14c2:	803a30        	add	a3, a10, a3
    14c5:	fecd92        	addi	a9, a13, -2
    14c8:	c0d350        	sub	a13, a3, a5
    14cb:	c23d40        	quou	a3, a13, a4
    14ce:	e2dd40        	remu	a13, a13, a4
    14d1:	0bfdc0        	depbits	a12, a13, 16, 16
    14d4:	824230        	mull	a4, a2, a3
    14d7:	032d           	mov.n	a2, a3
    14d9:	0cbc47        	bgeu	a12, a4, 14e9 <__divdi3+0x159>
    14dc:	230b           	addi.n	a2, a3, -1
    14de:	5caa           	add.n	a5, a12, a10
    14e0:	0535a7        	bltu	a5, a10, 14e9 <__divdi3+0x159>
    14e3:	02b547        	bgeu	a5, a4, 14e9 <__divdi3+0x159>
    14e6:	fec322        	addi	a2, a3, -2
    14e9:	030c           	movi.n	a3, 0
    14eb:	11e900        	slli	a14, a9, 16
    14ee:	2022e0        	or	a2, a2, a14
    14f1:	ffb6c6        	j	13d0 <__divdi3+0x40>

000014f4 <__divdi3+0x164>:
	...

000014f6 <__divdi3+0x166>:
    14f6:	130c           	movi.n	a3, 1
    14f8:	c2f320        	quou	a15, a3, a2
    14fb:	832f20        	moveqz	a2, a15, a2
    14fe:	02ad           	mov.n	a10, a2
    1500:	40f250        	nsau	a5, a2
    1503:	0d1516        	beqz	a5, 15d8 <__divdi3+0x248>
    1506:	c0fd50        	sub	a15, a13, a5
    1509:	401500        	ssl	a5
    150c:	a1a200        	sll	a10, a2
    150f:	9a2d           	ae_zext16	a2, a10
    1511:	400f00        	ssr	a15
    1514:	91e090        	srl	a14, a9
    1517:	401500        	ssl	a5
    151a:	81b940        	src	a11, a9, a4
    151d:	a1c400        	sll	a12, a4
    1520:	404010        	ssai	16
    1523:	f540a0        	extui	a4, a10, 16, 16
    1526:	c2de40        	quou	a13, a14, a4
    1529:	0d9d           	mov.n	a9, a13
    152b:	8252d0        	mull	a5, a2, a13
    152e:	e2ee40        	remu	a14, a14, a4
    1531:	81eeb0        	src	a14, a14, a11
    1534:	0e3d           	mov.n	a3, a14
    1536:	0ebe57        	bgeu	a14, a5, 1548 <__divdi3+0x1b8>
    1539:	9d0b           	addi.n	a9, a13, -1
    153b:	3eaa           	add.n	a3, a14, a10
    153d:	0733a7        	bltu	a3, a10, 1548 <__divdi3+0x1b8>
    1540:	04b357        	bgeu	a3, a5, 1548 <__divdi3+0x1b8>
    1543:	3a3a           	add.n	a3, a10, a3
    1545:	fecd92        	addi	a9, a13, -2
    1548:	c0e350        	sub	a14, a3, a5
    154b:	e2fe40        	remu	a15, a14, a4
    154e:	0bffb0        	depbits	a11, a15, 16, 16
    1551:	c2ee40        	quou	a14, a14, a4
    1554:	0e5d           	mov.n	a5, a14
    1556:	82d2e0        	mull	a13, a2, a14
    1559:	0b3d           	mov.n	a3, a11
    155b:	0ebbd7        	bgeu	a11, a13, 156d <__divdi3+0x1dd>
    155e:	5e0b           	addi.n	a5, a14, -1
    1560:	3baa           	add.n	a3, a11, a10
    1562:	0733a7        	bltu	a3, a10, 156d <__divdi3+0x1dd>
    1565:	04b3d7        	bgeu	a3, a13, 156d <__divdi3+0x1dd>
    1568:	3a3a           	add.n	a3, a10, a3
    156a:	fece52        	addi	a5, a14, -2
    156d:	c0b3d0        	sub	a11, a3, a13
    1570:	113900        	slli	a3, a9, 16
    1573:	203530        	or	a3, a5, a3

00001576 <__divdi3+0x1e6>:
    1576:	404010        	ssai	16
    1579:	e2eb40        	remu	a14, a11, a4
    157c:	c2db40        	quou	a13, a11, a4
    157f:	8252d0        	mull	a5, a2, a13
    1582:	0dbd           	mov.n	a11, a13
    1584:	81eec0        	src	a14, a14, a12
    1587:	0e9d           	mov.n	a9, a14
    1589:	0ebe57        	bgeu	a14, a5, 159b <__divdi3+0x20b>
    158c:	bd0b           	addi.n	a11, a13, -1
    158e:	9eaa           	add.n	a9, a14, a10
    1590:	0739a7        	bltu	a9, a10, 159b <__divdi3+0x20b>
    1593:	04b957        	bgeu	a9, a5, 159b <__divdi3+0x20b>
    1596:	9a9a           	add.n	a9, a10, a9
    1598:	fecdb2        	addi	a11, a13, -2
    159b:	c0f950        	sub	a15, a9, a5
    159e:	c25f40        	quou	a5, a15, a4
    15a1:	e2ff40        	remu	a15, a15, a4
    15a4:	0bffc0        	depbits	a12, a15, 16, 16
    15a7:	824250        	mull	a4, a2, a5
    15aa:	052d           	mov.n	a2, a5
    15ac:	0cbc47        	bgeu	a12, a4, 15bc <__divdi3+0x22c>
    15af:	250b           	addi.n	a2, a5, -1
    15b1:	9caa           	add.n	a9, a12, a10
    15b3:	0539a7        	bltu	a9, a10, 15bc <__divdi3+0x22c>
    15b6:	02b947        	bgeu	a9, a4, 15bc <__divdi3+0x22c>
    15b9:	fec522        	addi	a2, a5, -2
    15bc:	116b00        	slli	a6, a11, 16
    15bf:	202260        	or	a2, a2, a6
    15c2:	e0d856        	bnez	a8, 13d3 <__divdi3+0x43>

000015c5 <__divdi3+0x235>:
    15c5:	f01d           	retw.n

000015c7 <__divdi3+0x237>:
	...

000015c8 <__divdi3+0x238>:
    15c8:	023597        	bltu	a5, a9, 15ce <__divdi3+0x23e>
    15cb:	043427        	bltu	a4, a2, 15d3 <__divdi3+0x243>

000015ce <__divdi3+0x23e>:
    15ce:	120c           	movi.n	a2, 1
    15d0:	ff7e86        	j	13ce <__divdi3+0x3e>

000015d3 <__divdi3+0x243>:
    15d3:	020c           	movi.n	a2, 0
    15d5:	ff7d46        	j	13ce <__divdi3+0x3e>

000015d8 <__divdi3+0x248>:
    15d8:	c0b920        	sub	a11, a9, a2
    15db:	f54020        	extui	a4, a2, 16, 16
    15de:	922d           	ae_zext16	a2, a2
    15e0:	ffe486        	j	1576 <__divdi3+0x1e6>

000015e3 <__divdi3+0x253>:
	...

000015e4 <_fini>:
    15e4:	008136        	entry	a1, 64

000015e7 <_fini+0x3>:
    15e7:	f01d           	retw.n
