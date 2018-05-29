# 编译
1. 下载[项目](!https://excellmedia.dl.sourceforge.net/project/geekos/geekos/geekos-0.3.0/geekos-0.3.0.zip),解压
2. 进入`scripts`文件夹，使用`startProject`脚本拷贝一个项目至当前目录，用法 `./startProject project0 ../src`
3. 进入`project`目录，进入`build`目录，使用`make`编译
    * 编译的选项是在makefile中 `CC_GENERAL_OPTS` ,这行是编译选项
    * 编译后很多错误,原因编译选项使用了`-Werror`选项，将警告当作了error，在makefile中找到 `-Werror` 删掉
    * 提示`__stack__chk_fail 未定义的引用`,解法是在编译选项里加上`-fno-stack-protector`选项,使用`make clean`之后再`make`
    * 提示架构问题，在`NASM_KERNEL_OPTS`选项中将`elf`修改为`elf64`，使用`make clean`之后再`make`
# 调试
1. 在`project0`的`build`目录中修改`.bochsrc`文件，修改成功后，使用`bochs`命令打开虚拟机，看到七个选项，第六个为启动，输入6,回车启动
2. 在 `.bochsrc` 默认设置启动日志文件是 `bochs.out`
3. 启动后，在控制台输入 `c`(意为continue) ，回车，在虚拟机中出现提示为`cant read boot disk`
4. 然后点击虚拟机右上角的`config`，查看软盘1,发现状态是`ejected`，大小是1.44M，经过了解，1.44M是软盘的统一标准容量，也是在配置文件中设置`fd.img`文件时设置的，但是实际的img文件有2.1M，因此有可能是这里的问题
5. 考虑到编译结果大小的问题，可能是与编译时候的选项有关系的，因此考虑使用成套的32未编译选项，即
    * 在 `TARGET_LD` 行最后面加上 `-m elf_i386`
    * 在 `CC_GENERAL_OPTS` 行加上 `-m32`
    * 修改之前改的`elf64` 回`elf`
    * 编译之后，只有58K，运行之后，发现应该是成功载入了，可以显示内存大小和分页情况，但是还有一点小问题