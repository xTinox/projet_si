#!/usr/bin/python3
from smbus2 import SMBus
import datetime
import time
class Arduino(SMBus):
    """docstring for Arduino."""
    def __init__(self,addr,path_src,path_dst):
        super().__init__(1)
        self.addr=addr
        self.src=path_src
        self.dst=path_dst
    def __str__(self):
        return "Arduino sur l'addresse {}".format(self.addr)
    #envoyer les uids
    def send_allow_users(self):
        self.write_byte(self.addr,0)
        tmp=self.read_byte(self.addr)
        f=open(self.src,"r")
        for i in f:
            #on lie les uid et on enlève le \n (retour ligne) d'ou le [:1]
            self.write_i2c_block_data(self.addr,1,i[:-1].encode())
            time.sleep(0.6)
        self.write_byte(self.addr,2)
        f.close()
    #recurer les uid dans un fichier
    def recv_users(self):
        f=open(self.dst,"a")
        while True:
            now=datetime.datetime.now()
            uid=""
            recv=self.read_i2c_block_data(self.addr,2,8)
            if recv==[0 for x in range(8)]:
                break;
            for i in range(len(recv)):
                uid+=chr(recv[i])
            self.dst.write(uid+"-"+str(now.year)+"-"+str(now.hour)+"-"+str(now.minute)+"\n")
            time.sleep(0.6)
        f.close()
#on instancie un objet de la class
arduino=Arduino(0x12,path_src="allow_users.txt",path_dst="log_users.txt")
while True:
    try:
        arduino.send_allow_users()
        while True:
            try:
                time.sleep(25)
                arduino.recv_users()

            except:
                time.sleep(2)
                arduino.send_allow_users()
                time.sleep(5)
    except:
        time.sleep(4)
