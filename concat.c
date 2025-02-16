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

void tabToOut(int startw, int width, int height, PIXEL ** tab, PIXEL ** tabout){
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            tabout[i][j+startw] = tab[i][j];
}

int main(int argc, char const *argv[]){
    int width1,height1,max;
    int width2,height2;

    if(argc==4){
        FILE* f1 = openFile(argv[1],"r");
        readHeader(&width1,&height1,&max,f1);
        FILE* f2 = openFile(argv[2],"r");
        readHeader(&width2,&height2,&max,f2);

        PIXEL** tab1 = newMat(width1,height1);
        readValues(width1,height1,max,f1,tab1);
        PIXEL** tab2 = newMat(width2,height2);
        readValues(width2,height2,max,f2,tab2);

        int outwidth = width1+width2;
        int outheight = (height1<=height2) ? height1 : height2; //menor altura entre as 2 imagens
        PIXEL** tabout = newMat(outwidth,outheight);

        tabToOut(0,width1,outheight,tab1,tabout);//começa da width 0
        tabToOut(width1,width2,outheight,tab2,tabout);//começa da width da primeira img

        FILE* out = openFile(argv[3],"w");
    
        saveHeader(out,outwidth,outheight,max);
        outputPixels(out,outwidth,outheight,tabout);
        closeFile(f1);
        closeFile(f2);
        closeFile(out);
    }
    else if(argc==3){
        FILE* f1 = openFile(argv[1],"r");
        readHeader(&width1,&height1,&max,f1);
        FILE* f2 = openFile(argv[2],"r");
        readHeader(&width2,&height2,&max,f2);

        PIXEL** tab1 = newMat(width1,height1);
        readValues(width1,height1,max,f1,tab1);
        PIXEL** tab2 = newMat(width2,height2);
        readValues(width2,height2,max,f2,tab2);

        int outwidth = width1+width2;
        int outheight = (height1<=height2) ? height1 : height2; //menor altura entre as 2 imagens
        PIXEL** tabout = newMat(outwidth,outheight);
        tabToOut(0,width1,outheight,tab1,tabout);
        tabToOut(width1,width2,outheight,tab2,tabout);
    
        saveHeader(stdout,outwidth,outheight,max);
        outputPixels(stdout,outwidth,outheight,tabout);
        closeFile(f1);
        closeFile(f2);
    }
    else{
        fprintf(stderr,"NUMERO ERRADO DE ARGUMENTOS");
    }
}