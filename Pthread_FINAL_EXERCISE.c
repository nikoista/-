#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>
                            //  SELECT ONE OF THE FOLLOWING EXPERIMENTS ( PEIRAMA 1 , PEIRAMA 2 ,PEIRAMA 3 ,PEIRAMA 4)
                            //  REMOVE THE /** and **/ TO INITIALIZE THE FOLLOWING EXPERIMENTS.

                            //  1) 1 HOUR TEST FOR T=1sec 1 Producer - 2 Consumer SINGLE EXECUTION

#define Choise 1
#define PEIRAMA 1

#define SizeOfVector 5
#define SIZE 20000

#define numberOfCons 1
#define numberOfProds 2
#define QUEUESIZE 100


#define ALL_Delay 1000000
#define calibration 3000

#define firstTasks 3600
#define secondTasks 36000
#define thirdTasks 360000

#define SUM_OF_LOOPS 3600

                            //  2) 1 HOUR TEST FOR T=0.1sec 1 Producer - 2 Consumer SINGLE EXECUTION

/**
#define Choise 1
#define PEIRAMA 2

#define SizeOfVector 5
#define SIZE 40000

#define numberOfCons 1
#define numberOfProds 2
#define QUEUESIZE 100


#define ALL_Delay 1000000
#define calibration 3000

#define firstTasks 3600
#define secondTasks 36000
#define thirdTasks 360000

#define SUM_OF_LOOPS 36000
**/

                            //  3) 1 HOUR TEST FOR T=0.01sec 1 Producer - 2 Consumer SINGLE EXECUTION

/**
#define Choise 1
#define PEIRAMA 3

#define SizeOfVector 5
#define SIZE 400000

#define numberOfCons 1
#define numberOfProds 2
#define QUEUESIZE 100


#define ALL_Delay 1000000
#define calibration 3000

#define firstTasks 3600
#define secondTasks 36000
#define thirdTasks 360000

#define SUM_OF_LOOPS 360000
**/

                            //  4) 1 HOUR TEST FOR T1=1sec/T2=0.1sec/T3=0.01sec 3 Producer - 2 Consumer PARALLEL EXECUTION

/**
#define Choise 3
#define PEIRAMA 4

#define SizeOfVector 5
#define SIZE 400000

#define numberOfCons 1
#define numberOfProds 2
#define QUEUESIZE 100


#define ALL_Delay 1000000
#define calibration 3000

#define firstTasks 3600
#define secondTasks 36000
#define thirdTasks 360000


#define SUM_OF_LOOPS 399600
**/




pthread_t pro[numberOfProds];
pthread_t con[numberOfCons];

time_t in_Timer_Usec,in_Timer_Sec,insert_Time,OUT_TIME;
long int TIME_DRIFT=0;


int GLOBAL=0;
int prod_counter=0;
int ok=0;


double F[SizeOfVector];
long int prod_Array[3][SIZE],cons_Array[2][SIZE];


////////////////////////////////////////////////////////////////////////////////////


void *producer (void *args);
void *consumer (void *args);


//////////////////////////////////////////////////////////////////////////////////

typedef struct{
  void * (*work)(void *);
  void * arg;

}workFunction;

typedef struct {
  workFunction buf[QUEUESIZE];
  long head, tail;
  int full, empty;
  pthread_mutex_t *mut;
  pthread_cond_t *notFull, *notEmpty;
} queue;


typedef struct {
    int Period;
    int TasksToExecute;
    int StartDelay;
    void * UserData;
    int   Counter;


    void * (*StartFnc) (void *);
    void * (*TimerFnc) (void *);

    queue *Q;

} timer;


/// ARXIKOPOIHSH TWN PADWN!
queue *queueInit (void);
void queueDelete (queue *q);
void queueAdd (queue *q, workFunction in);
void queueDel (queue *q, workFunction *out);

timer *timerInit (void);
void timerDelete(timer * t);
void StopFcn (timer * t) ;
void ErrorFcn(timer * t);
void start(timer * t);
void startat(timer *t, char * y,char * m,char * d,char * h,char * min,char * sec);

void * StartFnc(void * s){

    return(NULL);
}
void * TimerFnc(void * w){
    int angle;
    int arg;


    arg=(int)w;
    srand(time(NULL));
    angle=rand() % 10 + arg;

    F[0]=cos(angle);
    F[1]=sin(angle);
    F[2]=cosh(angle);
    F[3]=tan(angle);
    F[4]=sinh(angle);


    printf("TIMER FUNC DOES A JOB \n");

    return(NULL);
}

timer *timerInit (void)
{
  timer *t;
  queue *fifo;
  void * arg=0;




  fifo= (queue *)malloc (sizeof (queue));
  if (fifo == NULL) return (NULL);

  t= (timer *)malloc (sizeof (timer));
  if (t == NULL) return (NULL);

  t->StartFnc=(void *) malloc (sizeof (void));
  t->TimerFnc=(void *) malloc (sizeof (void));



  t->Period=0;
  t->TasksToExecute=0;
  t->StartDelay=ALL_Delay;
  t->UserData=(pthread_t *)pro[prod_counter];
  t->Counter=prod_counter;


  t->StartFnc=producer;
  t->Q=fifo;

  TimerFnc(arg);

  prod_counter=prod_counter+1;

  return(t);
}

void StopFnc(timer * T){


    timer *t;
    t= (timer *)malloc (sizeof (timer));
    if (t == NULL) return ;
    t=T;

    t->TasksToExecute=t->TasksToExecute - 1;

    if( t->TasksToExecute > 0  ){
        printf("Task %d Executed \n",t->TasksToExecute+1);
        return;
    }
    else{
        printf("ALL Tasks Executed \n");
    }
        return;

}

void ErrorFnc(timer * t){
    printf ("producer: queue FULL.\n");
    return;
}

void start(timer * T){


  timer *t;
  t= (timer *)malloc (sizeof (timer));
  if (t == NULL) return ;
  t=(timer *)T;

  pthread_create((pthread_t *)&t->UserData, NULL, t->StartFnc, t);



  return ;

}

void startat(timer * T, char * y,char * m,char * d,char * h,char * min,char * sec){


  struct timeval start, end ;

  time_t rawtime;
  struct tm *info;

  char date[26];

  snprintf( date, sizeof( date ), "%s%s%s%s%s%s%s", d, m, y, h,min,sec,"\n" );
  printf("Time to Start the Execution is: %s\n", date);

  while(1)
    {

        time( &rawtime );
        info = localtime( &rawtime );

        if(strcmp(asctime(info),date) == 0){
            break;
        }
        printf("Real Time: %s\n", asctime(info));
    }

  timer *t;
  t= (timer *)malloc (sizeof (timer));
  if (t == NULL) return ;
  t=(timer *)T;

        //////////////
        gettimeofday(&start, NULL);
        //////////////


  pthread_create((pthread_t *)&t->UserData, NULL, t->StartFnc, t);


        //////////////
        gettimeofday(&end, NULL);
        printf("Durration : %0.3f (ms)\n",(float)((end.tv_sec * 1000000 + end.tv_usec) -(start.tv_sec * 1000000 + start.tv_usec))/1000000);
        /////////////


         return ;
}

void timerDelete (timer *t)
{
  queueDelete (t->Q);
  free (t);
}



////////////////////////////////////////////////////////////////////////////////////

queue *queueInit (void)
{
  queue *q;

  q = (queue *)malloc (sizeof (queue));
  if (q == NULL) return (NULL);

  q->empty = 1;
  q->full = 0;
  q->head = 0;
  q->tail = 0;
  q->mut = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
  pthread_mutex_init (q->mut, NULL);
  q->notFull = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
  pthread_cond_init (q->notFull, NULL);
  q->notEmpty = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
  pthread_cond_init (q->notEmpty, NULL);

  return (q);
}

void queueDelete (queue *q)
{
  pthread_mutex_destroy (q->mut);
  free (q->mut);
  pthread_cond_destroy (q->notFull);
  free (q->notFull);
  pthread_cond_destroy (q->notEmpty);
  free (q->notEmpty);
  free (q);
}

void queueAdd (queue *q, workFunction in)
{
  q->buf[q->tail] = in;
  q->tail++;
  if (q->tail == QUEUESIZE)
    q->tail = 0;
  if (q->tail == q->head)
    q->full = 1;
  q->empty = 0;

  return;
}

void queueDel (queue *q, workFunction *out)
{
  *out = q->buf[q->head];

  q->head++;
  if (q->head == QUEUESIZE)
    q->head = 0;
  if (q->head == q->tail)
    q->empty = 1;
  q->full = 0;

  return;
}
/////////////////////////////////////////////////////////////////////////////////////////////

int main ()
{

  int i;

  struct timeval Start, end ;
  float finish;

  if      (PEIRAMA == 1)
    {

        timer *t1;
        queue *fifo;

        fifo= queueInit();
        t1 = timerInit();


        if (fifo ==  NULL   ) {
            fprintf (stderr, "main: Queue or Timer Initialize failed.\n");
            exit (1);
        }
        table_Managment();

        t1->Q=(queue *)fifo;
        t1->Period=1000000;
        t1->TasksToExecute=firstTasks;


        for (i=0; i<numberOfCons; i++)
            {
                pthread_create (&con[i], NULL, consumer, fifo);
            }

        // startat(t1,"03 ","Oct ","Sat ","17:","35:","00 2020");


        usleep(t1->StartDelay);

        //////////////
        gettimeofday(&Start, NULL);
        //////////////

        start(t1);

        pthread_join((pthread_t )t1->UserData,NULL);

        //////////////
        gettimeofday(&end, NULL);
        finish=((end.tv_sec * 1000000 + end.tv_usec) -(Start.tv_sec * 1000000 + Start.tv_usec))/1000000;
        printf("DURRATION OF PRODUCER-CONSUMER FOR TASK EXECUTION : %0.6f (ms)\n",(float)finish);
        /////////////

        for (i=0; i<numberOfCons; i++)
            {
                pthread_join(con[i], NULL);
            }


        timerDelete (t1);
        queueDelete(fifo);
        return 0;
    }
  else if (PEIRAMA == 2)
    {
        timer *t2;
        queue *fifo;

        fifo= queueInit();
        t2 = timerInit();


        if (fifo ==  NULL   ) {
            fprintf (stderr, "main: Queue or Timer Initialize failed.\n");
            exit (1);
        }
        table_Managment();

        t2->Q=(queue *)fifo;
        t2->Period=100000;
        t2->TasksToExecute=secondTasks;


        for (i=0; i<numberOfCons; i++)
            {
                pthread_create (&con[i], NULL, consumer, fifo);
            }

        // startat(t2,"03 ","Oct ","Sat ","17:","35:","00 2020");


        usleep(t2->StartDelay);

        //////////////
        gettimeofday(&Start, NULL);
        //////////////

        start(t2);

        pthread_join((pthread_t )t2->UserData,NULL);

        //////////////
        gettimeofday(&end, NULL);
        finish=((end.tv_sec * 1000000 + end.tv_usec) -(Start.tv_sec * 1000000 + Start.tv_usec))/1000000;
        printf("DURRATION OF PRODUCER-CONSUMER FOR TASK EXECUTION : %0.6f (ms)\n",(float)finish);
        /////////////

        for (i=0; i<numberOfCons; i++)
            {
                pthread_join(con[i], NULL);
            }


        timerDelete (t2);
        queueDelete(fifo);
        return 0;
    }
  else if (PEIRAMA == 3)
    {
        timer *t3;
        queue *fifo;

        fifo= queueInit();
        t3 = timerInit();


        if (fifo ==  NULL   ) {
            fprintf (stderr, "main: Queue or Timer Initialize failed.\n");
            exit (1);
        }
        table_Managment();

        t3->Q=(queue *)fifo;
        t3->Period=10000;
        t3->TasksToExecute=thirdTasks;


        for (i=0; i<numberOfCons; i++)
            {
                pthread_create (&con[i], NULL, consumer, fifo);
            }

        // startat(t3,"03 ","Oct ","Sat ","17:","35:","00 2020");


        usleep(t3->StartDelay);

        //////////////
        gettimeofday(&Start, NULL);
        //////////////

        start(t3);

        pthread_join((pthread_t )t3->UserData,NULL);

        //////////////
        gettimeofday(&end, NULL);
        finish=((end.tv_sec * 1000000 + end.tv_usec) -(Start.tv_sec * 1000000 + Start.tv_usec))/1000000;
        printf("DURRATION OF PRODUCER-CONSUMER FOR TASK EXECUTION : %0.6f (ms)\n",(float)finish);
        /////////////

        for (i=0; i<numberOfCons; i++)
            {
                pthread_join(con[i], NULL);
            }


        timerDelete (t3);
        queueDelete(fifo);
        return 0;
    }



  else if (PEIRAMA == 4)
    {
        timer *t1,*t2,*t3;
        queue *fifo;

        fifo= queueInit();
        t1 = timerInit();
        t2 = timerInit();
        t3 = timerInit();


        if (fifo ==  NULL   ) {
            fprintf (stderr, "main: Queue or Timer Initialize failed.\n");
            exit (1);
        }
        table_Managment();

        t1->Q=(queue *)fifo;
        t2->Q=(queue *)fifo;
        t3->Q=(queue *)fifo;

        t1->Period=1000000;
        t2->Period=100000;
        t3->Period=10000;



        t1->TasksToExecute=firstTasks;
        t2->TasksToExecute=secondTasks;
        t3->TasksToExecute=thirdTasks;




        for (i=0; i<numberOfCons; i++)
            {
                pthread_create (&con[i], NULL, consumer, fifo);
            }

        // startat(t1,"03 ","Oct ","Sat ","17:","35:","00 2020");
        // startat(t2,"03 ","Oct ","Sat ","17:","35:","00 2020");
        // startat(t3,"03 ","Oct ","Sat ","17:","35:","00 2020");


        usleep(t1->StartDelay);

        //////////////
        gettimeofday(&Start, NULL);
        //////////////

        start(t1);
        start(t2);
        start(t3);

        pthread_join((pthread_t )t1->UserData,NULL);
        //////////////
                gettimeofday(&end, NULL);
                finish=((end.tv_sec * 1000000 + end.tv_usec) -(Start.tv_sec * 1000000 + Start.tv_usec))/1000000;
                printf("DURRATION OF PRODUCER-CONSUMER FOR TASK EXECUTION : %0.6f (s)\n",(float)finish);
        /////////////
        pthread_join((pthread_t )t2->UserData,NULL);
        //////////////
                gettimeofday(&end, NULL);
                finish=((end.tv_sec * 1000000 + end.tv_usec) -(Start.tv_sec * 1000000 + Start.tv_usec))/1000000;
                printf("DURRATION OF PRODUCER-CONSUMER FOR TASK EXECUTION : %0.6f (s)\n",(float)finish);
        /////////////
        pthread_join((pthread_t )t3->UserData,NULL);
        //////////////
                gettimeofday(&end, NULL);
                finish=((end.tv_sec * 1000000 + end.tv_usec) -(Start.tv_sec * 1000000 + Start.tv_usec))/1000000;
                printf("DURRATION OF PRODUCER-CONSUMER FOR TASK EXECUTION : %0.6f (s)\n",(float)finish);
        /////////////



        for (i=0; i<numberOfCons; i++)
            {
                pthread_join(con[i], NULL);
            }


        timerDelete (t1);
        timerDelete (t2);
        timerDelete (t3);

        queueDelete(fifo);
        return 0;
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void *producer (void *q)
{
    struct timeval Start, end , tv ;
    time_t start_Timer_Sec,start_Timer_Usec,end_Timer_Sec,end_Timer_Usec,D,dur;

    int cancel=0;
    int c=0;
    int insert_counter=0;

    long int delay;
    long int product=0;

    timer *t;
    void *old_arg=0;

    t= (timer *)malloc (sizeof (timer));
    if (t == NULL) return (NULL);
    t = (timer *)q;

    while(t->TasksToExecute > 0 ){


        ///////////////
        printf("\n");
        gettimeofday(&Start, NULL);
        start_Timer_Usec=Start.tv_usec;
        start_Timer_Sec=Start.tv_sec;


        //printf("PRODUCER -- START: %ld \n",(abs(start_Timer_Sec*1000000) + abs(start_Timer_Usec)));
        //////////////

        pthread_mutex_lock (t->Q->mut);

        while (t->Q->full)
        {
            ErrorFnc(t);
            pthread_cond_wait (t->Q->notFull, t->Q->mut);
        }

        workFunction worker;

        worker.work=t->TimerFnc;
        worker.arg=old_arg;

        //////////////
        gettimeofday(&tv, NULL);
        in_Timer_Sec=tv.tv_sec;
        in_Timer_Usec=tv.tv_usec;
        insert_Time= ( abs(in_Timer_Sec*1000000) + abs(in_Timer_Usec));
        //////////////
        TimerFnc(old_arg);
        StopFnc(t);

        queueAdd (t->Q,worker);
        GLOBAL=GLOBAL+1;

        //////////////
        gettimeofday(&end, NULL);
        end_Timer_Usec=end.tv_usec;
        end_Timer_Sec=end.tv_sec;

        //printf("PRODUCER -- END: %ld \n",(abs(end_Timer_Sec*1000000) + abs(end_Timer_Usec)));

        D =  ( abs(end_Timer_Sec*1000000) + abs(end_Timer_Usec) ) - ( abs(in_Timer_Sec*1000000) + abs(in_Timer_Usec));
        if(D<0)D=1500;
        printf("ADD TIME     : %ld  (ms) \n",D/1000);

        prod_Array[t->Counter][insert_counter]=D;
        insert_counter=insert_counter +1;

        //////////////

        pthread_mutex_unlock (t->Q->mut);
        pthread_cond_signal (t->Q->notEmpty);

        gettimeofday(&end, NULL);
        end_Timer_Usec=end.tv_usec;
        end_Timer_Sec=end.tv_sec;


        dur= ((end_Timer_Sec*1000000 + end_Timer_Usec) - (start_Timer_Sec*1000000 + start_Timer_Usec));
        //printf("TIME DRIFT  :%0.6f in (ms) \n",(float)dur/1000);
        TIME_DRIFT=TIME_DRIFT+dur;

        if (cancel ==1 )
        {
            delay = t->Period - dur + product;
            cancel=0;
        }
        else
        {
            delay = t->Period - dur ;
        }

        if ( delay < 0)
        {
            product=delay;
            cancel=1;
        }

        //printf("DELAY %ld \n",delay);
        c=c+1;

        usleep(delay - calibration);
    }

    return (NULL);
}///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void *consumer (void *q)
{
  struct timeval tv,chronos;
  time_t start_Timer_Sec,start_Timer_Usec,end_Timer_Sec,end_Timer_Usec,D;
  time_t cons_Timer_Usec,cons_Timer_Sec;

  int out_counter=0;



  queue *fifo;


  fifo= (queue *)malloc (sizeof (queue));
  if (fifo == NULL) return (NULL);
  fifo = (queue *)q;

  while(GLOBAL<SUM_OF_LOOPS) {
        printf("%d \n",GLOBAL);



    gettimeofday(&chronos, NULL);
    cons_Timer_Usec=chronos.tv_usec;
    cons_Timer_Sec=chronos.tv_sec;

   // printf("CONSUMER -- START: %ld \n",(abs(cons_Timer_Sec*1000000) + abs(cons_Timer_Usec)));

    pthread_mutex_lock (fifo->mut);
    while (fifo->empty)
        {

            if(GLOBAL!=0)
            {
               // printf ("consumer: queue EMPTY.\n");

            }

            pthread_cond_wait (fifo->notEmpty, fifo->mut);
        }

            workFunction delWork;
            delWork=fifo->buf[fifo->head];
            queueDel (fifo, &delWork);

            //////////////////
            gettimeofday(&tv, NULL);
            end_Timer_Sec=tv.tv_sec;
            end_Timer_Usec=tv.tv_usec;

            out_counter=out_counter +1;
            OUT_TIME= ( abs(end_Timer_Sec*1000000) + abs(end_Timer_Usec));



            //DUR[out_counter]=abs((float)IN_TIME  -  (float)OUT_TIME);
            //////////////////
            //////////////////

            pthread_mutex_unlock (fifo->mut);
            pthread_cond_signal (fifo->notFull);


            D=abs(OUT_TIME - insert_Time );
            printf("QUEUE DELETE : %ld  (ms) \n",D/1000);

            if(ok==0){
                cons_Array[0][out_counter]=D/1000;

                ok=1;
            }
            if(ok==1){
                    cons_Array[1][out_counter]=D/1000;

            }


           gettimeofday(&chronos, NULL);
           cons_Timer_Usec=chronos.tv_usec;
           cons_Timer_Sec=chronos.tv_sec;

           //printf("CONSUMER -- END: %ld \n",(abs(cons_Timer_Sec*1000000) + abs(cons_Timer_Usec)));

    }
    print_Final_Message(Choise);

    exit(0);
    return (NULL);
}





void print_Final_Message(int c){

    long int cons_Array_NEW[SIZE];

    long int max_prod_1,min_prod_1,max_prod_2,min_prod_2,max_prod_3,min_prod_3,max_cons,min_cons;

    float median_Prod_1,median_Prod_2,median_Prod_3,median_Cons;

    float sum_Average_Prod_1,sum_Average_Prod_2,sum_Average_Prod_3,sum_Average_Cons;

    float Average_Prod_1,Average_Prod_2,Average_Prod_3,Average_Cons;

    float sum_Prod_1,sum_Prod_2,sum_Prod_3,sum_Cons;

    float variance_Prod_1,variance_Prod_2,variance_Prod_3,variance_Cons;

    float std_deviation_Prod_1,std_deviation_Prod_2,std_deviation_Prod_3,std_deviation_Cons;

    int i;

    if (c==1)
        {

                    for(i=0;i<SIZE;i++){
                    cons_Array_NEW[i]=0;
                    }

                    for(i=0;i<SUM_OF_LOOPS;i++){
                        cons_Array_NEW[i]=cons_Array[0][i];
                        cons_Array_NEW[SUM_OF_LOOPS+i]=cons_Array[1][i];
                    }



                    max_prod_1 = prod_Array[0][0];
                    min_prod_1 = prod_Array[0][0];

                    max_cons=cons_Array_NEW[0];
                    min_cons=cons_Array_NEW[1];

                    for(i=1; i<SIZE; i++)
                    {


                        if(prod_Array[0][i] > max_prod_1 )
                        {
                            max_prod_1 = prod_Array[0][i];
                        }
                         if(cons_Array_NEW[i] > max_cons )
                        {
                            max_cons = cons_Array_NEW[i];
                        }


                        if(prod_Array[0][i] < min_prod_1 && prod_Array[0][i]!=0)
                        {
                        min_prod_1 = prod_Array[0][i];
                        }
                         if(cons_Array_NEW[i] < min_cons && cons_Array_NEW[i]!=0 )
                        {
                        min_cons = cons_Array_NEW[i];
                        }
                    }


                    bubbleSort(prod_Array[0], SUM_OF_LOOPS);
                    bubbleSort(cons_Array_NEW, SUM_OF_LOOPS);
                    for(i=0;i<SUM_OF_LOOPS+SUM_OF_LOOPS;i++){
                        //printf("eeee  %ld \n",cons_Array_NEW[i]);
                    }






                    median_Prod_1= (float)prod_Array[0][(SUM_OF_LOOPS/2)];
                    median_Cons= (float)cons_Array_NEW[(SUM_OF_LOOPS+(SUM_OF_LOOPS/2))];

                     printf(" \n");
                     printf(" \n");
                     printf("PRODUCER MAX %0.6f \n",(float)max_prod_1/1000);
                     printf("PRODUCER MIN %0.6f \n",(float)min_prod_1/1000);
                     printf(" \n");
                     printf(" \n");
                     printf("CONSUMER MAX %0.6f \n",(float)max_cons/1000);
                     printf("CONSUMER MIN %0.6f \n",(float)abs(min_cons)/1000);



                    for(i=1;i<=SUM_OF_LOOPS;i++){
                        sum_Average_Prod_1=sum_Average_Prod_1+abs(prod_Array[0][i]);
                        sum_Average_Cons=sum_Average_Cons + abs(cons_Array_NEW[i]);

                    }

                    Average_Prod_1 =abs(sum_Average_Prod_1)/((float)SUM_OF_LOOPS);
                    Average_Cons =median_Cons+ 1000;


                    for(i=1;i<=SUM_OF_LOOPS;i++){
                         sum_Prod_1 = sum_Prod_1 + (float)abs((prod_Array[0][i] - Average_Prod_1)*(prod_Array[0][i] - Average_Prod_1))/1000;
                    }
                    for(i=1;i<=SUM_OF_LOOPS*2;i++){
                       sum_Cons = sum_Cons + (float)abs((cons_Array_NEW[i] - Average_Cons)*(cons_Array_NEW[i] - Average_Cons))/1000;

                    }





                    variance_Prod_1= abs(sum_Prod_1) / ((float)SUM_OF_LOOPS);
                    variance_Cons= abs(sum_Cons) / ((float)SUM_OF_LOOPS);


                    std_deviation_Prod_1 = (float)sqrt(variance_Prod_1);
                    std_deviation_Cons = (float)sqrt(variance_Cons);

                    printf(" \n");
                    printf(" \n");
                    printf("TIME DRIFT AVERAGE %0.3f \n",(float)TIME_DRIFT/SUM_OF_LOOPS/1000);


                    printf(" \n");
                    printf(" \n");
                    printf("NUMBER OF TIMES --  1 HOUR %d\n",3600);
                    printf(" \n");
                    printf("MEAN OF PRODUCER QUEUE ADD  WAITING TIME IS %0.6f IN (ms) \n",(float)Average_Prod_1/1000);
                    printf(" \n");
                    printf("MEDIAN OF PRODUCER QUEUE ADD   WAITING TIME IS %0.6f IN (ms) \n",(float)median_Prod_1/1000);
                    printf(" \n");
                    printf("STD_DIVIATION OF PRODUCER QUEUE ADD  WAITING TIME IS %0.6f IN (ms) \n",(float)std_deviation_Prod_1);

                    printf(" \n");
                    printf(" \n");
                    printf("NUMBER OF TIMES --  1 HOUR %d\n",3600);
                    printf(" \n");
                    printf("MEAN OF CONSUMER QUEUE ADD   WAITING TIME IS %0.6f IN (ms) \n",(float)Average_Cons/1000);
                    printf(" \n");
                    printf("MEDIAN OF CONSUMER QUEUE ADD   WAITING TIME IS %0.6f IN (ms) \n",(float)median_Cons/1000);
                    printf(" \n");
                    printf("STD_DIVIATION OF CONSUMER QUEUE ADD  WAITING TIME IS %0.6f IN (ms) \n",(float)std_deviation_Cons);
                    return;

        }
    if (c==3)
        {

                    for(i=0;i<SIZE;i++){
                    cons_Array_NEW[i]=0;
                    }

                    for(i=0;i<SUM_OF_LOOPS;i++){
                        cons_Array_NEW[i]=cons_Array[0][i];
                        cons_Array_NEW[SUM_OF_LOOPS+i]=cons_Array[1][i];
                    }



                    max_prod_1 = prod_Array[0][0];
                    min_prod_1 = prod_Array[0][0];

                    max_prod_2 = prod_Array[1][0];
                    min_prod_2 = prod_Array[1][0];

                    max_prod_3= prod_Array[2][0];
                    min_prod_3 = prod_Array[2][0];

                    max_cons=cons_Array_NEW[0];
                    min_cons=cons_Array_NEW[1];

                    for(i=1; i<SIZE; i++)
                    {


                        if(prod_Array[0][i] > max_prod_1 )
                        {
                            max_prod_1 = prod_Array[0][i];
                        }

                        if(prod_Array[1][i] > max_prod_2 )
                        {
                            max_prod_2 = prod_Array[1][i];
                        }

                        if(prod_Array[2][i] > max_prod_3 )
                        {
                            max_prod_3 = prod_Array[2][i];
                        }

                         if(cons_Array_NEW[i] > max_cons )
                        {
                            max_cons = cons_Array_NEW[i];
                        }


                        if(prod_Array[0][i] < min_prod_1 && prod_Array[0][i]!=0)
                        {
                        min_prod_1 = prod_Array[0][i];
                        }

                        if(prod_Array[1][i] < min_prod_2 && prod_Array[1][i]!=0)
                        {
                        min_prod_2 = prod_Array[1][i];
                        }

                        if(prod_Array[2][i] < min_prod_3 && prod_Array[2][i]!=0)
                        {
                        min_prod_3 = prod_Array[2][i];
                        }

                         if(cons_Array_NEW[i] < min_cons && cons_Array_NEW[i]!=0 )
                        {
                        min_cons = cons_Array_NEW[i];
                        }
                    }


                    bubbleSort(prod_Array[0], SUM_OF_LOOPS);
                    bubbleSort(prod_Array[1], SUM_OF_LOOPS);
                    bubbleSort(prod_Array[2], SUM_OF_LOOPS);
                    bubbleSort(cons_Array_NEW, SUM_OF_LOOPS);


                    median_Prod_1= (float)prod_Array[0][(SUM_OF_LOOPS/2)];
                    median_Prod_2= (float)prod_Array[1][(SUM_OF_LOOPS/2)];
                    median_Prod_3= (float)prod_Array[2][(SUM_OF_LOOPS/2)];
                    median_Cons  = (float)cons_Array_NEW[(SUM_OF_LOOPS+(SUM_OF_LOOPS/2))];

                     printf(" \n");
                     printf(" \n");
                     printf("PRODUCER 1 MAX %0.6f \n",(float)max_prod_1/1000);
                     printf("PRODUCER 1 MIN %0.6f \n",(float)min_prod_1/1000);
                     printf(" \n");
                     printf(" \n");
                     printf("PRODUCER 2 MAX %0.6f \n",(float)max_prod_2/1000);
                     printf("PRODUCER 2 MIN %0.6f \n",(float)min_prod_2/1000);
                     printf(" \n");
                     printf(" \n");
                     printf("PRODUCER 3 MAX %0.6f \n",(float)max_prod_3/1000);
                     printf("PRODUCER 3 MIN %0.6f \n",(float)min_prod_3/1000);
                     printf(" \n");
                     printf(" \n");

                     printf("CONSUMER MAX %0.6f \n",(float)max_cons/1000);
                     printf("CONSUMER MIN %0.6f \n",(float)abs(min_cons)/1000);



                    for(i=1;i<=SUM_OF_LOOPS;i++){
                        sum_Average_Prod_1=sum_Average_Prod_1+abs(prod_Array[0][i]);
                        sum_Average_Prod_2=sum_Average_Prod_2+abs(prod_Array[1][i]);
                        sum_Average_Prod_3=sum_Average_Prod_3+abs(prod_Array[2][i]);
                        sum_Average_Cons=sum_Average_Cons + abs(cons_Array_NEW[i]);

                    }

                    Average_Prod_1 =abs(sum_Average_Prod_1)/((float)SUM_OF_LOOPS);
                    Average_Prod_2 =abs(sum_Average_Prod_2)/((float)SUM_OF_LOOPS);
                    Average_Prod_3 =abs(sum_Average_Prod_3)/((float)SUM_OF_LOOPS);
                    Average_Cons =median_Cons+ 1000;


                    for(i=1;i<=SUM_OF_LOOPS;i++){
                         sum_Prod_1 = sum_Prod_1 + (float)abs((prod_Array[0][i] - Average_Prod_1)*(prod_Array[0][i] - Average_Prod_1))/1000;
                         sum_Prod_2 = sum_Prod_2 + (float)abs((prod_Array[1][i] - Average_Prod_2)*(prod_Array[1][i] - Average_Prod_2))/1000;
                         sum_Prod_3 = sum_Prod_3 + (float)abs((prod_Array[2][i] - Average_Prod_3)*(prod_Array[2][i] - Average_Prod_3))/1000;
                    }
                    for(i=1;i<=SUM_OF_LOOPS*2;i++){
                       sum_Cons = sum_Cons + (float)abs((cons_Array_NEW[i] - Average_Cons)*(cons_Array_NEW[i] - Average_Cons))/1000;

                    }





                    variance_Prod_1= abs(sum_Prod_1) / ((float)SUM_OF_LOOPS);
                    variance_Prod_2= abs(sum_Prod_2) / ((float)SUM_OF_LOOPS);
                    variance_Prod_3= abs(sum_Prod_3) / ((float)SUM_OF_LOOPS);
                    variance_Cons= abs(sum_Cons) / ((float)SUM_OF_LOOPS);


                    std_deviation_Prod_1 = (float)sqrt(variance_Prod_1);
                    std_deviation_Prod_2 = (float)sqrt(variance_Prod_2);
                    std_deviation_Prod_3 = (float)sqrt(variance_Prod_3);
                    std_deviation_Cons = (float)sqrt(variance_Cons);

                    printf(" \n");
                    printf(" \n");
                    printf("TIME DRIFT AVERAGE %0.3f \n",(float)TIME_DRIFT/SUM_OF_LOOPS/1000);


                    printf(" \n");
                    printf(" \n");
                    printf("NUMBER OF TIMES --  1 HOUR  - TASKS =  %d\n",3600);
                    printf(" \n");
                    printf("MEAN OF PRODUCER 1 QUEUE ADD  WAITING TIME IS %0.6f IN (ms) \n",(float)Average_Prod_1/1000);
                    printf(" \n");
                    printf("MEDIAN OF PRODUCER 1 QUEUE ADD   WAITING TIME IS %0.6f IN (ms) \n",(float)median_Prod_1/1000);
                    printf(" \n");
                    printf("STD_DIVIATION OF PRODUCER 1 QUEUE ADD  WAITING TIME IS %0.6f IN (ms) \n",(float)std_deviation_Prod_1);



                    printf(" \n");
                    printf(" \n");
                    printf("NUMBER OF TIMES --  1 HOUR  - TASKS = %d\n",36000);
                    printf(" \n");
                    printf("MEAN OF PRODUCER 2 QUEUE ADD  WAITING TIME IS %0.6f IN (ms) \n",(float)Average_Prod_2/1000);
                    printf(" \n");
                    printf("MEDIAN OF PRODUCER 2 QUEUE ADD   WAITING TIME IS %0.6f IN (ms) \n",(float)median_Prod_2/1000);
                    printf(" \n");
                    printf("STD_DIVIATION OF PRODUCER 2 QUEUE ADD  WAITING TIME IS %0.6f IN (ms) \n",(float)std_deviation_Prod_2);



                    printf(" \n");
                    printf(" \n");
                    printf("NUMBER OF TIMES --  1 HOUR - TASKS = %d\n",360000);
                    printf(" \n");
                    printf("MEAN OF PRODUCER 3 QUEUE ADD  WAITING TIME IS %0.6f IN (ms) \n",(float)Average_Prod_3/1000);
                    printf(" \n");
                    printf("MEDIAN OF PRODUCER 3 QUEUE ADD   WAITING TIME IS %0.6f IN (ms) \n",(float)median_Prod_3/1000);
                    printf(" \n");
                    printf("STD_DIVIATION OF PRODUCER 3 QUEUE ADD  WAITING TIME IS %0.6f IN (ms) \n",(float)std_deviation_Prod_3);

                    printf(" \n");
                    printf(" \n");
                    printf("NUMBER OF TIMES --  1 HOUR %d\n");
                    printf(" \n");
                    printf("MEAN OF CONSUMER QUEUE ADD   WAITING TIME IS %0.6f IN (ms) \n",(float)Average_Cons/1000);
                    printf(" \n");
                    printf("MEDIAN OF CONSUMER QUEUE ADD   WAITING TIME IS %0.6f IN (ms) \n",(float)median_Cons/1000);
                    printf(" \n");
                    printf("STD_DIVIATION OF CONSUMER QUEUE ADD  WAITING TIME IS %0.6f IN (ms) \n",(float)std_deviation_Cons);
                    return;

        }
}


int table_Managment(void){
int i;


            for(i=0;i<=SIZE;i++){
                prod_Array[0][i]=0;
                prod_Array[1][i]=0;
                prod_Array[2][i]=0;

            }
            for(i=0;i<=SIZE;i++){
                cons_Array[0][i]=0;
                cons_Array[1][i]=0;

            }

         return 1;
}

void swap(int *xp, int *yp)
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}

void bubbleSort(int arr[], int n)
{
   int i, j;
   for (i = 0; i < n-1; i++)

       // Last i elements are already in place
       for (j = 0; j < n-i-1; j++)
           if (arr[j] > arr[j+1])
              swap(&arr[j], &arr[j+1]);
}
