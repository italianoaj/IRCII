#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

#define UF "/etc/italiano_verify/usrf"

int main(){
	printf("Enter your fullname: ");
	getline(&line_buf,&line_buf_size,stdin);
	name=strtok_r(line_buf,"\n",&tmp);
	//printf("You entered %s\n", name);
	printf("Enter your username: ");
	getline(&line_buf2,&line_buf_size,stdin);
	username=strtok_r(line_buf2,"\n",&tmp2);
	//printf("You entered %s\n", username);
	printf("Carrier extensions:\nAlltel - mms.alltelwireless.com\nAT&T - mms.att.net\nBoost Mobile - myboostmobile.com\nCricket Wireless - mms.att.net\nMetroPCS - mymetropcs.com\nProject Fi - msg.fi.google.com\nSprint - pm.sprint.com\nStraight Talk - mms.att.net\nT-Mobile - tmomail.net\nVerizon - vzwpix.com\nVirgin Mobile - vmobl.com\n\n");
	printf("Please enter your phone number follwoinf the correct carrier extension. Example - 1234567890@mms.att.net: ");
	getline(&line_buf3,&line_buf_size,stdin);
	number=strtok_r(line_buf3,"\n",&tmp3);
	//printf("You entered %s\n", number);
	char *line=malloc(strlen(name+1)+1+strlen(username+1)+1+strlen(number+1)+1);
	strcat(line,name);
	strcat(line, ":");
	strcat(line, username);
	strcat(line, ":");
	strcat(line, number);
	strcat(line, "\n");
	//printf("Line to be entered is\n%s\n", line);
	fp=fopen(UF,"a");
	if(!fp){
		printf("Unable to open %s\n",UF);
		return 1;
	}
	fputs(line,fp);
	fclose(fp);
	free(line);
	return 0;
}
