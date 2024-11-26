#include <iostream>
#include <vector>
#include <memory>
#include <mutex>
#include <algorithm> // For std::lower_bound

// Define Node Structure
class Node {
public:
    bool isLeaf;                         // True if leaf node
    std::vector<int> keys;               // Keys stored in the node
    std::vector<std::shared_ptr<Node>> children; // Pointers to child nodes (only for internal nodes)
    std::vector<std::string> values;     // Values (only for leaf nodes)

    Node(bool leaf) : isLeaf(leaf) {}

    // Add a utility function to print the node for debugging
    void printNode() {
        std::cout << (isLeaf ? "Leaf Node: " : "Internal Node: ");
        for (auto key : keys) {
            std::cout << key << " ";
        }
        std::cout << std::endl;
    }
};

// Define BzTree Structure
class BzTree {
private:
    std::shared_ptr<Node> root;          // Root node of the tree
    static const int MAX_KEYS = 3;       // Maximum number of keys per node for simplicity

public:
    BzTree() {
        root = std::make_shared<Node>(true); // Start with an empty leaf node as the root
    }

    // Function to insert a key-value pair into the tree
    void insert(int key, const std::string& value) {
        // If the root is full, split it and create a new root
        if (root->keys.size() == MAX_KEYS) {
            auto newRoot = std::make_shared<Node>(false); // Internal node
            newRoot->children.push_back(root);           // Old root becomes a child
            splitNode(root, newRoot, 0);                 // Split the root
            root = newRoot;                              // Update the root
        }
        insertRecursive(root, key, value);
    }

    // Function to search for a key in the tree
    std::string search(int key) {
        return searchRecursive(root, key);
    }

    // Function to print the tree
    void printTree() {
        printRecursive(root, 0);
    }

private:
    // Recursive insert helper
    void insertRecursive(std::shared_ptr<Node> node, int key, const std::string& value) {
        if (node->isLeaf) {
            // Insert the key into the leaf node
            auto it = std::lower_bound(node->keys.begin(), node->keys.end(), key);
            int index = it - node->keys.begin();
            node->keys.insert(it, key);
            node->values.insert(node->values.begin() + index, value);
        } else {
            // Find the child to recurse into
            int i = 0;
            while (i < node->keys.size() && key > node->keys[i]) ++i;

            // Recursively insert into the child
            if (node->children[i]->keys.size() == MAX_KEYS) {
                splitNode(node->children[i], node, i);
                if (key > node->keys[i]) ++i;
            }
            insertRecursive(node->children[i], key, value);
        }
    }

    // Split a child node and update the parent
    void splitNode(std::shared_ptr<Node> child, std::shared_ptr<Node> parent, int index) {
        auto sibling = std::make_shared<Node>(child->isLeaf);
        int mid = MAX_KEYS / 2;

        // Move the second half of keys to the sibling
        sibling->keys.assign(child->keys.begin() + mid + 1, child->keys.end());
        child->keys.resize(mid);

        if (child->isLeaf) {
            sibling->values.assign(child->values.begin() + mid + 1, child->values.end());
            child->values.resize(mid);
        } else {
            sibling->children.assign(child->children.begin() + mid + 1, child->children.end());
            child->children.resize(mid + 1);
        }

        // Insert the middle key into the parent
        parent->keys.insert(parent->keys.begin() + index, child->keys[mid]);
        parent->children.insert(parent->children.begin() + index + 1, sibling);

        // Clear the middle key from the child
        if (!child->isLeaf) {
            child->keys.pop_back(); // Remove the middle key
        }
    }

    // Recursive search helper
    std::string searchRecursive(std::shared_ptr<Node> node, int key) {
        if (node->isLeaf) {
            for (size_t i = 0; i < node->keys.size(); ++i) {
                if (node->keys[i] == key) {
                    return node->values[i];
                }
            }
            return "Key not found";
        } else {
            for (size_t i = 0; i < node->keys.size(); ++i) {
                if (key < node->keys[i]) {
                    return searchRecursive(node->children[i], key);
                }
            }
            return searchRecursive(node->children.back(), key);
        }
    }

    // Recursive print helper
    void printRecursive(std::shared_ptr<Node> node, int level) {
        for (int i = 0; i < level; ++i) std::cout << "  ";
        node->printNode();
        if (!node->isLeaf) {
            for (auto child : node->children) {
                printRecursive(child, level + 1);
            }
        }
    }
};

int main() {
    BzTree tree;

    // Insert keys and values
    tree.insert(10, "A");
    tree.insert(20, "B");
    tree.insert(5, "C");
    tree.insert(6, "D");
    tree.insert(15, "E");

    // Print the tree structure
    tree.printTree();

    return 0;
}
