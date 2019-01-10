if [ -e "mnFifo" ]
then
    echo y|rm mnFifo
fi
for ((i=1; i<=$1; ++i))
do
    echo "I am starting the loop $i"
    mkfifo mnFifo
    ~/pox/pox.py forwarding.l2_multi openflow.discovery openflow.spanning_tree &>>poxOutput.txt &
    poxPid=$!
    sudo mn --custom ~/mininet/custom/topo-2sw-2host.py --topo randomtopo,6,10,3 --controller remote --mac < mnFifo &>>mnOutput.txt &
    exec 100> mnFifo
    sleep 20s
    echo pingall >&100
    sleep 3s
    echo exit >&100
    sleep 3s
    kill $poxPid
    sleep 3s
    rm mnFifo
    sudo mn --clean
    echo "loop $i is done."
done
