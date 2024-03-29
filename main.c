#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "inttypes.h"

#pragma pack(1) //desativa o padding da struct

/**
 * @author Arthur Benites da Silva - Turma 4323
 * @brief Programa recebe um arquivo BMP, e gera 4 arquivos com canais de cores diferentes, além de mostrar dados do arquivo de entrada
 * @date 22/11/2021
**/


void generate_output_name(char *input, char *str);
void close_files(FILE* img_array[]);

char name_in[30];

int main(void){



    char name_out_r[35];
    char name_out_g[35];
    char name_out_b[35];
    char name_out_gs[35];

    typedef struct{
        char id[2];
        uint32_t size;
        uint16_t reser[2];
        uint32_t offset;
        uint32_t bisize;
        uint32_t width;
        uint32_t height;
        uint16_t planes;
        uint16_t colors;
        uint32_t compress;
        uint32_t sizeimg;
        uint32_t xppmeter;
        uint32_t yppmeter;
        uint32_t colorused;
        uint32_t colorused_important;
    }header;

    typedef struct{
        //em imagens de 24 bits, a ordem é BGR
        uint8_t blue; 
        uint8_t green;
        uint8_t red;
    }BGR;
    
    header id;
    BGR pixel, pixel_r, pixel_g, pixel_b, pixel_gs;
    uint32_t gray;
    uint8_t empty = 0;
    uint32_t qtdBytes = 0;


    puts("Digite o nome do arquivo (SEM O .BMP)");
    gets(name_in);

    
    generate_output_name(name_out_r, "_R.bmp");
    generate_output_name(name_out_g, "_G.bmp");
    generate_output_name(name_out_b, "_B.bmp");
    generate_output_name(name_out_gs, "_GS.bmp");

    generate_output_name(name_in, ".bmp");

    
    FILE* img_in = fopen(name_in, "rb");

    if(img_in == NULL){
        puts("Falha em abrir o arquivo de entrada.");
        return -1;
    } //não gera os arquivos de saída em caso de erro

    FILE* img_out_r = fopen(name_out_r, "wb");
    FILE* img_out_g = fopen(name_out_g, "wb");
    FILE* img_out_b = fopen(name_out_b, "wb");
    FILE* img_out_gs = fopen(name_out_gs, "wb");

    FILE* img_array[] = {
        img_out_r,
        img_out_g,
        img_out_b,
        img_out_gs
    };

    for(int i=0; i < 4; i++){
        if(img_array[i] == NULL) {
            puts("Falha em abrir os arquivos de entrada");
            return -1;
        }
    }


    //pegando informações
    fread(&id, sizeof(header), 1, img_in);

    if(id.id[0] != 'B' || id.id[1] != 'M'){
        puts("Arquivo BMP inválido");
        return -1;
    }

    qtdBytes = id.height * 3;
    if(!(qtdBytes % 4)) qtdBytes = 0;

    while(qtdBytes % 4 != 0){
        qtdBytes++; //valor que gera um número divisível por 4
    }
    
    printf("Quantidade de bytes que faltam: %d\n", qtdBytes);

    for(int i=0; i < 4; i++){
        fwrite(&id, sizeof(header), 1, img_array[i]); // copiando o cabeçalho para os arquivos de saída
    }

    for(int i=0; i<(id.width * id.height); i++){ //percorre a área do retângulo
        fread(&pixel, sizeof(BGR), 1, img_in); //lê a cor de um pixel
        //imagem cinza
        gray = (pixel.red + pixel.green + pixel.blue) / 3;
        pixel_gs.red = gray;
        pixel_gs.green = gray;
        pixel_gs.blue = gray;
        fwrite(&pixel_gs, sizeof(BGR), 1, img_out_gs);
        //imagem vermelha
        pixel_r.red = pixel.red;
        pixel_r.green = 0x00;
        pixel_r.blue = 0x0;
        fwrite(&pixel_r, sizeof(BGR), 1,  img_out_r);
        //imagem verde
        pixel_g.red = 0x00;
        pixel_g.green = pixel.green;
        pixel_g.blue = 0x00;
        fwrite(&pixel_g, sizeof(BGR), 1,  img_out_g);       
        //imagem azul
        pixel_b.red = 0x00;
        pixel_b.green = 0x00;
        pixel_b.blue = pixel.blue;
        fwrite(&pixel_b, sizeof(BGR), 1,  img_out_b);
    }

    for(int i = 0; i < 4; i++){ // escreve os bytes necessários para o arquivo BMP ser válido
        fwrite(&empty, 1, qtdBytes, img_array[i]);
    }


    printf("Tamanho do arquivo: %d Kb\n", id.size/1024);
    printf("Largura: %d\nAltura: %d\n", id.width, id.height);
    printf("Profundidade de cores: %d bits (2^%d cores)\n", id.colors, id.colors);
    close_files(img_array);

    return 0;
}


void close_files(FILE* img_array[]){
    for(int i = 0; i < 4; i++){ // fecha os arquivos
        fclose(img_array[i]);
    }
}

void generate_output_name(char input[], char str[]){
    if(input != name_in) strcpy(input, name_in);
    strcat(input, str);
}