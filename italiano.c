/*This program is designed to server as a PAM module to handle authentication between a program and PAM
Please run the make file to compile correctly.*/

// @author italianoaj //

//Include statements
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <stdbool.h>
#include <security/pam_modules.h>
#include <security/pam_ext.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <security/pam_appl.h>
#include <security/pam_misc.h>

//Varibale Declarations
#define MAX 1024
// The usrf file must be in this directory
#define UF "/etc/italiano_verify/usrf"
char *full_name;

//Function Declartions
int authenticate(const char*, const char*, int);
char* getPhone(const char*);
static struct pam_conv conv = {
    misc_conv,
    NULL
};

//The authenticate() function unsures the user input matches both the username within the usrf file
//and the random password sent to the user
int authenticate(const char* username, const char* password, int rng){
	//open the usrf file
	FILE *fp=fopen(UF,"r");
	if(!fp){
		//if the file cannot be opened, close.
		printf("Cannot open file: %s\n",UF);
		exit(1);
	}
	char info[MAX];
	int p=0;
	int authenticated=1;
	int c;
	//ascii to integer
	int newpass=atoi(password);
	//read input from file
	while((c=fgetc(fp))!=EOF){
		info[p++]=c;
	}
	//get user's fullname
	full_name=strtok(info,":");
	//get user's username
	char *user=strtok(NULL,":");
	//get users password on file
	char *phone=strtok(NULL,"\n");
	//check for comparission
	while(1){
		//data matches, authenticate
		if(strcmp(user,username)==0 && newpass==rng){
			authenticated=0;
			break;
		}
		full_name=strtok(NULL, ":");
		if(full_name==NULL){
			break;
		}
		user=strtok(NULL, ":");
		if(user==NULL){
			break;
		}
		phone=strtok(NULL,"\n");
		if(phone==NULL){
			break;
		}
	}
	return authenticated;
}

//PAM function that handles the authentication when a user uses this module with an auth type
PAM_EXTERN int pam_sm_authenticate(pam_handle_t *handle, int flags, int argc, const char **argv){
	int pam_code;
	const char *username = NULL;
	const char *password = NULL;
	//get username from the user
	pam_code=pam_get_user(handle, &username, "Username: ");
	if(pam_code!=PAM_SUCCESS){
		printf("You weren't supposed to do that!\n");
		return PAM_PERM_DENIED;
	}
	char *phone=getPhone(username);
	//phone is null if the user does not exist or if the user is not enrolled in the verification process.
	if(phone==NULL){
		printf("User does not have a verification contact on file. Please contact the system administrator for enrollment.\n");
		//begin login process for non-enrolled user
		pam_code=pam_start("login2", username, &conv, &handle);
			if(pam_code!=PAM_SUCCESS){
				printf("An error has occured.\n");
				return pam_code;
			}
		//get password from the user and attempt to login
		pam_code=pam_authenticate(handle,flags);
			//if pam_code is not 0 (PAM_SUCCESS), authentication has failed.
			if(pam_code!=PAM_SUCCESS){
				printf("An error has occured in Authentication.\n");
				return pam_code;
			}else if(pam_code==PAM_SUCCESS){
				printf("Welcome, %s!\n", username);
			}
		return pam_code;
	}
	int n;
	//generate random number and place it into n
        srand(time(0));
       	n=(rand() % (999999-100000+1))+100000;
	char num[6];
	//num holds the string equivilent of n, to pass into the verify.py program
	sprintf(num,"%i",n);
	char* pargv[3];
	//generate command line arguements
	pargv[0]="verify.py";
	//random number generated for OTP password
	pargv[1]=num;
	//phone number
	pargv[2]=phone;
	//must be null terminated array for execvp() call
	pargv[3]=NULL;
	char *p="/etc/italiano_verify/verify.py";
	//create child process to run the verify script. 
	int rc=fork();
	if(rc<0){
		//if fork failed, return
		printf("fork failed\n");
		return PAM_ABORT;
	}else if(rc==0){
		//run verify.py
		execvp(p,pargv);
	}
	//wait for verify.py to finish running by child process. 
	wait(NULL);
	//get verification code from the user
	pam_code=pam_get_authtok(handle, PAM_AUTHTOK, &password, "Verification code: ");
	if(pam_code!=PAM_SUCCESS){
		printf("An error has occured\n");
		return PAM_PERM_DENIED;
	}
	//check if password is null
	if(flags & PAM_DISALLOW_NULL_AUTHTOK){
		if(password==NULL || strcmp(password,"")==0){
			printf("Null passwords are not allowed on this system\n");
			return PAM_PERM_DENIED;
		}
	}
	//call authenticate
	if(authenticate(username,password,n)==0){
		printf("Welcome, %s!\n", username);
		return PAM_SUCCESS;
	}else{
		printf("Incorrect username-verification code combination\n");
		return PAM_PERM_DENIED;
	}
}

//This function checks to see if the accoutn has expired with a made up account exp. date
PAM_EXTERN int pam_sm_acct_mgmt(pam_handle_t *handle, int flags, int argc, const char **argv){
	//struct to simulate exp date
	struct tm doomsday;
	doomsday.tm_mday=31;
	doomsday.tm_mon=12;
	doomsday.tm_year=2099;
	doomsday.tm_sec=59;
	doomsday.tm_min=59;
	doomsday.tm_hour=23;

	time_t end;
	time_t now;
	end=mktime(&doomsday);
	//get current date and time
	now=time(NULL);

	if(now>end){
		printf("Your account has expired, please contact the system admin\n");
		return PAM_PERM_DENIED;
	}else{
		return PAM_SUCCESS;
	}
}
//sets envirometn variable of the user full name for the OS
PAM_EXTERN int pam_sm_setcred(pam_handle_t *handle, int flags, int argc, const char **argv){
	const char *env_var="USER_FULL_NAME";

	char *line_buf = NULL;
	size_t line_buf_size = 0;

	const char *name=full_name;

	char env_path[1024];

	if(flags & PAM_ESTABLISH_CRED){
		sprintf(env_path,"%s=%s",env_var,name);
	}else if(flags & PAM_DELETE_CRED){
		sprintf(env_path,"%s",env_var);
	}
	pam_putenv(handle, env_path);
	return PAM_SUCCESS;
}
//mounts home directory for the user
PAM_EXTERN int pam_sm_open_session(pam_handle_t *handle, int flags, int argc, const char **argv){
	const char *username;
	char home_dir[100];

	pam_get_item(handle,PAM_USER,(const void **)&username);
	sprintf(home_dir,"/home/%s",username);
	mkdir(home_dir, 0770);
	return PAM_SUCCESS;
}
//removes home directory
PAM_EXTERN int pam_sm_close_session(pam_handle_t *handle, int flags, int argc, const char **argv){
	const char *username;
	char home_dir[1024];

	pam_get_item(handle,PAM_USER,(const void **)&username);
	sprintf(home_dir,"/home/%s",username);
	rmdir(home_dir);
	return PAM_SUCCESS;
}
//This function retrieves the phone number associated with the account that is attempting to login.
char* getPhone(const char* uname){
	//open the usrf file
	FILE *fp=fopen(UF,"r");
	if(!fp){
		printf("Cannot open file: %s\n",UF);
		exit(1);
	}
	char info[MAX];
	int p=0;
	int c;
	//read input from file
	while((c=fgetc(fp))!=EOF){
		info[p++]=c;
	}
	//get user's fullname
	full_name=strtok(info,":");
	//get user's username
	char *user=strtok(NULL,":");
	//get users phone number  on file
	char *phone=strtok(NULL,"\n");
	//check for comparission
	while(1){
		if(strcmp(user,uname)==0){
			break;
		}
		full_name=strtok(NULL, ":");
		if(full_name==NULL){
			break;
		}
		user=strtok(NULL, ":");
		if(user==NULL){
			break;
		}
		phone=strtok(NULL,"\n");
		if(phone==NULL){
			break;
		}
	}
	if(phone!=NULL && user!=NULL && full_name!=NULL){
		return phone;
	}
	return NULL;
}