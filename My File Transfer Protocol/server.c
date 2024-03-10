#include <stdio.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <strings.h>
#include <stdlib.h>
#include <signal.h>
#include <wait.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <openssl/aes.h>

int clientNumber;
extern int errno;

void encrypt_passwd(char password[], int key) {

    for (int i = 0; i < strlen(password); i++) {

        password[i] = password[i] - key;
    }
}

void decrypt_passwd(char password[], int key) {

    for (int i = 0; i < strlen(password); i++) {

        password[i] = password[i] + key;
    }
}
void handle()
{
    
    while(waitpid(-1, NULL, WNOHANG) > 0);  //// astept ca fiecare proces copil sa fie terminat
}

int number_lines (char * name) {

    FILE *f;
    f = fopen (name, "r");

    if ( f == NULL) {

        printf("%s\n", "Eroare la deschiderea fisierului pentru numararea liniilor.");
        return -1;
    }

    int lines = 0;
    int i = 0;


    while ((i = fgetc(f)) != EOF) {

        if ( i == '\n')
            lines++;

    }
    return lines;
}


int confirm_login ( char username[], char password[]) {

    FILE *f;
    f = fopen("login.txt", "r");

    if ( f== NULL ) {

        printf ("%s\n", "Eroarea la deschiderea fisierului pentru verificarea utilizatorilor.");
        return 0;
    }

    int nr_lines = 0;
    nr_lines = number_lines("login.txt");

    char aux_username[100];
    char aux_password[100];

    for (int i = 0; i < nr_lines; i++) {

        memset(aux_username, 0 , strlen(aux_username));
        memset(aux_password, 0, strlen(aux_password));

        fscanf(f, "%s %s", aux_username, aux_password);

        if (strcmp(username, aux_username) == 0 && strcmp(password,aux_password) == 0) {

            fclose(f);
            return 1;
        }


    }

    fclose(f);
    return 0;



}

int is_blacklist(char username[])
{
    FILE *f = fopen("blacklist.txt", "r");
    char temp_username[100];
    int lines_number = number_lines("blacklist.txt");
    for(int i = 0; i < lines_number; i++)
        {
            memset(temp_username, 0, strlen(temp_username));
            fscanf(f, "%s", temp_username);
            if(strcmp(username, temp_username) == 0)
            {
                fclose(f);
                return 1;
            }
        }
    fclose(f);
    return 0;
}


int is__in_login(char username[])
{
    FILE *f = fopen("login.txt", "r");
    char temp_username[100];
    int lines_number = number_lines("login.txt");
    for(int i = 0; i < lines_number; i++)
        {
            memset(temp_username, 0, strlen(temp_username));
            fscanf(f, "%s", temp_username);
            if(strcmp(username, temp_username) == 0)
            {
                fclose(f);
                return 1;
            }
        }
    fclose(f);
    return 0;
}

int add_in_file (char username[], char password[]) {

    FILE *f;

    f = fopen ("login.txt", "a");

    int i;

    if (f == NULL) {

        printf ("%s\n", "Eroare la deschiderea fisierului pentru adaugarea utilizatorilor.");
        return 0;
    }

    else {

        char linie_fisier[100];
        strcpy (linie_fisier, username);
        strcat (linie_fisier, " ");
        strcat (linie_fisier, password);

       while ((i = fgetc(f)) != EOF) {
        ;
      }
      fputs (linie_fisier,f);
      putc ('\n', f);

    }
    fclose(f);
    return 1;
}

int check_if_is_safe (char username[], char password[]) {

    FILE *f;
    f = fopen("login.txt", "r");

    if ( f== NULL ) {

        printf ("%s\n", "Eroarea la deschiderea fisierului pentru verificarea utilizatorilor.");
        return 0;
    }

    int nr_lines = 0;
    nr_lines = number_lines("login.txt");

    char aux_username[100];
    char aux_password[100];

    for (int i = 0; i < nr_lines; i++) {

        memset(aux_username, 0 , strlen(aux_username));
        memset(aux_password, 0, strlen(aux_password));

        fscanf(f, "%s %s", aux_username, aux_password);

        if (strcmp(password,aux_password) == 0) {

            fclose(f);
            return 0;
        }

    }
    return 1;
}

int exists_dir (char path[], char name[]) {

    char local_path[1000];

    strcpy (local_path, path);
    strcat (local_path, "/");
    strcat (local_path, name);

    DIR * dir = opendir(local_path);

    if(dir==NULL){
    return 0;
    }
    return 1;
}

int check_file (char path[], char name[]) {

    char local_path[1000];

    strcpy (local_path, path);
    strcat (local_path, "/");
    strcat (local_path, name);

    FILE *fp = fopen(local_path, "r");

    if (fp != NULL)
    {
        return 1;
        fclose(fp); 
    }
    fclose (fp);
    return 0;
}

int file_exists(char path[], char message[])
{
    DIR *d;
    d = opendir(path);
    struct dirent *dir;
    if(d != NULL)
    {
        while(dir = readdir(d))
        {
            if(strcmp(dir->d_name, message) == 0)
            {
                return 0;
            }
        }
        closedir(d);
    }
    return 1;
}


int create_dir(char path[], char name[])
{
    char local_path[1000];
    strcpy(local_path, path);
    strcat(local_path, name);

    if(mkdir(local_path, 0777) == -1)
    {
        printf("%s\n", "Eroare la crearea directorului!");
        return 0;
    }
    return 1;
}

char* change_dir(char mesaj[], char path[], char username[])
{           
    char user_path[1000] = {0};
    strcat(user_path, "Home/");
    strcat(user_path, username);
    strcat(user_path, "/");
    char new_path[1000];
    char local_path[1000];
    strcpy(local_path, path);
    char *ret;
    char *ret_value = local_path;
    int number = 0;
    int i;
    int local_number = 0;
    struct stat filestat; 

    if(strcmp(mesaj, ".") == 0)
    {
        strcpy(ret_value, "NuExista");
        return ret_value;
    }

    if(strcmp(mesaj, "..") == 0)
    {
        memset(new_path, 0, strlen(new_path));
        if(strcmp(local_path, user_path) == 0)
        {
            strcpy(ret_value, "MaxHeight");
            return ret_value;
        }

        
        for(i=0; i < strlen(local_path); i++)
        {
            if(local_path[i] == '/')
                number++;
        }

        for(i=0; i < strlen(local_path); i++)
        {
            if(local_path[i] == '/')
                local_number++;
            if(local_number == number-1)
                break;
            new_path[i] = local_path[i];
        }
        new_path[i] = '/';
        new_path[i+1] = '\0';
        strcpy(ret_value, new_path);
        return ret_value;
    }

    strcat(local_path, mesaj);     //tratez cazurile cand numele dat nu este de la un director    
    stat(local_path, &filestat);     //si cand directorul nu exista; 

    DIR *d;                    //incerc sa deschid directorul si daca nu se poate inseamna ca nu exista
    d = opendir(local_path);
    if(d == NULL)
    {
        strcpy(ret_value, "NuExista");
        return ret_value;
    }

    if(S_ISREG(filestat.st_mode) != 0)
    {
        strcpy(ret_value, "NuExista");
        return ret_value;
    }


    strcat(local_path, "/");
    return ret_value;
}


int create_file (char path[], char mesaj[]) {

    char local_path[1000];
    strcpy (local_path, path);
    //strcat (local_path, "/");
    strcat (local_path, mesaj);

    if (creat (local_path, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) == -1) {
        printf ("%s\n", "Eroarea la crearea fisierului.");
        return 0;
    } 
    return 1;


}


int delete_file(char path[], char mesaj[])        
{
    char local_path[1000] = {0};
    strcpy(local_path, path);
    strcat(local_path, mesaj);
    if(unlink(local_path) != 0)
    {
        return -1;
    }
    return 0;
}

int delete_subdir(char path[])
{
    DIR *d;
    struct dirent *dir;
    d = opendir(path);
    if(d != NULL)
    {
        while(dir = readdir(d))
        {
            if(strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
                continue;
            struct stat fstat;
            char newpath[100];
            strcpy(newpath, path);
            strcat(newpath, "/");
            strcat(newpath, dir->d_name);
            stat(newpath, &fstat);

            if(S_ISREG(fstat.st_mode) != 0)
            {
                printf("%s\n", dir->d_name);
                remove(newpath);    //call unlink pentru fisier
            }
            
            if(S_ISDIR(fstat.st_mode) != 0)
            {
                printf("%s\n", dir->d_name);
                delete_subdir(newpath);
            }
        }
        remove(path);  //call rmdir pentru directoare
    }

}

int check_if_directory(char path[], char mesaj[])
{
    DIR *dd;
    dd = opendir(path);
    struct dirent *dir;
    struct stat fstat;
    char local_path[1000] = "\0";
    if(dd == NULL)
    {
        return -1;
    }
    if( dd!= NULL)
    {
        while(dir = readdir(dd))
        {
            if(strcmp(dir->d_name, mesaj) == 0)
            {
                strcat(local_path, path);
                strcat(local_path, "/");
                strcat(local_path, mesaj);
                stat(local_path, &fstat);
                if(S_ISDIR(fstat.st_mode) != 0)
                {
                    closedir(dd);
                    return 0;
                }
                if(S_ISREG(fstat.st_mode) != 0)
                {
                    closedir(dd);
                    return 1;
                }   
            }
        }
    }
    closedir(dd);
    return 1;
}
                                                            
int delete_dir(char mesaj[], char path[])           
{
    DIR *d;

    if(check_if_directory(path, mesaj) == 1)
    {
        return -1;
    }

    char lmesaj[1000];
    char local_path[1000];
    strcpy(local_path, path);
    strcpy(lmesaj, mesaj);  
    strcat(local_path, lmesaj);

    d = opendir(local_path);
    struct dirent *dir;
    
    if(d != NULL)
    {
        while(dir = readdir(d))
        {
            if(strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
                continue;
            struct stat fstat;
            char newpath[100]= {0};
            strcat(newpath, local_path);
            strcat(newpath, "/");
            strcat(newpath, dir->d_name);
            stat(newpath, &fstat);
                
            if(S_ISREG(fstat.st_mode) != 0)        
            {
                remove(newpath);
            }
            
            if(S_ISDIR(fstat.st_mode) != 0)
            {
                delete_subdir(newpath);
            }
        }

        if(remove(local_path) == 0)
        {
            closedir(d);
            return 0;                      
        }
        else
            closedir(d);
            return -1;
    }

    return -1;
}

int renamefile(char mesaj[], char mesaj_2[], char path[])
{
    char local_path[1000];
    char newpath[100];
    struct stat fstat;
    strcpy(newpath, path);
    strcpy(local_path, path);
    strcat(local_path, mesaj);
    strcat(newpath, mesaj_2);
    stat(local_path, &fstat);

    if(S_ISDIR(fstat.st_mode) != 0)
    {
        return -1;
    }

    if(rename(local_path, newpath) < 0 )
    {
        return 0;
    }

    return 1;
}

int move_file (char path[], char name[], char  destinatie[]) {

    char ch;
    FILE *f1;
    FILE* f2;

    char local_path1[1000];
    char local_path2[1000];

    strcpy (local_path1, path);

    strcat (local_path1, "/");
    strcat (local_path1, name);

    strcpy (local_path2, path);
    strcat (local_path2,"/");
    strcat (local_path2, destinatie);
    strcat (local_path2, "/");
    strcat (local_path2, name);

    f1 = fopen (local_path1, "r");
    f2 = fopen (local_path2, "w");


    fclose (f1);
    fclose (f2);

   return  unlink(local_path1);


}

void add_in_blacklist(char username[])
{
    FILE *f;
    f = fopen("blacklist.txt", "a");
    if(f == NULL)
    {
        printf("%s\n", "Nu s-a putut adauga clientul in blacklist");
        return;
    }
    char umessage[100] = {0};
    strcat(umessage, username);
    fprintf(f, "\n%s", umessage);
    fclose(f);
}

void add_in_whitelist(char username[])
{
    FILE *f;
    f = fopen("blacklist.txt", "a");
    if(f == NULL)
    {
        printf("%s\n", "Nu s-a putut adauga clientul in blacklist");
        return;
    }
    char umessage[100] = {0};
    strcat(umessage, username);
    fprintf(f, "\n%s", umessage);
    fclose(f);
}

#define port 2090
#define key 290103

int main()
{
    int serverSocket;   
    struct sockaddr_in serverAddr;
    struct sockaddr_in from;
    
    //creare socket
    if ((serverSocket = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("[server]Eroare la socket().\n");
      return 0;
    }

    //umplere structura socket
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    //reutilizam adresa
    int enable=1 ;
    if(setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int) )< 0 )
    {
        perror ("[server]Eroare la reuse().\n");
        return 0;
    }

    //atasam socketul
    if (bind (serverSocket, (struct sockaddr *) &serverAddr, sizeof (struct sockaddr)) == -1)
    {
      perror ("[server]Eroare la bind().\n");
      return 0;
    }

    //punem serverul sa asculte
    if (listen (serverSocket, 5) == -1)
    {
      perror ("[server]Eroare la listen().\n");
      return 0;
    }

    printf ("[server]Asteptam la portul %d...\n", port);
    fflush (stdout);

    //se accepta clientii
    while(1)
    {
        int clientSocket;
        int length = sizeof(from);

        clientSocket = accept(serverSocket, (struct sockaddr *) &from, &length); //structura from contine informatii de la client
                                                                                 //facem fork dupa accept, ca serverul sa fie concurent
        if(clientSocket < 0)
        {
             perror ("[server]Eroare la accept().\n");
             continue;
        }


        int pid;
        pid = fork();

        if(pid == -1)
        {
            printf("%s", "Eroare la fork");
            return 0;
        }

        signal(SIGCHLD, handle);        
                                       
        clientNumber ++;
        if(pid > 0)
            {  ; }//parinte 

        else
        {
            //copil
            printf("%s", "A venit clientul numarul : ");
            printf("%d\n", clientNumber);

            char mesaj[1000]; 
            char mesaj_2[1000];
            int size;
            int status;
            char raspuns[1000];
            char username[100];
            char password[100];
            int logat = 0;
            int vreau_dir;
            int flag = 0;
            int blacklist = 0;
            char comenzi_black[] = {"exit logout help ccrdir chelp cchdir cdelfile crenamefile\0"};
            char comenzi_valide[] = {"exit login crcont logout help crdir chdir mvfile crfile deldir delfile renamefile location get\0"};
            char path[10000];
            memset(path, 0, strlen(path));
            strcpy(path, "Home\0");
            char oldpath[10000];
            char local_path[10000]; 

        while(1)         
        {
            memset(mesaj, 0, strlen(mesaj));
            memset(raspuns, 0, strlen(raspuns));

            if (read(clientSocket, &size, sizeof(int)) == -1)
            {
                printf("%s\n", "Eroare le citirea dimensiunii comenzii.");
                continue;
            }

            if (read(clientSocket, mesaj, size) == -1)
            {
                printf("%s\n","Eroare la citirea comenzii.");       //daca clientul se deconecteaza fara exit, va da eroare aici
                close(clientSocket);
                printf("%s", "A iesit clientul numarul : ");
                printf("%d\n", clientNumber);
                exit(1);
            }

            if(strcmp(mesaj, "help") == 0) {

                memset(raspuns, 0, strlen(raspuns));
                strcpy(raspuns, "help");
                size = strlen(raspuns);

                write(clientSocket, &size, sizeof(int));
                write(clientSocket, raspuns, size);

                if(logat == 0) {
                    memset(raspuns, 0, strlen(raspuns));
                    strcpy(raspuns, "help login exit");
                    size = strlen(raspuns); 
                    write(clientSocket, &size, sizeof(int));
                    write(clientSocket, raspuns, size);
                    continue;
                }

                if (logat == 1 && blacklist == 1) {


                    memset(raspuns, 0, strlen(raspuns));
                    strcpy(raspuns, comenzi_black);
                    size = strlen(raspuns);

                    write(clientSocket, &size, sizeof(int));
                    write(clientSocket, raspuns, size);
                    continue;

                }

                if (logat == 1 && blacklist == 1) {


                    memset(raspuns, 0, strlen(raspuns));
                    strcpy(raspuns, comenzi_valide);
                    size = strlen(raspuns);

                    write(clientSocket, &size, sizeof(int));
                    write(clientSocket, raspuns, size);
                    continue;

                }
            } 

             if(strcmp(mesaj, "exit") == 0)
            {
                write(clientSocket, &size, sizeof(int));
                write(clientSocket, mesaj, size);
                close(clientSocket);
                printf("%s", "A iesit clientul numarul : ");
                printf("%d\n", clientNumber);
                exit(1);
            }

            if(strcmp(mesaj, "logout") == 0 && logat == 1)
            {
                memset(raspuns, 0, strlen(raspuns));
                strcpy(raspuns, "logout");
                size = strlen(raspuns);

                write(clientSocket, &size, sizeof(int));
                write(clientSocket, raspuns, size);

                size = 0;
                logat = 0;
                blacklist = 0;

                memset(path, 0, strlen(path));
                memset(oldpath, 0, strlen(oldpath));
                memset(local_path, 0, strlen(local_path));
                memset(username, 0, strlen(username));
                memset(password, 0, strlen(password));
                memset(comenzi_valide, 0, strlen(comenzi_valide));

                strcpy(path, "Home\0");
                size = 1;
                write(clientSocket, &size, sizeof(int));
                continue;
            }


            if (strcmp(mesaj,"login") ==0 && logat == 0) {

                    memset(raspuns, 0, strlen(raspuns));
                    memset(username, 0, strlen(username));
                    memset(password, 0, strlen(password));

                    strcpy(raspuns,"login");

                    size = strlen(raspuns);

                    write(clientSocket, &size, sizeof(int));
                    write(clientSocket, raspuns, size);

                    read(clientSocket, &size, sizeof(int));
                    read(clientSocket, username, size);

                    read(clientSocket, &size, sizeof(int));
                    read(clientSocket, password, size);

                    // decriptez parola

                  //  decrypt_passwd (password, key);

                    memset(raspuns, 0, strlen(raspuns));


                    if (confirm_login(username, password) == 1) {

                        strcpy (raspuns,"succes");

                        size = strlen(raspuns);

                        write(clientSocket, &size, sizeof(int));
                        write(clientSocket, raspuns, size);

                        logat = 1;

                        strcat (path, "/");
                        
                        if (exists_dir(path, username) == 1) {

                             strcat(path, username);
                             strcat (path, "/");
                        }

                        if (is_blacklist(username) == 1) {

                            printf("%s\n", "Clientul se afla in blacklist!");

                            blacklist = 1;

                          //  size = strlen(comenzi_valide);

                          //  write(clientSocket, &size, sizeof(int));
                          //  write(clientSocket, comenzi_valide, size);

                            strcpy (raspuns,"isblack");

                            size = strlen(raspuns);

                            write(clientSocket, &size, sizeof(int));
                            write(clientSocket, raspuns, size);

                        }

                        else {

                            printf("%s\n", "Clientul se afla in whitelist!");

                            // size = strlen(comenzi_valide);

                            // write(clientSocket, &size, sizeof(int));
                            // write(clientSocket, comenzi_valide, size);

                            // strcpy (raspuns,"iswhite");

                            // size = strlen(raspuns);

                            // write(clientSocket, &size, sizeof(int));
                            // write(clientSocket, raspuns, size);
                        }

                    }

                    else {

                        strcpy (raspuns, "fail");

                        size = strlen (raspuns);

                        write (clientSocket, &size, sizeof(int));
                        write (clientSocket, raspuns, size);

                        memset (mesaj, 0, strlen(mesaj));
                    }
                    continue;
                }

            if (strcmp(mesaj, "crcont") == 0 && logat == 0) {

                memset(raspuns, 0, strlen(raspuns));
                strcpy(raspuns, "crcont");
                size = strlen(raspuns);
                write(clientSocket, &size, sizeof(int));
                write(clientSocket, raspuns, size);

                read(clientSocket, &size, sizeof(int));
                read(clientSocket, username, size);

                read(clientSocket, &size, sizeof(int));
                read(clientSocket, password, size);

                if (is__in_login(username) == 1) {

                    memset(raspuns, 0, strlen(raspuns));
                    strcpy(raspuns, "exista_deja");
                    size = strlen(raspuns);

                    write(clientSocket, &size, sizeof(int));
                    write(clientSocket, raspuns, size);

                    continue;
                }

                if (check_if_is_safe(username, password) == 1) {

                    add_in_whitelist(username);

                     if (add_in_file(username, password) == 1) {

                        logat = 1;

                        strcat (path, "/");

                        if (create_dir(path, username) == 1) {
                            strcat (path, username);
                            strcat (path, "/");
                        }

                        strcpy (raspuns, "add_succ");

                        size = strlen (raspuns);

                        write(clientSocket, &size, sizeof(int));
                        write(clientSocket, raspuns, size);
                        continue;
                    } 

                    else {
                    
                        strcpy (raspuns, "add_fail");

                        size = strlen (raspuns);

                        write(clientSocket, &size, sizeof(int));
                        write(clientSocket, raspuns, size);
                    }
                }
                else {

                    add_in_blacklist (username);

                    strcpy (raspuns, "add_blacklist");

                    size = strlen (raspuns);

                    write(clientSocket, &size, sizeof(int));
                    write(clientSocket, raspuns, size);
                    continue;
                }



            }


            if(strcmp(mesaj, "location") == 0 && logat == 1)
            {
                memset(raspuns, 0, strlen(raspuns));
                strcpy(raspuns, "location");
                size = strlen(raspuns) +1;
                write(clientSocket, &size, sizeof(int));
                write(clientSocket, raspuns, size);
                size = strlen(path) +1;
                write(clientSocket, &size, sizeof(int));
                write(clientSocket, path, size);
                continue;
            }

            if(strcmp(mesaj, "crdir") == 0 && logat == 1) {

                memset(raspuns, 0, strlen(raspuns));
                memset(mesaj, 0, strlen(mesaj));

                strcpy(raspuns, "crdir");   
                size = strlen(raspuns);

                write(clientSocket, &size, sizeof(int));
                write(clientSocket, raspuns, size);

                read(clientSocket, &size, sizeof(int));
                read(clientSocket, mesaj, size);
              
                status = create_dir(path, mesaj);  
                
                write(clientSocket, &status, sizeof(int));
                continue;
            }

            if(strcmp(mesaj, "chdir") == 0 && logat == 1) 
            {
                memset(raspuns, 0, strlen(raspuns));
                memset(mesaj, 0, strlen(mesaj));
                strcpy(raspuns, "chdir");
                size = strlen(raspuns);
                write(clientSocket, &size, sizeof(int));
                write(clientSocket, raspuns, size);
                read(clientSocket, &size, sizeof(int));
                read(clientSocket, mesaj, size);
                memset(raspuns, 0, strlen(raspuns));
                memset(oldpath, 0, strlen(oldpath));
                memset(local_path, 0, strlen(local_path));
                strcpy(oldpath, path);
                strcpy(local_path, path);
                strcpy(raspuns, change_dir(mesaj, local_path, username));
                if(strcmp(raspuns, "MaxHeight") == 0)
                {
                    size = strlen(raspuns) +1;
                    write(clientSocket, &size, sizeof(int));
                    write(clientSocket, raspuns, size);
                    continue;
                }
                if(strcmp(raspuns, "NuExista") == 0)
                {
                    size = strlen(raspuns) +1;
                    write(clientSocket, &size, sizeof(int));
                    write(clientSocket, raspuns, size);
                    continue;
                }
                memset(local_path, 0,strlen(local_path));
                memset(path, 0, strlen(path));
                strcpy(path, raspuns);
                size = strlen(raspuns) +1;
                write(clientSocket, &size, sizeof(int));
                write(clientSocket, raspuns, size);
                continue;
            }

            if(strcmp(mesaj, "mvfile") == 0)
            {
                memset(raspuns, 0, strlen(raspuns));
                memset(mesaj, 0, strlen(mesaj));
                memset(mesaj_2, 0, strlen(mesaj_2));

                strcpy(raspuns, "mvfile");
                size = strlen(raspuns);

                write(clientSocket, &size, sizeof(int));
                write(clientSocket, raspuns, size);

                read(clientSocket, &size, sizeof(int));
                read(clientSocket, mesaj, size);
                
                read(clientSocket, &size, sizeof(int));
                read(clientSocket, mesaj_2, size);

                // 1 daca s-a reusit, -2 fisierul nu exista, -3 destinatie gresita 
                status = move_file(path,mesaj, mesaj_2);
                write(clientSocket, &status, sizeof(int));
                continue;
            }

            if (strcmp(mesaj, "deldir") == 0)
            {
                memset(raspuns, 0, strlen(raspuns));
                memset(mesaj, 0, strlen(mesaj));

                strcpy(raspuns, "deldir");
                size = strlen(raspuns);

                write(clientSocket, &size, sizeof(int));
                write(clientSocket, raspuns, size);

                read(clientSocket, &size, sizeof(int));
                read(clientSocket, mesaj, size);

                printf("%s\n", mesaj);
                status = delete_dir(mesaj, path);
                write(clientSocket, &status, sizeof(int));

                continue;
            }

            if (strcmp(mesaj, "crfile") == 0 && logat == 1) {

                memset(raspuns, 0, strlen(raspuns));
                memset(mesaj, 0, strlen(mesaj));

                strcpy(raspuns, "crfile");
                size = strlen(raspuns);

                write(clientSocket, &size, sizeof(int));
                write(clientSocket, raspuns, size);

                read(clientSocket, &size, sizeof(int));
                read(clientSocket, mesaj, size);

                status = create_file(path, mesaj);

                write(clientSocket, &status, sizeof(int));
                continue;


            }

            if (strcmp(mesaj, "renamefile") == 0)
            {
                memset(raspuns, 0, strlen(raspuns));
                memset(mesaj, 0, strlen(mesaj));

                memset(mesaj_2, 0, strlen(mesaj_2));
                strcpy(raspuns, "renamefile");
                
                size = strlen(raspuns);
                write(clientSocket, &size, sizeof(int));
                write(clientSocket, raspuns, size);

                read(clientSocket, &size, sizeof(int));
                read(clientSocket, mesaj, size);

                read(clientSocket, &size, sizeof(int));
                read(clientSocket, mesaj_2, size);

                status = renamefile(mesaj, mesaj_2, path);
                write(clientSocket, &status, sizeof(int));

                continue;
            }

            if(strcmp(mesaj, "delfile") == 0)
            {
                memset(raspuns, 0, strlen(raspuns));
                memset(mesaj, 0, strlen(mesaj));
                strcpy(raspuns, "delfile");
                size = strlen(raspuns) +1;
                write(clientSocket, &size, sizeof(int));
                write(clientSocket, raspuns, size);
                read(clientSocket, &size, sizeof(int));
                read(clientSocket, mesaj, size);
                status = delete_file(path, mesaj);
                write(clientSocket, &status, sizeof(int));
                continue;
            }

            if(strcmp(mesaj, "put") == 0)
            {
                memset(mesaj, 0, strlen(mesaj));
                memset(raspuns, 0, strlen(raspuns));
                strcpy(raspuns, "put");
                size = strlen(raspuns);
                write(clientSocket, &size, sizeof(int));
                write(clientSocket, raspuns, size);

              
                read(clientSocket, &status, sizeof(int));
                if(status == 0)
                    continue;

                read(clientSocket, &size, sizeof(int));
                read(clientSocket, mesaj, size);
                status = file_exists(path, mesaj);
                write(clientSocket, &status, sizeof(int));
                if(status == 0)
                    continue;
               
                //incepe transferul
                memset(local_path, 0, strlen(local_path));
                strcpy(local_path, path);
                strcat(local_path, mesaj);
                FILE *f;
                f = fopen(local_path, "w");
                if(f ==  NULL)
                {
                    //printf("%s\n", "Eroare la crearea fisierului pe server");
                    continue;
                }
            
                int leng;
                while(1)                         
                {
                    unsigned char data[1] = {0};
                    leng = 0;                             
                    read(clientSocket, &leng,  sizeof(int));
                    if(leng == -1)
                        break;
                    read(clientSocket, data, leng);
                    fwrite(data, 1, leng, f);
                }
                fclose(f);
                continue;
            }

            if(strcmp(mesaj, "get") == 0)     
            {
                memset(mesaj, 0, strlen(mesaj));
                memset(raspuns, 0, strlen(raspuns));
                strcpy(raspuns, "get");
                size = strlen(raspuns);
                write(clientSocket, &size, sizeof(int));
                write(clientSocket, raspuns, size);
                read(clientSocket, &size, sizeof(int));
                read(clientSocket, mesaj, size);

                read(clientSocket, &status, sizeof(int));
                if(status == 0)
                    continue;
                
                status = check_file(path, mesaj);

                write(clientSocket, &status, sizeof(int));
                if(status == 0)
                    continue;
                // transferul

                memset(local_path, 0, strlen(local_path));
                strcpy(local_path, path);
                strcat(local_path, mesaj);
                FILE *f;
                f = fopen(local_path, "rb");
                struct stat fstat;
                stat(local_path, &fstat);
                size = fstat.st_size;

                read(clientSocket, &status, sizeof(int));
                if(status == 0)
                    continue;

                while(1)
                {
                    unsigned char data[1] = {0};
                    int leng = 0;
                    leng = fread(data, 1, 1, f);
                    if(leng > 0)
                    {
                        write(clientSocket, &leng, sizeof(int));
                        write(clientSocket, data, leng);
                        continue;
                    }

                    if(leng == 0)
                    {
                        if(feof(f))
                            leng = -1;
                        if(ferror(f) != 0 )
                        {                        
                            leng = -2;
                        }
                        leng = -1;
                        write(clientSocket, &leng, sizeof(int));
                        break;
                    }
                }
                fclose(f);
                continue;
            }

            memset(raspuns, 0, sizeof(raspuns));
            strcpy(raspuns, "necunoscut");
            size = strlen(raspuns) + 1;
            write(clientSocket, &size, sizeof(int));
            write(clientSocket, raspuns, size);
        }
        }
    }
    return 0;
}