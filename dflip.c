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

void horizontalFlip(int width, int height, PIXEL ** tab, PIXEL ** tabout){
    for(int i=0; i<height; i++){
        for (int j=0; j < width; j++)
            tabout[i][j] = tab[i][width-1-j];
    }
}

void verticalFlip(int width, int height, PIXEL ** tab, PIXEL ** tabout){
    for(int i=0; i< height; i++){
		for(int j= 0; j<width; j++){
			tabout[i][j] = tab[height-1-i][j];
		 }
	}
}

void diagonalFlip(int width, int height, PIXEL ** tab, PIXEL ** tabout){
    horizontalFlip(width,height,tab,tabout);
    verticalFlip(width,height,tabout,tab);
}


int main(int argc, char const *argv[]){
    int width,height,max;

    if(argc==3){
        FILE* f = openFile(argv[1],"r");
        readHeader(&width,&height,&max,f);

        PIXEL** tab = newMat(width,height);
        PIXEL** tabout = newMat(width,height);
        readValues(width,height,max,f,tab);

        FILE* out = openFile(argv[2],"w");
        saveHeader(out,width,height,max);
        diagonalFlip(width,height,tab,tabout);
        outputPixels(out,width,height,tab);

        closeFile(f);
        closeFile(out);
    }
    else if(argc==2){
        FILE* f = openFile(argv[1],"r");
        readHeader(&width,&height,&max,f);

        PIXEL ** tab = newMat(width,height);
        PIXEL** tabout = newMat(width,height);
        readValues(width,height,max,f,tab);

        saveHeader(stdout,width,height,max);
        diagonalFlip(width,height,tab, tabout);
        outputPixels(stdout,width,height,tab);

        closeFile(f);
    }
    else if(argc==1){
        readHeader(&width,&height,&max,stdin);
        PIXEL ** tab = newMat(width,height);
        PIXEL** tabout = newMat(width,height);
        readValues(width,height,max,stdin,tab);
		saveHeader(stdout,width,height,max);
        diagonalFlip(width,height,tab,tabout);
        
        outputPixels(stdout,width,height,tab);
    }
    else{
        fprintf(stderr,"NUMERO ERRADO DE ARGUMENTOS");
    }
}