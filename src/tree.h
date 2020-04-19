#include <string>
#include <vector>
#include <stack>
#include <algorithm>


namespace inmem {


// generic node
class Node
{
	protected:
		bool isLeaf_;
		vector<std::string> keys_;

	public:
		bool Get_IsLeaf();
		vector<float> Get_Keys();
		virtual void Insert(float key, string value){}
		virtual void Insert(float key, Node* rightChild){}
		virtual void Insert(float key, Node* leftChild, Node* rightChild){}
		virtual void Search(float key){}
		virtual void Search(float key1, float key2){}
		virtual Node* Split(float* keyToParent){}
		virtual vector<Node*> Get_Children(){}
		virtual vector< vector <string> > Get_Values(){}
		virtual Node* Get_Next(){}
};


// internal node
class InternalNode : public Node
{
	private:
		vector<Node*> children;

	public:
		InternalNode();
		void Insert(std::string key, Node* rightChild);
		void Insert(std::string key, Node* leftChild, Node* rightChild);
		Node* Split(char* keyToParent, int size);
		vector<Node*> Get_Children();
};


// leaf node
class LeafNode : public Node
{
	private:
		LeafNode* prev;
		LeafNode* next;
		vector< vector <string> > values;

	public:
		LeafNode();
		void Insert(std::string key, string value);
		Node* Split(char* keyToParent, int size);
		vector< vector <string> > Get_Values();
		Node* Get_Next();
};


// B+ tree
class BPlusTree
{
	private:
		int order;	
		Node* root;
		void Search_Path(Node* node, float key, stack<Node*>* path);
		void Destroy(Node* node);


		void Reveal_Tree(Node* node);

	public:
		void Initialize(int m);
		void Insert(float key, string value);
		void Search(float key);
		void Search(float key1, float key2);

		~BPlusTree();

		void Print_Tree();

};

}
