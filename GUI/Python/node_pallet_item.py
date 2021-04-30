
from PyQt5 import QtCore, QtGui, QtWidgets
from drawing_constants import *



class NodePallet(QtWidgets.QGraphicsItem):

    def __init__(self):
        super(NodePallet,self).__init__()
        self.width = 0
        self.height = 0

        self.setZValue(1)

        self.tabs = {}
        self.selectedTab = None

    def rebuildDimensions(self):
        if self.scene() is not None:
            self.prepareGeometryChange()
            sceneRect = self.scene().sceneRect()

            self.width = DrawingConstants.PALLET_TAB_WIDTH + DrawingConstants.NODE_WIDTH + DrawingConstants.PALLET_MARGIN
            self.height = sceneRect.height()
    
    def boundingRect(self):
        return QtCore.QRectF(0, 0, self.width, self.height)

    def findNodeByType(self, nodeType):
        for tab in self.tabs.values():
            for node in tab.nodes:
                if node.nodeType == nodeType:
                    return node

    def addNode(self, node, tabName):
        # Find the tab by the requested name
        if tabName not in self.tabs:
            self.addTab(tabName)
        tab = self.tabs[tabName]

        # Move the node into the pallet immediately following the previous node in the tab
        nodeX = DrawingConstants.PALLET_TAB_WIDTH + DrawingConstants.PALLET_MARGIN / 2
        if tab.nodes:
            prevNode = tab.nodes[-1]
            nodeY = self.mapFromItem(prevNode, 0, prevNode.height).y() + DrawingConstants.PALLET_NODE_SPACING
        else:
            nodeY = DrawingConstants.PALLET_NODE_SPACING
        node.setPos(nodeX, nodeY)

        # Make the node visible if its bad is active, otherwise make it invisible
        if tab.isActive:
            node.show()
        else:
            node.hide()

        # Add the node to the tab
        tab.addNode(node)

    def replaceNode(self, oldNode, newNode):
        # Find which tab the old node is in and replace it there
        for tab in self.tabs.values():
            if oldNode in tab:
                tab.replaceNode(oldNode, newNode)

    def addTab(self, tabName):
        newTabIndex = len(self.tabs)
        newTab = NodePalletTab(tabName, newTabIndex)
        if not self.tabs:
            self.selectedTab = newTab
            newTab.activate()
        self.tabs[tabName] = newTab

    def paint(self, painter, option, widget=None):
        borderPen = QtGui.QPen(DrawingConstants.NODE_OUTLINE_COLOR, DrawingConstants.NODE_OUTLINE_WIDTH, QtCore.Qt.SolidLine)
        backgroundBrush = QtGui.QBrush(DrawingConstants.NODE_BACKGROUND_COLOR, QtCore.Qt.SolidPattern)
        # Select pen/brush for painting the main body of the node
        painter.setBrush(backgroundBrush)
        painter.setPen(borderPen)

        # Paint main body
        painter.drawRect(0,0,self.width,self.height)

        # Paint the tabs
        for tab in self.tabs.values():
            tabBrush = QtGui.QBrush(DrawingConstants.getAccentColor(tab.index, tab.isActive), QtCore.Qt.SolidPattern)
            if tab.isActive:
                painter.setPen(QtCore.Qt.NoPen)
            else:
                painter.setPen(borderPen)

            painter.setBrush(tabBrush)
            painter.drawRect(tab.boundingRect)

            if tab.isActive:
                painter.setPen(QtCore.Qt.black)
                painter.setFont(QtGui.QFont(DrawingConstants.LABEL_FONT, DrawingConstants.NODE_TITLE_FONTSIZE, QtGui.QFont.Bold))
            else:
                painter.setPen(DrawingConstants.PALLET_SHADED_TEXT_COLOR)
                painter.setFont(QtGui.QFont(DrawingConstants.LABEL_FONT, DrawingConstants.NODE_TITLE_FONTSIZE))

            painter.drawText(tab.boundingRect, QtCore.Qt.AlignCenter, tab.name)


    # Swallow mouse events (so they don't get passed on to items behind this one) so that nothing happens when you click on the pallet

    def mouseMoveEvent(self, e):
        e.accept()

    def mousePressEvent(self, e):
        for tab in self.tabs.values():
            if tab.boundingRect.contains(e.pos()):
                self.selectedTab.deactivate()
                self.selectedTab = tab
                self.selectedTab.activate()
                self.update()
                break

        e.accept()

    def mouseReleaseEvent(self, e):
        e.accept()

    
class NodePalletTab:
    
    def __init__(self, name, index):
        self.name = name
        self.index = index
        self.nodes = []
        self.isActive = False
        self.boundingRect = QtCore.QRectF(0, DrawingConstants.PALLET_TAB_HEIGHT * index, DrawingConstants.PALLET_TAB_WIDTH, DrawingConstants.PALLET_TAB_HEIGHT)

    def addNode(self, node):
        # Add the node to the list of nodes
        self.nodes.append(node)

    def removeNode(self, node):
        self.nodes.remove(node)

    def replaceNode(self, oldNode, newNode):
        oldNodeIndex = self.nodes.index(oldNode)
        self.nodes[oldNodeIndex] = newNode

    def activate(self):
        self.isActive = True
        for node in self.nodes:
            node.show()

    def deactivate(self):
        self.isActive = False
        for node in self.nodes:
            node.hide()

    def __contains__(self, item):
        return item in self.nodes