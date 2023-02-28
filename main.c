/* CRACIUN Alexandra-Georgiana -311CB */

#include "funct.h"

int main(int argc, char*argv[]){

    int Q, C, nrtasks, priority, time, T=0;
    FILE *f = fopen(argv[1], "r");
    FILE *f2= fopen(argv[2],"w");
    char * cerinta = malloc(256);
    fscanf(f, "%d", &Q);
    fscanf(f, "%d", &C);
    int N = 2*C;
    int nr_id=0; //pentru a construi stiva de id uri
    char *p;
    Queue * WaitQ = CreateQueue();
    Queue * RunQ = CreateQueue();
    Queue * FinishQ = CreateQueue();
    Stack * IdAvailable = CreateStack();
    Stack* pool = CreatePoolTh(N);
    while(fgets(cerinta, 256, f)){

        //pentru a exclude endline-ul
        if(strchr(cerinta, '\r')) p = strtok(cerinta , "\r");
        //pentru a exclude caracterul de returnare
        if(strchr(cerinta, '\n')) p = strtok(cerinta , "\n");
        p = strtok(cerinta," ");
        if (strcmp(p, "add_tasks")==0){
            
            p = strtok(NULL," ");
            nrtasks = atoi(p);
            p = strtok(NULL," ");
            time = atoi(p);
            p = strtok(NULL," ");
            priority = atoi(p);
            for (int i = 1; i<= nrtasks; i++)
            {
                Task *c = malloc(sizeof(Task)); //creeaza structura de tip Task
                c->time = time;
                int *k = malloc(sizeof(int)); //pentru stiva de id uri
                *k = nr_id+1;
                c->priority = priority;
                PushOrder(IdAvailable, k, CompareIds); //pune in stiva id ul
                c->TaskId = *(int*)(IdAvailable->top->value); // asociaza id ul
                                                              // taskului
                nr_id++;    // pentru urmatorul task                             
                pop(IdAvailable, nimic); //elimina id ul asociat din tasl
                c->exec = time;
                //adauga in ordine in coada de waiting
                AddQueue(c, WaitQ, comparePriority, compareTime, compareId);
                fprintf(f2, "Task created successfully : ID %d.\n", c->TaskId);
            }
    }
    else if (strcmp(p, "print")==0){
        p = strtok(NULL," ");
        if (strcmp(p, "waiting")==0)
            PrintWaitQueue(WaitQ, f2);
        else if (strcmp(p, "running")==0) 
            PrintRunQueue(RunQ, f2);
        else 
            PrintFinishQueue(FinishQ, f2);}

    else if (strcmp(p,"get_task")==0){     
        p = strtok(NULL, " ");
        int nr = atoi(p);
        if(RunQ->size >0 && FindTaskInQueue(RunQ, FindTask, nr, Tasktime)>0)
            fprintf(f2,"Task %d is running (remaining_time = %d).\n", nr,
        FindTaskInQueue(RunQ, FindTask, nr, Tasktime));
        else if (WaitQ->size >0 && FindTaskInQueue(WaitQ, FindTask, nr,
        Tasktime)>0){
            fprintf(f2,"Task %d is waiting (remaining_time = %d).\n", nr,
        FindTaskInQueue(WaitQ, FindTask, nr, Tasktime));}
        else if (FinishQ->size>0 && FindTaskInQueue(FinishQ, FindTask, nr,
        Taskexec)!=-1) 
            fprintf(f2,"Task %d is finished (executed_time = %d).\n", nr,
        FindTaskInQueue(FinishQ, FindTask, nr, Taskexec));
        else fprintf(f2,"Task %d not found.\n",nr);
    }
    else if (strcmp(p, "get_thread")==0){
            
            p = strtok(NULL," ");
            int nr = atoi(p);
            if(nr < N) {
             if(RunQ->size ==0) //daca coada de running e goala
                fprintf(f2,"Thread %d is idle.\n", nr);
            else{
            ThreadFound* f = FindThreadInQueue(RunQ, FindThread, nr, TF);
            if(f)
            fprintf(f2,"Thread %d is running task %d (remaining_time = %d).\n",
            nr, f->IdTaskFound, f->ItsTime);
                else fprintf(f2,"Thread %d is idle.\n", nr);
            }}
        }
    else if (strcmp(p,"run")==0){
        p = strtok(NULL," ");
        int nr = atoi(p);
        fprintf(f2,"Running tasks for %d ms...\n", nr);
        int times;
        // cat timp sunt threaduri disponibile si coada de wait are elemente
        // se baga elementele in coada de running in ordinea ceruta
        while(pool->size > 0 && WaitQ ->size >0){
            Node *n = WaitQ->front;             
            Thread t = *((Thread*)(pool->top->value));
            pop(pool, nimic); // nu mai e available acest thread
            Task* a = (Task*)(n->value);
            a->th = t;// i se asociaza threadul din capul stivei
            AddQueue(n->value, RunQ, comparePriority, compareTime, compareId);
            dequeue(WaitQ, nimic);}
        // daca nr e multiplu de Q - capacitatea procesorului, run se executa
        // de nr/Q ori
        if(nr % Q == 0) times = nr/Q;
        else times = nr/Q + 1; //altfel, de nr/Q +1
        int val; //cat timp va rula un task
        if (nr > Q) val = Q; // daca timpul de run e mai mare, se ruleaza
                             // cate Q sec
        else val = nr;  //altfel, cate nr sec
        Node *m;
        int max ;
        for (int i = 1; i <= times; i++)
            {   
                // daca nu mai sunt taskuri de rulat, sau nr de run e depasit,
                // nu se mai executa nimic
                if(RunQ->size ==0 && WaitQ->size==0 || nr<=0) break;
                
                if (RunQ->size!=0){ //daca sunt elemente in coada de run
                // daca nr ramas din running este mai mic decat valoarea cu
                // care rulau taskurile, se vor rula cu nr
                    if (nr < val) val = nr;
                    else{
                        max = FindVal2(RunQ); //se ia timpul maxim din coada
                //daca maximul este mai mic decat valoarea cu care se rulau
                //taskurile, se vor rula cu max
                        if( max < val) val = max;}
                    T=T+val; //se adauga timpului total
                    nr -= val; //se actualizeaza timpul de run; se face comanda
                    RunComand (RunQ, FinishQ, pool, IdAvailable, val);}
                // se reseteaza timpul in cazul in care lista de running este
                // goala, se umple la finalul acestei iteratii, iar la
                // urmatoarea trebuie sa se execute comanda run 
                val = Q;
                //se mai pune in run cat mai sunt threaduri disponibile
                while(pool->size>0 && WaitQ->size >0){
                    Node *n = WaitQ->front;
                    Thread t = *((Thread*)(pool->top->value));
                    pop(pool, nimic);
                    Task* a = (Task*)(n->value);
                    a->th = t;
                    AddQueue(n->value, RunQ, comparePriority,
                    compareTime, compareId);
                    dequeue(WaitQ, nimic);
                }
            }
        }

    else if(strcmp(p,"finish")==0) {
        
        //se pun taskurile in coada de run
        while(WaitQ->size!=0)
        {
            int max = 0;
            while(pool->size!=0 && WaitQ->size!=0){
                
                Node *n = WaitQ->front;
                Thread t = *((Thread*)(pool->top->value));
                pop(pool, nimic);
                Task* a = (Task*)(n->value);
                a->th = t;
                if (a->exec > max) max = a->exec; //se calculeaza timpul max
                AddQueue(n->value, RunQ, comparePriority,
            compareTime, compareId);
                dequeue(WaitQ, nimic);
            }
            while(RunQ->size!=0){
                // se executa run ul cu timpul maxim
                RunComand (RunQ, FinishQ, pool, IdAvailable, max);
                T= T+max; //se adauga in timpul total
            }}
            fprintf(f2, "Total time: %d", T);}}
            fclose(f);
            fclose(f2);
            freeQueue(&RunQ,nimic);
            freeQueue(&WaitQ,nimic);
            freeQueue(&FinishQ, nimic);
            freeStack(&pool, nimic);
            freeStack(&IdAvailable, free);
            free(cerinta);
            }
          
                
