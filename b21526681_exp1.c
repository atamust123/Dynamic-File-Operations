#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>//don't forget to use remove words are number--

struct File
{
    char *fileName;
    char *message;
    int number;
    struct File *next;
};

void start(FILE *inputF,FILE *output);

struct File *insert(struct File *root, char *fileName, char *message, int number,FILE *output);

struct File *delete (struct File *root, char *fileName,FILE *output);

struct File *rEmove(struct File *root, char *fileName, int start, int length,FILE *output);

struct File *append(struct File *root, char *fileName, char *message,FILE *output);

struct File *replace(struct File *root, char *fileName, char *ow, char *nw,FILE *output);

void printAllFiles(struct File *root,FILE *output);                       //print -a
void printSameFiles(struct File *root, char *type,FILE *output);          //print -e
void printContent(struct File *root,FILE *output);                        //print -c
void printText(struct File *root, char *fileName,FILE *output);           //-n -t
void countSentence(struct File *root, char *fileName,FILE *output);       //-n -cs
void cwCommand(struct File *root, char *fileName, char *cw,FILE *output); //-n -cw
void freeList(struct File **root);

int main(int argc, char **argv)
{
    FILE *inputF = fopen(argv[1], "r");
    FILE *output = fopen("output.txt", "w+");
    if (!inputF)
    {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }
    start(inputF,output);
    fclose(inputF);
    fclose(output);
    return 0;
}

void start(FILE *inputF,FILE *output)
{
    struct File *fileDatas = NULL;

    int fileNumber = 0;
    char line[256], *token, command[10], parameter[2], fileName[256], message[256];
    while ((fgets(line, 256, inputF)))
    {
        if (line[strlen(line) - 1] == '\n') //print the input
            fprintf(output,"%s", line);
        else
            fprintf(output,"%s\n", line);
        token = strtok(line, " ");
        strcpy(command, token);
        if (strcmp(command, "create") == 0)
        {
            token = strtok(NULL, " "); //parameter -n or -t
            strcpy(parameter, token);
            if (strcmp(parameter, "-n") == 0)
            {
                token = strtok(NULL, " "); //filename
                if (strcmp(token, "-t") != 0)
                {
                    strcpy(fileName, token);
                    fileName[strlen(fileName)] = '\0';
                    token = strtok(NULL, " ");              //parameter
                    token = strtok(NULL, "");               //message
                    if (!token || strcmp(token, "\n") == 0) //last char is newline or space
                    {
                        //strcpy(message, "Empty File"); //printf("%s",message);
                        fileDatas = insert(fileDatas, fileName, "Empty File", (fileNumber + 1),output);
                    }
                    else
                    {
                        token = strtok(token, "\n"); //message
                        strcpy(message, token);
                        fileDatas = insert(fileDatas, fileName, message, (fileNumber + 1),output);
                    }
                }
                else
                {
                    fprintf(output,"File name not found.No file created.\n");
                    fileNumber--;
                }
            }
            else if (strcmp(parameter, "-t") == 0) //-t -n command
            {
                token = strtok(NULL, "-"); //read message
                if (*(token + 0) == 'n')   //empty file
                {
                    token = strtok(token, " ");    //parameter
                    token = strtok(NULL, "\n");    //file name
                    strcpy(message, "Empty File"); //if token null or startswith -n write empty file
                    if ((token == " " || token == NULL))
                    {
                        fprintf(output,"File name not found.No file created.\n");
                        fileNumber--;
                    }
                    else
                    {
                        fileDatas = insert(fileDatas, token, message, (fileNumber + 1),output);
                    }
                }
                else
                { //read message
                    strcpy(message, token);
                    message[strlen(message) - 1] = '\0';
                    token = strtok(NULL, " ");  //parameter
                    token = strtok(NULL, " \n"); //file name
                    //
                    if (token == " " || token == NULL) //if file name was not entered.
                    {
                        fprintf(output,"File name not found.No file created.\n");
                        fileNumber--;
                    }
                    else
                    {
                        strcpy(fileName, token);
                        fileDatas = insert(fileDatas, fileName, message, (fileNumber + 1),output);
                    }
                }
            }
            fileNumber++;
        }
        else if (strcmp(command, "delete") == 0)
        {
            token = strtok(NULL, " ");  //parameter
            token = strtok(NULL, " \n"); //filename
            if (token == NULL || token == " ")
            {
                fprintf(output,"NO file name entered.\n");
            }
            else
            {
                strcpy(fileName, token);
                fileDatas = delete (fileDatas, fileName,output);
            }
        }
        else if (strcmp(command, "remove") == 0)
        {
            token = strtok(NULL, " "); //-n
            token = strtok(NULL, " "); //fileName
            if (strcmp(command, "-s") == 0)
            {
                fprintf(output,"No filename entered.\n");
            }
            else
            {
                strcpy(fileName, token);

                token = strtok(NULL, " "); //-s
                token = strtok(NULL, " "); //startindex

                int startIndex = atoi(token);
                token = strtok(NULL, " ");  //-l command
                token = strtok(NULL, "\n"); //length
                int length = atoi(token);
                fileDatas = rEmove(fileDatas, fileName, startIndex, length,output);
            }
        }
        else if (strcmp(command, "append") == 0)
        {
            token = strtok(NULL, " ");    //parameter
            if (strcmp(token, "-n") == 0) //-n -t command
            {
                token = strtok(NULL, " ");
                if (strcmp(token, "-t") == 0)
                {
                    fprintf(output,"No filename entered.\n");
                }
                else
                {
                    strcpy(fileName, token);
                    token = strtok(NULL, " ");  //parameter -t
                    token = strtok(NULL, "\n"); //message
                    if (token == NULL || (*(token + 0) == ' '))
                    {
                        fprintf(output,"No message entered.\n");
                    }
                    else
                    {
                        strcpy(message, token);
                        fileDatas = append(fileDatas, fileName, message,output);
                    }
                }
            }
            else if (strcmp(token, "-t") == 0) //-t -n command
            {
                token = strtok(NULL, "-"); //message
                if (*(token + 0) == 'n')
                {
                    fprintf(output,"No message entered.\n");
                }
                else
                {
                    strcpy(message, token);
                    message[strlen(message) - 1] = '\0';
                    token = strtok(NULL, " ");  //parameter -n
                    token = strtok(NULL, " \n"); //filename
                    strcpy(fileName,token);
                    if (token == NULL || (token == " "))
                    {
                        fprintf(output,"No filename entered.\n");
                    }
                    else
                        fileDatas = append(fileDatas, token, message,output);
                }
            }
        }
        else if (strcmp(command, "replace") == 0)
        {
            char ow[256];
            char nw[256];
            int i;
            int everythinkISokay = 1;
            for (i = 0; i < 3; i++)
            {
                token = strtok(NULL, " "); //parameter
                if (strcmp(token, "-n") == 0)
                {
                    token = strtok(NULL, " \n");
                    if ((token == NULL) || (strcmp(token, "-ow") == 0) || (strcmp(token, "-nw") == 0))
                    {
                        fprintf(output,"File not found.\n");
                        everythinkISokay = 0;
                        break;
                    }
                    else
                        strcpy(fileName, token);
                }
                else if (strcmp(token, "-nw") == 0)
                {
                    token = strtok(NULL, " \n");
                    if ((token == NULL) || (strcmp(token, "-n") == 0) || (strcmp(token, "-ow") == 0))
                    {
                        fprintf(output,"No new word found.\n");
                        everythinkISokay = 0;
                        break;
                    }
                    else
                    {
                        strcpy(nw, token);
                    }
                }
                else if (strcmp(token, "-ow") == 0)
                {
                    token = strtok(NULL, "\n "); //before the change word
                    if ((token == NULL) || (strcmp(token, "-n") == 0) || (strcmp(token, "-nw") == 0))
                    {
                        fprintf(output,"No old word found.\n");
                        everythinkISokay = 0;
                        break;
                    }
                    else
                        strcpy(ow, token);
                }
            }
            if (everythinkISokay)
            {                                                     //if there is no problem execute
                fileDatas = replace(fileDatas, fileName, ow, nw,output); //message has changed
            }
        }
        else if (strcmp(command, "print") == 0)
        {
            token = strtok(NULL, " \n");
            if (strcmp(token, "-a") == 0)
            {
                if (fileDatas)
                    printAllFiles(fileDatas,output);
                else
                    fprintf(output,"There is no file.\n");
            }
            else if (strcmp(token, "-e") == 0)
            {
                token = strtok(NULL, " \n");
                if (token == NULL || token == " ")
                {
                    fprintf(output,"No extension entered.\n");
                }
                else
                {
                    char type[256];
                    strcpy(type, token);
                    type[strlen(type)] = '\0';
                    printSameFiles(fileDatas, type,output);
                }
            }
            else if (strcmp(token, "-c") == 0)
            {
                if (fileDatas)
                    printContent(fileDatas,output);
                else
                    fprintf(output,"There is no file.\n");
            }
            else if (strcmp(token, "-n") == 0)
            {
                token = strtok(NULL, " ");
                if ((strcmp(token, "-t") == 0) || (strcmp(token, "-cw") == 0) || (strcmp(token, "-cs") == 0) || (strcmp(token, "-t\n") == 0) || (strcmp(token, "-cw\n") == 0) || (strcmp(token, "-cs\n") == 0))
                { //if  file name was not entered
                    fprintf(output,"Filename not found.\n");
                }
                else
                {
                    strcpy(fileName, token);
                    token = strtok(NULL, " \n");

                    if (strcmp(token, "-t") == 0)
                    {
                        printText(fileDatas, fileName,output);
                    }
                    else if (strcmp(token, "-cs") == 0)
                    {
                        countSentence(fileDatas, fileName,output);
                    }
                    else if (strcmp(token, "-cw") == 0)
                    {
                        token = strtok(NULL, " \n"); //cw word
                        if (token == NULL)
                        {
                            fprintf(output,"No cw entered.\n");
                        }
                        else
                            cwCommand(fileDatas, fileName, token,output);
                    }
                }
            }
        }else{
            fprintf(output,"Invalid Command.\n");
        }
    }
    freeList(&fileDatas);
    return;
}

void freeList(struct File **root)
{
    struct File *current = *root;
    struct File *next;

    while (current != NULL)
    {
        next = current->next;
        free(current->message);
        free(current->fileName);
        free(current);
        current = next;
    }
    *root = NULL;
}

void printAllFiles(struct File *root,FILE *output) //print -a
{
    struct File *iter = root;
    while (iter != NULL)
    {
        fprintf(output,"Filename %d: %s\n", iter->number, iter->fileName);
        if (iter->next != NULL)
            iter = iter->next;
        else
            return;
    }
    return;
}
void printSameFiles(struct File *root, char *type,FILE *output)
{ //-e
    struct File *iter = root;
    int i = 0, j = 0;
    while (iter != NULL)
    {
        i = 0;
        j = 0;
        while (iter->fileName[i] != '.')
        {
            i++;
        }
        while (iter->fileName[i + 1] != '\0' && iter->fileName[i + 1] == type[j])
        {
            i++;
            j++;
        }
        if (j == strlen(type))
        {
            i = 0;
            fprintf(output,"Filename %d: ", iter->number);
            while (iter->fileName[i] != '.')
            {
                fprintf(output,"%c", iter->fileName[i++]);
            }
            fprintf(output," \n");
            fprintf(output,"Text: %s\n", iter->message);
        }
        if (iter->next != NULL)
            iter = iter->next;
        else
            return;
    }
}
void printContent(struct File *root,FILE *output)
{ //print -c
    struct File *iter = root;

    while (iter != NULL)
    {
        fprintf(output,"Num: %d\n", iter->number);
        fprintf(output,"Name: ");
        int i = 0;
        while (iter->fileName[i] != '.')
        {
            fprintf(output,"%c", iter->fileName[i++]);
        }
        fprintf(output,"\n");
        fprintf(output,"Text: %s\n", iter->message);
        if (iter->next)
            iter = iter->next;
        else
            return;
    }
}
void printText(struct File *root, char *fileName,FILE *output)
{ //print ”-n” and ”-t”
    struct File *iter = root;
    while (iter != NULL && iter->next != NULL && strcmp(iter->fileName, fileName) != 0)
        iter = iter->next;
    if (!iter)
        return;
    if (strcmp(iter->fileName, fileName) == 0)
        fprintf(output,"Text: %s \n", iter->message);
    else
        fprintf(output,"Filename not found.\n");
    return;
}
void countSentence(struct File *root, char *fileName,FILE *output)
{ //-n -cs
    struct File *iter = root;
    int i, counter = 0;
    while (iter != NULL && iter->next != NULL && strcmp(iter->fileName, fileName) != 0)
        iter = iter->next;
    if (!iter)
        return;
    if (strcmp(iter->fileName, fileName) == 0)
    {
        for (i = 0; i < strlen(iter->message); i++)
            if (iter->message[i] == '.' || iter->message[i] == '?' || iter->message[i] == '!')
                counter++;
        fprintf(output,"Number Of Sentences : %d\n", counter);
    }
    else
    {
        fprintf(output,"FileName not found.\n");
    }
}
void cwCommand(struct File *root, char *fileName, char *cw,FILE *output)
{ // -n -cw
    struct File *iter = root;
    int i = 0, counter = 0;
    while (iter != NULL && iter->next != NULL && strcmp(iter->fileName, fileName) != 0)
        iter = iter->next;
    if (!iter)
        return;
    if (strcmp(iter->fileName, fileName) == 0)
    {
        fprintf(output,"Text: %s \n", iter->message);
        while (i < strlen(iter->message))
        {
            if (strstr((iter->message + i), cw) == (iter->message + i))
            {
                counter++;
                i = strlen(cw) + i;
            }
            else
                i++;
        }
        fprintf(output,"Number Of Occurrence of \"%s\" : %d \n", cw, counter);
    }
    else
    { //if the linked list do not contain the file
        fprintf(output,"FileName not found\n");
    }
}

struct File *replace(struct File *root, char *fileName, char *ow, char *nw,FILE *output)
{
    struct File *temp = root;                                     //create iterator to find wanted file
    while (temp != NULL && strcmp(temp->fileName, fileName) != 0) //find the wanted Node
        temp = temp->next;
    if (!temp)
        return root;
    if (strcmp(temp->fileName, fileName) == 0)
    {
        char *tempString = malloc(strlen(temp->message) + 1);
        strcpy(tempString, temp->message);
        int i = 0, owCounter = 0;
        while (i < strlen(tempString))
        {
            if (strstr(&(tempString[i]), ow) == &(tempString[i]))//control each char 
            {
                owCounter++;
                i = strlen(ow) + i; //move old word length times
            }
            else
                i++; //next word
        }
        char *newMessage = (char *)malloc(sizeof(char) * ((strlen(nw) - strlen(ow)) * 2 + i + 1)); 
        i = 0;
        while (*tempString)
        {
            if (strstr((tempString), ow) == tempString) 
            {
                strcpy(&newMessage[i], nw);
                i += strlen(nw);
                tempString += strlen(ow);
            }
            else
            {
                newMessage[i++] = *tempString++;
            }
        }
        newMessage[i] = '\0';
        temp->message = realloc(temp->message, strlen(newMessage) + 1);//new message after replace
        strcpy(temp->message, newMessage);

        free(newMessage); //no need anymore, also tempstring is already null because i changed its adresses 
        return root;
    }
    else
    {
        fprintf(output,"File not found.\n");
    }
}

struct File *append(struct File *root, char *fileName, char *message,FILE *output)
{
    if (!root) //if root is empty
        return root;
    struct File *temp = root;
    while (temp != NULL && strcmp(temp->fileName, fileName) != 0) //iterate until you find
        temp = temp->next;
    if (!temp){
        fprintf(output,"File not found.\n");
        return root;
    }
        
    if (strcmp(temp->fileName, fileName) == 0)
    {
        char *result = malloc(sizeof(char) * (strlen(temp->message) + strlen(message) + 1));
        //dont worry about eos is is stated
        strcpy(result, temp->message);

        strcat(result, message);
        temp->message = (char *)realloc(temp->message, sizeof(char) * (strlen(result) + 1));
        strcpy(temp->message, result); //message is changed
        free(result);
    }
    return root;
}

struct File *rEmove(struct File *root, char *fileName, int start, int length,FILE *output)
{
    if (!root) //if root is empty
        return root;
    struct File *temp = root;
    char message[256]; //temporary array

    while (temp != NULL && strcmp(temp->fileName, fileName) != 0)
        temp = temp->next;
    if (!temp)
    {
        fprintf(output,"File not found.\n");
        return root;
    }

    if (strcmp(temp->fileName, fileName) == 0)
    { //if you find file change it
        if ((length + start) > strlen(temp->message))
        {
            fprintf(output,"Owerflow.No file message part is removed.\n");
            return root;
        }
        int i = 0;
        for (; i < start; i++)
        {
            message[i] = temp->message[i]; //add until start index
        }
        for (; i < strlen(temp->message) - length; i++)
        {
            message[i] = temp->message[length + i]; //pass length times
        }
        message[i] = '\0';
        temp->message = (char *)realloc(temp->message, sizeof(char) * (strlen(message) + 1));
        strcpy(temp->message, message); //message is changed
        //temp->message[strlen(temp->message)-1] = '\0'; think that later
    }
    return root;
}

struct File *delete (struct File *root, char *fileName,FILE *output)
{

    if (!root)
    {
        return root;
    }
    struct File *temp = root, *prev;

    if (strcmp(temp->fileName, fileName) == 0)
    { //delete first element
        root = temp->next;
        free(temp->message);
        free(temp->fileName);
        free(temp);
        return root;
    }

    while (temp != NULL && strcmp(temp->fileName, fileName) != 0) //iterate until the wanted file is found
    {
        prev = temp;
        temp = temp->next;
    }
    if (!temp)
    {
        fprintf(output,"File not found.No file deleted.\n");
        return root;
    }
    prev->next = temp->next;//if file found.
    free(temp->message);
    free(temp->fileName);
    free(temp);
    return root;
}

struct File *insert(struct File *root, char *fileName, char *message, int number,FILE *output)
{
    if (!root) //if root is empty create first node
    {
        root = malloc(sizeof(struct File));
        root->next = NULL;

        root->fileName = (char *)malloc(strlen(fileName) + 1);
        strcpy(root->fileName, fileName);
        root->fileName[strlen(root->fileName)] = '\0';
        root->message = (char *)malloc(sizeof(char) * (strlen(message) + 1));
        strcpy(root->message, message);
        root->message[strlen(root->message)] = '\0';
        root->number = number;
        return root;
    }
    struct File *iter = root;
    while (iter->next != NULL) //go to the end of linked list to add new node
    {
        if (strcmp(iter->fileName,fileName)==0){
            fprintf(output,"The filename \"%s\" is exist, use append function to add something.\n",fileName);
            return root;
        }else
            iter = iter->next;
    }
    struct File *temp = malloc(sizeof(struct File)); //this will be new node at the end of linked list
    temp->next = iter->next;
    iter->next = temp;
    temp->number = number;
    temp->fileName = (char *)malloc(sizeof(char) * (strlen(message) + 1));
    strcpy(temp->fileName, fileName);
    temp->fileName[strlen(temp->fileName)] = '\0';
    temp->message = (char *)malloc(sizeof(char) * (strlen(message) + 1));
    strcpy(temp->message, message);

    temp->message[strlen(temp->message)] = '\0'; //last char is end of string
    return root;
}