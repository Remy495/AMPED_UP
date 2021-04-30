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

class AdvancedGuiWindow(QtWidgets.QMainWindow):

	def __init__(self, basicGui):
		super(AdvancedGuiWindow, self).__init__(basicGui)

		self.hcom = HardwareComms("E8:DB:84:03:F1:32", 1)
		self.hcom.runAsync()

		self.appContext = ApplicationContext()
		self.appContext.comms = self.hcom
		self.appContext.basicGui = basicGui

		self.scene = NodeScene(self.appContext)
		self.view = NodeGraphicsView(self.scene)


		self.scene.addPalletNode('Guitar Signal', NodeType.GUITAR_SIGNAL, {},["Volume", "Pitch"], "Inputs")
		self.scene.addPalletNode('Expression Pedal', NodeType.EFFECTS_PEDAL, {},["Position"], "Inputs")
		self.scene.addPalletNode('Add', NodeType.ADD,{"Signal 1": (False, 0, 1),"Signal 2": (False, 0, 1)},["Sum"], "Math")
		self.scene.addPalletNode('Multiply', NodeType.MULTIPLY,{"Signal 1": (False, 0, 1),"Signal 2": (False, 0, 1)},["Product"], "Math")
		self.scene.addPalletNode('Average', NodeType.AVERAGE,{"Signal 1": (False, 0, 1),"Signal 2": (False, 0, 1)},["Average"], "Math")
		self.scene.addPalletNode('Set Range', NodeType.REMAP_RANGE,{"Signal": (False, 0, 1), "Minimum": (False, 0, 2), "Maximum": (False, 0, 2)},["Signal"], "Utility")
		self.scene.addPalletNode('Reverse', NodeType.REVERSE,{"Signal": (False, 0, 1)},["Signal"], "Utility")
		self.scene.addPalletNode('Oscilate', NodeType.SINE_WAVE,{"Frequency": (False, 0, 2), "Amplitude": (False, 0, 2), "Phase": (False, 0, 360)},["Signal"], "Signals")

		self.scene.outputNode = Node('Output', NodeType.KNOB_POSITIONS,{"Volume": (True, 1, 12), "Drive": (True, 1, 12), "Trebble": (True, 1, 12), "Bass": (True, 1, 12), "Midd": (True, 1, 12), "Master": (True, 1, 12), "Reverb": (True, 1, 12), "Presence": (True, 1, 12)},[],False,400,400)
		self.scene.addNode(self.scene.outputNode)

		self.setCentralWidget(self.view)

	def serialize(self):
		return self.scene.serialize()

	def deserialize(self, buf):
		return self.scene.deserialize(buf)

	def upload(self):
		self.scene.upload()

	def setKnobPos(self, knobIndex, pos):
		self.scene.outputNode.inputs[knobIndex].textBox.value = pos
		self.scene.outputNode.inputs[knobIndex].textBox.update()

if __name__=='__main__':

	import sys
	app = QtWidgets.QApplication(sys.argv)

	window = AdvancedGuiWindow()

	window.show()
	sys.exit(app.exec_())

