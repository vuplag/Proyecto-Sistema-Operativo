#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"


// Funcion auxiliar adaptada de "ls.c" para obtener solo el nombre del archivo a partir de una ruta completa
// (Ej: si la ruta es "a/b/archivo.txt", devuelve un puntero a "archivo.txt")
char* fmtname(char *path) {
    char *p;

    // Buscar el ultimo slash ('/') en la ruta
    for(p=path+strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;

    // retornar una copia limpia del nombre
    return p;
}

void find(char *path, char *filename) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    // 1. Intentar abrir el archivo o directorio con open()
    // El segundo argumento '0' significa modo de solo lectura (O_RDONLY)
    if((fd = open(path, 0)) < 0){
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    // 2. Obtener informacion de metadatos (como si es archivo o directorio) usando fstat()
    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch(st.type){
    case T_FILE:
        // CASO A: El usuario paso un archivo directamente como argumento inicial (ej: "find README b")
        // Verificamos si el nombre del archivo coincide con el que buscamos.
        if(strcmp(fmtname(path), filename) == 0){
            printf("%s\n", path);
        }
        break;

    case T_DIR:
        // CASO B: Es un directorio. Debemos recorrer todo su contenido
        
        // Verificamos que la ruta no sea demasiado grade para nuestro buffer
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
            printf("find: path too long\n");
            break;
        }
        
        // Copimamos la ruta actual al buffer y le agregamos un '/' al final para concatenar
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        
        // Leemos las entradas del directorio una por una usando un bucle while y read()
        while(read(fd, &de, sizeof(de)) == sizeof(de)){
            // Si el inodo es 0, significa que la entrada del directorio no esta en uso
            if(de.inum == 0)
                continue;
                
            // Evitar recursión infinita en las referencias al directorio actual (".") y padre ("..")
            if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                continue;
                
            // Copiamos el nombre del archivo/directorio (`de.name`) justo despues del '/' en el buffer
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0; // Asegurar el fin de cadena (null-terminator)
            
            // Obtenemos los estadísticos del nuevo archivo/carpeta construido en `buf`
            if(stat(buf, &st) < 0){
                printf("find: cannot stat %s\n", buf);
                continue;
            }
            
            // Si es un directorio, hacemos llamada recursiva bajando un nivel más
            if(st.type == T_DIR) {
                find(buf, filename);
            } 
            // Si es un archivo, comparamos su nombre con el "filename" buscado
            else if(st.type == T_FILE) {
                // Comparamos el nombre del archivo actual con el buscado
                if(strcmp(de.name, filename) == 0) {
                    // Imprimimos la ruta completa calculada en "buf"
                    printf("%s\n", buf);
                }
            }
        }
        break;
    }
    
    // 3. Siempre debemos cerrar el descriptor de archivos al terminar para evitar fugas de recursos
    close(fd);
}

int main(int argc, char *argv[]) {
    // Validar que se pasen exactamente 2 argumentos (el directorio base y el nombre a buscar)
    // Nota: argv[0] es el nombre del programa ("find"), argv[1] es la ruta, argv[2] es el archivo.
    if(argc != 3){
        fprintf(2, "Uso: find <directorio> <archivo>\n");
        exit(1);
    }
    
    // Llamar a la funcion principal pasandole la ruta inicial y el archivo objetivo
    find(argv[1], argv[2]);
    exit(0);
}