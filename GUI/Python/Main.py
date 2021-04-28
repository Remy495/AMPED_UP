import sys
from node_scene import *
from node_item import *
import time
from node_pallet_item import *
from slider_item import *
from node_scene_background import *

from AmpedUpNodes.NodeType import *

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
	scene.addPalletNode('Add', NodeType.ADD,{"Signal 1": (0, 1),"Signal 2": (0, 1)},["Sum"], "Math")
	scene.addPalletNode('Multiply', NodeType.MULTIPLY,{"Signal 1": (0, 1),"Signal 2": (0, 1)},["Product"], "Math")
	scene.addPalletNode('Remap Range', NodeType.REMAP_RANGE,{"Signal": (0, 1), "Input Low": (0, 1), "Input High": (0, 1), "Output Low": (0, 1), "Output High": (0, 1)},["Signal"], "Math")
	scene.addPalletNode('Sine Wave', NodeType.SINE_WAVE,{"Frequency": (0, 2), "Amplitude": (0, 2), "Phase": (0, 360)},["Signal"], "Signals")

	scene.addNode(Node('Output', NodeType.KNOB_POSITIONS,{"Volume": (0, 1), "Drive": (0, 1), "Trebble": (0, 1), "Bass": (0, 1), "Midd": (0, 1), "Master": (0, 1), "Reverb": (0, 1), "Presence": (0, 1)},[],False,400,400))

	view.show()
	sys.exit(app.exec_())

