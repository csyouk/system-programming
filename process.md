# Process

- 프로세스는 현재 실행중인 하나의 **프로그램** 이다.
- 리눅스에서 프로세스를 실행시키기 위해서는 반드시 **부모 프로세스** 가 필요하다.
- 모든 프로세스는 부모가 필요하다.

#### 프로세스 목록 보기
- `ps -ef`
- 다음은 **ps -ef**의 실행결과를 나타낸 것이다.
  - UID : User ID
  - PID : Process ID
  - PPID : Parent Process ID
  - CMD : Command
    - **[ ]** 안에 표시된 것들 : daemon process라고 한다.
      - 운영체제에서 생성. 특정한 일들을 전담한다.
      - 멀티태스킹 운영 체제에서 데몬은 사용자가 직접적으로 제어하지 않고, 백그라운드에서 돌면서 여러 작업을 하는 프로그램을 말한다
    - 접두사 'k'


```bash
UID        PID  PPID  C STIME TTY     TIME     CMD
root         1     0  0 11월07 ?      00:00:01 /sbin/init
root         2     0  0 11월07 ?      00:00:00 [kthreadd]
root         3     2  0 11월07 ?      00:01:33 [ksoftirqd/0]
root         4     2  0 11월07 ?      00:00:00 [kworker/0:0]
user      2296  2288  0 11월07 pts/1  00:00:01 bash
user      3906  2296  0 09:15 pts/1    00:00:00 ps -ef
user      2288  1611  0 11월07 ?      00:00:15 gnome-terminal

```

- `sudo cat /proc/1/maps`
  - **/proc** : 장치 관리자
  - **/1** : process id
  - **/maps** : 메모리 정보.


#### 주소공간은 프로세스마다 독립적이다.
- 동일한 라이브러리이지만, 주소공간의 시작 주소가 다르다.
- 프로세스는 VA(virtual address)를 통해서 RAM을 온전히 자신이 다 쓴다고 생각한다.
  - 실제로 4G RAM을 운영하는 방식이, 1G는 kernel이 3G는 process가 올라간다.
  - 각각의 프로세스는 자신이 3G를 다 쓴다고 생각한다. 각각의 프로세스는 다른 프로세스의 존재에 대해서 신경쓰지 않는다.
  - 커널 내부에서는 VA를 쓴다.
  - MMU는 Memory translation table을 사용해서 각각의 프로세스의 가상 주소를 실제 물리주소를 변환시킨다. 또한 메모리에 대한 접근 권한을 제어한다.
    - Translation Table
      - Virtual Address(CPU) -> MMU -> Physical Address(RAM)
    - Access Permission



-----------------
- 아래 2가지 명령어를 내린 결과를 살펴보면 두 프로세스는 동일한 라이브러리(`/lib/i386-linux-gnu/libc-2.19.so`)를 사용하고 있음을 알 수 있다.
- 하지만, 두 프로세스의 메모리 시작번지는 서로 다름을 알 수 있는데, 이는 프로세스가 서로 독립적으로 작동함을 알 수 있다.
- 해당 프로세스에서 볼 수 있는 메모리의 시작주소는 모두 가상주소이다.

```bash
    sudo cat /proc/1/maps

    start-end / permission / offset / major:minor / inode / file
    permission 부분을 자세히 살펴보면 메모리에 대한 정보를 알 수 있다.
    - r-x : code
    - r-- : read only
    - rw- : data

    b746d000-b7615000 r-xp 00000000 08:01 786531     /lib/i386-linux-gnu/libc-2.19.so
    b7615000-b7617000 r--p 001a8000 08:01 786531     /lib/i386-linux-gnu/libc-2.19.so
    b7617000-b7618000 rw-p 001aa000 08:01 786531     /lib/i386-linux-gnu/libc-2.19.so

    sudo cat /proc/871/maps

    b756a000-b7712000 r-xp 00000000 08:01 786531     /lib/i386-linux-gnu/libc-2.19.so
    b7712000-b7714000 r--p 001a8000 08:01 786531     /lib/i386-linux-gnu/libc-2.19.so
    b7714000-b7715000 rw-p 001aa000 08:01 786531     /lib/i386-linux-gnu/libc-2.19.so

```


--------------

#### 프로세스 상태 확인
![Process State](./img/Process_states.svg)
- `ps -ax`
- **프로세스의 상태는 크게 3가지로 나눈다. 물론 이 상태는 운영체제마다 상태 정의가 다르다.**
  - Running : 실행중.
  - Ready : 실행 가능한 상태.(아직 실행한 상태는 아님.)
  - Blocking/Sleeping/Waiting
    - Event / Condition이 만족될때까지 멈춘 상태. 실행이 중단된 상태.
    - Synchronous / Asynchronous
    - 특정 프로세스의 일이 오래걸릴때, CPU를 쓰지 않을때, 잠시 process를 running상태에서 배제시킨다.

- 리눅스는 Running/Ready 를 합쳐서 TASK_RUNNING 상태라고 정의한다.
- 리눅스는 Blocking/Sleeping/Waiting 상태를 2가지 상태로 나눈다.
  - Uninterruptible sleep :
  - Interruptible sleep : 외부 이벤트에 의해서 sleep상태를 빠져나올 수 있는 상태.
- 프로세스의 상태는 다음과 같은 것들이 있다.
  - R : 실행 대기 상태.
  - D : Uninterruptible sleep
  - S : Interruptible sleep
  - T : 멈춤.
  - Z : Zombie
  - <, N, L, s, l, +


-------------
#### 프로세스 생성
- `system()`
- C 프로그램에서 프로세스를 생성하는 방법을 알아본다.
- 프로세스가 생성되기 위해서는 부모 프로세스를 복재한 다음, 바꿔서 실행하는 일이다.
- 즉, 프로세스가 새로 생성되는 일은 매우 비효율 적인 일이다.

- **int system(const char *command)**
  - 실패시 -1, 그 외에는 command return status를 리턴.
  - return status는 16비트 구성되어 있고, 해석이 필요하다.
    - exit code 8비트 + 시그널 번호 8비트.
    - c main함수에서 exit(0)을 호출하면, exit code의 8비트는 0으로 채워진다.

- 다음은 system함수의 호출과 return status에서 exit code와 signal code를 출력한 예제이다.

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(void)
{
        int ret;

        printf("command = ls -l\n");
        ret = system("ls -l");
        printf("-> ret = %#x (%#x)(%#x)\n", ret, WEXITSTATUS(ret), WIFSIGNALED(ret));

        printf("command = wrong command\n");
        ret = system("wrong command");
        printf("-> ret = %#x (%#x)(%#x)\n", ret, WEXITSTATUS(ret), WIFSIGNALED(ret));

        printf("command = sleep 3\n");
        ret = system("sleep 3");
        printf("-> ret = %#x (%#x)\n", ret, WEXITSTATUS(ret));

        printf("command = sleep 3 &\n");
        ret = system("sleep 3 &");
        printf("-> ret = %#x (%#x)\n", ret, WEXITSTATUS(ret));

        return EXIT_SUCCESS;
}

// 출력 결과
// root@WT4412:/mnt/nfs# ./system
command = ls -l
-rwxr-xr-x    1 root     root          5747 Nov  7  2017 Hello_ARM
-rwxr-xr-x    1 root     root          6099 Nov  7  2017 a.out
-rwxr-xr-x    1 root     root          6099 Nov  7  2017 hello
-rwxr-xr-x    1 root     root          6530 Nov  8  2017 system
-> ret = 0 (0)(0)
command = wrong command
sh: wrong: command not found
-> ret = 0x7f00 (0x7f)(0)
command = sleep 3
-> ret = 0 (0)
command = sleep 3 &
-> ret = 0 (0)
```


------------
#### 프로세스의 교체
- `exec()`
  - `execl()`
  - `execlp()`
- 현재 프로세스를 새로운 프로세스로 교체한다.
- 복제 비용이 생기지 않기 때문에, 자원관리에 좀 더 효율적일 수 있다.

```c
ret = execl("/bin/ls", "ls", "-l", NULL); // execute file
ret = execlp("ls", "ls", "-l", NULL);     // path에서 경로를 찾아서 실행.
```

- 다음은 `exec`계열 함수의 결과들이다.

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

pid_t pid;
int main(int argc, char **argv)
{
        int i, ret;
        printf("[%d] running %s\n", pid = getpid(), argv[0]);
        ret = execl("/bin/ls", "ls", "-l", NULL);
        printf("[%d] ret = %d\n", pid, ret);
        printf("[%d] terminted\n", pid);
        return EXIT_SUCCESS;
}

```


```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

pid_t pid;

int main(int argc, char **argv)
{
        int i, ret;

        printf("[%d] running %s\n", pid = getpid(), argv[0]);

        ret = execl("ls", "ls", "-l", NULL);
        printf("[%d] ret = %d\n", pid, ret);

        printf("[%d] terminted\n", pid);

        return EXIT_SUCCESS;
}
```


```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

pid_t pid;

int main(int argc, char **argv)
{
        int i, ret;

        printf("[%d] running %s\n", pid = getpid(), argv[0]);

        ret = execlp("ls", "ls", "-l", NULL);

        printf("[%d] ret = %d\n", pid, ret);

        printf("[%d] terminted\n", pid);

        return EXIT_SUCCESS;
}
```