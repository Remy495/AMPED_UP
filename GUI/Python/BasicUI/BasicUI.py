import sys

from PyQt5.QtWidgets import QApplication, QWidget,QGridLayout, QPushButton, QLineEdit,QLabel,QMainWindow,QDial
from PyQt5.QtCore import *
from PyQt5.QtGui import *
import copy

class basicUI(QWidget):
    def __init__(self):
        super().__init__()
        self.layout = QGridLayout()
        self.knobs=[["Volume",1.0],["Drive",1.0],["Treble",1.0],["Bass",1.0],["Middle",1.0],["Master",1.0],["Reverb",1.0],["Presence",1.0]]
        self.buttons=[]
        self.dials = []
        self.valueFields = []
        self.presets = [copy.deepcopy(self.knobs),copy.deepcopy(self.knobs),copy.deepcopy(self.knobs),copy.deepcopy(self.knobs),copy.deepcopy(self.knobs),copy.deepcopy(self.knobs),copy.deepcopy(self.knobs),copy.deepcopy(self.knobs),copy.deepcopy(self.knobs),copy.deepcopy(self.knobs)]
        for i in range(10):
            self.buttons.append([saveButton(i,'Save'),loadButton(i,'Load'),sendButton(i,'Send'),readButton(i,'Read')])
        for i in range(len(self.knobs)):
            self.valueFields.append([QLabel(self.knobs[i][0]),valueField(str(self.knobs[i][1]),self.knobs[i][0])])
        self.labels = [QLabel('Preset 1: '),QLabel('Preset 2: '),QLabel('Preset 3: '),QLabel('Preset 4: '),QLabel('Preset 5: '),QLabel('Preset 6: '),QLabel('Preset 7: '),QLabel('Preset 8: '),QLabel('Preset 9: '),QLabel('Preset 10: ')] 
        for i in range(len(self.buttons)):
            for j in range(len(self.buttons[i])):
                if j == 0:
                    self.buttons[i][j].clicked.connect(self.saveButtonClicked)
                if j == 1:
                    self.buttons[i][j].clicked.connect(self.loadButtonClicked)
                if j==2:
                    self.buttons[i][j].clicked.connect(self.sendButtonClicked)
                if j==3:
                    self.buttons[i][j].clicked.connect(self.readButtonClicked)
        for i in range(len(self.valueFields)):
            self.valueFields[i][1].editingFinished.connect(self.textChanged)
        for i in range(len(self.knobs)):
            self.dials.append(QDial())
            self.dials[i].setNotchesVisible(True)
            self.dials[i].setValue(0)
            self.dials[i].setNotchTarget(9)
            self.dials[i].valueChanged.connect(self.sliderMove)
            self.dials[i].setMinimum(int(1*100/12))
            self.dials[i].setMaximum(int(12*100/12))
            

    def buildLayout(self):
        k=0
        for i in range(len(self.labels)):
            self.layout.addWidget(self.labels[i],i,0)
        for i in range(len(self.buttons)):
            for j in range(len(self.buttons[i])):
                self.layout.addWidget(self.buttons[i][j],i,j+1)
        for i in range(len(self.valueFields)):
            for j in range(len(self.valueFields[i])):
                self.layout.addWidget(self.valueFields[i][j],10,k)
                k+=1
        for i in range(len(self.dials)):
            self.layout.addWidget(self.dials[i],11,2*i+1)
    def sendButtonClicked(self,preset=0):
        send=self.sender()
        tosend = []
        tosend=self.presets[send.preset]
        ##CASEY: Here is where you would do anything you need to send this currently printed preset over bluetooth
        print(tosend)
    def readButtonClicked(self,preset=0):
        send=self.sender()
        ##CASEY: Here is where you would read some values back into a preset from the main board over bluetooth. I will write everything but the receiving portion
        values = [0.00,0.5,0.5,0.5,0.5,0.5,0.5,1.0]##Read them into this
        for i in range(len(values)):
            self.presets[send.preset][i][1] = 1 + values[i]*12
    def saveButtonClicked(self,preset=0):
        send=self.sender()
        for i in range(len(self.valueFields)):
            self.presets[send.preset][i][1] = float(self.valueFields[i][1].value)
    def loadButtonClicked(self,preset=0):
        send=self.sender()
        for i in range(len(self.valueFields)):
            self.valueFields[i][1].value = self.presets[send.preset][i][1]
            self.valueFields[i][1].setText(str("{:.2f}".format((float(self.presets[send.preset][i][1])))))
        for i in range(len(self.dials)):
            self.dials[i].setValue(int(100*self.presets[send.preset][i][1]/12))
    def sliderMove(self):
        sender = self.sender()
        target=0
        for i in range(len(self.dials)):
            if sender.value() == self.dials[i].value():
                target = i
        self.valueFields[target][1].value = float(12*sender.value()/100)
        self.valueFields[target][1].setText(str(float(12*sender.value()/100)))
        
    def textChanged(self):
        send=self.sender()
        target=0
        send.value=float(send.text())
        send.setText(str("{:.2f}".format(send.value)))

        for i in range(len(self.valueFields)):
            if(self.valueFields[i][1].paramater == send.paramater):
                target=i
        self.dials[target].setValue(int(100*(send.value)/12))

class sendButton(QPushButton):
    def __init__(self,preset=0,text="",parent=None):
        super().__init__(text, parent)
        self.preset=preset
class readButton(QPushButton):
    def __init__(self,preset=0,text="",parent=None):
        super().__init__(text, parent)
        self.preset=preset

class saveButton(QPushButton):
    def __init__(self,preset=0,text="",parent=None):
        super().__init__(text, parent)
        self.preset=preset

class loadButton(QPushButton):
    def __init__(self,preset=0,text="",parent=None):
        super().__init__(text,parent)
        self.preset=preset
class valueField(QLineEdit):
    def __init__(self,value='0',paramater = "",parent=None):
        self.value=value
        super().__init__(self.value,parent)
        self.paramater=paramater
        

app = QApplication(sys.argv)
window = basicUI()
window.buildLayout()
window.setLayout(window.layout)
window.show()
sys.exit(app.exec_())