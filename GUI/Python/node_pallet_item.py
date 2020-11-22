
from PyQt5 import QtCore, QtGui, QtWidgets
from drawing_constants import *



class NodePallet(QtWidgets.QGraphicsItem):

    def __init__(self):
        super(NodePallet,self).__init__()
        self.width = 0
        self.height = 0

        self.setZValue(1)

    def rebuildDimensions(self):
        if self.scene() is not None:
            sceneRect = self.scene().sceneRect()

            self.width = DrawingConstants.NODE_WIDTH + 100
            self.height = sceneRect.height()
    def boundingRect(self):
        return QtCore.QRectF(0, 0, self.width, self.height)

    def paint(self, painter, option, widget=None):
        borderPen = QtGui.QPen(DrawingConstants.NODE_OUTLINE_COLOR, DrawingConstants.NODE_OUTLINE_WIDTH, QtCore.Qt.SolidLine)
        # Select pen/brush for painting the main body of the node
        painter.setBrush(QtGui.QBrush(DrawingConstants.NODE_BACKGROUND_COLOR, QtCore.Qt.SolidPattern))
        painter.setPen(borderPen)

        # Paint main body
        painter.drawRect(0,0,self.width,self.height)

