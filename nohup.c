#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include <fcntl.h>

int main(int argc, char* argv[])
{
    // Проверяем, передан ли путь к исполняемому файлу
    if (argc < 2) {
        printf("Please specify path to executable file\n");
        return 1;
    }

    // Массив для хранения полного пути к исполняемому файлу
    char full_executable_path[PATH_MAX];

    // Если путь не является абсолютным, добавляем текущую директорию
    if (argv[1][0] != '/') {
        getcwd(full_executable_path, PATH_MAX);  // Получаем текущую директорию
        strcat(full_executable_path, "/");       // Добавляем разделитель "/"
    }
    
    // Добавляем к пути имя исполняемого файла
    strcat(full_executable_path, argv[1]);

    // Создаем дочерний процесс
    int child_pid = fork();
    
    // Проверяем, успешно ли создан процесс
    if (child_pid == -1) {
        printf("Error creating child process\n");
        return 1;
    } else if (child_pid == 0) {
        // В дочернем процессе создаем новую сессию
        if (setsid() < 0) {
            printf("Failed creating new session for child process\n");
            return 1;
        }

        // Открываем /dev/null для перенаправления ввода/вывода
        int dev_null_fd = open("/dev/null", O_RDWR);

        // Проверяем успешность открытия /dev/null
        if (dev_null_fd == -1) {
            printf("Failed to open /dev/null in read write mode\n");
            return 1;
        }

        // Перенаправляем стандартные потоки ввода/вывода в /dev/null
        dup2(dev_null_fd, 0);  // Перенаправляем stdin
        dup2(dev_null_fd, 1);  // Перенаправляем stdout
        dup2(dev_null_fd, 2);  // Перенаправляем stderr

        // Закрываем файловый дескриптор /dev/null, так как он больше не нужен
        close(dev_null_fd);

        // Выполняем исполняемый файл по полному пути
        execv(full_executable_path, &argv[1]);

        // Если execv вернулся, значит произошла ошибка выполнения файла
        printf("Error executing %s\n", full_executable_path);
        return 1;
    }
    
    return 0;
}
