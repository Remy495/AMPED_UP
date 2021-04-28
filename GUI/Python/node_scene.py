
from PyQt5 import QtCore, QtGui, QtWidgets
from drawing_constants import *
from node_item import *
from node_pallet_item import *
from node_scene_background import *

import flatbuffers
from node_graph_serializer import NodeGraphSerializer, NodeGraph
from AmpedUpMessaging import Message, MessagePayload, SavePresetMessage

class NodeScene(QtWidgets.QGraphicsScene):
	def __init__(self, context, parent = None):
		# Initialize super class
		super(NodeScene,self).__init__(parent)

		self.context = context

		# Set default values for mouse position (will be updates when mouse moves)
		self.mousePos = QtCore.QPointF(0, 0)

		# Reference to the currently active connection (i.e. the one that the user is currently dragging)
		self.activeConnection = None

		# List of nodes currently present in the scene
		self.nodes = []

		self.sceneRectChanged.connect(self.onSceneRectChanged)

		self.pallet = NodePallet()
		self.addItem(self.pallet)
		self.pallet.rebuildDimensions()

		self.background = NodeSceneBackground()
		self.addItem(self.background)
		self.background.rebuild()

		self.isEditingText = False

		self.sortNumber = 0

	def addNode(self, node):
		self.nodes.append(node)
		self.addItem(node)
		node.rebuild()

	def addPalletNode(self, name, nodeType, inputs, outputs, palletTab):
		newNode = Node(name, nodeType, inputs, outputs)
		self.addNode(newNode)
		self.pallet.addNode(newNode, palletTab)

	def replacePalletNode(self, oldPalletNode, newPalletNode):
		self.pallet.replaceNode(oldPalletNode, newPalletNode)

	def removeNode(self, node):
		# Remove all of the node connections
		for connectionPoint in node.connectionPoints:
			for connection in connectionPoint.connections:
				connection.input.unregisterConnection(connection)
				connection.output.unregisterConnection(connection)
				self.removeItem(connection)

		# Remove the node from the list of nodes
		self.nodes.remove(node)

		# Remove the node from the scene
		self.removeItem(node)

	def sortedNetwork(self):
		sortedNodes = []
		# Find all of the nodes with no connections going into them
		inProgressNodes = [node for node in self.nodes if not node.isInPallet and not node.hasUnmarkedPrecursors(self.sortNumber)]

		while inProgressNodes:
			# Select a node from inputNodes and add it to the sorted list. This is safe, as it is known to have no precursors not in the sorted list
			currentNode = inProgressNodes.pop()
			currentNode.indexNumber = len(sortedNodes)
			sortedNodes.append(currentNode)

			# Iterate over all of the connections going out of the node just added
			for connectionPoint in currentNode.outputs:
				for connection in connectionPoint.connections:
					# Mark the connection point, as its output has been added to the sorted list
					connection.sortMarkNumber = self.sortNumber

					# Check if the input of this connection now has no unmarked connections going into it
					nextNode = connection.input.owner
					if not nextNode.hasUnmarkedPrecursors(self.sortNumber):
						# If so, all of its precursors are in the sorted list, and it can be safely processed
						inProgressNodes.append(nextNode)

		self.sortNumber += 1
		return sortedNodes

	
	def clearNetwork(self):
		nodesToRemove = [node for node in self.nodes if node.isDeleteable]

		for node in nodesToRemove:
			self.removeNode(node)


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
						# If the active connection is being dragged from an output to an input, check if the mouse is within any of this node's input bubbles
						for connectionPoint in node.inputs:
							if connectionPoint.bubbleRect.contains(relativeMousePosition):
								# If the user selected an input connection point, check that creating this connection will not create a cycle in the graph
								if not node.comesBefore(self.activeConnection.output.owner):
									selectedConnectionPoint = connectionPoint

					else:
						# If the active connection is being dragged from an input to an output, check if the mouse is within any of this node's output bubbles
						for connectionPoint in node.outputs:
							if connectionPoint.bubbleRect.contains(relativeMousePosition):
								# If the user selected an output connection point, check that creating this connection will not create a cycle in the graph
								if not node.comesAfter(self.activeConnection.input.owner):
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

	def keyPressEvent(self, e):
		super(NodeScene, self).keyPressEvent(e)
		if not self.isEditingText:
			if e.key() == QtCore.Qt.Key_Delete or e.key() == QtCore.Qt.Key_Backspace:
				for item in self.selectedItems():
					# For now, the only things that are selectable are nodes. So just run deleteNode on each one (if it is deleteable)
					if item.isDeleteable:
						self.removeNode(item)

				# Any non-deleteable nodes should be deselected
				self.clearSelection()

			if e.key() == QtCore.Qt.Key_Space:
				builder = flatbuffers.Builder(1024)
				nodeGraphOffset = NodeGraphSerializer.serialize(self, builder)
				
				SavePresetMessage.SavePresetMessageStart(builder)
				SavePresetMessage.SavePresetMessageAddId(builder, 2)
				SavePresetMessage.SavePresetMessageAddValue(builder, nodeGraphOffset)
				savePresetMessageOffset = SavePresetMessage.SavePresetMessageEnd(builder)

				Message.MessageStart(builder)
				Message.MessageAddPayloadType(builder, MessagePayload.MessagePayload.SavePresetMessage)
				Message.MessageAddPayload(builder, savePresetMessageOffset)
				messageOffset = Message.MessageEnd(builder)

				builder.Finish(messageOffset)
				buf = builder.Output()

				self.context.comms.sendMessage(bytes(buf))

				# nodeGraph = NodeGraph.NodeGraph.GetRootAsNodeGraph(buf, 0)
				# NodeGraphSerializer.deserialize(nodeGraph, self)
			

	def onSceneRectChanged(self, rect):
		if self.pallet is not None and self.background is not None:
			self.pallet.rebuildDimensions()
			self.background.rebuild()