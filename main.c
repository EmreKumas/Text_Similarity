//Emre Kumas

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

//Creating our structure which will hold the words and the next pointers.
typedef struct structNode{
    char word[64];
    struct structNode *next;
}Words;

//Creating our file pointers.
FILE *file1ptr;
FILE *file2ptr;

//Global variables.
int numberCommon = 0;
int numberTwoGrams = 0;

Words *readFromFile(Words *list, FILE *file, int wP);
Words *twoGram(Words *list, Words *newList);
Words *commonWords(Words *listCommon, char *commonWord, int isTwoGram);
Words *compare(Words *list1, Words *list2, Words *listCommon, int isTwoGram);
Words *sortList(Words *list);
char *checkPunctuation(char *word);
void printList(Words *list);
void cleanUp(Words *list);
int isDotorComma(char *text);

int main(){
    //Firstly, we need to check if the file1.txt and file2.txt exist.
    if(access("file1.txt", F_OK) != -1 && access("file2.txt", F_OK) != -1){

        //Means files exist. Now we need to read them.
        file1ptr = fopen("file1.txt", "r");
        file2ptr = fopen("file2.txt", "r");

        //Creating two struct pointers for our word linked lists which will be NULL at first.
        Words *file1 = NULL;
        Words *file2 = NULL;

        //I'm gonna use extra two linked lists for storing two grams with punctuations. (This is necessary because without punctuations
        // the program can't know where the sentence finishes.)
        Words *file1wP = NULL;
        Words *file2wP = NULL;

        //The linked lists which will hold the common words and two grams in them.
        Words *common = NULL;
        Words *twoGram1 = NULL;
        Words *twoGram2 = NULL;
        Words *commonTwoGrams = NULL;

        //After creating linked lists, we need to fill them with the files.
        file1 = readFromFile(file1, file1ptr, 0);
        file2 = readFromFile(file2, file2ptr, 0);
        rewind(file1ptr);
        rewind(file2ptr);
        file1wP = readFromFile(file1wP, file1ptr, 1);
        file2wP = readFromFile(file2wP, file2ptr, 1);

        //Comparing the linked lists and creating a new linked list consisting of the common words.
        common = compare(file1, file2, common, 0);
        //Sorting the common linked list alphabetically.
        common = sortList(common);

        //Creating the two-grams linked lists. Then we need to compare them and create a new linked list called common 2-grams.
        //Then we need to sort it too.
        twoGram1 = twoGram(file1wP, twoGram1);
        twoGram2 = twoGram(file2wP, twoGram2);
        commonTwoGrams = compare(twoGram1, twoGram2, commonTwoGrams, 1);
        commonTwoGrams = sortList(commonTwoGrams);

        //Printing the results.
        printf("\nthe number of common words: %d\n", numberCommon);
        printf("\nthe common words are:\n\n");
        printList(common);

        printf("\nthe number of common 2-grams: %d\n", numberTwoGrams);
        printf("\nthe common 2-grams are:\n\n");
        printList(commonTwoGrams);

        //We need to close the file pointers.
        fclose(file1ptr);
        fclose(file2ptr);

        //We need to free the memory we used.
        cleanUp(file1);
        cleanUp(file2);
        cleanUp(file1wP);
        cleanUp(file2wP);
        cleanUp(common);
        cleanUp(twoGram1);
        cleanUp(twoGram2);
        cleanUp(commonTwoGrams);

    }else{
        //Means one or two of the files don't exist. We will give error to the user.
        printf("file1.txt or/and file2.txt don't exist. Please make sure they exist before starting this program.\n");
    }
    printf("\n");
    return 0;
}

Words *readFromFile(Words *list, FILE *file, int wP){

    //Tracking the last pointer by the help of currentPtr.
    Words *currentPtr = list;

    //Variable to hold the word.
    char chrWord[64];
    //An extra variable for the errors caused by numbers.
    char extChrWord[64];

    //Until reaching to the end of file, we will store every word in the linked list.
    while(!feof(file)){
        //Read the word if it exists.
        fscanf(file, "%s", chrWord);
        strcpy(extChrWord, chrWord);

        if(strlen(checkPunctuation(extChrWord)) > 0){
            Words *newWord = malloc(sizeof(Words));

            //We need to check if it has any punctuation. If it has we need to delete them. But for two-grams, we will hold them for now.
            if(wP == 0)
                strcpy(chrWord, checkPunctuation(chrWord));

            //We need to convert the first letter of the word to the lowercase.
            chrWord[0] = (char) tolower(chrWord[0]);
            strcpy(newWord -> word, chrWord);

            if(list == NULL){
                //Which means the list is empty. We will create the first element of the linked list.
                currentPtr = newWord;
                currentPtr -> next = NULL;
                list = currentPtr;
            }else{
                //Means the list already includes something.
                currentPtr -> next = newWord;
                newWord -> next = NULL;
                currentPtr = newWord;
            }
        }
    }
    return list;
}

Words *compare(Words *list1, Words *list2, Words *listCommon, int isTwoGram){

    Words *current1 = list1;
    Words *current2 = list2;

    while(current1 != NULL){
        while(current2 != NULL){
            if(strcmp(current1 -> word, current2 -> word) == 0){
                //Means two words are the same. We will store them in a new linked list.
                if(strlen(current1 -> word) > 0)
                    listCommon = commonWords(listCommon, current1 -> word, isTwoGram);

            }

            //Go to the next word for the second linked list.
            current2 = current2 -> next;
        }

        //Go to the next word for the first linked list and go back to the start for the second list.
        current1 = current1 -> next;
        current2 = list2;
    }
    return listCommon;
}

Words *commonWords(Words *listCommon, char *commonWord, int isTwoGram){

    int isStored = 0;
    Words *current = listCommon;

    if(current == NULL){
        //If the list is empty we need to create the first element of the list.
        Words *newWord = malloc(sizeof(Words));
        strcpy(newWord -> word, commonWord);

        current = newWord;
        current -> next = NULL;
        listCommon = current;
        //If isTwoGram parameter is 1 that means this is checking to find two-grams.
        (isTwoGram == 0) ? numberCommon++ : numberTwoGrams++;
    }else{
        //If the list already contains some elements, we need to check if it contains the same word before.
        while(current != NULL){
            if(strcmp(current -> word, commonWord) == 0){
                //Means it is already stored. We need to break out of the loop and exit the function.
                isStored = 1;
                break;
            }

            current = current -> next;
        }

        if(isStored == 0){
            //Means the word is not stored yet.
            Words *newWord = malloc(sizeof(Words));
            strcpy(newWord -> word, commonWord);

            //We will go to the last element of the list and add the word there.
            current = listCommon;
            while(current -> next != NULL){
                current = current -> next;
            }

            current -> next = newWord;
            newWord -> next = NULL;

            (isTwoGram == 0) ? numberCommon++ : numberTwoGrams++;
        }

    }
    return listCommon;
}

Words *twoGram(Words *list, Words *newList){

    //This function creates a new linked lists based on the linked list passed as parameter. The new linked list consists of
    //2-grams of the parameter linked list.
    Words *currentSource = list;
    Words *currentDest = newList;
    char twoWord[64];

    while(currentSource != NULL){
        //This is checking if it is the last element and if the first element contains dot or comma.
        if(currentSource -> next != NULL && isDotorComma(currentSource -> word) == 0){
            sprintf(twoWord, "%s %s", currentSource -> word, currentSource -> next -> word);

            Words *newWord = malloc(sizeof(Words));
            //Now we need to get rid of punctuations.
            strcpy(twoWord, checkPunctuation(twoWord));
            strcpy(newWord -> word, twoWord);

            if(newList == NULL){
                //Which means the list is empty. We will create the first element of the linked list.
                currentDest = newWord;
                currentDest -> next = NULL;
                newList = currentDest;
            }else{
                //Means the list already includes something.
                currentDest -> next = newWord;
                newWord -> next = NULL;
                currentDest = newWord;
            }
        }

        currentSource = currentSource -> next;
    }
    return newList;
}

Words *sortList(Words *list){

    int swapped;
    char temp[64];
    Words *current = list;

    //If the list is not NULL, then sorting can be done.
    while(list != NULL){
        swapped = 0;

        while(current -> next != NULL){

            if(strcmp(current -> word, current -> next -> word) > 0){
                //Means the first word is greater than the second word, means they need to change.
                strcpy(temp, current -> word);
                strcpy(current -> word, current -> next -> word);
                strcpy(current -> next -> word, temp);

                //If any sorting process happened, we need to check the entire list again.
                swapped = 1;
            }

            current = current -> next;
        }

        //Return to the beginning.
        current = list;

        if(swapped == 0){
            //If no sorting has happened, then we can exit the loop.
            break;
        }
    }

    return list;
}

int isDotorComma(char *text){

    //This function checks if the last character of the word is dot or comma.
    int length = strlen(text);

    if( text[length - 1] == '.' || text[length - 1] == ',' )
        return 1;

    return 0;
}

char *checkPunctuation(char *word){

    //Getting the length of the word.
    int length = strlen(word);
    int i,j = 0;
    //We will create a new char array to hold the word without punctuations.
    char newWord[64];

    for(i = 0; i < length; i++){
        //If the letter is an alphabetical letter or a space, we will add it in to new char array.
        if(isalpha(word[i]) || word[i] == ' '){
            newWord[j++] = word[i];
        }
    }

    //We need to do this or the program doesn't know where the array is gonna end.
    newWord[j] = '\0';

    //And finally return the new word.
    strcpy(word, newWord);
    return word;
}

void printList(Words *list){
    while(list != NULL){
        printf("%s\n", list -> word);
        list = list -> next;
    }
}

void cleanUp(Words *list){

    //The clean up function for removing linked lists.
    Words *holdMe = list;
    Words *freeMe = NULL;

    while(holdMe != NULL){
        freeMe = holdMe;
        holdMe = holdMe -> next;
        free(freeMe);
    }
}
