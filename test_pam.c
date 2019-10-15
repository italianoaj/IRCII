//gcc -o test_pam test_pam.c -lpam -lpam_misc
#include <stdio.h>
#include <stdlib.h>
#include <security/pam_appl.h>
#include <security/pam_misc.h>

static struct pam_conv conv={
	misc_conv,
	NULL
};

int main(){

	pam_handle_t *handle = NULL;
        const char *service_name = "test_pam";
        int retval;
        char *username; /* This will be set by PAM with pam_get_item (see below) */

        retval = pam_start(service_name, NULL, &conv, &handle); /* Initializing PAM */
        if (retval != PAM_SUCCESS){
                fprintf(stderr, "Failure in pam initialization: %s\n", pam_strerror(handle, retval));
                return 1;
        }

        retval = pam_authenticate(handle, 0); /* Do authentication (user will be asked for username and password)*/
        if (retval != PAM_SUCCESS) {
                fprintf(stderr, "Failure in pam authentication: %s\n", pam_strerror(handle, retval));
                return 1;
        }

        retval = pam_acct_mgmt(handle, 0); /* Do account management (check the account can access the system) */
        if (retval != PAM_SUCCESS) {
                fprintf(stderr, "Failure in pam account management: %s\n", pam_strerror(handle, retval));
                return 1;
       }

	retval=pam_open_session(handle, 0);
	if(retval!=PAM_SUCCESS){
		fprintf(stderr, "Failure in opening session: %s\n", pam_strerror(handle, retval));
		return 1;
	}

	retval=pam_setcred(handle, 0);
	if(retval!=PAM_SUCCESS){
		fprintf(stderr, "Failure in setting user variable: %s\n", pam_strerror(handle, retval));
		return 1;
	}

        /* We now get the username given by the user */
        pam_get_item(handle,PAM_USER,(const void **)&username);

        printf("Do you want to change your password? (answer y/n): ");
        char answer = getc(stdin); /* Taking user answer */
        if (answer == 'y') {
                retval = pam_chauthtok(handle, 0); /* Do update (user will be asked for current and new password) */
                if (retval != PAM_SUCCESS) {
                        fprintf(stderr, "Failure in pam password: %s", pam_strerror(handle, retval));
                        return 1;
                }
        }else{
		printf("okay, bye-bye\n");
		retval=pam_close_session(handle,0);
		if(retval!=PAM_SUCCESS){
			fprintf(stderr, "Failure in closing session: %s", pam_strerror(handle, retval));
			return 1;
		}
	}

        pam_end(handle, retval); /* ALWAYS terminate the pam transaction!! */
}

