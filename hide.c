#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

// Определение константы для имени директории
#define DARK_DIR_PATH   "darkroom/"

int main(int argc, char* argv[])
{
    // Проверяем, передан ли путь к файлу в аргументах командной строки
    if (argc < 2) {
        puts("Please specify filepath");  // Выводим сообщение, если аргумент не передан
        return 1;  // Завершаем программу с кодом ошибки
    }

    // Инициализируем массивы для хранения путей
    char cur_dir_path[PATH_MAX];   // Текущий путь к директории
    char file_path[PATH_MAX];      // Полный путь к файлу
    char new_file_path[PATH_MAX];  // Новый путь к файлу (внутри darkroom)
    char dark_dir_path[PATH_MAX];  // Путь к директории darkroom

    getcwd(cur_dir_path, PATH_MAX);  // Получаем текущую рабочую директорию
    strcat(cur_dir_path, "/");       // Добавляем разделитель "/" к пути

    // Проверяем, является ли переданный путь абсолютным (начинается с '/')
    if (argv[1][0] != '/') {
        strcpy(file_path, cur_dir_path);  // Если путь относительный, добавляем текущую директорию к пути файла
    }
    strcat(file_path, argv[1]);  // Добавляем имя файла к пути

    strcpy(dark_dir_path, cur_dir_path);  // Копируем текущую директорию в переменную для darkroom
    strcat(dark_dir_path, DARK_DIR_PATH);  // Добавляем название папки darkroom к пути

    strcpy(new_file_path, dark_dir_path);  // Копируем путь к darkroom в переменную нового пути файла
    strcat(new_file_path, argv[1]);        // Добавляем имя файла к новому пути

    struct stat sb;

    // Проверяем, существует ли директория darkroom и является ли она директорией
    if (stat(dark_dir_path, &sb) != 0 || !S_ISDIR(sb.st_mode)) {
        // Если директория не существует, создаем её с правами доступа 0311 (x--x--x--)
        if (mkdir(dark_dir_path, 0311) != 0) {
            printf("Error creating directory %s\n", dark_dir_path);  // Сообщаем об ошибке создания директории
            return 1;  // Завершаем программу с кодом ошибки
        }
    } else if (access(dark_dir_path, 0311) != 0) {
        // Если права доступа недостаточны, пытаемся изменить владельца и права доступа на x--x--x--
        if (chown(dark_dir_path, getuid(), getgid()) != 0 || chmod(dark_dir_path, 0311) != 0) {
            printf("Failed to change owner or access mode for %s\n", dark_dir_path);  // Сообщаем об ошибке изменения прав доступа или владельца
            return 1;  // Завершаем программу с кодом ошибки
        }
    }

    // Перемещаем файл из исходного местоположения в директорию darkroom
    if (rename(file_path, new_file_path) != 0) {
        printf("Error moving file %s to %s\n", file_path, new_file_path);  // Сообщаем об ошибке перемещения файла
        return 1;  // Завершаем программу с кодом ошибки
    }

    return 0;
}
