export port=${3}
export userName=${2}
export remoteMachine=${1}

if [ "${port}" == "" ]
then
   export port=22
fi
if [ "${userName}" == "" ]
then
   export userName=${USER}
fi

export userName=${userName}@

echo Gerando a chave local
ssh-keygen -q -N '' -f ~/.ssh/id_rsa
echo Copiando a chave para o remoto
scp -P${port} $HOME/.ssh/id_rsa.pub ${userName}${remoteMachine}:~/.tempoKey.out
echo Inserindo a chave local na lista remota
ssh -p${port} ${userName}${remoteMachine} "cat ~/.tempoKey.out >> ~/.ssh/authorized_keys"
echo Removendo a chave local do remoto
ssh -p${port} ${userName}${remoteMachine} "rm ~/.tempoKey.out"
echo Gerando a chave remota
ssh -p${port} ${userName}${remoteMachine} "ssh-keygen -q -N '' -f ~/.ssh/id_rsa"
echo Copiando a chave remota
scp -P${port} ${userName}${remoteMachine}:~/.ssh/id_rsa.pub ~/.tempoKey.out
echo Inserindo a chave remota na lista local
cat ~/.tempoKey.out >> ~/.ssh/authorized_keys
echo Removendo a chave remota
rm ~/.tempoKey.out

