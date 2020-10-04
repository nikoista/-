/*
 *	File	: pc.c
 *
 *	Title	: Demo Producer/Consumer.
 *
 *	Short	: A solution to the producer consumer problem using
 *		pthreads.
 *
 *	Long 	:
 *
 *	Author	: Andrae Muys
 *
 *	Date	: 18 September 1997
 *
 *	Revised	:
 */

#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>

#define Choise 1
#define SizeOfVector 5
#define SizeOfA 2000
#define SizeOfB 100
#define SizeOfC 100

#define numberOfCons 2
#define numberOfProds 1
#define QUEUESIZE 50

#define period 1000000     // allazw edw
#define firstPeriod 1000000
#define secondPeriod 100000
#define thirdPeriod 10000

#define ALL_Delay 1000000
#define calibration 7000   // 7000 -- 2000 --
#define firstTasks 60
#define secondTasks 600
#define thirdTasks 6000
#define SUM_OF_LOOPS 6660




pthread_t pro[numberOfProds];
pthread_t con[numberOfCons];

long int dur=0;
int GLOBAL=0;
int prod_counter=0;

double F[SizeOfVector];
float average, variance, std_deviation;
long int prod_A[SizeOfA],cons_A[SizeOfA];
//long int prod_B[SizeOfB],cons_B[SizeOfB];
//long int prod_C[SizeOfC],cons_C[SizeOfC];
long int DUR[SizeOfC];
long int Inside_TimeA=0;
long int out_Time_A=0;
long int out_Time_B=0;
long int out_Time_C=0;
long int IN_TIME;
long int OUT_TIME;

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
  void *p;
  pthread_mutex_t *mut;
  pthread_cond_t *notFull, *notEmpty;
} queue;


typedef struct {
    int Period;
    int TasksToExecute;
    int StartDelay;
    void * UserData;
    void * ArrayData;

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

    return(NULL);
}

timer *timerInit (void)
{
  timer *t;
  queue *fifo;




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
  t->ArrayData=0;


  t->StartFnc=producer;
  t->TimerFnc=0;
  t->Q=fifo;

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
        printf("Task %d Executed \n",abs(firstTasks - t->TasksToExecute));
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

  q->p=0;
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


  table_Managment(Choise);


  timer *t1,*t2,*t3;
  queue *fifo;

  fifo= queueInit();
  t1 = timerInit();
  //t2 = timerInit();
  //t3 = timerInit();

  if (fifo ==  NULL   ) {
    fprintf (stderr, "main: Queue or Timer Initialize failed.\n");
    exit (1);
  }

  t1->Q=(queue *)fifo;
  //t2->Q=(queue *)fifo;
 // t3->Q=(queue *)fifo;

  t1->Period=period;
  //t2->Period=100000;
 // t3->Period=10000;

  t1->Q->p=&cons_A[0];
  //t2->Q->p=&cons_B[0];
 // t3->Q->p=&cons_C[0];

  t1->ArrayData=&prod_A[0];
 // t2->ArrayData=&prod_B[0];
  //t3->ArrayData=&prod_C[0];


  t1->TasksToExecute=firstTasks;
  //t2->TasksToExecute=secondTasks;
  //t3->TasksToExecute=thirdTasks;


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
  //start(t2);
  //start(t3);


  pthread_join((pthread_t )t1->UserData,NULL);


    //////////////
        gettimeofday(&end, NULL);
        finish=((end.tv_sec * 1000000 + end.tv_usec) -(Start.tv_sec * 1000000 + Start.tv_usec))/1000000;
        printf("DURRATION OF PRODUCER-CONSUMER FOR TASK EXECUTION : %0.6f (ms)\n",(float)finish);
        /////////////
/**  pthread_join((pthread_t )t2->UserData,NULL);
    //////////////
        gettimeofday(&end, NULL);
        finish=((end.tv_sec * 1000000 + end.tv_usec) -(Start.tv_sec * 1000000 + Start.tv_usec))/1000000;
        printf("Durration : %0.6f (ms)\n",(float)finish);
        /////////////
  pthread_join((pthread_t )t3->UserData,NULL);
    //////////////
        gettimeofday(&end, NULL);
        finish=((end.tv_sec * 1000000 + end.tv_usec) -(Start.tv_sec * 1000000 + Start.tv_usec))/1000000;
        printf("Durration : %0.6f (ms)\n",(float)finish);
        /////////////
**/
  print_Final_Message();


  for (i=0; i<numberOfCons; i++)
    {
        pthread_join(con[i], NULL);
    }


  timerDelete (t1);
 // timerDelete (t2);
 // timerDelete (t3);

  queueDelete(fifo);
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void *producer (void *q)
{
    struct timeval Start, end , tv ;
    time_t in_Timer_Usec,in_Timer_Sec,start_Timer_Sec,start_Timer_Usec,end_Timer_Sec,end_Timer_Usec;

    int cancel=0;
    int c=0;
    int insert_counter=0;

    long int insert_Time=0;
    long int delay;
    long int product=0;

    void *p=0;
    int *Array;
    timer *t;

    t= (timer *)malloc (sizeof (timer));
    if (t == NULL) return (NULL);
    t = (timer *)q;

    Array= (int *)malloc (sizeof (int));
    if (Array == NULL) return (NULL);
    Array=t->ArrayData;


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
        worker.arg=p;
        StopFnc(t);

        //////////////
        gettimeofday(&tv, NULL);
        in_Timer_Sec=end.tv_sec;
        in_Timer_Usec=end.tv_usec;
        //////////////

        queueAdd (t->Q,worker);
        GLOBAL=GLOBAL+1;

        //////////////
        insert_counter=insert_counter +1;
        Array[insert_counter]= (abs(in_Timer_Sec * 1000000) + abs(in_Timer_Usec));
        insert_Time=insert_Time+abs(in_Timer_Sec * 1000000) + abs(in_Timer_Usec);
        Inside_TimeA=Inside_TimeA+abs(in_Timer_Sec * 1000000) + abs(in_Timer_Usec);
        IN_TIME=abs(in_Timer_Sec * 1000000) + abs(in_Timer_Usec);
        //////////////

        pthread_mutex_unlock (t->Q->mut);
        pthread_cond_signal (t->Q->notEmpty);

        gettimeofday(&end, NULL);
        end_Timer_Usec=end.tv_usec;
        end_Timer_Sec=end.tv_sec;

        //printf("PRODUCER -- END: %ld \n",(abs(end_Timer_Sec*1000000) + abs(end_Timer_Usec)));

        dur= ((end_Timer_Sec*1000000 + end_Timer_Usec) - (start_Timer_Sec*1000000 + start_Timer_Usec));
        printf("TIME DRIFT  :%0.6f in (ms) \n",(float)dur/1000);

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
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void *consumer (void *q)
{
  struct timeval tv,chronos;
  time_t end_Timer_Usec,end_Timer_Sec;
  time_t cons_Timer_Usec,cons_Timer_Sec;

  int out_counter=0;


  int *Array;
  queue *fifo;


  fifo= (queue *)malloc (sizeof (queue));
  if (fifo == NULL) return (NULL);
  fifo = (queue *)q;

  Array= (int *)malloc (sizeof (int));
  if (Array == NULL) return (NULL);


  Array=fifo->p;


  while(GLOBAL<=SUM_OF_LOOPS) {



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
            OUT_TIME=(abs(end_Timer_Sec * 1000000) + abs(end_Timer_Usec));

            cons_A[out_counter] =  (abs(end_Timer_Sec * 1000000) + abs(end_Timer_Usec));
            //cons_B[out_counter] =  (abs(end_Timer_Sec * 1000000) + abs(end_Timer_Usec));
            //cons_C[out_counter] =  (abs(end_Timer_Sec * 1000000) + abs(end_Timer_Usec));
            out_Time_A=out_Time_A+(abs(end_Timer_Sec * 1000000) + abs(end_Timer_Usec));
            //out_Time_B=out_Time_B+(abs(end_Timer_Sec * 1000000) + abs(end_Timer_Usec));
            //out_Time_C=out_Time_C+(abs(end_Timer_Sec * 1000000) + abs(end_Timer_Usec));


            DUR[out_counter]=abs((float)IN_TIME  -  (float)OUT_TIME);
            //printf("DUR : %0.6f\n",(float)DUR[out_counter]/1000);
            //////////////////
            //////////////////

            pthread_mutex_unlock (fifo->mut);
            pthread_cond_signal (fifo->notFull);


           gettimeofday(&chronos, NULL);
           cons_Timer_Usec=chronos.tv_usec;
           cons_Timer_Sec=chronos.tv_sec;

           //printf("CONSUMER -- END: %ld \n",(abs(cons_Timer_Sec*1000000) + abs(cons_Timer_Usec)));

    }
    print_Final_Message();
    exit(0);
    return (NULL);
}





void print_Final_Message(timer *t){

    long int max,min;
    int i;
    int first=1;
    int second =2;
    int third =3;
    float average_A,average_B,average_C;
    float median_A,median_B,median_C;
    float variance_A,variance_B,variance_C;
    float std_deviation_A,std_deviation_B,std_deviation_C;
    float sum_Average_A=0;
    float sum_Average_B=0;
    float sum_Average_C=0;

    float sum_A=0;
    float sum_B=0;
    float sum_C=0;
    long int  sum_prodA=0;
    long int  sum_prodB=0;
    long int  sum_prodC=0;
    long int  sum_consA=0;
    long int  sum_consB=0;
    long int  sum_consC=0;


                    max = DUR[1];
                    min = DUR[1];
                    for(i=1; i<firstTasks/2; i++)
                    {
                        /* If current element is greater than max */
                        if(DUR[i] > max )
                        {
                            max = DUR[i];
                        }

                        /* If current element is smaller than min */
                        if(DUR[i] < min && DUR[i]!=0)
                        {
                        min = DUR[i];
                        }
                    }

                    bubbleSort(DUR, firstTasks/2);

                    median_A= (float)DUR[(firstTasks/2)];

                     printf("MAX %0.6f \n",(float)max/1000);
                     printf("MIN %0.6f \n",(float)min/1000);



                    for(i=1;i<=firstTasks/2;i++){
                        sum_Average_A=sum_Average_A+abs(DUR[i]);

                    }
                    /**for(i=1;i<=SizeOfB;i++){
                        sum_prodB=sum_prodB+abs(prod_B[i]);
                        sum_consB= sum_consB+abs(cons_B[i]);
                        printf("EEEEEEEEe %ld \n",abs(cons_B[i]));
                    }
                    for(i=1;i<=SizeOfC;i++){
                        sum_prodC=sum_prodC+abs(prod_B[i]);
                        sum_consC= sum_consC+abs(cons_B[i]);
                    }
**/
                    average_A =abs(sum_Average_A)/((float)firstTasks/2);
                    /**
                    average_B =abs((abs(sum_prodB) - abs(out_Time_B)))/(float)secondTasks;
                    average_C =abs((abs(sum_prodC) - abs(out_Time_C)))/(float)thirdTasks;
                    **/
                   /**
                   for(i=1;i<=SizeOfC;i++){
                        sum_C = sum_C + abs(pow((abs(prod_C[i]) - abs(cons_C[i]) - average_A), 2));
                    }
                     for(i=1;i<=SizeOfB;i++){
                        sum_B = sum_B + abs(pow((abs(prod_B[i]) - abs(cons_B[i]) - average_C), 2));
                    }
                    **/
                    for(i=1;i<=firstTasks/2;i++){
                         sum_A = sum_A + (float)abs((DUR[i] - average_A)*(DUR[i] - average_A))/1000;

                    }



                    variance_A = abs(sum_A) / ((float)firstTasks/2);
                    //variance_B = sum_B / (float)secondTasks;
                    //variance_C = sum_C / (float)thirdTasks;

                    std_deviation_A = (float)sqrt(variance_A);
                    //std_deviation_B = sqrt(variance_B);
                    //std_deviation_C = sqrt(variance_C);


                    printf(" \n");
                    printf(" \n");
                    printf("NUMBER OF TIMES --  1 HOUR %d\n",3600);
                    printf(" \n");
                    printf("TIME OF %d PRODUCER %ld \n",first,abs(Inside_TimeA));
                    printf(" \n");
                    printf("TIME OF CONSUMERS %ld \n",abs(out_Time_A));
                    printf(" \n");
                    printf("MEAN OF PRODUCER--CONSUMER  WAITING TIME IS %0.6f IN (ms) \n",(float)average_A/1000);
                    printf(" \n");
                    printf("MEDIAN OF PRODUCER--CONSUMER  WAITING TIME IS %0.6f IN (ms) \n",(float)median_A/1000);
                    printf(" \n");
                    printf("STD_DIVIATION OF PRODUCER--CONSUMER WAITING TIME IS %0.10f IN (ms) \n",(float)std_deviation_A);
                    return;


/**
                    printf(" \n");
                    printf(" \n");
                    printf("NUMBER OF TIMES -- %d\n",secondTasks);
                    printf(" \n");
                    printf("TIME OF %d PRODUCER %ld \n",second,abs(sum_prodB));
                    printf(" \n");
                    printf("TIME OF CONSUMERS %ld \n",out_Time_B);
                    printf(" \n");
                    printf("MEAN WAITING TIME IS %0.6f IN MICROSECONDS\n",(float)average_B/1000000);
                    printf(" \n");
                    printf("VARIANCE WAITING TIME IS %0.6f IN MILLIESECONDS \n",(float)variance_B/1000000);
                    printf(" \n");
                    printf("STD_DIVIATION WAITING TIME IS %0.6f IN MILLIESECONDS \n",(float)std_deviation_B/1000000);

                    printf(" \n");
                    printf(" \n");
                    printf("NUMBER OF TIMES -- %d\n",thirdTasks);
                    printf(" \n");
                    printf("TIME OF %d PRODUCER %ld \n",third,abs(sum_prodC));
                    printf(" \n");
                    printf("TIME OF CONSUMERS %ld \n",out_Time_C);
                    printf(" \n");
                    printf("TIME OF CONSUMERS %ld \n",sum_consC);
                    printf(" \n");
                    printf("MEAN WAITING TIME IS %0.6f IN MICROSECONDS\n",(float)average_C/1000000);
                    printf(" \n");
                    printf("VARIANCE WAITING TIME IS %0.6f IN MILLIESECONDS \n",(float)variance_C/1000000);
                    printf(" \n");
                    printf("STD_DIVIATION WAITING TIME IS %0.6f IN MILLIESECONDS \n",(float)std_deviation_C/1000000);
                    return;
**/
}
int table_Managment(int choise){
int i=0;

if(choise == 1)
    {
        for(i=0;i<=SizeOfA;i++){
            prod_A[i]=0;
            cons_A[i]=0;
        }
         return 1;
    }
    /**
else if(choise == 2)
    {
        for(i=0;i<=SizeOfB;i++){
            prod_B[i]=0;
            cons_B[i]=0;
        }
         return 1;
    }
else if(choise == 3)
    {
        for(i=0;i<=SizeOfC;i++){
            prod_C[i]=0;
            cons_C[i]=0;
        }
         return 1;
    }
    **/
else {
 return 0;
}
}
void swap(int *xp, int *yp)
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}

// A function to implement bubble sort
void bubbleSort(int arr[], int n)
{
   int i, j;
   for (i = 0; i < n-1; i++)

       // Last i elements are already in place
       for (j = 0; j < n-i-1; j++)
           if (arr[j] > arr[j+1])
              swap(&arr[j], &arr[j+1]);
}

