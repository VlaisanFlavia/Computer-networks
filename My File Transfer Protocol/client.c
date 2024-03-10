#include <stdio.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <stdlib.h>
#include <wait.h>
#include <signal.h>
#include <openssl/aes.h>

#define green "\x1b[32m"
#define yellow "\x1b[33m"
#define blue  "\x1b[34m"
#define reset "\x1b[0m"


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
    while(waitpid(-1, NULL, WNOHANG) > 0);
}


 void change_dir(char path[], char msg[])
{
    int i;
    int number;
    int local_number;
    char new_path[1000];
    struct stat dirstat;
    memset(new_path, 0, strlen(new_path)); 

    if(strcmp(msg, ".") == 0)
    {
        printf("Directorul selectat nu exista.\n");
        return;
    }

    if(strcmp(msg, "..") == 0)
    {
        if(strcmp(path, "/") == 0)
        {
            printf("%s\n", "Nu se poate face schimbarea la un nivel superior, maximul a fost atins");
            return;
        }
        
        number = 0;
        local_number = 0;
        for(i=0; i < strlen(path); i++)
        {
            if(path[i] == '/')
                number++;
        }
        for(i=0; i< strlen(path); i++)
        {
            if(path[i] == '/')
                local_number++;
            if(local_number == number)
                break;
            new_path[i] = path[i];
        }
        new_path[i] = '\0'; 
        memset(path, 0, strlen(path));
        strcpy(path, new_path);

        if(strcmp(path, "") == 0)
            strcpy(path, "/");
        printf("%s%s\n", "Noua locatie este : ", path);
        return;
    }

    if(strcmp(path, "/") == 0)
        memset(path, 0, strlen(path));

    DIR *d;
    strcpy(new_path, path);
    strcat(new_path, "/");
    strcat(new_path, msg);
    d = opendir(new_path);

    if(d == NULL)
    {
        printf("%s\n", "Directorul selectat nu se poate dechide sau nu exista");
        return;
    }

    stat(new_path, &dirstat);
    if(S_ISREG(dirstat.st_mode) != 0)
    {
        printf("%s\n", "Numele introduc apartine unui fisier");
        return;
    }


    if(S_ISDIR(dirstat.st_mode) != 0)
    {
        memset(path, 0, strlen(path));
        strcpy(path, new_path);
        printf("%s%s\n", "Noua locatie este : ", path);
        return;
    }

}

int exists(char path[], char msg[])
{
    DIR *d;
    struct dirent *dir;
    d = opendir(path);
    if(d != NULL)
    {
        while(dir = readdir(d))
        {
            if(strcmp(dir->d_name, msg) == 0)
            {
                return 0;
            }
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

void create_dir(char path[], char mesaj[])
{
    char local_path[1000];

    strcpy(local_path, path);
    strcat(local_path, "/");
    strcat(local_path, mesaj);

    if(mkdir(local_path, 0777) == -1)
    {
        printf("%s\n", "Eroare la crearea directorului.");
        return;
    }

    printf("%s\n", "Directorul a fost creat cu succes.");
}

void delete_file(char path[], char mesaj[]) 
{
    char local_path[1000];
    strcpy(local_path, path);
    strcat(local_path, "/");
    strcat(local_path, mesaj);
    struct stat filestat;

    stat(path, &filestat);
    if(S_ISREG(filestat.st_mode) != 0)
    {
        printf("%s\n", "Fisierul nu a putut fi sters, introduceti un nume de fisier");
        return;
    }

    if(remove(local_path) != 0)
    {
        printf("%s\n", "Fisierul nu a putut fi sters");
        return;
    }

    printf("%s\n", "Fisierul a fost sters cu succes");
}

void delete_subdir(char ppath[])
{
    DIR *d;
    struct dirent *dir;
    char oldpath[1000];
    d = opendir(ppath);

    if(d != NULL)
    {
        while(dir = readdir(d))
        {
            if(strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
                continue;
            struct stat fstat;
            strcpy(oldpath, ppath);
            strcat(ppath, "/");
            strcat(ppath, dir->d_name);
            stat(ppath, &fstat);

            if(S_ISREG(fstat.st_mode) != 0)
            {
                remove(ppath);
                memset(ppath, 0, strlen(ppath));
                strcpy(ppath, oldpath);  
            }

            if(S_ISDIR(fstat.st_mode) != 0)
            {
                delete_subdir(ppath);
                memset(ppath, 0, strlen(ppath));
                strcpy(ppath, oldpath);  
            }
        }
        remove(ppath);
    }
}

void delete_dir(char path[], char mesaj[])
{
    DIR *d;
    char local_path[1000];
    char old_path[1000];
    strcpy(local_path, path);
    strcat(local_path, "/");
    strcat(local_path, mesaj);

    printf("%s\n", local_path);
    d = opendir(local_path);
    struct dirent *dir;

    if(d == NULL)
    {
        printf("%s\n", "Nu s-a putut sterge directorul selectat");
        return;
    }

    while(dir = readdir(d))
    {
        if(strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
            continue;
            
        struct stat filestat;

        
        memset(old_path, 0, strlen(old_path));
        strcpy(old_path, local_path);
       
        strcat(local_path, "/");
        strcat(local_path, dir->d_name);
        stat(local_path, &filestat);

        if(S_ISREG(filestat.st_mode) != 0)
        {
            remove(local_path);
            memset(local_path, 0, strlen(local_path));
            strcpy(local_path, old_path);
        }

        
        if(S_ISDIR(filestat.st_mode) != 0)
        {
            delete_subdir(local_path);
            memset(local_path, 0, strlen(local_path));
            strcpy(local_path, old_path);
        }
    }

    if(remove(local_path) != 0) 
    {
        perror("Eroare la stergerea directorului\n");
        printf("%s\n", "Eroare la eliminarea directorului");
        return ;
    }

    printf("%s\n", "Directorul a fost sters cu succes");
}


void create_file (char path[], char name[]) {

    char local_path[1000];

    strcpy (local_path, path);
    strcat (local_path, "/");
    strcat (local_path, name);

    if (creat (local_path, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) == -1) {
        printf ("%s\n", "Eroarea la crearea fisierului.");
        return;
    } 

    printf ("%s\n", "Fisierul s-a creat cu succes!");


}


void rename_file(char path[], char mesaj[], char mesaj_2[])
{
    char local_path[1000];
    char new_path[1000];

    strcpy(local_path, path);
    strcat(local_path, "/");
    strcat(local_path, mesaj);
    strcpy(new_path, path);
    strcat(new_path, "/");
    strcat(new_path, mesaj_2);

    if(rename(local_path, new_path) != 0 )
    {
        printf("%s\n", "Eroare la redenumirea fisierului");
        return;
    }

    printf("%s\n", "Fisierul a fost redenumit cu succes");
}

int count_lines(char* name)
{
    FILE *f = fopen(name, "r");
    int lines_number = 0;
    for(int i = getc(f); i != EOF; i = getc(f) )
        if(i == '\n')
            lines_number ++;
    fclose(f);
    return lines_number +1;
}

void move_file (char path[], char name[], char  destinatie[]) {

    int ch;
    FILE *f1;
    FILE* f2;

    char local_path1[1000];
    char local_path2[1000];

    strcpy (local_path1, path);

    strcat (local_path1, "/");
    strcat (local_path1, name);

    strcpy (local_path2, path);

    strcat (local_path2, destinatie);
    strcat (local_path2, "/");
    strcat (local_path2, name);

    f1 = fopen (local_path1, "r");
    f2 = fopen (local_path2, "w");

     while ((ch = fgetc(f1)) != EOF) {
         fputc(ch, f2);
    }

    fclose (f1);
    fclose (f2);

    unlink(local_path1);


}



int add_in_file (char username[], char password[]) {

    FILE *f;

    f = fopen ("login.txt", "w");

    if (f == NULL) {

        printf ("%s\n", "Eroare la deschiderea fisierului pentru adaugarea utilizatorilor.");
        return 0;
    }

    else {

      //  int line = number_lines ("login.txt"); //nu stiu daca am nevoie de asta
        char linie_fisier[100];
        char ch = '\n';
        strcpy (linie_fisier, username);
        strcat (linie_fisier, " ");
        strcat (linie_fisier, password);

        fputs (linie_fisier,f);
        fputc (ch, f);
     //   fprintf ("\n");
    }

    fclose(f);
    return 1;
}


#define port 2090
#define key 290103

int main()
{
    int clientSocket;
    struct sockaddr_in serverAddr;
    

    //creare socket
    if ((clientSocket = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("Eroare la socket().\n");
      return 0;
    }

    //umplere structura socket
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 

    //conectare server
    if (connect (clientSocket, (struct sockaddr *) &serverAddr, sizeof (struct sockaddr)) == -1)
    {
      perror ("[client]Eroare la connect().\n");
      return 0;
    }

    char username[100];
    char password[100];
    char comanda[30];
    char raspuns_server[1000];
    char mesaj[100];
    char mesaj_2[100];
    char vreau_cont[3];
 //   char mesaj6[100];
  //  char mesaj7[100];
    int size;
    int logat = 0;
    int status;
    int nr_incercari = 0;
    char local_path[1000];
    char comenzi_disponibile[] = {"exit login crcont logout list help crdir chdir mvfile crfile deldir delfile renamefile location get\0"};
    char comenzi_backlist[100];
    int blacklist = 0;
    char path[1000];
    getcwd(path, sizeof(path));
    path[strlen(path)] = '\0';  

    printf ("%s\n", "Bine ai venit!");

    while(1)
    {
        printf ("%s", "\n");
        
        printf("%s", "Introduceti comanda dorita : ");
        memset(comanda, 0, strlen(comanda));        
        memset(raspuns_server, 0, strlen(raspuns_server));   
        scanf("%s", comanda);
        //fgets(comanda, sizeof(comanda), stdin);
        //comanda[strcspn(comanda, "\n")] = '\0';


        //ca sa nu cer comenzile disponibile de la server, ele sunt salvate si local
        if (strcmp(comanda, "help") == 0 && logat == 1)    
        {
            if(blacklist == 1) 
                printf("%s\n", "Anumite comenzi va sunt restrictionate.");
                printf("%s%s\n", "Comenzi disponibile : " , comenzi_disponibile);
            continue;
        }

        if (strcmp(comanda, "help") == 0 && logat == 0) {

            printf ("%s\n", "Trebuie sa fii logat pentru a folosi aceasta comanda!");
            printf ("%s\n", "Acceseaza comanda chelp pentru a vedea ce comenzi poti folosi fara a fi logat.");
            continue;
        }

        if (strcmp(comanda, "list") == 0 && logat == 0)
        {
            printf("%s\n", "Trebuie sa fii logat pentru a folosi acesta comanda");
            continue;
        }

        if (strcmp(comanda, "logout") == 0 && logat == 0)
        {
            printf("%s\n", "Trebuie sa fii logat pentru a folosi acesta comanda");
            continue;
        }

        if (strcmp(comanda, "location") == 0 && logat == 0)
        {
            printf("%s\n", "Trebuie sa fii logat pentru a folosi acesta comanda");
            continue;
        }

        if(strcmp(comanda, "chdir") == 0 && logat == 0)
        {
            printf("%s\n", "Trebuie sa fii logat pentru a folosi acesta comanda");
            continue;
        }

        if(strcmp(comanda, "crdir") == 0 && logat == 0)
        {
            printf("%s\n", "Trebuie sa fii logat pentru a folosi acesta comanda");
            continue;
        }

        if(strcmp(comanda, "deldir") == 0 && logat == 0)
        {
            printf("%s\n", "Trebuie sa fii logat pentru a folosi acesta comanda");
            continue;
        }


        if(strcmp(comanda, "crfile") == 0 && logat == 0)
        {
            printf("%s\n", "Trebuie sa fii logat pentru a folosi acesta comanda");
            continue;
        }


        if(strcmp(comanda, "delfile") == 0 && logat == 0)
        {
            printf("%s\n", "Trebuie sa fii logat pentru a folosi acesta comanda");
            continue;
        }

        if(strcmp(comanda, "renamefile") == 0 && logat == 0)
        {
            printf("%s\n", "Trebuie sa fii logat pentru a folosi acesta comanda");
            continue;
        }

        if(strcmp(comanda, "put") == 0 && logat == 0)
        {
            printf("%s\n", "Trebuie sa fii logat pentru a folosi acesta comanda");
            continue;
        }

        if(strcmp(comanda, "get") == 0 && logat == 0)
        {
            printf("%s\n", "Trebuie sa fii logat pentru a folosi acesta comanda");
            continue;
        }

        //Comenzi pe partea de client
        if(strcmp(comanda, "clocation") == 0)
        {
            printf("%s\n", path);
            continue;
        }
        if(strcmp(comanda, "chelp") == 0)
        {
            printf("%s\n", "Comnenzi disponibile local : clocation clist chelp cchdir ccrdir cdelfile cdeldir crenamefile ");
            continue;
        }
        

        if(strcmp(comanda, "cchdir") == 0)
        {
            memset(mesaj, 0, strlen(mesaj));
            printf("%s\n", "Introduceti '..' pentru a merge la nivelul superior sau numele unui director pentru a se deschide.");
            scanf("%s", mesaj);
            change_dir(path, mesaj);
            continue;
        } 

        if(strcmp(comanda, "ccrdir") == 0)
        {
            printf("%s", "Introduceti numele directorului ce urmeaza a fi creat : ");
            memset(mesaj, 0, strlen(mesaj));
            scanf("%s", mesaj);
            create_dir(path, mesaj);
            continue;
        }

        if(strcmp(comanda, "cdelfile") == 0)
        {
            printf("%s", "Introduceti numele fisierului ce urmeaza a fi sters : ");
            memset(mesaj, 0, strlen(mesaj));
            scanf("%s", mesaj);
            delete_file(path, mesaj);
            continue;
        }

        if(strcmp(comanda, "cdeldir") == 0)
        {   
            printf("%s", "Introduceti numele directorului ce urmeaza a fi sters : ");
            memset(mesaj, 0, strlen(mesaj));
            scanf("%s", mesaj);
            delete_dir(path, mesaj);
            continue;
        }


        if(strcmp(comanda, "crenamefile") == 0)
        {
            printf("%s\n", "Introduceti numele fisierului ce urmeaza a fi redenumit: ");
            memset(mesaj, 0, strlen(mesaj));
            scanf("%s", mesaj);
            printf("%s\n", "Introduceti noul nume al fisierului: ");
            memset(mesaj_2, 0, strlen(mesaj_2));
            scanf("%s", mesaj_2);
            rename_file(path, mesaj, mesaj_2);
            continue;
        }

        signal(SIGCHLD, handle);

        if (strcmp(comanda, "login") == 0  && logat == 1)
        {
            printf("%s\n", "Sunteti deja logat.");
            continue;
        }

        if (strcmp(comanda, "crcont") == 0 && logat == 1) {

            printf ("%s\n", "Sunteti deja logat.");
            continue;
        }
 
        
        if (strstr(comenzi_disponibile, comanda) == NULL && logat == 1)      
        {
            //printf("%s\n", "Nu aveti dreptul sa executati aceasta comanda");
            printf("%s\n", "Comanda necunoscuta, introduceti alta comanda");
            continue;
        } 
    
       

        //comenzi catre server
        size = strlen(comanda);  

        if (write (clientSocket, &size ,sizeof(int)) <= 0)
        {
        perror ("[client]Eroare la write() spre server.\n");
        return 0;
        }

        if( write(clientSocket, comanda, size)  <= 0 )
        {
            printf(" %s\n","Eroare la trimiterea comenzii catre server");
            return 0;
        }   

        
        if (read(clientSocket, &size, sizeof(int)) <= 0 )
            printf("%s\n", "Eroare la citire lungime raspuns");

        if(read(clientSocket, raspuns_server, size) <= 0 )
            printf("%s\n", "Eroare la citire raspuns");



        //Comenzi pe partea de server
        if(strcmp(raspuns_server, "logout") == 0)      
        {
            logat = 0;
            memset(username, 0, strlen(username));
            memset(password, 0, strlen(password));
            read(clientSocket, &size, sizeof(int));
            if(size == 1)
            {
                printf("%s\n", "Ai fost deconectat cu succes!");
            }
            else
            {
                printf("%s\n", "Eroare la deconectare!");
            }
            continue;
        }

        if(strcmp(raspuns_server, "help") == 0)     
        {
            memset(mesaj, 0, strlen(mesaj));
            read(clientSocket, &size, sizeof(int));
            read(clientSocket, mesaj,size);
            printf("%s%s\n", "Comenzi disponibile : ", mesaj);
            printf("%s\n", "De asemenea sunt disponibile si comenzi locale ce nu necesita logare. Folositi comanda chelp pentru a afla mai multe detalii.");
        }

        if (strcmp(raspuns_server,"login") == 0) {

            memset(raspuns_server, 0, strlen(raspuns_server));

            printf("%s", "Introduceti username : ");
            scanf("%s", username);

            size = strlen(username);

            write(clientSocket, &size, sizeof(int));
            write(clientSocket, username, size);

            printf("%s", "Introduceti parola : ");
            scanf("%s", password);

            // se cripteaza parola

            encrypt_passwd (password, key);

            size = strlen(password);

            write(clientSocket, &size, sizeof(int));
            write(clientSocket, password, size);

            memset (raspuns_server, 0, strlen(raspuns_server));
            read(clientSocket, &size, sizeof(int));
            read(clientSocket, raspuns_server, size);

            if (strcmp(raspuns_server,"succes") == 0) {

                logat = 1;
                printf("%s\n", "Logarea s-a realizat cu succes!");
                // treaba cu statusul
            }

            if (strcmp(raspuns_server, "isblack") == 0) {
                
                blacklist = 1;
                printf ("%s\n", "Nu puteti folosi anumite comenzi!");
            }

            if (strcmp(raspuns_server,"fail") == 0) {

                printf("%s\n", "Logarea a esuat!");
                printf("%s\n", "Nu ai un cont. Trebuie sa iti creezi unul!");
                printf("%s\n", "Acceseaza comanda crcont.");
            }
        }

        if (strcmp(raspuns_server, "crcont") == 0) {

            printf ("%s\n", "Aveti dreptul la 3 incercari. Daca depasiti acest numar, veti fi adaugat in blacklist.");

            nr_incercari++;

            //printf ("%d\n", nr_incercari);

            printf ("%s", "Introduceti username-ul dorit : ");
            scanf ("%s", username);

            size = strlen(username);

            write(clientSocket, &size, sizeof(int));
            write(clientSocket, username, size);

            printf("%s", "Introduceti parola dorita : ");
            scanf("%s", password);

            encrypt_passwd (password, key);

            size = strlen(password);

            write(clientSocket, &size, sizeof(int));
            write(clientSocket, password, size);

            if (nr_incercari == 4) {

                printf ("%s\n", "Ati depasit numarul de incercari...veti fi adaugat in blacklist.");
                blacklist = 1;
                write(clientSocket, &nr_incercari, sizeof(int));

                continue;
            }


            memset (raspuns_server, 0, strlen(raspuns_server));
            read(clientSocket, &size, sizeof(int));
            read(clientSocket, raspuns_server, size);

            if (strcmp(raspuns_server, "exista_deja") == 0) {

                printf ("%s\n", "Te rugam sa alegi alt username. Acesta a fost folosit deja.");
                printf ("%s\n", "Apeleaza din nou comanda crcont pentru a reincerca.");
                continue;
            }

            if (strcmp(raspuns_server, "add_succ") == 0) {

                logat = 1;
                printf("%s\n", "Contul dumneavoastra a fost creat cu succes!");
                continue;
            }

            if (strcmp(raspuns_server, "add_fail") == 0) {

                printf("%s\n", "Contul dumneavoastra nu a putut fi creat. Incercati din nou.");
                continue;
            }

            if (strcmp(raspuns_server, "add_blacklist") == 0) {

                printf ("%s\n", "Ai fost adaugat in backlist deoarece ai incercat sa accesezi contul altei persoane!");
                printf ("%s\n", "Creaza-ti propriul cont!!");
                continue;

            }
            continue;

        }


        if(strcmp(raspuns_server, "location") == 0) 
        {
            memset(mesaj, 0, strlen(mesaj));
            memset(raspuns_server, 0, strlen(raspuns_server));
            read(clientSocket, &size, sizeof(int));
            read(clientSocket, mesaj, size);
            strncpy(raspuns_server, mesaj, strlen(mesaj)-1);
            printf("%s\n", raspuns_server);
            continue;
        }

        if(strcmp(raspuns_server, "crdir") == 0)  
        {
            if (blacklist == 1) {
                
                printf ("%s\n", "Nu puteti folosi aceasta comanda.....Va aflati in blacklist.");
                continue;
            }
            printf("%s", "Introduceti numele directorului ce urmeaza a fi creat : ");

            memset(mesaj, 0, strlen(mesaj));
            scanf("%s", mesaj);
            size = strlen(mesaj);

            write(clientSocket, &size, sizeof(int));
            write(clientSocket, mesaj, size);

            size = 0;
            read(clientSocket, &size, sizeof(int)); 

            if(size == 0)
                printf("%s\n", "Directorul nu s-a putut crea.");
            else
                printf("%s\n", "Directorul s-a creat cu succes.");
            
            continue;
        }

        if(strcmp(raspuns_server, "chdir") == 0)   
        {
            printf("%s\n", "Introduceti '..' pentru a merge la nivelul superior sau numele unui director pentru a se deschide.");
            memset(mesaj, 0, strlen(mesaj));
            scanf("%s", mesaj);
            size = strlen(mesaj) + 1;
            write(clientSocket, &size, sizeof(int));
            write(clientSocket, mesaj, size);
            memset(mesaj, 0, strlen(mesaj));
            read(clientSocket, &size, sizeof(int));
            read(clientSocket, mesaj, size);

           
            if(strcmp(mesaj, "MaxHeight") == 0)
                printf("%s\n", "Nu se poate trece la un nivel superior, radacina a fost atinsa");
            else
                if(strcmp(mesaj, "NuExista") == 0)
                    printf("%s\n", "Directorul selectat nu exista");
                else
                {
                    memset(raspuns_server, 0, strlen(raspuns_server));
                    strncpy(raspuns_server,mesaj,strlen(mesaj)-1);
                    printf("%s%s\n","Noua locatie este : " , raspuns_server);
                }
            // continue;
        }                                      

        if(strcmp(raspuns_server, "delfile") == 0)   
        {
            if (blacklist == 1) {
                
                printf ("%s\n", "Nu puteti folosi aceasta comanda.....Va aflati in blacklist.");
                continue;
            }

            printf("%s\n", "Introduceti numele fisierului ce urmeaza a fi sters : ");
            memset(mesaj, 0, strlen(mesaj));
            scanf("%s", mesaj);
            size = strlen(mesaj) + 1;
            write(clientSocket, &size, sizeof(int));
            write(clientSocket, mesaj, size);
            read(clientSocket, &size, sizeof(int));
            if(size == 0)
                printf("%s\n", "Fisierul s-a sters cu succes");  
            else if (size == -1)
                printf("%s\n", "Fisierul nu s-a putut sterge");   
          //  continue;
        }

        if (strcmp(raspuns_server, "deldir") == 0)
        {
            if (blacklist == 1) {
                
                printf ("%s\n", "Nu puteti folosi aceasta comanda.....Va aflati in blacklist.");
                continue;
            }

            memset(mesaj, 0, strlen(mesaj));
            printf("%s", "Introduceti numele directorului ce urmeaza a fi sters : ");

            scanf("%s", mesaj);
            size = strlen(mesaj);

            write(clientSocket, &size, sizeof(int));
            write(clientSocket, mesaj, size);

            read(clientSocket, &size, sizeof(int));

            if(size == 0)
                printf("%s\n", "Directorul s-a sters cu succes");  
            else
                printf("%s\n", "Directorul nu s-a putut sterge");  
        }


        if (strcmp(raspuns_server, "crfile") == 0) {

            if (blacklist == 1) {
                
                printf ("%s\n", "Nu puteti folosi aceasta comanda.....Va aflati in blacklist.");
                continue;
            }

            memset(mesaj, 0, strlen(mesaj));

            printf("%s", "Introduceti numele fisierului ce urmeaza a fi creat : ");

            scanf("%s", mesaj);
            size = strlen(mesaj);

            write(clientSocket, &size, sizeof(int));
            write(clientSocket, mesaj, size);

            size = 0;
            read(clientSocket, &size, sizeof(int));

            if(size == 0)
                printf("%s\n", "Fisierul s-a creat cu succes.");  
            else
                printf("%s\n", "Fisierul nu s-a putut crea.");  

            continue;
        }

        if (strcmp(raspuns_server, "renamefile") == 0) 
        {

            if (blacklist == 1) {
                
                printf ("%s\n", "Nu puteti folosi aceasta comanda.....Va aflati in blacklist.");
                continue;
            }

            memset(mesaj, 0, strlen(mesaj));
            memset(mesaj_2, 0, strlen(mesaj_2));
            printf("%s\n", "Introduceti numele fisierului ce va fi redenumit :");
            scanf("%s", mesaj);
            printf("%s\n", "Introduceti noul nume al fisierlui:");
            scanf("%s", mesaj_2);
            size = strlen(mesaj);
            write(clientSocket, &size, sizeof(int));
            write(clientSocket, mesaj, size);
            size = strlen(mesaj_2);
            write(clientSocket, &size, sizeof(int));
            write(clientSocket, mesaj_2, size);
            read(clientSocket, &size, sizeof(int));
            if(size == -1)
            {
                printf("%s\n", "Eroare la redenumire, fisierul introdus este un director");
              //  continue;
            }
            if(size == 0)
            {
                printf("%s\n", "Eroare la redenumirea fisierlui");
              //  continue;
            }
            if(size == 1)
            {
                printf("%s\n", "Fisierul a fost redenumit cu succes");
                continue;
            }
            continue;
        }

        if(strcmp(raspuns_server, "mvfile") == 0)
        {

            if (blacklist == 1) {
                
                printf ("%s\n", "Nu puteti folosi aceasta comanda.....Va aflati in blacklist.");
                continue;
            }

            printf("%s\n", "Introduceti numele fisierului ce urmeaza a fi mutat: ");
            memset(mesaj, 0, strlen(mesaj));
            scanf("%s", mesaj);
            printf("%s\n", "Introduceti noua destinatie a fisierlui: ");
            memset(mesaj_2, 0, strlen(mesaj_2));
            scanf("%s", mesaj_2);
            size = strlen(mesaj);
            write(clientSocket, &size, sizeof(int));
            write(clientSocket, mesaj, size);
            size = strlen(mesaj_2);
            write(clientSocket, &size, sizeof(int));
            write(clientSocket, mesaj_2, size);

            read(clientSocket, &status, sizeof(int));

            if(status == -2)
            {
                printf("%s\n", "Fisierul dat nu exista in directorul curent.");
                continue;
            }
            if(status == -3)
            {
                printf("%s\n", "Destinatia introdusa nu este corecta.");
                continue;
            }

            if(status == -1)
            {
                printf("%s\n", "Fisierul nu s-a putut transfera.");
                continue;
            }
            printf("%s\n", "Fisierul s-a terminat de transferat."); 
            continue;
        }

        if(strcmp(raspuns_server, "get") == 0)
        {
            memset(mesaj, 0, strlen(mesaj));
            printf("%s\n", "Introduceti numele fisierului ce va fi luat de pe server: ");
            scanf("%s", mesaj);
            size = strlen(mesaj);
            write(clientSocket, &size, sizeof(int));
            write(clientSocket, mesaj, size);

    
            status = 1;
            status = exists(path, mesaj);
            write(clientSocket, &status, sizeof(int));
            if(status == 0)
            {
                printf("%s\n", "Exista deja un fisier cu acelasi nume in directorul curent");
                continue;
            }

            status = 1;
            read(clientSocket, &status, sizeof(int));
            if(status == 0)
            {
                printf("%s\n", "Fisierul cerut nu exista pe server");
                continue;
            }

            // transferul
            memset(local_path, 0, strlen(local_path));
            strcpy(local_path, path);
            strcat(local_path, "/");
            strcat(local_path, mesaj);
            FILE *f;
            f = fopen(local_path, "w");
            status = 1;
            if(f ==  NULL)
            {
                printf("%s\n", "Eroare la crearea noului fisierului");
                status = 0;
            }

            write(clientSocket, &status, sizeof(int));
            if(status == 0)
                continue;

            while(1)
                {
                    unsigned char data[1] = {0};
                    int leng = 0;
                    read(clientSocket, &leng,  sizeof(int));
                    if(leng == -1)
                    {
                        printf("%s\n", "Fisierul a fost primit cu succes");
                        break;
                    }
                    if(leng == -2)
                    {
                        printf("%s\n", "Eroare la citirea fisierlui pe server");
                        break;
                    }

                    read(clientSocket, data, leng);
                    if(leng > 0)
                    {
                       fwrite(data, 1, leng, f);
                    }
                }
                fclose(f);
                continue;
        } 

        if(strcmp(raspuns_server, "necunoscut") == 0) 
        {
            printf("%s\n", "Comanda necunoscuta, introduceti alta comanda");
            continue;
        }

        if(strcmp(raspuns_server, "exit") == 0) 
        {   
            printf ("%s\n", "Ati iesit.");
            close (clientSocket);
            return 0;
        }
   // close (clientSocket);
  //  return 0;  
    }
    close (clientSocket);
    return 0; 
} 

