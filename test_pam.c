/* This program is used to test the PAM module before placing it into a live enviroment.
The program will test all of the functions within the italiano.c program. */
// @author italianoaj //

//Include statements
#include <stdio.h>
#include <stdlib.h>
#include <security/pam_appl.h>
#include <security/pam_misc.h>

//Conversation structure
static struct pam_conv conv={
	misc_conv,
	NULL
};

//The main function will run the tests of the italiano.c program.
int main(){

	pam_handle_t *handle = NULL;
        //name of the file inside of the /etc/pam.d direcotry to look for PAM rules
        const char *service_name = "test_pam";
        //return value from different function calls
        int retval;
        //username of the user testing the Italiano Verify
        char *username;

        //Initialize PAM 
        retval=pam_start(service_name, NULL, &conv, &handle);
        if (retval!=PAM_SUCCESS){
                fprintf(stderr, "Failure in pam initialization: %s\n", pam_strerror(handle, retval));
                return 1;
        }
        //Run authentication, retrieve username and password from user
        retval=pam_authenticate(handle, 0);
        if (retval!=PAM_SUCCESS) {
                fprintf(stderr, "Failure in pam authentication: %s\n", pam_strerror(handle, retval));
                return 1;
        }
        //Check to see if the user's account has expired.
        retval=pam_acct_mgmt(handle, 0); 
                if (retval!=PAM_SUCCESS) {
                fprintf(stderr, "Failure in pam account management: %s\n", pam_strerror(handle, retval));
                return 1;
       }
        //Open the session for the user and mount the home directory.
	retval=pam_open_session(handle, 0);
	if(retval!=PAM_SUCCESS){
		fprintf(stderr, "Failure in opening session: %s\n", pam_strerror(handle, retval));
		return 1;
	}
        //Test that the module is able to set the user's full name to the ENV variable. 
	retval=pam_setcred(handle, 0);
	if(retval!=PAM_SUCCESS){
		fprintf(stderr, "Failure in setting user variable: %s\n", pam_strerror(handle, retval));
		return 1;
	}

        //Retrieve the username entered by the user.
        retval=pam_get_item(handle,PAM_USER,(const void **)&username);
	if(retval!=PAM_SUCCESS){
		fprintf(stderr, "Could not obtain username: %s\n", pam_strerror(handle, retval));
		return 1;
	}
        //Let user know that the test was successful
	printf("Test for user %s is successful. Goodbye\n", username);

        //End the PAM transaction between this application and the Italiano Verify module
        pam_end(handle, retval);
}