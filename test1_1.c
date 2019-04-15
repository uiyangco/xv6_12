#include "types.h"
#include "stat.h"
#include "user.h"


void test_p1_1()
{
	int pid = getpid();
	int i, j;
	int key =1234;
	double x = 0, z;
	//int t;
	printf(1, "1st pid: %d\n", pid);
	//t =pid;
	setnice(pid, 3);
	pid =fork();
	for(i=0;i<3;i++){
	
		 
		 if(pid ==0){
		 pid = fork();
		 }
		
	
	}
	
	setnice(pid, -2);
	
	
	
	
	//wakeup(key);
	
	
	
	/*if(pid ==t+1){
		sleep(1324);
	}*/
	
	if(pid == 0){	//child
		for(i = 0; i < 2; i++){
			for ( z = 0; z < 3000000.0; z += 0.1 )
				x =  x + 3.14 * 89.64;
		        
		        
		        if(i ==1){
		        if(pid== 0){
	
				sleep(key);
			
			}
			}		
			ps();
			//yield();
		}
		
	
		exit();
		
	} else {	//parent
		for(j = 0; j < 2; j++){
			for ( z = 0; z < 3000000.0; z += 0.1 )
				x =  x + 3.14 * 89.64;
			
			if(pid ==5){
				setnice(pid, 4);
			}
			
			ps();
			//yield();
		}
		wait();
	}
}

int main(int argc, char **argv)
{
		printf(1, "=== TEST START ===\n");
		test_p1_1();
		printf(1, "=== TEST   END ===\n");

		exit();
}
