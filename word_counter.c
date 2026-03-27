//====================
//==== LIBRARIES =====
//====================
//Standard Libraries + Libraries needed for multiprocessing, multithreading, memory sharing, calculating time ...
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/wait.h>
#define WORD_MAXSIZE 75 //Assume longest word is 75 chars
#define INITIAL_ARRAY_SIZE 35000 //Assume initial array size of 35000
#define FILE_NAME "text.txt"  // <- SET THE FILE NAME HERE
//====================
//===== STRUCTS ======
//====================
//Struct to hold a word and frequency
typedef struct cell_node Cell;
struct cell_node {
    char word[WORD_MAXSIZE];
    int frequency;
};
//Struct of arguments to pass to the thread function
typedef struct arg_node Args;
struct arg_node {
    int firstWord; //The first word to read by the thread
    int lastWord;   //Stop here
    int uniqueCount;  //Size for the array, and count for the unique words counted by the thread
    Cell *words;  //Array of cell structs
};
//==========================
//= FUNCTIONS DECLARATIONS =
//==========================
void displayStartMenu();
//Naive Approach Functions
void naiveApproach();
void naive_insertion(Cell *, char* );
//Multiprocessing/threading Functions
void multiprocessingApproach();
int multip_insertion(Cell *array,char word[], int uniqueCount);
void multithreadingApproach();
void* threadCounter(void* arg);
//Functions needed for data processing
int sumFrequencies(Cell *array, int size);
int compareCellsByWord(const void* a, const void* b);
int compareCells(const void* a, const void* b);
//======================
//== GLOBAL VARIABLES ==
//======================
int uniqueCountNaive=0;  //Stores number of unique words counted, used during Naive Approach
struct timeval start, end ,startParallel, endParallel;  //Used to calculate the real time spent during the execution.
double timeInSeconds, timeParallel;  //The total time is printed in seconds, time after User Input until end of execution.
//=====================
//=== MAIN FUNCTION ===
//=====================
int main(void) {
    int approach = 0;
    displayStartMenu();
    while (approach!=1 && approach!=2 && approach!=3) {  //Take user input, until input is accepted
        printf("Choice:");
        scanf("%d", &approach);
        switch (approach) {
            case 1: //Naive Approach
                naiveApproach();  //Call naive approach function
                    break;
            case 2: //Multiprocessing Approach
                multiprocessingApproach();  //Call multi-process approach function
                    break;
            case 3: //Multithreading Approach
                multithreadingApproach();  //Call multi-threading approach function
                    break;
            default: //False input ask user for input again
                printf("Invalid Choice - Please try again (1, 2 or 3)\n");
            break;
        }
    }
    return 0;
}
//=========================
//= FUNCTIONS DEFINITIONS =
//=========================
void displayStartMenu() {  //Prints the main menu options
    printf("=====================\n");
    printf("= List Top 10 Words =\n");
    printf("=====================\n");
    printf("Please choose on of the following approaches: (Enter an integer 1, 2 or 3)\n");
    printf("1.Naive Approach\n");
    printf("2.Multiprocessing Approach\n");
    printf("3.Multithreading Approach\n");
}
int compareCells(const void* a, const void* b) {  //Comparison function for sorting based on frequency, used for qsort()
    return ((Cell*)b)->frequency-((Cell*)a)->frequency;
}
int compareCellsByWord(const void* a, const void* b) {  //Comparison function for sorting based on word, used for qsort()
    return (strcmp(((Cell*)a)->word,((Cell*)b)->word));
}
void naiveApproach() {  //Function to execute naive approach
    gettimeofday(&start, NULL);
    int size = INITIAL_ARRAY_SIZE;
    Cell *array = malloc(sizeof(Cell) * size);  //Allocate memory for the array of structs based on the initial size
    if (array == NULL) {
        printf("ERROR === MEMORY ALLOCATION FAILED\n");
        return;
    }
    //Access elements using array[index].word or array[index].frequency
    //Read each word from the file and check if it exists in the array if it does, then increase the frequency, else add the word to the array with frequency=1
    FILE *Fpointer_in = fopen(FILE_NAME,"r");
    if(Fpointer_in !=NULL){  //Check if the input file exists
        //printf("Counting ...\n");
        char str[WORD_MAXSIZE];  //String to load characters into
        char ch;  //Char to hold characters from the file to insert into str
        int i=0;  //Index
        while ((ch= fgetc(Fpointer_in))!=EOF){  //While loop reads until EOF "END OF FILE" is found.
            if(ch != ' '  && ch != '\n'){  //If "ch" is not space or new line insert into "str"
                str[i]=ch;  //Insert ch into str
                i++;  //Increment index
            }else if (i>0){  //If space letter or new line encountered then terminate the string and insert it
                str[i]='\0';  //Add the Null Terminator
                if(uniqueCountNaive==size) {  //Check if the array needs resizing
                    //Reallocate
                    size=size*2;  //Double the size
                    //If reallocate fails, we lose the og pointer. create a temp pointer, to reallocate at to not lose memory if it fails.
                    Cell* new_array=realloc(array,sizeof(Cell) * size);
                    if(new_array==NULL) {
                        printf("ERROR === MEMORY REALLOC FAILED\n");
                        free(array);
                        return ;
                    }else {  //If reallocate succeed set the "array" to point to the new reallocated memory
                        array=new_array;
                    }
                    //printf("%d Unique words and counting :)\n",uniqueCountNaive);
                }
                //printf("%d %d - unique words - size\n :",uniqueCountNaive,size);
                naive_insertion(array,str);  // Insert into array of struct
                i=0;  //Reset index to 0 to read next word
            }
        }
        //EOF reached, the last char read was alphabetical and there is now a word in "str" that needs to be inserted if index is>0
        if(i>0){
            str[i]='\0';  //Terminate the string
            if(uniqueCountNaive==size) {  //Check if the array needs resizing
                size=size*2;  //Double the size
                //If reallocate fails, we lose the og pointer. create a temp pointer, to reallocate at to not lose memory if it fails.
                Cell* new_array=realloc(array,sizeof(Cell) * size);
                if(new_array==NULL) {
                    printf("ERROR === MEMORY REALLOC FAILED\n");
                    free(array);
                    return;
                }else {
                    array=new_array;  //If reallocate succeed set the "array" to point to the new reallocated memory
                }
                //printf("%d Unique words and counting :)\n",uniqueCountNaive);
            }
            naive_insertion(array,str);  //Insert into array of struct
        }
        fclose(Fpointer_in);  //Close file
    }else{
        printf("===ERROR===\n===FILE NOT FOUND===\n=========\n");
        return;
    }

    //Sort Array of Struct according to frequency
    //printf("Final Unique words count:%d\n",uniqueCountNaive);
    //printf("Sorting ...\n");
    qsort(array,uniqueCountNaive,sizeof(Cell),compareCells);
    //Print top 10 Words
    printf("=== Top 10 Repeated Words ===\n");
    printf ("Word\tFrequency\n");
    for(int i=0;i<10;i++) {
        printf("%s\t%d\n",array[i].word,array[i].frequency);
    }
    printf("==============================\n");
    gettimeofday(&end, NULL);
    timeInSeconds = (end.tv_sec - start.tv_sec) *1000000;  // Convert Seconds to microseconds
    timeInSeconds = timeInSeconds + (end.tv_usec - start.tv_usec);  //Microseconds
    printf("Time used: %f\n", timeInSeconds/1000000);
    free(array);  //Free the allocated memory
}
void naive_insertion(Cell *array,char word[]) {  //Function to insert a word into the struct of array
    //Check if word exist
    //uniqueCountNaive represents occupied cells in array
    //printf("Count of unique:%d\n: ", uniqueCountNaive);
    for (int i=0;i<uniqueCountNaive;i++) {
        if (strcmp(array[i].word,word)==0) {  //Check if the word exists
            array[i].frequency++; //If it does exist,then increase its frequency
            return;  //If found return
        }
    }
    //else If the word is not found add it to the index occupied then increment the occupied counter
    strcpy(array[uniqueCountNaive].word,word);  //Copy the word
    array[uniqueCountNaive].frequency=1;  //Set the frequency counter to 1
    uniqueCountNaive++;  //Increase the count of unique words
}
void multiprocessingApproach() {  //Main multiprocessing function
    printf("=============================\n");
    int numberOfChild=0;  //Take the number of child processes from user input
    while (numberOfChild!=2 && numberOfChild!=4 && numberOfChild!=6 && numberOfChild!=8) {
        printf("Choose the number of children: (2, 4, 6 or 8):");
        scanf("%d", &numberOfChild);
        if (numberOfChild==2 || numberOfChild==4 || numberOfChild==6 || numberOfChild==8) {
            printf("\nNumber of Children processes -> %d\n",numberOfChild);
        }else {
            printf("Invalid Choice - Please try again (2, 4, 6 or 8)\n");
        }
    }
    gettimeofday(&start, NULL);
    system("wc -w < "FILE_NAME" > CountTemp.txt");  //Shell command in the system to find word count in the file and direct output to temp file.
    FILE *Fpointer_inTemp = fopen("CountTemp.txt","r");  //Read the count from the file then close file and remove it using rm command
    if(Fpointer_inTemp ==NULL) {
        printf("ERROR === GENERATING COUNT FAILED\n");
        return;
    }
    int fileWC;  //Count of words in the file
    fscanf(Fpointer_inTemp,"%d",&fileWC);
    fclose(Fpointer_inTemp);
    system("rm CountTemp.txt");  //Delete the file after done using it

    //Now we have the total count of words in the file, and the number of processes.
    //We can estimate how much words each process gets to handle to be almost equal
    //Calculate part size for each child

    int partSize=(fileWC/numberOfChild)+1;  //I added the 1 in case the number of words is not div by number of child processes
    //The last child will have smaller text to deal with by a few words, but it is nothing for millions of words

    //printf("Number of Words: %d\tSize of Each Part:%d",fileWC,partSize);

    // Use mmap to allocate shared memory
    int memorySize = (partSize * numberOfChild * sizeof(Cell)) + (numberOfChild * sizeof(int));  //Calculate the needed memory size
    void *sharedMemory = mmap(NULL, memorySize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    Cell *arrays = (Cell *)sharedMemory;  //Shared Memory array of struct for children processes
    int *sharedUniqueCounts = (int *)(arrays+(numberOfChild * partSize));  //Shared unique words count arrays for each child
    for (int i = 0; i < numberOfChild; i++) {  //Initialize unique words counts
        sharedUniqueCounts[i] = 0;
    }
    //CHILD PROGRAM EXECUTES HERE
    //READ FROM FILE AND COUNT INTO EACH CHILD ARRAY
    gettimeofday(&startParallel, NULL);  //Start counting time for parallel execution
    for (int i = 0; i < numberOfChild; i++) {
        pid_t processID = fork();  //Create a new child process
        if (processID == 0) {  //If Child process
            int firstWord = i * partSize;
            int lastWord = (i + 1) * partSize;
            if (lastWord > fileWC) {
                lastWord = fileWC;
            }  //Calculate the range of words, that this child should process
            //printf("\nChild(PID): %d || First Word: %d || Last Word: %d\n", getpid(), firstWord, lastWord);

            FILE *Fpointer_inFile = fopen(FILE_NAME, "r");
            if (Fpointer_inFile == NULL) {
                printf("ERROR === FILE NOT FOUND IN CHILD EXECUTION\n");
                exit(0);
            }
            char str[WORD_MAXSIZE];
            char ch;
            int wordIndex=0;
            int j=0;
            Cell *childArray = arrays + (i * partSize);  //Access the child array through childArray[i]
            int flagInsert = 0;  //If flag 0 do not insert if 1 insert because the process is reading words in range
            while ((ch = fgetc(Fpointer_inFile)) != EOF) {  //Read until EOF
                if (ch != ' ' && ch != '\n') {  //Check for a space or a newline(there is no new lines in the clean text we got via Ritaj but just in case)
                    str[j++] = ch;
                    continue;  //Go to next char
                }
                if (j > 0) {  //If a space or new line check if a word is in str and insert it
                    str[j] = '\0';  //Add the null terminator
                    j = 0;  //Reset j index
                    if (flagInsert==0 && wordIndex == firstWord) {
                        flagInsert=1;  //If we got to the range set the flag
                    }
                    if (flagInsert==1 && wordIndex<lastWord) {  //If the flag is set, and the word is not the last yet, then insert
                        sharedUniqueCounts[i] = multip_insertion(childArray, str, sharedUniqueCounts[i]);  //This returns an incremented count, and the array gets modified
                    }
                    if (wordIndex >= lastWord) {  //If last word then exit the loop
                        break;
                    }
                    wordIndex++;  //Increment the word index
                }
            }
            if (j > 0) {  //If we broke the while loop by achieving EOF, then str might have a word in it, check and insert
                str[j] = '\0';  //Add null terminator
                if (flagInsert==1 && wordIndex < lastWord) {   //Insert if we are in correct range
                    sharedUniqueCounts[i] = multip_insertion(childArray, str, sharedUniqueCounts[i]);
                }
            }
            fclose(Fpointer_inFile);  //Close file
            exit(0);  //Exit child
        } else if (processID < 0) {
            printf("ERROR === FORKING FAILED\n");  //Error if forking failed
        }
    }
    for (int i = 0; i < numberOfChild; i++) {  //Wait for all child processes to finish
        wait(NULL);
    }
    gettimeofday(&endParallel, NULL);  //Stop counting for parallel execution

    //Merge arrays, and treat duplicates, and sort, then print top 10
    //all arrays are from arrays[0] -> arrays [(numberOfChild*partSize)-1], but there empty spaces, we need to copy valid cells into a single array (merge)

    int totalUniqueCountInArrays=0;
    for (int i = 0; i < numberOfChild; i++) {  //Calculate total unique count in all arrays duplicated
        totalUniqueCountInArrays = totalUniqueCountInArrays+sharedUniqueCounts[i];
    }
    Cell *mergedData = malloc(totalUniqueCountInArrays * sizeof(Cell));  //Allocate Memory for merged data array
    if (mergedData == NULL) {
        printf("ERROR === MALLOC FAILED\n");
        return;
    }
    int mergedDataIndex = 0;
    //printf("total unique count in arrays:%d\n",totalUniqueCountInArrays);
    for (int i = 0; i < numberOfChild; i++) {  //Traverse child arrays and copy valid cells, into the merged data array
        //printf("I/SUQ/%d/%d\n ", i,sharedUniqueCounts[i]);
        Cell *childArray = arrays + (i * partSize);  //Access the child array through childArray[index]
        for (int j=0; j<sharedUniqueCounts[i]; j++) {  //Here we access each child cell through childArray[j]
            //printf ("Copying word/freq %s/%d\n",childArray[j].word,childArray[j].frequency);
            strcpy (mergedData[mergedDataIndex].word, childArray[j].word);  //Copy Str
            mergedData[mergedDataIndex].frequency = childArray[j].frequency;  //Copy Freq
            //printf ("At Index %d in merged Data : Word/Freq %s/%d \n",mergedDataIndex,mergedData[mergedDataIndex].word,mergedData[mergedDataIndex].frequency);
            mergedDataIndex++;  //Increment the index of the merged dara array
        }
        //printf("==Merged #%d Child data==\n",i);
    }
    qsort(mergedData,totalUniqueCountInArrays,sizeof(Cell),compareCellsByWord);  //This way the array is sorted based on alphabet, and duplicates words are adjacent
    //Now get rid of duplicates by calculating the sum of their frequencies
    int newSize=sumFrequencies(mergedData,totalUniqueCountInArrays);  //Now we have the new size, and the arrays are unique only
    qsort(mergedData,newSize,sizeof(Cell),compareCells);  //Sort arrays based on frequency
    //Print top 10 most frequent words
    printf("=== Top 10 Repeated Words ===\n");
    printf ("Word\tFrequency\n");
    for(int i=0;i<10;i++) {
       printf("%s\t%d\n",mergedData[i].word,mergedData[i].frequency);
    }
    printf("==============================\n");
    gettimeofday(&end, NULL);
    timeInSeconds = (end.tv_sec - start.tv_sec) *1000000;  //Convert Seconds to microseconds
    timeInSeconds = timeInSeconds + (end.tv_usec - start.tv_usec);
    timeParallel = (endParallel.tv_sec - startParallel.tv_sec) *1000000;
    timeParallel = timeParallel + (endParallel.tv_usec - startParallel.tv_usec);
    printf("Total time used: %f\n", timeInSeconds/1000000);  //Convert back to seconds
    printf("Time in parallel: %f\n", timeParallel/1000000);
    free(mergedData);  //Free allocated merged data array
    munmap(sharedMemory, memorySize);  //Unmap shared memory
}
int multip_insertion(Cell *array,char word[], int uniqueCount) {  //Pass to this Array ARRAYS[i], string, uniqueCount[i]
    for (int i=0;i<uniqueCount;i++) {  //Check if word exist
        if (strcmp(array[i].word,word)==0) {
            array[i].frequency++; //If the word exists increase its frequency
            return uniqueCount; //If found return
        }
    }
    //else If the word is not found add it to the index occupied then increment the occupied counter
    strcpy(array[uniqueCount].word,word);  //Copy the word
    array[uniqueCount].frequency=1;  //Set the frequency counter
    return ++uniqueCount;  //Return incremented count
}
int sumFrequencies(Cell *array, int size) {  //Function to get rid of duplicates by summing their frequencies
    int uniqueCount=0;
    for (int i=1;i<size;i++) {
        if (strcmp(array[i].word,array[uniqueCount].word)==0) {  //Check if the previous unique element is the same
            array[uniqueCount].frequency = array[uniqueCount].frequency + array[i].frequency;
        }else {  //If element at 'i' is unique, then increment unique count
            uniqueCount++;
            array[uniqueCount]=array[i];
        }
    }
    return uniqueCount+1;  //Return the final count of unique elements
}
void multithreadingApproach() {  //Function to list the top 10 most frequent words using the multi-threading approach
    printf("=============================\n");
    int numberOfThreads=0;  //Take the number of threads processes from user input
    while (numberOfThreads!=2 && numberOfThreads!=4 && numberOfThreads!=6 && numberOfThreads!=8) {
        printf("Choose the number of threads: (2, 4, 6 or 8):");
        scanf("%d", &numberOfThreads);
        if (numberOfThreads==2 || numberOfThreads==4 || numberOfThreads==6 || numberOfThreads==8) {
            printf("\nNumber of threads -> %d\n",numberOfThreads);
        }else {
            printf("Invalid Choice - Please try again (2, 4, 6 or 8)\n");
        }
    }
    gettimeofday(&start, NULL);
    system("wc -w < "FILE_NAME" > CountTemp.txt");  //Shell command in the system to find word count in the file and direct output to temp file.
    FILE *Fpointer_inTemp = fopen("CountTemp.txt","r");  //Read the count from the file then close file and remove it using rm command
    if(Fpointer_inTemp ==NULL) {
        printf("ERROR === GENERATING COUNT FAILED\n");
        return;
    }
    int fileWC;  //Count of words in the file
    fscanf(Fpointer_inTemp,"%d",&fileWC);
    fclose(Fpointer_inTemp);
    system("rm CountTemp.txt");  //Delete the file after done using it

    //Now we have the total count of words in the file, and the number of threads.
    //We can estimate how much words each thread gets to handle to be almost equal.

    // Calculate part size for each threads
    int part_size=(fileWC/numberOfThreads)+1; //I added the 1 in case the number of words is not div by number of threads
    //one thread will have smaller text to deal with by a few words, but it is nothing for millions of words

    Args *ArgsArray=malloc(sizeof(Args)*numberOfThreads);  //Array of structs, to be passed to threads functions as arguments
    if (ArgsArray==NULL) {
        printf("ERROR === MALLOC FAILED\n");
        return;
    }
    pthread_t *threadsID=malloc(sizeof(pthread_t)*numberOfThreads);  //Array to hold thread ids
    if (threadsID==NULL) {
        printf("ERROR === MALLOC FAILED\n");
        return;
    }
    Cell *wordsList = malloc(numberOfThreads*(part_size+1)*sizeof(Cell));  //Arrays of structs for counting
    if (wordsList==NULL) {
        printf("ERROR === MALLOC FAILED\n");
        return;
    }
    gettimeofday(&startParallel,NULL);
    for (int i=0;i<numberOfThreads;i++) {  //For each thread fill the arguments
        int firstWord = i * part_size;  //First word for this thread to process
        int lastWord = (i + 1) * part_size;  //Last Word for this thread range
        if (lastWord > fileWC) {
            lastWord = fileWC;
        }
        (ArgsArray+i)->firstWord=firstWord;
        (ArgsArray+i)->lastWord=lastWord;
        (ArgsArray+i)->uniqueCount=0;
        (ArgsArray+i)->words=(wordsList+(part_size*i));
        pthread_create(&threadsID[i], NULL, threadCounter, (void *)(ArgsArray+i)); //Create thread and pass the args struct
    }
    for (int i=0;i<numberOfThreads;i++) {  //Wait for threads to finish
        pthread_join(threadsID[i], NULL);
    }
    gettimeofday(&endParallel,NULL);
    int totalWordsCells=0;
    for (int i=0;i<numberOfThreads;i++) {  //Calculate total unique words count
        totalWordsCells += (ArgsArray+i)->uniqueCount;
        //printf("Total words cells -> %d\n",totalWordsCells);
    }
    Cell *mergedData = malloc(totalWordsCells * sizeof(Cell));  //Allocate Memory for data to be merged
    if (mergedData == NULL) {
        printf("ERROR === MALLOC FAILED\n");
        return;
    }
    int mergedDataI=0;
    for (int i=0;i<numberOfThreads;i++) {  //Same as merging data in multiprocess approach
        //printf("Merging Thread ID %d\n",i);
        Cell *threadArray = (ArgsArray+i)->words;
        for (int j=0; j<(ArgsArray+i)->uniqueCount;j++) {
            //printf("WORD/FREQ %s/%d\n",(ArgsArray+i)->words[j].word,(ArgsArray+i)->words[j].frequency);
            strcpy(mergedData[mergedDataI].word,threadArray[j].word); //Copy the word into final array
            mergedData[mergedDataI].frequency = threadArray[j].frequency;  //Copy the frequency
            mergedDataI++;  //Increment index
        }
    }
    qsort(mergedData,totalWordsCells,sizeof(Cell),compareCellsByWord);  //Sorting Alphabetically
    int newSize=sumFrequencies(mergedData,totalWordsCells);  //Fixing Duplication
    qsort(mergedData,newSize,sizeof(Cell),compareCells);  //Sorting by frequency
    //Print top 10 Words
    printf("=== Top 10 Repeated Words ===\n");
    printf ("Word\tFrequency\n");
    for(int i=0;i<10;i++) {
        printf("%s\t%d\n",mergedData[i].word,mergedData[i].frequency);
    }
    printf("==============================\n");
    gettimeofday(&end, NULL);
    timeInSeconds = (end.tv_sec - start.tv_sec) *1000000;  //Convert Seconds to microseconds
    timeInSeconds = timeInSeconds + (end.tv_usec - start.tv_usec);
    timeParallel = (endParallel.tv_sec - startParallel.tv_sec) *1000000;
    timeParallel = timeParallel + (endParallel.tv_usec - startParallel.tv_usec);
    printf("Total time used: %f\n", timeInSeconds/1000000);  //Convert back to seconds
    printf("Time in parallel: %f\n", timeParallel/1000000);
    //Free allocated memory
    free(wordsList);
    free(mergedData);
    free(ArgsArray);
    free(threadsID);
}
void* threadCounter(void* arg) {  //Function to run in each thread, counts words
    Args *arguments = (Args*)arg;
    int firstWord=arguments->firstWord;
    int lastWord=arguments->lastWord;
    int uniqueCount=arguments->uniqueCount;
    Cell *threadsWordsList=arguments->words;  //Changes to this can be seen in main through (ArgsArray+i)->words
    FILE *filePointer=fopen(FILE_NAME,"r");
    if (filePointer==NULL) {
        printf("ERROR === FILE OPEN FAILED\n");
        exit(0);
    }
    char str[WORD_MAXSIZE];
    char ch;
    int wordIndex=0;
    int j=0;
    int flagInsert = 0;  //If flag 0 do not insert if 1 insert because the process is reading words in range
    while ((ch = fgetc(filePointer)) != EOF) {  //Read until EOF
        if (ch != ' ' && ch != '\n') {  //Check for a space or a newline(there is no new lines in the clean text we got via Ritaj but just in case)
            str[j++] = ch;
            continue; //Go to next char
        }
        if (j > 0) {  //If a space or new line check if a word is in str and insert it
            str[j] = '\0';  //Add the null terminator
            j = 0;  //Reset j index
            if (flagInsert==0 && wordIndex == firstWord) {  //If we got to the range set the flag
                flagInsert = 1;
            }
            if (flagInsert==1 && wordIndex<lastWord) {  //If the flag is set, and the word is not the last yet, then insert
                uniqueCount = multip_insertion(threadsWordsList, str, uniqueCount);  //this returns an incremented count, and the array gets modified
            }
            if (wordIndex >= lastWord) {  //If last word then exit the loop
                break;
            }
            wordIndex++;  //Increment the word index
        }
    }
    if (j > 0) {  //If we broke the while loop by achieving EOF, then str might have a word in it, check and insert
        str[j] = '\0';  //Add null terminator
        if (flagInsert==1 && wordIndex < lastWord) {  //Insert if we are in correct range
            uniqueCount = multip_insertion(threadsWordsList, str, uniqueCount);
        }
    }
    arguments->uniqueCount=uniqueCount;
    arguments->words=threadsWordsList;
    fclose(filePointer);  //Close file
    //printf ("done\n");
    pthread_exit(NULL);
}
//====================
//=== END OF CODE ====
//====================