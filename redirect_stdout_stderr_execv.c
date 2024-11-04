#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include <fcntl.h>

#define STDOUT_FILE "out.txt"
#define STDERR_FILE "err.txt"

// Функция для получения полного пути к исполняемому файлу
void get_executable_full_path(char* full_path, const char* file_name) {
    if (file_name[0] != '/') {
        getcwd(full_path, PATH_MAX);  // Получаем текущую директорию
        strcat(full_path, "/");       // Добавляем разделитель "/"
    }
    strcat(full_path, file_name);     // Добавляем имя файла
}

// Функция для обработки ошибок при создании процесса
void handle_fork_error() {
    printf("Error creating child process\n");
}

// Функция для обработки ошибок при выполнении программы
void handle_exec_error(const char* full_path) {
    printf("Error executing %s\n", full_path);
}

int main(int argc, char* argv[])
{
    // Проверяем наличие аргумента с путем к исполняемому файлу
    if (argc < 2) {
        printf("Please specify path to executable file\n");
        return 1;
    }

    // Буфер для полного пути к исполняемому файлу
    char executable_full_path[PATH_MAX];
    get_executable_full_path(executable_full_path, argv[1]);

    // Создаем дочерний процесс
    int process_id = fork();
    
    if (process_id == -1) {
        handle_fork_error();
        return 1;
    } 
    else if (process_id == 0) {  // Код выполняется в дочернем процессе
        // Создаем новую сессию для дочернего процесса
        if (setsid() < 0) {
            printf("Failed creating new session for child process\n");
            return 1;
        }

        // Открываем файлы для перенаправления вывода и ошибок
        FILE *stdout_file = fopen(STDOUT_FILE, "a");
        FILE *stderr_file = fopen(STDERR_FILE, "a");

        if (!stdout_file || !stderr_file) {
            printf("Error opening output or error files\n");
            return 1;
        }

        // Перенаправляем стандартный вывод и ошибки в файлы
        dup2(fileno(stdout_file), STDOUT_FILENO);
        dup2(fileno(stderr_file), STDERR_FILENO);

        // Закрываем файловые дескрипторы после перенаправления
        fclose(stdout_file);
        fclose(stderr_file);

        // Выполняем указанный исполняемый файл с переданными аргументами
        execv(executable_full_path, &argv[1]);

        // Если execv вернул управление, значит произошла ошибка выполнения программы
        handle_exec_error(executable_full_path);
        return 1;
    }

    return 0;
}
