import sys
from PyQt5 import QtCore, QtGui, QtWidgets
from drawing_constants import *

class Node(QtWidgets.QGraphicsItem):

	def __init__(self,title=None,inputs=None,x=0,y=0):
		super(Node,self).__init__()
		self.x=x
		self.y=y
		self.width = DrawingConstants.NODE_WIDTH
		self.baseHeight = DrawingConstants.NODE_BASE_HEIGHT
		self.title=title
		self.inputs=inputs
		self.outputs=[1,2]
		self.setFlags(QtWidgets.QGraphicsItem.ItemIsMovable | QtWidgets.QGraphicsItem.ItemIsSelectable)
		self.setPos(QtCore.QPoint(x,y))


	def boundingRect(self):
		totalWidth = self.width + DrawingConstants.CONNECTION_POINT_DIAMETER
		totalLeftEdge = self.x - DrawingConstants.CONNECTION_POINT_DIAMETER / 2
		return QtCore.QRectF(totalLeftEdge, self.y, totalWidth, self.height)

	@property
	def height(self):
		stackedConnectionCount = max(len(self.inputs), len(self.outputs))
		connectionPointSpacing = DrawingConstants.CONNECTION_POINT_DIAMETER + DrawingConstants.CONNECTION_POINT_PADDING
		return self.baseHeight + stackedConnectionCount * connectionPointSpacing + DrawingConstants.CONNECTION_POINT_PADDING

	def paint(self, painter, option, widget=None):

		# Set pen for drawing outlines; color based on whether or not the node is selected
		if option.state & QtWidgets.QStyle.State_Selected:
			painter.setPen(QtGui.QPen(DrawingConstants.HIGHLIGHT_COLOR, DrawingConstants.NODE_OUTLINE_WIDTH, QtCore.Qt.SolidLine))
		else:
			painter.setPen(QtGui.QPen(DrawingConstants.NODE_OUTLINE_COLOR, DrawingConstants.NODE_OUTLINE_WIDTH, QtCore.Qt.SolidLine))

		# Set brush for painting the main body of the node
		painter.setBrush(QtGui.QBrush(DrawingConstants.NODE_BACKGROUND_COLOR, QtCore.Qt.SolidPattern))

		# Paint main body
		painter.drawRect(self.x,self.y,self.width,self.height)
		painter.drawRect(self.x,self.y,self.width, self.baseHeight)

		# Set brush for painting the node's connection points
		painter.setBrush(QtGui.QBrush(DrawingConstants.CONNECTION_POINT_COLOR, QtCore.Qt.SolidPattern))

		# Find the y coordinate at which the first bubble should be start
		connectionStackTopY = self.y + self.baseHeight + DrawingConstants.CONNECTION_POINT_PADDING

		# Draw all of the inputs
		connectionPointY = int(connectionStackTopY)
		connectionPointX = int(self.x - DrawingConstants.CONNECTION_POINT_DIAMETER / 2)
		for i in range(len(self.inputs)):
			painter.drawEllipse(connectionPointX, connectionPointY, DrawingConstants.CONNECTION_POINT_DIAMETER, DrawingConstants.CONNECTION_POINT_DIAMETER)
			connectionPointY += DrawingConstants.CONNECTION_POINT_DIAMETER + DrawingConstants.CONNECTION_POINT_PADDING

		# Draw all of the outputs
		connectionPointY = int(connectionStackTopY)
		connectionPointX = int(self.x + self.width - DrawingConstants.CONNECTION_POINT_DIAMETER / 2)
		for i in range(len(self.outputs)):
			painter.drawEllipse(connectionPointX, connectionPointY, DrawingConstants.CONNECTION_POINT_DIAMETER, DrawingConstants.CONNECTION_POINT_DIAMETER)
			connectionPointY += DrawingConstants.CONNECTION_POINT_DIAMETER + DrawingConstants.CONNECTION_POINT_PADDING


if __name__=='__main__':
	import sys
	app =QtWidgets.QApplication(sys.argv)
	scene = QtWidgets.QGraphicsScene()
	view = QtWidgets.QGraphicsView(scene)
	scene.addItem(Node('Node1',[0,1,2,3],40,40))
	scene.addItem(Node('Node2',[0,1],500,500))
	view.show()
	sys.exit(app.exec_())


