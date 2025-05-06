#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_PANTRY_SIZE 100

typedef struct ingredient {
    char name[30];
} ingredient;

typedef enum {
    sweet,
    savoury,
} category;

typedef struct recipe {
    char recipename[50];
    ingredient ingredients[8];
    int numofingredients;
    category category;
    char link[150];
} recipe;

typedef struct ingredientNode {
    ingredient data;
    struct ingredientNode* left;
    struct ingredientNode* right;
} ingredientNode;

typedef struct dataset {
    recipe recipes[50];
    int numofrecipes;
    ingredientNode* sweetIngredientTreeRoot;
    ingredientNode* savouryIngredientTreeRoot;
} dataset;

typedef struct allergyNode {
    char name[30];
    struct allergyNode* next;
} allergyNode;

typedef struct {
    allergyNode* head;
} allergyList;

typedef struct matchingScore {
    int recipeIndex;
    int score;
} matchingScore;


void insertIngredient(ingredientNode** root, const ingredient* newIngredient) {
    if (*root == NULL) {
        
        root = (ingredientNode)malloc(sizeof(ingredientNode));
        (*root)->data = *newIngredient;
        (*root)->left = (*root)->right = NULL;
    } else {
      
        int compareResult = strcmp(newIngredient->name, (*root)->data.name);
        if (compareResult < 0) {
            insertIngredient(&((*root)->left), newIngredient);
        } else if (compareResult > 0) {
            insertIngredient(&((*root)->right), newIngredient);
        }
        
    }
}


void buildIngredientTree(dataset* myDataset, const recipe* currentRecipe) {
    for (int i = 0; i < currentRecipe->numofingredients; i++) {
        
        if (currentRecipe->category == sweet) {
            insertIngredient(&(myDataset->sweetIngredientTreeRoot), &(currentRecipe->ingredients[i]));
        } else {
            insertIngredient(&(myDataset->savouryIngredientTreeRoot), &(currentRecipe->ingredients[i]));
        }
    }
}


void printIngredientTree(const ingredientNode* root) {
    if (root != NULL) {
        printIngredientTree(root->left);
        printf("Ingredient: %s\n", root->data.name);
        printIngredientTree(root->right);
    }
}


void displayIngredientTree(dataset* myDataset, category choice) {
    switch (choice) {
        case sweet:
            printf("\nSweet Ingredients:\n");
            printIngredientTree(myDataset->sweetIngredientTreeRoot);
            break;
        case savoury:
            printf("\nSavoury Ingredients:\n");
            printIngredientTree(myDataset->savouryIngredientTreeRoot);
            break;
        default:
            printf("Invalid choice.\n");
    }
}
int searchIngredient(const ingredientNode* root, const char* ingredientName) {
    if (root == NULL) {
        return 0;  
    }

    int compareResult = strcmp(ingredientName, root->data.name);
    if (compareResult == 0) {
        return 1; 
    } else if (compareResult < 0) {
        return searchIngredient(root->left, ingredientName);
    } else {
        return searchIngredient(root->right, ingredientName);
    }
}







allergyNode* createAllergyNode(const char* name) {
    allergyNode* newNode = (allergyNode*)malloc(sizeof(allergyNode));
    strncpy(newNode->name, name, sizeof(newNode->name));
    newNode->next = NULL;
    return newNode;
}


void insertAllergy(allergyList* list, const char* name) {
    allergyNode* newNode = createAllergyNode(name);

    if (list->head == NULL) {
        list->head = newNode;
    } else {
        allergyNode* current = list->head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
}


int isAllergic(allergyList* allergies, const char* ingredientName) {
    allergyNode* current = allergies->head;
    while (current != NULL) {
        if (strcmp(current->name, ingredientName) == 0) {
            return 1; 
        }
        current = current->next;
    }
    return 0; 
}


void freeAllergyList(allergyList* allergies) {
    allergyNode* current = allergies->head;
    while (current != NULL) {
        allergyNode* next = current->next;
        free(current);
        current = next;
    }
    allergies->head = NULL;
}


void AllergyCheck(allergyList* allergies) {
    char response;

    printf("Do you have any allergies? (y/n): ");
    scanf(" %c", &response);

    while ((getchar()) != '\n');

    if (response == 'y') {
        char allergyInput[30];
        printf("Enter the name of the allergic ingredient (enter 'done' when finished):\n");

        do {
            printf("Allergic ingredient: ");
            fgets(allergyInput, sizeof(allergyInput), stdin);
            allergyInput[strcspn(allergyInput, "\n")] = '\0';

            if (strcmp(allergyInput, "done") != 0) {
                insertAllergy(allergies, allergyInput);
            }

        } while (strcmp(allergyInput, "done") != 0);
    }
}

int calculateMatchingScore(const recipe* currentRecipe, const ingredient* pantry, int pantrySize) {
    int score = 0;
    for (int i = 0; i < currentRecipe->numofingredients; i++) {
        for (int j = 0; j < pantrySize; j++) {
            if (strcasecmp(currentRecipe->ingredients[i].name, pantry[j].name) == 0) {
                score++;
                break;
            }
        }
    }
    return score;
}


void displayRecipesByMatchingScore(dataset* myDataset, const matchingScore* scores, int numRecipes) {
    printf("\nRecipies that you can consider with the ingredients in your pantry:\n");
    for (int i = 0; i < numRecipes; i++) {
        int recipeIndex = scores[i].recipeIndex;
        if (scores[i].score > 0) {
            printf("Recipe Name: %s, Matching Score: %d\n", myDataset->recipes[recipeIndex].recipename, scores[i].score);
            printf("Ingredients:\n");
            for (int j = 0; j < myDataset->recipes[recipeIndex].numofingredients; j++) {
                printf("- %s\n", myDataset->recipes[recipeIndex].ingredients[j].name);
            }
            printf("Link: %s\n", myDataset->recipes[recipeIndex].link);
            printf("\n");
            
        }
    }
}





int main() {
    FILE *the_file = fopen("C:\\Users\\abina\\Downloads\\platepalsdataset (1).csv", "r");
    if (the_file == NULL) {
        perror("Unable to open the file.");
        exit(1);
    }

    char line[200];
    dataset myDataset;  
    myDataset.numofrecipes = 0;  

    fgets(line, sizeof(line), the_file);

    while (fgets(line, sizeof(line), the_file)) {
        char *token;
        int tokenCount = 0;
        recipe *currentRecipe = &myDataset.recipes[myDataset.numofrecipes]; 

        token = strtok(line, ",");
        while (token != NULL) {
            if (strcmp(token, "NULL") != 0) {  
                switch (tokenCount) {
                case 0:
                        strncpy(currentRecipe->recipename, token, sizeof(currentRecipe->recipename));
                        break;
               
                case 1:
                    currentRecipe->category = (atoi(token) == 0) ? sweet : savoury;
                    break;
                case 2:
                    strncpy(currentRecipe->ingredients[0].name, token, sizeof(currentRecipe->ingredients[0].name));
                    break;
                case 3:
                    strncpy(currentRecipe->ingredients[1].name, token, sizeof(currentRecipe->ingredients[1].name));
                    break;
                case 4:
                    strncpy(currentRecipe->ingredients[2].name, token, sizeof(currentRecipe->ingredients[2].name));
                    break;
                case 5:
                    strncpy(currentRecipe->ingredients[3].name, token, sizeof(currentRecipe->ingredients[3].name));
                break;
                case 6:
                    strncpy(currentRecipe->ingredients[4].name, token, sizeof(currentRecipe->ingredients[4].name));
                break;
                case 7:
                strncpy(currentRecipe->ingredients[5].name, token, sizeof(currentRecipe->ingredients[5].name));
                break;
                case 8:
                strncpy(currentRecipe->ingredients[6].name, token, sizeof(currentRecipe->ingredients[6].name));
                break;
              
                    default:
                        strncpy(currentRecipe->link, token, sizeof(currentRecipe->link));
                        break;
                }

                if (tokenCount >= 2 && tokenCount <= 7 && currentRecipe->numofingredients < 8) {
                    strncpy(currentRecipe->ingredients[currentRecipe->numofingredients].name, token, sizeof(currentRecipe->ingredients[0].name));
                    currentRecipe->numofingredients++;
                }
            }

            tokenCount++;
            token = strtok(NULL, ",");
        }

        
        for (int i = currentRecipe->numofingredients; i < 8; i++) {
            currentRecipe->ingredients[i].name[0] = '\0'; 
        }

        myDataset.numofrecipes++;
    }

    fclose(the_file);  

   
    for (int i = 0; i < myDataset.numofrecipes; i++) {
        printf("Recipe Name: %s\n", myDataset.recipes[i].recipename);
        printf("Number of Ingredients: %d\n", myDataset.recipes[i].numofingredients);
        printf("Category: %s\n", (myDataset.recipes[i].category == sweet) ? "Sweet" : "Savoury");

        for (int j = 0; j < myDataset.recipes[i].numofingredients; j++) {
            printf("Ingredient %d: %s\n", j + 1, myDataset.recipes[i].ingredients[j].name);
        }

        printf("Link: %s\n", myDataset.recipes[i].link);
        printf("\n");
    }

  
    myDataset.sweetIngredientTreeRoot = NULL;
    myDataset.savouryIngredientTreeRoot = NULL;

   
    for (int i = 0; i < myDataset.numofrecipes; i++) {
        buildIngredientTree(&myDataset, &(myDataset.recipes[i]));
    }

    int userChoice;
    category userCategory;

    printf("Choose a category:\n");
    printf("1. Sweet\n");
    printf("2. Savoury\n");
    printf("Enter your choice (1 or 2): ");
    scanf("%d", &userChoice);

    while ((getchar()) != '\n');
    
   
    userCategory = (userChoice == 1) ? sweet : ((userChoice == 2) ? savoury : -1);

    if (userCategory != -1) {
      
        displayIngredientTree(&myDataset, userCategory);
    } else {
        printf("Invalid choice.\n");
    }

    myDataset.sweetIngredientTreeRoot = NULL;
    myDataset.savouryIngredientTreeRoot = NULL;

   
    for (int i = 0; i < myDataset.numofrecipes; i++) {
        buildIngredientTree(&myDataset, &(myDataset.recipes[i]));
    }

    
    ingredient pantry[MAX_PANTRY_SIZE];
    int pantrySize = 0;

        
    
    char userResponse;
    char newIngredientName[30];

    do {
    printf("Enter the name of the ingredient to add to the pantry: ");
    fgets(newIngredientName, sizeof(newIngredientName), stdin);
    newIngredientName[strcspn(newIngredientName, "\n")] = '\0';  

    
    for (int i = 0; newIngredientName[i]; i++) {
        newIngredientName[i] = tolower(newIngredientName[i]);
    }

    int isInPantry = 0;

   
    for (int i = 0; i < pantrySize; i++) {
        char pantryIngredientLower[30];
        
        for (int j = 0; pantry[i].name[j]; j++) {
            pantryIngredientLower[j] = tolower(pantry[i].name[j]);
        }
        pantryIngredientLower[strcspn(pantryIngredientLower, "\n")] = '\0'; 

        if (strcmp(newIngredientName, pantryIngredientLower) == 0) {
            isInPantry = 1;
            break;
        }
    }

    if (isInPantry) {
        
        printf("Ingredient already in the pantry: %s\n", newIngredientName);
    } else {
        int isInSweetTree = 0;
        int isInSavouryTree = 0;

        if (userCategory == sweet) {
            isInSweetTree = searchIngredient(myDataset.sweetIngredientTreeRoot, newIngredientName);
        } else if (userCategory == savoury) {
            isInSavouryTree = searchIngredient(myDataset.savouryIngredientTreeRoot, newIngredientName);
        }

        if (isInSweetTree || isInSavouryTree) {
            
            strncpy(pantry[pantrySize].name, newIngredientName, sizeof(pantry[pantrySize].name));
            pantrySize++;
        } else {
            
            printf("Invalid ingredient: %s\n", newIngredientName);
        }
    }

    printf("Do you want to add another ingredient to the pantry? (y/n): ");
    scanf(" %c", &userResponse);  

    while ((getchar()) != '\n');

} while (userResponse == 'y' && pantrySize < MAX_PANTRY_SIZE);



    
    printf("\nPantry contents:\n");
    for (int i = 0; i < pantrySize; i++) {
        printf("%d. %s\n", i + 1, pantry[i].name);
    }


 allergyList allergies;
    allergies.head = NULL;

    AllergyCheck(&allergies);

   
    for (int i = 0; i < pantrySize; i++) {
        if (isAllergic(&allergies, pantry[i].name)) {
            printf("Removing allergic ingredient from pantry: %s\n", pantry[i].name);
           
            for (int j = i; j < pantrySize - 1; j++) {
                pantry[j] = pantry[j + 1];
            }
            pantrySize--;
            i--; 
        }
    }

   
    printf("\nUpdated Pantry contents:\n");
    for (int i = 0; i < pantrySize; i++) {
        printf("%d. %s\n", i + 1, pantry[i].name);
    }

    freeAllergyList(&allergies);

   
matchingScore scores[MAX_PANTRY_SIZE];

   
    for (int i = 0; i < myDataset.numofrecipes; i++) {
        scores[i].recipeIndex = i;
        scores[i].score = calculateMatchingScore(&(myDataset.recipes[i]), pantry, pantrySize);
    }

    
    for (int i = 0; i < myDataset.numofrecipes - 1; i++) {
        for (int j = i + 1; j < myDataset.numofrecipes; j++) {
            if (scores[j].score > scores[i].score) {
                
                matchingScore temp = scores[i];
                scores[i] = scores[j];
                scores[j] = temp;
            }
        }
    }

    
    displayRecipesByMatchingScore(&myDataset, scores, myDataset.numofrecipes);
    
    return 0;


}
