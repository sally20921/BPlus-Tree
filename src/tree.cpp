#include "tree.h"


namespace inmem {
    /*Generic Node*/
    // getter function for accessing isLeaf
    bool Node :: Get_IsLeaf()
    {
        // return whether leaf or internal node
        return isLeaf_;
    }
    
    
    // getter function for accessing keys
    vector<std::string> Node :: Get_Keys()
    {
        // return the vector of keys
        return keys_;
    }
    
    
    /*Internal Node*/
    // constructor for internal node
    InternalNode :: InternalNode()
    {
        isLeaf_ = false;
    }
    
    
    // function for insertion in an internal node
    void InternalNode :: Insert(std::string key, Node* rightChild)
    {
        // insert key in to suitable position in the given internal node
        vector<std::String>::iterator index = lower_bound(keys_.begin(), keys_.end(), key);
        keys_.insert(index, key);
        
        // insert right child in the immediately next index in the children vector
        index = lower_bound(keys_.begin(), keys_.end()_, key);
        children_.insert(children_.begin() + (index - keys_.begin() + 1), rightChild);
    }
    
    
    // function for insertion in a new internal root node
    void InternalNode :: Insert(std::string key, Node* leftChild, Node* rightChild)
    {
        // insert key, left child and right child
        keys_.push_back(key);
        children_.push_back(leftChild);
        children_.push_back(rightChild);
    }
    
    
    // function for splitting an internal node
    Node* InternalNode :: Split(char* keyToParent, int* size)
    {
        int length = keys_.size();
        
        // create a new right internal node
        InternalNode* rightNode = new InternalNode;
        
        // key to be moved up to the parent is the middle element in the current internal node
        std::string temp = keys_[length/2];
        *keyToParent = temp;
        *size = temp.size();
        
        // Copy the second half of the current internal node excluding the middle element to the
        // new right internal node. Erase the second half of the current internal node including
        // the middle element, and thus current internal node becomes the left internal node.
        rightNode->keys_.assign(keys_.begin() + (length/2 + 1), keys_.end());
        rightNode->children_.assign(children_.begin() + (length/2 + 1), children_.end());
        keys_.erase(keys_.begin() + length/2, keys_.end());
        children_.erase(children_.begin() + (length/2 + 1), children_.end());
        
        // return the new right internal node
        return rightNode;
    }
    
    
    // getter function for accessing children
    vector<Node*> InternalNode :: Get_Children()
    {
        // return the children vector
        return children_;
    }
    /*Leaf Node*/
    // constructor for leaf node
    LeafNode :: LeafNode()
    {
        isLeaf_ = true;
        prev_ = this;
        next_ = this;
    }
    
    
    // function for insertion in a leaf node
    void LeafNode :: Insert(std::string key)
    {
        // search for the key in the given leaf node
        vector<std::string>::iterator index = lower_bound(keys_.begin(), keys_.end(), key);
        
        
        // insert the new key
        keys_.insert(index, key);
        
    }
    
    
    // function for splitting a leaf node
    Node* LeafNode :: Split(char* keyToParent, int* size)
    {
        // create a new right leaf node
        LeafNode* rightNode = new LeafNode;
        
        // key to be moved up to the parent is the middle element in the current leaf node
        std::string temp =  keys_[keys_.size()/2]
        *keyToParent = temp;
        *size = temp.size();
        
        
        // Copy the second half of the current leaf node to the new right leaf node. Erase the second
        // half of the current leaf node, and thus the current leaf node becomes the left leaf node.
        rightNode->keys_.assign(keys_.begin() + keys_.size()/2, keys_.end());
        
        keys_.erase(keys_.begin() + keys_.size()/2, keys_.end());
        
        
        // link the leaf nodes to form a doubly linked list
        rightNode->next_ = next_;
        rightNode->prev_ = this;
        next_ = rightNode;
        (rightNode->next)->prev_ = rightNode;
        
        // return the right leaf node
        return rightNode;
    }
    
    
    
    // getter function for accessing the next pointer
    Node* LeafNode :: Get_Next()
    {
        // return the pointer to the next leaf node
        return next_;
    }
    
    /*Tree*/
    BPlusTree :: BPlusTree()
    {
        order = 1024;
        root = NULL;
    }
    
    // function for searching from root to leaf node and pushing on to a stack
        void BPlusTree :: Search_Path(Node* node, std::string key, stack<Node*>* path)
    {
        // push node to stack
        path->push(node);
        
        // check if the node pushed to stack is an internal node
        if(!node->Get_IsLeaf())
        {
            // search for the given key in the current node
            vector<std::string> keys_ = node->Get_Keys();
            vector<Node*> children = node->Get_Children();
            vector<std::string>::iterator index = lower_bound(keys_.begin(), keys_.end(), key);
            
            // check if key is found
            if(key == keys_[index - keys_.begin()])
            {
                // recursively repeat by searching the path through the corresponding right child index
                Search_Path(children[(index - keys_.begin()) + 1], key, path);
            }
            
            // if key is not found
            else
            {
                // recursively repeat by searching the path through the corresponding left child index
                Search_Path(children[index - keys_.begin()], key, path);
            }
        }
    }
        
        // operation: Insert(key, value)
        void BPlusTree :: Insert(std::string key)
    {
        // check if tree is empty
        if(NULL == root)
        {
            // Irrespective of the order, root is always a leaf node for
            // the first insertion. So, create a new leaf node.
            root = new LeafNode;
            root->Insert(key, value);
        }
        
        // if it is a subsequent insertion
        else
        {
            Node* leftNode = NULL;
            Node* rightNode = NULL;
            char* keyToParent = new std::string;
            bool rootPopped = false;
            
            // obtain the search path from the root to leaf node and push it on to a stack
            stack<Node*>* path = new stack<Node*>;
            Search_Path(root, key, path);
            
            // insert the key-value pair in the leaf node
            path->top()->Insert(key, value);
            
            // Split the current node and insert the middle key & children in to the parent. Perform
            // this as long as there is an imbalance in the tree, moving up the stack every iteration.
            while(path->top()->Get_Keys().size() == order)
            {
                // Update the current node as the left half and return the right half. Also
                // obtain the middle element, which is the key to be moved up to the parent.
                leftNode = path->top();
                rightNode = leftNode->Split(keyToParent);
                
                // check if currently split node is not the root node
                path->pop();
                if(!path->empty())
                {
                    // Insert the middle key and the right half in to
                    // the parent. The parent will be an internal node.
                    path->top()->Insert(*keyToParent, rightNode);
                }
                
                // if currently split node is the root node
                else
                {
                    // set flag indicating that the root has popped from the stack
                    rootPopped = true;
                    break;
                }
            }
            
            // new internal node needs to be created and assigned as the root
            if(rootPopped)
            {
                // create a new internal node
                InternalNode* tempRoot = new InternalNode;
                
                // insert the left and the right halves as the children of this new internal node
                tempRoot->Insert(*keyToParent, leftNode, rightNode);
                
                // mark this new internal node as the root of the tree
                root = tempRoot;
            }
            
            delete(keyToParent);
            delete(path);
        }
    }
        
        
        // operation: Search(key)
        void BPlusTree :: Search(std::string key)
    {
        // check if tree is empty
        if(NULL == root)
        {
            outputFile<<"Null"<<endl;
        }
        
        // if it is a vaild search
        else
        {
            int i = 0;
            
            // obtain the search path from root to leaf node and push it on to a stack
            stack<Node*>* path = new stack<Node*>;
            Search_Path(root, key, path);
            
            // search for the key in the leaf node, which is at the top of the stack
            vector<std::string> keys = path->top()->Get_Keys();
            vector< vector <string> > values = path->top()->Get_Values();
            vector<std::string>::iterator index = lower_bound(keys.begin(), keys.end(), key);
            
            // check if key is found
            if(key == keys[index - keys.begin()])
            {
                // display the values
                for(i = 0; i < values[index - keys.begin()].size() - 1; i++)
                {
                    outputFile<<values[index - keys.begin()][i]<<",";
                }
                outputFile<<values[index - keys.begin()][i]<<endl;
            }
            
            // if key is not found
            else
            {
                outputFile<<"Null"<<endl;
            }
            
            delete(path);
        }
    }
