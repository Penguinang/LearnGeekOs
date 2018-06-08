1. 按照`Hint`填完所有函数后，出现了两个`Exception 13`，调试发现，这个是在`Switch_To_User_Context`函数中，调用了`Switch_To_Address_Space(kthread->userContext);`后导致，原来系统开始阶段，对内核态进程掉调用这个函数后，内核态进程的`userContext`是空，因此导致了空指针错误，在这里具体解决只需要加上一个条件判断就好。
2. 具体了解了一下，关于中断和异常的知识，整理如下：
    1. 中断分广义和狭义两种，指外部硬件设备发出的请求是狭义终端，CPU 内部计算出现的错误如除0、溢出等等叫作异常，异常和狭义的中断都是广义的中断
    2. 中断号是由CPU产生，然后根据中断号，在IDTR指向的中断表中找到处理函数，并进入。中断号中的前32位是固定的，其中13号终端也就是之前总是碰到的错误，叫作`General Protection Fault`， 主要是指内存访问错误，很多情况下是由于段寄存器的错误。一些中断发生后，CPU还会产生一个`error code`，13号中断的`error code`含义如下：若是异常原因是和段相关的，则`error code`为段选择子索引，否则为0。[来源](https://wiki.osdev.org/Exceptions)