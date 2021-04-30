import sys

from PyQt5.QtWidgets import QApplication, QWidget,QGridLayout, QPushButton, QLineEdit,QLabel,QMainWindow,QDial
from PyQt5.QtCore import *
from PyQt5.QtGui import *
import copy
import os.path
import KnobValues

from AdvancedUI import AdvancedGuiWindow

def presetFilename(presetIndex):
    return os.path.join("presets", str(presetIndex + 1) + ".prst")

class basicUI(QWidget):
    def __init__(self):
        super().__init__()

        self.advancedWindow = AdvancedGuiWindow(self)

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

        self.advancedGuiButton = QPushButton('Advanced...')
        self.advancedGuiButton.clicked.connect(self.openAdvancedGui)

        self.setWindowTitle("Amped Up Remote")
            

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
        self.layout.addWidget(self.advancedGuiButton)
        
    def sendButtonClicked(self,preset=0):
        send=self.sender()
        tosend = []
        tosend=self.presets[send.preset]
        self.advancedWindow.upload()
        print(tosend)
    def readButtonClicked(self,preset=0):
        send=self.sender()

        values = [value for value in KnobValues.knobValues]

        for i in range(len(values)):
            self.setKnobPos(i, values[i] * 11 + 1)
            self.advancedWindow.setKnobPos(i, values[i] * 11 + 1)
    def saveButtonClicked(self,preset=0):

        send=self.sender()
        for i in range(len(self.valueFields)):
            self.presets[send.preset][i][1] = float(self.valueFields[i][1].value)

        presetData = self.advancedWindow.serialize()
        f = open(presetFilename(send.preset), "wb")
        f.write(presetData)
        f.close()


    def loadButtonClicked(self,preset=0):
        send=self.sender()
        for i in range(len(self.valueFields)):
            self.valueFields[i][1].value = self.presets[send.preset][i][1]
            self.valueFields[i][1].setText(str("{:.2f}".format((float(self.presets[send.preset][i][1])))))
        for i in range(len(self.dials)):
            self.dials[i].setValue(int(100*self.presets[send.preset][i][1]/12))

        f = open(presetFilename(send.preset), "rb")
        presetData = f.read()
        f.close()

        self.advancedWindow.deserialize(presetData)

    def sliderMove(self):
        sender = self.sender()
        target=0
        for i in range(len(self.dials)):
            if sender.value() == self.dials[i].value():
                target = i
        self.valueFields[target][1].value = float(12*sender.value()/100)
        self.valueFields[target][1].setText(str(float(12*sender.value()/100)))
        self.advancedWindow.setKnobPos(target, self.valueFields[target][1].value)
        
    def textChanged(self):
        send=self.sender()
        target=0
        send.value=float(send.text())
        send.setText(str("{:.2f}".format(send.value)))

        for i in range(len(self.valueFields)):
            if(self.valueFields[i][1].paramater == send.paramater):
                target=i
        self.dials[target].setValue(int(100*(send.value)/12))

    def openAdvancedGui(self):
        self.advancedWindow.show()

    def setKnobPos(self, knobIndex, pos):
        if pos is None:
            self.dials[knobIndex].setEnabled(False)
            self.valueFields[knobIndex][1].setEnabled(False)
        else:
            self.dials[knobIndex].setEnabled(True)
            self.valueFields[knobIndex][1].setEnabled(True)

            self.dials[knobIndex].setValue(int(100*(pos)/12))
            self.valueFields[knobIndex][1].value = float(pos)
            self.valueFields[knobIndex][1].setText("%.2f" % pos)

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