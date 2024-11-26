#include <iostream>
#include <vector>
#include <memory>
#include <algorithm> // For std::lower_bound and std::upper_bound

// Define Node Structure
class Node {
public:
    bool isLeaf;                         // True if leaf node
    std::vector<int> keys;               // Keys stored in the node
    std::vector<std::shared_ptr<Node>> children; // Pointers to child nodes (only for internal nodes)
    std::vector<std::string> values;     // Values (only for leaf nodes)
    std::shared_ptr<Node> next;          // Pointer to the next leaf node (only for leaf nodes)

    Node(bool leaf) : isLeaf(leaf), next(nullptr) {}

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

    void insert(int key, const std::string& value) {
        if (root->keys.size() == MAX_KEYS) {
            auto newRoot = std::make_shared<Node>(false);
            newRoot->children.push_back(root);
            splitNode(root, newRoot, 0);
            root = newRoot;
        }
        insertRecursive(root, key, value);
    }

    std::string search(int key) {
        return searchRecursive(root, key);
    }

    void printTree() {
        printRecursive(root, 0);
    }

private:
    void splitNode(std::shared_ptr<Node> child, std::shared_ptr<Node> parent, int index) {
    auto sibling = std::make_shared<Node>(child->isLeaf);
    int mid = MAX_KEYS / 2;

    // Adjust key and child splitting
    if (child->isLeaf) {
        // For leaf nodes, split values and keys
        sibling->keys.assign(child->keys.begin() + mid, child->keys.end());
        sibling->values.assign(child->values.begin() + mid, child->values.end());
        
        child->keys.resize(mid);
        child->values.resize(mid);

        sibling->next = child->next;
        child->next = sibling;

        // Insert the first key of the sibling into the parent
        parent->keys.insert(parent->keys.begin() + index, sibling->keys[0]);
        parent->children.insert(parent->children.begin() + index + 1, sibling);
    } else {
        // For internal nodes, more complex splitting
        sibling->keys.assign(child->keys.begin() + mid + 1, child->keys.end());
        sibling->children.assign(child->children.begin() + mid + 1, child->children.end());
        
        // The median key goes up to the parent
        int medianKey = child->keys[mid];
        
        child->keys.resize(mid);
        child->children.resize(mid + 1);

        // Insert the median key and new child into the parent
        parent->keys.insert(parent->keys.begin() + index, medianKey);
        parent->children.insert(parent->children.begin() + index + 1, sibling);
    }
}

void insertRecursive(std::shared_ptr<Node> node, int key, const std::string& value) {
    if (node->isLeaf) {
        // Find the correct insertion point
        auto it = std::lower_bound(node->keys.begin(), node->keys.end(), key);
        int index = it - node->keys.begin();
        
        // Insert key and value
        node->keys.insert(it, key);
        node->values.insert(node->values.begin() + index, value);
    } else {
        // Find the correct child to descend into
        int i = 0;
        while (i < node->keys.size() && key > node->keys[i]) ++i;

        // Check if the child needs splitting
        if (node->children[i]->keys.size() == MAX_KEYS) {
            splitNode(node->children[i], node, i);
            
            // Adjust insertion point after split
            if (i < node->keys.size() && key > node->keys[i]) {
                ++i;
            }
        }
        
        // Recursively insert into the appropriate child
        insertRecursive(node->children[i], key, value);
        }
    }

std::string searchRecursive(std::shared_ptr<Node> node, int key) {
    if (node->isLeaf) {
        auto it = std::lower_bound(node->keys.begin(), node->keys.end(), key);
        int index = it - node->keys.begin();
        if (index < node->keys.size() && node->keys[index] == key) {
            return node->values[index];
        }
        return "Key not found";
    } else {
        // Find the appropriate child for the key
        int i = 0;
        while (i < node->keys.size() && key >= node->keys[i]) ++i;
        return searchRecursive(node->children[i], key);
    }
}

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

    // Insert various keys
    tree.insert(10, "Value 10");
    tree.insert(20, "Value 20");
    tree.insert(5, "Value 5");
    tree.insert(6, "Value 6");
    tree.insert(15, "Value 15");
    tree.insert(12, "Value 12");
    tree.insert(7, "Value 7");

    // Print the tree structure
    std::cout << "Tree Structure:" << std::endl;
    tree.printTree();

    // Test searches
    std::cout << "\nSearch Tests:" << std::endl;
    std::cout << "Search 10: " << tree.search(10) << std::endl;
    std::cout << "Search 15: " << tree.search(15) << std::endl;
    std::cout << "Search 7: " << tree.search(7) << std::endl;
    std::cout << "Search 12: " << tree.search(12) << std::endl;
    std::cout << "Search 30: " << tree.search(30) << std::endl;
}