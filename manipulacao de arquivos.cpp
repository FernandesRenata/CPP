#include <iostream>
#include <fstream>
#include <cstdint>
#include <string>
#include <locale>
#include <istream>
#include <sstream>


using namespace std;

#pragma pack(push, 1)


struct headBMP_t {
    char tipo[2];
    uint32_t tamanho;
    uint32_t reservado;
    uint32_t offset;
    uint32_t cabecalho;
    uint32_t largura;
    uint32_t altura;
    uint16_t planos;
    uint16_t pixel;
    uint32_t compressao;
    uint32_t imagem;
    uint32_t resolucao_h;
    uint32_t resolucao_v;
    uint32_t cores;
    uint32_t coresimp;
};

#pragma pack(pop)

int main() {
    headBMP_t himg;

    setlocale(LC_ALL, "");

    string nomedoarquivo;
    cout << "Digite o nome do arquivo com a extensão:" << endl;
    cin >> nomedoarquivo;

    string gs = nomedoarquivo;
    string pretobranco = nomedoarquivo;
    string arquivinho = nomedoarquivo;

    ifstream arquivo(nomedoarquivo, ios::binary); // Abre o arquivo no modo binário

    if (!arquivo.is_open()) {
        cerr << "Não foi possível abrir o arquivo." << endl;
        return 1;
    }
    arquivo.read((char *)&himg, sizeof(himg));

    size_t pontobmp = nomedoarquivo.find(".bmp");

    if (pontobmp == string::npos) {
        cerr << "O arquivo " << nomedoarquivo << " não é BMP." << endl;
        arquivo.close();
        return 1;
    }

    //Acrescenta os bits que faltam
    int larg_nova;
    int acrescenta;
    larg_nova = himg.largura * 3;
    if ((larg_nova % 4) != 0) {
        acrescenta = 4 - (larg_nova % 4);
        larg_nova = (larg_nova + acrescenta);
    }
    int dados;
    dados = larg_nova * himg.altura;

    uint8_t *imagem = new uint8_t[dados]; // Cria a variavel imagem utilizando alocação dinâmica



    ofstream arquivogs;

    size_t posicao = gs.find('.');

    gs.insert(posicao, "_gs");

    arquivogs.open(gs, ios::binary | ios::out); // Abre o arquivo para saída no modo binário
    if (!arquivogs) {
        cerr << "O arquivo de saída não pode ser gerado." << endl;
        return -1;
    }

    arquivogs.write((char *)&himg, sizeof(himg)); // Escreve o cabeçalho no arquivo de saída

    arquivo.read((char *)imagem, dados); // Lê o restante da imagem

    //Transformando imagem em gray scale

    for (int x = 0; x < dados; x += 3) {
        imagem[x] = ((imagem[x] * 30) + (imagem[x + 1] * 59) + (imagem[x + 2] * 11)) / 100;
        imagem[x + 1] = imagem[x];
        imagem[x + 2] = imagem[x];
    }

    cout << "O nome do arquivo é: " << nomedoarquivo << endl;
    cout << "O tamanho da imagem é: " << himg.altura << "x" << himg.largura << endl;
    cout << "A profundidade de cores é: " << himg.pixel << endl << endl;

    arquivogs.write((char *)imagem, dados); // Escreve os dados da imagem processada no arquivo de saída
    cout << "Seu arquivo em escala de cinza está pronto." << endl << endl;

    arquivogs.close();


    int valorlimiar = -1; // O valor inicia como -1 para que o while inicialize

    while (valorlimiar < 0 || valorlimiar > 255) {
        cout << "Qual o valor de limiarização da imagem?" << endl;
        cin >> valorlimiar;
    }

    int preto = 0;
    int branco = 0;
    int quantidade = 0;
    for (int x = 0; x < dados; x++) {
        if (imagem[x] < valorlimiar) {
            imagem[x] = 0;
            preto++;
        } else {
            imagem[x] = 255;
            branco++;
        }
    }

    //Diz a predominância de tom da imagem, podendo ser preto ou branco
    quantidade = preto + branco;
    if (preto >= (quantidade * 0.5)) {
        cout << "Sua imagem será predominantemente preta." << endl;
    }
    if (branco >= (quantidade * 0.5)) {
        cout << "Sua imagem será predominantemente branca." << endl;
    }

    cout << "Seu arquivo em preto e branco está pronto!" << endl;

    //Arquivo Preto e Branco bmp
    ofstream arquivopb;

    pretobranco.insert(posicao, "_pb");

    arquivopb.open(pretobranco, ios::binary | ios::out); // abre o arquivo para saída no modo binário
    if (!arquivopb) {
        cerr << "O arquivo de saída não pode ser gerado." << endl;
        return -1;
    }

    arquivopb.write((char *)&himg, sizeof(himg));
    arquivopb.write((char *)imagem, dados);

    arquivopb.close();


    //Arquivo.h preto e branco
    ofstream arquivopbh;
    arquivopbh.open("IMAGEM.h", ios::out); // abre o arquivo para saída
    if (!arquivopbh) {
        cerr << "O arquivo de saída não pode ser gerado." << endl;
        return -1;
    }


    ofstream recorteh;
    recorteh.open("RECORTE.h", ios::out); // abre o arquivo para saída
    if (!recorteh) {
        cerr << "O arquivo de saída não pode ser gerado." << endl;
        return -1;
    }


    // Cria a variavel matriz utilizando alocação dinâmica

    int (**matriz) = new int*[himg.largura];
        if (matriz==nullptr)
        {
            cerr << "Falha alocacao memoria\n";
            return -1;
        }
    for(int i=0;i<himg.largura;i++)
    {
        matriz[i]=new int[himg.altura];
        if (matriz[i]==nullptr)
        {
            cerr << "Falha alocacao memoria\n";
            return -1;
        }
    }


    //Preenche a matriz com toda a imagem
    int preenche = 0;
    for (int j = 0; j < himg.altura; j++)
    {
        for (int i = 0; i < himg.largura; i++)
        {
            if (imagem[preenche] == 255)
            {
                matriz[i][j] = 0;//branco no display
            }
            else
            {
                matriz[i][j] = 1;//preto no display
            }
            preenche+=3;
        }
    }


    //Inverte a matriz de cabeça para baixo, mudando a última linha e a primeira
    for (int j = 0; j < himg.altura/2; j++)
    {
        for (int i = 0; i < himg.largura; i++)
        {
            int aux = matriz[i][j];
            matriz[i][j] = matriz[i][himg.altura - 1 - j];
            matriz[i][himg.altura - 1 - j] = aux;
        }
    }
    //Colocando matriz no arquivo de saída
    for (int j = 0; j < himg.altura; j++)
    {
        for (int i = 0; i < himg.largura; i++)
        {
            arquivopbh << matriz[i][j];
        }
        arquivopbh << endl;
    }
    arquivopbh << endl << endl << endl;


    int altura = 0;
    int largura = 0;
    int alturamax = (himg.altura - 48);
    int larguramax = (larg_nova - 84);

    do{

        cout << "Digite as coordenadas (largura altura): ";
        cin >> largura >> altura;
    }while (altura > alturamax || altura < 0 || largura > larguramax || largura < 0);



    int matrizDisplay[84][48];

    //cria o arquivo.h
    ofstream arquivoh;
    //separa a string do nome do .bmp
    arquivinho= arquivinho.substr(0, posicao);
    //insere .h ao fim do nome
    arquivinho.insert(posicao, ".h");
    arquivoh.open(arquivinho,  ios::out);
    if (!arquivoh) {
        cerr << "O arquivo de saída não pode ser gerado." << endl;
        return -1;
    }

     arquivoh << "unsigned const char [504] = {" << endl;

    for (int l = altura; l < altura + 48; l++) {
        for (int m = largura; m < largura + 84; m++) {
            recorteh << matriz[m][l];
        }
        recorteh << endl;
    }
    //Passa para hexadecimal


    int r = 0;
    int soma;
    int conta=0;

for (int x = 0; x < 48; x++) {
    for (int y = 0; y < 84; y++) {



            int hexValue[8];

            hexValue [r]=matrizDisplay[x][y];
            r++;

            //Eleva 2 ao valor contida na variavel e adiciona a variavel soma

            if(hexValue[r]==1){
                soma+=2^r;
            }
            else{
                soma+0;
            };
            //A cada 8 bits processados em soma é adicionado o valor de  soma em hexadecimal ao arquivo
            if (r>7){
                conta++;

                if(conta<504){
                arquivoh << "0x" << hex << soma << ", ";
                r=0;
                soma=0;
                }
                 else{
                    arquivoh << "0x" << hex << soma << "}";
                 }



}
        }


        }


    cout << endl << "Todos os seus arquivos estão prontos!" << endl;



    delete[] imagem;
    arquivoh.close();

    arquivo.close();

    arquivopbh.close();

}
