from PyQt5 import QtCore, QtGui

class DrawingConstants:
    NODE_WIDTH = 225
    NODE_BASE_HEIGHT = 25
    CONNECTION_POINT_DIAMETER = 20
    NODE_PADDING = 10
    DIAL_HEIGHT = 70

    NODE_OUTLINE_COLOR = QtCore.Qt.black
    NODE_OUTLINE_WIDTH = 2
    NODE_BACKGROUND_COLOR = QtGui.QColor(230, 230, 230)
    NODE_TEXT_COLOR = QtCore.Qt.black
    CONNECTION_POINT_COLOR = QtGui.QColor(127, 127, 255)
    HIGHLIGHT_COLOR = QtGui.QColor(100, 220, 220)

    LABEL_FONT = "Helvetica"
    NODE_TITLE_FONTSIZE = 12

    SLIDER_EMPTY_COLOR = QtGui.QColor(210, 210, 210)
    SLIDER_FULL_COLOR = QtGui.QColor(160, 160, 160)
    SLIDER_FOCUS_COLOR = QtGui.QColor(120, 120, 120)

    PALLET_MARGIN = 100
    PALLET_TAB_WIDTH = 100
    PALLET_TAB_HEIGHT = 50
    PALLET_NODE_SPACING = 20
    PALLET_SHADED_TEXT_COLOR = QtGui.QColor(110, 110, 110)

    ACCENT_COLOR_SATURATION = 200
    ACCENT_COLOR_VALUE = 75
    ACCENT_COLOR_HIGHLIGHTED_SATURATION = 150
    ACCENT_COLOR_HIGHLIGHTED_VALUE = 255
    ACCENT_COLOR_HUE_OFFSET = 137.5

    @classmethod
    def getAccentColor(cls, index, isHighlighted):
        color = QtGui.QColor()

        hue = index * cls.ACCENT_COLOR_HUE_OFFSET
        if isHighlighted:
            saturation = cls.ACCENT_COLOR_HIGHLIGHTED_SATURATION
            value = cls.ACCENT_COLOR_HIGHLIGHTED_VALUE
        else:
            saturation = cls.ACCENT_COLOR_SATURATION
            value = cls.ACCENT_COLOR_VALUE
        color.setHsv(hue, saturation, value)
        return color