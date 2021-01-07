
from PyQt5 import QtCore, QtGui, QtWidgets
from drawing_constants import *

class Connection(QtWidgets.QGraphicsItem):

	def __init__(self, output, input):
		super(Connection,self).__init__()

		self.output = output
		if output is not None:
			output.registerConnection(self)

		self.input = input
		if input is not None:
			input.registerConnection(self)

		self.rebuildEndpoints()

		self.setZValue(-10)

		# Used by node scene for topological sorting. Kahn's algorithm for topological sort involves removing edges from the graph.
		# However, as the sort is done on the live copy of the graph, we must not actually remove nodes. So instead, the node scene
		# marks edges as "removed" by setting sortMarkNumber to an arbitrary value selected each time a sort is performed. It can
		# then recognize edges that have been "removed" and ignore them
		self.sortMarkNumber = -1

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