# 编译
1. 下载[项目](!https://excellmedia.dl.sourceforge.net/project/geekos/geekos/geekos-0.3.0/geekos-0.3.0.zip),解压
2. 进入`scripts`文件夹，使用`startProject`脚本拷贝一个项目至当前目录，用法 `./startProject project0 ../src`
3. 进入`project`目录，进入`build`目录，使用`make`编译
    * 编译的选项是在makefile中 `CC_GENERAL_OPTS` ,这行是编译选项
    * 编译后很多错误,原因编译选项使用了`-Werror`选项，将警告当作了error，在makefile中找到 `-Werror` 删掉
    * 提示`__stack__chk_fail 未定义的引用`,解法是在编译选项里加上`-fno-stack-protector`选项,使用`make clean`之后再`make`
    * ~~提示架构问题，在`NASM_KERNEL_OPTS`选项中将`elf`修改为`elf64`，使用`make clean`之后再`make`~~ 错误解法，下为正解
    * `CC`在编译选项里面添加`-m32`
    * 在`TARGET_LD`编译命令后面加上`-m elf_i386`

# 调试
1. 在`project0`的`build`目录中修改`.bochsrc`文件，修改成功后，使用`bochs`命令打开虚拟机，看到七个选项，第六个为启动，输入6,回车启动
2. 在 `.bochsrc` 默认设置启动日志文件是 `bochs.out`
3. 启动后，在控制台输入 `c`(意为continue) ，回车
4. ~~在虚拟机中出现提示为`cant read boot disk`~~
4. ~~然后点击虚拟机右上角的`config`，查看软盘1,发现状态是`ejected`，大小是1.44M，经过了解，1.44M是软盘的统一标准容量，也是在配置文件中设置`fd.img`文件时设置的，但是实际的img文件有2.1M，因此有可能是这里的问题~~
5. ~~考虑到编译结果大小的问题，可能是与编译时候的选项有关系的，因此考虑使用成套的32位编译选项，即~~
    * ~~在 `TARGET_LD` 行最后面加上 `-m elf_i386`~~
    * ~~在 `CC_GENERAL_OPTS` 行加上 `-m32`~~
    * ~~修改之前改的`elf64` 回`elf`~~
    * ~~编译之后，只有58K，运行之后，发现应该是成功载入了，可以显示内存大小和分页情况，但是还有一点小问题~~
6. 上一步之后，启动显示`failed assertion in init_idt :g_handlersizenoterr == g_handlersizeerr`,了解之后是`nasm`汇编编译器的版本问题，需要更早的版本如[`2.08.02`](!https://www.nasm.us/pub/nasm/releasebuilds/2.08.02/nasm-2.08.02.tar.gz),下载之后，解压，在目录内运行 `./configure` 脚本，生成`Makefile`,然后编译，生成目标`nasm`,之后使用自己编译的`nasm`编译OS。可以使用软链接，`sudo ln -s [path-to-nasm]/nasm /usr/local/bin/nasm2`,然后在`project0`的`makefile`里面修改`NASM`的值为`nasm2`; 或者不使用软链接，直接将`makefile`里面的`NASM`值改为绝对路径。`make clean`之后再编译,启动`bochs`成功打印出`Welcome to GeekOs`
7. 在`src/geekos/main.c`中写自己的代码，如打印输出等，编译查看结果