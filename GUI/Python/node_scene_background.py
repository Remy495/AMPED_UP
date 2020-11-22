
from PyQt5 import QtCore, QtGui, QtWidgets
from drawing_constants import *

from node_item import *

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

    def mouseMoveEvent(self, e):
        super(NodeSceneBackground, self).mouseMoveEvent(e)
        for item in self.childItems():
            if isinstance(item, Node):
                item.updateConnections()

    def mousePressEvent(self, e):
        if self.scene() is not None:
            self.scene().clearSelection()

        super(NodeSceneBackground, self).mousePressEvent(e)


    def paint(self, painter, option, widget=None):
        pass
    

