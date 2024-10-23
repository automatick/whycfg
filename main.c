#include "why.h"


int main(int argc, char* argv[]) {
    Config config = {0};

    if (parse_config(argv[1], &config) == 0) {
        if (check_required_classes(&config) == 0) {
            const char *value = get_value_by_key(&config, argv[2], argv[3]);
            if (value) {
                printf("Значение %s->%s: %s\n", argv[2], argv[3], value );
            } else {
                printf("Ключ 'mount' в секции 'server' не найден.\n");
            }
        } else {
            fprintf(stderr, "Конфигурация содержит ошибки.\n");
        }
    } else {
        fprintf(stderr, "Ошибка при парсинге конфигурации.\n");
    }
    for (int i = 0; i < config.required_count; i++) {
        free((char *)config.required_classes[i]);
    }

    return 0;
}
