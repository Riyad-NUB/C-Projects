#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include <unistd.h>

#define ADMIN_USERNAME "admin"
#define ADMIN_PASSWORD "12345"
#define MAX_VEHICLES 100
#define MAX_CUSTOMERS 200
#define MAX_RENTALS 200
#define FILENAME_VEHICLES "vehicles.dat"
#define FILENAME_CUSTOMERS "customers.dat"
#define FILENAME_RENTALS "rentals.dat"
#define FILENAME_TRANSACTIONS "transactions.dat"

typedef struct {
    int id;
    char name[50];
    char type[50];
    float rent_price;
    bool available;
} Vehicle;

typedef struct {
    char username[50];
    char password[50];
    char name[50];
    char email[50];
    char phone[15];
    char license[20];
    char address[100];
} Customer;

typedef struct {
    int rental_id;
    int vehicle_id;
    char customer_username[50];
    time_t rent_date;
    time_t return_date;
    float total_amount;
    bool returned;
    bool payment_completed;
} Rental;

Vehicle vehicles[MAX_VEHICLES];
Customer customers[MAX_CUSTOMERS];
Rental rentals[MAX_RENTALS];
int vehicleCount = 0;
int customerCount = 0;
int rentalCount = 0;
Customer* currentCustomer = NULL;

void clearInputBuffer();
void mainMenu();
void loadAllData();
void saveAllData();
int adminLogin();
void adminMenu();
void customerSignUp();
int customerLogin();
void customerMenu();
void displayVehicles(bool showAll);
void searchVehicleById();
void searchVehicleByName();
void searchVehicleByType();
void addVehicle();
void rentVehicle();
void deleteVehicle();
void updateVehicle();
void viewRentedVehicles();
void viewAllRentals();
void viewCustomerRentals();
void returnVehicle();
void processPayment(Rental *rental, Customer *customer, const char *paymentMethod, const char *paymentDetails);
void viewPaymentReceipts();
void printVehicleDetails(const Vehicle *v);
void printCustomerDetails(const Customer *c);
void printRentalDetails(const Rental *r);
bool isVehicleIdUnique(int id);
bool isUsernameUnique(const char *username);
bool isEmailValid(const char *email);
bool isPhoneValid(const char *phone);
bool isLicenseValid(const char *license);
bool isCardValid(const char *card);
bool isExpiryValid(const char *expiry);
bool isCVVValid(const char *cvv);
int getIntInput(const char *prompt);
float getFloatInput(const char *prompt);
void getStringInput(const char *prompt, char *buffer, int size);
void saveVehiclesToFile();
void loadVehiclesFromFile();
void saveCustomersToFile();
void loadCustomersFromFile();
void saveRentalsToFile();
void loadRentalsFromFile();
void printReceipt(const Rental *rental, const Customer *customer,
                 const Vehicle *vehicle, const char *paymentMethod,
                 const char *paymentDetails);
int findCustomerByUsername(const char *username);
void viewCustomerDatabase();

int main() {
    loadAllData();
    mainMenu();
    return 0;
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void printWelcomeScreen() {
    printf("\n\n");
    printf("================================================================\n");
    printf("WELCOME TO 'GARI BONDHU' (Largest Rental Vehicles service in BD)      \n");
    printf("================================================================\n");
    printf("                  Developed By Team Despair                     \n");
    printf("================================================================\n\n");
}

void mainMenu() {
    printWelcomeScreen();

    while (1) {
        printf("\n===== MAIN MENU =====\n");
        printf("1. Admin Login\n");
        printf("2. Customer Login\n");
        printf("3. Customer Sign Up\n");
        printf("4. Exit\n");
        printf("=====================\n");

        int choice = getIntInput("Enter your choice: ");

        switch (choice) {
            case 1:
                if (adminLogin()) {
                    adminMenu();
                }
                break;
            case 2:
                if (customerLogin()) {
                    customerMenu();
                }
                break;
            case 3:
                customerSignUp();
                break;
            case 4:
                saveAllData();
                printf("\nThank you for using Vehicle Rental System. Goodbye!\n");
                exit(0);
            default:
                printf("Invalid option. Please try again.\n");
        }
    }
}

int adminLogin() {
    char username[50];
    char password[50];
    int attempts = 3;

    while (attempts > 0) {
        printf("\n===== ADMIN LOGIN =====\n");
        getStringInput("Username: ", username, sizeof(username));
        getStringInput("Password: ", password, sizeof(password));

        if (strcmp(username, ADMIN_USERNAME) == 0 && strcmp(password, ADMIN_PASSWORD) == 0) {
            printf("\nLogin successful! Welcome, Admin.\n");
            return 1;
        }

        attempts--;
        if (attempts > 0) {
            printf("\nInvalid username or password. %d attempts remaining.\n", attempts);
        } else {
            printf("\nToo many failed attempts. Returning to main menu.\n");
        }
    }

    return 0;
}

void adminMenu() {
    while (1) {
        printf("\n===== ADMIN MENU =====\n");
        printf("1. Display All Vehicles\n");
        printf("2. Add Vehicle\n");
        printf("3. Update Vehicle\n");
        printf("4. Delete Vehicle\n");
        printf("5. Search Vehicle by ID\n");
        printf("6. Search Vehicle by Name\n");
        printf("7. Search Vehicle by Type\n");
        printf("8. View Rented Vehicles\n");
        printf("9. View All Rentals\n");
        printf("10. View Payment Receipts\n");
        printf("11. View Customer Database\n");
        printf("12. Logout\n");
        printf("======================\n");

        int choice = getIntInput("Enter your choice: ");

        switch (choice) {
            case 1:
                displayVehicles(true);
                break;
            case 2:
                addVehicle();
                break;
            case 3:
                updateVehicle();
                break;
            case 4:
                deleteVehicle();
                break;
            case 5:
                searchVehicleById();
                break;
            case 6:
                searchVehicleByName();
                break;
            case 7:
                searchVehicleByType();
                break;
            case 8:
                viewRentedVehicles();
                break;
            case 9:
                viewAllRentals();
                break;
            case 10:
                viewPaymentReceipts();
                break;
            case 11:
                viewCustomerDatabase();
                break;
            case 12:
                printf("Logging out...\n");
                return;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
}

void customerSignUp() {
    if (customerCount >= MAX_CUSTOMERS) {
        printf("Cannot register more customers. Database limit reached.\n");
        return;
    }

    Customer newCustomer;

    printf("\n===== CUSTOMER REGISTRATION =====\n");

    do {
        getStringInput("Choose a username: ", newCustomer.username, sizeof(newCustomer.username));
        if (!isUsernameUnique(newCustomer.username)) {
            printf("Username already exists. Please choose another.\n");
        }
    } while (!isUsernameUnique(newCustomer.username));

    getStringInput("Choose a password: ", newCustomer.password, sizeof(newCustomer.password));

    do {
        getStringInput("Full Name: ", newCustomer.name, sizeof(newCustomer.name));
    } while (strlen(newCustomer.name) == 0);

    do {
        getStringInput("Email: ", newCustomer.email, sizeof(newCustomer.email));
        if (!isEmailValid(newCustomer.email)) {
            printf("Invalid email format. Please try again.\n");
        }
    } while (!isEmailValid(newCustomer.email));

    do {
        getStringInput("Phone: ", newCustomer.phone, sizeof(newCustomer.phone));
        if (!isPhoneValid(newCustomer.phone)) {
            printf("Invalid phone number. Please enter at least 10 digits.\n");
        }
    } while (!isPhoneValid(newCustomer.phone));

    do {
        getStringInput("Driver's License: ", newCustomer.license, sizeof(newCustomer.license));
        if (!isLicenseValid(newCustomer.license)) {
            printf("Invalid license. Please enter at least 5 characters.\n");
        }
    } while (!isLicenseValid(newCustomer.license));

    getStringInput("Address: ", newCustomer.address, sizeof(newCustomer.address));

    customers[customerCount++] = newCustomer;
    saveAllData();
    printf("\nRegistration successful!\n");
    printf("You can now login with your username and password.\n");
}

int customerLogin() {
    char username[50];
    char password[50];

    printf("\n===== CUSTOMER LOGIN =====\n");
    getStringInput("Username: ", username, sizeof(username));
    getStringInput("Password: ", password, sizeof(password));

    for (int i = 0; i < customerCount; i++) {
        if (strcmp(customers[i].username, username) == 0 &&
            strcmp(customers[i].password, password) == 0) {
            currentCustomer = &customers[i];
            printf("\nLogin successful! Welcome back, %s.\n", customers[i].name);
            return 1;
        }
    }

    printf("Invalid username or password.\n");
    return 0;
}

void customerMenu() {
    if (currentCustomer == NULL) {
        printf("No customer logged in.\n");
        return;
    }

    while (1) {
        printf("\n===== CUSTOMER MENU =====\n");
        printf("Welcome, %s!\n", currentCustomer->name);
        printf("1. Display Available Vehicles\n");
        printf("2. Search Vehicle by ID\n");
        printf("3. Search Vehicle by Name\n");
        printf("4. Search Vehicle by Type\n");
        printf("5. Rent a Vehicle\n");
        printf("6. View My Rentals\n");
        printf("7. Return a Vehicle\n");
        printf("8. View My Profile\n");
        printf("9. Logout\n");
        printf("=========================\n");

        int choice = getIntInput("Enter your choice: ");

        switch (choice) {
            case 1:
                displayVehicles(false);
                break;
            case 2:
                searchVehicleById();
                break;
            case 3:
                searchVehicleByName();
                break;
            case 4:
                searchVehicleByType();
                break;
            case 5:
                rentVehicle();
                break;
            case 6:
                viewCustomerRentals();
                break;
            case 7:
                returnVehicle();
                break;
            case 8:
                printCustomerDetails(currentCustomer);
                break;
            case 9:
                currentCustomer = NULL;
                printf("Logged out successfully.\n");
                return;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
}

void printCustomerDetails(const Customer *c) {
    printf("\n===== Customer Details =====\n");
    printf("Username: %s\n", c->username);
    printf("Name: %s\n", c->name);
    printf("Email: %s\n", c->email);
    printf("Phone: %s\n", c->phone);
    printf("License: %s\n", c->license);
    printf("Address: %s\n", c->address);
    printf("============================\n");
}

void displayVehicles(bool showAll) {
    printf("\n===== %s Vehicles =====\n", showAll ? "All" : "Available");
    int found = 0;

    for (int i = 0; i < vehicleCount; i++) {
        if (showAll || vehicles[i].available) {
            printVehicleDetails(&vehicles[i]);
            found = 1;
        }
    }

    if (!found) {
        printf("No %s vehicles found.\n", showAll ? "" : "available");
    }
}

void printVehicleDetails(const Vehicle *v) {
    printf("ID: %-5d Name: %-20s Type: %-15s Rent: $%-8.2f/day %s\n",
           v->id, v->name, v->type, v->rent_price,
           v->available ? "(Available)" : "(Rented)");
}

void printRentalDetails(const Rental *r) {
    printf("\n===== Rental Details =====\n");
    printf("Rental ID: %d\n", r->rental_id);
    printf("Vehicle ID: %d\n", r->vehicle_id);
    printf("Customer: %s\n", r->customer_username);
    printf("Rent Date: %s", ctime(&r->rent_date));
    if (r->returned) {
        printf("Return Date: %s", ctime(&r->return_date));
        printf("Days Rented: %.1f\n",
               difftime(r->return_date, r->rent_date) / (60 * 60 * 24));
    }
    printf("Total Amount: $%.2f\n", r->total_amount);
    printf("Status: %s%s\n",
           r->returned ? "Returned" : "Active",
           r->payment_completed ? ", Paid" : ", Payment Pending");
    printf("=========================\n");
}

void searchVehicleById() {
    int id = getIntInput("Enter Vehicle ID to search: ");

    for (int i = 0; i < vehicleCount; i++) {
        if (vehicles[i].id == id) {
            printf("\nVehicle Found:\n");
            printVehicleDetails(&vehicles[i]);
            return;
        }
    }
    printf("Vehicle with ID %d not found.\n", id);
}

void searchVehicleByName() {
    char name[50];
    getStringInput("Enter Vehicle Name to search: ", name, sizeof(name));

    int found = 0;
    printf("\nSearch Results:\n");
    for (int i = 0; i < vehicleCount; i++) {
        if (strstr(vehicles[i].name, name) != NULL) {
            printVehicleDetails(&vehicles[i]);
            found = 1;
        }
    }

    if (!found) {
        printf("No vehicles found matching '%s'.\n", name);
    }
}

void searchVehicleByType() {
    char type[50];
    getStringInput("Enter Vehicle Type to search: ", type, sizeof(type));

    printf("\n===== Vehicles of Type: %s =====\n", type);
    int found = 0;

    for (int i = 0; i < vehicleCount; i++) {
        if (strcasecmp(vehicles[i].type, type) == 0) {
            printVehicleDetails(&vehicles[i]);
            found = 1;
        }
    }

    if (!found) {
        printf("No vehicles found of type '%s'.\n", type);
    }
}

bool isVehicleIdUnique(int id) {
    for (int i = 0; i < vehicleCount; i++) {
        if (vehicles[i].id == id) {
            return false;
        }
    }
    return true;
}

bool isUsernameUnique(const char *username) {
    for (int i = 0; i < customerCount; i++) {
        if (strcmp(customers[i].username, username) == 0) {
            return false;
        }
    }
    return true;
}

bool isEmailValid(const char *email) {
    return strchr(email, '@') != NULL && strchr(email, '.') != NULL;
}

bool isPhoneValid(const char *phone) {
    int digits = 0;
    for (int i = 0; phone[i]; i++) {
        if (isdigit(phone[i])) digits++;
    }
    return digits >= 10;
}

bool isLicenseValid(const char *license) {
    return strlen(license) >= 5;
}

bool isCardValid(const char *card) {
    if (strlen(card) != 16) return false;
    for (int i = 0; card[i]; i++) {
        if (!isdigit(card[i])) return false;
    }
    return true;
}

bool isExpiryValid(const char *expiry) {
    if (strlen(expiry) != 5 || expiry[2] != '/') return false;
    for (int i = 0; i < 5; i++) {
        if (i == 2) continue;
        if (!isdigit(expiry[i])) return false;
    }
    return true;
}

bool isCVVValid(const char *cvv) {
    if (strlen(cvv) < 3 || strlen(cvv) > 4) return false;
    for (int i = 0; cvv[i]; i++) {
        if (!isdigit(cvv[i])) return false;
    }
    return true;
}

int getIntInput(const char *prompt) {
    int value;
    char input[20];

    while (1) {
        printf("%s", prompt);
        fgets(input, sizeof(input), stdin);

        if (sscanf(input, "%d", &value) == 1) {
            return value;
        }

        printf("Invalid input. Please enter a valid integer.\n");
    }
}

float getFloatInput(const char *prompt) {
    float value;
    char input[20];

    while (1) {
        printf("%s", prompt);
        fgets(input, sizeof(input), stdin);

        if (sscanf(input, "%f", &value) == 1 && value > 0) {
            return value;
        }

        printf("Invalid input. Please enter a valid positive number.\n");
    }
}

void getStringInput(const char *prompt, char *buffer, int size) {
    printf("%s", prompt);
    if (fgets(buffer, size, stdin)) {
        buffer[strcspn(buffer, "\n")] = '\0';
    } else {
        buffer[0] = '\0';
    }
}

void loadAllData() {
    loadVehiclesFromFile();
    loadCustomersFromFile();
    loadRentalsFromFile();
}

void saveAllData() {
    saveVehiclesToFile();
    saveCustomersToFile();
    saveRentalsToFile();
}

void saveVehiclesToFile() {
    FILE *file = fopen(FILENAME_VEHICLES, "wb");
    if (!file) {
        printf("Error: Could not save vehicle data!\n");
        return;
    }
    fwrite(&vehicleCount, sizeof(int), 1, file);
    fwrite(vehicles, sizeof(Vehicle), vehicleCount, file);
    fclose(file);
}

void loadVehiclesFromFile() {
    FILE *file = fopen(FILENAME_VEHICLES, "rb");
    if (file) {
        fread(&vehicleCount, sizeof(int), 1, file);
        fread(vehicles, sizeof(Vehicle), vehicleCount, file);
        fclose(file);
    }
}

void saveCustomersToFile() {
    FILE *file = fopen(FILENAME_CUSTOMERS, "wb");
    if (!file) {
        printf("Error: Could not save customer data!\n");
        return;
    }
    fwrite(&customerCount, sizeof(int), 1, file);
    fwrite(customers, sizeof(Customer), customerCount, file);
    fclose(file);
}

void loadCustomersFromFile() {
    FILE *file = fopen(FILENAME_CUSTOMERS, "rb");
    if (file) {
        fread(&customerCount, sizeof(int), 1, file);
        fread(customers, sizeof(Customer), customerCount, file);
        fclose(file);
    }
}

void saveRentalsToFile() {
    FILE *file = fopen(FILENAME_RENTALS, "wb");
    if (!file) {
        printf("Error: Could not save rental data!\n");
        return;
    }
    fwrite(&rentalCount, sizeof(int), 1, file);
    fwrite(rentals, sizeof(Rental), rentalCount, file);
    fclose(file);
}

void loadRentalsFromFile() {
    FILE *file = fopen(FILENAME_RENTALS, "rb");
    if (file) {
        fread(&rentalCount, sizeof(int), 1, file);
        fread(rentals, sizeof(Rental), rentalCount, file);
        fclose(file);
    }
}

void addVehicle() {
    if (vehicleCount >= MAX_VEHICLES) {
        printf("Cannot add more vehicles. Storage limit reached.\n");
        return;
    }

    Vehicle newVehicle;

    while (1) {
        newVehicle.id = getIntInput("Enter Vehicle ID: ");
        if (isVehicleIdUnique(newVehicle.id)) {
            break;
        }
        printf("ID already exists. Please enter a unique ID.\n");
    }

    getStringInput("Enter Vehicle Name: ", newVehicle.name, sizeof(newVehicle.name));
    getStringInput("Enter Vehicle Type: ", newVehicle.type, sizeof(newVehicle.type));
    newVehicle.rent_price = getFloatInput("Enter Rental Price per Day: ");
    newVehicle.available = true;

    vehicles[vehicleCount++] = newVehicle;
    saveAllData();
    printf("Vehicle added successfully!\n");
}

void rentVehicle() {
    if (currentCustomer == NULL) {
        printf("No customer logged in.\n");
        return;
    }

    if (vehicleCount == 0) {
        printf("No vehicles available for rent.\n");
        return;
    }

    int id = getIntInput("Enter Vehicle ID to rent: ");
    int vehicleIndex = -1;

    for (int i = 0; i < vehicleCount; i++) {
        if (vehicles[i].id == id && vehicles[i].available) {
            vehicleIndex = i;
            break;
        }
    }

    if (vehicleIndex == -1) {
        printf("Vehicle not available or not found.\n");
        return;
    }

    int days;
    do {
        days = getIntInput("Enter number of rental days (1-30): ");
        if (days < 1 || days > 30) {
            printf("Please enter a value between 1 and 30.\n");
        }
    } while (days < 1 || days > 30);

    float totalAmount = vehicles[vehicleIndex].rent_price * days;
    printf("\nTotal Rental Cost: %.2f TK\n", totalAmount);

    char paymentMethod[20];
    char cardNumber[20] = "";
    char expiryDate[10] = "";
    char cvv[5] = "";

    while (1) {
        printf("\nSelect Payment Method:\n");
        printf("1. Cash\n");
        printf("2. Credit Card\n");
        printf("3. Debit Card\n");
        printf("4. bKash\n");
        int paymentChoice = getIntInput("Enter choice (1-4): ");

        switch (paymentChoice) {
            case 1:
                strcpy(paymentMethod, "cash");
                break;
            case 2:
                strcpy(paymentMethod, "credit");
                break;
            case 3:
                strcpy(paymentMethod, "debit");
                break;
            case 4:
                strcpy(paymentMethod, "bKash");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
                continue;
        }

        if (strcmp(paymentMethod, "credit") == 0 || strcmp(paymentMethod, "debit") == 0) {
            do {
                getStringInput("Card Number (16 digits): ", cardNumber, sizeof(cardNumber));
                if (!isCardValid(cardNumber)) {
                    printf("Invalid card number. Must be 16 digits.\n");
                }
            } while (!isCardValid(cardNumber));

            do {
                getStringInput("Expiry Date (MM/YY): ", expiryDate, sizeof(expiryDate));
                if (!isExpiryValid(expiryDate)) {
                    printf("Invalid expiry date. Use MM/YY format.\n");
                }
            } while (!isExpiryValid(expiryDate));

            do {
                getStringInput("CVV: ", cvv, sizeof(cvv));
                if (!isCVVValid(cvv)) {
                    printf("Invalid CVV. Must be 3 or 4 digits.\n");
                }
            } while (!isCVVValid(cvv));
        }
        else if (strcmp(paymentMethod, "bKash") == 0) {
            do {
                getStringInput("bKash Mobile Number (11 digits): ", cardNumber, sizeof(cardNumber));
                if (strlen(cardNumber) != 11) {
                    printf("Invalid bKash number. Must be 11 digits.\n");
                }
            } while (strlen(cardNumber) != 11);
        }

        break;
    }

    char confirm;
    printf("\nConfirm rental (Y/N)? ");
    scanf(" %c", &confirm);
    clearInputBuffer();

    if (toupper(confirm) != 'Y') {
        printf("Rental canceled.\n");
        return;
    }

    Rental newRental;
    newRental.rental_id = rentalCount > 0 ? rentals[rentalCount-1].rental_id + 1 : 1;
    newRental.vehicle_id = vehicles[vehicleIndex].id;
    strcpy(newRental.customer_username, currentCustomer->username);
    newRental.rent_date = time(NULL);
    newRental.return_date = 0;
    newRental.total_amount = totalAmount;
    newRental.returned = false;
    newRental.payment_completed = false;

    processPayment(&newRental, currentCustomer, paymentMethod, cardNumber);

    vehicles[vehicleIndex].available = false;
    rentals[rentalCount++] = newRental;
    saveAllData();

    printReceipt(&newRental, currentCustomer, &vehicles[vehicleIndex], paymentMethod, cardNumber);
    printf("\nVehicle rented successfully!\n");
}

void printReceipt(const Rental *rental, const Customer *customer,
                 const Vehicle *vehicle, const char *paymentMethod,
                 const char *paymentDetails) {
    time_t now = time(NULL);
    char dateStr[30];
    strftime(dateStr, sizeof(dateStr), "%d-%m-%Y %H:%M:%S", localtime(&now));

    int days = (int)ceil(rental->total_amount / vehicle->rent_price);
    char rentDateStr[30];
    strftime(rentDateStr, sizeof(rentDateStr), "%d-%m-%Y %H:%M", localtime(&rental->rent_date));

    printf("\n");
    printf("=====================================================\n");
    printf("             GARI BONDHU RENTAL SERVICE                   \n");
    printf("              204/1, Mirpur 1, Dhaka               \n");
    printf("               Tel: +880 1711-223344                 \n");
    printf("=====================================================\n");
    printf("                  RENTAL RECEIPT                     \n");
    printf("-----------------------------------------------------\n");
    printf(" Receipt No: R%05d           Date: %s\n", rental->rental_id, dateStr);
    printf("-----------------------------------------------------\n");
    printf(" CUSTOMER INFORMATION:\n");
    printf(" Name:    %-30s\n", customer->name);
    printf(" Contact: %-15s License: %s\n", customer->phone, customer->license);
    printf("-----------------------------------------------------\n");
    printf(" VEHICLE DETAILS:\n");
    printf(" ID:      %-5d\n", vehicle->id);
    printf(" Model:   %-20s\n", vehicle->name);
    printf(" Type:    %-15s\n", vehicle->type);
    printf(" Rate:    %-10.2f TK per day\n", vehicle->rent_price);
    printf("-----------------------------------------------------\n");
    printf(" RENTAL PERIOD:\n");
    printf(" Start Date:    %s\n", rentDateStr);
    printf(" Rental Days:   %d days\n", days);
    printf(" Total Amount:  %.2f TK\n", rental->total_amount);
    printf("-----------------------------------------------------\n");
    printf(" PAYMENT INFORMATION:\n");
    printf(" Method:  %s\n", paymentMethod);

    if (strcmp(paymentMethod, "bKash") == 0) {
        printf(" bKash No: %s\n", paymentDetails);
        printf(" TrxID:    BK%04d%04d\n", rand()%10000, rand()%10000);
    }
    else if (strcmp(paymentMethod, "credit") == 0 || strcmp(paymentMethod, "debit") == 0) {
        printf(" Card:     **** **** **** %s\n", paymentDetails + strlen(paymentDetails) - 4);
        printf(" Auth:     %04d\n", rand()%10000);
    }
    else {
        printf(" Payment will be collected on vehicle pickup\n");
    }

    printf(" Status:   %s\n", rental->payment_completed ? "PAID" : "PENDING");
    printf("-----------------------------------------------------\n");
    printf(" TERMS & CONDITIONS:\n");
    printf(" 1. Valid ID required for vehicle pickup\n");
    printf(" 2. Late return: 20%% of daily rate per hour\n");
    printf(" 3. Fuel policy: Full-to-Full\n");
    printf(" 4. Damage charges apply as per assessment\n");
    printf("===========================================================\n");
    printf("     Thank you for choosing Gari Bondhu Rental Service      \n");
    printf("===========================================================\n\n");

    FILE *file = fopen("receipt_log.txt", "a");
    if (file) {
        fprintf(file, "RECEIPT R%05d\n", rental->rental_id);
        fprintf(file, "Date: %s\n", dateStr);
        fprintf(file, "Customer: %s (%s)\n", customer->name, customer->phone);
        fprintf(file, "Vehicle: %s (ID: %d)\n", vehicle->name, vehicle->id);
        fprintf(file, "Rental Period: %s for %d days\n", rentDateStr, days);
        fprintf(file, "Amount: %.2f TK\n", rental->total_amount);
        fprintf(file, "Payment: %s", paymentMethod);
        if (strcmp(paymentMethod, "bKash") == 0) {
            fprintf(file, " (%s)", paymentDetails);
        }
        fprintf(file, "\nStatus: %s\n", rental->payment_completed ? "PAID" : "PENDING");
        fprintf(file, "--------------------------------------------\n");
        fclose(file);
    }
}

void processPayment(Rental *rental, Customer *customer, const char *paymentMethod, const char *paymentDetails) {
    printf("\n===== Payment Processing =====\n");
    printf("Amount Due: %.2f TK\n", rental->total_amount);
    printf("Payment Method: %s\n", paymentMethod);

    if (strcmp(paymentMethod, "cash") == 0) {
        printf("Please pay %.2f TK in cash when you pick up the vehicle.\n", rental->total_amount);
        rental->payment_completed = false;
    } else {
        printf("Processing %s payment...\n", paymentMethod);
        printf("Charging %.2f TK\n", rental->total_amount);
        if (strcmp(paymentMethod, "bKash") == 0) {
            printf("to bKash number: %s\n", paymentDetails);
        } else {
            printf("to card ending in %s\n", paymentDetails + strlen(paymentDetails) - 4);
        }
        printf("Payment approved!\n");
        rental->payment_completed = true;
    }

    FILE *file = fopen(FILENAME_TRANSACTIONS, "a");
    if (file) {
        fprintf(file, "Rental ID: %d\n", rental->rental_id);
        fprintf(file, "Date: %s", ctime(&rental->rent_date));
        fprintf(file, "Customer: %s\n", rental->customer_username);
        fprintf(file, "Vehicle ID: %d\n", rental->vehicle_id);
        fprintf(file, "Amount: %.2f TK\n", rental->total_amount);
        fprintf(file, "Payment Method: %s\n", paymentMethod);
        if (strcmp(paymentMethod, "bKash") == 0) {
            fprintf(file, "bKash Number: %s\n", paymentDetails);
        } else if (strcmp(paymentMethod, "cash") != 0) {
            fprintf(file, "Card: **** **** **** %s\n", paymentDetails + strlen(paymentDetails) - 4);
        }
        fprintf(file, "Status: %s\n\n", rental->payment_completed ? "Paid" : "Pending");
        fclose(file);
    }
}

void returnVehicle() {
    if (currentCustomer == NULL) {
        printf("No customer logged in.\n");
        return;
    }

    int rentalId = getIntInput("Enter Rental ID to return: ");
    int rentalIndex = -1;

    for (int i = 0; i < rentalCount; i++) {
        if (rentals[i].rental_id == rentalId &&
            strcmp(rentals[i].customer_username, currentCustomer->username) == 0 &&
            !rentals[i].returned) {
            rentalIndex = i;
            break;
        }
    }

    if (rentalIndex == -1) {
        printf("Active rental with ID %d not found for your account.\n", rentalId);
        return;
    }

    int vehicleIndex = -1;
    for (int i = 0; i < vehicleCount; i++) {
        if (vehicles[i].id == rentals[rentalIndex].vehicle_id) {
            vehicleIndex = i;
            break;
        }
    }

    if (vehicleIndex == -1) {
        printf("Associated vehicle not found!\n");
        return;
    }

    time_t returnTime = time(NULL);
    double actualDays = difftime(returnTime, rentals[rentalIndex].rent_date) / (60 * 60 * 24);
    int roundedDays = (int)ceil(actualDays);
    float actualAmount = vehicles[vehicleIndex].rent_price * roundedDays;

    printf("\n===== Return Details =====\n");
    printf("Vehicle: %s (ID: %d)\n", vehicles[vehicleIndex].name, vehicles[vehicleIndex].id);
    printf("Rented by: %s\n", rentals[rentalIndex].customer_username);
    printf("Rented on: %s", ctime(&rentals[rentalIndex].rent_date));
    printf("Returned on: %s", ctime(&returnTime));
    printf("Days rented: %.1f (rounded to %d days)\n", actualDays, roundedDays);

    if (!rentals[rentalIndex].payment_completed) {
        printf("Original amount: $%.2f\n", rentals[rentalIndex].total_amount);
        printf("Actual amount due: $%.2f\n", actualAmount);

        if (actualAmount > rentals[rentalIndex].total_amount) {
            float balance = actualAmount - rentals[rentalIndex].total_amount;
            printf("Additional balance due: $%.2f\n", balance);

            printf("Processing additional payment...\n");
            printf("Please pay %.2f TK now.\n", balance);
            printf("Payment received. Thank you!\n");
        } else if (actualAmount < rentals[rentalIndex].total_amount) {
            float refund = rentals[rentalIndex].total_amount - actualAmount;
            printf("Refund due: $%.2f\n", refund);
        }

        rentals[rentalIndex].total_amount = actualAmount;
        rentals[rentalIndex].payment_completed = true;
    }

    rentals[rentalIndex].returned = true;
    rentals[rentalIndex].return_date = returnTime;
    vehicles[vehicleIndex].available = true;

    saveAllData();

    printf("\n===== Return Receipt =====\n");
    printRentalDetails(&rentals[rentalIndex]);
    printf("Vehicle returned successfully!\n");
}

void viewRentedVehicles() {
    printf("\n===== Currently Rented Vehicles =====\n");
    int found = 0;
    time_t now = time(NULL);

    for (int i = 0; i < rentalCount; i++) {
        if (!rentals[i].returned) {
            double daysRented = difftime(now, rentals[i].rent_date) / (60 * 60 * 24);
            printf("Rental ID: %d\n", rentals[i].rental_id);
            printf("Vehicle ID: %d\n", rentals[i].vehicle_id);
            printf("Customer: %s\n", rentals[i].customer_username);
            printf("Days rented: %.1f\n", daysRented);
            printf("Amount Due: $%.2f\n", rentals[i].total_amount);
            printf("Payment Status: %s\n\n",
                   rentals[i].payment_completed ? "Paid" : "Pending");
            found = 1;
        }
    }

    if (!found) {
        printf("No vehicles are currently rented.\n");
    }
}

void viewAllRentals() {
    printf("\n===== All Rental Records =====\n");
    if (rentalCount == 0) {
        printf("No rental records found.\n");
        return;
    }

    for (int i = 0; i < rentalCount; i++) {
        printRentalDetails(&rentals[i]);
    }
}

void viewCustomerRentals() {
    if (currentCustomer == NULL) {
        printf("No customer logged in.\n");
        return;
    }

    printf("\n===== Your Rental History =====\n");
    int found = 0;

    for (int i = 0; i < rentalCount; i++) {
        if (strcmp(rentals[i].customer_username, currentCustomer->username) == 0) {
            printRentalDetails(&rentals[i]);
            found = 1;
        }
    }

    if (!found) {
        printf("You have no rental history.\n");
    }
}

void viewCustomerDatabase() {
    printf("\n===== Customer Database =====\n");
    if (customerCount == 0) {
        printf("No customers registered.\n");
        return;
    }

    for (int i = 0; i < customerCount; i++) {
        printCustomerDetails(&customers[i]);
    }
}

void viewPaymentReceipts() {
    FILE *file = fopen(FILENAME_TRANSACTIONS, "r");
    if (file == NULL) {
        printf("No payment records found yet.\n");
        return;
    }

    printf("\n===== Payment Receipts =====\n");
    char line[200];
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }
    fclose(file);
}

void deleteVehicle() {
    int id = getIntInput("Enter Vehicle ID to delete: ");
    int vehicleIndex = -1;

    for (int i = 0; i < vehicleCount; i++) {
        if (vehicles[i].id == id) {
            vehicleIndex = i;
            break;
        }
    }

    if (vehicleIndex == -1) {
        printf("Vehicle with ID %d not found.\n", id);
        return;
    }

    for (int i = 0; i < rentalCount; i++) {
        if (rentals[i].vehicle_id == id && !rentals[i].returned) {
            printf("Cannot delete a rented vehicle. Please return it first.\n");
            return;
        }
    }

    printf("\nVehicle to be deleted:\n");
    printVehicleDetails(&vehicles[vehicleIndex]);

    char confirm;
    printf("Are you sure you want to delete this vehicle? (Y/N): ");
    scanf(" %c", &confirm);
    clearInputBuffer();

    if (toupper(confirm) != 'Y') {
        printf("Deletion canceled.\n");
        return;
    }

    for (int i = vehicleIndex; i < vehicleCount - 1; i++) {
        vehicles[i] = vehicles[i + 1];
    }
    vehicleCount--;

    saveAllData();
    printf("Vehicle deleted successfully!\n");
}

void updateVehicle() {
    int id = getIntInput("Enter Vehicle ID to update: ");
    int vehicleIndex = -1;

    for (int i = 0; i < vehicleCount; i++) {
        if (vehicles[i].id == id) {
            vehicleIndex = i;
            break;
        }
    }

    if (vehicleIndex == -1) {
        printf("Vehicle with ID %d not found.\n", id);
        return;
    }

    for (int i = 0; i < rentalCount; i++) {
        if (rentals[i].vehicle_id == id && !rentals[i].returned) {
            printf("Cannot update a rented vehicle. Please return it first.\n");
            return;
        }
    }

    printf("\nCurrent Vehicle Details:\n");
    printVehicleDetails(&vehicles[vehicleIndex]);

    printf("\nEnter new details (leave blank to keep current value):\n");

    char name[50];
    getStringInput("Name: ", name, sizeof(name));
    if (strlen(name) > 0) {
        strcpy(vehicles[vehicleIndex].name, name);
    }

    char type[50];
    getStringInput("Type: ", type, sizeof(type));
    if (strlen(type) > 0) {
        strcpy(vehicles[vehicleIndex].type, type);
    }

    char price[20];
    getStringInput("Price: ", price, sizeof(price));
    if (strlen(price) > 0) {
        vehicles[vehicleIndex].rent_price = atof(price);
    }

    saveAllData();
    printf("\nUpdated Vehicle Details:\n");
    printVehicleDetails(&vehicles[vehicleIndex]);
    printf("Vehicle updated successfully!\n");
}

int findCustomerByUsername(const char *username) {
    for (int i = 0; i < customerCount; i++) {
        if (strcmp(customers[i].username, username) == 0) {
            return i;
        }
    }
    return -1;
}
