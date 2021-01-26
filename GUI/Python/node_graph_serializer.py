from AmpedUpNodes import ConstantValueFloat, Connection, Input, InputValue, Node, NodeGraph

class NodeGraphSerializer:

    @classmethod
    def deserialize(clss, nodeGraph, nodeScene):
        for nodeIndex in range(nodeGraph.NodesLength()):
            node = nodeGraph.Nodes(nodeIndex)
            print(str(nodeIndex) + ": " + str(node.Type()))

            for inputIndex in range(node.InputsLength()):
                input = node.Inputs(inputIndex)

                if input.ValueType() == InputValue.InputValue().Connection:
                    connectionTable = input.Value()
                    connection = Connection.Connection()
                    connection.Init(connectionTable.Bytes, connectionTable.Pos)

                    print("    " + str(connection.NodeIndex()) + " " + str(connection.OutputIndex()))
                elif input.ValueType() == InputValue.InputValue().ConstantValueFloat:
                    constantValueTable = input.Value()
                    constantValue = ConstantValueFloat.ConstantValueFloat()
                    constantValue.Init(constantValueTable.Bytes, constantValueTable.Pos)

                    print("    " + str(constantValue.Value()))
                else:
                    print("    ERROR: No value type supplied")


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
            nodeOffset = Node.NodeEnd(builder)

            # Add the node to the list of serialized nodes
            nodeOffsets.append(nodeOffset)

        # Serialize the list of nodes
        NodeGraph.NodeGraphStartNodesVector(builder, len(nodeOffsets))
        for nodeOffset in reversed(nodeOffsets):
            builder.PrependUOffsetTRelative(nodeOffset)
        nodes = builder.EndVector(len(nodeOffsets))

        # Serialize the node graph itself
        NodeGraph.NodeGraphStart(builder)
        NodeGraph.NodeGraphAddNodes(builder, nodes)
        nodeGraphOffset = NodeGraph.NodeGraphEnd(builder)

        return nodeGraphOffset









        
