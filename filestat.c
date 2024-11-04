#include <stdio.h> 
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <dirent.h> 

int main(int argc, char *argv[]) {
    DIR* dir;  // Указатель на директорию
    struct dirent* dir_entry;  // Структура для хранения информации о файле или поддиректории в каталоге
    struct stat dir_entry_stat;  // Структура для хранения информации о файле, возвращаемой функцией stat()
    const char* current_dir_path = ".";  // Путь к текущей директории (по умолчанию — текущая директория)
    int stat_result;  // Переменная для хранения результата выполнения функции stat()
    
    // Массив для подсчета количества различных типов файлов:
    // индекс 0 - обычные файлы,
    // индекс 1 - директории,
    // индекс 2 - блочные устройства,
    // индекс 3 - символьные устройства,
    // индекс 4 - символические ссылки,
    // индекс 5 - каналы FIFO,
    // индекс 6 - сокеты.
    int file_types_counts[] = {0, -2, 0, 0, 0, 0, 0};  
    
    // Массив строк для вывода типов файлов
    char* file_types[] = {
        "regulars",      // Обычные файлы
        "directories",   // Директории
        "blocks",        // Блочные устройства
        "chars",         // Символьные устройства
        "links",         // Символические ссылки
        "fifos",         // Каналы FIFO
        "sockets"        // Сокеты
    };

    size_t file_types_seize = sizeof(file_types) / sizeof(file_types)[0];  // Определение размера массива типов файлов

    // Открытие текущей директории. Если не удается открыть, выводится сообщение об ошибке и программа завершает работу.
    if ((dir = opendir(current_dir_path)) == NULL) {
        perror("Error opening current directory");  // Вывод сообщения об ошибке
        exit(1);  // Завершение программы с кодом ошибки
    }

    // Чтение содержимого директории с помощью readdir(), пока не будут прочитаны все файлы и поддиректории.
    while ((dir_entry = readdir(dir)) != NULL) {
        // Получение информации о текущем файле/директории с использованием функции stat().
        if (stat(dir_entry->d_name, &dir_entry_stat) != 0 ) {
            printf("Error stat entry('%s')\n", dir_entry->d_name);  // Сообщение об ошибке при вызове stat()
            continue;  // Пропуск текущей итерации цикла в случае ошибки
        }

        // Проверка типа файла и увеличение соответствующего счетчика в массиве file_types_counts.
        if (S_ISREG(dir_entry_stat.st_mode)) {              // Если это обычный файл
            file_types_counts[0]++;
        } else if (S_ISDIR(dir_entry_stat.st_mode)) {       // Если это директория
            file_types_counts[1]++;
        } else if (S_ISBLK(dir_entry_stat.st_mode)) {       // Если это блочное устройство
            file_types_counts[2]++;
        } else if (S_ISCHR(dir_entry_stat.st_mode)) {       // Если это символьное устройство
            file_types_counts[3]++;
        } else if (S_ISLNK(dir_entry_stat.st_mode)) {       // Если это символическая ссылка
            file_types_counts[4]++;
        } else if (S_ISFIFO(dir_entry_stat.st_mode)) {      // Если это канал FIFO
            file_types_counts[5]++;
        } else if (S_ISSOCK(dir_entry_stat.st_mode)) {      // Если это сокет
            file_types_counts[6]++;
        }
    }

    // Вывод количества файлов каждого типа.
    for (size_t i = 0; i < file_types_seize; i++) {
        printf("%s files count - %d\n", file_types[i], file_types_counts[i]);
    }

    closedir(dir);  // Закрытие директории после завершения работы

    return 0;
}
