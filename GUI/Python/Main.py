import sys
from node_scene import *
from node_item import *
import time
from node_pallet_item import *
from slider_item import *
from node_scene_background import *

from AmpedUpNodes.NodeType import *
from dial_item import Dial

from hardware_comms import HardwareComms
from application_context import ApplicationContext


class NodeGraphicsView(QtWidgets.QGraphicsView):

	def __init__(self, scene):
		super(NodeGraphicsView, self).__init__(scene)

	def resizeEvent(self, e):
		newSceneRect = QtCore.QRectF(0, 0, e.size().width(), e.size().height())
		self.scene().setSceneRect(newSceneRect)

if __name__=='__main__':

	hcom = HardwareComms("E8:DB:84:03:F1:32", 1)
	hcom.runAsync()

	appContext = ApplicationContext()
	appContext.comms = hcom

	import sys
	app = QtWidgets.QApplication(sys.argv)
	scene = NodeScene(appContext)
	view = NodeGraphicsView(scene)


	scene.addPalletNode('Guitar Signal', NodeType.GUITAR_SIGNAL, {},["Volume", "Pitch"], "Inputs")
	scene.addPalletNode('Expression Pedal', NodeType.EFFECTS_PEDAL, {},["Position"], "Inputs")
	scene.addPalletNode('Add', NodeType.ADD,{"Signal 1": (False, 0, 1),"Signal 2": (False, 0, 1)},["Sum"], "Math")
	scene.addPalletNode('Multiply', NodeType.MULTIPLY,{"Signal 1": (False, 0, 1),"Signal 2": (False, 0, 1)},["Product"], "Math")
	scene.addPalletNode('Average', NodeType.AVERAGE,{"Signal 1": (False, 0, 1),"Signal 2": (False, 0, 1)},["Average"], "Math")
	scene.addPalletNode('Set Range', NodeType.REMAP_RANGE,{"Signal": (False, 0, 1), "Minimum": (False, 0, 2), "Maximum": (False, 0, 2)},["Signal"], "Utility")
	scene.addPalletNode('Reverse', NodeType.REVERSE,{"Signal": (False, 0, 1)},["Signal"], "Utility")
	scene.addPalletNode('Oscilate', NodeType.SINE_WAVE,{"Frequency": (False, 0, 2), "Amplitude": (False, 0, 2), "Phase": (False, 0, 360)},["Signal"], "Signals")

	scene.outputNode = Node('Output', NodeType.KNOB_POSITIONS,{"Volume": (True, 0, 12), "Drive": (True, 0, 12), "Trebble": (True, 0, 12), "Bass": (True, 0, 12), "Midd": (True, 0, 12), "Master": (True, 0, 12), "Reverb": (True, 0, 12), "Presence": (True, 0, 12)},[],False,400,400)
	scene.addNode(scene.outputNode)


	view.show()
	sys.exit(app.exec_())

