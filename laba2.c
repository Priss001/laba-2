    #include <sys/types.h>
    #include <sys/wait.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <ncurses.h>
    #include <time.h>
    #include <unistd.h>
    #include <signal.h>
    #include <string.h>
    #define MAX_COUNT 200
    pid_t child[MAX_COUNT];
    int fend = 1;
    int cur = 1;
    int chlength = 0;
    int fprint = 0;

    struct sigaction printSignal, endSignal;

    void IfEnd()
    {
    if(fend)
    {
    kill(getppid(),SIGUSR2);
    exit(0);
    }
    }
    void Print(int signo)
    {
    fprint = 1;
    }
    void setEndFlag(int signo)
    {
    fend = 1;
    }

    int main(void)
    {
    initscr();
    clear();
    noecho();
    refresh();

    printSignal.sa_handler = Print;
    sigaction(SIGUSR1,&printSignal,NULL);

    endSignal.sa_handler = setEndFlag;
    sigaction(SIGUSR2,&endSignal,NULL);

    char c = 0;
    int i = 0;

    child[0]=getpid();

    while(c!='q')
    {
    switch(c=getchar())
    {
    case '+':
    {
    if(chlength < MAX_COUNT)
    {
    chlength++;
    child[chlength] = fork();
    }

    switch(child[chlength])
    {
    case 0: //child process
    {
    fend = 0;
    while(!fend)
    {
    usleep(10000);
    if(fprint)
    {
    IfEnd();
    printf("\r\n%d %d", getpid(), chlength-1);
    usleep(1000000);
    
    refresh();
    fprint = 0;
    kill(getppid(),SIGUSR2); //отправка родителю сигнал о завершении
    }
    IfEnd();
    }
    }
    break;

    case -1:
    {
    printf("Child process[%d] failed!\n",chlength);
    }
    break;
    }
    }
    break;

    case '-':
    {
    if(chlength==0)
    break;
    kill(child[chlength],SIGUSR2); //завершение последнего процесса
    waitpid(child[chlength],NULL,0);
    if(cur == chlength) cur = 1;
    chlength--;
    }
    break;
    }
    if(fend && chlength>0) //если текущий процесс закончил вывод
    {
    fend = 0;
    kill(child[cur++],SIGUSR1);
    if(cur > chlength) // если номер текущего процесса > кол-ва процессов,
    cur = 1; //стартуем заново
    }
    refresh();
    }

    //закрытие всех процессов
    if(child[chlength]!=0)
    {
    for(;chlength>0;chlength--)
    {
    kill(child[chlength],SIGUSR2);
    waitpid(child[chlength],NULL,0);
    }
    }
    clear();
    endwin();

    return 0;
    }


