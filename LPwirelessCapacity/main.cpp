/*
1- Carregar o multi-grafo Gm da rede em estruturas proprias guardando a direçao das arestas
2- Cria um grafo dirigido Gd a partir do multigrafo deixando no maximo uma aresta entre dois vertices em estruturas proprias
3- Inicializar o conjunto de restriçoes C baseado no multigrafo, que nesse momento contem apenas uma restiçao: uma soma das variaveis xzinho
   (uma variavel para cada aresta que participa do primeiro matching incluido no conjunto Mok)
   Mok nesse momento contem apenas esse matching, D nao esta vazio (contém um lixo inicial) e mi=1
4- Repita enquanto D nao for vazio
4.1- OK=falso e D=vazio
4.2- Repita enquanto nao OK
4.2.1- Rodar o prog linear com o C (o conjunto D esta vazio nesse momento) e quardar em E'
4.2.2- Utilizar a saida do prog linear para atribuir pesos as arestas do grafo Gd, pesos estes baseados no valores da váriaveis xzinhos obtidas na saida do prog linear.
       Onde havia duas arestas entre dois vértices, atribuir o maior valor dos respectivos xzinhos e direcinar a aresta de acordo.
4.2.3- Executar o algoritmo do Max Wei Matching em Gd (carregar Gd nas estruturas do algoritmo)
4.2.4- Somar os pesos do matching obitido pelo algoritmo
4.2.5- Calcular a razao sinal ruido dos links envolvidos no matching, lebrando da direçao das arestas.
4.2.6- SEs (w<=1, w>mi e m* nao ok, w>1 e m* ok) entao Incluir a restricao no devido conjunto (C ou D)
4.2.7- Se 1<w<mi e m* nao ok
       Executar uma heuristica que tente tornar m* ok mantendo w>1
          Se encontrar incluir a devida restricao em C
          Se nao encontrar entao OK=verdadeiro
4.2.8- Fim do repita em 4.2-
4.3- Se D nao for vazio incremente mi com um delta
4.4- Fim do repita em 4-
5- Imprimir o conjunto C e o conjunto E'
*/
/*
Dúvidas
a- Como inicializar C? (eu estou colocando um matching para cada aresta apenas e com o respectivo xzinho igual a 1)
b- O grafo G possui os mesmo vertices do multigrafo Gm, correto?
c- Qual dos matching incluido em C é a resposta que queremos?
d- a resposta nao seriam os pesos da saida o programa linear da ultima rodada?
*/

#include "graph.hpp"

int main(int nargs, char * args[])
{
	int result = 0;
	Network * network;
	WeightGraph * weightGraph;
	std::vector < LinkIdentification > * linkIdentifications = new std::vector < LinkIdentification >;
	std::vector < LinkIdentification > sortedLinkIdentifications, mOKlinkIdentifications;
	bool mOK;

	loadParameters(args);
	network = new Network(args[6], args[7]);
	weightGraph = new WeightGraph(network);

	//loop vava begins

	weightGraph->MaxWeightedMatching(*linkIdentifications);
	mOK = true;
	for (unsigned int linkIdentificationIndex = 0; linkIdentificationIndex < linkIdentifications->size() and mOK; linkIdentificationIndex++)
		mOK = (Network::snr(linkIdentifications->at(linkIdentificationIndex), linkIdentifications) >= SNRthreshold);

	//declare D and C sets
	//solve the linear program and get the weights

	if (not mOK)
	{
		//???optimization insert sort
		sortedLinkIdentifications = (*linkIdentifications);
		std::sort(sortedLinkIdentifications.begin(), sortedLinkIdentifications.end());

		//try to remove the first ones and test
		//???optimization make LinkIdentification a pointer class
		mOKlinkIdentifications = sortedLinkIdentifications;
		while (not mOK)
		{
			//???optimization std::vector to std::list (must see if specialized std::sort works with lists)
			mOKlinkIdentifications.erase(mOKlinkIdentifications.begin());
		}

		//try to remove the last ones and test
		mOKlinkIdentifications = sortedLinkIdentifications;
		while (not mOK)
		{
			mOKlinkIdentifications.pop_back();
		}
	}

	//loop vava ends

	return result;
}
