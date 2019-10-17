import socket
import argparse
import datetime
import tkinter
from tkinter import messagebox

import smtplib


It1 = "Kurkure Yellow"
P1 = "25"
It2 = "Kurkure Green"
P2 = "25"
It3 = "DoraCake"
P3 = "10"
It4 = "AllTime Butter Bun"
P4 = "10"

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
host = ''
port = 12345
s.bind((host, port))
s.listen(5)


while True:
    c, addr = s.accept()
    data = c.recv(4096)
    res = data.decode("utf8")
    datalist = res.split(';')
    ID = datalist[0]
    code = datalist[1]

    if code == '5':
        messagebox.showwarning("Warning!", "Security Alert!")
        print("Security Alert!")
    elif code == '1':
        Item = It1
        Price = P1
    elif code == '2':
        Item = It2
        Price = P2
    elif code == '3':
        Item = It3
        Price = P3
    elif code == '4':
        Item = It4
        Price = P4

    list = ['1', '2', '3', '4']

    if code in list:
        print("Purchased by: " + ID + ",   Item: " + Item + ",   Amount: " + Price)
        filename = ID + ".csv"

        ap = argparse.ArgumentParser()
        ap.add_argument("-o", "--output", type=str, default=filename,
                        help="path to output CSV file containing barcodes")
        args = vars(ap.parse_args())
        csv = open(args["output"], "a")
        found = set()

        if ID not in found:
            csv.write("{},{},{}\n".format(datetime.datetime.now(), Item, Price))
            csv.flush()
            found.add(ID)

        TO = ''
        import csv
        with open('Pin_List.csv', 'rt') as f:
            read = csv.reader(f, delimiter=',')
            for row in read:
                for field in row:
                    if field == ID:
                        TO = row[2]
        Dues = 0
        with open(filename, 'rt') as i:
            read2 = csv.reader(i, delimiter=',')
            for row in read2:
                Dues = Dues + int(row[2])

        SUBJECT = 'Purchase Receipt'
        TEXT = "Credit Based Vending Machine\n\n\nPurchase Receipt:\n\nPurchased by: " + ID + "\nItem: " + Item + "\nAmount: Tk." + Price + "\n\n\nTotal dues left: Tk." + str(Dues) + "\nFor any sort of queries, feel free to contact us. Don't forget to clear your dues! Happy Snacking!\n\nThis wasn't you?\nContact operators immediately at: cb.vending.machine@gmail.com"

        # Gmail Sign In
        gmail_sender = 'cb.vending.machine@gmail.com'
        gmail_passwd = '331331CBVM'

        server = smtplib.SMTP('smtp.gmail.com', 587)
        server.ehlo()
        server.starttls()
        server.login(gmail_sender, gmail_passwd)

        BODY = '\r\n'.join(['To: %s' % TO,
                            'From: %s' % gmail_sender,
                            'Subject: %s' % SUBJECT,
                            '', TEXT])

        try:
            server.sendmail(gmail_sender, [TO], BODY)
            print('email sent')
        except:
            print('error sending mail')

        server.quit()
    data = ''
    TO = ''
    c.close()

