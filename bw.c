#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define THRESHOLD argv[1]


typedef struct pixel{
        int r,g,b;
}PIXEL;

PIXEL ** newMat(int width, int height){
    PIXEL ** tab = (PIXEL **) malloc(height * sizeof(PIXEL *));
        for(int i=0; i< height; i++){
            tab[i] = (PIXEL *) malloc(width * sizeof(PIXEL));
        }
    return tab;
}

FILE* openFile(char const* s, char* rw){ //Abre o arquivo
    FILE* f = fopen(s,rw);
    if(f==NULL) {
        fprintf(stderr,"ERRO AO ABRIR FICHEIRO");
        exit(2);
        }
    return f;
}

int closeFile(FILE* f){
    if(fclose(f)!=0){
        fprintf(stderr,"ERRO AO FECHAR FICHEIRO");
        exit(3);
    }
    return 0;
}

char* readLine(FILE* f){ //le linha do ficheiro, 
    //se for comment, chama a si recursivamente 
    char * line = NULL;
    size_t len = 0;
    getline(&line, &len, f);
    if (line[0]=='#') line=readLine(f);
    return line;
}

void readHeader(int *width,int *height, int* max, FILE* f){
    char * line = NULL;
    line=readLine(f);
    if(strcmp(line,"P3\n")!=0){ 
        fprintf(stderr,"PPM ERRADO");
        exit(4);
    }
    
    line=readLine(f);
    sscanf(line,"%d %d",width,height);//le height width
    
    line=readLine(f);
    sscanf(line,"%d",max);//le maxvalue
}

PIXEL readPixel(FILE *f){
    PIXEL p;
    fscanf(f,"%d %d %d",&p.r,&p.g,&p.b);
    return p;
}

void readValues(int width,int height, int max, FILE* f, PIXEL ** tab){
    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            tab[i][j] = readPixel(f);
        }
    }
    
}

void saveHeader(FILE *out, int n, int m, int max){
    char *str = malloc(sizeof(char) * 20);
    sprintf(str,"P3\n%d %d\n%d\n",n,m,max);
    fputs(str,out);
    free(str);
}

void outputPixels(FILE *out, int width, int height, PIXEL** tabout){
    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            fprintf(out,"%d %d %d\n",tabout[i][j].r,tabout[i][j].g,tabout[i][j].b);
        }
    }
}

PIXEL fillPixel(PIXEL p, int color){
    p.r=color;
    p.g=color;
    p.b=color;
    return p;
}

PIXEL pixelBW(int t, int max, PIXEL p){
    int gr = round (p.r*0.2126 + p.g*0.7152 + p.b*0.0722);
    if(gr>t){ //Se for maior que o threshold t preenche com max
        p=fillPixel(p,max);
    }
    else p=fillPixel(p,0); //Se for menor que o threshold t preenche com 0
    return p;
}

void toBW(int width, int height, int max, int t, PIXEL ** tab){
    for (int i = 0; i < height; i++)
        for (int j=0; j < width; j++){
            tab[i][j] = pixelBW(t, max, tab[i][j]);
        }    
}

int main(int argc, char const *argv[]){
    int width,height,max;

    if(argc==4){
        FILE* f = openFile(argv[2],"r");
        readHeader(&width,&height,&max,f);

        PIXEL** tab = newMat(width,height);
        readValues(width,height,max,f,tab);

        FILE* out = openFile(argv[3],"w");
        saveHeader(out,width,height,max);
        toBW(width,height,max,atoi(THRESHOLD),tab);
        outputPixels(out,width,height,tab);

        closeFile(f);
        closeFile(out);
    }
    else if(argc==3){
        FILE* f = openFile(argv[2],"r");
        readHeader(&width,&height,&max,f);

        PIXEL ** tab = newMat(width,height);
        readValues(width,height,max,f,tab);

        saveHeader(stdout,width,height,max);
        toBW(width,height,max,atoi(THRESHOLD),tab);
        outputPixels(stdout,width,height,tab);

        closeFile(f);
    }
    else if(argc==2){
        readHeader(&width,&height,&max,stdin);
        PIXEL ** tab = newMat(width,height);
        readValues(width,height,max,stdin,tab);

		saveHeader(stdout,width,height,max);
        toBW(width,height,max,atoi(THRESHOLD),tab);        
        outputPixels(stdout,width,height,tab);
    }
    else{
        fprintf(stderr,"NUMERO ERRADO DE ARGUMENTOS");
    }
}