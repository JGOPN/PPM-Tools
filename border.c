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

void fillMat(int width, int height, PIXEL bord, PIXEL ** tab){
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
        {
            tab[i][j] = bord;
        }   
}

void addBorder(int width, int height, int bwidth, int bheight, int npix, PIXEL ** tab, PIXEL ** tabout){
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            tabout[i+npix][j+npix] = tab[i][j];
}

int main(int argc, char const *argv[]){
    //Nao funciona pra img.ppm (mas sim pra no_comments)
    int width,height,max;
    PIXEL bord; //pixel com cor da moldura
    bord.r = atoi(argv[2]);
    bord.g = atoi(argv[3]);
    bord.b = atoi(argv[4]);
    int npixels = atoi(argv[1]);

    if(argc==7){
        FILE* f = openFile(argv[5],"r");
        readHeader(&width,&height,&max,f);

        PIXEL** tab = newMat(width,height);
        readValues(width,height,max,f,tab);

        int outwidth = width+(npixels*2);
        int outheight = height+(npixels*2);
        PIXEL** tabout = newMat(outwidth,outheight);
        fillMat(outwidth,outheight,bord,tabout);

        FILE* out = openFile(argv[6],"w");
        addBorder(width,height,outwidth,outheight,npixels,tab,tabout);
    
        saveHeader(out,outwidth,outheight,max);
        outputPixels(out,outwidth,outheight,tabout);
        closeFile(f);
        closeFile(out);
    }
    else if(argc==6){
        FILE* f = openFile(argv[5],"r");
        readHeader(&width,&height,&max,f);

        PIXEL ** tab = newMat(width,height);
        readValues(width,height,max,f,tab);

        int outwidth = width+(npixels*2);
        int outheight = height+(npixels*2);
        PIXEL** tabout = newMat(outwidth,outheight);
        fillMat(outwidth,outheight,bord,tabout);

        addBorder(width,height,outwidth,outheight,npixels,tab,tabout);
    
        saveHeader(stdout,outwidth,outheight,max);
        outputPixels(stdout,outwidth,outheight,tabout);

        closeFile(f);
    }
    else if(argc==5){
        readHeader(&width,&height,&max,stdin);
        PIXEL ** tab = newMat(width,height);
        readValues(width,height,max,stdin,tab);

		int outwidth = width+(npixels*2);
        int outheight = height+(npixels*2);
        PIXEL** tabout = newMat(outwidth,outheight);
        fillMat(outwidth,outheight,bord,tabout);

        addBorder(width,height,outwidth,outheight,npixels,tab,tabout);
    
        saveHeader(stdout,outwidth,outheight,max);
        outputPixels(stdout,outwidth,outheight,tabout);
    }
    else{
        fprintf(stderr,"NUMERO ERRADO DE ARGUMENTOS");
    }
}