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

/*
--help
parameters like -nodeFile
if no link file outputs one by calculating the minimum distance
missing destructors
OK - check noise floor with Rezende
*/

#include "linearprogram.hpp"

//try to remove the less weighted link (the last ones) and test
bool heuristics(float matchingWeight, std::vector < LinkIdentification > matchingLinkIdentifications)
{
   bool matchingOK = false;

   do
   {
      matchingWeight -= matchingLinkIdentifications.front().getLink()->getWeight();
      if (matchingWeight < 1) break;
      matchingLinkIdentifications.erase(matchingLinkIdentifications.begin());
      matchingOK = Network::scheduleOK(&matchingLinkIdentifications);
   }
   while (not matchingOK);

   return matchingOK;
}

int main(int nargs, char * args[])
{
	int result = 0;
	Network * network;
	WeightGraph * weightGraph;
	std::vector < LinkIdentification > * matchingLinkIdentifications;
	LinearProgram * linearProgram;
	std::vector < float > linkWeightsHistory, matchingWeightsHistory;
	bool matchingOK, ok, optimal;
	float matchingWeight;

	if (args[1][0] == 'N')
   {
	   //square side = 8,16,32,...,1024,2^20 // nodes = 4, 8, 16, 1024
	   //-NoiseFloor 0
	   Network::randomNetworks(atoi(args[2]), atoi(args[3]), atoi(args[4]), atoi(args[5]), args[6]);
	   return result;
   }

	loadParameters(nargs, args);
	printParameters();
	network = new Network(NodeFileName, LinkFileName);
	weightGraph = new WeightGraph(network);
	linearProgram = new LinearProgram(network->getLinks());

	//loop vava begins
	do
	{
		linearProgram->updateStep();
		//init MOK done in LinearProgram constructor
		linearProgram->clearLinkMatchingNotOK();
		ok = false;

		do
		{
		   optimal = true;
			linearProgram->solve(linkWeightsHistory, matchingWeightsHistory);
			matchingLinkIdentifications = new std::vector < LinkIdentification >;
			matchingWeight = weightGraph->MaxWeightedMatching(matchingLinkIdentifications);
			matchingOK = Network::scheduleOK(matchingLinkIdentifications);

			/*debug*/
			std::cout << std::endl << "MWM" << std::endl;
			for (unsigned int linkIndex = 0; linkIndex < matchingLinkIdentifications->size(); linkIndex++)
				std::cout << matchingLinkIdentifications->at(linkIndex).getIdentification() << " " << std::endl;
			std::cout << std::endl;
			/*debug*/

			if (matchingWeight <= 1) ok = true;
			else if (matchingWeight > linearProgram->getConstraintsMatchingNotOKupperBound() and not matchingOK)
				linearProgram->addLinkMatching(LINEAR_PROGRAM_MATCHING_NOK, matchingLinkIdentifications);
			else if (matchingWeight > 1 and matchingOK)
				linearProgram->addLinkMatching(LINEAR_PROGRAM_MATCHING_OK, matchingLinkIdentifications);
			else if (matchingWeight > 1 and matchingWeight <= linearProgram->getConstraintsMatchingNotOKupperBound() and not matchingOK)
			{
				//???optimization insert sort while insert in matchingLinkIdentifications
				//sort in a non-decreasing order of weights
				std::sort(matchingLinkIdentifications->rbegin(), matchingLinkIdentifications->rend());
            matchingOK = heuristics(matchingWeight, *matchingLinkIdentifications);

				if (not matchingOK)
            {
				   optimal = false;
				   ok = true;
            }
				else linearProgram->addLinkMatching(LINEAR_PROGRAM_MATCHING_OK, matchingLinkIdentifications);
			}
			else std::cerr << "Xi..." << std::endl;
		}
		while (not ok);
	}
	while (not linearProgram->emptyLinkMatchingNotOK());
	//loop vava ends

	std::cout << std::endl << "Optimal solutions (" << optimal << ")" << std::endl;
	std::cout << "z";
	for (unsigned int solutionIndex = 0; solutionIndex < linkWeightsHistory.size(); solutionIndex++)
	{
		std::cout << " l" << solutionIndex;
		if (solutionIndex % (network->getLinks()->size() +1) == 0) std::cout << std::endl;
	}
   for (unsigned int solutionIndex = 0; solutionIndex < linkWeightsHistory.size(); solutionIndex++)
   {
      std::cout << linkWeightsHistory.at(solutionIndex) ;
      if (solutionIndex % (network->getLinks()->size() +1) == 0) std::cout << std::endl;
   }

   ???ta errado porque os matching sao adicionados conforme o programa vai rodando
         melhor seria zerar os dois vetores e colocar um debug no loop e no final pegar a soluçao
   matchingWeight = 0;
   std::cout << "matchingWeight";
   for (unsigned int solutionIndex = 0; solutionIndex < matchingWeightsHistory.size(); solutionIndex++)
   {
      std::cout << " m" << solutionIndex;
      if (solutionIndex % (linearProgram->getLinkMatchingOK().size() +1) == 0) std::cout << std::endl;
   }

	return result;
}
