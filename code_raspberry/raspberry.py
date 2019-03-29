#!/usr/bin/python3
from smbus2 import SMBus
import datetime
import time
class Arduino(SMBus):
    """docstring for Arduino."""
    def __init__(self,addr,path_src,path_dst):
        super().__init__(1)
        self.addr=addr
        self.src=open(path_src,"r")
        self.dst=open(path_dst,"a")
        self.dico={
        0:self.send_allow_users,
        1:self.recv_users
        }
    def __str__(self):
        return "Arduino sur l'address {}".format(self.addr)
    def send_allow_users(self):
        for i in self.src:
            #on lie les uid et on enlève le \n (retour ligne) d'ou le [:1]
            self.write_i2c_block_data(self.addr,1,i[:-1].encode())
            time.sleep(0.6)
        self.write_byte(self.addr,2)
    def recv_users(self):
        iud=""
        for i in self.read_i2c_block_data(self.addr,2,8):
            iud+=i
        self.dst.write(iud+"\n")
    def get_status(self):
        self.write_byte(self.addr,0)
        tmp=self.read_byte(self.addr)
        retour=self.dico.get(self.read_byte(self.addr))
        if(retour!=None): retour()

arduino=Arduino(0x12,path_src="allow_users.txt",path_dst="log_users.txt")
arduino.get_status()
