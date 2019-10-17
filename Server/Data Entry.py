import argparse
print("CBVM Registration:\n")


def entry():

    print("******************************** New Entry ********************************")
    ID = 'S20' + input("Enter student ID: S20")
    import csv
    with open('Pin_List.csv', 'rt') as f:
        reader = csv.reader(f, delimiter=',')  # good point by @paco
        for row in reader:
            for field in row:
                if field == ID:
                    print("You have already registered!\nContact operators for further assistance...\n")
                    return
    digitlist = ['1', '2', '3', '4', '5', '6', '7', '8', '9']
    while True:
        err = 0
        PIN = input("Enter a 3 digit pin: ")
        if len(PIN) != 3:
            print("Error: Pin entered does not contain 3 digits!")
            err = 1
        for digit in PIN:
            if digit not in digitlist:
                print("Error: invalid pin (pin must be digits 1-9 only)")
                err = 1
        if err == 0:
            PIN = int(PIN)
            break
    email = input("Enter your email address: ")
    print("\nYour information:\nStudent ID: " + ID + "\nPin: " + str(PIN) + "\nEmail Address: " + email)
    confirm = input("All data correct?(Y/N): ")
    con = ['Y', 'y']
    if confirm in con:
        filename = "Pin_List.csv"

        ap = argparse.ArgumentParser()
        ap.add_argument("-o", "--output", type=str, default=filename,
                        help="path to output CSV file containing barcodes")
        args = vars(ap.parse_args())
        csv = open(args["output"], "a")
        found = set()

        if ID not in found:
            csv.write("{},{},{}\n".format(ID, PIN, email))
            csv.flush()
            found.add(ID)
        print("Ok, data saved!")
    else:
        print("Data discarded!")
    print("\n")


while True:
    entry()
