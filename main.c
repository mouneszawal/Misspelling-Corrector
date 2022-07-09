#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>
#define FNSIZE 50
#define R 31
#define M 997
#define MAX_DIF 2

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })
     


/* 
	the dictionary hashtable and the wrongWords hashtable structures are  
	very similar to each other, so instead making another structure for 
	wrong word table i will make an instance of the dictionary table,
	when the table is dictionary instance the suggestion part will be empty
 */
typedef struct{
	char *kelime;
	char *suggestion;
}word;

typedef struct{
	word * words;
	int tableSize;
}hashTable;


/* PROTOTYPES and declerations: the explanation is under the main function:*/
unsigned long long int horner(const char* str);
int h1(unsigned long long int key);
int h2(unsigned long long int key);
int hash(unsigned long long int key,int i);
char * upper(char *str);
hashTable* init_table(hashTable * );
word * createWord();
void print(hashTable *);
void insertToTable(hashTable * ,char* ,char* );
int searchTable(hashTable * ,char* wrd);
int searchForUserWord(hashTable * ,char* wrd);
int length(char*);
void searchEveryWord(hashTable *,hashTable * ,char*);
void checkdifference(hashTable* ,hashTable* wrongWord, char* wrd,char* sentence);
int updateTable(hashTable * ,char * filename);
int levinshtein(char* str1,char* str2, int );

int main(){
	char userSentence[200]={0},takenWord1[100],takenWord2[100];int option;
	hashTable * table = NULL;
	hashTable * wrongWord = NULL;
	table =	init_table(table);
	wrongWord = init_table(wrongWord);	//initialization
	updateTable(table,"smalldictionary.txt");
//	print(table);
	while(true){
        printf("(1) --> Tell us something.\n(2) --> Check bonus question.\n(3) --> Print Dictionary.\n(4) --> Print wrongWord.\n(5) --> Clear screen.\n(999) --> Exit.\nPlease Enter an option :");
        scanf("%d",&option);
        
        switch(option){
            case 1 :
				printf("What are you thinking ? ");
				scanf("\n%[^\n]s",userSentence);
				searchEveryWord(table, wrongWord, upper(userSentence));
				if(strcmp(userSentence,""))
					printf("So you are thinking of: %s\n",userSentence);
                puts("Press any key to continue the program:");
                getch();
                system("CLS");
                break;
            case 2 :
            	printf("\t\t<< when the distance is bigger than MAX_DIFFERENCE\n\t\t   and the algorithm is not finished then the matrix of dist.\n\t\t   should not be displayed.\n\t\t   When the matrix is not in the screen:\n\t\t   The number shown indicates that the algorithm stoped running\n\t\t   before it is actually finished >>\n\n");
                printf("Enter Word #1: ");
                scanf("\n%s",takenWord1);
                printf("Enter Word #2: ");
                scanf("\n%s",takenWord2);       	
                printf("\n%d\n",levinshtein(upper(takenWord1) ,upper(takenWord2) ,option));
                puts("Press any key to continue the program:");
                getch();
                system("CLS");
                break;
            case 3 :
                print(table);
                puts("Press any key to continue the program:");
                getch();
                system("CLS");
                break;
            case 4 :
      		    print(wrongWord);
                puts("Press any key to continue the program:");
                getch();
                system("CLS");
                break;        
            case 5 :
                system("CLS");
                break;
            case 999 :
            	system("CLS");
                return 0;
            default :
                printf("Please Enter a valid option..\nPress Any key to continue...\n");
                getch();
                break;
        }
        printf("_______________________________\n");
	}
}

int h1(unsigned long long int key) {
   return key % M;
}
int h2(unsigned long long int key) {
   return 1 + (key % M-1);
}
int hash(unsigned long long int key, int i) {
   return (h1(key) + i * h2(key)) % M;
}
unsigned long long int horner(const char* str){
	unsigned long long int key=0;
	int i=0;
	
	while(str[i]!='\0'){
		key+=(str[i])*pow(R,strlen(str)-i-1);
		i++;
	}
	return key;
}
char * upper(char *str){
	int i=0;
	while(str[i]!='\0'){
		if(str[i]>96&&str[i]<123)
			str[i]=str[i]-32;	
		i++;
	}
	return str;
}

//have I filled the entire table?
// if yes return 1
bool isFull(hashTable * table){
	return (table->tableSize==M);
}

/* Initializing my hash table either dictionary or wrongWord*/
hashTable* init_table(hashTable * table){
	int i;
	word *temp;
	table = (hashTable*)calloc(M,sizeof(hashTable)); 
	table->tableSize=0;
	for(i=0;i<M;i++){
		table[i].words=createWord();
	}

    return table;
}
/*-----------------------------------*/
/* Allocating memory for word struct: INITIALIZATION*/
word * createWord(){
	word *temp;
	temp = (word*)malloc(sizeof(word));
	temp->kelime = (char*)calloc(50,sizeof(char));
	temp->kelime[0]='\0';
	temp->suggestion = (char*)calloc(50,sizeof(char));
	temp->suggestion[0]='\0';
	return temp;
}

/*
if the word is found , then return its index,other wise -1
*/
int searchTable(hashTable * dictionary,char* wrd){
	int i =0;
	int index = hash(horner(wrd),i);
	while(( dictionary[index].words->kelime[0] !='\0' && strcmp(dictionary[index].words->kelime,wrd)!=0 )){
		i++;
		index=hash(horner(wrd),i);
	}
	if(strcmp(dictionary[index].words->kelime,wrd)==0){
		return index;
	}
	return -1;
}
/*
Inserting new words to the dictionary or the wrongWord Table, and add its suggestion (the word chosen by the user) with it;
*/
void  insertToTable(hashTable * table,char* wrd,char* suggest){
	int i,index;
	assert(isFull(table) == false);
	index =searchTable(table,wrd);//if it did not find the word returns -1
	if(index==-1){//if the world is not found in the table then insert
		i=0;
		index = hash(horner(wrd),i);
		while(i<M && table[index].words->kelime[0] != '\0'){
			i++;
			index = hash(horner(wrd),i);
		}
		strcpy(table[index].words->kelime,wrd);
		strcpy(table[index].words->suggestion,suggest);
		table->tableSize++;
	}
}

/*
	reading the smalldictionary file and insert it in my dictionary hash table
*/
int updateTable(hashTable * dictionary,char * filename){
	int i;char c,word[50];// flag var is to protect the program from displaying the worning more than once
	//c char is to receive every char from the file,
	//word[50] is to store the word after it has been read char by char
	FILE * fp = fopen(filename, "r");
	if (fp == NULL){
		fprintf(stderr,"could not open this file %s !\n",filename);
		return 0;
	}
	i=0;
	while(((c = fgetc(fp)) != EOF)){
		if((c >= 65 && c<=90) || (c>=97 && c <=122 )){
			word[i]=c;
			word[i+1] = '\0';
			i++;
		}else if(c == ' ' || c == '\n'){
			if(!isFull(dictionary)){
				insertToTable(dictionary,upper(word)," ");
			}
			i=0;
		}
	}
}
/*
searching for the words entered by the user
if it is found then return its index
otherwise return 0
*/
int searchForUserWord(hashTable * table,char* wrd){
	if(table->tableSize == 0){
		return 0;
	}
	int	index = searchTable(table,wrd);
	if(index!=-1){
		return index;
	}else{
		return 0;
	}
}

/*
printing the table // if needed
*/
void print(hashTable * table){
	int i,j;
	for(i=0;i<M;i++){
		if (table[i].words->kelime[0] != '\0'){
			printf("\"Index\":\"%d\",\t\"Word\":\"%3s\"",i,table[i].words->kelime);
			if(table[i].words->suggestion[0] != ' '){
				printf("\t\"suggestion:\"%3s\"",table[i].words->suggestion);
			}
			printf("\n");
		}
	}	
}
// i forgot about strlen() :) so i coded myself
int length(char * str){
	int len=0;
	int i=0;
	while(str[i]!=0){
		len++;
		i++;
	}
	return len;
}

/*
	calculating the distance between two words
*/
int levinshtein(char* str1,char* str2,int option){
	char temp1[50];
	char temp2[50];
	/*
		copy the smaller str to temp1 and the bigger to temp2
	*/
	if(length(str1) <= length(str2)){
		strcpy(temp1,str1);
		strcpy(temp2,str2);	
	}else if(length(str1) > length(str2)){
		strcpy(temp1,str2);
		strcpy(temp2,str1);
	}
	int len1=length(temp1);// len1 is the length of the smaller world
	int len2=length(temp2);// len2 is the length of the bigger world
/*
	if len2-len1 > 2 (max difference) then we shouldn't run
	the algorithm and we return
*/	
	if (len2-len1 > MAX_DIF){
		return len2-len1;
	}
/*
	otherwise
*/
	int editDistance[len1+1][len2+1];
	int i,j;
	for(i=0;i<=len1;i++){
		for(j=0;j<= len2;j++){
			if (i==0 && j<=len2){
				editDistance[i][j]=j;
			}else if(j==0 && i<=len1){
				editDistance[i][j]=i;
			}else if(i>0 && j >0 & temp1[i-1] == temp2[j-1]){
				editDistance[i][j]=editDistance[i-1][j-1];
			}else{
				editDistance[i][j]=min(editDistance[i-1][j-1]+1,min(editDistance[i-1][j]+1,editDistance[i][j-1]+1));
			}
			/*
				when len2-len1 is < MAX_DIFFERENCE , we can stop the algorithm when editDistance[i-1][j+(len2-len1)-1] > MAX_DIFFERENCE
				here we stop the algorithm and return.
				that will be enough for us to be sure 100% that the distance will exceed MAX_DIFFERENCE;
			*/
			if(  i!=0 && j!=0 && i == j && editDistance[i-1][j+len2-len1-1] > MAX_DIF){
				return editDistance[i-1][j+len2-len1-1];
			}
		}
	}
	if (option == 2){
		for(i=0;i<=len2;i++){
			if(i==0){
				printf("\t%c",'-');
			}else{
				printf("\t%c",temp2[i-1]);
			}
		}
		printf("\n");
		for(i=0;i<=len1;i++){
			if(i==0){
				printf("%c\t",'-');
			}else{
				printf("%c\t",temp1[i-1]);
			}
			for(j=0;j<= len2;j++){
				printf("%d\t",editDistance[i][j]);
			}
			printf("\n");
		}
	}
	return editDistance[len1][len2];
}

/*
	takes the sentence and split it into words, if the word 
	is not in the dictionary table then we search in the worngWord table
	if it is found then we replace the wrong word with the suggestion
	(right word entered before by the user)
	if it is not found then we calculate its distance with the words 
	in the dictionary,
	then displays the words with distance of 1 ,if there is not words with distance of 1
	then displays the words with distance of 2;
	if there are no words with dist 1 or 2 then inform the user that there isn't any word
	similar to his word in our dictionary
*/
void searchEveryWord(hashTable* table, hashTable* wrongWord, char* sentence){
	char *wrd, oldSentence[200];
	int flag;
	strcpy(oldSentence,sentence);
	strcpy(sentence,"");
	
	wrd = strtok(oldSentence," ");
	while(wrd != NULL){
		flag = searchForUserWord(table, wrd);
		if(flag == 0){ // the word is not found in the dictionary
			flag = searchForUserWord(wrongWord, wrd);
			if(flag != 0){ // the word is found in the wrongWord table and not found in the dictionary, so replace the word with the right word
				strcat(sentence,wrongWord[flag].words->suggestion);
				strcat(sentence," ");
			}else{ // the word is not found in the wrongWord and dictionary tables
				checkdifference(table,wrongWord,wrd,sentence);
			}
		}else{
			strcat(sentence,table[flag].words->kelime);
			strcat(sentence," ");
		}
		wrd = strtok(NULL," ");
	}
	
}

/*
	adding the words with distance 1 to an array,
	and adding the words with distance 2 to a different array
	if there is one word in the first array then display it as a suggestion
	otherwise display the second array
*/

void checkdifference(hashTable* table,hashTable* wrongWord, char* wrd,char* sentence){
	int i,j=0,k=0,index;
	char oneDist[M][50];
	char twoDist[M][50];
	printf("%s is not found in the dictionary.\n", wrd);
	for(i=0;i<M;i++){
		if(table[i].words->kelime[0]!= '\0'){
			if (levinshtein(table[i].words->kelime,wrd,0) == 1 ){
				strcpy(oneDist[j],table[i].words->kelime);
				j++;
			}else if(levinshtein(table[i].words->kelime,wrd,0) == 2){
				strcpy(twoDist[k],table[i].words->kelime);
				k++;
			}
		}
	}
	
	if(j>0){// if there is one element in onDist array then we display it and ignore the twoDist elements
		printf("did you mean: (Enter an index:)\n");
		for(i=0;i<j;i++){
			printf("%d-> %s\n",i+1,oneDist[i]);
		}
		printf("if you think this word was entered right then press 444 to add it to our dictionary\n");
		scanf("%d",&index);
		while(index>j && index != 444){
			printf("OH OH! wrong index! please try again..\n");
			scanf("%d",&index);
		}
		if (index!= 444){
			insertToTable(wrongWord,wrd,oneDist[index-1]);
			strcat(sentence,oneDist[index-1]);
			strcat(sentence," ");
		}else if(index == 444){
			insertToTable(table,wrd,"");
			strcat(sentence,wrd);
			strcat(sentence," ");
		}
					
	}else if(k>0){//else display 2 distance words 
		printf("did you mean: (Enter an index:)\n");
		for(i=0;i<k;i++){
			printf("%d-> %s\n",i+1,twoDist[i]);
		}
		printf("if you think this word was entered right then press 444 to add it to our dictionary\n");
		scanf("%d",&index);
		while(index>k && index!=444){
			printf("OH OH! wrong index! please try again..\n");
			scanf("%d",&index);
		}
		if (index != 444){
			insertToTable(wrongWord,wrd,twoDist[index-1]);
			strcat(sentence,twoDist[index-1]);
			strcat(sentence," ");	
		}else if(index == 444){
			insertToTable(table,wrd,"");
			strcat(sentence,wrd);
			strcat(sentence," ");
		}
	}else if(j==0 && k==0){	// otherwise:
			printf("No near words to %s found :(\n\n",wrd);
			strcat(sentence,wrd);
			strcat(sentence," ");
	}
}


