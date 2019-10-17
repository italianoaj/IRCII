#!/usr/bin/python3
#Imports
import smtplib
import sys

#Fields
emails = []
mailserver = 'smtp.gmail.com'
port = 587
mail = smtplib.SMTP(mailserver,port)
user = 'jmitaliano42@gmail.com'
pswd = 'L0ck3d_0ut'
to = '3302724894@mms.att.net'
mail.ehlo()
mail.starttls()

#Authenticates the server login.
mail.login(user, pswd)

#Sends an email to each of the addresses in the email list

header = 'To: ' +to+ '\n' + 'From: ' +user+ '\n' + 'Subject:Your One Time Password is:\n'
msg = header + "\n"+sys.argv[1]
mail.sendmail(user, to, msg)

#Closes the connection to the mail server.
mail.close
