import sys
from PyQt5 import QtCore, QtGui, QtWidgets


class Rectangle(QtWidgets.QGraphicsRectItem):
	def __init__(self, x, y, w, h):
		super(Rectangle, self).__init__(0,0,w,h)
		self.setPen(QtGui.QPen(QtCore.Qt.red, 2))
		self.setFlags(QtWidgets.QGraphicsItem.ItemIsSelectable
			| QtWidgets.QGraphicsItem.ItemIsMovable
			| QtWidgets.QGraphicsItem.ItemIsFocusable)
		self.setPos(QtCore.QPointF(x,y))
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
    scene.addItem(Rectangle(0, 0, 100, 100))
    view.show()
    sys.exit(app.exec_())
