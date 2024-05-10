#include <iostream>
#include "generator.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/resource.h>

#define STRINGIFY_INTERNAL(x) #x
#define STRINGIFY(x) STRINGIFY_INTERNAL(x)

using namespace std;
using namespace Generator;

int out_fd, dev_null_fd;
char *d8_path;

void errExit(char *msg)
{
    perror(msg);
    exit(-1);
}

void initEnv()
{
    system("mkdir fuzz_out");
    out_fd = open("fuzz.log", O_RDWR | O_CREAT | O_APPEND, 0777);
    if (out_fd < 0)
    {
        errExit("Unable to creat out_fd");
    }
    dev_null_fd = open("/dev/null", O_RDWR);
    if (dev_null_fd < 0)
        errExit("Unable to open /dev/null");
}

int test_one(char *js_path, uint32_t timeout)
{
    static struct itimerval it;
    int status;

    int child_pid = fork();
    if (child_pid < 0)
        errExit("fork() failed");
    // 子进程
    if (!child_pid)
    {
        struct rlimit r;

        /* Dumping cores is slow and can lead to anomalies if SIGKILL is delivered
       before the dump is complete. */
        r.rlim_max = r.rlim_cur = 0;
        setrlimit(RLIMIT_CORE, &r); /* Ignore errors */
        /* Isolate the process and configure standard descriptors. If out_file is
       specified, stdin is /dev/null; otherwise, out_fd is cloned instead. */
        setsid();

        // 将程序的输出重定向到文件中
        //dup2(out_fd, 1);
        //dup2(out_fd, 2);

        // 关闭标准输入
        dup2(dev_null_fd, 0);

        // close(dev_null_fd);
        // close(out_fd);

        /* This should improve performance a bit, since it stops the linker from
       doing extra work post-fork(). */

        if (!getenv("LD_BIND_LAZY"))
            setenv("LD_BIND_NOW", "1", 0);
        /* Set sane defaults for ASAN if nothing else specified. */

        setenv("ASAN_OPTIONS", "abort_on_error=1:"
                               "detect_leaks=0:"
                               "symbolize=0:"
                               "detect_odr_violation=0:"
                               "allocator_may_return_null=1",
               0);

        /* MSAN is tricky, because it doesn't support abort_on_error=1 at this
       point. So, we do this in a very hacky way. */

        setenv("MSAN_OPTIONS", "exit_code=" STRINGIFY(MSAN_ERROR) ":"
                                                                  "symbolize=0:"
                                                                  "abort_on_error=1:"
                                                                  "allocator_may_return_null=1:"
                                                                  "msan_track_origins=0",
               0);
        char *argv[] = {"./d8", "--allow-natives-syntax", js_path, NULL};
        execv(d8_path, argv);
        exit(0);
    }

    it.it_value.tv_sec = (timeout / 1000);
    it.it_value.tv_usec = (timeout % 1000) * 1000;

    setitimer(ITIMER_REAL, &it, NULL);

    if (wait(&status) <= 0)
        errExit("wait() failed");
    if (WIFSIGNALED(status))
    {
        printf("Child process received singal %d\n", WTERMSIG(status));
        return 1;
    }
    return 0;
}

void generate_one(Engine *engine)
{
    engine->generate();
    string s = engine->operator string();
    int fd = open("t.js", O_RDWR | O_CREAT | O_TRUNC, 0777);
    write(fd, s.c_str(), s.length());
    close(fd);
}

int crash_count = 0;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("usage: %s d8_path", argv[0]);
        exit(0);
    }
    d8_path = argv[1];
    initEnv();
    Generator::init();
    Engine *engine = new Engine();
    while (1)
    {
        generate_one(engine);
        if (test_one("t.js", 3000))
        {
            printf("crash!\n");
            char cmd[100];
            sprintf(cmd, "cp t.js fuzz_out/poc%d.js", crash_count++);
            system(cmd);
        }
    }
    delete engine;
    return 0;
}