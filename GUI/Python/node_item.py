import sys
from PyQt5 import QtCore, QtGui, QtWidgets

class Node(QtWidgets.QGraphicsItem):
	def __init__(self,title=None,inputs=None,x=0,y=0):
		super(Node,self).__init__()
		self.x=x
		self.y=y
		self.title=title
		self.inputs=inputs
		self.outputs=[1,2]
		self.setFlags(QtWidgets.QGraphicsItem.ItemIsMovable 
			| QtWidgets.QGraphicsItem.ItemIsSelectable)
		self.setPos(QtCore.QPoint(x,y))
	def boundingRect(self):
		return QtCore.QRectF(self.x,self.y,500,500)
	def paint(self, painter, option, widget=None):
		painter.setPen(QtGui.QPen(QtCore.Qt.black,4,QtCore.Qt.SolidLine))
		length=100*len(self.inputs)+50
		painter.drawRect(self.x,self.y,200,length)
		painter.drawRect(self.x,self.y,200,50)
		painter.setBrush(QtGui.QBrush(QtCore.Qt.blue,QtCore.Qt.SolidPattern))
		out_space=(length+50)/(2*len(self.outputs))
		for i in range(len(self.inputs)):
			painter.drawEllipse(self.x-30,self.y+100+100*i,50,50)
		for i in range(len(self.outputs)):
			painter.drawEllipse(self.x+180,self.y+int(out_space+out_space*i),50,50)
	def mouseMoveEvent(self, e):
		if e.buttons() & QtCore.Qt.LeftButton:
			super(Node,self).mouseMoveEvent(e)
		if e.buttons() & QtCore.Qt.RightButton:
			self.setRect(QtCore.QRectF(QtCore.QPoint(), e.pos()).normalized())

if __name__=='__main__':
	import sys
	app =QtWidgets.QApplication(sys.argv)
	scene = QtWidgets.QGraphicsScene()
	view = QtWidgets.QGraphicsView(scene)
	scene.addItem(Node('Node1',[0,1,2,3],40,40))
	scene.addItem(Node('Node2',[0,1],500,500))
	view.show()
	sys.exit(app.exec_())


