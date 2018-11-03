import Tkinter
import ttk

import time
#from ttk import *

#   Python 3
#from tkinter import *
#from tkinter.ttk import *

from datetime import datetime
import serial

root = Tkinter.Tk()
root.title('KPD&LCD')

class ser:
    ser = None
    
    def connect(self, port, baudrate = 57600):
            '''
            self.ser = serial.Serial(
                port=port,
                baudrate=9600,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                bytesize=serial.EIGHTBITS,
                rtscts=0
            )
            '''
            self.ser = serial.Serial()
            self.ser.port = port
            self.ser.baudrate = baudrate
            self.ser.parity=serial.PARITY_NONE
            self.ser.stopbits=serial.STOPBITS_ONE
            self.ser.bytesize=serial.EIGHTBITS
            #self.ser.timeout = 1
            #self.ser.setDTR(False)
            self.ser.open()
        
            #self.ser.setDTR(False)
            print('Connected to: ' + self.ser.portstr)
            return True
            
    def disconnect(self):
        if self.ser != None:
            self.ser.close()
        self.ser = None
        print('Disconnected')
        
             
    def sendnl(self, cmd):
        if self.ser:
            #self.ser.flushInput() #flush input buffer, discarding all its contents
            #self.ser.flushOutput()#flush output buffer, aborting current output 
            data = cmd
            #print('> ' + data)
            self.ser.write(data + '\r\n') 
        else:
            pass
        
    def read(self):
        if self.ser:
            #print(self.ser.read())
            #print('.')
            if self.ser.inWaiting() > 0:
                data = self.ser.readline()
                #print('< '  + data)
                return data
        return None
            

g = ser()

def disconnect():
    g.disconnect()
    commStatus.set('DISCONNECTED')

def connect():
    if g.connect('COM42', 57600):
        commStatus.set('CONNECTED')
    
    
def button1():
    g.sendnl('KPD:1')
    readVar();    
def button2():
    g.sendnl('KPD:2') 
    readVar();  
def button3():
    g.sendnl('KPD:3')
    readVar();   
def buttonA():
    g.sendnl('KPD:A')
    readVar(); 
def button4():
    g.sendnl('KPD:4')
    readVar();    
def button5():
    g.sendnl('KPD:5')
    readVar(); 
def button6():
    g.sendnl('KPD:6')
    readVar(); 
def buttonB():
    g.sendnl('KPD:B')
    readVar(); 
def button7():
    g.sendnl('KPD:7')
    readVar();    
def button8():
    g.sendnl('KPD:8')
    readVar(); 
def button9():
    g.sendnl('KPD:9')
    readVar(); 
def buttonC():
    g.sendnl('KPD:C')
    readVar();
def buttonEsc():
    g.sendnl('KPD:-')
    readVar();    
def button0():
    g.sendnl('KPD:0')
    readVar(); 
def buttonEnt():
    g.sendnl('KPD:+')
    readVar(); 
def buttonD():
    g.sendnl('KPD:D')
    readVar(); 
    
 
def readVar():
    '''
    data = g.read()
    temp.set(g.getValue('TA=', data))
    humi.set(g.getValue('HA=', data))
    temp2.set(g.getValue('TM=', data))
    ec.set(g.getValue('EC=', data))
    ph.set(g.getValue('PH=', data))
    level.set(g.getValue('LV=', data))
    power.set(g.getValue('PW=', data))
    light.set(g.getValue('LA=', data))

    lightControl.set(g.getControlValue('L:', data)[0])
    heaterControl.set(g.getControlValue('H:', data)[0])
    fanControl.set(g.getControlValue('F:', data)[0])
    cyclerControl.set(g.getControlValue('L:', data)[0])
    lightMode.set(g.getControlValue('L:', data)[1])
    heaterMode.set(g.getControlValue('H:', data)[1])
    fanMode.set(g.getControlValue('F:', data)[1])
    cyclerMode.set(g.getControlValue('C:', data)[1])    
    '''                                
def readParam():
    '''
    #lightMode.set(g.getParamInt(4))
    lightOnHour.set(g.getParamInt(8))
    lightOnMin.set(g.getParamInt(12))  
    lightOffHour.set(g.getParamInt(16))
    lightOffMin.set(g.getParamInt(20))  
    #heaterMode.set(g.getParamInt(24))
    heaterOnTemp.set(g.getParamFloat(28))
    heaterOffTemp.set(g.getParamFloat(32))  
    #fanMode.set(g.getParamInt(36))
    fanOnTemp.set(g.getParamFloat(40))
    fanOffTemp.set(g.getParamFloat(44))
    #cyclerMode.set(g.getParamInt(48))
    cyclerOnMin.set(g.getParamInt(52))
    cyclerOnSec.set(g.getParamInt(56))  
    cyclerOffMin.set(g.getParamInt(60))
    cyclerOffSec.set(g.getParamInt(64))
    
    tempHigh.set(g.getParamFloat(68))
    tempLow.set(g.getParamFloat(72))    
    levelHigh.set(g.getParamFloat(76))
    levelLow.set(g.getParamFloat(80)) 
    phHigh.set(g.getParamFloat(84))
    phLow.set(g.getParamFloat(88))
    phLowX.set(g.getParamInt(92))
    phHighX.set(g.getParamInt(96))
    phLowY.set(g.getParamFloat(100))
    phHighY.set(g.getParamFloat(104))
    ecHigh.set(g.getParamFloat(108))
    ecLow.set(g.getParamFloat(112))
    ecLowX.set(g.getParamInt(116))
    ecHighX.set(g.getParamInt(120))
    ecLowY.set(g.getParamFloat(124))
    ecHighY.set(g.getParamFloat(128))    
    
    heaterOnTempNight.set(g.getParamFloat(132))
    heaterOffTempNight.set(g.getParamFloat(136))  
    fanOnTempNight.set(g.getParamFloat(140))
    fanOffTempNight.set(g.getParamFloat(144))
    
    tempHighNight.set(g.getParamFloat(148))
    tempLowNight.set(g.getParamFloat(152))    
    levelHigh.set(g.getParamFloat(156))
    levelLow.set(g.getParamFloat(160)) 
    humiHigh.set(g.getParamInt(164))
    humiLow.set(g.getParamInt(168)) 
    
    extMode.set(g.getParamInt(196) & 0xF)
    

    gsmCode.set(g.getParamInt(216))   
    gsmMode.set(g.getParamInt(220)) 
    '''
   

   




def writeParam():
    '''
    g.setParamFloat(72, tempLow.get())
    time.sleep(2)
    g.setParamFloat(68, tempHigh.get())
    time.sleep(2)
    '''
  
lcd0 = Tkinter.StringVar() 
lcd1 = Tkinter.StringVar()
lcd2 = Tkinter.StringVar() 
lcd3 = Tkinter.StringVar()
lcd4 = Tkinter.StringVar()
lcd5 = Tkinter.StringVar() 
lcd6 = Tkinter.StringVar()

commName = Tkinter.StringVar()
commSpeed = Tkinter.StringVar()
commStatus = Tkinter.StringVar()
  
mainframe = ttk.Frame(root, padding="3 3 12 12")
mainframe.grid(column=0, row=0, sticky=('N', 'W', 'E', 'S'))
mainframe.columnconfigure(0, weight=1)
mainframe.rowconfigure(0, weight=1)



row = 7

lcd0_entry = ttk.Entry(mainframe,  width=50, textvariable=lcd0)
lcd0_entry.grid(column=0, columnspan=4,row=row, sticky=('W',))
row = row + 1
lcd1_entry = ttk.Entry(mainframe, width=50, textvariable=lcd1)
lcd1_entry.grid(column=0, columnspan=4, row=row, sticky=('W',))
row = row + 1
lcd2_entry = ttk.Entry(mainframe, width=50, textvariable=lcd2)
lcd2_entry.grid(column=0, columnspan=4, row=row, sticky=('W',))
row = row + 1
lcd3_entry = ttk.Entry(mainframe, width=50, textvariable=lcd3)
lcd3_entry.grid(column=0, columnspan=4, row=row, sticky=('W',))
row = row + 1
lcd4_entry = ttk.Entry(mainframe, width=50, textvariable=lcd4)
lcd4_entry.grid(column=0, columnspan=4, row=row, sticky=('W',))
row = row + 1
lcd5_entry = ttk.Entry(mainframe, width=50, textvariable=lcd5)
lcd5_entry.grid(column=0, columnspan=4, row=row, sticky=('W',))
row = row + 1
lcd6_entry = ttk.Entry(mainframe, width=50, textvariable=lcd6)
lcd6_entry.grid(column=0, columnspan=4, row=row, sticky=('W',))
row = row + 1

ttk.Button(mainframe, text='1', command=button1).grid(column=0, row=row, sticky='W')
ttk.Button(mainframe, text='2', command=button2).grid(column=1, row=row, sticky='W')
ttk.Button(mainframe, text='3', command=button3).grid(column=2, row=row, sticky='W')
ttk.Button(mainframe, text='A', command=buttonA).grid(column=3, row=row, sticky='W')
row = row + 1
ttk.Button(mainframe, text='4', command=button4).grid(column=0, row=row, sticky='W')
ttk.Button(mainframe, text='5', command=button5).grid(column=1, row=row, sticky='W')
ttk.Button(mainframe, text='6', command=button6).grid(column=2, row=row, sticky='W')
ttk.Button(mainframe, text='B', command=buttonB).grid(column=3, row=row, sticky='W')
row = row + 1
ttk.Button(mainframe, text='7', command=button7).grid(column=0, row=row, sticky='W')
ttk.Button(mainframe, text='8', command=button8).grid(column=1, row=row, sticky='W')
ttk.Button(mainframe, text='9', command=button9).grid(column=2, row=row, sticky='W')
ttk.Button(mainframe, text='C', command=buttonC).grid(column=3, row=row, sticky='W')
row = row + 1
ttk.Button(mainframe, text='*', command=buttonEsc).grid(column=0, row=row, sticky='W')
ttk.Button(mainframe, text='0', command=button0).grid(column=1, row=row, sticky='W')
ttk.Button(mainframe, text='#', command=buttonEnt).grid(column=2, row=row, sticky='W')
ttk.Button(mainframe, text='D', command=buttonD).grid(column=3, row=row, sticky='W')
row = row + 1



ttk.Button(mainframe, text="CONNECT", command=connect).grid(column=0, row=5, sticky='W')
ttk.Button(mainframe, text="DISCONNECT", command=disconnect).grid(column=1, row=5, sticky='W')

commName_entry = ttk.Entry(mainframe,  width=50, textvariable=commName)
commName_entry.grid(column=0, columnspan=4, row=0, sticky=('W',))
commSpeed_entry = ttk.Entry(mainframe,  width=50, textvariable=commSpeed)
commSpeed_entry.grid(column=0, columnspan=4, row=1, sticky=('W',))

commStatus_entry = ttk.Entry(mainframe,  width=50, textvariable=commStatus)
commStatus_entry.grid(column=0, columnspan=4, row=4, sticky=('W',))
commStatus_entry.config(state='readonly')


commName.set('COM42')
commSpeed.set(57600)


for child in mainframe.winfo_children(): child.grid_configure(padx=5, pady=5)

#feet_entry.focus()
#root.bind('<Return>', calculate)

def task():
    data = g.read()
    while data:
        if data[0] == 'X':
            data = data.replace('\n', '')
            data = data.replace('\r', '')
            print(data)
    
        #print(data)
        if data.find('LCD0:') >= 0:
            lcd0.set(data[5:])
        if data.find('LCD1:') >= 0:
            lcd1.set(data[5:])
        if data.find('LCD2:') >= 0:
            lcd2.set(data[5:])
        if data.find('LCD3:') >= 0:
            lcd3.set(data[5:])
        if data.find('LCD4:') >= 0:
            lcd4.set(data[5:])
        if data.find('LCD5:') >= 0:
            lcd5.set(data[5:])
        if data.find('LCD6:') >= 0:
            lcd6.set(data[5:])
        data = g.read()
    root.after(1, task)


root.after(1, task)
root.mainloop()
