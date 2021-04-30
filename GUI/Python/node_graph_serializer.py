from AmpedUpNodes import ConstantValueFloat, Connection, Input, InputValue, Node, NodeGraph, OutputRange, NodeType

import connection_item


class NodeGraphSerializer:

    @classmethod
    def deserialize(clss, nodeGraph, nodeScene):
        nodeScene.clearNetwork()
        nodePallet = nodeScene.pallet

        # Deserialize all of the node in the node graph

        deserializedNodes = []

        for nodeIndex in range(nodeGraph.NodesLength()):
            nodeData = nodeGraph.Nodes(nodeIndex)

            nodeType = nodeData.Type()

            guiXPos = nodeData.GuiXPos()
            guiYPos = nodeData.GuiYPos()

            if nodeType == NodeType.NodeType.KNOB_POSITIONS:
                newNode = nodeScene.outputNode
            else:
                palletNode = nodePallet.findNodeByType(nodeType)
                newNode = palletNode.copy()

            newNode.setX(guiXPos)
            newNode.setY(guiYPos)
            deserializedNodes.append(newNode)

            for inputIndex in range(nodeData.InputsLength()):
                inputData = nodeData.Inputs(inputIndex)

                if inputData.ValueType() == InputValue.InputValue().Connection:
                    connectionTable = inputData.Value()
                    connection = Connection.Connection()
                    connection.Init(connectionTable.Bytes, connectionTable.Pos)

                    outputConnectionPoint = deserializedNodes[connection.NodeIndex()].outputs[connection.OutputIndex()]
                    inputConnectionPoint = newNode.inputs[inputIndex]

                    newConnection = connection_item.Connection(outputConnectionPoint, inputConnectionPoint)
                    nodeScene.addItem(newConnection)



                elif inputData.ValueType() == InputValue.InputValue().ConstantValueFloat:
                    constantValueTable = inputData.Value()
                    constantValue = ConstantValueFloat.ConstantValueFloat()
                    constantValue.Init(constantValueTable.Bytes, constantValueTable.Pos)

                    if nodeType == NodeType.NodeType.KNOB_POSITIONS:
                        newNode.inputs[inputIndex].textBox.percentage = constantValue.Value()
                    else:
                        newNode.inputs[inputIndex].textBox.value = constantValue.Value()
                else:
                    print("    ERROR: No value type supplied")

                newNode.inputs[inputIndex].textBox.update()

            newNode.update()

        # Set the output ranges

        outputNode = nodeScene.outputNode
        for outputRangeIndex in range(nodeGraph.OutputRangesLength()):
            outputRange = nodeGraph.OutputRanges(outputRangeIndex)

            minimum = outputRange.Minimum()
            maximum = outputRange.Maximum()

            outputNode.inputs[outputRangeIndex].textBox.range = [minimum, maximum]


    @classmethod
    def serialize(clss, nodeScene, builder):
        # List of nodes from the node scene
        nodes = nodeScene.sortedNetwork()

        # List to temporarily store the offsets of the serialized nodes within the buffer
        nodeOffsets = []

        # Serialize all of the nodes in the node graph
        for node in nodes:
            # Serialize all of the inputs for this node
            # List to temporarily store the offsets of the serialized inputs within the buffer
            nodeInputOffsets = []
            for inputConnectionPoint in node.inputs:
                inputValueOffset = None
                inputValueType = None

                if inputConnectionPoint.connections:
                    # If this input is connected to another node, serialize information about where it is connected
                    connection = inputConnectionPoint.connections[0]

                    Connection.ConnectionStart(builder)
                    Connection.ConnectionAddNodeIndex(builder, connection.output.owner.indexNumber)
                    Connection.ConnectionAddOutputIndex(builder, connection.output.indexNumber)
                    connectionOffset = Connection.ConnectionEnd(builder)

                    inputValueOffset = connectionOffset
                    inputValueType = InputValue.InputValue().Connection
                
                else:
                    # If this input is not connected to another node, serialize its literal value
                    # Note that, for now, all literal values are considered floats. In a future revision some literal values may be considered integers

                    # Use percentage for the output node to cancel out the range of the dial
                    if (node.nodeType == NodeType.NodeType.KNOB_POSITIONS):
                        literalValue = inputConnectionPoint.literalPercentage
                    else:
                        literalValue = inputConnectionPoint.literalValue

                    ConstantValueFloat.ConstantValueFloatStart(builder)
                    ConstantValueFloat.ConstantValueFloatAddValue(builder, literalValue)
                    constantValueOffset = ConstantValueFloat.ConstantValueFloatEnd(builder)

                    inputValueOffset = constantValueOffset
                    inputValueType = InputValue.InputValue().ConstantValueFloat

                # Add the serialized input to the list of inputs
                Input.InputStart(builder)
                Input.InputAddValueType(builder, inputValueType)
                Input.InputAddValue(builder, inputValueOffset)
                inputOffset = Input.InputEnd(builder)
                nodeInputOffsets.append(inputOffset)

            # Serialize the list of inputs
            Node.NodeStartInputsVector(builder, len(nodeInputOffsets))
            # Note that, due to a quirk of Flatbuffers we must add the inputs to the serialized vector in reverse order
            for inputOffset in reversed(nodeInputOffsets):
                builder.PrependUOffsetTRelative(inputOffset)
            nodeInputs = builder.EndVector(len(nodeInputOffsets))

            # Serialize the node itself
            Node.NodeStart(builder)
            Node.NodeAddType(builder, node.nodeType)
            Node.NodeAddInputs(builder, nodeInputs)
            Node.NodeAddGuiXPos(builder, node.x())
            Node.NodeAddGuiYPos(builder, node.y())
            nodeOffset = Node.NodeEnd(builder)

            # Add the node to the list of serialized nodes
            nodeOffsets.append(nodeOffset)

        # Serialize the list of nodes
        NodeGraph.NodeGraphStartNodesVector(builder, len(nodeOffsets))
        for nodeOffset in reversed(nodeOffsets):
            builder.PrependUOffsetTRelative(nodeOffset)
        nodes = builder.EndVector(len(nodeOffsets))

        # Serialize the list of output ranges
        outputRangeOffsets = []

        outputNode = nodeScene.outputNode
        for inputConnectionPoint in outputNode.inputs:

            OutputRange.OutputRangeStart(builder),
            if inputConnectionPoint.connections:
                outputRange = inputConnectionPoint.textBox.range
                OutputRange.OutputRangeAddMinimum(builder, outputRange[0])
                OutputRange.OutputRangeAddMaximum(builder, outputRange[1])
            else:
                OutputRange.OutputRangeAddMinimum(builder, 0)
                OutputRange.OutputRangeAddMaximum(builder, 1)

            outputRangeOffsets.append(OutputRange.OutputRangeEnd(builder))

        NodeGraph.NodeGraphStartOutputRangesVector(builder, len(outputRangeOffsets))
        for outputRangeOffset in reversed(outputRangeOffsets):
            builder.PrependUOffsetTRelative(outputRangeOffset)
        outputRanges = builder.EndVector(len(outputRangeOffsets))
            

        # Serialize the node graph itself
        NodeGraph.NodeGraphStart(builder)
        NodeGraph.NodeGraphAddNodes(builder, nodes)
        NodeGraph.NodeGraphAddOutputRanges(builder, outputRanges)
        nodeGraphOffset = NodeGraph.NodeGraphEnd(builder)

        return nodeGraphOffset









        
