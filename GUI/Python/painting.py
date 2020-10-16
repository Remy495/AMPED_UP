from PyQt5 import QtGui
from PyQt5.QtWidgets import QApplication, QMainWindow
import sys
from PyQt5.QtGui import QPainter, QBrush, QPen
from PyQt5.QtCore import Qt
class Window(QMainWindow):
	def __init__(self):
		super().__init__()
		self.title="Drawing"
		self.top = 150
		self.left=150
		self.width=500
		self.height=500
		self.InitWindow()
	def InitWindow(self):
		self.setWindowTitle(self.title)
		self.setGeometry(self.top,self.left,self.width,self.height)
		self.show()
	def paintEvent(self,event):
		#All Painting should be done in here
		painter  = QPainter(self)
		painter.setPen(QPen(Qt.black,4,Qt.SolidLine))
		painter.drawRect(40,40,200,300)
		painter.drawRect(40,40,200,50)
		painter.setBrush(QBrush(Qt.blue,Qt.SolidPattern))
		painter.drawEllipse(10,100,50,50)
		painter.drawEllipse(220,100,50,50)

app=QApplication(sys.argv)
window = Window()
sys.exit(app.exec_())
