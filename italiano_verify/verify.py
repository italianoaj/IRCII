#!/usr/bin/python3
#This program is used to send messages via SMTP
# @author italianoaj #

#Imports
import smtplib
import sys
#If there are not 3 arguments, wrong syntax, exit -- sanilty check 
if(len(sys.argv)!=3):
    print("usage: verify.py <message> <email address>")
    sys.exit()

#Fields
#SMTP server and port for conneciton
mailserver = 'smtp.gmail.com'
port = 587
mail = smtplib.SMTP(mailserver,port)
#Username and password for the email that will send the message
user = 'italiano.verify@gmail.com'
pswd = ''
#The address the message will be sent to
to = sys.argv[2]

mail.ehlo()
mail.starttls()

#Authenticates the server login.
mail.login(user, pswd)
#Generate SMTP header
header = 'To: ' +to+ '\n' + 'From: ' +user+ '\n' + 'Subject:Your One Time Password is:\n'
#Generate the message. Combine the header information and the message passed to the program
msg = header + "\n"+sys.argv[1]
#Send the email
mail.sendmail(user, to, msg)
#Closes the connection to the mail server.
mail.close
#exit the program
sys.exit()
