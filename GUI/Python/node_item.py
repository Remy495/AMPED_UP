from PyQt5 import QtCore, QtGui, QtWidgets
from drawing_constants import *
from connection_item import *
from slider_item import *

	

class Node(QtWidgets.QGraphicsItem):

	def __init__(self, title="", inputs=None, outputs=None, isInPallet = True, x=0, y=0):
		super(Node,self).__init__()
		self.width = DrawingConstants.NODE_WIDTH
		self.height = DrawingConstants.NODE_BASE_HEIGHT
		self.title = title

		self.inputTable = inputs
		self.outputTable = outputs

		self.x = x
		self.y = y

		# Calculate the positions at which each of the connection points should be drawn (relative to the node)
		self.rebuild()

		# Set the node as draggable and selectable
		self.setFlags(QtWidgets.QGraphicsItem.ItemIsMovable | QtWidgets.QGraphicsItem.ItemIsSelectable)
		self.setAcceptHoverEvents(True)

		# Move the node to the specified position
		self.setPos(QtCore.QPoint(x,y))

		self.isInPallet = isInPallet
		if isInPallet:
			self.setZValue(-1)


		self.inputTextItems = []


	def boundingRect(self):
		totalWidth = self.width + DrawingConstants.CONNECTION_POINT_DIAMETER
		totalLeftEdge = -DrawingConstants.CONNECTION_POINT_DIAMETER / 2
		return QtCore.QRectF(totalLeftEdge, 0, totalWidth, self.height)

	@property
	def connectionPoints(self):
		return self.inputs + self.outputs

	def rebuild(self):
		# Reset the lists of connection points
		self.inputs = []
		self.outputs = []

		# Reset the list of connection point sliders
		self.inputTextItems = []

		# Find the y coordinate at which the first connection point should be start
		connectionStackTopY = DrawingConstants.NODE_BASE_HEIGHT + DrawingConstants.NODE_PADDING

		# Calculate positions for all of the inputs
		connectionPointY = int(connectionStackTopY)
		connectionPointX = int(-DrawingConstants.CONNECTION_POINT_DIAMETER / 2)
		for inputTitle, inputRange in self.inputTable.items():
			connectionPoint = ConnectionPoint(inputTitle, self, False)

			connectionPoint.xRelative = connectionPointX
			connectionPoint.yRelative = connectionPointY

			if self.scene() is not None:
				inputTextItem = Slider(inputRange, self)
				textItemX = self.width / 2
				textItemY = connectionPointY
				inputTextItem.setPos(textItemX, textItemY)
				self.inputTextItems.append(inputTextItem)

				connectionPoint.textBox = inputTextItem
			
			self.inputs.append(connectionPoint)

			connectionPointY += DrawingConstants.CONNECTION_POINT_DIAMETER + DrawingConstants.NODE_PADDING

		# Adjust the height of the node to contain all of the inputs
		self.height = connectionPointY

		# Calculate positions for all of the outputs
		connectionPointX = int(self.width - DrawingConstants.CONNECTION_POINT_DIAMETER / 2)
		for outputTitle in self.outputTable:
			connectionPoint = ConnectionPoint(outputTitle, self, True)

			connectionPoint.xRelative = connectionPointX
			connectionPoint.yRelative = connectionPointY

			self.outputs.append(connectionPoint)
			
			connectionPointY += DrawingConstants.CONNECTION_POINT_DIAMETER + DrawingConstants.NODE_PADDING

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
		if self.scene().activeConnection is None:
			super(Node, self).mouseMoveEvent(e)
			if e.buttons() & QtCore.Qt.LeftButton:
				for connectionPoint in self.connectionPoints:
					connectionPoint.updateConnections()

	def mousePressEvent(self, e):
		# Determine if any of the connection points were clicked on

		if self.isInPallet:
			newNode = Node(self.title, self.inputTable, self.outputTable, True, self.x, self.y)
			self.isInPallet = False
			self.setZValue(0)
			self.scene().addNode(newNode)


		selectedConnectionPoint = None
		for connectionPoint in self.connectionPoints:
			if connectionPoint.bubbleRect.contains(e.pos()):
				selectedConnectionPoint = connectionPoint
		
		if selectedConnectionPoint is not None and not self.isInPallet:

			if not selectedConnectionPoint.isOutput and selectedConnectionPoint.connections:
				# If the user clicked an input which already has a connection, let them grab the existing connection
				# Note that, as inputs can only have one connection, we can just use connecitons[0] to get that connection
				grabbedConnection = selectedConnectionPoint.connections[0]
				selectedConnectionPoint.unregisterConnection(grabbedConnection)
				grabbedConnection.input = None
				self.scene().activeConnection = grabbedConnection
				self.scene().activeConnection.rebuildEndpoints()

			else:
				# If the user clicked an output or an input with no connections, create a new connection coming from that point
				if selectedConnectionPoint.isOutput:
					newConnection = Connection(selectedConnectionPoint, None)
				else:
					newConnection = Connection(None, selectedConnectionPoint)

				self.scene().addItem(newConnection)
				self.scene().activeConnection = newConnection
				self.scene().activeConnection.rebuildEndpoints()

			e.accept()
			return
		else:
			super(Node, self).mousePressEvent(e)

		



class ConnectionPoint:

	def __init__(self, title, owner, isOutput, xRelative = 0, yRelative = 0):
		self.title = title
		self.owner = owner
		self.xRelative = xRelative
		self.yRelative = yRelative
		self.connections = []
		self.isOutput = isOutput

		self.textBox = None

	@property
	def bubbleRect(self):
		size = DrawingConstants.CONNECTION_POINT_DIAMETER

		return QtCore.QRectF(self.xRelative, self.yRelative, size, size)

	@property
	def leftAlignedTextRect(self):
		width = self.owner.width / 2 - DrawingConstants.CONNECTION_POINT_DIAMETER / 2 - DrawingConstants.NODE_PADDING
		height = DrawingConstants.CONNECTION_POINT_DIAMETER
		x = self.xRelative + DrawingConstants.CONNECTION_POINT_DIAMETER + DrawingConstants.NODE_PADDING
		y = self.yRelative

		return QtCore.QRectF(x, y, width, height)

	@property
	def rightAlignedTextRect(self):
		width = self.owner.width / 2 - DrawingConstants.CONNECTION_POINT_DIAMETER / 2 - DrawingConstants.NODE_PADDING
		height = DrawingConstants.CONNECTION_POINT_DIAMETER
		x = self.xRelative - DrawingConstants.NODE_PADDING - width
		y = self.yRelative

		return QtCore.QRectF(x, y, width, height)

	def registerConnection(self, connection):
		# If this is an input and there is already a connection, remove it
		if self.connections and not self.isOutput:
			for oldConnection in self.connections:
				oldConnection.output.unregisterConnection(oldConnection)
				self.owner.scene().removeItem(oldConnection)
			self.connections.clear()

		if self.textBox is not None:
			self.textBox.hide()

		self.connections.append(connection)

	def unregisterConnection(self, connection):
		self.connections.remove(connection)

		if not self.connections and self.textBox is not None:
			self.textBox.show()

	def updateConnections(self):
		for connection in self.connections:
			connection.rebuildEndpoints()



