
def testValid(value):
    tempStr = "{0:010b}".format(value)
    flag = True
    if tempStr[-1] == "1":
        for i in range(3):
            if (tempStr[-5-i] == "1") or (tempStr[-8-i] == "1"):
                if tempStr[-2-i] != "1":
                    flag = False
                    break
            if (tempStr[-5-i] == "1") and (tempStr[-8-i] == "1"):
                flag = False
                break
    elif tempStr[-1] == "0":
        flag = False
    if flag == True:
        print(tempStr[0:3], tempStr[3:6], tempStr[6:9], tempStr[9], value)

def main():
    for i in range(1024):
        testValid(i)

if __name__ == "__main__":
    main()
