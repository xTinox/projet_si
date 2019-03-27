#!/usr/bin/python3
from smbus2 import SMBus
import datetime
import time
class Arduino(SMBus):
    """docstring for Arduino."""
    def __init__(self,addr):
        super().__init__(1)
        self.addr=addr
        self.dico={0:self.send_allow_users}
    def __str__(self):
        return "Arduino sur l'address {}".format(self.addr)
    def send_allow_users(self,path):
        f=open(path,"r")
        for i in f.readline():
            #on lie les uid et on enlève le \n (retour ligne) d'ou le [:2]
            self.write_i2c_block_data(self.addr,1,i[:2].encode())
        self.write_byte(self.addr,2)
    def get_status(self):
        self.write_byte(self.addr,0)
        retour=self.dico.get(self.read_byte(self.addr))
        if(retour!=None): retour()

arduino=Arduino(0x12)
arduino.get_status()
