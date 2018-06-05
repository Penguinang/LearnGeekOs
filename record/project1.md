1. 学习了解`elf`文件
2. 编译`project1`
    * 报错：`没有可选的插件diskc`，原因是`bochs`新版本已经废弃了这个选项，新版本如下配置
    ```
    ata0: enabled=1, ioaddr1=0x1f0, ioaddr2=0x3f0, irq=14
    ata0-master: type=disk, path=diskc.img, cylinders=40, heads=8, spt=63
    ```
    然后启动
3. 在`elf.c`中完成`Parse_ELF_Executable`函数，结果显示只有第一个字符串。
4. 调试发现，在读第三个程序头时候，读出了偏移、长度、地址均为0的段，看其类型显示为奇怪的`6474e551`，了解后知道，代表linux中的[`PT_GNU_STACK`](http://refspecs.linuxbase.org/LSB_3.0.0/LSB-PDA/LSB-PDA/progheader.html),目的是为了保护内存，但是似乎和问题没有关系
5. 打开`lprog.c`中的`lprogdebug`选项，显示出打印第一条字符串之后，产生了13号异常（异常是中断的一种，参见[文档](../doc/overview.html)），关于这个异常，在注释中写的是`general protection fault`，这个异常在`trap.c`中注册，在`lowlevel.asm`的`Handle_Interrupt`中调用，而`Int_With_Err`宏使用后者在`g_entryPointTableStart`后面定义了一系列终端处理程序入口。在`idt.c`的`Init_IDT`函数中，使用了`g_entryPointTableStart`来构建`IDT(中断描述符表)`,然后将构建的`IDT`地址`idtBaseAddr`通过定义在`lowlevel.asm`中的函数`Load_IDTR`，使用指令`lidt`加载到`IDT`寄存器中
6. 在`Parse_ELF_Executable`函数执行之后，追踪到`Trampoline(lpprog.c)`函数中出现了问题，这个函数在`lowlevel.asm`中定义。通过调试器，在`1ab87`(开启调试打印后为`1ac2d`，`cs`皆为`0x8`。注：32位cpu中`cs`不再是简单的实地址中的偏移，而是一个段选择符的索引)处打上断点，这里是`Trampoline`开始的地方（关于获取这个地址的方法，我是这样做的：在Trampoline函数调用的前面写了一个1/0的错误运算，调试器执行到这里的时候，会自动打印出异常信息和寄存器现场）。不过每次打开调试打印之后，`bochs`就不在断点处停止了，很郁闷。查不下去了。找不到产生`exception`的原因。
7. 在知乎上找到了[答案](https://www.zhihu.com/question/47582842)
8. 最后一个问题是，无法打印出`s2`,但是可以使用将`s2`作为数组遍历所有字符