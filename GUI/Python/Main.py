import sys
from node_scene import *
from node_item import *
import time
from node_pallet_item import *
from slider_item import *


class NodeGraphicsView(QtWidgets.QGraphicsView):

	def __init__(self, scene):
		super(NodeGraphicsView, self).__init__(scene)

	def resizeEvent(self, e):
		newSceneRect = QtCore.QRectF(0, 0, e.size().width(), e.size().height())
		self.scene().setSceneRect(newSceneRect)

if __name__=='__main__':

	import sys
	app =QtWidgets.QApplication(sys.argv)
	scene = NodeScene()
	view = NodeGraphicsView(scene)

	nodes = [
		Node('Guitar Signal',{},["Volume", "Pitch"],True, 40,40),
		Node('Add',{"Signal 1": (0, 1),"Signal 2": (0, 1)},["Sum"],True, 40,200),
		Node('Multiply',{"Signal 1": (0, 1),"Signal 2": (0, 1)},["Product"],True,40,400),
		Node('Remap Range',{"Signal": (0, 1), "Input Low": (0, 100), "Input High": (0, 100), "Output Low": (0, 100), "Output High": (0, 100)},["Signal"],True,40,600),
		Node('Sine Wave',{"Frequency": (0, 10), "Amplitude": (0, 1), "Phase": (0, 360)},["Signal"],True,40,900),
		Node('Output',{"Volume": (0, 1), "Gain": (0, 1), "Trebble": (0, 1), "Mid": (0, 1), "Bass": (0, 1), "Presence": (0, 1)},[],False,400,400)
	]
	pallet = NodePallet()
	scene.addItem(pallet)
	pallet.rebuildDimensions()
	scene.pallet = pallet

	for node in nodes:
		scene.addNode(node)

	view.show()
	pallet.rebuildDimensions()
	sys.exit(app.exec_())

