#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256
#define MAX_VALUE_LENGTH 64
#define MAX_ITEMS 10
#define MAX_SECTION_NAME_LENGTH 32
#define MAX_REQUIRED_CLASSES 5

typedef struct {
    char key[MAX_VALUE_LENGTH];
    char value[MAX_VALUE_LENGTH];
} KeyValue;

typedef struct {
    char name[MAX_SECTION_NAME_LENGTH];
    KeyValue items[MAX_ITEMS];
    int count;
} ConfigSection;

typedef struct {
    ConfigSection sections[MAX_ITEMS];
    int section_count;
    const char *required_classes[MAX_REQUIRED_CLASSES];
    int required_count;
} Config;

void add_key_value(ConfigSection *section, const char *key, const char *value) {
    if (section->count < MAX_ITEMS) {
        strncpy(section->items[section->count].key, key, MAX_VALUE_LENGTH);
        strncpy(section->items[section->count].value, value, MAX_VALUE_LENGTH);
        section->count++;
    }
}

const char* get_value_by_key(const Config *config, const char *section_name, const char *key) {
    for (int i = 0; i < config->section_count; i++) {
        if (strcmp(config->sections[i].name, section_name) == 0) {
            for (int j = 0; j < config->sections[i].count; j++) {
                if (strcmp(config->sections[i].items[j].key, key) == 0) {
                    return config->sections[i].items[j].value;
                }
            }
        }
    }
    return NULL;
}

int parse_config(const char *filename, Config *config) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Ошибка открытия файла");
        return -1;
    }

    char line[MAX_LINE_LENGTH];
    ConfigSection current_section = {0};

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "REQUIRED", 8) == 0) {
            char class_name[MAX_VALUE_LENGTH];
            sscanf(line, "REQUIRED - class: %s", class_name);
            config->required_classes[config->required_count++] = strdup(class_name);
            continue;
        } else if (strncmp(line, "OPTIONAL", 8) == 0) {
            char class_name[MAX_VALUE_LENGTH];
            sscanf(line, "OPTIONAL - class: %s", class_name);
            continue;
        }

        line[strcspn(line, "\n")] = 0;

        if (line[0] == '\0' || line[0] == '#') {
            continue;
        }
        if (line[0] == '[') {
            if (current_section.count > 0) {
                config->sections[config->section_count++] = current_section;
                current_section.count = 0;
            }
            sscanf(line, "[%31[^]]", current_section.name);
            continue;
        }

        char key[MAX_VALUE_LENGTH];
        char value[MAX_VALUE_LENGTH];

        if (sscanf(line, "> %s : %s", key, value) == 2) {
            add_key_value(&current_section, key, value);
        }
    }

    if (current_section.count > 0) {
        config->sections[config->section_count++] = current_section;
    }

    fclose(file);
    return 0;
}

int check_required_classes(const Config *config) {
    for (int i = 0; i < config->required_count; i++) {
        int found = 0;
        for (int j = 0; j < config->section_count; j++) {
            if (strcmp(config->sections[j].name, config->required_classes[i]) == 0) {
                found = 1;
                break;
            }
        }
        if (!found) {
            fprintf(stderr, "Ошибка: отсутствует обязательный класс: %s\n", config->required_classes[i]);
            return -1;
        }
    }
    return 0;
}

void print_config(const Config *config) {
    for (int i = 0; i < config->section_count; i++) {
        printf("Section: %s\n", config->sections[i].name);
        for (int j = 0; j < config->sections[i].count; j++) {
            printf("  %s : %s\n", config->sections[i].items[j].key, config->sections[i].items[j].value);
        }
    }
}
