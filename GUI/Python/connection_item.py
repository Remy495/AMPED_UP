
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