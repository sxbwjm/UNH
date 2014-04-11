/**************************************************************
 *                  Programming Assessment                    *
 *                                                            *
 * Author:  Xiaobo Sun                                        *
 *                                                            *
 * Purpose:  Programming Assessment of UNH                    *
 *                                                            *
 * Usage:                                                     *
 *      Find the twenty most frequently used words            *
 *      across all text files specified by the parameters.    *
 **************************************************************/

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/********************************************************************
*                     Declaration of Constants
*********************************************************************/
//Return codes of function
#define NORMAL_END 0
#define ABNORMAL_END 9
#define TRUE 1
#define FALSE 0

//Word length
#define WORD_MIN_LEN 6
#define WORD_MAX_LEN 50

//The number of the most frequently used words to output
#define WORD_MOST_USED_NUM 20

//Use binary tree to store words found in the text files.
//I choose binary tree instead of hash table for two reasons:
//  1. the pattern of words in the text files is uncertain.
//  2. the total number of words is unknown.
struct BTREE_NODE
{
	char *word;
	int count;
	int file_count;               //to count the number of files in which the word is used.
	struct BTREE_NODE *left;
	struct BTREE_NODE *right;
};

typedef struct BTREE_NODE BTREE_NODE;

/********************************************************************
*                    Declaration of Functions 
* See details for each function in their definition.
*********************************************************************/
int CheckParam(int argc,char *argv[]);
int isLetter(char c);
void wordToLower(char *word);
int getWord(FILE *fp, char *word);
BTREE_NODE *createNode (char *str);
BTREE_NODE *addNode (BTREE_NODE *node, char *str,int fileNo);
void outputTopWords(BTREE_NODE *node, int fileNo);
void outputBtree(BTREE_NODE *node, int fileNo);

/********************************************************************
*                    Main Function
* Parameters:
*   1 - argc  The number of parameters
*   2 - argv[] The value(array) of parameters
*********************************************************************/
int main(int argc,char *argv[])
{
	FILE *fp;
	int word_len;
	char word[WORD_MAX_LEN + 1];
    BTREE_NODE *head = NULL;

	//Check the parameters.
	if(CheckParam(argc,argv) == ABNORMAL_END)
	{	
		return ABNORMAL_END;
	}

	//Read all of the text files specified by the parameters.
	int i;
	for(i=1;i<argc;i++)
	{
		//Open the text file.
		fp = fopen(argv[i],"r");
		
		//Read words until no more word can be found in the file, and store them to the binary tree.
		while( (word_len = getWord(fp,word)) > 0)
		{
			head = addNode(head,word,i);
		}
		
		//Close the text file.
		fclose(fp);

	}
	
	//Find the most frequently used words from the binary tree and output them to screen.
	outputTopWords(head,argc-1);

}

/********************************************************************
*Check the parameters: 
*  1. the number of parmeter(at least one file name is required)
*  2. the existence of files
*********************************************************************/
int CheckParam(int argc,char *argv[])
{
	FILE *fp;

	//Check the parameters number
	if(argc < 2)
	{
		printf("Please specify at least one file name.\n");
		return ABNORMAL_END;
	}
	
	//Check the existence of files
	int i;
	for(i=1;i<argc;i++)
	{
		fp = fopen(argv[i],"r");
		if(fp == NULL)
		{
			printf("Cannot open the following file: %s\n",argv[i]);
			return ABNORMAL_END;
		}
		else
		{
			fclose(fp);
		}
	}
	return NORMAL_END;
}

/********************************************************************
*Check if a character is a letter(a-z A-Z): 
*Return TRUE if the character is  a letter
*********************************************************************/
int isLetter(char c)
{
	if((c >= 65 && c <= 90)||(c >= 97 && c<= 122))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/********************************************************************
*Convert all the letters in a word to lowercase
*Parameter:
* 1. word - the word that is to be converted
*********************************************************************/
void wordToLower(char *word)
{
	while ( *word != '\0' )
	{
		*word = tolower(*word);
		word++;
	}
}

/********************************************************************
*Read a word from a text file.
* 1. Ingnore the Non-Letter characters.
* 2. Ignore the word that is either less than WORD_MIN_LEN or 
*    longer than WORD_MAX_LEN in length
*Parameters:
* 1. fp - pointer to the file to be read
* 2. word - to store the word
*Return :
* the length of the word (zero when no word is found)
*********************************************************************/
int getWord(FILE *fp, char *word)
{
	char c;
	int word_len = 0;
	
	//Read characters until a word is found or the file is end.
    while( (c = fgetc(fp)) != EOF )
	{
        if ( isLetter(c) == TRUE )
		{
			//Append the character to the word if the current length of the word is no more than WORD_MAX_LEN
			if (word_len < WORD_MAX_LEN )
			{
				word[word_len] = c;
			}
			
			word_len++;
        }
		//Ignore the word that is either less than WORD_MIN_LEN or longer than WORD_MAX_LEN in length
        else if ( (word_len > WORD_MAX_LEN) || (word_len < WORD_MIN_LEN) )
		{
			word_len = 0;
            continue;
        }
		else
		{
			break;
		}
    }
    word[word_len] = '\0';
    //Convert the word to lowercase.
	wordToLower(word);
	
    return word_len;
}

/********************************************************************
*Create a node for binary tree
*
* Parameters:
*   1. str -  the word to store
* Return: 
*   the pointer to the node
*********************************************************************/
BTREE_NODE *createNode(char *str)
{
	BTREE_NODE *node;

	//Allocate neccessary memory for a node.
	size_t len = strlen(str);
	node = (BTREE_NODE *)malloc(sizeof(BTREE_NODE));

	if ( node == NULL)
	{
		return NULL;
	}

	//Initiate the rest fields
	node -> count = 1;
	node -> file_count = 1;
	node -> right = NULL;
	node->left = NULL;
	node->word = (char *)malloc(len+1);

	if ( node->word == NULL)
	{
		return NULL;
	}
	//Copy the word to the node
	strcpy(node->word,str);

	return node;
}

/********************************************************************
*Add a word to the binary tree according to the following rules:
* a) If the word is same to the "word" field of a node, and the "file_count" 
*    field of the node is bigger than fileNo(the sequence number of text file) - 1
*    (which means the word have been used in all the privous files), 
*    Just count up the "count" field of that node.
* b) If the word is bigger(alphabetically) than the "word" field of a node, 
*    coutinue searching in the node's right branch by calling this function itself(recursion)
* c) If the word is smaller(alphabetically) than the "word" field of a node,
*    coutinue searching in the node's left branch by calling this function itself(recursion)
* d) If no node whose "word" field is same to the word can be found, add a new node when the text file is 
*    the first file, otherwise ingore the word.
* Parameters:
*   1.node - binary tree node
*   2.str - word to store
*   3.fileNo - the sequence number of text file
* Return value: 
*   the pointer to the node
*********************************************************************/
BTREE_NODE *addNode(BTREE_NODE *node, char *str, int fileNo )
{
	int cmpResult;
	//When the word cannot be found in the binary tree
	if(node == NULL)
	{
		//Add new BTREE_NODE only when reading the first file
		if(fileNo == 1)
		{
			node = createNode(str);
		}		
	}
	else
	{
		//Compare the word alphabetically with the "word" field of the node.
		cmpResult = strcmp(str, node->word);
		//If the word is same to "word" field of the node.
		if(cmpResult == 0)
		{
			//Count the word only when it have been found in all previous files
			if( node->file_count >= fileNo -1 )
			{
				node->count++;
				node->file_count = fileNo;
			}
		
		}
		//If the word if bigger than the "word" field of the node, coutinue searching in its right branch.
		else if(cmpResult > 0)
		{
			node->right = addNode(node->right,str, fileNo);
		}
		//If the word if smaller than the "word" field of the node, coutinue searching in its left branch.
		else
		{
			node->left = addNode(node->left,str,fileNo);
		}
	}
	
	return node;
}

/********************************************************************
*Traverse a binary tree to get the most frequently used word according
*to the following rules:
* a) the "count" field is the biggest in the binary tree.
* b) the "file_count" field is equal to the number of the text files.

* Parameters:
*   1.node - the binary tree node
*   2.mostUsedWordNode - the pointer to the node that contain the most frequently used word
*   3.fileNum - the number of text files
* Return value: 
*   the pointer to the node that contain the most frequently used word
*********************************************************************/
BTREE_NODE *getMostUsedWord(BTREE_NODE *node, BTREE_NODE *mostUsedWordNode, int fileNum)
{
	//If no more node to traverse, keep the orginal "mostUsedWordNode" as
	//the node of the most frenquently used word
	if(node == NULL)
	{
		return mostUsedWordNode;
	}
	//If the "file_count" field is equal to the number of text files,
	if(node->file_count == fileNum)
	{
		//And the "count" field of the node is bigger than that of the current "mostUsedWordNode",
		//set the node as "mostUsedWordNode"
		if( (mostUsedWordNode == NULL) || (node->count > mostUsedWordNode->count) )
		{
			mostUsedWordNode = node;
		}
	}
	
	//recursion
	mostUsedWordNode = getMostUsedWord(node->left, mostUsedWordNode, fileNum);
    mostUsedWordNode = getMostUsedWord(node->right, mostUsedWordNode, fileNum);

	return mostUsedWordNode;
}

/********************************************************************
*Output the WORD_MOST_USED_NUM most frequently used words by calling 
*the function "getMostUsedWord" repeatedly
* - After calling "getMostUsedWord" function, set the "file_count" field of the node,
* - so that the next frequently used word can be found next time
*
* Parameters:
*   1.head - the head node of a binary tree
*   2.fileNum - the number of text files
* Return value: 
*    none
*********************************************************************/
void outputTopWords(BTREE_NODE *head, int fileNum)
{
	int i = 0;
	int previousCount = 0;

	if(head == NULL)
	{
		return;
	}
	
	BTREE_NODE *mostUsedWordNode;
	while(1)
	{
		mostUsedWordNode = getMostUsedWord(head,NULL,fileNum);
		//Break when no more word can be found
		if(mostUsedWordNode == NULL)
		{
			break;
		}
		//Break when the word is not the tie of the previous word and more than WORD_MOST_USED_NUM words have been found 
		if( (mostUsedWordNode->count != previousCount) && (i >= WORD_MOST_USED_NUM) )
		{
			break;
		}
        //Output the word
		printf("%s\n", mostUsedWordNode->word);

		previousCount = mostUsedWordNode->count;
		//Set the "file_count" field of the node so that the next frequently used word can be found next time
		mostUsedWordNode->file_count = 0;
		i++;
	}
}
