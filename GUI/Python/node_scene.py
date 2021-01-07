
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

		self.sceneRectChanged.connect(self.onSceneRectChanged)

		self.pallet = None

	def addNode(self, node):
		self.nodes.append(node)
		self.addItem(node)
		node.rebuild()

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
				activeConnectionNeedsInput = True
			else:
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

	def onSceneRectChanged(self, rect):
		if self.pallet is not None:
			self.pallet.rebuildDimensions()