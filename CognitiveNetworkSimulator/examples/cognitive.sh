      #writeOn << std::setw(INTERVAL_SIZE) << std::setfill(BINARY_FILLER) << std::right << intToByte(cognitiveRequestQueue.request.at(cognitiveRequestIndex).interval);
      #writeOn << AgentTypeChar[cognitiveRequestQueue.request.at(cognitiveRequestIndex).transmitter->type];
      #writeOn << std::setw(AGENT_ID_SIZE) << std::setfill(CHAR_FILLER) << std::right << cognitiveRequestQueue.request.at(cognitiveRequestIndex).transmitter->identification;

      #writeOn << AgentTypeChar[cognitiveRequestQueue.request.at(cognitiveRequestIndex).receiver->type];
      #writeOn << std::setw(AGENT_ID_SIZE) << std::setfill(CHAR_FILLER) << std::right << cognitiveRequestQueue.request.at(cognitiveRequestIndex).receiver->identification;
      #writeOn << std::setw(DELTA_INTERVAL_SIZE) << std::setfill(BINARY_FILLER) << std::right << intToByte(cognitiveRequestQueue.request.at(cognitiveRequestIndex).deltaInterval);
      #writeOn << std::setw(THROUGHPUT_SIZE) << std::setfill(BINARY_FILLER) << std::right << intToByte(cognitiveRequestQueue.request.at(cognitiveRequestIndex).minimumThroughput);
   
for i in `seq 0 86400`
do
   j=$(($i%3600))
   j=$((i-j))
   j=$((j/3600))
   j=$((j%2))
   #if [ $j == 0 ]
   #then
      echo $i" CA 000 CC 000 3600 1"
      echo $i" CA 000 CC 001 3600 1"
   #fi
done

#0000000 CA000 CC000 3600 1
