from PyQt5 import QtCore, QtGui, QtWidgets
from drawing_constants import *
import math

class Dial(QtWidgets.QGraphicsItem):

    MARK_LENGTH = 4
    MARK_RADIUS = 30

    DIAL_INDICATOR_RADIUS = 19
    DIAL_INDICATOR_SIZE = 3

    RANGE_RADIUS = 30
    RANGE_WIDTH = 3

    RANGE_HANDLE_WIDTH = 7

    MINIMUM_RANGE = 0.04

    def __init__(self, bounds, index, parent = None):
        super(Dial,self).__init__(parent)

        self.bounds = bounds
        self.isRange = False

        self.percentage = 0.5
        self.range = [0.1, 0.9]
        self.index = index

        self.setFlags(QtWidgets.QGraphicsItem.ItemIsSelectable)

        self.isDraggingBeginStop = False

    def getMousePointerPercentage(self, pos):
        mouseAngle = math.atan2(pos.y(), pos.x())
        if mouseAngle > math.pi / 2 and mouseAngle < math.pi:
            mouseAngle -= 2 * math.pi

        lowerBound = - 5 * math.pi / 4
        upperBound = math.pi / 4

        
        if mouseAngle < lowerBound:
            mousePercentage = 0
        elif mouseAngle > upperBound:
            mousePercentage = 1
        else:
            mousePercentage = (mouseAngle - lowerBound) / (upperBound - lowerBound)

        return mousePercentage

    def mousePressEvent(self, e):
        mousePercentage = self.getMousePointerPercentage(e.pos())
        self.isDraggingBeginStop = (abs(mousePercentage - self.range[0]) < abs(mousePercentage - self.range[1]))

        super(Dial, self).mousePressEvent(e)

    @property
    def value(self):
        return self.percentage * (self.bounds[1] - self.bounds[0]) + self.bounds[0]

    @value.setter
    def value(self, newValue):
        self.percentage = (newValue - self.bounds[0]) / (self.bounds[1] - self.bounds[0])

    def makeLiteral(self):
        self.isRange = False
        self.update()

    def makeDriven(self):
        self.isRange = True
        self.update()

    def mouseMoveEvent(self, e):

        if self.isRange:
            if self.isDraggingBeginStop:
                self.range[0] = self.getMousePointerPercentage(e.pos())

                if (self.range[1] < self.range[0] + self.MINIMUM_RANGE):
                    if (self.range[0] + self.MINIMUM_RANGE <= 1):
                        self.range[1] = self.range[0] + self.MINIMUM_RANGE
                    else:
                        self.range[0] = 1 - self.MINIMUM_RANGE
            else:
                self.range[1] = self.getMousePointerPercentage(e.pos())

                if (self.range[0] > self.range[1] - self.MINIMUM_RANGE):
                    if (self.range[1] - self.MINIMUM_RANGE >= 0):
                        self.range[0] = self.range[1] - self.MINIMUM_RANGE
                    else:
                        self.range[1] = self.MINIMUM_RANGE
        else:
            self.percentage = self.getMousePointerPercentage(e.pos())

        self.update()
        super(Dial,self).mouseMoveEvent(e)

        self.scene().updateBasicGui()


    def boundingRect(self):
        return QtCore.QRectF(-1 * (self.RANGE_RADIUS + self.RANGE_HANDLE_WIDTH), -1 * (self.RANGE_RADIUS + self.RANGE_HANDLE_WIDTH), 2 * (self.RANGE_RADIUS + self.RANGE_HANDLE_WIDTH), 2 * (self.RANGE_RADIUS + self.RANGE_HANDLE_WIDTH))

    def paint(self, painter, option, widget=None):

        # Draw the dial itself

        painter.setPen(QtGui.QPen(QtGui.QColor(92, 92, 92), 1.5, QtCore.Qt.SolidLine))
        painter.setBrush(QtGui.QBrush(QtGui.QColor(167, 167, 167), QtCore.Qt.SolidPattern))

        painter.drawEllipse(QtCore.QPointF(0, 0), 25, 25)

        # Draw the tick marks

        markAngle = 3 * math.pi / 4

        markCount = math.floor(abs(self.bounds[1] - self.bounds[0]))

        for i in range(markCount):
            markStartX = math.cos(markAngle) * self.MARK_RADIUS
            markStartY = math.sin(markAngle) * self.MARK_RADIUS
            markEndX = math.cos(markAngle) * (self.MARK_RADIUS + self.MARK_LENGTH)
            markEndY = math.sin(markAngle) * (self.MARK_RADIUS + self.MARK_LENGTH)

            painter.drawLine(markStartX, markStartY, markEndX, markEndY)

            markAngle += (3 * math.pi / 2) / (markCount - 1)


        # Draw the knob direction indicator

        indicatorAngle = (3 * math.pi / 2) * self.percentage + 3 * math.pi / 4
        indicatorX = math.cos(indicatorAngle) * self.DIAL_INDICATOR_RADIUS
        indicatorY = math.sin(indicatorAngle) * self.DIAL_INDICATOR_RADIUS

        painter.setPen(QtCore.Qt.NoPen)
        if self.isRange:
            painter.setBrush(QtGui.QBrush(QtGui.QColor(122, 122, 122), QtCore.Qt.SolidPattern))
        else:
            painter.setBrush(QtGui.QBrush(QtGui.QColor(92, 92, 92), QtCore.Qt.SolidPattern))

        painter.drawEllipse(QtCore.QPointF(indicatorX, indicatorY), self.DIAL_INDICATOR_SIZE, self.DIAL_INDICATOR_SIZE)

        if self.isRange:
            # Draw range highlight
            painter.setPen(QtGui.QPen(QtGui.QColor(100, 150, 200), self.RANGE_WIDTH, QtCore.Qt.SolidLine))
            rangeStartAngleQt = -(3 * math.pi / 2) * self.range[0] + 5 * math.pi / 4
            rangeEndAngleQt = -(3 * math.pi / 2) * self.range[1] + 5 * math.pi / 4

            painter.drawArc(-self.RANGE_RADIUS, -self.RANGE_RADIUS, self.RANGE_RADIUS * 2, self.RANGE_RADIUS * 2, rangeStartAngleQt * 16 * 180 / math.pi, (rangeEndAngleQt - rangeStartAngleQt) * 16 * 180 / math.pi)

            # Draw the range handles
            painter.setPen(QtGui.QPen(QtGui.QColor(20, 20, 20), 3, QtCore.Qt.SolidLine))

            handleAngle = (3 * math.pi / 2) * self.range[0] + 3 * math.pi / 4
            handleStartX = math.cos(handleAngle) * self.RANGE_RADIUS
            handleStartY = math.sin(handleAngle) * self.RANGE_RADIUS
            handleEndX = math.cos(handleAngle) * (self.RANGE_RADIUS + self.RANGE_HANDLE_WIDTH)
            handleEndY = math.sin(handleAngle) * (self.RANGE_RADIUS + self.RANGE_HANDLE_WIDTH)

            painter.drawLine(handleStartX, handleStartY, handleEndX, handleEndY)

            handleAngle = (3 * math.pi / 2) * self.range[1] + 3 * math.pi / 4
            handleStartX = math.cos(handleAngle) * self.RANGE_RADIUS
            handleStartY = math.sin(handleAngle) * self.RANGE_RADIUS
            handleEndX = math.cos(handleAngle) * (self.RANGE_RADIUS + self.RANGE_HANDLE_WIDTH)
            handleEndY = math.sin(handleAngle) * (self.RANGE_RADIUS + self.RANGE_HANDLE_WIDTH)

            painter.drawLine(handleStartX, handleStartY, handleEndX, handleEndY) 





