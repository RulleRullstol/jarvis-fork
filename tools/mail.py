
import sys
import os
import glob
from dotenv import load_dotenv
from os import getenv

parent_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
sys.path.append(parent_dir)

import smtplib
import ssl
from configHandler import getValue
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart
from email.mime.base import MIMEBase
from email import encoders


SMTP_SERVER = "smtp.gmail.com"
SMTP_PORT = 465  # Use 587 for TLS
SENDER_EMAIL = "jarvis.mailservice@gmail.com"
RECEIVER_EMAIL = getValue("home_assistant_settings", "mail")

boddies = {}
subjects = {}
names = {}
paths = {}

light_body = """Hello

I am here to deliver you a manual on how we can work with your lights.
Remeber you can always contact my creators if any there are any questions.

// Jarvis"""
light_subject = "Help with Lights"
light_name = "jarvisLights.pdf"
light_path = parent_dir + "/pdfs/jarvisLights.pdf"


boddies["light"] = light_body
subjects["light"] = light_subject
names["light"] = light_name
paths["light"] = light_path


def sendEmail(subject: str, body: str, fileName: str, filePath: str):
    load_dotenv()
    msg = MIMEMultipart()
    msg["From"] = SENDER_EMAIL
    msg["To"] = RECEIVER_EMAIL
    msg["Subject"] = subject
    msg.attach(MIMEText(body, "plain"))

    #fileName = "mail.txt"
    #script_dir = os.path.dirname(os.path.abspath(__file__))  # Get the script's directory
    #filePath = os.path.join(script_dir, "mail.txt")

    try:
        with open(filePath, "rb") as attachment:
            part = MIMEBase("application", "pdf")
            part.set_payload(attachment.read())
        
        encoders.encode_base64(part)
        part.add_header("Content-Disposition", f"attachment; filename={fileName}")
        msg.attach(part)

        # Send email
        context = ssl.create_default_context()
        with smtplib.SMTP_SSL(SMTP_SERVER, SMTP_PORT, context=context) as server:
            server.login(SENDER_EMAIL, getenv("EMAIL_PASSWORD"))
            server.sendmail(SENDER_EMAIL, RECEIVER_EMAIL, msg.as_string())

        print("Email with attachment sent successfully!")
    except Exception as e:
        print(f"Error Here: {e}")
        

def getManual(id: str):
    body_ = boddies[id]
    subject_ = subjects[id]
    name_ = names[id]
    path_ = paths[id]
    
    sendEmail(subject_, body_, name_, path_)
