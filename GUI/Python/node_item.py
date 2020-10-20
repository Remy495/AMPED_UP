import sys
from PyQt5 import QtCore, QtGui, QtWidgets
from drawing_constants import *

class Node(QtWidgets.QGraphicsItem):

	def __init__(self, title="", inputs=None, outputs=None, x=0, y=0):
		super(Node,self).__init__()
		self.x=x
		self.y=y
		self.width = DrawingConstants.NODE_WIDTH
		self.height = DrawingConstants.NODE_BASE_HEIGHT
		self.title = title
		self.inputs = [ConnectionPoint(inputTitle, self) for inputTitle in inputs]
		self.outputs = [ConnectionPoint(outputTitle, self) for outputTitle in outputs]
		self.setFlags(QtWidgets.QGraphicsItem.ItemIsMovable | QtWidgets.QGraphicsItem.ItemIsSelectable)
		self.setPos(QtCore.QPoint(x,y))

		self.rebuild()


	def boundingRect(self):
		totalWidth = self.width + DrawingConstants.CONNECTION_POINT_DIAMETER
		totalLeftEdge = self.x - DrawingConstants.CONNECTION_POINT_DIAMETER / 2
		return QtCore.QRectF(totalLeftEdge, self.y, totalWidth, self.height)

	@property
	def connectionPoints(self):
		return self.inputs + self.outputs

	def rebuild(self):
		# Find the y coordinate at which the first connection point should be start
		connectionStackTopY = DrawingConstants.NODE_BASE_HEIGHT + DrawingConstants.CONNECTION_POINT_PADDING

		# Calculate positions for all of the inputs
		connectionPointY = int(connectionStackTopY)
		connectionPointX = int(-DrawingConstants.CONNECTION_POINT_DIAMETER / 2)
		for connectionPoint in self.inputs:
			connectionPoint.xRelative = connectionPointX
			connectionPoint.yRelative = connectionPointY

			connectionPointY += DrawingConstants.CONNECTION_POINT_DIAMETER + DrawingConstants.CONNECTION_POINT_PADDING

		# Adjust the height of the node to contain all of the inputs
		self.height = connectionPointY

		# Calculate positions for all of the outputs
		connectionPointY = int(connectionStackTopY)
		connectionPointX = int(self.width - DrawingConstants.CONNECTION_POINT_DIAMETER / 2)
		for connectionPoint in self.outputs:
			connectionPoint.xRelative = connectionPointX
			connectionPoint.yRelative = connectionPointY
			
			connectionPointY += DrawingConstants.CONNECTION_POINT_DIAMETER + DrawingConstants.CONNECTION_POINT_PADDING

		# Adjust the height of the node to contain all of the outputs
		if connectionPointY > self.height:
			self.height = connectionPointY


	def paint(self, painter, option, widget=None):

		# Create pen for drawing outlines; color based on whether or not the node is selected
		if option.state & QtWidgets.QStyle.State_Selected:
			borderPen = QtGui.QPen(DrawingConstants.HIGHLIGHT_COLOR, DrawingConstants.NODE_OUTLINE_WIDTH, QtCore.Qt.SolidLine)
		else:
			borderPen = QtGui.QPen(DrawingConstants.NODE_OUTLINE_COLOR, DrawingConstants.NODE_OUTLINE_WIDTH, QtCore.Qt.SolidLine)

		# Select pen/brush for painting the main body of the node
		painter.setBrush(QtGui.QBrush(DrawingConstants.NODE_BACKGROUND_COLOR, QtCore.Qt.SolidPattern))
		painter.setPen(borderPen)

		# Paint main body
		painter.drawRect(self.x,self.y,self.width,self.height)

		# Create pen for drawing text
		textPen = QtGui.QPen(DrawingConstants.NODE_TEXT_COLOR, DrawingConstants.NODE_OUTLINE_WIDTH, QtCore.Qt.SolidLine)

		# Paint the title for the node
		titleBox = QtCore.QRectF(self.x, self.y, self.width, DrawingConstants.NODE_BASE_HEIGHT)
		painter.drawRect(titleBox)

		painter.setPen(textPen)
		painter.setFont(QtGui.QFont(DrawingConstants.LABEL_FONT, DrawingConstants.NODE_TITLE_FONTSIZE))
		painter.drawText(titleBox, QtCore.Qt.AlignCenter, self.title)


		# Select pen/brush for painting the node's connection points
		painter.setBrush(QtGui.QBrush(DrawingConstants.CONNECTION_POINT_COLOR, QtCore.Qt.SolidPattern))
		painter.setPen(borderPen)

		# Draw the circles for all of the connection points
		for connectionPoint in self.connectionPoints:
			painter.drawEllipse(connectionPoint.bubbleRect)

		# Select the brush for drawing text
		painter.setPen(textPen)

		# Draw labels for inputs
		for connectionPoint in self.inputs:
			painter.drawText(connectionPoint.leftAlignedTextRect, QtCore.Qt.AlignLeft, connectionPoint.title)

		# Draw labels for outputs
		for connectionPoint in self.outputs:
			painter.drawText(connectionPoint.rightAlignedTextRect, QtCore.Qt.AlignRight, connectionPoint.title)


class ConnectionPoint:

	def __init__(self, title, owner, xRelative = 0, yRelative = 0):
		self.title = title
		self.owner = owner
		self.xRelative = xRelative
		self.yRelative = yRelative

	@property
	def xAbsolute(self):
		return self.xRelative + self.owner.x

	@property
	def yAbsolute(self):
		return self.yRelative + self.owner.y

	@property
	def bubbleRect(self):
		size = DrawingConstants.CONNECTION_POINT_DIAMETER

		return QtCore.QRectF(self.xAbsolute, self.yAbsolute, size, size)

	@property
	def leftAlignedTextRect(self):
		width = self.owner.width / 2 - DrawingConstants.CONNECTION_POINT_DIAMETER / 2 - DrawingConstants.CONNECTION_POINT_PADDING
		height = DrawingConstants.CONNECTION_POINT_DIAMETER
		x = self.xAbsolute + DrawingConstants.CONNECTION_POINT_DIAMETER + DrawingConstants.CONNECTION_POINT_PADDING
		y = self.yAbsolute

		return QtCore.QRectF(x, y, width, height)

	@property
	def rightAlignedTextRect(self):
		width = self.owner.width / 2 - DrawingConstants.CONNECTION_POINT_DIAMETER / 2 - DrawingConstants.CONNECTION_POINT_PADDING
		height = DrawingConstants.CONNECTION_POINT_DIAMETER
		x = self.xAbsolute - DrawingConstants.CONNECTION_POINT_PADDING - width
		y = self.yAbsolute

		return QtCore.QRectF(x, y, width, height)



if __name__=='__main__':
	import sys
	app =QtWidgets.QApplication(sys.argv)
	scene = QtWidgets.QGraphicsScene()
	view = QtWidgets.QGraphicsView(scene)
	scene.addItem(Node('Node1',["first","second","third","fourth"],["output"],40,40))
	scene.addItem(Node('Node2',["input"],["first", "second"],500,500))
	view.show()
	sys.exit(app.exec_())


