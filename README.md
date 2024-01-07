# self-Libs


## c-hash-lib

## get-signal-table

### 执行过程  python process_addr.py a.out
```
[fio_climate_model@ln131%bscc-a6 timer]$ python process_addr.py a.out
will processing a.out ......
will processing a.out.greped ......
will processing a.out.colonfree ......
will processing a.out.terminal ......
will processing a.out down
```


### 1、a.out.signal
```
a.out:     file format elf64-x86-64


Disassembly of section .init:

00000000004003e0 <_init>:
  4003e0:        48 83 ec 08                  sub    $0x8,%rsp
  4003e4:        48 8b 05 0d 0c 20 00         mov    0x200c0d(%rip),%rax        # 600ff8 <__gmon_start__>
  4003eb:        48 85 c0                     test   %rax,%rax
  4003ee:        74 05                        je     4003f5 <_init+0x15>
  4003f0:        e8 3b 00 00 00               callq  400430 <__gmon_start__@plt>
  4003f5:        48 83 c4 08                  add    $0x8,%rsp
  4003f9:        c3                           retq   

Disassembly of section .plt:

0000000000400400 <.plt>:
  400400:        ff 35 02 0c 20 00            pushq  0x200c02(%rip)        # 601008 <_GLOBAL_OFFSET_TABLE_+0x8>
  400406:        ff 25 04 0c 20 00            jmpq   *0x200c04(%rip)        # 601010 <_GLOBAL_OFFSET_TABLE_+0x10>
  40040c:        0f 1f 40 00                  nopl   0x0(%rax)

0000000000400410 <printf@plt>:
  400410:        ff 25 02 0c 20 00            jmpq   *0x200c02(%rip)        # 601018 <printf@GLIBC_2.2.5>
  400416:        68 00 00 00 00               pushq  $0x0
  40041b:        e9 e0 ff ff ff               jmpq   400400 <.plt>
```


### 2、a.out.grepd
```
7:00000000004003e0 <_init>:
18:0000000000400400 <.plt>:
23:0000000000400410 <printf@plt>:
28:0000000000400420 <__libc_start_main@plt>:
33:0000000000400430 <__gmon_start__@plt>:
40:0000000000400440 <_start>:
55:0000000000400470 <deregister_tm_clones>:
72:00000000004004a0 <register_tm_clones>:
94:00000000004004e0 <__do_global_dtors_aux>:
105:0000000000400500 <frame_dummy>:
121:000000000040052d <do_multi>:
131:0000000000400540 <do_calc>:
145:000000000040055f <main>:
175:00000000004005c0 <__libc_csu_init>:
215:0000000000400630 <__libc_csu_fini>:
220:0000000000400634 <_fini>:
```


### 3、a.out.colonfree
```
7 0x4003e0  _init  
18 0x400400  .plt  
23 0x400410  printf@plt  
28 0x400420  __libc_start_main@plt  
33 0x400430  __gmon_start__@plt  
40 0x400440  _start  
55 0x400470  deregister_tm_clones  
72 0x4004a0  register_tm_clones  
94 0x4004e0  __do_global_dtors_aux  
105 0x400500  frame_dummy  
121 0x40052d  do_multi  
131 0x400540  do_calc  
145 0x40055f  main  
175 0x4005c0  __libc_csu_init  
215 0x400630  __libc_csu_fini  
220 0x400634  _fini  
```

### 4、a.out.terminal
```
0x4003e0 _init
0x400400 .plt
0x400410 printf@plt
0x400420 __libc_start_main@plt
0x400430 __gmon_start__@plt
0x400440 _start
0x400470 deregister_tm_clones
0x4004a0 register_tm_clones
0x4004e0 __do_global_dtors_aux
0x400500 frame_dummy
0x40052d do_multi
0x400540 do_calc
0x40055f main
0x4005c0 __libc_csu_init
0x400630 __libc_csu_fini
0x400634 _fini
```