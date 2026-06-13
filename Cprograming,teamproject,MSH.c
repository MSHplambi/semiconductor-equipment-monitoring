#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEMP_ERROR 1
#define VOLT_ERROR 2
#define PRESS_ERROR 4
#define VIB_ERROR 8
#define SENSOR_ERROR 16

typedef struct {
    int tempMin, tempMax;
    int voltMin, voltMax;
    int pressMin, pressMax;
    int vibMin, vibMax;
} Standard;

typedef struct {
    char name[30];
    char process[30];
    Standard standard;
    int temperature;
    int voltage;
    int pressure;
    int vibration;
    int sensor;
    int errorCode;
    char finalStatus[20];
} Equipment;

void printMenu();
int setDefaultStandard(char name[], Standard* std, char process[]);
void inputCustomStandard(Standard* std);
void inputEquipment(Equipment* list, int* count, int maxCount);
int checkStatus(Equipment* eq);
void setFinalStatus(Equipment* eq);
void printOneEquipment(Equipment eq);
void printAllEquipment(Equipment* list, int count);
void printAbnormalEquipment(Equipment* list, int count);
void searchByErrorType(Equipment* list, int count);
void saveToFile(Equipment* list, int count);

int main() {
    int maxCount;
    int count = 0;
    int menu;

    printf("Input max equipment count: ");
    scanf("%d", &maxCount);

    Equipment* list = (Equipment*)malloc(sizeof(Equipment) * maxCount);

    if (list == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    while (1) {
        printMenu();
        scanf("%d", &menu);

        if (menu == 1) {
            inputEquipment(list, &count, maxCount);
        }
        else if (menu == 2) {
            printAllEquipment(list, count);
        }
        else if (menu == 3) {
            printAbnormalEquipment(list, count);
        }
        else if (menu == 4) {
            searchByErrorType(list, count);
        }
        else if (menu == 5) {
            saveToFile(list, count);
        }
        else if (menu == 0) {
            printf("Program ended.\n");
            break;
        }
        else {
            printf("Invalid menu.\n");
        }
    }

    free(list);
    return 0;
}

void printMenu() {
    printf("\n===== Semiconductor Equipment Monitoring System =====\n");
    printf("1. Add equipment data\n");
    printf("2. Print all equipment\n");
    printf("3. Print abnormal equipment only\n");
    printf("4. Search by error type\n");
    printf("5. Save data to file\n");
    printf("0. Exit\n");
    printf("Select menu: ");
}

int setDefaultStandard(char name[], Standard* std, char process[]) {
    if (strcmp(name, "Etcher") == 0 || strcmp(name, "etcher") == 0) {
        strcpy(process, "Etching");
        std->tempMin = 20;  std->tempMax = 90;
        std->voltMin = 180; std->voltMax = 240;
        std->pressMin = 40; std->pressMax = 100;
        std->vibMin = 0;    std->vibMax = 30;
        return 1;
    }
    else if (strcmp(name, "CVD") == 0 || strcmp(name, "cvd") == 0) {
        strcpy(process, "Deposition");
        std->tempMin = 30;  std->tempMax = 120;
        std->voltMin = 180; std->voltMax = 240;
        std->pressMin = 30; std->pressMax = 110;
        std->vibMin = 0;    std->vibMax = 35;
        return 1;
    }
    else if (strcmp(name, "Lithography") == 0 || strcmp(name, "lithography") == 0) {
        strcpy(process, "Exposure");
        std->tempMin = 20;  std->tempMax = 70;
        std->voltMin = 190; std->voltMax = 230;
        std->pressMin = 50; std->pressMax = 100;
        std->vibMin = 0;    std->vibMax = 20;
        return 1;
    }
    else if (strcmp(name, "CMP") == 0 || strcmp(name, "cmp") == 0) {
        strcpy(process, "Planarization");
        std->tempMin = 20;  std->tempMax = 80;
        std->voltMin = 180; std->voltMax = 240;
        std->pressMin = 40; std->pressMax = 120;
        std->vibMin = 0;    std->vibMax = 40;
        return 1;
    }

    return 0;
}

void inputCustomStandard(Standard* std) {
    printf("\nThis equipment is not registered.\n");
    printf("Input custom standard range.\n");

    printf("Temperature min: ");
    scanf("%d", &std->tempMin);
    printf("Temperature max: ");
    scanf("%d", &std->tempMax);

    printf("Voltage min: ");
    scanf("%d", &std->voltMin);
    printf("Voltage max: ");
    scanf("%d", &std->voltMax);

    printf("Pressure min: ");
    scanf("%d", &std->pressMin);
    printf("Pressure max: ");
    scanf("%d", &std->pressMax);

    printf("Vibration min: ");
    scanf("%d", &std->vibMin);
    printf("Vibration max: ");
    scanf("%d", &std->vibMax);
}

void inputEquipment(Equipment* list, int* count, int maxCount) {
    Equipment eq;
    int isRegistered;

    if (*count >= maxCount) {
        printf("Cannot add more equipment.\n");
        return;
    }

    printf("\nEquipment name(Etcher/CVD/Lithography/CMP/etc): ");
    scanf("%s", eq.name);

    isRegistered = setDefaultStandard(eq.name, &eq.standard, eq.process);

    if (isRegistered == 1) {
        printf("Registered equipment. Default standard applied.\n");
    }
    else {
        printf("Process type: ");
        scanf("%s", eq.process);
        inputCustomStandard(&eq.standard);
    }

    printf("\nInput measured data.\n");

    printf("Temperature: ");
    scanf("%d", &eq.temperature);

    printf("Voltage: ");
    scanf("%d", &eq.voltage);

    printf("Pressure: ");
    scanf("%d", &eq.pressure);

    printf("Vibration: ");
    scanf("%d", &eq.vibration);

    printf("Sensor status(1=Normal, 0=Error): ");
    scanf("%d", &eq.sensor);

    eq.errorCode = checkStatus(&eq);
    setFinalStatus(&eq);

    list[*count] = eq;
    (*count)++;

    printf("Equipment data saved.\n");
}

int checkStatus(Equipment* eq) {
    int error = 0;

    if (eq->temperature < eq->standard.tempMin || eq->temperature > eq->standard.tempMax) {
        error = error | TEMP_ERROR;
    }

    if (eq->voltage < eq->standard.voltMin || eq->voltage > eq->standard.voltMax) {
        error = error | VOLT_ERROR;
    }

    if (eq->pressure < eq->standard.pressMin || eq->pressure > eq->standard.pressMax) {
        error = error | PRESS_ERROR;
    }

    if (eq->vibration < eq->standard.vibMin || eq->vibration > eq->standard.vibMax) {
        error = error | VIB_ERROR;
    }

    if (eq->sensor == 0) {
        error = error | SENSOR_ERROR;
    }

    return error;
}

void setFinalStatus(Equipment* eq) {
    int errorCount = 0;

    if (eq->errorCode & TEMP_ERROR) errorCount++;
    if (eq->errorCode & VOLT_ERROR) errorCount++;
    if (eq->errorCode & PRESS_ERROR) errorCount++;
    if (eq->errorCode & VIB_ERROR) errorCount++;
    if (eq->errorCode & SENSOR_ERROR) errorCount++;

    if (eq->errorCode & SENSOR_ERROR) {
        strcpy(eq->finalStatus, "Immediate_Check");
    }
    else if (errorCount == 0) {
        strcpy(eq->finalStatus, "Normal");
    }
    else if (errorCount == 1) {
        strcpy(eq->finalStatus, "Caution");
    }
    else {
        strcpy(eq->finalStatus, "Danger");
    }
}

void printOneEquipment(Equipment eq) {
    printf("\n-----------------------------\n");
    printf("Name: %s\n", eq.name);
    printf("Process: %s\n", eq.process);

    printf("[Standard Range]\n");
    printf("Temperature: %d ~ %d\n", eq.standard.tempMin, eq.standard.tempMax);
    printf("Voltage: %d ~ %d\n", eq.standard.voltMin, eq.standard.voltMax);
    printf("Pressure: %d ~ %d\n", eq.standard.pressMin, eq.standard.pressMax);
    printf("Vibration: %d ~ %d\n", eq.standard.vibMin, eq.standard.vibMax);

    printf("[Measured Data]\n");
    printf("Temperature: %d\n", eq.temperature);
    printf("Voltage: %d\n", eq.voltage);
    printf("Pressure: %d\n", eq.pressure);
    printf("Vibration: %d\n", eq.vibration);
    printf("Sensor: %s\n", eq.sensor == 1 ? "Normal" : "Error");

    printf("Final Status: %s\n", eq.finalStatus);
    printf("Error Type: ");

    if (eq.errorCode == 0) {
        printf("None");
    }
    else {
        if (eq.errorCode & TEMP_ERROR) printf("Temperature_Error ");
        if (eq.errorCode & VOLT_ERROR) printf("Voltage_Error ");
        if (eq.errorCode & PRESS_ERROR) printf("Pressure_Error ");
        if (eq.errorCode & VIB_ERROR) printf("Vibration_Error ");
        if (eq.errorCode & SENSOR_ERROR) printf("Sensor_Error ");
    }

    printf("\n");
}

void printAllEquipment(Equipment* list, int count) {
    int i;

    if (count == 0) {
        printf("No equipment data.\n");
        return;
    }

    printf("\n===== All Equipment List =====\n");

    for (i = 0; i < count; i++) {
        printOneEquipment(list[i]);
    }
}

void printAbnormalEquipment(Equipment* list, int count) {
    int i;
    int found = 0;

    if (count == 0) {
        printf("No equipment data.\n");
        return;
    }

    printf("\n===== Abnormal Equipment List =====\n");

    for (i = 0; i < count; i++) {
        if (list[i].errorCode != 0) {
            printOneEquipment(list[i]);
            found = 1;
        }
    }

    if (found == 0) {
        printf("No abnormal equipment.\n");
    }
}

void searchByErrorType(Equipment* list, int count) {
    int choice;
    int targetError = 0;
    int i;
    int found = 0;

    if (count == 0) {
        printf("No equipment data.\n");
        return;
    }

    printf("\nSelect error type.\n");
    printf("1. Temperature Error\n");
    printf("2. Voltage Error\n");
    printf("3. Pressure Error\n");
    printf("4. Vibration Error\n");
    printf("5. Sensor Error\n");
    printf("Select: ");
    scanf("%d", &choice);

    if (choice == 1) targetError = TEMP_ERROR;
    else if (choice == 2) targetError = VOLT_ERROR;
    else if (choice == 3) targetError = PRESS_ERROR;
    else if (choice == 4) targetError = VIB_ERROR;
    else if (choice == 5) targetError = SENSOR_ERROR;
    else {
        printf("Invalid choice.\n");
        return;
    }

    printf("\n===== Search Result =====\n");

    for (i = 0; i < count; i++) {
        if (list[i].errorCode & targetError) {
            printOneEquipment(list[i]);
            found = 1;
        }
    }

    if (found == 0) {
        printf("No equipment with this error type.\n");
    }
}

void saveToFile(Equipment* list, int count) {
    FILE* fp;
    int i;

    fp = fopen("equipment_log.txt", "w");

    if (fp == NULL) {
        printf("File save failed.\n");
        return;
    }

    fprintf(fp, "===== Semiconductor Equipment Monitoring Result =====\n");

    for (i = 0; i < count; i++) {
        fprintf(fp, "\n-----------------------------\n");
        fprintf(fp, "Name: %s\n", list[i].name);
        fprintf(fp, "Process: %s\n", list[i].process);

        fprintf(fp, "[Standard Range]\n");
        fprintf(fp, "Temperature: %d ~ %d\n", list[i].standard.tempMin, list[i].standard.tempMax);
        fprintf(fp, "Voltage: %d ~ %d\n", list[i].standard.voltMin, list[i].standard.voltMax);
        fprintf(fp, "Pressure: %d ~ %d\n", list[i].standard.pressMin, list[i].standard.pressMax);
        fprintf(fp, "Vibration: %d ~ %d\n", list[i].standard.vibMin, list[i].standard.vibMax);

        fprintf(fp, "[Measured Data]\n");
        fprintf(fp, "Temperature: %d\n", list[i].temperature);
        fprintf(fp, "Voltage: %d\n", list[i].voltage);
        fprintf(fp, "Pressure: %d\n", list[i].pressure);
        fprintf(fp, "Vibration: %d\n", list[i].vibration);
        fprintf(fp, "Sensor: %s\n", list[i].sensor == 1 ? "Normal" : "Error");

        fprintf(fp, "Final Status: %s\n", list[i].finalStatus);

        fprintf(fp, "Error Type: ");
        if (list[i].errorCode == 0) {
            fprintf(fp, "None");
        }
        else {
            if (list[i].errorCode & TEMP_ERROR) fprintf(fp, "Temperature_Error ");
            if (list[i].errorCode & VOLT_ERROR) fprintf(fp, "Voltage_Error ");
            if (list[i].errorCode & PRESS_ERROR) fprintf(fp, "Pressure_Error ");
            if (list[i].errorCode & VIB_ERROR) fprintf(fp, "Vibration_Error ");
            if (list[i].errorCode & SENSOR_ERROR) fprintf(fp, "Sensor_Error ");
        }

        fprintf(fp, "\n");
    }

    fclose(fp);

    printf("Saved to equipment_log.txt\n");
}