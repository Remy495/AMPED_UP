import sys
from PyQt5 import QtCore, QtGui, QtWidgets

class LineEditItem(QtWidgets.QGraphicsTextItem):

	def __init__(self, text, parent, width):
		super(LineEditItem, self).__init__(text, parent)
		self.setFlags(QtWidgets.QGraphicsItem.ItemIsFocusable)
		self.setTextInteractionFlags(QtCore.Qt.TextEditable)
		self.width = width

	def keyPressEvent(self, e):
		previousState = self.document()
		super(LineEditItem, self).keyPressEvent(e)
		print(self.textWidth())
		if self.textWidth() > self.width:
			print('text clipped!')
			self.setDocument(previousState)



class Rectangle(QtWidgets.QGraphicsRectItem):
	def __init__(self, x, y, w, h):
		super(Rectangle, self).__init__(0,0,w,h)
		self.setPen(QtGui.QPen(QtCore.Qt.red, 2))
		self.setFlags(QtWidgets.QGraphicsItem.ItemIsSelectable
			| QtWidgets.QGraphicsItem.ItemIsMovable
			| QtWidgets.QGraphicsItem.ItemIsFocusable
			| QtWidgets.QGraphicsItem.ItemClipsChildrenToShape)
		self.setPos(QtCore.QPointF(x,y))

		self.textItem = LineEditItem("This is an example of a QT Text Item", self, w)
		self.textItem.setTextInteractionFlags(QtCore.Qt.TextEditable)


	def mouseMoveEvent(self, e):
		if e.buttons() & QtCore.Qt.LeftButton:
			super(Rectangle, self).mouseMoveEvent(e)
		if e.buttons() & QtCore.Qt.RightButton:
			self.setRect(QtCore.QRectF(QtCore.QPoint(), e.pos()).normalized())


if __name__ == '__main__':
    import sys
    app = QtWidgets.QApplication(sys.argv)
    scene = QtWidgets.QGraphicsScene()
    view = QtWidgets.QGraphicsView(scene)
    scene.addItem(Rectangle(0, 0, 100, 50))
    scene.addItem(LineEditItem("example", None, 50))
    view.show()
    sys.exit(app.exec_())
