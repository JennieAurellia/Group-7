#include<stdio.h>
#include<stdlib.h>

#define MIN_DEGREE 2

typedef struct BTreeNode{
	
    int keys[2 * MIN_DEGREE - 1];
    struct BTreeNode *children[2 * MIN_DEGREE];
    int n; // jmlh keys nya
    int leaf; // utk menandai apkh dia leaf atau bukan
}BTreeNode;

BTreeNode* createNode(int leaf){
	
    BTreeNode *node = (BTreeNode*)malloc(sizeof(BTreeNode));
    node->leaf = leaf;
    node->n = 0;
    for(int i = 0; i < 2 * MIN_DEGREE; i++){
        node->children[i] = NULL;
    }
    return node;
}

void traverse(BTreeNode *root){
	
    if(root != NULL){
    	int i;
        for(i = 0; i < root->n; i++){
            if(!root->leaf) traverse(root->children[i]);
            printf("%d ", root->keys[i]);
        }
        
        if(!root->leaf) traverse(root->children[i]);
    }
}

BTreeNode* search(BTreeNode *root, int k){
	
    int i = 0;
    while(i < root->n && k > root->keys[i]){
        i++;
	}

    if(i < root->n && root->keys[i] == k) return root;

    if(root->leaf) return NULL;

    return search(root->children[i], k);
}

void splitChild(BTreeNode *x, int i, BTreeNode *y){
	
    BTreeNode *z = createNode(y->leaf);
    z->n = MIN_DEGREE - 1;

    for(int j = 0; j < MIN_DEGREE - 1; j++){
        z->keys[j] = y->keys[j + MIN_DEGREE];
	}

    if(!y->leaf){
        for (int j = 0; j < MIN_DEGREE; j++){
            z->children[j] = y->children[j + MIN_DEGREE];
		}
    }

    y->n = MIN_DEGREE - 1;

    for(int j = x->n; j >= i + 1; j--){
        x->children[j + 1] = x->children[j];
	}

    x->children[i + 1] = z;

    for(int j = x->n - 1; j >= i; j--){
        x->keys[j + 1] = x->keys[j];
	}

    x->keys[i] = y->keys[MIN_DEGREE - 1];
    x->n++;
}

void insertNonFull(BTreeNode *x, int k){
	
    int i = x->n - 1;

    if(x->leaf){
        while(i >= 0 && x->keys[i] > k){
            x->keys[i + 1] = x->keys[i];
            i--;
        }
        x->keys[i + 1] = k;
        x->n++;
    } 
	else{
        while(i >= 0 && x->keys[i] > k)
            i--;

        if(x->children[i + 1]->n == 2 * MIN_DEGREE - 1){
            splitChild(x, i + 1, x->children[i + 1]);

            if (k > x->keys[i + 1])
                i++;
        }
        insertNonFull(x->children[i + 1], k);
    }
}

void insert(BTreeNode **root, int k){
	
    BTreeNode *r = *root;
    if(r->n == 2 * MIN_DEGREE - 1){
        BTreeNode *s = createNode(0);
        *root = s;
        s->children[0] = r;
        splitChild(s, 0, r);
        insertNonFull(s, k);
    } 
	else{
        insertNonFull(r, k);
    }
}

int findKey(BTreeNode *x, int k){
	
    int idx = 0;
    while(idx < x->n && x->keys[idx] < k)
        ++idx;
    return idx;
}

void removeKey(BTreeNode *x, int k);

void removeFromLeaf(BTreeNode *x, int idx){
	
    for(int i = idx + 1; i < x->n; ++i){
        x->keys[i - 1] = x->keys[i];
	}
    x->n--;
}

void removeFromNonLeaf(BTreeNode *x, int idx){
	
    int k = x->keys[idx];

    if(x->children[idx]->n >= MIN_DEGREE){
    	
        BTreeNode *cur = x->children[idx];
        while (!cur->leaf) cur = cur->children[cur->n];
        
        int pred = cur->keys[cur->n - 1];
        x->keys[idx] = pred;
        removeKey(x->children[idx], pred);
    }
	else if(x->children[idx + 1]->n >= MIN_DEGREE){
		
        BTreeNode *cur = x->children[idx + 1];
        while (!cur->leaf) cur = cur->children[0];
        
        int succ = cur->keys[0];
        x->keys[idx] = succ;
        removeKey(x->children[idx + 1], succ);
    }
	else{
        BTreeNode *child = x->children[idx];
        BTreeNode *sibling = x->children[idx + 1];

        child->keys[MIN_DEGREE - 1] = k;

        for(int i = 0; i < sibling->n; ++i){
            child->keys[i + MIN_DEGREE] = sibling->keys[i];
		}

        if(!child->leaf){
            for(int i = 0; i <= sibling->n; ++i){
                child->children[i + MIN_DEGREE] = sibling->children[i];
			}
        }

        for(int i = idx + 1; i < x->n; ++i)
            x->keys[i - 1] = x->keys[i];

        for(int i = idx + 2; i <= x->n; ++i)
            x->children[i - 1] = x->children[i];

        child->n += sibling->n + 1;
        x->n--;

        free(sibling);

        removeKey(child, k);
    }
}

void removeKey(BTreeNode *x, int k){
	
    int idx = findKey(x, k);

    if(idx < x->n && x->keys[idx] == k){
        if(x->leaf)
            removeFromLeaf(x, idx);
        else
            removeFromNonLeaf(x, idx);
    }
	else{
        if(x->leaf){
            printf("The key %d does'nt exist in the tree\n", k);
            return;
        }

        int flag = ((idx == x->n) ? 1 : 0);

        if(x->children[idx]->n < MIN_DEGREE)
        {
            if(idx != 0 && x->children[idx - 1]->n >= MIN_DEGREE)
            {
                BTreeNode *child = x->children[idx];
                BTreeNode *sibling = x->children[idx - 1];

                for(int i = child->n - 1; i >= 0; --i)
                    child->keys[i + 1] = child->keys[i];

                if(!child->leaf){
                    for(int i = child->n; i >= 0; --i)
                        child->children[i + 1] = child->children[i];
                }

                child->keys[0] = x->keys[idx - 1];

                if(!child->leaf)
                    child->children[0] = sibling->children[sibling->n];

                x->keys[idx - 1] = sibling->keys[sibling->n - 1];

                child->n += 1;
                sibling->n -= 1;
            }
            else if(idx != x->n && x->children[idx + 1]->n >= MIN_DEGREE)
            {
                BTreeNode *child = x->children[idx];
                BTreeNode *sibling = x->children[idx + 1];

                child->keys[(child->n)] = x->keys[idx];

                if(!(child->leaf))
                    child->children[(child->n) + 1] = sibling->children[0];

                x->keys[idx] = sibling->keys[0];

                for(int i = 1; i < sibling->n; ++i)
                    sibling->keys[i - 1] = sibling->keys[i];

                if(!sibling->leaf) {
                    for(int i = 1; i <= sibling->n; ++i)
                        sibling->children[i - 1] = sibling->children[i];
                }

                child->n += 1;
                sibling->n -= 1;
            }
            else
            {
                if(idx != x->n)
                    removeFromNonLeaf(x, idx);
                else
                    removeFromNonLeaf(x, idx - 1);
            }
        }

        if(flag && idx > x->n)
            removeKey(x->children[idx - 1], k);
        else
            removeKey(x->children[idx], k);
    }
}

void deleteKey(BTreeNode **root, int k){
	
    if(!*root){
        printf("The tree is empty\n");
        return;
    }

    removeKey(*root, k);

    if((*root)->n == 0){
        BTreeNode *tmp = *root;
        
        if((*root)->leaf)
            *root = NULL;
        else
            *root = (*root)->children[0];

        free(tmp);
    }
}

int main() {
    BTreeNode *root = createNode(1);

    insert(&root, 10);
    insert(&root, 20);
    insert(&root, 5);
    insert(&root, 6);
    insert(&root, 12);
    insert(&root, 30);
    insert(&root, 7);
    insert(&root, 17);

    printf("Traversal of tree constructed is\n");
    traverse(root);
    printf("\n\n");

    deleteKey(&root, 6);
    printf("Traversal after deletion of 6\n");
    traverse(root);
    printf("\n\n");

    deleteKey(&root, 13);
    printf("Traversal after deletion of 13 (non-existent)\n");
    traverse(root);
    printf("\n\n");

    deleteKey(&root, 7);
    printf("Traversal after deletion of 7\n");
    traverse(root);
    printf("\n\n");

    deleteKey(&root, 4);
    printf("Traversal after deletion of 4 (non-existent)\n");
    traverse(root);
    printf("\n\n");

    deleteKey(&root, 20);
    printf("Traversal after deletion of 20\n");
    traverse(root);
    printf("\n\n");

    deleteKey(&root, 16);
    printf("Traversal after deletion of 16 (non-existent)\n");
    traverse(root);
    printf("\n\n");

    return 0;
}
