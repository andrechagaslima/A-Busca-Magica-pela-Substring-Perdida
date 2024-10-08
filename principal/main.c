#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <string.h>
#include <stdbool.h>
#include "leitura.h"
#include "shiftAnd.h"
#include "bmhs.h"

int main(int argc, char *argv[]) {
    
     char *algoritmo = NULL;
     char *inputFile = NULL;
     char *outputFile = "saida.txt";
     struct rusage start, end; //Usados para calcular o tempo (getrusage)
     struct timeval inicio, fim; //Usados para calcular o tempo (gettimeofday)
     char *texto = NULL;
     char *padrao = NULL;
     Query *queries = NULL;
     int num_queries;

     //Recebe o algoritmo e o nome do arquivo de entrada
     algoritmo = argv[1];
     inputFile = argv[2];

     verificacaoArquivoEntrada(argv, inputFile, algoritmo);

     // Abrir o arquivo de entrada
     FILE *arquivo = fopen(inputFile, "r");
     testaAberturaArquivo(arquivo, inputFile);

     leituraArquivo(arquivo, &texto, &padrao, &queries, &num_queries);

     bool *resultados = (bool *)malloc(num_queries * sizeof(bool));

     size_t tam_padrao = strlen(padrao) - 1;

     // Abrir o arquivo de saída
     FILE *arquivoSaida = fopen(outputFile, "w");
     testaAberturaArquivo(arquivoSaida, outputFile);

     getrusage(RUSAGE_SELF, &start);
     gettimeofday(&inicio, NULL);

     //Escolha do Algoritmo
     for(int i = 0; i < num_queries; i++){
          int tam_texto = queries[i].end - queries[i].start + 1;
          char *process_text = (char *)malloc((tam_texto + 1) * sizeof(char));
          processQuery(&queries[i], texto, process_text);
          if(algoritmo[0] == 'S'){
               resultados[i] = shiftAnd(process_text, tam_texto, padrao, tam_padrao);
          } else if(algoritmo[0] == 'B'){
               resultados[i] = BMHS(process_text, tam_texto, padrao, tam_padrao);
          }
          free(process_text);
     }

     gettimeofday(&fim, NULL);
     getrusage(RUSAGE_SELF, &end);

     //Cálculos de tempo
     double tempoUsuario = (fim.tv_sec - inicio.tv_sec) + (fim.tv_usec - inicio.tv_usec) / 1000000.0;
     double tempoNoSistema = (end.ru_stime.tv_sec - start.ru_stime.tv_sec) 
          + 1e-6 * (end.ru_stime.tv_usec - start.ru_stime.tv_usec);

     imprimirSaidas(arquivoSaida, resultados, num_queries ,tempoUsuario, tempoNoSistema);
     
     //Finalizações

     free(queries);
     free(padrao);
     free(texto);
     free(resultados);
     fclose(arquivo);
     fclose(arquivoSaida);

     return 0;

}