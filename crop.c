#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    line=readLine(f);
    fseek(f,-strlen(line),SEEK_CUR);
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

void crop(int a, int b, int owidth, int oheight, PIXEL ** tab, PIXEL **tabout){
    for (int i = 0; i < oheight; i++)
        for (int j = 0; j < owidth; j++)
        //if((i+b)>oheight && (j+a)>owidth)
            tabout[i][j] = tab[i+b][j+a];
    
}

int main(int argc, char const *argv[]){
    //falta lidar com coordenadas fora da img
    int width,height,max;
    int a = atoi(argv[1]);
    int b = atoi(argv[2]);
    int c = atoi(argv[3]);
    int d = atoi(argv[4]);

    int owidth = c-a+1;
    
    int oheight= d-b+1;

    if(argc==7){
        FILE* f = openFile(argv[5],"r");
        readHeader(&width,&height,&max,f);

        PIXEL** tab = newMat(width,height);
        readValues(width,height,max,f,tab);

        if(a+owidth>width) owidth=width-a;
        if(b+oheight>height) oheight=height-b;

        PIXEL** tabout = newMat(owidth,oheight);
        crop(a,b,owidth,oheight,tab,tabout);

        FILE* out = openFile(argv[6],"w");
    
        saveHeader(out,owidth,oheight,max);
        outputPixels(out,owidth,oheight,tabout);
        closeFile(f);
        closeFile(out);
    }
    else if(argc==6){
        FILE* f = openFile(argv[5],"r");
        readHeader(&width,&height,&max,f);

        PIXEL** tab = newMat(width,height);
        readValues(width,height,max,f,tab);

        PIXEL** tabout = newMat(owidth,oheight);
        crop(a,b,owidth,oheight,tab,tabout);
    
        saveHeader(stdout,owidth,oheight,max);
        outputPixels(stdout,owidth,oheight,tabout);
        closeFile(f);
    }
    else if(argc==5){
        readHeader(&width,&height,&max,stdin);

        PIXEL** tab = newMat(width,height);
        readValues(width,height,max,stdin,tab);

        PIXEL** tabout = newMat(owidth,oheight);
        crop(a,b,owidth,oheight,tab,tabout);
    
        saveHeader(stdout,owidth,oheight,max);
        outputPixels(stdout,owidth,oheight,tabout);
    }
    else{
        fprintf(stderr,"NUMERO ERRADO DE ARGUMENTOS");
    }
}