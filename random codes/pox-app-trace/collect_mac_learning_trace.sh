echo>/home/ubuntu/assignments/random\ codes/pox-app-trace/mac-learning-trace.txt 
echo>mnOutput.txt
echo>poxOutput.txt
# topo=("linear,2")
topo=("linear,2" "linear,3" "tree,2,2" "tree,2,3" "tree,3,2")
if [ -e "mnFifo" ]
then
    echo y|rm mnFifo
fi
for i in "${topo[@]}"
do
    echo "I am starting the topology $i"
    printf "$i\n" >> "/home/ubuntu/assignments/random codes/pox-app-trace/mac-learning-trace.txt"
    echo>"/home/ubuntu/assignments/random codes/pox-app-trace/mac-learning-flow/$i.txt"
    rm -f "/home/ubuntu/assignments/random codes/pox-app-trace/mac-learning-flow.txt"
    ln -s "/home/ubuntu/assignments/random codes/pox-app-trace/mac-learning-flow/$i.txt" "/home/ubuntu/assignments/random codes/pox-app-trace/mac-learning-flow.txt"
    mkfifo mnFifo
    sudo mn --custom ~/mininet/custom/topo-2sw-2host.py --topo $i --controller remote --mac --arp < mnFifo &>>mnOutput.txt &
    exec 100> mnFifo
    sleep 20s
    ~/pox/pox.py forwarding.l2_learning &>>poxOutput.txt &
    poxPid=$!
    sleep 5s
    echo pingall >&100
    sleep 3s
    echo exit >&100
    sleep 3s
    kill $poxPid
    rm mnFifo
    sudo mn --clean
    echo "topology $i is done."
done
