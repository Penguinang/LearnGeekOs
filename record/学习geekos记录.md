# 编译
1. 下载[项目](!https://excellmedia.dl.sourceforge.net/project/geekos/geekos/geekos-0.3.0/geekos-0.3.0.zip),解压
2. 进入scripts文件夹，使用startProject脚本拷贝一个项目至当前目录，用法 `./startProject project0 ../src`
3. 进入project目录，进入build目录，使用make编译
    * 编译的选项是在makefile中 GENERAL_OPTS ,这行是编译选项
    * 编译后很多错误,原因编译选项使用了-Werror选项，将警告当作了error，在makefile中找到 -Werror 删掉
    * 提示__stack__chk_fail 未定义的引用,解法是在编译选项里加上-fno-stack-protector选项,使用make clean之后再make
    * 提示架构问题，在编译选项中将elf修改为elf64，使用make clean之后再make
# 调试
1. 在project0的build目录中修改.bochsrc文件，修改成功后，使用bochs命令打开虚拟机，看到七个选项，第六个为启动，输入6,回车启动
