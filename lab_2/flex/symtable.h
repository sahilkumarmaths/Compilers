/*****************************************************************
Author:
	Gunamgari Sharath Reddy        
	P V S Dileep               	   
	Sahil Kumar Goyal  
Date:
	5/3/2013
Purpose: 
	This file defines the data structure for symbol table
    Symbol Table is implemented as singly linked list.
*****************************************************************/


 


#include <stdio.h>
#include <string.h>
#include <stdlib.h>		    // For atof() and atoi()

typedef struct node 			// Node for linked list
{
   	struct node *next;		// Pointer to next node
	char  token[256];		// Array to store the lexeme
} node;

node *head;				// Head of Symbol Table
node *curr, *temp;

/* This function takes a character string as arguement. If the string is not
   present in the linked list then it inserts the srting at the last and returns
   the position of the node;
 */

int install_token(char *str)
{
	int i = 0;

	temp = (node *)malloc(sizeof(node));	// Allocate memory for new node
        strcpy(temp->token,str);		// Copy the string in the node
        temp->next = NULL;

        if (head == NULL)			// First lexeme
        {
                head = temp;
                return 0;
        }
        else
        {
                curr = head;
                while(curr->next!=NULL)		// Search for the string
                {
                        if(strcmp(curr->token, temp->token)==0)
                                return i;
                        else
                        {
                                i++;
                                curr=curr->next;
                        }
                }
                if(strcmp(curr->token, temp->token)==0)	// String found
                        return i;

                curr->next = temp;			// String not found
                return i+1;

        }
}
/* This function is to print the symbol table comtents
 */

void showTable()
{
	int i = 0;
	curr = head;
        while(curr!=NULL)
        {
		printf(" %4d\t%s\n", i, curr->token);
		i++;
		curr=curr->next;
	}
	//getch();
}

void install_id(){
	int type;
    {
        type = install_token(yytext);	// insert in symbol table
        printf("<ID , %d> 	",type);
    }
}

