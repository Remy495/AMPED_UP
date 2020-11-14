import sys
from PyQt5 import QtCore, QtGui, QtWidgets
from drawing_constants import *

class NodeScene(QtWidgets.QGraphicsScene):
	def __init__(self, parent = None):
		# Initialize super class
		super(NodeScene,self).__init__(parent)

		# Set default values for mouse position (will be updates when mouse moves)
		self.mousePos = QtCore.QPointF(0, 0)

		# Reference to the currently active connection (i.e. the one that the user is currently dragging)
		self.activeConnection = None

		# List of nodes currently present in the scene
		self.nodes = []

	def addNode(self, node):
		self.nodes.append(node)
		self.addItem(node)

	def mouseMoveEvent(self, e):
		# Whenever the mouse moves, update mousePos
		self.mousePos = e.scenePos()
		if self.activeConnection is not None:
			self.activeConnection.rebuildEndpoints()

		super(NodeScene, self).mouseMoveEvent(e)

	def mouseReleaseEvent(self, e):
		# If there is an active connection (following the mouse), find which connection point the user has dropped it on
		if self.activeConnection is not None:
			selectedConnectionPoint = None

			# Every connection must lead from an output to an input. Check which one this connection is missing so that
			# the corrisponding connection points can be checked
			if self.activeConnection.input is None:
				print("looking for input")
				activeConnectionNeedsInput = True
			else:
				print("looking for output")
				activeConnectionNeedsInput = False

			for node in self.nodes:
				# Check if the mouse pointer is within the node
				relativeMousePosition = node.mapFromScene(e.scenePos())
				if node.boundingRect().contains(relativeMousePosition):
					# Decide which kind of connection point to check based on which end of the edge has no conneciton
					if activeConnectionNeedsInput:
						availableConnectionPoints = node.inputs
					else:
						availableConnectionPoints = node.outputs

					# Check if the mouse pointer is within any of the node's connection points
					for connectionPoint in availableConnectionPoints:
						if connectionPoint.bubbleRect.contains(relativeMousePosition):
							selectedConnectionPoint = connectionPoint
			
			# If a connection point was selected, set it as the end point of the connection. Otherwise remove the conneciton
			if selectedConnectionPoint is not None:
				if activeConnectionNeedsInput:
					self.activeConnection.input = selectedConnectionPoint
				else:
					self.activeConnection.output = selectedConnectionPoint
				selectedConnectionPoint.registerConnection(self.activeConnection)
				self.activeConnection.rebuildEndpoints()
			else:
				if activeConnectionNeedsInput:
					self.activeConnection.output.unregisterConnection(self.activeConnection)
				else:
					self.activeConnection.input.unregisterConnection(self.activeConnection)
				self.removeItem(self.activeConnection)
			
			# No matter what happened, the previously active connection is no longer active.
			self.activeConnection = None
				
			
		super(NodeScene, self).mouseReleaseEvent(e)

	

class Node(QtWidgets.QGraphicsItem):

	def __init__(self, title="", inputs=None, outputs=None, x=0, y=0):
		super(Node,self).__init__()
		self.width = DrawingConstants.NODE_WIDTH
		self.height = DrawingConstants.NODE_BASE_HEIGHT
		self.title = title

		# Add input and output connection points
		self.inputs = [ConnectionPoint(inputTitle, self, False) for inputTitle in inputs]
		self.outputs = [ConnectionPoint(outputTitle, self, True) for outputTitle in outputs]

		# Calculate the positions at which each of the connection points should be drawn (relative to the node)
		self.rebuild()

		# Set the node as draggable and selectable
		self.setFlags(QtWidgets.QGraphicsItem.ItemIsMovable | QtWidgets.QGraphicsItem.ItemIsSelectable)
		self.setAcceptHoverEvents(True)

		# Move the node to the specified position
		self.setPos(QtCore.QPoint(x,y))


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
		if self.scene().activeConnection is None:
			super(Node, self).mouseMoveEvent(e)
			if e.buttons() & QtCore.Qt.LeftButton:
				for connectionPoint in self.connectionPoints:
					connectionPoint.updateConnections()

	def mousePressEvent(self, e):
		# Determine if any of the connection points were clicked on

		selectedConnectionPoint = None
		for connectionPoint in self.connectionPoints:
			if connectionPoint.bubbleRect.contains(e.pos()):
				selectedConnectionPoint = connectionPoint
		
		if selectedConnectionPoint is not None:

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
			for oldConnection in self.connections:
				oldConnection.output.unregisterConnection(oldConnection)
				self.owner.scene().removeItem(oldConnection)
			self.connections.clear()

		self.connections.append(connection)

		print("register connection:", connection)
		print("connection list:", self.connections)

	def unregisterConnection(self, connection):
		print("unregister connection:", connection)
		print("connection list:", self.connections)
		self.connections.remove(connection)

	def updateConnections(self):
		for connection in self.connections:
			connection.rebuildEndpoints()



class Connection(QtWidgets.QGraphicsItem):

	def __init__(self, output, input):
		super(Connection,self).__init__()

		self.output = output
		if output is not None:
			print("output is connected")
			output.registerConnection(self)

		self.input = input
		if input is not None:
			print("input is connected")
			input.registerConnection(self)

		self.rebuildEndpoints()

		self.setZValue(-1)

	def rebuildEndpoints(self):
		self.prepareGeometryChange()

		if self.input is not None:
			# If an input node is connection, move the input point to the corrisponding connection point
			self.inputPoint = self.mapFromItem(self.input.owner, self.input.bubbleRect.center())
		elif self.scene() is not None:
			# If not, move to the mouse position
			self.inputPoint = self.scene().mousePos
		else:
			# If this connection has not yet been added to a scene, just set a default value
			self.inputPoint = QtCore.QPointF(0, 0)


		if self.output is not None:
			self.outputPoint = self.mapFromItem(self.output.owner, self.output.bubbleRect.center())
		elif self.scene() is not None:
			self.outputPoint = self.scene().mousePos
		else:
			self.outputPoint = QtCore.QPointF(0, 0)


	def boundingRect(self):
		x = min(self.inputPoint.x(), self.outputPoint.x())
		y = min(self.inputPoint.y(), self.outputPoint.y())
		width = abs(self.inputPoint.x() - self.outputPoint.x())
		height = abs(self.inputPoint.y() - self.outputPoint.y())

		return QtCore.QRectF(x, y, width, height)

	def paint(self, painter, option, widget=None):

		borderPen = QtGui.QPen(DrawingConstants.NODE_OUTLINE_COLOR, DrawingConstants.NODE_OUTLINE_WIDTH, QtCore.Qt.SolidLine)
		painter.setPen(borderPen)
		painter.drawLine(self.inputPoint, self.outputPoint)




if __name__=='__main__':
	import sys
	app =QtWidgets.QApplication(sys.argv)
	scene = NodeScene()
	view = QtWidgets.QGraphicsView(scene)
	node1 = Node('Node1',["first","second","third","fourth"],["output"],40,40)
	node2 = Node('Node2',["input", "other"],["first", "second"],500,500)
	#connection = Connection(node2.inputs[0], node1.outputs[0])
	scene.addNode(node1)
	scene.addNode(node2)
	#scene.addItem(connection)
	view.show()
	sys.exit(app.exec_())


