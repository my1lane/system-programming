#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAGIC_NUM_BYTES 3  // Количество байтов для проверки "магических чисел" JPEG файла
#define WRITE_OPERATION  "write"   // Операция записи сообщения
#define READ_OPERATION   "read"    // Операция чтения сообщения
#define REMOVE_OPERATION "remove"  // Операция удаления сообщения

int main(int argc, char* argv[])
{
    // Проверяем, что передано достаточно аргументов
    if (argc < 3) {
        printf("Usage: %s write | read | remove [filename] [message]\n", argv[0]);
        return 1;
    }

    // Получаем операцию и имя файла из аргументов командной строки
    char *operation = argv[1];
    char *filename = argv[2];

    // Если операция "write", проверяем, что передано сообщение для записи
    if (strcmp(operation, WRITE_OPERATION) == 0 && argc < 4) {
        printf("Usage: %s write | read | remove [filename] [message]. Please specify message\n", argv[0]);
        return 1;
    }

    // Открываем файл для чтения в бинарном режиме
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Failed to open file %s\n", filename);
        return 1;
    }

    // Определяем размер файла
    fseek(file, 0, SEEK_END);
    const long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Выделяем память для хранения содержимого файла
    char *file_data;
    file_data = malloc(file_size);

    // Массив для хранения первых трех байтов файла (магические числа)
    char jpeg_magic_numbers[MAGIC_NUM_BYTES] = {0};
    
    // Ожидаемые магические числа для JPEG файла (0xFF, 0xD8, 0xFF)
    char expected_jpeg_magic[MAGIC_NUM_BYTES] = {0xFF, 0xD8, 0xFF};
    
    // Последовательность байтов, обозначающая конец JPEG файла (0xFF, 0xD9)
    char jpeg_end_marker[2] = {0xFF, 0xD9};

    // Читаем первые три байта файла для проверки магических чисел
    int bytes_read = fread(jpeg_magic_numbers, 1, MAGIC_NUM_BYTES, file);

    // Если не удалось прочитать магические числа, выводим ошибку
    if (bytes_read != MAGIC_NUM_BYTES) {
        printf("Failed to check magic bytes\n");
        return 1;
    }

    // Сравниваем прочитанные магические числа с ожидаемыми значениями для JPEG
    if (memcmp(jpeg_magic_numbers, expected_jpeg_magic, MAGIC_NUM_BYTES) != 0) {
        printf("%s is not a jpeg\n", filename);
        return 1;
    }

    // Читаем весь файл в память
    fseek(file, 0, SEEK_SET);
    fread(file_data, 1, file_size, file);
    
    // Закрываем файл после чтения
    fclose(file);

    long int jpeg_content_size = 0;  // Переменная для хранения размера JPEG данных

    // Ищем конец JPEG файла по последовательности байтов (0xFF, 0xD9)
    for (int i = 0; i < file_size - 1; i++) {
        char current_sequence[2] = {file_data[i], file_data[i + 1]};
        
        // Если находим конец JPEG файла (последовательность FF D9), сохраняем позицию
        if (memcmp(current_sequence, jpeg_end_marker, 2) == 0) {
            jpeg_content_size = i + 2;  // Размер JPEG данных включает два байта конца FF D9
            break;
        }
    }

    // Выполняем операцию в зависимости от команды пользователя
    if (strcmp(operation, WRITE_OPERATION) == 0) {
        char *message_to_write = argv[3];   // Сообщение для записи
        
        // Обрезаем файл до конца JPEG данных и открываем его для добавления текста
        truncate(filename, jpeg_content_size);
        file = fopen(filename, "a");
        
        // Записываем сообщение в файл после конца JPEG данных
        fputs(message_to_write, file);
        
        fclose(file);   // Закрываем файл после записи

    } else if (strcmp(operation, READ_OPERATION) == 0) {
        
        // Читаем и выводим все данные после конца JPEG файла (скрытое сообщение)
        for (int i = jpeg_content_size; i < file_size; i++) {
            printf("%c", file_data[i]);
        }
        
        printf("\n");   // Печатаем новую строку после вывода сообщения

    } else if (strcmp(operation, REMOVE_OPERATION) == 0) {
        
        // Удаляем все данные после конца JPEG данных путем обрезки файла
        truncate(filename, jpeg_content_size);
        
    }

    free(file_data);   // Освобождаем выделенную память

    return 0;
}
