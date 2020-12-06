from pynput.keyboard import Key, Listener
import serial
import time

prev_steering_key=None
serialPort = serial.Serial(port = "COM3", baudrate=115200, bytesize=8, timeout=2, stopbits=serial.STOPBITS_ONE)
PREV_KEY=None

def on_press(key):
     
    global PREV_KEY,prev_steering_key
    key = str(key).replace("'", "")
    if key == "z":
        if key == PREV_KEY:
            return
        else:
            PREV_KEY = key
        serialPort.write(key.encode());
        print(key)
        return
    if key == "s":
        if key == PREV_KEY:
            return
        else:
            PREV_KEY = key
        
        serialPort.write(key.encode());
        print(key)
        return
    if key == "d":
        if key == PREV_KEY:
            return
        else:
            PREV_KEY = key
        prev_steering_key=key
        serialPort.write(key.encode());
        print(key)
        return
    if key == "q":
        if key == PREV_KEY:
            return
        else:
            PREV_KEY = key
        prev_steering_key=key
        serialPort.write(key.encode());
        print(key)
        return
    if key == "Key.up":
        serialPort.write("i".encode());
        print(key)
        return
    if key == "Key.down":
        serialPort.write("k".encode());
        print(key)
        return
    if key == "Key.left":
        serialPort.write("j".encode());
        print(key)
        return
    if key == "Key.right":
        serialPort.write("l".encode());
        print(key)
        return

def on_release(key):
    global PREV_KEY,prev_steering_key
    #PREV_KEY='sgsg'

    key = '!' + str(key).replace("'", "")
    
    if key == "!z":
        if key == PREV_KEY:
            return
        else:
            PREV_KEY = key
        serialPort.write("t".encode());
        print(key)
        return
    if key == "!s":
        if key == PREV_KEY:
            return
        else:
            PREV_KEY = key
        serialPort.write("g".encode());
        print(key)
        return
    if key == "!d":
        if key == PREV_KEY:
            return
        else:
            PREV_KEY = key
        if prev_steering_key=="d":
            serialPort.write("h".encode());
            print(key)
        return
    if key == "!q":
        if key == PREV_KEY:
            return
        else:
            PREV_KEY = key
        if prev_steering_key=="q":
            serialPort.write("f".encode());
            print(key)
        return
    if key == "!Key.esc":
        print('Exiting...')
        return

with Listener(on_release=on_release,
        on_press=on_press
        ) as listener:
    listener.join()

while True:
    for line in serialPort.read():
        print(chr(line))
