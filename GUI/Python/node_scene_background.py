
from PyQt5 import QtCore, QtGui, QtWidgets
from drawing_constants import *

class NodeSceneBackground(QtWidgets.QGraphicsItem):

    def __init__(self, parent = None):
        super(NodeSceneBackground,self).__init__(parent)
        self.setFlags(QtWidgets.QGraphicsItem.ItemIsMovable)

        self.sceneRect = QtCore.QRectF(0, 0, 0, 0)

    def boundingRect(self):
        return self.sceneRect

    def rebuild(self):
        self.prepareGeometryChange()

        if self.scene() is not None:
            self.sceneRect = self.mapRectFromScene(self.scene().sceneRect())

    def mouseReleaseEvent(self, e):
        super(NodeSceneBackground, self).mouseReleaseEvent(e)
        self.rebuild()

    def mousePressEvent(self, e):
        if self.scene() is not None:
            self.scene().clearSelection()


    def paint(self, painter, option, widget=None):
        pass
        # # Select pen/brush for painting the main body of the node
        # painter.setBrush(QtGui.QBrush(DrawingConstants.NODE_BACKGROUND_COLOR, QtCore.Qt.SolidPattern))
        # painter.setPen(QtGui.QPen(DrawingConstants.NODE_OUTLINE_COLOR, DrawingConstants.NODE_OUTLINE_WIDTH, QtCore.Qt.SolidLine))

        # painter.drawRect(self.boundingRect())
    

