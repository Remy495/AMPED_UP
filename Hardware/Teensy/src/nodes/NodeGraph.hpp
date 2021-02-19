
#ifndef _NODE_GRAPH_
#define _NODE_GRAPH_

#include <cstring>
#include <type_traits>
#include <iostream>

#include "Node.hpp"

namespace AmpedUp {
    
    class NodeGraph {
    public:

        ///////////////////////////////////////////////////////
        /// @brief Create an empty node graph
        ///
        /// @param nodeCapacity The maximum number of nodes that can be added to this node grpah
        /// @param nodeBufferSize The amount of space required to store all of this node graph's nodes
        ///////////////////////////////////////////////////////
        NodeGraph(size_t nodeCapacity, size_t nodeBufferSize);

        ///////////////////////////////////////////////////////
        /// @brief Destructor
        ///////////////////////////////////////////////////////
        ~NodeGraph();

        ///////////////////////////////////////////////////////
        /// @brief Add a new node to this node graph
        /// @note Space for the new node must have been allocated in advance when the node graph was constructed
        ///
        /// @param node The node to add (will be added by copy)
        ///
        /// @return boolean status code; true if the node was added successfully, false if an error occured
        ///////////////////////////////////////////////////////
        template<typename T>
        bool addNode(const T& node);

        ///////////////////////////////////////////////////////
        /// @brief Get a node in this node graph by its index
        ///
        /// @param index the index of the node to retrieve
        ///////////////////////////////////////////////////////
        Node& getNode(size_t index);

        ///////////////////////////////////////////////////////
        /// @brief Get a node in this node graph by its index
        ///
        /// @param index the index of the node to retrieve
        ///////////////////////////////////////////////////////
        const Node& getNode(size_t index) const;

        void makeConnection(size_t sourceNodeIndex, size_t sourceOutputIndex, size_t destNodeIndex, size_t destInputIndex);

        void makeFixedValue(size_t destNodeIndex, size_t destInputIndex, NodeValue value);

        ///////////////////////////////////////////////////////
        /// @brief Update all of the nodes in this node graph and recalculate the output values
        ///////////////////////////////////////////////////////
        void update();

        ///////////////////////////////////////////////////////
        /// @brief Set a particular node in this node graph as the output node.
        /// @details The outputs of the designated output node are the outputs of the node graph as a whole
        ///
        /// @param index The index of the output node
        ///////////////////////////////////////////////////////
        void setOutputNode(size_t index);

        size_t getOutputNodeIndex() const;

        ///////////////////////////////////////////////////////
        /// @brief Get the number of outputs this node graph has
        ///////////////////////////////////////////////////////
        size_t numOutputs() const;

        ///////////////////////////////////////////////////////
        /// @brief Get the value of one of this node graph's outputs
        ///////////////////////////////////////////////////////
        NodeValue getOutputValue(size_t outputIndex) const;

    private:
        char* runtimeBufferPtr_{nullptr}; //<! Pointer to a dynamically allocated buffer; contains both node buffer and node pointer index
        size_t runtimeBufferSize_{0}; //<! Size of runtime buffer

        char* nextNodePtr_{nullptr}; //<! Pointer into the runtime buffer, represents where the first byte of the next node should go

        size_t nodeCount_{0}; //<! Number of nodes that are currently in the node graph
        size_t maxNodeCount_{0}; //<! Maximum number of nodes that can be added to the node graph

        size_t outputNodeIndex_{0}; //<! Index of the designated output node

        ///////////////////////////////////////////////////////
        /// @brief Get a pointer to the node buffer
        /// @details in order to reduce heap fragmentation, all of the nodes in a node graph are stored back-to-back in
        ///          one large buffer, as opposed to allocating memory for each node separately
        ///////////////////////////////////////////////////////
        char* nodeBuffer();

        ///////////////////////////////////////////////////////
        /// @brief Get a pointer to the node buffer
        /// @details in order to reduce heap fragmentation, all of the nodes in a node graph are stored back-to-back in
        ///          one large buffer, as opposed to allocating memory for each node separately
        ///////////////////////////////////////////////////////
        const char* nodeBuffer() const;

        ///////////////////////////////////////////////////////
        /// @brief Get the size (in bytes) of the node buffer
        ///////////////////////////////////////////////////////
        size_t nodeBufferSize() const;

        ///////////////////////////////////////////////////////
        /// @brief Get the amount of space (in bytes) within the node buffer that is currently occupied by nodes
        ///////////////////////////////////////////////////////
        size_t nodeBufferSpaceUsed() const;

        ///////////////////////////////////////////////////////
        /// @brief Get the amount of free space (int bytes) withint the node buffer
        ///////////////////////////////////////////////////////
        size_t nodeBufferSpaceRemaining() const;

        ///////////////////////////////////////////////////////
        /// @brief Get a pointer to the node index
        /// @details Nodes in the node buffer are "disorganized" meaning that there is no regular spacing or in-band
        ///          metadata indicating where one node ends and the next begins. Thus it is necessary to keep an array
        ///          of pointers indicating the start of each node
        ///////////////////////////////////////////////////////
        Node** nodePtrIndex();

        ///////////////////////////////////////////////////////
        /// @brief Get a pointer to the node index
        /// @details Nodes in the node buffer are "disorganized" meaning that there is no regular spacing or in-band
        ///          metadata indicating where one node ends and the next begins. Thus it is necessary to keep an array
        ///          of pointers indicating the start of each node
        ///////////////////////////////////////////////////////
        const Node** nodePtrIndex() const;

        ///////////////////////////////////////////////////////
        /// @brief Get the size (in bytes) of the node pointer index
        ///////////////////////////////////////////////////////
        size_t nodePtrIndexSize() const;
    };

    ///////////////////////////////////////////////////////
    /// @brief Add a new node to this node graph
    /// @note Space for the new node must have been allocated in advance when the node graph was constructed
    ///
    /// @param node The node to add (will be added by copy)
    ///
    /// @return boolean status code; true if the node was added successfully, false if an error occured
    ///////////////////////////////////////////////////////
    template<typename T>
    bool NodeGraph::addNode(const T& node) {
        bool status = true;

        // Check that there is room for another node in the index
        if (nodeCount_ >= maxNodeCount_) {
            status = false;
        }
        
        // Check that there is enough space to store the new node 
        if (nodeBufferSpaceRemaining() < sizeof(T)) {
            status = false;
        }

        
        if (status) {
            // Copy the node into the node buffer
            T& newNode = *reinterpret_cast<T*>(nextNodePtr_);
            // FIXME: Copy-assigning polymorphic objects does not copy the vtable ptr. When copying a polymorphic object
            //        into uninitialized memory, it is necessary to manually set the vtable ptr. Find a better way to do
            //        this.
            memcpy(nextNodePtr_, reinterpret_cast<const void*>(&node), sizeof(T));
            newNode = node;
            // Update the node index
            nodePtrIndex()[nodeCount_] = &newNode;

            // Update counters to reflect the new node
            nextNodePtr_ += sizeof(T);
            nodeCount_++;
        }

        return status;
    }

}

#endif