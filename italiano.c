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

#define MAX 1024
#define UF "/etc/usrf"
char *full_name;

int authenticate(const char*, const char*, int);
//int cngpass(const char*, const char*);

int authenticate(const char* username, const char* password, int rng){
	FILE *fp=fopen(UF,"r");
	if(!fp){
		printf("Cannot open file: %s\n",UF);
		exit(1);
	}
	char info[MAX];
	int p=0;
	int authenticated=1;
	int c;

	while((c=fgetc(fp))!=EOF){
		info[p++]=c;
	}

	full_name=strtok(info,":");
	char *user=strtok(NULL,":");
	char *pass=strtok(NULL,"\n");

	while(1){
		if(strcmp(user,"test")){
			int newpass=atoi(password);
			if(newpass==rng){
				authenticated=0;
				break;
			}else{
				break;
			}
		}
		if(strcmp(user,username)==0 && strcmp(pass,password)==0){
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
		pass=strtok(NULL,"\n");
		if(pass==NULL){
			break;
		}
	}
	return authenticated;
}

PAM_EXTERN int pam_sm_authenticate(pam_handle_t *handle, int flags, int argc, const char **argv){
	int pam_code;
	const char *username = NULL;
	const char *password = NULL;

	pam_code=pam_get_user(handle, &username, "Username: ");
	if(pam_code!=PAM_SUCCESS){
		printf("You weren't supposed to do that!\n");
		return PAM_PERM_DENIED;
	}
	int n;
	if(strcmp(username,"test")==0){
        	srand(time(0));
        	n=(rand() % (999999-100000+1))+100000;
		char num[6];
		sprintf(num,"%i",n);
		char* argv[3];
		argv[0]="verify.py";
		argv[1]=num;
		argv[2]=NULL;
		char *p="/home/italianoaj/pam/dev/IRC/verify.py";
		int rc=fork();
		if(rc<0){
			printf("fork failed\n");
			return 1;
		}else if(rc==0){
			execvp(p,argv);
		}
		wait(NULL);
		pam_code=pam_get_authtok(handle, PAM_AUTHTOK, &password, "Verification code: ");
		if(pam_code!=PAM_SUCCESS){
			printf("An error has occured\n");
			return PAM_PERM_DENIED;
		}
	}
	pam_code=pam_get_authtok(handle, PAM_AUTHTOK, &password, "Password: ");
	if(pam_code!=PAM_SUCCESS){
		printf("You weren't supposed to do that!\n");
		return PAM_PERM_DENIED;
	}
	if(flags & PAM_DISALLOW_NULL_AUTHTOK){
		if(password==NULL || strcmp(password,"")==0){
			printf("Null passwords are not allowed on this system\n");
			return PAM_PERM_DENIED;
		}
	}
	if(authenticate(username,password, n)==0){
		printf("Welcome, %s!\n", username);
		return PAM_SUCCESS;
	}else{
		printf("Incorrect username/password combination\n");
		return PAM_PERM_DENIED;
	}
}

PAM_EXTERN int pam_sm_acct_mgmt(pam_handle_t *handle, int flags, int argc, const char **argv){
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
	now=time(NULL);

	if(now>end){
		printf("Your account has expired, please contact the system admin\n");
		return PAM_PERM_DENIED;
	}else{
		return PAM_SUCCESS;
	}
}

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

PAM_EXTERN int pam_sm_open_session(pam_handle_t *handle, int flags, int argc, const char **argv){
	const char *username;
	char home_dir[100];

	pam_get_item(handle,PAM_USER,(const void **)&username);
	sprintf(home_dir,"/home/italianoaj/%s",username);
	mkdir(home_dir, 0770);
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_close_session(pam_handle_t *handle, int flags, int argc, const char **argv){
	const char *username;
	char home_dir[1024];

	pam_get_item(handle,PAM_USER,(const void **)&username);
	sprintf(home_dir,"/home/italianoaj/%s",username);
	rmdir(home_dir);
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_chauthtok(pam_handle_t *handle, int flags, int argc, const char **argv){
	printf("Oops... didn't get that far yet\n");
	return PAM_PERM_DENIED;
}
