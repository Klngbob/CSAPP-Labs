
kernels.o：     文件格式 elf32-i386


Disassembly of section .text:

00000000 <naive_rotate>:
   0:	55                   	push   %ebp
   1:	57                   	push   %edi
   2:	56                   	push   %esi
   3:	53                   	push   %ebx
   4:	83 ec 04             	sub    $0x4,%esp
   7:	8b 4c 24 18          	mov    0x18(%esp),%ecx
   b:	8b 5c 24 20          	mov    0x20(%esp),%ebx
   f:	85 c9                	test   %ecx,%ecx
  11:	7e 4c                	jle    5f <naive_rotate+0x5f>
  13:	8d 51 ff             	lea    -0x1(%ecx),%edx
  16:	8d 3c 49             	lea    (%ecx,%ecx,2),%edi
  19:	8b 44 24 1c          	mov    0x1c(%esp),%eax
  1d:	0f af d1             	imul   %ecx,%edx
  20:	01 ff                	add    %edi,%edi
  22:	8d 34 52             	lea    (%edx,%edx,2),%esi
  25:	01 ca                	add    %ecx,%edx
  27:	8d 14 52             	lea    (%edx,%edx,2),%edx
  2a:	6b c9 fa             	imul   $0xfffffffa,%ecx,%ecx
  2d:	8d 34 73             	lea    (%ebx,%esi,2),%esi
  30:	8d 2c 53             	lea    (%ebx,%edx,2),%ebp
  33:	90                   	nop
  34:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  38:	8d 1c 07             	lea    (%edi,%eax,1),%ebx
  3b:	89 f2                	mov    %esi,%edx
  3d:	89 34 24             	mov    %esi,(%esp)
  40:	8b 30                	mov    (%eax),%esi
  42:	83 c0 06             	add    $0x6,%eax
  45:	89 32                	mov    %esi,(%edx)
  47:	0f b7 70 fe          	movzwl -0x2(%eax),%esi
  4b:	66 89 72 04          	mov    %si,0x4(%edx)
  4f:	01 ca                	add    %ecx,%edx
  51:	39 d8                	cmp    %ebx,%eax
  53:	75 eb                	jne    40 <naive_rotate+0x40>
  55:	8b 34 24             	mov    (%esp),%esi
  58:	83 c6 06             	add    $0x6,%esi
  5b:	39 ee                	cmp    %ebp,%esi
  5d:	75 d9                	jne    38 <naive_rotate+0x38>
  5f:	83 c4 04             	add    $0x4,%esp
  62:	5b                   	pop    %ebx
  63:	5e                   	pop    %esi
  64:	5f                   	pop    %edi
  65:	5d                   	pop    %ebp
  66:	c3                   	ret    
  67:	89 f6                	mov    %esi,%esi
  69:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000070 <rotate>:
  70:	55                   	push   %ebp
  71:	57                   	push   %edi
  72:	56                   	push   %esi
  73:	53                   	push   %ebx
  74:	83 ec 04             	sub    $0x4,%esp
  77:	8b 4c 24 18          	mov    0x18(%esp),%ecx
  7b:	8b 5c 24 20          	mov    0x20(%esp),%ebx
  7f:	8d 51 ff             	lea    -0x1(%ecx),%edx
  82:	0f af d1             	imul   %ecx,%edx
  85:	85 c9                	test   %ecx,%ecx
  87:	7e 46                	jle    cf <rotate+0x5f>
  89:	8d 34 52             	lea    (%edx,%edx,2),%esi
  8c:	01 ca                	add    %ecx,%edx
  8e:	8d 3c 49             	lea    (%ecx,%ecx,2),%edi
  91:	8d 14 52             	lea    (%edx,%edx,2),%edx
  94:	8b 44 24 1c          	mov    0x1c(%esp),%eax
  98:	6b c9 fa             	imul   $0xfffffffa,%ecx,%ecx
  9b:	8d 34 73             	lea    (%ebx,%esi,2),%esi
  9e:	01 ff                	add    %edi,%edi
  a0:	8d 2c 53             	lea    (%ebx,%edx,2),%ebp
  a3:	90                   	nop
  a4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  a8:	8d 1c 07             	lea    (%edi,%eax,1),%ebx
  ab:	89 f2                	mov    %esi,%edx
  ad:	89 34 24             	mov    %esi,(%esp)
  b0:	8b 30                	mov    (%eax),%esi
  b2:	83 c0 06             	add    $0x6,%eax
  b5:	89 32                	mov    %esi,(%edx)
  b7:	0f b7 70 fe          	movzwl -0x2(%eax),%esi
  bb:	66 89 72 04          	mov    %si,0x4(%edx)
  bf:	01 ca                	add    %ecx,%edx
  c1:	39 d8                	cmp    %ebx,%eax
  c3:	75 eb                	jne    b0 <rotate+0x40>
  c5:	8b 34 24             	mov    (%esp),%esi
  c8:	83 c6 06             	add    $0x6,%esi
  cb:	39 ee                	cmp    %ebp,%esi
  cd:	75 d9                	jne    a8 <rotate+0x38>
  cf:	83 c4 04             	add    $0x4,%esp
  d2:	5b                   	pop    %ebx
  d3:	5e                   	pop    %esi
  d4:	5f                   	pop    %edi
  d5:	5d                   	pop    %ebp
  d6:	c3                   	ret    
  d7:	89 f6                	mov    %esi,%esi
  d9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

000000e0 <naive_smooth>:
  e0:	55                   	push   %ebp
  e1:	57                   	push   %edi
  e2:	56                   	push   %esi
  e3:	53                   	push   %ebx
  e4:	83 ec 40             	sub    $0x40,%esp
  e7:	8b 44 24 54          	mov    0x54(%esp),%eax
  eb:	85 c0                	test   %eax,%eax
  ed:	0f 8e 84 01 00 00    	jle    277 <naive_smooth+0x197>
  f3:	8b 44 24 54          	mov    0x54(%esp),%eax
  f7:	c7 44 24 38 00 00 00 	movl   $0x0,0x38(%esp)
  fe:	00 
  ff:	83 e8 01             	sub    $0x1,%eax
 102:	89 44 24 30          	mov    %eax,0x30(%esp)
 106:	8b 44 24 54          	mov    0x54(%esp),%eax
 10a:	8d 04 40             	lea    (%eax,%eax,2),%eax
 10d:	01 c0                	add    %eax,%eax
 10f:	89 44 24 10          	mov    %eax,0x10(%esp)
 113:	8b 44 24 5c          	mov    0x5c(%esp),%eax
 117:	89 44 24 3c          	mov    %eax,0x3c(%esp)
 11b:	83 44 24 38 01       	addl   $0x1,0x38(%esp)
 120:	8b 7c 24 30          	mov    0x30(%esp),%edi
 124:	be 00 00 00 00       	mov    $0x0,%esi
 129:	8b 5c 24 38          	mov    0x38(%esp),%ebx
 12d:	c7 44 24 20 00 00 00 	movl   $0x0,0x20(%esp)
 134:	00 
 135:	89 d8                	mov    %ebx,%eax
 137:	83 e8 02             	sub    $0x2,%eax
 13a:	0f 49 f0             	cmovns %eax,%esi
 13d:	39 df                	cmp    %ebx,%edi
 13f:	89 d8                	mov    %ebx,%eax
 141:	0f 4e c7             	cmovle %edi,%eax
 144:	89 74 24 28          	mov    %esi,0x28(%esp)
 148:	8b 5c 24 3c          	mov    0x3c(%esp),%ebx
 14c:	0f af 74 24 54       	imul   0x54(%esp),%esi
 151:	89 44 24 2c          	mov    %eax,0x2c(%esp)
 155:	83 c0 01             	add    $0x1,%eax
 158:	89 5c 24 24          	mov    %ebx,0x24(%esp)
 15c:	89 44 24 14          	mov    %eax,0x14(%esp)
 160:	89 74 24 34          	mov    %esi,0x34(%esp)
 164:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
 168:	8b 5c 24 28          	mov    0x28(%esp),%ebx
 16c:	8b 44 24 2c          	mov    0x2c(%esp),%eax
 170:	39 c3                	cmp    %eax,%ebx
 172:	0f 8f 07 01 00 00    	jg     27f <naive_smooth+0x19f>
 178:	83 44 24 20 01       	addl   $0x1,0x20(%esp)
 17d:	b9 00 00 00 00       	mov    $0x0,%ecx
 182:	8b 74 24 58          	mov    0x58(%esp),%esi
 186:	8b 7c 24 20          	mov    0x20(%esp),%edi
 18a:	89 1c 24             	mov    %ebx,(%esp)
 18d:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
 194:	00 
 195:	89 f8                	mov    %edi,%eax
 197:	83 e8 02             	sub    $0x2,%eax
 19a:	0f 49 c8             	cmovns %eax,%ecx
 19d:	8b 44 24 30          	mov    0x30(%esp),%eax
 1a1:	89 4c 24 08          	mov    %ecx,0x8(%esp)
 1a5:	39 f8                	cmp    %edi,%eax
 1a7:	0f 4e f8             	cmovle %eax,%edi
 1aa:	8b 44 24 34          	mov    0x34(%esp),%eax
 1ae:	31 db                	xor    %ebx,%ebx
 1b0:	8d 57 01             	lea    0x1(%edi),%edx
 1b3:	89 7c 24 0c          	mov    %edi,0xc(%esp)
 1b7:	01 c8                	add    %ecx,%eax
 1b9:	29 ca                	sub    %ecx,%edx
 1bb:	8d 04 40             	lea    (%eax,%eax,2),%eax
 1be:	89 54 24 1c          	mov    %edx,0x1c(%esp)
 1c2:	8d 6c 46 02          	lea    0x2(%esi,%eax,2),%ebp
 1c6:	89 f8                	mov    %edi,%eax
 1c8:	31 f6                	xor    %esi,%esi
 1ca:	29 c8                	sub    %ecx,%eax
 1cc:	8d 04 40             	lea    (%eax,%eax,2),%eax
 1cf:	01 c0                	add    %eax,%eax
 1d1:	89 44 24 18          	mov    %eax,0x18(%esp)
 1d5:	31 c0                	xor    %eax,%eax
 1d7:	89 f6                	mov    %esi,%esi
 1d9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
 1e0:	8b 54 24 0c          	mov    0xc(%esp),%edx
 1e4:	39 54 24 08          	cmp    %edx,0x8(%esp)
 1e8:	7f 2f                	jg     219 <naive_smooth+0x139>
 1ea:	8b 7c 24 18          	mov    0x18(%esp),%edi
 1ee:	89 ea                	mov    %ebp,%edx
 1f0:	8d 7c 3d 06          	lea    0x6(%ebp,%edi,1),%edi
 1f4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
 1f8:	0f b7 4a fe          	movzwl -0x2(%edx),%ecx
 1fc:	83 c2 06             	add    $0x6,%edx
 1ff:	01 c8                	add    %ecx,%eax
 201:	0f b7 4a fa          	movzwl -0x6(%edx),%ecx
 205:	01 ce                	add    %ecx,%esi
 207:	0f b7 4a fc          	movzwl -0x4(%edx),%ecx
 20b:	01 cb                	add    %ecx,%ebx
 20d:	39 fa                	cmp    %edi,%edx
 20f:	75 e7                	jne    1f8 <naive_smooth+0x118>
 211:	8b 54 24 1c          	mov    0x1c(%esp),%edx
 215:	01 54 24 04          	add    %edx,0x4(%esp)
 219:	83 04 24 01          	addl   $0x1,(%esp)
 21d:	03 6c 24 10          	add    0x10(%esp),%ebp
 221:	8b 3c 24             	mov    (%esp),%edi
 224:	39 7c 24 14          	cmp    %edi,0x14(%esp)
 228:	75 b6                	jne    1e0 <naive_smooth+0x100>
 22a:	8b 4c 24 04          	mov    0x4(%esp),%ecx
 22e:	99                   	cltd   
 22f:	8b 7c 24 24          	mov    0x24(%esp),%edi
 233:	f7 f9                	idiv   %ecx
 235:	66 89 07             	mov    %ax,(%edi)
 238:	89 f0                	mov    %esi,%eax
 23a:	89 fe                	mov    %edi,%esi
 23c:	99                   	cltd   
 23d:	83 c6 06             	add    $0x6,%esi
 240:	f7 f9                	idiv   %ecx
 242:	89 74 24 24          	mov    %esi,0x24(%esp)
 246:	66 89 47 02          	mov    %ax,0x2(%edi)
 24a:	89 d8                	mov    %ebx,%eax
 24c:	99                   	cltd   
 24d:	f7 f9                	idiv   %ecx
 24f:	66 89 47 04          	mov    %ax,0x4(%edi)
 253:	8b 44 24 20          	mov    0x20(%esp),%eax
 257:	39 44 24 54          	cmp    %eax,0x54(%esp)
 25b:	0f 85 07 ff ff ff    	jne    168 <naive_smooth+0x88>
 261:	8b 5c 24 10          	mov    0x10(%esp),%ebx
 265:	8b 44 24 38          	mov    0x38(%esp),%eax
 269:	01 5c 24 3c          	add    %ebx,0x3c(%esp)
 26d:	3b 44 24 54          	cmp    0x54(%esp),%eax
 271:	0f 85 a4 fe ff ff    	jne    11b <naive_smooth+0x3b>
 277:	83 c4 40             	add    $0x40,%esp
 27a:	5b                   	pop    %ebx
 27b:	5e                   	pop    %esi
 27c:	5f                   	pop    %edi
 27d:	5d                   	pop    %ebp
 27e:	c3                   	ret    
 27f:	0f 0b                	ud2    
 281:	eb 0d                	jmp    290 <smooth>
 283:	90                   	nop
 284:	90                   	nop
 285:	90                   	nop
 286:	90                   	nop
 287:	90                   	nop
 288:	90                   	nop
 289:	90                   	nop
 28a:	90                   	nop
 28b:	90                   	nop
 28c:	90                   	nop
 28d:	90                   	nop
 28e:	90                   	nop
 28f:	90                   	nop

00000290 <smooth>:
 290:	55                   	push   %ebp
 291:	57                   	push   %edi
 292:	56                   	push   %esi
 293:	53                   	push   %ebx
 294:	83 ec 40             	sub    $0x40,%esp
 297:	8b 44 24 54          	mov    0x54(%esp),%eax
 29b:	85 c0                	test   %eax,%eax
 29d:	0f 8e 84 01 00 00    	jle    427 <smooth+0x197>
 2a3:	8b 44 24 54          	mov    0x54(%esp),%eax
 2a7:	c7 44 24 38 00 00 00 	movl   $0x0,0x38(%esp)
 2ae:	00 
 2af:	83 e8 01             	sub    $0x1,%eax
 2b2:	89 44 24 30          	mov    %eax,0x30(%esp)
 2b6:	8b 44 24 54          	mov    0x54(%esp),%eax
 2ba:	8d 04 40             	lea    (%eax,%eax,2),%eax
 2bd:	01 c0                	add    %eax,%eax
 2bf:	89 44 24 10          	mov    %eax,0x10(%esp)
 2c3:	8b 44 24 5c          	mov    0x5c(%esp),%eax
 2c7:	89 44 24 3c          	mov    %eax,0x3c(%esp)
 2cb:	83 44 24 38 01       	addl   $0x1,0x38(%esp)
 2d0:	8b 7c 24 30          	mov    0x30(%esp),%edi
 2d4:	be 00 00 00 00       	mov    $0x0,%esi
 2d9:	8b 5c 24 38          	mov    0x38(%esp),%ebx
 2dd:	c7 44 24 20 00 00 00 	movl   $0x0,0x20(%esp)
 2e4:	00 
 2e5:	89 d8                	mov    %ebx,%eax
 2e7:	83 e8 02             	sub    $0x2,%eax
 2ea:	0f 49 f0             	cmovns %eax,%esi
 2ed:	39 df                	cmp    %ebx,%edi
 2ef:	89 d8                	mov    %ebx,%eax
 2f1:	0f 4e c7             	cmovle %edi,%eax
 2f4:	89 74 24 28          	mov    %esi,0x28(%esp)
 2f8:	8b 5c 24 3c          	mov    0x3c(%esp),%ebx
 2fc:	0f af 74 24 54       	imul   0x54(%esp),%esi
 301:	89 44 24 2c          	mov    %eax,0x2c(%esp)
 305:	83 c0 01             	add    $0x1,%eax
 308:	89 5c 24 24          	mov    %ebx,0x24(%esp)
 30c:	89 44 24 14          	mov    %eax,0x14(%esp)
 310:	89 74 24 34          	mov    %esi,0x34(%esp)
 314:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
 318:	8b 5c 24 28          	mov    0x28(%esp),%ebx
 31c:	8b 44 24 2c          	mov    0x2c(%esp),%eax
 320:	39 c3                	cmp    %eax,%ebx
 322:	0f 8f 07 01 00 00    	jg     42f <smooth+0x19f>
 328:	83 44 24 20 01       	addl   $0x1,0x20(%esp)
 32d:	b9 00 00 00 00       	mov    $0x0,%ecx
 332:	8b 74 24 58          	mov    0x58(%esp),%esi
 336:	8b 7c 24 20          	mov    0x20(%esp),%edi
 33a:	89 1c 24             	mov    %ebx,(%esp)
 33d:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
 344:	00 
 345:	89 f8                	mov    %edi,%eax
 347:	83 e8 02             	sub    $0x2,%eax
 34a:	0f 49 c8             	cmovns %eax,%ecx
 34d:	8b 44 24 30          	mov    0x30(%esp),%eax
 351:	89 4c 24 08          	mov    %ecx,0x8(%esp)
 355:	39 f8                	cmp    %edi,%eax
 357:	0f 4e f8             	cmovle %eax,%edi
 35a:	8b 44 24 34          	mov    0x34(%esp),%eax
 35e:	31 db                	xor    %ebx,%ebx
 360:	8d 57 01             	lea    0x1(%edi),%edx
 363:	89 7c 24 0c          	mov    %edi,0xc(%esp)
 367:	01 c8                	add    %ecx,%eax
 369:	29 ca                	sub    %ecx,%edx
 36b:	8d 04 40             	lea    (%eax,%eax,2),%eax
 36e:	89 54 24 1c          	mov    %edx,0x1c(%esp)
 372:	8d 6c 46 02          	lea    0x2(%esi,%eax,2),%ebp
 376:	89 f8                	mov    %edi,%eax
 378:	31 f6                	xor    %esi,%esi
 37a:	29 c8                	sub    %ecx,%eax
 37c:	8d 04 40             	lea    (%eax,%eax,2),%eax
 37f:	01 c0                	add    %eax,%eax
 381:	89 44 24 18          	mov    %eax,0x18(%esp)
 385:	31 c0                	xor    %eax,%eax
 387:	89 f6                	mov    %esi,%esi
 389:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
 390:	8b 54 24 0c          	mov    0xc(%esp),%edx
 394:	39 54 24 08          	cmp    %edx,0x8(%esp)
 398:	7f 2f                	jg     3c9 <smooth+0x139>
 39a:	8b 7c 24 18          	mov    0x18(%esp),%edi
 39e:	89 ea                	mov    %ebp,%edx
 3a0:	8d 7c 3d 06          	lea    0x6(%ebp,%edi,1),%edi
 3a4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
 3a8:	0f b7 4a fe          	movzwl -0x2(%edx),%ecx
 3ac:	83 c2 06             	add    $0x6,%edx
 3af:	01 c8                	add    %ecx,%eax
 3b1:	0f b7 4a fa          	movzwl -0x6(%edx),%ecx
 3b5:	01 ce                	add    %ecx,%esi
 3b7:	0f b7 4a fc          	movzwl -0x4(%edx),%ecx
 3bb:	01 cb                	add    %ecx,%ebx
 3bd:	39 fa                	cmp    %edi,%edx
 3bf:	75 e7                	jne    3a8 <smooth+0x118>
 3c1:	8b 54 24 1c          	mov    0x1c(%esp),%edx
 3c5:	01 54 24 04          	add    %edx,0x4(%esp)
 3c9:	83 04 24 01          	addl   $0x1,(%esp)
 3cd:	03 6c 24 10          	add    0x10(%esp),%ebp
 3d1:	8b 3c 24             	mov    (%esp),%edi
 3d4:	39 7c 24 14          	cmp    %edi,0x14(%esp)
 3d8:	75 b6                	jne    390 <smooth+0x100>
 3da:	8b 4c 24 04          	mov    0x4(%esp),%ecx
 3de:	99                   	cltd   
 3df:	8b 7c 24 24          	mov    0x24(%esp),%edi
 3e3:	f7 f9                	idiv   %ecx
 3e5:	66 89 07             	mov    %ax,(%edi)
 3e8:	89 f0                	mov    %esi,%eax
 3ea:	89 fe                	mov    %edi,%esi
 3ec:	99                   	cltd   
 3ed:	83 c6 06             	add    $0x6,%esi
 3f0:	f7 f9                	idiv   %ecx
 3f2:	89 74 24 24          	mov    %esi,0x24(%esp)
 3f6:	66 89 47 02          	mov    %ax,0x2(%edi)
 3fa:	89 d8                	mov    %ebx,%eax
 3fc:	99                   	cltd   
 3fd:	f7 f9                	idiv   %ecx
 3ff:	66 89 47 04          	mov    %ax,0x4(%edi)
 403:	8b 44 24 20          	mov    0x20(%esp),%eax
 407:	39 44 24 54          	cmp    %eax,0x54(%esp)
 40b:	0f 85 07 ff ff ff    	jne    318 <smooth+0x88>
 411:	8b 5c 24 10          	mov    0x10(%esp),%ebx
 415:	8b 44 24 38          	mov    0x38(%esp),%eax
 419:	01 5c 24 3c          	add    %ebx,0x3c(%esp)
 41d:	3b 44 24 54          	cmp    0x54(%esp),%eax
 421:	0f 85 a4 fe ff ff    	jne    2cb <smooth+0x3b>
 427:	83 c4 40             	add    $0x40,%esp
 42a:	5b                   	pop    %ebx
 42b:	5e                   	pop    %esi
 42c:	5f                   	pop    %edi
 42d:	5d                   	pop    %ebp
 42e:	c3                   	ret    
 42f:	0f 0b                	ud2    
 431:	eb 0d                	jmp    440 <register_rotate_functions>
 433:	90                   	nop
 434:	90                   	nop
 435:	90                   	nop
 436:	90                   	nop
 437:	90                   	nop
 438:	90                   	nop
 439:	90                   	nop
 43a:	90                   	nop
 43b:	90                   	nop
 43c:	90                   	nop
 43d:	90                   	nop
 43e:	90                   	nop
 43f:	90                   	nop

00000440 <register_rotate_functions>:
 440:	53                   	push   %ebx
 441:	e8 fc ff ff ff       	call   442 <register_rotate_functions+0x2>
 446:	81 c3 02 00 00 00    	add    $0x2,%ebx
 44c:	83 ec 10             	sub    $0x10,%esp
 44f:	8d 83 00 00 00 00    	lea    0x0(%ebx),%eax
 455:	50                   	push   %eax
 456:	8d 83 00 00 00 00    	lea    0x0(%ebx),%eax
 45c:	50                   	push   %eax
 45d:	e8 fc ff ff ff       	call   45e <register_rotate_functions+0x1e>
 462:	58                   	pop    %eax
 463:	8d 83 00 00 00 00    	lea    0x0(%ebx),%eax
 469:	5a                   	pop    %edx
 46a:	50                   	push   %eax
 46b:	8d 83 00 00 00 00    	lea    0x0(%ebx),%eax
 471:	50                   	push   %eax
 472:	e8 fc ff ff ff       	call   473 <register_rotate_functions+0x33>
 477:	83 c4 18             	add    $0x18,%esp
 47a:	5b                   	pop    %ebx
 47b:	c3                   	ret    
 47c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

00000480 <register_smooth_functions>:
 480:	53                   	push   %ebx
 481:	e8 fc ff ff ff       	call   482 <register_smooth_functions+0x2>
 486:	81 c3 02 00 00 00    	add    $0x2,%ebx
 48c:	83 ec 10             	sub    $0x10,%esp
 48f:	8d 83 00 00 00 00    	lea    0x0(%ebx),%eax
 495:	50                   	push   %eax
 496:	8d 83 00 00 00 00    	lea    0x0(%ebx),%eax
 49c:	50                   	push   %eax
 49d:	e8 fc ff ff ff       	call   49e <register_smooth_functions+0x1e>
 4a2:	58                   	pop    %eax
 4a3:	8d 83 00 00 00 00    	lea    0x0(%ebx),%eax
 4a9:	5a                   	pop    %edx
 4aa:	50                   	push   %eax
 4ab:	8d 83 00 00 00 00    	lea    0x0(%ebx),%eax
 4b1:	50                   	push   %eax
 4b2:	e8 fc ff ff ff       	call   4b3 <register_smooth_functions+0x33>
 4b7:	83 c4 18             	add    $0x18,%esp
 4ba:	5b                   	pop    %ebx
 4bb:	c3                   	ret    

Disassembly of section .text.__x86.get_pc_thunk.bx:

00000000 <__x86.get_pc_thunk.bx>:
   0:	8b 1c 24             	mov    (%esp),%ebx
   3:	c3                   	ret    
