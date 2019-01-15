echo>firewall-trace.txt
echo>mac-learning-with-fw-trace.txt 
echo>mnOutput.txt
echo>poxOutput.txt

#topo=("linear,2" "linear,3" "tree,2,2" "tree,2,3" "tree,3,2")
#maxHost=(2 3 4 9 8)
allTopo=("linear,2" "linear,3")
maxHost=(2 3)
if [ -e "mnFifo" ]
then
    echo y|rm mnFifo
fi
for ((idx=0; idx<${#allTopo[@]}; ++idx)); do
    range=${maxHost[idx]}
    topo=${allTopo[idx]}
    h1=$(($RANDOM%$range+1))
    h2=$(($RANDOM%$range+1))
    while (($h1 == $h2))
    do
        h1=$(($RANDOM%$range+1))
        h2=$(($RANDOM%$range+1))
    done
    echo id,mac_0,mac_1>firewall-policies.csv
    echo "1,00:00:00:00:00:$h1,00:00:00:00:00:$h2">>firewall-policies.csv
    echo "I am starting the topology $topo"
    printf "$topo\n" >> firewall-trace.txt
    echo>firewall-flow/$topo.txt
    rm -f firewall-flow.txt
    ln -s firewall-flow/$topo.txt firewall-flow.txt
    printf "$topo\n" >> mac-learning-with-fw-trace.txt
    echo>mac-learning-with-fw-flow/$topo.txt
    rm -f mac-learning-with-fw-flow.txt
    ln -s mac-learning-with-fw-flow/$topo.txt mac-learning-with-fw-flow.txt
    mkfifo mnFifo
    sudo mn --topo $topo --controller remote --mac --arp < mnFifo &>>mnOutput.txt &
    exec 100> mnFifo
    ~/pox/pox.py forwarding.l2_learning forwarding.l2_firewall &>>poxOutput.txt &
    poxPid=$!
    sleep 5s
    for ((i=1; i<=$range; ++i)); do
        for ((j=1; j<=$range; ++j)); do
            if (($i != $j))
            then
                printf "h$i ping -c 1 -W 1 h$j\n" >&100
                printf "\n" >&100
            fi
        done
    done
    sleep 5s
    echo exit >&100
    sleep 3s
    kill $poxPid
    rm mnFifo
    sudo mn --clean
    echo "topology $topo is done."
done
