#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>												//waitpid()
#include <stdbool.h>
#include <time.h>
#include <sys/ipc.h>												//key_t
#include <signal.h>
#include <sys/times.h>
#include <errno.h>   
#include <limits.h>  

#define len 50
#define max_size 256

typedef struct process{
	char file_name[20];
	int priority;
	int pid;														//pid_t
	int status;									       				//0-> Ready, 1-> Running, 2-> Stopped, 3-> Exited									
}process;

typedef struct node{
	process data;													//ta data typou proccess giati tha vazw olokliro process ekei`
	struct node *prev;												//to prev typou node
	struct node *next;												//to prev typou node
}node;

void display();
void sort_array(process list[max_size], int s);

void deleteNode(node **pheadp, node *pdelete);						//gia na svinw apo to doubly linked list

void catch(int sig_out);											//gia ta sigchld
void catchSTOP(int sig_out);
void catchCONT(int sig_out);

int main(int argc, char *argv[]){
	
	FILE *file;
	
	if(argc==3){													//an plithos orismatwn xristi ==3, SJF, FCFS
		file=fopen(argv[2], "r");
	}else if(argc==4){												//RR, PRIO
		file=fopen(argv[3], "r");
	}else{
		printf("You've given invalid number of arguments\n");
		return 0;
	}
	process record[max_size];										//dimiourgw enan pinaka me structs processes

	char p[19];
	char *item;
	int i=0;
	int k;
	
	//anoigw to arxeio kai to xwrizw se processes
	while(fgets(p, 19, file)){										//xorizw tis grammes tou arxeiou se metavlites tou kathe process 
		
		//printf("Grammes arxeiou: %s", p);							//SOS: an thelw ektupswsi grammwn arxeiou
		
		item = strtok(p,"\t");
		strcpy(record[i].file_name, item);
			
		item = strtok(NULL," ");									//null, giati thelw na sunexisei apo ekei pou emine to proigoumeno token
		record[i].priority = atoi(item);
		
		i++;
	}
	

	//paw na dwsw pids se kathe proccess twra
	int random_pid;													//key_t
	
	for(k=0; k<i; k++){
		
		random_pid=rand()%999999+1;
		record[k].pid = random_pid;
	}


	//ksekinaw tous algorithmous
	printf("---------------------------------------------------------------------------\n");
	
	char *epilogi=argv[1];											//argv[1] einai to prwto orisma pou dinei o xristis, to vazw se pointer gia na dw ti
																	//algorithmo thelei
	
	char FCFS[15];													//auta gia na exw exception an dinei algorithmo pou den upostirizetai
	char fcfs[15];
	char SJF[15];
	char sjf[15];
	char RR[15];
	char rr[15];
	char PRIO[15];
	char prio[15];
	strcpy(FCFS, "FCFS");
	strcpy(SJF, "SJF");
	strcpy(RR, "RR");
	strcpy(PRIO, "PRIO");
	strcpy(fcfs, "fcfs");
	strcpy(sjf, "sjf");
	strcpy(rr, "rr");
	strcpy(prio, "prio");

	if((strcmp(epilogi, fcfs)!=0) && (strcmp(epilogi, sjf)!=0) && (strcmp(epilogi, rr)!=0) && (strcmp(epilogi, prio)!=0) && (strcmp(epilogi, FCFS)!=0) && (strcmp(epilogi, SJF)!=0) && (strcmp(epilogi, RR)!=0) && (strcmp(epilogi, PRIO)!=0)){
		printf("The programm doesnt currently work for your algorithm choice.\nPlease use FCFS, SJF, RR, or PRIO.\n");
	}else{
		printf("The algorithm you chose is compatible. Please wait for your results.\n");
	}



//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
	printf("---------------------------------------------------------------------------\n");



	//SJF - me doubly linked list
	if(strcmp(epilogi, sjf)==0 || strcmp(epilogi, SJF)==0){
		
		process sortedRecord[max_size];									//ftiaxnw neo pinaka typou struct gia na perasw ta idia me ton record[], alla na ta taskinomisw
		
		for(k=0; k<i; k++){												//pernaw ta stoixeia tou record sto sortedRecord
			sortedRecord[k]=record[k];	
		}
		
		sort_array(sortedRecord, i);									//ta taksinomw vasi priority
		
		
		//nea dipla sindedemeni lista giati ta thelw taksinomimena edw mesa
		
		node *headSorted;												//head, opou deixnei to prwto stoixeio tis listas
		node *queueSorted[i];										//ftiaxnw pointer pinaka typou node
	
		for(k=0; k<i; k++){												//ta kanw null prwta
			queueSorted[k]=NULL;
		}

		for(k=0; k<i; k++){												//desmeuw dynamika xwro
			queueSorted[k]=(node*)malloc(sizeof(node));	
		}


		for(k=0; k<i; k++){												//antistoixizw se kathe koutaki->data ena struct
			queueSorted[k]->data=sortedRecord[k];
		}


		queueSorted[0]->next=queueSorted[1];						//queue 0 prev, next ektos for
		queueSorted[0]->prev=NULL;
	
		queueSorted[i-1]->prev=queueSorted[i-2];					//queue teleutaiou prev, next ektos for, i-1 giati ksekinaw apo 0 alliws segmentation fault
		queueSorted[i-1]->next=NULL;


		headSorted=queueSorted[0];									//kefali = me queue[0]
	
		for(k=1; k<i-1; k++){											//for loop gia na dwsw sto kathena to epomeno kai to proigoumeno 
			queueSorted[k]->next=queueSorted[k+1];
			queueSorted[k]->prev=queueSorted[k-1];
		}
		
		printf("Before the algorithm starts running (it's sorted): \n");
		display(queueSorted[0]);
		printf("\n");
		
		
		//pame twra gia thn ulopoihsh algorithmou 
		printf("\nLet's start: \n");
		printf("\n");
		
		int pids_sjf[max_size];
		int status_sjf;
		
		for(k=0; k<i; k++){												//kanw to status tou kathe proccess=ready, perimenei stin oura
			queueSorted[k]->data.status=0;	
		}
		
		char *forexec_sjf = "/usr/bin/echo";
		char *const z_sjf[] = {"/usr/bin/echo", " ", NULL};
		float total_workload;
		
		for(k=0; k<i; k++){
			
			struct sigaction sig;    									//gia na kanw catch to sigchld           
			sigemptyset(&sig.sa_mask);          
			sig.sa_flags = 0;                   
			sig.sa_handler = catch;
			
			struct tms tms_start, tms_end;								//gia elapsed, workload time				
			
			float pinakas_elapsed_xronwn[max_size];						
			int j;
			float workload_time=0;			

			
			
			times(&tms_start);											//gia na pairnw arxiko xrono me to pou arxizei to fork
			pids_sjf[k]=fork();											//tha kanei tosa fork osa kai ta diaforetika pids
			
			if(pids_sjf[k]==0){
				
				queueSorted[k]->data.status=1;						//tou kanw to status running giati molis ekane fork kai tha ginei exec
				
				printf("PID: %d - CMD: %s\n", queueSorted[k]->data.pid,  queueSorted[k]->data.file_name);
				execv(forexec_sjf, z_sjf);
				
				kill(queueSorted[k]->data.pid, SIGCHLD);
				
				exit(status_sjf);
				
			}else{
				waitpid(pids_sjf[k], &status_sjf, 0);
				times(&tms_end);										//gia na pairnw teliko xrono
				
				if((sigaction(SIGCHLD,&sig,NULL) == 0) && WIFEXITED(status_sjf)){
				
					//parent waiting
					struct timespec tim1, tim2;											//gia nanosleep, yparxei entos tis time.h
					tim1.tv_sec=queueSorted[k]->data.priority;
					tim1.tv_nsec=0;
					nanosleep(&tim1, &tim2);
					
					clock_t real = tms_end.tms_cutime  - tms_start.tms_utime;
					float running_time = real/(double)sysconf(_SC_CLK_TCK);
					running_time=running_time+queueSorted[k]->data.priority;
					printf("\t\t\tElapsed Time: %.2f\n", running_time);
					
					pinakas_elapsed_xronwn[k]=running_time;								//apotikeuw kathe running time gia na to exw gia to workload time
					for(j=k; j>=0; j--){
						workload_time=workload_time+pinakas_elapsed_xronwn[j];
					}
					printf("\t\t\tWorkload Time: %.2f\n", workload_time);
					total_workload=workload_time;
					
					queueSorted[k]->data.status=3;									//kanw to status tou proccess pou molis teleiwse = exited
				
				}else{
					printf("Proccess %d didnt exit normally.\n", queueSorted[k]->data.pid);
				}
			}
				
			printf("\n");
		}
		
		//pame gia teliko xrono kai remove ta nodes apo thn doubly linked list
		
		printf("Total Workload: %.2f\n", total_workload);
		for(k=0; k<i; k++){														//vgazw apo tin oura kathe diergasia pou exei kanei run
			deleteNode(&queueSorted[0], queueSorted[k]);					
		}
	}
	
	
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
	
	
	
	//FCFS - me doubly linked list
	if(strcmp(epilogi, fcfs)==0 || strcmp(epilogi, FCFS)==0 ){
		
		
		//dipla sindedemeni lista gia tis diergasies ston fcfs
		node *head;														//head, opou deixnei to prwto stoixeio tis listas
		node *queue[i];												//ftiaxnw pointer pinaka typou node
	
		for(k=0; k<i; k++){												//ta kanw null prwta
			queue[k]=NULL;
		}

		for(k=0; k<i; k++){												//desmeuw dynamika xwro
			queue[k]=(node*)malloc(sizeof(node));
		}


		for(k=0; k<i; k++){												//antistoixizw se kathe koutaki->data ena struct
			queue[k]->data=record[k];
		}


		queue[0]->next=queue[1];									//queue 0 prev, next ektos for
		queue[0]->prev=NULL;
	
		queue[i-1]->prev=queue[i-2];								//queue max prev, next ektos for, i-1 giati ksekinaw apo 0 alliws segmentation fault
		queue[i-1]->next=NULL;


		head=queue[0];												//kefali = me queue[0]
	
		for(k=1; k<i-1; k++){											//for loop gia na dwsw sto kathena to epomeno kai to proigoumeno 
			queue[k]->next=queue[k+1];
			queue[k]->prev=queue[k-1];
		}

		printf("Before the algorithms starts running: \n");
		display(queue[0]);									
		printf("\n");
		
		
		//pame twra gia thn ulopoihsh algorithmou		
		printf("Let's start: \n");
		printf("\n");
		
		int pids[max_size];
		int status;
		
		for(k=0; k<i; k++){												//kanw to status tou kathe proccess=ready, perimenei stin oura
			queue[k]->data.status=0;		
		}
		
		char *forexec = "/usr/bin/echo";
		char *const z[] = {"/usr/bin/echo", " ", NULL};
		float total_workload;
		
		for(k=0; k<i; k++){
			
			struct sigaction sig;               
			sigemptyset(&sig.sa_mask);          
			sig.sa_flags = 0;                   
			sig.sa_handler = catch;
			
			struct tms tms_start, tms_end;								//gia elapsed, workload time				
			
			float pinakas_elapsed_xronwn[max_size];						
			int j;
			float workload_time=0;			

			times(&tms_start);											//gia na pairnw arxiko xrono me to pou arxizei to fork		
			pids[k]=fork();												//tha kanei tosa fork osa kai ta diaforetika pids
			
			if(pids[k]==0){
				
				queue[k]->data.status=1;								//tou kanw to status running giati molis ekane fork kai tha ginei exec
				
				printf("PID: %d - CMD: %s\n", queue[k]->data.pid,  queue[k]->data.file_name);
				execv(forexec, z);
				
				kill(queue[k]->data.pid, SIGCHLD);
				
				exit(status);
				
			}else{	
				waitpid(pids[k], &status, 0);	
				times(&tms_end);				
				
				if((sigaction(SIGCHLD,&sig,NULL) == 0) && WIFEXITED(status)){
				
					//parent waiting
					struct timespec tim1, tim2;											//gia nanosleep, yparxei entos tis time.h
					tim1.tv_sec=queue[k]->data.priority;
					tim1.tv_nsec=0;
					nanosleep(&tim1, &tim2);
					
					clock_t real = tms_end.tms_cutime  - tms_start.tms_utime;
					float running_time = real/(double)sysconf(_SC_CLK_TCK);
					running_time=running_time+queue[k]->data.priority;
					printf("\t\t\tElapsed Time: %.2f\n", running_time);
					
					pinakas_elapsed_xronwn[k]=running_time;								//apotikeuw kathe running time gia na to exw gia to workload time
					for(j=k; j>=0; j--){
						workload_time=workload_time+pinakas_elapsed_xronwn[j];
					}
					printf("\t\t\tWorkload Time: %.2f\n", workload_time);
					total_workload=workload_time;
					
					queue[k]->data.status=3;									//kanw to status tou proccess pou molis teleiwse = exited
				
				}else{
					printf("Proccess %d didnt exit normally.\n", queue[k]->data.pid);
				}
			}
		
			printf("\n");
		}
		
		//pame gia teliko xrono kai remove ta nodes apo thn doubly linked list
		printf("Total Workload: %.2f\n", total_workload);
		for(k=0; k<i; k++){												//vgazw apo tin oura kathe diergasia pou exei kanei run
			deleteNode(&queue[0], queue[k]);					
		}
	}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

	
	
	//RR - me sorted doubly linked list	- thewrw oti exoun ola arival_time = 0
	//kathe for pou mia diergasia diakoptetai mpainei sto telos tis ouras giati oles einai etoimes afou ftanoun mazi
	if(strcmp(epilogi, rr)==0 || strcmp(epilogi, RR)==0 ){
		
		//quantum apo xristi
		int quantum = atoi(argv[2]);									//gia na parw to argv[2] se morfi integer
		printf("The Quantum you chose is (in ms):%d\n", quantum);
		printf("\n");
		
		//int quantum=(quantum1/1000);									//an thelw se ms
		
		
		//dipla sindedemeni lista gia tis diergasies ston rr
		//theloume oura taksinomimeni opws ston fcfs
		node *head;														//head, opou deixnei to prwto stoixeio tis listas
		node *queue[i];												//ftiaxnw pointer pinaka typou node
	
		for(k=0; k<i; k++){												//ta kanw null prwta
			queue[k]=NULL;
		}

		for(k=0; k<i; k++){												//desmeuw dynamika xwro
			queue[k]=(node*)malloc(sizeof(node));
		}


		for(k=0; k<i; k++){												//antistoixizw se kathe koutaki->data ena struct
			queue[k]->data=record[k];
		}


		queue[0]->next=queue[1];									//queue 0 prev, next ektos for
		queue[0]->prev=NULL;
	
		queue[i-1]->prev=queue[i-2];								//queue max prev, next ektos for, i-1 giati ksekinaw apo 0 alliws segmentation fault
		queue[i-1]->next=NULL;


		head=queue[0];												//kefali = me queue[0]
	
		for(k=1; k<i-1; k++){											//for loop gia na dwsw sto kathena to epomeno kai to proigoumeno 
			queue[k]->next=queue[k+1];
			queue[k]->prev=queue[k-1];
		}

		printf("Before the algorithms starts running: \n");
		display(queue[0]);									
		printf("\n");
		
		//pame twra gia thn ulopoihsh algorithmou		
		printf("Let's start: \n");
		printf("\n");
		
		int pids[max_size];
		int status;
		
		for(k=0; k<i; k++){												//kanw to status tou kathe proccess=ready, perimenei stin oura
			queue[k]->data.status=0;		
		}
		
		int remaining_time[max_size];
		for(k=0; k<i; k++){
			remaining_time[k]=queue[k]->data.priority;
		}
		
		char *forexec = "/usr/bin/echo";
		char *const z[] = {"/usr/bin/echo", " ", NULL};
		
		int arrival_time[max_size];
		for(k=0; k<i; k++){
			arrival_time[k]=0;
		}
		
		struct tms tms_start, tms_end;									//gia xronous 
		int nop;														//number of processes
		nop=i;
		int count=0;													//gia workload se kathe mia periptwsi
		float total_workload1;
		float total_workload2;
		float total_workload;											//sinoliko workload time meta ton rr
		
		
		
		while(nop!=0){
			
			for(k=0; k<i; k++){
				
				if(remaining_time[k]<=quantum && remaining_time[k]>0 && queue[k]->data.status!=3){
					
					struct sigaction sig;    									//gia na kanw catch to sigchld           
					sigemptyset(&sig.sa_mask);          
					sig.sa_flags = 0;                   
					sig.sa_handler = catch;
					
					float workload_time=0;
					times(&tms_start);
					pids[k]=fork();
					
					if(pids[k]==0){
						
						printf("PID: %d - Finished - CMD: %s\n", queue[k]->data.pid, queue[k]->data.file_name);
						
						kill(queue[k]->data.pid, SIGCHLD);									//SIGCHLD
						execv(forexec, z);
						exit(5);
						
					}else{
					
						waitpid(pids[k], &status, 0);
						times(&tms_end);
						
						if((sigaction(SIGCHLD,&sig,NULL) == 0) && WIFEXITED(status)){

							struct timespec tim1, tim2;											//gia nanosleep, yparxei entos tis time.h
							tim1.tv_sec=remaining_time[k];
							tim1.tv_nsec=0;
							nanosleep(&tim1, &tim2);
							count++;
							
							clock_t real = tms_end.tms_cutime  - tms_start.tms_utime;
							float running_time = real/(double)sysconf(_SC_CLK_TCK);
							float running_time1=running_time+remaining_time[k];
							printf("\t\t\t\tElapsed Time: %.2f\n", running_time1);				//panta isi me quantum

							workload_time=(count*quantum)+running_time;
							printf("\t\t\t\tWorkload Time: %.2f\n", workload_time);
							total_workload1+=running_time;
							printf("\n");
							
							remaining_time[k]=0;
							queue[k]->data.status=3;
							nop--;
							
						}
					}	

					deleteNode(&queue[0], queue[k]);										//kathe fora pou teleiwnei mia diergasia tin vgazw apo tin oura
					
				}else if(remaining_time[k]>quantum){
					
					struct sigaction sig;    									//gia na kanw catch to sigchld           
					sigemptyset(&sig.sa_mask);          
					sig.sa_flags = 0;                   
					sig.sa_handler = catchSTOP;
					
					float workload_time=0;
					times(&tms_start);
					pids[k]=fork();
					
					if(pids[k]==0){
						
						printf("PID: %d - %ds left - CMD: %s\n", queue[k]->data.pid, remaining_time[k]-quantum, queue[k]->data.file_name);						
						execv(forexec, z);
						//signal(SIGSTOP, catchSTOP);
						exit(6);
						
							
					}else{
						
						waitpid(pids[k], &status, 0);
						times(&tms_end);
						
						if(/*(sigaction(SIGSTOP,&sig,NULL) == 0) &&*/ WIFEXITED(status)){
							
							//kill(queue[k]->data.pid, SIGSTOP);
							struct timespec tim1, tim2;											//gia nanosleep, yparxei entos tis time.h
							tim1.tv_sec=quantum;
							tim1.tv_nsec=0;
							nanosleep(&tim1, &tim2);
							count++;
							
							clock_t real = tms_end.tms_cutime  - tms_start.tms_utime;			
							float running_time = real/(double)sysconf(_SC_CLK_TCK);
							float running_time1=running_time+quantum;
							printf("\t\t\t\tElapsed Time: %.2f\n", running_time1);					//panta isi me quantum edw

							workload_time=(count*quantum)+running_time;
							printf("\t\t\t\tWorkload Time: %.2f\n", workload_time);
							total_workload2+=running_time;
							
							printf("\n");
							remaining_time[k]=remaining_time[k]-quantum;
							queue[k]->data.status=2;
							
							//kill(queue[k]->data.pid, SIGCONT);
						}
						
					}
				}
			}
		}
		printf("\n");
		total_workload=total_workload1+total_workload2+(count*quantum);
		printf("Total workload: %.2f \n", total_workload);
		
		
		
		
	
	}						





	if(argc!=1){
		fclose(file);
	}
	
	
	return 0;

}
	
void display(node *ptr){												//function gia ektipwsi periexomenwn twn data twn koutiwn => structs processes
	
	node *last;
	
	printf("The doubled linked list elements are:\n");
	
	while(ptr!=NULL){
		
		printf("Filename: %s ", ptr->data.file_name);
		printf("Priority: %d ", ptr->data.priority);
		printf("Pid: %d \n", ptr->data.pid);
		
		last=ptr;
		ptr=ptr->next;
	}
}
	
void sort_array(process list[max_size], int s){
	
	int i, j;
	process temp;
	
	for(i=0; i<s; i++){
			
		for(j=0; j<(s-1-i); j++){
			
			if(list[j].priority > list[j+1].priority){
				
				temp=list[j];
				list[j]=list[j+1];
				list[j+1]=temp;
			}
		}
	}
}
		
void deleteNode(node **pheadp, node *pdelete){
	
	if(*pheadp==NULL || pdelete==NULL){
		return;
	}
	
	//an o node pou svinw einai to head
	if(*pheadp==pdelete){
		*pheadp=pdelete->next;
	}
	
	//na allazei o next mono an to node pou svinw den einai to teleutaio
	if(pdelete->next != NULL){
		pdelete->next->prev=pdelete->prev;
	}
	
	//na allazzei o prev mono an to node pou svinw den einai to prwto
	if(pdelete->prev != NULL){
		pdelete->prev->next=pdelete->next;
	}
	
	free(pdelete);
	return;
}
	
void catch(int sig_out){
	if(sig_out == SIGCHLD)
	return;
}

void catchSTOP(int sig_out){
	if(sig_out==SIGSTOP)
		return;
}

void catchCONT(int sig_out){
	if(sig_out==SIGCONT)
		return;
}
