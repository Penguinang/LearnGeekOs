1. 学习了解`elf`文件
2. 编译`project1`
    * 报错：`没有可选的插件diskc`，原因是`bochs`新版本已经废弃了这个选项，新版本如下配置
    ```
    ata0: enabled=1, ioaddr1=0x1f0, ioaddr2=0x3f0, irq=14
    ata0-master: type=disk, path=diskc.img, cylinders=40, heads=8, spt=63
    ```
    然后启动
3. 在`elf.c`中完成`Parse_ELF_Executable`函数，结果显示除了第一个字符串。
4. 调试发现，在读第三个程序头时候，读出了偏移、长度、地址均为0的段，看其类型显示为奇怪的`6474e551`，了解后知道，代表linux中的[`PT_GNU_STACK`](!http://refspecs.linuxbase.org/LSB_3.0.0/LSB-PDA/LSB-PDA/progheader.html),目的是为了保护内存，但是似乎和问题没有关系