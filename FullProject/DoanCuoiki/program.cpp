#include "stdafx.h"
#include <Windows.h>
#include "createMetadata.h"
#include "manageFiles.h"

int getChoice()
{
    system("cls");
    printf("\n\t\t\t\tSEARCH ENGINE\n");
    printf("(1) Search\n");
    printf("(2) Add A File\n");
    printf("(3) Remove A File\n");
    printf("Your choice: ");
    int choice;
    scanf("%d", &choice);
    return choice;
}

void clearstdin()
{
    while (getchar() != '\n')
    {
    }
}

int startProgram()
{
    char stopwordFile[] = "stopword_one_word.txt";
    char indexFile[] = "index.txt";
    char metadataFile[] = "metadata.txt";
    //generateMetadata("train", stopwordFile, indexFile, metadataFile);
    while (true)
    {
        int choice = getChoice();
        switch (choice)
        {
        case 1:
        {
            break;
        }
        case 2:
        {
            char filePath[100];
            printf("File to add (absolute path): ");
            clearstdin();
            fgets(filePath, 100, stdin);
            filePath[strlen(filePath) - 1] = '\0';
            getValidPath(filePath);
            bool check = addFile(filePath, indexFile, metadataFile, stopwordFile, string());
            if (check == false)
                printf("\nINVALID FILE'S PATH!\n");
            break;
        }
        case 3:
        {
            char filePath[100];
            printf("File to remove: ");
            clearstdin();
            fgets(filePath, 100, stdin);
            filePath[strlen(filePath) - 1] = '\0';
            getValidPath(filePath);
            bool check = removeFile(filePath, indexFile);
            if (!check)
                printf("\nINVALID FILE'S PATH!\n");
            break;
        }
        default:
        {
            printf("\nINVALID CHOICE!\n");
            break;
        }
        }

        char c;
        printf("Continue? (y/n): ");
        scanf("%c", &c);
        if (tolower(c) != 'y')
            break;
    }
    return 0;
}