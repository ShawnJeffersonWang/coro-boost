#include <stdio.h>
#include <sys/ucontext.h>
#include <ucontext.h>

void func1(void* arg) {
    puts("1");
    puts("11");
    puts("111");
    puts("1111");
}

void context_test() {
    char stack[1024 * 128];
    ucontext_t child, main;

    // 获取当前上下文
    getcontext(&child);
    child.uc_stack.ss_sp = stack;
    child.uc_stack.ss_size = sizeof(stack);
    child.uc_stack.ss_flags = 0;
    // 设置后继上下文
    // child.uc_link = nullptr;
    child.uc_link = &main;

    makecontext(&child, (void (*)())func1, 0);
    swapcontext(&main, &child);
    puts("main");
}

int main() {
    context_test();
    return 0;
}