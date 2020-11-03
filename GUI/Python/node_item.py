import sys
from PyQt5 import QtCore, QtGui, QtWidgets
from drawing_constants import *



class Node(QtWidgets.QGraphicsItem):

	def __init__(self, title="", inputs=None, outputs=None, x=0, y=0):
		super(Node,self).__init__()
		self.width = DrawingConstants.NODE_WIDTH
		self.height = DrawingConstants.NODE_BASE_HEIGHT
		self.title = title
		self.inputs = [ConnectionPoint(inputTitle, self, False) for inputTitle in inputs]
		self.outputs = [ConnectionPoint(outputTitle, self, True) for outputTitle in outputs]
		self.setFlags(QtWidgets.QGraphicsItem.ItemIsMovable | QtWidgets.QGraphicsItem.ItemIsSelectable)
		self.setPos(QtCore.QPoint(x,y))

		self.rebuild()


	def boundingRect(self):
		totalWidth = self.width + DrawingConstants.CONNECTION_POINT_DIAMETER
		totalLeftEdge = -DrawingConstants.CONNECTION_POINT_DIAMETER / 2
		return QtCore.QRectF(totalLeftEdge, 0, totalWidth, self.height)

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
		painter.drawRect(0,0,self.width,self.height)

		# Create pen for drawing text
		textPen = QtGui.QPen(DrawingConstants.NODE_TEXT_COLOR, DrawingConstants.NODE_OUTLINE_WIDTH, QtCore.Qt.SolidLine)

		# Paint the title for the node
		titleBox = QtCore.QRectF(0, 0, self.width, DrawingConstants.NODE_BASE_HEIGHT)
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

	def mouseMoveEvent(self, e):
		super(Node, self).mouseMoveEvent(e)
		if e.buttons() & QtCore.Qt.LeftButton:
			for connectionPoint in self.connectionPoints:
				connectionPoint.updateConnections()
		



class ConnectionPoint:

	def __init__(self, title, owner, isOutput, xRelative = 0, yRelative = 0):
		self.title = title
		self.owner = owner
		self.xRelative = xRelative
		self.yRelative = yRelative
		self.connections = []

	@property
	def bubbleRect(self):
		size = DrawingConstants.CONNECTION_POINT_DIAMETER

		return QtCore.QRectF(self.xRelative, self.yRelative, size, size)

	@property
	def leftAlignedTextRect(self):
		width = self.owner.width / 2 - DrawingConstants.CONNECTION_POINT_DIAMETER / 2 - DrawingConstants.CONNECTION_POINT_PADDING
		height = DrawingConstants.CONNECTION_POINT_DIAMETER
		x = self.xRelative + DrawingConstants.CONNECTION_POINT_DIAMETER + DrawingConstants.CONNECTION_POINT_PADDING
		y = self.yRelative

		return QtCore.QRectF(x, y, width, height)

	@property
	def rightAlignedTextRect(self):
		width = self.owner.width / 2 - DrawingConstants.CONNECTION_POINT_DIAMETER / 2 - DrawingConstants.CONNECTION_POINT_PADDING
		height = DrawingConstants.CONNECTION_POINT_DIAMETER
		x = self.xRelative - DrawingConstants.CONNECTION_POINT_PADDING - width
		y = self.yRelative

		return QtCore.QRectF(x, y, width, height)

	def registerConnection(self, connection):
		# If this is an input and there is already a connection, remove it
		if self.connections and not self.isOutput:
			self.connections.clear()

		self.connections.append(connection)

	def unregisterConnection(self, connection):
		self.connections.remove(connection)

	def updateConnections(self):
		for connection in self.connections:
			connection.update()



class Connection(QtWidgets.QGraphicsItem):

	def __init__(self, input, output):
		super(Connection,self).__init__()
		self.input = input
		input.registerConnection(self)

		self.output = output
		output.registerConnection(self)

		self.rebuildEndpoints()

		self.setZValue(-1)

	@property
	def inputNode(self):
		return self.input.owner

	@property
	def outputNode(self):
		return self.output.owner

	def rebuildEndpoints(self):
		self.inputPoint = self.mapFromItem(self.inputNode, self.input.bubbleRect.center())
		self.outputPoint = self.mapFromItem(self.outputNode, self.output.bubbleRect.center())

	def boundingRect(self):
		x = min(self.inputPoint.x(), self.outputPoint.x())
		y = min(self.inputPoint.y(), self.outputPoint.y())
		width = abs(self.inputPoint.x() - self.outputPoint.x())
		height = abs(self.inputPoint.y() - self.outputPoint.y())

		return QtCore.QRectF(x, y, width, height)

	def paint(self, painter, option, widget=None):
		self.rebuildEndpoints()

		borderPen = QtGui.QPen(DrawingConstants.NODE_OUTLINE_COLOR, DrawingConstants.NODE_OUTLINE_WIDTH, QtCore.Qt.SolidLine)
		painter.setPen(borderPen)
		painter.drawLine(self.inputPoint, self.outputPoint)

	# FIXME: There is a graphical glitch where, when you move a node fast, you sometimes get gaps in the connection line
	#        that persist when draggin stops. This probably happens because the bounding box only contains where the
	#        line was last drawn, not where it is about to be drawn. 




if __name__=='__main__':
	import sys
	app =QtWidgets.QApplication(sys.argv)
	scene = QtWidgets.QGraphicsScene()
	view = QtWidgets.QGraphicsView(scene)
	node1 = Node('Node1',["first","second","third","fourth"],["output"],40,40)
	node2 = Node('Node2',["input"],["first", "second"],500,500)
	connection = Connection(node2.inputs[0], node1.outputs[0])
	scene.addItem(node1)
	scene.addItem(node2)
	scene.addItem(connection)
	view.show()
	sys.exit(app.exec_())


