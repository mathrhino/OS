#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<error.h>
#include<errno.h>
#include<pwd.h>

int get_ID(char *u);
void function1();
void function2();
void function3();
void function4();

FILE *fp =NULL;
int get_ID(char *u){
  struct passwd *s ;
  if((s=getpwnam(u))==NULL){
	perror(u);
	printf("Error");
	return -1;
  }	
  return (int)s->pw_uid;
}
void function4(){
  fprintf(fp,"2 -2");
}
void function3(){
  char proc[256];
  char user_k[256];
  int uid_pri;
  
  printf("Give me username to prevent other killing process\n");
  scanf("%s", user_k);

  uid_pri=get_ID(user_k);
  fprintf(fp, "2  %d\n", uid_pri);
}
void function2(){
  fprintf(fp, "1 -1, -1");
}
void function1(){
  char file[256];
  char uid[256];
  int uid_num;

  printf("Give me file name\n");
  scanf("%s", file);

  printf("Give me user\n");
  scanf("%s",uid);
  printf("%s\n", uid);
  uid_num = get_ID(uid);
  
  printf("uid_num is %d\n",uid_num);
   
  fprintf(fp, "1 %d %s\n",uid_num, file);  

}


int
main(){
  int press =0;
  printf("Press 1 for Function 1 blocking file access\n");
  printf("Press 2 for Function 1 restoring\n");
  printf("press 3 for function 2 block kill\n");
  printf("press 4 for function 2 allow:\n");
  scanf("%d", &press);

  fp = fopen("/proc/mousehole","w");
  if (press ==1){
    function1();
  }
  else if (press==2){
    function2();
  }
  else if (press==3){
    function3();	
  }
  else if (press==4){
    function4();	  
  }
  fclose(fp);
 }
