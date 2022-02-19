// 用户态代码无法直接引用到内核头文件<linux/compiler.h>
// 需要自己定义相关宏操作

// step 1: 内存屏障
#define barrier() __asm__ __volatile__("": : :"memory")

// step 2: volatile read
#define __READ_ONCE(x) (*((const volatile typeof(x) *)(&x)))

// step 3: volatile read and write
#define __WRITE_ONCE(x, val) do {*((volatile typeof(x) *)(&x)) = (val);} while(0)  

int a, b;
int i, j;

int foo() {
    a = i;
    b = j / 16;

    return 0;
}

int foo1() {
    a = i;
    barrier();
    b = j / 16;

    return 0;
}

int foo2() {
    a = __READ_ONCE(i);
    b = __READ_ONCE(j) / 16;

    return 0;
}

int foo3() {
    a = __READ_ONCE(i);
    __WRITE_ONCE(b, 0);

    return 0;
}

int foo4() {
    __WRITE_ONCE(a, __READ_ONCE(i));
    __WRITE_ONCE(b, 0);

    return 0;
}

// 为了效率，编译器和CPU优化了指令执行的顺序，这种优化是在单线程情况下的，
// 而在多核多线程情况下，需要对其做一定的抑制


// [李浩: 再谈 volatile 关键字](https://cloud.tencent.com/developer/article/1801450)
// [对优化说不 - Linux中的Barrier](https://zhuanlan.zhihu.com/p/96001570)
// 
// volatile 实验
// - compiler barriers 阻止编译的优化导致指令乱序执行
// - sequence point 
//   编译器只能保证 volatile 变量之间的读写顺序，不能保证 non-volatile 变量与 volatile 变量之间的顺序 
// gcc -O2 volatile.c -S
// 
// memory barriers 内存屏障
// CPU 乱序执行指令的内存访问顺序与代码设定的顺序不一致
// 内存屏障只能提供对一段代码执行顺序的保证，不能提供SMP系统中，多核对
// 同一个内存变量的访问顺序的保证

// 原子操作
// [读写一气呵成 - Linux中的原子操作](https://zhuanlan.zhihu.com/p/8929939)
// 
// UP 系统
// * 读写
//   CPU仅仅重内存中读取(read/load)或者写入(write/store)一个变量的值，
//   是不可打断、不可分分割的。
// * RMW - Read Modify Write 
//   可被中断，通过关闭中断保证原子性
// 
// SMP 系统
// * 读写
//   总线仲裁，同上
// * RMW
//   - x86 锁总线，缓存一致性，cache lock, cache split
//   - ARMv6 LL/SC, ARMv7 LDREX/STREX, ARMv8 LSE