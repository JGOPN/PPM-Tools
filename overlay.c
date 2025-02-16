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

void overlayImgs(int width, int height, int owidth, int oheight, int x, int y, PIXEL ** overlay, PIXEL ** tabout){
    for (int i = 0; i < oheight && (i+y < height); i++)
        for (int j = 0; j < owidth && (j+x < width); j++)
            tabout[i+y][j+x] = overlay[i][j];

}

int main(int argc, char const *argv[]){
    int owidth,oheight,omax;
    int width,height,max;
    int x=atoi(argv[1]);
    int y=atoi(argv[2]);


    if(argc==6){
        FILE* overf = openFile(argv[3],"r");
        readHeader(&owidth,&oheight,&omax,overf);
        PIXEL** overlay = newMat(owidth,oheight);
        readValues(owidth,oheight,omax,overf,overlay);

        FILE* basef = openFile(argv[4],"r");
        readHeader(&width,&height,&max,basef);
        PIXEL** base = newMat(width,height);
        readValues(width,height,max,basef,base);

        FILE* out = openFile(argv[5],"w");
        saveHeader(out,width,height,max);
        PIXEL** tabout = newMat(width,height);
        tabout=base;
        if(y<=height && x<=width) overlayImgs(width,height,owidth,oheight,x,y,overlay,tabout);

        outputPixels(out,width,height,tabout);

        closeFile(overf);
        closeFile(basef);
        closeFile(out);
    }
    else if(argc==5){
        FILE* overf = openFile(argv[3],"r");
        readHeader(&owidth,&oheight,&omax,overf);
        PIXEL** overlay = newMat(owidth,oheight);
        readValues(owidth,oheight,omax,overf,overlay);

        FILE* basef = openFile(argv[4],"r");
        readHeader(&width,&height,&max,basef);
        PIXEL** base = newMat(width,height);
        readValues(width,height,max,basef,base);

        saveHeader(stdout,width,height,max);
        PIXEL** tabout = newMat(width,height);
        tabout=base;
        if(y<=height && x<=width) overlayImgs(width,height,owidth,oheight,x,y,overlay,tabout);

        outputPixels(stdout,width,height,tabout);

        closeFile(overf);
        closeFile(basef);
    }
    else if(argc==4){
        FILE* overf = openFile(argv[3],"r");
        readHeader(&owidth,&oheight,&omax,overf);
        PIXEL** overlay = newMat(owidth,oheight);
        readValues(owidth,oheight,omax,overf,overlay);

        readHeader(&width,&height,&max,stdin);
        PIXEL** base = newMat(width,height);
        readValues(width,height,max,stdin,base);

        saveHeader(stdout,width,height,max);
        PIXEL** tabout = newMat(width,height);
        tabout=base;
        if(y<=height && x<=width) overlayImgs(width,height,owidth,oheight,x,y,overlay,tabout);

        outputPixels(stdout,width,height,tabout);

        closeFile(overf);
    }
    else{
        fprintf(stderr,"NUMERO ERRADO DE ARGUMENTOS");
    }
}