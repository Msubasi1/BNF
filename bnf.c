#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h> 


struct zero_child{ //OP PRE_OP SET_OP REL_OP VAR 
	char *zero_content;
};

struct one_child{ //op pre_op set_op rel_op var

	struct zero_child child;
};

struct two_child{ //cond

	struct one_child two_op;
	struct two_child *left;
	struct two_child *right;
};

struct three_child{ //expr

	struct one_child three_op;
	struct three_child *left;
	struct three_child *right;
	struct three_child *middle;
};

struct node { //for linked list implementation
	int data; 
	struct zero_child zero; 	//data =0
	struct one_child one;		//data =1
	struct two_child two;		//data =2
	struct three_child three;	//data =3
			
	struct node *next;
	struct node *left;
	struct node *right;
};

int randomGenerator(int max,int min){ //random number generator
	int r = ( rand() % max-min+1 ) + min;
	return r;
}

char* randomLine(FILE *fptr){		//gives a random line in file
	
	rewind(fptr); //go to the begining of file
	char *line = NULL;
    	size_t len = 0;
    	ssize_t line_len;	

	int upper=0;
	while ((line_len = getline(&line, &len, fptr)) != -1){
		upper++;
	}
	
	//random number generator
	int r=randomGenerator(upper,1);
		
	rewind(fptr); //go to the beginning of file	
	char *line_r = NULL;
    	size_t len_r = 0;
    	ssize_t line_len_r;
 
    	int i=1;
    	while ((line_len_r= getline(&line_r, &len_r, fptr)) != -1){
		if(i==r){
			break;
		}
		else{
			i++;
		}
	}
	char *pos;
	if ((pos=strchr(line_r, '\n')) != NULL)
    		*pos = '\0';
	return line_r;
}


void insertion(struct node *n,struct node **root){
	//insert tree
	if((*root)==NULL){
		(*root)=n;
		return ;
	}
	
	struct node *l_curr=(struct node *)malloc(sizeof(struct node));
	l_curr=*root;
	
	struct node *r_curr=(struct node *)malloc(sizeof(struct node));
	r_curr=*root;
	while(l_curr->left!=NULL && r_curr->right!=NULL ){
		l_curr=l_curr->left;
		r_curr=r_curr->right;
	}
	if(l_curr->left==NULL){
		insertion(n,&(l_curr->left));
	}
	else{
		insertion(n,&(r_curr->right));
	}
}

void printPreorder(struct node* node) { 
	if (node == NULL){
		return; 
	}
    
   	if(node->data==-1){
		printPreorder(node->next);
	}
	else{
		if(node->data==2 ||node->data==3){
			printf("(");
		}
		else if(node->data==1){
			printf("%s",node->one.child.zero_content);
		}
		else if(node->data==0){
			printf("%s))",node->one.child.zero_content);
		}
		printPreorder(node->left);   
		printPreorder(node->right);
	}
}


void append(struct node *n,struct node **root){	//root->cond
	struct node *last=(struct node *)malloc(sizeof(struct node));
	last=*root;
	while(last->next!=NULL){
		last=last->next;
	}
	last->next=n;
	n->next=NULL;
	last=*root;
	while(last!=NULL){
		last=last->next;
	}
}


void createExpr(FILE *f_op,FILE *f_var,FILE *f_pre,struct node **root);

void execution(FILE *f_set,FILE *f_rel,FILE *f_op,FILE *f_var,FILE *f_pre,struct node **root){
	//create <cond> field
	//root->cond
	if((*root)->data==-1){
		//first execution field
		struct two_child cond;
		struct node *firstCond=(struct node *)malloc(sizeof(struct node));
		firstCond->two=cond;
		firstCond->data=2;
		*root=firstCond;
		append(firstCond,root);
		execution(f_set,f_rel,f_op,f_var,f_pre,&firstCond);		//recursion
		return;
	}
	
	int random=randomGenerator(2,1);	
	if(random==1){
		//<cond><set_op><cond>
		
		//<cond>
		struct node *first=(struct node *)malloc(sizeof(struct node));
		first->data=2;
		insertion(first,root);
		append(first,root);
		execution(f_set,f_rel,f_op,f_var,f_pre,&first);
			
		//<set_op>
		struct node *second=(struct node *)malloc(sizeof(struct node));
		second->data=1;
		struct one_child middle;
		struct zero_child operation;
		operation.zero_content=randomLine(f_set);
		middle.child=operation;
		second->one=middle;
		insertion(second,root);
		append(second,root);
				
		//<cond>
		struct node *third=(struct node *)malloc(sizeof(struct node));
		third->data=2;
		insertion(third,root);
		append(third,root);
		execution(f_set,f_rel,f_op,f_var,f_pre,&third);		//recursion
		return;				
	}
	else if(random==2){
		//<expr><rel_op><expr>

		//<expr>
		struct node *first=(struct node *)malloc(sizeof(struct node));
		first->data=3;
		insertion(first,root);
		append(first,root);
		createExpr(f_op,f_var,f_pre,&first);

		//<rel_op>
		struct node *second=(struct node *)malloc(sizeof(struct node));
		second->data=1;
		struct one_child middle;
		struct zero_child operation;
		operation.zero_content=randomLine(f_rel);
		middle.child=operation;
		second->one=middle;
		insertion(second,root);
		append(second,root);

		//<expr>
		struct node *third=(struct node *)malloc(sizeof(struct node));
		third->data=3;
		insertion(third,root);
		append(third,root);
		createExpr(f_op,f_var,f_pre,&third);
		return;	
	}		
}


void createExpr(FILE *f_op,FILE *f_var,FILE *f_pre,struct node **root){
	//create <expr> field
	//random number generator
	int random=randomGenerator(3,1);
	if(random==1){
		//<expr><rel_op><expr>

		//<expr>
		struct node *first=(struct node *)malloc(sizeof(struct node));
		first->data=3;
		insertion(first,root);
		append(first,root);
		createExpr(f_op,f_var,f_pre,&first);		//recursion

		//<op>
		struct node *second=(struct node *)malloc(sizeof(struct node));
		second->data=1;
		struct one_child middle;
		struct zero_child operation;
		operation.zero_content=randomLine(f_op);
		middle.child=operation;
		second->one=middle;
		insertion(second,root);
		append(second,root);

		//<expr>
		struct node *third=(struct node *)malloc(sizeof(struct node));
		third->data=3;
		insertion(third,root);
		append(third,root);
		createExpr(f_op,f_var,f_pre,&third);		//recursion
		return;
	
	}
	else if(random==2){
		//<pre-op><expr>
		
		//pre_op>
		struct node *value=(struct node *)malloc(sizeof(struct node));
		value->data=1;
		struct one_child middle;
		struct zero_child operation;
		operation.zero_content=randomLine(f_pre);
		middle.child=operation;
		value->one=middle;
		insertion(value,root);
		append(value,root);
		
		//<expr>
		struct node *first=(struct node *)malloc(sizeof(struct node));
		first->data=3;
		insertion(first,root);
		append(first,root);
		createExpr(f_op,f_var,f_pre,&first);		//Recursion
		return;
	}
	else if(random==3){
		//<var>
		struct node *value=(struct node *)malloc(sizeof(struct node));
		value->data=0;
		struct one_child middle;
		struct zero_child operation;
		operation.zero_content=randomLine(f_var);
		middle.child=operation;
		value->one=middle;
		insertion(value,root);
		append(value,root);
		return;
	}
}


void main(int argc, char *argv[]){

	FILE *f_rel = fopen("rel_op","r");
	FILE *f_pre = fopen("pre_op","r");
	FILE *f_set = fopen("set_op","r");
	FILE *f_op = fopen("op","r");
	FILE *f_var = fopen("var","r");

	srand(time(0));
	
	struct node *root=(struct node*)malloc(sizeof(struct node));
	root->data=-1;
	execution(f_set,f_rel,f_op,f_var,f_pre,&root);
	printf("if (");
	printPreorder(root);
	printf(")){}\n");  

}
