import sys

from PyQt5.QtWidgets import QApplication, QWidget,QGridLayout, QPushButton, QLineEdit,QLabel,QMainWindow,QDial
from PyQt5.QtCore import *
from PyQt5.QtGui import *
import copy

class basicUI(QWidget):
    def __init__(self):
        super().__init__()
        self.layout = QGridLayout()
        self.knobs=[["Volume",0.0],["Drive",0.0],["Treble",0.0],["Bass",0.0],["Middle",0.0],["Master",0.0],["Reverb",0.0],["Presence",0.0]]
        self.buttons=[]
        self.dials = []
        self.valueFields = []
        self.presets = [copy.deepcopy(self.knobs),copy.deepcopy(self.knobs),copy.deepcopy(self.knobs),copy.deepcopy(self.knobs),copy.deepcopy(self.knobs),copy.deepcopy(self.knobs),copy.deepcopy(self.knobs),copy.deepcopy(self.knobs),copy.deepcopy(self.knobs),copy.deepcopy(self.knobs)]
        for i in range(10):
            self.buttons.append([saveButton(i,'Save'),loadButton(i,'Load')])
        for i in range(len(self.knobs)):
            self.valueFields.append([QLabel(self.knobs[i][0]),valueField(str(self.knobs[i][1]),self.knobs[i][0])])
        self.labels = [QLabel('Preset 1: '),QLabel('Preset 2: '),QLabel('Preset 3: '),QLabel('Preset 4: '),QLabel('Preset 5: '),QLabel('Preset 6: '),QLabel('Preset 7: '),QLabel('Preset 8: '),QLabel('Preset 9: '),QLabel('Preset 10: ')] 
        for i in range(len(self.buttons)):
            for j in range(len(self.buttons[i])):
                if j == 0:
                    self.buttons[i][j].clicked.connect(self.saveButtonClicked)
                if j == 1:
                    self.buttons[i][j].clicked.connect(self.loadButtonClicked)
        for i in range(len(self.valueFields)):
            self.valueFields[i][1].editingFinished.connect(self.textChanged)
        for i in range(len(knobs)):
            self.dials.append(Qdial())

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
        for i in range(len(dials)):
            self.layout.addWidget(self.dials[i],11,i)
    def saveButtonClicked(self,preset=0):
        send=self.sender()
        ##print("clicked save button "+ str(send.preset))
        for i in range(len(self.valueFields)):
            self.presets[send.preset][i][1] = float(self.valueFields[i][1].value)
        ##print(self.presets)
    def loadButtonClicked(self,preset=0):
        send=self.sender()
        for i in range(len(self.valueFields)):
            self.valueFields[i][1].value = self.presets[send.preset][i][1]
            self.valueFields[i][1].setText(str(float(self.presets[send.preset][i][1])))
        ##print("Clicked Load")

    def textChanged(self):
        send=self.sender()
        send.value=float(send.text())
        send.setText(str(send.value))

class saveButton(QPushButton):
    def __init__(self,preset=0,text="",parent=None):
        super().__init__(text, parent)
        self.preset=preset

class loadButton(QPushButton):
    def __init__(self,preset=0,text="",parent=None):
        super().__init__(text,parent)
        self.preset=preset
        ##Call BT Send Functions Here
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