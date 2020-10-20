from PyQt5 import QtCore, QtGui

class DrawingConstants:
    NODE_WIDTH = 200
    NODE_BASE_HEIGHT = 25
    CONNECTION_POINT_DIAMETER = 20
    CONNECTION_POINT_PADDING = 10

    NODE_OUTLINE_COLOR = QtCore.Qt.black
    NODE_OUTLINE_WIDTH = 2
    NODE_BACKGROUND_COLOR = QtGui.QColor(230, 230, 230)
    NODE_TEXT_COLOR = QtCore.Qt.black
    CONNECTION_POINT_COLOR = QtGui.QColor(127, 127, 255)
    HIGHLIGHT_COLOR = QtGui.QColor(100, 220, 220)

    LABEL_FONT = "Helvetica"
    NODE_TITLE_FONTSIZE = 12