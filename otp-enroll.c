//@author italianoaj

//OTP-enroll is used to add users to the otp process

//include statements
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//variable declarations
char error_message[30]="An Error has occured\n";
char *number;
char *name;
char *username;
char *tmp;
char *tmp2;
char *tmp3;
char *line_buf=NULL;
char *line_buf2=NULL;
char *line_buf3=NULL;
size_t line_buf_size=0;
FILE *fp;
char buffer[64];

//define statements
#define UF "/etc/italiano_verify/usrf"
#define MAX 1024

//function declarations
int search();
int exclude();
int show();

//main function
int main(int argc, char** argv){
	if(argc>3){
		fprintf(stderr, "\033[0;31m");
		fprintf(stderr, "Error: correct usage otp-enroll <option>\n");
		fprintf(stderr, "\033[0m");
		return 1;
	}
	if(argc>1){
		if(strcmp(argv[1], "-remove")==0){
			if(argc!=3){
				fprintf(stderr, "\033[0;31m");
                                fprintf(stderr, "Error: no user specified\n");
                                fprintf(stderr, "\033[0m");
                                return 1;
			}
			int retval=exclude(argv[3]);
			if(retval!=0){
				fprintf(stderr, "\033[0;31m");
				fprintf(stderr, "Error: unable to remove user\n");
				fprintf(stderr, "\033[0m");
				return 1;
			}
			return 0;
		}if(strcmp(argv[1], "-show")==0){
			int retval=show();
			if(retval!=0){
				fprintf(stderr, "\033[0;31m");
				fprintf(stderr, "Error: unable to show users\n");
				fprintf(stderr, "\033[0m");
				return 1;
			}
			return 0;
		}else{
			fprintf(stderr, "\033[0;31m");
			fprintf(stderr, "Error: invalid flag. Valid flags include:\n-remove\n-show\n");
			fprintf(stderr, "\033[0m");
			return 1;
		}
	}
	//retrive full name of new user
	printf("Enter your fullname: ");
	getline(&line_buf,&line_buf_size,stdin);
	name=strtok_r(line_buf,"\n",&tmp);
	//printf("You entered %s\n", name);
	//retrieve username of new user
	printf("Enter your username: ");
	getline(&line_buf2,&line_buf_size,stdin);
	username=strtok_r(line_buf2,"\n",&tmp2);
	//check if username is null
	if(username==NULL){
		fprintf(stderr, "\033[0;31m");
		fprintf(stderr, "Error: username invalid\n");
		fprintf(stderr, "\033[0m");
		return 1;
	}
	//search usrf file for username
	int retval=search();
	//unable to open usrf file
	if(retval==-1){
		fprintf(stderr, "\033[0;31m");
		fprintf(stderr, "Error: unable to open %s with read access. Please check your permissions.\n", UF);
		fprintf(stderr, "\033[0m");
		return 1;
	//username is alread enrolled
	}else if(retval==1){
		fprintf(stderr, "\033[0;31m");
		fprintf(stderr, "Error: %s is already enrolled\n", username);
		fprintf(stderr, "\033[0m");
		return 1;
	}
	//printf("You entered %s\n", username);
	printf("Carrier extensions:\nAlltel - mms.alltelwireless.com\nAT&T - mms.att.net\nBoost Mobile - myboostmobile.com\nCricket Wireless - mms.att.net\nMetroPCS - mymetropcs.com\nProject Fi - msg.fi.google.com\nSprint - pm.sprint.com\nStraight Talk - mms.att.net\nT-Mobile - tmomail.net\nVerizon - vzwpix.com\nVirgin Mobile - vmobl.com\n\n");
	printf("Please enter your phone number with the correct carrier extension. Example - 1234567890@mms.att.net: ");
	//retrieve phoen number of new user
	getline(&line_buf3,&line_buf_size,stdin);
	number=strtok_r(line_buf3,"\n",&tmp3);
	//printf("You entered %s\n", number);
	//generate line with correct syntax for the usrf file
	char *line=malloc(strlen(name+1)+1+strlen(username+1)+1+strlen(number+1)+1);
	strcat(line,name);
	strcat(line, ":");
	strcat(line, username);
	strcat(line, ":");
	strcat(line, number);
	strcat(line, "\n");
	//printf("Line to be entered is\n%s\n", line);
	//open usrf
	fp=fopen(UF,"a");
	//unable to open usrf with append access
	if(!fp){
		fprintf(stderr, "\033[0;31m");
		fprintf(stderr, "Error: unable to open %s with append access. Please check your permissions.\n",UF);
		fprintf(stderr, "\033[0m");
		return 1;
	}
	//apppend new line to usrf
	fputs(line,fp);
	//close usrf
	fclose(fp);
	//free the malloc
	free(line);
	//return
	printf("\033[0;32m");
	printf("Success: %s has been enrolled.\n", username);
	printf("\033[0m");
	return 0;
}

//this function is used to search the usrf file for redunadnt usernames
int search(){
	fp=fopen(UF,"r");
	//unable to open usrf with read access
	if(!fp){
		return -1;
	}
	char info[MAX];
	int p=0;
	int c;
	//read in data from usrf
	while((c=fgetc(fp))!=EOF){
		info[p++]=c;
	}
	//get furst full name
	char *full_name=strtok(info, ":");
	//get first username
	char *user=strtok(NULL, ":");
	//get first phoen number
	char *phone=strtok(NULL, "\n");
	while(1){
		//username matches a username on file -- redundant
		if(strcmp(user,username)==0){
			return 1;
		}
		//get next full name
		full_name=strtok(NULL, ":");
		//EOF
		if(full_name==NULL){
			break;
		}
		//get next username in file
		user=strtok(NULL, ":");
		//EOF
		if(user==NULL){
			break;
		}
		//get next phone number on file
		phone=strtok(NULL, "\n");
		//EOF
		if(phone==NULL){
			break;
		}
	}
	//close access to usrf
	fclose(fp);
	//return
	return 0;
}
int exclude(char** user){
	printf("in remove\n");
	printf("%s", user);
	fp=fopen(UF,"w");
	if(!fp){
		return 1;
	}
	char info[MAX];
	int p=0;
	int c;
	while((c=fgetc(fp))!=EOF){
		info[p++]=c;
	}
	return 1;
}

int show(){
	fp=fopen(UF,"r");
	if(!fp){
		return 1;
	}
	char info[MAX];
	int p=0;
	int c;
	while((c=fgetc(fp))!=EOF){
		info[p++]=c;
	}
	char *full_name=strtok(info, ":");
	char *user=strtok(NULL, ":");
	char *phone=strtok(NULL, "\n");
	printf("\nUsers enrolled: \n");
	printf("%s\n", user);
	while(1){
		full_name=strtok(NULL, ":");
		if(full_name==NULL){
			break;
		}
		user=strtok(NULL, ":");
		if(user==NULL){
			break;
		}
		phone=strtok(NULL, "\n");
		if(phone==NULL){
			break;
		}
		printf("%s\n\n", user);
	}
	fclose(fp);
	return 0;
}
