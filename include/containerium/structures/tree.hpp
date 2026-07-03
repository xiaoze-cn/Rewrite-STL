#pragma once

#include <algorithm>
#include <functional>
#include <queue>
#include <stack>
#include <containerium/core/error.hpp>

namespace containerium {

template<typename T>
class BinaryTree {
public:
    struct Node {
        T data;
        Node* parent;
        Node* left;
        Node* right;
    };

protected:
    Node* root;

public:
    BinaryTree() : root(nullptr) {}

    virtual ~BinaryTree() { clear(); }

    std::expected<void, DataStructureError> createRoot(const T& value) {
        if (root != nullptr) return std::unexpected(DataStructureError::ContainerIsFull);
        root = new Node{value, nullptr, nullptr, nullptr};
        return {};
    }

    bool isEmpty() const { return root == nullptr; }

    int getHeight(Node* node) const {
        if (node == nullptr) return 0;
        return std::max(getHeight(node->left), getHeight(node->right)) + 1;
    }

    std::expected<int, DataStructureError> getDepth(Node* node) const {
        if (node == nullptr) return std::unexpected(DataStructureError::ContainerIsEmpty);
        if (node->parent == nullptr) return 0;
        return getDepth(node->parent).value() + 1;
    }

    int countNodes(Node* node) const {
        if (node == nullptr) return 0;
        return countNodes(node->left) + countNodes(node->right) + 1;
    }

    int countLeaves(Node* node) const {
        if (node == nullptr) return 0;
        if (node->left == nullptr && node->right == nullptr) return 1;
        return countLeaves(node->left) + countLeaves(node->right);
    }

    //std::function<void(Node*)> visitor = [](auto* node) { std::cout << node->data << " "; };
    std::expected<void, DataStructureError> preorderRecursive(Node* node, std::function<void(Node*)> visitor) const {
        if (node == nullptr) return std::unexpected(DataStructureError::ContainerIsEmpty);
        visitor(node);
        if (node->left) preorderRecursive(node->left, visitor);
        if (node->right) preorderRecursive(node->right, visitor);
        return {};
    }

    std::expected<void, DataStructureError> inorderRecursive(Node* node, std::function<void(Node*)> visitor) const {
        if (node == nullptr) return std::unexpected(DataStructureError::ContainerIsEmpty);
        if (node->left) inorderRecursive(node->left, visitor);
        visitor(node);
        if (node->right) inorderRecursive(node->right, visitor);
        return {};
    }

    std::expected<void, DataStructureError> postorderRecursive(Node* node, std::function<void(Node*)> visitor) const {
        if (node == nullptr) return std::unexpected(DataStructureError::ContainerIsEmpty);
        if (node->left) postorderRecursive(node->left, visitor);
        if (node->right) postorderRecursive(node->right, visitor);
        visitor(node);
        return {};
    }

    std::expected<void, DataStructureError> preorderIterative(Node* node, std::function<void(Node*)> visitor) const {
        if (node == nullptr) return std::unexpected(DataStructureError::ContainerIsEmpty);
        std::stack<Node*> stack;
        stack.push(node);
        while (!stack.empty()) {
            Node* current = stack.top();
            stack.pop();
            visitor(current);
            if (current->right) stack.push(current->right);
            if (current->left) stack.push(current->left);
        }
        return {};
    }

    std::expected<void, DataStructureError> inorderIterative(Node* node, std::function<void(Node*)> visitor) const {
        if (node == nullptr) return std::unexpected(DataStructureError::ContainerIsEmpty);
        std::stack<Node*> stack;
        Node* current = node;
        while (current != nullptr || !stack.empty()) {
            while (current != nullptr) {
                stack.push(current);
                current = current->left;
            }
            current = stack.top();
            stack.pop();
            visitor(current);
            current = current->right;
        }
        return {};
    }

    std::expected<void, DataStructureError> postorderIterative(Node* node, std::function<void(Node*)> visitor) const {
        if (node == nullptr) return std::unexpected(DataStructureError::ContainerIsEmpty);
        std::stack<Node*> stack;
        Node* current = node;
        Node* lastVisited = nullptr;
        while (current != nullptr || !stack.empty()) {
            if (current != nullptr) {
                stack.push(current);
                current = current->left;
            } 
            else {
                Node* peekNode = stack.top();
                if (peekNode->right != nullptr && lastVisited != peekNode->right) {
                    current = peekNode->right;
                } 
                else {
                    visitor(peekNode);
                    lastVisited = stack.top();
                    stack.pop();
                }
            }
        }
        return {};
    }

    std::expected<void, DataStructureError> levelorder(std::function<void(Node*)> visitor) const {
        if (root == nullptr ) return std::unexpected(DataStructureError::ContainerIsEmpty);
        std::queue<Node*> queue;
        queue.push(root);
        while (!queue.empty()) {
            Node* current = queue.front();
            queue.pop();
            visitor(current);
            if (current->left) queue.push(current->left);
            if (current->right) queue.push(current->right);
        }
        return {};
    }

    std::expected<void, DataStructureError> insertLeft(Node* parent, const T& value) {
        if (parent == nullptr) return std::unexpected(DataStructureError::InvalidArgument);
        if (parent->left != nullptr) return std::unexpected(DataStructureError::ContainerIsFull);
        parent->left = new Node{value, parent, nullptr, nullptr};
        return {};
    }

    std::expected<void, DataStructureError> insertRight(Node* parent, const T& value) {
        if (parent == nullptr) return std::unexpected(DataStructureError::InvalidArgument);
        if (parent->right != nullptr) return std::unexpected(DataStructureError::ContainerIsFull);
        parent->right = new Node{value, parent, nullptr, nullptr};
        return {};
    }

    std::expected<void, DataStructureError> betweenLeft(Node* parent, const T& value) {
        if (parent == nullptr) return std::unexpected(DataStructureError::InvalidArgument);
        if (parent->left == nullptr) return std::unexpected(DataStructureError::ContainerIsEmpty);
        Node* newNode = new Node{value, parent, parent->left, nullptr};
        parent->left->parent = newNode;
        parent->left = newNode;
        return {};
    }

    std::expected<void, DataStructureError> betweenRight(Node* parent, const T& value) {
        if (parent == nullptr) return std::unexpected(DataStructureError::InvalidArgument);
        if (parent->right == nullptr) return std::unexpected(DataStructureError::ContainerIsEmpty);
        Node* newNode = new Node{value, parent, parent->right, nullptr};
        parent->right->parent = newNode;
        parent->right = newNode;
        return {};
    }

    std::expected<T, DataStructureError> get(Node* node) const {
        if (node == nullptr) return std::unexpected(DataStructureError::ContainerIsEmpty);
        return node->data;
    }

    std::expected<T, DataStructureError> getLeft(Node* node) const {
        if (node == nullptr) return std::unexpected(DataStructureError::ContainerIsEmpty);
        if (node->left == nullptr) return std::unexpected(DataStructureError::ContainerIsEmpty);
        return node->left->data;
    }

    std::expected<Node*, DataStructureError> getLeftNode(Node* node) const {
        if (node == nullptr) return std::unexpected(DataStructureError::ContainerIsEmpty);
        if (node->left == nullptr) return std::unexpected(DataStructureError::ContainerIsEmpty);
        return node->left;
    }

    std::expected<T, DataStructureError> getRight(Node* node) const {
        if (node == nullptr) return std::unexpected(DataStructureError::ContainerIsEmpty);
        if (node->right == nullptr) return std::unexpected(DataStructureError::ContainerIsEmpty);
        return node->right->data;
    }

    std::expected<Node*, DataStructureError> getRightNode(Node* node) const {
        if (node == nullptr) return std::unexpected(DataStructureError::ContainerIsEmpty);
        if (node->right == nullptr) return std::unexpected(DataStructureError::ContainerIsEmpty);
        return node->right;
    }

    std::expected<T, DataStructureError> getParent(Node* node) const {
        if (node == nullptr) return std::unexpected(DataStructureError::ContainerIsEmpty);
        if (node->parent == nullptr) return std::unexpected(DataStructureError::ContainerIsEmpty);
        return node->parent->data;
    }

    std::expected<Node*, DataStructureError> getParentNode(Node* node) const {
        if (node == nullptr) return std::unexpected(DataStructureError::ContainerIsEmpty);
        if (node->parent == nullptr) return std::unexpected(DataStructureError::ContainerIsEmpty);
        return node->parent;
    }

    std::expected<T, DataStructureError> getRoot() const {
        return get(root);
    }

    std::expected<Node*, DataStructureError> getRootNode() const {
        if (root == nullptr) return std::unexpected(DataStructureError::ContainerIsEmpty);
        return root;
    }

    std::expected<Node*, DataStructureError> find(const T& value) const {
        if (root == nullptr) return std::unexpected(DataStructureError::ContainerIsEmpty);
        if (root->data == value) return root;
        auto leftNode = find(root->left, value);
        return leftNode.has_value() ? leftNode : find(root->right, value);
    }

    void erase(Node* node) {
        if (node != nullptr) {
            erase(node->left);
            erase(node->right);
            delete node;
        }
    }

    std::expected<void, DataStructureError> eraseLeft(Node* parent) {
        erase(parent->left);
        parent->left = nullptr;
        return {};
    }
    
    std::expected<void, DataStructureError> eraseRight(Node* parent) {
        erase(parent->right);
        parent->right = nullptr;
        return {};
    }

    std::expected<T, DataStructureError> update(Node* node, const T& newValue) {
        if (node == nullptr) return std::unexpected(DataStructureError::ContainerIsEmpty);
        T value = node->data;
        node->data = newValue;
        return value;
    }

    bool isEqual(Node* node1, Node* node2) const {
        if (node1 == nullptr && node2 == nullptr) return true;
        if (node1 == nullptr || node2 == nullptr) return false;
        if (node1->data != node2->data) return false;
        return isEqual(node1->left, node2->left) && isEqual(node1->right, node2->right);
    }

    std::expected<bool, DataStructureError> isCompleteTree() const {
        if (root == nullptr) return std::unexpected(DataStructureError::ContainerIsEmpty);
        std::queue<Node*> queue;
        queue.push(root);
        bool foundNull = false;
        while(!queue.empty()) {
            Node* current = queue.front();
            queue.pop();
            if (current == nullptr) foundNull = true;
            else {
                if (foundNull) return false;
                queue.push(current->left);
                queue.push(current->right);
            }
        }
        return true;
    }

    std::expected<bool, DataStructureError> isFullTree(Node* node = nullptr) const {
        if (node == nullptr) {
            if (root == nullptr ) return std::unexpected(DataStructureError::ContainerIsEmpty);
            node = root;
        }
        if (node->left == nullptr && node->right == nullptr) return true;
        if (node->left == nullptr || node->right == nullptr) return false;
        return isFullTree(node->left).value() && isFullTree(node->right).value();
    }

    std::expected<bool, DataStructureError> isBalanced(Node* node = nullptr) const {
        if (node == nullptr) {
            if (root == nullptr) return std::unexpected(DataStructureError::ContainerIsEmpty);
            node = root;
        }
        if (node->left == nullptr && node->right == nullptr) return true;
        int leftHeight = getHeight(node->left);
        int rightHeight = getHeight(node->right);
        if (std::abs(leftHeight - rightHeight) > 1) return false;
        return isBalanced(node->left).value() && isBalanced(node->right).value();
    }

    void clear() {
        erase(root);
        root = nullptr;
    }
};

template<typename T>
class BinarySearchTree : public BinaryTree<T> {
public:
    using Node = typename BinaryTree<T>::Node;

protected:
    using BinaryTree<T>::root;

public:
    BinarySearchTree() : BinaryTree<T>() {}

    std::expected<void, DataStructureError> insert(const T& value) {
        if (root == nullptr) {
            root = new Node{value, nullptr, nullptr, nullptr};
            return {};
        }
        Node* current = root;
        while (current != nullptr) {
            if (value < current->data) {
                if (current->left == nullptr) {
                    current->left = new Node{value, current, nullptr, nullptr};
                    break;
                }
                current = current->left;
            }
            else if (value > current->data) {
                if (current->right == nullptr) {
                    current->right = new Node{value, current, nullptr, nullptr};
                    break;
                }
                current = current->right;
            }
            else return std::unexpected(DataStructureError::DuplicateValue);
        }
        return {};
    }

    std::expected<Node*, DataStructureError> find(const T& value) {
        Node* current = root;
        while (current != nullptr) {
            if (value == current->data) return current;
            current = (value < current->data) ? current->left : current->right;
        }
        return std::unexpected(DataStructureError::ElementNotFound);
    }

    std::expected<T, DataStructureError> getMin() {
        if (root == nullptr) return std::unexpected(DataStructureError::ContainerIsEmpty);
        Node* current = root;
        while (current->left != nullptr) current = current->left;
        return current->data;
    }

    std::expected<T, DataStructureError> getMax() {
        if (root == nullptr) return std::unexpected(DataStructureError::ContainerIsEmpty);
        Node* current = root;
        while (current->right != nullptr) current = current->right;
        return current->data;
    }

    std::expected<Node*, DataStructureError> remove(const T& value) {
        if (root == nullptr) return std::unexpected(DataStructureError::ContainerIsEmpty);
        CONTAINERIUM_TRY(target, find(value));
        Node* rebalanceStart = nullptr;
        if (target->left == nullptr && target->right == nullptr) {
            rebalanceStart = target->parent;
            if (target == root) root = nullptr;
            else if (target == target->parent->left) target->parent->left = nullptr;
            else target->parent->right = nullptr;
            delete target;
        }
        else if (target->left == nullptr || target->right == nullptr) {
            Node* child = target->left ? target->left : target->right;
            rebalanceStart = target->parent;
            if (target == root) {
                root = child;
                root->parent = nullptr;
            }
            else if (target->parent->left == target) {
                target->parent->left = child;
                child->parent = target->parent;
            }
            else {
                target->parent->right = child;
                child->parent = target->parent;
            }
            delete target;
        }
        else {
            Node* successor = target->right;
            while (successor->left != nullptr) successor = successor->left;
            target->data = successor->data;
            rebalanceStart = successor->parent;
            if (successor->parent->left == successor) successor->parent->left = successor->right;
            else successor->parent->right = successor->right;
            if (successor->right) successor->right->parent = successor->parent;
            delete successor;
        }
        return rebalanceStart;
    }
};

template<typename T>
class AVLTree : public BinarySearchTree<T> {
public:
    using Node = typename BinaryTree<T>::Node;
    using BinaryTree<T>::root;

protected:
    int getBalanceFactor (Node* node) const {
        if (node == nullptr) return 0;
        return getHeight(node->left) - getHeight(node->right);
    }

    void rotateLeft(Node* node) {
        Node* newRoot = node->right;
        Node* orphan = newRoot->left;
        newRoot->left = node;
        node->right = orphan;
        newRoot->parent = node->parent;
        node->parent = newRoot;
        if (orphan) orphan->parent = node;
        if (newRoot->parent == nullptr) root = newRoot;
        else if (newRoot->parent->left == node) newRoot->parent->left = newRoot;
        else newRoot->parent->right = newRoot;
    }

    void rotateRight(Node* node) {
        Node* newRoot = node->left;
        Node* orphan = newRoot->right;
        newRoot->right = node;
        node->left = orphan;
        newRoot->parent = node->parent;
        node->parent = newRoot;
        if (orphan) orphan->parent = node;
        if (newRoot->parent == nullptr) root = newRoot;
        else if (newRoot->parent->left == node) newRoot->parent->left = newRoot;
        else newRoot->parent->right = newRoot;
    }

    void rebalance(Node* node) {
        while (node != nullptr) {
            int balance = getBalanceFactor(node);
            if (balance > 1 && getBalanceFactor(node->left) >= 0) rotateRight(node);
            else if (balance > 1 && getBalanceFactor(node->left) < 0) {
                rotateLeft(node->left);
                rotateRight(node);
            }
            else if (balance < -1 && getBalanceFactor(node->right) <= 0) rotateLeft(node);
            else if (balance < -1 && getBalanceFactor(node->right) > 0) {
                rotateRight(node->right);
                rotateLeft(node);
            }
            node = node->parent;
        }
    }

public:
    AVLTree() : BinarySearchTree<T>() {}
    
    std::expected<void, DataStructureError> insert(const T& value) {
        if (root == nullptr) {
            root = new Node(value, nullptr, nullptr, nullptr);
            return {};
        }
        Node* current = root;
        while (current != nullptr) {
            if (value < current->data) {
                if (current->left == nullptr) {
                    current->left = new Node(value, nullptr, nullptr, current);
                    break;
                }
                current = current->left;
            } 
            else if (value > current->data) {
                if (current->right == nullptr) {
                    current->right = new Node(value, nullptr, nullptr, current);
                    break;
                }
                current = current->right;
            } 
            else return std::unexpected(DataStructureError::DuplicateValue);
        }
        rebalance(current->parent);
        return {};
    }

    std::expected<void, DataStructureError> remove(const T& value) {
        CONTAINERIUM_TRY(rebalanceStart, BinarySearchTree<T>::remove(value));
        rebalance(rebalanceStart);
        return {};
    }
};

template<typename T>
class RedBlackTree {
public:
    enum Color {RED, BLACK};
    struct Node {
        T data;
        Node* parent;
        Node* left;
        Node* right;
        Color color;
    };

protected:
    Node* root;

    void rotateLeft(Node* node) {
        Node* newRoot = node->right;
        Node* orphan = newRoot->left;
        newRoot->left = node;
        node->right = orphan;
        newRoot->parent = node->parent;
        node->parent = newRoot;
        if (orphan) orphan->parent = node;
        if (newRoot->parent == nullptr) root = newRoot;
        else if (newRoot->parent->left == node) newRoot->parent->left = newRoot;
        else newRoot->parent->right = newRoot;
    }

    void rotateRight(Node* node) {
        Node* newRoot = node->left;
        Node* orphan = newRoot->right;
        newRoot->right = node;
        node->left = orphan;
        newRoot->parent = node->parent;
        node->parent = newRoot;
        if (orphan) orphan->parent = node;
        if (newRoot->parent == nullptr) root = newRoot;
        else if (newRoot->parent->left == node) newRoot->parent->left = newRoot;
        else newRoot->parent->right = newRoot;
    }

    void insertFixUp(Node* node) {
        while (node->parent != nullptr && node->parent->color == RED) {
            Node* parent = node->parent;
            Node* grandparent = parent->parent;
            if (parent == grandparent->left) {
                Node* uncle = grandparent->right;
                if (uncle != nullptr && uncle->color == RED) {
                    parent->color = BLACK;
                    uncle->color = BLACK;
                    grandparent->color = RED;
                    node = grandparent;
                } 
                else {
                    if (node == parent->right) {
                        rotateLeft(parent);
                        node = parent;
                        parent = node->parent;
                    }
                    parent->color = BLACK;
                    grandparent->color = RED;
                    rotateRight(grandparent);
                }
            }
            else {
                Node* uncle = grandparent->left;
                if (uncle != nullptr && uncle->color == RED) {
                    parent->color = BLACK;
                    uncle->color = BLACK;
                    grandparent->color = RED;
                    node = grandparent;
                } 
                else {
                    if (node == parent->left) {
                        rotateRight(parent);
                        node = parent;
                        parent = node->parent;
                    }
                    parent->color = BLACK;
                    grandparent->color = RED;
                    rotateLeft(grandparent);
                }
            }
        }
        root->color = BLACK;
    }

    void removeFixUp(Node* node, Node* parent) {
        while (node != root && (node == nullptr || node->color == BLACK)) {
            if (node == parent->left) {
                Node* sibling = parent->right;
                if (sibling != nullptr && sibling->color == RED) {
                    sibling->color = BLACK;
                    parent->color = RED;
                    rotateLeft(parent);
                    sibling = parent->right;
                }
                bool leftChildBlack = (sibling->left == nullptr || sibling->left->color == BLACK);
                bool rightChildBlack = (sibling->right == nullptr || sibling->right->color == BLACK);
                if (leftChildBlack && rightChildBlack) {
                    sibling->color = RED;
                    node = parent;
                    parent = node->parent;
                }
                else {
                    if (rightChildBlack) {
                        if (sibling->left != nullptr) sibling->left->color = BLACK;
                        sibling->color = RED;
                        rotateRight(sibling);
                        sibling = parent->right;
                    }
                    sibling->color = parent->color;
                    parent->color = BLACK;
                    if (sibling->right != nullptr) sibling->right->color = BLACK;
                    rotateLeft(parent);
                    node = root;
                }
            }
            else {
                Node* sibling = parent->left;
                if (sibling != nullptr && sibling->color == RED) {
                    sibling->color = BLACK;
                    parent->color = RED;
                    rotateRight(parent);
                    sibling = parent->left;
                }
                bool leftChildBlack = (sibling->left == nullptr || sibling->left->color == BLACK);
                bool rightChildBlack = (sibling->right == nullptr || sibling->right->color == BLACK);
                if (leftChildBlack && rightChildBlack) {
                    sibling->color = RED;
                    node = parent;
                    parent = node->parent;
                }
                else {
                    if (leftChildBlack) {
                        if (sibling->right != nullptr) sibling->right->color = BLACK;
                        sibling->color = RED;
                        rotateLeft(sibling);
                        sibling = parent->left;
                    }
                    sibling->color = parent->color;
                    parent->color = BLACK;
                    if (sibling->left != nullptr) sibling->left->color = BLACK;
                    rotateRight(parent);
                    node = root;
                }
            }
        }
        if (node != nullptr) node->color = BLACK;
    }

    void erase(Node* node) {
        if (node != nullptr) {
            erase(node->left);
            erase(node->right);
            delete node;
        }
    }

public:
    RedBlackTree() : root(nullptr) {}

    ~RedBlackTree() { clear(); }

    bool isEmpty() const { return root == nullptr; }

    std::expected<Node*, DataStructureError> find(const T& value) const {
        Node* current = root;
        while (current != nullptr) {
            if (value == current->data) return current;
            current = (value < current->data) ? current->left : current->right;
        }
        return std::unexpected(DataStructureError::ElementNotFound);
    }

    std::expected<T, DataStructureError> getMin() const {
        if (root == nullptr) return std::unexpected(DataStructureError::ContainerIsEmpty);
        Node* current = root;
        while (current->left != nullptr) current = current->left;
        return current->data;
    }

    std::expected<T, DataStructureError> getMax() const {
        if (root == nullptr) return std::unexpected(DataStructureError::ContainerIsEmpty);
        Node* current = root;
        while (current->right != nullptr) current = current->right;
        return current->data;
    }

    std::expected <void, DataStructureError> insert(const T& value) {
        Node* newNode = nullptr;
        if (root == nullptr) {
            root = new Node{value, nullptr, nullptr, nullptr, BLACK};
            return {};
        }
        Node* current = root;
        while (current != nullptr) {
            if (value < current->data) {
                if (current->left == nullptr) {
                    newNode = new Node{value, current, nullptr, nullptr, RED};
                    current->left = newNode;
                    break;
                }
                current = current->left;
            }
            else if (value > current->data) {
                if (current->right == nullptr) {
                    newNode = new Node{value, current, nullptr, nullptr, RED};
                    current->right = newNode;
                    break;
                }
                current = current->right;
            }
            else {
                return std::unexpected(DataStructureError::DuplicateValue);
            }
        }
        insertFixUp(newNode);
        return {};
    }
    
    std::expected<void, DataStructureError> remove(const T& value) {
        if (root == nullptr) return std::unexpected(DataStructureError::ContainerIsEmpty);
        CONTAINERIUM_TRY(target, find(value));
        Node* toDelete = target;
        if (target->left != nullptr && target->right != nullptr) {
            toDelete = target->right;
            while (toDelete->left != nullptr) toDelete = toDelete->left;
            target->data = toDelete->data;
        }
        Node* replacement = (toDelete->left != nullptr) ? toDelete->left : toDelete->right;
        Node* replacementParent = toDelete->parent;
        Color deletedColor = toDelete->color;
        if (replacement != nullptr) replacement->parent = toDelete->parent;
        if (toDelete->parent == nullptr) root = replacement;
        else if (toDelete == toDelete->parent->left) toDelete->parent->left = replacement;
        else toDelete->parent->right = replacement;
        delete toDelete;
        if (deletedColor == BLACK && (replacement != nullptr || replacementParent != nullptr)) removeFixUp(replacement, replacementParent);
        return {};
    }

    void clear() {
        erase(root);
        root = nullptr;
    }
};

} // namespace containerium
