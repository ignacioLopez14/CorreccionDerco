//Proyecto Derco (Coreccion)    29/06/2023
//Integrantes grupo: Fernando Bascunan, Gabriel Lopez, Ignacio Lopez

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRINGBUFFERSIZE 100

enum Menu {
    mADD = 1,
    mDELETE,
    mFIND,
    mMODIFY1,
    mPRINT,
    mSPECIALOPTION
};

enum Find {
    fDELETE = 1,
    fMODIFY,
    fDATA_STRUCTURE1,
    fDATA_STRUCTURE2
};

enum ProductType {
    CAR,
    CAR_SPARE_PART,
    ACCESSORY,
    END
};

//Struct General Derco
struct Derco {
    struct ZoneList* zoneList;      //Nodo encapsulamiento
    struct BrandNode* brands;
} *derco;

//Struct para las Marcas (Lista circular doblemente enlazada)
struct Brand {
    char** name;
    float status;
    float productTravelTime;
    struct ProductNode *stock;
};
struct BrandNode {
    struct Brand *brand;
    struct BrandNode *next;
    struct BrandNode *prev;
};
struct BrandNameNode {
    char* brandName;
    struct BrandNameNode* next;
}; //Lista simple, utilizada en funcion adicional

//Struct para los Clientes (Arbol binario de busqueda)
struct Customer {
    int ID;
    char* name;
    int age;
    struct Purchases *purchases;    //Nodo encapsulamiento
};
struct CustomerNode {
    struct Customer *customer;
    struct CustomerNode *left;
    struct CustomerNode *right;
};

//Struct para los Centros de Distribucion
struct Warehouse {
    int numberOfProducts;
    float cbMetersMaxCapacity;
    float cbMetersSpaceUsed;
    struct ProductNode *products;
};
struct DistributionCenter {
    int ID;
    char *address;
    int numberOfEmployes;
    struct Warehouse *Warehouse;     //Nodo encapsulamiento
    struct CustomerNode *customers;
};
struct DistributionCenterNode {
    struct DistributionCenter* distributionCenter;
    struct DistributionCenterNode *next;
    struct DistributionCenterNode *prev;
};

//Struct Para los productos (Arbol binario de busqueda)
struct ProductSharedData {
    int ID;
    enum ProductType type;
    int recalls;
    char* name;
    char* description;
    char** brandName;
    float cbMetersSpacePerUnit;
};
struct Product {
    struct ProductSharedData *sharedData;
    int amount;
    float price;
};
struct ProductNode {
    struct Product *product;
    struct ProductNode *left,*right;
};
struct ProductIDNode {              //Lista utilizada en funcion de buscar producto mas vendido
    struct ProductSharedData* ID;
    struct ProductIDNode* next;
};  //Lista simple, utilizada en funcion adicional

//Struct para las Compras
struct Purchase {
    int ID;
    struct ProductSharedData* productSharedData;
    int amount;
    float boughtPrice;
};  //Vector
struct Purchases {              //Encapsulamiento
    int numberOfPurchaseProducts;
    int lenght;
    int indexFirstEmpty;
    struct Purchase **purchaseArray;
};

//Struct para cada Zona (Lista circular simple)
struct Zone {
    char *name;
    int numberOfHabitants;
    float habitantsAverageAge;
    struct DistributionCenterNode *distributionCenters;
};
struct ZoneNode{
    struct Zone *zone;
    struct ZoneNode *next;
};
struct ZoneList {       //Struct de encapsulamiento
    int lenght;
    struct ZoneNode *tail;
    struct ZoneNode *zones;
};

//Definicion de struct Globales
struct BrandNode *actualBrand;
struct DistributionCenterNode *actualDistributionCenter;
//**********************************************************************************************************************

//Funciones de entrada de datos
int EnterInteger(char* text) {
    int output;
    printf("[ENTERO POSITIVO] ");
    printf("%s",text);
    if (!scanf("%d", &output) || output < 0) {
        printf("\nValor invalido.\n");
        output = -1;
    }
    printf("\n");
    return output;
}
char* EnterText(char* text) {
    char* output;
    printf("[MAX %d CARACTERES] ", STRINGBUFFERSIZE);
    printf("%s",text);
    output = (char*)malloc(sizeof(char) * STRINGBUFFERSIZE);
    if (!scanf(" %[^\n]", output)) {
        printf("\nValor invalido.\n");

        output = NULL;
    }
    printf("\n");
    return output;
}
float EnterFloatingPoint(char* text) {
    float output;
    printf("[PUNTO FLOTANTE POSITIVO] ");
    printf("%s",text);
    if (!scanf("%f", &output) || output < 0) {
        printf("\nValor invalido.\n");

        output = -1;
    }
    printf("\n");

    return output;
}

//Funciones Productos
struct ProductSharedData* CreateProductSharedData(int ID, enum ProductType type, char* name, char* description, char** brandName, float spacePerUnit) {
    struct ProductSharedData* sharedData;
    sharedData = (struct ProductSharedData*)malloc(sizeof(struct ProductSharedData));

    if (!sharedData) { return NULL; }

    sharedData->ID = ID;
    sharedData->type = type;
    sharedData->recalls = 0;

    sharedData->name = (char*)malloc(sizeof(char) * strlen(name));
    if (!sharedData->name) { return NULL; }
    strcpy(sharedData->name, name);

    sharedData->description = (char*)malloc(sizeof(char) * strlen(description));
    if (!sharedData->description) { return NULL; }
    strcpy(sharedData->description, description);

    sharedData->brandName = brandName;
    sharedData->cbMetersSpacePerUnit = spacePerUnit;

    return sharedData;
}
struct Product* CreateProduct(struct ProductSharedData* sharedData, int amount, float price) {
    struct Product* product;
    product = (struct Product*)malloc(sizeof(struct Product));

    if (!product) { return NULL; }

    product->sharedData = sharedData;
    product->price = price;
    product->amount = amount;

    return product;
}
struct ProductNode* CreateProductNode(struct Product* product) {
    struct ProductNode* node;
    node = (struct ProductNode*)malloc(sizeof(struct ProductNode));

    if (!node) { return NULL; }

    node->product = product;
    node->left  = NULL;
    node->right = NULL;

    return node;
}
int AddProduct(struct ProductNode** root, struct Product* product) {
    if (!(*root)) {
        *root = CreateProductNode(product);
        return (*root) ? 0 : 1;
    }

    if (product->sharedData->ID < (*root)->product->sharedData->ID) { return AddProduct(&((*root)->left), product); }
    if ((*root)->product->sharedData->ID == product->sharedData->ID) { return 1; }
    return AddProduct(&((*root)->right), product);
}
int DeleteProduct(struct ProductNode** root, int ID) {
    if (!(*root)) { return 1; }

    if (ID < (*root)->product->sharedData->ID) { return DeleteProduct(&((*root)->left), ID); }

    if ((*root)->product->sharedData->ID == ID) {
        if ((*root)->left && (*root)->right) {
            struct ProductNode** parentLeftmostTraverse = &(*root)->right;

            while ((*parentLeftmostTraverse)->left) { parentLeftmostTraverse = &(*parentLeftmostTraverse)->left; }

            (*root)->product = (*parentLeftmostTraverse)->product;
            *parentLeftmostTraverse = (*parentLeftmostTraverse)->right;
        }
        else { *root = ((*root)->left) ? (*root)->left : (*root)->right; }
        return 0;
    }

    return DeleteProduct(&((*root)->right), ID);
}
struct ProductNode* FindProductByID(struct ProductNode* root, int ID) {
    if (!root) { return NULL; }

    if (ID < root->product->sharedData->ID) { return FindProductByID(root->left, ID); }
    if (root->product->sharedData->ID == ID) { return root; }
    return FindProductByID(root->right, ID);
}
//Funciones modificar datos Producto
int ChangeProductType(struct ProductNode* nodeToChange, enum ProductType type) {
    if (!nodeToChange) { return 1; }
    nodeToChange->product->sharedData->type = type;
    return 0;
}
int ChangeProductRecalls(struct ProductNode* nodeToChange, int recalls) {
    if (!nodeToChange) { return 1; }
    nodeToChange->product->sharedData->recalls = recalls;
    return 0;
}
int ChangeProductName(struct ProductNode* nodeToChange, char* name) {
    if (!nodeToChange) { return 1; }

    nodeToChange->product->sharedData->name = (char*)malloc(sizeof(char) * strlen(name));
    if (!nodeToChange->product->sharedData->name) { return 1; }
    strcpy(nodeToChange->product->sharedData->name, name);
    return 0;
}
int ChangeProductDescription(struct ProductNode* nodeToChange, char* description) {
    if (!nodeToChange) { return 1; }

    nodeToChange->product->sharedData->description = (char*)malloc(sizeof(char) * strlen(description));
    if (!nodeToChange->product->sharedData->description) { return 1; }
    strcpy(nodeToChange->product->sharedData->description, description);
    return 0;
}
int ChangeProductPrice(struct ProductNode* nodeToChange, float price) {
    if (!nodeToChange || price == (float)(-1)) { return 1; }
    nodeToChange->product->price = price;
    return 0;
}
int ChangeProductSpacePerUnit(struct ProductNode* nodeToChange, float spacePerUnit) {
    if (!nodeToChange || spacePerUnit == (float)(-1)) { return 1; }
    nodeToChange->product->sharedData->cbMetersSpacePerUnit = spacePerUnit;
    return 0;
}
int ChangeProductAmount(struct ProductNode* nodeToChange, int amount) {
    if (!nodeToChange || amount == -1) { return 1; }
    nodeToChange->product->amount = amount;
    return 0;
}
//Modificar datos Almacen
int ChangeWarehouseMaxCapacity(struct Warehouse* Warehouse, float maxCapacity) {
    if (!Warehouse || maxCapacity < Warehouse->cbMetersSpaceUsed || maxCapacity == -1) { return 1; }
    Warehouse->cbMetersMaxCapacity = maxCapacity;
    return 0;
}
//Mostrar Almacen
void PrintProductSharedData(struct ProductSharedData* productSharedData) {
    printf("ID = %d\nTipo de producto = ", productSharedData->ID);

    switch (productSharedData->type) {
        case CAR: printf("Vehiculo\nRecalls = %d", productSharedData->recalls); break;
        case CAR_SPARE_PART: printf("Pieza de repuesto"); break;
        case ACCESSORY: printf("Accesorio"); break;
    }

    printf("\nNombre = \"%s\"\nDescripcion = \"%s\"\nNombre de la marca = \"%s\"\nVolumen por unidad (m^3) = %.2f\n\n",
           productSharedData->name, productSharedData->description, *productSharedData->brandName, productSharedData->cbMetersSpacePerUnit);
}
void PrintProduct(struct Product* product) {
    PrintProductSharedData(product->sharedData);
    printf("Cantidad = %d\nPrecio = %.2f\n\n", product->amount, product->price);
}
void PrintProducts(struct ProductNode* root) {
    if (!root) { return; }

    PrintProducts(root->left);
    PrintProduct(root->product);
    PrintProducts(root->right);
}
void RecPrintProductIDs(struct ProductNode* root, int* number) {
    if (!root) { return; }

    RecPrintProductIDs(root->left, number);
    printf("%d. %d\n", (*number)++, root->product->sharedData->ID);
    RecPrintProductIDs(root->right, number);
}
void PrintProductIDs(struct ProductNode* root) {
    if (!root) { return; }
    int i = 0;

    printf("--------------------------------------------------\n\n");
    printf("ID DE LOS PRODUCTOS CREADOS:\n\n");
    RecPrintProductIDs(root, &i);
    printf("\n--------------------------------------------------\n\n");
}
void PrintWarehouse(struct Warehouse* Warehouse) {
    if (!Warehouse) { return; }

    printf("---------------------------------------------------------\n\n");
    printf("Numero de productos = %d\n\nCapacidad maxima (m^3) = %.2f\nEspacio usado (m^3) = %.2f\n\n",
           Warehouse->numberOfProducts, Warehouse->cbMetersMaxCapacity, Warehouse->cbMetersSpaceUsed);
    printf("---------------------------------------------------------\n\n");

    PrintProducts(Warehouse->products);
}

//Funciones compras
struct Purchase* CreatePurchase(int ID, struct ProductSharedData* productSharedData, int amount, float boughtPrice) {
    struct Purchase* purchase;
    purchase = (struct Purchase*)malloc(sizeof(struct Purchase));

    if (!purchase) { return NULL; }

    purchase->ID = ID;
    purchase->productSharedData = productSharedData;
    purchase->amount = amount;
    purchase->boughtPrice = boughtPrice;

    return purchase;
}
struct Purchases* CreatePurchases(int arrayLenght) {
    struct Purchases* purchases;
    int i;
    purchases = (struct Purchases*)malloc(sizeof(struct Purchases));

    if (!purchases) { return NULL; }

    purchases->numberOfPurchaseProducts = 0;

    purchases->lenght = arrayLenght;
    purchases->indexFirstEmpty = 0;
    purchases->purchaseArray = (struct Purchase**)malloc(sizeof(struct Purchase*) * arrayLenght);

    if (!purchases->purchaseArray) { return NULL; }

    for (i = 0; i < arrayLenght; i++) { purchases->purchaseArray[i] = NULL; }

    return purchases;
}
void Compact(struct Purchases* purchases, int indexStartToCompact) {
    int i;

    purchases->indexFirstEmpty--;

    for (i = indexStartToCompact; i < purchases->indexFirstEmpty; i++) {
        purchases->purchaseArray[i] = purchases->purchaseArray[i + 1];
    }

    purchases->purchaseArray[purchases->indexFirstEmpty] = NULL;
}
struct Purchase* FindPurchaseByID(struct Purchases* purchases, int ID) {
    int i;

    for (i = 0; i < purchases->indexFirstEmpty; i++) {
        if (purchases->purchaseArray[i]->ID == ID) { return purchases->purchaseArray[i]; }
    }

    return NULL;
}
int AddPurchase(struct Purchases* purchases, struct Purchase* purchase) {
    if (purchases->indexFirstEmpty >= purchases->lenght || FindPurchaseByID(purchases, purchase->ID)) { return 1; }

    purchases->numberOfPurchaseProducts += purchase->amount;
    purchases->purchaseArray[purchases->indexFirstEmpty++] = purchase;

    return 0;
}
int DeletePurchase(struct Purchases* purchases, struct Purchase* purchase) {
    int i;

    for (i = 0; i < purchases->indexFirstEmpty; i++) {
        if (purchases->purchaseArray[i] == purchase) {
            purchases->numberOfPurchaseProducts -= purchase->amount;
            Compact(purchases, i);
            return 0;
        }
    }
    return 1;
}
//Modificar datos Compra
int ChangePurchaseProductSharedData(struct Purchase* purchase, struct ProductSharedData* productSharedData) {
    if (!purchase) { return 1; }
    purchase->productSharedData = productSharedData;
    return 0;
}
int ChangePurchaseAmount(struct Purchase* purchase, int amount) {
    if (!purchase) { return 1; }
    purchase->amount = amount;
    return 0;
}
int ChangePurchaseBoughtPrice(struct Purchase* purchase, float boughtPrice) {
    if (!purchase) { return 1; }
    purchase->boughtPrice = boughtPrice;
    return 0;
}
//Imprimir Compra
void PrintPurchase(struct Purchase* purchase) {
    printf("Producto comprado:\n\n");
    PrintProductSharedData(purchase->productSharedData);
    printf("Numero de boleta de la compra = %d\nCantidad comprada = %d\nPrecio de compra = %.2f\n\n",
           purchase->ID, purchase->amount, purchase->boughtPrice);
}
void PrintPurchases(struct Purchases* purchases) {
    int i;

    printf("---------------------------------------------------------\n\n");
    printf("Numero de productos comprados = %d\n\nLongitud de arreglo = %d\nIndice pLibre arreglo = %d\n\n",
           purchases->numberOfPurchaseProducts, purchases->lenght, purchases->indexFirstEmpty);
    printf("---------------------------------------------------------\n\n");

    for (i = 0; i < purchases->indexFirstEmpty; i++) {
        PrintPurchase(purchases->purchaseArray[i]);
    }
}
void PrintPurchaseIDs(struct Purchases* purchases) {
    if (!purchases->purchaseArray[0]) { return; }

    int i;

    printf("--------------------------------------------------\n\n");
    printf("NUMERO DE BOLETA DE LAS COMPRAS CREADAS:\n\n");

    for (i = 0; i < purchases->indexFirstEmpty; i++) {
        printf("%d. %d\n", i, purchases->purchaseArray[i]->ID);
    }
    printf("\n--------------------------------------------------\n\n");
}

//Funciones Clientes
struct Customer* CreateCustomer(int ID, char* name, int age, int maxPurchases) {
    struct Customer* customer;
    customer = (struct Customer*)malloc(sizeof(struct Customer));

    if (!customer) { return NULL; }

    customer->ID = ID;

    customer->name = (char*)malloc(sizeof(char) * strlen(name));
    if (!customer->name) { return NULL; }
    strcpy(customer->name, name);

    customer->age = age;
    customer->purchases = CreatePurchases(maxPurchases);

    return customer;
}
struct CustomerNode* CreateCustomerNode(struct Customer* customer) {
    struct CustomerNode* node;
    node = (struct CustomerNode*)malloc(sizeof(struct CustomerNode));

    if (!node) { return NULL; }

    node->customer = customer;
    node->left = NULL;
    node->right = NULL;

    return node;
}
int AddCustomer(struct CustomerNode** root, struct Customer* customer) {
    if (!(*root)) {
        *root = CreateCustomerNode(customer);
        return (*root) ? 0 : 1;
    }

    if ((*root)->customer->ID == customer->ID) { return 1; }

    return (customer->ID < (*root)->customer->ID) ? AddCustomer(&((*root)->left), customer) : AddCustomer(&((*root)->right), customer);
}
int DeleteCustomer(struct CustomerNode** root, int ID) {
    if (!(*root)) { return 1; }

    if ((*root)->customer->ID == ID) {
        if ((*root)->left && (*root)->right) {
            struct CustomerNode** parentLeftmostTraverse = &(*root)->right;

            while ((*parentLeftmostTraverse)->left) { parentLeftmostTraverse = &(*parentLeftmostTraverse)->left; }

            (*root)->customer = (*parentLeftmostTraverse)->customer;
            *parentLeftmostTraverse = (*parentLeftmostTraverse)->right;
        } else { *root = ((*root)->left) ? (*root)->left : (*root)->right; }
        return 0;
    }

    return (ID < (*root)->customer->ID) ? DeleteCustomer(&((*root)->left), ID) : DeleteCustomer(&((*root)->right), ID);
}
struct CustomerNode* FindCustomerByID(struct CustomerNode* root, int ID) {
    if (!root || root->customer->ID == ID) { return root; }
    return (ID < root->customer->ID) ? FindCustomerByID(root->left, ID) : FindCustomerByID(root->right, ID) ;
}
//Modificar datos Cliente
int ChangeCustomerName(struct CustomerNode* nodeToChange, char* name) {
    if (!nodeToChange) { return 1; }
    nodeToChange->customer->name = (char*)malloc(sizeof(char) * strlen(name));
    if (!nodeToChange->customer->name) { return 1; }
    strcpy(nodeToChange->customer->name, name);
    return 0;
}
int ChangeCustomerAge(struct CustomerNode* nodeToChange, int age) {
    if (!nodeToChange || age == -1) { return 1; }
    nodeToChange->customer->age = age;
    return 0;
}
//Mostrar Clientes
void PrintCustomer(struct Customer* customer) {
    printf("ID = %d\nNombre = \"%s\"\nEdad = %d\n\n", customer->ID, customer->name, customer->age);
}
void PrintCustomers(struct CustomerNode* root) {
    if (!root) { return; }

    PrintCustomer(root->customer);
    PrintCustomers(root->left);
    PrintCustomers(root->right);
}
void RecPrintCustomerIDs(struct CustomerNode* root, int* number) {
    if (!root) { return; }

    printf("%d. %d\n", (*number)++, root->customer->ID);
    RecPrintCustomerIDs(root->left, number);
    RecPrintCustomerIDs(root->right, number);
}
void PrintCustomerIDs(struct CustomerNode* root) {
    if (!root) { return; }

    int i = 0;

    printf("--------------------------------------------------\n\n");
    printf("IDs DE LOS CLIENTES CREADOS:\n\n");
    RecPrintCustomerIDs(root, &i);
    printf("\n--------------------------------------------------\n\n");
}

//Funciones Centro Distribucion
struct Warehouse* CreateWarehouse(float maxCapacity) {
    struct Warehouse* Warehouse;
    Warehouse = (struct Warehouse*)malloc(sizeof(struct Warehouse));

    if (!Warehouse) { return NULL; }

    Warehouse->numberOfProducts = 0;
    Warehouse->cbMetersMaxCapacity = maxCapacity;
    Warehouse->cbMetersSpaceUsed = 0;
    Warehouse->products = NULL;

    return Warehouse;
}
struct DistributionCenter* CreateDistributionCenter(int ID, char* address, int numberOfEmployes, float WarehouseMaxCapacity) {
    struct DistributionCenter* distributionCenter;
    distributionCenter = (struct DistributionCenter*)malloc(sizeof(struct DistributionCenter));

    if (!distributionCenter) { return NULL; }

    distributionCenter->ID = ID;

    distributionCenter->address = (char*)malloc(sizeof(char) * strlen(address));
    if (!distributionCenter->address) { return NULL; }
    strcpy(distributionCenter->address, address);

    distributionCenter->numberOfEmployes = numberOfEmployes;
    distributionCenter->Warehouse = CreateWarehouse(WarehouseMaxCapacity);
    distributionCenter->customers = NULL;

    return distributionCenter;
}
struct DistributionCenterNode* CreateDistributionCenterNode(struct DistributionCenter* distributionCenter){
    struct DistributionCenterNode* node;
    node = (struct DistributionCenterNode*)malloc(sizeof(struct DistributionCenterNode));

    if (!node) { return NULL; }

    node->distributionCenter = distributionCenter;
    node->next = NULL;
    node->prev = NULL;

    return node;
}
struct DistributionCenterNode* FindDistributionCenterByID(struct DistributionCenterNode* distributionCenters, int ID) {
    struct DistributionCenterNode* traverse = distributionCenters->next;

    while (traverse) {
        if (traverse->distributionCenter->ID == ID) { return traverse; }
        traverse = traverse->next;
    }

    return NULL;
}
int AddDistributionCenter(struct DistributionCenterNode* distributionCenters, struct DistributionCenter* distributionCenter) {
    if (FindDistributionCenterByID(distributionCenters, distributionCenter->ID)) { return 1; }

    struct DistributionCenterNode *newNode, *traverse = distributionCenters;

    newNode = CreateDistributionCenterNode(distributionCenter);

    if (!newNode) { return 1; }

    while (traverse->next) { traverse = traverse->next; }

    traverse->next = newNode;
    newNode->prev = traverse;

    return 0;
}
int DeleteDistributionCenter(struct DistributionCenterNode* distributionCenters, struct DistributionCenterNode* nodeToDelete) {
    struct DistributionCenterNode* traverse = distributionCenters->next;

    while (traverse) {
        if (traverse == nodeToDelete) {
            traverse->prev->next = traverse->next;
            if (traverse->next) { traverse->next->prev = traverse->prev; }
            return 0;
        }
        traverse = traverse->next;
    }

    return 1;
}
//Modificar datos Centro de Distribucion
int ChangeDistributionCenterAddress(struct DistributionCenterNode* nodeToChange, char* address) {
    if (!nodeToChange) { return 1; }
    nodeToChange->distributionCenter->address = (char*)malloc(sizeof(char) * strlen(address));
    if (!nodeToChange->distributionCenter->address) { return 1; }
    strcpy(nodeToChange->distributionCenter->address, address);
    return 0;
}
int ChangeDistributionCenterNumberOfEmployes(struct DistributionCenterNode* nodeToChange, int numberOfEmployes) {
    if (!nodeToChange || numberOfEmployes == -1) { return 1; }
    nodeToChange->distributionCenter->numberOfEmployes = numberOfEmployes;
    return 0;
}
//Mostrar Centros de Distribucion
void PrintDistributionCenter(struct DistributionCenter* distributionCenter) {
    printf("ID = %d\nDireccion = \"%s\"\nNumero de empleados = %d\n\n",
           distributionCenter->ID, distributionCenter->address, distributionCenter->numberOfEmployes);
}
void PrintDistributionCenters(struct DistributionCenterNode* distributionCenters) {
    struct DistributionCenterNode *traverse = distributionCenters->next;

    while (traverse) {
        PrintDistributionCenter(traverse->distributionCenter);
        traverse = traverse->next;
    }
}
void PrintDistributionCenterIDs(struct DistributionCenterNode* distributionCenters) {
    if (!distributionCenters->next) { return; }

    struct DistributionCenterNode* traverse = distributionCenters->next;
    int i = 0;

    printf("--------------------------------------------------\n\n");
    printf("IDs DE LOS CENTROS DE DISTRIBUCION CREADOS:\n\n");

    while (traverse) {
        printf("%d. %d\n", i++, traverse->distributionCenter->ID);
        traverse = traverse->next;
    }
    printf("\n--------------------------------------------------\n\n");
}

//Funciones Zonas
struct ZoneList* CreateZoneCircularList(){
    struct ZoneList* list;
    list = (struct ZoneList*)malloc(sizeof(struct ZoneList));

    if (!list) { return NULL; }

    list->zones = NULL;
    list->tail  = NULL;
    list->lenght = 0;

    return list;
}
struct Zone* CreateZone(char* name, int numberOfHabitants, float habitantsAverageAge){
    struct Zone* zone;
    zone = (struct Zone*)malloc(sizeof(struct Zone));

    if (!zone) { return NULL; }

    zone->name = (char*)malloc(sizeof(char) * strlen(name));
    if (!zone->name) { return NULL; }
    strcpy(zone->name, name);

    zone->numberOfHabitants = numberOfHabitants;
    zone->habitantsAverageAge = habitantsAverageAge;
    zone->distributionCenters = CreateDistributionCenterNode(NULL);

    return zone;
}
struct ZoneNode* CreateZoneNode(struct Zone* zone){
    struct ZoneNode* node;
    node = (struct ZoneNode*)malloc(sizeof(struct ZoneNode));

    if (!node) { return NULL; }

    node->zone = zone;
    node->next = NULL;

    return node;
}
struct ZoneNode* FindZoneByName(struct ZoneList* zoneList, char* name){
    if (!zoneList || !zoneList->zones) { return NULL; }

    struct ZoneNode* traverse = zoneList->zones;

    do {
        if (!strcmp(traverse->zone->name, name)) { return traverse; }
        traverse = traverse->next;
    } while (traverse != zoneList->zones);

    return NULL;
}
int AddZone(struct ZoneList* zoneList, struct Zone* zone){
    if (!zoneList || FindZoneByName(zoneList, zone->name)) { return 1; }

    struct ZoneNode *newNode;

    newNode = CreateZoneNode(zone);

    if (!newNode) { return 1; }

    if (!zoneList->zones){
        zoneList->zones = newNode;
        zoneList->tail  = newNode;
    }

    newNode->next = zoneList->zones;
    zoneList->tail->next = newNode;

    zoneList->lenght++;
    zoneList->tail = newNode;

    return 0;
}
int DeleteZone(struct ZoneList* zoneList, struct ZoneNode* nodeToDelete){
    if (!zoneList || !zoneList->zones) { return 1; }

    if (zoneList->zones == nodeToDelete){

        if (zoneList->zones->next == zoneList->zones){
            zoneList->zones = NULL;
            zoneList->tail  = NULL;
        } else{
            zoneList->zones = zoneList->zones->next;
            zoneList->tail->next = zoneList->zones;
        }
        zoneList->lenght--;
        return 0;
    }

    struct ZoneNode* traverse = zoneList->zones;

    do{
        if (traverse->next == nodeToDelete) {
            if (traverse->next == zoneList->tail) { zoneList->tail = traverse; }
            traverse->next = traverse->next->next;
            zoneList->lenght--;
            return 0;
        }
        traverse = traverse->next;
    }while (traverse->next != zoneList->zones);

    return 1;
}
//Modificar datos Zonas
int ChangeZoneName(struct ZoneNode* nodeToChange, char* name) {
    if (!nodeToChange) { return 1; }
    nodeToChange->zone->name = (char*)malloc(sizeof(char)*strlen(name));
    if (!nodeToChange->zone->name) { return 1; }
    strcpy(nodeToChange->zone->name, name);
    return 0;
}
int ChangeZonesNumberOfHabitants(struct ZoneNode* nodeToChange, int numberOfHabitants) {
    if (!nodeToChange || numberOfHabitants == -1) { return 1; }
    nodeToChange->zone->numberOfHabitants = numberOfHabitants;
    return 0;
}
int ChangeZonesHabitantsAverageAge(struct ZoneNode* nodeToChange, float habitantsAverageAge) {
    if (!nodeToChange || habitantsAverageAge == (float)(-1)) { return 1; }
    nodeToChange->zone->habitantsAverageAge = habitantsAverageAge;
    return 0;
}
//Mostrar Zonas
void PrintZone(struct Zone* zone) {
    printf("Nombre = \"%s\"\nNumero de habitantes = %d\nEdad promedio de los habitantes %.2f\n\n",
           zone->name, zone->numberOfHabitants, zone->habitantsAverageAge);
}
void PrintZoneList(struct ZoneList* zoneList) {
    if (!zoneList || !zoneList->zones) { return; }

    struct ZoneNode* traverse = zoneList->zones;

    printf("---------------------------------------------------------\n\n");
    printf("Longitud lista circular = %d\n\nCola de la lista:\n\nNombre = \"%s\"\nNumero de habitantes = %d\nEdad promedio de los habitantes %.2f\n\n",
           zoneList->lenght, zoneList->tail->zone->name, zoneList->tail->zone->numberOfHabitants, zoneList->tail->zone->habitantsAverageAge);
    printf("---------------------------------------------------------\n\n");

    do {
        PrintZone(traverse->zone);
        traverse = traverse->next;
    } while (traverse != zoneList->zones);
}
//Funciones Marcas
struct Brand* CreateBrand(char* name, float productTravelTime, float status) {
    struct Brand* brand;
    brand = (struct Brand*)malloc(sizeof(struct Brand));

    if (!brand) { return NULL; }

    brand->name = (char**)malloc(sizeof(char*));
    if (!brand->name) { return NULL; }

    (*brand->name) = (char*)malloc(sizeof(char) * strlen(name));
    if (!(*brand->name)) { return NULL; }
    strcpy(*brand->name, name);

    brand->productTravelTime = productTravelTime;
    brand->status = status;
    brand->stock = NULL;

    return brand;
}
struct BrandNode* CreateBrandNode(struct Brand* brand) {
    struct BrandNode* node;
    node = (struct BrandNode*)malloc(sizeof(struct BrandNode));

    if (!node) { return NULL; }

    node->brand = brand;
    node->next = NULL;
    node->prev = NULL;

    return node;
}
struct BrandNode* FindBrandByName(struct BrandNode* brands, char* name) {
    if (!brands) { return NULL; }

    struct BrandNode* traverse = brands;

    do {
        if (!strcmp(*traverse->brand->name, name)) { return traverse; }
        traverse = traverse->next;
    } while (traverse != brands);

    return NULL;
}
int AddBrand(struct BrandNode** brands, struct Brand* brand) {
    if (FindBrandByName(*brands, *brand->name)) { return 1; }

    struct BrandNode *newNode;

    newNode = CreateBrandNode(brand);
    if (!newNode) { return 1; }

    if (!(*brands)) {
        *brands = newNode;
        newNode->next = newNode;
        newNode->prev = newNode;
        return 0;
    }

    newNode->next = *brands;
    newNode->prev = (*brands)->prev;
    newNode->prev->next = newNode;
    newNode->next->prev = newNode;

    return 0;
}
int DeleteBrand(struct BrandNode** brands, struct BrandNode* nodeToDelete) {
    if (!(*brands)) { return 1; }

    struct BrandNode* traverse = *brands;

    do {
        if (traverse == nodeToDelete) {
            if (traverse->next == traverse) { *brands = NULL; }
            else {
                if (traverse == *brands) { *brands = (*brands)->next; }
                traverse->prev->next = traverse->next;
                traverse->next->prev = traverse->prev;
            }
            return 0;
        }
        traverse = traverse->next;
    } while (traverse != *brands);

    return 1;
}
//Modificar datos Marcas
int ChangeBrandName(struct BrandNode* nodeToChange, char* name) {
    if (!nodeToChange) { return 1; }

    (*nodeToChange->brand->name) = (char*)malloc(sizeof(char) * strlen(name));
    if (!(*nodeToChange->brand->name)) { return 1; }
    strcpy(*nodeToChange->brand->name, name);
    return 0;
}
int ChangeBrandProductTravelTime(struct BrandNode* nodeToChange, float productTravelTime) {
    if (!nodeToChange || productTravelTime == (float)(-1)) { return 1; }
    nodeToChange->brand->productTravelTime = productTravelTime;
    return 0;
}
int ChangeBrandStatus(struct BrandNode* nodeToChange, float status) {
    if (!nodeToChange || status == -1) { return 1; }
    nodeToChange->brand->status = status;
    return 0;
}
//Mostrar Marcas
void PrintBrand(struct Brand* brand) {
    printf("Nombre = \"%s\"\nTiempo de desplazamiento de mercancias (dias) = %.2f\nValoracion = %.2f\n\n", *brand->name, brand->productTravelTime, brand->status);
}
void PrintBrands(struct BrandNode* brands) {
    if (!brands) { return; }

    struct BrandNode* traverse = brands;

    do {
        PrintBrand(traverse->brand);
        traverse = traverse->next;
    } while (traverse != brands);
}
void PrintBrandNames(struct BrandNode* brands) {
    if (!brands) { return; }

    struct BrandNode* traverse = brands;
    int i = 0;

    printf("--------------------------------------------------\n\n");
    printf("NOMBRE DE LAS MARCAS/PROVEEDORES CREADOS:\n\n");

    do {
        printf("%d. \"%s\"\n", i++, *traverse->brand->name);
        traverse = traverse->next;
    } while (traverse != brands);
    printf("\n--------------------------------------------------\n\n");
}

struct Product* FindProductByIDInAllBrands(int ID) {
    if (!derco->brands) { return NULL; }

    struct BrandNode* traverse = derco->brands;
    struct ProductNode* product = NULL;

    do {
        product = FindProductByID(traverse->brand->stock, ID);
        if (product) { return product->product; }
        traverse = traverse->next;
    } while (traverse != derco->brands);

    return NULL;
}
void PrintProductIDsInAllBrands() {
    if (!derco->brands) { return; }

    struct BrandNode* traverse = derco->brands;

    printf("PRODUCTOS DE LAS MARCAS/PROVEEDORES:\n\n");

    do {
        printf("Marca: \"%s\"\n\n", *traverse->brand->name);
        PrintProductIDs(traverse->brand->stock);
        traverse = traverse->next;
    } while (traverse != derco->brands);
}

//Funciones adicionales

//Funcion imprimir autos vendidos por zona
int FindProductIDInList(struct ProductIDNode* productIDs, struct ProductSharedData* productSharedData) {
    struct ProductIDNode* traverse = productIDs;

    while (traverse) {
        if (traverse->ID == productSharedData) { return 0; }
        traverse = traverse->next;
    }

    return 1;
}
struct ProductIDNode* AddProductIDNode(struct ProductIDNode* productIDs, struct ProductSharedData* productSharedData) {
    struct ProductIDNode* newNode;
    newNode = (struct ProductIDNode*)malloc(sizeof(struct ProductIDNode));

    if (!newNode) { return NULL; }

    newNode->ID = productSharedData;
    newNode->next = productIDs;

    return newNode;
}

void PrintProductBought(struct Purchases* purchases, struct ProductIDNode** productIDs) {
    int i;

    for (i = 0; i < purchases->indexFirstEmpty; i++) {
        if (FindProductIDInList(*productIDs, purchases->purchaseArray[i]->productSharedData)) {
            *productIDs = AddProductIDNode(*productIDs, purchases->purchaseArray[i]->productSharedData);
            PrintProductSharedData(purchases->purchaseArray[i]->productSharedData);
        }
    }
}
void CustomerTraversePrintCars(struct CustomerNode* root, struct ProductIDNode** productIDs) {
    if (!root) { return; }

    PrintProductBought(root->customer->purchases, productIDs);
    CustomerTraversePrintCars(root->left, productIDs);
    CustomerTraversePrintCars(root->right, productIDs);
}
void PrintCarsSoldInZone(struct DistributionCenterNode* distributionCenters) {
    struct ProductIDNode* productIDs = NULL;
    struct DistributionCenterNode *distributionCenterTraverse = distributionCenters->next;

    while (distributionCenterTraverse) {
        CustomerTraversePrintCars(distributionCenterTraverse->distributionCenter->customers, &productIDs);
        distributionCenterTraverse = distributionCenterTraverse->next;
    }
}

//Funcion: Encontrar Marca con mayor venta de autos en una zona especifica
int CountCarSalesOfBrand(struct Purchases* purchases, char* brandName) {
    int i, acumulator = 0;

    for (i = 0; i < purchases->indexFirstEmpty; i++) {
        if (purchases->purchaseArray[i]->productSharedData->type == CAR && !strcmp(*purchases->purchaseArray[i]->productSharedData->brandName, brandName)) {
            acumulator += purchases->purchaseArray[i]->amount;
        }
    }

    return acumulator;
}
int FindBrandNameInList(struct BrandNameNode* brandNames, char* name) {
    struct BrandNameNode* traverse = brandNames;

    while (traverse) {
        if (!strcmp(traverse->brandName, name)) { return 0; }
        traverse = traverse->next;
    }

    return 1;
}
void CustomerCarMostSellBrandTraverse(struct CustomerNode* root, char* brandName, int* carSales) {
    if (!root) { return; }

    *carSales += CountCarSalesOfBrand(root->customer->purchases, brandName);
    CustomerCarMostSellBrandTraverse(root->left, brandName, carSales);
    CustomerCarMostSellBrandTraverse(root->right, brandName, carSales);
}
struct BrandNameNode* AddBrandNameNode(struct BrandNameNode* brandNames, char* brandName) {
    struct BrandNameNode* newNode;
    newNode = (struct BrandNameNode*)malloc(sizeof(struct BrandNameNode));

    if (!newNode) { return NULL; }

    newNode->brandName = brandName;
    newNode->next = brandNames;

    return newNode;
}
struct BrandNameNode* AddNewCarBrandNames(struct Purchases* purchases, struct BrandNameNode* brandNames) {
    int i;

    for (i = 0; i < purchases->indexFirstEmpty; i++) {
        if (purchases->purchaseArray[i]->productSharedData->type == CAR && FindBrandNameInList(brandNames, *purchases->purchaseArray[i]->productSharedData->brandName)) {
            brandNames = AddBrandNameNode(brandNames, *purchases->purchaseArray[i]->productSharedData->brandName);
        }
    }

    return brandNames;
}
void CustomerTraverseCarBrandNames(struct CustomerNode* root, struct BrandNameNode** brandNames) {
    if (!root) { return; }

    *brandNames = AddNewCarBrandNames(root->customer->purchases, *brandNames);
    CustomerTraverseCarBrandNames(root->left, brandNames);
    CustomerTraverseCarBrandNames(root->right, brandNames);
}
struct BrandNameNode* CreateCarBrandNameList(struct DistributionCenterNode* distributionCenters, struct BrandNameNode* brandNames) {
    struct DistributionCenterNode* distributionCenterTraverse = distributionCenters->next;

    while (distributionCenterTraverse) {
        CustomerTraverseCarBrandNames(distributionCenterTraverse->distributionCenter->customers, &brandNames);
        distributionCenterTraverse = distributionCenterTraverse->next;
    }

    return brandNames;
}
char* BestSellingCarBrandInZone(struct DistributionCenterNode* distributionCenters) {
    struct BrandNameNode *brandNames = NULL, *brandNameTraverse = NULL;
    struct DistributionCenterNode *distributionCenterTraverse;
    char* carMostSellBrandName = NULL;
    int carBestSeller = 0, carSales;

    brandNames = CreateCarBrandNameList(distributionCenters, brandNames);

    brandNameTraverse = brandNames;

    while (brandNameTraverse) {
        distributionCenterTraverse = distributionCenters->next;
        carSales = 0;
        while (distributionCenterTraverse) {
            CustomerCarMostSellBrandTraverse(distributionCenterTraverse->distributionCenter->customers, brandNameTraverse->brandName, &carSales);
            distributionCenterTraverse = distributionCenterTraverse->next;
        }
        if (carBestSeller < carSales) {
            carBestSeller = carSales;
            carMostSellBrandName = brandNameTraverse->brandName;
        }
        brandNameTraverse = brandNameTraverse->next;
    }

    return carMostSellBrandName;
}

//Funcion cliente que mas haya gastado
float TotalMoneySpent(struct Purchases* purchases) {
    int i;
    float acumulator = 0;

    for (i = 0; i < purchases->indexFirstEmpty; i++) {
        acumulator += purchases->purchaseArray[i]->amount * purchases->purchaseArray[i]->boughtPrice;
    }

    return acumulator;
}
void CustomerSpendTheMostTraverse(struct CustomerNode* root, struct Customer** customer, float *moneySpend) {
    if (!root) { return; }

    float aux;

    aux = TotalMoneySpent(root->customer->purchases);
    if (*moneySpend < aux) {
        *moneySpend = aux;
        *customer = root->customer;
    }

    CustomerSpendTheMostTraverse(root->left, customer, moneySpend);
    CustomerSpendTheMostTraverse(root->right, customer, moneySpend);
}
struct Customer* CustomerThatHasSpendTheMost() {
    if (!derco->zoneList->zones) { return NULL; }

    struct ZoneNode* zoneTraverse = derco->zoneList->zones;
    struct DistributionCenterNode* distributionCenterTraverse = NULL;
    struct Customer* customer = NULL;
    float moneySpend = 0;

    do {
        distributionCenterTraverse = zoneTraverse->zone->distributionCenters->next;
        while (distributionCenterTraverse) {
            CustomerSpendTheMostTraverse(distributionCenterTraverse->distributionCenter->customers, &customer, &moneySpend);
            distributionCenterTraverse = distributionCenterTraverse->next;
        }
        zoneTraverse = zoneTraverse->next;
    }while (zoneTraverse != derco->zoneList->zones);

    if(moneySpend == 0){
        return NULL;
    }

    return customer;
}

//Funcion: Auto con mas recalls
void ProductTraverseCarWithMoreRecalls(struct ProductNode* root, struct Product** carWithMoreRecalls, int* carRecalls) {
    if (!root) { return; }

    ProductTraverseCarWithMoreRecalls(root->left, carWithMoreRecalls, carRecalls);

    if (root->product->sharedData->type == CAR && *carRecalls < root->product->sharedData->recalls) {
        *carRecalls = root->product->sharedData->recalls;
        *carWithMoreRecalls = root->product;
    }

    ProductTraverseCarWithMoreRecalls(root->right, carWithMoreRecalls, carRecalls);
}
struct Product* FindCarWithMoreRecalls() {
    if (!derco->brands) { return NULL; }

    struct BrandNode* brandTraverse = derco->brands;
    struct Product* carWithMoreRecalls = NULL;
    int carRecalls = 0;

    do {
        ProductTraverseCarWithMoreRecalls(brandTraverse->brand->stock, &carWithMoreRecalls, &carRecalls);
        brandTraverse = brandTraverse->next;
    } while (brandTraverse != derco->brands);

    return carWithMoreRecalls;
}

//Funcion: Centro distribucion con mayor cantidad de ventas
void CustomerTraverseDistributionCenterWithMoreSales(struct CustomerNode* root, int* numberOfSoldProducts) {
    if (!root) { return; }

    *numberOfSoldProducts += root->customer->purchases->numberOfPurchaseProducts;
    CustomerTraverseDistributionCenterWithMoreSales(root->left, numberOfSoldProducts);
    CustomerTraverseDistributionCenterWithMoreSales(root->right, numberOfSoldProducts);
}
struct DistributionCenter* DistributionCenterWithMoreSales() {
    if (!derco->zoneList->zones) { return NULL; }

    struct ZoneNode* zoneTraverse = derco->zoneList->zones;
    struct DistributionCenterNode* distributionCenterTraverse;
    struct DistributionCenter* distributionCenterWithMoreSales = NULL;
    int numberOfSoldProducts, greaterSales = 0;

    do {
        distributionCenterTraverse = zoneTraverse->zone->distributionCenters->next;
        while (distributionCenterTraverse) {
            numberOfSoldProducts = 0;
            CustomerTraverseDistributionCenterWithMoreSales(distributionCenterTraverse->distributionCenter->customers, &numberOfSoldProducts);

            if (greaterSales < numberOfSoldProducts) {
                greaterSales = numberOfSoldProducts;
                distributionCenterWithMoreSales = distributionCenterTraverse->distributionCenter;
            }

            distributionCenterTraverse = distributionCenterTraverse->next;
        }
        zoneTraverse = zoneTraverse->next;
    } while (zoneTraverse != derco->zoneList->zones);

    return distributionCenterWithMoreSales;
}

//Funcion: Marca con peor valoracion
struct Brand* FindBrandWithLowestStatus(){
    if (!derco->brands) { return NULL; }

    struct BrandNode* brandTraverse = derco->brands;
    struct Brand* brandWithLowestStatus = NULL;
    float lowestStatus = 6;

    do {
        if (brandTraverse->brand->status < lowestStatus) {
            lowestStatus = brandTraverse->brand->status;
            brandWithLowestStatus = brandTraverse->brand;
        }
        brandTraverse = brandTraverse->next;
    } while (brandTraverse != derco->brands);

    return brandWithLowestStatus;
}

//Funcion: Encontrar Producto mas vendido segun tipo de producto
struct ProductIDNode* AddNewProductIDs(struct Purchases* purchases, enum ProductType type, struct ProductIDNode* productIDs) {
    int i;

    for (i = 0; i < purchases->indexFirstEmpty; i++) {
        if ((purchases->purchaseArray[i]->productSharedData->type == type || type == END) && FindProductIDInList(productIDs, purchases->purchaseArray[i]->productSharedData)) {
            productIDs = AddProductIDNode(productIDs, purchases->purchaseArray[i]->productSharedData);
        }
    }

    return productIDs;
}
void CustomerTraverseProductIDs(struct CustomerNode* root, enum ProductType type, struct ProductIDNode** productIDs) {
    if (!root) { return; }

    *productIDs = AddNewProductIDs(root->customer->purchases, type, *productIDs);
    CustomerTraverseProductIDs(root->left, type, productIDs);
    CustomerTraverseProductIDs(root->right, type, productIDs);
}
struct ProductIDNode* CreateProductIDsList(struct DistributionCenterNode* distributionCenters, enum ProductType type) {
    struct ProductIDNode* productIDs = NULL;
    struct DistributionCenterNode* distributionCenterTraverse = distributionCenters->next;

    while (distributionCenterTraverse) {
        CustomerTraverseProductIDs(distributionCenterTraverse->distributionCenter->customers, type, &productIDs);
        distributionCenterTraverse = distributionCenterTraverse->next;
    }

    return productIDs;
}
int CountProductSales(struct Purchases* purchases, struct ProductSharedData* ID) {
    int i, acumulator = 0;

    for (i = 0; i < purchases->indexFirstEmpty; i++) {
        if (purchases->purchaseArray[i]->productSharedData == ID) {
            acumulator += purchases->purchaseArray[i]->amount;
        }
    }

    return acumulator;
}
void CustomerTraverseBestSellingProduct(struct CustomerNode* root, struct ProductSharedData* ID, int* sales) {
    if (!root) { return; }

    *sales += CountProductSales(root->customer->purchases, ID);
    CustomerTraverseBestSellingProduct(root->left, ID, sales);
    CustomerTraverseBestSellingProduct(root->right, ID, sales);
}
struct ProductSharedData* FindBestSellingProductByType(struct DistributionCenterNode* distributionCenters, enum ProductType type, int* bestSeller) {
    struct ProductIDNode* ProductIDS = NULL, *ProductIDTraverse = NULL;
    struct DistributionCenterNode* distributionCenterTraverse;
    struct ProductSharedData* bestSellingProduct = NULL;
    int sales;

    *bestSeller = 0;

    ProductIDS = CreateProductIDsList(distributionCenters, type);

    ProductIDTraverse = ProductIDS;

    while (ProductIDTraverse) {
        distributionCenterTraverse = distributionCenters->next;
        sales = 0;
        while (distributionCenterTraverse) {
            CustomerTraverseBestSellingProduct(distributionCenterTraverse->distributionCenter->customers, ProductIDTraverse->ID, &sales);
            distributionCenterTraverse = distributionCenterTraverse->next;
        }
        if (*bestSeller < sales) {
            *bestSeller = sales;
            bestSellingProduct = ProductIDTraverse->ID;
        }
        ProductIDTraverse = ProductIDTraverse->next;
    }

    return bestSellingProduct;
}
struct ProductSharedData* FindBestSellingProductByTypeInAllZones(enum ProductType type) {
    if (!derco->zoneList->zones) { return NULL; }

    struct ZoneNode* zoneTraverse = derco->zoneList->zones;
    struct ProductSharedData* bestSellingProduct = NULL, *bestSellingProductInZone = NULL;
    int bestSeller = 0, sales;

    do {
        bestSellingProductInZone = FindBestSellingProductByType(zoneTraverse->zone->distributionCenters, type, &sales);
        if (bestSeller < sales) {
            bestSeller = sales;
            bestSellingProduct = bestSellingProductInZone;
        }
        zoneTraverse = zoneTraverse->next;
    } while (zoneTraverse != derco->zoneList->zones);

    return bestSellingProduct;
}

/////////////////////////////FUNCION EXTRA: Total de dinero recaudado///////////////////////////////////

void dineroTotalClientes(struct CustomerNode *root, float *monto){
    if(!root){ return; }

    *monto += TotalMoneySpent(root->customer->purchases);
    dineroTotalClientes(root->left, monto);
    dineroTotalClientes(root->right, monto);
}
void dineroRecaudadoPorCentroDistribucion(){
    if(!derco->zoneList->zones){
        printf("No hay centros de distribucion\n\n");
        return;
    }

    struct ZoneNode *recZona = derco->zoneList->zones;
    struct DistributionCenterNode *recCentro;
    float monto, montoTotal = 0;

    do{
        recCentro = recZona->zone->distributionCenters->next;
        printf("Zona: %s\n\n", recZona->zone->name);
        while(recCentro){
            monto = 0;
            dineroTotalClientes(recCentro->distributionCenter->customers, &monto);

            PrintDistributionCenter(recCentro->distributionCenter);
            printf("Monto total recaudado: %f\n\n", monto);

            montoTotal += monto;
            recCentro = recCentro->next;
        }
        recZona = recZona->next;
    }while(recZona != derco->zoneList->zones);

    if(montoTotal != 0){
        printf("Monto total recaudado en todos los centros de distribucion: %f\n\n", montoTotal);
    }

    else{
        printf("No se han realizado compras\n\n");
    }
}

//MENUS

//Menus Compras
int MenuDeletePurchase(struct Purchases* purchases) {
    return DeletePurchase(purchases, FindPurchaseByID(purchases, EnterInteger("Ingrese el numero de boleta de la compra que desea eliminar: ")));
}
int MenuAddPurchase(struct Purchases* purchases) {
    int ID;
    int productID;
    int amount;
    struct ProductNode* boughtProduct;

    PrintProductIDs(actualDistributionCenter->distributionCenter->Warehouse->products);

    productID = EnterInteger("Ingrese el ID del producto que quiere comprar: ");

    boughtProduct = FindProductByID(actualDistributionCenter->distributionCenter->Warehouse->products, productID);
    if (!boughtProduct) {
        printf("No se ha encontrado el producto.\n\n");

        return 1;
    }

    PrintPurchaseIDs(purchases);

    ID = EnterInteger("Ingrese el numero de boleta de la compra: ");
    if (ID == -1) { return 1; }
    if (FindPurchaseByID(purchases, ID)){
        printf("No se puede repetir el numero de boleta.\n\n");
        return 1;
    }

    printf("--------------------------------------------------\n\n");
    printf("Cantidad del producto en almacen: %d\n\n", boughtProduct->product->amount);
    printf("--------------------------------------------------\n\n");

    amount = EnterInteger("Ingrese la cantidad de productos de la compra: ");
    if (amount == -1) { return 1; }

    if (boughtProduct->product->amount < amount) {
        printf("El almacen tiene menos productos de los que se desea comprar.\n\n");

        return 1;
    }

    if (AddPurchase(purchases, CreatePurchase(ID, boughtProduct->product->sharedData, amount, boughtProduct->product->price))) { return 1; }

    boughtProduct->product->amount -= amount;
    actualDistributionCenter->distributionCenter->Warehouse->numberOfProducts -= amount;
    actualDistributionCenter->distributionCenter->Warehouse->cbMetersSpaceUsed -= (float)amount * boughtProduct->product->sharedData->cbMetersSpacePerUnit;

    return 0;
}
int MenuModifyPurchase(struct Purchases* purchases, struct Purchase* purchase) {
    int input;
    int amount;
    struct ProductNode* product;

    if (!purchase) {
        purchase = FindPurchaseByID(purchases, EnterInteger("Ingrese el numero de boleta de la compra a modificar: "));
        if (!purchase) { return 1; }
    }

    do{
        printf("--------------------------------------------------\n\n");
        PrintPurchase(purchase);
        printf("--------------------------------------------------\n\n");
        printf("MENU MODIFICAR COMPRA:\n\n0. Atras.\n1. Modificar el producto comprado.\n2. Modificar la cantidad de la compra.\n3. Modificar el precio de compra.\n\n");
        input = EnterInteger("Ingrese un numero: ");
        if (input == -1) { continue; }

        switch (input) {
            case 0: break;
            case 1:
                product = FindProductByID(actualDistributionCenter->distributionCenter->Warehouse->products, EnterInteger("Ingrese el numero de boleta del producto comprado: "));
                if (!product) {
                    printf("El producto no se ha podido encontrar.\n\n");
                }
                else { ChangePurchaseProductSharedData(purchase, product->product->sharedData); }
                break;
            case 2:
                amount = EnterInteger("Ingrese la cantidad de la compra: ");
                if (amount != -1) {
                    purchases->numberOfPurchaseProducts += amount - purchase->amount;
                    ChangePurchaseAmount(purchase, amount);
                }
                break;
            case 3: ChangePurchaseBoughtPrice(purchase, EnterFloatingPoint("Ingrese el precio de la compra: ")); break;
            default: printf("Valor invalido.\n"); break;
        }
    } while (input);

    return 0;
}
int MenuFindPurchase(struct Purchases* purchases) {
    enum Find input;
    struct Purchase* purchase;

    purchase = FindPurchaseByID(purchases, EnterInteger("Ingrese el numero de boleta de la compra que quiere buscar: "));
    if (!purchase) { return 1; }

    do {

        printf("--------------------------------------------------\n\n");
        PrintPurchase(purchase);
        printf("--------------------------------------------------\n\n");
        printf("MENU COMPRA:\n\n0. Atras.\n1. Eliminar compra.\n2. Modificar datos de la compra.\n\n");
        input = EnterInteger("Ingrese un numero: ");
        if (input == -1) { continue; }

        switch (input) {
            case 0: break;
            case fDELETE:
                if (DeletePurchase(purchases, purchase)) {
                    printf("La compra no se ha podido eliminar correctamente.\n");

                }
                else {
                    printf("La compra se ha eliminado exitosamente.\n");

                    return 0;
                }
                break;
            case fMODIFY:
                MenuModifyPurchase(purchases, purchase);
                break;
            default: printf("Valor invalido.\n");  break;
        }
    }while (input);

    return 0;
}
void MenuPurchases(struct Purchases* purchases) {
    enum Menu input;

    do{
        printf("MENU PRINCIPAL COMPRAS:\n\n0. Atras.\n1. Agregar compra.\n2. Eliminar compra.\n3. Buscar compra.\n4. Modificar compra.\n5. Mostrar compras.\n\n");
        input = EnterInteger("Ingrese un numero: ");
        if (input == -1) { continue; }

        switch (input) {
            case 0: break;
            case mADD:
                if (MenuAddPurchase(purchases)) {
                    printf("La compra no se ha podido agregar correctamente.\n");
                } else { printf("La compra se ha agregado exitosamente.\n"); }

                break;
            case mDELETE:
                PrintPurchaseIDs(purchases);
                if (MenuDeletePurchase(purchases)) {
                    printf("La compra no se ha podido eliminar correctamente.\n");
                } else { printf("La compra se ha eliminado exitosamente.\n"); }

                break;
            case mFIND:
                PrintPurchaseIDs(purchases);
                if (MenuFindPurchase(purchases)) {
                    printf("La compra no se ha podido encontrar.\n");

                }
                break;
            case mMODIFY1:
                PrintPurchaseIDs(purchases);
                if (MenuModifyPurchase(purchases, NULL)) {
                    printf("La compra no se ha podido encontrar.\n");

                }
                break;

            case mPRINT:
                PrintPurchases(purchases);
                break;

            default: printf("Valor invalido.\n");  break;
        }
    } while (input);
}

//Menus Clientes
int MenuAddCustomer(struct CustomerNode** customers) {
    int ID;
    char* name;
    int age;
    int maxPurchases;

    ID = EnterInteger("Ingrese el ID del cliente: ");
    if (ID == -1) { return 1; }
    if (FindCustomerByID(*customers, ID)) {
        printf("No se puede repetir el ID.\n\n");

        return 1;
    }
    name = EnterText("Ingrese el nombre del cliente: ");
    if (!name) { return 1; }
    age = EnterInteger("Ingrese la edad del cliente: ");
    if (age == -1) { return 1; }
    maxPurchases = EnterInteger("Ingrese la cantidad maxima de compras que puede realizar el cliente: ");
    if (maxPurchases == -1) { return 1; }

    return AddCustomer(customers, CreateCustomer(ID, name, age, maxPurchases));
}
int MenuDeleteCustomer(struct CustomerNode** customers) {
    return DeleteCustomer(customers, EnterInteger("Ingrese el ID del cliente que desea eliminar: "));
}
int MenuModifyCustomer(struct CustomerNode** customers, struct CustomerNode* customer) {
    int input;

    if (!customer) {
        customer = FindCustomerByID(*customers, EnterInteger("Ingrese el ID del cliente a modificar: "));
        if (!customer) { return 1; }
    }

    do{
        printf("--------------------------------------------------\n\n");
        PrintCustomer(customer->customer);
        printf("--------------------------------------------------\n\n");
        printf("MENU MODIFICAR CLIENTE:\n\n0. Atras.\n1. Modificar nombre del cliente.\n2. Modificar edad del cliente.\n\n");
        input = EnterInteger("Ingrese un numero: ");
        if (input == -1) { continue; }

        switch (input) {
            case 0: break;
            case 1: ChangeCustomerName(customer, EnterText("Ingrese el nombre del cliente: ")); break;
            case 2: ChangeCustomerAge(customer, EnterInteger("Ingrese la edad del cliente: ")); break;
            default: printf("Valor invalido.\n"); break;
        }
    }while (input);

    return 0;
}
int MenuFindCustomer(struct CustomerNode** customers) {
    enum Find input;
    struct CustomerNode* customer;

    customer = FindCustomerByID(*customers, EnterInteger("Ingrese el ID del cliente que quiere buscar: "));
    if (!customer) { return 1; }

    do{
        printf("--------------------------------------------------\n\n");
        PrintCustomer(customer->customer);
        printf("--------------------------------------------------\n\n");
        printf("MENU CLIENTE:\n\n0. Atras.\n1. Eliminar cliente.\n2. Modificar datos del cliente.\n3. Modificar las compras del cliente.\n\n");
        input = EnterInteger("Ingrese un numero: ");
        if (input == -1) { continue; }

        switch (input) {
            case 0: break;
            case fDELETE:
                if (DeleteCustomer(customers, customer->customer->ID)) {
                    printf("El cliente no se ha podido eliminar correctamente.\n");

                }
                else {
                    printf("El cliente se ha eliminado exitosamente.\n");

                    return 0;
                }
                break;

            case fMODIFY:
                MenuModifyCustomer(customers, customer);
                break;

            case fDATA_STRUCTURE1:
                MenuPurchases(customer->customer->purchases);
                break;

            default: printf("Valor invalido.\n");  break;
        }
    }while (input);

    return 0;
}
void MenuCustomers(struct CustomerNode** customers) {
    enum Menu input;

    do {

        printf("MENU PRINCIPAL CLIENTES:\n\n0. Atras.\n1. Agregar cliente.\n2. Eliminar cliente.\n3. Buscar cliente.\n4. Modificar cliente.\n5. Mostrar clientes.\n\n");
        input = EnterInteger("Ingrese un numero: ");
        if (input == -1) { continue; }

        if (0 < input && input < 5) { PrintCustomerIDs(*customers); }

        switch (input) {
            case 0: break;
            case mADD:
                if (MenuAddCustomer(customers)) {
                    printf("El cliente no se ha podido agregar correctamente.\n");
                }
                else { printf("El cliente se ha agregado exitosamente.\n"); }

                break;
            case mDELETE:
                if (MenuDeleteCustomer(customers)) {
                    printf("El cliente no se ha podido eliminar correctamente.\n");
                }
                else { printf("El cliente se ha eliminado exitosamente.\n"); }

                break;
            case mFIND:
                if (MenuFindCustomer(customers)) {
                    printf("No se ha podido encontrar el cliente.\n");

                }
                break;
            case mMODIFY1:
                if (MenuModifyCustomer(customers, NULL)) {
                    printf("No se ha podido encontrar el cliente.\n");

                }
                break;
            case mPRINT:
                PrintCustomers(*customers);
                break;

            default: printf("Valor invalido.\n");  break;
        }
    } while (input);
}

//Menu Almacenes
int MenuAddProductToWarehouse(struct Warehouse* Warehouse) {
    int productID;
    int amount;
    float price;
    struct Product* product;

    PrintProductIDsInAllBrands();

    productID = EnterInteger("Ingrese el ID del producto a almacenar: ");

    if (FindProductByID(Warehouse->products, productID)) {
        printf("El producto ya se encuentra en el almacen\n\n");

        return 1;
    }

    product = FindProductByIDInAllBrands(productID);
    if (!product || !product->amount) {
        printf("No se ha encontrado el producto.\n\n");

        return 1;
    }

    printf("--------------------------------------------------\n\n");
    printf("Cantidad de producto en stock de marca/proveedor: %d\n\n", product->amount);
    printf("--------------------------------------------------\n\n");

    amount = EnterInteger("Ingrese la cantidad que quiere almacenar: ");
    if (amount == -1) { return 1; }
    if (amount > product->amount) {
        printf("La cantidad solicitada excede la cantidad presente en el stock de la marca/proveedor.\n\n");

        return 1;
    }
    if (amount * product->sharedData->cbMetersSpacePerUnit > Warehouse->cbMetersMaxCapacity - Warehouse->cbMetersSpaceUsed) {
        printf("Al almacen no le queda espacio suficiente para almacenar todos los productos solicitados.\n\n");

        return 1;
    }

    Warehouse->numberOfProducts += amount;
    Warehouse->cbMetersSpaceUsed += (float)amount * product->sharedData->cbMetersSpacePerUnit;
    product->amount -= amount;

    price = EnterFloatingPoint("Ingrese el precio que quiere darle al producto: ");
    if (price == -1) { return 1; }

    return AddProduct(&Warehouse->products, CreateProduct(product->sharedData, amount, price));
}
int MenuDeleteWarehouseProduct(struct Warehouse* Warehouse) {
    int ID;
    struct ProductNode* product;

    ID = EnterInteger("Ingrese el ID del producto que desea eliminar: ");

    product = FindProductByID(Warehouse->products, ID);
    if (!product) { return 1; }

    if (DeleteProduct(&Warehouse->products, ID)) { return 1; }

    Warehouse->numberOfProducts -= product->product->amount;
    Warehouse->cbMetersSpaceUsed -= (float)product->product->amount * product->product->sharedData->cbMetersSpacePerUnit;

    return 0;
}
int MenuModifyWarehouseProduct(struct Warehouse* Warehouse, struct ProductNode* product) {
    int input;
    int amount;
    struct Product* stockProduct;

    if (!product) {
        product = FindProductByID(Warehouse->products, EnterInteger("Ingrese el ID del producto a modificar: "));
        if (!product) { return 1; }
    }

    stockProduct = FindProductByIDInAllBrands(product->product->sharedData->ID);

    do {

        printf("--------------------------------------------------\n\n");
        PrintProduct(product->product);
        printf("--------------------------------------------------\n\n");
        printf("MENU MODIFICAR PRODUCTO EN ALMACEN:\n\n0. Atras.\n1. Modificar cantidad del producto en almacen.\n2. Modificar precio del producto en centro de distribucion.\n\n");
        input = EnterInteger("Ingrese un numero: ");
        if (input == -1) { continue; }

        switch (input) {
            case 0: break;
            case 1:
                amount = EnterInteger("Ingrese la cantidad del producto en almacen: ");
                if (amount == -1) { break; }
                if (amount - product->product->amount > stockProduct->amount) {
                    printf("La cantidad solicitada excede la cantidad presente en el stock de la marca/proveedor.\n\n");

                }
                else if (amount - product->product->amount > Warehouse->cbMetersMaxCapacity - Warehouse->cbMetersSpaceUsed) {
                    printf("No hay suficiente espacio en el almacen.\n");

                }
                else {
                    stockProduct->amount += amount - product->product->amount;
                    Warehouse->cbMetersSpaceUsed += (float)(amount - product->product->amount) * product->product->sharedData->cbMetersSpacePerUnit;
                    Warehouse->numberOfProducts += amount - product->product->amount;
                    ChangeProductAmount(product, amount);
                }
                break;
            case 2: ChangeProductPrice(product, EnterFloatingPoint("Ingrese el nuevo precio del producto en almacen: ")); break;
            default: printf("Valor invalido.\n");  break;
        }
    } while (input);

    return 0;
}
int MenuFindWarehouseProduct(struct Warehouse* Warehouse) {
    enum Find input;
    struct ProductNode* product;

    product = FindProductByID(Warehouse->products, EnterInteger("Ingrese el ID del producto que quiere buscar: "));
    if (!product) { return 1; }

    do {

        printf("--------------------------------------------------\n\n");
        PrintProduct(product->product);
        printf("--------------------------------------------------\n\n");
        printf("MENU PRODUCTO EN ALMACEN:\n\n0. Atras.\n1. Eliminar producto.\n2. Modificar datos del producto.\n\n");
        input = EnterInteger("Ingrese un numero: ");
        if (input == -1) { continue; }

        switch (input) {
            case 0: break;
            case fDELETE:
                if (DeleteProduct(&Warehouse->products, product->product->sharedData->ID)) {
                    printf("El producto no se ha podido eliminar correctamente.\n");

                }
                else {
                    Warehouse->numberOfProducts -= product->product->amount;
                    Warehouse->cbMetersSpaceUsed -= (float)product->product->amount * product->product->sharedData->cbMetersSpacePerUnit;
                    printf("El producto se ha eliminado exitosamente.\n");
                    return 0;
                }
                break;
            case fMODIFY:
                MenuModifyWarehouseProduct(Warehouse, product);
                break;
            default: printf("Valor invalido.\n");  break;
        }
    } while (input);

    return 0;
}
void MenuWarehouse(struct Warehouse* Warehouse) {
    int input;
    int maxCapacity;

    do {

        printf("MENU PRINCIPAL ALMACEN:\n\n0. Atras.\n1. Agregar producto.\n2. Eliminar producto.\n3. Buscar producto."
               "\n4. Modificar producto.\n5. Mostrar almacen.\n6. Modificar capacidad maxima del almacen.\n\n");
        input = EnterInteger("Ingrese un numero: ");
        if (input == -1) { continue; }

        if (0 < input && input < 5) { PrintProductIDs(Warehouse->products); }

        switch (input) {
            case 0: break;
            case mADD:
                if (MenuAddProductToWarehouse(Warehouse)) {
                    printf("El producto no se ha podido agregar correctamente.\n");
                }
                else { printf("El producto se ha agregado exitosamente.\n"); }

                break;
            case mDELETE:
                if (MenuDeleteWarehouseProduct(Warehouse)) {
                    printf("El producto no se ha podido eliminar correctamente.\n");
                }
                else { printf("El producto se ha eliminado exitosamente.\n"); }

                break;
            case mFIND:
                if (MenuFindWarehouseProduct(Warehouse)) {
                    printf("No se ha podido encontrar el producto.\n");

                }
                break;
            case mMODIFY1:
                if (MenuModifyWarehouseProduct(Warehouse, NULL)) {
                    printf("No se ha podido encontrar el producto.\n");

                }
                break;
            case mPRINT:
                PrintWarehouse(Warehouse);
                break;
            case mSPECIALOPTION:
                printf("---------------------------------------------------------\n\n");
                printf("Actual capacidad maxima del almacen : %.2f\nEspacio utilizado en el almacen: %.2f\n\n",
                       Warehouse->cbMetersMaxCapacity, Warehouse->cbMetersSpaceUsed);
                printf("---------------------------------------------------------\n\n");
                if (ChangeWarehouseMaxCapacity(Warehouse, EnterFloatingPoint("Ingrese la capacidad maxima del almacen: "))) {
                    printf("La capacidad maxima del almacen no se ha podido modificar correctamente.\n");
                } else { printf("La capacidad maxima del almacen se ha modificado exitosamente.\n"); }

                break;
            default: printf("Valor invalido.\n");  break;
        }
    } while (input);
}

//Menus Centro Distribucion
int MenuAddDistributionCenter(struct DistributionCenterNode* distributionCenters) {
    int ID;
    char* address;
    int numberOfEmployes;
    float WarehouseMaxCapacity;

    ID = EnterInteger("Ingrese la ID del centro de distribucion: ");
    if (ID == -1) { return 1; }
    if (FindDistributionCenterByID(distributionCenters, ID)) {
        printf("No se puede repetir el ID.\n\n");

        return 1;
    }
    address = EnterText("Ingrese la direccion del centro de distribucion: ");
    if (!address) { return 1; }
    numberOfEmployes = EnterInteger("Ingrese el numero de empleados del centro de distribucion: ");
    if (numberOfEmployes == -1) { return 1; }
    WarehouseMaxCapacity = EnterFloatingPoint("Ingrese la capacidad maxima del almacen (m^3): ");
    if (WarehouseMaxCapacity == -1) { return 1; }

    return AddDistributionCenter(distributionCenters, CreateDistributionCenter(ID, address, numberOfEmployes, WarehouseMaxCapacity));
}
int MenuDeleteDistributionCenter(struct DistributionCenterNode* distributionCenters) {
    return DeleteDistributionCenter(distributionCenters, FindDistributionCenterByID(distributionCenters, EnterInteger("Ingrese el ID del centro de distribucion que desea eliminar: ")));
}
int MenuModifyDistributionCenter(struct DistributionCenterNode* distributionCenters, struct DistributionCenterNode* distributionCenter) {
    int input;

    if (!distributionCenter) {
        distributionCenter = FindDistributionCenterByID(distributionCenters, EnterInteger("Ingrese el ID del centro de distribucion a modificar: "));
        if (!distributionCenter) { return 1; }
    }

    do {

        printf("--------------------------------------------------\n\n");
        PrintDistributionCenter(distributionCenter->distributionCenter);
        printf("--------------------------------------------------\n\n");
        printf("MENU MODIFICAR CENTRO DE DISTRIBUCION:\n\n0. Atras.\n1. Modificar direccion del centro de distribucion."
               "\n2. Modificar numero de empleados del centro de distribucion.\n\n");
        input = EnterInteger("Ingrese un numero: ");
        if (input == -1) { continue; }

        switch (input) {
            case 0: break;
            case 1: ChangeDistributionCenterAddress(distributionCenter, EnterText("Ingrese la direccion del centro de distribucion: ")); break;
            case 2: ChangeDistributionCenterNumberOfEmployes(distributionCenter, EnterInteger("Ingrese el numero de empleados del centro de distribucion: ")); break;
            default: printf("Valor invalido.\n"); break;
        }
    } while (input);

    return 0;
}
int MenuFindDistributionCenter(struct DistributionCenterNode* distributionCenters) {
    enum Find input;
    struct DistributionCenterNode* distributionCenter;

    distributionCenter = FindDistributionCenterByID(distributionCenters, EnterInteger("Ingrese el ID del centro de distribucion que quiere buscar: "));
    if (!distributionCenter) { return 1; }

    actualDistributionCenter = distributionCenter;

    do {

        printf("--------------------------------------------------\n\n");
        PrintDistributionCenter(distributionCenter->distributionCenter);
        printf("--------------------------------------------------\n\n");
        printf("MENU CENTRO DE DISTRIBUCION:\n\n0. Atras.\n1. Eliminar centro de distribucion.\n2. Modificar datos del centro de distribucion."
               "\n3. Modificar los clientes del centro de distribucion.\n4. Modificar el almacen del centro de distribucion.\n\n");
        input = EnterInteger("Ingrese un numero: ");
        if (input == -1) { continue; }

        switch (input) {
            case 0: break;
            case fDELETE:
                if (DeleteDistributionCenter(distributionCenters, distributionCenter)) {
                    printf("El centro de distribucion no se ha podido eliminar correctamente.\n");

                }
                else {
                    printf("El centro de distribucion se ha eliminado exitosamente.\n");

                    return 0;
                }
                break;
            case fMODIFY:
                MenuModifyDistributionCenter(distributionCenters, distributionCenter);
                break;
            case fDATA_STRUCTURE1:
                MenuCustomers(&distributionCenter->distributionCenter->customers);
                break;
            case fDATA_STRUCTURE2:
                MenuWarehouse(distributionCenter->distributionCenter->Warehouse);
                break;
            default: printf("Valor invalido.\n");  break;
        }
    } while (input);

    return 0;
}
void MenuDistributionCenters(struct DistributionCenterNode* distributionCenters) {
    enum Menu input;

    do {

        printf("MENU PRINCIPAL CENTROS DE DISTRIBUCION:\n\n0. Atras.\n1. Agregar centro de distribucion.\n2. Eliminar centro de distribucion."
               "\n3. Buscar centro de distribucion.\n4. Modificar centro de distribucion.\n5. Mostrar centros de distribucion.\n\n");
        input = EnterInteger("Ingrese un numero: ");
        if (input == -1) { continue; }

        if (0 < input && input < 5) { PrintDistributionCenterIDs(distributionCenters); }

        switch (input) {
            case 0: break;
            case mADD:
                if (MenuAddDistributionCenter(distributionCenters)) {
                    printf("El centro de distribucion no se ha podido agregar correctamente.\n");
                } else { printf("El centro de distribucion se ha agregado exitosamente.\n"); }

                break;
            case mDELETE:
                if (MenuDeleteDistributionCenter(distributionCenters)) {
                    printf("El centro de distribucion no se ha podido eliminar correctamente.\n");
                } else { printf("El centro de distribucion se ha eliminado exitosamente.\n"); }

                break;
            case mFIND:
                if (MenuFindDistributionCenter(distributionCenters)) {
                    printf("No se ha podido encontrar el centro de distribucion.\n");

                }
                break;
            case mMODIFY1:
                if (MenuModifyDistributionCenter(distributionCenters, NULL)) {
                    printf("No se ha podido encontrar el centro de distribucion.\n");
                }
                break;
            case mPRINT:
                PrintDistributionCenters(distributionCenters);

                break;
            default: printf("Valor invalido.\n");  break;
        }
    }while(input);
}

//Menus Zonas
int MenuAddZone(struct ZoneList* zoneList) {
    char* name;
    int numberOfHabitants;
    float habitantsAverageAge;

    name = EnterText("Ingrese el nombre de la zona: ");
    if (!name) { return 1; }
    if (FindZoneByName(zoneList, name)) {
        printf("No se puede repetir el nombre.\n\n");

        return 1;
    }
    numberOfHabitants = EnterInteger("Ingrese el numero de habitantes de la zona: ");
    if (numberOfHabitants == -1) { return 1; }
    habitantsAverageAge = EnterFloatingPoint("Ingrese la edad promedio de los habitantes de la zona: ");
    if (habitantsAverageAge == -1) { return 1; }

    return AddZone(zoneList, CreateZone(name, numberOfHabitants, habitantsAverageAge));
}
int MenuModifyZone(struct ZoneList* zoneList, struct ZoneNode* zone) {
    int input;
    char* name;

    if (!zone) {
        zone = FindZoneByName(zoneList, EnterText("Ingrese el nombre de la zona a modificar: "));
        if (!zone) { return 1; }
    }

    do {

        printf("--------------------------------------------------\n\n");
        PrintZone(zone->zone);
        printf("--------------------------------------------------\n\n");
        printf("MENU MODIFICAR ZONA:\n\n0. Atras.\n1. Modificar nombre de la zona.\n2. Modificar numero de habitantes de la zona.\n3. Modificar edad promedio de habitantes de la zona.\n\n");
        input = EnterInteger("Ingrese un numero: ");
        if (input == -1) { continue; }

        switch (input) {
            case 0: break;
            case 1:
                name = EnterText("Ingrese el nombre de la zona: ");
                if (!name) { break; }
                if (FindZoneByName(zoneList, name)) {
                    printf("No se puede repetir el nombre.\n\n");

                }
                else { ChangeZoneName(zone, name); }
                break;
            case 2: ChangeZonesNumberOfHabitants(zone, EnterInteger("Ingrese el numero de habitantes de la zona: ")); break;
            case 3: ChangeZonesHabitantsAverageAge(zone, EnterFloatingPoint("Ingrese la edad promedio de los habitantes de la zona: ")); break;
            default: printf("Valor invalido.\n"); break;
        }

    } while (input);

    return 0;
}
int MenuFindZone(struct ZoneList* zoneList) {
    enum Find input;
    struct ZoneNode* zone;
    char* carMostSellBrandName;
    struct BrandNode* carMostSellBrand;
    struct ProductSharedData* product;
    enum ProductType type;
    int bestSeller;

    zone = FindZoneByName(zoneList, EnterText("Ingrese el nombre de la zona que quieres buscar: "));
    if (!zone) { return 1; }

    do {

        printf("--------------------------------------------------\n\n");
        PrintZone(zone->zone);
        printf("--------------------------------------------------\n\n");
        printf("MENU ZONA:\n\n0. Atras.\n1. Eliminar zona.\n2. Modificar datos de la zona.\n3. Modificar centros de distribucion."
               "\n4. Mostrar marca/proveedor con mas autos vendidos\n5. Mostrar producto mas vendido en base al tipo de producto.\n6. Mostrar modelos de autos vendidos\n\n");
        input = EnterInteger("Ingrese un numero: ");
        if (input == -1) { continue; }

        switch (input) {
            case 0: break;
            case fDELETE:
                if (DeleteZone(zoneList, zone)) {
                    printf("La zona no se ha podido eliminar correctamente.\n");
                }
                else {
                    printf("La zona se ha eliminado exitosamente.\n");
                    return 0;
                }
                break;
            case fMODIFY:
                MenuModifyZone(zoneList, zone);
                break;
            case fDATA_STRUCTURE1:
                MenuDistributionCenters(zone->zone->distributionCenters);
                break;

            case 4:
                carMostSellBrandName = BestSellingCarBrandInZone(zone->zone->distributionCenters);
                if (!carMostSellBrandName) { printf("No se ha vendido ningun auto tadavia.\n\n"); }
                else {
                    printf("Nombre de la marca/proveedor con mas autos vendidos: \"%s\"\n\n", carMostSellBrandName);
                    carMostSellBrand = FindBrandByName(derco->brands, carMostSellBrandName);
                    if (carMostSellBrand) { PrintBrand(carMostSellBrand->brand); }
                    else { printf("Esta marca ya no esta asociada con Derco.\n\n"); }
                }

                break;
            case 5:
                printf("0. Vehiculo\n1. Pieza de repuesto\n2. Accesorio\n3. Todos los tipos\n\n");
                type = EnterInteger("Ingrese el tipo de producto: ");
                if (type == -1 || type > END) {
                    printf("Valor invalido.\n");
                } else {
                    product = FindBestSellingProductByType(zone->zone->distributionCenters, type, &bestSeller);
                    if (!product) {
                        printf("No se ha encontrado un producto de ese tipo.\n\n");
                    } else {
                        PrintProductSharedData(product);
                    }
                }
                break;

            case 6:
                PrintCarsSoldInZone(zone->zone->distributionCenters);
                break;

            default: printf("Valor invalido.\n");  break;
        }
    } while (input);

    return 0;
}
int MenuDeleteZone(struct ZoneList* zoneList) {
    return DeleteZone(zoneList, FindZoneByName(zoneList, EnterText("Ingrese el nombre de la zona que desea eliminar: ")));
}
void MenuZoneList(struct ZoneList* zoneList) {
    enum Menu input;

    do {

        printf("MENU PRINCIPAL ZONAS:\n\n0. Atras.\n1. Agregar zona.\n2. Eliminar zona.\n3. Buscar zona.\n4. Modificar zona.\n5. Mostrar zonas.\n\n");
        input = EnterInteger("Ingrese un numero: ");



        switch (input) {
            case 0: break;
            case mADD:
                if (MenuAddZone(zoneList)) {
                    printf("La zona no se ha podido agregar correctamente.\n");
                } else { printf("La zona se ha agregado exitosamente.\n"); }

                break;
            case mDELETE:
                if (MenuDeleteZone(zoneList)) {
                    printf("La zona no se ha podido eliminar correctamente.\n");
                } else { printf("La zona se ha eliminado exitosamente.\n"); }

                break;
            case mFIND:
                if (MenuFindZone(zoneList)) {
                    printf("No se ha podido encontrar la zona.\n");

                }
                break;
            case mMODIFY1:
                if (MenuModifyZone(zoneList, NULL)) {
                    printf("No se ha podido encontrar la zona.\n");

                }
                break;
            case mPRINT:
                PrintZoneList(zoneList);
                break;
            default: printf("Valor invalido.\n");  break;
        }
    } while (input);
}

//Menu Stock Producto
int MenuAddProductToStock(struct ProductNode** products) {
    int ID;
    enum ProductType type;
    char* name = NULL;
    char* description = NULL;
    float price;
    float spacePerUnit;
    int amount;

    PrintProductIDsInAllBrands();

    ID = EnterInteger("Ingrese el ID del producto que quiere agregar: ");
    if (ID == -1) { return 1; }
    if (FindProductByIDInAllBrands(ID)) {
        printf("No se puede repetir el ID entre marcas/proveedores.\n\n");

        return 1;
    }
    printf("0. Vehiculo\n1. Pieza de repuesto\n2. Accesorio\n\n");
    type = EnterInteger("Ingrese el numero del tipo producto que quiere agregar: ");
    if (type >= END) { return 1; }
    name = EnterText("Ingrese el nombre del producto que quiere agregar: ");
    if (!name) { return 1; }
    description = EnterText("Ingrese la descripcion del producto que quiere agregar: ");
    if (!description) { return 1; }
    spacePerUnit = EnterFloatingPoint("Ingrese el espacio por unidad del producto (m^3) que quiere agregar: ");
    if (spacePerUnit== -1) { return 1; }
    amount = EnterInteger("Ingrese la cantidad del producto que quiere agregar: ");
    if (amount == -1) { return 1; }
    price = EnterFloatingPoint("Ingrese el precio del producto que quiere agregar: ");
    if (price == -1) { return 1; }

    return AddProduct(products, CreateProduct(CreateProductSharedData(ID, type, name, description, actualBrand->brand->name, spacePerUnit), amount, price));
}
int MenuDeleteStockProduct(struct ProductNode** stock) {
    return DeleteProduct(stock, EnterInteger("Ingrese el ID del producto que desea eliminar: "));
}
int MenuModifyStockProduct(struct ProductNode** products, struct ProductNode* product) {
    int input;
    enum ProductType type;

    if (!product) {
        product = FindProductByID(*products, EnterInteger("Ingrese el ID del producto a modificar: "));
        if (!product) { return 1; }
    }

    do {

        printf("--------------------------------------------------\n\n");
        PrintProduct(product->product);
        printf("--------------------------------------------------\n\n");
        printf("MENU MODIFICAR PRODUCTO EN STOCK:\n\n0. Atras.\n1. Modificar tipo de producto.\n2. Modificar nombre del producto. "
               "\n3. Modificar descripcion del producto.\n4. Modificar espacio por unidad del producto.\n5. Modificar la cantidad del producto.\n6. Modificar precio del producto.\n");

        if (product->product->sharedData->type == CAR) {
            printf("7. Modificar recalls del vehiculo.\n");
        }
        printf("\n");

        input = EnterInteger("Ingrese un numero: ");
        if (input == -1) { continue; }

        switch (input) {
            case 0: break;
            case 1:
                printf("0. Vehiculo\n1. Pieza de repuesto\n2. Accesorio\n\n");
                type = EnterInteger("Ingrese el numero del tipo producto que quiere agregar: ");
                if (type >= END) {
                    printf("Valor invalido.\n\n");

                } else { ChangeProductType(product, type); }
                break;
            case 2: ChangeProductName(product, EnterText("Ingrese el nombre del producto: ")); break;
            case 3: ChangeProductDescription(product, EnterText("Ingrese la descripcion del producto: ")); break;
            case 4: ChangeProductSpacePerUnit(product, EnterFloatingPoint("Ingrese el espacio por unidad del producto: ")); break;
            case 5: ChangeProductAmount(product, EnterInteger("Ingrese la cantidad del producto: ")); break;
            case 6: ChangeProductPrice(product, EnterFloatingPoint("Ingrese el precio del producto: ")); break;
        }

        if (product->product->sharedData->type == CAR && input == 7) {
            ChangeProductRecalls(product, EnterInteger("Ingrese recall del auto: "));
        }
        else if ((input > 7 && product->product->sharedData->type == CAR) || (input > 6 && product->product->sharedData->type != CAR)) {
            printf("Valor invalido.\n");
        }

    } while (input);

    return 0;
}
int MenuFindStockProduct(struct ProductNode** stock) {
    enum Find input;
    struct ProductNode* product;

    product = FindProductByID(*stock, EnterInteger("Ingrese el ID del producto que quiere buscar: "));
    if (!product) { return 1; }

    do {

        printf("--------------------------------------------------\n\n");
        PrintProduct(product->product);
        printf("--------------------------------------------------\n\n");
        printf("MENU PRODUCTO EN STOCK:\n\n0. Atras.\n1. Eliminar producto.\n2. Modificar datos del producto.\n\n");
        input = EnterInteger("Ingrese un numero: ");
        if (input == -1) { continue; }

        switch (input) {
            case 0: break;
            case fDELETE:
                if (DeleteProduct(stock, product->product->sharedData->ID)) {
                    printf("El producto no se ha podido eliminar correctamente.\n");

                }
                else {
                    printf("El producto se ha eliminado exitosamente.\n");

                    return 0;
                }
                break;
            case fMODIFY:
                MenuModifyStockProduct(stock, product);
                break;
            default: printf("Valor invalido.\n");  break;
        }
    } while (input);

    return 0;
}
void MenuStock(struct ProductNode** stock) {
    enum Menu input;

    do {

        printf("MENU PRINCIPAL STOCK:\n\n0. Atras\n1. Agregar producto\n2. Eliminar producto\n3. Buscar producto\n4. Modificar producto\n5. Mostrar stock.\n\n");
        input = EnterInteger("Ingrese un numero: ");
        if (input == -1) { continue; }
        if (1 < input && input < 5) { PrintProductIDs(*stock); }

        switch (input) {
            case 0: break;
            case mADD:
                if (MenuAddProductToStock(stock)) {
                    printf("El producto no se ha podido agregar correctamente.\n");
                } else { printf("El producto se ha agregado exitosamente.\n"); }

                break;
            case mDELETE:
                if (MenuDeleteStockProduct(stock)) {
                    printf("El producto no se ha podido eliminar correctamente.\n");
                } else { printf("El producto se ha eliminado exitosamente.\n"); }

                break;
            case mFIND:
                if (MenuFindStockProduct(stock)) {
                    printf("El producto no se ha podido encontrar.\n");
                }
                break;
            case mMODIFY1:
                if (MenuModifyStockProduct(stock, NULL)) {
                    printf("El producto no se ha podido encontrar.\n");
                }
                break;
            case mPRINT:
                PrintProducts(*stock);
                break;
            default: printf("Valor invalido.\n");  break;
        }
    } while (input);
}

//Menus Marcas
int MenuAddBrand(struct BrandNode** brands) {
    char* name;
    float productTravelTime;
    float status;

    name = EnterText("Ingrese el nombre de la marca/proveedor: ");
    if (!name) { return 1; }
    if (FindBrandByName(*brands, name)) {
        printf("No se puede repetir el nombre.\n\n");
        return 1;
    }
    productTravelTime = EnterFloatingPoint("Ingrese el tiempo de desplazamiento de mercancias (dias) de la marca/proveedor: ");
    if (productTravelTime == -1) { return 1; }
    status = EnterFloatingPoint("Ingrese la valoracion (0.0 - 5.0) de la marca/proveedor: ");
    if (status == -1) { return 1; }
    if (status > 5) {
        printf("Valor invalido\n\n");

        return 1;
    }

    return AddBrand(brands, CreateBrand(name, productTravelTime, status));
}
int MenuDeleteBrand(struct BrandNode** brands) {
    return DeleteBrand(brands, FindBrandByName(*brands, EnterText("Ingrese el nombre de la marca/proveedor que desea eliminar: ")));
}
int MenuModifyBrand(struct BrandNode** brands, struct BrandNode* brand) {
    int input;
    float status;
    char* name = NULL;

    if (!brand) {
        brand = FindBrandByName(*brands, EnterText("Ingrese el nombre de la marca/proveedor: "));
        if (!brand) { return 1; }
    }

    do {

        printf("--------------------------------------------------\n\n");
        PrintBrand(brand->brand);
        printf("--------------------------------------------------\n\n");
        printf("MENU MODIFICAR MARCA/PROVEEDOR:\n\n0. Atras.\n1. Modificar el nombre de la marca/proveedor.\n"
               "2. Modificar el tiempo de desplazamiento de mercancias de la marca/proveedor.\n3. Modificar la valoracion de la marca/proveedor.\n\n");
        input = EnterInteger("Ingrese un numero: ");
        if (input == -1) { continue; }

        switch (input) {
            case 0: break;
            case 1:
                name = EnterText("Ingrese el nombre de la marca/proveedor: ");
                if (!name) { break; }
                if (FindBrandByName(*brands, name)) {
                    printf("No se puede repetir el nombre.\n\n");

                }
                else { ChangeBrandName(brand, name); }
                break;
            case 2: ChangeBrandProductTravelTime(brand, EnterFloatingPoint("Ingrese el tiempo de desplazamiento de mercancias (dias) de la marca/proveedor: ")); break;
            case 3:
                status = EnterFloatingPoint("Ingrese la valoracion (0.0 - 5.0) de la marca/proveedor: ");
                if (status > 5) {
                    printf("\nValor invalido\n");

                }
                else { ChangeBrandStatus(brand, status); }
                break;
            default: printf("Valor invalido.\n");  break;
        }
    } while (input);

    return 0;
}
int MenuFindBrand(struct BrandNode** brands) {
    enum Find input;
    struct BrandNode* brand;

    brand = FindBrandByName(*brands, EnterText("Ingrese el nombre de la marca/proveedor que quieres buscar: "));
    if (!brand) { return 1; }

    actualBrand = brand;

    do {
        printf("--------------------------------------------------\n\n");
        PrintBrand(brand->brand);
        printf("--------------------------------------------------\n\n");
        printf("MENU MARCA/PROVEEDOR:\n\n0. Atras.\n1. Eliminar marca/proveedor.\n2. Modificar datos de la marca/proveedor.\n3. Modificar stock de la marca/proveedor.\n\n");
        input = EnterInteger("Ingrese un numero: ");
        if (input == -1) { continue; }

        switch (input) {
            case 0: break;
            case fDELETE:
                if (DeleteBrand(brands, brand)) {
                    printf("La marca/proveedor no se ha podido eliminar correctamente.\n");
                }
                else {
                    printf("La marca/proveedor se ha eliminado exitosamente.\n");
                    return 0;
                }
                break;
            case fMODIFY:
                MenuModifyBrand(brands, brand);
                break;
            case fDATA_STRUCTURE1:
                MenuStock(&brand->brand->stock);
                break;
            default: printf("Valor invalido.\n");  break;
        }
    } while (input);

    return 0;
}
void MenuBrands(struct BrandNode** brands) {
    enum Menu input;

    do {

        printf("MENU PRINCIPAL MARCAS/PROVEEDORES:\n\n0. Atras\n1. Agregar marca/proveedor\n2. Eliminar marca/proveedor\n"
               "3. Buscar marca/proveedor\n4. Modificar marca/proveedor\n5. Mostrar marcas/proveedores.\n\n");
        input = EnterInteger("Ingrese un numero: ");
        if (input == -1) { continue; }

        if (0 < input && input < 6) {  }
        if (0 < input && input < 5) { PrintBrandNames(*brands); }

        switch (input) {
            case 0: break;
            case mADD:
                if (MenuAddBrand(brands)) {
                    printf("La marca/proveedor no se ha podido agregar correctamente.\n");
                } else { printf("La marca/proveedor se ha agregado exitosamente.\n"); }

                break;
            case mDELETE:
                if (MenuDeleteBrand(brands)) {
                    printf("La marca/proveedor no se ha podido eliminar correctamente.\n");
                } else { printf("La marca/proveedor se ha eliminado exitosamente.\n"); }

                break;
            case mFIND:
                if (MenuFindBrand(brands)) {
                    printf("La marca/proveedor no se ha podido encontrar.\n");

                }
                break;
            case mMODIFY1:
                if (MenuModifyBrand(brands, NULL)) {
                    printf("La marca/proveedor no se ha podido encontrar.\n");

                }
                break;
            case mPRINT:
                PrintBrands(*brands);
                break;
            default: printf("Valor invalido.\n");  break;
        }
    } while (input);
}

//MAIN
int main() {
    int input;

    derco = (struct Derco*)malloc(sizeof(struct Derco));

    if (!derco) { return 1; }

    derco->zoneList = CreateZoneCircularList();
    derco->brands   = NULL;
    struct Customer* customer;
    struct Product* car;
    struct DistributionCenter *distributionCenter;
    struct Brand* brand;

    do {

        printf("MENU PRINCIPAL DERCO:\n\n0. Cerrar programa.\n1. Modificar lista de zonas.\n2. Modificar marcas/proveedores."
               "\n3. Cliente que mas ha gastado.\n4. Auto con mas recalls.\n5. Centro de distribucion con mayor numero de ventas\n"
               "6. Mostrar marca/proveedor con una valoracion mas baja.\n7. Mostrar producto de cierto tipo que Derco mas haya vendido.\n"
               "8. Mostrar monto recaudado de cada Centro de Distribucion\n\n");
        input = EnterInteger("Ingrese un numero : ");
        if (input == -1) { continue; }

        switch (input) {
            case 0: break;
            case 1: MenuZoneList(derco->zoneList); break;
            case 2: MenuBrands(&derco->brands); break;
            case 3:
                customer = CustomerThatHasSpendTheMost();
                if (!customer) {
                    printf("No se ha encontrado ningun cliente.\n\n");
                } else {
                    printf("--------------------------------------------------\n\n");
                    PrintCustomer(customer);
                    printf("--------------------------------------------------\n\n");
                }

                break;
            case 4:
                car = FindCarWithMoreRecalls();
                if (!car) {
                    printf("No se ha encontrado ningun auto.\n\n");
                }
                else {
                    printf("--------------------------------------------------\n\n");
                    PrintProduct(car);
                    printf("--------------------------------------------------\n\n");
                }

                break;
            case 5:
                distributionCenter = DistributionCenterWithMoreSales();
                if (!distributionCenter) {
                    printf("No se ha encontrado ningun centro de distribucion.\n\n");
                }
                else {
                    printf("--------------------------------------------------\n\n");
                    PrintDistributionCenter(distributionCenter);
                    printf("--------------------------------------------------\n\n");
                }

                break;
            case 6:
                brand = FindBrandWithLowestStatus();
                if (!brand) {
                    printf("No se ha encontrado ninguna marca/proveedor.\n\n");
                }
                else {
                    printf("--------------------------------------------------\n\n");
                    PrintBrand(brand);
                    printf("--------------------------------------------------\n\n");
                }

                break;

            case 7:
                printf("0. Vehiculo\n1. Pieza de repuesto\n2. Accesorio\n3. Todos los tipos\n\n");
                enum ProductType type = EnterInteger("Ingrese el tipo de producto: ");
                if (type == -1 || type > END) {
                    printf("Valor invalido.\n");

                } else {

                    struct ProductSharedData* product;
                    product = FindBestSellingProductByTypeInAllZones(type);
                    if (!product) {
                        printf("No se ha encontrado ningun producto de ese tipo.\n\n");
                    }
                    else {
                        printf("--------------------------------------------------\n\n");
                        PrintProductSharedData(product);
                        printf("--------------------------------------------------\n\n");
                    }
                }
                break;

            case 8:
                dineroRecaudadoPorCentroDistribucion();
                break;

            default: printf("Valor invalido.\n");  break;
        }
    }while (input);

    return 0;
}

