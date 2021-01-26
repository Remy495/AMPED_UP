from PyQt5 import QtCore, QtGui, QtWidgets
from drawing_constants import *

class Slider(QtWidgets.QGraphicsTextItem):

    def __init__(self, range, parent = None):
        super(Slider, self).__init__(parent)

        # self.setFlags(QtWidgets.QGraphicsItem.ItemIsSelectable)
        self.setTextInteractionFlags(QtCore.Qt.TextEditable)

        self.width = DrawingConstants.NODE_WIDTH / 2 - 2 * DrawingConstants.NODE_PADDING

        self.setTextWidth(self.width)

        self.minValue = range[0]
        self.maxValue = range[1]

        self.userIsDragging = False

        self.percentage = 0.5
        self.setPlainText(str(round(self.value, 2)))
        self.alignText(QtCore.Qt.AlignCenter)

    @property
    def percentage(self):
        return (self.value - self.minValue) / (self.maxValue - self.minValue)

    @percentage.setter
    def percentage(self, newPercentage):
        self.value = self.minValue + (self.maxValue - self.minValue) * newPercentage

    def focusInEvent(self, e):
        # Ignore focus caused be mouse, as that is handled separately to avoid taking keyboard focus when user drags
        self.setPlainText(str(self.value))
        if e.reason() != QtCore.Qt.MouseFocusReason:
            super(Slider, self).focusInEvent(e)

            # Select the current text (so it can be easily replaced)
            cursor = self.textCursor()
            cursor.movePosition(QtGui.QTextCursor.Start)
            cursor.movePosition(QtGui.QTextCursor.End, QtGui.QTextCursor.KeepAnchor)
            self.setTextCursor(cursor)
        else:
            self.clearFocus()

    def focusOutEvent(self, e):
        super(Slider, self).focusInEvent(e)

        # Get the text the user entered
        enteredText = self.toPlainText()
        # If the user entered a number, set it as the new value for this slider
        try:
            self.value = float(enteredText)
        except ValueError:
            pass

        # Set the slider text as the first few digits of the value
        self.setPlainText(str(round(self.value, 2)))
        self.alignText(QtCore.Qt.AlignCenter)

    def keyPressEvent(self, e):
        if e.key() == QtCore.Qt.Key_Return:
            self.clearFocus()
        else:
            super(Slider, self).keyPressEvent(e)
            e.accept()
            return

    def mouseMoveEvent(self, e):
        if self.hasFocus():
            super(Slider, self).mousePressEvent(e)
        else:
            self.userIsDragging = True
            mouseXPos = e.pos().x()
            self.percentage = mouseXPos / self.width
            self.value = round(self.value, 2)
            self.setPlainText(str(self.value))
            self.alignText(QtCore.Qt.AlignCenter)
            self.update()

    def mousePressEvent(self, e):
        if self.hasFocus():
            super(Slider, self).mousePressEvent(e)
        else:
            e.accept()
            return

    def mouseReleaseEvent(self, e):
        if self.hasFocus():
            # If this slider is currently being text-edited, let the superclass deal with mouse eventsss
            super(Slider, self).mousePressEvent(e)
        else:
            # If user was not just dragging the slider, start editing text
            if not self.userIsDragging:
                self.setFocus()
                self.update()

        self.userIsDragging = False

    def alignText(self, alignment):
        textFormat = QtGui.QTextBlockFormat()
        textFormat.setAlignment(alignment)
        textFormat.setNonBreakableLines(True)
        cursor = self.textCursor()
        cursor.select(QtGui.QTextCursor.Document)
        cursor.mergeBlockFormat(textFormat)
        cursor.clearSelection()
        self.setTextCursor(cursor)
        
    def boundingRect(self):
        # Get the bounding box of the textItem so that the hight will be correct
        originalBoundingRect = super(Slider, self).boundingRect()
        # Set the width so that excessively long text is clipped
        originalBoundingRect.setWidth(self.width)
        return originalBoundingRect

    def paint(self, painter, option, widget=None):

        if option.state & QtWidgets.QStyle.State_HasFocus:
            # If this slider is currently being text-edited, simply draw a background rectangle
            painter.setBrush(QtGui.QBrush(DrawingConstants.SLIDER_FOCUS_COLOR, QtCore.Qt.SolidPattern))
            painter.setPen(QtGui.QPen(DrawingConstants.NODE_OUTLINE_COLOR, DrawingConstants.NODE_OUTLINE_WIDTH, QtCore.Qt.SolidLine))

            painter.drawRect(self.boundingRect())

            clipBox = self.boundingRect()
            painter.setClipRect(clipBox)

            super(Slider, self).paint(painter, option, widget)
        else:
            # If this slider is not currently being text-edited, draw the filled / empty sections
            painter.setBrush(QtGui.QBrush(DrawingConstants.SLIDER_EMPTY_COLOR, QtCore.Qt.SolidPattern))
            painter.setPen(QtGui.QPen(DrawingConstants.NODE_OUTLINE_COLOR, DrawingConstants.NODE_OUTLINE_WIDTH, QtCore.Qt.SolidLine))

            painter.drawRect(self.boundingRect())

            painter.setBrush(QtGui.QBrush(DrawingConstants.SLIDER_FULL_COLOR, QtCore.Qt.SolidPattern))
            filledWidth = self.boundingRect().width() * max(0, min(self.percentage, 1))
            filledRect = self.boundingRect()
            filledRect.setWidth(filledWidth)

            painter.drawRect(filledRect)

            super(Slider, self).paint(painter, option, widget)

    