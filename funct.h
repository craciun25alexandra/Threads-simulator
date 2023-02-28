/* CRACIUN Alexandra-Georgiana -311CB */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _Node
{
    void* value;
    struct _Node* next;
}Node;

typedef struct{
    int ThId;
} Thread;

typedef struct{
    int TaskId;
    Thread th;
    int priority;
    int time; //initial timpul exec, apoi timpul ramas dupa run
    int exec; //timpul executat de care are nevoie
} Task;

typedef struct _Stack
{
    Node* top;
    Node* bottom;
    int size;
}Stack;

typedef struct _Queue
{
    Node* front;
    Node* back;
    int size;
}Queue;

void nimic (void *c){
    ;
}
Node* CreateNode(Node* next, void* value)
{
    Node* newNode = calloc(sizeof(struct _Node), 1);
    newNode->next = next;
    newNode->value = value;
    return newNode;
}

Queue* CreateQueue()
{
    Queue* q = calloc(sizeof(struct _Queue),1);
    q->back = q->front = NULL;
    q->size = 0;
    return q;
}

Stack* CreateStack()
{
    Stack* stack = calloc(sizeof(struct _Stack),1);
    stack->bottom = stack->top = NULL;
    stack->size = 0;
    return stack;
}
void dequeue(Queue* q, void (*freeEL)(void*))
{
    if( q->size == 1 )
    {
        freeEL(q->front->value);
        q->front = q->back = NULL;
        q->size = 0;
        return;
    }

    Node* frontNode = q->front;
    q->front = frontNode->next;

    freeEL(frontNode->value);
    free(frontNode);
    q->size--;
}


void enqueue(Queue* q, void* value)
{
    if( q->back == NULL )
    {
        q->back = q->front = CreateNode(NULL, value);
        q->size++;
        return;
    }

    Node* n = CreateNode(NULL, value);
    q->back->next = n;
    q->back = n;
    q->size++;
}
//copiaza coada q1 in coada q2
void CopyQueues(Queue* q1, Queue* q2){

    while(q1->size != 0){
        
        enqueue(q2, q1->front->value);
        dequeue(q1,nimic);
    }
}
//adauga un element in stiva
void push(Stack* stack, void* value)
{
    if( stack->size == 0 )
    {
        stack->bottom = stack->top = CreateNode(NULL, value);
        stack->size++;
        return;
    }

    Node* n = CreateNode(NULL, value);
    stack->top->next = n;
    stack->top = n;
    stack->size++;
}
//scoate un element din stiva
void pop(Stack* stack, void (*freeEl)(void*))
{
    if( stack->size == 1 )
    {
        freeEl(stack->bottom->value);
        free(stack->bottom);
        stack->bottom = stack->top = NULL;
        stack->size = 0;
        return;
    }

    Node* n = stack->bottom;
    while( n != NULL && n->next != stack->top )
        n =  n->next;

    Node* topNode = stack->top;
    stack->top = n;

    freeEl(topNode->value);
    free(topNode);
    stack->size--;
}

Stack* CreatePoolTh(int n) //creeaza poolul de threaduri de n elemente
{
    Stack *stack = CreateStack();
    for(int i = n-1; i>=0; i--)
        {   Thread *th= calloc(sizeof(Thread),1);
            th->ThId = i;
            push(stack, (void*)th);
        }
    return stack;
}

int comparePriority(void* t, void* n){

    Task* a =  (Task*)t;
    Task* b =  (Task*)n;
    if (a->priority > b->priority) return 1;
        else if (a->priority == b->priority) return 0;
    return -1;
}

int compareTime(void* t, void* n){ //compara timpul ramas

    Task* a =  (Task*)t;
    Task* b =  (Task*)n;
    if (a->time > b->time) return 1;
        else if (a->time == b->time) return 0;
    return -1;
}
int compareTime2(void* t, void* n){ //compara timpul executat

    Task* a =  (Task*)t;
    Task* b =  (Task*)n;
    if (a->exec > b->exec) return 1;
        else if (a->exec == b->exec) return 0;
    return -1;
}

int compareId(void* t, void* n){

    Task* a =  (Task*)t;
    Task* b =  (Task*)n;
    if (a->TaskId > b->TaskId) return 1;
        else if (a->TaskId == b->TaskId) return 0;
    return -1;
}
void freeStack(Stack** stack, void (*freeEl)(void*))
{
    while( (*stack)->size!=0 )
        pop(*stack, freeEl);
    *stack = NULL;
}

void freeQueue(Queue** q, void (*freeEL)(void*))
{
    while( (*q)->size!=0 )
        dequeue(*q, freeEL);
    *q = NULL;
}

void freeEl( void *t){

    Task *a = (Task*)t;
    free(a);
}

void PrintWaitQueue( Queue* q, FILE *f2){

    fprintf(f2,"====== Waiting queue =======\n[");
    if(q->size == 0) fprintf(f2, "]\n");
    else{
        Node *n = q->front;
        Queue *aux = CreateQueue();  
        while(n!=q->back){

        Task* a = (Task*)n->value;
        fprintf(f2,"(%d: priority = %d, remaining_time = %d),\n",
    a->TaskId, a->priority, a->time);
        enqueue(aux, n->value); //muta elementul printat in copie
        dequeue(q, nimic); //elimina primul element din coada pt 
        n = q->front;       //a trece la urmatorul
    }
        //pentru ultimul element, care trebuie marcat cu "]" la final
        Task* a = (Task*)n->value;
        fprintf(f2,"(%d: priority = %d, remaining_time = %d)]\n",
    a->TaskId, a->priority, a->time);
        enqueue(aux,n->value);
        dequeue(q,nimic);
        CopyQueues(aux,q);//restituie coada initiala
        aux=NULL;
    }
}

void PrintRunQueue( Queue* q, FILE *f2){

    
    fprintf(f2,"====== Running in parallel =======\n[");
    if(q->size == 0) fprintf(f2,"]\n");
    else{
    Node *n = q->front;
    Queue *aux = CreateQueue();
    while(n!=q->back){

        enqueue(aux, n->value);
        Task* a = (Task*)n->value;
        fprintf(f2,
    "(%d: priority = %d, remaining_time = %d, running_thread = %d),\n",
    a->TaskId, a->priority, a->time, a->th.ThId);
        dequeue(q, nimic);
        n = q->front;
    }
        Task* a = (Task*)n->value;
        fprintf(f2,
    "(%d: priority = %d, remaining_time = %d, running_thread = %d)]\n",
    a->TaskId, a->priority, a->time,  a->th.ThId);
        enqueue(aux,n->value);
        dequeue(q,nimic);
        CopyQueues(aux,q);
        aux=NULL;
  
    }
}
void PrintFinishQueue( Queue* q, FILE *f2){

    
    fprintf(f2,"====== Finished queue =======\n[");
    if(q->size == 0) fprintf(f2, "]\n");
    else{
    Node *n = q->front;
    Queue *aux = CreateQueue();
    while(n!=q->back){

        enqueue(aux, n->value);
        Task* a = (Task*)n->value;
        fprintf(f2,"(%d: priority = %d, executed_time = %d),\n",
    a->TaskId, a->priority, a->exec);
        dequeue(q, nimic);
        n = q->front;
    }
        Task* a = (Task*)n->value;
        fprintf(f2,"(%d: priority = %d, executed_time = %d)]\n",
    a->TaskId, a->priority, a->exec);
        enqueue(aux,n->value);
        dequeue(q,nimic);
        CopyQueues(aux,q);
        aux=NULL;
    }
}
//functie care casteaza la Task si compara id urile
int FindTask(void *a, void *b){

    Task* A =(Task*) a;
    int B = *(int*)b;
    if(A->TaskId == B) return 1;
    return 0;
}

int Tasktime(void* a){//returneaza timpul

    Task *A = (Task*)a;
    return A->time;
}
int Taskexec(void* a){

    Task *A = (Task*)a;
    return A->exec;
}
//functie care casteaza la Task si compara id urile threadurilor
int FindThread (void *a, void*b){

    Task *A = (Task*) a;
    int B = *(int*)b;
    if (A->th.ThId==B) return 1;
    return 0;
}

typedef struct{

    int IdTaskFound; //Id ul taskului gasit
    int ItsTime; //timpul taskului gasit
}ThreadFound;

//functia primeste un void* si returneaza un task cu informatiile date
ThreadFound* TF(void* a){

    ThreadFound *f = calloc(1, sizeof(ThreadFound));
    Task *t = (Task*) a;
    f->IdTaskFound = t->TaskId;
    f->ItsTime = t->time;
    return f; 
}
//copiaza stiva s1 in stiva s2
void CopyStacks(Stack* s1, Stack* s2){

    while(s1->size != 0){
        
        push(s2, s1->top->value);
        pop(s1,nimic);
    }
}

//functia cauta taskul in coada q si returneaza timpul sau ramas in cazul
//gasirii, respectiv -1 in caz contrar
int FindTaskInQueue(Queue* q, int (*compare1)(void*, void*), int IdOut,
                    int (*f2)(void*)){

    Node *n = q->front;//incepe de la primul element din coada
    int val =-1, ok =1;
    Queue *aux = CreateQueue();//coada auxiliara pt restabilirea cozii date
    while(n!=NULL && ok){
        
        enqueue(aux,n->value); //adauga val in auxiliar
        if (compare1(n->value, (void*)&IdOut) != 0)//compara idurile taskurilor
                {
                    val = f2(n->value); //returneaza timpul
                    ok=0;} //opreste cautarea

        dequeue(q, nimic); //elimina elementul verificat
        n= q->front;       //trece la urmatorul   
        }
    CopyQueues(q,aux);  //copiaza posibilele elemente ramase in coada data
    CopyQueues(aux, q); //restabileste coada
    aux=NULL;
    return val;
}

//functia cauta in coada threadul dat si returneaza o variabila de tip
//ThreadFound, ce contine timpul si id ul, ambele informatii fiind necesare
ThreadFound* FindThreadInQueue(Queue*q, int (*compare1)(void*, void*),
                int IdOut, ThreadFound* (*f2)(void*)){

    Node *n = q->front;
    int ok =1;
    ThreadFound *val;
    Queue *aux = CreateQueue();
    while(n!=NULL && ok){
        
        enqueue(aux,n->value);
        if (compare1(n->value, (void*)&IdOut) != 0) {
            val = f2(n->value);
            ok = 0;}
        dequeue(q, nimic);
        n= q->front;
    }
    CopyQueues(q,aux);
    CopyQueues(aux, q);
    aux=NULL;
    return val;
}
int CompareIds(void*a, void*b){

    int A = *(int*)a;
    int B = *(int*)b;
    if(A>B) return 1;
    else return -1;

}
int ComparePool(void*a, void*b){

    Thread* A = (Thread*) a;
    int B =*(int*)b;
    if (A->ThId > B) return 1;
    else return -1;
}

//functia Pushorder este destinata stivei de Id uri, ce baga in ordine
//crescatoare atat threadurile noi, cat si pe cele eliberate 
void PushOrder(Stack* s, void*info, int(*compare)(void*, void*)){

    if(s->size == 0){ //daca stiva e goala, se creeaza primul element
        s->bottom = s->top = CreateNode(NULL, info);
        s->size++;
    }
    else {
        if(compare(info, s->top->value)<0) //daca este cel mai mic,
        {                                  // este pus primul
            push(s,info);
            return;}
        Stack *aux = CreateStack(); //stiva auxiliara; prima se distruge
        Node * n = s->top; //cauta pozitia
        while( n && compare(info, n->value)>0){ //cat trebuie mers in adancime
            push(aux, n->value);
            pop(s, nimic);
            n=s->top;
        }
        push(aux, info); //adauga informatia
        while(s->size>0){ //copiaza eventualele elemente ramase in stiva data
            push(aux, s->top->value);
            pop(s, nimic);
        }
        CopyStacks(aux, s); //restabileste stiva
        aux=NULL;
    }
}
//functia adauga in coada in oridinea prioritatii, timpului si al id ului
void AddQueue(void* t, Queue* q, int (*compare1)(void*, void*),
  int (*compare2)(void*, void*), int (*compare3)(void*, void*)){

    if( q->size == 0 ) { //daca stiva e goala
        q->back = q->front = CreateNode(NULL, t);
        q->size++;// creez primul element cu informatia t
        
    }
    else{
        // daca trebuie pus la inceputul cozii
        if(compare1 (t, q->front->value) > 0 ||
    compare1 (t, q->front->value) == 0 && compare2 (t, q->front->value) < 0 || 
    compare1 (t, q->front->value) == 0 && compare2 (t, q->front->value) == 0 &&
    compare3(t, q->front->value) < 0){

            Queue*aux = CreateQueue();
            enqueue(aux, t); // adauga elementul in aux
            CopyQueues(q, aux); // le copiaza pe celelalte
            CopyQueues(aux,q); //restabileste coada cu valoarea t
            aux=NULL;           //pe prima pozitie
            return;

        }
        //daca trebuie pus pe ultima pozitie
        if(compare1 (t, q->back->value) < 0 || compare1(t, q->back->value) == 0
        && compare2 (t, q->back->value) > 0 || compare1(t, q->back->value) == 0
        && compare2 (t, q->back->value) == 0 && compare3(t, q->back->value) > 0){

            enqueue(q, t);

            return;
        }
        // trebuie cautata pozitia in coada
        Node *n = q->front; 
        Queue*aux = CreateQueue();
        while(compare1 (t, n->value) < 0 || compare1 (t, n->value) == 0 &&
        compare2 (t, n->value) > 0 || compare1 (t, n->value) == 0 && 
        compare2 (t, n->value) == 0 && compare3(t, n->value) > 0)
        {
            enqueue(aux,n->value); //se salveaza in aux elementele iterate
            dequeue(q, nimic); //se trece la elem urmator
            n = q->front;
        }
        enqueue(aux, t); //se pune elementul
        CopyQueues(q,aux); //se copiaza elementele ramase
        CopyQueues(aux, q); //se restabileste coada
        aux=NULL;

    }
}    
void RunComand(Queue *RunQ, Queue *FinishQ, Stack * pool, Stack *IdAvailable,
                int val){

    Node * m = RunQ->front; //primul element
    Queue *aux = CreateQueue();
    while(m != NULL){
        Task*a = (Task*)(m->value);
        if(a->time > val) {
            a->time -= val; //scade timpul val din timpul taskului
            enqueue(aux, m->value);}
    else {
        a->time = 0; //s a terminat timpul
        enqueue(FinishQ, m->value); //se adauga in coada de finish
        PushOrder(IdAvailable, &a->TaskId, CompareIds); //elibereaza id ul
        push(pool, &a->th); //elibereaza threadul
    }
    dequeue(RunQ, nimic); // trece la elementul urmator din coada
    m=RunQ->front;}
    CopyQueues(aux, RunQ); //restabileste coada de running
    aux=NULL;
}
//functia gaseste timpul maxim din coada q
int FindVal2(Queue *q){
    
    int val = 0;
    Queue *aux = CreateQueue();
    Node *m = q->front;
    while(m){
        if (((Task*)m->value)-> time > val) 
        val = ((Task*)m->value)-> time;
        enqueue(aux, m->value);
        dequeue(q, nimic);
        m = q->front;
    }
    CopyQueues(aux,q);
    aux=NULL;
    return val;
}

//functia gaseste timpul executat maxim din coada q
int FindVal(Queue *q){

    int val = 0;
    Queue *aux = CreateQueue();
    Node *m = q->front;
    while(m){
        if (((Task*)m->value)-> exec > val) 
        val = ((Task*)m->value)-> exec;
        enqueue(aux, m->value);
        dequeue(q, nimic);
        m = q->front;
    }
    CopyQueues(aux,q);
    aux=NULL;
    return val;
}