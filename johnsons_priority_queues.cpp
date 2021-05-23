#include<bits/stdc++.h> 
using namespace std;
//#pragma warning(disable : 4996)

/*
		program to implement johnsons algorithm using the for data structures provided
		1. Array
		2. Binary heap
		3. Binomial heap
		4. Fibonacci heap

		approach: I have implemented all the four data structures.
		then depending on the command line argument given I call the respective
		functions for the data structure during dijkstra.


*/


/*================================fibbonaci heap code begins================================*/
//the struct for fibonacci heap node
struct fib_node
{
	fib_node* parent;
	fib_node* child;
	fib_node* left;
	fib_node* right;
	int vertex_id;
	int vertex_distance;
	int degree;
	int mark;
};

struct fib_node* fib_min_ptr = NULL;
int fib_num_nodes = 0;
vector<fib_node*> vertex_id_to_fib_node_pointer(10000009);

/*
	function to insert a node in a fibonacci heap
*/

fib_node* fib_insert_node(int vertex_id, int vertex_distance)
{
	struct fib_node* n = (struct fib_node*)malloc(sizeof(struct fib_node));
	n->degree = 0;
	n->parent = NULL;
	n->child = NULL;
	n->mark = 0;
	n->vertex_id = vertex_id;
	n->vertex_distance = vertex_distance;
	n->left = n;
	n->right = n;

	if (fib_min_ptr == NULL)
	{
		fib_min_ptr = n;
	}

	else if (fib_min_ptr != NULL)
	{
		fib_min_ptr->left->right = n;
		n->right = fib_min_ptr;
		n->left = fib_min_ptr->left;
		fib_min_ptr->left = n;
		if (fib_min_ptr->vertex_distance > n->vertex_distance)
		{
			fib_min_ptr = n;
		}
	}

	fib_num_nodes += 1;
	return n;
}

/*
	function to make tree2 child of tree1 in a fibonacci heap by interchanging the pointers.
*/
void fib_make_tree2_childof_tree1(fib_node* tree2, fib_node* tree1)
{
	if (tree1->child == NULL)
	{
		tree2->parent = tree1;
		tree1->child = tree2;
		tree2->left = tree2;
		tree2->right = tree2;
		tree1->degree += 1;
		tree2->mark = 0;
	}
	else if (tree1->child != NULL)
	{
		tree2->parent = tree1;

		tree1->child->left->right = tree2;
		tree2->left = tree1->child->left;
		tree2->right = tree1->child;
		tree1->child->left = tree2;

		tree1->degree += 1;
		tree2->mark = 0;
	}
}

/*
	function to consolidate the trees in i fibonacci heap by merging the trees of same degree
	finally there is at most one tree for a given degree
*/
void fib_consolidate()
{
	int max_degree = log(fib_num_nodes) / log(1.6);
	vector<fib_node*> a(max_degree + 2, NULL);
	vector<fib_node*> root_list;

	fib_node* ptr = fib_min_ptr;
	do
	{
		root_list.push_back(ptr);
		ptr = ptr->right;
	} while (ptr != fib_min_ptr);

	for (int i = 0; i < root_list.size(); i++)
	{
		fib_node* w = root_list[i];
		if (w->parent == NULL)
		{
			fib_node* x = w;
			int d = x->degree;
			while (a[d] != NULL)
			{
				fib_node* y = a[d];
				if (x->vertex_distance > y->vertex_distance)
				{
					fib_node* temp = x;
					x = y;
					y = temp;
				}
				fib_make_tree2_childof_tree1(y, x);
				a[d] = NULL;
				d = d + 1;
			}
			a[d] = x;
		}
	}

	fib_min_ptr = NULL;
	for (int i = 0; i < a.size(); i++)
	{
		if (a[i] != NULL)
		{
			if (fib_min_ptr == NULL)
			{
				a[i]->parent = NULL;
				a[i]->right = a[i];
				a[i]->left = a[i];
				fib_min_ptr = a[i];
			}
			else
			{
				fib_min_ptr->left->right = a[i];
				a[i]->left = fib_min_ptr->left;
				a[i]->right = fib_min_ptr;
				fib_min_ptr->left = a[i];
				if (a[i]->vertex_distance < fib_min_ptr->vertex_distance)
				{
					fib_min_ptr = a[i];
				}
			}
		}
	}
}

/*
	function to return the minimum node in the fibonacci heap. It removes the node from the heap and 
	return it. It puts all the children nodes of the given node in the root list
	It also calls consolidates for the fibonacci heap
*/
fib_node* fib_extract_min()
{
	fib_node* z = fib_min_ptr;

	if (z != NULL)
	{
		if (z->child != NULL)
		{
			fib_node* ch = z->child;
			fib_node* ch_r = ch->right;

			do
			{
				fib_min_ptr->left->right = ch;
				ch->right = fib_min_ptr;
				ch->left = fib_min_ptr->left;
				fib_min_ptr->left = ch;
				ch->parent = NULL;
				//ch->mark = 0;
				ch = ch_r;
				ch_r = ch_r->right;
			} while (ch != z->child);
		}

		z->left->right = z->right;
		z->right->left = z->left;

		if (z == z->right)
		{
			fib_min_ptr = NULL;
		}
		else
		{
			fib_min_ptr = z->right;
			fib_consolidate();
		}
		fib_num_nodes--;
	}
	return z;
}

/*
	function to remove the node x from the children list of node y
	puts the node in the root list of the fibonacci heap. It unmarks the node x
	then it calls function cascading cut to check if y was marked earlier.
	if y was marked it is also removed and cascading cut is called again, else y is marked
*/
void cut(fib_node* x, fib_node* y)
{
	if (x->right == x)
	{
		y->child = NULL;
		y->degree -= 1;

		fib_min_ptr->left->right = x;
		x->left = fib_min_ptr->left;
		x->right = fib_min_ptr;
		fib_min_ptr->left = x;

		x->parent = NULL;
		x->mark = 0;
	}
	else if (x->right != x)
	{
		if (y->child == x)
		{
			y->child = x->right;
		}
		x->right->left = x->left;
		x->left->right = x->right;
		y->degree -= 1;

		fib_min_ptr->left->right = x;
		x->left = fib_min_ptr->left;
		x->right = fib_min_ptr;
		fib_min_ptr->left = x;

		x->parent = NULL;
		x->mark = 0;
	}
}

/*
	function to recursively remove marked nodes whose child has been removed.
*/
void cascading_cut(fib_node* y)
{
	fib_node* z = y->parent;
	if (z != NULL)
	{
		if (y->mark == 0)
		{
			y->mark = 1;
		}
		else
		{
			cut(y, z);
			cascading_cut(z);
		}
	}
}

/*
	function to decrease key in a fibonacci heap. It decreases the key of the node and if now the 
	key is less than the key of the parent the node is cut and put in the root list
*/
void fib_decrease_key(fib_node* x, int decreased_vertex_distance)
{
	if (decreased_vertex_distance > x->vertex_distance)
	{
		cout << "key value is greater compared to current key" << endl;
	}
	else
	{
		x->vertex_distance = decreased_vertex_distance;
		fib_node* y = x->parent;

		if (y != NULL && x->vertex_distance < y->vertex_distance)
		{
			cut(x, y);
			cascading_cut(y);
		}
		if (x->vertex_distance < fib_min_ptr->vertex_distance)
		{
			fib_min_ptr = x;
		}
	}
}
/*================================fibbonaci heap code ends================================*/


/*================================binomial heap code begins================================*/
//struct node of binomial heap
struct node
{
	int vertex_id;
	int vertex_distance;
	int degree;
	node* parent;
	node* left_child;
	node* sibling;
};

/*
	function to construct a new node for the binomial heap
*/
node* construct_new_node(int vertex_id, int vertex_distance)
{
	node* n = new node;
	n->vertex_id = vertex_id;
	n->vertex_distance = vertex_distance;
	n->degree = 0;
	n->parent = NULL;
	n->left_child = NULL;
	n->sibling = NULL;
	return n;
}

node* root = NULL;
vector<node*> vertex_id_to_node_pointer(10000009);

/*
	function to make tree1 child of tree2 for a binomial heap
*/
void make_tree1_childof_tree2(node* tree1, node* tree2)
{
	tree1->parent = tree2;
	tree1->sibling = tree2->left_child;
	tree2->left_child = tree1;
	tree2->degree += 1;
}

/*
	function to do union of two binaomial heaps.
	it merges two binomials heap such that the final binomial heap has at max one 
	tree for a given degree
*/

node* union_binomial_heap(node* b_heap1, node* b_heap2)
{
	node* b_heap1_curr = b_heap1;
	node* b_heap2_curr = b_heap2;
	node* prev_ptr = NULL;
	node* final_b_heap = NULL;

	if (b_heap1 == NULL)
	{
		return b_heap2;
	}

	else if (b_heap2 == NULL)
	{
		return b_heap1;
	}

	if (b_heap1_curr->degree <= b_heap2_curr->degree)
	{
		prev_ptr = b_heap1_curr;
		b_heap1_curr = b_heap1_curr->sibling;
	}

	else if (b_heap1_curr->degree > b_heap2_curr->degree)
	{
		prev_ptr = b_heap2_curr;
		b_heap2_curr = b_heap2_curr->sibling;
	}

	final_b_heap = prev_ptr;

	while (b_heap1_curr != NULL && b_heap2_curr != NULL)
	{
		if (b_heap1_curr->degree <= b_heap2_curr->degree)
		{
			prev_ptr->sibling = b_heap1_curr;
			b_heap1_curr = b_heap1_curr->sibling;
		}

		else if (b_heap1_curr->degree > b_heap2_curr->degree)
		{
			prev_ptr->sibling = b_heap2_curr;
			b_heap2_curr = b_heap2_curr->sibling;
		}

		prev_ptr = prev_ptr->sibling;
	}

	if (b_heap1_curr != NULL)
	{
		while (b_heap1_curr != NULL)
		{
			prev_ptr->sibling = b_heap1_curr;
			b_heap1_curr = b_heap1_curr->sibling;
			prev_ptr = prev_ptr->sibling;
		}
	}

	if (b_heap2_curr != NULL)
	{
		while (b_heap2_curr != NULL)
		{
			prev_ptr->sibling = b_heap2_curr;
			b_heap2_curr = b_heap2_curr->sibling;
			prev_ptr = prev_ptr->sibling;
		}
	}

	node* curr_ptr = final_b_heap;
	prev_ptr = NULL;
	node* next_ptr = curr_ptr->sibling;

	while (next_ptr != NULL)
	{
		if (curr_ptr->degree != next_ptr->degree)
		{
			prev_ptr = curr_ptr;
			curr_ptr = next_ptr;
			next_ptr = next_ptr->sibling;
		}
		else if (next_ptr->sibling != NULL && curr_ptr->degree == next_ptr->sibling->degree)
		{
			prev_ptr = curr_ptr;
			curr_ptr = next_ptr;
			next_ptr = curr_ptr->sibling;
		}
		else if (curr_ptr->degree == next_ptr->degree)
		{
			if (curr_ptr->vertex_distance <= next_ptr->vertex_distance)
			{
				curr_ptr->sibling = next_ptr->sibling;
				make_tree1_childof_tree2(next_ptr, curr_ptr);
				next_ptr = curr_ptr->sibling;
			}
			else if (curr_ptr->vertex_distance > next_ptr->vertex_distance)
			{
				if (prev_ptr == NULL)
				{
					final_b_heap = next_ptr;
				}
				else
				{
					prev_ptr->sibling = next_ptr;
				}
				make_tree1_childof_tree2(curr_ptr, next_ptr);
				curr_ptr = next_ptr;
				next_ptr = curr_ptr->sibling;
			}
		}
	}
	return final_b_heap;
}


/*
	function to insert a new node in a binomial heap
	a binomial heap with a single node is made which is merged with the original binomial heap
*/
node* binomial_heap_insert(int vertex_id, int vertex_distance)
{
	node* n = construct_new_node(vertex_id, vertex_distance);
	root = union_binomial_heap(root, n);
	return n;
}


/*
	during extract min this function is required to return the children of the node
	to be deleted.
*/
node* reverse_children_list(node* n)
{
	//node* children_binomial_heap = NULL;
	node* root1 = NULL;
	if (n->sibling != NULL)
	{
		reverse_children_list(n->sibling);
		n->sibling->sibling = n;
	}
	else if (n->sibling == NULL)
	{
		root = n;
	}
	root1 = root;
	return root1;
	//return children_binomial_heap;
}

/*
	function to extract minimum node in a binomial heap
	it removes the minimum node from the root list. It merges
	the binomial heap with the children of the node removed.
*/
node* binomial_heap_extract_min(node* heap_root)
{
	if (heap_root == NULL)
	{
		return NULL;
	}

	node* prev_min_distance_node = NULL;
	node* min_distance_node = heap_root;
	int minimum_distance = heap_root->vertex_distance;
	node* root_itr = heap_root;

	while (root_itr->sibling != NULL)
	{
		if (root_itr->sibling->vertex_distance < minimum_distance)
		{
			prev_min_distance_node = root_itr;
			min_distance_node = root_itr->sibling;
			minimum_distance = root_itr->sibling->vertex_distance;
		}
		root_itr = root_itr->sibling;
	}

	if (min_distance_node->sibling == NULL && prev_min_distance_node == NULL)
	{
		heap_root = NULL;
	}
	else if (prev_min_distance_node == NULL)
	{
		heap_root = min_distance_node->sibling;
	}
	else
	{
		prev_min_distance_node->sibling = min_distance_node->sibling;
	}

	//node* children_binomial_heap = NULL;
	node* root1;
	if (min_distance_node->left_child != NULL)
	{
		root1 = reverse_children_list(min_distance_node->left_child);
		min_distance_node->left_child->sibling = NULL;
	}
	else
	{
		root1 = NULL;
	}

	root = union_binomial_heap(heap_root, root1);
	return min_distance_node;
}

/*
	first the key of the node is decreased
	if the key of the node after decresing is less than the key of its parent
	then percolate up
*/
void binomial_heap_decrease_key(node* n, int decreased_value)
{
	if (n == NULL)
	{
		return;
	}
	else
	{
		n->vertex_distance = decreased_value;
		node* p = n->parent;

		while (p != NULL && n->vertex_distance < p->vertex_distance)
		{
			vertex_id_to_node_pointer[n->vertex_id] = p;
			vertex_id_to_node_pointer[p->vertex_id] = n;
			int temp_id = n->vertex_id;
			int temp_dist = n->vertex_distance;
			n->vertex_id = p->vertex_id;
			n->vertex_distance = p->vertex_distance;
			p->vertex_id = temp_id;
			p->vertex_distance = temp_dist;
			n = p;
			p = p->parent;
		}
	}
}

/*================================binomial heap code ends================================*/

int infinite_distance = 999999;
class vertex
{

public:

	int id;
	int distance;
	int parent;
	int start_time;
	int finish_time;
	int color;
	int bellman_ford_distance;

	vertex(int id1)
	{
		id = id1;
		distance = infinite_distance;
		parent = -1; //NULL
		bellman_ford_distance = 0;
	}


};

/*================================binary heap code begins================================*/
/*
	a class to implement the binary heap
*/
class min_heap
{
public:
	int* min_heap_arr;
	int capacity;
	int current_heap_size;
	int* vertexidto_heapindex;

	/*
		constructor to initialize the min heap
	*/
	min_heap(int capa, vector<vertex>& vertex_list)
	{
		min_heap_arr = new int[capa];
		vertexidto_heapindex = new int[capa + 1];
		capacity = capa;
		current_heap_size = capa;

		for (int i = 0; i < capa; i++)
		{
			min_heap_arr[i] = i + 1;
			vertexidto_heapindex[i + 1] = i;
		}
		build_min_heap(vertex_list);
	}
	/*
		functions to return the indices of the parent and left and right children of
		a given element in the binary heap.
	*/
	int parent(int index)
	{
		return (index - 1) / 2;
	}

	int left_child(int index)
	{
		return (2 * index) + 1;
	}

	int right_child(int index)
	{
		return (2 * index) + 2;
	}
	/*
		function to restore the min heap property
	*/
	void min_heapify(int root, vector<vertex>& vertex_list)
	{
		int left_child_index = left_child(root);
		int right_child_index = right_child(root);
		int smallest_index = root;

		if (left_child_index < current_heap_size && vertex_list[min_heap_arr[left_child_index]].distance < vertex_list[min_heap_arr[root]].distance)
		{
			smallest_index = left_child_index;
		}

		if (right_child_index < current_heap_size && vertex_list[min_heap_arr[right_child_index]].distance < vertex_list[min_heap_arr[smallest_index]].distance)
		{
			smallest_index = right_child_index;
		}

		if (smallest_index != root)
		{
			int temp = min_heap_arr[root];

			min_heap_arr[root] = min_heap_arr[smallest_index];
			vertexidto_heapindex[min_heap_arr[smallest_index]] = root;

			min_heap_arr[smallest_index] = temp;
			vertexidto_heapindex[temp] = smallest_index;

			min_heapify(smallest_index, vertex_list);
		}
	}

	/*
		function to build min heap by repeatidly calling min_heapify in 
		bottom up fashion
	*/
	void build_min_heap(vector<vertex>& vertex_list)
	{
		int ind = (current_heap_size / 2) - 1;
		for (int i = ind; i >= 0; i--)
		{
			min_heapify(i, vertex_list);
		}

	}

	/*
		function to decrease the key of a given node and percolate up if necessary
	*/
	void decrease_key(int i, vector<vertex>& vertex_list)
	{
		while (i != 0 && vertex_list[min_heap_arr[parent(i)]].distance > vertex_list[min_heap_arr[i]].distance)
		{
			int temp = min_heap_arr[i];

			min_heap_arr[i] = min_heap_arr[parent(i)];
			vertexidto_heapindex[min_heap_arr[parent(i)]] = i;

			min_heap_arr[parent(i)] = temp;
			vertexidto_heapindex[temp] = parent(i);

			i = parent(i);
		}
	}

	/*
		function to extract min from a binary heap and calls min heapify 
		to restore the min heap property.
	*/
	int extract_min(vector<vertex>& vertex_list)
	{
		if (current_heap_size <= 0)
		{
			return -1;
		}

		if (current_heap_size == 1)
		{
			current_heap_size--;
			return min_heap_arr[0];
		}

		else
		{
			int min_dist_vertex = min_heap_arr[0];
			min_heap_arr[0] = min_heap_arr[current_heap_size - 1];
			vertexidto_heapindex[min_heap_arr[current_heap_size - 1]] = 0;
			current_heap_size--;
			min_heapify(0, vertex_list);
			return min_dist_vertex;
		}
	}
};


/*
	class to maintain the graph given as input by the user.
*/

class graph
{

public:
	int num_vertex;
	int source;
	std::vector<vertex> vertex_list;
	bool negative_weight_cycle_present;
	vector<vector<int> > shortest_paths_adjacency_list;
	vector<vector<int> > shortest_paths_weight_adjacency_list;
	vector<vector<int> > shortest_paths_modified_weight_adjacency_list;

	graph(int num_vertices1, int source1 = 1)
	{
		num_vertex = num_vertices1;
		source = source1;
		negative_weight_cycle_present = false;

		for (int i = 0; i <= num_vertex; i++)		//0th vertex to be ignored. Aadded because of 1 based indexing  
		{
			vertex_list.push_back(vertex(i));
		}

		input_adjacency_matrix();

	}

	void input_adjacency_matrix()
	{
		//int dat = num_vertex + 2;
		//shortest_paths_adjacency_list.assign(dat, {} );
		//shortest_paths_weight_adjacency_list.assign(dat, {} );
		//shortest_paths_modified_weight_adjacency_list.assign(dat, {} );
		for (int i = 0; i <= num_vertex + 2; i++)
		{
			shortest_paths_adjacency_list.push_back(vector<int>());
			shortest_paths_weight_adjacency_list.push_back(vector<int>());
			shortest_paths_modified_weight_adjacency_list.push_back(vector<int>());			
		}
		
		for (int i = 1; i <= num_vertex; i++)
		{
			for (int j = 1; j <= num_vertex; j++)
			{
				int edge_i_j;
				cin >> edge_i_j;

				if (i != j && edge_i_j != infinite_distance)
				{
					shortest_paths_adjacency_list[i].push_back(j);
					shortest_paths_weight_adjacency_list[i].push_back(edge_i_j);
					shortest_paths_modified_weight_adjacency_list[i].push_back(edge_i_j);
				}
			}
		}
	}

	void initialize_single_source(int source)
	{
		for (int i = 0; i < vertex_list.size(); i++)
		{
			vertex_list[i].distance = infinite_distance;
			vertex_list[i].parent = -1;
		}

		vertex_list[source].distance = 0;
	}

	/*
		implements dijkstras algorithm using array based priority queue
	*/
	void dijkstra_array(int current_source)
	{
		initialize_single_source(current_source);
		vector<int> solution_set;

		for (int i = 0; i <= num_vertex; i++)
		{
			solution_set.push_back(0);
		}

		int solution_set_size = 0;

		while (solution_set_size < num_vertex)
		{
			int u;
			int min_dist = infinite_distance + 1;
			for (int i = 1; i <= num_vertex; i++)
			{
				if (solution_set[i] == 0 && vertex_list[i].distance < min_dist)
				{
					u = vertex_list[i].id;
					min_dist = vertex_list[i].distance;
				}
			}
			solution_set[u] = 1;
			solution_set_size++;

			for (int i = 0; i < shortest_paths_adjacency_list[u].size(); i++)
			{
				int v = shortest_paths_adjacency_list[u][i];
				if ((vertex_list[u].distance != infinite_distance) && (vertex_list[v].distance > vertex_list[u].distance + shortest_paths_modified_weight_adjacency_list[u][i]))
				{
					vertex_list[v].distance = vertex_list[u].distance + shortest_paths_modified_weight_adjacency_list[u][i];
					vertex_list[v].parent = u;
				}
			}
		}

		for (int i = 1; i < vertex_list.size(); i++)
		{
			if (vertex_list[i].distance != infinite_distance)
			{
				cout << vertex_list[i].distance - vertex_list[current_source].bellman_ford_distance + vertex_list[i].bellman_ford_distance << " ";
			}

			else
			{
				cout << vertex_list[i].distance << " ";
			}
		}
	}

	/*
		implements dijkstras algorithm using binary heap based priority queue
	*/
	void dijkstra_binary_heap(int current_source)
	{
		initialize_single_source(current_source);
		vector<int> solution_set;

		for (int i = 0; i <= num_vertex; i++)
		{
			solution_set.push_back(0);
		}

		int solution_set_size = 0;

		min_heap pq(num_vertex, vertex_list);

		while (solution_set_size < num_vertex)
		{
			int u;
			u = pq.extract_min(vertex_list);
			solution_set[u] = 1;
			solution_set_size++;

			for (int i = 0; i < shortest_paths_adjacency_list[u].size(); i++)
			{
				int v = shortest_paths_adjacency_list[u][i];
				if ((vertex_list[u].distance != infinite_distance) && (vertex_list[v].distance > vertex_list[u].distance + shortest_paths_modified_weight_adjacency_list[u][i]))
				{

					vertex_list[v].distance = vertex_list[u].distance + shortest_paths_modified_weight_adjacency_list[u][i];
					vertex_list[v].parent = u;
					pq.decrease_key(pq.vertexidto_heapindex[v], vertex_list);
				}
			}
		}

		for (int i = 1; i < vertex_list.size(); i++)
		{
			if (vertex_list[i].distance != infinite_distance)
			{
				cout << vertex_list[i].distance - vertex_list[current_source].bellman_ford_distance + vertex_list[i].bellman_ford_distance << " ";
			}

			else
			{
				cout << vertex_list[i].distance << " ";
			}
		}
	}
	/*
		implements dijkstras algorithm using binomial heap based priority queue
	*/
	void dijkstra_binomial_heap(int current_source)
	{
		root = NULL;
		initialize_single_source(current_source);
		vector<int> solution_set;

		for (int i = 0; i <= num_vertex; i++)
		{
			solution_set.push_back(0);
		}

		int solution_set_size = 0;

		for (int i = 1; i <= num_vertex; i++)
		{
			node* n = binomial_heap_insert(i, vertex_list[i].distance);
			vertex_id_to_node_pointer[i] = n;
		}

		while (solution_set_size < num_vertex)
		{
			int u;
			node* node_u = binomial_heap_extract_min(root);
			u = node_u->vertex_id;
			solution_set[u] = 1;
			solution_set_size++;

			for (int i = 0; i < shortest_paths_adjacency_list[u].size(); i++)
			{
				int v = shortest_paths_adjacency_list[u][i];
				if ((vertex_list[u].distance != infinite_distance) && (vertex_list[v].distance > vertex_list[u].distance + shortest_paths_modified_weight_adjacency_list[u][i]))
				{

					vertex_list[v].distance = vertex_list[u].distance + shortest_paths_modified_weight_adjacency_list[u][i];
					vertex_list[v].parent = u;
					binomial_heap_decrease_key(vertex_id_to_node_pointer[v], vertex_list[v].distance);
				}
			}
		}

		for (int i = 1; i < vertex_list.size(); i++)
		{
			if (vertex_list[i].distance != infinite_distance)
			{
				cout << vertex_list[i].distance - vertex_list[current_source].bellman_ford_distance + vertex_list[i].bellman_ford_distance << " ";
			}

			else
			{
				cout << vertex_list[i].distance << " ";
			}
		}
	}
	/*
		implements dijkstras algorithm using fibonacci heap based priority queue
	*/
	void dijkstra_fib_heap(int current_source)
	{
		fib_min_ptr = NULL;
		fib_num_nodes = 0;
		initialize_single_source(current_source);
		vector<int> solution_set;

		for (int i = 0; i <= num_vertex; i++)
		{
			solution_set.push_back(0);
		}

		int solution_set_size = 0;

		for (int i = 1; i <= num_vertex; i++)
		{
			fib_node* n = fib_insert_node(i, vertex_list[i].distance);
			vertex_id_to_fib_node_pointer[i] = n;
		}

		while (solution_set_size < num_vertex)
		{
			int u;
			fib_node* node_u = fib_extract_min();
			u = node_u->vertex_id;
			solution_set[u] = 1;
			solution_set_size++;

			for (int i = 0; i < shortest_paths_adjacency_list[u].size(); i++)
			{
				int v = shortest_paths_adjacency_list[u][i];
				if ((vertex_list[u].distance != infinite_distance) && (vertex_list[v].distance > vertex_list[u].distance + shortest_paths_modified_weight_adjacency_list[u][i]))
				{

					vertex_list[v].distance = vertex_list[u].distance + shortest_paths_modified_weight_adjacency_list[u][i];
					vertex_list[v].parent = u;
					fib_decrease_key(vertex_id_to_fib_node_pointer[v], vertex_list[v].distance);
				}
			}
		}

		for (int i = 1; i < vertex_list.size(); i++)
		{
			if (vertex_list[i].distance != infinite_distance)
			{
				cout << vertex_list[i].distance - vertex_list[current_source].bellman_ford_distance + vertex_list[i].bellman_ford_distance << " ";
			}
			else
			{
				cout << vertex_list[i].distance << " ";
			}
		}
	}

	void bellman_ford()
	{
		int num_relax_edge_operations = 0;
		int num_relax_edge_modifications = 0;

		initialize_single_source(0);
		for (int i = 1; i <= num_vertex; i++)
		{
			shortest_paths_adjacency_list[0].push_back(i);
			shortest_paths_weight_adjacency_list[0].push_back(0);
		}

		for (int i = 1; i <= (num_vertex + 1) - 1; i++)
		{
			bool any_update_in_this_iteration = false;

			for (int j = 0; j < vertex_list.size(); j++)
			{
				int u = vertex_list[j].id;

				for (int k = 0; k < shortest_paths_adjacency_list[u].size(); k++)
				{
					int v = shortest_paths_adjacency_list[u][k];

					//starting relax edge u,v
					num_relax_edge_operations++;

					if ((vertex_list[u].distance != infinite_distance) && (vertex_list[v].distance > vertex_list[u].distance + shortest_paths_weight_adjacency_list[u][k]))
					{
						num_relax_edge_modifications++;
						any_update_in_this_iteration = true;
						vertex_list[v].distance = vertex_list[u].distance + shortest_paths_weight_adjacency_list[u][k];
						vertex_list[v].parent = u;
					}
					//ending relax edge u,v
				}
			}

			if (any_update_in_this_iteration == false)
			{
				break;
			}
		}
		//bool negative_weight_cycle_present = false;

		for (int j = 0; j < vertex_list.size(); j++)
		{
			int u = vertex_list[j].id;

			for (int k = 0; k < shortest_paths_adjacency_list[u].size(); k++)
			{
				int v = shortest_paths_adjacency_list[u][k];
				if ((vertex_list[u].distance != infinite_distance) && (vertex_list[v].distance > vertex_list[u].distance + shortest_paths_weight_adjacency_list[u][k]))
				{
					negative_weight_cycle_present = true;
					break;
				}
			}

			if (negative_weight_cycle_present == true)
			{
				break;
			}
		}

		if (negative_weight_cycle_present == true)
		{
			cout << -1 << endl;
		}

		else if (negative_weight_cycle_present == false)
		{
			for (int i = 1; i < vertex_list.size(); i++)
			{
				vertex_list[i].bellman_ford_distance = vertex_list[i].distance;

			}

			for (int i = 1; i <= num_vertex; i++)
			{
				for (int j = 0; j < shortest_paths_adjacency_list[i].size(); j++)
				{
					int u = i;
					int v = shortest_paths_adjacency_list[i][j];
					shortest_paths_modified_weight_adjacency_list[i][j] += vertex_list[u].distance - vertex_list[v].distance;
				}
			}

			//cout << num_relax_edge_operations << " " << num_relax_edge_modifications;
		}
	}



	void johnson(int directed, char priority_queue_type)
	{
		if (directed == 0)
		{
			for (int i = 1; i <= num_vertex; i++)
			{
				for (int j = 0; j < shortest_paths_weight_adjacency_list[i].size(); j++)
				{
					if (shortest_paths_weight_adjacency_list[i][j] < 0)
					{
						cout << -1;
						cout << "\n";
						return;
					}
				}
			}


			for (int i = 1; i <= num_vertex; i++)
			{
				if (priority_queue_type == '1')
				{
					dijkstra_array(i);
				}
				else if (priority_queue_type == '2')
				{
					dijkstra_binary_heap(i);
				}
				else if (priority_queue_type == '3')
				{
					dijkstra_binomial_heap(i);
				}
				else
				{
					dijkstra_fib_heap(i);
				}
				cout << "\n";
			}

		}

		else if (directed == 1)
		{
			bellman_ford();

			if (negative_weight_cycle_present == false)
			{
				for (int i = 1; i <= num_vertex; i++)
				{
					if (priority_queue_type == '1')
					{
						dijkstra_array(i);
					}
					else if (priority_queue_type == '2')
					{
						dijkstra_binary_heap(i);
					}
					else if (priority_queue_type == '3')
					{
						dijkstra_binomial_heap(i);
					}
					else
					{
						dijkstra_fib_heap(i);
					}
					cout << "\n";
				}
			}
		}
	}
};

signed main(int argc, char** argv)
{
	/*ios_base::sync_with_stdio(false);
	cin.tie(NULL);
	cout.tie(0);

#ifndef ONLINE_JUDGE 
	freopen("sirinput.txt", "r", stdin);
	freopen("error.txt", "w", stderr);
	freopen("output.txt", "w", stdout);
#endif*/

	int t = 0;
	int test_cases;
	cin >> test_cases;
	vector<double> time_log;
	while (t++ < test_cases)
	{
		int n, d;
		cin >> n >> d;
		graph g(n);

		clock_t begin = clock();
		if (argc == 1)
		{
			g.johnson(d, 4);
			
		}
		else
		{
			g.johnson(d, argv[1][0]);
		}
		
		clock_t end = clock();
		double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
		time_log.push_back(time_spent);
		//cout << "\n";
	}
	for (int i = 0; i < time_log.size(); i++)
	{
		cout << time_log[i] << " ";
	}

	return 0;
}
