from imutils.video import VideoStream
import imutils
from pyzbar import pyzbar
import cv2
import socket
import serial
import time
import csv

ser = serial.Serial('/dev/ttyACM0', 9600)
# s = socket.socket()
host = '192.168.43.65'
port = 12345

Error = 0


def reconfirm():  # Checks if food passed through the laser sensor or not
    global Error
    while True:
        if ser.in_waiting > 0:
            newdata = str(int(ser.readline(), 16))
            if newdata == '9':  # Arduino sends digit 9 to Pi if food is delivered successfully
                print("Purchase Confirmed")
                newdata = '0'
                return
            elif newdata == '8':  # Arduino sends digit 8 to Pi if food is delivery failed
                Error = 1
                print("Error in preocessing order!")
                print("Order cancelled \n\n")
                return


def flaggedID():
    global Error
    global barcodeData
    Flagged = ['S201610045', 'S201610037']
    if barcodeData in Flagged:  # Checks whether the student has cleared previous dues or not
        Error = 2
        print("Dues not cleared! \n\n")
        return
    return


def pincheck():
    global pin
    global Error
    print("Pin: ")
    print(pin)
    while True:
        if ser.in_waiting > 0:
            x = str(int(ser.readline(), 16))
            print(x)
            while True:
                if ser.in_waiting > 0:
                    y = str(int(ser.readline(), 16))
                    print(y)
                    while True:
                        if ser.in_waiting > 0:
                            z = str(int(ser.readline(), 16))
                            print(z)
                            dataPin = x + y + z
                            if dataPin != pin:
                                Error = 1
                                print("Pin verification failed! \n\n")
                                return
                            return





def scan():
    global Error
    global barcodeData
    global pin
    global pinnotfound
    pinnotfound = 1
    while True:
        if ser.in_waiting > 0:  # Receives item code from Arduino and starts card scanning process
            Adata = str(int(ser.readline(), 16))
            listcommand = ['1', '2', '3', '4', '5']
            if Adata not in listcommand:
                return

            if Adata == '5':
                print("Security Alert")
                s = socket.socket()
                s.connect((host, port))  # Connect to server via socket connection
                print("Ready to transmit")
                time.sleep(1.0)
                send = "Alert" + ";" + Adata  # Sends student ID and item code to server
                s.send(send.encode())
                print("Alert sent! \n\n")
                send = ''
                s.close()
                time.sleep(1.0)
                Adata = '0'
                return

            print("Data received from Arduino: " + Adata)
            print("[INFO] starting video stream...")
            vs = VideoStream(src=0).start()
            time.sleep(1.0)

            while True:
                frame = vs.read()
                frame = imutils.resize(frame, width=800)
                barcodes = pyzbar.decode(frame)

                for barcode in barcodes:
                    (x, y, w, h) = barcode.rect
                    cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 0, 255), 2)

                    barcodeData = barcode.data.decode("utf-8")  # Decodes the scanned barcode
                    barcodeType = barcode.type

                    text = "{} ({})".format(barcodeData, barcodeType)
                    cv2.putText(frame, text, (x, y - 10),
                                cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 2)
                    print("Barcode Scanned: " + barcodeData)
                    vs.stop()  # Stops scanning after barcode is found

                    if barcodeData[0] != 'S':  # Checks for validity of the student ID
                        a = 2  # sends digit 2 to Arduino if student ID verification fails
                        b = str(a)
                        ser.write(b.encode())
                        print("Error! No Student ID Number Detected!")
                        print("Varification Failed! \n\n")
                        a = 0
                        b = '0'
                        cv2.destroyAllWindows()
                        return

                    flaggedID()  # Checks if the student have any dues left
                    if Error == 2:
                        a = 3  # sends digit 3 to Arduino if student has dues left
                        b = str(a)
                        ser.write(b.encode())
                        a = 0
                        b = '0'
                        Error = 0
                        cv2.destroyAllWindows()
                        return

                    a = 1
                    b = str(a)
                    ser.write(b.encode())  # Confirm Arduino that card has been scanned and verified
                    print("Conformation sent to Arduino")
                    a = 0
                    b = '0'

                    with open('Pin_List.csv', 'rt') as p:
                        reader = csv.reader(p, delimiter=',')  # good point by @paco
                        for row in reader:
                            for field in row:
                                if field == barcodeData:
                                    pin = row[1]
                                    pinnotfound = 0

                    p.close()

                    if pinnotfound == 1:
                        a = 5  # sends digit 5 to Arduino if ID not found in list
                        b = str(a)
                        ser.write(b.encode())
                        a = 0
                        b = '0'
                        return

                    pincheck()

                    if Error == 1:
                        a = 2  # sends digit 2 to Arduino if pin not matched
                        b = str(a)
                        ser.write(b.encode())
                        a = 0
                        b = '0'
                        Error = 0
                        return
                    else:
                        a = 1  # sends digit 1 to Arduino if pin matched
                        b = str(a)
                        ser.write(b.encode())
                        a = 0
                        b = '0'
                        Error = 0



                    reconfirm()  # Wait for Arduino to confirm the delivery of food
                    if Error == 1:
                        cv2.destroyAllWindows()
                        Error = 0
                        return
                    time.sleep(1.0)

                    s = socket.socket()
                    s.connect((host, port))  # Connect to server via socket connection
                    print("Ready to transmit")
                    time.sleep(1.0)

                    send = barcodeData + ";" + Adata  # Sends student ID and item code to server
                    s.send(send.encode())
                    print("Data sent")
                    send = ''

                    s.close()
                    time.sleep(1.0)
                    cv2.destroyAllWindows()
                    Adata = '0'
                    print("Purchase Complete! \n\n")
                    return

                cv2.imshow("Barcode Scanner", frame)
                key = cv2.waitKey(1) & 0xFF
                if ser.in_waiting > 0:  # Cancels card scanning process if zero is pressed on keypad
                    cancel = str(int(ser.readline(), 16))
                    if cancel == '0':
                        cv2.destroyAllWindows()
                        vs.stop()
                        print("User cancelled purchase! \n\n")
                        return
                k = cv2.waitKey(30)
                if k == 27:
                    cv2.destroyAllWindows()
                    vs.stop()
                    return


while True:
    print("Client Active")
    scan()  # Recursive main loop
