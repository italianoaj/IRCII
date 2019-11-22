Anthony Italiano - anthony@italianoaj.com  
Hiram College   
Fall 2019  

This project was written as my Independent Research Component II for Hiram College in Fall of 2019. The purpose of this project to reseach, understand,and implement various functions of pluggable authentication modules, better known as PAM. This project was written for Ubuntu 16.04 and I am not sureif it will run the same, if at all, on other operating systems. Below are the correct instructions for compiling the C programs in this project.

Run the Makefile to properly compile the code(You will need sudo access). A tester program will also be present to test the PAM module before implementation into a live system. To remove the tester program, run "make clean"

There are also some requirements the user should be aware of:   
1. The italiano_verify directory must be placed in the /etc directory with the files inside.
2. The contents of the 'pam.d files' directory must be moved to /etc/pam.d
3. This module must be implemented on Ubuntu 16.04 systems.
4. Edits must be made to the /etc/pam.d/login file:  
	I. add the following to the begining of the file:  
		auth	required	italiano.so  
		account	required	italiano.so  
		session	required	italiano.so   
	II. Comment out (with #) the other auth lines in the file, including the @include common-auth line.     


The usrf file must follow the follwoing format:   
Full Name:username:PhoneNumberWithExtension\n

Example:  
John Doe:john:4208886969@mms.att.net  
Jane Doe:jane:3303303300@vzwpix.com  

Extensions for Emailing Phone Numbers(by carrier):
1. Alltel - mms.alltelwireless.com
2. AT&T - mms.att.net
3. Boost Mobile - myboostmobile.com
4. Cricket Wireless - mms.att.net
5. MetroPCS - mymetropcs.com
6. Project Fi - msg.fi.google.com
7. Sprint - pm.sprint.com
8. Straight Talk - mms.att.net
9. T-Mobile - tmomail.net
10. Verizon - vzwpix.com
11. Virgin Mobile - vmobl.com

Required packages (full command):  
sudo apt install git  
sudo apt install gcc  
sudo apt install libpam0g-dev  
sudo apt install python3  
sudo apt install make  
