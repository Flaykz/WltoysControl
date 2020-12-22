import threading
import time

import serial
from pynput.keyboard import Listener

prev_steering_key = None
serialPort = serial.Serial(port="COM7", baudrate=115200, bytesize=8, timeout=2, stopbits=serial.STOPBITS_ONE)
PREV_KEY = None

print("LAUNCH")


def on_press(key):
    global PREV_KEY, prev_steering_key
    key = str(key).replace("u'", "'")
    key = str(key).replace("'", "")
    if key == "z":
        if key == PREV_KEY:
            return
        else:
            PREV_KEY = key
        serialPort.write(key.encode())
        print(key)
        return
    if key == "s":
        if key == PREV_KEY:
            return
        else:
            PREV_KEY = key

        serialPort.write(key.encode())
        print(key)
        return
    if key == "d":
        if key == PREV_KEY:
            return
        else:
            PREV_KEY = key
        prev_steering_key = key
        serialPort.write(key.encode())
        print(key)
        return
    if key == "q":
        if key == PREV_KEY:
            return
        else:
            PREV_KEY = key
        prev_steering_key = key
        serialPort.write(key.encode())
        print(key)
        return
    if key == "Key.up":
        serialPort.write("i".encode())
        print(key)
        return
    if key == "Key.down":
        serialPort.write("k".encode())
        print(key)
        return
    if key == "Key.left":
        serialPort.write("j".encode())
        print(key)
        return
    if key == "Key.right":
        serialPort.write("l".encode())
        print(key)
        return


def on_release(key):
    global PREV_KEY, prev_steering_key

    key = str(key).replace("u'", "'")
    key = '!' + str(key).replace("'", "")
    if key == "!z":
        if key == PREV_KEY:
            return
        else:
            PREV_KEY = key
        serialPort.write("t".encode())
        print(key)
        return
    if key == "!s":
        if key == PREV_KEY:
            return
        else:
            PREV_KEY = key
        serialPort.write("g".encode())
        print(key)
        return
    if key == "!d":
        if key == PREV_KEY:
            return
        else:
            PREV_KEY = key
        if prev_steering_key == "d":
            serialPort.write("h".encode())
            print(key)
        return
    if key == "!q":
        if key == PREV_KEY:
            return
        else:
            PREV_KEY = key
        if prev_steering_key == "q":
            serialPort.write("f".encode())
            print(key)
        return
    if key == "!Key.esc":
        print('Exiting...')
        return False


def f_a():
    with Listener(on_release=on_release,
                  on_press=on_press
                  ) as listener:
        listener.join()
    global exitProgram
    exitProgram = True

def f_b():
    prev_time = int(round(time.time() * 1000))
    while True:
        global exitProgram
        if exitProgram:
            break
        bytesToRead = serialPort.inWaiting()
        if bytesToRead and (int(round(time.time() * 1000)) - prev_time > 100):
            bloc = serialPort.read(bytesToRead)
            last = bloc.split('\n').pop(0)
            print(last)
            prev_time = int(round(time.time() * 1000))



if __name__ == "__main__":
    exitProgram = False
    x = threading.Thread(target=f_a)
    x.start()
    y = threading.Thread(target=f_b)
    y.start()
    x.join()
    y.join()
